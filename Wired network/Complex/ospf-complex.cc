#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/position-allocator.h"
#include "ns3/mobility-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/constant-position-mobility-model.h"

//Headers for Flow Monitor
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("OSPFRouting");

AnimationInterface * anim= 0;

//Function for breaking the link between interfaces
void setLinkDown(Ptr<Ipv4> interface, uint32_t start, uint32_t end, uint32_t index){
	interface->SetDown(index);
	anim->UpdateLinkDescription (start, end, "Link Down");
}

//Function for set up the link between interfaces
void setLinkUp(Ptr<Ipv4> interface, uint32_t start, uint32_t end, uint32_t index, std::string cost){
	interface->SetUp(index);
	anim->UpdateLinkDescription (start, end, cost);
}

//Function to monitor packets at the receiving end
uint32_t receivedPckts= 0;
void PcktReceived(std::string context, Ptr<const Packet> pkt){
	NS_LOG_INFO(ns3::Simulator::Now().GetSeconds());
	receivedPckts++;
}

int main (int argc, char **argv){
	bool verbose = true;
	bool printRoutingTables = true;
	bool showPings = true;
	bool enableFlowMonitor = true;

	Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));

	CommandLine cmd (__FILE__);
	cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
	cmd.AddValue ("printRoutingTables", "Print routing tables at 30, 60 and 90 seconds", printRoutingTables);
	cmd.AddValue ("verbose", "turn on log components", verbose);
	cmd.AddValue ("showPings", "Show Ping6 reception", showPings);
	cmd.Parse (argc, argv);

	if (verbose){
		LogComponentEnable ("OSPFRouting", LOG_LEVEL_INFO);
		LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
		LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
	}

	//Creating nodes
	NS_LOG_INFO ("Creating nodes.");
	Ptr<Node> src = CreateObject<Node> ();
	Names::Add ("SrcNode", src);
	Ptr<Node> dst = CreateObject<Node> ();
	Names::Add ("DstNode", dst);
	Ptr<Node> a = CreateObject<Node> ();
	Names::Add ("RouterA", a);
	Ptr<Node> b = CreateObject<Node> ();
	Names::Add ("RouterB", b);
	Ptr<Node> c = CreateObject<Node> ();
	Names::Add ("RouterC", c);
	Ptr<Node> d = CreateObject<Node> ();
	Names::Add ("RouterD", d);
	Ptr<Node> e = CreateObject<Node> ();
	Names::Add ("RouterE", e);
	Ptr<Node> f = CreateObject<Node> ();
	Names::Add ("RouterF", f);

	//Containers
	NodeContainer cont1 (src, a);
	NodeContainer cont2 (a, b);
	NodeContainer cont3 (a, c);
	NodeContainer cont4 (b, e);
	NodeContainer cont5 (c, d);
	NodeContainer cont6 (c, e);
	NodeContainer cont7 (d, dst);
	NodeContainer cont8 (e, d);
	NodeContainer cont9 (e, f);
	NodeContainer cont10 (f, d);  
	NodeContainer routers (a, b, c, d);
	NodeContainer routers2 (e, f);
	NodeContainer edges (src, dst);

	//Allocating positions for nodes in the topology
	Ptr<ListPositionAllocator>alloc= CreateObject<ListPositionAllocator>();
	alloc->Add(Vector(30.0, 40.0, 1));
	alloc->Add(Vector(60.0, 50, 1));
	alloc->Add(Vector(60.0, 30, 1));
	alloc->Add(Vector(90.0, 60, 1));
	alloc->Add(Vector(120.0, 50, 1));
	alloc->Add(Vector(150.0, 40, 1));
	alloc->Add(Vector(90.0, 20, 1));
	alloc->Add(Vector(120.0, 30, 1));

	//Installing positions in the respective nodes
	MobilityHelper mobility;
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.SetPositionAllocator(alloc);
	mobility.Install(src);
	mobility.Install(a);
	mobility.Install(b);
	mobility.Install(c);
	mobility.Install(d);
	mobility.Install(dst);
	mobility.Install(e);
	mobility.Install(f);

	//Installing the internet stack in all the nodes
	InternetStackHelper internetNodes;
	internetNodes.Install (edges);
	internetNodes.Install (routers);
	internetNodes.Install (routers2);

	//Creating properties of links
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute  ("DataRate", StringValue ("5Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

	//Creating net devices(aka NIC cards) in nodes through node containers
	NS_LOG_INFO("Creating Net Devices");
	NetDeviceContainer netDev1 = p2p.Install (cont1);//a(1)
	NetDeviceContainer netDev2 = p2p.Install (cont2);//a(2)b(1)
	NetDeviceContainer netDev3 = p2p.Install (cont3);//a(3)c(1)
	NetDeviceContainer netDev4 = p2p.Install (cont4);//b(2)e(1)
	NetDeviceContainer netDev5 = p2p.Install (cont5);//c(2)d(1)
	NetDeviceContainer netDev6 = p2p.Install (cont6);//c(3)e(2)
	NetDeviceContainer netDev7 = p2p.Install (cont7);//d(2)dst(1)
	NetDeviceContainer netDev8 = p2p.Install (cont8);//e(3)d(3)
	NetDeviceContainer netDev9 = p2p.Install (cont9);
	NetDeviceContainer netDev10 = p2p.Install (cont10);

	//Assinging IPv4 addresses to all the interfaces
	NS_LOG_INFO ("Assign IPv4 Addresses.");
	Ipv4AddressHelper ipv4;

	ipv4.SetBase ("10.0.0.0", "255.255.255.0");
	Ipv4InterfaceContainer iic1 = ipv4.Assign (netDev1);
	iic1.SetMetric(0, 5);
	iic1.SetMetric(1, 5);

	ipv4.SetBase ("10.0.1.0", "255.255.255.0");
	Ipv4InterfaceContainer iic2 = ipv4.Assign (netDev2);
	iic2.SetMetric(0, 4);
	iic2.SetMetric(1, 4);

	ipv4.SetBase ("10.0.2.0", "255.255.255.0");
	Ipv4InterfaceContainer iic3 = ipv4.Assign (netDev3);
	iic3.SetMetric(0, 6);
	iic3.SetMetric(1, 6);

	ipv4.SetBase ("10.0.3.0", "255.255.255.0");
	Ipv4InterfaceContainer iic4 = ipv4.Assign (netDev4);
	iic4.SetMetric(0, 20);
	iic4.SetMetric(1, 20);

	ipv4.SetBase ("10.0.4.0", "255.255.255.0");
	Ipv4InterfaceContainer iic5 = ipv4.Assign (netDev5);
	iic5.SetMetric(0, 50);
	iic5.SetMetric(1, 50);

	ipv4.SetBase ("10.0.5.0", "255.255.255.0");
	Ipv4InterfaceContainer iic6 = ipv4.Assign (netDev6);
	iic6.SetMetric(0, 9);
	iic6.SetMetric(1, 9);


	ipv4.SetBase ("10.0.6.0", "255.255.255.0");
	Ipv4InterfaceContainer iic7 = ipv4.Assign (netDev7);
	iic7.SetMetric(0, 7);
	iic7.SetMetric(1, 7);

	ipv4.SetBase ("10.0.7.0", "255.255.255.0");
	Ipv4InterfaceContainer iic8 = ipv4.Assign (netDev8);
	iic8.SetMetric(0, 18);
	iic8.SetMetric(1, 18);

	ipv4.SetBase ("10.0.8.0", "255.255.255.0");
	Ipv4InterfaceContainer iic9 = ipv4.Assign (netDev9);
	iic9.SetMetric(0, 8);
	iic9.SetMetric(1, 8);

	ipv4.SetBase ("10.0.9.0", "255.255.255.0");
	Ipv4InterfaceContainer iic10 = ipv4.Assign (netDev10);
	iic10.SetMetric(0, 6);
	iic10.SetMetric(1, 6);

	//Assinging OSPF as the routing protocol
	NS_LOG_INFO ("Creating and Assigning IPv4 addresses and routing");
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	Ipv4GlobalRoutingHelper routingHelper;

	//Printing routing tables to a file
	Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("ospf-routing-table.txt", std::ios::out);

	routingHelper.PrintRoutingTableAt (Seconds (7.0), src, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (7.0), dst, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (7.0), a, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (7.0), b, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (7.0), c, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (7.0), d, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (7.0), e, routingStream);

	routingHelper.PrintRoutingTableAt (Seconds (90.0), src, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (90.0), dst, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (90.0), a, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (90.0), b, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (90.0), c, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (90.0), d, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (90.0), e, routingStream);

	routingHelper.PrintRoutingTableAt (Seconds (110.0), src, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (110.0), dst, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (110.0), a, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (110.0), b, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (110.0), c, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (110.0), d, routingStream);
	routingHelper.PrintRoutingTableAt (Seconds (110.0), e, routingStream);

	//Creating traffic
	uint32_t totalPackets= 40;
	uint32_t packetSize= 1024;

	UdpEchoServerHelper echoServer (9);

	//Creating server apps
	ApplicationContainer serverApps = echoServer.Install (dst);
	serverApps.Start (Seconds (1.0));
	serverApps.Stop (Seconds (150.0));

	UdpEchoClientHelper echoClient (iic7.GetAddress (1), 9);
	echoClient.SetAttribute ("MaxPackets", UintegerValue (totalPackets));
	echoClient.SetAttribute ("Interval", TimeValue (Seconds (4)));
	echoClient.SetAttribute ("PacketSize", UintegerValue (packetSize));

	//Creating client apps
	ApplicationContainer clientApps = echoClient.Install (src);
	clientApps.Start (Seconds (2.0));
	clientApps.Stop (Seconds (150.0));

	//Calling the monitoring function for received packets
	Ptr<PacketSink> sink= StaticCast<PacketSink>(serverApps.Get(0));
	sink->TraceConnect("Rx", "Packet Received", MakeCallback(&PcktReceived));
	
	AsciiTraceHelper ascii;
	p2p.EnableAsciiAll (ascii.CreateFileStream ("ospf-trace-topo2.tr"));
	// p2p.EnablePcapAll ("ospf", true);
	
	//Pointer objects to interfaces
	Ptr<Ipv4> ipv4a = a->GetObject<Ipv4> ();
	Ptr<Ipv4> ipv4c = c->GetObject<Ipv4> ();
	Ptr<Ipv4> ipv4e = e->GetObject<Ipv4> ();
	Ptr<Ipv4> ipv4f = f->GetObject<Ipv4> ();

	//Setting up and down the links
	Simulator::Schedule (Seconds (8), &setLinkDown,ipv4c, 2, 4, 1);
	Simulator::Schedule (Seconds (8), &setLinkDown,ipv4a, 2, 4, 3);

	Simulator::Schedule (Seconds (40), &setLinkDown,ipv4e, 6, 7, 4);
	Simulator::Schedule (Seconds (40), &setLinkDown,ipv4f, 6, 7, 1);

	Simulator::Schedule (Seconds (100), &setLinkDown,ipv4a, 2, 3, 2);
	Simulator::Schedule (Seconds (105), &setLinkUp,ipv4a, 2, 3, 2, "4");

	//Running the simulation
	NS_LOG_INFO ("Run Simulation.");
	Simulator::Stop (Seconds (150.0));

	anim= new AnimationInterface("ospf-anim2.xml");

	anim->UpdateNodeDescription (0, "Source");
	anim->UpdateNodeDescription (1, "Destination");
	anim->UpdateNodeDescription (2, "Node:A");
	anim->UpdateNodeDescription (3, "Node:B");
	anim->UpdateNodeDescription (4, "Node:C");
	anim->UpdateNodeDescription (5, "Node:D");
	anim->UpdateNodeDescription (6, "Node:E");
	anim->UpdateNodeDescription (7, "Node:F");

	anim->UpdateLinkDescription (0, 2, "5");
	anim->UpdateLinkDescription (2, 3, "4");
	anim->UpdateLinkDescription (2, 4, "6");
	anim->UpdateLinkDescription (3, 6, "20");
	anim->UpdateLinkDescription (4, 5, "50");
	anim->UpdateLinkDescription (4, 6, "9");
	anim->UpdateLinkDescription (1, 5, "7");
	anim->UpdateLinkDescription (5, 6, "18");
	anim->UpdateLinkDescription (6, 7, "8");
	anim->UpdateLinkDescription (5, 7, "6");

	//Flow Monitor
	Ptr<FlowMonitor> flowMonitor;
	FlowMonitorHelper flowHelper;
	flowMonitor= flowHelper.InstallAll();

	Simulator::Run ();
	Simulator::Destroy ();

	flowMonitor->SerializeToXmlFile("ospf-flow2.xml", true, true);

	//Packet loss ratio
	NS_LOG_INFO ("\n***************************\n");
	uint32_t lostPckts= totalPackets - receivedPckts;
	double  lostPcktRatio= ((double)lostPckts / totalPackets) * 100;

	NS_LOG_INFO("Total number of packets lost");
	NS_LOG_INFO(lostPckts);

	NS_LOG_INFO("Packet loss ratio");
	NS_LOG_INFO(lostPcktRatio);

	NS_LOG_INFO ("\n***************************\n");
	
	//Packet delivery ratio
	NS_LOG_INFO ("\n***************************\n");
	double deliveryPcktRatio= ((double)receivedPckts / totalPackets) * 100;

	NS_LOG_INFO("Total number of packets received");
	NS_LOG_INFO(receivedPckts);

	NS_LOG_INFO("Packet delivery ratio");
	NS_LOG_INFO(deliveryPcktRatio);

	NS_LOG_INFO ("\n***************************\n");

	// Throughtput calculation
	uint32_t receivedBytes= 1024 * receivedPckts;
	uint32_t time= 150;
	NS_LOG_INFO ("Throughtput in Kbps :");
	double throughput= (((double)receivedBytes * 8)/ (time * 1000));
	NS_LOG_INFO (throughput);
	NS_LOG_INFO ("\n***************************");

	NS_LOG_INFO ("Done.");
}

// 1.96608
// 10
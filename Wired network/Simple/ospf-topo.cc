#include <fstream>
#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/animation-interface.h"
#include "ns3/netanim-module.h"
#include "ns3/position-allocator.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/point-to-point-module.h"

//Headers for Flow Monitor
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("OSPFRouting");

AnimationInterface * anim= 0;

uint32_t receivedPckts= 0;

void PcktReceived(std::string context, Ptr<const Packet> pkt){
	receivedPckts++;
}

int main (int argc, char **argv){
	bool verbose = true;
	bool showPings = true;
	std::string SplitHorizon ("PoisonReverse");

	CommandLine cmd (__FILE__);
	cmd.AddValue ("verbose", "turn on log components", verbose);
	cmd.AddValue ("showPings", "Show Ping6 reception", showPings);
	cmd.AddValue ("splitHorizonStrategy", "Split Horizon strategy to use (NoSplitHorizon, SplitHorizon, PoisonReverse)", SplitHorizon);
	cmd.Parse (argc, argv);


	if (verbose){
	  LogComponentEnable ("OSPFRouting", LOG_LEVEL_INFO);
	  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
	  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
	}

	if (SplitHorizon == "NoSplitHorizon")
	  Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::NO_SPLIT_HORIZON));

	else if (SplitHorizon == "SplitHorizon")
	  Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::SPLIT_HORIZON));

	else
	  Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::POISON_REVERSE));

	//Creating individual nodes
	NS_LOG_INFO ("Creating nodes.");
	Ptr<Node> src = CreateObject<Node> ();
	Names::Add ("SrcNode", src);
	Ptr<Node> dst = CreateObject<Node> ();
	Names::Add ("DstNode", dst);
	Ptr<Node> mid = CreateObject<Node> ();
	Names::Add ("RouterMid", mid);

	//Containers for nodes
	NodeContainer cont1 (src, mid);
	NodeContainer cont2 (mid, dst);
	NodeContainer cont3 (src, dst);
	NodeContainer routers (src, dst, mid);

	//Allocating positions for nodes in the topology
	Ptr<ListPositionAllocator>alloc= CreateObject<ListPositionAllocator>();
	alloc->Add(Vector(0.0, 30.0, 1));
	alloc->Add(Vector(30.0, 0, 1));
	alloc->Add(Vector(60.0, 30, 1));

	//Installing positions in the respective nodes
	MobilityHelper mobility;
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.SetPositionAllocator(alloc);
	mobility.Install(src);
	mobility.Install(mid);
	mobility.Install(dst);

	InternetStackHelper internetNodes;
	internetNodes.Install (routers);

	NS_LOG_INFO ("Creating csma channels(buses which are connecting nodes)");

	//Point to Point helper for creating links between nodes
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute  ("DataRate", StringValue ("1Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

	//Creating net devices(aka NIC cards) in nodes through node containers
	NS_LOG_INFO("Creating Net Devices");
	NetDeviceContainer netDev1 = p2p.Install (cont1);
	NetDeviceContainer netDev2 = p2p.Install (cont2);

	p2p.SetDeviceAttribute  ("DataRate", StringValue ("5Kbps"));
	NetDeviceContainer netDev3 = p2p.Install (cont3);

	//Assigning IPv4 Addresses to interfaces - a single node can have many interfaces
	NS_LOG_INFO ("Assign IPv4 Addresses.");
	Ipv4AddressHelper ipv4;

	ipv4.SetBase ("10.0.0.0", "255.255.255.0");
	Ipv4InterfaceContainer iic1 = ipv4.Assign (netDev1);
	iic1.SetMetric(0, 3);
	iic1.SetMetric(1, 3);

	ipv4.SetBase ("10.0.1.0", "255.255.255.0");
	Ipv4InterfaceContainer iic2 = ipv4.Assign (netDev2);
	iic2.SetMetric(0, 5);
	iic2.SetMetric(1, 5);

	ipv4.SetBase ("10.0.2.0", "255.255.255.0");
	Ipv4InterfaceContainer iic3 = ipv4.Assign (netDev3);
	iic3.SetMetric(0, 20);
	iic3.SetMetric(1, 20);

	//Installing the protocols in the routers
	NS_LOG_INFO ("Creating and Assigning IPv4 addresses and routing");
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	//Creating traffic in nodes
	uint32_t totalPackets= 20;
	uint32_t packetSize= 1024;

	UdpEchoServerHelper echoServer (9);

	//Installing the server
	ApplicationContainer serverApps = echoServer.Install (dst);
	serverApps.Start (Seconds (1.0));
	serverApps.Stop (Seconds (50.0));

	UdpEchoClientHelper echoClient (iic2.GetAddress (1), 9);
	echoClient.SetAttribute ("MaxPackets", UintegerValue (totalPackets));
	echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
	echoClient.SetAttribute ("PacketSize", UintegerValue (packetSize));

	//Installing the client
	ApplicationContainer clientApps = echoClient.Install (src);
	clientApps.Start (Seconds (2.0));
	clientApps.Stop (Seconds (50.0));

	//Recording received packets for throughput calculation
	Ptr<PacketSink> sink= StaticCast<PacketSink>(serverApps.Get(0));
	sink->TraceConnect("Rx", "Packet Received", MakeCallback(&PcktReceived));

	//Enabling tracing functions
	AsciiTraceHelper ascii;
	p2p.EnableAsciiAll (ascii.CreateFileStream ("ospf-trace-topo1.tr"));
	p2p.EnablePcapAll ("ospf-pcap-topo1", true);

	//Simulation Details
	NS_LOG_INFO ("Run Simulation.");
	Simulator::Stop (Seconds (50.0));

	anim= new AnimationInterface("ospf-anim1.xml");

	anim->UpdateNodeDescription (0, "Node:A");
	anim->UpdateNodeDescription (1, "Node:C");
	anim->UpdateNodeDescription (2, "Node:B");

	anim->UpdateLinkDescription (0, 2, "3");
	anim->UpdateLinkDescription (1, 2, "5");
	anim->UpdateLinkDescription (0, 1, "20");

	//Installing Flow Monitor to monitor all nodes, its behaviour in a seperate file
	Ptr<FlowMonitor> flowMonitor;
	FlowMonitorHelper flowHelper;
	flowMonitor= flowHelper.InstallAll();

	Simulator::Run ();
	Simulator::Destroy ();

	flowMonitor->SerializeToXmlFile("flow-ospf-topo1.xml", true, true);  

	//Calculating Packet loss ratio
	NS_LOG_INFO ("\n***************************\n");
	uint32_t lostPckts= totalPackets - receivedPckts;
	double  lostPcktRatio= ((double)lostPckts / totalPackets) * 100;

	NS_LOG_INFO("Total number of packets lost");
	NS_LOG_INFO(lostPckts);

	NS_LOG_INFO("Packet loss ratio");
	NS_LOG_INFO(lostPcktRatio);

	NS_LOG_INFO ("\n***************************\n");

	//Calculating Packet delivery ratio
	NS_LOG_INFO ("\n***************************\n");
	double deliveryPcktRatio= ((double)receivedPckts / totalPackets) * 100;

	NS_LOG_INFO("Total number of packets received");
	NS_LOG_INFO(receivedPckts);

	NS_LOG_INFO("Packet delivery ratio");
	NS_LOG_INFO(deliveryPcktRatio);

	NS_LOG_INFO ("\n***************************\n");

	//Throughtput calculation
	uint32_t receivedBytes= 1024 * receivedPckts;
	uint32_t time= 50;
	NS_LOG_INFO ("Throughtput in Kbps :");
	double throughput= (((double)receivedBytes * 8)/ (time * 1000));
	NS_LOG_INFO (throughput);
	NS_LOG_INFO ("\n***************************\n");

	NS_LOG_INFO ("Done.");
}

// 3.2768
// 0
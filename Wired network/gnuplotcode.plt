set terminal pdf
set output "Throughput Graph.pdf"

set title "Throughputs of RIP vs OSPF at Each Node"
set xlabel "RIP and OSPF"
set ylabel "Throughput"

set boxwidth 0.2
set style fill solid

plot "Seperate Throughput RIP.dat" using 1:3:xtic(2) with lines title "RIP - Throughput", "Seperate Throughput OSPF.dat" using 1:3:xtic(2) with lines title "OSPF - Throughput"

set title "Goodputs of RIP vs OSPF at Each Node"
set ylabel "Goodput"

plot "Seperate Goodput RIP.dat" using 1:3:xtic(2) with lines title "RIP - Goodput", "Seperate Goodput OSPF.dat" using 1:3:xtic(2) with lines title "OSPF - Goodput"

set title "Average Throughputs RIP vs OSPF in Simple and Complex Topologies"
set ylabel "Average Throughput"

plot "Simple_Throughputs.dat" using 1:3:xtic(2) with boxes title "Throughput of Simple Topology" lt rgb "#0000FF" , "Complex_Throughputs.dat" using 1:3:xtic(2) with boxes title "Throughput of Complex Topology" lt rgb "#FF0000"

set title "Packet Loss Ratio of RIP vs OSPF in Simple and Complex Topologies"
set ylabel "Packet Loss Ratio"

plot "Simple_PacketLossRatio.dat" using 1:3:xtic(2) with boxes title "Packet Loss Ratio of Simple Topology" lt rgb "#0000FF" , "Complex_PacketLossRatio.dat" using 1:3:xtic(2) with boxes title "Packet Loss Ratio of Complex Topology" lt rgb "#FF0000"
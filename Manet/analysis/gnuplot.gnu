set terminal pdf
set output "Graph Analysis.pdf"
set key outside

set boxwidth 0.2
set style fill solid

#=============================================
#Packet Delivery Ratio of the 3 Protocols
#=============================================

set xlabel "Protocols"
set ylabel "Packet Delivery ratio"
set title "Packet Delivery Ratio"

plot "pdr.dat" using 1:3:xtic(2) with boxes title "Protocols" lt rgb "#0000FF"


#=============================================
#Average Throughput of the 3 Protocols
#=============================================

set xlabel "Protocols"
set ylabel "Average Throughput(kbps)"
set title "Average Throughputs"

plot "avg-thr.dat" using 1:3:xtic(2) with boxes title "Protocols" lt rgb "#FF0000"


#=============================================
#Throughput of the 3 Protocols under different mobility
#=============================================

set title "Throughputs across various mobility"
set xlabel "Speed of the nodes(m/s)"
set ylabel "Throughputs(kbps)"

plot "dsr_thr.dat" using 1:2 with lp title "DSR" , "aodv_thr.dat" using 1:2 with lp title "AODV", "dsdv_thr.dat" using 1:2 with lp title "DSDV"


#=============================================
#Routing load with increasing no of sources
#=============================================

set title "No of Sources v/s Routing Load"
set xlabel "No of Sources"
set ylabel "Routing Load"

plot "../load/dsdv_routing_load.dat" using 1:2 with lp title "DSDV" lt rgb "#0000FF", "../load/aodv_routing_load.dat" using 1:2 with lp title "AODV" lt rgb "#00FF00", "../load/dsr_routing_load.dat" using 1:2 with lp title "DSR" lt rgb "#FF0000"




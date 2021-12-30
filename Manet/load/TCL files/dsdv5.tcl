#===================================
#     Simulation parameters setup
#===================================
set val(chan)   Channel/WirelessChannel    ;# channel type
set val(prop)   Propagation/TwoRayGround   ;# radio-propagation model
set val(netif)  Phy/WirelessPhy            ;# network interface type
set val(mac)    Mac/802_11                 ;# MAC type
set val(ifq)    Queue/DropTail/PriQueue    ;# interface queue type
set val(ll)     LL                         ;# link layer type
set val(ant)    Antenna/OmniAntenna        ;# antenna model
set val(ifqlen) 50                         ;# max packet in ifq
set val(nn)     20                         ;# number of mobilenodes
set val(rp)     DSDV                       ;# routing protocol
set val(x)      1143                      ;# X dimension of topography
set val(y)      1002                      ;# Y dimension of topography
set val(stop)   50.0                         ;# time of simulation end

#===================================
#        Initialization        
#===================================
#Create a ns simulator
set ns [new Simulator]

#Setup topography object
set topo [new Topography]
$topo load_flatgrid $val(x) $val(y)
create-god $val(nn)

#Open the NS trace file
set tracefile [open dsdv5.tr w]
$ns trace-all $tracefile

#Open the NAM trace file
set namfile [open dsdv5.nam w]
$ns namtrace-all $namfile
$ns namtrace-all-wireless $namfile $val(x) $val(y)

set chan [new $val(chan)]

#===================================
#     Mobile node parameter setup
#===================================
$ns node-config -adhocRouting  $val(rp) \
                -llType        $val(ll) \
                -macType       $val(mac) \
                -ifqType       $val(ifq) \
                -ifqLen        $val(ifqlen) \
                -antType       $val(ant) \
                -propType      $val(prop) \
                -phyType       $val(netif) \
                -channel       $chan \
                -topoInstance  $topo \
                -agentTrace    ON \
                -routerTrace   ON \
                -macTrace      ON \
                -movementTrace ON

#===================================
#        Nodes Definition        
#===================================
#Create 20 nodes
set n0 [$ns node]
$n0 set X_ 169
$n0 set Y_ 216
$n0 set Z_ 0.0
$ns initial_node_pos $n0 50
set n1 [$ns node]
$n1 set X_ 429
$n1 set Y_ 411
$n1 set Z_ 0.0
$ns initial_node_pos $n1 50
set n2 [$ns node]
$n2 set X_ 193
$n2 set Y_ 450
$n2 set Z_ 0.0
$ns initial_node_pos $n2 50
set n3 [$ns node]
$n3 set X_ 430
$n3 set Y_ 116
$n3 set Z_ 0.0
$ns initial_node_pos $n3 50
set n4 [$ns node]
$n4 set X_ 401
$n4 set Y_ 207
$n4 set Z_ 0.0
$ns initial_node_pos $n4 50
set n5 [$ns node]
$n5 set X_ 227
$n5 set Y_ 11
$n5 set Z_ 0.0
$ns initial_node_pos $n5 50
set n6 [$ns node]
$n6 set X_ 417
$n6 set Y_ 618
$n6 set Z_ 0.0
$ns initial_node_pos $n6 50
set n7 [$ns node]
$n7 set X_ 40
$n7 set Y_ 551
$n7 set Z_ 0.0
$ns initial_node_pos $n7 50
set n8 [$ns node]
$n8 set X_ 631
$n8 set Y_ 133
$n8 set Z_ 0.0
$ns initial_node_pos $n8 50
set n9 [$ns node]
$n9 set X_ 709
$n9 set Y_ 390
$n9 set Z_ 0.0
$ns initial_node_pos $n9 50
set n10 [$ns node]
$n10 set X_ 9
$n10 set Y_ 137
$n10 set Z_ 0.0
$ns initial_node_pos $n10 50
set n11 [$ns node]
$n11 set X_ 612
$n11 set Y_ 619
$n11 set Z_ 0.0
$ns initial_node_pos $n11 50
set n12 [$ns node]
$n12 set X_ 536
$n12 set Y_ 277
$n12 set Z_ 0.0
$ns initial_node_pos $n12 50
set n13 [$ns node]
$n13 set X_ 186
$n13 set Y_ 747
$n13 set Z_ 0.0
$ns initial_node_pos $n13 50
set n14 [$ns node]
$n14 set X_ 14
$n14 set Y_ 369
$n14 set Z_ 0.0
$ns initial_node_pos $n14 50
set n15 [$ns node]
$n15 set X_ 528
$n15 set Y_ 783
$n15 set Z_ 0.0
$ns initial_node_pos $n15 50
set n16 [$ns node]
$n16 set X_ 814
$n16 set Y_ 205
$n16 set Z_ 0.0
$ns initial_node_pos $n16 50
set n17 [$ns node]
$n17 set X_ 40
$n17 set Y_ 848
$n17 set Z_ 0.0
$ns initial_node_pos $n17 50
set n18 [$ns node]
$n18 set X_ 313
$n18 set Y_ 1009
$n18 set Z_ 0.0
$ns initial_node_pos $n18 50
set n19 [$ns node]
$n19 set X_ 305
$n19 set Y_ 863
$n19 set Z_ 0.0
$ns initial_node_pos $n19 50

#===================================
#        Nodes Movement        
#===================================
set speed 5
$ns at 0.0 " $n18 setdest 418 801 $speed " 
$ns at 0.0 " $n6 setdest 288 430 $speed " 

$ns at 1.0 " $n13 setdest 272 602 $speed " 
$ns at 1.0 " $n15 setdest 691 930 $speed " 

$ns at 0.0 " $n1 setdest 345 507 $speed " 
$ns at 2.0 " $n3 setdest 507 299 $speed " 

$ns at 14.0 " $n8 setdest 524 38 $speed " 
$ns at 10.0 " $n0 setdest 210 220 $speed " 

$ns at 30.0 " $n11 setdest 690 518 $speed " 
$ns at 5.0 " $n9 setdest 899 210 $speed " 

$ns at 25.0 " $n4 setdest 489 319 $speed " 
$ns at 11.0 " $n2 setdest 97 613 $speed " 

$ns at 7.0 " $n12 setdest 716 367 $speed " 
$ns at 18.0 " $n7 setdest 138 503 $speed " 

$ns at 20.0 " $n10 setdest 104 3 $speed " 

#===================================
#        Agents Definition        
#===================================
#Setup a UDP connection
set udp0 [new Agent/UDP]
$ns attach-agent $n18 $udp0
set null16 [new Agent/Null]
$ns attach-agent $n6 $null16
$ns connect $udp0 $null16
$udp0 set packetSize_ 1024

#Setup a UDP connection
set udp2 [new Agent/UDP]
$ns attach-agent $n13 $udp2
set null1 [new Agent/Null]
$ns attach-agent $n15 $null1
$ns connect $udp2 $null1
$udp2 set packetSize_ 1024

#Setup a UDP connection
set udp8 [new Agent/UDP]
$ns attach-agent $n8 $udp8
set null13 [new Agent/Null]
$ns attach-agent $n0 $null13
$ns connect $udp8 $null13
$udp8 set packetSize_ 1024

#Setup a UDP connection
set udp9 [new Agent/UDP]
$ns attach-agent $n11 $udp9
set null15 [new Agent/Null]
$ns attach-agent $n9 $null15
$ns connect $udp9 $null15
$udp9 set packetSize_ 1024

#Setup a UDP connection
set udp18 [new Agent/UDP]
$ns attach-agent $n2 $udp18
set null7 [new Agent/Null]
$ns attach-agent $n10 $null7
$ns connect $udp18 $null7
$udp18 set packetSize_ 1024


#===================================
#        Applications Definition        
#===================================
#Setup a CBR Application over UDP connection
set cbr3 [new Application/Traffic/CBR]
$cbr3 attach-agent $udp18
$cbr3 set packetSize_ 1024
$cbr3 set rate_ 1.0Mb
$cbr3 set random_ null
$ns at 1.0 "$cbr3 start"
$ns at 50.0 "$cbr3 stop"

#Setup a CBR Application over UDP connection
set cbr5 [new Application/Traffic/CBR]
$cbr5 attach-agent $udp8
$cbr5 set packetSize_ 1024
$cbr5 set rate_ 1.0Mb
$cbr5 set random_ null
$ns at 1.0 "$cbr5 start"
$ns at 50.0 "$cbr5 stop"

#Setup a CBR Application over UDP connection
set cbr7 [new Application/Traffic/CBR]
$cbr7 attach-agent $udp9
$cbr7 set packetSize_ 1024
$cbr7 set rate_ 1.0Mb
$cbr7 set random_ null
$ns at 1.0 "$cbr7 start"
$ns at 50.0 "$cbr7 stop"

#Setup a CBR Application over UDP connection
set cbr8 [new Application/Traffic/CBR]
$cbr8 attach-agent $udp0
$cbr8 set packetSize_ 1024
$cbr8 set rate_ 1.0Mb
$cbr8 set random_ null
$ns at 1.0 "$cbr8 start"
$ns at 50.0 "$cbr8 stop"

#Setup a CBR Application over UDP connection
set cbr9 [new Application/Traffic/CBR]
$cbr9 attach-agent $udp2
$cbr9 set packetSize_ 1024
$cbr9 set rate_ 1.0Mb
$cbr9 set random_ null
$ns at 1.0 "$cbr9 start"
$ns at 50.0 "$cbr9 stop"


#===================================
#        Termination        
#===================================
#Define a 'finish' procedure
proc finish {} {
    global ns tracefile namfile
    $ns flush-trace
    close $tracefile
    close $namfile
    # exec nam dsdv5.nam &
	exec gawk -f load_metrics.awk dsdv5.tr &
    exit 0
}

for {set i 0} {$i < $val(nn) } { incr i } {
    $ns at $val(stop) "\$n$i reset"
}

$ns at $val(stop) "$ns nam-end-wireless $val(stop)"
$ns at $val(stop) "finish"
$ns at $val(stop) "$ns halt"
$ns run

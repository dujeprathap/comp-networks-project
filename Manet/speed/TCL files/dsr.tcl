#===================================
#			DSR
#===================================

#===================================
#     Simulation parameters setup
#===================================
set list(chan)   Channel/WirelessChannel    ;# channel type
set list(prop)   Propagation/TwoRayGround   ;# radio-propagation model
set list(netif)  Phy/WirelessPhy            ;# network interface type
set list(mac)    Mac/802_11                 ;# MAC type
set list(ifq)    CMUPriQueue    ;# interface queue type
set list(ll)     LL                         ;# link layer type
set list(ant)    Antenna/OmniAntenna        ;# antenna model
set list(ifqlen) 55                      	;# max packet in ifq
set list(nn)     10                         ;# number of mobilenodes
set list(rp)     DSR                       ;# routing protocol
set list(x)      902                      ;# X dimension of topography
set list(y)      700                      ;# Y dimension of topography
set list(stop)   60.0                      ;# time of simulation end

#===================================
#        Initialization        
#===================================
#Creating a ns simulator
set ns [new Simulator]

#Setting up topography object
set topology [new Topography]
$topology load_flatgrid $list(x) $list(y)
create-god $list(nn)

#Tracing in a NS trace file
set tracefile [open dsr_trace.tr w]
$ns trace-all $tracefile

#Creating a nam file for simulation
set namfile [open dsr_nam.nam w]
$ns namtrace-all $namfile
$ns namtrace-all-wireless $namfile $list(x) $list(y)

#Creating a wireless channel
set chan [new $list(chan)];

#===================================
#     Parameters of nodes
#===================================
$ns node-config -adhocRouting  $list(rp) \
                -llType        $list(ll) \
                -macType       $list(mac) \
                -ifqType       $list(ifq) \
                -ifqLen        $list(ifqlen) \
                -antType       $list(ant) \
                -propType      $list(prop) \
                -phyType       $list(netif) \
                -channel       $chan \
                -topoInstance  $topology \
                -agentTrace    ON \
                -routerTrace   ON \
                -macTrace      ON \
                -movementTrace ON


#===================================
#        Nodes Creation        
#===================================

set n0 [$ns node]
$n0 set X_ 112
$n0 set Y_ 120
$n0 set Z_ 0.0
$ns initial_node_pos $n0 50
set n1 [$ns node]
$n1 set X_ 244
$n1 set Y_ 210
$n1 set Z_ 0.0
$ns initial_node_pos $n1 50
set n2 [$ns node]
$n2 set X_ 124
$n2 set Y_ 297
$n2 set Z_ 0.0
$ns initial_node_pos $n2 50
set n3 [$ns node]
$n3 set X_ 130
$n3 set Y_ 460
$n3 set Z_ 0.0
$ns initial_node_pos $n3 50
set n4 [$ns node]
$n4 set X_ 410
$n4 set Y_ 340
$n4 set Z_ 0.0
$ns initial_node_pos $n4 50
set n5 [$ns node]
$n5 set X_ 412
$n5 set Y_ 527
$n5 set Z_ 0.0
$ns initial_node_pos $n5 50
set n6 [$ns node]
$n6 set X_ 257
$n6 set Y_ 594
$n6 set Z_ 0.0
$ns initial_node_pos $n6 50
set n7 [$ns node]
$n7 set X_ 455
$n7 set Y_ 10
$n7 set Z_ 0.0
$ns initial_node_pos $n7 50
set n8 [$ns node]
$n8 set X_ 384
$n8 set Y_ 158
$n8 set Z_ 0.0
$ns initial_node_pos $n8 50
set n9 [$ns node]
$n9 set X_ 587
$n9 set Y_ 615
$n9 set Z_ 0.0
$ns initial_node_pos $n9 50

#===================================
#        Nodes Movement        
#===================================

puts "\nSet the speed of the nodes: "
set speed [gets stdin]

$ns at 0.0 " $n0 setdest 19 376 $speed " 
$ns at 0.0 " $n9 setdest 642 400 $speed " 

$ns at 20.0 " $n0 setdest 112 120 $speed " 
$ns at 20.0 " $n9 setdest 587 615 $speed " 

$ns at 40.0 " $n0 setdest 19 376 $speed " 
$ns at 40.0 " $n9 setdest 642 400 $speed " 

#===================================
#        Agents Definition        
#===================================

set udp1 [new Agent/UDP]
$udp1 set fid_ 1

$ns at 0.0 "$n0 label \"Source1\""
$ns at 0.0 "$n0 color green"
$n0 color green

$ns attach-agent $n0 $udp1

set null1 [new Agent/Null]
$ns at 0.0 "$n9 label \"Destination1\""
$ns at 0.0 "$n9 color green"
$n9 color green

$ns attach-agent $n9 $null1

$ns connect $udp1 $null1
	
#===================================
#        Applications Definition        
#===================================
set cbr1 [new Application/Traffic/CBR]
$cbr1 attach-agent $udp1
$cbr1 set packetSize_ 1024
$cbr1 set rate_ 1.0Mb
$cbr1 set interval_ 0.04
$cbr1 set random_ null

$ns at 0.0 "$cbr1 start"
$ns at $list(stop) "$cbr1 stop"

#===================================
#        Termination        
#===================================
#Define a 'finish' procedure
proc finish {} {
    global ns tracefile namfile
    $ns flush-trace
    close $tracefile
    close $namfile
    # exec nam dsr_nam.nam &
    exec gawk -f metrics.awk dsr_trace.tr &
    exit 0
}

for {set i 0} {$i < $list(nn) } { incr i } {
    $ns at $list(stop) "\$n$i reset"
}

$ns at $list(stop) "$ns nam-end-wireless $list(stop)"
$ns at $list(stop) "finish"
$ns at $list(stop) "$ns halt"
$ns run


#0 - 0 1 4 5 9
#17
#19 - 0 2 1 4 9
#26
#27 - 0 1 4 5 9


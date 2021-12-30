BEGIN{
	sentPckts1= 0.0
	recPckts1= 0.0
	
	totalPacketSize1= 0.0
	lastPcktRecTime1= 0.0

	pdr1= 0.0
	thr1= 0.0

}

{
	event= $1
	time= $2
	node= $3
	agent= $4
	packetSize= $8
	
	if(event == "s" && agent == "AGT"){
		if(node == "_0_"){
			sentPckts1++
		}
		
	}
	
	if(event == "r" && agent == "AGT"){
		if(node == "_9_"){
			totalPacketSize1= totalPacketSize1 + packetSize
			lastPcktRecTime1= time
			recPckts1++
		}
	}
	
}

END{
	pdr1= (recPckts1 / sentPckts1) * 100
	printf("\n===========================================\n")
	printf("\nPacket Delivery Ratio of Source 1\n")
	printf("%d ", pdr1)
	printf("\n")

	printf("\n===========================================\n")
	printf("\n===========================================\n")

	thr1= (((totalPacketSize1 * 8) / 40) / 1000)
	printf("\nThroughput of Source 1 (in Kbps)\n")
	printf("%f ", thr1)
	printf("\n")
	printf("\n===========================================\n")
}






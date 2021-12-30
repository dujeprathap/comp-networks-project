BEGIN{
	recPckts= 0
	routingPckts= 0
	load= 0.0
}

{
	event= $1
	time= $2
	node= $3
	agent= $4
	packetSize= $8

	if(event == "r"){
		if(agent == "RTR"){
			routingPckts++;
		}
		
		recPckts++;
	}
	
}

END{
	load= (routingPckts / recPckts) * 100
	printf("\n===========================================\n")
	printf("\nRouting Load\n")
	printf("%f", load)
	printf("\n")
}






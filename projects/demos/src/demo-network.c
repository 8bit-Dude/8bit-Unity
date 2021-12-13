
#include "unity.h"

extern const char nextKey, nextMsg[];

unsigned char svIp[] = { 199, 47, 196, 106 };
unsigned char message[] = "Packet received"; //Messages can be up-to 256 bytes long (this string is NULL ended for printing convenience)

int DemoNetwork(void)
{
	unsigned char state, line = 0;
	unsigned char* packet;
	
	// Clear screen
	clrscr();
	
	// Print header
	gotoxy (0, line++);
	cprintf("Initializing Network...");	
	
	// Init network and listen on UDP port 5000
	state = InitNetwork();
	if (state == ADAPTOR_ERR) {
		gotoxy (0, line++);
		cprintf("ADAPTOR ERROR");
		
	} else if (state == DHCP_ERR) {
		gotoxy (0, line++);
		cprintf("DHCP ERROR");
	
	} else {
		gotoxy (0, line++);
		cprintf("ADAPTOR AND DHCP OK!");
		
		///////////////////////////////////////////////////////
		// Setup connection to TCP echo server
		SlotTCP(0); OpenTCP(svIp, 1234);
		
		// Send TCP packet
		SendTCP(message, 16);

		// Fetch server response
		gotoxy (0, line);
		cprintf(" TCP:");
		packet = RecvTCP(3*TCK_PER_SEC); // Allow some time-out
		gotoxy (6, line++);
		if (!(int)packet)
			cprintf("Timeout");			
		else
			cprintf(packet);

		// Close connection
		CloseTCP();		

		///////////////////////////////////////////////////////
		// Setup connection to UDP echo server
		SlotUDP(0); OpenUDP(svIp, 1234, 4321);
		
		// Send UDP packet
		SendUDP(message, 16);

		// Fetch server response
		gotoxy (0, line);
		cprintf(" UDP:");
		packet = RecvUDP(3*TCK_PER_SEC); // Allow some time-out
		gotoxy (6, line++);
		if (!(int)packet)
			cprintf("Timeout");			
		else 
			cprintf(packet);

		// Close connection
		CloseUDP();
		
		///////////////////////////////////////////////////////
		// Setup request to URL server
		gotoxy (0, line);
		cprintf(" URL:");
		GetURL("http://www.8bit-unity.com/test.txt");
		packet = ReadURL(16, 3*TCK_PER_SEC); // Allow some time-out
		gotoxy (6, line++);
		if (!(int)packet)
			cprintf("Timeout");			
		else
			cprintf(packet);		
	}
	
	// Just show next page message
	gotoxy (0, line++);
	cprintf(nextMsg);
	while (!kbhit () || cgetc () != nextKey) {
	#if defined(__LYNX__) || defined(__NES__)
		UpdateDisplay(); // Manually refresh Display
	#endif		
	}

	// Reset state
	clrscr();	

    // Done
    return EXIT_SUCCESS;	
}

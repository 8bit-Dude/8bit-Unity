
#include "unity.h"

#define REQUEST_INFO 1

int DemoUDP(void)
{
	unsigned char state, line;
	unsigned char sendBuffer[1];	// Can be as long as you wish...
	unsigned int packet;
	
	// Clear screen
	clrscr();
	
	// Prepare bitmap
	InitBitmap();
	ClearBitmap();	
	EnterBitmapMode();
	
	// Print header
	paperColor = BLACK; inkColor = WHITE;
	PrintStr(0, line++, "Initializing Network...");
	
	// Init network and listen on UDP port 5000
	state = InitNetwork();
	if (state == ADAPTOR_ERR) {
		PrintStr(0, line++, "ADAPTOR ERROR");
		
	} else if (state == DHCP_ERR) {
		PrintStr(0, line++, "DHCP ERROR");
	
	} else {
		PrintStr(0, line++, "ADAPTOR AND DHCP OK!");

		// Setup UDP connection
		InitUDP(199, 47, 196, 106, 1234, 4321);
		
		// Send UDP packet
		sendBuffer[0] = REQUEST_INFO;
		SendUDP(sendBuffer, 1);
			
		// Fetch server response
		packet = RecvUDP(3*CLK_TCK); // Allow some time-out
		if (!packet) {
			// No data received: packet pointer is null
			PrintStr(0, line++, "ERROR: TIMEOUT");
			
		} else if (PEEK(packet) != REQUEST_INFO) {
			// Packet header does not match
			PrintStr(0, line++, "ERROR: CORRUPTION");
			
		} else {
			// Print packet contents (you can check byte length with PEEK(packet+1))
			PrintStr(0, line++, "Received Packet:");
			PrintStr(0, line++, (char*)(packet+2));
		}
	}

	// Run endlessly
	PrintStr(0,line++, "All demos completed");	
	while (1) {	
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
	}	
}


#include "unity.h"

#define REQUEST_INFO 1

int DemoUDP(void)
{
	unsigned char state, line;
	unsigned char sendBuffer[1];	// Can be as long as you wish...
	
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

		// Setup UDP listener
		ListenUDP(5000);
		
		// Send UDP packet
		udp_send_ip = EncodeIP(127,0,0,1);
		udp_send_port = 5000;
		sendBuffer[0] = REQUEST_INFO;
		SendUDPPacket(sendBuffer, 1);
			
		// Fetch server response
		RecvUDPPacket(5*CLK_TCK); // Allow short time-out, as server is localhost
		if (!udp_packet) {
			// No data received: packet pointer is null
			PrintStr(0, line++, "ERROR: TIMEOUT");
			
		} else if (PEEK(udp_packet) != REQUEST_INFO) {
			// Packet header does not match
			PrintStr(0, line++, "ERROR: CORRUPTION");
			
		} else {
			// Print packet contents (you can check byte length with PEEK(udp_packet+1))
			PrintStr(0, line++, "Received Packet:");
			PrintStr(0, line++, (char*)(udp_packet+2));
		}
	}
	
	// Wait for keyboard
	PrintStr(0,line++, "Press SPACE for next test");	
	cgetc();
	
    // Done
	ExitBitmapMode();	
    return EXIT_SUCCESS;	
}

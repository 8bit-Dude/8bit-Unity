
#include "unity.h"

#define REQUEST_INFO 1

int DemoUDP(void)
{
	unsigned char state, line;
	
	// Prepare bitmap
	InitBitmap();
	ClearBitmap();	
	EnterBitmapMode();
	
	// Print header
	paperColor = BLACK; inkColor = WHITE;
	PrintStr(0, line++, "Network demo");	
	
	// Init network and listen on UDP port 5000
	state = InitNetwork();
	if (state == ADAP_ERR) {
		PrintStr(0, line++, "ADAPTOR ERROR");
		
	} else if (state == DHCP_ERR) {
		PrintStr(0, line++, "DHCP ERROR");
	
	} else {
		PrintStr(0, line++, "ADAPTOR AND DHCP OK!");

		// Setup UDP listener
		ListenUDP(5000);
		
		// Send UDP packet
		//svAddr = ParseIP(127,0,0,1);
		udp_send_ip = parse_dotted_quad("127.0.0.1");
		udp_send_port = 5000;
		udp_send_buf[0] = REQUEST_INFO;
		SendUDPPacket(1);
			
		// Fetch server response
		RecvUDPPacket(5*CLK_TCK); // Allow short time-out, as server is localhost
		if (!udp_recv_buf[0]) {
			// No data received: packet length is 0
			PrintStr(0, line++, "ERROR: TIMEOUT");
			
		} else if (udp_recv_buf[1] != REQUEST_INFO) {
			// Packet header does not match
			PrintStr(0, line++, "ERROR: CORRUPTION");
			
		} else {
			// Print packet information
			PrintStr(0, line++, "Received Bytes:");
			PrintNum(16, line, udp_recv_buf[0]);
			PrintStr(0, line++, &buffer[2]);
		}
	}
	
	// Wait for keyboard
	PrintStr(0,line++, "Press SPACE for next test");	
	cgetc();
	
    // Done
	ExitBitmapMode();	
    return EXIT_SUCCESS;	
}

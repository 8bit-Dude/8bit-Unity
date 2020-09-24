
#include "unity.h"

unsigned char message[] = "8bit-Unity is easy!"; //Messages can be up-to 256 bytes long (this string is NULL ended for printing convenience)

int DemoNET(void)
{
	unsigned char state, line;
	unsigned int packet;
	
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
		// Setup UDP connection on slot #0
		SlotUDP(0); OpenUDP(199, 47, 196, 106, 1234, 4321);
		
		// Send UDP packet
		SendUDP(message, 20);

		// Fetch server response
		gotoxy (0, line);
		cprintf("UDP Packet:");
		packet = RecvUDP(3*TCK_PER_SEC); // Allow some time-out
		gotoxy (12, line++);
		if (!packet) {
			// No answer from server... :-(
			cprintf("TIMEOUT");			
		} else {
			// Print back echoed packet
			cprintf((char*)(packet));
		}

		// Close connection
		CloseUDP();
		
		///////////////////////////////////////////////////////
		// Setup TCP connection on slot #0
		SlotTCP(0); OpenTCP(199, 47, 196, 106, 1234);
		
		// Send TCP packet
		SendTCP(message, 20);

		// Fetch server response
		gotoxy (0, line);
		cprintf("TCP Packet:");
		packet = RecvTCP(3*TCK_PER_SEC); // Allow some time-out
		gotoxy (12, line++);
		if (!packet) {
			// No answer from server... :-(
			cprintf("TIMEOUT");			
		} else {
			// Print back echoed packet
			cprintf((char*)(packet));
		}

		// Close connection
		CloseTCP();
	}
		
	// Just show next page message
	gotoxy (0, line++);
	cprintf(pressKeyMsg);
	while (!kbhit () || cgetc () != keyNext) {
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
	}
	
    // Done
    return EXIT_SUCCESS;	
}

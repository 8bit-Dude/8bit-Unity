
#include "unity.h"

#define REQUEST_INFO 1

char sendBuffer[10];
unsigned int packet;

unsigned long svAddr;
unsigned int svPort = 5000;
unsigned int clPort = 5000;
unsigned int timeOut, timeSv;
unsigned char line;

void SendUDP(unsigned char length) 
{
	udp_send(sendBuffer, length, svAddr, svPort, clPort);
}

void ReceiveUDP(void)
{
	packet = &udp_recv_buf[0];
}

unsigned int UpdateUDP() 
{
	packet = 0;
	ip65_process();
	return packet;
}

unsigned int RequestInfo()
{
	// Send Request
	sendBuffer[0] = REQUEST_INFO;
	SendUDP(1);

	// Wait for answer
	UpdateUDP();
	timeSv = clock();
	while (!packet | PEEK(packet) != 1) { 
		if (clock() - timeSv > timeOut) { return 0; }
        UpdateUDP();
#if defined __APPLE2__
		tick();
#endif			
	}
	return packet;
}

void InitNetwork()
{
	// Set time-outs and svr address
	timeOut = 5*CLK_TCK;
	svAddr = parse_dotted_quad("127.0.0.1");
	
	// Init IP65 > DHCP > UDP
	PrintStr(0,line++,"INIT NETWORK");
	if (!ip65_init()) { 
		PrintStr(0,line++,"IP OK!");
		if (!dhcp_init()) {
			PrintStr(0,line++,"DHCP OK!");
			if (!udp_add_listener(clPort, ReceiveUDP)) {
				PrintStr(0,line++,"UDP READY!");
				packet = RequestInfo();	// First packet is always lost!
			}
		}
	}
	
}

int DemoUDP(void)
{
	unsigned char k,n;
	char buffer[16];
	
	// Prepare bitmap
	InitBitmap();
	ClearBitmap();	
	EnterBitmapMode();
	
	// Print header
	paperColor = BLACK; inkColor = WHITE;
	PrintStr(0,line++, "Network demo");	
	
	// Init network
	InitNetwork();
	
	// Fetch server list
	packet = RequestInfo();
	if (packet == 0) {
		// Timeout error
		PrintStr(0,line++,"ERROR: TIMEOUT");
		
	} else if (PEEK(packet) != REQUEST_INFO) {
		// Unexpected error
		PrintStr(0,line++,"ERROR: CORRUPTION");
		
	} else {
		// Print information
		n = PEEK(++packet);
		k = 0;
		while (k<n) {
			buffer[k++] = PEEK(++packet);
		}
		buffer[k] = 0;
		PrintStr(0,line++,"Received Packet: ");
		PrintStr(0,line++,&buffer[0]);
	}

	// Wait for keyboard
	PrintStr(0,line++, "Press SPACE for next test");	
	cgetc();
	
	// Black-out screen
	ExitBitmapMode();	
		
    // Done
    return EXIT_SUCCESS;	
}

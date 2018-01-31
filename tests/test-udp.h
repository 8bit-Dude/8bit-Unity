
#include "unity.h"

#define REQUEST_INFO 1

char sendBuffer[10];
unsigned int packet;

unsigned long svAddr;
unsigned int svPort = 5000;
unsigned int clPort = 5000;
unsigned int timeOut, timeSv;

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
	svAddr = parse_dotted_quad("192.168.1.21");
	
	// Init IP65 > DHCP > UDP
	printf("INIT NETWORK\n");
	if (!ip65_init()) { 
		printf("IP OK!\n");
		if (!dhcp_init()) {
			printf("DHCP OK!\n");
			if (!udp_add_listener(clPort, ReceiveUDP)) {
				printf("UDP READY!\n");
				packet = RequestInfo();	// First packet is always lost!
			}
		}
	}
	
}

int TestUDP(void)
{
	unsigned char k,n;
	char buffer[16];
	
	// Reset screen
	clrscr();
	
	// Init network
	InitNetwork();
	
	// Fetch server list
	packet = RequestInfo();
	if (packet == 0) {
		// Timeout error
		printf("ERROR: TIMEOUT\n");
		
	} else if (PEEK(packet) != REQUEST_INFO) {
		// Unexpected error
		printf("ERROR: CORRUPTION\n");
		
	} else {
		// Print information
		n = PEEK(++packet);
		k = 0;
		while (k<n) {
			buffer[k++] = PEEK(++packet);
		}
		buffer[k] = 0;
		printf(&buffer[0]);
	}

	// Wait for keyboard
	printf("PRESS ANY KEY FOR NEXT TEST\n");
	cgetc();
	
    // Done
    return EXIT_SUCCESS;	
}

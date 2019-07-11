
#include <peekpoke.h>
#include <time.h>

// Mode definitions
#define MODE_A2   1
#define MODE_A8   2
#define MODE_C64  3
#define MODE_LYNX 4
#define MODE_ORIC 5

unsigned char hubMode, hubNetwork, hubPacket, hubVersion;
clock_t hubClock;

unsigned char recvLen = 0;
unsigned char recvHead[5] = { 255, 255, 255, 255, 255 };
unsigned char recvBuf[256];

unsigned char tick;
void SendByte(unsigned char value)
{
	// Send 1 byte to printer port
	POKE(0x0301, value);		// Write 1 Byte to Printer Port
	POKE(0x0300, 175);			// Send STROBE signal (falling pulse)
	tick++; tick++; tick++; 
	POKE(0x0300, 255);			// Reset STROBE
}

void InitData()
{
	unsigned char i, j;
	
	// Reset ORA
	i = PEEK(0x0301); 

	// Read data header (mode, network, version)
	j = 0;
	while (j<3) {
		// Wait for flag CA1 = 1 (%00000010)
		i = 255;
		while (1) {  // Make sure we don't get stuck...
			if (PEEK(0x030d)&2) { break; }
			if (!i) { hubMode = 0; return; }		
			i--;
		}
		recvHead[j++] = PEEK(0x0301);
	}
 
	// Assign data from buffer
	hubMode	   = recvHead[0];
	hubNetwork = recvHead[1];
	hubVersion = recvHead[2];
}	

unsigned char InitHub()
{
	// Disable interrupts
	__asm__("sei");
	
	// Send reset code
	SendByte(213);

	// Open Port A for listening
	POKE(0x0303, 0);
	
	// Receive data from hub
	InitData();
	
	// Close port A
	POKE(0x0303, 255);

	// Enable interrupts
	__asm__("cli");	

	// Return HUB mode
	return hubMode;	
}

void FetchData() 
{
	unsigned char i, j;
	
	// Reset ORA
	i = PEEK(0x0301); 

	// Read data header (joy1, joy2, joy3, joy4, datalen)
	j = 0;
	while (j<5) {
		// Wait for flag CA1 = 1 (%00000010)
		i = 255;
		while (1) {  // Make sure we don't get stuck...
			if (PEEK(0x030d)&2) { break; }
			if (!i) { recvBuf[0] = 0; recvLen = 0; return; }		
			i--;
		}
		recvHead[j++] = PEEK(0x0301);
	}
	
	// 5th byte encodes length of remaining data
	recvLen = recvHead[4];
	
	// Read packet data
	j = 0;
	while (j<recvLen-1) {
		// Wait for flag CA1 = 1 (%00000010)
		i = 255;
		while (1) {  // Make sure we don't get stuck...
			if (PEEK(0x030d)&2) { break; }
			if (!i) { recvBuf[0] = 0; recvLen = 0; return; }		
			i--;
		}
		// Read next byte
		recvBuf[j++] = PEEK(0x0301);		
	}
	recvBuf[j] = 0;
	
	// Save packet information
	if (j) {
		hubPacket = 1;
	} else {
		hubPacket = 0;
	}
}

void FetchHub(void) 
{
	// Check hub is ready
	if (!hubMode) { return; }
	
	// Perform every 3 clock cycles max.
	if (clock() - hubClock < 3) { return; }
	hubClock = clock();

	// Disable interrupts
	__asm__("sei");
	
	// Send read code
	SendByte(170);
			
	// Open Port A for listening
	POKE(0x0303, 0);
	
	// Receive data from hub
	FetchData();
	
	// Close port A
	POKE(0x0303, 255);

	// Enable interrupts
	__asm__("cli");
}

void SendHub(unsigned char *buffer, unsigned char length) 
{
	unsigned char i = 0;
	
	// Check hub is ready
	if (!hubMode) { return; }	
	
	// Wait 3 clock cycles between HUB access
	while (clock() - hubClock < 3) { }
	hubClock = clock();

	__asm__("sei");
	
	// Send data length
	SendByte(length+1);
	
	// Send buffer to printer port
	while (i<length) {
		POKE(0x0301, buffer[i++]);	// Write 1 Byte to Printer Port
		POKE(0x0300, 175);			// Send STROBE signal
		tick++; tick++; tick++;
		POKE(0x0300, 255);			// Reset STROBE
	}
	
	// Enable interrupts	
	__asm__("cli");
}


#include "unity.h"
#include <peekpoke.h>
#include <time.h>
#if defined __LYNX__
  #include <serial.h>
  #define HUB_REFRESH_RATE 6	// every 6 clock cycles
#else
#endif

unsigned char hubVersion;
unsigned char hubState[8] = { 0, 255, 255, 255, 255, 0, 0, 0 };
unsigned char sendLen, sendBuffer[256];
unsigned char recvLen, recvID, recvBuffer[256];
unsigned char tick;
clock_t hubClock;

void SendByte(unsigned char value)
{
	// Send 1 byte to HUB
#if defined __LYNX__	
	unsigned char ch;
	while (ser_put(value) != SER_ERR_OK) ;	// Send byte
	while (ser_get(&ch) != SER_ERR_OK) ;	// Read byte (sent to oneself)
		
#elif defined __ATMOS__
	POKE(0x0301, value);		// Write 1 Byte to Printer Port
	POKE(0x0300, 175);			// Send STROBE signal (falling pulse)
	tick++; tick++; tick++; 
	POKE(0x0300, 255);			// Reset STROBE
#endif
}

unsigned char InitHub()
{
#if defined __LYNX__
	hubClock = clock();
	
#elif defined __ATMOS__
	unsigned char i, j;
	
	__asm__("sei");		// Disable interrupts
	SendByte(213);		// Send reset code
	POKE(0x0303, 0);	// Open Port A for listening

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
		hubState[j++] = PEEK(0x0301);
	}

	POKE(0x0303, 255);	// Close port A
	__asm__("cli");		// Resume interrupts
#endif
	return hubState[0];	
}

void SendHub() 
{
	unsigned char i, checksum = 0;
	
#if defined __LYNX__	
	// Clear Buffer
	while (ser_get(&i) == SER_ERR_OK) ;
	
	// Send: Header, Length, Buffer, Footer	
	SendByte(170);
	SendByte(recvID);
	SendByte(sendLen);
	for (i=0; i<sendLen; i++) { 
		SendByte(sendBuffer[i]); 
		checksum += sendBuffer[i];
	}
	SendByte(checksum);	
	sendLen = 0;
	
#elif defined __ATMOS__	
	// Interrupt Hub
	__asm__("sei");		// Disable interrupts
	SendByte(sendLen);	// Send data length
	while (i<sendLen) {
		POKE(0x0301, sendBuffer[i++]);	// Write 1 Byte to Printer Port
		POKE(0x0300, 175);			// Send STROBE signal
		tick++; tick++; tick++;
		POKE(0x0300, 255);			// Reset STROBE
	}
	__asm__("cli");		// Enable interrupts	
	sendLen = 0;	
#endif
}

void RecvHub(unsigned char timeOut) 
{
	unsigned char i, j, len;
	unsigned char header, footer, checksum;
	clock_t recvClock;
		
#if defined __LYNX__
	recvClock = clock();
	
	// Find header
	header = 0;
	while (header != 170) {
		if (clock() - recvClock >  timeOut) { hubState[0] = COMM_ERR_HEADER; return; }		
		ser_get(&header);
	}
	
	// Read joystick/mouse data
	for (i=1; i<8; i++) {
		while (ser_get(&hubState[i]) != SER_ERR_OK) { 
			if (clock() - recvClock >  timeOut) { hubState[0] = COMM_ERR_TRUNCAT; return; }
		}
	}
	
	// Get buffer length
	while (ser_get(&len) != SER_ERR_OK) { 
		if (clock() - recvClock >  timeOut) { hubState[0] = COMM_ERR_TRUNCAT; return; }		
	}
	
	// Read buffer data
	for (i=0; i<len; i++) {
		while (ser_get(&recvBuffer[i]) != SER_ERR_OK) { 
			if (clock() - recvClock >  timeOut) { hubState[0] = COMM_ERR_TRUNCAT; return; }
		}
	}
	
	// Get footer 
	while (ser_get(&footer) != SER_ERR_OK) { 
		if (clock() - recvClock >  timeOut) { hubState[0] = COMM_ERR_TRUNCAT; return; }		
	}
	
	// Verify checkum
	checksum = 0;
	for (i=1; i<8; i++) { checksum += hubState[i]; }
	for (i=0; i<len; i++) { checksum += recvBuffer[i]; }
	if (footer != checksum) { hubState[0] = COMM_ERR_CORRUPT; return; }
	
	// All good!
	if (len) {
		recvLen = len;		
		recvID = recvBuffer[0];
	}
	hubState[0] = COMM_ERR_OK;
	
#elif defined __ATMOS__	
	__asm__("sei");		// Disable interrupts
	SendByte(170);		// Send read code
	POKE(0x0303, 0);	// Open Port A for listening

	// Reset ORA
	i = PEEK(0x0301); 

	// Read data header (joy1, joy2, joy3, joy4, datalen)
	j = 0;
	while (j<5) {
		// Wait for flag CA1 = 1 (%00000010)
		i = 255;
		while (1) {  // Make sure we don't get stuck...
			if (PEEK(0x030d)&2) { break; }
			if (!i) { recvBuffer[0] = 0; recvLen = 0; return; }		
			i--;
		}
		hubState[j++] = PEEK(0x0301);
	}
	
	// 5th byte encodes length of remaining data
	recvLen = hubState[4];
	
	// Read packet data
	j = 0;
	while (j<recvLen-1) {
		// Wait for flag CA1 = 1 (%00000010)
		i = 255;
		while (1) {  // Make sure we don't get stuck...
			if (PEEK(0x030d)&2) { break; }
			if (!i) { recvBuffer[0] = 0; recvLen = 0; return; }		
			i--;
		}
		// Read next byte
		recvBuffer[j++] = PEEK(0x0301);		
	}
	recvBuffer[j] = 0;
	
	POKE(0x0303, 255);	// Close port A
	__asm__("cli");		// Resume interrupts	
#endif
}

unsigned char ok, err;

unsigned char UpdateHub() 
{
	unsigned char code;
	
	// Check hub is ready
	//if (!hubState[0] & ???????) { return COMM_ERR_NOHUB; }	
	
	// Check refresh rate
	if (clock() - hubClock < HUB_REFRESH_RATE) { return COMM_ERR_NODATA; }
	hubClock = clock();
	
	// Check for received data
	RecvHub(1);
	SendHub();
	return COMM_ERR_OK;
/* 	if (code) { 
		err+=1;
		PrintNum(3, 16, err);
	} else {
		ok+=1;
		PrintNum(0, 16, ok); 	
	} */				
}

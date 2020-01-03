/*
 * Copyright (c) 2019 Anthony Beaucamp.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented * you must not
 *   claim that you wrote the original software. If you use this software in a
 *   product, an acknowledgment in the product documentation would be
 *   appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not
 *   be misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any distribution.
 *
 *   4. The names of this software and/or it's copyright holders may not be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 */
 
#include "unity.h"

// Debugging flags
//#define DEBUG_HUB
#if defined DEBUG_HUB
  unsigned char ok, hd, tr, cu, cmd = 4;
#endif  

#if defined __LYNX__
  #include <serial.h>
  #define HUB_REFRESH_RATE 3	// every 3 clock cycles
#else
  #define HUB_REFRESH_RATE 3	// every 3 clock cycles
#endif

unsigned char hubState[5] = { COM_ERR_OFFLINE, 255, 255, 0, 0 };
unsigned char sendLen = 0, sendBuffer[192];
unsigned char recvLen = 0, recvBuffer[192];
unsigned char recvID = 0, tick;
clock_t hubClock = 0;

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

void SendHub()
{
	unsigned char i, checksum, packetLen;
	
#if defined __LYNX__	
	// Clear Buffer
	while (ser_get(&i) == SER_ERR_OK) ;
	
	// Send: Header, RecvID
	SendByte(170);
	SendByte(recvID);
	checksum = recvID;
	if (!sendLen) {
		// NULL command
		SendByte(0); 
		SendByte(checksum);	
	} else {
	#if defined DEBUG_HUB
		PrintStr(0, CHR_ROWS-2, "LEN ");
		PrintNum(4, CHR_ROWS-2, sendBuffer[0]);
	#endif				
		// Length > Command > Data
		packetLen = sendBuffer[0];
		SendByte(packetLen);
		for (i=1; i<=packetLen; i++) {
			SendByte(sendBuffer[i]); 
			checksum += sendBuffer[i];
		}
		SendByte(checksum);	
		//sendLen = 0;
		
		// Shift any remaining data
		packetLen += 1;
		if (packetLen < sendLen) {
			memcpy(&sendBuffer[0], &sendBuffer[packetLen], sendLen-packetLen);
			sendLen -= packetLen;		
		} else {
			sendLen = 0;
		}
	}
	
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
	unsigned char i, j, len, ID;
	unsigned char header, footer, checksum;
	clock_t recvClock;
		
#if defined __LYNX__
	recvClock = clock();
	
	// Find header
	header = 0;
	while (header != 170) {
		while (ser_get(&header) != SER_ERR_OK) { 
			if (clock() - recvClock >  timeOut) { 
				if (hubState[0] != COM_ERR_OFFLINE) { hubState[0] = COM_ERR_HEADER; }
				return; 
			}		
		}
	}

	// Get ID
	while (ser_get(&ID) != SER_ERR_OK) { 
		if (clock() - recvClock >  timeOut) { hubState[0] = COM_ERR_TRUNCAT; return; }		
	}

	// Read joystick/mouse data
	for (i=1; i<5; i++) {
		while (ser_get(&hubState[i]) != SER_ERR_OK) { 
			if (clock() - recvClock >  timeOut) { hubState[0] = COM_ERR_TRUNCAT; return; }
		}
	}
	
	// Get buffer length
	while (ser_get(&len) != SER_ERR_OK) { 
		if (clock() - recvClock >  timeOut) { hubState[0] = COM_ERR_TRUNCAT; return; }		
	}	
	
	// Read buffer data
	for (i=0; i<len; i++) {
		while (ser_get(&recvBuffer[i]) != SER_ERR_OK) { 
			if (clock() - recvClock >  timeOut) { hubState[0] = COM_ERR_TRUNCAT; return; }
		}
	}
	
	// Get footer 
	while (ser_get(&footer) != SER_ERR_OK) { 
		if (clock() - recvClock >  timeOut) { hubState[0] = COM_ERR_TRUNCAT; return; }		
	}
	
	// Verify checkum
	checksum = ID;
	for (i=1; i<5; i++) { checksum += hubState[i]; }
	for (i=0; i<len; i++) { checksum += recvBuffer[i]; }
	if (footer != checksum) { hubState[0] = COM_ERR_CORRUPT; return; }
	
	// All good!
	if (len) {
		recvID = ID;
		recvLen = len;		
	}
	hubState[0] = COM_ERR_OK;
	
#elif defined __ATMOS__	
	__asm__("sei");		// Disable interrupts
	SendByte(170);		// Send read code
	POKE(0x0303, 0);	// Open Port A for listening

	// Reset ORA
	i = PEEK(0x0301); 

	// Read data header (joy1, joy2, mouseX, mouseY)
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

void InitHub()
{
	// Send reset command
	sendLen = 0;
	sendBuffer[sendLen++] = 1;
	sendBuffer[sendLen++] = CMD_SYS_RESET;
	SendHub();	
}

void UpdateHub(unsigned char timeout) 
{	
	// Check refresh rate
	if (clock() - hubClock < HUB_REFRESH_RATE) { return; }
	hubClock = clock();
	RecvHub(timeout);
		
	// Check hub is connected
	if (hubState[0] == COM_ERR_OFFLINE) {
		InitHub();
	} else {
		SendHub();
	}
#if defined DEBUG_HUB
	PrintStr(0, CHR_ROWS-1, "PCKT ");
	     if (hubState[0] == COM_ERR_OK)      { ok+=1; PrintStr( 5, CHR_ROWS-1, "OK"); PrintNum( 7, CHR_ROWS-1, ok); }	
 	else if (hubState[0] == COM_ERR_HEADER)  { hd+=1; PrintStr(10, CHR_ROWS-1, "HD"); PrintNum(12, CHR_ROWS-1, hd); }	
 	else if (hubState[0] == COM_ERR_TRUNCAT) { tr+=1; PrintStr(15, CHR_ROWS-1, "TR"); PrintNum(17, CHR_ROWS-1, tr); } 
 	else if (hubState[0] == COM_ERR_CORRUPT) { cu+=1; PrintStr(20, CHR_ROWS-1, "CU"); PrintNum(22, CHR_ROWS-1, cu); }
#endif
}

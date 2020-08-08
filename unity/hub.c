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
 
#include "hub.h"
#include "unity.h"

// Debugging flags
//#define DEBUG_HUB
#if defined DEBUG_HUB
  unsigned char ok, hd, tr, cu, cmd = 4;
#endif  

#if defined __LYNX__
  #include <serial.h>
  struct ser_params comLynx = { SER_BAUD_62500, SER_BITS_8, SER_STOP_1, SER_PAR_SPACE, SER_HS_NONE };  							  
  unsigned char recvTimeOut;
  clock_t recvClock;
#else
  unsigned char tick;
#endif

unsigned char hubState[7] = { COM_ERR_OFFLINE, 255, 255, 255, 255, 80, 100 };
unsigned char sendID = 0, sendLen = 0, sendHub[256];
unsigned char recvID = 0, recvLen = 0, recvHub[256];
unsigned char countID = 0;

unsigned char QueueHub(unsigned char packetCmd, unsigned char* packetBuffer, unsigned char packetLen)
{
	unsigned char i;
	
	// Check if there is enough space in buffer
	if (packetLen > 255-sendLen)
		return 0;
	
	// Add to send buffer
	if (++countID > 15) { countID = 1; }
	sendHub[sendLen++] = countID;	
	sendHub[sendLen++] = packetLen+1;
	sendHub[sendLen++] = packetCmd;
	for (i=0; i<packetLen; i++) 
		sendHub[sendLen++] = packetBuffer[i];
	return 1;
}

void SendByte(unsigned char value)
{
	// Send 1 byte to HUB
#if defined __LYNX__	
	unsigned char ch;
	while (ser_put(value) != SER_ERR_OK) ;	// Send byte
	while (ser_get(&ch) != SER_ERR_OK) ;	// Read byte (sent to oneself)
		
#elif defined __ORIC__
	POKE(0x0301, value);		// Write 1 Byte to Printer Port
	POKE(0x0300, 175);			// Send STROBE (falling signal)
	tick++; tick++; tick++; 	// Wait 3 cycles
	POKE(0x0300, 255);			// Reset STROBE
#endif
}

unsigned char RecvByte(unsigned char* value)
{
	// Recv 1 byte from HUB
#if defined __LYNX__
	while (ser_get(value) != SER_ERR_OK) {
		if (clock() >  recvClock+recvTimeOut) { return 0; }
	}
	return 1;
			
#elif defined __ORIC__	
	unsigned char i = 255;
	while (1) {  // Countdown i to 0
		if (PEEK(0x030d)&2) { break; }	// Look for CA1 signal
		if (!i--) { return 0; }	
	}
	*value = PEEK(0x0301);
	return 1;
#endif
}


void SendHub()
{
	unsigned char i, checksum, packetLen;
#if defined __ORIC__	
	__asm__("sei");	// Disable interrupts
#elif defined __LYNX__	
	while (ser_get(&i) == SER_ERR_OK) ; // Clear UART Buffer
#endif

	// Send Header
	SendByte(170);
	
	// Send Packet ID
	if (sendLen) { sendID = sendHub[0]; }
	checksum = (recvID & 0xf0) + sendID;
	SendByte(checksum);
	
	// Send Packet Data (if any)
	if (sendLen) {	
		packetLen = sendHub[1];
		SendByte(packetLen);
		for (i=2; i<packetLen+2; i++) {
			SendByte(sendHub[i]); 
			checksum += sendHub[i];
		}
	} else {	
		SendByte(0); 
	}
	
	// Send footer
	SendByte(checksum);			
	
#if defined __ORIC__	
	__asm__("cli");	// Enable interrupts
#endif
}

void RecvHub(unsigned char timeOut) 
{
	unsigned char i, len, ID, packetLen;
	unsigned char header, footer, checksum;
		
#if defined __LYNX__
	recvClock = clock();	// Set clock
	recvTimeOut = timeOut;	// Set timeout
#elif defined __ORIC__	
	__asm__("sei");		// Disable interrupts
	SendByte(85);		// Send read code
	POKE(0x0303, 0);	// Set Port A as Input
	i = PEEK(0x0301);   // Reset ORA
#endif
	
	// Check header
	if (!RecvByte(&header) || header != 170) {
		if (hubState[0] != COM_ERR_OFFLINE) { hubState[0] = COM_ERR_HEADER; }
		return;
	}
			
	// Get packet ID
	if (!RecvByte(&ID)) { hubState[0] = COM_ERR_TRUNCAT; return; }
	
	// Read joystick/mouse data
	for (i=1; i<7; i++) {
		if (!RecvByte(&hubState[i])) { hubState[0] = COM_ERR_TRUNCAT; return; }
	}

	// Get buffer length
	if (!RecvByte(&len)) { hubState[0] = COM_ERR_TRUNCAT; return; }

	// Read buffer data
	for (i=0; i<len; i++) {
		if (!RecvByte(&recvHub[i])) { hubState[0] = COM_ERR_TRUNCAT; return; }
	}	

	// Get footer
	if (!RecvByte(&footer)) { hubState[0] = COM_ERR_TRUNCAT; return; }
		
#if defined __ORIC__	
	POKE(0x0303, 255);	// Set port A as Output
	__asm__("cli");		// Resume interrupts	
#endif

	// Verify checkum
	checksum = ID;
	for (i=1; i<7; i++) { checksum += hubState[i]; }
	for (i=0; i<len; i++) { checksum += recvHub[i]; }
	if (footer != checksum) { 
		hubState[0] = COM_ERR_CORRUPT; 
		return; 
	}
	hubState[0] = COM_ERR_OK;

	// Check packet reception
	if (ID != recvID) {
		// Check ID against last packet sent
		if ((ID & 0x0f) == sendID) {
			// Shift any remaining data
			packetLen = sendHub[1]+2;
			if (packetLen < sendLen) {
				memcpy(&sendHub[0], &sendHub[packetLen], sendLen-packetLen);
				sendLen -= packetLen;		
			} else {
				sendLen = 0;
			}		
		}
		
		// Check ID against last packet received
		if ((ID & 0xf0) != (recvID & 0xf0)) {
			// Accept packet
			if (len) {
				recvLen = len;
			}
		}
		
		// Update ID
		recvID = ID;
	}
	
#if defined DEBUG_HUB
	PrintNum(0,10,header); PrintNum(4,10,ID); 
	PrintNum(8,10,len); PrintNum(12,10,footer);
	for (i=1; i<7; i++) {
		PrintNum(4*(i-1),11,hubState[i]);
	}	
	for (i=0; i<len; i++) {
		PrintNum(4*(i%10),12+i/10,recvHub[i]);
	}	
#endif	
}

void InitHub()
{
#if defined __ORIC__
	// Set port A as Output
	POKE(0x0303, 255);
#elif defined __LYNX__
	// Setup Comlynx Interrupt
	ser_install(lynx_comlynx_ser);
	ser_open(&comLynx);
#endif		
	// Send reset command
	recvID = 0; sendID = 0;
	recvLen = 0; sendLen = 0;
	QueueHub(HUB_SYS_RESET, 0, 0);
	SendHub();	
	RecvHub(10);
}

void UpdateHub(unsigned char timeout) 
{			
	// Check hub was already initialized?
	if (hubState[0] == COM_ERR_OFFLINE) {
		InitHub();
	} else {
		// Restrict update rate (to prevent COM port flooding)
		while (clock() < recvClock+HUB_REFRESH_RATE)
			if (clock() > recvClock+timeout) { return; }
		SendHub();
		RecvHub(timeout);
	}
#if defined DEBUG_HUB
	PrintStr(0, CHR_ROWS-1, "LN ");
	PrintNum(2, CHR_ROWS-1, sendLen);	
	     if (hubState[0] == COM_ERR_OK)      { ok+=1; PrintStr( 5, CHR_ROWS-1, "OK"); PrintNum( 7, CHR_ROWS-1, ok); }	
 	else if (hubState[0] == COM_ERR_HEADER)  { hd+=1; PrintStr(10, CHR_ROWS-1, "HD"); PrintNum(12, CHR_ROWS-1, hd); }	
 	else if (hubState[0] == COM_ERR_TRUNCAT) { tr+=1; PrintStr(15, CHR_ROWS-1, "TR"); PrintNum(17, CHR_ROWS-1, tr); } 
 	else if (hubState[0] == COM_ERR_CORRUPT) { cu+=1; PrintStr(20, CHR_ROWS-1, "CU"); PrintNum(22, CHR_ROWS-1, cu); }
#endif
}

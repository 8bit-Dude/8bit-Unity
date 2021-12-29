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
#if defined(DEBUG_HUB)
  unsigned char i, ok, hd, tr, co, chk1, chk2, dbgID, dbgLen;
  clock_t tic, toc;
#endif  

// Serial functions
#if defined(__APPLE2__)
  unsigned char byte;	
  unsigned char RecvByte() { return 0; }
  unsigned char SendByte() { return 0; }
//  struct ser_params comParm = { SER_BAUD_19200, SER_BITS_8, SER_STOP_1, SER_PAR_NONE, SER_HS_HW };
#elif defined(__LYNX__)
  #include <serial.h>
  unsigned char __fastcall__ SerialOpen(void* data); // See serial.s
  unsigned char __fastcall__ SerialGet(unsigned char* data);
  unsigned char __fastcall__ SerialPut(unsigned char data);
  unsigned char comParm = 0;  //struct ser_params comParm = { SER_BAUD_62500, SER_BITS_8, SER_STOP_1, SER_PAR_SPACE, SER_HS_NONE };							  
  unsigned char byte;	
  unsigned char RecvByte() {
	unsigned char i = 255;
	while (i) {  // Countdown i to 0
		if (SerialGet(&byte) == SER_ERR_OK)		// Wait for byte to arrive on serial port
			return 1;
		i--;
	}
	return 0;
  }
  unsigned char SendByte() {
	while (SerialPut(byte) != SER_ERR_OK);  // Send byte
	while (SerialGet(&byte) != SER_ERR_OK); // Read byte (sent to oneself)	
	return 1;
  } 
#elif defined(__ATARI__) || defined(__CBM__) || defined(__NES__) || defined(__ORIC__)
  void InputMode();
  void OutputMode();
  unsigned char SendByte();
  unsigned char RecvByte();
  extern unsigned char byte;
#endif

#if defined __NES__
 #pragma bss-name(push, "XRAM")
#endif 

// Hub State
unsigned char hubState[7] = { COM_ERR_OFFLINE, 255, 255, 255, 255, 255, 255 };
unsigned char recvHub[HUB_RX_LEN], recvID = 0, recvLen = 0;
unsigned char sendHub[HUB_TX_LEN], sendID = 0, sendLen = 0;
unsigned char queueID = 0;

#if defined __NES__
  #pragma bss-name(pop)
#endif 

void RecvHub() 
{
	unsigned char i, ID, len, checksum, packetLen;

  #if defined(__ATARI__) || defined(__CBM__) || defined(__NES__) || defined(__ORIC__)
	// Ask Hub to send Data  
	byte = 85; if (!SendByte()) return;
	InputMode();
  #endif	
	
	// Check header
	if (!RecvByte()) {
		return;
	}
	if (byte != 170) {
		hubState[0] = COM_ERR_HEADER; 
		return;
	}
	hubState[0] = COM_ERR_TRUNCAT; 
					
	// Get packet ID
	if (!RecvByte()) return; 
	ID = byte;
	
	// Read joystick/mouse data
	for (i=1; i<7; i++) {
		if (!RecvByte()) return; 
		hubState[i] = byte;
	}

	// Get buffer length
	if (!RecvByte()) return; 
	len = byte;

	// Read buffer data
	for (i=0; i<len; i++) {
		if (!RecvByte()) return; 
		recvHub[i] = byte;
	}	

	// Get footer
	if (!RecvByte()) return; 

	// Verify checkum
	checksum = ID;
	for (i=1; i<7; i++) { checksum += hubState[i]; }
	for (i=0; i<len; i++) { checksum += recvHub[i]; }
#if defined DEBUG_HUB
	dbgID = ID; dbgLen = len; chk1 = byte; chk2 = checksum;
#endif	
	if (byte != checksum) { 
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
		if (len && ((ID & 0xf0) != (recvID & 0xf0)))
			recvLen = len;
		
		// Update ID
		recvID = ID;
	}
}

void SendHub()
{
	unsigned char i, checksum, packetLen;
	
  //#if defined(__APPLE2__) || 
  #if defined(__LYNX__)
	while (SerialGet(&i) == SER_ERR_OK) ; // Clear UART Buffer
  #elif defined(__ATARI__) || defined(__CBM__) || defined(__NES__) || defined(__ORIC__)
	OutputMode();
  #endif	
	
	// Send Header
	byte = 170; if (!SendByte()) return;
	
	// Send Packet ID
	if (sendLen) { sendID = sendHub[0]; }
	checksum = (recvID & 0xf0) + sendID;
	byte = checksum; if (!SendByte()) return;
	
	// Send Packet Data (if any)
	if (sendLen) {	
		packetLen = sendHub[1];
		byte = packetLen; if (!SendByte()) return;
		for (i=2; i<packetLen+2; i++) {
			byte = sendHub[i]; if (!SendByte()) return;
			checksum += sendHub[i];
		}
	} else {	
		byte = 0; if (!SendByte()) return; 
	}
	
	// Send footer
	byte = checksum; if (!SendByte()) return;
}

unsigned char QueueHub(unsigned char packetCmd, unsigned char* packetBuffer, unsigned char packetLen)
{
	// Check if there is enough space in buffer
	if (packetLen > HUB_TX_LEN-sendLen)
		return 0;
	
	// Add to send buffer
	if (++queueID > 15) { queueID = 1; }
	sendHub[sendLen++] = queueID;	
	sendHub[sendLen++] = packetLen+1;
	sendHub[sendLen++] = packetCmd;
	memcpy(&sendHub[sendLen], packetBuffer, packetLen);
	sendLen += packetLen;
	return 1;
}

clock_t updateClock;

void UpdateHub() 
{			
	// Throttle requests according to refresh rate
	if (clock() < updateClock)
		return;
	updateClock = clock()+HUB_REFRESH_RATE;

	// Was hub already initialized?
	if (hubState[0] == COM_ERR_OFFLINE) {
		// Queue reset command
		if (sendHub[0] != HUB_SYS_RESET) {
			// Setup initial request
			recvID = 0; recvLen = 0;
			sendID = 0; sendLen = 0;
			QueueHub(HUB_SYS_RESET, 0, 0);
		  //#if defined(__APPLE2__) || 
		  #if defined(__LYNX__)
			SerialOpen(&comParm); 
		  #endif		
		}
	}

  #if defined DEBUG_HUB
	tic = clock();
  #endif
  
  #if defined __ORIC__
	__asm__("sei");		// Disable interrupts
  #endif	

	// Process HUB I/O
	SendHub();
	RecvHub();	
	
  #if defined __ORIC__
	OutputMode();
	__asm__("cli");		// Resume interrupts	
  #endif	

  #if defined DEBUG_HUB
	toc = clock();
	gotoxy(0,  TXT_ROWS-3); cprintf("TC:%u ", toc-tic); 
	gotoxy(7,  TXT_ROWS-3); cprintf("LN:%u ", recvLen);
	 if (hubState[0] == COM_ERR_OK)      { ok+=1; gotoxy(14, TXT_ROWS-3); cprintf("OK:%u", ok); }
else if (hubState[0] == COM_ERR_HEADER)  { hd+=1; gotoxy(21, TXT_ROWS-3); cprintf("HD:%u", hd); }	
else if (hubState[0] == COM_ERR_TRUNCAT) { tr+=1; gotoxy(28, TXT_ROWS-3); cprintf("TR:%u", tr); } 
else if (hubState[0] == COM_ERR_CORRUPT) { co+=1; gotoxy(35, TXT_ROWS-3); cprintf("CO:%u", co); } 
	gotoxy(0,  TXT_ROWS-2); cprintf("CTRL:"); for (i=1; i<7; i++) { cprintf("%u,", hubState[i]); } 
	gotoxy(0,  TXT_ROWS-1); cprintf("ID:%u ", dbgID); 
	gotoxy(7,  TXT_ROWS-1); cprintf("C1:%u ", chk1); 
	gotoxy(14, TXT_ROWS-1); cprintf("C2:%u ", chk2);
  #endif
}

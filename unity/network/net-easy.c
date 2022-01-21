/*
 * Copyright (c) 2021 Anthony Beaucamp.
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

// Message Identifiers
#define EASY_HOST 11
#define EASY_JOIN 12
#define EASY_SEND 13
#define EASY_RECV 14
#define EASY_QUIT 15

// Internal Variables
clock_t easyTimer;
unsigned char easyProt = 0;
unsigned char easyLive = 0;
unsigned char easySlot = 0;
unsigned char easyBuffer[EASY_BUFFER];
unsigned char easyIP[] = { 199, 47, 196, 106 };
//unsigned char easyIP[] = { 127, 0, 0, 1 };

unsigned char EasyHost(unsigned char protocol, unsigned char slots, unsigned int *ID, unsigned int *PASS)
{
	unsigned char *packet;
	
	// Set 2s time-out
	easyTimer = clock()+2*TCK_PER_SEC;	
	
	// Prepare data
	easyProt = protocol;
	easyBuffer[0] = EASY_HOST;
	easyBuffer[1] = slots;
	
	// Initiate protocol
	if (easyProt == EASY_TCP) {
		OpenTCP(easyIP, 1499);
		SendTCP(easyBuffer, 2);		
	} else {
		OpenUDP(easyIP, 1499, 1499+(clock()&255));
		SendUDP(easyBuffer, 2);		
	}

	// Wait for answer back
	while (1) {
		// Fetch packet
		if (easyProt == EASY_TCP) {
			packet = RecvTCP(0);
		} else {
			packet = RecvUDP(0);
		}
		
		// Check contents
		if ((int)packet && packet[0] == EASY_HOST) {
			easyLive = 1;
			easySlot = 0;			
			*ID = PEEKW(&packet[1]);
			*PASS = PEEKW(&packet[3]);
			return EASY_OK;
		}
		
		// Check time-out
		if (clock() >= easyTimer)
			return EASY_TIMEOUT;
	}
}

unsigned char EasyJoin(unsigned char protocol, unsigned int *ID, unsigned int *PASS)
{
	unsigned char *packet;
	
	// Set 2s time-out
	easyTimer = clock()+2*TCK_PER_SEC;	

	// Prepare data
	easyProt = protocol;
	easyBuffer[0] = EASY_JOIN;
	POKEW(&easyBuffer[1], *ID);
	POKEW(&easyBuffer[3], *PASS);
	
	// Initiate protocol
	if (easyProt == EASY_TCP) {
		OpenTCP(easyIP, 1499);
		SendTCP(easyBuffer, 5);
	} else {
		OpenUDP(easyIP, 1499, 1499+(clock()&255));
		SendUDP(easyBuffer, 5);		
	}
		
	// Wait for answer back
	while (1) {
		// Fetch packet
		if (easyProt == EASY_TCP) {
			packet = RecvTCP(0);
		} else {
			packet = RecvUDP(0);
		}
		
		// Check contents
		if ((int)packet) {
			switch (packet[0]) {
			case EASY_JOIN:
				easyLive = 1;
				easySlot = packet[1];
				return EASY_OK;
			case EASY_CRED:
				return EASY_CRED;
			case EASY_FULL:
				return EASY_FULL;
			}
		}
		
		// Check time-out
		if (clock() >= easyTimer)
			return EASY_TIMEOUT;
	}
}

void EasySend(unsigned char *buffer, unsigned char len)
{
	if (!easyLive) 
		return;

	// Prepare data
	easyBuffer[0] = EASY_SEND;
	easyBuffer[1] = len;
	memcpy(&easyBuffer[2], buffer, len);
	
	// Select protocol	
	if (easyProt == EASY_TCP) {
		SendTCP(easyBuffer, len+2);
	} else {
		SendUDP(easyBuffer, len+2);
	}
}

unsigned char *EasyRecv(unsigned char timeout, unsigned char* len)
{
	unsigned char *packet;
	
	if (!easyLive) 
		return 0;

	// Fetch packet
	if (easyProt == EASY_TCP) {
		packet = RecvTCP(timeout);
	} else {
		packet = RecvUDP(timeout);
	}
		
	// Check contents
	if ((int)packet && packet[0] == EASY_RECV) {
		*len = packet[1];
		return &packet[2];
	} else {
		*len = 0;
		return 0;		
	}
}

void EasyQuit(void)
{
	// Reset live flag
	if (!easyLive) 
		return;
	easyLive = 0;
	
	// Prepare data
	easyBuffer[0] = EASY_QUIT;
	
	// Select protocol		
	if (easyProt == EASY_TCP) {
		SendTCP(easyBuffer, 1);
		CloseTCP();
	} else {		
		SendUDP(easyBuffer, 1);
		CloseUDP();
	}
}

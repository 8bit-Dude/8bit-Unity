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
#include "hub.h"

//#define DEBUG_TCP

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifndef __HUB__
  // Use IP65 library
#endif

void SlotTCP(unsigned char slot)
{
#ifdef __HUB__
	QueueHub(HUB_TCP_SLOT, &slot, 1);
#else
#endif
}

void OpenTCP(unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4, unsigned int svPort)
{
#ifdef __HUB__
	// Ask HUB to set up connection
	clock_t timer;
	unsigned char buffer[6];
	buffer[0] = ip1;
	buffer[1] = ip2;
	buffer[2] = ip3;
	buffer[3] = ip4;
	buffer[4] = svPort & 0xFF;
	buffer[5] = svPort >> 8;	
	QueueHub(HUB_TCP_OPEN, buffer, 6);	
	
	// Wait while HUB sets-up connection
	timer = clock();
	while ((clock()-timer) < 2*TCK_PER_SEC) {
		UpdateHub(5);
	}
#else
#endif
}

void CloseTCP()
{
#ifdef __HUB__
	QueueHub(HUB_TCP_CLOSE, 0, 0);
#else
#endif
}

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

void SendTCP(unsigned char* buffer, unsigned char length) 
{
#ifdef __HUB__
	QueueHub(HUB_TCP_SEND, buffer, length);
#else
#endif
}

unsigned int RecvTCP(unsigned int timeOut)
{	
#ifdef __HUB__
	// Check if data was received from Hub
	clock_t timer = clock();
	while (1) {
		// Check if we received packet
		if (recvLen && recvHub[0] == HUB_TCP_RECV) { 
		#if defined DEBUG_TCP
			PrintStr(0, 13, "TCP:");
			PrintNum(5, 13, recvLen);
		#endif		
			recvLen = 0;  // Clear packet
			return &recvHub[2]; 
		}		
		
		// Inquire next packet
		UpdateHub(MIN(5,timeOut));	

		// Check time-out
		if (clock() >= timer+timeOut) { return 0; }	
	}
#else
#endif
}

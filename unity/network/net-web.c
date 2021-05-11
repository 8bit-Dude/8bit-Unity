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

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

void OpenWEB(unsigned int port, unsigned int timeOut)
{
#if defined __HUB__
	// Ask HUB to set up connection
	unsigned char buffer[4];
	buffer[0] = port & 0xFF;
	buffer[1] = port >> 8;	
	buffer[2] = timeOut & 0xFF;
	buffer[3] = timeOut >> 8;	
	QueueHub(HUB_WEB_OPEN, buffer, 4);
	UpdateHub();
#elif defined __FUJINET__	
	// TODO	
#elif defined __IP65__
	// TODO	
#endif
}

void CloseWEB()
{
#if defined __HUB__
	while (sendLen) UpdateHub();
	QueueHub(HUB_WEB_CLOSE, 0, 0);
	UpdateHub(); // Send immediately
#elif defined __FUJINET__	
	// TODO	
#elif defined __IP65__
	// TODO	
#endif
}

void HeaderWEB(unsigned char* buffer, unsigned char length) 
{
#if defined __HUB__
	while (sendLen) UpdateHub();
	QueueHub(HUB_WEB_HEADER, buffer, length);
	UpdateHub(); // Send immediately
#elif defined __FUJINET__	
	// TODO	
#elif defined __IP65__
	// TODO	
#endif
}

void BodyWEB(unsigned char* buffer, unsigned char length) 
{
#if defined __HUB__
	while (sendLen) UpdateHub();
	QueueHub(HUB_WEB_BODY, buffer, length);
	UpdateHub(); // Send immediately
#elif defined __FUJINET__	
	// TODO	
#elif defined __IP65__
	// TODO	
#endif
}

void SendWEB() 
{
#if defined __HUB__
	// Flush Hub Queue
	while (sendLen) UpdateHub();
	QueueHub(HUB_WEB_SEND, 0, 0);
	UpdateHub(); // Send immediately
#elif defined __FUJINET__	
	// TODO	
#elif defined __IP65__
	// TODO	
#endif
}

unsigned char* RecvWEB(unsigned int timeOut)
{	
#if defined __HUB__
	// Wait until data is received from Hub
	clock_t timer = clock()+timeOut;
	while (!recvLen || recvHub[0] != HUB_WEB_RECV) {
		if (clock() > timer) return 0;
		UpdateHub();	
	}
	recvLen = 0;  // Clear packet
	return &recvHub[2]; 
#elif defined __FUJINET__	
	// TODO	
	return 0;
#elif defined __IP65__
	// TODO	
	return 0;
#endif
}

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

#ifdef __APPLE2__
  #pragma code-name("LOWCODE")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM2")
#endif

#ifndef __HUB__
#endif

void GetURL(unsigned char* url)
{
#ifdef __HUB__
	QueueHub(HUB_URL_GET, url, strlen(url));	
	UpdateHub();
#else
#endif
}

unsigned char* ReadURL(unsigned char size, unsigned int timeOut)
{	
#ifdef __HUB__
	// Wait until data is received from Hub
	clock_t timer = clock()+timeOut;
	QueueHub(HUB_URL_READ, &size, 1);	
	while (!recvLen || recvHub[0] != HUB_URL_READ) {
		if (clock() > timer) return 0;
		UpdateHub();	
	}
	recvLen = 0;  // Clear packet
	return &recvHub[2]; 
#else
	return 0;
#endif
}

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

#if defined __HUB__
  #include "hub.h"
#elif defined __FUJINET__	
  // Nothing
#else
  #include "IP65/ip65.h"  
  #define URL_LEN 512	// Note: HTTP header (parsed out) takes about 256 bytes...
  unsigned char url_buf[URL_LEN];
  unsigned int url_ind, url_len;
#endif

void GetURL(unsigned char* url)
{
#if defined __HUB__
	QueueHub(HUB_URL_GET, url, strlen(url));	
	UpdateHub();
	
#elif defined __FUJINET__
	// Open HTTP address
	strcpy(&fujiHostname[0], "N:HTTP");
	strcpy(&fujiHostname[6], &url[4]);
	FujiOpen();
	
#else
	// Read URL and strip out HTTP header
	url_len = url_download((const char*)url, url_buf, URL_LEN);	
	url_ind = 0;
	while (url_ind<url_len) {
		if (url_buf[url_ind+0] == 13 && url_buf[url_ind+1] == 10 && url_buf[url_ind+2] == 13 && url_buf[url_ind+3] == 10)
			break;
		url_ind++;
	}
	url_ind += 4;
#endif
}

unsigned char* ReadURL(unsigned char size, unsigned int timeOut)
{	
#if defined __HUB__
	// Wait until data is received from Hub
	clock_t timer = clock()+timeOut;
	QueueHub(HUB_URL_READ, &size, 1);	
	while (!recvLen || recvHub[0] != HUB_URL_READ) {
		if (clock() > timer) return 0;
		UpdateHub();	
	}
	recvLen = 0;  // Clear packet
	return &recvHub[2]; 
	
#elif defined __FUJINET__	
	clock_t timer = clock()+timeOut;
	OS.dvstat[0] = 0;
	while (!OS.dvstat[0]) {
		// Check status once per frame
		FujiStatus();
		if (clock() > timer) return 0;
	}
	
	// Get data then close connection
	FujiRead();
	FujiClose();
	return fujiBuffer;
	
#else
	unsigned char *ptr;
	if (url_len && url_ind < url_len-1) {
		ptr = &url_buf[url_ind];
		url_ind += size;
	}
	return ptr;
#endif
}

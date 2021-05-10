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

#if defined __HUB__
  // Nothing
#elif defined __FUJINET__
  // Nothing
#elif defined __IP65__
  #define EncodeIP(a,b,c,d) (a+b*256+c*65536+d*16777216)
  unsigned char* tcp_buffer, tcp_len;
  void __fastcall__ PacketTCP(const unsigned char* buf, int len) { 
	tcp_buffer = (unsigned char*)buf;
	tcp_len = len;	
  }
#endif

void SlotTCP(unsigned char slot)
{
#if defined __HUB__
	QueueHub(HUB_TCP_SLOT, &slot, 1);
	UpdateHub();
	
#elif defined __FUJINET__	
	// TODO	
	
#elif defined __IP65__
	// TODO
#endif
}

void OpenTCP(unsigned char* ip, unsigned int svPort)
{
#if defined __HUB__
	// Ask HUB to set up connection
	unsigned char buffer[6];
	memcpy(buffer, ip, 4);
	POKEW(&buffer[4], svPort);	
	QueueHub(HUB_TCP_OPEN, buffer, 6);	
	UpdateHub();
	
#elif defined __FUJINET__
	// Open TCP address
	sprintf(fujiHost, "N:TCP://%i.%i.%i.%i:%i/", ip[0], ip[1], ip[2], ip[3], svPort);
	FujiOpen(0x71, 0);
	
#elif defined __IP65__
	unsigned long svIp = EncodeIP(ip[0], ip[1], ip[2], ip[3]);
	tcp_connect(svIp, svPort, PacketTCP);
#endif
}

void CloseTCP()
{
#if defined __HUB__
	QueueHub(HUB_TCP_CLOSE, 0, 0);
	UpdateHub();
	
#elif defined __FUJINET__
	FujiClose(0x71);
	
#elif defined __IP65__
	tcp_close();
#endif
}

void SendTCP(unsigned char* buffer, unsigned char length) 
{
#if defined __HUB__
	QueueHub(HUB_TCP_SEND, buffer, length);
	
#elif defined __FUJINET__
	memcpy(fujiBuffer, buffer, length);
	FujiWrite(0x71, length);
	
#elif defined __IP65__
	tcp_send(buffer, length);
#endif
}

unsigned char* RecvTCP(unsigned int timeOut)
{	
	clock_t timer = clock()+timeOut;
#if defined __HUB__
	// Wait until data is received from Hub
	while (!recvLen || recvHub[0] != HUB_TCP_RECV) {
		if (clock() > timer) return 0;
		UpdateHub();	
	}
	recvLen = 0;  // Clear packet
	return &recvHub[2]; 

#elif defined __FUJINET__
	// Wait until timeout expires...
	while (!fujiReady) {
		if (clock() > timer) return 0;
	}
	if (FujiRead(0x71)) {
		return fujiBuffer;
	} else {
		return 0;
	}
	
#elif defined __IP65__
	// Process IP65 until receiving packet
	while (!tcp_len) {
		if (clock() > timer) return 0;
		ip65_process();
	#if defined __APPLE2__
		if (timeOut) { wait(1); } else { clk += 1; }
	#endif
	}
	tcp_len = 0;
	return tcp_buffer;	
#endif
}

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

#ifdef __HUB__
  #include "hub.h"
#else
  #include "IP65/ip65.h"
  #define EncodeIP(a,b,c,d) (a+b*256+c*65536+d*16777216)
  unsigned char* tcp_buffer, tcp_len;
  void __fastcall__ PacketTCP(const unsigned char* buf, int len) { 
	tcp_buffer = (unsigned char*)buf;
	tcp_len = len;	
  }
#endif

void SlotTCP(unsigned char slot)
{
#ifdef __HUB__
	QueueHub(HUB_TCP_SLOT, &slot, 1);
	UpdateHub();		
#else
#endif
}

void OpenTCP(unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4, unsigned int svPort)
{
#ifdef __HUB__
	// Ask HUB to set up connection
	unsigned char buffer[6];
	buffer[0] = ip1;
	buffer[1] = ip2;
	buffer[2] = ip3;
	buffer[3] = ip4;
	buffer[4] = svPort & 0xFF;
	buffer[5] = svPort >> 8;	
	QueueHub(HUB_TCP_OPEN, buffer, 6);	
	UpdateHub();
#else
	unsigned long svIp = EncodeIP(ip1,ip2,ip3,ip4);
	tcp_connect(svIp, svPort, PacketTCP);
#endif
}

void CloseTCP()
{
#ifdef __HUB__
	QueueHub(HUB_TCP_CLOSE, 0, 0);
	UpdateHub();
#else
	tcp_close();
#endif
}

void SendTCP(unsigned char* buffer, unsigned char length) 
{
#ifdef __HUB__
	QueueHub(HUB_TCP_SEND, buffer, length);
#else
	tcp_send(buffer, length);
#endif
}

unsigned char* RecvTCP(unsigned int timeOut)
{	
	clock_t timer = clock()+timeOut;
#ifdef __HUB__
	// Wait until data is received from Hub
	while (!recvLen || recvHub[0] != HUB_TCP_RECV) {
		if (clock() > timer) return 0;
		UpdateHub();	
	}
	recvLen = 0;  // Clear packet
	return &recvHub[2]; 
#else
	// Process IP65 until receiving packet
	while (!tcp_len) {
		if (clock() > timer) return 0;
		ip65_process();
	#if defined __APPLE2__
		wait(1);
	#endif
	}
	tcp_len = 0;
	return tcp_buffer;	
#endif
}

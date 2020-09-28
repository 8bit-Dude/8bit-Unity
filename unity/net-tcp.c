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

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifndef __HUB__
  // Use IP65 library
  #define EncodeIP(a,b,c,d) (a+b*256+c*65536+d*16777216)
  unsigned char* tcp_recv_packet;
  void __fastcall__ PacketTCP(const unsigned char* buf, int len) { tcp_recv_packet = buf; }
  unsigned char __fastcall__ tcp_connect(unsigned long dest, unsigned int dest_port, void (*callback)(const unsigned char* buf, int len));
  unsigned char __fastcall__ tcp_send(const unsigned char* buf, unsigned int len);
  unsigned char tcp_close(void);
  unsigned char ip65_process(void);
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
	while ((clock()-timer) < 2*TCK_PER_SEC)
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
#ifdef __HUB__
	// Wait until data is received from Hub
	clock_t timer = clock()+timeOut;
	while (!recvLen || recvHub[0] != HUB_TCP_RECV) {
		if (clock() > timer) return 0;
		UpdateHub();	
	}
	recvLen = 0;  // Clear packet
	return &recvHub[2]; 
#else
	// Try to process UDP
	clock_t timer = clock()+timeOut;
	*tcp_recv_packet = 0;
	while (!*tcp_recv_packet) {
		if (clock() > timer) return 0;
		ip65_process();
	#if defined __APPLE2__
		wait(1);
	#endif
	}
	return tcp_recv_packet;	
#endif
}

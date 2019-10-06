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

#define EncodeIP(a,b,c,d) (a+b*256+c*65536+d*16777216)

#ifndef __HUB__
  unsigned long udp_send_ip;
  unsigned int udp_send_port, udp_recv_port;
#else
  extern unsigned char hubPacket;	// see Oric/hub.c
  extern unsigned char recvBuf[256];
  void SendHub(unsigned char *buffer, unsigned char length);
  void FetchHub(void);
#endif
unsigned int udp_packet;

// IP65 functions (see linked libraries)
#ifndef __HUB__
  extern unsigned char udp_recv_buf[192];   // Buffer with data received (length hard-coded in IP65)
  unsigned char ip65_init(void);
  unsigned char ip65_process(void);
  unsigned char dhcp_init(void);
  unsigned char __fastcall__ udp_send(const unsigned char* buf, unsigned int len, unsigned long dest,  unsigned int dest_port, unsigned int src_port);
  unsigned char __fastcall__ udp_add_listener(unsigned int port, void (*callback)(void));
  unsigned char __fastcall__ udp_remove_listener(unsigned int port);
#endif

unsigned char InitNetwork(void)
{
#ifndef __HUB__
	// Init IP65 and DHCP
	if (ip65_init()) { return ADAPTOR_ERR; }
	if (dhcp_init()) { return DHCP_ERR; }
	return NETWORK_OK;
#else
	if (hubMode && hubNetwork) {
		return NETWORK_OK;
	} else {
		return ADAPTOR_ERR;
	}
#endif
}

#ifndef __HUB__
void PacketReceived(void)
{
	udp_packet = &udp_recv_buf[0];
}
#endif

void InitUDP(unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4, unsigned int svPort, unsigned int clPort)
{
#ifndef __HUB__
	unsigned char dummy[1];
	udp_send_ip = EncodeIP(ip1,ip2,ip3,ip4);
	udp_send_port = svPort;
	udp_recv_port = clPort;
	if (!udp_add_listener(clPort, PacketReceived)) {
		// Send dummy packet, as first one is always lost!
		dummy[0] = 0;
		SendUDP(dummy, 1);
		RecvUDP(5*CLK_TCK);
	}
#else
	// Ask HUB to set up connection
	clock_t timer;
	unsigned char buffer[9];
	unsigned char i = 0;
	buffer[i++] = ip1;
	buffer[i++] = ip2;
	buffer[i++] = ip3;
	buffer[i++] = ip4;
	buffer[i++] = svPort & 0xFF;
	buffer[i++] = svPort >> 8;	
	buffer[i++] = clPort & 0xFF;
	buffer[i++] = clPort >> 8;
	buffer[i++] = 1;	// Init UDP 		
	SendHub(buffer, 9);
	
	// Wait while HUB sets-up connection
	timer = clock();
	while (clock() - timer < CLK_TCK) { }
#endif
}

void SendUDP(unsigned char* buffer, unsigned char length) 
{
#ifndef __HUB__
	udp_send(buffer, length, udp_send_ip, udp_send_port, udp_recv_port);
#else
	buffer[length++] = 2;	// Send UDP 
	SendHub(buffer, length);
#endif
}

unsigned char RecvUDP(unsigned int timeOut)
{	
	// Try to process UDP
	clock_t timer;
	udp_packet = 0;
#ifndef __HUB__
	ip65_process();
#else
	FetchHub();
	if (hubPacket) {
		udp_packet = &recvBuf[0];
		hubPacket = 0;
	}
#endif
	if (!timeOut) { return udp_packet; }
	
	// Wait for an answer (within time-out)
	timer = clock();
	while (!udp_packet) { 
		if (clock() - timer > timeOut) { break; }
#ifndef __HUB__
		ip65_process();
#else
		FetchHub();
		if (hubPacket) {
			udp_packet = &recvBuf[0];
			hubPacket = 0;
		}
#endif
#if defined __APPLE2__
		tick();
#endif			
	}
	return udp_packet;
}

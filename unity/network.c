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

//#define DEBUG_NET

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __HUB__
  // Use serial communication
#else
  // Use IP65 library
  #define EncodeIP(a,b,c,d) (a+b*256+c*65536+d*16777216)
  unsigned long udp_send_ip;
  unsigned int udp_send_port;
  unsigned int udp_recv_port;
  unsigned int udp_recv_packet;
  extern unsigned char udp_recv_buf[192];   // Buffer with data received (length hard-coded in IP65)
  unsigned char __fastcall__ udp_send(const unsigned char* buf, unsigned int len, unsigned long dest,  unsigned int dest_port, unsigned int src_port);
  unsigned char __fastcall__ udp_add_listener(unsigned int port, void (*callback)(void));
  unsigned char __fastcall__ udp_remove_listener(unsigned int port);
  unsigned char ip65_init(void);
  unsigned char ip65_process(void);
  unsigned char dhcp_init(void);
#endif

unsigned char InitNetwork(void)
{
#ifdef __HUB__
	// Detect if HUB is connected
	clock_t timer = clock();
	while ((clock()-timer) < 2*TCK_PER_SEC) { 
		if (hubState[0] == COM_ERR_OK) { return NETWORK_OK; }
		UpdateHub(5);
	}
	return ADAPTOR_ERR;
#else
	// Init IP65 and DHCP
	if (ip65_init()) { return ADAPTOR_ERR; }
	if (dhcp_init()) { return DHCP_ERR; }
	return NETWORK_OK;
#endif
}

#ifndef __HUB__
void PacketReceived(void)
{
	udp_recv_packet = &udp_recv_buf[0];
}
#endif

void InitUDP(unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4, unsigned int svPort, unsigned int clPort)
{
#ifdef __HUB__
	// Ask HUB to set up connection
	clock_t timer;
	sendHub[sendLen++] = NextID();
	sendHub[sendLen++] = 9;
	sendHub[sendLen++] = HUB_UDP_INIT;
	sendHub[sendLen++] = ip1;
	sendHub[sendLen++] = ip2;
	sendHub[sendLen++] = ip3;
	sendHub[sendLen++] = ip4;
	sendHub[sendLen++] = svPort & 0xFF;
	sendHub[sendLen++] = svPort >> 8;	
	sendHub[sendLen++] = clPort & 0xFF;
	sendHub[sendLen++] = clPort >> 8;
	
	// Wait while HUB sets-up connection
	timer = clock();
	while ((clock()-timer) < 2*TCK_PER_SEC) {
		UpdateHub(5);
	}
#else
	// Set-up UDP params and listener
	unsigned char dummy[1];
	udp_send_ip = EncodeIP(ip1,ip2,ip3,ip4);
	udp_send_port = svPort;
	udp_recv_port = clPort;
	if (!udp_add_listener(clPort, PacketReceived)) {
		// Send dummy packet, as first one is always lost!
		dummy[0] = 0;
		SendUDP(dummy, 1);
		RecvUDP(2*TCK_PER_SEC);
	}
#endif
}

void SendUDP(unsigned char* buffer, unsigned char length) 
{
#ifdef __HUB__
	unsigned char i;
	sendHub[sendLen++] = NextID();	
	sendHub[sendLen++] = length+1;
	sendHub[sendLen++] = HUB_UDP_SEND;
	for (i=0; i<length; i++) {
		sendHub[sendLen++] = buffer[i];
	}
#else
	udp_send(buffer, length, udp_send_ip, udp_send_port, udp_recv_port);
#endif
}

unsigned int RecvUDP(unsigned int timeOut)
{	
#ifdef __HUB__
	// Check if data was received from Hub
	clock_t timer = clock();
	while (1) {
		// Check if we received packet
		if (recvLen && recvHub[0] == HUB_UDP_RECV) { 
		#if defined DEBUG_NET
			PrintStr(0, 13, "UDP:");
			PrintNum(5, 13, recvLen);
		#endif		
			recvLen = 0;  // Clear packet
			return &recvHub[1]; 
		}		
		
		// Inquire next packet
		UpdateHub(MIN(5,timeOut));	

		// Check time-out
		if ((clock() - timer) >= timeOut) { return 0; }	
	}
#else
	// Try to process UDP
	clock_t timer = clock();
	udp_recv_packet = 0;
	while (1) {
		// Check if we received packet
		ip65_process();
		if (udp_recv_packet) { return udp_recv_packet; }
		
		// Check time-out
		if (clock() - timer >= timeOut) { return 0; }	
	#if defined __APPLE2__
		tick();
	#endif
	}
#endif
}

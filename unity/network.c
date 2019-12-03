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
  #pragma code-name("SHADOW_RAM2")
#endif

#ifdef __HUB__
  extern unsigned char sendLen, recvLen;
  extern unsigned char sendBuffer[256];
  extern unsigned char recvBuffer[256];
#else
  #define EncodeIP(a,b,c,d) (a+b*256+c*65536+d*16777216)
  unsigned long udp_send_ip;
  unsigned int udp_send_port;
  unsigned int udp_recv_port;
  unsigned int udp_recv_packet;
#endif

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
#ifdef __HUB__
	// Detect if HUB is connected
	clock_t timer = clock();
	while ((clock()-timer) < 2*CLK_TCK) { 
		if (hubState[0] == COM_ERR_OK) { return NETWORK_OK; }
		UpdateHub();
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
	unsigned char len = 0;
	sendBuffer[len++] = CMD_UDP_INIT;
	sendBuffer[len++] = ip1;
	sendBuffer[len++] = ip2;
	sendBuffer[len++] = ip3;
	sendBuffer[len++] = ip4;
	sendBuffer[len++] = svPort & 0xFF;
	sendBuffer[len++] = svPort >> 8;	
	sendBuffer[len++] = clPort & 0xFF;
	sendBuffer[len++] = clPort >> 8;
	sendLen = len;
	
	// Wait while HUB sets-up connection
	timer = clock();
	while ((clock()-timer) < 2*CLK_TCK) {
		UpdateHub();
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
		RecvUDP(2*CLK_TCK);
	}
#endif
}

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

void SendUDP(unsigned char* buffer, unsigned char length) 
{
#ifndef __HUB__
	udp_send(buffer, length, udp_send_ip, udp_send_port, udp_recv_port);
#else
	unsigned char i, len = 0;
	sendBuffer[len++] = CMD_UDP_SEND;
	sendBuffer[len++] = length;
	for (i=0; i<length; i++) {
		sendBuffer[len++] = buffer[i];
	}
	sendLen = len;
#endif
}

unsigned int RecvUDP(unsigned int timeOut)
{	
#ifdef __HUB__
	// Check if data was received from Hub
	clock_t timer = clock();
	while (1) {
		// Check if we received packet
		if (recvLen) { 
			recvLen = 0;  // Clear packet
			return &recvBuffer[1]; 
		}		
		
		// Inquire next packet
		UpdateHub();
		
		// Check time-out
		if ((clock() - timer) > timeOut) { return 0; }
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
		if (clock() - timer > timeOut) { return 0; }	
	#if defined __APPLE2__
		tick();
	#endif
	}
#endif
}

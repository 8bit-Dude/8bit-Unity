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

unsigned long udp_send_ip;
unsigned int udp_send_port, udp_recv_port;

// IP65 functions (see linked libraries)
unsigned char udp_send_buf[32];   // Buffer with data to send (can be any length you wish...)
#ifdef __ATMOS__
  unsigned char udp_recv_buf[192];   // Buffer with data received (length hard-coded in IP65)
#else
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
#ifndef __ATMOS__
	// Init IP65 and DHCP
	if (ip65_init()) { return ADAP_ERR; }
	if (dhcp_init()) { return DHCP_ERR; }
#endif
	return NETW_OK;
}

void SendUDPPacket(unsigned char length) 
{
#ifndef __ATMOS__
	udp_send(udp_send_buf, length, udp_send_ip, udp_send_port, udp_recv_port);
#endif
}

void ReceiveUDP(void)
{
	// Placeholder for IP65 packet processing
}

void RecvUDPPacket(unsigned char timeOut)
{
	unsigned int timer;
	
	// Try to process UDP
	udp_recv_buf[0] = 0;
#ifndef __ATMOS__
	ip65_process();
#endif
	if (!timeOut) { return; }
	
	// Wait for an answer (within time-out)
	timer = clock();
	while (!udp_recv_buf[0]) { 
		if (clock() - timer > timeOut) { return; }
#ifndef __ATMOS__
		ip65_process();
#endif
#if defined __APPLE2__
		tick();
#endif			
	}
}

void ListenUDP(unsigned int port)
{
	udp_recv_port = port;
#ifndef __ATMOS__
	if (!udp_add_listener(port, ReceiveUDP)) {
		// Send first packet, as it is always lost!
		udp_send_buf[0] = 0;
		SendUDPPacket(1);
		RecvUDPPacket(0);
	}
#endif
}

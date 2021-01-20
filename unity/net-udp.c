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
  #include "hub.h"
#elif defined __FUJINET__	
  // Nothing
#else
  #include "IP65/ip65.h"
  #define EncodeIP(a,b,c,d) (a+b*256+c*65536+d*16777216)
  unsigned long udp_send_ip;
  unsigned int udp_send_port;
  unsigned int udp_recv_port;
  unsigned char *udp_buffer, udp_len;
  void PacketUDP(void) { 
	udp_buffer = udp_recv_buf; 
	udp_len = udp_recv_len;
  }  
#endif

void SlotUDP(unsigned char slot)
{
#if defined __HUB__
	QueueHub(HUB_UDP_SLOT, &slot, 1);
	UpdateHub();	
	
#elif defined __FUJINET__	
	// TODO
	
#else
	// TODO
#endif
}

void OpenUDP(unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4, unsigned int svPort, unsigned int clPort)
{
#if defined __HUB__
	// Ask HUB to set up connection
	unsigned char buffer[6];
	buffer[0] = ip1;
	buffer[1] = ip2;
	buffer[2] = ip3;
	buffer[3] = ip4;
	buffer[4] = svPort & 0xFF;
	buffer[5] = svPort >> 8;	
	buffer[6] = clPort & 0xFF;
	buffer[7] = clPort >> 8;	
	QueueHub(HUB_UDP_OPEN, buffer, 8);
	UpdateHub();
	
#elif defined __FUJINET__	
	// Open UDP address
	sprintf(fujiBuffer, "N:UDP://%i.%i.%i.%i:%i/", ip1, ip2, ip3, ip4, svPort);
	FujiOpen(0);
  
#else
	// Set-up UDP params and listener
	unsigned char dummy[1];
	udp_send_ip = EncodeIP(ip1,ip2,ip3,ip4);
	udp_send_port = svPort;
	udp_recv_port = clPort;
	if (!udp_add_listener(clPort, PacketUDP)) {
		// Send dummy packet, as first one is always lost!
		dummy[0] = 0;
		SendUDP(dummy, 1);
		RecvUDP(TCK_PER_SEC);
	}
#endif
}

void CloseUDP()
{
#if defined __HUB__
	QueueHub(HUB_UDP_CLOSE, 0, 0);
	UpdateHub();	
	
#elif defined __FUJINET__	
	FujiClose();
	
#else
	udp_remove_listener(udp_recv_port);
#endif
}

void SendUDP(unsigned char* buffer, unsigned char length) 
{
#if defined __HUB__
	QueueHub(HUB_UDP_SEND, buffer, length);
	
#elif defined __FUJINET__	
	memcpy(fujiBuffer, buffer, length);
	FujiWrite(length);
	
#else
	udp_send(buffer, length, udp_send_ip, udp_send_port, udp_recv_port);
#endif
}

unsigned char* RecvUDP(unsigned int timeOut)
{	
	clock_t timer = clock()+timeOut;
#if defined __HUB__
	// Wait until data is received from Hub
	while (!recvLen || recvHub[0] != HUB_UDP_RECV) {
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
	FujiRead();	// Get data
	return fujiBuffer;
	
#else
	// Process IP65 until receiving packet
	while (!udp_len) {
		if (clock() > timer) return 0;
		ip65_process();
	#if defined __APPLE2__
		wait(1);
	#endif
	}
	udp_len = 0;
	return udp_buffer;	
#endif
}

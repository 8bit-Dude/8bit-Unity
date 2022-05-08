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

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

#if defined __HUB__
  // Nothing
#elif defined __FUJINET__	
  // Nothing
#elif defined __ULTIMATE__
  unsigned char udp_socket;  
#elif defined __IP65__
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
	SendHub(HUB_UDP_SLOT, &slot, 1);
#elif defined __FUJINET__	
	// TODO
#elif defined __ULTIMATE__
	// TODO	
#elif defined __IP65__
	// TODO
#endif
}

void OpenUDP(unsigned char* svIP, unsigned int svPort, unsigned int clPort)
{
#if defined __HUB__
	// Ask HUB to set up connection
	unsigned char buffer[8];
	memcpy(buffer, svIP, 4);
	POKEW(&buffer[4], svPort);	
	POKEW(&buffer[6], clPort);	
	SendHub(HUB_UDP_OPEN, buffer, 8);
	
#elif defined __FUJINET__	
	// Open UDP address
	unsigned char dummy[1];
	sprintf(fujiHost, "N:UDP://%i.%i.%i.%i:%i/", svIP[0], svIP[1], svIP[2], svIP[3], svPort);
	FujiOpen(0x71, 0);
	
	// Send dummy packet, as first one is always lost!
	dummy[0] = 0;
	SendUDP(dummy, 1);
	RecvUDP(TCK_PER_SEC);	

#elif defined __ULTIMATE__
	unsigned char host[17];
	sprintf(host, "%i.%i.%i.%i", svIP[0], svIP[1], svIP[2], svIP[3]);
	udp_socket = uii_socketopen(host, svPort, NET_CMD_UDP_SOCKET_CONNECT);
	  
#elif defined __IP65__
	// Set-up UDP params and listener
	unsigned char dummy[1];
	udp_send_ip = EncodeIP(svIP[0],svIP[1],svIP[2],svIP[3]);
	udp_send_port = svPort;
	udp_recv_port = clPort;
	if (!udp_add_listener(clPort, PacketUDP)) {
		// Send dummy packet, as first one is always lost!
		dummy[0] = 0;
		SendUDP(dummy, 1);
		RecvUDP(TCK_PER_SEC);
	}
	udp_len = 0;
#endif
}

void CloseUDP()
{
#if defined __HUB__
	SendHub(HUB_UDP_CLOSE, 0, 0);
	
#elif defined __FUJINET__	
	FujiClose(0x71);
	
#elif defined __ULTIMATE__
	uii_socketclose(udp_socket);	
	
#elif defined __IP65__
	udp_remove_listener(udp_recv_port);
#endif
}

void SendUDP(unsigned char* buffer, unsigned char length) 
{
#if defined __HUB__
	SendHub(HUB_UDP_SEND, buffer, length);
	
#elif defined __FUJINET__	
	memcpy(fujiBuffer, buffer, length);
	FujiWrite(0x71, length);
	
#elif defined __ULTIMATE__
	uii_socketwrite(udp_socket, buffer, length);	
	
#elif defined __IP65__
	udp_send(buffer, length, udp_send_ip, udp_send_port, udp_recv_port);
#endif
}

unsigned char* RecvUDP(unsigned int timeOut)
{	
	// Keep trying until timeout expires...
	clock_t timer = clock()+timeOut;
#if defined __HUB__	
	while (!RecvHub(HUB_UDP_RECV)) {
		if (clock() >= timer) return 0;
	#if defined __APPLE2__
		clk += 6; 
	#endif		
	}	
	return hubBuf; 
	
#elif defined __FUJINET__	
	while (!fujiReady) {
		if (clock() >= timer) return 0;
	}
	if (FujiRead(0x71)) {
		return fujiBuffer;
	} else {
		return 0;
	}

#elif defined __ULTIMATE__
	while (uii_socketread(udp_socket, 255) < 1) {
		if (clock() >= timer) return 0;
	}
	return &uii_data[2];
	
#elif defined __IP65__
	if (!udp_len) {		
		while (1) {
			ip65_process();
			if (udp_len) break;
			if (clock() >= timer) return 0;
		#if defined __APPLE2__
			wait(1); 
		#endif
		}
	}
	udp_len = 0;
	return udp_buffer;		
#endif
}

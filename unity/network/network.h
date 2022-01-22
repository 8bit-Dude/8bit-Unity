/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2021/05/13
 *	
 * Copyright (c) 2021 Anthony Beaucamp
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
 *
 *	Credits: 
 *		* Oliver Schmidt for his IP65 network interface
 *		* Christian Groessler for helping optimize the memory maps on Commodore and Atari
 *		* Bill Buckels for his Apple II Double Hi-Res bitmap code
 *		* Karri Kaksonen for his many contributions on the Lynx
 */

// CC65 includes
#include <string.h>
#include <conio.h>
#include <dirent.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Platform IDs
#if defined __CBM__
	#define PLATFORM 0	
  #define TCK_PER_SEC	CLK_TCK	
#elif defined __ATARI__
	#define PLATFORM 1
  #define TCK_PER_SEC	CLK_TCK	
#elif defined __APPLE2__
  // Workaround for missing clock (see CLOCK.c)
  #define TCK_PER_SEC	64u
  void wait(unsigned char ticks);
  clock_t clock(void);
  unsigned sleep(unsigned seconds);
  extern clock_t clk;			
#elif defined __ATMOS__
	#define PLATFORM 3
  #define TCK_PER_SEC	CLK_TCK	
#elif defined __LYNX__
  #include <lynx.h>
	#define PLATFORM 4
  // Clock
  #define TCK_PER_SEC	60	
#elif defined __NES__
	#define PLATFORM 5
    #include "targets/nes/platform.h"	
#endif

// Adaptors
#if defined __HUB__
  #include "../adaptors/hub.h"  
  #define HUB_TX_LEN 256
  #define HUB_RX_LEN 256
#elif defined __FUJINET__	
  // Nothing
#else
  #include "../adaptors/ip65.h"
#endif

// Network functions (see net-base.c, net-ip.c, net-easy.c, net-udp.c, net-tcp.c, net-url.c, net-web.c)
#define NETWORK_OK  0
#define ADAPTOR_ERR 1
#define DHCP_ERR    2
unsigned char InitNetwork(void);							// Initialize network adapter
unsigned char GetLocalIP(unsigned char* ip);				// Fetch local IP

// Easynet protocols
#define EASY_TCP     1
#define EASY_UDP     2

// Easynet return states
#define EASY_OK      0
#define EASY_CRED    1
#define EASY_FULL    2
#define EASY_TIMEOUT 3
#define EASY_BUFFER 256

// Easynet variables and functions
extern unsigned char easyProt, easyLive, easySlot;
unsigned char EasyHost(unsigned char protocol, unsigned char slots, unsigned int *ID, unsigned int *PASS); 	// protocol: EASY_TCP or EASY_UDP (ID and PASS generated automatically)
unsigned char EasyJoin(unsigned char protocol,  unsigned int *ID, unsigned int *PASS);						// 				   " "		      (ID and PASS must match host)		  	 
void EasySend(unsigned char *buffer, unsigned char len);
unsigned char *EasyRecv(unsigned char timeout, unsigned char* len);
void EasyQuit(void);

// TCP functions
void SlotTCP(unsigned char slot);							// Set TCP slot (0~15)
void OpenTCP(unsigned char *ip, unsigned int svPort);		// Open connection on current TCP slot (local port allocated automatically)
void CloseTCP(void);										// Close current TCP slot
void SendTCP(unsigned char* buffer, unsigned char length);  // Send contents of buffer on current TCP slot
unsigned char* RecvTCP(unsigned int timeOut);				// Check all slots for incoming TCP packet (within time-out period)

// UDP functions
void SlotUDP(unsigned char slot);							// Set UDP slot (0~15)
void OpenUDP(unsigned char *ip, unsigned int svPort,     	// Open connection on current UDP slot (local port allocated on clPort)
								unsigned int clPort);
void CloseUDP(void);										// Close current UDP slot
void SendUDP(unsigned char* buffer, unsigned char length);  // Send contents of buffer on current UDP slot
unsigned char* RecvUDP(unsigned int timeOut);				// Check all slots for incoming UDP packet (within time-out period)
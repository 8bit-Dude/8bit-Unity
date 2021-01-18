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

#if defined __HUB__
  #include "hub.h"
#elif defined __FUJINET__	
  // Nothing
#else
  #include "IP65/ip65.h"
#endif

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

unsigned char InitNetwork(void)
{
#if defined __HUB__
	// Detect if HUB is connected
	clock_t timer = clock();
	while ((clock()-timer) < TCK_PER_SEC) { 
		if (hubState[0] == COM_ERR_OK)
			return NETWORK_OK;
		UpdateHub();
	}
	return ADAPTOR_ERR;

#elif defined __FUJINET__
	// Turn off SIO clicks
	enable_rom();
	OS.soundr = 0;
	restore_rom();
	return NETWORK_OK;
	
#else
	// Init IP65 and DHCP
	if (ip65_init(ETH_INIT_DEFAULT)) return ADAPTOR_ERR;
	if (dhcp_init()) return DHCP_ERR;
	return NETWORK_OK;
#endif
}

unsigned char GetLocalIP(unsigned char* ip)
{
#if defined __HUB__
	// Check if data was received from Hub
	clock_t timer = clock();
	QueueHub(HUB_SYS_IP, 0, 0);
	while (1) {
		// Inquire next packet
		UpdateHub();	

		// Check if we received packet
		if (recvLen && recvHub[0] == HUB_SYS_IP) { 
			recvLen = 0;  // Clear packet
			memcpy(ip, &recvHub[2], recvHub[1]);
			return 1; 
		}		
		
		// Check time-out
		if ((clock() - timer) >= 20) { return 0; }	
	}
	
#elif defined __FUJINET__	
	// TODO	
	
#else
	// TODO	
#endif
}

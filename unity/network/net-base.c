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

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

unsigned char InitNetwork(void)
{
#if defined __HUB__
	// Detect if HUB is connected
	clock_t timer = clock();
	while ((clock()-timer) < TCK_PER_SEC) { 
		if (hubState[0] == COM_ERR_OK) return NETWORK_OK;
		UpdateHub();
	#if defined __APPLE2__
		clk += 1;
	#endif		
	}
	return ADAPTOR_ERR;

#elif defined __FUJINET__
	// Initialize Fujinet
	FujiInit();
	return NETWORK_OK;
	
#else
	// Init IP65 and DHCP
	if (ip65_init(ETH_INIT_DEFAULT)) return ADAPTOR_ERR;
	if (dhcp_init()) return DHCP_ERR;
	return NETWORK_OK;
#endif
}

/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2019/05/05
 *	
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
 *
 */

#include "../unity.h"

extern unsigned char hubState[7];	// see hub.c
unsigned char joyState[5];

void InitJoy(void) { 
	// Do nothing
}

unsigned char GetJoy(unsigned char joy)
{
	unsigned char state;
	switch (joy) {
	case 0:
		// Get state from registry
		joyState[joy] = ~PEEK(0xfcb0);
		break;
	default:
		// Make sure to use RefreshHub() in main loop!
		if (hubState[0] < COMM_ERR_TRUNCAT) {
			joyState[joy] = hubState[joy];
		}
	}
	return joyState[joy];
}

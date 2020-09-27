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

#include "hub.h"
#include "unity.h"

unsigned char joyState[3];

unsigned char GetJoy(unsigned char joy)
{
	unsigned char reg;
	switch (joy) {
	case 0:
		// Get state from registry
		reg = PEEK(0xfcb0); 
		joyState[0] = 255;
		if (reg & 128) { joyState[0] &= ~JOY_UP; }
		if (reg & 64)  { joyState[0] &= ~JOY_DOWN; }
		if (reg & 32)  { joyState[0] &= ~JOY_LEFT; }
		if (reg & 16)  { joyState[0] &= ~JOY_RIGHT; }
		if (reg & 2)   { joyState[0] &= ~JOY_BTN2; }
		if (reg & 1)   { joyState[0] &= ~JOY_BTN1; }		
		break;
		
	default:
		// Get state from HUB
		UpdateHub();
		if (hubState[0] < COM_ERR_TRUNCAT)
			joyState[joy] = hubState[joy];
		break;
	}
	return joyState[joy];
}

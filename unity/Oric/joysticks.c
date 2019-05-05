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

#include "unity.h"

unsigned char GetJoy(unsigned char joy)
{
	unsigned char state = 255;
	if (joy) {
		if (GetKey(3+8*5)) { state -= JOY_UP;    } // P
		if (GetKey(2+8*3)) { state -= JOY_DOWN;  } // ;
		if (GetKey(1+8*7)) { state -= JOY_LEFT;  } // L
		if (GetKey(7+8*3)) { state -= JOY_RIGHT; } // '
		if (GetKey(5+8*7)) { state -= JOY_FIRE;  } // RETURN
	} else {
		if (GetKey(7+8*6)) { state -= JOY_UP;    } // W
		if (GetKey(6+8*6)) { state -= JOY_DOWN;  } // S
		if (GetKey(5+8*6)) { state -= JOY_LEFT;  } // A
		if (GetKey(7+8*1)) { state -= JOY_RIGHT; } // D
		if (GetKey(4+8*2)) { state -= JOY_FIRE;  } // CTRL
	}
	return state;
}

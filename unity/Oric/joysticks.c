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

#include "hub.h"
#include "unity.h"

#define ADAPTOR_PASE 0
#define ADAPTOR_HUB  1

unsigned char InitPaseIJK(void);
unsigned char GetPaseIJK(unsigned char);	// see JOY.s
unsigned char GetKey(unsigned char);		// see keyboard.s

unsigned char joyAdaptor = ADAPTOR_PASE;

void InitJoy(void)
{
	// Check if 8bit-Hub connected (otherwise assume PASE/IJK interface)
	if (hubState[0] == COM_ERR_OFFLINE) { UpdateHub(); }
	if (hubState[0] != COM_ERR_OFFLINE) { joyAdaptor = ADAPTOR_HUB; }
}

unsigned char GetJoy(unsigned char joy)
{
	unsigned char state;
	switch (joy) {
	case 0:
		state = 255;				// Joy #0:  Keyboard
		if (GetKey(7+8*6)) { state -= JOY_UP;    } // W
		if (GetKey(6+8*6)) { state -= JOY_DOWN;  } // S
		if (GetKey(5+8*6)) { state -= JOY_LEFT;  } // A
		if (GetKey(7+8*1)) { state -= JOY_RIGHT; } // D
		if (GetKey(4+8*2)) { state -= JOY_BTN1;  } // CTRL
		return state;
	case 1:
		state = 255;				// Joy #1:  Keyboard
		if (GetKey(1+8*5)) { state -= JOY_UP;    } // I
		if (GetKey(0+8*3)) { state -= JOY_DOWN;  } // K
		if (GetKey(0+8*1)) { state -= JOY_LEFT;  } // J
		if (GetKey(1+8*7)) { state -= JOY_RIGHT; } // L
		if (GetKey(5+8*7)) { state -= JOY_BTN1;  } // RETURN
		return state;
	default:
		if (joyAdaptor) {			// Joy #3-#4: 8bit-Hub
			UpdateHub();			
			return hubState[joy-1];
		} else {					// Joy #3-#4: ALTAI/PASE/IJK
			state = GetPaseIJK(joy-2);
			return state;
		}
	}
}

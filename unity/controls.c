/*
 * Copyright (c) 2020 Anthony Beaucamp.
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

#if (defined __LYNX__) || (defined __ORIC__)
  #include "hub.h"
#endif

///////////////// MOUSE ////////////////////

unsigned char mouseState[3] = {80, 100, 255};

unsigned char* GetMouse(void) 
{
#if (defined __LYNX__) || (defined __ORIC__)
	UpdateHub();
	mouseState[0] = hubState[5];
	mouseState[1] = hubState[6];
	mouseState[2] = 255;
	if (!(hubState[1] & 64))  mouseState[2] &= ~MOU_LEFT;
	if (!(hubState[1] & 128)) mouseState[2] &= ~MOU_RIGHT;
	if (!(hubState[2] & 64))  mouseState[2] &= ~MOU_MIDDLE;
#endif	
	return mouseState;
}

#ifndef __CBM__
// see C64/JOY.s

#ifndef __APPLE2__
// see Apple/JOY.s

/////////////// JOYTICKS ///////////////////

#if (defined __ORIC__)
  #define ADAPTOR_PASE 0
  #define ADAPTOR_HUB  1
  unsigned char joyAdaptor = ADAPTOR_PASE;
  unsigned char InitPaseIJK(void);			// see Oric/JOY.s
  unsigned char GetPaseIJK(unsigned char);	// see Oric/JOY.s
  unsigned char GetKey(unsigned char);		// see Oric/keyboard.s
#endif

void InitJoy(void)
{
#if defined __ORIC__
	// Check if 8bit-Hub connected (otherwise assume PASE/IJK interface)
	if (hubState[0] == COM_ERR_OFFLINE) UpdateHub();
	if (hubState[0] != COM_ERR_OFFLINE) joyAdaptor = ADAPTOR_HUB;
#else
	return;
#endif
}

unsigned char GetJoy(unsigned char joy)
{
#if defined __ATARI__
	return PEEK(0x0278+joy)+(PEEK(0x0284+joy)<<4);
#elif defined __LYNX__
	// 2 input types: D-Pad (#0) or 8bit-Hub (#1,#2,#3)
	unsigned char reg, state;
	switch (joy) {
	case 0:
		// Get state from registry
		reg = PEEK(0xfcb0); 
		state = 255;
		if (reg & 128) { state &= ~JOY_UP; }
		if (reg & 64)  { state &= ~JOY_DOWN; }
		if (reg & 32)  { state &= ~JOY_LEFT; }
		if (reg & 16)  { state &= ~JOY_RIGHT; }
		if (reg & 2)   { state &= ~JOY_BTN2; }
		if (reg & 1)   { state &= ~JOY_BTN1; }		
		break;
		
	default:
		// Get state from HUB
		UpdateHub();
		state = hubState[joy];
		break;
	}
	return state;
	
#elif defined __ORIC__
	// 3 input types: Keyboard (#0,#1) 8bit-Hub (#2,#3,#4) or PASE (#2,#3)
	unsigned char state;
	switch (joy) {
	case 0:
		state = 255;				// Joy #0:  Keyboard
		if (GetKey(7+8*6)) { state -= JOY_UP;    } // W
		if (GetKey(6+8*6)) { state -= JOY_DOWN;  } // S
		if (GetKey(5+8*6)) { state -= JOY_LEFT;  } // A
		if (GetKey(7+8*1)) { state -= JOY_RIGHT; } // D
		if (GetKey(4+8*2)) { state -= JOY_BTN1;  } // CTRL
		break;
		
	case 1:
		state = 255;				// Joy #1:  Keyboard
		if (GetKey(1+8*5)) { state -= JOY_UP;    } // I
		if (GetKey(0+8*3)) { state -= JOY_DOWN;  } // K
		if (GetKey(0+8*1)) { state -= JOY_LEFT;  } // J
		if (GetKey(1+8*7)) { state -= JOY_RIGHT; } // L
		if (GetKey(5+8*7)) { state -= JOY_BTN1;  } // RETURN
		break;
		
	default:
		if (joyAdaptor) {			// Joy #2-#4: 8bit-Hub
			UpdateHub();			
			state = hubState[joy-1];
		} else {					// Joy #2-#3: ALTAI/PASE/IJK
			state = GetPaseIJK(joy-2);
		}
	}
	return state;
#endif
}

/////////////////////////////////////////////

#endif
#endif

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

#ifdef __HUB__
  #include "hub.h"
#endif

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#define MOU_XMAX 160
#define MOU_YMAX 200

unsigned char mouseState[3] = {80, 100, 255};
clock_t mouseClock;

unsigned char* GetMouse(void) 
{
    unsigned char step, joy;
	mouseState[2] = 255;
#if (defined __LYNX__) || (defined __ORIC__)
	// Get mouse state from Hub
	UpdateHub();
	if (hubState[5] != 255) {
		if (hubState[5] < 127) mouseState[0] = MIN(MOU_XMAX, mouseState[0]+hubState[5]); 
		                  else mouseState[0] -= MIN(mouseState[0], ~hubState[5]);
		if (hubState[6] < 127) mouseState[1] = MIN(MOU_YMAX, mouseState[1]+hubState[6]); 
		                  else mouseState[1] -= MIN(mouseState[1], ~hubState[6]);
		if (hubState[5] || hubState[6]) mouseState[2] &= ~MOU_MOTION;
		if (!(hubState[1] & 64))  mouseState[2] &= ~MOU_LEFT;
		if (!(hubState[1] & 128)) mouseState[2] &= ~MOU_RIGHT;
		if (!(hubState[2] & 64))  mouseState[2] &= ~MOU_MIDDLE;
		if (!(hubState[3] & 64))  mouseState[2] &= ~MOU_UP;
		if (!(hubState[3] & 128)) mouseState[2] &= ~MOU_DOWN;
	} else {
#endif
	// Read mouse state from joystick #1
	step = clock()-mouseClock;
	if (step > 30) step = 0;
	mouseClock = clock();
	joy = GetJoy(0);
	if (!(joy & JOY_LEFT))  mouseState[0] -= MIN(mouseState[0], step);
	if (!(joy & JOY_RIGHT)) mouseState[0] = MIN(MOU_XMAX, mouseState[0]+step); 
	if (!(joy & JOY_UP))    mouseState[1] -= MIN(mouseState[1], step);
	if (!(joy & JOY_DOWN))  mouseState[1] = MIN(MOU_YMAX, mouseState[1]+step); 
	if (!(joy & JOY_BTN1))  mouseState[2] &= ~MOU_LEFT;  
	if (!(joy & JOY_BTN2))  mouseState[2] &= ~MOU_RIGHT;  
#if (defined __LYNX__) || (defined __ORIC__)
	}
#endif
	return mouseState;
}

/*
 * Copyright (c) 2018 Anthony Beaucamp.
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
 
//	Workaround for missing Apple II clock
//	Last modified: 2018/01/29

#include "unity.h"

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

clock_t clk;

// Wait for X clock ticks
void wait(unsigned char ticks)
{
	unsigned int i = 0;
	while (i<220*ticks) { i++; }
	clk += 1;		
}

// Sleep for x seconds
unsigned sleep(unsigned seconds)
{
	unsigned int i;
	while (seconds) {
		i = 0;
		while (i<13200) { i++; }
		clk += TCK_PER_SEC;
		seconds--;
	}
	return 1;
}

// Return clock state
clock_t clock()
{
	return clk;
}

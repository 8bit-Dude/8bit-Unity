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
 
#include <peekpoke.h>
#include "platform.h"

int vicconf[3];

void SwitchBank(char bank) 
{
	// notice that 0 selects vic bank 3, 1 selects vic bank 2, etc.
	POKE(0xDD00, (PEEK(0xDD00) & 252 | (3 - bank))); // switch VIC base
}

void SetupVIC2()
{
	// Backup VIC config
	vicconf[0] = PEEK(53272);
	vicconf[1] = PEEK(53265);
	vicconf[2] = PEEK(53270);	
	
	// Set data direction flag and Switch bank
	POKE(0xDD02, (PEEK(0xDD02) | 3));
	SwitchBank(VIDEOBANK);	
	
	// Enter multicolor mode
	POKE(0xD018, SCREENLOC*16 + BITMAPLOC);	// 53272: address of screen and bitmap RAM
	POKE(0xD016, PEEK(0xD016) | 0x10);		// 53270: set multicolor mode	
}

void ResetVIC2()
{
	// Return VIC and Bank back to previous state
	SwitchBank(0);	
	POKE(53272, vicconf[0]);
	POKE(53265, vicconf[1]);
	POKE(53270, vicconf[2]);
}

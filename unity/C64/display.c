/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2020/05/13
 *	
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
 *
 *	Credits: 
 *		* Oliver Schmidt for his IP65 network interface
 *		* Christian Groessler for helping optimize the memory maps on Commodore and Atari
 *		* Bill Buckels for his Apple II Double Hi-Res bitmap code
 */

#include <peekpoke.h>
 
void DoubleHeightSprite(unsigned char index, unsigned char onoff)
{
	if (onoff) {
		POKE(0xD017, PEEK(0xD017) |  (1 << index));
	} else {
		POKE(0xD017, PEEK(0xD017) & !(1 << index));
	}
}

void DoubleWidthSprite(unsigned char index, unsigned char onoff)
{
	if (onoff) {
		POKE(0xD01D, PEEK(0xD01D) |  (1 << index));
	} else {
		POKE(0xD01D, PEEK(0xD01D) & !(1 << index));
	}
}

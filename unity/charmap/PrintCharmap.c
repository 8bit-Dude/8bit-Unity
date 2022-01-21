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

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

void PrintCharmap(unsigned char x, unsigned char y, unsigned char chr)
{
#if defined(__ATARI__)
	POKE((char*)SCREENRAM+y*40+x, chr+charatrData[chr]);
#elif defined(__CBM__)
	unsigned int addr = y*40+x;
	POKE((char*)SCREENRAM+addr, chr);
	POKE((char*)COLORRAM+addr, charatrData[chr]);
#elif defined(__LYNX__)
	unsigned char *src, *dst;
	unsigned char i;
	src = &charsetData[2*chr];
	dst = (char*)(BITMAPRAM+1 + y*ROW_SIZE + x*CHAR_WIDTH);
	for (i = 0; i<4; i++) {
		memcpy(dst, src, 2);
		src += 256; dst += 82;
	}
#endif
}

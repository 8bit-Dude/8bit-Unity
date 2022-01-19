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

#include "unity.h"

#ifdef __APPLE2__
  #pragma code-name("LOWCODE")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

// Copy string from one area of screen to another
void CopyStr(unsigned char col1, unsigned char row1, unsigned char col2, unsigned char row2, unsigned char len)
{
#if defined __CBM__
	// Copy bitmap and screen ram
	rom_disable();
	memcpy((char*)BITMAPRAM+row1*320+col1*8, (char*)BITMAPRAM+row2*320+col2*8, len*8);
	rom_restore();
	memcpy((char*)SCREENRAM+row1*40+col1, (char*)SCREENRAM+row2*40+col2, len);
#elif defined __ATARI__
	// Copy bitmap 1 and 2
	unsigned char i;
	unsigned int src, dst;
	src = row2*320+col2;
	dst = row1*320+col1;
	for (i=0; i<8; ++i) {
		memcpy((char*)BITMAPRAM1+dst, (char*)BITMAPRAM1+src, len);
	  #ifdef __ATARIXL__	
		memcpy((char*)BITMAPRAM2+dst, (char*)BITMAPRAM2+src, len);
	  #endif	
		src += 40; dst += 40;
	}
#elif defined __ORIC__
	// Copy bitmap RAM
	unsigned char i;
	unsigned int src, dst;
	src = BITMAPRAM+1+row2*320+col2;
	dst = BITMAPRAM+1+row1*320+col1;
	for (i=0; i<8; ++i) {
		memcpy((char*)dst, (char*)src, len);
		src += 40; dst += 40;
	}
#elif defined __APPLE2__
	// Always copy 7 pixels at a time!
	unsigned int src, dst;
	unsigned char i, x1, y1, x2, y2;
	col1 += col1&1; col2 += col2&1;	
	x1 = (col1*35)/10u; y1 = (row1*8);
	x2 = (col2*35)/10u; y2 = (row2*8);	
	for (i=0; i<8; ++i) {
		SetHiresPointer(x1, y1+i); dst = hiresPtr;
		SetHiresPointer(x2, y2+i); src = hiresPtr;
	  #if defined __DHR__	
		*dhraux = 0;
		memcpy((char*)dst, (char*)src, len);
		*dhrmain = 0;
	  #endif
		memcpy((char*)dst, (char*)src, len);
	}
#elif defined __LYNX__
	// Copy bitmap RAM
	unsigned char i=0;
	unsigned int src, dst;
	src = BITMAPRAM+1+row2*492+col2*2;
	dst = BITMAPRAM+1+row1*492+col1*2;
	len *= 2;
	while (i<6)	{
		memcpy((char*)dst, (char*)src, len);
		src += 82; dst += 82; i++;
	}
#elif defined __NES__
	// Copy data using chunks
	char *chunk = GetChunk(col2, row2, len, 1);
	SetChunk(chunk, col1, row1);
	FreeChunk(chunk);
#endif	
}

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

#ifdef __ATARI__
  extern unsigned char inkColor1,inkColor2;
  extern unsigned char paperColor1,paperColor2;
  extern unsigned char bgByte1,bgByte2;
#endif

#ifdef __CBM__
  extern unsigned char pow2;
#endif

// Rapidly fill memory area with blank characters
void PrintBlanks(unsigned char col, unsigned char row, unsigned char width, unsigned char height)
{
	// Black-out menu area
	unsigned char i=0;
#if defined __ATARI__
	unsigned int addr1, addr2;
	paperColor1 = paperColor&3;
	paperColor2 = paperColor/4u;
	addr1 = BITMAPRAM1+row*320+col;
	addr2 = BITMAPRAM2+row*320+col;
	bgByte1 = BYTE4(paperColor1,paperColor2,paperColor1,paperColor2);
	bgByte2 = BYTE4(paperColor2,paperColor1,paperColor2,paperColor1);
	while (i<height*8) {
		if (i&1) {
			memset((char*)addr1, bgByte2, width);
			if (bitmapVBI)
				memset((char*)addr2, bgByte1, width);
		} else {
			memset((char*)addr1, bgByte1, width);
			if (bitmapVBI)
				memset((char*)addr2, bgByte2, width);
		}
		addr1 += 40; addr2 += 40; ++i;
	}	
#elif defined __APPLE2__
	unsigned char x, y;
	unsigned int dataAux, dataMain;
	
	// Make sure columns start and end on full 7 pixel blocks
	col = (col-(col&1))/2u;
	width = (width+(width&1))/2u;
	
	// Create sample block at top-left (to encode color info)
	x = 7*col;
	y = row*8;
	for (i=0; i<7; ++i) {
		SetHiresPointer(x+i, y);
	  #if defined __DHR__
		SetColorDHR(paperColor);
	  #else
		SetColorSHR(paperColor);
	  #endif
	}
	
	// Copy sample block across the rest of DHR memory
	SetHiresPointer(x, y);
  #if defined __DHR__	
	*dhraux = 0;  dataAux = PEEKW(hiresPtr); *dhrmain = 0; 
  #endif
	dataMain = PEEKW(hiresPtr);
	for (y=row*8; y<(row+height)*8; ++y) {
		SetHiresPointer(x, y);
		for (i=0; i<width; ++i) {
		  #if defined __DHR__	
			*dhraux = 0;  POKEW(hiresPtr, dataAux); *dhrmain = 0; 
		  #endif
			POKEW(hiresPtr, dataMain);
			hiresPtr += 2;
		}
	}
  
#elif defined __ORIC__
	// Fill with 0s (papercolor) or 1s (inkcolor)
	unsigned int addr;
	unsigned char value;
	addr = BITMAPRAM+1+row*320+col;
	if (paperColor) value = 127; else value = 64;
	while (i<height*8) {
		memset((char*)addr, value, width);
		addr += 40; ++i;
	}
#elif defined __CBM__
	unsigned int addr1, addr2;
	addr1 = BITMAPRAM+row*8*40+col*8;
	addr2 = SCREENRAM+row*40+col;
	while (i<height) {
		memset((char*)addr1, pow2, width*8);
		memset((char*)addr2, paperColor, width);		
		addr1 += 320; addr2 += 40; i++;
	}	
#elif defined __LYNX__
	unsigned int addr;
	unsigned char value;
	addr = BITMAPRAM+1+row*(6*82)+col*2;
	value = (paperColor << 4) | paperColor;
	while (i<height*6) {
		memset((char*)addr, value, width*2);
		addr += 82; ++i;
	}
	if (autoRefresh) { UpdateDisplay(); }
#endif
}

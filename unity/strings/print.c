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

#include "../unity.h"

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

// Atari specific variables & functions
#ifdef __ATARI__
  unsigned char inkColor1,inkColor2;
  unsigned char paperColor1,paperColor2;
  unsigned char bgByte1,bgByte2;
#endif

#ifdef __CBM__
  unsigned char pow2 = (2 | 2<<2 | 2<<4 | 2<<6);
#endif

// Oric specific variables & functions
#ifdef __ORIC__
  // INK attributes for characters
  unsigned char ink1[20] = { 0, 3, 3, 6, 3, 7, 4, 4, 7, 4, 7, 2, 3, 1, 3, 7, 5, 5, 7, 5 };
  unsigned char ink2[20] = { 0, 2, 3, 6, 6, 6, 6, 4, 6, 6, 6, 1, 7, 1, 1, 7, 7, 6, 1, 5 };
  void SetAttributes(unsigned char color)
  {
	unsigned char i, line1, line2;
	unsigned int addr;
	addr = BITMAPRAM+1 + txtY*320 + txtX - 1;
	line1 = ink1[color];
	line2 = ink2[color];
	for (i=0; i<4; ++i) {
		POKE((char*)addr, line1); addr += 40;
		POKE((char*)addr, line2); addr += 40;
	}
  }
#endif

// Position and Colors used for printing
unsigned char txtX, txtY;
unsigned char inkColor = WHITE; 
unsigned char paperColor = BLACK;

// Print character using background and foreground colors
void PrintChr(char chr)
{
#if defined __NES__
	// Handle Lower/Upper Case
	if (chr > 96)
		chr += 128;
	else
		chr += 160;

	// Write attribute to VRAM buffer
	SetVramAttr();
	SetVramColor(1);
	
	// Write char to VRAM buffer
	SetVramName();
	SetVramChar(chr);
	
#else	
	// Declare variables
	char *src;
  #if defined __APPLE2__
	unsigned int x,y;
	unsigned char i,j,n,line;
  #elif defined __ATARI__	
	unsigned char i,line;
	unsigned int dst1,dst2;
  #elif defined __ORIC__
	unsigned char i, line;
	unsigned char a0,a2,a4,b,blank;
	unsigned int dst;
  #elif defined __CBM__
	unsigned int dst;
	unsigned char i,line;
  #elif defined __LYNX__
	unsigned char i, j, line, paperShift, inkShift;
	unsigned int dst;
  #endif
  
	// Handle Lower/Upper Case
  #if defined __CBM__
	if (chr > 192) { chr -= 128; }	// Upper case (C64)
	else
  #endif
    if (chr >  96) { chr -= 32;  }	// Lower case (Apple/Atari/Oric/Lynx) and compatibility with Ascii files (C64)
		
	// Get character data
	src = (char*)&charData[(chr-CHR_DATA_TOP)*3];
	
  #if defined __APPLE2__
	if (txtX&1) { n=4; } else { n=3; }
	x = (txtX*35)/10u; y = (txtY*8);
	SetHiresPointer(x, y++);	
	for (j=0; j<n; j++) {
	  #if defined __DHR__	
		SetColorDHR(paperColor);
	  #else
		SetColorSHR(paperColor);
	  #endif
		hiresPixel++;
	}
	for (i=0; i<3; ++i) {
		line = src[i];
		SetHiresPointer(x, y++);
		for (j=0; j<n; j++) {
		  #if defined __DHR__	
			SetColorDHR(((line>>(7-j))&1) ? inkColor : paperColor);
		  #else
			SetColorSHR(((line>>(7-j))&1) ? inkColor : paperColor);
		  #endif
			hiresPixel++;
		}
		SetHiresPointer(x, y++);
		for (j=0; j<n; j++) {
		  #if defined __DHR__	
			SetColorDHR(((line>>(3-j))&1) ? inkColor : paperColor);
		  #else
			SetColorSHR(((line>>(3-j))&1) ? inkColor : paperColor);
		  #endif
			hiresPixel++;
		}
	}
	SetHiresPointer(x, y);
	for (j=0; j<n; j++) {
	  #if defined __DHR__	
		SetColorDHR(paperColor);
	  #else
		SetColorSHR(paperColor);
	  #endif
		hiresPixel++;
	}

	// Update clock (slow function)
  #if defined __DHR__ 	
	clk += 3;
  #else
	clk += 3;
  #endif
	
  #elif defined __ATARI__	
  #ifndef __ATARIXL__	
	// Single buffer cannot do shaded colors
    i = inkColor;
	if (i==1 || i==4) i=5; if (i==2 || i==8) i=10; if (i==3 || i==12) i=15;
	inkColor1 = i&3; inkColor2 = i/4u;
  #else	
	inkColor1 = inkColor&3; inkColor2 = inkColor/4u;
  #endif  
	paperColor1 = paperColor&3; paperColor2 = paperColor/4u;
	bgByte1 = BYTE4(paperColor1,paperColor2,paperColor1,paperColor2);
	bgByte2 = BYTE4(paperColor2,paperColor1,paperColor2,paperColor1);	
	dst1 = dst2 = txtY*320 + txtX;
	dst1 += BITMAPRAM1; 
  #ifdef __ATARIXL__	
	dst2 += BITMAPRAM2;
  #endif
	if (chr == ' ') {
		for (i=0; i<8; ++i) {
			if (i&1) {
								  POKE((char*)dst1, bgByte2);
			  #ifdef __ATARIXL__	
				if (doubleBuffer) POKE((char*)dst2, bgByte1);
			  #endif	
			} else {
								  POKE((char*)dst1, bgByte1);
			  #ifdef __ATARIXL__	
				if (doubleBuffer) POKE((char*)dst2, bgByte2);
			  #endif	
			}
			dst1 += 40; 
		  #ifdef __ATARIXL__
		    dst2 += 40;
		  #endif	
		}
	} else {
						  POKE((char*)dst1, bgByte1); dst1 += 40;
	  #ifdef __ATARIXL__	
	    if (doubleBuffer) POKE((char*)dst2, bgByte2); dst2 += 40;
	  #endif
		for (i=0; i<3; ++i) {
				line = src[i];
				POKE((char*)dst1, BYTE4(((line&128) ? inkColor2 : paperColor2), ((line&64) ? inkColor1 : paperColor1), ((line&32) ? inkColor2 : paperColor2), paperColor1)); dst1 += 40;
				POKE((char*)dst1, BYTE4(((line&8  ) ? inkColor1 : paperColor1), ((line&4 ) ? inkColor2 : paperColor2), ((line&2 ) ? inkColor1 : paperColor1), paperColor2)); dst1 += 40;
		  #ifdef __ATARIXL__	
			if (doubleBuffer) {
				POKE((char*)dst2, BYTE4(((line&128) ? inkColor1 : paperColor1), ((line&64) ? inkColor2 : paperColor2), ((line&32) ? inkColor1 : paperColor1), paperColor2)); dst2 += 40;
				POKE((char*)dst2, BYTE4(((line&8  ) ? inkColor2 : paperColor2), ((line&4 ) ? inkColor1 : paperColor1), ((line&2 ) ? inkColor2 : paperColor2), paperColor1)); dst2 += 40;
			}
		  #endif
		}
						  POKE((char*)dst1, bgByte2);
	  #ifdef __ATARIXL__	
		if (doubleBuffer) POKE((char*)dst2, bgByte1);
	  #endif
	}
	
  #elif defined __ORIC__
	dst = BITMAPRAM+1 + txtY*320 + txtX;
	blank = 64 + (paperColor ? 63 : 0);
	if (chr == ' ') {
		for (i=0; i<8; ++i) {
			POKE((char*)dst, blank);
			dst += 40;
		}
	} else {
		if (paperColor != 0) {
			a0 = 2; a2 = 0; a4 = 1; b = 3;
		} else {
			a0 = 1; a2 = 3; a4 = 2; b = 0;
		}
		POKE((char*)dst, blank); dst += 40;
		for (i=0; i<3; ++i) {
			line = src[i];
			POKE((char*)dst, BYTE4(1, ((line&128) ? a0 : b), ((line&64) ? a2 : b), ((line&32) ? a4 : b))); dst += 40;
			POKE((char*)dst, BYTE4(1, ((line&8  ) ? a0 : b), ((line&4 ) ? a2 : b), ((line&2 ) ? a4 : b))); dst += 40;
		}
		POKE((char*)dst, blank);
	}
	
  #elif defined __CBM__
	dst = BITMAPRAM + txtY*320 + txtX*8;
	if (chr == ' ') {
		memset((char*)dst, pow2, 8);
	} else {
		POKE((char*)dst++, pow2);
		for (i=0; i<3; ++i) {
			line = src[i];
			POKE((char*)dst++, BYTE4(((line&128) ? 1 : 2), ((line&64) ? 1 : 2), ((line&32) ? 1 : 2), 2));
			POKE((char*)dst++, BYTE4(((line&8  ) ? 1 : 2), ((line&4 ) ? 1 : 2), ((line&2 ) ? 1 : 2), 2));
		}
		POKE((char*)dst, pow2);
	}
	dst = SCREENRAM + txtY*40 + txtX;
	POKE((char*)dst, inkColor << 4 | paperColor);		
		
  #elif defined __LYNX__
	dst = BITMAPRAM+1 + txtY*492 + txtX*2;
	paperShift = paperColor << 4;
	inkShift = inkColor << 4;
	POKE((char*)dst++, paperShift | paperColor);
	POKE((char*)dst,   paperShift | paperColor);
	dst += 81;
	for (i=0; i<3; ++i) {
		line = src[i];
		for (j=0; j<2; ++j) {
			if (i!= 1 || j != 0) {
			  POKE((char*)dst++, ((line&128) ? inkShift : paperShift) | ((line&64) ? inkColor : paperColor));
			  POKE((char*)dst,   ((line&32)  ? inkShift : paperShift) | paperColor);  
			  dst += 81;
			}
			line <<= 4;
		}
	}		
  #endif
#endif
}

// Parse string and print characters one-by-one (can be slow...)
void PrintStr(const char *buffer)
{
#if defined __NES__
	unsigned char chr, *src;
	
	// Write chars to VRAM buffer
	src = (unsigned char*)buffer;
	SetVramName();
	while (*src) {
		// Handle Lower/Upper Case
		chr = *src++;
		if (chr > 96)
			chr += 128;
		else
			chr += 160;
		SetVramChar(chr);
	}

	// Write attributes to VRAM buffer
	src = (unsigned char*)buffer;
	SetVramAttr();
	while (*src) {
		src++;
		SetVramColor(!*src);
	}
	
	// Display immediately?
	if (autoRefresh) { UpdateDisplay(); }
	
#else	
	char *src = (char*)buffer;
	unsigned char bckX = txtX;
	while (*src) {
		PrintChr(*src);
		++src; ++txtX;
	}
	txtX = bckX;
  #if defined __LYNX__
	// Display immediately?
	if (autoRefresh) { UpdateDisplay(); }
  #endif
#endif  
}

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

// Atari specific variables & functions
#ifdef __ATARI__
  unsigned char inkColor1,inkColor2;
  unsigned char paperColor1,paperColor2;
  unsigned char bgByte1,bgByte2;
#endif

#ifdef __CBM__
  unsigned char videoMode = TXT_MODE;
  unsigned char pow2 = (2 | 2<<2 | 2<<4 | 2<<6);
#endif

// Oric specific variables & functions
#if defined __ORIC__
  // INK attributes for characters
  unsigned char ink1[20] = { 0, 3, 3, 6, 3, 7, 4, 4, 7, 4, 7, 2, 3, 1, 3, 7, 5, 5, 7, 5 };
  unsigned char ink2[20] = { 0, 2, 3, 6, 6, 6, 6, 4, 6, 6, 6, 1, 7, 1, 1, 7, 7, 6, 1, 5 };
  void SetAttributes(signed char col, unsigned char row, unsigned char color)
  {
	unsigned char i, line1, line2;
	unsigned int addr;
	addr = BITMAPRAM+1 + row*320 + col;
	line1 = ink1[color];
	line2 = ink2[color];
	for (i=0; i<4; ++i) {
		POKE((char*)addr, line1); addr += 40;
		POKE((char*)addr, line2); addr += 40;
	}
  }
#endif

// Colors used in printing function
unsigned char inkColor = WHITE, paperColor = BLACK;

// Print character using background and foreground colors
void PrintChr(unsigned char col, unsigned char row, const char *chr)
{
#if defined __APPLE2__
	// Set Character over 3/4 pixels out of 7 in a cell
	unsigned int x,y;
	unsigned char i,j,n;
	if (col&1) { n=4; } else { n=3; }
	x = (col*35)/10u; y = (row*8);
	SetHiresPointer(x, y);	
	for (j=0; j<n; j++) {
	  #if defined __DHR__	
		SetColorDHR(paperColor);
	  #else
		SetColorSHR(paperColor);
	  #endif
		hiresPixel++;
	}
	for (i=0; i<3; ++i) {
		SetHiresPointer(x, y+i*2+1);
		for (j=0; j<n; j++) {
		  #if defined __DHR__	
			SetColorDHR(((chr[i]>>(7-j))&1) ? inkColor : paperColor);
		  #else
			SetColorSHR(((chr[i]>>(7-j))&1) ? inkColor : paperColor);
		  #endif
			hiresPixel++;
		}
		SetHiresPointer(x, y+i*2+2);
		for (j=0; j<n; j++) {
		  #if defined __DHR__	
			SetColorDHR(((chr[i]>>(3-j))&1) ? inkColor : paperColor);
		  #else
			SetColorSHR(((chr[i]>>(3-j))&1) ? inkColor : paperColor);
		  #endif
			hiresPixel++;
		}
	}
	SetHiresPointer(x, y+7);
	for (j=0; j<n; j++) {
	  #if defined __DHR__	
		SetColorDHR(paperColor);
	  #else
		SetColorSHR(paperColor);
	  #endif
		hiresPixel++;
	}

	// Update clock (slow function)
	clk += 1;
	
#elif defined __ATARI__	
	// Set Character across double buffer
	unsigned char i,line;
	unsigned int addr1,addr2;
	inkColor1 = inkColor&3; inkColor2 = inkColor/4u;
	paperColor1 = paperColor&3; paperColor2 = paperColor/4u;
	bgByte1 = BYTE4(paperColor1,paperColor2,paperColor1,paperColor2);
	bgByte2 = BYTE4(paperColor2,paperColor1,paperColor2,paperColor1);	
	addr1 = addr2 = row*320 + col;
	addr1 += BITMAPRAM1; addr2 += BITMAPRAM2;
	if (chr == &charBlank[0]) {
		for (i=0; i<8; ++i) {
			if (i&1) {
				POKE((char*)addr1, bgByte2);
				if (bitmapVBI)
					POKE((char*)addr2, bgByte1);
			} else {
				POKE((char*)addr1, bgByte1);
				if (bitmapVBI)
					POKE((char*)addr2, bgByte2);
			}
			addr1 +=40; addr2 +=40;
		}
	} else {
		POKE((char*)addr1, bgByte1); addr1 +=40;
		POKE((char*)addr2, bgByte2); addr2 +=40;
		for (i=0; i<3; ++i) {
			line = chr[i];
			POKE((char*)addr1, BYTE4(((line&128) ? inkColor2 : paperColor2), ((line&64) ? inkColor1 : paperColor1), ((line&32) ? inkColor2 : paperColor2), paperColor1)); addr1 +=40;
			POKE((char*)addr1, BYTE4(((line&8  ) ? inkColor1 : paperColor1), ((line&4 ) ? inkColor2 : paperColor2), ((line&2 ) ? inkColor1 : paperColor1), paperColor2)); addr1 +=40;
			if (bitmapVBI) {
				POKE((char*)addr2, BYTE4(((line&128) ? inkColor1 : paperColor1), ((line&64) ? inkColor2 : paperColor2), ((line&32) ? inkColor1 : paperColor1), paperColor2)); addr2 +=40;
				POKE((char*)addr2, BYTE4(((line&8  ) ? inkColor2 : paperColor2), ((line&4 ) ? inkColor1 : paperColor1), ((line&2 ) ? inkColor2 : paperColor2), paperColor1)); addr2 +=40;
			}
		}
		POKE((char*)addr1, bgByte2);
		if (bitmapVBI)
			POKE((char*)addr2, bgByte1);
	}
#elif defined __ORIC__
	// Set Character inside 6*8 cell
	unsigned char i, line;
	unsigned char a0,a2,a4,b,blank;
	unsigned int addr;
	addr = BITMAPRAM+1 + row*320 + col;
	blank = 64 + (paperColor ? 63 : 0);
	if (chr == &charBlank[0]) {
		for (i=0; i<8; ++i) {
			POKE((char*)addr, blank);
			addr += 40;
		}
	} else {
		if (paperColor != 0) {
			a0 = 2; a2 = 0; a4 = 1; b = 3;
		} else {
			a0 = 1; a2 = 3; a4 = 2; b = 0;
		}
		POKE((char*)addr, blank); addr += 40;
		for (i=0; i<3; ++i) {
			line = chr[i];
			POKE((char*)addr, BYTE4(1, ((line&128) ? a0 : b), ((line&64) ? a2 : b), ((line&32) ? a4 : b))); addr += 40;
			POKE((char*)addr, BYTE4(1, ((line&8  ) ? a0 : b), ((line&4 ) ? a2 : b), ((line&2 ) ? a4 : b))); addr += 40;
		}
		POKE((char*)addr, blank);
	}
#elif defined __CBM__
	// Set Character inside 4*8 cell
	unsigned char i,line;
	unsigned int addr;
	addr = BITMAPRAM + row*320 + col*8;
	if (chr == &charBlank[0]) {
		memset((char*)addr, pow2, 8);
	} else {
		POKE((char*)addr++, pow2);
		for (i=0; i<3; ++i) {
			line = chr[i];
			POKE((char*)addr++, BYTE4(((line&128) ? 1 : 2), ((line&64) ? 1 : 2), ((line&32) ? 1 : 2), 2));
			POKE((char*)addr++, BYTE4(((line&8  ) ? 1 : 2), ((line&4 ) ? 1 : 2), ((line&2 ) ? 1 : 2), 2));
		}
		POKE((char*)addr, pow2);
	}
	
	// Set Color
	addr = SCREENRAM + row*40 + col;
	POKE((char*)addr, inkColor << 4 | paperColor);
#elif defined __LYNX__
	// Set Character Pixels
	unsigned char i, line, paperShift, inkShift;
	unsigned int addr;
	addr = BITMAPRAM+1 + row*(492) + col*2u;
	paperShift = paperColor << 4;
	inkShift = inkColor << 4;
	POKE((char*)addr++, paperShift | paperColor);
	POKE((char*)addr,   paperShift | paperColor);
	addr += 81;
	for (i=0; i<3; ++i) {
		line = chr[i];
		if (i!=1) {
		  POKE((char*)addr++, ((line&128) ? inkShift : paperShift) | ((line&64) ? inkColor : paperColor));
		  POKE((char*)addr,   ((line&32)  ? inkShift : paperShift) | paperColor);  
		  addr += 81;
		}
		POKE((char*)addr++, ((line&8) ? inkShift : paperShift) | ((line&4)  ? inkColor : paperColor));
		POKE((char*)addr,   ((line&2) ? inkShift : paperShift) | paperColor);
		addr += 81;
	}	
#endif
}

// Find pointer to associated character
const char *GetChr(unsigned char chr)
{
	// Select the correct bitmask
	     if (chr == 95) { return &charUnderbar[0]; }
	else if (chr == 92) { return &charBwSlash[0]; }
#if defined __CBM__
	else if (chr > 192) { return &charLetter[(chr-193)*3]; }	// Upper case (C64)
	else if (chr > 96)  { return &charLetter[(chr-97)*3]; }		// Compatibility with Ascii files
	else if (chr > 64)  { return &charLetter[(chr-65)*3]; }		// Lower case (C64)
#else
	else if (chr > 96)  { return &charLetter[(chr-97)*3]; }	// Lower case (Apple/Atari/Oric/Lynx)
	else if (chr > 64)  { return &charLetter[(chr-65)*3]; }	// Upper case (Apple/Atari/Oric/Lynx)
#endif
	else if (chr == 63) { return &charQuestion[0]; }
	else if (chr == 58) { return &charColon[0]; }
	else if (chr >  47) { return &charDigit[(chr-48)*3]; }
	else if (chr == 47) { return &charFwSlash[0]; }
	else if (chr == 46) { return &charDot[0]; }
	else if (chr == 45) { return &charHyphen[0]; }
	else if (chr == 44) { return &charComma[0]; }
	else if (chr == 43) { return &charPlus[0]; }
	else if (chr == 42) { return &charStar[0]; }
	else if (chr >  39) { return &charBracket[(chr-40)*3]; }
	else if (chr == 39) { return &charQuote[0]; }
	else if (chr == 36) { return &charDollar[0]; }
	else if (chr == 33) { return &charExclaim[0]; }
	else if (chr ==  4) { return &charDeath[0]; }
	else if (chr ==  3) { return &charShield[0]; }
	else if (chr ==  2) { return &charPotion[0]; }
	else if (chr ==  1) { return &charHeart[0]; }
	return &charBlank[0];
}

// Parse string and print characters one-by-one (can be slow...)
void PrintStr(unsigned char col, unsigned char row, const char *buffer)
{
#if defined __CBM__
	unsigned int addr1, addr2;
	unsigned char i, chr, len = strlen(buffer);
	if (videoMode == CHR_MODE) {
		// Charmap mode
		addr1 = SCREENRAM + 40*row + col;
		addr2 = COLORRAM + 40*row + col;
		for (i=0; i<len; i++) {
			chr = buffer[i];
			if (chr > 192)
				chr += 32;	// Upper case (petscii)
			else 
			if (chr > 96) 
				chr += 128;	// Lower case (ascii)
			else 
			if (chr > 32) 
				chr += 160;	// Lower case (petscii)
			else
				chr += 128;	// Icons
			POKE(addr1++, chr);
			POKE(addr2++, inkColor);
		}		
	} else {
		// Bitmap mode
		for (i=0; i<len; ++i)
			PrintChr(col+i, row, GetChr(buffer[i]));
	}
#else
	unsigned char i, len = strlen(buffer);
	for (i=0; i<len; ++i)
		PrintChr(col+i, row, GetChr(buffer[i]));
#endif
	
#if defined __LYNX__
	if (autoRefresh) { UpdateDisplay(); }
#endif		
}

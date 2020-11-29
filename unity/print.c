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
	addr = BITMAPRAM + row*320 + (col+1);
	line1 = ink1[color];
	line2 = ink2[color];
	for (i=0; i<4; ++i) {
		POKE((char*)addr+i*80, line1);
		POKE((char*)addr+i*80+40, line2);
	}
  }
#endif

// Helper
#define BYTE4(a,b,c,d) ((a<<6) | (b<<4) | (c<<2) | d)

// Mode and colors used in printing function
unsigned char videoMode = TXT_MODE;
unsigned char inkColor = WHITE, paperColor = BLACK;

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
		if (i%2) {
			memset((char*)addr1, bgByte2, width);
			memset((char*)addr2, bgByte1, width);
		} else {
			memset((char*)addr1, bgByte1, width);
			memset((char*)addr2, bgByte2, width);
		}
		addr1 += 40; addr2 += 40; ++i;
	}	
#elif defined __APPLE2__
	unsigned char x, y;
	unsigned int dataAux, dataMain;
	
	// Make sure columns start and end on full 7 pixel blocks
	col = (col-col%2)/2u;
	width = (width+width%2)/2u;
	
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

// Print numerical value
void PrintNum(unsigned char col, unsigned char row, unsigned int num)
{
	unsigned int step = 1;
	unsigned char tmp;
	while ((step*10)<=num) {
		step *= 10;
	}
	while (step>0) {
		if (num >= step) { 
			tmp = num/step;
			num -= tmp*step;
		} else {
			tmp = 0;
		}
	  #if defined __CBM__
		if (videoMode == CHR_MODE) {
			// Charmap mode
			POKE(SCREENRAM+40*row+col, 208+tmp);
			POKE(COLORRAM+40*row+col, inkColor);
			col++;
		} else {
			// Bitmap mode	
			PrintChr(col++, row, &charDigit[tmp*3]);
		}
	  #else
		PrintChr(col++, row, &charDigit[tmp*3]);
	  #endif
		step /= 10u;
	}
#if defined __LYNX__
	if (autoRefresh) { UpdateDisplay(); }
#endif	
}

// Print multicolor logos of the various unity platforms
void PrintLogo(unsigned char col, unsigned char row, unsigned char index)
{
#if defined __APPLE2__
	// Define logos
	unsigned char logos[6][5][3] = { { { 0,11, 0}, {11, 0,11}, {11, 0, 0}, {11, 0,12}, { 0,11, 0} },   // C64
									 { { 0, 7, 0}, { 7, 0, 7}, { 9, 9, 9}, { 4, 0, 4}, {11, 0,11} },   // ATR
								     { { 0, 0, 7}, { 0, 7, 0}, { 9, 9, 9}, { 4, 4, 4}, { 0,11, 0} },   // APP
								     { { 0,15,12}, {15,12,15}, {15,12,15}, {12,15, 0}, {12,12,12} },   // ORI
								     { {12, 0,12}, {12,12, 0}, { 0,12, 0}, { 0,12,12}, {12, 0,12} },   // LNX
								     { {11, 7, 0}, {11, 7,11}, {11,11,11}, {11, 7,11}, {11,11,11} } }; // FLP
	unsigned int x,y;
	unsigned char i,j,n;
	
	// Compute location of character
	if (col%2) { n=4; } else { n=3; }
	x = (col*35)/10u; y = (row*8);
	
	// Set character over 3/4 pixels out of 7 in a cell
	for (i=0; i<5; ++i) {
		SetHiresPointer(x, y+i+3);
		for (j=0; j<n; j++) {
			if (j<3) {
			  #if defined __DHR__		
				SetColorDHR(logos[index][i][j]);
			  #else
				SetColorSHR(logos[index][i][j]);
			  #endif
			} else {
			  #if defined __DHR__		
				SetColorDHR(BLACK);
			  #else
				SetColorSHR(BLACK);
			  #endif
			}
			hiresPixel++;
		}
	}
	
#elif defined __ATARI__
	// Define logos (1=Red, 2=Blue, 3=Green)
	unsigned char logos[6][8] = { {0,0,0, 32,136,128,132, 32}, 		// C64: (0,2,0,0) (2,0,2,0) (2,0,0,0) (2,0,1,0) (0,2,0,0)
								  {0,0,0, 48,204, 84, 68,136},		// ATR: (0,3,0,0) (3,0,3,0) (1,1,1,0) (1,0,1,0) (2,0,2,0)
								  {0,0,0, 12, 48, 84,168, 32},  	// APP: (0,0,3,0) (0,3,0,0) (1,1,1,0) (2,2,2,0) (0,2,0,0)
								  {0,0,0, 52,220,220,112, 84},  	// ORI: (0,3,1,0) (3,1,3,0) (3,1,3,0) (1,3,0,0) (1,1,1,0)
								  {0,0,0, 68, 80, 16, 20, 68},  	// LNX: (1,0,1,0) (1,1,0,0) (0,1,0,0) (0,1,1,0) (1,0,1,0)
								  {0,0,0,188,190,170,190,170} };	// FLP: (2,3,3,0) (2,3,3,2) (2,2,2,2) (2,3,3,2) (2,2,2,2)
	unsigned int addr1, addr2;
	unsigned char i;

	// Get memory addresses
	addr1 = BITMAPRAM1+row*320+col;
	addr2 = BITMAPRAM2+row*320+col;		
	
	// Set Character data
	for (i=0; i<8; ++i) {
		POKE(addr1, logos[index][i]); addr1 += 40;
		POKE(addr2, logos[index][i]); addr2 += 40;
	}
#elif defined __ORIC__
	unsigned char logos[6][8] = { {0,0,0,12,18,16,23,12}, 	// C64: (0,0,1,1,0,0) (0,1,0,0,1,0) (0,1,0,0,0,0) (0,1,0,1,1,1) (0,0,1,1,0,0)
								  {0,0,0,12,18,30,18,18}, 	// ATR: (0,0,1,1,0,0) (0,1,0,0,1,0) (0,1,1,1,1,0) (0,1,0,0,1,0) (0,1,0,0,1,0)
								  {0,0,0, 2,12,30,30,12}, 	// APP: (0,0,0,0,1,0) (0,0,1,1,0,0) (0,1,1,1,1,0) (0,1,1,1,1,0) (0,0,1,1,0,0)
								  {0,0,0,13,18,22,28,30}, 	// ORI: (0,0,1,1,0,1) (0,1,0,0,1,0) (0,1,0,1,1,0) (0,1,1,1,0,0) (0,1,1,1,1,0)
								  {0,0,0,27,28, 4, 7,27}, 	// LNX: (0,1,1,0,1,1) (0,1,1,1,0,0) (0,0,0,1,0,0) (0,0,0,1,1,1) (0,1,1,0,1,1)
								  {0,0,0,30,18,30,30,30} };	// FLP: (0,1,1,1,1,0) (0,1,0,0,1,0) (0,1,1,1,1,0) (0,1,1,1,1,0) (0,1,1,1,1,0)
	unsigned int addr;
	unsigned char i;
	
	// Set Character data
	addr = BITMAPRAM+1 + row*320u + col;
	for (i=0; i<8; ++i) {
		POKE((char*)addr, 64+logos[index][i]); 
		addr += 40;
	}								  
#elif defined __CBM__
	// Define logos (1=Green, 2=Red, 3=Blue) 
	unsigned char logos[6][8] = { {0,0,0, 48,204,192,200, 48}, 		// C64: (0,3,0,0) (3,0,3,0) (3,0,0,0) (3,0,2,0) (0,3,0,0)
								  {0,0,0, 16, 68,168,204,204},		// ATR: (0,1,0,0) (1,0,1,0) (2,2,2,0) (3,0,3,0) (3,0,3,0)
								  {0,0,0,  4, 16,168,252, 48},		// APP: (0,0,1,0) (0,1,0,0) (2,2,2,0) (3,3,3,0) (0,3,0,0)
								  {0,0,0, 24,100,100,144,168},		// ORI: (0,1,2,0) (1,2,1,0) (1,2,1,0) (2,1,0,0) (2,2,2,0)
								  {0,0,0,136,160, 32, 40,136},		// LNX: (2,0,2,0) (2,2,0,0) (0,2,0,0) (0,2,2,0) (2,0,2,0)
								  {0,0,0,212,215,255,215,255} };	// FLP: (3,1,1,0) (3,1,1,3) (3,3,3,3) (3,1,1,3) (3,3,3,3)
	unsigned int addr1, addr2, addr3;
	unsigned char i;
	
	// Get memory addresses
	addr1 = BITMAPRAM + row*320 + col*8;
	addr2 = SCREENRAM + row*40 + col;
	addr3 = COLORRAM + row*40 + col;
	
	// Set logo colors
	POKE(addr2, GREEN << 4 | RED);
	POKE(addr3, BLUE);

	// Set Character data
	for (i=0; i<8; ++i) {
		POKE(addr1++, logos[index][i]);
	}
#elif defined __LYNX__	
	// Define logos (2=Blue, 5=Green, 8=Orange, b=Red, d=White) 
	unsigned char logos[6][6][2] = { {{0xff,0xff},{0xf2,0xff},{0x2f,0x2f},{0x2f,0xff},{0x2f,0xbf},{0xf2,0xff}}, 	// C64
									 {{0xff,0xff},{0xf5,0xff},{0x5f,0x5f},{0x88,0x8f},{0xbf,0xbf},{0x2f,0x2f}}, 	// ATR
									 {{0xff,0xff},{0xff,0x5f},{0xf5,0xff},{0x88,0x8f},{0xbb,0xbf},{0xf2,0xff}}, 	// APP
								     {{0xff,0xff},{0xfd,0xbf},{0xdb,0xdf},{0xdb,0xdf},{0xbd,0xff},{0xbb,0xbf}}, 	// ORI
									 {{0xff,0xff},{0x8f,0x8f},{0x88,0xff},{0xf8,0xff},{0xf8,0x8f},{0x8f,0x8f}}, 	// LNX
									 {{0xff,0xff},{0x2d,0xdf},{0x2d,0xd2},{0x22,0x22},{0x2d,0xd2},{0x22,0x22}} }; 	// FLP
	unsigned int addr;
	unsigned char i,j;
								  
	// Set Character data
	addr = BITMAPRAM+1 + row*492 + col*2;
	for (i=0; i<6; ++i) {
		for (j=0; j<2; ++j) {
			POKE(addr++, logos[index][i][j]);
		}
		addr += 80;
	}
#endif
}

// Print character using background and foreground colors
void PrintChr(unsigned char col, unsigned char row, const char *chr)
{
#if defined __APPLE2__
	// Set Character over 3/4 pixels out of 7 in a cell
	unsigned int x,y;
	unsigned char i,j,n;
	if (col%2) { n=4; } else { n=3; }
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
	unsigned char i;
	unsigned int addr1,addr2;
	inkColor1 = inkColor&3; inkColor2 = inkColor/4u;
	paperColor1 = paperColor&3; paperColor2 = paperColor/4u;
	bgByte1 = BYTE4(paperColor1,paperColor2,paperColor1,paperColor2);
	bgByte2 = BYTE4(paperColor2,paperColor1,paperColor2,paperColor1);	
	addr1 = BITMAPRAM1 + row*320 + col;
	addr2 = BITMAPRAM2 + row*320 + col;
	if (chr == &charBlank[0]) {
		for (i=0; i<8; ++i) {
			if (i%2) {
				POKE((char*)addr1, bgByte2);
				POKE((char*)addr2, bgByte1);
			} else {
				POKE((char*)addr1, bgByte1);
				POKE((char*)addr2, bgByte2);
			}
			addr1 +=40; addr2 +=40;
		}
	} else {
		POKE((char*)addr1, bgByte1); addr1 +=40;
		POKE((char*)addr2, bgByte2); addr2 +=40;
		for (i=0; i<3; ++i) {
			POKE((char*)addr2, BYTE4(((chr[i]&128) ? inkColor1 : paperColor1), ((chr[i]&64) ? inkColor2 : paperColor2), ((chr[i]&32) ? inkColor1 : paperColor1), paperColor2)); addr2 +=40;
			POKE((char*)addr1, BYTE4(((chr[i]&128) ? inkColor2 : paperColor2), ((chr[i]&64) ? inkColor1 : paperColor1), ((chr[i]&32) ? inkColor2 : paperColor2), paperColor1)); addr1 +=40;
			POKE((char*)addr1, BYTE4(((chr[i]&8  ) ? inkColor1 : paperColor1), ((chr[i]&4 ) ? inkColor2 : paperColor2), ((chr[i]&2 ) ? inkColor1 : paperColor1), paperColor2)); addr1 +=40;
			POKE((char*)addr2, BYTE4(((chr[i]&8  ) ? inkColor2 : paperColor2), ((chr[i]&4 ) ? inkColor1 : paperColor1), ((chr[i]&2 ) ? inkColor2 : paperColor2), paperColor1)); addr2 +=40;
		}
		POKE((char*)addr1, bgByte2);
		POKE((char*)addr2, bgByte1);
	}
#elif defined __ORIC__
	// Set Character inside 6*8 cell
	unsigned char i;
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
			POKE((char*)addr, BYTE4(1, ((chr[i]&128) ? a0 : b), ((chr[i]&64) ? a2 : b), ((chr[i]&32) ? a4 : b))); addr += 40;
			POKE((char*)addr, BYTE4(1, ((chr[i]&8  ) ? a0 : b), ((chr[i]&4 ) ? a2 : b), ((chr[i]&2 ) ? a4 : b))); addr += 40;
		}
		POKE((char*)addr, blank);
	}
#elif defined __CBM__
	// Set Character inside 4*8 cell
	unsigned char i;
	unsigned int addr;
	addr = BITMAPRAM + row*320 + col*8;
	if (chr == &charBlank[0]) {
		memset((char*)addr, pow2, 8);
	} else {
		POKE((char*)addr++, pow2);
		for (i=0; i<3; ++i) {
			POKE((char*)addr++, BYTE4(((chr[i]&128) ? 1 : 2), ((chr[i]&64) ? 1 : 2), ((chr[i]&32) ? 1 : 2), 2));
			POKE((char*)addr++, BYTE4(((chr[i]&8  ) ? 1 : 2), ((chr[i]&4 ) ? 1 : 2), ((chr[i]&2 ) ? 1 : 2), 2));
		}
		POKE((char*)addr, pow2);
	}
	
	// Set Color
	addr = SCREENRAM + row*40 + col;
	POKE((char*)addr, inkColor << 4 | paperColor);
#elif defined __LYNX__
	// Set Character Pixels
	unsigned char i;
	unsigned int addr;
	addr = BITMAPRAM+1 + row*(492) + col*2u;
	POKE((char*)addr++, (paperColor << 4) | paperColor);
	POKE((char*)addr,   (paperColor << 4) | paperColor);
	addr += 81;
	for (i=0; i<3; ++i) {
		if (i!=1) {
		  POKE((char*)addr++, ((chr[i]&128) ? inkColor : paperColor) << 4 | ((chr[i]&64) ? inkColor : paperColor));
		  POKE((char*)addr,   ((chr[i]&32)  ? inkColor : paperColor) << 4 | paperColor);  
		  addr += 81;
		}
		POKE((char*)addr++, ((chr[i]&8) ? inkColor : paperColor) << 4 | ((chr[i]&4)  ? inkColor : paperColor));
		POKE((char*)addr,   ((chr[i]&2) ? inkColor : paperColor) << 4 | paperColor);
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
	else if (chr > 47)  { return &charDigit[(chr-48)*3]; }
	else if (chr == 47) { return &charFwSlash[0]; }
	else if (chr == 46) { return &charDot[0]; }
	else if (chr == 45) { return &charHyphen[0]; }
	else if (chr == 44) { return &charComma[0]; }
	else if (chr == 43) { return &charPlus[0]; }
	else if (chr == 42) { return &charStar[0]; }
	else if (chr > 39)  { return &charBracket[(chr-40)*3]; }
	else if (chr == 39) { return &charQuote[0]; }
	else if (chr == 36) { return &charDollar[0]; }
	else if (chr == 33) { return &charExclaim[0]; }
	else if (chr == 4) { return &charDeath[0]; }
	else if (chr == 3) { return &charShield[0]; }
	else if (chr == 2) { return &charPotion[0]; }
	else if (chr == 1) { return &charHeart[0]; }
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
			if (buffer[i] > 192)
				chr = buffer[i]+32;		// Upper case (petscii)
			else 
			if (buffer[i] > 96) 
				chr = buffer[i]+128;	// Lower case (ascii)
			else 
			if (buffer[i] > 32) 
				chr = buffer[i]+160;	// Lower case (petscii)
			else
				chr = buffer[i]+128;	// Icons
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

// Copy string from one area of screen to another
void CopyStr(unsigned char col1, unsigned char row1, unsigned char col2, unsigned char row2, unsigned char len)
{
#if defined __CBM__
	// Copy bitmap and screen ram
	rom_disable();
	memcpy((char*)BITMAPRAM+row1*320+col1*8, (char*)BITMAPRAM+row2*320+col2*8, len*8);
	rom_enable();
	memcpy((char*)SCREENRAM+row1*40+col1, (char*)SCREENRAM+row2*40+col2, len);
#elif defined __ATARI__
	// Copy bitmap 1 and 2
	unsigned char i;
	unsigned int src, dst;
	src = row2*320+col2;
	dst = row1*320+col1;
	for (i=0; i<8; ++i) {
		memcpy((char*)BITMAPRAM1+dst, (char*)BITMAPRAM1+src, len);
		memcpy((char*)BITMAPRAM2+dst, (char*)BITMAPRAM2+src, len);
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
	col1 += col1%2; col2 += col2%2;	
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
	unsigned char i;
	unsigned int src, dst;
	src = BITMAPRAM+1+row2*492+col2*2;
	dst = BITMAPRAM+1+row1*492+col1*2;
	for (i=0; i<6; ++i)	{
		memcpy((char*)dst, (char*)src, len*2);
		src += 82; dst += 82;
	}
#endif	
}

// Interactive text input function
unsigned char InputStr(unsigned char col, unsigned char row, unsigned char width, char *buffer, unsigned char len, unsigned char key)
{
	unsigned char i, curlen, offset;
	
	// Check current length of input
	curlen = strlen(buffer);
	if 	(curlen < width)
		offset = curlen;
	else
		offset = width;
		
	// Was a new key received?
	if (!key) {
		// Initialize input field
		PrintBlanks(col, row, width+1, 1);
		PrintStr(col, row, &buffer[curlen-offset]);
		
	} else {		
		// Process Letter keys
		if (curlen < len) { 
		#if (defined __ATARI__) || (defined __ORIC__)
			if (key == 32 | key == 33 | (key > 38 & key < 59) | key == 63 | key == 92 | key == 95 | (key > 96 & key < 123)) {	// Atari/Oric
		#else
			if (key == 32 | key == 33 | (key > 38 & key < 59) | key == 63 | (key > 64 & key < 91) | key == 92 | key == 95) {	// Apple/C64/Lynx
		#endif
				buffer[curlen] = key;
				buffer[curlen+1] = 0;
				if (curlen >= width) {
					CopyStr(col, row, col+1, row, width-1);
					offset--;
				}
				PrintChr(col+offset, row, GetChr(key));
				offset++;
			}
		}
		
		// Process Delete key
		if (curlen > 0) {
			if (key == CH_DEL) {
				buffer[curlen-1] = 0;				
				if 	(curlen > width) {
					for (i=width-1; i>0; i--)
						CopyStr(col+i, row, col+i-1, row, 1);
					PrintChr(col, row, GetChr(buffer[curlen-width-1]));					
				} else {
					PrintChr(col+offset, row, &charBlank[0]);
					offset--;					
				}
			}
		}

		// Was return key pressed?
		if (key == CH_ENTER) { 
			return 1; 
		}
	}

	// Show cursor
	PrintChr(col+offset, row, &charUnderbar[0]);	
	return 0;
}

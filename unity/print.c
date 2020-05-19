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
  #pragma code-name("LC")
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

#ifdef __LYNX__   
  unsigned char autoRefresh = 1;	// Allows auto-refresh of the display when PrintStr(), PrintNum() or PrintBlanks() are used
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

// Helper functions
#define BYTE4(a,b,c,d) ((a<<6) | (b<<4) | (c<<2) | d)

// Colors used in printing function
unsigned char inkColor, paperColor;

// Rapidly fill memory area with blank characters
void PrintBlanks(unsigned char colBeg, unsigned char rowBeg, unsigned char colEnd, unsigned char rowEnd)
{
	// Black-out menu area
	unsigned char y;
	unsigned int span;
#if defined __ATARI__
	rowEnd++;
	span = colEnd-colBeg+1;
	paperColor1 = paperColor%4;
	paperColor2 = paperColor/4;
	bgByte1 = BYTE4(paperColor1,paperColor2,paperColor1,paperColor2);
	bgByte2 = BYTE4(paperColor2,paperColor1,paperColor2,paperColor1);
	for (y=rowBeg*8; y<rowEnd*8; ++y) {
		if (y%2) {
			memset((char*)(BITMAPRAM1+y*40+colBeg), bgByte2, span);
			memset((char*)(BITMAPRAM2+y*40+colBeg), bgByte1, span);
		} else {
			memset((char*)(BITMAPRAM1+y*40+colBeg), bgByte1, span);
			memset((char*)(BITMAPRAM2+y*40+colBeg), bgByte2, span);
		}
	}	
#elif defined __APPLE2__
	unsigned char i, x, col1, col2;
	unsigned int dataAux, dataMain;
	
	// Extend loops to last col/row
	rowEnd++; colEnd++;	
	
	// Make sure columns are even
	colBeg -= colBeg%2;
	colEnd += colEnd%2;
	
	// Create sample block at top-left (to encode color info)
	x = (7*colBeg)/2;
	y = rowBeg*8;
	for (i=0; i<7; ++i) {
		SetDHRPointer(x+i, y);
		SetDHRColor(paperColor);
	}
	
	// Get sample block value
	SetDHRPointer(x, y);
	*dhraux = 0;  dataAux = PEEKW(dhrptr);
	*dhrmain = 0; dataMain = PEEKW(dhrptr);
	
	// Compute column range (can only copy across evenly numbered columns)
	col1 = colBeg/2;
	col2 = colEnd/2;

	// Copy block across the rest of DHR memory
	for (y=rowBeg*8; y<rowEnd*8; ++y) {
		SetDHRPointer(x, y);
		for (i=col1; i<col2; ++i) {
			*dhraux = 0;  POKEW(dhrptr, dataAux);
			*dhrmain = 0; POKEW(dhrptr, dataMain);
			dhrptr += 2;
		}
	}
#elif defined __ORIC__
	// Fill with 0s (papercolor) or 1s (inkcolor)
	unsigned char value;
	rowEnd++;
	span = colEnd-colBeg+1;
	if (paperColor) value = 127; else value = 64;
	for (y=rowBeg*8; y<rowEnd*8; ++y) {
		memset((char*)(BITMAPRAM+40*y+colBeg+1), value, span);
	}
#elif defined __CBM__
	rowEnd++;
	span = colEnd-colBeg+1;
	for (y=rowBeg; y<rowEnd; ++y) {
		memset((char*)(BITMAPRAM+320*y+colBeg*8), pow2, span*8);
		memset((char*)(SCREENRAM+40*y+colBeg), paperColor, span);		
	}	
#elif defined __LYNX__
	rowEnd++;
	span = colEnd-colBeg+1;
	for (y=rowBeg*6; y<rowEnd*6; ++y) {
		memset(BITMAPRAM+y*82+colBeg*2+1, (paperColor << 4) | paperColor, span*2);
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
		PrintChr(col++, row, &charDigit[tmp*3]);
		step /= 10;
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
	x = (col*35)/10; y = (row*8);
	
	// Set character over 3/4 pixels out of 7 in a cell
	for (i=0; i<5; ++i) {
		SetDHRPointer(x, y+i+3);
		for (j=0; j<n; j++) {
			if (j<3) {
				SetDHRColor(logos[index][i][j]);
			} else {
				SetDHRColor(BLACK);
			}
			dhrpixel++;
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
		POKE(addr1+i*40, logos[index][i]);
		POKE(addr2+i*40, logos[index][i]);
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
	addr = BITMAPRAM + row*320 + (col+1);
	for (i=0; i<8; ++i) {
		POKE((char*)addr+i*40, 64+logos[index][i]);
	}								  
#elif defined __CBM__
	// Define logos (1=Green, 2=Red, 3=Blue) 
	unsigned char logos[6][8] = { {0,0,0, 48,204,192,200, 48}, 		// C64: (0,3,0,0) (3,0,3,0) (3,0,0,0) (3,0,2,0) (0,3,0,0)
								  {0,0,0, 16, 68,168,204,204},		// ATR: (0,1,0,0) (1,0,1,0) (2,2,2,0) (3,0,3,0) (3,0,3,0)
								  {0,0,0,  4, 16,168,252, 48},		// APP: (0,0,1,0) (0,1,0,0) (2,2,2,0) (3,3,3,0) (0,3,0,0)
								  {0,0,0, 24,100,100,144,168},		// ORI: (0,1,2,0) (1,2,1,0) (1,2,1,0) (2,1,0,0) (2,2,2,0)
								  {0,0,0,136,160, 32, 40,136},		// LNX: (2,0,2,0) (2,2,0,0) (0,2,0,0) (0,2,2,0) (2,0,2,0)
								  {0,0,0,212,215,255,215,255} };	// FLP: (3,1,1,0) (3,1,1,3) (3,3,3,3) (3,1,1,3) (3,3,3,3)
	unsigned int addr1, addr2;
	unsigned char i;
	
	// Get memory addresses
	addr1 = BITMAPRAM + 40*((row*8)&248)+((col*8)&504);
	addr2 = SCREENRAM + row*40+col;
	
	// Set logo colors
	POKE(addr2, GREEN << 4 | RED);
	POKE(COLORRAM + row*40+col, BLUE);

	// Set Character data
	for (i=0; i<8; ++i) {
		POKE(addr1+i, logos[index][i]);
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
	addr = BITMAPRAM+1 + row*(6*82) + col*2;
	for (i=0; i<6; ++i) {
		for (j=0; j<2; ++j) {
			POKE(addr+i*82+j, logos[index][i][j]);
		}
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
	if ((col > CHR_COLS) || (row > CHR_ROWS)) { return; }		
	if (col%2) { n=4; } else { n=3; }
	x = (col*35)/10; y = (row*8);
	SetDHRPointer(x, y);	
	for (j=0; j<n; j++) {
		SetDHRColor(paperColor);
		dhrpixel++;
	}
	for (i=0; i<3; ++i) {
		SetDHRPointer(x, y+i*2+1);
		for (j=0; j<n; j++) {
			SetDHRColor(((chr[i]>>(7-j))&1) ? inkColor : paperColor);
			dhrpixel++;
		}
		SetDHRPointer(x, y+i*2+2);
		for (j=0; j<n; j++) {
			SetDHRColor(((chr[i]>>(3-j))&1) ? inkColor : paperColor);
			dhrpixel++;
		}
	}
	SetDHRPointer(x, y+7);
	for (j=0; j<n; j++) {
		SetDHRColor(paperColor);
		dhrpixel++;
	}

	// Update clock (slow function)
	clk += 2;
#elif defined __ATARI__	
	// Set Character across double buffer
	unsigned char i;
	unsigned int addr1,addr2;
	if ((col > CHR_COLS) || (row > CHR_ROWS)) { return; }		
	inkColor1 = inkColor%4; inkColor2 = inkColor/4;
	paperColor1 = paperColor%4; paperColor2 = paperColor/4;
	bgByte1 = BYTE4(paperColor1,paperColor2,paperColor1,paperColor2);
	bgByte2 = BYTE4(paperColor2,paperColor1,paperColor2,paperColor1);	
	addr1 = BITMAPRAM1 + row*320 + col;
	addr2 = BITMAPRAM2 + row*320 + col;
	if (chr == &charBlank[0]) {
		for (i=0; i<8; ++i) {
			if (i%2) {
				POKE((char*)addr1+i*40, bgByte2);
				POKE((char*)addr2+i*40, bgByte1);
			} else {
				POKE((char*)addr1+i*40, bgByte1);
				POKE((char*)addr2+i*40, bgByte2);
			}
		}
	} else {
		POKE((char*)addr1+0*40, bgByte1);
		POKE((char*)addr2+0*40, bgByte2);
		for (i=0; i<3; ++i) {
			POKE((char*)addr2+(i*2+1)*40, BYTE4(((chr[i]&128) ? inkColor1 : paperColor1), ((chr[i]&64) ? inkColor2 : paperColor2), ((chr[i]&32) ? inkColor1 : paperColor1), paperColor2));
			POKE((char*)addr1+(i*2+2)*40, BYTE4(((chr[i]&8  ) ? inkColor1 : paperColor1), ((chr[i]&4 ) ? inkColor2 : paperColor2), ((chr[i]&2 ) ? inkColor1 : paperColor1), paperColor2));
			POKE((char*)addr1+(i*2+1)*40, BYTE4(((chr[i]&128) ? inkColor2 : paperColor2), ((chr[i]&64) ? inkColor1 : paperColor1), ((chr[i]&32) ? inkColor2 : paperColor2), paperColor1));
			POKE((char*)addr2+(i*2+2)*40, BYTE4(((chr[i]&8  ) ? inkColor2 : paperColor2), ((chr[i]&4 ) ? inkColor1 : paperColor1), ((chr[i]&2 ) ? inkColor2 : paperColor2), paperColor1));
		}
		POKE((char*)addr1+7*40, bgByte2);
		POKE((char*)addr2+7*40, bgByte1);
	}
#elif defined __ORIC__
	// Set Character inside 6*8 cell
	unsigned char i;
	unsigned char a0,a2,a4,b,blank;
	unsigned int addr;
	if ((col > CHR_COLS) || (row > CHR_ROWS)) { return; }		
	addr = BITMAPRAM+1 + row*320 + col;
	blank = 64+ (paperColor ? 63 : 0);
	if (chr == &charBlank[0]) {
		for (i=0; i<8; ++i) {
			POKE((char*)addr+i*40, blank);
		}
	} else {
		if (paperColor != 0) {
			a0 = 2; a2 = 0; a4 = 1; b = 3;
		} else {
			a0 = 1; a2 = 3; a4 = 2; b = 0;
		}
		POKE((char*)addr+0*40, blank);
		for (i=0; i<3; ++i) {
			POKE((char*)addr+(i*2+1)*40, BYTE4(1, ((chr[i]&128) ? a0 : b), ((chr[i]&64) ? a2 : b), ((chr[i]&32) ? a4 : b)));
			POKE((char*)addr+(i*2+2)*40, BYTE4(1, ((chr[i]&8  ) ? a0 : b), ((chr[i]&4 ) ? a2 : b), ((chr[i]&2 ) ? a4 : b)));	
		}
		POKE((char*)addr+7*40, blank);
	}
#elif defined __CBM__
	// Set Character inside 4*8 cell
	unsigned char i;
	unsigned int addr;
	if ((col > CHR_COLS) || (row > CHR_ROWS)) { return; }		
	addr = BITMAPRAM + 40*((row*8)&248) + ((col*8)&504);
	if (chr == &charBlank[0]) {
		memset((char*)addr, pow2, 8);
	} else {
		POKE((char*)addr, pow2);
		for (i=0; i<3; ++i) {
			POKE((char*)addr+2*i+1, BYTE4(((chr[i]&128) ? 1 : 2), ((chr[i]&64) ? 1 : 2), ((chr[i]&32) ? 1 : 2), 2));
			POKE((char*)addr+2*i+2, BYTE4(((chr[i]&8  ) ? 1 : 2), ((chr[i]&4 ) ? 1 : 2), ((chr[i]&2 ) ? 1 : 2), 2));
		}
		POKE((char*)addr+7, pow2);
	}
	
	// Set Color
	addr = SCREENRAM + row*40 + col;
	POKE((char*)addr, inkColor << 4 | paperColor);
#elif defined __LYNX__
	// Set Character Pixels
	unsigned char i,j,offset;
	unsigned int addr;
	addr = BITMAPRAM+1 + row*(6*82) + col*2;
	POKE((char*)addr++, (paperColor << 4) | paperColor);
	POKE((char*)addr++, (paperColor << 4) | paperColor);
	addr += 80;
	for (i=0; i<3; ++i) {
		if (i!=1) {
		  POKE((char*)addr++, ((chr[i]&128) ? inkColor : paperColor) << 4 | ((chr[i]&64) ? inkColor : paperColor));
		  POKE((char*)addr++, ((chr[i]&32)  ? inkColor : paperColor) << 4 | paperColor);
		  addr += 80;
		}
		POKE((char*)addr++, ((chr[i]&8)   ? inkColor : paperColor) << 4 | ((chr[i]&4)  ? inkColor : paperColor));
		POKE((char*)addr++, ((chr[i]&2)   ? inkColor : paperColor) << 4 | paperColor);
		addr += 80;
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
	else if (chr > 64)  { return &charLetter[(chr-65)*3]; }		// Lower case (C64)
#else
	else if (chr > 96)  { return &charLetter[(chr-97)*3]; }	// Lower case (Apple/Atari)
	else if (chr > 64)  { return &charLetter[(chr-65)*3]; }	// Upper case (Apple/Atari)
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
	else if (chr == 33) { return &charExclaim[0]; }
	return &charBlank[0];
}

// Parse string and print characters one-by-one (can be slow...)
void PrintStr(unsigned char col, unsigned char row, const char *buffer)
{
	// Parse buffer
	const char *chr;
	unsigned char i;
	for (i=0; i<strlen(buffer); ++i) {
		chr = GetChr(buffer[i]);
		PrintChr(col+i, row, chr);
	}
#if defined __LYNX__
	if (autoRefresh) { UpdateDisplay(); }
#endif		
}

// Rolling buffer at the top of the screen, that moves text leftward when printing
void PrintBuffer(char *buffer)
{
	unsigned char len, i;
	buffer[CHR_COLS] = 0;
	len = strlen(buffer);
#if defined __CBM__
	// Copy bitmap and screen ram
	DisableRom();
	memcpy((char*)BITMAPRAM, (char*)BITMAPRAM+len*8, (CHR_COLS-len)*8);
	EnableRom();
	memcpy((char*)SCREENRAM, (char*)SCREENRAM+len, (CHR_COLS-len));
#elif defined __ATARI__
	// Copy bitmap 1 and 2
	for (i=0; i<8; ++i) {
		memcpy((char*)BITMAPRAM1+i*40, (char*)BITMAPRAM1+i*40+len, (CHR_COLS-len));
		memcpy((char*)BITMAPRAM2+i*40, (char*)BITMAPRAM2+i*40+len, (CHR_COLS-len));
	}
#elif defined __ORIC__
	// Copy bitmap RAM
	for (i=0; i<8; ++i) {
		memcpy((char*)BITMAPRAM+1+i*40, (char*)BITMAPRAM+1+i*40+len, (CHR_COLS-len));
	}
	// Apply ink change
	if (buffer[0] == '^') {
		SetAttributes(CHR_COLS-len, 0, inkColor);
		buffer = &buffer[1];
	}
#elif defined __APPLE2__
	// Always copy 7 pixels at a time!
	len = 2*(len/2+len%2);
	for (i=0; i<8; ++i) {
		SetDHRPointer(0, i);		
		*dhraux = 0;
		memcpy((char*)(dhrptr), (char*)(dhrptr+len), (CHR_COLS-len));
		*dhrmain = 0;
		memcpy((char*)(dhrptr), (char*)(dhrptr+len), (CHR_COLS-len));
	}
	if (strlen(buffer)%2) { PrintChr(CHR_COLS-1, 0, charBlank); }
#elif defined __LYNX__
	// Copy bitmap RAM
	for (i=0; i<6; ++i) {	
		memcpy((char*)BITMAPRAM+1+i*82, (char*)BITMAPRAM+1+i*82+len*2, (CHR_COLS-len)*2);
	}
#endif
	// Print new message
	PrintStr(CHR_COLS-len, 0, buffer);
}

// Interactive text input function
unsigned char InputUpdate(unsigned char col, unsigned char row, char *buffer, unsigned char len, unsigned char key)
{
	unsigned char curlen;
	
	// Was a new key received?
	if (!key) {
		// Initialize input field
		PrintBlanks(col, row, col+len, row);
		PrintStr(col, row, buffer);
	} else {
		// Check current length of input
		curlen = strlen(buffer);
		
		// Process Letter keys
#if (defined __ATARI__) || (defined __ORIC__)
		if (key == 32 | key == 33 | (key > 38 & key < 59) | key == 63 | key == 92 | key == 95 | (key > 96 & key < 123)) {	// Atari/Oric
#else
		if (key == 32 | key == 33 | (key > 38 & key < 59) | key == 63 | (key > 64 & key < 91) | key == 92 | key == 95) {	// Apple/C64
#endif
			if (curlen < len) { 
				buffer[curlen] = key;
				buffer[curlen+1] = 0; 
				PrintChr(col+curlen, row, GetChr(key));
			}
		}
		
		// Process Delete key
		if (key == CH_DEL) {
			if (curlen > 0) {
				buffer[curlen-1] = 0;
				PrintChr(col+curlen, row, &charBlank[0]);
			}
		}

		// Return key
		if (key == CH_ENTER) { return 1; }
	}
	
	// Show cursor
	PrintChr(col+strlen(buffer), row, &charUnderbar[0]);
	return 0;
}

void InputStr(unsigned char col, unsigned char row, char *buffer, unsigned char len)
{
	// Print initial condition
	InputUpdate(col, row, buffer, len, 0);

	// Run input loop
#if defined __LYNX__ 
	ShowKeyboardOverlay();
	while (1) {
		while (!KeyboardOverlayHit()) { UpdateDisplay(); } // Refresh Lynx screen
		if (InputUpdate(col, row, buffer, len, GetKeyboardOverlay())) {
			HideKeyboardOverlay();
			return; 
		}
	}
#else
	while (1) {
		while (!kbhit()) {}
		if (InputUpdate(col, row, buffer, len, cgetc())) { 
			return; 
		}
	}
#endif
}

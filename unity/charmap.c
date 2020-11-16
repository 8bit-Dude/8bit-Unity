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

#if (defined __APPLE2__) || (defined __ORIC__)
  extern unsigned char sprDrawn[];
  void Scroll(void);
#elif defined __LYNX__	
  extern unsigned char charsetData[];
  unsigned char *charmapData;
#endif

extern unsigned char inkColor;

unsigned char charmapX, charmapY, charmapWidth, charmapHeight;
unsigned char scrollCol1 = 0, scrollCol2 = CHR_COLS;
unsigned char scrollRow1 = 0, scrollRow2 = CHR_ROWS;

// Initialize Charmap Mode
void InitCharmap() 
{	
#if (defined __APPLE2__) || (defined __LYNX__) || (defined __ORIC__)
	InitBitmap();

#elif defined __ATARI__	
	// Disable cursor
	POKEW(0x0058, 0);	// SAVMSC
	POKEW(0x005E, 0);	// OLDADR
	POKE(0x005D, 0);	// OLDCHR
	
	// Switch OFF ANTIC
	POKE(559, 2);	
#endif		
}

// Switch from Text mode to Charmap mode
void EnterCharmapMode()
{		
#if (defined __APPLE2__) || (defined __LYNX__)
	EnterBitmapMode();
	
#elif defined __ATARI__	
	// Setup Charmap DLIST
	CharmapDLIST();

	// Set character set page
	charsetPage1 = 0xa0;
	charsetPage2 = 0xa4;
	POKE(0x02f4, 0xa0);	
	
	// DLI parameters
	StartDLI();
	waitvsync();
	charmapDLI = 1;
	
	// ANTIC: DMA Screen
	POKE(559, PEEK(559)|32);	
	
#elif defined __CBM__	
	// Setup VIC2 (memory bank and multicolor mode)
	SetupVIC2();	
#endif	

	// Reset scroll coords (out of range)
	charmapX = 255; charmapY = 255;
}

// Switch back to Text mode
void ExitCharmapMode()
{
#if defined __APPLE2__	
	ExitBitmapMode();
	
#elif defined __ATARI__	
    // Switch screen DMA and DLI
	POKE(559, PEEK(559)&~32);
	charmapDLI = 0;
	
#elif defined __CBM__	
	ResetVIC2();
#endif	
}

// Load charset and associated attributes / flags
void LoadCharset(char* filename)
{
#if defined __APPLE2__
	FILE* fp = fopen(filename, "rb");
  #if defined __DHR__	
	fread((char*)CHARSETRAM, 1, 0x1080, fp);
  #else	
	fread((char*)CHARSETRAM, 1, 0x0880, fp);
  #endif
	fclose(fp);		
	
#elif defined __ATARI__
	if (FileOpen(filename)) {
		FileRead((char*)CHARSETRAM, 0x0800);
		FileRead((char*)CHARATRRAM, 0x0100);
	}
	
	// Set Colors
	POKE(0x02c8, 0x00);
	POKE(0x02c4, 0x0c);	
	POKE(0x02c5, 0x78);
	POKE(0x02c6, 0x62);
	POKE(0x02c7, 0x12);	// WARNING: This color is shared with sprite 5!
	
#elif defined __CBM__	
	FILE* fp = fopen(filename, "rb");	
	fread((char*)CHARSETRAM, 1, 0x0800, fp);
	fread((char*)CHARATRRAM, 1, 0x0100, fp);
	fclose(fp);	
	
	// Set Colors
	POKE(0xd021, BLACK);	// BG Color
	POKE(0xd022, WHITE); 	// MC1
	POKE(0xd023, LBLUE);	// MC2	
	
#elif defined __ORIC__
	FileRead(filename, (void*)CHARSETRAM);
#endif			
}

// Clear entire screen
void ClearCharmap()
{
#if (defined __APPLE2__) || (defined __LYNX__) || (defined __ORIC__)
	ClearBitmap();

#elif defined __ATARI__
	bzero((char*)SCREENRAM, 1000);
	
#elif defined __CBM__
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)COLORRAM,  1000);	
#endif
}

// Load charmap from file
void LoadCharmap(char *filename) 
{
#if defined __ATARI__
	if (FileOpen(filename))
		FileRead((char*)CHARMAPRAM, 0x2000);
	
#elif (defined __APPLE2__) || (defined __CBM__)
	FILE* fp = fopen(filename, "rb");	
	fread((char*)CHARMAPRAM, 1, 0x2000, fp);
	fclose(fp);
	
#elif defined __LYNX__
	// Free previous map and load new one
	unsigned int size;
	if (charmapData) free(charmapData);	
	size = FileRead(filename);	
	if (size) {
		charmapData = malloc(size);
		memcpy(charmapData, BITMAPRAM, size);
	}
	ClearBitmap();
	
#elif defined __ORIC__
	FileRead(filename, (void*)CHARMAPRAM);	
#endif
	// Set Dimensions
	charmapWidth = 96;
	charmapHeight = 64;
}

unsigned char GetCharFlags(unsigned char x, unsigned char y)
{
	// Get flags of specified tile
	unsigned char chr;
#if defined __LYNX__
	chr = charmapData[charmapWidth*y + x];
	return charsetData[1536+chr];
#else
	chr = PEEK(CHARMAPRAM + charmapWidth*y + x);
	return PEEK(CHARFLGRAM + chr);
#endif
}

void ScrollCharmap(unsigned char x, unsigned char y)
{
	unsigned char i, j, k, chr;
	unsigned int src, dst, col, tmp1, tmp2;
	
	// Check if map was moved?
	if (x == charmapX && y == charmapY)
		return;
	charmapX = x;
	charmapY = y;

#if defined __APPLE2__
	// Reset sprite background
	for (i=0; i<SPRITE_NUM; i++)
		sprDrawn[i] = 0;

	src = CHARMAPRAM + charmapWidth*y + (x - scrollCol1);
	POKE(0xe3, scrollRow1); 		
	POKE(0xeb, scrollRow2);		
	POKE(0xec, scrollCol1);		
	POKE(0xed, scrollCol2);		
	POKE(0xee, charmapWidth);
	POKEW(0xef, src);
  #if defined __DHR__
	ScrollDHR();
  #else
	ScrollSHR();
  #endif

#elif defined __ATARI__	
	src = CHARMAPRAM + charmapWidth*y + (x - scrollCol1);
	dst = SCREENRAM + scrollRow1*40;
	for (j=scrollRow1; j<scrollRow2; j++) {
		for (i=scrollCol1; i<scrollCol2; i++) {
			chr = PEEK(src+i);
			k = PEEK(CHARATRRAM+chr);	// 0 or 128
			POKE(dst+i, chr+k);
		}
		src += charmapWidth;
		dst += 40;
	}
	
#elif defined __CBM__	
	src = CHARMAPRAM + charmapWidth*y + (x - scrollCol1);	
	dst = SCREENRAM + scrollRow1*40;
	col = COLORRAM + scrollRow1*40;
	for (j=scrollRow1; j<scrollRow2; j++) {
		for (i=scrollCol1; i<scrollCol2; i++) {
			chr = PEEK(src+i);
			POKE(dst+i, chr);
			POKE(col+i, PEEK(CHARATRRAM+chr));
		}
		src += charmapWidth;
		dst += 40;
		col += 40;
	}
		
#elif defined __LYNX__	
	src = &charmapData[charmapWidth*y + (x - scrollCol1)];
	dst = BITMAPRAM + scrollRow1*6*82 + 1;
	POKE(0xb0, scrollRow1); 		
	POKE(0xb1, scrollRow2);		
	POKE(0xb2, scrollCol1);		
	POKE(0xb3, scrollCol2);		
	POKE(0xb4, charmapWidth);
	POKEW(0xb5, src);
	POKEW(0xb7, dst);
	POKEW(0xb9, charsetData);
	Scroll();	
	
#elif defined __ORIC__	
	// Reset sprite background
	for (i=0; i<SPRITE_NUM; i++)
		sprDrawn[i] = 0;

	src = CHARMAPRAM + charmapWidth*y + (x - scrollCol1);
	dst = BITMAPRAM + scrollRow1*320 + 1;
	POKE(0xb0, scrollRow1); 		
	POKE(0xb1, scrollRow2);		
	POKE(0xb2, scrollCol1);		
	POKE(0xb3, scrollCol2);		
	POKE(0xb4, charmapWidth);
	POKEW(0xb5, src);
	POKEW(0xb7, dst);
	Scroll();
#endif
}

void PrintCharmap(unsigned char x, unsigned char y, unsigned char* str)
{
	unsigned char i, chr;
	unsigned int addr1;
	
#if (defined __APPLE2__) || (defined __LYNX__) || (defined __ORIC__)
	PrintStr(x, y, str);	
	
#elif (defined __ATARI__)
	addr1 = SCREENRAM + 40*y + x;
	for (i=0; i<strlen(str); i++) {
		if (str[i] > 96) 
			chr = str[i];		// Lower case
		else
		if (str[i] > 32) 
			chr = str[i]+32;	// Upper case
		else
			chr = str[i];		// Icons
		POKE(addr1++, chr);
	}
	
#elif defined __CBM__	
	unsigned int addr2;
	addr1 = SCREENRAM + 40*y + x;
	addr2 = COLORRAM + 40*y + x;
	for (i=0; i<strlen(str); i++) {
		if (str[i] > 192)
			chr = str[i]+32;	// Upper case (petscii)
		else 
		if (str[i] > 96) 
			chr = str[i]+128;	// Lower case (ascii)
		else 
		if (str[i] > 32) 
			chr = str[i]+160;	// Lower case (petscii)
		else
			chr = str[i]+128;	// Icons
		POKE(addr1++, chr);
		POKE(addr2++, inkColor);
	}
#endif
}

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

#if defined __CBM__	
  extern unsigned int spritePtr;
#elif defined __LYNX__	
  extern unsigned char charFlags[];
#endif

extern unsigned char inkColor;

unsigned char charmapWidth, charmapHeight;
unsigned char scrollWidth = 40, scrollHeight = 25;
unsigned char charmapX, charmapY, charmapFrame = 0;

// Initialize Charmap Mode
void InitCharmap() 
{	
#if defined __LYNX__
	InitDisplay();
#endif		
}

// Switch from Text mode to Charmap mode
void EnterCharmapMode()
{		
#if defined __ATARI__	
	// Setup Charmap DLIST
	CharmapDLIST();

	// Page to character set
	POKE(0x02f4, 0xa0);	
	
	// DLI parameters
	charmapDLI = 1; StartDLI();
	charsetPage1 = 0xa0;
	charsetPage2 = 0xa4;
	
#elif defined __CBM__	
	// Setup VIC2 (memory bank and multicolor mode)
	SetupVIC2();

#elif defined __LYNX__
	videoMode = MODE_CHARMAP;	
#endif	

	// Show Top-Left
	charmapX = 255;
}

// Switch from Charmap mode to Text mode
void ExitCharmapMode()
{
#if defined __CBM__	
	// Switch OFF multicolor mode
	ResetVIC2();
#endif	
}

// Load charset and associated attributes / flags
void LoadCharset(char* filename)
{
#if defined __ATARI__
	// Read Charset and Attributes
	if (FileOpen(filename)) {
		FileRead((char*)CHARSETRAM, 0x0800);
		FileRead((char*)CHARATRRAM, 0x0100);
	}
	
	// Set Colors
	POKE(0x02c8, 0x00);
	POKE(0x02c4, 0x0c);	
	POKE(0x02c5, 0x78);
	POKE(0x02c6, 0x62);
	POKE(0x02c7, 0x12);		
	
#elif defined __CBM__	
	// Read Charset and Attributes
	FILE* fp = fopen(filename, "rb");	
	fread((char*)CHARSETRAM, 1, 0x0800, fp);
	fread((char*)CHARATRRAM, 1, 0x0100, fp);
	fclose(fp);	
	
	// Set Colors
	POKE(0xd021, BLACK);	// BG Color
	POKE(0xd022, WHITE); 	// MC1
	POKE(0xd023, LBLUE);	// MC2	
#endif			
}

// Clear entire screen
void ClearCharmap()
{
#if defined __ATARI__
	bzero((char*)SCREENRAM, 1000);
	
#elif defined __CBM__
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)COLORRAM,  1000);

#elif defined __LYNX__
	bzero((char*)SCREENRAM, 680);

#endif
}

// Load charmap from file
void LoadCharmap(char *filename) 
{
#if defined __ATARI__
	if (FileOpen(filename))
		FileRead((char*)CHARMAPRAM, 0x2000);
	
#elif defined __CBM__	
	// Read Char Map
	FILE* fp = fopen(filename, "rb");	
	fread((char*)CHARMAPRAM, 1, 0x2000, fp);
	fclose(fp);
	
#elif defined __LYNX__
	FileRead(filename);	
#endif
	// Set Dimensions
	charmapWidth = 96;
	charmapHeight = 64;
}

unsigned char GetCharFlags(unsigned char x, unsigned char y)
{
	// Get flags of specified tile
	unsigned char chr;
	chr = PEEK(CHARMAPRAM + charmapWidth*y + x);
#if defined __ATARI__
	chr = PEEK(CHARFLGRAM + chr);
#elif defined __CBM__
	rom_disable();
	chr = PEEK(CHARFLGRAM + chr);
	rom_enable();	
#elif defined __LYNX__
	chr = charFlags[chr];
#endif
	return chr;
}

void ScrollCharmap(unsigned char x, unsigned char y)
{
	unsigned int src, scr, col;
	unsigned char i, j, chr, att;
	
	// Check if moved?
	if (x == charmapX && y == charmapY)
		return;
	charmapX = x;
	charmapY = y;
		
#if defined __ATARI__	
	// Copy Map Section
	src = CHARMAPRAM + charmapWidth*y + x;
	scr = SCREENRAM;
	for (j=0; j<scrollHeight; j++) {
		for (i=0; i<scrollWidth; i++) {
			chr = PEEK(src+i);
			att = PEEK(CHARATRRAM+chr);
			POKE(scr++, chr+att);
		}
		src += charmapWidth;
	}
	
#elif defined __CBM__	
	// Switch Frame
	//charmapFrame ^= 1;

	// Copy Map Section
	src = CHARMAPRAM + charmapWidth*y + x;
	scr = SCREENRAM + 0x0400*charmapFrame;
	col = COLORRAM;
	for (j=0; j<scrollHeight; j++) {
		for (i=0; i<scrollWidth; i++) {
			chr = PEEK(src+i);
			POKE(scr++, chr);
			POKE(col++, PEEK(CHARATRRAM+chr));
		}
		src += charmapWidth;
	}
	
	// Update frame address (and sprite pointers)
	//POKE(0xD018, charmapFrame*16 + BITMAPLOC);
	//memcpy(SPRITEPTR + 0x0400*charmapFrame, spritePtr, 8);
	//spritePtr = SPRITEPTR + 0x0400*charmapFrame;
	
#elif defined __LYNX__	
	// Copy Map Section
	src = CHARMAPRAM + charmapWidth*y + x;
	scr = SCREENRAM;
	for (j=0; j<scrollHeight; j++) {
		for (i=0; i<scrollWidth; i++) {
			chr = PEEK(src+i);
			POKE(scr++, chr);
		}
		src += charmapWidth;
	}
	
#endif
}

void PrintCharmap(unsigned char x, unsigned char y, unsigned char* str)
{
	unsigned char i, chr;
	unsigned int addr1;
#if (defined __ATARI__)
	addr1 = SCREENRAM + 40*y + x;
	for (i=0; i<strlen(str); i++) {
		if (str[i] > 96) 
			chr = str[i];		// Lower case
		else
			chr = str[i]+32;	// Upper case
		POKE(addr1++, chr);
	}
	
#elif defined __CBM__	
	unsigned int addr2, addr3;
	addr1 = SCREENRAM + 40*y + x;
	addr2 = SCREENRAM + 0x0400 + 40*y + x;
	addr3 = COLORRAM + 40*y + x;
	for (i=0; i<strlen(str); i++) {
		if (str[i] > 192)
			chr = str[i]+32;	// Upper case (petscii)
		else if (str[i] > 96) 
			chr = str[i]+128;	// Lower case (ascii)
		else
			chr = str[i]+160;	// Lower case (petscii)
		POKE(addr1++, chr);
		POKE(addr2++, chr);
		POKE(addr3++, inkColor);
	}
	
#elif defined __LYNX__
	addr1 = SCREENRAM + 40*y + x;
	for (i=0; i<strlen(str); i++) {
		if (str[i] > 96) 
			chr = str[i]+128;	// Lower case
		else
			chr = str[i]+160;	// Upper case
		POKE(addr1++, chr);
	}
#endif
}

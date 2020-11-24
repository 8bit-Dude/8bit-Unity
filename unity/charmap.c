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

// Zero Page pointers (tile decoding and map scrolling)
#if defined(__APPLE2__)
  #define tilesetDataZP 0xef
  #define charPointerZP 0xce
  #define row1PointerZP 0xfb
  #define row2PointerZP 0xfd
#elif defined(__ATARI__)
  #define tilesetDataZP 0xe0
  #define charattDataZP 0xe0
  #define charPointerZP 0xe2
  #define scrPointerZP  0xe4
  #define row1PointerZP 0xe4
  #define row2PointerZP 0xe6
#elif defined(__CBM__)
  #define tilesetDataZP 0x61
  #define charattDataZP 0x61
  #define charPointerZP 0x63
  #define scrPointerZP  0xfb
  #define colPointerZP  0xfd
  #define row1PointerZP 0xfb
  #define row2PointerZP 0xfd
#elif defined(__LYNX__)
  #define tilesetDataZP 0xb3
  #define charPointerZP 0xb5
  #define scrPointerZP  0xb7
  #define row1PointerZP 0xb7
  #define row2PointerZP 0xb9
#elif defined(__ORIC__)
  #define tilesetDataZP 0xef
  #define charPointerZP 0xb5
  #define scrPointerZP  0xb7
  #define row1PointerZP 0xb7
  #define row2PointerZP 0xb9
#endif

// Assembly function (see tiles.s)
void __fastcall__ DecodeTiles(void);

// Map size and location properties
unsigned char charmapWidth, charmapHeight, tileWidth, tileHeight, worldWidth, worldHeight;
unsigned char screenCol1 = 0, screenCol2 = CHR_COLS, screenWidth = CHR_COLS;
unsigned char screenRow1 = 0, screenRow2 = CHR_ROWS, screenHeight = CHR_ROWS;

// Scrolling properties
unsigned char blockWidth, tileCols, tileRows;
unsigned char decodeWidth, decodeHeight, *decodeScreen;
  
// Pointers to various data sets
unsigned char *charmapData;
unsigned char *charsetData;
unsigned char *charflagData;
#if defined __CBM__	
  unsigned char *charattData;
#endif
unsigned char *tilesetData;

#if (defined __APPLE2__) || (defined __ORIC__)
  extern unsigned char sprDrawn[];
#endif
  
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
	// Setup Charmap Page and DLIST
	POKE(0x02f4, 0xa0);	
	CharmapDLIST();
	
	// Setup DLI
	StartDLI();
	waitvsync();
	charmapDLI = 1;
	
	// ANTIC: DMA Screen
	POKE(559, PEEK(559)|32);	
	
#elif defined __CBM__	
	// Setup VIC2 (memory bank and multicolor mode)
	SetupVIC2();	
#endif	

	// Set Video Mode
	videoMode = CHR_MODE;
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
	videoMode = TXT_MODE;
}

// Clear entire screen
void ClearCharmap()
{
#if (defined __APPLE2__) || (defined __LYNX__) || (defined __ORIC__)
	ClearBitmap();

#elif defined __ATARI__
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)0x8e10, 320);
	
#elif defined __CBM__
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)COLORRAM,  1000);	
#endif
}

void DisplayCharmap(unsigned char c1, unsigned char c2, unsigned char r1, unsigned char r2)
{
	// Define rendering window
	screenCol1 = c1; screenCol2 = c2;
	screenRow1 = r1; screenRow2 = r2;
	screenWidth = c2-c1; screenHeight = r2-r1;	
}

// Load charset and associated attributes / flags
void LoadCharset(char* filename, char* palette)
{
#if defined __APPLE2__
	FILE* fp = fopen(filename, "rb");
  #if defined __DHR__	
	if (!charsetData)
		charsetData = malloc(0x1080);
	fread((char*)charsetData, 1, 0x1080, fp);
	charflagData = (char*)(charsetData+0x1000);
  #else	
	if (!charsetData)
		charsetData = malloc(0x0880);
	fread((char*)charsetData, 1, 0x0880, fp);
	charflagData = (char*)(charsetData+0x0800);
  #endif
	fclose(fp);
	
#elif defined __ATARI__
	if (FileOpen(filename)) {
		FileRead((char*)CHARSETRAM, 0x0400);
		FileRead((char*)CHARATRRAM, 0x0100);
		charflagData = (char*)(CHARATRRAM+0x80);
	}
	
	// Set palette colors (WARNING: 5th color shared with sprite 5!)
	memcpy(chrPalette, palette, 5);
	
#elif defined __CBM__	
	FILE* fp = fopen(filename, "rb");	
	fread((char*)CHARSETRAM, 1, 0x0800, fp);
	if (!charattData)
		charattData = malloc(0x0100);
	fread((char*)charattData, 1, 0x0100, fp);
	charflagData = (char*)(charattData+0x80);
	fclose(fp);	
	
	// Set palette colors
	POKE(0xd021, palette[0]); // BG Color
	POKE(0xd022, palette[1]); // MC1
	POKE(0xd023, palette[2]); // MC2	

#elif defined __LYNX__
	if (!charsetData)
		charsetData = malloc(0x0680);
	if (FileRead(filename))
		memcpy(charsetData, BITMAPRAM, 0x0680);
	charflagData = (char*)(charsetData+0x0600);
	ClearBitmap();
	
#elif defined __ORIC__
	if (!charsetData)
		charsetData = malloc(0x0480);
	FileRead(filename, charsetData);
	charflagData = (char*)(charsetData+0x0400);
#endif			
}

// Load charmap from file
void LoadCharmap(char *filename, unsigned int w, unsigned int h) 
{
#if (defined __APPLE2__) || (defined __CBM__)
	FILE* fp;
#endif
	unsigned int size = w*h;
	
	// Update Dimensions of World Map
	charmapWidth = w; 
	charmapHeight = h;	
	worldWidth = w*tileHeight;
	worldHeight = h*tileWidth;
	
	// Assign memory
#if defined  __ATARI__
	charmapData = (char*)CHARMAPRAM;
#else
	if (charmapData) free(charmapData);	
	charmapData = malloc(size);
#endif	
	
	// Load data from file
#if (defined __APPLE2__) || (defined __CBM__)
	fp = fopen(filename, "rb");	
	fread(charmapData, 1, size, fp);
	fclose(fp);

#elif defined __ATARI__
	if (FileOpen(filename))
		FileRead(charmapData, size);
	
#elif defined __LYNX__
	if (FileRead(filename))
		memcpy(charmapData, BITMAPRAM, size);
	ClearBitmap();
	
#elif defined __ORIC__
	FileRead(filename, charmapData);	
#endif
}

// Load tileset from file
void LoadTileset(char *filename, unsigned int n, unsigned int w, unsigned int h) 
{
#if (defined __APPLE2__) || (defined __CBM__)
	FILE* fp;
#endif	
	unsigned int size = n*w*h;
	tileWidth = w; tileHeight = h;
	
	// Assign memory	
	if (tilesetData) free(tilesetData);	
	tilesetData = malloc(size);
	
#if (defined __APPLE2__) || (defined __CBM__)
	fp = fopen(filename, "rb");	
	fread(tilesetData, 1, size, fp);
	fclose(fp);
	
#elif defined __ATARI__
	if (FileOpen(filename))
		FileRead(tilesetData, size);
	
#elif defined __LYNX__
	if (FileRead(filename))
		memcpy(tilesetData, BITMAPRAM, size);
	ClearBitmap();
	
#elif defined __ORIC__
	FileRead(filename, tilesetData);	
#endif

	// Allocate buffer for tile to char conversion
	decodeHeight = screenHeight+tileHeight;
	decodeWidth = screenWidth+tileWidth;
	tileRows = decodeHeight/tileHeight;
	tileCols = decodeWidth/tileWidth;
	if (decodeScreen) free(decodeScreen);	
	decodeScreen = malloc(decodeWidth*decodeHeight);
}

unsigned char GetFlag(unsigned char x, unsigned char y)
{
	// Get flags of specified tile
	unsigned char chr;
	if (tilesetData) {
		chr = charmapData[charmapWidth*(y/2u) + x/2u];
		chr = tilesetData[4*chr+(2*(y%2))+x%2];
	} else {
		chr = charmapData[charmapWidth*y + x];
	}
	return charflagData[chr];
}

unsigned char GetTile(unsigned char x, unsigned char y)
{
	return charmapData[charmapWidth*(y/2u) + x/2u];	
}

void SetTile(unsigned char x, unsigned char y, unsigned char tile)
{
	charmapData[charmapWidth*(y/2u) + x/2u] = tile;	
}

void SetChar(unsigned char x, unsigned char y, unsigned char chr)
{
	charmapData[charmapWidth*y + x] = chr;
}

void ScrollCharmap(unsigned char x, unsigned char y)
{
	unsigned char i;
	unsigned int src, dst, col;
	
	// Using tileset?
	if (tilesetData) {
		// Decode tilemap to screen buffer
		POKEW(tilesetDataZP, tilesetData);
		POKEW(charPointerZP, &charmapData[charmapWidth*(y/2u) + x/2u]);
		POKEW(row1PointerZP, &decodeScreen[0]);
		POKEW(row2PointerZP, &decodeScreen[screenWidth+tileWidth]);	
		blockWidth = 2*decodeWidth;
		DecodeTiles();
		
		// Assign offset area of screen buffer
		src = (char*)&decodeScreen[(screenWidth+tileWidth)*(y%2)+x%2];
		blockWidth = screenWidth+tileWidth;
	} else {
		// Point directly to charmap data
		src = &charmapData[charmapWidth*y + x];
		blockWidth = charmapWidth;
	}
	
#if defined __APPLE2__
	// Reset sprite background
	for (i=0; i<SPRITE_NUM; i++)
		sprDrawn[i] = 0;
	POKEW(charPointerZP, src);
  #if defined __DHR__
	ScrollDHR();
  #else
	ScrollSHR();
  #endif
	clk += 20;

#elif defined __ATARI__	
	dst = SCREENRAM + screenRow1*40 + screenCol1;
	POKEW(charattDataZP, CHARATRRAM);
	POKEW(charPointerZP, src);
	POKEW(scrPointerZP, dst);
	Scroll();
	
#elif defined __CBM__	
	dst = SCREENRAM + screenRow1*40 + screenCol1;
	col = COLORRAM + screenRow1*40 + screenCol1;
	POKEW(charattDataZP, charattData);
	POKEW(charPointerZP, src);
	POKEW(scrPointerZP, dst);
	POKEW(colPointerZP, col);
	Scroll();
		
#elif defined __LYNX__	
	dst = BITMAPRAM + screenRow1*6*82 + screenCol1*2 + 1;
	POKEW(charPointerZP, src);
	POKEW(scrPointerZP, dst);
	Scroll();	
	
#elif defined __ORIC__	
	// Reset sprite background
	for (i=0; i<SPRITE_NUM; i++)
		sprDrawn[i] = 0;
	dst = BITMAPRAM + screenRow1*320 + screenCol1 + 1;
	POKEW(charPointerZP, src);
	POKEW(scrPointerZP, dst);
	Scroll();
#endif
}

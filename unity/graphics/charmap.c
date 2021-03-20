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

#ifdef __CBM__
  extern unsigned char videoMode;
#endif

// Zero Page pointers (for tile decoding and scrolling)
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
  #define tilesetDataZP 0xb0
  #define charPointerZP 0xb2
  #define scrPointerZP  0xb4
  #define row1PointerZP 0xb4
  #define row2PointerZP 0xb6 
#endif

// Byte size of screen elements
#if defined(__LYNX__)
  #define CHR_WIDTH        2
  #define CHR_HEIGHT       4
  #define LINE_SIZE	  	  82
  #define ROW_SIZE		 328  
#else
  #define CHR_WIDTH        1
  #define CHR_HEIGHT       8
  #define LINE_SIZE	  	  40	
  #define ROW_SIZE		 320 
#endif

// Assembly functions (see tiles.s and scroll.s)
void __fastcall__ DecodeTiles2x2(void);
void __fastcall__ Scroll(void);

// Map size and location properties
unsigned char charmapWidth, charmapHeight, tileWidth, tileHeight;
unsigned char worldWidth, worldHeight, worldX, worldY, worldMaxX, worldMaxY;
unsigned char screenCol1 = 0, screenCol2 = CMP_COLS, screenWidth = CMP_COLS;
unsigned char screenRow1 = 0, screenRow2 = CMP_ROWS, screenHeight = CMP_ROWS;
unsigned char lineBlock;
#if defined __ATARI__	
	unsigned char chrRows, bmpRows;
	unsigned int bmpAddr;
#endif

// Drawing properties
unsigned char blockWidth, decodeWidth, decodeHeight, *decodeData;
unsigned char tileX, tileY, tileCols, tileRows;
unsigned char scrollCols, scrollRows, scrollDirX, scrollDirY, scrollLine;
  
// Pointers to various data sets
unsigned char *charmapData;
unsigned char *charsetData;
unsigned char *charflagData;
unsigned char *tilesetData;
unsigned char *screenData;
#if defined __CBM__	
  unsigned char *charattData;
  unsigned char *colorData;
#endif

// Helper function for soft sprites
#if (defined __APPLE2__) || (defined __ORIC__)
  extern unsigned char sprDrawn[];
  void RestoreSprBG(unsigned char index);
  void HideSprites(void) {
	unsigned char i;
	for (i=0; i<SPRITE_NUM; i++)
		if (sprDrawn[i]) {			
			RestoreSprBG(i);
			sprDrawn[i] = 0;
		}
  }
#endif
  
// Initialize Charmap Mode
void InitCharmap(unsigned char col1, unsigned char col2, unsigned char row1, unsigned char row2) 
{
	// Define rendering window
	screenCol1 = col1; screenCol2 = col2;
	screenRow1 = row1; screenRow2 = row2;
	screenWidth = col2-col1; screenHeight = row2-row1;	
	lineBlock = screenWidth * CHR_WIDTH;
#if (defined __ATARI__) || (defined __CBM__)
	screenData = (char*)(SCREENRAM + screenRow1*LINE_SIZE + screenCol1*CHR_WIDTH);
#elif (defined __LYNX__) || (defined __ORIC__)
	screenData = (char*)(BITMAPRAM+1 + screenRow1*ROW_SIZE + screenCol1*CHR_WIDTH);
#endif	
	
#if (defined __APPLE2__) || (defined __LYNX__) || (defined __ORIC__)
	InitBitmap();

#elif defined __ATARI__	
	// Charmap/Bitmap transition params
	chrRows = row2-1;
	bmpRows = 3 + chrRows + 8*(CMP_ROWS-row2);
	bmpAddr = BITMAPRAM1 + row2*8*40;
	InitBitmap();
		
#elif defined __CBM__
	colorData = COLORRAM + screenRow1*LINE_SIZE + screenCol1;

#endif
}

// Switch from Text mode to Charmap mode
void ShowCharmap()
{
#if (defined __APPLE2__)
	ShowBitmap();
	
#elif defined __ATARI__	
	// Setup Charmap Page and DLIST
	POKE(0x02f4, 0xa0);	
	CharmapDLIST();
	
	// Start Graphics DLI
	StartDLI();

	// Enable Charmap DLI for split screens
	if (chrRows < CMP_ROWS) {
		waitvsync();
		charmapDLI = 1;
	}

	// ANTIC: DMA Screen
	POKE(559, PEEK(559)|32);	
	
#elif defined __CBM__	
	// Setup VIC2 (memory bank and multicolor mode)
	videoMode = CHR_MODE;
	SetupVIC2();	
#endif	
}

// Switch back to Text mode
void HideCharmap()
{
#if defined __APPLE2__	
	HideBitmap();
	
#elif defined __ATARI__	
	// Disable Charmap DLI for split screens
	if (chrRows < CMP_ROWS) {
		waitvsync();
		charmapDLI = 0;
	}
	
    // Switch off screen DMA
	POKE(559, PEEK(559)&~32);
	
#elif defined __CBM__	
	videoMode = TXT_MODE;
	ResetVIC2();
#endif	
}

// Clear entire screen
void ClearCharmap()
{
#if (defined __APPLE2__) || (defined __LYNX__) || (defined __ORIC__)
	ClearBitmap();

#elif defined __ATARI__
	ClearBitmap();
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)0x8e10, 320);
	
#elif defined __CBM__
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)COLORRAM,  1000);	
#endif
}

// Load charset and associated attributes / flags
void LoadCharset(char* filename, char* palette)
{
#if defined __APPLE2__
	if (FileOpen(filename)) {
	  #if defined __DHR__	
		if (!charsetData)
			charsetData = malloc(0x1080);
		FileRead((char*)charsetData, 0x1080);
		charflagData = (char*)(charsetData+0x1000);
	  #else	
		if (!charsetData)
			charsetData = malloc(0x0880);
		FileRead((char*)charsetData, 0x0880);
		charflagData = (char*)(charsetData+0x0800);
	  #endif		
		FileClose();
	}
	
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
		charsetData = malloc(0x0480);
	if (FileRead(filename))
		memcpy(charsetData, BITMAPRAM, 0x0480);
	charflagData = (char*)(charsetData+0x0400);
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
#if (defined __CBM__)
	FILE* fp;
#endif
	unsigned int size = w*h;
	
	// Update Dimensions of World Map
	charmapWidth = w; 
	charmapHeight = h;	
	worldWidth = w*tileHeight;
	worldHeight = h*tileWidth;
	
	// Compute max World Map coordinates
	worldMaxX = worldWidth-screenWidth;
	worldMaxY = worldHeight-screenHeight;	
	
	// Assign memory
#if defined  __ATARI__
	charmapData = (char*)CHARMAPRAM;
#else
	if (charmapData) free(charmapData);	
	charmapData = malloc(size);
#endif	
	
	// Load data from file
#if defined __APPLE2__
	if (FileOpen(filename)) {
		FileRead(charmapData, size);
		FileClose();
	}

#elif defined __ATARI__
	if (FileOpen(filename))
		FileRead(charmapData, size);

#elif defined __CBM__
	fp = fopen(filename, "rb");	
	fread(charmapData, 1, size, fp);
	fclose(fp);
	
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
#if (defined __CBM__)
	FILE* fp;
#endif	
	unsigned int size = n*w*h;
	tileWidth = w; tileHeight = h;
	
	// Assign memory and ZP pointer
	if (tilesetData) free(tilesetData);	
	tilesetData = malloc(size);
	
#if (defined __APPLE2__)
	if (FileOpen(filename)) {
		FileRead(tilesetData, size);
		FileClose();
	}
	
#elif defined __ATARI__
	if (FileOpen(filename))
		FileRead(tilesetData, size);

#elif (defined __CBM__)
	fp = fopen(filename, "rb");	
	fread(tilesetData, 1, size, fp);
	fclose(fp);
		
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
	if (decodeData) free(decodeData);	
	decodeData = malloc(decodeWidth*decodeHeight);
}

void FreeCharmap()
{
#ifndef __ATARI__
	if (charmapData) free(charmapData);	
	if (charsetData) free(charsetData);
#endif	
	if (tilesetData) free(tilesetData);	
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
	tileX = 255; tileY = 255;	// Trick to force re-decoding
}

void PrintCharmap(unsigned char x, unsigned char y, unsigned char chr)
{
#if defined(__ATARI__)
	POKE((char*)SCREENRAM+y*40+x, chr);
#elif defined(__LYNX__)
	unsigned char i;
	unsigned int src, dst;
	src = (char*)charsetData + 2*chr;
	dst = BITMAPRAM + y*ROW_SIZE + x*CHR_WIDTH + 1;
	for (i = 0; i<4; i++) {
		memcpy((char*)dst, (char*)src, 2);
		src += 256; dst += 82;
	}
#endif
}

unsigned char *DecodeTiles(unsigned char x, unsigned char y)
{
	// Decode tilemap to screen buffer
	if (tileX != x/2u || tileY != y/2u) {
		tileX = x/2u; tileY = y/2u;
		POKEW(tilesetDataZP, tilesetData);
		POKEW(charPointerZP, &charmapData[charmapWidth*tileY + tileX]);
		POKEW(row1PointerZP, &decodeData[0]);
		POKEW(row2PointerZP, &decodeData[decodeWidth]);	
		blockWidth = 2*decodeWidth;
		DecodeTiles2x2();
	}
	
	// Assign offset area of screen buffer
	return &decodeData[decodeWidth*(y%2)+x%2];	
}

void ScrollCharmap(unsigned char x, unsigned char y)
{
#if defined(__APPLE2__) || defined(__ATARI__) || defined(__CBM__) || defined(__LYNX__) || defined(__ORIC__)	
	DrawCharmap(x,y);
#else
	unsigned char i, tmp;
	signed char stepX, stepY;
	unsigned int src, srcOff, dstOff;
	unsigned int cpyDst, cpySrc;

  #if defined(__APPLE2__) || defined(__ORIC__)
	HideSprites();
  #endif
	
	// Using tileset?
	if (tilesetData) {
		src = DecodeTiles(x, y);
		blockWidth = decodeWidth;
	} else {
		src = &charmapData[charmapWidth*y + x];
		blockWidth = charmapWidth;
	}
	
	// Compute step
	stepX = x - worldX; 
	stepY = y - worldY;
	worldX = x; worldY = y;
	
	// Init copy addresses
	cpySrc = cpyDst = screenData;
	scrollCols = lineBlock;
	scrollLine = LINE_SIZE;
	scrollDirX = 1;
	scrollDirY = 1;
	
	// Determine screen area
	if (stepY) {
		if (stepY < 0) {
			cpyDst += screenHeight*ROW_SIZE - LINE_SIZE;
			cpySrc = cpyDst;
			scrollDirY = -1;
		}
		cpySrc += stepY*ROW_SIZE;
	}
	if (stepX) {
		if (stepX < 0) {
			cpyDst += (-stepX)*CHR_WIDTH;
			scrollDirX = -1;
		} else {
			cpySrc += stepX*CHR_WIDTH;
		}
		scrollCols -= ABS(stepX)*CHR_WIDTH;
	}
	
	// Copy screen area
	scrollRows = (screenHeight-ABS(stepY))*CHR_HEIGHT;
	POKEW(row1PointerZP, cpySrc);
	POKEW(row2PointerZP, cpyDst);
	Scroll();
		
	// Blit new areas
	if (stepY) {
		if (stepY > 0) {
			srcOff = (screenHeight-stepY);
			dstOff = srcOff*ROW_SIZE;
			srcOff *= blockWidth;
		} else {
			srcOff = 0;
			dstOff = 0;
		}
		tmp = screenHeight;
		screenHeight = ABS(stepY);
		POKEW(charPointerZP, src+srcOff);
		POKEW(scrPointerZP, screenData+dstOff);
		BlitCharmap();			
		screenHeight = tmp;
	}
	if (stepX) {
		if (stepX > 0) {
			srcOff = (screenWidth-stepX);
			dstOff = srcOff*CHR_WIDTH;
		} else {
			srcOff = 0;
			dstOff = 0;
		}
		tmp = screenWidth;
		screenWidth = ABS(stepX);
		POKEW(charPointerZP, src+srcOff);
		POKEW(scrPointerZP, screenData+dstOff);
		BlitCharmap();			
		screenWidth = tmp;
	}		
#endif
}
		
void DrawCharmap(unsigned char x, unsigned char y)
{
	unsigned int src;
#if defined(__APPLE2__)
	x = 2*(x/2u)+1;
#endif
#if defined(__APPLE2__) || defined(__ORIC__)
	HideSprites();
#endif
	
	// Using tileset?
	if (tilesetData) {
		src = DecodeTiles(x, y);
		blockWidth = decodeWidth;
	} else {
		src = &charmapData[charmapWidth*y + x];
		blockWidth = charmapWidth;
	}

	// Save new coordinates (for scrolling)
	worldX = x; worldY = y;
	
#if defined __APPLE2__
	POKEW(charPointerZP, src);
  #if defined __DHR__
	BlitCharmapDHR(); clk += 20;
  #else
	BlitCharmapSHR(); clk += 10;
  #endif
#else
	POKEW(charPointerZP, src);
	POKEW(scrPointerZP, screenData);
  #if defined __ATARI__
	POKEW(charattDataZP, CHARATRRAM);
  #elif defined __CBM__	
	POKEW(charattDataZP, charattData);
	POKEW(colPointerZP, colorData);
  #endif
	BlitCharmap();
#endif
}

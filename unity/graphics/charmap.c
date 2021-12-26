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

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

// Zero Page pointers (for tile decoding and scrolling)
#if defined(__APPLE2__)
  #define charPtrZP 0xce
  #define decPtr1ZP 0xfb
  #define decPtr2ZP 0xfd
  #define scrPtr1ZP 0xfb
  #define scrPtr2ZP 0xfd
  #define scrRow1ZP 0xfb
  #define scrCol1ZP 0xfc
  #define scrRow2ZP 0xfd
  #define scrCol2ZP 0xfe  
#elif defined(__ATARI__)
  #define charatrDataZP 0xe0
  #define charPtrZP 0xe2
  #define decPtr1ZP 0xe4
  #define decPtr2ZP 0xe6
  #define scrPtrZP  0xe4
  #define scrPtr1ZP 0xe4
  #define scrPtr2ZP 0xe6
#elif defined(__CBM__)
  #define charatrDataZP 0x61
  #define charPtrZP 0x63
  #define scrPtrZP  0xfb
  #define colPtrZP  0xfd
  #define decPtr1ZP 0xfb
  #define decPtr2ZP 0xfd
  #define scrPtr1ZP 0x61
  #define scrPtr2ZP 0x63
  #define clrPtr1ZP 0xfb
  #define clrPtr2ZP 0xfd
#elif defined(__LYNX__)
  #define charPtrZP 0xb5
  #define decPtr1ZP 0xb7
  #define decPtr2ZP 0xb9
  #define scrPtrZP  0xb7
  #define scrPtr1ZP 0xb7
  #define scrPtr2ZP 0xb9
#elif defined(__NES__)
  #define charPtrZP 0xf0
  #define decPtr1ZP 0xf2
  #define decPtr2ZP 0xf4 
#elif defined(__ORIC__)
  #define charPtrZP 0xb2
  #define decPtr1ZP 0xb4
  #define decPtr2ZP 0xb6 
  #define scrPtrZP  0xb4
  #define scrPtr1ZP 0xb4
  #define scrPtr2ZP 0xb6 
#endif

// Byte size of screen elements
#if defined(__APPLE2__) 
  #define CHAR_WIDTH       1u
  #define CHAR_HEIGHT	   8u
  #define LINE_SIZE	  	  40u
  #define ROW_SIZE		 320u   
#elif defined(__ATARI__) || defined(__CBM__)
  #define CHAR_WIDTH       1u
  #define CHAR_HEIGHT      1u
  #define LINE_SIZE	  	  40u
  #define ROW_SIZE		  40u 
#elif defined(__LYNX__)
  #define CHAR_WIDTH       2u
  #define CHAR_HEIGHT      4u
  #define LINE_SIZE	  	  82u
  #define ROW_SIZE		 328u 
#elif defined(__NES__)
  #define CHAR_WIDTH       1u
  #define CHAR_HEIGHT      1u
  #define LINE_SIZE	  	  32u	
  #define ROW_SIZE		  32u
#elif defined(__ORIC__)
  #define CHAR_WIDTH       1u
  #define CHAR_HEIGHT      8u
  #define LINE_SIZE	  	  40u	
  #define ROW_SIZE		 320u
#endif

// Tile size
#if defined(__TILE_NONE__)
  #define TILE_WIDTH  1u
  #define TILE_HEIGHT 1u
#elif defined(__TILE_2X2__)
  #define TILE_WIDTH  2u
  #define TILE_HEIGHT 2u
#elif defined(__TILE_3X3__)
  #define TILE_WIDTH  3u
  #define TILE_HEIGHT 3u
#elif defined(__TILE_4X4__)
  #define TILE_WIDTH  4u
  #define TILE_HEIGHT 4u
#endif

// Assembly functions (see tiles.s and scroll.s)
void __fastcall__ DecodeTiles2x2(void);
void __fastcall__ Scroll(void);

#if defined __NES__
 #pragma bss-name(push, "XRAM")
#endif

// Map size and location properties
unsigned char charmapWidth, charmapHeight;
unsigned char worldWidth, worldHeight, worldX, worldY, worldMaxX, worldMaxY;
unsigned char screenCol1 = 0, screenCol2 = CMP_COLS, screenWidth = CMP_COLS;
unsigned char screenRow1 = 0, screenRow2 = CMP_ROWS, screenHeight = CMP_ROWS;
unsigned char lineBlock;

// Decoding properties
unsigned char blockWidth, lineWidth, decodeWidth, decodeHeight;
unsigned char tileX, tileY, tileCols, tileRows;
unsigned char scrollCols, scrollRows, scrollDirX, scrollDirY;
  
// Pointers to various data sets
#if defined(__APPLE2__)
  unsigned char *charmapData;
 #if defined __DHR__	
  unsigned char  charset[0x1080];
  unsigned char *charsetData = (char*)(charset+0x0000);
  unsigned char *charflagData = (char*)(charset+0x1000);
 #else
  unsigned char  charset[0x0880];
  unsigned char *charsetData = (char*)(charset+0x0000);
  unsigned char *charflagData = (char*)(charset+0x0800);
 #endif
#elif defined(__ATARI__) 
  unsigned char *charmapData = (char*)(CHARMAPRAM);
  unsigned char *charatrData = (char*)(CHARATRRAM);
  unsigned char *charflagData = (char*)(CHARATRRAM+0x80);
  unsigned char *charsetData = (char*)(CHARSETRAM);
#elif defined(__CBM__)   
  unsigned char *charmapData;
  unsigned char  charatr[0x0100];			
  unsigned char *charatrData = (char*)(charatr+0x0000);
  unsigned char *charflagData = (char*)(charatr+0x0080);
  unsigned char *charsetData = (char*)(CHARSETRAM);
  unsigned char *colorData;
#elif defined(__LYNX__)
  unsigned char *charmapData;
  unsigned char  charset[0x0480];
  unsigned char *charsetData = (char*)(charset+0x0000);
  unsigned char *charflagData = (char*)(charset+0x0400);
#elif defined(__NES__)
  unsigned char  charmapData[CHARMAPRAM];
  unsigned char  charflagData[CHARFLAGRAM];  
#elif defined(__ORIC__)
  unsigned char *charmapData;
  unsigned char  charset[0x0480];
  unsigned char *charsetData = (char*)(charset+0x0000);
  unsigned char *charflagData = (char*)(charset+0x0400);
#endif

#if defined(__NES__)
  unsigned char  decodeData[DECODERAM];  
  unsigned char  tileset[128*TILE_WIDTH*TILE_HEIGHT];  
  unsigned char *tilesetData = tileset;  
#else
  unsigned char *decodeData;
  unsigned char *screenData;
  unsigned char *tilesetData;
#endif

#if defined __NES__
 #pragma bss-name(pop)
#endif

// Initialize Charmap Mode
void InitCharmap(unsigned char col1, unsigned char col2, unsigned char row1, unsigned char row2) 
{
	// Define rendering window
	lineWidth = 40;
	screenCol1 = col1; screenCol2 = col2;
	screenRow1 = row1; screenRow2 = row2;
	screenWidth = col2-col1; screenHeight = row2-row1;	
	lineBlock = screenWidth * CHAR_WIDTH;
#if (defined __ATARI__) || (defined __CBM__)
	screenData = (char*)(SCREENRAM + screenRow1*LINE_SIZE + screenCol1*CHAR_WIDTH);
#elif (defined __LYNX__) || (defined __ORIC__)
	screenData = (char*)(BITMAPRAM+1 + screenRow1*ROW_SIZE + screenCol1*CHAR_WIDTH);
#endif

#if (defined __APPLE2__) || (defined __LYNX__) || (defined __ORIC__)
	InitBitmap();
#elif defined __ATARI__	
	// Init Graphic Mode
	InitBitmap();
  #ifdef __ATARIXL__
	doubleBuffer = 0;	// Disable Frame Blending
  #else
	POKE(0x02F4, 0xA0);	// Set Address of Char. Gen.
  #endif
  
	// Charmap/Bitmap transition params	
	chrRows = row2-1;
	bmpRows = chrRows + 8*(CMP_ROWS-row2) + 2;
	bmpAddr = BITMAPRAM1 + row2*(8*40);
#elif defined __CBM__
	// Charmap/Bitmap transition params
	rasterLine = 50 + row2*8;
	colorData  = (char*)(COLORRAM + screenRow1*LINE_SIZE + screenCol1*CHAR_WIDTH);
#endif
}

// Switch from Text mode to Charmap mode
void ShowCharmap()
{
#if (defined __APPLE2__)
	ShowBitmap();
	
#elif defined __ATARI__	
	// Set palette, DLIST and screen DMA
	SetPalette(chrPalette);
	CharmapDLIST(); 
	paletteDLI = 1;
	POKE(559, PEEK(559)|32);	
	
#elif defined __CBM__	
	SetupVIC2(); // Switch memory bank and multicolor mode	
	StartDLI();  // Setup raster line interrupt
#endif	
}

// Switch back to Text mode
void HideCharmap()
{
#if defined __APPLE2__	
	HideBitmap();
	
#elif defined __ATARI__	
	// Disable Screen DMA and Palette DLI
	POKE(559, PEEK(559)&~32);
	paletteDLI = 0;
	
#elif defined __CBM__	
	StopDLI();	  // Stop raster line interrupt
	ResetVIC2();  // Exit multicolor mode
#endif	
}

// Clear entire screen
void ClearCharmap()
{
#if (defined __APPLE2__) || (defined __LYNX__)  || (defined __NES__) || (defined __ORIC__)
	ClearBitmap();

#elif defined __ATARI__
	bzero((char*)SCREENRAM, 1000);
	
#elif defined __CBM__
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)COLORRAM,  1000);	
#endif
}

// Load charset and associated attributes / flags
#if defined __NES__
  extern unsigned char fileIndex;
#endif
void LoadCharset(char* filename)
{
#if defined __APPLE2__
	if (FileOpen(filename)) {
	  #if defined __DHR__	
		FileRead((char*)charsetData, 0x1080);
	  #else	
		FileRead((char*)charsetData, 0x0880);
	  #endif		
		FileClose();
	}
#elif defined __ATARI__
	if (FileOpen(filename)) {
		FileRead((char*)chrPalette,  0x0005);	// Shared Colors
		FileRead((char*)charsetData, 0x0400);	// Pixel Data
		FileRead((char*)charatrData, 0x0100);	// 5th Color Attribute (0 or 128)
	}
#elif defined __CBM__	
	if (FileOpen(filename)) {	
		FileRead((char*)0xd021, 0x0003);		// Shared Colors
		FileRead((char*)charsetData, 0x0400);	// Pixel Data
		FileRead((char*)charatrData, 0x0100);	// Color Attributes
		FileClose();		
	}
#elif defined __LYNX__
	if (FileOpen(filename)) {	// Data is loaded into BITMAP memory on open
		memcpy(charsetData, BITMAPRAM, 0x0480);
	}
#elif defined __NES__
	if (FileOpen(filename)) {
		// Copy palette data
		FileRead(charflagData, -1);
		memcpy(palBG, &charflagData[0x80], 4);
		
		// Switch to new palette/charset
		ppu_off();			
		pal_bg(palBG);
		set_chr_bank_0(2+fileIndex);
		ppu_on_all();
	}
#elif defined __ORIC__
	if (FileOpen(filename)) {
		FileRead(charsetData, 0x0480);
	}
#endif			
}

// Load charmap from file
void LoadCharmap(char *filename, unsigned int w, unsigned int h) 
{
	// Compute byte size of charmap
	unsigned int size = w*h;
	
	// Update Dimensions of World Map
	charmapWidth = w; 
	charmapHeight = h;	
	worldWidth = w*TILE_HEIGHT;
	worldHeight = h*TILE_WIDTH;
	
	// Compute max World Map coordinates
	worldMaxX = worldWidth-screenWidth;
	worldMaxY = worldHeight-screenHeight;	
	
	// Allocate memory
#if defined(__APPLE2__) || defined(__CBM__) || defined(__LYNX__) || defined(__ORIC__)
	charmapData = malloc(size);
#endif	
	
	// Load data from file
#if defined(__APPLE2__) || defined(__ATARI__) || defined(__CBM__) || defined(__NES__) || defined(__ORIC__)
	if (FileOpen(filename)) {
		FileRead(charmapData, size);
		FileClose();
	}
#elif defined(__LYNX__)
	if (FileOpen(filename)) {	// Data is immediately loaded into BITMAP memory on open
		memcpy(charmapData, (char*)BITMAPRAM, size);
	}
#endif
}

// Load tileset from file
void LoadTileset(char *filename, unsigned int n) 
{
	// Assign memory and ZP pointer
	unsigned int size = n*TILE_WIDTH*TILE_HEIGHT;	
#ifndef __NES__
	tilesetData = malloc(size);
#endif
	
#if defined(__APPLE2__) || defined(__ATARI__) || defined(__CBM__) || defined(__NES__) || defined(__ORIC__)
	if (FileOpen(filename)) {
		FileRead(tilesetData, size);
		FileClose();
	}	
#elif defined __LYNX__
	if (FileOpen(filename))	{ // Data is immediately loaded into BITMAP memory on open
		memcpy(tilesetData, (char*)BITMAPRAM, size);
	}
#endif
	
	// Allocate buffer for tile to char conversion
	decodeHeight = screenHeight+TILE_HEIGHT;
	decodeWidth  = screenWidth+TILE_WIDTH;
	tileRows = decodeHeight/TILE_HEIGHT;
	tileCols = decodeWidth/TILE_WIDTH;
#ifndef __NES__	
	decodeData = malloc(decodeWidth*decodeHeight);
#endif
}

void FreeCharmap()
{
#if defined(__APPLE2__) || defined(__CBM__) || defined(__LYNX__) || defined(__ORIC__)
	if (charmapData) free(charmapData);	
#endif		
#ifndef __NES__
	if (tilesetData) free(tilesetData);	
	if (decodeData) free(decodeData);	
#endif	
}

unsigned char GetFlag(unsigned char x, unsigned char y)
{
	// Get flags of specified tile
	unsigned char chr;
	chr = GetTile(x, y);
#if defined __TILE_2X2__
	chr = tilesetData[ 4*chr+(2*(y&1))+(x&1)];
#elif defined __TILE_3X3__
	chr = tilesetData[ 9*chr+(3*(y%3))+(x%3)];
#elif defined __TILE_4X4__
	chr = tilesetData[16*chr+(4*(y&3))+(x&3)];
#endif
	return charflagData[chr];
}

unsigned char GetTile(unsigned char x, unsigned char y)
{
#if defined __TILE_NONE__
	return charmapData[charmapWidth*y + x];	
#elif defined __TILE_2X2__
	return charmapData[charmapWidth*(y/2u) + x/2u];	
#elif defined __TILE_3X3__
	return charmapData[charmapWidth*(y/3u) + x/3u];	
#elif defined __TILE_4X4__
	return charmapData[charmapWidth*(y/3u) + x/3u];	
#endif
}

void SetTile(unsigned char x, unsigned char y, unsigned char tile)
{
#if defined __TILE_NONE__
	charmapData[charmapWidth*y + x] = tile;
#else	
  #if defined __TILE_2X2__
	charmapData[charmapWidth*(y/2u) + x/2u] = tile;
  #elif defined __TILE_3X3__
	charmapData[charmapWidth*(y/3u) + x/3u] = tile;
  #elif defined __TILE_4X4__
	charmapData[charmapWidth*(y/4u) + x/4u] = tile;
  #endif
	tileX = 255; tileY = 255;	// Trick to force re-decoding
#endif
}

void PrintCharmap(unsigned char x, unsigned char y, unsigned char chr)
{
#if defined(__ATARI__)
	POKE((char*)SCREENRAM+y*40+x, chr+charatrData[chr]);
#elif defined(__CBM__)
	unsigned int addr = y*40+x;
	POKE((char*)SCREENRAM+addr, chr);
	POKE((char*)COLORRAM+addr, charatrData[chr]);
#elif defined(__LYNX__)
	unsigned char *src, *dst;
	unsigned char i;
	src = &charsetData[2*chr];
	dst = (char*)(BITMAPRAM+1 + y*ROW_SIZE + x*CHAR_WIDTH);
	for (i = 0; i<4; i++) {
		memcpy(dst, src, 2);
		src += 256; dst += 82;
	}
#endif
}

unsigned int DecodeTiles()
{
#if defined __TILE_NONE__
	blockWidth = charmapWidth;
	return (unsigned int)&charmapData[charmapWidth*worldY + worldX];
#else
	// Decode tilemap to screen buffer
  #if defined __TILE_2X2__
	if (tileX != worldX/2u || tileY != worldY/2u) {
		tileX = worldX/2u; tileY = worldY/2u;
		POKEW(charPtrZP, (unsigned int)&charmapData[charmapWidth*tileY + tileX]);
		POKEW(decPtr1ZP, (unsigned int)&decodeData[0]);
		POKEW(decPtr2ZP, (unsigned int)&decodeData[decodeWidth]);	
		blockWidth = 2*decodeWidth;
		DecodeTiles2x2();
	}
  #endif	
	// Assign offset area of screen buffer
	blockWidth = decodeWidth;
	return (unsigned int)&decodeData[decodeWidth*(worldY&1)+(worldX&1)];	
#endif
}

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

void ScrollCharmap(unsigned char x, unsigned char y)
{
#ifdef __NES__
	DrawCharmap(x, y);
#else
	signed char stepX, stepY;
	unsigned int src, srcOff, dstOff;
	unsigned int cpyDst = 0, cpySrc = 0;
	unsigned char tmp1;
	
	// Platform specific handling
#if defined(__APPLE2__)
	unsigned char tmp2;
	x = 2*(x/2u)+1;
  #if defined(__DHR__)
	clk += 16;
  #else
	clk += 8;  
  #endif	
#endif

	// Compute step
	stepX = x - worldX; 
	stepY = y - worldY;
	worldX = x; worldY = y;
	
	// Decode tiles (if necessary)
	src = DecodeTiles();
	
	// Init copy parameters
	scrollCols = lineBlock;
	scrollDirX = 1;
	scrollDirY = 1;
	
	// Determine screen area
	if (stepY) {
		if (stepY < 0) {
			cpySrc = cpyDst = screenHeight*ROW_SIZE - LINE_SIZE;
			scrollDirY = -1;
		}
		cpySrc += stepY*ROW_SIZE;
	}
	if (stepX) {
		if (stepX < 0) {
			cpyDst += (-stepX)*CHAR_WIDTH;
			scrollDirX = -1;
		} else {
			cpySrc += stepX*CHAR_WIDTH;
		}
		scrollCols -= ABS(stepX)*CHAR_WIDTH;
	}

	// Hide soft sprites
#if defined(__APPLE2__) || defined(__ORIC__)
	HideSprites();
#endif
	
	// Scroll screen area
	scrollRows = (screenHeight-ABS(stepY))*CHAR_HEIGHT;
#if defined __APPLE2__	
	POKE(scrRow1ZP, screenRow1*CHAR_HEIGHT+(cpySrc/LINE_SIZE));
	POKE(scrRow2ZP, screenRow1*CHAR_HEIGHT+(cpyDst/LINE_SIZE));
	POKE(scrCol1ZP, screenCol1+(cpySrc%LINE_SIZE));
	POKE(scrCol2ZP, screenCol1+(cpyDst%LINE_SIZE));	
#else
	POKEW(scrPtr1ZP, screenData+cpySrc);
	POKEW(scrPtr2ZP, screenData+cpyDst);
  #if defined __CBM__		
	POKEW(clrPtr1ZP, colorData+cpySrc);
	POKEW(clrPtr2ZP, colorData+cpyDst);
  #endif
#endif  
	Scroll();
		
	// Blit new area on Top/Bottom
	if (stepY) {
		// Assign charmap pointer
		if (stepY > 0) {
			srcOff = (screenHeight-stepY);
			dstOff = srcOff*ROW_SIZE;
			srcOff *= blockWidth;
		} else {
			srcOff = 0;
			dstOff = 0;
		}
		POKEW(charPtrZP, src+srcOff);
		
		// Assign screen information
	#if defined __APPLE2__	
		tmp1 = screenRow1;
		tmp2 = screenRow2;
		screenRow1 += dstOff/ROW_SIZE;
		screenRow2 = screenRow1+ABS(stepY);
	#else
		tmp1 = screenHeight;
		screenHeight = ABS(stepY);
		POKEW(scrPtrZP, (unsigned int)screenData+dstOff);
	  #if defined(__ATARI__)
		POKEW(charatrDataZP, charatrData);
	  #elif defined __CBM__	
		POKEW(charatrDataZP, charatrData);
		POKEW(colPtrZP, (unsigned int)colorData+dstOff);
	  #endif
	#endif
		BlitCharmap();	

		// Restore screen information
	#if defined __APPLE2__	
		screenRow1 = tmp1;
		screenRow2 = tmp2;
	#else
		screenHeight = tmp1;
	#endif
	}
	
	// Blit new area on Left/Right
	if (stepX) {
		// Assign charmap pointer
		if (stepX > 0) {
			srcOff = (screenWidth-stepX);
			dstOff = srcOff*CHAR_WIDTH;
		} else {
			srcOff = 0;
			dstOff = 0;
		}
		POKEW(charPtrZP, src+srcOff);
		
		// Assign screen information		
		tmp1 = screenWidth;
		screenWidth = ABS(stepX);
	#if defined __APPLE2__	
		tmp2 = screenCol1;
		screenCol1 += dstOff%LINE_SIZE;
	#else
		POKEW(scrPtrZP, (unsigned int)screenData+dstOff);
	  #if defined __ATARI__
		POKEW(charatrDataZP, charatrData);
	  #elif defined __CBM__	
		POKEW(charatrDataZP, charatrData);
		POKEW(colPtrZP, (unsigned int)colorData+dstOff);
	  #endif
	#endif
		BlitCharmap();			
		
		// Restore screen information
		screenWidth = tmp1;
	#if defined __APPLE2__	
		screenCol1 = tmp2;
	#endif
	}		
#endif	
}

void DrawCharmap(unsigned char x, unsigned char y)
{
#ifdef __NES__
	unsigned char i,j;
	unsigned char *ptr;
#endif
	
	// Platform specific handling
#if defined(__APPLE2__)
	x = 2*(x/2u)+1;
  #if defined(__DHR__)
	clk += 20;
  #else
	clk += 10;  
  #endif
#endif

	// Hide soft sprites
#if defined(__APPLE2__) || defined(__ORIC__)
	HideSprites();
#endif

	// Save new coordinates (for scrolling)
	worldX = x; worldY = y;
		
#ifdef __NES__
	// Decode tiles (if necessary)
	ptr = DecodeTiles();

	// Send to VRAM
	txtX = screenCol1; txtY = screenRow1;
	for (j=0; j<screenHeight; j++) {
		SetVramName();
		for (i=0; i<screenWidth; i++)
			SetVramChar(*(ptr+i));
		ptr += decodeWidth;
		txtY++; 
	}
#else
	// Decode tiles (if necessary)
	POKEW(charPtrZP, DecodeTiles());
	
	// Blit decoded map to screen
 #ifndef __APPLE2__
	POKEW(scrPtrZP, (unsigned int)screenData);
  #if defined __ATARI__
	POKEW(charatrDataZP, charatrData);
  #elif defined __CBM__	
	POKEW(charatrDataZP, charatrData);
	POKEW(colPtrZP, colorData);
  #endif
 #endif
	BlitCharmap();
#endif
}

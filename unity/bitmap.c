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

#if defined __ATMOS__
  #include <Oric/libsedoric.h>
#endif

// Helper functions
#define BYTE4(a,b,c,d) ((a<<6) | (b<<4) | (c<<2) | d)

// Colors for printing
unsigned char paperHead = 0;
unsigned char inkColor, paperColor;

// Apple specific variables & functions
#ifdef __APPLE2__
  extern void RestoreSprLine(unsigned char x, unsigned char y);
#endif

// Atari specific variables & functions
#ifdef __ATARI__
  unsigned char inkColor1,inkColor2;
  unsigned char paperColor1,paperColor2;
  unsigned char bgByte1,bgByte2;
#endif

// C64 specific variables & functions
#ifdef __CBM__
  int vicconf[3];
  unsigned char bg = 0;
  unsigned char pow2 = (2 | 2<<2 | 2<<4 | 2<<6);
  void SwitchBank(char bank) {
	// notice that 0 selects vic bank 3, 1 selects vic bank 2, etc.
	POKE(0xDD00, (PEEK(0xDD00) & 252 | (3 - bank))); // switch VIC base
  }
#endif

// Initialize Bitmap Screen
void InitBitmap() {
#if defined __CBM__
	// Backup VIC config
	vicconf[0] = PEEK(53272);
	vicconf[1] = PEEK(53265);
	vicconf[2] = PEEK(53270);
#elif defined __ATARI__
	// Set default palette
	POKE(PALETTERAM+0, 0x00);
	POKE(PALETTERAM+1, 0x24);
	POKE(PALETTERAM+2, 0x86);
	POKE(PALETTERAM+3, 0xd8);
#elif defined __APPLE2__
	// Prepare Double Hi-Res Mode
	asm("sta $c052"); // TURN ON FULLSCREEN       
	asm("sta $c057"); // TURN ON HI-RES           
	asm("sta $c001"); // TURN ON 80 STORE
#elif defined __ATMOS__
	// Switch to Hires mode
	if PEEK((char*)0xC800) {
		asm("jsr $EC33");	// Atmos (ROM 1.1)
	} else {
		asm("jsr $E9BB");	// Oric-1 (ROM 1.0)
	}
#endif	
}

// Switch from Text mode to Bitmap mode
void EnterBitmapMode()
{		
#if defined __CBM__
	// Set data direction flag and Switch bank
	POKE(0xDD02, (PEEK(0xDD02) | 3));
	SwitchBank(VIDEOBANK);

	// Enter multicolor mode
	POKE(0xD018, SCREENLOC*16 + BITMAPLOC);	// 53272: address of screen and bitmap RAM
	POKE(0xD011, PEEK(0xD011) | 32);		// 53265: set bitmap mode
	POKE(0xD016, PEEK(0xD016) | 16);		// 53270: set multicolor mode
#elif defined __ATARI__
	// Assign palette
	POKE(0x02c8, PEEK(PALETTERAM+0));
	POKE(0x02c4, PEEK(PALETTERAM+1));
	POKE(0x02c5, PEEK(PALETTERAM+2));
	POKE(0x02c6, PEEK(PALETTERAM+3));	
	
	// Switch ON graphic mode and antic
	__asm__("jsr %w", STARTBMP);			
	POKE(559, 32+16+8+4+2); // ANTIC: DMA Screen + Enable P/M + DMA Players + DMA Missiles + Single resolution
#elif defined __APPLE2__
	// Switch ON Double Hi-Res Mode
	asm("sta $c00d"); // TURN ON 80 COLUMN MODE	  
    asm("sta $c050"); // TURN ON GRAPHICS         
    asm("sta $c05e"); // TURN ON DOUBLE HI-RES
#endif
}

// Switch from Bitmap mode to Text mode
void ExitBitmapMode()
{
#if defined __CBM__
	// Return VIC and Bank back to previous state
	POKE(53272, vicconf[0]);
	POKE(53265, vicconf[1]);
	POKE(53270, vicconf[2]);
	SwitchBank(0);
#elif defined __ATARI__
    // Switch OFF graphic mode and antic
	__asm__("jsr %w", STOPBMP);
	POKE(559, 2);
#elif defined __APPLE2__
    // Switch OFF Double Hi-Res Mode
    asm("sta $c051"); // TEXT - HIDE GRAPHICS
    asm("sta $c05f"); // TURN OFF DOUBLE RES
	asm("sta $c00c"); // TURN OFF 80 COLUMN MODE	  
#endif
}

// Location of current pixel 
unsigned char bmpX, bmpY;

void LocatePixel(unsigned int x, unsigned int y)
{
// This function maps pixel coordinates from a 320x200 screen definition
// It can be by-passed by assigning bmpX, bmpY directly in your code
#if defined __APPLE2__
	bmpX = (x*140)/320;
	bmpY = (y*192)/200;
#elif defined __ATARI__
	bmpX = x/2;
	bmpY = y;
#elif defined __ATMOS__
	bmpX = (x*117)/320;	
	bmpY = y/2;
#elif defined __CBM__
	bmpX = x/2;
	bmpY = y;
#endif
}

unsigned char GetPixel()
{
#if defined __CBM__
	unsigned char index;
	unsigned int addr, offset;
	
	// Check color index
	DisableRom();
	addr = BITMAPRAM + 40*(bmpY&248)+(bmpY&7)+((bmpX*2)&504);
	index = (PEEK(addr) >> (2*(3-(bmpX%4)))) & 3;
	EnableRom();
	
	// Is background color?
	if (index==0) { return bg; }
	
	// Analyze color index
	offset = (bmpY/8)*40+(bmpX/4);
	if (index==1) {	// Upper bits of screen RAM
		addr = SCREENRAM + offset;
		return (PEEK(addr) & 0xF0) >> 4;		
	}
	if (index==2) {	// Lower bits of screen RAM
		addr = SCREENRAM + offset;
		return (PEEK(addr) & 0x0F);
	}
	if (index==3) { // Lower bits of color RAM
		addr = COLORRAM + offset;
		return (PEEK(addr) & 0x0F);
	}
#elif defined __ATARI__
	unsigned int offset;
	unsigned char val1, val2, shift;
	
	// Compute pixel location
	offset = 40*bmpY+bmpX/4;
	shift = 6 - 2*(bmpX%4);

	// Dual buffer (colour/shade)
	val1 = (PEEK((char*)BITMAPRAM1+offset) & ( 3 << shift )) >> shift;
	val2 = (PEEK((char*)BITMAPRAM2+offset) & ( 3 << shift )) >> shift;
	if (val1 > val2) {
		return val1*4 + val2;
	} else {
		return val2*4 + val1;
	}
#elif defined __APPLE2__
	// Use DHR routines
	RestoreSprLine(bmpX,bmpY);
	SetDHRPointer(bmpX,bmpY);
	return GetDHRColor();
#elif defined __ATMOS__
	unsigned int offset;
	unsigned char byte1, byte2, color, shift;
	
	// Compute pixel offset
	offset = bmpY*80 + bmpX/3 + 1;
	
	// Get bytes from Bitmap RAM
	byte1 = PEEK((char*)BITMAPRAM+offset);
	byte2 = PEEK((char*)BITMAPRAM+offset+40);	

	// Get PAPER/INK inversion group
	color = 5 * ((byte2>191)*2 + (byte1>191));
		
	// Get pixels state
	shift = 2 * (bmpX%3);
	byte1 = (byte1 & (48 >> shift)) << shift;
	byte2 = (byte2 & (48 >> shift)) << shift;
	switch (byte1) {
	case 0:
		if (byte2 == 48) {
			color += 3;
		}
		break;
	case 32:
		color += 1;
		break;
	case 48:
		if (byte2 == 48) {
			color += 4;
		} else {
			color += 2;
		}
		break;	
	}
	return color;
#endif	
}

void SetPixel(unsigned char color)
{
#if defined __CBM__
	unsigned int offset;
	unsigned char shift;
	
	// Set index to 3
	DisableRom();
	offset = 40*(bmpY&248)+(bmpY&7)+((bmpX*2)&504);
	shift = (2*(3-(bmpX%4)));
	POKE(BITMAPRAM+offset, PEEK(BITMAPRAM+offset) | 3 << shift);
	EnableRom();
	
	// Set color in COLORAM
	offset = (bmpY/8)*40+(bmpX/4);
	POKE(COLORRAM+offset, color);
#elif defined __ATARI__
	unsigned int offset;
	unsigned char shift, mask, col1, col2;	

	// Compute pixel location
	offset = 40*bmpY + bmpX/4;
	shift = 6 - 2*(bmpX%4);
	mask = 255 - (3 << shift);
	if ((bmpY+bmpX)%2) {
		col2 = (color%4) << shift;
		col1 = (color/4) << shift;
	} else {
		col1 = (color%4) << shift;
		col2 = (color/4) << shift;
	}

	// Dual buffer (colour/shade)
	POKE((char*)BITMAPRAM1+offset, (PEEK((char*)BITMAPRAM1+offset) & mask) | col1);
	POKE((char*)BITMAPRAM2+offset, (PEEK((char*)BITMAPRAM2+offset) & mask) | col2);
#elif defined __APPLE2__
	// Use DHR routines
	SetDHRPointer(bmpX,bmpY);
	SetDHRColor(color);	
#elif defined __ATMOS__
	unsigned int offset;
	unsigned char byte1, byte2, shift;
	
	// Compute pixel offset
	offset = bmpY*80 + bmpX/3 + 1;
	
	// Get bytes from Bitmap RAM
	byte1 = PEEK((char*)BITMAPRAM+offset) & 63;
	byte2 = PEEK((char*)BITMAPRAM+offset+40) & 63;	
	
	// Set PAPER/INK inversion
	switch (color/5) {
	case 0:
		byte1 |= 64;
		byte2 |= 64;
		break;
	case 1:
		byte1 |= 192;
		byte2 |= 64;
		break;
	case 2:
		byte1 |= 64;
		byte2 |= 192;
		break;
	case 3:
		byte1 |= 192;
		byte2 |= 192;	
		break;
	}
	
	// Set pixels
	shift = 2 * (bmpX%3);
	switch (color%5) {
    case 1:
		byte1 |= 32 >> shift;
		byte2 |= 16 >> shift;
		break;
	case 2:
		byte1 |= 48 >> shift;
		break;
	case 3:
		byte2 |= 48 >> shift;
		break;
	case 4:
		byte1 |= 48 >> shift;
		byte2 |= 48 >> shift;
		break;
	}
	
	// Assign bytes in Bitmap RAM
	POKE((char*)BITMAPRAM+offset,    byte1);
	POKE((char*)BITMAPRAM+offset+40, byte2);
#endif
}

// Load bitmap from file
void LoadBitmap(char *filename) 
{
#if defined __ATMOS__
	unsigned char i = 0;
	while (PEEK(filename+i) != '.') { i += 1; }
	i++; POKE(filename+i, 'c');
	i++; POKE(filename+i, 'o');
	i++; POKE(filename+i, 'm');
	loadfile(filename, (void*)(BITMAPRAM), 8000);
#else	
	unsigned int i;
	FILE* fp;

	// Open Map File
	fp = fopen(filename, "rb");
	
  #if defined __CBM__
	// Consume two bytes of header
	fgetc(fp);
	fgetc(fp);
	
	// 8000 bytes bitmap ram
	fread((char*)(BITMAPRAM), 1, 8000, fp);

	// 1000 bytes char ram
	fread((char*)(SCREENRAM), 1, 1000, fp);
	
	// 1000 bytes color ram
	fread((char*)(COLORRAM), 1, 1000, fp);
	
	// 1 byte background color
	bg = (char) fgetc(fp);	
  #elif defined __ATARI__	
	// 4 bytes palette ram
	fread((char*)PALETTERAM, 1, 4, fp);
	
	// 8000 bytes RAM1 (color 1)
	fread((char*)BITMAPRAM1, 1, 8000, fp);
	
	// 8000 bytes RAM2 (color 2)
	fread((char*)BITMAPRAM2, 1, 8000, fp);
  #elif defined __APPLE2__
	// Read 8192 bytes to AUX
	*dhraux = 0;
	fread((char*)BITMAPRAM, 1, 8192, fp);
	
	// Read 8192 bytes to MAIN
	*dhrmain = 0;
	fread((char*)BITMAPRAM, 1, 8192, fp);
  #endif
	// Close file
	fclose(fp);
#endif

	// Chat background color
	bmpX = 0; bmpY =0;
	paperHead = GetPixel();
}

// Clear entire bitmap screen
void ClearBitmap()
{
#if defined __CBM__
	bzero((char*)BITMAPRAM, 8000);
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)COLORRAM, 1000);
#elif defined __ATARI__
	bzero((char*)BITMAPRAM1, 8000);
	bzero((char*)BITMAPRAM2, 8000);
#elif defined __APPLE2__
    // clear main and aux screen memory	
	*dhraux = 0;
    bzero((char *)BITMAPRAM, 8192);
	*dhrmain = 0;
    bzero((char *)BITMAPRAM, 8192);
#elif defined __ATMOS__
	// reset pixels and set AIC Paper/Ink
	unsigned char y;
	memset((char*)BITMAPRAM, 64, 8000);	
    for (y=0; y<200; y++) {
		POKE((char*)BITMAPRAM+y*40, (y%2) ? 6 : 3);
	}	
#endif
}

void PrintNum(unsigned char col, unsigned char row, unsigned char num)
{
	if (num > 9) { PrintChr(col, row, &charDigit[(num/10)*3]); }
	PrintChr(col+1, row, &charDigit[(num%10)*3]);
}

// Draw panel using the background color
void DrawPanel(unsigned char colBeg, unsigned char rowBeg, unsigned char colEnd, unsigned char rowEnd)
{
	// Black-out menu area
	unsigned char j;
	unsigned int span;
	span = colEnd-colBeg+1;
	rowEnd++;
#if defined __CBM__
	for (j=rowBeg; j<rowEnd; ++j) {
		memset((char*)(BITMAPRAM+320*j+colBeg*8), pow2, span*8);
		memset((char*)(SCREENRAM+40*j+colBeg), paperColor, span);		
	}
#elif defined __ATARI__
	paperColor1 = paperColor%4;
	paperColor2 = paperColor/4;
	bgByte1 = BYTE4(paperColor1,paperColor2,paperColor1,paperColor2);
	bgByte2 = BYTE4(paperColor2,paperColor1,paperColor2,paperColor1);
	for (j=rowBeg*8; j<rowEnd*8; ++j) {
		if (j%2) {
			memset((char*)(BITMAPRAM1+j*40+colBeg), bgByte2, span);
			memset((char*)(BITMAPRAM2+j*40+colBeg), bgByte1, span);
		} else {
			memset((char*)(BITMAPRAM1+j*40+colBeg), bgByte1, span);
			memset((char*)(BITMAPRAM2+j*40+colBeg), bgByte2, span);
		}
	}	
#elif defined __APPLE2__
	// TODO: Implement panel drawing in color! (now only black)
	for (j=rowBeg*8; j<rowEnd*8; ++j) {
		SetDHRPointer((colBeg*35)/10, j);
		*dhraux = 0;
		bzero(dhrptr, span);
		*dhrmain = 0;
		bzero(dhrptr, span);
	}
#endif
}

// Print multicolor logo for the various platforms...
void PrintLogo(unsigned char col, unsigned char row, unsigned char index)
{
#if defined __CBM__
	// Define logos
	unsigned char logos[4][8] = { {0,0,0, 16, 68, 64, 72, 16}, 		// C64: (0,1,0,0) (1,0,1,0) (1,0,0,0) (1,0,2,0) (0,1,0,0)
								  {0,0,0, 16, 68,168,136,204},		// ATR: (0,1,0,0) (1,0,1,0) (2,2,2,0) (2,0,2,0) (3,0,3,0)
								  {0,0,0,  4, 16,168,168, 48},		// APP: (0,0,1,0) (0,1,0,0) (2,2,2,0) (2,2,2,0) (0,3,0,0)
								  {0,0,0,212,215,255,215,255} };	// FLP: (3,1,1,0) (3,1,1,3) (3,3,3,3) (3,1,1,3) (3,3,3,3)
	unsigned int addr1, addr2;
	unsigned char i;
	
	// Get memory addresses
	addr1 = BITMAPRAM + 40*((row*8)&248)+((col*8)&504);
	addr2 = SCREENRAM + row*40+col;
	
	// Set logo colors
	if (index == 0)	{	// C64
		POKE(addr2, BLUE << 4 | RED);
	} else {
		POKE(addr2, GREEN << 4 | PURPLE);
		POKE(COLORRAM + row*40+col, BLUE);
	}

	// Set Character data
	for (i=0; i<8; ++i) {
		POKE(addr1+i, logos[index][i]);
	}
#elif defined __ATARI__
	// Define logos
	unsigned char logos[4][8] = { {0,0,0, 32,136,128,132, 32}, 		// C64: (0,2,0,0) (2,0,2,0) (2,0,0,0) (2,0,1,0) (0,2,0,0)	BMP1
								  {0,0,0, 48,204, 84, 68,136},		// ATR: (0,3,0,0) (3,0,3,0) (1,1,1,0) (1,0,1,0) (2,0,2,0)
								  {0,0,0, 12, 48, 84,168, 32},  	// APP: (0,0,3,0) (0,3,0,0) (1,1,1,0) (2,2,2,0) (0,2,0,0)
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
#elif defined __APPLE2__
	// Define logos
	unsigned char logos[4][5][3] = { { { 0, 2, 0}, { 2, 0, 2}, { 2, 0, 0}, { 2, 0, 1}, { 0, 2, 0} },   // C64
									 { { 0,12, 0}, {12, 0,12}, {11,11,11}, {11, 0,11}, { 7, 0, 7} },   // ATR
								     { { 0, 0,12}, { 0,12, 0}, {11,11,11}, {11,11,11}, { 0, 7, 0} },   // APP
								     { { 7, 5, 0}, { 7, 5, 7}, { 7, 7, 7}, { 7, 5, 7}, { 7, 7, 7} } }; // FLP
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
#endif
}

// Print character using background and foreground colors
void PrintChr(unsigned char col, unsigned char row, const char *matrix)
{
#if defined __ATARI__	
	// Set Character across double buffer
	unsigned char i;
	unsigned int addr1,addr2;
	inkColor1 = inkColor%4; inkColor2 = inkColor/4;
	paperColor1 = paperColor%4; paperColor2 = paperColor/4;
	bgByte1 = BYTE4(paperColor1,paperColor2,paperColor1,paperColor2);
	bgByte2 = BYTE4(paperColor2,paperColor1,paperColor2,paperColor1);	
	addr1 = BITMAPRAM1 + row*320 + col;
	addr2 = BITMAPRAM2 + row*320 + col;
	if (matrix == &charBlank[0]) {
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
			POKE((char*)addr2+(i*2+1)*40, BYTE4(((matrix[i]&128) ? inkColor1 : paperColor1), ((matrix[i]&64) ? inkColor2 : paperColor2), ((matrix[i]&32) ? inkColor1 : paperColor1), paperColor2));
			POKE((char*)addr1+(i*2+2)*40, BYTE4(((matrix[i]&8  ) ? inkColor1 : paperColor1), ((matrix[i]&4 ) ? inkColor2 : paperColor2), ((matrix[i]&2 ) ? inkColor1 : paperColor1), paperColor2));
			POKE((char*)addr1+(i*2+1)*40, BYTE4(((matrix[i]&128) ? inkColor2 : paperColor2), ((matrix[i]&64) ? inkColor1 : paperColor1), ((matrix[i]&32) ? inkColor2 : paperColor2), paperColor1));
			POKE((char*)addr2+(i*2+2)*40, BYTE4(((matrix[i]&8  ) ? inkColor2 : paperColor2), ((matrix[i]&4 ) ? inkColor1 : paperColor1), ((matrix[i]&2 ) ? inkColor2 : paperColor2), paperColor1));
		}
		POKE((char*)addr1+7*40, bgByte2);
		POKE((char*)addr2+7*40, bgByte1);
	}
#elif defined __APPLE2__
	// Set Character over 3/4 pixels out of 7 in a cell
	unsigned int x,y;
	unsigned char i,j,n;
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
			SetDHRColor(((matrix[i]>>(7-j))&1) ? inkColor : paperColor);
			dhrpixel++;
		}
		SetDHRPointer(x, y+i*2+2);
		for (j=0; j<n; j++) {
			SetDHRColor(((matrix[i]>>(3-j))&1) ? inkColor : paperColor);
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
#elif defined __ATMOS__
	// Set Character inside 6*8 cell
	unsigned char i;
	unsigned char a0,a2,a4,b,blank;
	unsigned int addr;
	addr = BITMAPRAM + row*320 + col;
	blank = 64+ (paperColor ? 63 : 0);
	if (matrix == &charBlank[0]) {
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
			POKE((char*)addr+(i*2+1)*40, BYTE4(1, ((matrix[i]&128) ? a0 : b), ((matrix[i]&64) ? a2 : b), ((matrix[i]&32) ? a4 : b)));
			POKE((char*)addr+(i*2+2)*40, BYTE4(1, ((matrix[i]&8  ) ? a0 : b), ((matrix[i]&4 ) ? a2 : b), ((matrix[i]&2 ) ? a4 : b)));	
		}
		POKE((char*)addr+7*40, blank);
	}
#elif defined __CBM__
	// Set Character inside 4*8 cell
	unsigned char i;
	unsigned int addr;
	addr = BITMAPRAM + 40*((row*8)&248) + ((col*8)&504);
	if (matrix == &charBlank[0]) {
		memset((char*)addr, pow2, 8);
	} else {
		POKE((char*)addr, pow2);
		for (i=0; i<3; ++i) {
			POKE((char*)addr+2*i+1, BYTE4(((matrix[i]&128) ? 1 : 2), ((matrix[i]&64) ? 1 : 2), ((matrix[i]&32) ? 1 : 2), 2));
			POKE((char*)addr+2*i+2, BYTE4(((matrix[i]&8  ) ? 1 : 2), ((matrix[i]&4 ) ? 1 : 2), ((matrix[i]&2 ) ? 1 : 2), 2));
		}
		POKE((char*)addr+7, pow2);
	}
	
	// Set Color
	addr = SCREENRAM + row*40+col;
	POKE((char*)addr, inkColor << 4 | paperColor);	
#endif
}

#if defined __ATMOS__
  // INK attributes for characters
  unsigned char ink1[20] = { 0, 2, 3, 6, 3, 7, 5, 4, 7, 2, 7, 1, 3, 1, 1, 7, 5, 5, 5, 5 };
  unsigned char ink2[20] = { 0, 3, 3, 6, 6, 6, 6, 4, 6, 6, 6, 7, 7, 1, 3, 7, 4, 6, 7, 5 };
#endif

// Parse string and print characters one-by-one (slow)
void PrintStr(unsigned char col, unsigned char row, const char *buffer)
{
	const char *chr;
	unsigned char i;
#if defined __ATMOS__
	// Set INK attributes
	unsigned char i1, i2;
	unsigned int addr;	
	addr = BITMAPRAM + row*320 + col;
	if (paperColor != 0) {
		i1 = ink1[paperColor];
		i2 = ink2[paperColor];
	} else {
		i1 = ink1[inkColor];
		i2 = ink2[inkColor];
	}
	for (i=0; i<4; ++i) {
		POKE((char*)addr+i*80, i1);
		POKE((char*)addr+i*80+40, i2);
	}
	col++;
#endif
	for (i=0; i<strlen(buffer); ++i) {
		// Select the correct bitmask
		if (buffer[i] == 95) 	  { chr = &charUnderbar[0]; }
#if defined __CBM__
		else if (buffer[i] > 192) { chr = &charLetter[(buffer[i]-193)*3]; }		// Upper case (C64)
		else if (buffer[i] > 64)  { chr = &charLetter[(buffer[i]-65)*3]; }		// Lower case (C64)
#else
		else if (buffer[i] > 96)  { chr = &charLetter[(buffer[i]-97)*3]; }	// Lower case (Apple/Atari)
		else if (buffer[i] > 64)  { chr = &charLetter[(buffer[i]-65)*3]; }	// Upper case (Apple/Atari)
#endif
		else if (buffer[i] == 63) { chr = &charQuestion[0]; }
		else if (buffer[i] == 58) { chr = &charColon[0]; }
		else if (buffer[i] > 47)  { chr = &charDigit[(buffer[i]-48)*3]; }
		else if (buffer[i] == 47) { chr = &charSlash[0]; }
		else if (buffer[i] == 46) { chr = &charDot[0]; }
		else if (buffer[i] == 45) { chr = &charHyphen[0]; }
		else if (buffer[i] == 44) { chr = &charComma[0]; }
		else if (buffer[i] > 39)  { chr = &charBracket[(buffer[i]-40)*3]; }
		else if (buffer[i] == 39) { chr = &charQuote[0]; }
		else if (buffer[i] == 33) { chr = &charExclaim[0]; }
		else if (buffer[i] == 32) { chr = &charBlank[0]; }
		PrintChr(col+i, row, chr);
	}
}

// Rolling buffer at the top of the screen, that moves text leftward when printing
void PrintHeader(const char *buffer)
{
	unsigned char len, i;
	len = strlen(buffer);
	paperColor = paperHead;
#if defined __CBM__
	// Roll bitmap and screen ram
	DisableRom();
	memcpy((char*)BITMAPRAM, (char*)(BITMAPRAM+len*8), (40-len)*8);
	EnableRom();
	memcpy((char*)SCREENRAM, (char*)(SCREENRAM+len), (40-len));
#elif defined __ATARI__
	// Roll chroma and luma buffers
	for (i=0; i<8; ++i) {
		memcpy((char*)BITMAPRAM1+i*40, (char*)(BITMAPRAM1+len)+i*40, (40-len));
		memcpy((char*)BITMAPRAM2+i*40, (char*)(BITMAPRAM2+len)+i*40, (40-len));
	}
#elif defined __APPLE2__
	// Always move 7 pixels at a time!
	len = 2*(len/2+len%2);
	for (i=0; i<8; ++i) {
		SetDHRPointer(0, i);		
		*dhraux = 0;
		memcpy((char*)(dhrptr), (char*)(dhrptr+len), (40-len));
		*dhrmain = 0;
		memcpy((char*)(dhrptr), (char*)(dhrptr+len), (40-len));
	}
	if (strlen(buffer)%2) { PrintChr(39, 0, charBlank); }
#endif
	// Print new message
	PrintStr(40-len, 0, buffer);
	paperColor = COLOR_BLACK;
}

// Interactive text input function
unsigned char InputUpdate(unsigned char col, unsigned char row, char *buffer, unsigned char len, unsigned char key)
{
	unsigned char curlen;
	
	// Was a new key received?
	if (!key) {
		// Initialize input fied
		DrawPanel(col, row, col+len, row);
		PrintStr(col, row, buffer);
	} else {
		// Check current length of input
		curlen = strlen(buffer);

		// Process Letter keys
#if defined __ATARI__	
		if (key == 32 | key == 33 | (key > 38 & key < 42) | (key > 43 & key < 59) | key == 63 | (key > 96 & key < 123)) {	// Atari
#else
		if (key == 32 | key == 33 | (key > 38 & key < 42) | (key > 43 & key < 59) | key == 63 | (key > 64 & key < 91)) {	// Apple/C64
#endif
			if (curlen < len) { 
				buffer[curlen] = key;
				buffer[curlen+1] = 0; 
				PrintStr(col+curlen, row, &buffer[curlen]);
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
	char i;
	
	// Print initial condition
	InputUpdate(col, row, buffer, len, 0);
	
	// Run input loop
	while (1) {
		if (InputUpdate(col, row, buffer, len, cgetc())) { return; }
	}
}

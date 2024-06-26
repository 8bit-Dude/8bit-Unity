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

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
  #pragma bss-name(push, "XRAM")
    unsigned char pixelMap[768];
  #pragma bss-name(pop)	
#endif

// Apple specific variables & functions
#ifdef __APPLE2__
  extern unsigned char xHires, yHires, xptr, yptr, *bgPtr;
  void RestoreSprLine(unsigned char x, unsigned char y)
  {
	  // Restore 1 line from sprite background
	  unsigned char i;
	  for (i=0; i<SPRITE_NUM; i++) {
		  if (sprDrawn[i]) {
			  xHires = x-sprX[i];
			  yHires = y-sprY[i];
			  if (xHires<7 && yHires<sprRows[i]) {
				  xptr = sprHiresX[i];
				  yptr = sprY[i]+yHires;
				  bgPtr = sprBG[i]+yHires*BYTEWIDTH;				  
				  hiresPtr = HiresLine(yptr) + xptr;
				#if defined __DHR__  
				  AuxToAux(hiresPtr, bgPtr, 2);
				#endif
				  hiresPtr[0] = bgPtr[0];
				  hiresPtr[1] = bgPtr[1];		
				  return;
			  }
		  }
	  }		
  }
#endif

// Location of current pixel 
unsigned char pixelX, pixelY;

void LocatePixel(unsigned int x, unsigned int y)
{
// This function maps pixel coordinates from a 320x200 screen definition
// It can be by-passed by assigning pixelX, pixelY directly in your code
#if defined __APPLE2__	// Hires Mode: 140 x 192
	pixelX = (x*140)/320u;
	pixelY = (y*192)/200u;
#elif defined __ATARI__	// INP Mode: 160 x 200
	pixelX = x/2u;
	pixelY = y;
#elif defined __CBM__	// MLC Mode: 160 x 200
	pixelX = x/2u;
	pixelY = y;
#elif defined __LYNX__	// STD Mode: 160 x 102
	pixelX = x/2u;
	pixelY = (y*102)/200u;
#elif defined __NES__	// CHR Mode: 32 x 24
	pixelX = x/10u;	
	pixelY = (y*3)/25u;	
#elif defined __ORIC__	// AIC Mode: 117 x 100 equivalent pixels
	pixelX = (x*117)/320u;	
	pixelY = y/2u;	
#endif
}

unsigned char GetPixel()
{
#if defined __CBM__
	unsigned char index;
	unsigned int addr, offset;
	
	// Check color index
	addr = BITMAPRAM + (pixelY&248)*40 + (pixelY&7) + ((pixelX*2)&504);
	rom_disable();
	index = (PEEK(addr) >> (2*(3-(pixelX&3)))) & 3;
	rom_restore();
	
	// Is background color?
	if (index==0) { return 0; }
	
	// Analyze color index
	offset = (pixelY/8u)*40+(pixelX/4u);
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
	unsigned char shift, mask, col1, col2;

	// Compute pixel location
	offset = (pixelY*40) + (pixelX/4u);
	
  #ifdef __ATARIXL__	
	// Dual buffer
	shift = 6 - 2*(pixelX&3);
	mask  = (3 << shift);
	col1  = (PEEK((char*)BITMAPRAM1+offset) & mask) >> shift;
	col2  = (PEEK((char*)BITMAPRAM2+offset) & mask) >> shift;
	if (col1 > col2) {
		return col1*4 + col2;
	} else {
		return col2*4 + col1;
	}
  #else
	// Single buffer  
	shift = 4 - 2*(pixelX&2);
	mask  = (15 << shift);
	col1  = (PEEK((char*)BITMAPRAM1+offset) & mask) >> shift;	  
	if ((pixelY+pixelX)&1) {
		return col1;
	} else {
		return (((col1&3)<<2) + (col1>>2));
	}
  #endif
	
#elif defined __APPLE2__
	// Use Hires routines
	RestoreSprLine(pixelX,pixelY);
	SetHiresPointer(pixelX,pixelY);
  #if defined __DHR__	
    return GetColorDHR();
  #else
    return GetColorSHR();
  #endif
	
#elif defined __ORIC__
	unsigned int addr;
	unsigned char i, pX, pY, xO, yO, occlusion = 0;
	unsigned char byte1, byte2, shift, color = 0;
	extern unsigned char* sprBG[SPRITE_NUM];

	// Scale to block coordinates (6x2)
	pX = pixelX/3u+1; pY = pixelY*2;
	
	// Check for sprite occlusion
	for (i=0; i<SPRITE_NUM; i++) {
		if (sprDrawn[i]) {
			xO = pX - (sprX[i]);
			yO = pY - (sprY[i]);
			if (xO<3 && yO<SPRITEHEIGHT) {
				addr = sprBG[i]+yO*4u+xO;
				byte1 = PEEK(addr);
				byte2 = PEEK(addr+4);	
				occlusion = 1;
				break;
			}
		}
	}
	
	// Get 2 bytes from Bitmap RAM (interlaced lines)
	if (!occlusion) {
		addr = (char*)BITMAPRAM + pY*40 + pX;		
		byte1 = PEEK(addr);
		byte2 = PEEK(addr+40);	
	}
	
	// Get PAPER/INK inversion group
	if (byte1 & 128) { color += 5; }
	if (byte2 & 128) { color += 10; }
		
	// Get pixels state
	shift = 2 * (pixelX%3);
	byte1 = (byte1 << shift) & 48;
	byte2 = (byte2 << shift) & 48;
	switch (byte1) {
	case 0:
		if (byte2 == 48) 
			color += 3;
		break;
	case 32:
		color += 1;
		break;
	case 48:
		if (byte2 == 48) 
			color += 4;
		else
			color += 2;
		break;	
	}
	return color;
	
#elif defined __LYNX__
	unsigned int addr;
	addr = BITMAPRAM + pixelY*82 + pixelX/2u + 1;
	if (pixelX&1) { 
		return (PEEK((char*)addr) & 15);
	} else {
		return (PEEK((char*)addr) & 240) >> 4;
	}	
	
#elif defined __NES__
	unsigned char block;
	vram_adr(NTADR_A(pixelX,(pixelY+3)));
	ppu_off();	
	vram_read(&block, 1);
	ppu_on_all();
	return block;	
#endif	
}

void SetPixel(unsigned char color)
{
#if defined __APPLE2__
	// Use Hires routines
	SetHiresPointer(pixelX,pixelY);
  #if defined __DHR__	
	SetColorDHR(color);	
  #else
	SetColorSHR(color);	
  #endif
	
#elif defined __ATARI__
	unsigned int offset;
	unsigned char shift, mask, col1, col2;	

	// Compute pixel location
	offset = (pixelY*40) + (pixelX/4u);
	
  #ifdef __ATARIXL__
	// Dual buffer  
	shift = 6 - 2*(pixelX&3);
	mask = ~(3 << shift);
	if ((pixelY+pixelX)&1) {
		col2 = (color&3) << shift;
		col1 = (color>>2) << shift;
	} else {
		col1 = (color&3) << shift;
		col2 = (color>>2) << shift;
	}
	POKE((char*)BITMAPRAM1+offset, (PEEK((char*)BITMAPRAM1+offset) & mask) | col1);
	POKE((char*)BITMAPRAM2+offset, (PEEK((char*)BITMAPRAM2+offset) & mask) | col2);
  #else
	// Single buffer  
	shift = 4 - 2*(pixelX&2);
	mask = ~(15 << shift);
	if ((pixelY+pixelX)&1) {
		col1 = (color&15) << shift;
	} else {
		col1 = (((color&3)<<2) + (color>>2)) << shift;
	}
	POKE((char*)BITMAPRAM1+offset, (PEEK((char*)BITMAPRAM1+offset) & mask) | col1);
  #endif

#elif defined __CBM__
	unsigned int offset;
	unsigned char shift;
	
	// Set index to 3
	offset = (pixelY&248)*40 + (pixelY&7) + ((pixelX*2)&504);
	shift = 2*(3-(pixelX&3));
	rom_disable();
	POKE(BITMAPRAM+offset, PEEK(BITMAPRAM+offset) | 3 << shift);
	rom_restore();
	
	// Set color in COLORAM
	offset = (pixelY/8u)*40u+(pixelX/4u);
	POKE(COLORRAM+offset, color);

#elif defined __NES__
	// Set screen & map coordinates
	unsigned char *p;
	txtX = pixelX/2u; txtY = pixelY/2u;
	p = pixelMap[txtY*32+txtX];
	
	// Encode pixel data
	if (pixelY%2) {
		if (pixelX%2)
			p |= 0b10011000;
		else
			p |= 0b10010100;		
	} else {
		if (pixelX%2)
			p |= 0b10010010;
		else
			p |= 0b10010001;
	}
	
	// Write char to VRAM buffer
	SetVramName();
	SetVramChar(p);

	// Write attribute to VRAM buffer
	SetVramAttr();
	SetVramColor(1);
		
#elif defined __ORIC__
	unsigned int offset;
	unsigned char byte1, byte2, shift, shift48;
	
	// Compute pixel offset
	offset = pixelY*80 + pixelX/3u + 1;
	
	// Get bytes from Bitmap RAM
	byte1 = PEEK((char*)BITMAPRAM+offset) & 63;
	byte2 = PEEK((char*)BITMAPRAM+40+offset) & 63;	
	
	// Set PAPER/INK inversion
	switch (color/5u) {
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
	shift = 2 * (pixelX%3);
	shift48 = (48 >> shift);
	byte1 &= ~shift48;
	byte2 &= ~shift48;
	switch (color%5) {
    case 1:
		byte1 |= 32 >> shift;
		byte2 |= 16 >> shift;
		break;
	case 2:
		byte1 |= shift48;
		break;
	case 3:
		byte2 |= shift48;
		break;
	case 4:
		byte1 |= shift48;
		byte2 |= shift48;
		break;
	}
	
	// Assign bytes in Bitmap RAM
	POKE((char*)BITMAPRAM+offset,    byte1);
	POKE((char*)BITMAPRAM+40+offset, byte2);
	
#elif defined __LYNX__
	unsigned char* addr = (char*)BITMAPRAM + pixelY*82u + pixelX/2u + 1;
	if (pixelX&1) { 
		*addr &= 240;
		*addr |= color;
	} else {
		*addr &= 15;
		*addr |= color << 4;
	}
#endif
}

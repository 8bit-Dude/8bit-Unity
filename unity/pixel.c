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

#ifdef __ATARI__
  #pragma code-name("SHADOW_RAM")
#endif

// Apple specific variables & functions
#ifdef __APPLE2__
  extern void RestoreSprLine(unsigned char x, unsigned char y);
#endif

#ifdef __CBM__
  extern unsigned char bg;
#endif


// Location of current pixel 
unsigned char pixelX, pixelY;

void LocatePixel(unsigned int x, unsigned int y)
{
// This function maps pixel coordinates from a 320x200 screen definition
// It can be by-passed by assigning pixelX, pixelY directly in your code
#if defined __APPLE2__	// DHR Mode: 140 x 192
	pixelX = (x*140)/320;
	pixelY = (y*192)/200;
#elif defined __ATARI__	// INP Mode: 160 x 200
	pixelX = x/2;
	pixelY = y;
#elif defined __CBM__	// MLC Mode: 160 x 200
	pixelX = x/2;
	pixelY = y;
#elif defined __LYNX__	// STD Mode: 160 x 102
	pixelX = x/2;
	pixelY = (y*102)/200;
#elif defined __ORIC__	// AIC Mode: 117 x 100 equivalent pixels
	pixelX = (x*117)/320;	
	pixelY = y/2;	
#endif
}

unsigned char GetPixel()
{
#if defined __CBM__
	unsigned char index;
	unsigned int addr, offset;
	
	// Check color index
	addr = BITMAPRAM + 40*(pixelY&248)+(pixelY&7)+((pixelX*2)&504);
	rom_disable();
	index = (PEEK(addr) >> (2*(3-(pixelX%4)))) & 3;
	rom_enable();
	
	// Is background color?
	if (index==0) { return bg; }
	
	// Analyze color index
	offset = (pixelY/8)*40+(pixelX/4);
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
	offset = 40*pixelY+pixelX/4;
	shift = 6 - 2*(pixelX%4);

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
	RestoreSprLine(pixelX,pixelY);
	SetDHRPointer(pixelX,pixelY);
	return GetDHRColor();
#elif defined __ORIC__
	unsigned int addr;
	unsigned char i, pX, pY, xO, yO, occlusion = 0;
	unsigned char byte1, byte2, shift, color = 0;
	extern unsigned char sprDrawn[SPRITE_NUM];
	extern unsigned char* sprBG[SPRITE_NUM];
	extern unsigned char sprX[SPRITE_NUM];
	extern unsigned char sprY[SPRITE_NUM];
	extern unsigned char frameROWS;

	// Scale to block coordinates (6x2)
	pX = pixelX/3+1; pY = pixelY*2;
	
	// Check for sprite occlusion
	for (i=0; i<SPRITE_NUM; i++) {
		if (sprDrawn[i]) {
			xO = pX - (sprX[i]);
			yO = pY - (sprY[i]);
			if (xO<3 && yO<frameROWS) {
				addr = sprBG[i]+yO*4+xO;
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
		if (byte2 == 48) { color += 3; }
		break;
	case 32:
		color += 1;
		break;
	case 48:
		if (byte2 == 48) { color += 4; } else { color += 2; }
		break;	
	}
	return color;
#elif defined __LYNX__
	unsigned int addr;
	addr = BITMAPRAM + pixelY*82 + pixelX/2 + 1;
	if (pixelX%2) { 
		return (PEEK((char*)addr) & 15);
	} else {
		return (PEEK((char*)addr) & 240) >> 4;
	}	
#endif	
}

void SetPixel(unsigned char color)
{
#if defined __CBM__
	unsigned int offset;
	unsigned char shift;
	
	// Set index to 3
	offset = 40*(pixelY&248)+(pixelY&7)+((pixelX*2)&504);
	shift = (2*(3-(pixelX%4)));
	rom_disable();
	POKE(BITMAPRAM+offset, PEEK(BITMAPRAM+offset) | 3 << shift);
	rom_enable();
	
	// Set color in COLORAM
	offset = (pixelY/8)*40+(pixelX/4);
	POKE(COLORRAM+offset, color);
#elif defined __ATARI__
	unsigned int offset;
	unsigned char shift, mask, col1, col2;	

	// Compute pixel location
	offset = 40*pixelY + pixelX/4;
	shift = 6 - 2*(pixelX%4);
	mask = 255 - (3 << shift);
	if ((pixelY+pixelX)%2) {
		col2 = (color%4) << shift;
		col1 = (color/4) << shift;
	} else {
		col1 = (color%4) << shift;
		col2 = (color/4) << shift;
	}

	// Set color/color2 in dual buffer
	POKE((char*)BITMAPRAM1+offset, (PEEK((char*)BITMAPRAM1+offset) & mask) | col1);
	POKE((char*)BITMAPRAM2+offset, (PEEK((char*)BITMAPRAM2+offset) & mask) | col2);
#elif defined __APPLE2__
	// Use DHR routines
	SetDHRPointer(pixelX,pixelY);
	SetDHRColor(color);	
#elif defined __ORIC__
	unsigned int offset;
	unsigned char byte1, byte2, shift;
	
	// Compute pixel offset
	offset = pixelY*80 + pixelX/3 + 1;
	
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
	shift = 2 * (pixelX%3);
	byte1 &= ~(48 >> shift);
	byte2 &= ~(48 >> shift);
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
#elif defined __LYNX__
	unsigned char* addr = (char*)BITMAPRAM + pixelY*82 + pixelX/2 + 1;
	if (pixelX%2) { 
		*addr &= 240;
		*addr |= color;
	} else {
		*addr &= 15;
		*addr |= color << 4;
	}
#endif
}

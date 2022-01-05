/*
 * Copyright (c) 2022 Anthony Beaucamp.
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

extern unsigned char chunkBuf[CHUNKSIZE];
extern unsigned char *chunkPtr;

void SetChunk(unsigned char* chunk, unsigned char x, unsigned char y)
{
#if defined __APPLE2__
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	POKE(0xE3, (w*2)/7u);	// Bytes per line (x2 for MAIN/AUX)	
	POKE(0xCE, h);			// Number of lines (from screen)
	POKE(0xEB, h);			// Number of lines (to screen)
	POKE(0xEC, (x*2)/7u);	// Hires Offset X
	POKE(0xED, y);			// Hires Offset Y
	POKEW(0xEE, 0);			// Address for copying Hires > Output
	POKEW(0xFA, chunk+6);	// Address for copying Input > Hires
	BlitSprite();
	
#elif defined __ATARI__
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	unsigned char i, bytes = w/4u;
	unsigned char *src = chunk+6;	
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM1+(y+i)*40+x/4u), src, bytes);
		src += bytes;
	}
  #ifdef __ATARIXL__
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM2+(y+i)*40+x/4u), src, bytes);
		src += bytes;
	}
  #endif
#elif defined __ORIC__
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	unsigned int dst = BITMAPRAM + y*40u + x/6u;
	POKE(0xb0, h); 			// Number of lines
	POKE(0xb1, w/6u);		// Bytes per line
	POKEW(0xb2, chunk+5);	// Address of source (-1)
	POKEW(0xb4, dst-1);		// Address of target (-1)
	POKE(0xb6, w/6u); 		// Offset between source lines
	POKE(0xb7, 40); 		// Offset between target lines
	BlitSprite();	
	
#elif defined __C64__
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	unsigned char i, bytes;
	unsigned int src = chunk+6;	
	bytes = (w*8)/4u;
	rom_disable();
	for (i=0; i<h; i+=8) {
		memcpy((char*)(BITMAPRAM+40*((y+i)&248)+((y+i)&7)+((x*2)&504)), (char*)src, bytes);
		src += bytes;
	}
	rom_enable();
	bytes = w/4u;
	for (i=0; i<h; i+=8) {
		memcpy((char*)(SCREENRAM+40*(y+i)/8u+x/4u), (char*)src, bytes);
		src += bytes;
	}
	for (i=0; i<h; i+=8) {
		memcpy((char*)(COLORRAM+40*(y+i)/8u+x/4u), (char*)src, bytes);
		src += bytes;
	}
	
#elif defined __LYNX__
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	unsigned char *src = chunk+6;	
	unsigned char i, bytes = w/2u;
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM+(y+i)*82+x/2u+1), src, bytes);
		src += bytes;
	}
	UpdateDisplay();
	
#elif defined __NES__
	unsigned char i, j;
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	unsigned char *src = chunk+6;

	// Set names
	BackupCursor();
	txtX = x; txtY = y;
	for (i=0; i<h; ++i) {
		SetVramName();
		for (j=0; j<w; j++)
			SetVramChar(*src++);	
		txtY++;
	}	
	
	// Set attributes
 	txtY = y;
	h /= 4u; w /= 4u;	
	h += 1;  w += 1;
	for (i=0; i<h; ++i) {
		SetVramAttr();
		for (j=0; j<w; j++) {
			vram_attr[vram_attr_index++] = *src;
			SetVramChar(*src++); 
		}
		txtY += 4;		
	}
	RestoreCursor();
	UpdateDisplay();
#endif
}

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
void CheckMemory(void);

unsigned char *GetChunk(unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
	// Get chunk from Video RAM
  #if defined(__ATARI__) || defined(__C64__) || defined(__LYNX__)
	unsigned char i, bytes;
  #elif defined (__NES__)	
	unsigned int vaddr;
	unsigned char i;
  #endif
	unsigned char *chunk = chunkPtr;
	POKE(chunkPtr++, x);
	POKE(chunkPtr++, y);
	POKE(chunkPtr++, w);
	POKE(chunkPtr++, h);
	chunkPtr += 2; // Leave space for size
	
  #if defined __APPLE2__
	// Blit data 
	POKE(0xE3, (w*2)/7u);	// Bytes per line (x2 for DHR)	
	POKE(0xCE, h);			// Number of lines (from screen)
	POKE(0xEB, h);			// Number of lines (to screen)
	POKE(0xEC, (x*2)/7u);	// Hires Offset X
	POKE(0xED, y);			// Hires Offset Y
	POKEW(0xEE, chunkPtr);	// Address of Output
	POKEW(0xFA, 0);			// Address of Input
	BlitSprite();
	chunkPtr += (h*w*2)/7u;
  
  #elif defined __ATARI__
	bytes = w/4u;
	for (i=0; i<h; ++i) {
		memcpy((char*)chunkPtr, (char*)(BITMAPRAM1+(y+i)*40u+x/4u), bytes);
		chunkPtr += bytes;
	}
   #ifdef __ATARIXL__	
	for (i=0; i<h; ++i) {
		memcpy((char*)chunkPtr, (char*)(BITMAPRAM2+(y+i)*40u+x/4u), bytes);
		chunkPtr += bytes;
	}
   #endif
	
  #elif defined __C64__
	bytes = (w*8)/4u;	
	rom_disable();
	for (i=0; i<h; i+=8) {
		memcpy((char*)chunkPtr, (char*)(BITMAPRAM+40*((y+i)&248)+((y+i)&7)+((x*2)&504)), bytes);
		chunkPtr += bytes;
	}
	rom_enable();
	bytes = w/4u;
	for (i=0; i<h; i+=8) {
		memcpy((char*)chunkPtr, (char*)(SCREENRAM+40*(y+i)/8u+x/4u), bytes);
		chunkPtr += bytes;
	}
	for (i=0; i<h; i+=8) {
		memcpy((char*)chunkPtr, (char*)(COLORRAM+40*(y+i)/8u+x/4u), bytes);
		chunkPtr += bytes;
	}
	
  #elif defined __LYNX__
	bytes = w/2u;	
	for (i=0; i<h; ++i) {
		memcpy((char*)chunkPtr, (char*)(BITMAPRAM+(y+i)*82+x/2u+1), bytes);
		chunkPtr += bytes;
	}
	
  #elif defined __NES__
	// Get names
	y += 3;
	ppu_off();	
	for (i=0; i<h; ++i) {
		vaddr = NTADR_A(x,y);
		vram_adr(vaddr);
		vram_read(chunkPtr, w);
		chunkPtr += w; y++;
	}	
	ppu_on_all();	

	// Get attributes
	y -= h; // return to top
	x /= 4u; y /= 4u;
	h /= 4u; w /= 4u;	
	h += 1;  w += 1;
	for (i=0; i<h; ++i) {
		memcpy(chunkPtr, &vram_attr[8*y+x], w);
		chunkPtr += w; y += 1;
	}
	
  #elif defined __ORIC__
    w /= 6u;
	POKE(0xb0, h); 		// Number of lines
	POKE(0xb1, w);		// Bytes per line
	POKEW(0xb2, BITMAPRAM + y*40 + x/6u -1); // Address of source (-1)
	POKEW(0xb4, chunkPtr-1);// Address of target (-1)
	POKE(0xb6, 40); 	// Offset between source lines
	POKE(0xb7, w); 	    // Offset between target lines
	BlitSprite();
	chunkPtr += h*w;	
  #endif	
  
	// Assign size value
	*(unsigned int*)&chunk[4] = (unsigned int)(chunkPtr - chunk);

	// Detect buffer overflow
	CheckMemory();
	return chunk;
}

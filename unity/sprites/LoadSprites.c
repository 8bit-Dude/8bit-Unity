/*
 * Copyright (c) 2019 Anthony Beaucamp.
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
#include "Sprites.h"

// Platform specific parameters
#if defined __APPLE2__
	unsigned char *sprData;					// Pointer to sprite data (allocated dynamically)
	unsigned char  sprX[SPRITE_NUM], sprY[SPRITE_NUM];	 // Screen coordinates
	unsigned char  sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM]; // Draw/Collision status
	unsigned char  sprHiresX[SPRITE_NUM];  	// Byte offset within Hires line
	unsigned char *sprBG[SPRITE_NUM];  		// Sprite background
	unsigned char  sprRows[SPRITE_NUM];  	// Sprite dimensions used in algorithms
	
#elif defined __ATARI__	
	unsigned char *sprData, sprYOffset, sprCollision[SPRITE_NUM], sprCushion = 2;
	unsigned char sprMask[] = { 1, 2, 4, 8, 1, 2, 4, 8, 1, 2, 4, 8 };	

#elif defined __CBM__
	void DoubleHeightSprite(unsigned char index, unsigned char onoff) {
		if (onoff)
			POKE(0xD017, PEEK(0xD017) |  (1 << index));
		else
			POKE(0xD017, PEEK(0xD017) & !(1 << index));
	}
	void DoubleWidthSprite(unsigned char index, unsigned char onoff) {
		if (onoff)
			POKE(0xD01D, PEEK(0xD01D) |  (1 << index));
		else
			POKE(0xD01D, PEEK(0xD01D) & !(1 << index));
	}	
	
#elif defined __LYNX__
	unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM];	// Screen coordinates
	unsigned char sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM], sprCushion = 2; // Enable and Collision status
	SCB_REHV_PAL sprSCB[SPRITE_NUM] = { { BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
									    { BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } } };
	SCB_REHV_PAL *scb;
	
#elif defined __NES__
 #pragma bss-name(push, "XRAM")
	unsigned char metaSprites[136] = { 0,0,0,0, 8,0,0,0, 0,8,0,0, 8,8,0,0, 128,
									   0,0,0,0, 8,0,0,0, 0,8,0,0, 8,8,0,0, 128,
									   0,0,0,0, 8,0,0,0, 0,8,0,0, 8,8,0,0, 128,
									   0,0,0,0, 8,0,0,0, 0,8,0,0, 8,8,0,0, 128,
									   0,0,0,0, 8,0,0,0, 0,8,0,0, 8,8,0,0, 128,
									   0,0,0,0, 8,0,0,0, 0,8,0,0, 8,8,0,0, 128,
									   0,0,0,0, 8,0,0,0, 0,8,0,0, 8,8,0,0, 128,
									   0,0,0,0, 8,0,0,0, 0,8,0,0, 8,8,0,0, 128 };
	unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM];	// Screen coordinates
	unsigned char sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM], sprCushion = 2; // Enable and Collision status	
 #pragma bss-name(pop)
	
#elif defined __ORIC__	
	unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM], sprOverlap[SPRITE_NUM]; // Screen coordinates, Overlap flag
	unsigned char sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM]; // Enable Flag, Collision status
	unsigned char sprRows[SPRITE_NUM], *sprBG[SPRITE_NUM]; // Height of Sprite (rows), Pointer to background backup
	unsigned int  scrAddr[SPRITE_NUM], sprCOLOR;  // Screen address, Color vector
	unsigned char* sprMULTICOLOR[SPRITE_NUM];
	void MultiColorSprite(unsigned char index, unsigned char* multiColorDef) {
		// Assign multicolor definition to sprite { color, row, ... color, lastrow }
		sprMULTICOLOR[index] = multiColorDef;
	}	
#endif

void LoadSprites(unsigned char* filename, const unsigned char *spriteColors)
{
#if defined __APPLE2__
	unsigned char i;
	unsigned int size;
	
	// Load sprite sheet
	if (FileOpen(filename)) {
		FileRead((char*)&size, 2); 
		if (sprData) free(sprData);
		sprData = malloc(size);
	  #if defined __DHR__
		FileRead(sprData, size);	// Load AUX data
		MainToAux(sprData, size);	// Transfer MAIN -> AUX
	  #endif	
		FileRead(sprData, size);	// Load MAIN data
		FileClose();
	}
	
	// Set sprite rows size
	memset(sprRows, SPRITEHEIGHT, SPRITE_NUM);
	
#elif defined __ATARI__	
	unsigned int size;

	// Load sprite sheet
	if (FileOpen(filename)) {
		FileRead((char*)&size, 2);
		if (sprData) free(sprData);
		sprData = malloc(size);
		FileRead(sprData, size);
	}
	
	// Reset Sprite Mask, Frames, Colors and Rows
	memcpy(sprColor, spriteColors, SPRITE_NUM);
	sprRows = SPRITEHEIGHT;
	sprDLIs = (SPRITEHEIGHT+15)>>3;
	sprPads = sprDLIs*8+1;
	sprYOffset = (SPRITEHEIGHT/2u)+2;

	// Clear all PMG memory
	bzero(PMGRAM,0x400);
	
	// Setup ANTIC and GTIA
	POKE(54279, (PMGRAM-0x400)>>8); // Tell ANTIC where the PM data is located
	POKE(53277, 2+1);       // Tell GTIA to enable players + missile	
	POKE(623, 32+16+1);		// Tricolor players + enable fifth player + priority  (shadow for 53275)	
	
	// ANTIC settings: Enable P/M + DMA Players + Standard Playfield (no DMA Missiles = 4)
	POKE(559, PEEK(559) | (16+8+2));
	
	// Setup sprites DLI/VBI
	StartDLI(); StartVBI(); spriteVBI = 1;
	
#elif defined(__CBM__)
	// TODO: sprite sheets larger than $700 can be loaded by exomizer, 
	// but not cc65! (file loading under ROM not possible)
	
	// Set sprite colors
	memcpy(53287, spriteColors, SPRITE_NUM);
	
	// Set common colors
	POKE(53285, spriteColors[8]);
	POKE(53286, spriteColors[9]);	
	
	// Set to multicolor code
	POKE(53276, 255);	
	
#elif defined __LYNX__
	// TODO: only 1 sheet supported
	
#elif defined __NES__
	// TODO: only 1 sheet supported
	
	// Set palette
	pal_spr(spriteColors);
		
#elif defined __ORIC__	
	unsigned char i;
	
	// Load sprite sheet
	if (FileOpen(filename))
		FileRead((void*)SPRITERAM, -1);

	// Assign frame info and sprite colors
	memset(sprRows, SPRITEHEIGHT, SPRITE_NUM);
	sprCOLOR = spriteColors;	
#endif
}

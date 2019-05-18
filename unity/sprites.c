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

#ifdef __APPLE2__
#pragma code-name("LC")
#endif

#ifdef __ATARIXL__
#pragma code-name("SHADOW_RAM")
#endif

// Apple II specific init function
#if defined __APPLE2__
	// Extern variables (see bitmap.c)	
	extern unsigned dhrLines[192];
	
	// Sprite data
	#define sprWIDTH 4	// Byte width of sprite (7 pixels)
	unsigned char sprCOL[SPRITE_NUM] = {0,0,0,0};	// Collision flags
	unsigned char sprEN[SPRITE_NUM] = {0,0,0,0}; 	// Enable status
	unsigned char spriteXS[SPRITE_NUM];				// Screen coordinates
	unsigned char spriteYS[SPRITE_NUM];	
	unsigned char spriteXB[SPRITE_NUM];				// Byte coordinates
	unsigned char* sprBG[SPRITE_NUM];	    		// Sprite background
	unsigned char sprROWS;
	unsigned int sprBLOCK;
	void InitSprites(unsigned char rows, unsigned char frames)
	{
		// Set sprite rows, frames and resulting block size (there are 4 offset blocks for each sprite)
		int i;
		sprROWS = rows;
		sprBLOCK = frames*sprROWS*sprWIDTH;
		for (i=0; i<SPRITE_NUM; i++) {
			sprBG[i] = (unsigned char*)malloc(4*rows);	// Byte length of 1 sprite: rows * 4 bytes (7 pixels)
		}
	}	
// Atari specific init function
#elif defined __ATARI__	
	// Sprite flicker data (see DLI.a65)
	void SetupFlickerDLI(void);
	extern unsigned int flickerFrame[10];
	extern unsigned char flickerMask[5], flickerX[10], flickerY[10], flickerColor[10], flickerRows;
	void InitSprites(unsigned char rows, unsigned char *spriteColors)
	{			
		// Reset Sprite Mask, Frames, Colors and Rows
		unsigned char i;
		for (i=0; i<5; i++) {
			flickerMask[i] = 0;
		}
		for (i=0; i<10; i++) {
			flickerFrame[i] = 0;
			flickerColor[i] = spriteColors[i];
		}
		flickerRows = rows;

		// Clear all PMG memory
		bzero(PMGRAM+768,0x500);
		
		// Setup ANTIC and GTIA
		POKE(54279, PMGRAM>>8); // Tell ANTIC where the PM data is located
		POKE(53277, 2+1);       // Tell GTIA to enable players + missile	
		POKE(623, 32+16+1);		// Tricolor players + enable fifth player + priority  (shadow for 53275)	
		
		// Setup flicker DLI
		SetupFlickerDLI();
	}
// Atmos specific init function
#elif defined __ATMOS__	
	#define sprWIDTH 2	// Byte width of sprite (12 pixels)
	unsigned char sprCOL[SPRITE_NUM] = {0,0,0,0};	// Collision flags
	unsigned char sprEN[SPRITE_NUM] = {0,0,0,0}; 	// Enable status
	unsigned char spriteXS[SPRITE_NUM], spriteYS[SPRITE_NUM];
	unsigned char *sprBG[SPRITE_NUM];
	unsigned char sprROWS, inkVAL;
	unsigned int bgPTR, sprPTR, scrPTR, inkPTR, colPTR;
	extern unsigned char ink1[20];	// see bitmap.c
	void InitSprites(unsigned char rows, unsigned char *spriteColors)
	{			
		// Load sprite sheet and assigned colors
		unsigned char i;
		SedoricRead("sprites.dat", (void*)SPRITERAM);
		colPTR = spriteColors;
		sprROWS = rows;
		
		// Assign memory for sprite background
		for (i=0; i<SPRITE_NUM; i++) {
			sprBG[i] = (unsigned char*)malloc(4*rows);		// Byte length of 1 sprite: num rows * 4 bytes (Attribute + 12 pixels + Attribute)
		}
	}
// C64 specific init function
#elif defined __CBM__
	void InitSprites(unsigned char *spriteColors, unsigned char *sharedColors)
	{		
		// Set sprite colors
		unsigned int i;	
		for (i=0; i<8; ++i) { POKE(53287+i, spriteColors[i]); }
		
		// Set common colors
		POKE(53285, sharedColors[0]);
		POKE(53286, sharedColors[1]);	
		
		// Set to multicolor code
		POKE(53276, 255);			
	}	
#endif

// Apple II specific background redrawing function
#if defined __APPLE2__
	unsigned char xO,yO,delta;
	unsigned char *sprPTR, *bgPTR;
	unsigned char xptr, yptr;
	void RestoreSprBG(unsigned char index)
	{
		// Restore entire sprite background
		yO = 0;
		xptr = spriteXB[index];
		yptr = spriteYS[index];
		bgPTR = sprBG[index];
		while (yO<sprROWS) {
			dhrptr = (unsigned char *) (dhrLines[yptr++] + xptr);
			*dhraux = 0;
			dhrptr[0] = bgPTR[0];
			dhrptr[1] = bgPTR[1];
			*dhrmain = 0;
			dhrptr[0] = bgPTR[2];		
			dhrptr[1] = bgPTR[3];
			bgPTR += sprWIDTH;
			yO++;
		}
	}
	void RestoreSprLine(unsigned char x, unsigned char y)
	{
		// Restore 1 line from sprite background
		unsigned char i;
		for (i=0; i<SPRITE_NUM; i++) {
			if (sprEN[i]) {
				xO = x-spriteXS[i];
				yO = y-spriteYS[i];
				if (xO<7 && yO<sprROWS) {
					xptr = spriteXB[i];
					yptr = spriteYS[i]+yO;
					bgPTR = sprBG[i]+yO*sprWIDTH;
					dhrptr = (unsigned char *) (dhrLines[yptr] + xptr);
					*dhraux = 0;
					dhrptr[0] = bgPTR[0];
					dhrptr[1] = bgPTR[1];
					*dhrmain = 0;
					dhrptr[0] = bgPTR[2];		
					dhrptr[1] = bgPTR[3];
					return;
				}
			}
		}		
	}
#endif

// Oric specific background redrawing function
#if defined __ATMOS__
  void MultiBlockCopy(void);	// (see memory.s)
  void RestoreSprBG(unsigned char index)
  {
	// Restore sprite background
	POKE(0x00, sprROWS); POKE(0x01, 4);					// Number of: blocks / bytes per block
	POKEW(0x02, sprBG[index] - 1);						// Address of first source block (-1)
	POKEW(0x04, BITMAPRAM + spriteYS[index]*40 + spriteXS[index] - 1);	// Address of first target block (-1)
	POKE(0x06, 4); POKE(0x07, 40); 						// Offset between: source blocks / target blocks
	MultiBlockCopy();
  }
#endif

#if defined __CBM__
	unsigned int spriteX;
	unsigned char spriteY;	
#else
	unsigned char spriteX,spriteY;
#endif

void LocateSprite(unsigned int x, unsigned int y)
{
// This function maps sprite coordinates from a 320x200 screen definition
// It can be by-passed by assigning spriteX, spriteY directly in your code
#if defined __APPLE2__
	spriteX = (x*140)/320;
	spriteY = (y*192)/200;
#elif defined __ATARI__
	spriteX = x/2 + 45;
	spriteY = y + 24;
#elif defined __ATMOS__
	spriteX = x/8;	
	spriteY = y;
#elif defined __CBM__
	spriteX = x;
	spriteY = y;
#endif	
}

void SetSprite(unsigned char index, unsigned char frame)
{
#if defined __APPLE2__
	unsigned char i;
	unsigned int addr;
	
	// Compute sprite slots
	xO = (2*spriteX)/7;

	// Select the correct offset block (4 offset blocks per 7 pixels)
	if (xO%2) {
		addr = frame*sprROWS*sprWIDTH + 2*sprBLOCK;
		if (spriteX%7 > 5) { addr += sprBLOCK; }
	} else {
		addr = frame*sprROWS*sprWIDTH;
		if (spriteX%7 > 3) { addr += sprBLOCK; }
	}

	// Check for collisions
	sprCOL[index] = 0;
	for (i=0; i<SPRITE_NUM; i++) {
		if (sprEN[i] && i!=index) {
			delta = spriteXB[i] - xO;
			if (delta < 2 || delta>254) {
				delta = spriteYS[i] - spriteY;
				if (delta < sprROWS || delta>(256-sprROWS)) {
					// Redraw background of that sprite
					RestoreSprBG(i);
					sprCOL[i] = 1;
					sprCOL[index] = 1;
				}
			}
		}
	}	

	// Restore old background?
	if (sprEN[index]) { RestoreSprBG(index); }
	
	// Backup new background and draw sprite
	yO = 0;
	yptr = spriteY;	
	sprPTR = SPRITERAM+addr;
	bgPTR = sprBG[index];
	while (yO<sprROWS) {
		dhrptr = (unsigned char *) (dhrLines[yptr++] + xO);
		*dhraux = 0;
		bgPTR[0]  = dhrptr[0];
		bgPTR[1]  = dhrptr[1];
		dhrptr[0] = sprPTR[0];
		dhrptr[1] = sprPTR[1];
		*dhrmain = 0;
		bgPTR[2]  = dhrptr[0];
		bgPTR[3]  = dhrptr[1];
		dhrptr[0] = sprPTR[2];		
		dhrptr[1] = sprPTR[3];		
		bgPTR += sprWIDTH;
		sprPTR += sprWIDTH;
		yO++;
	}
	
	// Set sprite information
	sprEN[index] = 1;
	spriteXB[index] = xO;
	spriteXS[index] = spriteX;
	spriteYS[index] = spriteY;
#elif defined __ATARI__
	flickerX[index] = spriteX;
	flickerY[index] = spriteY;
	flickerFrame[index] = SPRITERAM + frame*flickerRows;
#elif defined __ATMOS__	
	unsigned char i, delta;

	// Offset from centre of sprite
	spriteX -= 1; spriteY -= sprROWS/2;

	// Check for collisions
	sprCOL[index] = 0;
	for (i=0; i<SPRITE_NUM; i++) {
		if (sprEN[i] && i!=index) {
			delta = spriteYS[i] - spriteY;
			if (delta < sprROWS || delta>(256-sprROWS)) {
				delta = spriteXS[i] - spriteX;
				if (delta < 4 || delta>252) {	// Including INK bytes
					// Redraw background of that sprite
					RestoreSprBG(i);
					if (delta < 2 || delta>254) {	// Not including INK bytes
						// Register collision
						sprCOL[i] = 1;
						sprCOL[index] = 1;
					}
				}
			}
		}
	}	

	// Restore old background?
	if (sprEN[index]) { RestoreSprBG(index); }
	
	// Backup sprite background
	POKE(0x00, sprROWS); POKE(0x01, 4);					// Number of: blocks / bytes per block
	POKEW(0x02, BITMAPRAM + spriteY*40 + spriteX - 1);	// Address of first source block (-1)
	POKEW(0x04, sprBG[index] - 1);						// Address of first target block (-1)
	POKE(0x06, 40); POKE(0x07, 4); 						// Offset between: source blocks / target blocks
	MultiBlockCopy();

	// Draw sprite frame
	POKE(0x00, sprROWS); POKE(0x01, 2);					// Number of: blocks / bytes per block
	POKEW(0x02, SPRITERAM + frame*sprROWS*2 - 1);		// Address of first source block (-1)
	POKEW(0x04, BITMAPRAM + spriteY*40 + spriteX);		// Address of first target block (-1)
	POKE(0x06, 2); POKE(0x07, 40); 						// Offset between: source blocks / target blocks
	MultiBlockCopy();
	
	// Adjust ink on even lines
	scrPTR = BITMAPRAM + (spriteY + spriteY%2)*40 + spriteX;
	inkVAL = ink1[PEEK(colPTR+index)];
	for (i=0; i<sprROWS/2; i++) {
		POKE(scrPTR, inkVAL); scrPTR+=3;	// Set Sprite INK
		POKE(scrPTR, 3); scrPTR+=77;		// Reset AIC INK (yellow)
	}
	
	// Save sprite information
	sprEN[index] = 1;
	spriteXS[index] = spriteX;
	spriteYS[index] = spriteY;	
#elif defined __CBM__
	// Tell VIC where to find the frame
	POKE(SPRITEPTR+index, SPRITELOC+frame);

	// Set X/Y Coordinates
	POKE(53249+2*index, spriteY+39);
	if (spriteX < 244) {
		POKE(53248+2*index, spriteX+12);
		POKE(53264, PEEK(53264) & ~(1 << index));
	} else {
		POKE(53248+2*index, spriteX-244);
		POKE(53264, PEEK(53264) |  (1 << index));
	}	
#endif
}

void EnableSprite(signed char index)
{
#if defined __CBM__
	// Set sprite bits
	POKE(53269, PEEK(53269) |  (1 << index));
#elif defined __ATARI__
	// Set bit in flicker mask	
	if (index<5) { 
		flickerMask[index] |= 1;
	} else {
		flickerMask[index-5] |= 2;
	}
#endif
}

void DisableSprite(signed char index)
{
	// Switch single sprite off
	if (index >= 0) {	
#if defined __CBM__
		POKE(53269, PEEK(53269) & ~(1 << index));
#elif defined __ATARI__
		// Set bit in flicker mask	
		if (index<5) { 
			flickerMask[index] &= ~1;
		} else {
			flickerMask[index-5] &= ~2;
		}
		bzero(PMGRAM+768+((index+1)%5)*256,0x100); // Clear PMG slot
#else
		// Soft sprites: Restore background if neccessary
		if (sprEN[index]) { RestoreSprBG(index); }
		sprEN[index] = 0;
#endif
	// Switch all sprites off
	} else {
#if defined __CBM__
		// Set sprite bits
		POKE(53269, 0);
#elif defined __ATARI__
		bzero(flickerMask, 5);	 // Clear flicker mask
		bzero(PMGRAM+768,0x500); // Clear PMG memory
#else	
		// Soft sprites: Restore background if necessary
		for (index=0; index<SPRITE_NUM; index++) {
			if (sprEN[index]) { RestoreSprBG(index); }				
			sprEN[index] = 0;
		}
#endif
	}
}

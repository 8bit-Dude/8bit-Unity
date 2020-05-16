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
	// Sprite data
	#define frameWIDTH 4	// Byte width of sprite (7 pixels)
	unsigned char frameROWS;
    unsigned int  frameBLOCK;	// Size of sprite offset block (4 blocks)
	unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM];	 // Screen coordinates
	unsigned char sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM]; // Enable and Collision status
	unsigned char sprXDHR[SPRITE_NUM];  // Byte offset within DHR line
	unsigned char*sprBG[SPRITE_NUM];  // Sprite background
	unsigned char sprROWS[SPRITE_NUM];  // Sprite dimensions used in algorithms
	void InitSprites(unsigned char frames, unsigned char cols, unsigned char rows, unsigned char *spriteColors)
	{			
		// Set sprite rows, frames and resulting block size (there are 4 offset blocks for each sprite)
		unsigned char i;
		frameROWS = rows;
		frameBLOCK = frames*frameROWS*frameWIDTH;
		for (i=0; i<SPRITE_NUM; i++) sprROWS[i] = frameROWS;
	}	
	void CropSprite(unsigned char index, unsigned char rows) 
	{
		// Only partially draw this sprite
		sprROWS[index] = rows;
	}	
// Atari specific init function
#elif defined __ATARI__	
	// Sprite flicker data (see DLI.a65)
	void SetupFlickerDLI(void);
	extern unsigned int flickerFrame[10];
	extern unsigned char flickerMask[5], flickerX[10], flickerY[10], flickerColor[10], flickerRows;
	void InitSprites(unsigned char frames, unsigned char cols, unsigned char rows, unsigned char *spriteColors)
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
#elif defined __ORIC__	
	#define frameWIDTH 2	// Byte width of sprite (12 pixels)
	unsigned char frameROWS;
	unsigned int  frameBLOCK;	// Size of sprite offset block (4 blocks), 
	unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM];	   // Screen coordinates
	unsigned char sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM]; // Enable Flag, Collision status
	unsigned char sprROWS[SPRITE_NUM], *sprBG[SPRITE_NUM]; // Height of Sprite (rows), Pointer to background backup
	unsigned int  scrAddr[SPRITE_NUM], sprCOLOR;  // Screen address, Color vector
	extern unsigned char ink1[20];	// see bitmap.c
	void InitSprites(unsigned char frames, unsigned char cols, unsigned char rows, unsigned char *spriteColors)
	{			
		// Load sprite sheet
		unsigned char i;
		FileRead("sprites.dat", (void*)SPRITERAM);
		
		// Assign frame info and sprite colors
		frameROWS = rows;
		frameBLOCK = frames*frameROWS*frameWIDTH;
		for (i=0; i<SPRITE_NUM; i++) sprROWS[i] = rows;
		sprCOLOR = spriteColors;
	}
	void CropSprite(unsigned char index, unsigned char rows) 
	{
		// Only partially draw this sprite
		sprROWS[index] = rows;
	}
// C64 specific init function
#elif defined __CBM__
	void InitSprites(unsigned char frames, unsigned char cols, unsigned char rows, unsigned char *spriteColors)
	{			
		// Set sprite colors
		unsigned int i;	
		for (i=0; i<8; ++i) { POKE(53287+i, spriteColors[i]); }
		
		// Set common colors
		POKE(53285, spriteColors[8]);
		POKE(53286, spriteColors[9]);	
		
		// Set to multicolor code
		POKE(53276, 255);			
	}	
// Lynx specific init function
#elif defined __LYNX__	
	// declare RO and TGI data
	unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM];	   // Screen coordinates
	unsigned char sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM]; // Enable and Collision status
	unsigned char sprCOLS, sprROWS;					       // Sprite dimensions
	SCB_REHV_PAL sprTGI[SPRITE_NUM];					   // Frame data
	extern unsigned int spriteData[]; 
	void InitSprites(unsigned char frames, unsigned char cols, unsigned char rows, unsigned char *spriteColors)
	{
		unsigned char i,j;
		SCB_REHV_PAL *scb;
		sprCOLS = cols; sprROWS = rows;		
		for (i=0; i<SPRITE_NUM; i++) {
			scb = &sprTGI[i];
			scb->sprctl0 = BPP_4 | TYPE_NONCOLL;
			scb->sprctl1 = REHV | LITERAL;
			scb->sprcoll = 0;
			scb->next = 0;
			scb->data = 0;
			scb->hpos = 0;
			scb->vpos = 0;
			scb->hsize = 0x0100;
			scb->vsize = 0x0100;
			for (j=0; j<8; j++) {
				scb->penpal[j] = spriteColors[i*8+j];
			}
		}
	}
#endif

void RecolorSprite(unsigned char index, unsigned char number, unsigned char color)
{
#if defined __ATARI__
	flickerColor[index] = color;
#elif defined __ORIC__
	POKE(sprCOLOR+index, color);
#elif defined __CBM__
	POKE(53287+index, color);
#elif defined __LYNX__
	sprTGI[index].penpal[number] = color;
#endif
}

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
#elif defined __ORIC__
	spriteX = x/4;	
	spriteY = y;
#elif defined __CBM__
	spriteX = x;
	spriteY = y;
#elif defined __LYNX__
	spriteX = x/2;
	spriteY = (y*102)/200;
#endif
}

// Apple II specific background redrawing function
#if defined __APPLE2__
  unsigned char xDHR, yDHR, xptr, yptr, *bgPTR;
  void __fastcall__ Blit(void);	// (see Apple/blit.s)
  unsigned int DHRLine(unsigned char y);
  void RestoreSprBG(unsigned char index)
  {
	  POKE(0xE3, 2);				// Bytes per line (x2 for MAIN/AUX)	
	  POKE(0xEB, sprROWS[index]);	// Number of lines
	  POKE(0xEC, sprXDHR[index]);	// DHR Offset X
	  POKE(0xED, sprY[index]);		// DHR Offset Y
	  POKEW(0xEE, 0);				// Address for copying DHR > Output
	  POKEW(0xFA, sprBG[index]);	// Address for copying Input > DHR
	  Blit();
  }
  void RestoreSprLine(unsigned char x, unsigned char y)
  {
	  // Restore 1 line from sprite background
	  unsigned char i;
	  for (i=0; i<SPRITE_NUM; i++) {
		  if (sprDrawn[i]) {
			  xDHR = x-sprX[i];
			  yDHR = y-sprY[i];
			  if (xDHR<7 && yDHR<sprROWS[i]) {
				  xptr = sprXDHR[i];
				  yptr = sprY[i]+yDHR;
				  bgPTR = sprBG[i]+yDHR*frameWIDTH;				  
				  dhrptr = DHRLine(yptr) + xptr;
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
#if defined __ORIC__
  void __fastcall__ Blit(void);	// (see Oric/blit.s)
  void RestoreSprBG(unsigned char index)
  {
	// Restore sprite background
	POKE(0xb0, sprROWS[index]); 	// Number of lines
	POKE(0xb1, 4);					// Bytes per line
	POKEW(0xb2, sprBG[index]-1);	// Address of source (-1)
	POKEW(0xb4, scrAddr[index]-1);	// Address of target (-1)
	POKE(0xb6, 4); 					// Offset between source lines
	POKE(0xb7, 40); 				// Offset between target lines
	Blit();
  }
#endif


#if (defined __APPLE2__) || (defined __ORIC__)  || (defined __LYNX__)
  void SpriteCollisions(unsigned char index)
  {
	unsigned char i, dX, dY, rows;
  #if defined __ORIC__	
	unsigned char x1, x2, y1, y2;
	unsigned int bgPTR1, bgPTR2;
  #endif					
	
	// Check for collisions
	sprCollision[index] = 0;
	for (i=0; i<SPRITE_NUM; i++) {
		// Should this sprite be checked?
		if (i == index) { continue; }
		if (!sprDrawn[i]) { continue; }
		
		// Check Y distance
		dY = sprY[i] - spriteY;
	#if defined __APPLE2__
		rows = MAX(sprROWS[index], sprROWS[i]);
		if (dY < rows || dY>(256-rows)) {
			dX = sprXDHR[i] - xDHR;
			if (dX < 2 || dX>254) {
				// Apply collision
				sprCollision[index] |= 1 << i;
				sprCollision[i] |= 1 << index;
				RestoreSprBG(i);
			}
		}
	#elif defined __ORIC__
		rows = MAX(sprROWS[index], sprROWS[i]);
		if (dY < rows || dY>(256-rows)) {
			dX = sprX[i] - spriteX;		
			if (dX < 4 || dX>252) {	// Including INK bytes
				// Check narrower collision sector
				if (dX < 2 || dX>254) {	// Not including INK bytes
					sprCollision[index] |= 1 << i;
					sprCollision[i] |= 1 << index;
				}					
			
				// Define X overlap
				if (dX < 4) { 
					x1 = dX; x2 = 4; 
				} else { 
					x1 = 0; x2 = 4+dX; 
				}
				
				// Define Y overlap
				if (dY < rows) { 
					y1 = dY; y2 = rows; 
				} else { 
					y1 = 0; y2 = rows+dY; 
				}
				
				// Copy overlapping background
				bgPTR1 = sprBG[index] + y1*4;
				bgPTR2 = sprBG[i] + (rows-y2)*4 + (4-x2) - x1;
				for (dY=y1; dY<y2; dY++) {
					for (dX=x1; dX<x2; dX++) {
						POKE(bgPTR1+dX, PEEK(bgPTR2+dX));	
					}
					bgPTR1 += 4;
					bgPTR2 += 4;
				}					
			}
		}
	#elif defined __LYNX__
		if (dY < (sprROWS-3) || dY>(256-(sprROWS-3))) {
			dX = sprX[i] - spriteX;
			if (dX < (sprCOLS-3) || dX>(256-(sprCOLS-3))) {
				// Apply collision
				sprCollision[index] |= 1 << i;
				sprCollision[i] |= 1 << index;
			}				
		}
	#endif					
	}
  }
#endif

void SetSprite(unsigned char index, unsigned char frame)
{
#if defined __APPLE2__
	unsigned int frameAddr;
	unsigned char rows = sprROWS[index];
	
	// Offset from centre of sprite
	if (spriteX > 2) { spriteX -= 3; }
	spriteY -= rows/2;
	
	// Compute sprite slots
	xDHR = (2*spriteX)/7;

	// Select the correct offset block (4 offset blocks per 7 pixels)
	frameAddr = SPRITERAM + frame*frameROWS*frameWIDTH;
	if (xDHR%2) {
		if (spriteX%7 > 5) { 
			frameAddr += 3*frameBLOCK; 
		} else {
			frameAddr += 2*frameBLOCK;
		}
	} else {
		if (spriteX%7 > 3) { 
			frameAddr += frameBLOCK; 
		}
	}
	
	// Check that sprite was enabled
	if (!sprBG[index]) EnableSprite(index);

	// Restore background if sprite was previously drawn
	if (sprDrawn[index]) RestoreSprBG(index);

	// Check collisions with other sprites
	SpriteCollisions(index);	
		
	// Backup new background and draw sprite
	POKE(0xE3, 2);				// Bytes per line (x2 for MAIN/AUX)
	POKE(0xEB, rows);			// Number of lines
	POKE(0xEC, xDHR);			// DHR Offset X
	POKE(0xED, spriteY);		// DHR Offset Y
	POKEW(0xEE, sprBG[index]);	// Address for copying DHR > Output
	POKEW(0xFA, frameAddr);		// Address for copying Input > DHR
	Blit();

	// Set sprite information
	sprXDHR[index] = xDHR;
	sprX[index] = spriteX;
	sprY[index] = spriteY;
	sprDrawn[index] = 1;	
	
#elif defined __ATARI__
	flickerX[index] = spriteX;
	flickerY[index] = spriteY;
	flickerFrame[index] = SPRITERAM + frame*flickerRows;
	
#elif defined __ORIC__
	unsigned char i, inkVAL;
	unsigned int frameAddr, inkAddr;
	unsigned char rows = sprROWS[index];
	
	// Check frame block (left or right)
	frameAddr = SPRITERAM + frame*frameROWS*2;
	if (spriteX%2) { frameAddr += frameBLOCK; }
	spriteX /= 2;

	// Offset from centre of sprite
	if (spriteX > 0) { spriteX -= 1; }
	spriteY -= rows/2;
	
	// Check that sprite was enabled
	if (!sprBG[index]) EnableSprite(index);

	// Restore background if sprite was previously drawn
	if (sprDrawn[index]) RestoreSprBG(index);
	
	// Compute new memory address
	scrAddr[index] = BITMAPRAM + spriteY*40 + spriteX;
	
	// Backup new background
	POKE(0xb0, frameROWS); 			// Number of lines
	POKE(0xb1, 4);					// Bytes per line
	POKEW(0xb2, scrAddr[index]-1);	// Address of source (-1)
	POKEW(0xb4, sprBG[index]-1);	// Address of target (-1)
	POKE(0xb6, 40); 				// Offset between source lines
	POKE(0xb7, 4); 					// Offset between target lines
	Blit();	
	
	// Draw new sprite frame
	POKE(0xb0, rows); 				// Number of lines
	POKE(0xb1, 2);					// Bytes per line
	POKEW(0xb2, frameAddr-1);		// Address of source (-1)
	POKEW(0xb4, scrAddr[index]);	// Address of target (-1)
	POKE(0xb6, 2);					// Offset between source lines
	POKE(0xb7, 40); 				// Offset between target lines
	Blit();		
	
	// Adjust ink on even lines
	if (PEEK(sprCOLOR+index) != AIC) {
		inkAddr = scrAddr[index] + (spriteY%2)*40;
		inkVAL = ink1[PEEK(sprCOLOR+index)];
		for (i=0; i<rows/2; i++) {
			POKE(inkAddr, inkVAL); inkAddr+=3;	// Set Sprite INK
			POKE(inkAddr, 3); inkAddr+=77;		// Reset AIC INK (yellow)
		}		
	}
	
	// Check collisions with other sprites
	SpriteCollisions(index);
	
	// Save sprite information
	sprX[index] = spriteX;
	sprY[index] = spriteY;
	sprDrawn[index] = 1;
	
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
#elif defined __LYNX__
	// Set sprite data for Suzy
	SCB_REHV_PAL *scb;
	scb = &sprTGI[index];
	scb->data = spriteData[frame];
	scb->hpos = spriteX;
	scb->vpos = spriteY;
	
	// Check collisions with other sprites
	SpriteCollisions(index);

	// Save sprite information
	sprX[index] = spriteX;
	sprY[index] = spriteY;
	sprDrawn[index] = 1;	
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
#elif (defined __APPLE2__) || (defined __ORIC__)
	// Allocate memory for background
	if (!sprBG[index]) {
		sprBG[index] = (unsigned char*)malloc(4*frameROWS);	// Byte length of 1 sprite
		sprDrawn[index] = 0;
	}
#endif
}

#if (defined __APPLE2__) || (defined __ORIC__)
  void EraseSprite(signed char index) {
	if (sprBG[index]) {
		if (sprDrawn[index]) RestoreSprBG(index); 
		free(sprBG[index]);
		sprBG[index] = 0;
	}
  }
#endif

void DisableSprite(signed char index)
{
	// Switch single sprite off
	if (index >= 0) {	
	#if defined __CBM__
		// Set bit in sprite register
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
	  #if (defined __APPLE2__) || (defined __ORIC__)
		EraseSprite(index);
	  #endif
		sprDrawn[index] = 0;
	#endif
	// Switch all sprites off
	} else {
	#if defined __CBM__
		// Reset sprite register
		POKE(53269, 0);
	#elif defined __ATARI__
		bzero(flickerMask, 5);	 // Clear flicker mask
		bzero(PMGRAM+768,0x500); // Clear PMG memory
	#else
		// Soft sprites: Restore background if necessary
		for (index=0; index<SPRITE_NUM; index++) {
		#if (defined __APPLE2__) || (defined __ORIC__)
			EraseSprite(index);
		#endif
			sprDrawn[index] = 0;
		}
	#endif
	}
}

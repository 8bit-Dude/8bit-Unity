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
  #pragma code-name("LOWCODE")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

// Platform specific function
#if defined __APPLE2__
  #if defined __DHR__
	#define frameWIDTH 4		// 4 bytes per 7 pixels
  #else
    #define frameWIDTH 2		// 2 bytes per 7 pixels
  #endif
	unsigned char  frameROWS;
    unsigned int   frameBLOCK;				// Size of sprite offset block (4 blocks)
	unsigned char* sprData;					// Pointer to sprite data (allocated dynamically)
	unsigned char  sprX[SPRITE_NUM], sprY[SPRITE_NUM];	 // Screen coordinates
	unsigned char  sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM]; // Enable and Collision status
	unsigned char  sprHiresX[SPRITE_NUM];  	// Byte offset within Hires line
	unsigned char* sprBG[SPRITE_NUM];  		// Sprite background
	unsigned char  sprROWS[SPRITE_NUM];  	// Sprite dimensions used in algorithms
	void CropSprite(unsigned char index, unsigned char rows) {
		// Only partially draw this sprite
		sprROWS[index] = rows;
	}	
	
#elif defined __ATARI__	
	unsigned char* sprData;					// Pointer to sprite data (allocated dynamically)
	unsigned char sprYOffset;
	extern unsigned int sprFrame[10];
	extern unsigned char sprRows, sprMask[5], sprX[10], sprY[10], sprColor[10];
	void DoubleHeightSprite(unsigned char index, unsigned char onoff) {
		extern unsigned char doubleHeight[10];
		doubleHeight[index] = onoff;
		if (onoff) sprYOffset = sprRows; else sprYOffset = sprRows/2;
	}

#elif defined __ORIC__	
	#define frameWIDTH 2		// Byte width of sprite (12 pixels)
	unsigned char frameROWS;
	unsigned int  frameBLOCK;	// Size of sprite offset block (4 blocks), 
	unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM];	   // Screen coordinates
	unsigned char sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM]; // Enable Flag, Collision status
	unsigned char sprROWS[SPRITE_NUM], *sprBG[SPRITE_NUM]; // Height of Sprite (rows), Pointer to background backup
	unsigned int  scrAddr[SPRITE_NUM], sprCOLOR;  // Screen address, Color vector
	unsigned char* sprMULTICOLOR[SPRITE_NUM];
	extern unsigned char ink1[20];	// see bitmap.c
	void CropSprite(unsigned char index, unsigned char rows) {
		// Only partially draw this sprite
		sprROWS[index] = rows;
	}
	void MultiColorSprite(unsigned char index, unsigned char* multiColorDef) {
		// Assign multicolor definition to sprite { color, row, ... color, lastrow }
		sprMULTICOLOR[index] = multiColorDef;
	}

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
	unsigned char defaultColors[] =  { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									   0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   
									   0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   
									   0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   
									   0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   
									   0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   
									   0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   
									   0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }; 
	unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM];	// Screen coordinates
	unsigned char sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM]; // Enable and Collision status
	unsigned char sprCOLS, sprROWS;					    // Sprite dimensions
	SCB_REHV_PAL sprSCB[SPRITE_NUM];					// Frame data
	extern unsigned int spriteData[]; 
	void ScaleSprite(unsigned char index, unsigned int xPercent, unsigned int yPercent) {
		SCB_REHV_PAL *scb;
		scb = &sprSCB[index];
		scb->hsize = xPercent;
		scb->vsize = yPercent;
	}
#endif

void LoadSprites(unsigned char* filename)
{
#if defined(__APPLE2__)
	unsigned int size;
	FILE* fp = fopen(filename, "rb");
	fread(&size, 1, 2, fp);
	if (sprData) free(sprData);
	sprData = malloc(size);
	fread(sprData, 1, size, fp);
	fclose(fp);

#elif defined __ATARI__	
	unsigned int size;
	if (FileOpen(filename)) {
		FileRead(&size, 2);
		if (sprData) free(sprData);
		sprData = malloc(size);
		FileRead(sprData, -1);
	}

#elif defined(__CBM__)
	// NEED SOLUTION: sprite sheets larger than $700
	// can be loaded by exomizer, but not cc65!
	// (file loading under ROM not possible)
	
#elif defined __ORIC__	
	FileRead(filename, (void*)SPRITERAM);
#endif
}

void SetupSprites(unsigned char frames, unsigned char cols, unsigned char rows, unsigned char *spriteColors)
{
#if defined __APPLE2__	
	// Set sprite rows, frames and resulting block size (there are 4 offset blocks for each sprite)
	unsigned char i;
	frameROWS = rows;
	frameBLOCK = frames*frameROWS*frameWIDTH;
	for (i=0; i<SPRITE_NUM; i++) 
		sprROWS[i] = frameROWS;
	
#elif defined __ATARI__	
	// Reset Sprite Mask, Frames, Colors and Rows
	unsigned char i;
	for (i=0; i<5; i++)
		sprMask[i] = 0;
	for (i=0; i<10; i++) {
		sprFrame[i] = 0;
		sprColor[i] = spriteColors[i];
	}
	sprRows = rows;
	sprYOffset = rows/2u;

	// Clear all PMG memory
	bzero(PMGRAM+768,0x500);
	
	// Setup ANTIC and GTIA
	POKE(54279, PMGRAM>>8); // Tell ANTIC where the PM data is located
	POKE(53277, 2+1);       // Tell GTIA to enable players + missile	
	POKE(623, 32+16+1);		// Tricolor players + enable fifth player + priority  (shadow for 53275)	
	
	// ANTIC settings: Enable P/M + DMA Players + DMA Missiles + Standard Playfield
	POKE(559, PEEK(559) | (16+8+4+2));
	
	// Setup flicker DLI for sprites
	spriteDLI = 1; StartDLI();
	
#elif defined __CBM__
	// Set sprite colors
	unsigned int i;	
	for (i=0; i<8; ++i)
		POKE(53287+i, spriteColors[i]);
	
	// Set common colors
	POKE(53285, spriteColors[8]);
	POKE(53286, spriteColors[9]);	
	
	// Set to multicolor code
	POKE(53276, 255);

#elif defined __ORIC__	
	// Assign frame info and sprite colors
	unsigned char i;
	frameROWS = rows;
	frameBLOCK = frames*frameROWS*frameWIDTH;
	for (i=0; i<SPRITE_NUM; i++) 
		sprROWS[i] = rows;
	sprCOLOR = spriteColors;

#elif defined __LYNX__	
	unsigned char i,j;
	SCB_REHV_PAL *scb;
	sprCOLS = cols; sprROWS = rows;		
	for (i=0; i<SPRITE_NUM; i++) {
		scb = &sprSCB[i];
		scb->sprctl0 = BPP_4 | TYPE_NONCOLL;
		scb->sprctl1 = REHV | LITERAL;
		scb->sprcoll = 0;
		scb->next = 0;
		scb->data = 0;
		scb->hpos = 0;
		scb->vpos = 0;
		scb->hsize = 0x0100;
		scb->vsize = 0x0100;
		for (j=0; j<8; j++)
			scb->penpal[j] = spriteColors[i*8+j];
	}	
#endif
}	

void RecolorSprite(unsigned char index, unsigned char offset, unsigned char color)
{
#if defined __ATARI__
	sprColor[index] = color;
#elif defined __ORIC__
	POKE(sprCOLOR+index, color);
#elif defined __CBM__
	POKE(53287+index, color);
#elif defined __LYNX__
	sprSCB[index].penpal[offset] = color;
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
	spriteX = (x*140)/320u;
	spriteY = (y*192)/200u;
#elif defined __ATARI__
	spriteX = x/2u + 45;
	spriteY = y + 32;
#elif defined __ORIC__
	spriteX = x/4u;	
	spriteY = y;
#elif defined __CBM__
	spriteX = x;
	spriteY = y;
#elif defined __LYNX__
	spriteX = x/2u;
	spriteY = (y*102)/200u;
#endif
}

// Apple II specific background redrawing function
#if defined __APPLE2__
  unsigned char xHires, yHires, xptr, yptr, *bgPtr;
  void RestoreSprBG(unsigned char index)
  {
	  POKE(0xE3, 2);				// Bytes per line (x2 for MAIN/AUX)	
	  POKE(0xEB, sprROWS[index]);	// Number of lines
	  POKE(0xEC, sprHiresX[index]);	// Hires Offset X
	  POKE(0xED, sprY[index]);		// Hires Offset Y
	  POKEW(0xEE, 0);				// Address for copying Hires > Output
	  POKEW(0xFA, sprBG[index]);	// Address for copying Input > DHR
    #if defined __DHR__	  
	  BlitDHR();
	#else
	  BlitSHR();
    #endif		
  }
  void RestoreSprLine(unsigned char x, unsigned char y)
  {
	  // Restore 1 line from sprite background
	  unsigned char i;
	  for (i=0; i<SPRITE_NUM; i++) {
		  if (sprDrawn[i]) {
			  xHires = x-sprX[i];
			  yHires = y-sprY[i];
			  if (xHires<7 && yHires<sprROWS[i]) {
				  xptr = sprHiresX[i];
				  yptr = sprY[i]+yHires;
				  bgPtr = sprBG[i]+yHires*frameWIDTH;				  
				  hiresPtr = HiresLine(yptr) + xptr;
				#if defined __DHR__  
				  *dhraux = 0;
				  hiresPtr[0] = bgPtr[0];
				  hiresPtr[1] = bgPtr[1];
				  *dhrmain = 0;
				#endif
				  hiresPtr[0] = bgPtr[2];		
				  hiresPtr[1] = bgPtr[3];
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
	unsigned int bgPtr1, bgPtr2;
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
			dX = sprHiresX[i] - xHires;
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
				bgPtr1 = sprBG[index] + y1*4;
				bgPtr2 = sprBG[i] + (rows-y2)*4 + (4-x2) - x1;
				for (dY=y1; dY<y2; dY++) {
					for (dX=x1; dX<x2; dX++)
						POKE(bgPtr1+dX, PEEK(bgPtr2+dX));	
					bgPtr1 += 4;
					bgPtr2 += 4;
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
	spriteY -= rows/2u;
	
	// Compute sprite slots
	xHires = (spriteX*2)/7u;

	// Select the correct offset block (4 offset blocks per 7 pixels)
	frameAddr = (char*)(sprData) + frame*frameROWS*frameWIDTH;
	if (xHires%2) {
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
	POKE(0xEC, xHires);			// Hires Offset X
	POKE(0xED, spriteY);		// Hires Offset Y
	POKEW(0xEE, sprBG[index]);	// Address for copying Hires > Output
	POKEW(0xFA, frameAddr);		// Address for copying Input > Hires
  #if defined __DHR__	
	BlitDHR();
  #else
	BlitSHR();  
  #endif	  

	// Set sprite information
	sprHiresX[index] = xHires;
	sprX[index] = spriteX;
	sprY[index] = spriteY;
	sprDrawn[index] = 1;	
	
#elif defined __ATARI__
	sprX[index] = spriteX;
	sprY[index] = spriteY-sprYOffset;
	sprFrame[index] = &sprData[frame*sprRows];
	
#elif defined __ORIC__
	unsigned char i, inkVAL, inkMUL;
	unsigned int frameAddr, inkAddr;
	unsigned char rows = sprROWS[index];
	
	// Check frame block (left or right)
	frameAddr = SPRITERAM + frame*frameROWS*2;
	if (spriteX%2) { frameAddr += frameBLOCK; }
	spriteX /= 2u;

	// Offset from centre of sprite
	if (spriteX > 0) 
		spriteX -= 1;
	else 
		spriteX = 0;
	spriteY -= rows/2u;
	
	// Make sure we do not print on line borders
	if (spriteX > 37) spriteX = 37;
	
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
	if (sprMULTICOLOR[index] || PEEK(sprCOLOR+index) != SPR_AIC) {
		inkAddr = scrAddr[index] + (spriteY%2)*40;
		if (sprMULTICOLOR[index]) {
			inkVAL = PEEK(sprMULTICOLOR[index]);
			inkMUL = 1;
		} else {
			inkVAL = PEEK(sprCOLOR+index);
		}
		for (i=0; i<rows/2u; i++) {
			if (sprMULTICOLOR[index] && i*2 > PEEK(sprMULTICOLOR[index]+inkMUL)) {
				inkVAL = PEEK(sprMULTICOLOR[index]+inkMUL+1);
				inkMUL += 2;
			}
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
	scb = &sprSCB[index];
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
		sprMask[index] |= 1;
	} else {
		sprMask[index-5] |= 2;
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
#if defined __ATARI__	
	unsigned char state;
#endif
	if (index >= 0) {	
	#if defined __CBM__
		// Set bit in sprite register
		POKE(53269, PEEK(53269) & ~(1 << index));
	#elif defined __ATARI__
		// Set bit in flicker mask	
		if (index<5) {
			state = sprMask[index];
			sprMask[index] &= ~1;
			if (state == sprMask[index])
				return;
		} else {
			state = sprMask[index-5];
			sprMask[index-5] &= ~2;
			if (state == sprMask[index-5])
				return;
		}
		// Clear PMG slot
		bzero(PMGRAM+768+((index+1)%5)*256,0x100);
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
		bzero(sprMask, 5);	 // Clear flicker mask
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

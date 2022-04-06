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

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

// Apple II specific background redrawing function
#if defined __APPLE2__
  unsigned char xHires, yHires, xptr, yptr, *bgPtr;
  void RestoreSprBG(unsigned char index)
  {
	  POKE(0xE3, 2);				// Bytes per line (x2 for MAIN/AUX)	
	  POKE(0xCE, sprRows[index]);	// Number of lines Hires > Output
	  POKE(0xEB, sprRows[index]);	// Number of lines Input > Hires
	  POKE(0xEC, sprHiresX[index]);	// Hires Offset X
	  POKE(0xED, sprY[index]);		// Hires Offset Y
	  POKEW(0xEE, 0);				// Disable copying Hires > Output
	  POKEW(0xFA, sprBG[index]);	// Address for copying Input > Hires
	  BlitSprite();
  }
#endif

// Oric specific background redrawing function
#if defined __ORIC__
  void __fastcall__ BlitSprite(void);
  void BackupSprBG(unsigned char index) {
	// Backup new background
	POKE(0xb0, SPRITEHEIGHT); 		// Number of lines
	POKE(0xb1, 4);					// Bytes per line
	POKEW(0xb2, scrAddr[index]-1);	// Address of source (-1)
	POKEW(0xb4, sprBG[index]-1);	// Address of target (-1)
	POKE(0xb6, 40); 				// Offset between source lines
	POKE(0xb7, 4); 					// Offset between target lines
	BlitSprite();	
  }	
  void RestoreSprBG(unsigned char index) {
	// Restore sprite background
	POKE(0xb0, sprRows[index]); 	// Number of lines
	POKE(0xb1, 4);					// Bytes per line
	POKEW(0xb2, sprBG[index]-1);	// Address of source (-1)
	POKEW(0xb4, scrAddr[index]-1);	// Address of target (-1)
	POKE(0xb6, 4); 					// Offset between source lines
	POKE(0xb7, 40); 				// Offset between target lines
	BlitSprite();
  }
#endif

#if defined(__C64__)	// Other platforms don't have HW sprites, or not compatible with multiplexing (e.g. Atari)	
  unsigned long sprClock;
  unsigned char sprCollision;
  unsigned char COLLISIONS(unsigned char i) {
	unsigned char mask;  
	// Check clock (HW register can only be read once per frame)
	if (sprClock != clock()) {
		sprClock = clock();
		sprCollision = PEEK(53278);
	}
	
	// Check that sprite bit
	mask = (1 << i);
	if (sprCollision & mask)
		// Return state of other collisions
		return (sprCollision & ~mask);
	else
		// No collisions
		return 0;
  }
#else
  unsigned char sc_dX, sc_dY;
 #if (defined __APPLE2__) || (defined __ORIC__)
  unsigned char sc_x1, sc_x2, sc_y1, sc_y2, sc_rows;
  unsigned int sc_bgPtr1, sc_bgPtr2;
 #elif (defined __ATARI__) || (defined __LYNX__) || (defined __NES__)
  unsigned char sc_cushion;
 #endif		
  void SpriteCollisions(unsigned char index) {
	unsigned char i;
	// Check for collisions
  #if (defined __ORIC__)
	sprOverlap[index] = 0;
  #endif	
	sprCollision[index] = 0;
	for (i=0; i<SPRITE_NUM; i++) {
		// Should this sprite be checked?
		if (i == index || !sprDrawn[i]) { continue; }
		
		// Check Y distance
	#if defined __APPLE2__
		sc_dY = sprY[i] - spriteY;
		sc_rows = MAX(sprRows[index], sprRows[i]);
		if (sc_dY < sc_rows || sc_dY>(256-sc_rows)) {
			
			sc_dX = sprHiresX[i] - xHires;
			if (sc_dX < 2 || sc_dX>254) {
				
				// Apply collision
				sprCollision[index] |= 1 << i;
				sprCollision[i] |= 1 << index;
				
				// Define X overlap
				if (sc_dX < 2) { 
					sc_x1 = sc_dX; sc_x2 = 2; 
				} else { 
					sc_x1 = 0; sc_x2 = 2+sc_dX; 
				}
				sc_dX = (sc_x2-sc_x1);
				
				// Define Y overlap
				if (sc_dY < sc_rows) { 
					sc_y1 = sc_dY; sc_y2 = sc_rows; 
				} else { 
					sc_y1 = 0; sc_y2 = sc_rows+sc_dY; 
				}
				
				// Copy overlapping background
				sc_bgPtr1 = sprBG[index] + sc_y1*BYTEWIDTH + sc_x1;
				sc_bgPtr2 = sprBG[i] + (sc_rows-sc_y2)*BYTEWIDTH + (2 - sc_x2);
				for (sc_dY=sc_y1; sc_dY<sc_y2; sc_dY++) {
				  #if defined __DHR__  
				    AuxToAux(sc_bgPtr1, sc_bgPtr2, sc_dX); // AUX data
				  #endif				
					memcpy(sc_bgPtr1, sc_bgPtr2, sc_dX);   // MAIN data
					sc_bgPtr1 += BYTEWIDTH; 
					sc_bgPtr2 += BYTEWIDTH;
				}				
			}
		}
	#elif defined __ORIC__
		sc_dY = sprY[i] - spriteY;
		sc_rows = MAX(sprRows[index], sprRows[i]);
		if (sc_dY < sc_rows || sc_dY>(256-sc_rows)) {
			
			sc_dX = sprX[i] - spriteX;		
			if (sc_dX < 4 || sc_dX>252) {	// Including INK bytes
			
				// Overlapping within Attribute area
				sprOverlap[index] = 1;
				
				// Check narrower collision sector
				if (sc_dX < 2 || sc_dX>254) {	// Not including INK bytes
					sprCollision[index] |= 1 << i;
					sprCollision[i] |= 1 << index;
				}					
			
				// Define X overlap (including 2 attribute bytes on L/R of sprite)
				if (sc_dX < 4) { 
					sc_x1 = sc_dX; sc_x2 = 4;
				} else { 
					sc_x1 = 0; sc_x2 = 4+sc_dX; 
				}
				sc_dX = (sc_x2-sc_x1);
				
				// Define Y overlap
				if (sc_dY < sc_rows) { 
					sc_y1 = sc_dY; sc_y2 = sc_rows; 
				} else { 
					sc_y1 = 0; sc_y2 = sc_rows+sc_dY; 
				}
				
				// Copy overlapping background
				sc_bgPtr1 = sprBG[index] + sc_y1*4 + sc_x1;
				sc_bgPtr2 = sprBG[i] + (sc_rows-sc_y2)*4 + (4-sc_x2);
				for (sc_dY=sc_y1; sc_dY<sc_y2; sc_dY++) {
					memcpy(sc_bgPtr1, sc_bgPtr2, sc_dX);
					sc_bgPtr1 += 4; 
					sc_bgPtr2 += 4;
				}					
			}
		}
	#elif (defined __ATARI__) || (defined __LYNX__) || (defined __NES__)
	  #if defined(__NES__)
		sc_cushion = SPRITEHEIGHT/2-sprCushion;
	  #else
		sc_cushion = SPRITEHEIGHT-sprCushion;
	  #endif
		sc_dY = sprY[i] - spriteY;
		if (sc_dY < sc_cushion || sc_dY>(256-sc_cushion)) {
		  #if defined(__NES__)
			sc_cushion = SPRITEWIDTH/2-sprCushion;
		  #else
			sc_cushion = SPRITEWIDTH-sprCushion;
		  #endif
			sc_dX = sprX[i] - spriteX;
			if (sc_dX < sc_cushion || sc_dX>(256-sc_cushion)) {
				
				// Apply collision
				sprCollision[index] |= 1 << i;
				sprCollision[i] |= 1 << index;
			}				
		}
	#endif					
	}
  }
#endif

#if defined __ATARI__
  unsigned char ss_dli, ss_lin, ss_pmg, ss_pos, ss_off;
  unsigned int ss_src, ss_dst;
#endif
	
void SetSprite(unsigned char index, unsigned int frame)
{
#if defined __APPLE2__
	unsigned int frameAddr;
	unsigned char rows = sprRows[index];
	
	// Offset from centre of sprite
	if (spriteX > 2) { spriteX -= 3; }
	spriteY -= rows/2u;
	
	// Compute sprite slots
	xHires = (spriteX*2)/7u;

	// Select correct offset block (4 shifted blocks for 7 pixels)
	frameAddr = sprData + frame*SPRITEHEIGHT*BYTEWIDTH;
	if (xHires&1) {
		if (spriteX%7 > 5) { 
			frameAddr += 3*FRAMEBLOCK; 
		} else {
			frameAddr += 2*FRAMEBLOCK;
		}
	} else {
		if (spriteX%7 > 3) { 
			frameAddr += FRAMEBLOCK; 
		}
	}
	
	// Check that sprite was safely enabled
	if (!sprBG[index]) { EnableSprite(index); }

	// Restore background if sprite was previously drawn
	if (sprDrawn[index]) {
		if (sprHiresX[index] == xHires && sprY[index] == spriteY) {
			POKEW(0xEE, 0);		// We already have the correct BG data!
		} else {
			RestoreSprBG(index);
			POKEW(0xEE, sprBG[index]);	// Address for copying Hires > Output (background backup)
		}
	} else {
		POKEW(0xEE, sprBG[index]);	// Address for copying Hires > Output (background backup)
	}
	
	// (Optionally) backup new background, then draw sprite
	POKE(0xE3, 2);				// Bytes per line (x2 for MAIN/AUX)
	POKE(0xCE, SPRITEHEIGHT);	// Number of lines Hires > Output
	POKE(0xEB, rows);			// Number of lines Input > Hires
	POKE(0xEC, xHires);			// Hires Offset X
	POKE(0xED, spriteY);		// Hires Offset Y
	POKEW(0xFA, frameAddr);		// Address for copying Input > Hires
	BlitSprite();
	
	// Check collisions with other sprites
	SpriteCollisions(index);	
	
	// Save sprite information
	sprHiresX[index] = xHires;
	sprX[index] = spriteX;
	sprY[index] = spriteY;
	sprDrawn[index] = 1;	
	
#elif defined __ATARI__
	// Compute vertical position and slot
	if (spriteY > sprYOffset)
		ss_pos = spriteY - sprYOffset;
	else
		ss_pos = 0;
	ss_dli = ss_pos>>3;
	ss_lin = ss_dli*8;
	ss_off = ss_pos-ss_lin+1;
	ss_pmg = index&3;

	// Prepare new data for VBI handler
	ss_src  = &sprData[frame*SPRITEHEIGHT]-ss_off;
	ss_dst  = (PMGRAM+32)+(ss_pmg*0x100)+ss_lin;  // 32 = 4 black rows at top of screen + 1 line	
	
	// Prepare dli data
	ss_dli += ss_pmg*25;
	
	// Send data to VBI handler	
	sprDrawn[index] = 0;
	sprBegDLI[index] = ss_dli;
	sprEndDLI[index] = ss_dli+sprDLIs;
	sprSrc[index] = ss_src;	
	sprDst[index] = ss_dst;
	sprLead[index] = ss_off;
	sprX[index] = spriteX;
	sprY[index] = spriteY;
	sprDrawn[index] = 1;
	
	// Check collisions with other sprites
	SpriteCollisions(index);	

#elif defined __ORIC__
	unsigned char i, inkMUL, inkVAL, backup = 1;
	unsigned int frameAddr, inkAddr, mulAddr;
	unsigned char rows = sprRows[index];
	
	// Check frame block (left or right)
	frameAddr = SPRITERAM + frame*SPRITEHEIGHT*2;
	if (spriteX&1) { frameAddr += FRAMEBLOCK; }
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

	// Restore background only if sprite moved
	if (sprDrawn[index]) {
		if (sprX[index] != spriteX || sprY[index] != spriteY) {
			RestoreSprBG(index);
		} else {
			backup = 0;
		}
	}

	// Compute new memory address
	scrAddr[index] = BITMAPRAM + spriteY*40 + spriteX;
	
	// Backup background if necessary
	if (backup) BackupSprBG(index);		
	
	// Check collisions with other sprites
	SpriteCollisions(index);
	
	// Adjust ink on even lines
	inkVAL = PEEK(sprCOLOR+index);
	inkMUL = (sprMULTICOLOR[index] != 0);
	inkAddr = scrAddr[index] + (spriteY&1)*40;
	if (inkMUL || inkVAL != SPR_AIC) {
		if (inkMUL) {
			mulAddr = sprMULTICOLOR[index];
			inkVAL = PEEK(mulAddr);
			mulAddr++;
		}
		for (i=0; i<rows/2u; i++) {
			if (inkMUL && i*2 > PEEK(mulAddr)) {
				inkVAL = PEEK(++mulAddr);
				++mulAddr;
			}
			POKE(inkAddr+3, 3); 	// Reset AIC INK (yellow)
			POKE(inkAddr, inkVAL);	// Set Sprite INK
			inkAddr+=80;			
		}
	} else if (sprOverlap[index]) {
		for (i=0; i<rows/2u; i++) {
			POKE(inkAddr+3, 3); 	// Reset AIC INK (yellow)
			inkAddr+=80;
		}		
	}

	// Draw new sprite frame
	POKE(0xb0, rows); 				// Number of lines
	POKE(0xb1, 2);					// Bytes per line
	POKEW(0xb2, frameAddr-1);		// Address of source (-1)
	POKEW(0xb4, scrAddr[index]);	// Address of target (-1)
	POKE(0xb6, 2);					// Offset between source lines
	POKE(0xb7, 40); 				// Offset between target lines
	BlitSprite();
		
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
	scb = &sprSCB[index];
	scb->data = &spriteData+FRAMESIZE*frame;
	scb->hpos = spriteX-SPRITEHEIGHT/2u;
	scb->vpos = spriteY-SPRITEWIDTH/2u;
	
	// Check collisions with other sprites
	SpriteCollisions(index);

	// Save sprite information
	sprX[index] = spriteX;
	sprY[index] = spriteY;
	sprDrawn[index] = 1;
#elif defined __NES__
	unsigned char base, *metaSprite;
	metaSprite = &metaSprites[index*17];
	base = frame*4;
	metaSprite[ 2] = base++;
	metaSprite[ 6] = base++;
	metaSprite[10] = base++;
	metaSprite[14] = base;
	oam_set(index<<4);
	oam_meta_spr(spriteX-8, spriteY-8, metaSprite);
	//oam_spr(spriteX, spriteY, frame, 0);
	
	// Check collisions with other sprites
	SpriteCollisions(index);	
	
	// Save sprite information
	sprX[index] = spriteX;
	sprY[index] = spriteY;	
	sprDrawn[index] = 1;
#endif
}

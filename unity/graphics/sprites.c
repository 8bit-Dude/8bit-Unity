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
    #define byteWIDTH 2		// 2 bytes per 7 pixels (4 bytes between AUX/MAIN in DHR)
	unsigned char  frameROWS;
    unsigned int   frameBLOCK;				// Size of sprite offset block (4 blocks)
	unsigned char* sprData;					// Pointer to sprite data (allocated dynamically)
	unsigned char  sprX[SPRITE_NUM], sprY[SPRITE_NUM];	 // Screen coordinates
	unsigned char  sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM]; // Enable and Collision status
	unsigned char  sprHiresX[SPRITE_NUM];  	// Byte offset within Hires line
	unsigned char* sprBG[SPRITE_NUM];  		// Sprite background
	unsigned char  sprRows[SPRITE_NUM];  	// Sprite dimensions used in algorithms
	void CropSprite(unsigned char index, unsigned char rows) {
		// Only partially draw this sprite
		sprRows[index] = rows;
	}	
	
#elif defined __ATARI__	
	#define BANK_NUM   3
	#define sprCols    8
	extern unsigned char sprRows, sprPads, sprDLIs, sprBank[BANK_NUM];
	extern unsigned char sprDrawn[SPRITE_NUM], sprX[SPRITE_NUM], sprY[SPRITE_NUM];
	extern unsigned char sprLine[SPRITE_NUM], sprOff[SPRITE_NUM], sprColor[SPRITE_NUM];
	extern unsigned int  sprSrc[SPRITE_NUM], sprDst[SPRITE_NUM];
	unsigned char *sprData, sprFrames, sprYOffset, sprCollision[SPRITE_NUM], sprCushion = 2;
	void EnableMultiColorSprite(unsigned char index) {
		EnableSprite(index++);
		EnableSprite(index);
	}
	void DisableMultiColorSprite(unsigned char index) {
		DisableSprite(index++);
		DisableSprite(index);		
	}
	void SetMultiColorSprite(unsigned char index, unsigned int frame) {
		SetSprite(index++, frame);
		SetSprite(index, frame+sprFrames);
	}

#elif defined __ORIC__	
	#define byteWIDTH 2		// Byte width of sprite (12 pixels)
	unsigned char frameROWS;
	unsigned int  frameBLOCK;	// Size of sprite offset block (4 blocks), 
	unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM], sprOverlap[SPRITE_NUM]; // Screen coordinates, Overlap flag
	unsigned char sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM]; // Enable Flag, Collision status
	unsigned char sprRows[SPRITE_NUM], *sprBG[SPRITE_NUM]; // Height of Sprite (rows), Pointer to background backup
	unsigned int  scrAddr[SPRITE_NUM], sprCOLOR;  // Screen address, Color vector
	unsigned char* sprMULTICOLOR[SPRITE_NUM];
	extern unsigned char ink1[20];	// see bitmap.c
	void CropSprite(unsigned char index, unsigned char rows) {
		// Only partially draw this sprite
		sprRows[index] = rows;
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
	extern unsigned char spriteData; 
	unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM];	// Screen coordinates
	unsigned char sprDrawn[SPRITE_NUM], sprCollision[SPRITE_NUM], sprCushion = 2; // Enable and Collision status
	unsigned char sprCols, sprRows, frameSize;		// Sprite dimensions
	SCB_REHV_PAL sprSCB[SPRITE_NUM] = { { BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
									    { BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } },
										{ BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } } };
	SCB_REHV_PAL *scb;
	void ScaleSprite(unsigned char index, unsigned int xPercent, unsigned int yPercent) {
		scb = &sprSCB[index];
		scb->hsize = xPercent;
		scb->vsize = yPercent;
	}
#endif

void LoadSprites(unsigned char* filename)
{
#if defined __APPLE2__
	unsigned int size;
	if (sprData) 
		free(sprData);
	if (FileOpen(filename)) {
		FileRead((char*)&size, 2); 
		sprData = malloc(size);
	  #if defined __DHR__
		FileRead(sprData, size);	// Load AUX data
		MainToAux(sprData, size);	// Transfer MAIN -> AUX
	  #endif	
		FileRead(sprData, size);	// Load MAIN data
		FileClose();
	}
#elif defined __ATARI__	
	unsigned int size;
	if (FileOpen(filename)) {
		FileRead(&size, 2);
		if (sprData) free(sprData);
		sprData = malloc(size);
		FileRead(sprData, -1);
	}
#elif defined(__CBM__)
	// TODO: sprite sheets larger than $700
	// can be loaded by exomizer, but not cc65!
	// (file loading under ROM not possible)	
#elif defined __LYNX__
	// TODO
#elif defined __ORIC__	
	FileRead(filename, (void*)SPRITERAM);
#endif
}

void SetupSprites(unsigned int frames, unsigned char cols, unsigned char rows, unsigned char *spriteColors)
{
#if defined __APPLE2__	
	// Set sprite rows, frames and resulting block size (there are 4 offset blocks for each sprite)
	unsigned char i;
	frameROWS = rows;
	frameBLOCK = frames*frameROWS*byteWIDTH;
	for (i=0; i<SPRITE_NUM; i++) 
		sprRows[i] = frameROWS;
	
#elif defined __ATARI__	
	// Reset Sprite Mask, Frames, Colors and Rows
	unsigned char i;
	for (i=0; i<SPRITE_NUM; i++)
		sprColor[i] = spriteColors[i];
	sprRows = rows;
	sprDLIs = (rows+15)>>3;
	sprPads = sprDLIs*8;
	sprYOffset = (rows/2u)+2;
	sprFrames = frames;

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
	frameBLOCK = frames*frameROWS*byteWIDTH;
	for (i=0; i<SPRITE_NUM; i++) 
		sprRows[i] = rows;
	sprCOLOR = spriteColors;

#elif defined __LYNX__	
	sprCols = cols; sprRows = rows;		
	frameSize = rows*((cols+(cols&1))/2+1)+1;
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
	spriteY = y;
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
	  POKE(0xCE, sprRows[index]);	// Number of lines Hires > Output
	  POKE(0xEB, sprRows[index]);	// Number of lines Input > Hires
	  POKE(0xEC, sprHiresX[index]);	// Hires Offset X
	  POKE(0xED, sprY[index]);		// Hires Offset Y
	  POKEW(0xEE, 0);				// Disable copying Hires > Output
	  POKEW(0xFA, sprBG[index]);	// Address for copying Input > Hires
	  BlitSprite();
  }
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
				  bgPtr = sprBG[i]+yHires*byteWIDTH;				  
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

// Oric specific background redrawing function
#if defined __ORIC__
  void __fastcall__ BlitSprite(void);
  void BackupSprBG(unsigned char index) {
	// Backup new background
	POKE(0xb0, frameROWS); 			// Number of lines
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


#if (defined __APPLE2__) || (defined __ATARI__) || (defined __ORIC__)  || (defined __LYNX__)
  unsigned char sc_dX, sc_dY;
 #if (defined __APPLE2__) || (defined __ORIC__)
  unsigned char sc_x1, sc_x2, sc_y1, sc_y2, sc_rows;
  unsigned int sc_bgPtr1, sc_bgPtr2;
 #elif (defined __ATARI__) || (defined __LYNX__)
  unsigned char sc_cushion;
 #endif		
  void SpriteCollisions(unsigned char index)
  {
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
				sc_bgPtr1 = sprBG[index] + sc_y1*byteWIDTH + sc_x1;
				sc_bgPtr2 = sprBG[i] + (sc_rows-sc_y2)*byteWIDTH + (2 - sc_x2);
				for (sc_dY=sc_y1; sc_dY<sc_y2; sc_dY++) {
				  #if defined __DHR__  
				    AuxToAux(sc_bgPtr1, sc_bgPtr2, sc_dX); // AUX data
				  #endif				
					memcpy(sc_bgPtr1, sc_bgPtr2, sc_dX);   // MAIN data
					sc_bgPtr1 += byteWIDTH; 
					sc_bgPtr2 += byteWIDTH;
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
	#elif (defined __ATARI__) || (defined __LYNX__)
		sc_dY = sprY[i] - spriteY;
		sc_cushion = sprRows-sprCushion;
		if (sc_dY < sc_cushion || sc_dY>(256-sc_cushion)) {
			
			sc_dX = sprX[i] - spriteX;
			sc_cushion = sprCols-sprCushion;
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
  unsigned char ss_dli, ss_line, ss_slot, ss_DLI, ss_pos, ss_off;
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
	frameAddr = (char*)(sprData) + frame*frameROWS*byteWIDTH;
	if (xHires&1) {
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
	POKE(0xCE, frameROWS);		// Number of lines Hires > Output
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
	ss_DLI = ss_dli*8;
	ss_off = ss_pos-ss_DLI;
	ss_slot = index&3;

	// Prepare new data for VBI handler
	ss_src  = &sprData[frame*sprRows]-ss_off;
	ss_dst  = (PMGRAM+34)+(ss_slot*0x100)+ss_DLI;  // 32 = 4 black rows at top of screen + 1 line	
	ss_line = ss_slot*25+ss_dli;
	
	// Send data to VBI handler	
	sprDrawn[index] = 0;
	sprLine[index] = ss_line;
	sprSrc[index] = ss_src;	
	sprDst[index] = ss_dst;
	sprOff[index] = ss_off;
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
	frameAddr = SPRITERAM + frame*frameROWS*2;
	if (spriteX&1) { frameAddr += frameBLOCK; }
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
	scb->data = &spriteData+frame*frameSize;
	scb->hpos = spriteX-sprRows/2u;
	scb->vpos = spriteY-sprCols/2u;
	
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
	// Set sprite bank
	sprBank[index/4u] = 1;
	
#elif (defined __APPLE2__) || (defined __ORIC__)
	// Allocate memory for background
	if (!sprBG[index]) {
	  #if defined __APPLE2__
		sprBG[index] = (unsigned char*)malloc(2*frameROWS);	// 2 bytes per line (4 bytes between AUX/MAIN for Apple DHR)
	  #else
		sprBG[index] = (unsigned char*)malloc(4*frameROWS);	// 4 bytes per line (2 atrributes + 12 pixels)
	  #endif
		sprDrawn[index] = 0;
	}
#endif
}

#if (defined __APPLE2__) || (defined __ORIC__)
  void ClearSprite(signed char index) {
	if (sprBG[index]) {
		if (sprDrawn[index]) 
			RestoreSprBG(index); 
		free(sprBG[index]);
		sprBG[index] = 0;
	}
  }
#endif

void DisableSprite(signed char index)
{
#if defined __ATARI__	
	unsigned char i, b;
#endif
	// Switch single sprite off
	if (index >= 0) {	
	#if defined __CBM__
		// Set bit in sprite register
		POKE(53269, PEEK(53269) & ~(1 << index));
	#elif defined __ATARI__
		// Was sprite drawn?
		if (sprDrawn[index]) {
			// Clear PMG memory and slot
			bzero(sprDst[index], sprPads);
			sprDrawn[index] = 0;

			// Check Bank
			b = index/4u;
			sprBank[b] = 0;
			for (i=b*4; i<(b+1)*4; i++)
				if (sprDrawn[i]) {
					sprBank[b] = 1;
					break;
				}
		}
	#else
		// Soft sprites: Restore background if neccessary
	  #if (defined __APPLE2__) || (defined __ORIC__)
		ClearSprite(index);
	  #endif
		sprDrawn[index] = 0;
	#endif
	// Switch all sprites off
	} else {
	#if defined __CBM__
		// Reset sprite register
		POKE(53269, 0);
	#elif defined __ATARI__
		// Clear sprite slots, banks and PMG memory
		bzero(sprDrawn, SPRITE_NUM);
		bzero(sprBank, BANK_NUM);
		bzero(PMGRAM,0x400);
	#elif defined __LYNX__
		// Clear sprite slots
		bzero(sprDrawn, SPRITE_NUM);
	#else
		// Soft sprites: Restore background if necessary
		for (index=0; index<SPRITE_NUM; index++) {
			ClearSprite(index);
			sprDrawn[index] = 0;
		}
	#endif
	}
}

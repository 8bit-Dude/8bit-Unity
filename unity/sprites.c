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
	#define FlickRows   FLICKDATA+0x00
	#define FlickMask   FLICKDATA+0x01
	#define FlickX      FLICKDATA+0x02
	#define FlickY      FLICKDATA+0x06
	#define FlickFrames FLICKDATA+0x0A
	unsigned char spritePalette[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0xFF, 0x74, 0xa6, 0x00, 0x10, 0x2c, 0xdc};	
								 // { BLACK, N/A, DBLUE, DGREEN, BROWN, RED, N/A, ORANGE, N/A, GREY, BLUE, GREEN, N/A, GREY, PINK, YELLOW }	
	unsigned int PMGbase[SPRITE_NUM] = {PMGRAM+1024, PMGRAM+1280, PMGRAM+1536, PMGRAM+1792, PMGRAM+768};
	unsigned int PMGaddr[SPRITE_NUM] = {PMGRAM+1024, PMGRAM+1280, PMGRAM+1536, PMGRAM+1792, PMGRAM+768};
	unsigned char sprROWS;
	void InitSprites(unsigned char rows, unsigned char *spriteColors)
	{			
		// Reset PMG RAM and set colors
		int i;	
		for (i=0; i<4; i++) {
			bzero(PMGaddr[i],0x100);   		// Clear Player memory
			POKE(704+i, spritePalette[spriteColors[i]]);   // Set Player colors  (shadow for 53266)
		}
		bzero(PMGaddr[4],0x100);  	// Clear Missile memory
		POKE(711, spritePalette[spriteColors[4]]);	// Set Missile color  (shadow for 53273)
		POKE(FlickMask, 0);  	  	// Clear rolling sprites mask (DLI.a65)
		
		// Setup ANTIC and GTIA
		POKE(54279, PMGRAM>>8); // Tell ANTIC where the PM data is located
		POKE(53277, 2+1);       // Tell GTIA to enable players + missile	
		POKE(623, 32+16+1);		// Tricolor players + enable fifth player + priority  (shadow for 53275)		
		
		// Set sprite rows
		sprROWS = rows;
		POKE(FlickRows, rows);		
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
	void RestoreSprBG(unsigned char index)
	{
		// Restore entire sprite background
		unsigned char i;
		scrPTR = BITMAPRAM + spriteYS[index]*40 + spriteXS[index];
		bgPTR = sprBG[index];
		for (i=0; i<sprROWS; i++) {
			memcpy(scrPTR, bgPTR, 4);
			scrPTR += 40;
			bgPTR += 4;
		}	
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
	spriteX = x/8 - 1;	
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
	if (index<4) {
		// Set X coordinate of ith sprite and copy frame to PMG column
		POKE(53248+index, spriteX);
		bzero(PMGaddr[index], sprROWS);
		PMGaddr[index] = PMGbase[index]+spriteY;
		memcpy(PMGaddr[index], SPRITERAM + frame*sprROWS, sprROWS);
	} else {
		// Pass information to flicker sprite handler (see DLI.a65)
		POKE(FlickX-4+index, spriteX);
		POKE(FlickY-4+index, spriteY);
		POKEW(FlickFrames+(index-4)*2, SPRITERAM + frame*sprROWS);			
	}
#elif defined __ATMOS__	
	unsigned char i, delta;

	// Offset from centre of sprite
	spriteX -= 1; spriteY -= sprROWS/2;

	// Check for collisions
	sprCOL[index] = 0;
	for (i=0; i<SPRITE_NUM; i++) {
		if (sprEN[i] && i!=index) {
			delta = spriteXS[i] - spriteX;
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
	
	// Backup new background
	scrPTR = BITMAPRAM + spriteY*40 + spriteX;
	bgPTR = sprBG[index];
	for (i=0; i<sprROWS; i++) {
		memcpy(bgPTR, scrPTR, 4);
		scrPTR += 40;
		bgPTR += 4;
	}	
	
	// Display corresponding frame
	sprPTR = SPRITERAM + frame*sprROWS*2;
	inkPTR = BITMAPRAM + spriteY*40;
	inkVAL = ink1[PEEK(colPTR+index)];
	for (i=spriteY+1; i<spriteY+sprROWS+1; i++) {
		// Set INK, Set Sprite, Reset INK
		scrPTR = inkPTR+spriteX;
		if (i%2) { POKE(scrPTR, inkVAL); } scrPTR++; 
		POKE(scrPTR++, PEEK(sprPTR++));
		POKE(scrPTR++, PEEK(sprPTR++));
		if (i%2) { POKE(scrPTR, PEEK(inkPTR)); }
		inkPTR += 40;
	}
	
	// Set sprite information
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
	// Set flicker sprite mask
	if (index>3) { POKE(FlickMask, PEEK(FlickMask) | (1 << (index-4))); }
#endif
}

void DisableSprite(signed char index)
{
	// Switch single sprite off
	if (index >= 0) {	
#if defined __CBM__
		POKE(53269, PEEK(53269) & ~(1 << index));
#elif defined __ATARI__
		if (index>3) {
			POKE(FlickMask, PEEK(FlickMask) & ~(1 << (index-4))); // Remove from rolling sprite mask
		} else {
			bzero(PMGaddr[index],0x100);  // Clear PMG memory			
		}
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
		POKE(FlickMask,0);   	   // Clear rolling sprite mask
		bzero(PMGRAM+768,0x500);   // Clear all PMG memory
#else	
		// Soft sprites: Restore background if necessary
		for (index=0; index<SPRITE_NUM; index++) {
			if (sprEN[index]) { RestoreSprBG(index); }				
			sprEN[index] = 0;
		}
#endif
	}
}

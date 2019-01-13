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
	unsigned char sprCOL[SPRITE_NUM] = {0,0,0,0};		// Collision flags
	unsigned char sprEN[SPRITE_NUM] = {0,0,0,0}; 		// Enable status
	unsigned char sprXO[SPRITE_NUM], sprYO[SPRITE_NUM];	// Processed coordinates
	unsigned char* sprBG[SPRITE_NUM];	    			// Sprite background
	unsigned char sprROWS, sprLENGTH;	
	unsigned int sprBLOCK;
	void InitSprites(unsigned char rows, unsigned char frames)
	{
		// Set sprite rows, frames and resulting block size (there are 4 offset blocks for each sprite)
		int i;
		sprROWS = rows;
		sprLENGTH = rows*4;	// Byte length of 1 sprite: num rows * 4 bytes (7 pixels)
		sprBLOCK = frames*sprROWS*sprWIDTH;
		for (i=0; i<SPRITE_NUM; i++) {
			sprBG[i] = (unsigned char*)malloc(sprLENGTH);
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
	unsigned int PMGbase[SPRITE_NUM] = {PMGRAM+1024, PMGRAM+1280, PMGRAM+1536, PMGRAM+1792, PMGRAM+768};
	unsigned int PMGaddr[SPRITE_NUM] = {PMGRAM+1024, PMGRAM+1280, PMGRAM+1536, PMGRAM+1792, PMGRAM+768};
	unsigned char sprROWS;
	void InitSprites(unsigned char rows, unsigned char *uniqueColors)
	{			
		// Reset PMG RAM and set colors
		int i;	
		for (i=0; i<4; i++) {
			bzero(PMGaddr[i],0x100);   		// Clear Player memory
			POKE(704+i, uniqueColors[i]);   // Set Player colors  (shadow for 53266)
		}
		bzero(PMGaddr[4],0x100);  	// Clear Missile memory
		POKE(711, uniqueColors[4]);	// Set Missile color  (shadow for 53273)
		POKE(FlickMask, 0);  	  	// Clear rolling sprites mask (DLI.a65)
		
		// Setup ANTIC and GTIA
		POKE(54279, PMGRAM>>8); // Tell ANTIC where the PM data is located
		POKE(53277, 2+1);       // Tell GTIA to enable players + missile	
		POKE(623, 32+16+1);		// Tricolor players + enable fifth player + priority  (shadow for 53275)		
		
		// Set sprite rows
		sprROWS = rows;
		POKE(FlickRows, rows);		
	}
// C64 specific init function
#elif defined __CBM__
	void InitSprites(unsigned char *uniqueColors, unsigned char *sharedColors)
	{		
		// Set sprite colors
		int i;	
		for (i=0; i<8; ++i) { POKE(53287+i, uniqueColors[i]); }
		
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
	void RestoreBg(unsigned char index)
	{
		yO = 0;
		xptr = sprXO[index];
		yptr = sprYO[index];
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
	unsigned char GetBGColor(unsigned char index)
	{
		// Return likely color of most central pixel block
		unsigned char ctrBlock = sprBG[index] + sprWIDTH*(sprROWS/2)+sprWIDTH/2;
		switch (ctrBlock) {
			case 85:  return MEDGRAY;
			case 87:
			case 93:
			case 117:
			case 119: return YELLOW;
			case 51:
			case 76:  return PURPLE;
		}
		return BLACK;
	}
#endif

#if defined __CBM__
	unsigned int sprX;
	unsigned char sprY;	
#else
	unsigned char sprX,sprY;
#endif

void LocateSprite(unsigned int x, unsigned int y)
{
// This function maps sprite coordinates from a 320x200 screen definition
// It can be by-passed by assigning sprX, sprY directly in your code
#if defined __APPLE2__
	sprX = (x*140)/320;
	sprY = (y*192)/200;
#elif defined __ATARI__
	sprX = x/2 + 45;
	sprY = y + 24;
#elif defined __CBM__
	sprX = x;
	sprY = y;
#endif	
}

void UpdateSprite(unsigned char index, unsigned char frame)
{
#if defined __APPLE2__
	unsigned char i;
	unsigned int addr;
	
	// Compute sprite slots
	xO = (2*sprX)/7;

	// Select the correct offset block (4 offset blocks per 7 pixels)
	if (xO%2) {
		addr = frame*sprROWS*sprWIDTH + 2*sprBLOCK;
		if (sprX%7 > 5) { addr += sprBLOCK; }
	} else {
		addr = frame*sprROWS*sprWIDTH;
		if (sprX%7 > 3) { addr += sprBLOCK; }
	}

	// Check for collisions
	sprCOL[index] = 0;
	for (i=0; i<SPRITE_NUM; i++) {
		if (sprEN[i] && i!=index) {
			delta = sprXO[i] - xO;
			if (delta < 2 || delta>254) {
				delta = sprYO[i] - sprY;
				if (delta < sprROWS || delta>(256-sprROWS)) {
					// Redraw background of that sprite
					RestoreBg(i);
					sprCOL[i] = 1;
					sprCOL[index] = 1;
				}
			}
		}
	}	

	// Restore old background
	if (sprEN[index]) { RestoreBg(index); }
	
	// Backup new background and draw sprite
	yO = 0;
	yptr = sprY;	
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
	
	// Background settings
	sprEN[index] = 1;
	sprXO[index] = xO;
	sprYO[index] = sprY;
#elif defined __ATARI__
	if (index<4) {
		// Set X coordinate of ith sprite and copy frame to PMG column
		POKE(53248+index, sprX);
		bzero(PMGaddr[index], sprROWS);
		PMGaddr[index] = PMGbase[index]+sprY;
		memcpy(PMGaddr[index], SPRITERAM + frame*sprROWS, sprROWS);
	} else {
		// Pass information to flicker sprite handler (see DLI.a65)
		POKE(FlickX-4+index, sprX);
		POKE(FlickY-4+index, sprY);
		POKEW(FlickFrames+(index-4)*2, SPRITERAM + frame*sprROWS);			
	}
#elif defined __CBM__
	// Tell VIC where to find the frame
	POKE(SPRITEPTR+index, SPRITELOC+frame);

	// Set X/Y Coordinates
	POKE(53249+2*index, sprY+39);
	if (sprX < 244) {
		POKE(53248+2*index, sprX+12);
		POKE(53264, PEEK(53264) & ~(1 << index));
	} else {
		POKE(53248+2*index, sprX-244);
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
	// Set rolling sprite mask
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
#elif defined __APPLE2__
		// Restore background if neccessary
		if (sprEN[index]) { RestoreBg(index); }
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
#elif defined __APPLE2__
		// Restore background
		for (index=0; index<SPRITE_NUM; index++) {
			if (sprEN[index]) { RestoreBg(index); }				
			sprEN[index] = 0;
		}
#endif
	}
}

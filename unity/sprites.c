
#include "unity.h"

#ifdef __APPLE2__
#pragma code-name("LC")
#endif

// Atari specific definitions
#if defined __ATARI__
	// 5th sprite flicker data (see DLI.a65)
	#define PMG5	(0xbfa5)
	#define PMGX 	PMG5+0x00
	#define PMGY 	PMG5+0x04
	#define PMGFram	PMG5+0x08
	#define PMGMask	PMG5+0x10

	// Sprite data
	unsigned int PMGbase[SPRITE_NUM] = {PMGRAM+1024, PMGRAM+1280, PMGRAM+1536, PMGRAM+1792, PMGRAM+768};
	unsigned int PMGaddr[SPRITE_NUM] = {PMGRAM+1024, PMGRAM+1280, PMGRAM+1536, PMGRAM+1792, PMGRAM+768};
#endif

// Apple II specific loading function
#if defined __APPLE2__
	#define SPRITE_BLOCK SPRITE_FRAMES*SPRITE_LENGTH	// Note: There are 2 x blocks for each sprite (offset variants)
	unsigned char sprDAT[SPRITE_NUM][SPRITE_BLOCK*4];	// Sprite RAW data
	unsigned char sprCOL[SPRITE_NUM] = {0,0,0,0};		// Collision flags
	void InitSprites(const char *filename)
	{
		FILE *fp;
		fp = fopen(filename,"rb");
		fread(sprDAT[0], 1, SPRITE_NUM*SPRITE_BLOCK*4, fp);
		fclose(fp);
	}
#else
	void InitSprites(unsigned char *colors)
	{
		int i;	
	#if defined __CBM__
		// Set to multicolor code
		POKE(53276, 255);	
		
		// Set sprite colors
		for (i=0; i<4; ++i) { 
			POKE(53287+i, colors[i]); 
		}
		
		// Set common colors
		POKE(53285, 3);
		POKE(53286, 0);	
	#elif defined __ATARI__	
		// Setup ANTIC and GTIA
		POKE(54279, PMGRAM>>8); // Tell ANTIC where the PM data is located
		POKE(53277, 2+1);       // Tell GTIA to enable players + missile	
		POKE(623, 32+16+1);		// Tricolor players + enable fifth player + priority  (shadow for 53275)

		// Reset PMG RAM and set colors
		for (i=0; i<4; i++) {
			bzero(PMGaddr[i],0x100);   // Clear Player memory
			POKE(704+i, colors[i]);    // Set Player colors  (shadow for 53266)
		}
		bzero(PMGaddr[4],0x100);  // Clear Missile memory
		POKE(711, colors[4]);	  // Set Missile color  (shadow for 53273)
		POKE(PMGMask, 0);  		  // Set rolling sprites mask (PMG.a65)
	#endif
	}
#endif

// Apple II specific background redrawing function
#if defined __APPLE2__
// Extern variables (see bitmap.c)
extern unsigned DHRBases[192];
unsigned char bgB[4] = {0,0,0,0}, bgX[SPRITE_NUM], bgY[SPRITE_NUM];
unsigned char bgDAT[SPRITE_NUM][SPRITE_WIDTH*SPRITE_HEIGHT];  
unsigned char *sprPTR, *bgPTR;
unsigned int x1, y1; 
void RestoreBg(unsigned char index)
{
	bgPTR = &bgDAT[index][0];
	for (y1=0; y1<SPRITE_HEIGHT; y1++) {
		dhrptr = (unsigned char *) (DHRBases[bgY[index]+y1] + bgX[index]);
		*dhraux = 0;
		*dhrptr = bgPTR[0];
		dhrptr++;
		*dhrptr = bgPTR[1];
		dhrptr--;
		*dhrmain = 0;
		*dhrptr = bgPTR[2];		
		dhrptr++;
		*dhrptr = bgPTR[3];
		bgPTR += SPRITE_WIDTH;
	}
}
unsigned char GetBGColor(unsigned char index)
{
	// Return likely color of most central pixel block
	unsigned char ctrBlock = bgDAT[index][SPRITE_WIDTH*(SPRITE_HEIGHT/2)+SPRITE_WIDTH/2];
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

void EnableSprite(signed char index)
{
	// Switch sprites on
	if (index >= 0) {
#if defined __CBM__
		// Set sprite bits
		POKE(53269, PEEK(53269) |  (1 << index));
#elif defined __ATARI__
		// Set rolling sprite mask
		POKE(PMGMask, PEEK(PMGMask) | (1 << index));
#endif
	// Switch all sprites off
	} else {
#if defined __CBM__
		// Set sprite bits		
		POKE(53269, 0);
#elif defined __ATARI__	
		POKE(PMGMask,0);    // Set rolling sprite mask
		bzero(PMGRAM+768,0x500);   // Clear all PMG memory
#elif defined __APPLE2__
		// Restore background if neccessary
		for (index=0; index<SPRITE_NUM; index++) {
			if (bgB[index]) { RestoreBg(index); }	
			bgB[index] = 0;
		}
#endif
	}
}

void DisableSprite(signed char index)
{
	// Switch sprite off
#if defined __CBM__
	POKE(53269, PEEK(53269) & ~(1 << index));
#elif defined __ATARI__
	// Set rolling sprite mask
	POKE(PMGMask, PEEK(PMGMask) & ~(1 << index));
	bzero(PMGaddr[index],0x100);   // Clear Player memory
#elif defined __APPLE2__	
	// Restore background if neccessary
	if (bgB[index]) { RestoreBg(index); }
	bgB[index] = 0;
#endif
}

void SetSprite(unsigned char index, unsigned int frame, unsigned int x, unsigned int y)
{
#if defined __CBM__
	// Tell VIC where to find the frame
	POKE(SPRITEPTR+index, SPRITELOC+frame);

	// Set X/Y Coordinates
	POKE(53249+2*index, y+39);
	if (x < 244) {
		POKE(53248+2*index, x+12);
		POKE(53264, PEEK(53264) & ~(1 << index));
	} else {
		POKE(53248+2*index, x-244);
		POKE(53264, PEEK(53264) |  (1 << index));
	}
#elif defined __ATARI__
	// Convert to Atari coordinates
	x = 45+(x*160)/320;
	y = 22+(y*192)/200;
	
	// Set X coordinate
	POKE(53248+index, x);
	
	// Refesh PMG colum
	bzero(PMGaddr[index], SPRITE_LENGTH);       
	PMGaddr[index] = PMGbase[index]+y;
	memcpy(PMGaddr[index], SPRITERAM1+frame*SPRITE_LENGTH, SPRITE_LENGTH);		

	// Pass information to 5th sprite handler (PMG5.a65)
	POKE(PMGX+index, x);
	POKE(PMGY+index, y);
	POKEW(PMGFram+index*2, frame*SPRITE_LENGTH);
#elif defined __APPLE2__
	unsigned char i,delta;
	
	// Convert to Apple coordinates
	x = (140*x)/320;
	y = (192*y)/200;
		
	// Comput sprite slots
	x1 = (2*x)/7;

	// Check for collisions
	sprCOL[index] = 0;
	for (i=0; i<SPRITE_NUM; i++) {
		if (i!=index && bgB[i]) {
			delta = bgX[i] - x1;
			if (delta < 2 || delta>254) {
				delta = bgY[i] - y;
				if (delta < SPRITE_HEIGHT || delta>(256-SPRITE_HEIGHT)) {
					// Redraw background of that sprite
					RestoreBg(i);
					sprCOL[index] = 1;
					sprCOL[i] = 1;
				}
			}
		}
	}	

	// Restore background
	if (bgB[index]>0) { RestoreBg(index); }
	
	// Select the correct offset data
	if (x1%2) {
		if (x%7 < 6) {
			y1 = frame*SPRITE_WIDTH*SPRITE_HEIGHT + 2*SPRITE_BLOCK;
		} else {
			y1 = frame*SPRITE_WIDTH*SPRITE_HEIGHT + 3*SPRITE_BLOCK;			
		}
	} else {
		if (x%7 < 2) {
			y1 = frame*SPRITE_WIDTH*SPRITE_HEIGHT;
		} else {
			y1 = frame*SPRITE_WIDTH*SPRITE_HEIGHT + 1*SPRITE_BLOCK;
		}
	}
	
	// Backup background and draw sprite	
	bgPTR = &bgDAT[index][0];
	sprPTR = &sprDAT[index][y1];
	for (y1=y; y1<y+SPRITE_HEIGHT; y1++) {
		dhrptr = (unsigned char *) (DHRBases[y1] + x1);
		*dhraux = 0;
		bgPTR[0] = *dhrptr;
		*dhrptr = sprPTR[0];
		dhrptr++;
		bgPTR[1] = *dhrptr;
		*dhrptr = sprPTR[1];
		dhrptr--;
		*dhrmain = 0;
		bgPTR[2] = *dhrptr;
		*dhrptr = sprPTR[2];		
		dhrptr++;
		bgPTR[3] = *dhrptr;
		*dhrptr = sprPTR[3];		
		bgPTR += SPRITE_WIDTH;
		sprPTR += SPRITE_WIDTH;
	}
	
	// Background settings
	bgB[index] = 1;
	bgX[index] = x1;
	bgY[index] = y;
#endif
}

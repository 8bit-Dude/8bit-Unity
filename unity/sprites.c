
#include "unity.h"

// Atari specific definitions
#if defined __ATARI__
	// 5th sprite flicker data (PMG5.a65)
	#define PMGX 	PMG5+0x00
	#define PMGY 	PMG5+0x04
	#define PMGFram	PMG5+0x08
	#define PMGMask	PMG5+0x10

	// 1-4th sprite data
	unsigned int PMGbase[4] = {PMGRAM+1024, PMGRAM+1280, PMGRAM+1536, PMGRAM+1792};
	unsigned int PMGaddr[4] = {PMGRAM+1024, PMGRAM+1280, PMGRAM+1536, PMGRAM+1792};
	unsigned int PMG5base = PMGRAM+768;
#endif

// Apple II specific definitions
#if defined __APPLE2__
	#define SPRITE_WIDTH  4
	#define SPRITE_HEIGHT 8
	#define SPRITE_FRAMES 16
	#define SPRITE_LENGTH SPRITE_FRAMES*SPRITE_WIDTH*SPRITE_HEIGHT
#endif


// Apple II specific loading function
#if defined __APPLE2__
unsigned char sprDAT[4][SPRITE_LENGTH];
void LoadSprite(unsigned char index, char *name)
{
	unsigned char header[5];
	FILE *fp;
	int c;

	fp = fopen(name,"rb");
	if (fp == NULL) return;

	/* read 5 byte header */
	c = fread(header,1,5,fp);
	if (c != 5) return;
	if (header[0] != 'D' || header[1] != 'H' || header[2] != 'R') return;
	if (header[3] != SPRITE_WIDTH || header[4] != SPRITE_FRAMES*SPRITE_HEIGHT) return;
	c = fread(&sprDAT[index][0], 1, SPRITE_LENGTH, fp);
   fclose(fp);
}
#endif

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
#elif defined __APPLE2__	
	// Load sprite sheets
	LoadSprite(0, "SPRITES0.DHR");
	LoadSprite(1, "SPRITES1.DHR");
	LoadSprite(2, "SPRITES2.DHR");
	LoadSprite(3, "SPRITES3.DHR");	
#endif
}

// Apple II specific background redrawing function
#if defined __APPLE2__
// Extern variables (see bitmap.c)
extern unsigned DHRBases[192];
unsigned char bgB[4] = {0,0,0,0}, bgX[4], bgY[4];
unsigned char bgDAT[4][SPRITE_WIDTH*SPRITE_HEIGHT];  
unsigned char *sprPTR, *bgPTR;
unsigned int x1, y1; 
void RestoreBg(unsigned char index)
{
	bgPTR = &bgDAT[index][0];
	x1 = (bgX[index]/7)*2;
	if ((bgX[index]%7) > 3) { x1++; }
	for (y1=0; y1<SPRITE_HEIGHT; y1++) {
		dhrptr = (unsigned char *) (DHRBases[bgY[index]+y1] + x1);
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
#endif

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
	bzero(PMGaddr[index], SPRITELEN);       
	PMGaddr[index] = PMGbase[index]+y;
	memcpy(PMGaddr[index], SPRITERAM1+frame*SPRITELEN, SPRITELEN);		

	// Pass information to 5th sprite handler (PMG5.a65)
	POKE(PMGX+index, x);
	POKE(PMGY+index, y);
	POKEW(PMGFram+index*2, frame*SPRITELEN);
#elif defined __APPLE2__
	// Convert to Apple coordinates
	x = (140*x)/320;
	y = (192*y)/200;

	// restore background?
	if (bgB[index]>0) { RestoreBg(index); }
	
	// backup background and show sprite
	sprPTR = &sprDAT[index][frame*SPRITE_WIDTH*SPRITE_HEIGHT];
	bgPTR = &bgDAT[index][0];
	x1 = (x/7)*2;
	if ((x%7) > 3) { x1++; }	
	for (y1=0; y1<SPRITE_HEIGHT; y1++) {
		dhrptr = (unsigned char *) (DHRBases[y+y1] + x1);
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
	
	// Background Toggle
	bgB[index] = 1;
	bgX[index] = x;
	bgY[index] = y;
#endif
}

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
		for (index=0; index<4; index++) {
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

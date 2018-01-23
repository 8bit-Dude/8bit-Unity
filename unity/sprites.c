
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
	#define PACKET_WIDTH  SPRITE_WIDTH/2
	#define PACKET_BOUND  PACKET_WIDTH-1
#endif

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
		POKE(704+i, colors[i]);   // Set Player colors  (shadow for 53266)
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

#if defined __APPLE2__
// Extern variables (see bitmap.c)
extern unsigned DHRBases[192];
extern unsigned *dhrsrc, *dhrdst;
unsigned char *PixelData;
unsigned x1, y1, y2, destoffset, auxoffset, mainoffset;  
unsigned char bgB[4] = {0,0,0,0}, bgX[4], bgY[4], bgDAT[4][SPRITE_WIDTH*SPRITE_HEIGHT];  
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

	// Pass information to 5th sprite handler (PMG.a65)
	POKE(PMGX+index, x);
	POKE(PMGY+index, y);
	POKEW(PMGFram+index*2, frame*SPRITELEN);
#elif defined __APPLE2__
	// Convert to Apple coordinates
	x = (80*x)/320;
	y = (192*y)/200;
	
	PixelData = &sprDAT[index][frame*SPRITE_WIDTH*SPRITE_HEIGHT];
	x1 = bgX[index]/2; y1 = bgY[index]; y2 = y1 + SPRITE_HEIGHT;
    auxoffset = 0; mainoffset = PACKET_WIDTH;

	// restore background?
	if (bgB[index]>0) {
		/* rasters are split between auxiliary memory and main memory */
		while (y1 < y2) {
			/* restore auxiliary memory */
			dhrsrc[0] = (unsigned) &bgDAT[index][auxoffset];
			dhrsrc[1] = (unsigned) &bgDAT[index][mainoffset-1];
			dhrdst[0] = destoffset = (DHRBases[y1] + x1);					
			asm("sec");
			asm("jsr $c311");
			
			/* restore main memory */
			memcpy((char *)destoffset,&bgDAT[index][mainoffset],PACKET_WIDTH);
			auxoffset+= SPRITE_WIDTH;
			mainoffset+=SPRITE_WIDTH;
			y1++;
		}		
	}

	x1 = x/2; y1 = y; y2 = y1 + SPRITE_HEIGHT; 
	auxoffset = 0; mainoffset = PACKET_WIDTH;

    /* rasters are split between auxiliary memory and main memory */
	while (y1 < y2) {
		/* backup auxiliary & main memory */
		dhrsrc[0] = (DHRBases[y1] + x1);
		dhrsrc[1] = (dhrsrc[0] + PACKET_BOUND);
		dhrdst[0] = (unsigned) &bgDAT[index][auxoffset];		
		asm("clc");
		asm("jsr $c311");
		
		/* assign auxiliary memory */
		dhrsrc[0] = (unsigned) &PixelData[auxoffset];
		dhrsrc[1] = (unsigned) &PixelData[mainoffset-1];
		dhrdst[0] = destoffset = (DHRBases[y1] + x1);
		asm("sec");
		asm("jsr $c311");
		
		/* backup and assign main memory */
		memcpy(&bgDAT[index][mainoffset],(char *)destoffset,PACKET_WIDTH);
		memcpy((char *)destoffset,&PixelData[mainoffset],PACKET_WIDTH);
		auxoffset+= SPRITE_WIDTH;
		mainoffset+=SPRITE_WIDTH;
		y1++;
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
		POKE(53269, 0);		// Set sprite bits		
#elif defined __ATARI__	
		POKE(PMGMask,0);    // Set rolling sprite mask
		bzero(PMGRAM+768,0x500);   // Clear all PMG memory
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
#endif
}

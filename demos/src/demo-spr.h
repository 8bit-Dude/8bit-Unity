
#include "unity.h"
#include <cc65.h>

#if defined __APPLE2__	//  Colors are pre-assigned in the sprite sheet
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __ATARI__ //  5th color shared between all sprites
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW };		// P1, P2, P3, P4
	unsigned char spriteColors[] = {0x88, 0x28, 0xba, 0xee, 0x00, 0x08, 0x08, 0x08, 0x08, 0x00 };	// Refer to atari palette in docs
#elif defined __ATMOS__
	unsigned char inkColors[] = { CYAN, MBLUE, LGREEN, GREY };		// P1, P2, P3, P4
	unsigned char spriteColors[] = { CYAN, MBLUE, LGREEN, GREY };	// Matching more or less with above
#elif defined __CBM__
	unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW };		// P1, P2, P3, P4
	unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW, 0, 0, 0, 0 };	// Main sprite colors
	unsigned char sharedColors[] = { CYAN, BLACK };					// Shared sprite colors
#endif

unsigned char buffer[20];

int DemoSPR(void) 
{
	unsigned char i, slot, frame;
	unsigned int xpos, ypos, angle;
	clock_t timer = clock();

	// Initialize sfx and bitmap
	InitSFX();
	InitBitmap();
	LoadBitmap("stadium.map");
	EnterBitmapMode();
	
	// Print some extra info
	paperColor = GREY; 
	inkColor = BLACK; 
	PrintStr(0, CHR_ROWS-1, "STADIUM");	
	paperColor = BLACK; 
	for (i=0; i<4; i++) {
		slot = 8*(i+1);
		inkColor = inkColors[i];
#if defined __ATMOS__
		SetInk(slot-1, CHR_ROWS-1);
#endif		
		PrintLogo(slot+4, CHR_ROWS-1, i);
		PrintNum(slot+2, CHR_ROWS-1, i+1);
		PrintStr(slot, CHR_ROWS-1, "CAR");
#if defined __ATMOS__
		inkColor = AIC;
		SetInk(slot+5, CHR_ROWS-1);
#endif		
	}
	
	// Initialize sprites
#if defined __APPLE2__
	// number of rows, number of frames
	InitSprites(5, 64);
#elif defined __ATARI__
	// number of rows, unique colors of sprites 0-4
	InitSprites(13, spriteColors);
#elif defined __ATMOS__
	// number of rows, unique colors of sprites 0-3
	InitSprites(6, spriteColors);
#elif defined __CBM__
	// unique colors of sprites 0-7, shared colors of all sprites 
	InitSprites(spriteColors, sharedColors);
#endif

	// Enable sprites
	for (i=0; i<4; i++) {
		EnableSprite(i);
#if defined __ATARI__
		EnableSprite(i+5);	// Also assign sprite for 2nd color (tires)
#endif
	}

	// Animate sprites
	while (!kbhit()) {
#if defined __APPLE2__
		tick();	// Apple 2 has no clock, so simulate ticks!
#endif
        // Update sprites position
        if (clock()>timer) {
            timer = clock();
            angle += 3;
            for (i=0; i<4; i++) {
                xpos = 230+cc65_cos((angle+i*90)%360)/5;
                ypos = 90+cc65_sin((angle+i*90)%360)/6;
                frame = ((12-(angle+(i+1)*90))%360)/23;
				LocateSprite(xpos, ypos);
#if defined __APPLE2__
				SetSprite(i, frame+(i*16));	// Point to sprite data associated with each player color
#elif defined __ATARI__
				SetSprite(i, frame);		// Body sprite using 1st color
				SetSprite(i+5, frame+16);	// Tire sprite using 2nd color
#elif defined __ATMOS__
				SetSprite(i, frame);		// Single coloured sprite (colour applies to second scan line)
#elif defined __CBM__
				SetSprite(i, frame);		// Single sprite including 1 unique + 2 shared colors
#endif
				EngineSFX(1, 300);
            }       
        }
	}
	
	// Black-out screen and clear key
	DisableSprite(-1);	// "-1" disables all sprites
	ExitBitmapMode();
	StopSFX();
	cgetc();
	
    // Done
    return EXIT_SUCCESS;	
}
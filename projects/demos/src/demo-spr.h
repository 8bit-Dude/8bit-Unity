
#include "unity.h"
#include <cc65.h>

// Sprite definitions
#if defined __APPLE2__
	#define spriteFrames 64
	#define spriteRows   5
	unsigned char spriteColors[] = { };	//  Colors are pre-assigned in the sprite sheet
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __ATARI__ //  5th color shared between all sprites
	#define spriteFrames 16
	#define spriteRows   13
	unsigned char spriteColors[] = {0x88, 0x28, 0xba, 0xee, 0x00, 0x08, 0x08, 0x08, 0x08, 0x00 };	// 0-3: 1st color, 5-8: 2nd color (Refer to atari palette in docs)
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __ATMOS__
	#define spriteFrames 16
	#define spriteRows   6
	unsigned char spriteColors[] = { CYAN, MBLUE, LGREEN, GREY };	// Matching more or less with above
	unsigned char inkColors[] = { CYAN, MBLUE, LGREEN, GREY };		// P1, P2, P3, P4
#elif defined __CBM__
	#define spriteFrames 16
	#define spriteRows   21
	unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW, 0, 0, 0, 0, CYAN, BLACK };  // 0-8: Sprite colors, 9-10: Shared colors
	unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW };		// P1, P2, P3, P4
#endif

int DemoSPR(void) 
{
	unsigned char i, slot, frame;
	unsigned int xpos, ypos, angle;
	clock_t timer = clock();

	// Initialize sfx, bitmap, sprites
	InitSFX();
	InitBitmap();
	InitSprites(spriteFrames, spriteRows, spriteColors);
	
	// Load and show bitmap
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
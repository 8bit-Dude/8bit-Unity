
#include "unity.h"
#include <cc65.h>

#if defined __ATARI__
	unsigned char uniqueColors[] = {0x74, 0x24, 0xa6, 0xdc, 0x08};  //  {blue, red, green, yellow, grey }
#elif defined __CBM__
	unsigned char uniqueColors[] = { BLUE, RED, GREEN, YELLOW, 0, 0, 0, 0 };
	unsigned char sharedColors[] = { CYAN, BLACK };
#endif

int DemoSPR(void) 
{
	unsigned char i, frame;
	unsigned int xpos, ypos, angle;
	clock_t timer = clock();

	// Initialize sfx and bitmap
	InitSFX();
	InitBitmap();
	LoadBitmap("stadium.map");
	EnterBitmapMode();
	
	// Initialize sprites
#if defined __APPLE2__
	// number of rows, number of frames
	InitSprites(5, 64);
#elif defined __ATARI__
	// number of rows, unique colors of sprites 0-4
	InitSprites(13, uniqueColors);
#elif defined __CBM__
	// unique colors of sprites 0-7, shared colors of all sprites 
	InitSprites(uniqueColors, sharedColors);
#endif

	// Enable sprites
	for (i=0; i<4; i++) {
		EnableSprite(i);
#if defined __ATARI__
		EnableSprite(4+i);	// Flicker sprites used for 2nd color (tires)
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
				UpdateSprite(i, (i*16)+frame);	// Point to sprite data associated with each player color
#elif defined __ATARI__
				UpdateSprite(i, frame);			// Normal sprite for 1st color (body)
				UpdateSprite(4+i, 16+frame);	// Flicker sprite for 2nd color (tires)	
#elif defined __CBM__
				UpdateSprite(i, frame);			// Single sprite including 1 unique + 2 shared colors
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
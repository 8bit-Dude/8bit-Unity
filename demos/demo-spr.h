
#include "unity.h"
#include <cc65.h>

// Sprite colors
#ifdef __CBM__
	unsigned char uniqueColors[8] = { BLUE, RED, GREEN, YELLOW, LITGRAY, LITGRAY, LITGRAY, LITGRAY };	// P1, P2, P3, P4, Lights
	unsigned char sharedColors[2] = { CYAN, BLACK };
#elif defined __ATARI__
	unsigned char spriteColors[5] = {0x74, 0x24, 0xa6, 0xdc, 0x06};				// P1, P2, P3, P4, TIRES
#endif

int DemoSPR(void) 
{
	unsigned char i, frame;
	unsigned int xpos, ypos, angle;
	clock_t timer = clock();

	// Reset screen
	clrscr();
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);

	// Prepare bitmap
	InitBitmap();
	LoadBitmap("stadium.map");
	EnterBitmapMode();
	
	// Prepare sprites
#if defined __APPLE2__
	InitSprites(5, 4, 16);
#elif defined __ATARI__
	InitSprites(spriteColors);
#elif defined __CBM__
	InitSprites(uniqueColors, sharedColors);
#endif

	// Prepare SFX
	InitSFX();

	// Rotate sprites
	while (!kbhit()) {
#if defined __APPLE2__
		tick();	// Apple 2 has no clock, so simulate ticks!
#endif
        // Update sprites position
        if (clock()>timer) {
            timer = clock();
            angle += 2;
            for (i=0; i<4; i++) {
                xpos = 230+cc65_cos((angle+i*90)%360)/5;
                ypos = 90+cc65_sin((angle+i*90)%360)/6;
                frame = ((12-(angle+(i+1)*90))%360)/23;
				LocateSprite(xpos, ypos);
				UpdateSprite(i, frame);
				EnableSprite(i);
				EngineSFX(1, 300);
            }       
        }
	}
	
	// Exit GFX mode and clear key
	DisableSprite(-1);
	ExitBitmapMode();
	InitSFX();
	cgetc();
	
    // Done
    return EXIT_SUCCESS;	
}
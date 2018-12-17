
#include "unity.h"

#if defined __ATARI__
	unsigned char spriteColors[5] = {0x28, 0x28, 0x28, 0x28, 0x28};
#elif defined __CBM__
	unsigned char uniqueColors[8] = {0, 0, 0, 0, 10, 10, 10, 10};
	unsigned char sharedColors[2] = {2, 1};
#endif

int TestSPR(void) 
{
	unsigned char i,j;
	clock_t timer = clock();

	// Reset screen
	clrscr();
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);

	// Prepare bitmap
	InitBitmap();
	ClearBitmap();
	EnterBitmapMode();
	
	// Prepare sprites
#if defined __APPLE2__
	InitSprites(13, 8);
#elif defined __ATARI__
	InitSprites(spriteColors);
#elif defined __CBM__
	InitSprites(uniqueColors, sharedColors);
#endif

	// Display sprite
	EnableSprite(0);
	sprX = 160, sprY = 100;
	while (!kbhit()) {
#if defined __APPLE2__
		tick();	// Apple 2 has no clock, so simulate ticks!
#endif
		// Animate sprite every 3 ticks
		if (clock()-timer > 3) {
			timer = clock();
			sprF += 1;
			sprF %= 8;
			SetSprite(0);
		}
	}
	
	// Wait until keyboard is pressed
	cgetc();
	ExitBitmapMode();
	
    // Done
    return EXIT_SUCCESS;	
}
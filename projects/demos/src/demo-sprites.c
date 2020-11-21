
#include "unity.h"
#include <cc65.h>

extern const char keyNext;

// Sprite definitions
#if defined __APPLE2__
	#define spriteFrames 64
	#define spriteCols   7
	#define spriteRows   5
	unsigned char spriteColors[] = { };	//  Colors are pre-assigned in the sprite sheet
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __ATARI__ //  5th color shared between all sprites
	#define spriteFrames 16
	#define spriteCols   8
	#define spriteRows   13
	unsigned char spriteColors[] = {0x88, 0x28, 0xba, 0xee, 0x00,   // 0-4: 1st color (car body)
									0x08, 0x08, 0x08, 0x08, 0x00 };	// 5-8: 2nd color (car tires) Refer to atari palette in docs
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __ORIC__
	#define spriteFrames 16
	#define spriteCols   12
	#define spriteRows   6
	unsigned char spriteColors[] = { SPR_CYAN, SPR_MAGENTA, SPR_GREEN, SPR_WHITE, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	// Matching more or less with above
	unsigned char inkColors[] = { CYAN, LPURPLE, LGREEN, GREY };		// P1, P2, P3, P4
#elif defined __CBM__
	#define spriteFrames 16
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW, 0, 0, 0, 0, CYAN, BLACK };  // 0-8: Sprite colors, 9-10: Shared colors
	unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __LYNX__
	#define spriteFrames 16
	#define spriteCols   7
	#define spriteRows   9
	unsigned char spriteColors[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x0b, 0x23, 0x45, 0x67, 0x89, 0xa1, 0xcd, 0xef,   // Swapped 1 and B
									 0x05, 0x23, 0x41, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Swapped 1 and 5
									 0x09, 0x23, 0x45, 0x67, 0x81, 0xab, 0xcd, 0xef,   // Swapped 1 and 9
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }; // Default palette
	unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW };		// P1, P2, P3, P4
#endif

int DemoSprites(void) 
{
	unsigned char i, slot, frame;
	unsigned int xpos, ypos, angle;
	clock_t timer = clock();
	
	// Clear screen
	clrscr();	

	// Initialize sfx/bitmap
	InitSFX();
	InitBitmap();
	
	// Setup sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
	
	// Load and show bitmap
	LoadBitmap("stadium.img");
	EnterBitmapMode();
	
	// Print some extra info
	paperColor = GREY; 
	inkColor = BLACK; 
#if defined __ORIC__
	SetAttributes(-1, CHR_ROWS-1, paperColor);
	SetAttributes( 7, CHR_ROWS-1, AIC); // Always reset attributes after a string!
#endif			
	PrintStr(0, CHR_ROWS-1, "STADIUM");	
	paperColor = BLACK; 
	for (i=0; i<4; i++) {
		slot = 8*(i+1);
		inkColor = inkColors[i];
	#if defined __ORIC__
		SetAttributes(slot-1, CHR_ROWS-1, inkColor);
		SetAttributes(slot+5, CHR_ROWS-1, AIC);	// Always reset attributes after a string!
	#endif		
		PrintLogo(slot+4, CHR_ROWS-1, i);
		PrintNum(slot+2, CHR_ROWS-1, i+1);
		PrintStr(slot, CHR_ROWS-1, "CAR");
	}

	// Enable sprites
	for (i=0; i<4; i++) {
		EnableSprite(i);
	#if defined __ATARI__
		EnableSprite(i+5);	// Also assign sprite for 2nd color (tires)
	#endif
	}

	// Repeat until 'SPACE' is pressed
	while (!kbhit () || cgetc () != keyNext) {
	#if defined __APPLE2__
		clk += 1;  // Manually update clock on Apple 2
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif
        // Update sprites position
        if (clock()>timer) {
            timer = clock();
            angle += 3;
            for (i=0; i<4; i++) {
				xpos = 230+_cos((angle+i*90)%360)/5;
				ypos = 90+_sin((angle+i*90)%360)/6;
                frame = ((12-(angle+(i+1)*90))%360)/23;
				LocateSprite(xpos, ypos);
			#if defined __APPLE2__
				SetSprite(i, frame+(i*16));	// Point to sprite data associated with each player color
			#elif defined __ATARI__
				SetSprite(i, frame);		// Body sprite using 1st color
				SetSprite(i+5, frame+16);	// Tire sprite using 2nd color
			#else
				SetSprite(i, frame);
			#endif
				EngineSFX(1, 300);
            }       
        }
	}
	
	// Black-out screen and stop SFX
	DisableSprite(-1);	// "-1" disables all sprites
	ExitBitmapMode();
	StopSFX();
	
    // Done
    return EXIT_SUCCESS;	
}
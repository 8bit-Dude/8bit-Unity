
#include "unity.h"
#include <cc65.h>

extern const char keyNext;

// Sprite definitions
#if defined __APPLE2__
	#define spriteFrames 64
	#define spriteCols    7
	#define spriteRows    5
	unsigned char spriteColors[] = { };	//  Colors are pre-assigned in the sprite sheet
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __ATARI__
	#define spriteFrames 18
	#define spriteCols    8
	#define spriteRows   10
	unsigned char spriteColors[] = {0x86, 0xe4, 0x26, 0xe4, 0xb6, 0xe4, 0xe8, 0xe4, 0x00, 0x00, 0x00, 0x00 };   // 0,2,4,6: car body, 1,3,5,7: car tires
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __ORIC__
	#define spriteFrames 16
	#define spriteCols   12
	#define spriteRows    6
	unsigned char spriteColors[] = { SPR_CYAN, SPR_MAGENTA, SPR_GREEN, SPR_WHITE, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	// Matching more or less with above
	unsigned char inkColors[] = { CYAN, LPURPLE, LGREEN, GREY };	// P1, P2, P3, P4
#elif defined __CBM__
	#define spriteFrames 16
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW, 0, 0, 0, 0, CYAN, BLACK };  // 0-8: Sprite colors, 9-10: Shared colors
	unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __LYNX__
	#define spriteFrames 16
	#define spriteCols    8
	#define spriteRows    9
	unsigned char *spriteColors = 0;  //  All sprites use the default palette
	unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW };		// P1, P2, P3, P4
#endif

int DemoSprites(void) 
{
	unsigned char i, slot, frame;
	unsigned int xpos, ypos, angle;
	clock_t timer;	

	// Initialize sfx/bitmap
	InitSFX();
	InitBitmap();
	
	// Setup sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
#if defined __LYNX__
	RecolorSprite(1, 0, 0x08); // BLUE -> ORANGE
	RecolorSprite(2, 0, 0x05); // BLUE -> GREEN
	RecolorSprite(3, 0, 0x09); // BLUE -> YELLOW
#endif

	// Load and show bitmap
	LoadBitmap("stadium.img");
	ShowBitmap();
	
	// Print some extra info
	paperColor = GREY; 
	inkColor = BLACK; 
#if defined __ORIC__
	SetAttributes(-1, TXT_ROWS-1, paperColor);
	SetAttributes( 7, TXT_ROWS-1, AIC); // Always reset attributes after a string!
#endif			
	PrintStr(0, TXT_ROWS-1, "STADIUM");	
	paperColor = BLACK; 
	for (i=0; i<4; i++) {
		slot = 8*(i+1);
		inkColor = inkColors[i];
	#if defined __ORIC__
		SetAttributes(slot-1, TXT_ROWS-1, inkColor);
		SetAttributes(slot+5, TXT_ROWS-1, AIC);	// Always reset attributes after a string!
	#endif		
		PrintLogo(slot+4, TXT_ROWS-1, i);
		PrintNum(slot+2, TXT_ROWS-1, i+1);
		PrintStr(slot, TXT_ROWS-1, "CAR");
	}

	// Enable sprites
	for (i=0; i<4; i++) {
	#if defined __ATARI__
		EnableMultiColorSprite(2*i);
	#else  
		EnableSprite(i);
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
				SetMultiColorSprite(2*i, frame); // Automatically assigns 2 sprite slots! (for car body and tires)
			#else
				SetSprite(i, frame);
			#endif
				PlaySFX(SFX_ENGINE, 64, 22, 0);
            }       
        }
	}
	
	// Black-out screen and stop SFX
	DisableSprite(-1);	// "-1" disables all sprites
	HideBitmap();
	StopSFX();
	
    // Done
    return EXIT_SUCCESS;	
}

#include "unity.h"
#include <cc65.h>

extern const char nextKey;

// Sprite definitions
#if defined __APPLE2__
  const unsigned char spriteColors[] = { };	//  Colors are pre-assigned in the sprite sheet
  const unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __ATARI__
  const unsigned char spriteColors[] = {0x86, 0xe4, 0x26, 0xe4, 0xb6, 0xe4, 0xe8, 0xe4, 0x00, 0x00, 0x00, 0x00 };   // 0,2,4,6: car body, 1,3,5,7: car tires
  const unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __CBM__
  const unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW, 0, 0, 0, 0, CYAN, BLACK };  // 0-8: Sprite colors, 9-10: Shared colors
  const unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __LYNX__
  const unsigned char *spriteColors = 0;  //  All sprites use the default palette
  const unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __NES__
  const unsigned char spriteColors[] = { SPR_VOID, SPR_BLUE,   SPR_CYAN, SPR_GREY,  
										 SPR_VOID, SPR_RED,    SPR_CYAN, SPR_GREY, 
										 SPR_VOID, SPR_GREEN,  SPR_CYAN, SPR_GREY, 
										 SPR_VOID, SPR_YELLOW, SPR_CYAN, SPR_GREY }; // 4 palettes of 3 colors (1st color is unused)
  const unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW };		// P1, P2, P3, P4
#elif defined __ORIC__
  const unsigned char spriteColors[] = { SPR_CYAN, SPR_MAGENTA, SPR_GREEN, SPR_WHITE, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	// Matching more or less with above
  const unsigned char inkColors[] = { CYAN, LPURPLE, LGREEN, GREY };	// P1, P2, P3, P4
#endif

// Interface definitions
#define SLOT_COL1 8
#if defined __NES__
  #define SLOT_WIDTH 6
#else
  #define SLOT_WIDTH 8
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
	SetupSprites(spriteColors);
#if defined __LYNX__
	RecolorSprite(1, 0, 0x08); // BLUE -> ORANGE
	RecolorSprite(2, 0, 0x05); // BLUE -> GREEN
	RecolorSprite(3, 0, 0x09); // BLUE -> YELLOW
#elif defined __NES__
	RecolorSprite(1, 0, 1); // Use Palette 1
	RecolorSprite(2, 0, 2); // Use Palette 2
	RecolorSprite(3, 0, 3); // Use Palette 3
#endif

	// Load and show bitmap
	LoadBitmap("stadium.img");
	ShowBitmap();
	
	// Print some extra info
	paperColor = GREY; inkColor = BLACK; 
	txtY = TXT_ROWS-1; txtX = 0; 
#if defined __ORIC__
	SetAttributes(AIC);
#endif
	PrintStr("STADIUM");	
	paperColor = BLACK; 
	for (i=0; i<4; i++) {
		slot = SLOT_COL1 + SLOT_WIDTH*i;
		inkColor = inkColors[i];
		txtX = slot; PrintStr("CAR");
	#if defined __ORIC__
		SetAttributes(inkColor);
	#endif		
		txtX = slot+3; PrintNum(i+1);
		txtX = slot+4; PrintLogo(i);
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
	while (!kbhit () || cgetc () != nextKey) {
	#if defined(__APPLE2__)
		clk += 1;  // Manually update clock on Apple 2
	#elif defined(__LYNX__) || defined(__NES__)
		UpdateDisplay(); // Manually refresh Display
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

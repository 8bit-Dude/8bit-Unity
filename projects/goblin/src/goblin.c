
#include "definitions.h"

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

// Sprite definitions
#if defined __APPLE2__	
	unsigned char spriteColors[] = { };  // Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	unsigned char spriteColors[] = { 0x0e, 0x80, 0x10, 0x2a, 0xba, 0x00, 0x00, 0x00, 0x00, 0x00 };  // Refer to atari palette in docs
#elif defined __ORIC__
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };  // AIC color allows faster drawing!
	unsigned char multiColorDef[] = { SPR_WHITE, 8, SPR_GREEN, 14, SPR_MAGENTA, 24 };	// Multicolor definition { color, row, ... }
#elif defined __CBM__
	unsigned char spriteColors[] = { WHITE, BLUE, BROWN, PINK, GREEN, WHITE, WHITE, WHITE, BLACK, WHITE };	// 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	unsigned char *spriteColors = 0; //  All sprites use the default palette
#elif defined __NES__
    const unsigned char spriteColors[] = { SPR_VOID, SPR_BLACK, SPR_GREEN, SPR_YELLOW, 
										   SPR_VOID, SPR_BLACK, SPR_GREEN, SPR_YELLOW, 
										   SPR_VOID, SPR_VOID, SPR_VOID, SPR_VOID, 
										   SPR_VOID, SPR_VOID, SPR_VOID, SPR_VOID }; // 4 palettes of 3 colors (1st color is unused)
#endif

int main(void) 
{
	// Set text mode colors
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	clrscr();
	
	// Initialize sfx/bitmap
	InitSFX();
	InitBitmap();
	
	// Setup sprites
	LoadSprites("sprites.dat", spriteColors);
#if defined __APPLE2__
	CropSprite(0,8); // Mouse cursor only occupies third of frame	
#elif defined __ORIC__
	CropSprite(0,8);  // Mouse cursor only occupies half frame
	MultiColorSprite(1, multiColorDef);	// Make sprite 1 multi-colored
#elif defined __CBM__ 
	DoubleHeightSprite(1, 1);
	DoubleHeightSprite(2, 1);
	DoubleHeightSprite(3, 1);
	DoubleHeightSprite(4, 1);
#endif
	
	// Show splash screen
	MainMenu();

	// Load first scene
	LoadScene("scene01");
	PlayScene();
	
	//// HACK FOR TESTING SCENES ////
/* 	itemLast = 2;
	strcpy(&gameItems[0], "Sausage");
	strcpy(&gameItems[9], "Water");
	LoadScene("scene03"); */
	//// HACK FOR TESTING SCENES ////
	
	// Black-out screen and clear key
	DisableSprite(-1);	// "-1" disables all sprites
	HideBitmap();
	StopMusic();
	StopSFX();
	
    // Done
    return EXIT_SUCCESS;	
}

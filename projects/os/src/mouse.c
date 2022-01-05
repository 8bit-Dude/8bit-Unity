
#include "definitions.h"

#if defined(__NES__)
 #pragma rodata-name("BANK0")
 #pragma code-name("BANK0")
#endif

// Mouse sprite definitions
#if defined __APPLE2__	
	const unsigned char spriteColors[] = { };  // Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	const unsigned char spriteColors[] = { 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // Refer to atari palette in docs
#elif defined __CBM__
	const unsigned char spriteColors[] = { WHITE, BLUE, BROWN, PINK, GREEN, WHITE, WHITE, WHITE, BLACK, WHITE };	// 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	const unsigned char *spriteColors = 0; //  All sprites use the default palette
#elif defined __NES__
    const unsigned char spriteColors[] = { SPR_VOID, SPR_WHITE, SPR_BLACK, SPR_VOID, 
										   SPR_VOID, SPR_VOID, SPR_VOID, SPR_VOID, 
										   SPR_VOID, SPR_VOID, SPR_VOID, SPR_VOID, 
										   SPR_VOID, SPR_VOID, SPR_VOID, SPR_VOID }; // 4 palettes of 3 colors (1st color is unused)
#elif defined __ORIC__
	const unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };  // AIC color allows faster drawing!
#endif

// Mouse state
unsigned char* mouse;

void InitMouse()
{
	// Setup mouse sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteColors);
	EnableSprite(0);	
}

void ProcessMouse()
{	
	// Update mouse location
	mouse = GetMouse();
	LocateSprite(2*mouse[0]+4, mouse[1]+4);
	if (mouse[2] & MOU_LEFT) 
		SetSprite(0, 0); 
	else 
		SetSprite(0, 1);
}

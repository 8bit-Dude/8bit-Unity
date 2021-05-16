
#include "definitions.h"

// Mouse sprite definitions
#define spriteFrames 2
#if defined __APPLE2__	
	#define spriteCols   7
	#define spriteRows   7
	unsigned char spriteColors[] = { };  // Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	#define spriteCols   8
	#define spriteRows   6
	unsigned char spriteColors[] = { 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // Refer to atari palette in docs
#elif defined __CBM__
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { WHITE, BLUE, BROWN, PINK, GREEN, WHITE, WHITE, WHITE, BLACK, WHITE };	// 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	#define spriteCols   9
	#define spriteRows   13
	unsigned char *spriteColors = 0; //  All sprites use the default palette
#elif defined __ORIC__
	#define spriteCols   12
	#define spriteRows   8
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };  // AIC color allows faster drawing!
#endif

// Mouse state
unsigned char* mouse;

void InitMouse()
{
	// Setup mouse sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
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

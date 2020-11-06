
#include "unity.h"

// Sprite definitions
#if defined __APPLE2__
	#define spriteFrames 8
	#define spriteCols   7
	#define spriteRows   5
	unsigned char spriteColors[] = { };	//  Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__ //  5th color shared between all sprites
	#define spriteFrames 8
	#define spriteCols   8
	#define spriteRows   18
	unsigned char spriteColors[] = {0x12, 0xb4, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // Brown, Green, Pink
#elif defined __ORIC__
	#define spriteFrames 8
	#define spriteCols   12
	#define spriteRows   6
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	// Matching more or less with above
#elif defined __CBM__
	#define spriteFrames 8
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { GREEN, 0, 0, 0, 0, 0, 0, 0, RED, YELLOW };  // 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	#define spriteFrames 8
	#define spriteCols   7
	#define spriteRows   9
	unsigned char* spriteColors = defaultColors;	// All sprites use the default palette
#endif

void gameInit(void)
{
	// Load charmap
	scrollHeight = CHR_ROWS-1;
	LoadCharmap("quedex.map");
	InitCharmap();		
	ClearCharmap();
	EnterCharmapMode();
	
	// Setup sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
	EnableSprite(0);
#if __ATARI__
	EnableSprite(1);
	EnableSprite(2);
#endif
	
	// Show some text
	inkColor = RED;    PrintCharmap(0, CHR_ROWS-1,"H100");
	inkColor = CYAN;   PrintCharmap(5, CHR_ROWS-1,"M100");
	inkColor = YELLOW; PrintCharmap(10,CHR_ROWS-1,"$000");
}

void gameLoop(void)
{
	unsigned int sprXPRV, sprYPRV, sprX = 160, sprY = 100; // Backup and current sprite position
	unsigned char mapXPRV, mapYPRV, mapX = 20, mapY = 20;  // Previous and current map position
	unsigned char flagX, flagY1, flagY2;  // Coords of collision detection (accounting for model height)
	unsigned char sprFrame, joy; 
	clock_t gameClock;	
	
	while (1) {
		// Check clock
		if (clock() <= gameClock+4) 
			continue;
		gameClock = clock();

		// Save previous location
		sprXPRV = sprX; sprYPRV = sprY;
		mapXPRV = mapX; mapYPRV = mapY;
		
		// Process motion
		joy = GetJoy(0);
		if (!(joy & JOY_LEFT))  {
			     if (sprX>80)  sprX -= 4; 
			else if (mapX>1)   mapX -= 1;
			else if (sprX>4)   sprX -= 4;
			sprFrame = 0 + gameClock%2;
		} else 
		if (!(joy & JOY_RIGHT)) { 
				 if (sprX<240) sprX += 4; 
			else if (mapX<(charmapWidth-41)) mapX += 1;
			else if (sprX<316) sprX += 4; 
			sprFrame = 2 + gameClock%2;
		} else
		if (!(joy & JOY_UP)) {
				 if (sprY>50)  sprY -= 3; 
			else if (mapY>1)   mapY -= 1;
			else if (sprY>3)   sprY -= 3; 
			sprFrame = 4 + gameClock%2;
		} else
		if (!(joy & JOY_DOWN)) {
				 if (sprY<150) sprY += 3; 
			else if (mapY<(charmapHeight-25)) mapY += 1;
			else if (sprY<180) sprY += 3;
			sprFrame = 6 + gameClock%2;
		}  
		
		// Check if new position is allowed?
		LocatePixel(sprX, sprY);
		flagX = mapX+XToCol(pixelX);
		flagY1 = mapY+YToRow(pixelY);
		flagY2 = mapY+YToRow(pixelY+5);
		if (!(GetCharmapFlags(flagX, flagY1) & CHAR_WALKABLE) ||
			!(GetCharmapFlags(flagX, flagY2) & CHAR_WALKABLE)) {
			sprX = sprXPRV; sprY = sprYPRV;
			mapX = mapXPRV; mapY = mapYPRV;
		}	
	
		// Update screen
		ScrollCharmap(mapX, mapY);
		LocateSprite(sprX, sprY);
		SetSprite(0, sprFrame);		
	#if defined __ATARI__
		SetSprite(1, sprFrame+spriteFrames);		
		SetSprite(2, sprFrame+spriteFrames*2);		
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
	}	
}
	
int main (void)
{		
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	clrscr();
	
	// Run the game!
	gameInit();
	gameLoop();
		
    // Done
    return EXIT_SUCCESS;
}

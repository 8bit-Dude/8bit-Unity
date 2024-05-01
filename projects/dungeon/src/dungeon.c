
#include "definitions.h"

#if defined __APPLE2__
	unsigned char spriteColors[] = {};  //  All colors are pre-assigned in the sprite sheet	
#elif defined __ATARI__
	unsigned char spriteColors[] = { 0x24, 0xc8, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // Brown, Green (player), White (sword)
#elif defined __CBM__
	unsigned char spriteColors[] = { WHITE, YELLOW, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, RED, GREEN };  // 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	unsigned char* spriteColors = 0;	//  All sprites use the default palette
#elif defined __NES__
    const unsigned char spriteColors[] = { SPR_VOID, SPR_BROWN, SPR_GREEN, SPR_YELLOW, 
										   SPR_VOID, SPR_BROWN, SPR_GREEN, SPR_YELLOW, 
										   SPR_VOID, SPR_BROWN, SPR_GREEN, SPR_YELLOW, 
										   SPR_VOID, SPR_BROWN, SPR_GREEN, SPR_YELLOW }; // 4 palettes of 3 colors (1st color is unused)
#elif defined __ORIC__
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	// AIC used as default
	unsigned char multiColorDef[] = { SPR_YELLOW, 7, SPR_WHITE, 11 , SPR_MAGENTA, 17 };	// Multicolor definition of player { color, row, ...  }
#endif

// See player.c
extern unsigned char mapX, mapY;

void GameInit(void)
{	
	// Setup charmap
	InitCharmap(CROP_X, CMP_COLS-CROP_X, CROP_Y, CMP_ROWS-CROP_Y-1);
	LoadCharset("quedex.chr");
	LoadTileset("level1.tls", 40);
	LoadCharmap("level1.map", 64, 64);
	LoadActors("level1.act");
	ClearCharmap();
	DrawCharmap(mapX, mapY);	
	ShowCharmap();
		
	// Setup sprites
	LoadSprites("sprites.dat", spriteColors);
	sprScroll = 0;	// Disable auto-scroll
#if defined __ATARI__
	EnableMultiColorSprite(SPRITE_PLAYER);
#elif defined __ORIC__
	MultiColorSprite(SPRITE_PLAYER, multiColorDef);
	EnableSprite(SPRITE_PLAYER);
#else
	EnableSprite(SPRITE_PLAYER);
#endif
	sprScroll = 1; // Re-enable auto-scroll

	// Show stats
#if (defined __ORIC__)
	txtY = TXT_ROWS-1;
	txtX = 0; SetAttributes(RED);
	txtX = 5; SetAttributes(CYAN);
	txtX = TXT_COLS-9; SetAttributes(YELLOW);
	txtX = TXT_COLS-4;  SetAttributes(WHITE);
#endif	
	PrintHealth(); PrintArmor();
	PrintGold(); PrintKills();
	
	// Setup SFX
	InitSFX();
}

void GameLoop(void)
{	
	clock_t playerClock, actorClock;	
	
	while (1) {
		// Platform specific
	#if defined __APPLE2__
		clk += 1;	// Manually update clock on Apple 2			
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
		
		// Check player clock
		if (clock() > playerClock) {			
			playerClock = clock()+(TCK_PER_SEC/20);
			ProcessPlayer();			
		}

		// Check actor clock
		if (clock() > actorClock) {
			actorClock = clock()+(TCK_PER_SEC/6);
			ProcessActors();
			DisplayActors();
		}			
	}
}

int main (void)
{		
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	clrscr();

	// Show Title Screen
	SplashScreen();

	// Run Game	
	GameInit();
	GameLoop();
		
    // Done
    return EXIT_SUCCESS;
}

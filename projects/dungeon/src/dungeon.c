
#include "definitions.h"

#if defined __APPLE2__
	#define spriteCols    7
	#define spriteRows   16
	unsigned char charColors[] = {};    //  All colors are pre-assigned in the char sheet
	unsigned char spriteColors[] = {};  //  All colors are pre-assigned in the sprite sheet	
#elif defined __ATARI__
	#define spriteFrames 32
	#define spriteCols   8
	#define spriteRows   18
	unsigned char charColors[] = { 0x00, 0x0c, 0x78, 0x62, 0x12 };   // Black, White, Light Blue, Dark Blue, Red
	unsigned char spriteColors[] = { 0x24, 0xc8, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // Brown, Green (player), White (sword)
#elif defined __CBM__
	#define spriteFrames 32
	#define spriteCols   12
	#define spriteRows   21
	unsigned char charColors[] = { BLACK, WHITE, LBLUE };
	unsigned char spriteColors[] = { WHITE, YELLOW, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, RED, GREEN };  // 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	#define spriteFrames 32
	#define spriteCols    8
	#define spriteRows   12
	unsigned char charColors[] = {};    //  All colors are pre-assigned in the char sheet
	unsigned char* spriteColors = defaultColors;	// All sprites use the default palette
#elif defined __ORIC__
	#define spriteFrames 32
	#define spriteCols   12
	#define spriteRows   17
	unsigned char charColors[] = {};    //  All colors are pre-assigned in the char sheet
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	// AIC (sword), AIC (player), AIC (actors)
	unsigned char multiColorDef[] = { SPR_WHITE, 7, SPR_MAGENTA, 12 , SPR_YELLOW, 17 };	// Multicolor definition { color, row, ...  }
#endif

// See player.c
extern unsigned char mapX, mapY;

void GameInit(void)
{	
	// Setup charmap
	InitCharmap(CROP_X, CMP_COLS-CROP_X, CROP_Y, CMP_ROWS-CROP_Y-1);
	ClearCharmap();
	LoadCharset("quedex.chr", charColors);
	LoadTileset("level1.tls", 40, 2, 2);
	LoadCharmap("level1.map", 64, 64);
	LoadActors("level1.act");
	DrawCharmap(mapX, mapY);	
	ShowCharmap();
		
	// Setup sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
#if defined __ATARI__
	EnableSprite(SPRITE_PLAYER0);
	EnableSprite(SPRITE_PLAYER1);
#elif defined __ORIC__
	EnableSprite(SPRITE_PLAYER);
	MultiColorSprite(SPRITE_PLAYER, multiColorDef);
#else
	EnableSprite(SPRITE_PLAYER);
#endif

	// Show stats
#if (defined __ORIC__)
	SetAttributes(-1, TXT_ROWS-1, RED);
	SetAttributes( 4, TXT_ROWS-1, CYAN);
	SetAttributes(TXT_COLS-10, TXT_ROWS-1, YELLOW);
	SetAttributes(TXT_COLS-5,  TXT_ROWS-1, WHITE);
#endif	
	PrintHealth(); PrintArmor();
	PrintGold(); PrintKills();
	
	// Setup SFX
	InitSFX();
}

void GameLoop(void)
{	
	clock_t playerClock, actorClock;	

	// Restart music playback
#if (defined __ATARI__) || (defined __CBM__) || (defined __LYNX__)
	PlayMusic();
#endif	
	
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

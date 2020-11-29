
#include "definitions.h"

// Sprite definitions
#if defined __APPLE2__
	#define spriteFrames 64
	#define spriteCols   7	
	#define spriteRows   5	
	unsigned char spriteColors[] = { };	//  Colors are pre-assigned in the sprite sheet
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __ATARI__
	#define spriteFrames 18
	#define spriteCols   8
	#define spriteRows   10
	unsigned char spriteColors[] = {0x74, 0x24, 0xa6, 0xdc, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22 };	// Refer to atari palette in docs
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __ORIC__
	#define spriteFrames 17
	#define spriteCols   12
	#define spriteRows   6
	unsigned char spriteColors[] = { SPR_CYAN, SPR_MAGENTA, SPR_GREEN, SPR_WHITE, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	
	unsigned char inkColors[] = { CYAN, LPURPLE, LGREEN, GREY, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __CBM__
	#define spriteFrames 18
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW, LGREY, LGREY, LGREY, 0, CYAN, BLACK };	// P1, P2, P3, P4, Light1, Light2, Light3, n/a, Shared Color 1, Shared Color 2
	unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __LYNX__
	#define spriteFrames 18
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
	unsigned char inkColors[] = { DBLUE, RED, LGREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
#endif

// See interface.c
extern unsigned char gameMap, gameMode, gameStep;
#if defined __APPLE2__
  void CheckFileExists(const char* filename);
#elif defined __LYNX__
  void NextMusic(unsigned char blank);
#endif

// See interface.c
extern const char *mapList[LEN_MAPS];

int main (void) 
{
	unsigned char carryon;
#if defined __LYNX__
	clock_t bannerClock;
#endif   
	
	// Reset screen
	clrscr();
	bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);

	// Initialize modules
	InitJoy();
	InitBitmap();
	
	// Show banner
#if defined __LYNX__
    LoadBitmap("banner.img");
	bannerClock = clock()+2*TCK_PER_SEC;
    while (clock()<bannerClock && !kbhit());
#endif
	
	// Setup sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteFrames, spriteCols, spriteRows, spriteColors);	// see definitions.h

	// Main Loop
	while (1) {
        // Load menu screen
	#if defined __APPLE2__
		CheckFileExists("menu.img");
	#endif
        LoadBitmap("menu.img");
		
		// Show menu
	#ifndef __CBM__
		LoadMusic("speednik.mus", MUSICRAM);
		PlayMusic();
	#endif	
		EnterBitmapMode();
		GameMenu(); 
		DisableSprite(-1);
		ExitBitmapMode();
        StopMusic();
		
		// Loop through map list
		while (1) {			
			// Load map and navigation
			GameInit(mapList[gameMap]);

			// Run game
			InitSFX();
		#ifdef __LYNX__
			NextMusic(0);
		#endif	
			EnterBitmapMode();
			carryon = GameLoop();
			DisableSprite(-1);
			ExitBitmapMode();
			StopSFX();
		#ifdef __LYNX__
			StopMusic();
		#endif							
			// Carry on?
			gameStep = STEP_WARMUP;
			if (!carryon) { break; }
			if (gameMode == MODE_LOCAL) {
				// Go to next map
				gameMap += 1;
				if (gameMap >= LEN_MAPS) { gameMap=0; }
			}
		}
	}
    	
    // Done
    return EXIT_SUCCESS;
}

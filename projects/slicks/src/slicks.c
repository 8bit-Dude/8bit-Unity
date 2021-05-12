
#include "definitions.h"

// Sprite definitions
#if defined __APPLE2__
  #define spriteCols   7
  #define spriteRows   5
  const unsigned char spriteColors[] = { };	//  Colors are pre-assigned in the sprite sheet
  #if defined __DHR__	
	const unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
  #else
	const unsigned char inkColors[] = { BLUE, PURPLE, GREEN, ORANGE, WHITE };		// P1, P2, P3, P4, SERVER INFO
  #endif
#elif defined __ATARI__
  #define spriteCols   8
  #define spriteRows   10
  const unsigned char spriteColors[] = { 0x84, 0xe4, 0x24, 0xe4, 0xb6, 0xe4, 0xe8, 0xe4, 0x04, 0x04, 0x04, 0x04 };	// Refer to atari palette in docs
  const unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __ORIC__
  #define spriteCols   12
  #define spriteRows   6
  const unsigned char spriteColors[] = { PAL_CYAN, PAL_MAGENTA, PAL_GREEN, PAL_WHITE, PAL_AIC, PAL_AIC, PAL_AIC, PAL_AIC };	
  const unsigned char inkColors[] = { CYAN, LPURPLE, LGREEN, GREY, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __CBM__
  #define spriteCols   12
  #define spriteRows   21
  const unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW, LGREY, LGREY, LGREY, 0, CYAN, BLACK };	// P1, P2, P3, P4, Light1, Light2, Light3, n/a, Shared Color 1, Shared Color 2
  const unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __LYNX__
  #define spriteCols   8
  #define spriteRows   9
  unsigned char *spriteColors =  0;  //  All sprites use the default palette
  const unsigned char inkColors[] = { DBLUE, ORANGE, LGREEN, YELLOW, WHITE };	// P1, P2, P3, P4, SERVER INFO
#elif defined __NES__
  #define spriteCols    8
  #define spriteRows    8
  const unsigned char spriteColors[] = { PAL_BLACK, PAL_BLUE, PAL_CYAN, PAL_GREY,  PAL_BLACK, PAL_RED, PAL_CYAN, PAL_GREY, 
										 PAL_BLACK, PAL_GREEN, PAL_CYAN, PAL_GREY, PAL_BLACK, PAL_YELLOW, PAL_CYAN, PAL_GREY }; // 4 palettes of 4 colors
  const unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW, WHITE };		// P1, P2, P3, P4	
#endif

// Build Information
const char* buildInfo = "BUILD: 2021/04/18";

// List of available maps
unsigned char mapNum  = 12;
const char *mapList[] = {"arizona","arto","cramp","freeway","gta","island","mtcarlo","rally","river","stadium","suzuka","trial"};

// List of lap goals
unsigned char lapNumber[] = { 5, 10, 20, 50 };

// Game state
extern unsigned char gameMap, gameMode, gameStep;

#if defined __LYNX__
  void NextMusic(unsigned char blank) {}
#endif

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
    LoadBitmap("promo.img");
	bannerClock = clock()+8*TCK_PER_SEC;
    while (clock()<bannerClock && !kbhit());
#endif
	
	// Setup sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteFrames, spriteCols, spriteRows, spriteColors);	// see definitions.h
#if defined __LYNX__
	RecolorSprite(1, 0, 0x08); // BLUE -> ORANGE
	RecolorSprite(2, 0, 0x05); // BLUE -> GREEN
	RecolorSprite(3, 0, 0x09); // BLUE -> YELLOW
	sprCushion = 5;
#elif defined __NES__
	RecolorSprite(1, 0, 1); // Use Palette 1
	RecolorSprite(2, 0, 2); // Use Palette 2
	RecolorSprite(3, 0, 3); // Use Palette 3
#endif   

	// Main Loop
	while (1) {
        // Load menu screen
        LoadBitmap("menu.img");
		
		// Show menu
		LoadMusic("speednik.mus");
		PlayMusic();
		ShowBitmap();
		GameMenu(); 
		DisableSprite(-1);
		HideBitmap();
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
			ShowBitmap();
			carryon = GameLoop();
			DisableSprite(-1);
			HideBitmap();
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
				if (gameMap >= mapNum) { gameMap=0; }
			}
		}
	}
    	
    // Done
    return EXIT_SUCCESS;
}

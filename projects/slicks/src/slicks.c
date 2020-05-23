
#include "game.h"

int main (void) 
{
	unsigned char carryon;
    
	// Reset screen
	clrscr();
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);

	// Initialize modules
	InitJoy();
	InitBitmap();
	InitSprites(spriteFrames, spriteCols, spriteRows, spriteColors);	// see definitions.h

	// Load Sound Track
#ifndef __CBM__
	LoadMusic("speednik.mus", MUSICRAM);
#endif	
	// Main Loop
	while (1) {
        // Load menu screen
	#if defined __APPLE2__
		CheckFileExists("menu.map");
	#endif
        LoadBitmap("menu.map");
		
		// Show menu
	#ifndef __CBM__
		PlayMusic(MUSICRAM);
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
			EnterBitmapMode();
			carryon = GameLoop();
			DisableSprite(-1);
			ExitBitmapMode();
			StopMusic();
			StopSFX();
						
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


#include "game.h"

#if defined __LYNX__
const char *musicList[4] = {"chase.mus","driven.mus","stroll.mus","whirlwnd.mus"};
unsigned char musicSel = 0;
void NextMusic() {
	// Select next music track
	LoadMusic(musicList[musicSel], MUSICRAM);
	if (++musicSel > 3) musicSel = 0;
	PlayMusic(MUSICRAM);
}
#endif

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
		#ifdef __LYNX__
			NextMusic();
		#endif	
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

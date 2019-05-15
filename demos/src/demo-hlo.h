
#include "unity.h"

static const char welcome[] = "Welcome to 8bit Unity!";
static const char presskey[] = " > press SPACE bar < ";

int DemoHLO(void) 
{
    unsigned char xSize, ySize;

	// Clear screen
	clrscr();
	
    // Ask for the screen size
    screensize(&xSize, &ySize);

    // Top line
    cputc(CH_ULCORNER);
    chline(xSize - 2);
    cputc(CH_URCORNER);

    // Vertical line, left side 
    cvlinexy(0, 1, ySize - 2);

    // Bottom line
    cputc(CH_LLCORNER);
    chline(xSize - 2);
    cputc(CH_LRCORNER);

    // Vertical line, right side 
    cvlinexy(xSize - 1, 1, ySize - 2);

    // Write the greeting in the mid of the screen
    gotoxy((xSize - strlen(welcome))/2, ySize/2-1);
    cprintf("%s", welcome);
    gotoxy((xSize - strlen(presskey))/2, ySize/2+1);
    cprintf("%s", presskey);
	
	// Play music until keyboard is pressed
	PlayMusic(MUSICRAM);
#if defined __APPLE2__	// no interrupt on Apple, need to progress step-by-step
	while (!kbhit () || cgetc () != KEY_SP) { PlayMusic(0); }
#else
	cgetc();
#endif
	StopMusic();
	
	// Clear screen
	clrscr();	
	
    // Done
    return EXIT_SUCCESS;	
}

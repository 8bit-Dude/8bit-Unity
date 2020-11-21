
#include "unity.h"

const char welcomeMsg[] = "Welcome to 8bit Unity!";
extern const char keyNext, pressKeyMsg[];

int DemoHello(void) 
{
    unsigned char xSize, ySize;

	// Clear screen
	clrscr();
	
    // Ask for the screen size
    screensize(&xSize, &ySize);
#ifndef __LYNX__
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
#endif	
    // Write the greeting in the mid of the screen
    gotoxy((xSize - strlen(welcomeMsg))/2, ySize/2-1);
    cprintf(welcomeMsg);
    gotoxy((xSize - strlen(pressKeyMsg))/2, ySize/2+1);
    cprintf(pressKeyMsg);
	
	// Play music until keyboard is pressed
	LoadMusic("demo.mus", MUSICRAM);
	PlayMusic();
	
	// Wait until 'SPACE' is pressed
	while (!kbhit () || cgetc () != keyNext) {
	#if defined __APPLE2__	
		UpdateMusic();	 // Apple has no interrupts: need to progress track manually
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif
	}

	// Stop music
	StopMusic();
	
    // Done
    return EXIT_SUCCESS;	
}

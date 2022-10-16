
#include "unity.h"

extern const char nextKey, nextMsg[];
const char helloMsg[] = "Welcome to 8bit Unity!";
const char versionMsg[] = "(version 0.5)";
unsigned char helloCol, versionCol, nextCol;

int DemoHello(void) 
{
    unsigned char xSize, ySize;

	// Clear screen
	clrscr();
	
    // Ask for the screen size
    screensize(&xSize, &ySize);
#if defined(__APPLE2__) || defined(__ATARI__) || defined(__CBM__)
    // Top line
    cputc(CH_ULCORNER);
    chline(xSize - 2);
    cputc(CH_URCORNER);

    // Vertical line, left-side 
    cvlinexy(0, 1, ySize - 2);

    // Bottom line
    cputc(CH_LLCORNER);
    chline(xSize - 2);
    cputc(CH_LRCORNER);

    // Vertical line, right-side 
    cvlinexy(xSize - 1, 1, ySize - 2);
#endif	

    // Write the greeting in the mid of the screen
	helloCol = (xSize - strlen(helloMsg))/2;
	versionCol = (xSize - strlen(versionMsg))/2;
	nextCol  = (xSize - strlen(nextMsg))/2;
    gotoxy(helloCol, ySize/2-2); cprintf(helloMsg);
    gotoxy(versionCol, ySize/2); cprintf(versionMsg);
    gotoxy(nextCol,  ySize/2+2); cprintf(nextMsg);
	
	// Play music until keyboard is pressed
	LoadMusic("demo.mus");
	PlayMusic();
		
	// Wait until 'SPACE' is pressed
	while (!kbhit () || cgetc () != nextKey) {
	#if defined(__APPLE2__)
		UpdateMusic();	 // Apple has no interrupts: need to progress track manually
	#elif defined(__LYNX__) || defined(__NES__)
		UpdateDisplay(); // Manually refresh Display
	#endif
	}
	
	// Reset state
	StopMusic();
	
    // Done
    return EXIT_SUCCESS;	
}

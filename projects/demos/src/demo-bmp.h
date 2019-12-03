
#include "unity.h"

extern const char keyNext;

int DemoBMP(void) 
{
	// Clear screen
	clrscr();
	
	// Prepare bitmap
	InitBitmap();
	LoadBitmap("banner.map");
	EnterBitmapMode();
	
	// Wait until 'SPACE' is pressed
	while (!kbhit () || cgetc () != keyNext) {	
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
	}
	
	// Exit bitmap mode
	ExitBitmapMode();
	
    // Done
    return EXIT_SUCCESS;	
}
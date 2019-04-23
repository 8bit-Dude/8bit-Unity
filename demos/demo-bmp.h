
#include "unity.h"

int DemoBMP(void) 
{
	// Reset screen
	clrscr();
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);

	// Prepare bitmap
	InitBitmap();
	LoadBitmap("banner.map");
	EnterBitmapMode();
	
	// Wait for any key
	cgetc();
	
	// Exit bitmap mode
	ExitBitmapMode();
	
    // Done
    return EXIT_SUCCESS;	
}
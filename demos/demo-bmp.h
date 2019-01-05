
#include "unity.h"

int DemoBMP(void) 
{
	unsigned char i,j;

	// Reset screen
	clrscr();
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);

	// Prepare bitmap
	InitBitmap();
	LoadBitmap("banner.map");
	EnterBitmapMode();
	
	// Wait until keyboard is pressed
	cgetc();
	ExitBitmapMode();
	
    // Done
    return EXIT_SUCCESS;	
}
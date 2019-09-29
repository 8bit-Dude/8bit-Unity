
#include "unity.h"

int DemoBMP(void) 
{
	// Prepare bitmap
	InitBitmap();
	LoadBitmap("banner.map");
	EnterBitmapMode();
	
	// Wait until 'SPACE' is pressed
	while (!kbhit () || cgetc () != KEY_SP) {	
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
	}
	
	// Exit bitmap mode
	ExitBitmapMode();
	
    // Done
    return EXIT_SUCCESS;	
}
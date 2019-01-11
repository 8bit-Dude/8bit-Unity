
#include "unity.h"

int DemoJOY(void) 
{
	unsigned char i, y, joy;
	
	// Prepare bitmap
	InitBitmap();
	ClearBitmap();	
	EnterBitmapMode();
	
#if defined __CBM__	
	InitJoy34();
#endif

	// Print header
	colorBG = BLACK; colorFG = WHITE;
	PrintStr(1,1, "Joystick demo (press SPACE for next)");
	for (i=0; i<JOY_MAX; i++) {
		y = 8+2*i;
		PrintStr(0, y, "Joy  :");
		PrintChr(4, y, &charDigit[3*i]);		
	}
	
	// Display joystick actions until 'SPACE' is pressed
	while (!kbhit () || cgetc () != KEY_SP) {
		for (i=0; i<JOY_MAX; i++) {
			y = 8+2*i;
			joy = GetJoy(i);
			if (joy & JOY_UP)    { PrintStr(8,  y, "U"); } else { PrintStr(8,  y, " "); }
			if (joy & JOY_DOWN)  { PrintStr(10, y, "D"); } else { PrintStr(10, y, " "); }
			if (joy & JOY_LEFT)  { PrintStr(12, y, "L"); } else { PrintStr(12, y, " "); }
			if (joy & JOY_RIGHT) { PrintStr(14, y, "R"); } else { PrintStr(14, y, " "); }
			if (joy & JOY_FIRE)  { PrintStr(16, y, "F"); } else { PrintStr(16, y, " "); }		
		}
	}
	
	// Black-out screen
	ExitBitmapMode();	
	
    // Done
    return EXIT_SUCCESS;	
}
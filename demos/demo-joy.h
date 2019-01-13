
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
		PrintStr(10, y, "Joy  :");
		PrintChr(14, y, &charDigit[3*i]);		
	}
	
	// Display joystick actions until 'SPACE' is pressed
	while (!kbhit () || cgetc () != KEY_SP) {
		for (i=0; i<JOY_MAX; i++) {
			y = 8+2*i;
			joy = GetJoy(i);
			if (joy & JOY_UP)    { PrintStr(18, y, "U"); } else { PrintStr(18, y, " "); }
			if (joy & JOY_DOWN)  { PrintStr(20, y, "D"); } else { PrintStr(20, y, " "); }
			if (joy & JOY_LEFT)  { PrintStr(22, y, "L"); } else { PrintStr(22, y, " "); }
			if (joy & JOY_RIGHT) { PrintStr(24, y, "R"); } else { PrintStr(24, y, " "); }
			if (joy & JOY_FIRE)  { PrintStr(26, y, "F"); } else { PrintStr(26, y, " "); }		
		}
	}
	
	// Black-out screen
	ExitBitmapMode();	
	
    // Done
    return EXIT_SUCCESS;	
}
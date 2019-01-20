
#include "unity.h"

int DemoJOY(void) 
{
	unsigned char i, joy;
		
#if defined __CBM__	
	InitJoy34();
#endif

	// Print header
	clrscr();
	gotoxy (1, 2);
	printf("Joystick demo (press SPACE for next)\n");	
	
	// Display joystick actions until 'SPACE' is pressed
	while (!kbhit () || cgetc () != KEY_SP) {
		for (i=0; i<JOY_MAX; i++) {
			joy = GetJoy(i);
			gotoxy (0, 8+2*i);
			cprintf ("Joy %i: ", i+1);
			cprintf ("%s ", joy & JOY_UP    ? "UP   ": "     ");
			cprintf ("%s ", joy & JOY_DOWN  ? "DOWN ": "     ");
			cprintf ("%s ", joy & JOY_LEFT  ? "LEFT ": "     ");
			cprintf ("%s ", joy & JOY_RIGHT ? "RIGHT": "     ");
			cprintf ("%s ", joy & JOY_FIRE  ? "FIRE ": "     ");
		}
	}
	
    // Done
    return EXIT_SUCCESS;	
}
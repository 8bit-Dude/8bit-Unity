
#include "unity.h"

int main (void) 
{
	unsigned char joy;

	// Reset screen
	clrscr();
	
	// Display joystick actions until 'q' was pressed
	while (!kbhit () || cgetc () != 'Q') {
		joy = GetJoy(0);
		gotoxy (0, 8);
		cprintf ("%s ", joy & JOY_UP    ? "UP   ": "     ");
		cprintf ("%s ", joy & JOY_DOWN  ? "DOWN ": "     ");
		cprintf ("%s ", joy & JOY_LEFT  ? "LEFT ": "     ");
		cprintf ("%s ", joy & JOY_RIGHT ? "RIGHT": "     ");
		cprintf ("%s ", joy & JOY_FIRE  ? "FIRE ": "     ");	
	}
	
    // Done
    return EXIT_SUCCESS;	
}
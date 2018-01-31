
#include "unity.h"

int TestJOY(void) 
{
	unsigned char i, joy;

	// Reset screen
	clrscr();
	
	// Init Joy 3/4 (only on CBM)
	if (JOY_MAX > 2) { InitJoy34(); }
	
	// Display joystick actions until 'Q' is pressed
	printf("PRESS Q FOR NEXT TEST\n");
	while (!kbhit () || cgetc () != 'Q') {
		for (i=0; i<JOY_MAX; i++) {
			switch (i) {
				case 0:
				case 1: joy = GetJoy(i); break;
				case 2: joy = GetJoy3(); break;
				case 3: joy = GetJoy4(); break;
			}
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
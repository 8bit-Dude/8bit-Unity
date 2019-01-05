
#include "unity.h"

int TestJOY(void) 
{
	unsigned char i, joy;

	// Reset screen
	clrscr();
	
#if defined __CBM__	
	InitJoy34();
#endif

	// Display joystick actions until 'Q' is pressed
	gotoxy (1, 2);
	printf("JOYSTICK TEST (PRESS SPACE NEXT TEST)\n");
	while (!kbhit () || cgetc () != KEY_SP) {
		for (i=0; i<JOY_MAX; i++) {
			switch (i) {
				case 0:
				case 1: joy = GetJoy(i); break;
#if defined __CBM__	
				case 2: joy = GetJoy3(); break;
				case 3: joy = GetJoy4(); break;
#endif
			}
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
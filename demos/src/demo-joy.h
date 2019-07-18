
#include "unity.h"

#if defined __ATMOS__
	const char* joyList[] = { "WASD+CTR", "IJKL+RET", "PASE/IJK 1", "PASE/IJK 2" };
#else
	const char* joyList[] = { "JOY 1", "JOY 2", "JOY 3", "JOY 4" };
#endif

int DemoJOY(void) 
{
	unsigned char i, joy;
	
	//	Init joystick adapters
	InitJoy();
	
	// Print header
	clrscr();
	gotoxy (1, 2);
	printf("Joystick demo (press SPACE for next)\n");	
	for (i=0; i<JOY_MAX; i++) {
		gotoxy (7, 8+2*i);
		cprintf (joyList[i]);
	}
	
	// Display joystick actions until 'SPACE' is pressed
	while (!kbhit () || cgetc () != KEY_SP) {
		for (i=0; i<JOY_MAX; i++) {
			joy = GetJoy(i);
			gotoxy (19, 8+2*i);
			cprintf ("%s ", joy & JOY_UP    ? "U ": "  ");
			cprintf ("%s ", joy & JOY_DOWN  ? "D ": "  ");
			cprintf ("%s ", joy & JOY_LEFT  ? "L ": "  ");
			cprintf ("%s ", joy & JOY_RIGHT ? "R ": "  ");
			cprintf ("%s ", joy & JOY_BTN1  ? "F ": "  ");
		}
	}
	
    // Done
    return EXIT_SUCCESS;	
}
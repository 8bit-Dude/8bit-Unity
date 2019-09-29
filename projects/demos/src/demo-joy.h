
#include "unity.h"

#if defined __ATMOS__
	const char* joyList[] = { "WASD+CTR", "IJKL+RET", "PASE/IJK 1", "PASE/IJK 2" };
#else
	const char* joyList[] = { "JOY 1", "JOY 2", "JOY 3", "JOY 4" };
#endif

int DemoJOY(void) 
{
	unsigned char i, joy, state[11];
	
	//	Init joystick adapters
	InitJoy();
	
	// Print header
	clrscr();
	gotoxy (2, 2);
	cprintf("Joystick demo (press SPACE for next)");	
	for (i=0; i<JOY_MAX; i++) {
		gotoxy (7, 8+2*i);
		cprintf(joyList[i]);
	}
	
	// Display joystick actions until 'SPACE' is pressed
	while (!kbhit () || cgetc () != KEY_SP) {
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif			
		for (i=0; i<JOY_MAX; i++) {
			joy = GetJoy(i);
			memset(state, ' ', 10);
			if (joy & JOY_UP)    { state[0] = 'U'; }
			if (joy & JOY_DOWN)  { state[2] = 'D'; }
			if (joy & JOY_LEFT)  { state[4] = 'L'; }
			if (joy & JOY_RIGHT) { state[6] = 'R'; }
			if (joy & JOY_BTN1)  { state[8] = 'F'; }
			gotoxy (19, 8+2*i);
			cprintf(state);
		}
	}
	
    // Done
	clrscr();	
    return EXIT_SUCCESS;	
}
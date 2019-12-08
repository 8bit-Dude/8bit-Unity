
#include "unity.h"

extern const char keyNext, pressKeyMsg[];

#if defined __ATMOS__
	const char* joyList[] = { "WASD+CTR", "IJKL+RET", "PASE/HUB 1", "PASE/HUB 2" };
#elif defined __LYNX__
	const char* joyList[] = { "JOY 1", "HUB 1", "HUB 2" };
#else 
	const char* joyList[] = { "JOY 1", "JOY 2", "JOY 3", "JOY 4" };
#endif

int DemoJOY(void) 
{
	unsigned char i, joy, state[11];
	
	// Clear screen
	clrscr();
	
	//	Init joystick adapters
	InitJoy();
	
	// Print header
	gotoxy (8, 2);
	cprintf(pressKeyMsg);	
	for (i=0; i<JOY_MAX; i++) {
		gotoxy (7, 4+2*i);
		cprintf(joyList[i]);
	}
	
	// Display joystick actions until 'SPACE' is pressed
	while (!kbhit () || cgetc () != keyNext) {
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
			gotoxy (19, 4+2*i);
			cprintf(state);
		}
	}
	
    // Done
    return EXIT_SUCCESS;	
}

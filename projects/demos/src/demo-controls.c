
#include "unity.h"

extern const char nextKey, nextMsg[];
extern unsigned char nextCol;

#if defined(__APPLE2__) || defined(__ATARIXL__)
  #if defined __HUB__
	const char* joyList[] = { "JOY 1", "JOY 2", "HUB 1", "HUB 2", "HUB 3" };
  #else
	const char* joyList[] = { "JOY 1", "JOY 2" };
  #endif	  
#elif defined(__ATARI__)
  #if defined __HUB__
	const char* joyList[] = { "JOY 1", "JOY 2", "JOY 3", "JOY 4", "HUB 1", "HUB 2", "HUB 3" };
  #else
	const char* joyList[] = { "JOY 1", "JOY 2", "JOY 3", "JOY 4" };
  #endif	  
#elif defined(__CBM__)
  #if defined __HUB__
	const char* joyList[] = { "JOY 1", "JOY 2", "HUB 1", "HUB 2", "HUB 3" };
  #else
	const char* joyList[] = { "JOY 1", "JOY 2", "JOY 3", "JOY 4" };
  #endif	  
#elif defined __LYNX__
	const char* joyList[] = { "JOY 1", "HUB 1", "HUB 2", "HUB 3" };
#elif defined __NES__
	const char* joyList[] = { "JOY 1", "JOY 2", "HUB 1", "HUB 2", "HUB 3" };
#elif defined __ORIC__
	const char* joyList[] = { "WASD+CTR", "IJKL+RET", "IJK/HUB 1", "IJK/HUB 2", "HUB 3" };
#endif

int DemoControls(void) 
{
	unsigned char i, *mou, joy;
	unsigned char state[] = "           ";
	
	// Clear screen
	clrscr();
	
	//	Init joystick adapters
	InitJoy();
	
	// Print header
	gotoxy(nextCol, 2); cprintf(nextMsg);	
	gotoxy(7, 4);       cprintf("MOUSE");	
	for (i=0; i<JOY_MAX; i++) {
		gotoxy(7, 6+2*i); cprintf(joyList[i]);
	}
	
	// Display joystick actions until 'SPACE' is pressed
	while (!kbhit () || cgetc () != nextKey) {
		// Display mouse state
		mou = GetMouse();
		gotoxy (18, 4); cprintf("%u  ", mou[0]);
		gotoxy (23, 4); cprintf("%u  ", mou[1]);
		strcpy(state, "   ");
		if (mou[2] & MOU_LEFT)   state[0] = 'L';
		if (mou[2] & MOU_MIDDLE) state[1] = 'M';
		if (mou[2] & MOU_RIGHT)  state[2] = 'R';
		gotoxy (28, 4); cprintf(state);
			
		// Display joystick states
		for (i=0; i<JOY_MAX; i++) {
			joy = GetJoy(i);
			memset(state, ' ', 11);
			if (joy & JOY_UP)    state[0] = 'U';
			if (joy & JOY_DOWN)  state[2] = 'D';
			if (joy & JOY_LEFT)  state[4] = 'L';
			if (joy & JOY_RIGHT) state[6] = 'R';
			if (joy & JOY_BTN1)  state[8] = 'A';
			if (joy & JOY_BTN2)  state[10] = 'B';
			gotoxy (19, 6+2*i); cprintf(state);
		}
	#if defined(__APPLE2__)
		clk += 1;  // Manually update clock on Apple 2
	#elif defined(__LYNX__) || defined(__NES__)
		UpdateDisplay(); // Manually refresh Display
	#endif
	}
	
    // Done
    return EXIT_SUCCESS;	
}

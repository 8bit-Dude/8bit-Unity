
#include "unity.h"

#define NUM_SLOTS 4

unsigned char joystick[NUM_SLOTS];

int main (void)
{
	unsigned char joy, lastKey, res;
	unsigned char myslot, slot, len;
	unsigned char input[12];
	unsigned int ID, PASS;
	
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	clrscr(); 

#if defined __LYNX__	
	// Set virtual keyboard location
	SetKeyboardOverlay(50,70);
#endif

	// Init Network
	if (InitNetwork() != NETWORK_OK) {
		gotoxy(0,0); cprintf("Network Error");
		while (!kbhit()); return EXIT_SUCCESS;		
	}

	// Host or Join?
	gotoxy(0,0); cprintf("(H)ost or (J)oin Session?");
	while (1) {
		res = cgetc();
		if (res == KB_H || res == KB_J)
			break;
	}
		
	// Setup Connection
	switch (res) {
	case KB_H:
		// Host new Session (ID:PASS generated automatically)
		res = EasyHost(EASY_UDP, NUM_SLOTS, &ID, &PASS); // You can use EASY_TCP or EASY_UDP
		switch (res) {
		case EASY_OK:	
			gotoxy(0,1); cprintf("Session ID: %i", ID);
			gotoxy(0,2); cprintf("Session PASS: %i", PASS);
			gotoxy(0,3); cprintf("Hosting session", ID);
			myslot = easyBuffer[0];
			break;
		case EASY_TIMEOUT:
			gotoxy(0,1); cprintf("Error: connection timed-out");			
			while (!kbhit()); return EXIT_SUCCESS;		
		}
		break;
		
	case KB_J:
		// Request credential (ID:PASS must match host)
		gotoxy(0,1); cprintf("Session ID: "); scanf("%i", &ID);  // Debug: PrintNum(30,1,ID);
		gotoxy(0,2); cprintf("Session PASS: "); scanf("%i", &PASS); // Debug: PrintNum(30,2,PASS);
		
		// Try joining session
		res = EasyJoin(EASY_UDP, &ID, &PASS);
		switch (res) {
		case EASY_OK:
			gotoxy(0,3); cprintf("Joined session", ID);
			myslot = easyBuffer[0];
			break;
		case EASY_CRED:
			gotoxy(0,3); cprintf("Error: ID/PASS incorrect");
			while (!kbhit()); return EXIT_SUCCESS;
		case EASY_FULL:
			gotoxy(0,3); cprintf("Error: session full");
			while (!kbhit()); return EXIT_SUCCESS;
		case EASY_TIMEOUT:
			gotoxy(0,3); cprintf("Error: connection timed-out");			
			while (!kbhit()); return EXIT_SUCCESS;
		}
	}
	
	// Show slot headers
	for (slot=0; slot<NUM_SLOTS; slot++) {	
		gotoxy(9, 6+2*slot); cprintf("Player %i", slot);
	}
	gotoxy(8, 22); cprintf("(press any key to exit)");
	
	// Exchange Joystick input in loop
	while (!kbhit()) {
		// Get local joystick state
		joy = GetJoy(0);
		joystick[myslot] = joy;
		
		// Exchange joystick states over Easynet
		EasySend(&joy, 1);			// Send my joystick state (1 byte)
		while (EasyRecv(0) == EASY_OK) {  // Receive other joystick states (time-out set to 0)
			slot = easyBuffer[0];	// Slot of client
			len  = easyBuffer[1];	// Number of bytes
			joystick[slot] = easyBuffer[2];
		}
		
		// Display all joystick states
		for (slot=0; slot<NUM_SLOTS; slot++) {
			memset(input, ' ', 11);
			joy = joystick[slot];
			if (joy != 0) {
				if (joy & JOY_UP)    input[0]  = 'U';
				if (joy & JOY_DOWN)  input[2]  = 'D';
				if (joy & JOY_LEFT)  input[4]  = 'L';
				if (joy & JOY_RIGHT) input[6]  = 'R';
				if (joy & JOY_BTN1)  input[8]  = 'A';
				if (joy & JOY_BTN2)  input[10] = 'B';
			}
			gotoxy(19, 6+2*slot); cprintf(input);
		}
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif
	}

	// Quit graciously
	gotoxy(0,4); cprintf("Left session");			
	EasyQuit();
	
	// Done
	lastKey = cgetc();
	while (!kbhit());
    return EXIT_SUCCESS;
}

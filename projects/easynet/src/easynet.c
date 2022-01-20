
#include "unity.h"

#define NUM_SLOTS 16
#define LEN_NAME  8
#define FPS       10

typedef struct {
	unsigned char x, y, c;
} player_t;

player_t *player;
player_t players[NUM_SLOTS];
unsigned char mySlot, myName[LEN_NAME];

int main (void)
{
	unsigned char joy, res;
	unsigned char slot, len;
	unsigned char input[12];
	unsigned int ID, PASS;
	clock_t timer = clock();
	
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	clrscr(); 

#if defined(__LYNX__) || defined(__NES__)
	// Set virtual keyboard location
	SetKeyboardOverlay(50,70);
#endif

	// Init Network
	if (InitNetwork() != NETWORK_OK) {
		gotoxy(0,0); cprintf("Network Error");
		while (!kbhit()); return EXIT_SUCCESS;		
	}

	// Host or Join?
	gotoxy(3,0); cprintf("Easynet Demo (v0.5)");
	gotoxy(0,1); cprintf("Choose name: "); scanf("%s", myName);  
	gotoxy(0,2); cprintf("(H)ost or (J)oin Session?");
	while (1) {
	#if defined(__LYNX__) || defined(__NES__)	
		res = cgetKeyboardOverlay();
	#else
		res = cgetc();
	#endif
		if (res == KB_H || res == KB_J)
			break;
	}
		
	////////////////////////	
	// Setup Connection
	switch (res) {
	case KB_H:
		// Host new Session (ID:PASS generated automatically)
		res = EasyHost(EASY_UDP, NUM_SLOTS, &ID, &PASS); // You can use EASY_TCP or EASY_UDP
		switch (res) {
		case EASY_OK:	
			mySlot = easyBuffer[0];
			break;
		case EASY_TIMEOUT:
			gotoxy(0,3); cprintf("Error: connection timed-out");			
			while (!kbhit()); return EXIT_SUCCESS;		
		}
		break;
		
	case KB_J:
		// Request credential (ID:PASS must match host)
		gotoxy(0,3); cprintf("Session ID: "); scanf("%i", &ID);  
		gotoxy(0,4); cprintf("Session PASS: "); scanf("%i", &PASS); 
		
		// Try joining session
		res = EasyJoin(EASY_UDP, &ID, &PASS);
		switch (res) {
		case EASY_OK:
			mySlot = easyBuffer[0];
			break;
		case EASY_CRED:
			gotoxy(0,5); cprintf("Error: ID/PASS incorrect");
			while (!kbhit()); return EXIT_SUCCESS;
		case EASY_FULL:
			gotoxy(0,5); cprintf("Error: session full");
			while (!kbhit()); return EXIT_SUCCESS;
		case EASY_TIMEOUT:
			gotoxy(0,5); cprintf("Error: connection timed-out");			
			while (!kbhit()); return EXIT_SUCCESS;
		}
		break;
	default:
		// Wrong selection
		gotoxy(0,3); cprintf("Error: connection aborted");			
		while (!kbhit()); return EXIT_SUCCESS;		
	}
	
	//////////////////////////////
	// Show connection status
	clrscr(); gotoxy(0,0);
	cprintf("Session ID:%u / PASS:%u / SLOT:%i/%i", ID, PASS, mySlot, NUM_SLOTS-1);
	
	//////////////////////////////
	// Setup player data
	player = &players[mySlot];
	player->x = clock()%24+8;	// Pseudo-random position
	player->y = clock()%15+2;
	player->c = myName[0];
	
	//////////////////////////////
	// Run mini interactive game
	while (!kbhit() || cgetc() != KB_Q) {
		// Use timer to regulate game
		if (clock() < timer)
			continue;
		timer = clock() + TCK_PER_SEC / FPS;

		// Eraze previous location
		player = &players[mySlot];
		gotoxy(player->x, player->y);
		cprintf(" ");
		
		// Move using joystick
		joy = GetJoy(0);
		if (!(joy & JOY_UP))    player->y -= 1;
		if (!(joy & JOY_DOWN))  player->y += 1;
		if (!(joy & JOY_LEFT))  player->x -= 1;
		if (!(joy & JOY_RIGHT)) player->x += 1;

		// Constrain coordinates
		player->x = MAX(player->x, 8);
		player->x = MIN(player->x, 31);
		player->y = MAX(player->y, 2);
		player->y = MIN(player->y, 16);
		
		// Show new location
		gotoxy(player->x, player->y);
		cprintf("%c", player->c);		
		
		// Exchange data over Easynet
		EasySend(player, 3);		      // Send my player state (3 bytes)
		while (EasyRecv(0) == EASY_OK) {  // Receive other player states (time-out set to 0)
			// Get player
			slot = easyBuffer[0];
			len  = easyBuffer[1];
			player = &players[slot];
			
			// Eraze previous location
			if (player->y) {
				gotoxy(player->x, player->y);
				cprintf(" ");
			}
			
			// Show new location
			memcpy(player, &easyBuffer[2], 3);
			gotoxy(player->x, player->y);
			cprintf("%c", player->c);			
		}		
		
		// Refresh screen (if needed)
	#if defined(__LYNX__) || defined(__NES__)
		UpdateDisplay();
	#endif
	}

	// Quit Easynet graciously
	EasyQuit();
	
	// Clear screen and show left message
	clrscr(); gotoxy(0,0); cprintf("Left session");			
	cgetc(); while (!kbhit());
	
	// Done
    return EXIT_SUCCESS;
}

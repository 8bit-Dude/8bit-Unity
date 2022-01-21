
#include "unity.h"

#define NUM_SLOTS 16
#define LEN_NAME  8
#define FPS       10


typedef struct {
	unsigned char x, y, c;
} player_t;

unsigned char myName[LEN_NAME];
player_t players[NUM_SLOTS];
player_t *player;

int main (void)
{
	unsigned char joy, res, slot, len, i;
	unsigned char *packetData, packetLen;
	unsigned char buffer[40];
	unsigned int ID, PASS;
	clock_t clTimer, svTimer;
	
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	clrscr(); 

	// Set virtual keyboard location
#if defined(__LYNX__)
	SetKeyboardOverlay(50,70);
#elif defined(__NES__)
	SetKeyboardOverlay(10,20);
#endif

	// Init Network
	if (InitNetwork() != NETWORK_OK) {
		gotoxy(0,0); cprintf("Network Error");
		while (!kbhit()); return EXIT_SUCCESS;		
	}

	// Host or Join?
	gotoxy(0,0); cprintf("Easynet Demo (8bit-Unity 0.5)");
	gotoxy(0,1); cprintf("(H)ost or (J)oin Session?");
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
		// Choose name first
		gotoxy(0,2); cprintf("Choose name: "); gets(myName);
	
		// Host new Session (ID:PASS generated automatically)
		res = EasyHost(EASY_UDP, NUM_SLOTS, &ID, &PASS); // You can use EASY_TCP or EASY_UDP
		switch (res) {
		case EASY_TIMEOUT:
			gotoxy(0,3); cprintf("Error: connection timed-out");			
			while (!kbhit()); return EXIT_SUCCESS;		
		}
		break;
		
	case KB_J:
		// Request credential (ID:PASS must match host)
		gotoxy(0,2); cprintf("Session ID: "); 
		gets(buffer); sscanf(buffer, "%i", &ID);  
		gotoxy(0,3); cprintf("Session PASS: "); 
		gets(buffer); sscanf(buffer, "%i", &PASS);  

		// Choose name first
		gotoxy(0,4); cprintf("Choose name: "); gets(myName);
		
		// Try joining session
		res = EasyJoin(EASY_UDP, &ID, &PASS);
		switch (res) {
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
		gotoxy(0,2); cprintf("Error: connection aborted");			
		while (!kbhit()); return EXIT_SUCCESS;		
	}
	
	//////////////////////////////
	// Show connection status
	sprintf(buffer, "Session ID:%u / PASS:%u / SLOT:%i/%i", ID, PASS, easySlot, NUM_SLOTS-1);
	clrscr(); gotoxy(0,0); cprintf(buffer);
	
	//////////////////////////////
	// Setup player data
	player = &players[easySlot];  // easySlot contains our own slot number
	player->x = clock()%24+8;	  // Pseudo-random position
	player->y = clock()%15+2;
	player->c = myName[0];
	
	//////////////////////////////
	// Run mini interactive game
	clTimer = clock();
	svTimer = clock();
	while (!kbhit() || cgetc() != KB_Q) {
		// Use timer to regulate player input
		if (clock() > clTimer + TCK_PER_SEC/FPS) {
			// Update timer
			clTimer = clock();

			// Eraze previous location
			player = &players[easySlot];
			gotoxy(player->x, player->y);
			putchar(' ');
			
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
			player->y = MIN(player->y, 15);
			
			// Show new location
			gotoxy(player->x, player->y);
			putchar(player->c);		
				
			// Broadcast new player state (3 bytes)
			EasySend(player, 3);
		}
		
		// Receive data from Easynet server (using time-out of 0)
		packetData = EasyRecv(0, &packetLen);
		if (packetLen) {
			// Update timer
			svTimer = clock();
			
			// Decode packet data: (slot,len,data),(slot,len,data),...
			i = 0;
			while (i<packetLen) {
				// Read data chunk
				slot = packetData[i++];
				len  = packetData[i++];
				if (slot != easySlot) {	// Note: our own data is also broadcasted back to us
					// Clear previous state
					player = &players[slot];
					if (player->y) {
						gotoxy(player->x, player->y);
						putchar(' ');
					}
					
					// Update player state
					memcpy(player, &packetData[i], len);
					gotoxy(player->x, player->y);
					putchar(player->c);						
				}
				i += len;			
			}		
		}
		
		// Watch-out for time issues....
		if (clock() > svTimer+10*TCK_PER_SEC) {
			gotoxy(0,TXT_ROWS-1);
			cprintf("Connection Timed-out...");
			break;
		}
		
		// Refresh screen (if needed)
	#if defined(__LYNX__) || defined(__NES__)
		UpdateDisplay();
	#endif
	}

	// Quit Easynet graciously
	EasyQuit();
	
	// Wait gracefully
	cgetc(); while (!kbhit());
	
	// Done
    return EXIT_SUCCESS;
}

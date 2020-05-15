
#include "network.h"

// Debugging flags
//#define DEBUG_FPS

// Platform specific colors
#if defined __APPLE2__
	#define INK_LAPS   	 BLACK
	#define INK_TAB		 GREEN
	#define INK_HIGHLT	 BLACK
	#define PAPER_HIGHLT YELLOW
	#define PAPER_SCORES BLACK
	#define ROW_SCORES   8
#elif defined __ATARI__
	#define INK_LAPS   	 RED
	#define INK_TAB		 GREEN
	#define INK_HIGHLT	 BLACK
	#define PAPER_HIGHLT YELLOW
	#define PAPER_SCORES BLACK
	#define ROW_SCORES   8
#elif defined __ORIC__
	#define INK_LAPS   	 RED
	#define INK_TAB		 GREEN
	#define INK_HIGHLT	 BLACK
	#define PAPER_HIGHLT YELLOW
	#define PAPER_SCORES BLACK
	#define ROW_SCORES   8
#elif defined __CBM__
	#define INK_LAPS   	 BLACK
	#define INK_TAB		 GREEN
	#define INK_HIGHLT	 BLACK
	#define PAPER_HIGHLT YELLOW
	#define PAPER_SCORES GREY
	#define ROW_SCORES   8
#elif defined __LYNX__
	#define INK_LAPS   	 BLACK
	#define INK_TAB		 YELLOW	
	#define INK_HIGHLT	 WHITE
	#define PAPER_HIGHLT BLACK
	#define PAPER_SCORES BLACK
	#define ROW_SCORES   4
#endif

// Platform specific panel location/size
#if defined __LYNX__
	#define MENU_CMIN 22
	#define MENU_CMAX 38
	#define MENU_RMIN  2
	#define MENU_RMAX 16
#else
	#define MENU_CMIN 22
	#define MENU_CMAX 38
	#define MENU_RMIN  4
	#define MENU_RMAX 19
#endif

// Other definitions
#define ROW_CHAT (CHR_ROWS-2)

// Paper for message Buffer
unsigned char paperBuffer;

// Performance Drawing
#ifdef DEBUG_FPS
clock_t fpsClock;
void DrawFPS(unsigned long  f)
{
    unsigned int fps;
	
	// Calculate stats
	fps = ( (f-60*(f/60)) * TCK_PER_SEC) / (clock() - fpsClock);

	// Output stats
	inkColor = WHITE;
	PrintNum(0, 0, fps);
	fpsClock = clock();	
}
#endif

// Print score after round ends
signed char rank[4];
signed int score[4];
void PrintScores()
{
	signed char i,j,f,s;
	unsigned char* string;
	
	// Play the background music
	StopSFX();
	PlayMusic(MUSICRAM);
	
	// Compute scores
	for (i=0; i<MAX_PLAYERS; ++i) {
		rank[i] = -1;
		if (controlIndex[i] > 0) {
			if (cars[i].lap < 0) { cars[i].lap = 0; }
			score[i] = cars[i].lap*16+cars[i].way;
		} else {
			score[i] = -1;
		}
	}
		
	// Compute ranks
	for (j=0; j<MAX_PLAYERS; ++j) {
		for (i=0; i<MAX_PLAYERS; ++i) {
			if (score[i] >= 0) {
				if (i != rank[0]) {
					if (i != rank[1]) {
						if (i != rank[2]) {
							if (i != rank[3]) {
								if (rank[j]<0 | score[i] >= score[rank[j]]) { 
									rank[j] = i; 
								}
							}
						}
					}
				}
			}
		}
	}
	
	// Create blank area
	DisableSprite(-1);
	paperColor = PAPER_SCORES;
	DrawPanel(13,ROW_SCORES,26,ROW_SCORES+8);
	
	// Print results and wait
	for (i=0; i<MAX_PLAYERS; ++i) {
		j = rank[i];
		s = ROW_SCORES+2*i+1;
		if (j >= 0) {
			inkColor = inkColors[j];
		#if defined __ORIC__
			SetInk(17, s);
		#endif			
			if (gameMode == MODE_ONLINE) {
				string = svUsers[j];
			} else {
				if (i == 0) { string = "WIN"; } else { string = "LOSE"; } 
			}
			PrintStr(18, s, string);
			inkColor = WHITE;
		#if defined __ORIC__
			SetInk(22, s);
		#endif			
			PrintChr(23, s, charHyphen);
			PrintNum(24, s, i+1);
			PrintChr(25, s, charHyphen);
		#if defined __ORIC__
			inkColor = AIC;
			SetInk(26, s);
		#endif			
			f = 0;
		#if defined __APPLE2__
			f += j*16;
			spriteX = 55;				// ((    15*8   )*140)/320;
			spriteY = 76+(i*384)/25;	// (((9+2*i)*8+3)*192)/200;
		#elif defined __ATARI__
			spriteX = 105; 
			spriteY = s*8+30;
		#elif defined __ORIC__
			spriteX = 30;	
			spriteY = s*8+3;
		#elif defined __CBM__	
			spriteX = 120; 
			spriteY = s*8+3;
		#elif defined __LYNX__
			spriteX = 60; 
			spriteY = s*6+3;			
		#endif	
			SetSprite(j, f);
			EnableSprite(j);
		}
	}
#if defined __LYNX__
	UpdateDisplay();
#endif		
	// Wait a few seconds
	paperColor = BLACK;
#if defined __APPLE2__
	PlayMusic(0);
#else
    sleep(7);
#endif
}

// Chat Row Management
#if defined __APPLE2__
static char chatBG[320];
#elif defined __ATARI__
static char chatBG[320];
#elif defined __ORIC__
static char chatBG[160];
#elif defined __CBM__
static char chatBG[180];
#elif defined __LYNX__
static char chatBG[240];
#endif

#ifdef __APPLE2__
  #pragma code-name("LOWCODE")
#endif

// Backup Chat Row
void BackupChatRow()
{
#ifdef __CBM__
	// Make backup of chat row
	DisableRom();
	memcpy(&chatBG[0],   (char*)(BITMAPRAM+320*ROW_CHAT), 160);
	EnableRom();
	memcpy(&chatBG[160], (char*)(SCREENRAM+40*ROW_CHAT), 20);
#elif defined __ATARI__
	unsigned char i;
	for (i=0; i<8; ++i) {
		memcpy(&chatBG[0]+i*20,   (char*)(BITMAPRAM1+320*ROW_CHAT+i*40), 20);
		memcpy(&chatBG[160]+i*20, (char*)(BITMAPRAM2+320*ROW_CHAT+i*40), 20);
	}	
#elif defined __ORIC__
	unsigned char i;
	for (i=0; i<8; ++i) {
		memcpy(&chatBG[0]+i*20, (char*)(BITMAPRAM+1+320*ROW_CHAT+i*40), 20);
	}
#elif defined __APPLE2__
	unsigned char i;	
	for (i=0; i<8; ++i) {
		SetDHRPointer(0, 8*ROW_CHAT+i);		
		*dhraux = 0;
		memcpy(&chatBG[0]+i*20,   (char*)(dhrptr), 20);
		*dhrmain = 0;
		memcpy(&chatBG[160]+i*20, (char*)(dhrptr), 20);
	}	
#elif defined __LYNX__
 	unsigned char i;
	for (i=0; i<6; ++i) {
		memcpy(&chatBG[0]+i*40, (char*)(BITMAPRAM+1+492*ROW_CHAT+i*82), 40);
	}
#endif
}

// Redraw Chat Row
void RedrawChatRow()
{
#ifdef __CBM__
	// Restore Row (to eraze chat)
	memcpy((char*)(BITMAPRAM+320*ROW_CHAT), &chatBG[0], 160);
	memcpy((char*)(SCREENRAM+40*ROW_CHAT), &chatBG[160], 20);
#elif defined __ATARI__
	unsigned char i;
	for (i=0; i<8; ++i) {
		memcpy((char*)(BITMAPRAM1+320*ROW_CHAT+i*40), &chatBG[0]+i*20, 20);
		memcpy((char*)(BITMAPRAM2+320*ROW_CHAT+i*40), &chatBG[0]+i*20+160, 20);
	}	
#elif defined __ORIC__
	unsigned char i;
	for (i=0; i<8; ++i) {
		memcpy((char*)(BITMAPRAM+1+320*ROW_CHAT+i*40), &chatBG[0]+i*20, 20);
	}	
#elif defined __APPLE2__
	unsigned char i;	
	for (i=0; i<8; ++i) {
		SetDHRPointer(0, 8*ROW_CHAT+i);		
		*dhraux = 0;
		memcpy((char*)(dhrptr),	&chatBG[0]+i*20, 20);
		*dhrmain = 0;
		memcpy((char*)(dhrptr), &chatBG[0]+i*20+160, 20);
	}		
#elif defined __LYNX__
 	unsigned char i;
	for (i=0; i<6; ++i) {
		memcpy((char*)(BITMAPRAM+1+492*ROW_CHAT+i*82), &chatBG[0]+i*40, 40);
	}	
#endif
}

// In-case connection drops out...
void PrintTimedOut()
{
	inkColor = WHITE;
    PrintStr(10,12, " CONNECTION TIMED-OUT ");
    sleep(3);
}

// Display lap numbers
void PrintLap(unsigned char i)
{
	if (cars[i].lap < 1) { return; }
	inkColor = inkColors[i];
	PrintNum((i+2)*8-3, CHR_ROWS-1, cars[i].lap);
}

// Print race message and laps
void PrintRace()
{	
	// Print race message
	paperColor = paperBuffer;
	PrintBuffer("      RACE STARTED, GOAL:    LAPS!      ");
	
	// Print laps
	inkColor = INK_LAPS;
	PrintNum(26, 0, lapGoal);
	
	// Reset to default colors
	inkColor = WHITE;
	paperColor = BLACK;
}

// Sub-function of GameMenu()
unsigned char serversLoaded;
void MenuServers()
{
	unsigned char j,k,n;
	char buffer[16];
	
	// Show action message
	PrintStr(MENU_CMIN+2, MENU_RMIN+6, "FETCH LIST...");				
	serversLoaded = 0;
	
	// Flush Net Queue
	while (RecvUDP(5)) {}	
	
	// Fetch server list
	udpBuffer[0] = CL_LIST;
	SendUDP(udpBuffer, 1);
	packet = RecvUDP(2*TCK_PER_SEC); // Allow some time-out
	
	// Check server response
	if (!packet) {
		// Timeout error
		PrintStr(MENU_CMIN+2, MENU_RMIN+7, "ERROR: TIMEOUT");
	} else if (PEEK(packet) != 1) {
		// Unexpected error
		PrintStr(MENU_CMIN+0, MENU_RMIN+7, "ERROR: CORRUPTION");
	} else {
		// Show server list				
		n = PEEK(++packet);
		j = 0;
		while (j<n) {
			// Separate at return carriage (char 10)
			k = 0;
		#ifndef __LYNX__	
			inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;			
			PrintChr(MENU_CMIN+0, (MENU_RMIN+2)+j, &charDigit[(j+1)*3]);
		#endif
			while (PEEK(++packet) != 10 && PEEK(packet) !=0) {
				buffer[k++] = PEEK(packet);
			}
			buffer[k] = 0;
			inkColor = WHITE; paperColor = BLACK;
			PrintStr(MENU_CMIN+2, (MENU_RMIN+2)+j, &buffer[0]);
			j++;
		}
		serversLoaded = 1;
	}
}

// Sub-function of GameMenu()
unsigned char MenuLogin(unsigned char serverIndex)
{
	unsigned char res;
#if defined __LYNX__ 
	SetKeyboardOverlay(13,70);
#endif	
	// Prompt for authentication
	PrintStr(MENU_CMIN+2, MENU_RMIN+2, "PLEASE LOGIN");
	PrintStr(MENU_CMIN+1, MENU_RMIN+4, "USER:");
	PrintStr(MENU_CMIN+1, MENU_RMIN+6, "PASS:");
	PrintStr(MENU_CMIN+2, MENU_RMIN+8, "REGISTER AT");
	PrintStr(MENU_CMIN+1, MENU_RMIN+9, "8BIT-SLICKS.COM");
	InputStr(MENU_CMIN+6, MENU_RMIN+4, &clUser[0], 4);
	PrintChr(MENU_CMIN+6+strlen(clUser), MENU_RMIN+4, &charBlank[0]);
	InputStr(MENU_CMIN+6, MENU_RMIN+6, &clPass[0], 10);	
	PrintChr(MENU_CMIN+6+strlen(clPass), MENU_RMIN+6, &charBlank[0]);

	// Show action message
	inkColor = YELLOW;
	PrintStr(MENU_CMIN+2, MENU_RMIN+12, "CONNECTING...");			
	res = ClientJoin(serverIndex);
	inkColor = WHITE;
	if (res == ERR_MESSAGE) {
		// Server error
		PrintStr(MENU_CMIN+1, MENU_RMIN+13, (char*)(packet+1));
		return 0;
	} else if (res == ERR_TIMEOUT) {
		// Timeout error
		PrintStr(MENU_CMIN+1, MENU_RMIN+13, "ERROR: TIMEOUT");					
		return 0;
	} else if (res == ERR_CORRUPT) {
		// Unexpected error
		PrintStr(MENU_CMIN+1, MENU_RMIN+13, "ERROR: CORRUPTION");
		return 0;
	} else {
		// All good
		PrintStr(MENU_CMIN+2, MENU_RMIN+13, "OK");
		gameMap = svMap;
		gameStep = svStep;
		return 1;
	}	
}

#ifdef __ATARIXL__
	#pragma code-name("SHADOW_RAM2")
#endif

// Sub-function for Animating Sprites in Main Menu
void SpriteAnimation(unsigned char index, unsigned char frame)
{
#if defined __APPLE2__
	frame += index*16;
	spriteX = 90+(index*96)/8;	
	spriteY = 16;			
#elif defined __ATARI__
	spriteX = 145+index*13; 
	spriteY = 40;
#elif defined __ORIC__
	spriteX = 48+index*8;
	spriteY = 16;	
#elif defined __CBM__
	spriteX = 200+index*26; 
	spriteY = 15;
#elif defined __LYNX__
	spriteX = 100+index*13; 
	spriteY = 6;
#endif		
	SetSprite(index, frame);
}

#if defined __LYNX__
unsigned char cursorJoy, cursorKey, cursorPressed;
unsigned char cursorFlick, cursorRow = MENU_RMIN+2;
clock_t cursorClock;

void LynxCursorFlicker()
{
	// Only do preiodically
	if (clock()-cursorClock < 20) { return; }
	cursorClock = clock();
	
	// Reset Column and show Cursor
	DrawPanel(MENU_CMIN, MENU_RMIN+2, MENU_CMIN+1, MENU_RMAX);
	if (cursorFlick) {
		inkColor = YELLOW;
		PrintChr(MENU_CMIN+0, cursorRow, &charHyphen[0]);
		PrintChr(MENU_CMIN+1, cursorRow, &charBracket[3]);
		inkColor = WHITE;
	}
	cursorFlick = !cursorFlick;
}

void LynxCursorControl()
{
	// Check if cursor was already pressed
	cursorKey = 0;
	cursorJoy = GetJoy(0);
	if (cursorJoy != 255) {
		if (cursorPressed) { return; }
		cursorPressed = 1;
		cursorFlick = 1;
		cursorClock = 0;
	} else { 
		cursorPressed = 0; 
	}
	
	// Process next event
	if (!(cursorJoy & JOY_LEFT)) { 
		     if (gameMode == MODE_INFO)   { cursorKey = KEY_O; cursorRow = MENU_RMIN+2; }
		else if (gameMode == MODE_ONLINE) { cursorKey = KEY_L; cursorRow = MENU_RMIN+2; }
	}	
	if (!(cursorJoy & JOY_RIGHT)) { 
		     if (gameMode == MODE_LOCAL)  { cursorKey = KEY_O; cursorRow = MENU_RMIN+2; }
		else if (gameMode == MODE_ONLINE) { cursorKey = KEY_I; cursorRow = MENU_RMIN+2; }
	}	
	if (!(cursorJoy & JOY_UP)) { 
		cursorRow -= 1; 
		if (gameMode == MODE_LOCAL) {
			     if (cursorRow  < MENU_RMIN+2)  { cursorRow = MENU_RMIN+2; }			
			else if (cursorRow == MENU_RMIN+6)  { cursorRow = MENU_RMIN+5; }			
			else if (cursorRow == MENU_RMIN+8)  { cursorRow = MENU_RMIN+7; }			
			else if (cursorRow == MENU_RMIN+10) { cursorRow = MENU_RMIN+9; }			
		} else {
				 if (cursorRow  < MENU_RMIN+2)  { cursorRow = MENU_RMIN+2; }		
		}
	}
	if (!(cursorJoy & JOY_DOWN)) { 
		cursorRow += 1; 
		if (gameMode == MODE_LOCAL) {
			     if (cursorRow  > MENU_RMIN+11) { cursorRow = MENU_RMIN+11; }
			else if (cursorRow == MENU_RMIN+10) { cursorRow = MENU_RMIN+11; }
			else if (cursorRow == MENU_RMIN+8)  { cursorRow = MENU_RMIN+9; }			
			else if (cursorRow == MENU_RMIN+6)  { cursorRow = MENU_RMIN+7; }			
		} else {
			     if (cursorRow  > MENU_RMIN+13) { cursorRow = MENU_RMIN+13; }
		}
	}
	if (!(cursorJoy & JOY_BTN1)) { 
		if (gameMode == MODE_LOCAL) {
			     if (cursorRow == MENU_RMIN+11) { cursorKey = KEY_SP; }
			else if (cursorRow == MENU_RMIN+9)  { cursorKey = KEY_L; }
			else if (cursorRow == MENU_RMIN+7)  { cursorKey = KEY_M; }
			else if (cursorRow >= MENU_RMIN+2)  { cursorKey = 49 + (cursorRow-(MENU_RMIN+2)); }
		} else {
			cursorKey = 49 + (cursorRow-(MENU_RMIN+2));
		}
	}
}
#endif

// Sub-function of GameMenu()
clock_t animClock;
unsigned char animFrame;
unsigned char MenuWait()
{
#if defined __APPLE2__
	// Just update music (it takes the entire CPU...)
	PlayMusic(0);
#else
	// Animate sprites while waiting for key input
	unsigned char i,f;
	while (!kbhit()) {
		// Check timer
		if (clock()-animClock > 2) {
			animClock = clock();
			animFrame = (animFrame+1) % 16;
			for (i=0; i<MAX_PLAYERS; i++) {
				// Animation showing counter rotating cars
				if (i%2) { f = (8-animFrame)%16; } else { f = animFrame; }	
				SpriteAnimation(i, f);
			}
		}	
	#if defined __LYNX__
		LynxCursorControl();
		if (cursorKey) { return cursorKey; }
		if (gameMode == MODE_LOCAL || (gameMode == MODE_ONLINE && serversLoaded)) { 
			LynxCursorFlicker(); 
		}			
		UpdateDisplay(); // Refresh Lynx screen
	#endif	
	}
#endif
	return cgetc();
}

// Sub-function of GameMenu()
void MenuPlayer(unsigned char i)
{	
	// Clear previous
	unsigned char row = MENU_RMIN+2+i;
	DrawPanel(MENU_CMIN+6, row, MENU_CMAX, row);
	
	// Print Characters
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	PrintNum(MENU_CMIN+3, row, i+1);
	inkColor = WHITE; paperColor = BLACK;
	PrintStr(MENU_CMIN+2, row, "P");
	PrintStr(MENU_CMIN+4, row, ":");
	PrintStr(MENU_CMIN+6, row, controlList[controlIndex[i]]);	
}

// Sub-function of GameMenu()
void MenuMap()
{
	// Clear previous
	DrawPanel(MENU_CMIN+7, MENU_RMIN+7, MENU_CMAX, MENU_RMIN+7);
	
	// Print Characters
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	PrintStr(MENU_CMIN+2, MENU_RMIN+7, "M");
	inkColor = WHITE; paperColor = BLACK;
	PrintStr(MENU_CMIN+3, MENU_RMIN+7, "AP:");
	PrintStr(MENU_CMIN+7, MENU_RMIN+7, mapList[gameMap]);	
}

// Sub-function of GameMenu()
void MenuLaps()
{
	// Clear previous
	PrintStr(MENU_CMIN+7, MENU_RMIN+9, "  ");
	
	// Print Characters
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	PrintStr(MENU_CMIN+2, MENU_RMIN+9, "L");
	inkColor = WHITE; paperColor = BLACK;
	PrintStr(MENU_CMIN+3, MENU_RMIN+9, "AP:");
	PrintNum(MENU_CMIN+7, MENU_RMIN+9, lapNumber[lapIndex]);
}

#if defined __ATARI__
// Sub-function of GameMenu()
void MenuGFX()
{
	// Clear previous
	PrintStr(MENU_CMIN+7, MENU_RMIN+11, "   ");
	
	// GFX mode selection
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	PrintStr(MENU_CMIN+2, MENU_RMIN+11, "G");
	inkColor = WHITE; paperColor = BLACK;
	if (PEEK(BLENDTOG) & 2) {
		PrintStr(MENU_CMIN+3, MENU_RMIN+11, "FX: OFF  ");				
	} else {
		PrintStr(MENU_CMIN+3, MENU_RMIN+11, "FX: BLEND");				
	}	
}
#endif

#if defined __APPLE2__
// Sub-function of GameMenu()
void MenuSFX()
{
	// Clear previous
	PrintStr(MENU_CMIN+7, MENU_RMIN+11, "   ");
	
	// GFX mode selection
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	PrintStr(MENU_CMIN+2, MENU_RMIN+11, "S");
	inkColor = WHITE; paperColor = BLACK;
	if (sfxOutput) {
		PrintStr(MENU_CMIN+3, MENU_RMIN+11, "FX: MOCKING");				
	} else {
		PrintStr(MENU_CMIN+3, MENU_RMIN+11, "FX: SPEAKER");
	}	
}
#endif

// Main menu function
void GameMenu()
{
	unsigned char i, f, lastchar;

	// Display cars on the top
	for (i=0; i<MAX_PLAYERS; i++) {
		// Counter rotating vehicles
		if (i%2) { f = 8; } else { f = 0; }
		SpriteAnimation(i,f);
		EnableSprite(i);
	}	
		
	// Show version, credits, and start music
	inkColor = WHITE; paperColor = BLACK;
	PrintStr(MENU_CMIN, CHR_ROWS-2, buildInfo);	
	
	// Show menu options
	while (1) {
		// Make Black Panel Area
		DrawPanel(MENU_CMIN, MENU_RMIN, MENU_CMAX, MENU_RMAX);
	
		// Display TAB keys
		inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
		PrintStr(MENU_CMIN+0,  MENU_RMIN, "L");
		PrintStr(MENU_CMIN+6,  MENU_RMIN, "O");
		PrintStr(MENU_CMIN+13, MENU_RMIN, "I");

		// Display LOCAL menu
		if (gameMode == MODE_LOCAL) {            
			// Display menu options
			inkColor = INK_TAB; paperColor = BLACK;
		#if defined __LYNX__			
			PrintStr(MENU_CMIN+0,  MENU_RMIN, "L");			
		#endif
			PrintStr(MENU_CMIN+1,  MENU_RMIN, "OCAL");
			inkColor = WHITE; 
			PrintStr(MENU_CMIN+7,  MENU_RMIN, "NLINE");
			PrintStr(MENU_CMIN+14, MENU_RMIN, "NFO");
			
			// Display game info
			for (i=0; i<MAX_PLAYERS; ++i) { MenuPlayer(i); }
			MenuMap(); 
			MenuLaps();

			// Platform specific menus
		#if defined __APPLE2__			
			MenuSFX();
		#elif defined __ATARI__			
			MenuGFX();
		#endif		

			// Race launcher
		#if defined __LYNX__
			PrintStr(MENU_CMIN+2, MENU_RMIN+11, "RACE!");				
		#else
			inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
			PrintStr(MENU_CMIN+2, MENU_RMIN+13, "SPACE");
			inkColor = WHITE; paperColor = BLACK;
			PrintStr(MENU_CMIN+7, MENU_RMIN+13, ": RACE!");				
		#endif		

			// Process user input
			while (1) { 
				// Get Character
				lastchar = MenuWait();
				
				// Switch Player 1-4
				i = lastchar - 49;
				if (i>=0 & i<4) {
					controlIndex[i]++;
					if (controlIndex[i] == NET_CONTROL) { controlIndex[i] = 0; }
					MenuPlayer(i);
				}
				// Switch Map
				if (lastchar == KEY_M) { 
					gameMap++;
					if (gameMap >= LEN_MAPS) { gameMap = 0; }
					MenuMap();
				}			
				// Switch Laps
				if (lastchar == KEY_L) { 
					lapIndex++;
					if (lapIndex >= LEN_LAPS) { lapIndex = 0; }
					MenuLaps();
				}
			#if defined __APPLE2__
				// Switch GFX Mode
				if (lastchar == KEY_S) {
					sfxOutput = !sfxOutput;
					if (sfxOutput) { InitMocking(); }
					MenuSFX();
				}
			#elif defined __ATARI__
				// Switch GFX Mode
				if (lastchar == KEY_G) { 
					POKE(BLENDTOG, PEEK(BLENDTOG) ^ 2);
					MenuGFX();
				}
			#endif				
				// Start game? / Switch screen?
				if (lastchar == KEY_SP) { return; }                
				if (lastchar == KEY_O) { gameMode = MODE_ONLINE; break; }
				if (lastchar == KEY_I) { gameMode = MODE_INFO; break; }                
			}
		} 
        
		// Display ONLINE menu
        else if (gameMode == MODE_ONLINE) {
			// Display menu options
			inkColor = INK_TAB; paperColor = BLACK;	
		#if defined __LYNX__			
			PrintStr(MENU_CMIN+6, MENU_RMIN, "O");
		#endif
			PrintStr(MENU_CMIN+7, MENU_RMIN, "NLINE");
			inkColor = WHITE;
			PrintStr(MENU_CMIN+1, MENU_RMIN, "OCAL");
			PrintStr(MENU_CMIN+14, MENU_RMIN, "NFO");

			// Is network ready?
			if (!networkReady) {
				// Init network
				PrintStr(MENU_CMIN+2, MENU_RMIN+2, "INIT NETWORK...");
				i = InitNetwork();
				if (i == ADAPTOR_ERR) {
					PrintStr(MENU_CMIN+4, MENU_RMIN+3, "ADAPTOR ERROR");
					
				} else if (i == DHCP_ERR) {
					PrintStr(MENU_CMIN+4, MENU_RMIN+3, "DHCP ERROR");
				
				} else {
					PrintStr(MENU_CMIN+4, MENU_RMIN+3, "ADAPTOR OK!");
					PrintStr(MENU_CMIN+4, MENU_RMIN+4, "DHCP OK!");

					// Setup UDP settings (svIP, svPort, clPort)
					InitUDP(199, 47, 196, 106, 5000+clock()%16, 5000+clock()%256);
					networkReady = 1;
				}
			}
			
			// Could initialize network?
			if (networkReady) { MenuServers(); }
			
			// Process user input
			while (1) { 
				// Get Character
				lastchar = MenuWait();
				networkReady = 1;
			
				// Try to join server?
				if (lastchar >= KEY_A) {
					i = lastchar - KEY_A + 9;
				} else {
					i = lastchar - 49;
				}
				if (networkReady & i>=0 & i<13) {
					// Black-out server list
					DrawPanel(MENU_CMIN, MENU_RMIN+2, MENU_CMAX, MENU_RMAX);
					if (MenuLogin(i)) {
						// Start game
						return;
					} else {
						// Redraw server list
						sleep(2);
						DrawPanel(22,6,38,19);
						MenuServers();
					}
				}				

				// Switch screen?
				if (lastchar == KEY_L) {
                    // Reset controls
                    controlIndex[0] = 3;
                    controlIndex[1] = 2;
                    controlIndex[2] = 0;
                    controlIndex[3] = 0;                    
                    gameMode = MODE_LOCAL; 
                    break; }
				if (lastchar == KEY_O) { break; }
                if (lastchar == KEY_I) { gameMode = MODE_INFO; break; }
			}
		}

        // Display CREDIT menu
        else {
			// Display menu options
			inkColor = INK_TAB; paperColor = BLACK;
		#if defined __LYNX__			
			PrintStr(MENU_CMIN+13, MENU_RMIN, "I");			
		#endif			
			PrintStr(MENU_CMIN+14, MENU_RMIN, "NFO");
			inkColor = WHITE; 
			PrintStr(MENU_CMIN+1,  MENU_RMIN, "OCAL");
			PrintStr(MENU_CMIN+7,  MENU_RMIN, "NLINE");
          
            PrintStr(MENU_CMIN+5, MENU_RMIN+2,  "CREDITS");
            PrintStr(MENU_CMIN+0, MENU_RMIN+4,  "CODE/GFX:");
            PrintStr(MENU_CMIN+1, MENU_RMIN+5,  "ANTHONY BEAUCAMP");
            PrintStr(MENU_CMIN+0, MENU_RMIN+7,  "MUSIC:");
            PrintStr(MENU_CMIN+1, MENU_RMIN+8,  "ANDREW FISHER");
            PrintStr(MENU_CMIN+0, MENU_RMIN+10, "ORIGINAL IDEA:");
            PrintStr(MENU_CMIN+1, MENU_RMIN+11, "TIMO KAUPPINEN");            
            
			// Process user input
			while (1) { 
				// Get Character
				lastchar = MenuWait();
            
				// Switch screen?
				if (lastchar == KEY_L) { gameMode = MODE_LOCAL; break; }
				if (lastchar == KEY_O) { gameMode = MODE_ONLINE; break; }
            }                
        }        
	}
}

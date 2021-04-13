
#include "definitions.h"

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

// Platform specific colors
#if defined __APPLE2__
	#define INK_LAPS   	 GREEN
	#define INK_TAB		 GREEN
	#define INK_HIGHLT	 BLACK
	#define PAPER_HIGHLT YELLOW
	#define PAPER_SCORES BLACK
	#define SCORES_ROW   8
#elif defined __ATARI__
	#define INK_LAPS   	 GREEN
	#define INK_TAB		 GREEN
	#define INK_HIGHLT	 BLACK
	#define PAPER_HIGHLT YELLOW
	#define PAPER_SCORES BLACK
	#define SCORES_ROW   8
#elif defined __ORIC__
	#define INK_LAPS   	 GREEN
	#define INK_TAB		 GREEN
	#define INK_HIGHLT	 BLACK
	#define PAPER_HIGHLT YELLOW
	#define PAPER_SCORES BLACK
	#define SCORES_ROW   8
#elif defined __CBM__
	#define INK_LAPS   	 BLACK
	#define INK_TAB		 GREEN
	#define INK_HIGHLT	 BLACK
	#define PAPER_HIGHLT YELLOW
	#define PAPER_SCORES GREY
	#define SCORES_ROW   8
#elif defined __LYNX__
	#define INK_LAPS   	 BLACK
	#define INK_TAB		 YELLOW	
	#define INK_HIGHLT	 WHITE
	#define PAPER_HIGHLT BLACK
	#define PAPER_SCORES GREY
	#define SCORES_ROW   	  4
	#define PAUSE_COL 		 15
	#define PAUSE_LOCAL_ROW   6
	#define PAUSE_ONLINE_ROW  4
#endif

// Platform specific panel location/size
#if defined __LYNX__
	#define MENU_COL 22
	#define MENU_ROW  2
	#define MENU_WID 17
	#define MENU_HEI 13
	#define MENU_BLD TXT_ROWS-1
#else
	#define MENU_COL 22
	#define MENU_ROW  4
	#define MENU_WID 17
	#define MENU_HEI 16
	#define MENU_BLD TXT_ROWS-2
#endif

// See slicks.c
extern const char* buildInfo, *mapList[]; 
extern unsigned char mapNum, lapNumber[], inkColors[];

// See game.c
extern unsigned char gameMode, gameMap, gameStep;
extern unsigned char lapIndex, lapGoal;

// See navigation.c
extern Vehicle cars[MAX_PLAYERS];

// See network.c
extern unsigned int packet;
extern unsigned char svMap, svStep; 
extern unsigned char svUsers[MAX_PLAYERS][5];
extern unsigned char clIndex, clUser[5], clPass[13];
extern char networkReady, chatBuffer[20], udpBuffer[28];

// See Unity/Lynx/Suzy.s
void __fastcall__ SuzyFlip(void);

// List of controller types
#if defined __LYNX__
	unsigned char controlIndex[MAX_PLAYERS] = { 4, 1, 1, 1 };
	unsigned char controlBackup[MAX_PLAYERS] = { 4, 1, 1, 1 };
#else
	unsigned char controlIndex[MAX_PLAYERS] = { 4, 1, 0, 0 };
	unsigned char controlBackup[MAX_PLAYERS] = { 4, 1, 0, 0 };
#endif
#if defined __APPLE2__
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU MEDIUM", "CPU HARD", "PADDLE 1", "PADDLE 2", "PADDLE 3", "PADDLE 4", "NETWORK" };
#elif defined __ATARI__
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU MEDIUM", "CPU HARD", "JOY 1", "JOY 2", "HUB 1", "HUB 2", "NETWORK" };
#elif defined __ORIC__
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU MEDIUM", "CPU HARD", "A,D,CTRL", "J,L,RET", "HUB/IJK 1", "HUB/IJK 2", "NETWORK" };
#elif defined __CBM__
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU MEDIUM", "CPU HARD", "JOY 1", "JOY 2", "JOY 3", "JOY 4", "NETWORK" };
#elif defined __LYNX__
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU MEDIUM", "CPU HARD", "JOY 1", "HUB 1", "HUB 2", "HUB 3", "NETWORK" };
#endif

// Performance Drawing
#ifdef DEBUG_FPS
clock_t fpsClock;
void DrawFPS(unsigned long f)
{
    unsigned int fps;
	
	// Calculate stats
	fps = ( (f-60u*(f/60u)) * TCK_PER_SEC) / (clock() - fpsClock);

	// Output stats
	inkColor = WHITE; 
	txtX = 0; txtY = 0;
	PrintNum(fps);
	fpsClock = clock();	
}
#endif

// Paper for message Buffer
#if defined(__CBM__) || defined(__LYNX__)
  unsigned char paperBuffer;
#endif 

// Flag for Server load status
unsigned char serversLoaded;

// Chat Row Management
#if defined __APPLE2__
#if defined __DHR__
static char chatBG[320];
#else
static char chatBG[160];
#endif
#elif defined __ATARI__
static char chatBG[320];
#elif defined __ORIC__
static char chatBG[160];
#elif defined __CBM__
static char chatBG[180];
#elif defined __LYNX__
static char chatBG[864];
#endif

void BackupRestoreChatRow(unsigned char mode)
{
	unsigned char i;
	unsigned char *buf = chatBG;
#if defined __APPLE2__
	unsigned char *line = 8*ROW_CHAT;
	for (i=0; i<8; ++i) {
		SetHiresPointer(0, line);		
		if (!mode) {
		  #if defined __DHR__
			*dhraux = 0;
			memcpy((char*)(buf+160), (char*)(hiresPtr), 20);
			*dhrmain = 0;
		  #endif
			memcpy((char*)(buf),     (char*)(hiresPtr), 20);
		} else {
		  #if defined __DHR__
			*dhraux = 0;
			memcpy((char*)(hiresPtr), (char*)(buf+160), 20);
			*dhrmain = 0;
		  #endif
			memcpy((char*)(hiresPtr), (char*)(buf),     20);
		}
		line++; buf += 20;
	}	
#elif defined __ATARI__
	unsigned char *bmp = BITMAPRAM1+320*ROW_CHAT;
	for (i=0; i<8; ++i) {
		if (!mode) {
			memcpy((char*)(buf),     (char*)(bmp), 		  20);
			memcpy((char*)(buf+160), (char*)(bmp+0x3000), 20);
		} else {
			memcpy((char*)(bmp), 		(char*)(buf),     20);
			memcpy((char*)(bmp+0x3000), (char*)(buf+160), 20);
		}
		bmp += 40; buf += 20;
	}	
#elif defined __CBM__
	unsigned char *bmp = BITMAPRAM+320*ROW_CHAT;
	unsigned char *scr = SCREENRAM+40*ROW_CHAT;
	if (!mode) {
		rom_disable();
		memcpy((char*)(buf),     (char*)(bmp), 160);
		rom_enable();
		memcpy((char*)(buf+160), (char*)(scr),  20);
	} else {
		memcpy((char*)(bmp), (char*)(buf),     160);
		memcpy((char*)(scr), (char*)(buf+160),  20);
	}
#elif defined __LYNX__
	unsigned char *bmp = BITMAPRAM+1+492*ROW_CHAT;
	for (i=0; i<6; ++i) {
		if (!mode) {
			memcpy((char*)(buf), (char*)(bmp), 40);
		} else {
			memcpy((char*)(bmp), (char*)(buf), 40);
		}
		bmp += 82; buf += 40;
	}
#elif defined __ORIC__
	unsigned char *bmp = BITMAPRAM+1+320*ROW_CHAT;
	for (i=0; i<8; ++i) {
		if (!mode) {
			memcpy((char*)(buf), (char*)(bmp), 20);
		} else {
			memcpy((char*)(bmp), (char*)(buf), 20);
		}
		bmp += 40; buf += 20;
	}
#endif
}

// Rolling buffer at the top of the screen, that moves text leftward when printing
void PrintBuffer(char *buffer)
{
	// Get length of new message
	unsigned char len;
	len = strlen(buffer);
	txtX = TXT_COLS-len;
	txtY = 0;
	
#if defined __ORIC__	
	// Need to insert ink changes...
	txtX--; len++;
	if (len<TXT_COLS) {
		CopyStr(0, 0, len, 0, TXT_COLS-len);
		SetAttributes(inkColor);
	}
	txtX++; len--;
	PrintStr(buffer);		
	
#elif defined __APPLE2__	
	// Make sure message has even length
	if (len&1) {
		buffer[len++] = ' ';
		buffer[len] = 0;
	}
	if (len<TXT_COLS)
		CopyStr(0, 0, len, 0, TXT_COLS-len);
	PrintStr(buffer);		
	
#else
	// Just shift buffer and print new message
	if (len<TXT_COLS)
		CopyStr(0, 0, len, 0, TXT_COLS-len);
	PrintStr(buffer);		
#endif
}

void InputField(char *buffer, unsigned char len)
{
	// Print initial condition
	InputStr(len, buffer, len, 0);

	// Run input loop
#if defined __LYNX__ 
	ShowKeyboardOverlay();
	while (1) {
		while (!KeyboardOverlayHit()) { UpdateDisplay(); } // Refresh Lynx screen
		if (InputStr(len, buffer, len, GetKeyboardOverlay())) {
			HideKeyboardOverlay();
			return; 
		}
	}
#else
	while (1) {
		while (!kbhit()) {}
		if (InputStr(len, buffer, len, cgetc())) { 
			return; 
		}
	}
#endif
}

#if defined __LYNX__
clock_t cursorClock;
unsigned char cursorJoy, cursorKey, cursorBut2, cursorPressed;
unsigned char cursorFlick, cursorCol = MENU_COL, cursorRow = MENU_ROW+2;
unsigned char cursorTop = MENU_ROW+2, cursorHeight = MENU_HEI-2;
unsigned char *pauseLabel[] = { "resume", "race!", "next!", "quit", "hello!", "bye!", "congrat", "thanks!" };
unsigned char pauseAction[] = { KB_PAUSE, KB_START, KB_NEXT, KB_QUIT, 4, 5, 6, 7 };
unsigned char gamePaused = 0;

void LynxCursorFlicker()
{
	// Only do preiodically
	if (clock()-cursorClock < 20) { return; }
	cursorClock = clock();
	
	// Reset Column and show Cursor
	txtX = cursorCol; txtY = cursorTop;
	PrintBlanks(2, cursorHeight);
	if (cursorFlick) {
		inkColor = YELLOW;
		txtY = cursorRow;
		PrintChr(&charHyphen[0]); txtX++;
		PrintChr(&charBracket[3]);
		inkColor = WHITE;
	}
	cursorFlick = !cursorFlick;
}

void LynxCursorControl()
{
	// Process screen flips
	if (kbhit()) {
		switch (cgetc()) {
		case KB_FLIP:
			SuzyFlip();
			break;
		case KB_MUSIC:
			if (!gamePaused)
				NextMusic(0);
			break;
		case KB_PAUSE:
			cursorKey = KB_PAUSE;
			return;
		}
	}
	
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
		if (!gamePaused) {
				 if (gameMode == MODE_INFO)   { cursorKey = KB_O; cursorRow = MENU_ROW+2; }
			else if (gameMode == MODE_ONLINE) { cursorKey = KB_L; cursorRow = MENU_ROW+2; }
		}
	}	
	if (!(cursorJoy & JOY_RIGHT)) { 
		if (!gamePaused) {
				 if (gameMode == MODE_LOCAL)  { cursorKey = KB_O; cursorRow = MENU_ROW+2; }
			else if (gameMode == MODE_ONLINE) { cursorKey = KB_I; cursorRow = MENU_ROW+2; }
		}
	}	
	if (!(cursorJoy & JOY_UP)) { 
		cursorRow -= 1; 
		if (gamePaused) {
			if (cursorRow < cursorTop)			   { cursorRow = cursorTop; }
		} else {		
			if (gameMode == MODE_LOCAL) {
					 if (cursorRow  < MENU_ROW+2)  { cursorRow = MENU_ROW+2; }			
				else if (cursorRow == MENU_ROW+6)  { cursorRow = MENU_ROW+5; }			
				else if (cursorRow == MENU_ROW+8)  { cursorRow = MENU_ROW+7; }			
				else if (cursorRow == MENU_ROW+10) { cursorRow = MENU_ROW+9; }			
			} else if (gameMode == MODE_ONLINE) {
					 if (cursorRow  < MENU_ROW+2)  { cursorRow = MENU_ROW+2; }
			}
		}
	}
	if (!(cursorJoy & JOY_DOWN)) { 
		cursorRow += 1; 
		if (gamePaused) {
			if (gameMode == MODE_LOCAL) {
			    if (cursorRow  > PAUSE_LOCAL_ROW+3)  { cursorRow = PAUSE_LOCAL_ROW+3; }
			} else {
				if (cursorRow  > PAUSE_ONLINE_ROW+7) { cursorRow = PAUSE_ONLINE_ROW+7; }
			}
		} else {
			if (gameMode == MODE_LOCAL) {
					 if (cursorRow  > MENU_ROW+11) { cursorRow = MENU_ROW+11; }
				else if (cursorRow == MENU_ROW+10) { cursorRow = MENU_ROW+11; }
				else if (cursorRow == MENU_ROW+8)  { cursorRow = MENU_ROW+9; }			
				else if (cursorRow == MENU_ROW+6)  { cursorRow = MENU_ROW+7; }			
			} else if (gameMode == MODE_ONLINE) {
					 if (cursorRow  > MENU_ROW+10) { cursorRow = MENU_ROW+10; }
			}
		}
	}
	if (!(cursorJoy & JOY_BTN1) || !(cursorJoy & JOY_BTN2)) { 
		cursorBut2 = 0;
		if (!(cursorJoy & JOY_BTN2))
			cursorBut2 = 1;
		if (gamePaused) {
				cursorKey = pauseAction[cursorRow-cursorTop];
		} else {
			if (gameMode == MODE_LOCAL) {
					 if (cursorRow == MENU_ROW+11) { cursorKey = KB_SP; }
				else if (cursorRow == MENU_ROW+9)  { cursorKey = KB_L; }
				else if (cursorRow == MENU_ROW+7)  { cursorKey = KB_M; }
				else if (cursorRow >= MENU_ROW+2)  { cursorKey = 49 - (MENU_ROW+2) + cursorRow; }
			}   else if (gameMode == MODE_ONLINE)  { cursorKey = 49 - (MENU_ROW+2) + cursorRow; }
		}
	}
}

void BackupRestorePauseBg(unsigned char mode)
{
	unsigned char i;
	unsigned int addr1 = chatBG;
	unsigned int addr2 = BITMAPRAM+1+2*PAUSE_COL+492*PAUSE_ONLINE_ROW;
	for (i=0; i<(8*6); ++i) {
		if (!mode)
			memcpy(addr1, addr2, 18);
		else
			memcpy(addr2, addr1, 18);
		addr1 += 18; addr2 += 82;
	}		
	
}

unsigned char pauseEvt;

unsigned char MenuPause()
{
	unsigned char i;
	
	// Set Cursor and Print options
	cursorCol = PAUSE_COL;
	if (gameMode == MODE_LOCAL) {
		cursorHeight = 4;	
		cursorRow = PAUSE_LOCAL_ROW;
		cursorTop = PAUSE_LOCAL_ROW;
	} else {
		cursorHeight = 8;	
		cursorRow = PAUSE_ONLINE_ROW;
		cursorTop = PAUSE_ONLINE_ROW;
	}		
	paperColor = PAPER_SCORES;
	txtX = PAUSE_COL; txtY = cursorRow;
	PrintBlanks(9, cursorHeight);
	inkColor = WHITE;
	txtX = PAUSE_COL+2; 
	for (i=0; i<cursorHeight; i++) {
		if (i>3) inkColor = inkColors[clIndex];
		PrintStr(pauseLabel[i]);
		txtY++;
	}
	inkColor = WHITE;
			
	while (!kbhit()) {
		// In online mode, check if a race/map/timeout event occured
		if (gameMode == MODE_ONLINE) {
			pauseEvt = NetworkUpdate();
			if (pauseEvt == EVENT_RACE || pauseEvt == EVENT_MAP || pauseEvt == ERR_TIMEOUT)
				return 0;
		}
		// Process Cursor
		LynxCursorControl();
		if (cursorKey) { 
			if (gameMode == MODE_ONLINE && cursorKey < 8) {
				// Process chat event then exit menu
				memcpy(chatBuffer, pauseLabel[cursorKey], 8);
				ClientEvent(EVENT_CHAT);
				return KB_PAUSE;
			} else {
				return cursorKey; 
			}
		}
		LynxCursorFlicker(); 
		UpdateDisplay();
	}	
}
#endif

#if defined __ATARIXL__
// Sub-function of GameMenu()
void MenuGFX()
{
	// GFX mode selection
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	txtX = MENU_COL+2; txtY = MENU_ROW+11;
	PrintStr("G");
	inkColor = WHITE; paperColor = BLACK;
	txtX = MENU_COL+3; 
	if (bmpToggle & 2) {
		PrintStr("FX: OFF  ");				
	} else {
		PrintStr("FX: BLEND");				
	}	
}
#endif

// Display lap numbers
void PrintLap(unsigned char i)
{
	if (cars[i].lap < 1) { return; }
	inkColor = inkColors[i]; 
	txtX = (i+2)*8-3; txtY = TXT_ROWS-1;
	PrintNum(cars[i].lap);
}

// Print race message and laps
void PrintRace()
{	
	// Print race message
#if defined(__CBM__) || defined(__LYNX__)
	paperColor = paperBuffer;
#else
	paperColor = BLACK; 
#endif	
	PrintBuffer("      RACE STARTED, GOAL:    LAPS!      ");
	
	// Print laps
	inkColor = INK_LAPS; 
	txtX = 26; txtY = 0; 
	PrintNum(lapGoal);
}

// Sub-function of GameMenu()
void MenuMap()
{
	// Clear previous
	txtX = MENU_COL+7; txtY = MENU_ROW+7;
	PrintBlanks(MENU_WID-7, 1);
	
	// Print Characters
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	txtX = MENU_COL+2; PrintChr(&charLetter[12*3]);	// 'M'
	inkColor = WHITE; paperColor = BLACK;
	txtX = MENU_COL+3; PrintStr("AP:");
	txtX = MENU_COL+7; PrintStr(mapList[gameMap]);	
}


// Sub-function of GameMenu()
void MenuLaps()
{
	// Clear previous
	txtX = MENU_COL+7; txtY = MENU_ROW+9;
	PrintStr("  ");
	
	// Print Characters
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	txtX = MENU_COL+2; PrintChr(&charLetter[11*3]);	// 'L'
	inkColor = WHITE; paperColor = BLACK;
	txtX = MENU_COL+3; PrintStr("AP:");
	txtX = MENU_COL+7; PrintNum(lapNumber[lapIndex]);
}

// In-case connection drops out...
void PrintTimedOut()
{
	inkColor = WHITE;
	txtX = 10; txtY = 12;
    PrintStr(" CONNECTION TIMED-OUT ");
    sleep(3);
}

// Sub-function for Animating Sprites in Main Menu
void SpriteAnimation(unsigned char index, unsigned char frame)
{
#if defined __APPLE2__
	frame += index*16;
	spriteX = 90+(index*96)/8u;	
	spriteY = 16;			
#elif defined __ATARI__
	spriteX = 145+index*13; 
	spriteY = 14;
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
#if defined __ATARI__
	SetMultiColorSprite(2*index, frame);
#else		
	SetSprite(index, frame);
#endif		
}

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

// Print score after round ends
signed int score[4];
void PrintScores()
{
	Vehicle *car;
	signed int s;
	unsigned int d;
	unsigned char i, j;
	unsigned int dist[4] = {0, 0, 0, 0};
	unsigned char rank[4] = {0, 1, 2, 3};
	unsigned char* string;
	
	// Play the background music
	StopSFX();
#if defined(__LYNX__)
	StopMusic();
	LoadMusic("speednik.mus");
#endif	
#ifndef __ORIC__
	PlayMusic();
#endif	
				
	// Compute scores and waypoint distances
	for (i=0; i<MAX_PLAYERS; ++i) {
		if (controlIndex[i] > 0) {
			car = &cars[i];
			if (car->lap < 0) { car->lap = 0; }
			score[i] = car->lap*18;
			if (car->way) {
				score[i] += car->way;
			} else {
				score[i] += 16;
			}
			dist[i] = GetWaypointDistance(car);
		#if defined(__ATARI__) || defined(__CBM__)
			if (car->x2 > 15*64-32 && car->x2 < 25*64+32 && car->y2 > 8*64-32 && car->y2 < 17*64+32) {
			#if defined(__ATARI__)			
				DisableMultiColorSprite(2*i);	// Car sprite
			#else
				DisableSprite(i);				// Car sprite
			#endif
				DisableSprite(SPR2_SLOT+i);		// Jump sprite			
			}
		#elif defined(__LYNX__)
			if (car->x2 > 15*64-32 && car->x2 < 25*64+32 && car->y2 > 4*96-48 && car->y2 < 13*96+48) {
				DisableSprite(i);			// Car sprite
				DisableSprite(SPR2_SLOT+i);	// Jump sprite			
			} else {
				spriteX = car->x2/16u; 
				spriteY = car->y2/16u;				
				j = (car->ang2+12)/23u;
				if (j>15) { j=0; }
				SetSprite(i, j);
			}
		#endif	
		} else {
			score[i] = -1;
		}
	}
		
	// Compute ranks (in case of same scores, check waypoint distance)
	for (i=0; i<MAX_PLAYERS; ++i) { 
		for (j=i+1; j<MAX_PLAYERS; ++j) { 
			if ( (score[i] < score[j]) || ((score[i] == score[j]) && (dist[i] > dist[j])) ) {
				d = rank[i];  rank[i]  = rank[j];  rank[j] = d;
				d = dist[i];  dist[i]  = dist[j];  dist[j] = d;
				s = score[i]; score[i] = score[j]; score[j] = s;
            }
        }				
	}
	
	// Create blank area
	paperColor = PAPER_SCORES;
	txtX = 15; txtY = SCORES_ROW;
	PrintBlanks(10, 9);
	
	// Print results and wait
	for (i=0; i<MAX_PLAYERS; ++i) {
		if (score[i] >= 0) {
			j = rank[i];
			inkColor = inkColors[j];
			txtY = SCORES_ROW+2*i+1;
		#if defined __ORIC__
			txtX = 15; SetAttributes(inkColor);
		#endif			
			if (gameMode == MODE_ONLINE) {
				string = svUsers[j];
			} else {
				if (i == 0) { string = "WIN"; } else { string = "LOSE"; } 
			}
			txtX = 16; PrintStr(string);
			inkColor = WHITE;
		#if defined __ORIC__
			txtX = 20; SetAttributes(inkColor);
		#endif		
			txtX = 21; PrintStr("- -");
			txtX = 22; PrintNum(i+1);
		#if defined __ORIC__
			txtX = 24; SetAttributes(AIC);
		#endif			
		}
	}
#if defined __LYNX__
	UpdateDisplay();
#endif		
	// Wait a few seconds
	paperColor = BLACK;
#if defined __APPLE2__
	UpdateMusic();
#else
    sleep(9);
#endif
	// Stop music if needed
#ifndef __ORIC__
	StopMusic();
#endif	
}

// Sub-function of GameMenu()
clock_t animClock;
unsigned char animFrame;
unsigned char MenuWait()
{
#if defined __APPLE2__
	// Just update music (it takes the entire CPU...)
	UpdateMusic();
#else
	// Animate sprites while waiting for key input
	unsigned char i,f;
#if defined __LYNX__
	while (1) {
#else
	while (!kbhit()) {
#endif
		// Check timer
		if (clock()-animClock > 2) {
			animClock = clock();
			animFrame = (animFrame+1)&15;
			for (i=0; i<MAX_PLAYERS; i++) {
				// Animation showing counter rotating cars
				if (i&1) { f = (24-animFrame)&15; } else { f = animFrame; }	
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
void MenuServers()
{
	clock_t timeout;
	unsigned char j,k,n;
	char buffer[16];
	
	// Show action message
	txtX = MENU_COL+2; txtY = MENU_ROW+6;
	PrintStr("FETCH LIST...");				
	serversLoaded = 0;

#ifdef NETCODE
	// Flush Net Queue
	while (RecvUDP(5));
			
	// Fetch server list
	udpBuffer[0] = CL_LIST;
	ServerConnect();
	SendUDP(udpBuffer, 1);
	timeout = clock()+2*TCK_PER_SEC;
	while (!packet || PEEK(packet) != 1) {
		packet = RecvUDP(0); // Allow some time-out
		if (clock() > timeout) break;
	}
	ServerDisconnect();
#endif

	// Check server response
	txtY = MENU_ROW+7;
	if (!packet) {
		// Timeout error
		txtX = MENU_COL+2; PrintStr("ERROR: TIMEOUT");
	} else if (PEEK(packet) != 1) {
		// Unexpected error
		txtX = MENU_COL+0; PrintStr("ERROR: CORRUPTION");
	} else {
		// Show server list				
		n = PEEK(++packet);
		n = MIN(n,12);
		j = 0;
		while (j<n) {
			// Separate at return carriage (char 10)
			k = 0;
			txtY = (MENU_ROW+2)+j;
		#ifndef __LYNX__	
			inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;			
			txtX = MENU_COL+0; PrintChr(&charDigit[(j+1)*3]);
		#endif
			while (PEEK(++packet) != 10 && PEEK(packet) !=0) {
				buffer[k++] = PEEK(packet);
			}
			k = MIN(k,15);
			buffer[k] = 0;
			inkColor = WHITE; paperColor = BLACK;
			txtX = MENU_COL+2; PrintStr(&buffer[0]);
			j++;
		}
		serversLoaded = 1;
	}
}

unsigned char  loginCol[] = { MENU_COL+2, MENU_COL+1, MENU_COL+1, MENU_COL+2, MENU_COL+1 };
unsigned char  loginRow[] = { MENU_ROW+2, MENU_ROW+4, MENU_ROW+6, MENU_ROW+8, MENU_ROW+9 };
unsigned char *loginTxt[] = { "PLEASE LOGIN", "USER:", "PASS:", "REGISTER AT", "8BIT-SLICKS.COM" };

// Sub-function of GameMenu()
unsigned char MenuLogin(unsigned char serverIndex)
{
	unsigned char res = 0;
	
	// Clear panel
	txtX = MENU_COL; txtY = MENU_ROW+2;
	PrintBlanks(MENU_WID, MENU_HEI-2);
	
#if defined __LYNX__ 
	// Load user/pass from EEPROM and set Softkeyboard position
	while (res < 18) {
		clUser[res] = lynx_eeprom_read(res);
		res++;
	}
	SetKeyboardOverlay(11,60);
#endif	
	// Prompt for authentication
	for (res=0; res<5; res++) {
		txtX = loginCol[res];
		txtY = loginRow[res];
		PrintStr(loginTxt[res]);
	}
	txtY = MENU_ROW+4;
	txtX = MENU_COL+6;      InputField(clUser, 4);
	txtX += strlen(clUser); PrintChr(charBlank);
	maskInput = 1;
	txtY = MENU_ROW+6;
	txtX = MENU_COL+6;      InputField(clPass, 10);	
	txtX += strlen(clPass); PrintChr(charBlank);
	maskInput = 0;
#if defined __LYNX__ 
	// Save user/pass to EEPROM
	res = 0;
	while (res < 18) {	
		lynx_eeprom_write(res, clUser[res]);
		res++;
	}
#endif	
	// Show action message
	inkColor = YELLOW;
	txtX = MENU_COL+2; txtY = MENU_ROW+11;
	PrintStr("CONNECTING...");
	ServerConnect();	
	res = ClientJoin(serverIndex);	
	inkColor = WHITE;
	txtX = MENU_COL+1; txtY = MENU_ROW+12;
	if (res == ERR_MESSAGE) {
		// Server error
		PrintStr((char*)(packet+1));
	} else if (res == ERR_TIMEOUT) {
		// Timeout error
		PrintStr("ERROR: TIMEOUT");					
	} else if (res == ERR_CORRUPT) {
		// Unexpected error
		PrintStr("ERROR: CORRUPTION");
	} else {
		// All good			
		PrintStr("JOINING SERVER");
		gameMap = svMap; 
		gameStep = svStep;
		return 1;
	}	
	ServerDisconnect();
	return 0;
}

// Sub-function of GameMenu()
void MenuPlayer(unsigned char i)
{	
	// Clear previous
	txtY = MENU_ROW+2+i; txtX = MENU_COL+6; 
	PrintBlanks(MENU_WID-6, 1);
	
	// Print Characters
	inkColor = inkColors[i]; paperColor = BLACK;
	txtX = MENU_COL+2; PrintChr(&charLetter[15*3]);	// 'P'
#ifndef __LYNX__
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
#endif
	txtX = MENU_COL+3; PrintNum(i+1);				//  i
	inkColor = WHITE; paperColor = BLACK;
	txtX = MENU_COL+4; PrintChr(charColon);			// ':'
	txtX = MENU_COL+6; PrintStr(controlList[controlIndex[i]]);	
}

// Sub-function of GameMenu()
void MenuConnect()
{
	// Init network
	unsigned char state = 1;
	unsigned char *report;
	txtX = MENU_COL+2; txtY = MENU_ROW+2;
#ifdef NETCODE
	PrintStr("NETWORK INIT");
	state = InitNetwork();
	if (state == ADAPTOR_ERR) {
		report = "ADAPTOR ERROR";		
	} else if (state == DHCP_ERR) {
		report = "DHCP ERROR";		
	} else {
		report = "DHCP OK!";
		networkReady = 1;
	}
	txtX = MENU_COL+4; txtY++; 
	PrintStr(report);
#else
	PrintStr("NOT SUPPORTED");	
#endif
}

// Display menu options
void MenuTab(unsigned char tab)
{
	txtY = MENU_ROW;
#if defined __LYNX__
	// Highlight currently selected tab
	inkColor = WHITE; paperColor = BLACK;	
	if (tab == 0) { inkColor = INK_TAB; };
	txtX = MENU_COL+0;  PrintStr("LOCAL");
	inkColor = WHITE;	
	if (tab == 1) { inkColor = INK_TAB; }
	txtX = MENU_COL+6;  PrintStr("ONLINE");
	inkColor = WHITE;	
	if (tab == 2) { inkColor = INK_TAB; }
	txtX = MENU_COL+13; PrintStr("INFO");
	inkColor = WHITE; 
#elif defined __ORIC__
	// Highlight first letter of each tab
	inkColor = BLACK; paperColor = AIC;
	txtX = MENU_COL+0;  PrintStr("L");			
	txtX = MENU_COL+6;  PrintStr("O");
	txtX = MENU_COL+13; PrintStr("I");
	inkColor = AIC; paperColor = BLACK;
	txtX = MENU_COL+1;  PrintStr("OCAL");
	txtX = MENU_COL+7;  PrintStr("NLINE");
	txtX = MENU_COL+14; PrintStr("NFO");
#else
	// Highlight first letter of each tab as well as current selection
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	txtX = MENU_COL+0;  PrintStr("L");			
	txtX = MENU_COL+6;  PrintStr("O");
	txtX = MENU_COL+13; PrintStr("I");
	inkColor = WHITE; paperColor = BLACK;
	if (tab == 0) { inkColor = INK_TAB; };
	txtX = MENU_COL+1;  PrintStr("OCAL");
	inkColor = WHITE;
	if (tab == 1) { inkColor = INK_TAB; }
	txtX = MENU_COL+7;  PrintStr("NLINE");
	inkColor = WHITE;	
	if (tab == 2) { inkColor = INK_TAB; }
	txtX = MENU_COL+14; PrintStr("NFO");
	inkColor = WHITE; 
#endif
}

#if defined __LYNX__
  #define CREDIT_ROWS 8
  unsigned char  creditCol[] = { MENU_COL+2, MENU_COL+0, MENU_COL+1, MENU_COL+0, MENU_COL+1, MENU_COL+1, MENU_COL+0, MENU_COL+1 };
  unsigned char  creditRow[] = { MENU_ROW+2, MENU_ROW+4, MENU_ROW+5, MENU_ROW+7, MENU_ROW+8, MENU_ROW+9, MENU_ROW+11, MENU_ROW+12 };
  unsigned char *creditTxt[] = { "2021 SONGBIRD", "CODE/GFX:", "ANTHONY BEAUCAMP", "MUSIC:", "ANDREW FISHER", "CARL FORHAN", "ORIGINAL IDEA:", "TIMO KAUPPINEN" };
#else
  #define CREDIT_ROWS 7
  unsigned char  creditCol[] = { MENU_COL+5, MENU_COL+0, MENU_COL+1, MENU_COL+0, MENU_COL+1, MENU_COL+0, MENU_COL+1 };
  unsigned char  creditRow[] = { MENU_ROW+2, MENU_ROW+4, MENU_ROW+5, MENU_ROW+7, MENU_ROW+8, MENU_ROW+10, MENU_ROW+11 };
  unsigned char *creditTxt[] = { "CREDITS", "CODE/GFX:", "ANTHONY BEAUCAMP", "MUSIC:", "ANDREW FISHER", "ORIGINAL IDEA:", "TIMO KAUPPINEN" };	
#endif

// Main menu function
void GameMenu()
{
	unsigned char i, f, lastchar;

	// Display cars on the top
	for (i=0; i<MAX_PLAYERS; i++) {
		// Counter rotating vehicles
		if (i&1) { f = 8; } else { f = 0; }
		SpriteAnimation(i,f);
	#if defined __ATARI__
		EnableMultiColorSprite(2*i);
	#else	
		EnableSprite(i);
	#endif
	}	
		
	// Show version, credits, and start music
	inkColor = WHITE; paperColor = BLACK;
	txtX = MENU_COL; txtY = MENU_BLD;
	PrintStr(buildInfo);

#if defined __LYNX__
	// Reset cursor state
	cursorCol = MENU_COL;
	cursorRow = MENU_ROW+2;
	cursorTop = MENU_ROW+2;
	cursorHeight = MENU_HEI-2;
#endif
	
	// Show menu options
	while (1) {
		// Make Black Panel Area
		txtX = MENU_COL; txtY = MENU_ROW;
		PrintBlanks(MENU_WID, MENU_HEI);
	
		if (gameMode == MODE_LOCAL) {    
			// Display LOCAL menu
			MenuTab(0);
			
			// Restore controls
			for (i=0; i<4; i++)
				controlIndex[i] = controlBackup[i];
								
			// Display game info
			for (i=0; i<MAX_PLAYERS; ++i) { MenuPlayer(i); }
			MenuMap(); 
			MenuLaps();

			// Platform specific menus
		#if defined __ATARI__			
			MenuGFX();
		#endif		

			// Race launcher
		#if defined __LYNX__
			txtX = MENU_COL+2; txtY = MENU_ROW+11;
			PrintStr("RACE!");				
		#else
			inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
			txtY = MENU_ROW+13;
			txtX = MENU_COL+2; PrintStr("SPACE");
			inkColor = WHITE; paperColor = BLACK;
			txtX = MENU_COL+7; PrintStr(": RACE!");				
		#endif		

			// Process user input
			while (1) { 
				// Get Character
				lastchar = MenuWait();
				
				// Switch Player 1-4
				i = lastchar - 49;
				if (i>=0 & i<4) {
				#if defined __LYNX__
					if (cursorBut2) {
						controlIndex[i]--; if (controlIndex[i] >= NET_CONTROL) controlIndex[i] = NET_CONTROL-1;
					} else {
						controlIndex[i]++; if (controlIndex[i] >= NET_CONTROL) controlIndex[i] = 0;
					}
				#else
					controlIndex[i]++; if (controlIndex[i] >= NET_CONTROL) controlIndex[i] = 0;
				#endif				
					MenuPlayer(i);
				}
				// Switch Map
				if (lastchar == KB_M) { 
				#if defined __LYNX__
					if (cursorBut2) {
						gameMap--; if (gameMap >= mapNum) gameMap = mapNum-1;
					} else {
						gameMap++; if (gameMap >= mapNum) gameMap = 0;
					}
				#else
					gameMap++; if (gameMap >= mapNum) gameMap = 0;
				#endif				
					MenuMap();
				}			
				// Switch Laps
				if (lastchar == KB_L) { 
				#if defined __LYNX__
					if (cursorBut2) {
						lapIndex--; if (lapIndex >= LEN_LAPS) lapIndex = LEN_LAPS-1;
					} else {
						lapIndex++; if (lapIndex >= LEN_LAPS) lapIndex = 0;
					}
				#else
					lapIndex++; if (lapIndex >= LEN_LAPS) lapIndex = 0;
				#endif				
					MenuLaps();
				}
			#if defined __ATARI__
				// Switch GFX Mode
				if (lastchar == KB_G) { 
					bmpToggle ^= 2;
					MenuGFX();
				}
			#endif				
			
				// Backup controls
				for (i=0; i<4; i++)
					controlBackup[i] = controlIndex[i];
					
				// Start game? / Switch screen?
				if (lastchar == KB_SP) { return; }                
				if (lastchar == KB_O)  { gameMode = MODE_ONLINE; break; }
				if (lastchar == KB_I)  { gameMode = MODE_INFO; break; }                
			}
		} 
        
        else if (gameMode == MODE_ONLINE) {
			// Display ONLINE menu
			MenuTab(1);
			
		#if defined(__ORIC__) || defined(__FUJINET__)
			// FujiNet and VIA are not happy with music...
			StopMusic();
		#endif

			// Is network ready?
			if (!networkReady)
				MenuConnect();
			
			// Could initialize network?
			if (networkReady) 
				MenuServers();
			
			// Process user input
			while (1) { 
				// Get Character
				lastchar = MenuWait();
			
				// Try to join server?
				if (lastchar >= KB_A) {
					i = lastchar - KB_A + 9;
				} else {
					i = lastchar - 49;
				}
				if (networkReady & i>=0 & i<13) {
					// Try to Login...
					if (MenuLogin(i)) {
						// Start game
						return;
					} else {
						// Redraw server list
						sleep(2);
						PrintBlanks(MENU_WID, MENU_HEI-2);
						MenuServers();
					}
				}				

				// Switch screen?
				if (lastchar == KB_L) { gameMode = MODE_LOCAL; break; }
				if (lastchar == KB_O) { break; }
                if (lastchar == KB_I) { gameMode = MODE_INFO; break; }
			}
		}

        else {
			// Display CREDITS
			MenuTab(2);
			for (i=0; i<CREDIT_ROWS; i++) {
				txtX = creditCol[i];
				txtY = creditRow[i];
				PrintStr(creditTxt[i]);
			}

			// Process user input
			while (1) { 
				// Get Character
				lastchar = MenuWait();
            
				// Switch screen?
				if (lastchar == KB_L) { gameMode = MODE_LOCAL; break; }
				if (lastchar == KB_O) { gameMode = MODE_ONLINE; break; }
            }       
        }        
	}
}

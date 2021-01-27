
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
	#define MENU_BLD CHR_ROWS-1
#else
	#define MENU_COL 22
	#define MENU_ROW  4
	#define MENU_WID 17
	#define MENU_HEI 16
	#define MENU_BLD CHR_ROWS-2
#endif

// See slicks.c
extern unsigned char inkColors[];
extern const char *mapList[LEN_MAPS];
extern unsigned char lapNumber[LEN_LAPS];

// See game.c
extern unsigned char gameMode, gameMap, gameStep;
extern unsigned char lapIndex, lapGoal;

// See navigation.c
extern Vehicle cars[MAX_PLAYERS];

// See network.c
extern unsigned int packet;
extern unsigned char clUser[5];
extern unsigned char clPass[13];
extern unsigned char svUsers[MAX_PLAYERS][5];
extern unsigned char svMap, svStep; 
extern char chatBuffer[20];
extern char udpBuffer[28];
extern char networkReady;

// See Unity/Lynx/Suzy.s
void __fastcall__ SuzyFlip(void);

// Build Information
const char* buildInfo = "BUILD: 2021/01/26";

// List of controller types
#if defined __LYNX__
	unsigned char controlIndex[MAX_PLAYERS] = { 4, 1, 1, 1 };
	unsigned char controlBackup[MAX_PLAYERS] = { 4, 1, 1, 1 };
#else
	unsigned char controlIndex[MAX_PLAYERS] = { 4, 1, 0, 0 };
	unsigned char controlBackup[MAX_PLAYERS] = { 4, 1, 0, 0 };
#endif
#if defined __APPLE2__
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU MED.", "CPU HARD", "PADDLE 1", "PADDLE 2", "PADDLE 3", "PADDLE 4", "NETWORK" };
#elif defined __ATARI__
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU MED.", "CPU HARD", "JOY 1", "JOY 2", "HUB 1", "HUB 2", "NETWORK" };
#elif defined __ORIC__
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU MED.", "CPU HARD", "A,D,CTRL", "J,L,RET", "HUB/IJK 1", "HUB/IJK 2", "NETWORK" };
#elif defined __CBM__
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU MED.", "CPU HARD", "JOY 1", "JOY 2", "JOY 3", "JOY 4", "NETWORK" };
#elif defined __LYNX__
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU MED.", "CPU HARD", "JOY 1", "HUB 1", "HUB 2", "NETWORK" };
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
	PrintNum(0, 0, fps);
	fpsClock = clock();	
}
#endif

#if defined __ATARIXL__
// Sub-function of GameMenu()
void MenuGFX()
{
	// Clear previous
	PrintStr(MENU_COL+7, MENU_ROW+11, "   ");
	
	// GFX mode selection
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	PrintStr(MENU_COL+2, MENU_ROW+11, "G");
	inkColor = WHITE; paperColor = BLACK;
	if (frameBlending & 2) {
		PrintStr(MENU_COL+3, MENU_ROW+11, "FX: OFF  ");				
	} else {
		PrintStr(MENU_COL+3, MENU_ROW+11, "FX: BLEND");				
	}	
}
#endif

// Display lap numbers
void PrintLap(unsigned char i)
{
	if (cars[i].lap < 1) { return; }
	inkColor = inkColors[i];
	PrintNum((i+2)*8-3, CHR_ROWS-1, cars[i].lap);
}

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

// Backup Chat Row
void BackupChatRow()
{
#ifdef __CBM__
	// Make backup of chat row
	rom_disable();
	memcpy(&chatBG[0],   (char*)(BITMAPRAM+320*ROW_CHAT), 160);
	rom_enable();
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
		SetHiresPointer(0, 8*ROW_CHAT+i);		
	  #if defined __DHR__
		*dhraux = 0;
		memcpy(&chatBG[160]+i*20, (char*)(hiresPtr), 20);
		*dhrmain = 0;
	  #endif
		memcpy(&chatBG[0]+i*20,   (char*)(hiresPtr), 20);
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
		SetHiresPointer(0, 8*ROW_CHAT+i);		
	  #if defined __DHR__
		*dhraux = 0;
		memcpy((char*)(hiresPtr), &chatBG[0]+i*20+160, 20);
		*dhrmain = 0;
	  #endif	
		memcpy((char*)(hiresPtr), &chatBG[0]+i*20, 20);
	}		
#elif defined __LYNX__
 	unsigned char i;
	for (i=0; i<6; ++i) {
		memcpy((char*)(BITMAPRAM+1+492*ROW_CHAT+i*82), &chatBG[0]+i*40, 40);
	}	
#endif
}

// Rolling buffer at the top of the screen, that moves text leftward when printing
void PrintBuffer(char *buffer)
{
	// Get length of new message
	unsigned char len;
	len = strlen(buffer);
	
#if defined __ORIC__	
	// Need to insert ink changes...
	len++;
	if (len<CHR_COLS) {
		CopyStr(0, 0, len, 0, CHR_COLS-len);
		SetAttributes(CHR_COLS-len, 0, inkColor);
	}
	len--;
	PrintStr(CHR_COLS-len, 0, buffer);		
	
#elif defined __APPLE2__	
	// Make sure message has even length
	if (len%2) {
		buffer[len++] = ' ';
		buffer[len] = 0;
	}
	if (len<CHR_COLS)
		CopyStr(0, 0, len, 0, CHR_COLS-len);
	PrintStr(CHR_COLS-len, 0, buffer);		
	
#else
	// Just shift buffer and print new message
	if (len<CHR_COLS)
		CopyStr(0, 0, len, 0, CHR_COLS-len);
	PrintStr(CHR_COLS-len, 0, buffer);		
#endif
}

void InputField(unsigned char col, unsigned char row, char *buffer, unsigned char len)
{
	// Print initial condition
	InputStr(col, row, len, buffer, len, 0);

	// Run input loop
#if defined __LYNX__ 
	ShowKeyboardOverlay();
	while (1) {
		while (!KeyboardOverlayHit()) { UpdateDisplay(); } // Refresh Lynx screen
		if (InputStr(col, row, len, buffer, len, GetKeyboardOverlay())) {
			HideKeyboardOverlay();
			return; 
		}
	}
#else
	while (1) {
		while (!kbhit()) {}
		if (InputStr(col, row, len, buffer, len, cgetc())) { 
			return; 
		}
	}
#endif
}

#if defined __LYNX__
unsigned char gamePaused = 0;
unsigned char cursorJoy, cursorKey, cursorBut2, cursorPressed;
unsigned char cursorFlick, cursorCol = MENU_COL, cursorRow = MENU_ROW+2;
unsigned char cursorTop = MENU_ROW+2, cursorHeight = MENU_HEI-2;
clock_t cursorClock;

void LynxCursorFlicker()
{
	// Only do preiodically
	if (clock()-cursorClock < 20) { return; }
	cursorClock = clock();
	
	// Reset Column and show Cursor
	PrintBlanks(cursorCol, cursorTop, 2, cursorHeight);
	if (cursorFlick) {
		inkColor = YELLOW;
		PrintChr(cursorCol, cursorRow, &charHyphen[0]);
		PrintChr(cursorCol+1, cursorRow, &charBracket[3]);
		inkColor = WHITE;
	}
	cursorFlick = !cursorFlick;
}

unsigned char pauseLocal[] =  { KB_PAUSE, KB_NEXT, KB_QUIT };
unsigned char pauseOnline[] = { KB_PAUSE, KB_START, KB_NEXT, 3, 4, 5, 6, KB_QUIT };

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
			if (gameMode == MODE_LOCAL) {
					 if (cursorRow  < PAUSE_LOCAL_ROW)  { cursorRow = PAUSE_LOCAL_ROW; }
			} else {
					 if (cursorRow  < PAUSE_ONLINE_ROW) { cursorRow = PAUSE_ONLINE_ROW; }
			}
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
					 if (cursorRow  > PAUSE_LOCAL_ROW+2)  { cursorRow = PAUSE_LOCAL_ROW+2; }
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
			if (gameMode == MODE_LOCAL) {
				cursorKey = pauseLocal[cursorRow-PAUSE_LOCAL_ROW];
			} else {
				cursorKey = pauseOnline[cursorRow-PAUSE_ONLINE_ROW];
			}
		} else {
			if (gameMode == MODE_LOCAL) {
					 if (cursorRow == MENU_ROW+11) { cursorKey = KB_SP; }
				else if (cursorRow == MENU_ROW+9)  { cursorKey = KB_L; }
				else if (cursorRow == MENU_ROW+7)  { cursorKey = KB_M; }
				else if (cursorRow >= MENU_ROW+2)  { cursorKey = 49 + (cursorRow-(MENU_ROW+2)); }
			}   else if (gameMode == MODE_ONLINE)  { cursorKey = 49 + (cursorRow-(MENU_ROW+2)); }
		}
	}
}

void BackupPauseBg()
{
	unsigned char i;
	for (i=0; i<(8*6); ++i) {
		memcpy(&chatBG[0]+i*18, (char*)(BITMAPRAM+1+2*PAUSE_COL+492*PAUSE_ONLINE_ROW+i*82), 18);
	}		
}

void RestorePauseBg()
{
 	unsigned char i;
	for (i=0; i<(8*6); ++i) {
		memcpy((char*)(BITMAPRAM+1+2*PAUSE_COL+492*PAUSE_ONLINE_ROW+i*82), &chatBG[0]+i*18, 18);
	}	
}

unsigned char *localMsg[] = { "resume", "next!", "quit" };
unsigned char *onlineMsg[] = { "resume", "race!", "next!", "hello!", "bye!", "congrat", "thanks!", "quit" };
unsigned char pauseEvt;

unsigned char MenuPause()
{
	// Draw Menu
	unsigned char i, num = 3, row = PAUSE_LOCAL_ROW, **msg = localMsg;
	
	// Set Cursor and Print Options
	cursorCol = PAUSE_COL;
	if (gameMode == MODE_LOCAL) {
		cursorHeight = 3;	
		cursorRow = PAUSE_LOCAL_ROW;
		cursorTop = PAUSE_LOCAL_ROW;
	} else {
		cursorHeight = 8;	
		cursorRow = PAUSE_ONLINE_ROW;
		cursorTop = PAUSE_ONLINE_ROW;
		row = PAUSE_ONLINE_ROW;
		msg = onlineMsg;
		num = 8;
	}		
	paperColor = PAPER_SCORES; inkColor = WHITE;
	PrintBlanks(PAUSE_COL, row, 9, num);
	for (i=0; i<num; i++)
		PrintStr(PAUSE_COL+2, row+i, msg[i]);
			
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
				memcpy(chatBuffer, onlineMsg[cursorKey], 8);
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
	PrintNum(26, 0, lapGoal);
	
	// Reset to default colors
	inkColor = WHITE;
	paperColor = BLACK;
}

// Sub-function of GameMenu()
void MenuMap()
{
	// Clear previous
	PrintBlanks(MENU_COL+7, MENU_ROW+7, MENU_WID-7, 1);
	
	// Print Characters
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	PrintChr(MENU_COL+2, MENU_ROW+7, &charLetter[12*3]);	// 'M'
	inkColor = WHITE; paperColor = BLACK;
	PrintStr(MENU_COL+3, MENU_ROW+7, "AP:");
	PrintStr(MENU_COL+7, MENU_ROW+7, mapList[gameMap]);	
}


// Sub-function of GameMenu()
void MenuLaps()
{
	// Clear previous
	PrintStr(MENU_COL+7, MENU_ROW+9, "  ");
	
	// Print Characters
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	PrintChr(MENU_COL+2, MENU_ROW+9, &charLetter[11*3]);	// 'L'
	inkColor = WHITE; paperColor = BLACK;
	PrintStr(MENU_COL+3, MENU_ROW+9, "AP:");
	PrintNum(MENU_COL+7, MENU_ROW+9, lapNumber[lapIndex]);
}

// In-case connection drops out...
void PrintTimedOut()
{
	inkColor = WHITE;
    PrintStr(10,12, " CONNECTION TIMED-OUT ");
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
	spriteY = 46;
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
			if (car->x2 > 15*64-32 && car->x2 < 25*64+32 && car->y2 > 8*64-32 && car->y2 < 17*64+32)
				DisableSprite(i);
		#elif defined(__LYNX__)
			if (car->x2 > 15*64-32 && car->x2 < 25*64+32 && car->y2 > 4*96-48 && car->y2 < 13*96+48) {
				DisableSprite(i);
			} else {
				spriteX = car->x2/16u; 
				spriteY = car->y2/16u;				
				SetSprite(i, (car->ang2+12)/23u);
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
	PrintBlanks(15, SCORES_ROW, 10, 9);
	
	// Print results and wait
	for (i=0; i<MAX_PLAYERS; ++i) {
		if (score[i] >= 0) {
			j = rank[i];
			s = SCORES_ROW+2*i+1;
			inkColor = inkColors[j];
		#if defined __ORIC__
			SetAttributes(15, s, inkColor);
		#endif			
			if (gameMode == MODE_ONLINE) {
				string = svUsers[j];
			} else {
				if (i == 0) { string = "WIN"; } else { string = "LOSE"; } 
			}
			PrintStr(16, s, string);
			inkColor = WHITE;
		#if defined __ORIC__
			SetAttributes(20, s, inkColor);
		#endif			
			PrintStr(21, s, "- -");
			PrintNum(22, s, i+1);
		#if defined __ORIC__
			SetAttributes(24, s, AIC);
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
				if (i%2) { f = (24-animFrame)&15; } else { f = animFrame; }	
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
	PrintStr(MENU_COL+2, MENU_ROW+6, "FETCH LIST...");				
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
	if (!packet) {
		// Timeout error
		PrintStr(MENU_COL+2, MENU_ROW+7, "ERROR: TIMEOUT");
	} else if (PEEK(packet) != 1 || PEEK(packet+2) != '-') {
		// Unexpected error
		PrintStr(MENU_COL+0, MENU_ROW+7, "ERROR: CORRUPTION");
	} else {
		// Show server list				
		n = PEEK(++packet);
		n = MIN(n,12);
		j = 0;
		while (j<n) {
			// Separate at return carriage (char 10)
			k = 0;
		#ifndef __LYNX__	
			inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;			
			PrintChr(MENU_COL+0, (MENU_ROW+2)+j, &charDigit[(j+1)*3]);
		#endif
			while (PEEK(++packet) != 10 && PEEK(packet) !=0) {
				buffer[k++] = PEEK(packet);
			}
			k = MIN(k,15);
			buffer[k] = 0;
			inkColor = WHITE; paperColor = BLACK;
			PrintStr(MENU_COL+2, (MENU_ROW+2)+j, &buffer[0]);
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
#if defined __LYNX__ 
	// Load user/pass from EEPROM and set Softkeyboard position
	while (res < 18) {
		clUser[res] = lynx_eeprom_read(res);
		res++;
	}
	SetKeyboardOverlay(13,70);
#endif	
	// Prompt for authentication
	for (res=0; res<5; res++)
		PrintStr(loginCol[res], loginRow[res], loginTxt[res]);
	InputField(MENU_COL+6, MENU_ROW+4, clUser, 4);
	PrintChr(MENU_COL+6+strlen(clUser), MENU_ROW+4, charBlank);
	maskInput = 1;
	InputField(MENU_COL+6, MENU_ROW+6, clPass, 10);	
	maskInput = 0;
	PrintChr(MENU_COL+6+strlen(clPass), MENU_ROW+6, charBlank);
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
	PrintStr(MENU_COL+2, MENU_ROW+11, "CONNECTING...");
	ServerConnect();	
	res = ClientJoin(serverIndex);	
	inkColor = WHITE;
	if (res == ERR_MESSAGE) {
		// Server error
		PrintStr(MENU_COL+1, MENU_ROW+12, (char*)(packet+1));
	} else if (res == ERR_TIMEOUT) {
		// Timeout error
		PrintStr(MENU_COL+1, MENU_ROW+12, "ERROR: TIMEOUT");					
	} else if (res == ERR_CORRUPT) {
		// Unexpected error
		PrintStr(MENU_COL+1, MENU_ROW+12, "ERROR: CORRUPTION");
	} else {
		// All good			
		PrintStr(MENU_COL+2, MENU_ROW+12, "OK");
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
	unsigned char row = MENU_ROW+2+i;
	PrintBlanks(MENU_COL+6, row, MENU_WID-6, 1);
	
	// Print Characters
	inkColor = inkColors[i]; paperColor = BLACK;
	PrintChr(MENU_COL+2, row, &charLetter[15*3]);	// 'P'
#ifndef __LYNX__
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
#endif
	PrintNum(MENU_COL+3, row, i+1);					//  i
	inkColor = WHITE; paperColor = BLACK;
	PrintChr(MENU_COL+4, row, charColon);			// ':'
	PrintStr(MENU_COL+6, row, controlList[controlIndex[i]]);	
}

// Sub-function of GameMenu()
void MenuConnect()
{
	// Init network
	unsigned char state = 1;
	unsigned char *report;
#ifdef NETCODE
	PrintStr(MENU_COL+2, MENU_ROW+2, "NETWORK INIT");
	state = InitNetwork();
	if (state == ADAPTOR_ERR) {
		report = "ADAPTOR ERROR";		
	} else if (state == DHCP_ERR) {
		report = "DHCP ERROR";		
	} else {
		report = "DHCP OK!";
		networkReady = 1;
	}
	PrintStr(MENU_COL+4, MENU_ROW+3, report);
#else
	PrintStr(MENU_COL+2, MENU_ROW+2, "NOT SUPPORTED");	
#endif
}

// Display menu options
void MenuTab(unsigned char tab)
{
#if defined __LYNX__			
	inkColor = WHITE; paperColor = BLACK;
	
	if (tab == 0) { inkColor = INK_TAB; };
	PrintStr(MENU_COL+0, MENU_ROW, "LOCAL");
	inkColor = WHITE;
	
	if (tab == 1) { inkColor = INK_TAB; }
	PrintStr(MENU_COL+6, MENU_ROW, "ONLINE");
	inkColor = WHITE;
	
	if (tab == 2) { inkColor = INK_TAB; }
	PrintStr(MENU_COL+13, MENU_ROW, "INFO");
	inkColor = WHITE; 
#elif defined __ORIC__
	inkColor = BLACK; paperColor = AIC;
	PrintStr(MENU_COL+0,  MENU_ROW, "L");			
	PrintStr(MENU_COL+6,  MENU_ROW, "O");
	PrintStr(MENU_COL+13, MENU_ROW, "I");
	inkColor = AIC; paperColor = BLACK;
	PrintStr(MENU_COL+1, MENU_ROW, "OCAL");
	PrintStr(MENU_COL+7, MENU_ROW, "NLINE");
	PrintStr(MENU_COL+14, MENU_ROW, "NFO");
#else
	inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
	PrintStr(MENU_COL+0,  MENU_ROW, "L");			
	PrintStr(MENU_COL+6,  MENU_ROW, "O");
	PrintStr(MENU_COL+13, MENU_ROW, "I");
	inkColor = WHITE; paperColor = BLACK;

	if (tab == 0) { inkColor = INK_TAB; };
	PrintStr(MENU_COL+1, MENU_ROW, "OCAL");
	inkColor = WHITE;

	if (tab == 1) { inkColor = INK_TAB; }
	PrintStr(MENU_COL+7, MENU_ROW, "NLINE");
	inkColor = WHITE;
	
	if (tab == 2) { inkColor = INK_TAB; }
	PrintStr(MENU_COL+14, MENU_ROW, "NFO");
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
		if (i%2) { f = 8; } else { f = 0; }
		SpriteAnimation(i,f);
		EnableSprite(i);
	}	
		
	// Show version, credits, and start music
	inkColor = WHITE; paperColor = BLACK;
	PrintStr(MENU_COL, MENU_BLD, buildInfo);

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
		PrintBlanks(MENU_COL, MENU_ROW, MENU_WID, MENU_HEI);
	
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
			PrintStr(MENU_COL+2, MENU_ROW+11, "RACE!");				
		#else
			inkColor = INK_HIGHLT; paperColor = PAPER_HIGHLT;
			PrintStr(MENU_COL+2, MENU_ROW+13, "SPACE");
			inkColor = WHITE; paperColor = BLACK;
			PrintStr(MENU_COL+7, MENU_ROW+13, ": RACE!");				
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
						gameMap--; if (gameMap >= LEN_MAPS) gameMap = LEN_MAPS-1;
					} else {
						gameMap++; if (gameMap >= LEN_MAPS) gameMap = 0;
					}
				#else
					gameMap++; if (gameMap >= LEN_MAPS) gameMap = 0;
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
					frameBlending ^= 2;
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
					// Black-out server list
					PrintBlanks(MENU_COL, MENU_ROW+2, MENU_WID, MENU_HEI-2);
					if (MenuLogin(i)) {
						// Start game
						return;
					} else {
						// Redraw server list
						sleep(2);
						PrintBlanks(MENU_COL, MENU_ROW+2, MENU_WID, MENU_HEI-2);
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
			for (i=0; i<CREDIT_ROWS; i++)
				PrintStr(creditCol[i], creditRow[i], creditTxt[i]);

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

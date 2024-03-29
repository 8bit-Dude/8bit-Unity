
#include "definitions.h"

#if defined(__LYNX__) || defined(__NES__)

#if defined(__LYNX__)
  #define PAUSE_COL 		15
  #define PAUSE_LOCAL_ROW   6
  #define PAUSE_ONLINE_ROW  2
#elif defined(__NES__)
  #define PAUSE_COL 		10
  #define PAUSE_LOCAL_ROW   9
  #define PAUSE_ONLINE_ROW  5	
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

#ifdef __LYNX__
void __fastcall__ SuzyFlip(void);
#endif

// See slicks.c
extern unsigned char mapNum, inkColors[];

// See game.c
extern unsigned char gameMode;

// See interface.c
extern unsigned int bestLapTime[];
#if defined __LYNX__
  extern char chatBG[1440];
#elif defined __NES__
  extern char *chatBG;  
#endif

// See navigation.c
extern Vehicle cars[MAX_PLAYERS];

// See network.c
extern unsigned char clIndex, clUser[];
extern char chatBuffer[20];

#if defined(__LYNX__)		
  ///////// EEPROM Functions //////////////
  const char eepromID[] = "SS";

  void ResetEEPROM(void) {
	// Write ID and Blanks to EEPROM
	unsigned char i = 0;
	while (i < 2) {	
		lynx_eeprom_write(i, eepromID[i]);
		i++;	
	}
	while (i < 20) {
		lynx_eeprom_write(i, 0);
		i++;
	}
	while (i < 64) {
		lynx_eeprom_write(i++, 52);
		lynx_eeprom_write(i++, 23);		
	}
  }
  
  unsigned char CheckEEPROM(void) {
	// Check EEPROM ID
	unsigned char i = 0;
	while (i < 2) {
		if (eepromID[i] != (unsigned char)lynx_eeprom_read(i)) {
			ResetEEPROM();
			return 0;
		}
		i++;
	}
	return 1;
  }    

  void ReadEEPROM(void) {
	unsigned int time;
	unsigned char i=2, j=0;
	
	// Check EEPROM Tag
	if (!CheckEEPROM())
		return;
	
	// Read user/pass
	while (i < 20)
		clUser[j++] = (unsigned char)lynx_eeprom_read(i++);
	
	// Read best lap times
	j = 0;
	while (i < 64) {	
		time =  (unsigned char)lynx_eeprom_read(i++);
		time += (unsigned char)lynx_eeprom_read(i++) * (unsigned int)256;
		if (!time)
			time = LAPMAX;		
		bestLapTime[j++] = time;
	}
  }

  void WriteEEPROM(void) {	
	unsigned int time;
	unsigned char i=2, j=0;

	// Write user/pass
	while (i < 20)
		lynx_eeprom_write(i++, clUser[j++]);
	
	// Write best lap times
	j=0;
	while (i < 64) {	
		time = bestLapTime[j++];
		lynx_eeprom_write(i++, (unsigned char)(time%256));
		lynx_eeprom_write(i++, (unsigned char)(time/256));
	}	
  }
#endif

unsigned char gamePaused = 0;
clock_t cursorClock;

#if defined __NES__
 #pragma bss-name(push, "XRAM")
#endif

const unsigned char *pauseLabel[] = { "resume", "race!", "next!", "quit", "bye!", "congrats", "hang on", "hello!", "nice", "so close", "thanks!", "yes" };
const unsigned char pauseAction[] = { KB_PAUSE, KB_START, KB_NEXT, KB_QUIT, 4, 5, 6, 7, 8, 9, 10, 11 };
unsigned char cursorJoy, cursorKey, cursorBut2, cursorPressed;
unsigned char cursorFlick, cursorCol = MENU_COL, cursorRow = MENU_ROW+2;
unsigned char cursorTop = MENU_ROW+2, cursorHeight = MENU_HEI-2;

#if defined __NES__
 #pragma bss-name(pop) 
#endif

void CursorFlicker(void)
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
		PrintChr('-'); txtX++;
		PrintChr(')');
		inkColor = WHITE;
	}
	cursorFlick = !cursorFlick;
}

void CursorControl(void)
{
#ifdef __LYNX__
	// Process EEPROM Reset
	clock_t timer;
	if (PEEK(0xFCB0) & 0b00001100) {
		if (clock() - timer > 120) {
			ResetEEPROM();
			txtX = 22; txtY = 16;
			if (CheckEEPROM()) {
				inkColor = GREEN;
				PrintStr("  EEPROM RESET!  ");
			} else {
				inkColor = RED;
				PrintStr("  EEPROM ERROR!  ");
			}
			inkColor = WHITE;
			BleepSFX(128);			
			timer = clock();
		}
	} else {
		timer = clock();
	}
#endif	

	// Process screen flips
	if (kbhit()) {
		switch (cgetc()) {
	#ifdef __LYNX__
		case KB_FLIP:
			SuzyFlip();
			break;
		case KB_MUSIC:
			if (!gamePaused)
				NextMusic(0);
			break;
	#endif
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
	if (gamePaused) {
		// Pause Menu
		if (!(cursorJoy & JOY_UP)) { 
			cursorRow -= 1; 
			if (cursorRow < cursorTop) {
				if (gameMode == MODE_LOCAL) {
					cursorRow = PAUSE_LOCAL_ROW+3;
				} else {
					cursorRow = PAUSE_ONLINE_ROW+11;
				}
			}
		}	
		if (!(cursorJoy & JOY_DOWN)) { 
			cursorRow += 1; 
			if (gameMode == MODE_LOCAL) {
				if (cursorRow > PAUSE_LOCAL_ROW+3)  { cursorRow = cursorTop; }
			} else {
				if (cursorRow > PAUSE_ONLINE_ROW+11) { cursorRow = cursorTop; }
			}
		}
		if (!(cursorJoy & JOY_BTN1) || !(cursorJoy & JOY_BTN2)) { 
			cursorKey = pauseAction[cursorRow-cursorTop];
		}
	} else {
		// Main Menu
		if (!(cursorJoy & JOY_LEFT)) { 
			cursorRow = MENU_ROW+2; 
				 if (gameMode == MODE_INFO)   { cursorKey = KB_O; }
			else if (gameMode == MODE_ONLINE) { cursorKey = KB_L; }
			else							  { cursorKey = KB_I; }
		}	
		if (!(cursorJoy & JOY_RIGHT)) { 
			cursorRow = MENU_ROW+2; 
				 if (gameMode == MODE_LOCAL)  { cursorKey = KB_O; }
			else if (gameMode == MODE_ONLINE) { cursorKey = KB_I; }
			else							  { cursorKey = KB_L; }
		}	
		if (!(cursorJoy & JOY_UP)) { 
			cursorRow -= 1; 
			if (gameMode == MODE_LOCAL) {
					 if (cursorRow  < MENU_ROW+2)  { cursorRow = MENU_ROW+11;}			
				else if (cursorRow == MENU_ROW+6)  { cursorRow = MENU_ROW+5; }			
				else if (cursorRow == MENU_ROW+8)  { cursorRow = MENU_ROW+7; }			
				else if (cursorRow == MENU_ROW+10) { cursorRow = MENU_ROW+9; }			
			} else if (gameMode == MODE_ONLINE) {
					 if (cursorRow  < MENU_ROW+2)  { cursorRow = MENU_ROW+10; }
			} else if (gameMode == MODE_INFO)   {
					 if (cursorRow  < MENU_ROW+2)  { cursorRow = MENU_ROW+3+mapNum/8u; }
					 cursorKey = KB_U;
			}
		}
		if (!(cursorJoy & JOY_DOWN)) { 
			cursorRow += 1; 
			if (gameMode == MODE_LOCAL) {
					 if (cursorRow  > MENU_ROW+11) { cursorRow = MENU_ROW+2; }
				else if (cursorRow == MENU_ROW+10) { cursorRow = MENU_ROW+11; }
				else if (cursorRow == MENU_ROW+8)  { cursorRow = MENU_ROW+9; }			
				else if (cursorRow == MENU_ROW+6)  { cursorRow = MENU_ROW+7; }			
			} else if (gameMode == MODE_ONLINE) {
					 if (cursorRow  > MENU_ROW+10) { cursorRow = MENU_ROW+2; }
			} else if (gameMode == MODE_INFO)   {  
					 if (cursorRow  > MENU_ROW+3+mapNum/8u)  { cursorRow = MENU_ROW+2; }
					 cursorKey = KB_U;
			}
		}
		if (!(cursorJoy & JOY_BTN1) || !(cursorJoy & JOY_BTN2)) { 
			cursorBut2 = 0;
			if (!(cursorJoy & JOY_BTN2))
				cursorBut2 = 1;
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
#if defined(__LYNX__)
	// Copy screen data
	unsigned char i;
	unsigned int addr1 = chatBG;
	unsigned int addr2 = BITMAPRAM+1+2*PAUSE_COL+492*PAUSE_ONLINE_ROW;
	for (i=0; i<(12*6); ++i) {
		if (!mode)
			memcpy(addr1, addr2, 20);
		else
			memcpy(addr2, addr1, 20);
		addr1 += 20; addr2 += 82;
	}		
#elif defined(__NES__)	
	// Use chunks
	if (!mode) {
		chatBG = GetChunk(PAUSE_COL, PAUSE_ONLINE_ROW, 10, 12);
	} else {
		SetChunk(chatBG, PAUSE_COL, PAUSE_ONLINE_ROW);
		FreeChunk(chatBG);
	}
#endif	
}

unsigned char pauseEvt;

unsigned char MenuPause(void)
{
	unsigned char i;
	
	// Set Cursor and Print options
	cursorCol = PAUSE_COL;
	if (gameMode == MODE_LOCAL) {
		cursorHeight = 4;	
		cursorRow = PAUSE_LOCAL_ROW;
		cursorTop = PAUSE_LOCAL_ROW;
	} else {
		cursorHeight = 12;	
		cursorRow = PAUSE_ONLINE_ROW;
		cursorTop = PAUSE_ONLINE_ROW;
	}		
	txtX = PAUSE_COL; txtY = cursorRow;
	PrintBlanks(10, cursorHeight);
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
			cars[clIndex].vel = 0;
			pauseEvt = NetworkUpdate();
			if (pauseEvt == EVENT_RACE || pauseEvt == EVENT_MAP || pauseEvt == ERR_TIMEOUT)
				return 0;
		}
		// Process Cursor
		CursorControl();
		if (cursorKey) { 
			if (gameMode == MODE_ONLINE && cursorKey < 12) {
				// Process chat event then exit menu
				memcpy(chatBuffer, pauseLabel[cursorKey], 9);
				ClientEvent(EVENT_CHAT);
				return KB_PAUSE;
			} else {
				return cursorKey; 
			}
		}
		CursorFlicker(); 
		UpdateDisplay();
	}	
}

#endif

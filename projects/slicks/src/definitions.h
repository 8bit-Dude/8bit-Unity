
// 8bit-Unity SDK
#include "unity.h"

// *** Platform specific HACKS ***
#if defined __CBM__
	#undef  MUSICRAM
	#define MUSICRAM (0x1000)  // This SID file is located at $1000 instead of usual $0800
	//sidInitAddr = 0x1048;	
	//sidPlayAddr = 0x1021;
#endif			

// Platform dependent definitions
#if defined __CBM__
	#define KB_START 	CH_F1
	#define KB_NEXT  	CH_F3
#else
	#define KB_START 	49
	#define KB_NEXT  	50
#endif
#if defined __LYNX__
	#define KB_CHAT 	KB_B	
	#define KB_QUIT 	KB_P	
#else
	#define KB_CHAT 	KB_C	
	#define KB_QUIT 	KB_Q	
#endif

// Game definitions
#define LEN_MAPS    10
#define LEN_LAPS    4
#define MAX_PLAYERS 4
#define MAX_WAYPOINTS 8
#define MAX_RAMPS     3
#define LERP_THRESHOLD 128

#define MODE_LOCAL  1
#define MODE_ONLINE 2
#define MODE_INFO   3

#define STEP_WARMUP 1
#define STEP_RACE   2

// Build Information
const char* buildInfo = "BUILD: 2020/11/02";

// Game data
unsigned char gameMap = 0;
unsigned char gameMode = MODE_LOCAL;
unsigned char gameStep = STEP_WARMUP;
unsigned char gameLineUp[4] = { 0, 1, 2, 3 };

// Lap information
unsigned char lapIndex = 0;
unsigned char lapGoal;

// Sprite definitions
#if defined __APPLE2__
	#define spriteFrames 64
	#define spriteCols   7	
	#define spriteRows   5	
	unsigned char spriteColors[] = { };	//  Colors are pre-assigned in the sprite sheet
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __ATARI__
	#define spriteFrames 18
	#define spriteCols   8
	#define spriteRows   10
	unsigned char spriteColors[] = {0x74, 0x24, 0xa6, 0xdc, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22 };	// Refer to atari palette in docs
	unsigned char inkColors[] = { BLUE, RED, GREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __ORIC__
	#define spriteFrames 17
	#define spriteCols   12
	#define spriteRows   6
	unsigned char spriteColors[] = { SPR_CYAN, SPR_MAGENTA, SPR_GREEN, SPR_WHITE, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	
	unsigned char inkColors[] = { CYAN, LPURPLE, LGREEN, GREY, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __CBM__
	#define spriteFrames 18
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW, LGREY, LGREY, LGREY, 0, CYAN, BLACK };	// P1, P2, P3, P4, Light1, Light2, Light3, n/a, Shared Color 1, Shared Color 2
	unsigned char inkColors[] = { BLUE, RED, LGREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
#elif defined __LYNX__
	#define spriteFrames 18
	#define spriteCols   7
	#define spriteRows   9
	unsigned char spriteColors[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x0b, 0x23, 0x45, 0x67, 0x89, 0xa1, 0xcd, 0xef,   // Swapped 1 and B
									 0x05, 0x23, 0x41, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Swapped 1 and 5
									 0x09, 0x23, 0x45, 0x67, 0x81, 0xab, 0xcd, 0xef,   // Swapped 1 and 9
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }; // Default palette
	unsigned char inkColors[] = { DBLUE, RED, LGREEN, YELLOW, WHITE };		// P1, P2, P3, P4, SERVER INFO
#endif

// List of available maps
const char *mapList[LEN_MAPS] = {"arizona","arto","cramp","freeway","gta","island","mtcarlo","rally","river","stadium"};

// List of lap goals
unsigned char lapNumber[LEN_LAPS] = { 5, 10, 20, 50 };

// List of controller types
unsigned char controlIndex[MAX_PLAYERS] = { 3, 1, 0, 0 };
#if defined __APPLE2__
	#define LEN_CONTROL 8
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU HARD", "PADDLE 1", "PADDLE 2", "PADDLE 3", "PADDLE 4", "NETWORK" };
#elif defined __ATARI__
	#define LEN_CONTROL 8
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU HARD", "JOY 1", "JOY 2", "HUB 1", "HUB 2", "NETWORK" };
#elif defined __ORIC__
	#define LEN_CONTROL 8
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU HARD", "A,D,CTRL", "J,L,RET", "PASE/HUB 1", "PASE/HUB 2", "NETWORK" };
#elif defined __CBM__
	#define LEN_CONTROL 8	
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU HARD", "JOY 1", "JOY 2", "JOY 3", "JOY 4", "NETWORK" };
#elif defined __LYNX__
	#define LEN_CONTROL 7
	const char* controlList[LEN_CONTROL] = { "NONE", "CPU EASY", "CPU HARD", "JOY 1", "HUB 1", "HUB 2", "NETWORK" };
#endif
#define NET_CONTROL (LEN_CONTROL-1)

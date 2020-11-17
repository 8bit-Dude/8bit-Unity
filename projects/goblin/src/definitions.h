
#include "unity.h"

// *** Platform specific HACKS ***
#if defined __ATARI__
	#undef  MUSICRAM
	#define MUSICRAM (0x9800)	// Moved Music RAM start to allow more space for sprites
#endif

// Motion parameters
#if defined __APPLE2__
  #define mouseStep 3	// Apple soft sprites cause slow animation...
  #define unitStep  3
  #define unitTicks 3
#elif defined __ORIC__
  #define mouseStep 3
  #define unitStep  3
  #define unitTicks 6
#else
  #define mouseStep 2
  #define unitStep  3
  #define unitTicks 5
#endif	

// Sprite animation frames
#define frameWaitLeft     2
#define frameWalkLeftBeg  3
#define frameWalkLeftEnd  7
#define frameWaitRight    8
#define frameWalkRightBeg 9
#define frameWalkRightEnd 13

// Sprite definitions
#define spriteFrames 14
#if defined __APPLE2__	
	#define spriteCols   7
	#define spriteRows   24
	unsigned char spriteColors[] = { };  // Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	#define spriteCols   8
	#define spriteRows   26
	unsigned char spriteColors[] = { 0x0e, 0x80, 0x10, 0x2a, 0xba, 0x00, 0x00, 0x00, 0x00, 0x00 };  // Refer to atari palette in docs
#elif defined __ORIC__
	#define spriteCols   12
	#define spriteRows   24
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };  // AIC color allows faster drawing!
	unsigned char multiColorDef[] = { SPR_WHITE, 8, SPR_GREEN, 14, SPR_MAGENTA, 24 };	// Multicolor definition { color, row, ... }
#elif defined __CBM__
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { WHITE, BLUE, BROWN, PINK, GREEN, WHITE, WHITE, WHITE, BLACK, WHITE };	// 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	#define spriteCols   9
	#define spriteRows   13
	unsigned char spriteColors[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }; // Default palette	
#endif

// Chunks for scene animation
unsigned char* chunkAnim[6];
unsigned char* chunkBcgr[3];

// Scene interactable flags
#define INACTIVE 0
#define ACTIVE   1
#define PICKABLE 2

// Scene interactable objects 
typedef struct {
	unsigned int x, y, r;	// Mouse detection circle
	unsigned int cx, cy;  // Mouse click coordinates
	unsigned char flags;
	unsigned char *label, *question, *answer;	
	unsigned char *chunk1, *chunk2;
} Interact;

#define MAX_INTERACT 7
Interact interacts[MAX_INTERACT] = 
	{ {  55,  81, 15,  90,  93, 	ACTIVE,		 "Notable",  "Dear sir, you look powerful.\nPlease help me with my quest?\0", "Go away, I am busy!\0", 0, 0 },
	  { 222,  66, 15, 194,  86, 	ACTIVE,		 "Old Men",  "Hey villagers, do you know\nthe house of lord Tazaar?\0", "We are hungry!! But the\nnotable keeps all the food.", 0, 0 },
	  { 260,  70,  7, 240,  94, ACTIVE|PICKABLE, "Bottle",   0, 0, 0, 0 },
	  { 230, 134, 10, 208, 141, ACTIVE|PICKABLE, "Flower",   0, 0, 0, 0 },
	  {  69,  60,  7,  94,  93, 	ACTIVE, 	 "Sausage",  0, "Hey, don't touch that!", 0, 0 },
	  {  32,  77,  5,  18, 101, 	ACTIVE,		 "Switch",   0, 0, 0, 0 },
	  { 300, 170, 30, 300, 170,    INACTIVE,	 "Fountain", 0, "Well done little goblin!\nThe tech-demo ends here!", 0, 0 } };
  
// Player inventory
typedef struct {
	char col, row;
	char  flags;
	char* label;
} Item;

#define INVENTORY_Y 180
#define MAX_ITEM 2
Item items[MAX_ITEM] = 
	{ { CHR_COLS-7,  CHR_ROWS-2, INACTIVE, 0 },
	  { CHR_COLS-7,  CHR_ROWS-1, INACTIVE, 0 } };

// Accessible polygon
#define MAX_POLYGON 19
signed int polygonX[MAX_POLYGON] = {   0,  53, 120, 138,  65,   0,  0, 32, 180, 270, 282, 251, 232, 210, 229, 320, 320,   0,   0 };
signed int polygonY[MAX_POLYGON] = { 138, 162, 169, 144, 138, 107, 99, 95, 78, 102, 120, 137, 124, 143, 168, 116, 180, 180, 138 };

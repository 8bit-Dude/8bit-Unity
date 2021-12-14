
#include "unity.h"

// Text colors
#if defined __SHR__	// Apple single hires (Orange paper can only be combined with Black, Blue or White ink)
	#define INK_DEFAULT		WHITE
	#define INK_GOBLIN		WHITE
	#define INK_INTERACT	BLACK
	#define INK_INVENTORY	BLACK
#else
	#define INK_DEFAULT		WHITE
	#define INK_GOBLIN		YELLOW
	#define INK_INTERACT	RED
	#define INK_INVENTORY	BLACK
#endif

// Motion parameters
#if defined __APPLE2__
  #define unitStep  3
  #define unitTicks 3
#elif defined __ORIC__
  #define unitStep  3
  #define unitTicks 6
#else
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

// Scene interactable flags
#define INACTIVE 0
#define ACTIVE   1
#define PICKABLE 2

// Interacts and Inventory
#define MAX_INTERACT  7
#define MAX_ITEM 	  2
#define INVENTORY_Y 180

// Scene interactable objects 
typedef struct {
	unsigned int x, y, r;	// Mouse detection circle
	unsigned int cx, cy;  // Mouse click coordinates
	unsigned char flags;
	unsigned char *label, *question, *answer;	
	unsigned char *chunk1, *chunk2;
} Interact;
  
// Player inventory
typedef struct {
	char col, row;
	char  flags;
	char* label;
} Item;

// See interface.c
void PrintInteract(unsigned char item, unsigned char *label);
void PrintMessage(unsigned char *msg);
void PrintInventory(void);
void DrawPointer(unsigned int x, unsigned int y, unsigned char pressed);
void DrawUnit(unsigned int x, unsigned int y, unsigned char frame);

// See scene.c
void InitScene(void);
void Wait(unsigned char ticks);
void PushItem(char* label);
void PopItem(unsigned char index);
unsigned char SelectItem(unsigned int x, unsigned int y);
unsigned char SearchScene(unsigned int searchX, unsigned int searchY);
unsigned char ProcessInteract(unsigned char index, unsigned char item, unsigned int unitX, unsigned int unitY);

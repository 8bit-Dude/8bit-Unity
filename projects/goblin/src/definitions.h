
#include "unity.h"

// Scene interactable flags
#define DISABLED  0
#define ACTIVE    1
#define PICKABLE  2
#define LOADSCENE 4

// Scene navigation data (loaded from file)
#define MAX_POLYGON  25
#define MAX_CHUNK     6
#define MAX_INTERACT  7
#define MAX_TRIGGER   5
#define MAX_MODIFIER  2
#define MAX_PATH      6

// Inventory management
#define MAX_ITEM 	  2
#define INVENTORY_Y 180

// Text colors
#if defined __NES__	
	#define MSG_WIDTH	   TXT_COLS
	#define INK_DEFAULT		WHITE
	#define INK_GOBLIN		GREEN
	#define INK_INTERACT	RED
	#define INK_INVENTORY	WHITE
	#define PAPER_DEFAULT   BLACK
#elif defined __ATARI__		
	#define MSG_WIDTH	  TXT_COLS-8
	#define INK_DEFAULT		BLACK
	#define INK_GOBLIN		GREEN
	#define INK_INTERACT	BLUE
	#define INK_INVENTORY	BLACK	
	#define PAPER_DEFAULT   RED
#elif defined __LYNX__
	#define MSG_WIDTH	  TXT_COLS-8
	#define INK_DEFAULT		WHITE
	#define INK_GOBLIN		YELLOW
	#define INK_INTERACT	BROWN
	#define INK_INVENTORY	BLACK
	#define PAPER_DEFAULT   ORANGE	
#else
	#define MSG_WIDTH	  TXT_COLS-8
  #if defined __SHR__	// Apple single hires (Orange paper can only be combined with Black, Blue or White ink)
	#define INK_DEFAULT		WHITE
	#define INK_GOBLIN		WHITE
	#define INK_INTERACT	BLACK
	#define INK_INVENTORY	BLACK
	#define PAPER_DEFAULT   ORANGE
  #else
	#define INK_DEFAULT		WHITE
	#define INK_GOBLIN		YELLOW
	#define INK_INTERACT	RED
	#define INK_INVENTORY	BLACK
	#define PAPER_DEFAULT   ORANGE
  #endif
#endif

// Motion parameters
#if defined __APPLE2__
  #define unitStep  3
  #define unitTicks 3
#elif defined __C64__
  #define unitStep  3
  #define unitTicks 4
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

// Interact Objects 
typedef struct {
	unsigned int label;							// Identifier
	unsigned int x, y, r, cx, cy;				// Mouse detection circle and move to target
	unsigned char flags, chk, bcg, frame, path;	// Flags and chunk IDs
	unsigned int question, answer;				// Strings
} Interact;

// Action Triggers 
typedef struct {
	unsigned int  item, label;				// Source and target of trigger (Strings)
	unsigned char modifier, chk, bcg;		// Modifier IDs
	unsigned int  answer;					// Strings
} Trigger;

// Interact Modifiers 
typedef struct {
	unsigned int  label;					// Source and target of trigger (Strings)
	unsigned char flags, chk, bcg, path;	// Flags and chunk IDs
	unsigned int  question, answer;			// Strings
} Modifier;
  
// Animation Paths
typedef struct {
	unsigned int  x, y;						// Coordinates of trajectory
	unsigned char speed, frame, next;		// Path speed, frame, and ID of next path
} Path;
  
// Player inventory
typedef struct {
	unsigned char col, row;
	unsigned int  label;
} Item;

// See interface.c
void PrintInteract(unsigned char item, unsigned int label);
void PrintMessage(unsigned int message);
void PrintInventory(void);
void DrawPointer(unsigned int x, unsigned int y, unsigned char pressed);
void DrawUnit(unsigned int x, unsigned int y, unsigned char frame);
void SplashScreen(void);

// See inventory.c
void PushItem(unsigned int label);
void PopItem(unsigned char index);
unsigned char SelectItem(unsigned int x, unsigned int y);

// See scene.c
void LoadScene(unsigned char* scene);
void PushItem(unsigned int label);
void PopItem(unsigned char index);
unsigned char SelectItem(unsigned int x, unsigned int y);
unsigned char SearchScene(unsigned int searchX, unsigned int searchY);
unsigned char *ProcessInteract(unsigned char index, unsigned char item);
void Wait(unsigned char ticks);

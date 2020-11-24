
#include "unity.h"

// Screen cropping around charmap
#if defined (__APPLE2__) || defined(__ORIC__)
	#define CROP_X  4	// Crop more for platforms that run in HIRES
	#define CROP_Y  3
#elif defined __LYNX__
	#define CROP_X  2	
	#define CROP_Y  0	// Lynx has only 17 lines (vs. 25 on other platforms)
#else
	#define CROP_X  2
	#define CROP_Y  2
#endif

// Player Motion
#if defined __LYNX__	
	#define SCALE_X 8
	#define SCALE_Y 12
	#define STEP_X  3
	#define STEP_Y  3 	
#else
	#define SCALE_X 8
	#define SCALE_Y 8
	#define STEP_X  3
	#define STEP_Y  2 
#endif

// Sprite definitions
#define spriteFrames 32

// Sprite slot assignments
#if defined __ATARI__	
	#define SPRITE_WEAPON   0
	#define SPRITE_PLAYER0  1
	#define SPRITE_PLAYER1  2
	#define SPRITE_PLAYER2  3
	#define SPRITE_ENEMY    4
	#define COLOR_GOLD 	   0xec
	#define COLOR_POTION   0x34
	#define COLOR_ARMOR    0x12
	#define COLOR_KEY      0xec
	#define COLOR_SKELETON 0x0e
	#define COLOR_GOBLIN   0xbc
#elif defined __ORIC__	
	#define SPRITE_WEAPON   0
	#define SPRITE_PLAYER   1
	#define SPRITE_ENEMY    2
	#define COLOR_GOLD 	   SPR_YELLOW
	#define COLOR_POTION   SPR_RED
	#define COLOR_ARMOR    SPR_MAGENTA
	#define COLOR_KEY      SPR_YELLOW
	#define COLOR_SKELETON SPR_WHITE
	#define COLOR_GOBLIN   SPR_GREEN	
#else
	#define SPRITE_WEAPON   0
	#define SPRITE_PLAYER   1
	#define SPRITE_ENEMY    2
	#define COLOR_GOLD 	   YELLOW
	#define COLOR_POTION   YELLOW
	#define COLOR_ARMOR    YELLOW
	#define COLOR_KEY      YELLOW
	#define COLOR_SKELETON WHITE
	#define COLOR_GOBLIN   YELLOW
#endif

// Animation Key Frames
#define KEY_PLAYER     0
#define KEY_WEAPON     8
#define KEY_GOLD      12
#define KEY_POTION    13
#define KEY_ARMOR     14
#define KEY_KEY       15
#define KEY_SKELETON  16
#define KEY_GOBLIN    24

// Animation Stance Frames
#define STANCE_RIGHT   0
#define STANCE_LEFT	   4

// Actor data
# define ACTOR_NUM   16

# define ACTOR_NULL   0
# define ACTOR_GUARD  1
# define ACTOR_ATTACK 2
# define ACTOR_GOLD   3
# define ACTOR_POTION 4
# define ACTOR_ARMOR  5
# define ACTOR_KEY    6
# define ACTOR_MAX    6

// Flag definitions
#define FLAG_NULL      0
#define FLAG_WALL  	   1
#define FLAG_TRAP      2
#define FLAG_TOMB      4
#define FLAG_BARREL    8
#define FLAG_GATE     16

// Tile definitions
#define TILE_FLOOR	     2
#define TILE_TOMB_SHUTL 18
#define TILE_TOMB_SHUTR	19
#define TILE_TOMB_OPENL 36
#define TILE_TOMB_OPENR	37
#define TILE_GATE_SHUTL	38
#define TILE_GATE_SHUTR	39

// Actor variables (Enemies/Items)
typedef struct {
	unsigned char state, color, health;
	unsigned char key, stance, frame;
	unsigned char mapX, mapY;
	unsigned int scrX, scrY;
	clock_t timer;
} Actor;

// Actor functions (actors.c)
void LoadActors(const char* filename);
void ProcessActors(void);
unsigned char CheckActorCollision(unsigned int scrX, unsigned int scrY, unsigned char flag);
unsigned char FindActorSlot(void);
void GenerateMonster(unsigned char mapX, unsigned char mapY);
void GenerateReward(unsigned char mapX, unsigned char mapY);
void DamageMonster(Actor* actor);
void PickupReward(Actor* actor);

// GUI functions (interface.c)
void SplashScreen(void);
void PrintHealth(void);
void PrintArmor(void);
void PrintGold(void);
void PrintKills(void);
void PrintKey(void);

// Player functions (player.c)
void ProcessPlayer(void);
void ProcessWeapon(void);


#include "unity.h"

// Memory tweaks
#if defined __ATARI__
	#undef  MUSICRAM
	#define MUSICRAM (0x9800)	// Moved Music RAM start to allow more space for sprites	
#endif

// Char flags
#define CHAR_NULL     0
#define CHAR_WALKABLE 1
#define CHAR_DAMAGE   2
#define CHAR_PICKUP   4
#define CHAR_INTERACT 8

// Sprite definitions
#if defined __APPLE2__
	#define spriteFrames 32
	#define spriteCols   7
	#define spriteRows   16
	unsigned char charColors[] = {};    //  All colors are pre-assigned in the char sheet
	unsigned char spriteColors[] = {};  //  All colors are pre-assigned in the sprite sheet	
#elif defined __ATARI__
	#define spriteFrames 32
	#define spriteCols   8
	#define spriteRows   18
	unsigned char charColors[] = { 0x00, 0x0c, 0x78, 0x62, 0x12 };   // Black, White, Light Blue, Dark Blue, Red
	unsigned char spriteColors[] = { 0x0c, 0x14, 0xbc, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // White (sword), Brown, Green, Pink (player), White (enemies)
#elif defined __ORIC__
	#define spriteFrames 32
	#define spriteCols   12
	#define spriteRows   17
	unsigned char charColors[] = {};    //  All colors are pre-assigned in the char sheet
	unsigned char spriteColors[] = { SPR_WHITE, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	// White (sword), AIC (player), AIC (enemies)
	unsigned char multiColorDef[] = { SPR_WHITE, 7, SPR_GREEN, 14 , SPR_YELLOW, 17 };	// Multicolor definition { color, row, ...  }
#elif defined __CBM__
	#define spriteFrames 32
	#define spriteCols   12
	#define spriteRows   21
	unsigned char charColors[] = { BLACK, WHITE, LBLUE };
	unsigned char spriteColors[] = { WHITE, YELLOW, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, RED, GREEN };  // 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	#define spriteFrames 32
	#define spriteCols   7
	#define spriteRows   9
	unsigned char charColors[] = {};    //  All colors are pre-assigned in the char sheet
	unsigned char* spriteColors = defaultColors;	// All sprites use the default palette
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

// Sprite slot assignments
#if defined __ATARI__	
	#define SPRITE_WEAPON   0
	#define SPRITE_PLAYER0  1
	#define SPRITE_PLAYER1  2
	#define SPRITE_PLAYER2  3
	#define SPRITE_ENEMY    4
	#define COLOR_SKELETON 0x0c
	#define COLOR_GOBLIN   0xbc
#elif defined __LYNX__	
	#define SPRITE_PLAYER   0
	#define SPRITE_WEAPON   1
	#define SPRITE_ENEMY    2
	#define COLOR_SKELETON WHITE
	#define COLOR_GOBLIN   GREEN
#elif defined __ORIC__	
	#define SPRITE_WEAPON   0
	#define SPRITE_PLAYER   1
	#define SPRITE_ENEMY    2
	#define COLOR_SKELETON SPR_WHITE
	#define COLOR_GOBLIN   SPR_GREEN
#else
	#define SPRITE_WEAPON   0
	#define SPRITE_PLAYER   1
	#define SPRITE_ENEMY    2
	#define COLOR_SKELETON WHITE
	#define COLOR_GOBLIN   YELLOW
#endif

// Animation Frames
#define FRAME_PLAYER     0
#define FRAME_WEAPON     8
#define FRAME_SKELETON  16
#define FRAME_GOBLIN    24

// Enemy data
# define ENEMY_NUM  8
# define ENEMY_NULL   0
# define ENEMY_GUARD  1
# define ENEMY_ATTACK 2
typedef struct {
	unsigned char state, color;
	unsigned char frame, offset;
	unsigned char mapX, mapY;
} Enemy;
Enemy enemies[ENEMY_NUM];

char life[] = { 1 , '1', '0', '0', 0};
char mana[] = { 2 , '1', '0', '0', 0};
char gold[] = {'$', '0', '0', '0', 0};
char kill[] = { 4 , '0', '0', '0', 0};

void SplashScreen(void)
{	
	// Load and show banner
	InitBitmap();
	LoadBitmap("menu.img");
	EnterBitmapMode();
	
	// Show credit/build
	paperColor = BLACK; 
	PrintBlanks(CHR_COLS-13, CHR_ROWS-4, 12, 3);
#if (defined __ORIC__)
	inkColor = AIC;
#else		
	inkColor = RED; 
#endif
	PrintStr(CHR_COLS-12, CHR_ROWS-4, "TECH DEMO");		
	PrintStr(CHR_COLS-13, CHR_ROWS-3, "BY 8BIT-DUDE");		
	PrintStr(CHR_COLS-12, CHR_ROWS-2,  "2020/11/16");
	
	// Start music
	PlayMusic();

	// Wait until key is pressed
	while (!kbhit()) {	
	#if defined __APPLE2__
		UpdateMusic();
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif
	}	
	
	// Exit banner screen
	ExitBitmapMode();
	StopMusic();	
}

void GameInit(void)
{
	// Setup charmap
#if defined (__APPLE2__) || defined(__ORIC__)
	screenCol1 = 6; screenCol2 = CHR_COLS-6;
	screenRow1 = 3; screenRow2 = CHR_ROWS-4; 
#elif defined __LYNX__
	screenCol1 = 2; screenCol2 = CHR_COLS-2;
	screenRow1 = 0; screenRow2 = CHR_ROWS-1; 
#else
	screenCol1 = 2; screenCol2 = CHR_COLS-2;
	screenRow1 = 2; screenRow2 = CHR_ROWS-3;
#endif
	screenWidth = screenCol2-screenCol1;
	screenHeight = screenRow2-screenRow1;
	InitCharmap();		
	ClearCharmap();
	LoadCharset("quedex.chr", charColors);
	LoadTileset("level1.tls", 36, 2, 2);
	LoadCharmap("level1.map", 64, 64);
	EnterCharmapMode();
	
	// Setup sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
#if defined __ATARI__
	EnableSprite(SPRITE_PLAYER0);
	EnableSprite(SPRITE_PLAYER1);
	EnableSprite(SPRITE_PLAYER2);
#elif defined __ORIC__
	EnableSprite(SPRITE_PLAYER);
	MultiColorSprite(SPRITE_PLAYER, multiColorDef);
#else
	EnableSprite(SPRITE_PLAYER);
#endif

	// Setup enemies
	enemies[0].state = ENEMY_GUARD;
	enemies[0].color = COLOR_SKELETON;
	enemies[0].frame = FRAME_SKELETON;
	enemies[0].mapX = 22;
	enemies[0].mapY = 22;
	enemies[1].state = ENEMY_GUARD;
	enemies[1].color = COLOR_SKELETON;
	enemies[1].frame = FRAME_SKELETON;
	enemies[1].mapX = 24;
	enemies[1].mapY = 24;
	enemies[2].state = ENEMY_GUARD;
	enemies[2].color = COLOR_GOBLIN;
	enemies[2].frame = FRAME_GOBLIN;
	enemies[2].mapX = 30;
	enemies[2].mapY = 30;

	// Show some text
#if (defined __ORIC__)
	SetAttributes(-1, CHR_ROWS-1, RED);
	SetAttributes( 4, CHR_ROWS-1, CYAN);
	SetAttributes( 9, CHR_ROWS-1, YELLOW);
	SetAttributes(CHR_COLS-5, CHR_ROWS-1, WHITE);
#endif	
	inkColor = RED;    PrintCharmap(0, CHR_ROWS-1, life);
	inkColor = CYAN;   PrintCharmap(5, CHR_ROWS-1, mana);
	inkColor = YELLOW; PrintCharmap(10,CHR_ROWS-1, gold);
	inkColor = WHITE;  PrintCharmap(CHR_COLS-4, CHR_ROWS-1, kill);
}

void GameLoop(void)
{
	unsigned char mapX =  92, mapY =   0, mapXPRV, mapYPRV; // Current and previous map position
	unsigned int  sprX = 200, sprY = 100, sprXPRV, sprYPRV; // Current and previous sprite position
	unsigned char maxX, maxY, flagX, flagY1, flagY2;  // Coords of collision detection (accounting for model height)
	unsigned char i, slot, joy, action, motion, direction;
	unsigned char sprFrame, weaFrame, enmFrame, togFrame; 
	unsigned int  weaX, weaY, enmX, enmY;
	signed int    deltaX, deltaY;
	clock_t gameClock;	
	Enemy* enemy;
	
	// Helper variables
	maxX = worldWidth-screenWidth;
	maxY = worldHeight-screenHeight;
	
	// Scroll to start position
	ScrollCharmap(mapX, mapY);
	
	// Restart music playback
#if (defined __ATARI__) || (defined __CBM__) || (defined __LYNX__)
	PlayMusic();
#endif	
	
	while (1) {
		// Platform specific
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
		
		// Check clock
		if (clock() < gameClock) {
		#if defined __APPLE2__
			wait(1);  // Manually update clock on Apple 2	
			clk += 6;			
		#endif
			continue;
		}
		gameClock = clock()+(TCK_PER_SEC/12);
		togFrame ^= 1;

		// Save previous location
		sprXPRV = sprX; sprYPRV = sprY;
		mapXPRV = mapX; mapYPRV = mapY;

		// Process Joystick
		motion = 0;
		joy = GetJoy(0);
		if (!(joy & JOY_BTN1)) {
			action = 1;
		} else {
			action = 0;
			if (!(joy & JOY_LEFT))  {
					 if (sprX>80)  sprX -= STEP_X; 
				else if (mapX>1)   mapX -= 1;
				else if (sprX>3)   sprX -= STEP_X;
				motion = 1; direction = 0;
			} else 
			if (!(joy & JOY_RIGHT)) { 
					 if (sprX<240)  sprX += STEP_X; 
				else if (mapX<maxX) mapX += 1;
				else if (sprX<317)  sprX += STEP_X; 
				motion = 1; direction = 2;
			} else
			if (!(joy & JOY_UP)) {
					 if (sprY>50)  sprY -= STEP_Y; 
				else if (mapY>1)   mapY -= 1;
				else if (sprY>2)   sprY -= STEP_Y; 
				motion = 1; direction = 4;
			} else
			if (!(joy & JOY_DOWN)) {
					 if (sprY<150)  sprY += STEP_Y; 
				else if (mapY<maxY) mapY += 1;
				else if (sprY<185)  sprY += STEP_Y;
				motion = 1; direction = 6;
			} 
		}	
		
		// Check if new position is allowed?
		LocatePixel(sprX, sprY);
		flagX = mapX+XToCol(pixelX)-screenCol1;
		flagY1 = mapY+YToRow(pixelY)-screenRow1;
		flagY2 = mapY+YToRow(pixelY+5)-screenRow1;
		if (!(GetCharFlags(flagX, flagY1) & CHAR_WALKABLE) ||
			!(GetCharFlags(flagX, flagY2) & CHAR_WALKABLE)) {
			sprX = sprXPRV; sprY = sprYPRV;
			mapX = mapXPRV; mapY = mapYPRV;
		} else {
			ScrollCharmap(mapX, mapY);
		}					
		
		// Update sprite
		LocateSprite(sprX, sprY);
		sprFrame = direction;
		if (motion)
			sprFrame += togFrame;
	#if defined __ATARI__
		SetSprite(SPRITE_PLAYER0, sprFrame);		
		SetSprite(SPRITE_PLAYER1, sprFrame+spriteFrames);		
		SetSprite(SPRITE_PLAYER2, sprFrame+spriteFrames*2);		
	#else
		SetSprite(SPRITE_PLAYER, sprFrame);		
	#endif		
	
		// Progress action
		if (action) {
			switch (direction) {
			case 0:
				weaX = sprX-12; weaY = sprY; weaFrame = 9; break;
			case 2:
				weaX = sprX+12; weaY = sprY; weaFrame = 11; break;
			case 4:
				weaX = sprX; weaY = sprY-14; weaFrame = 13; break;
			case 6:
				weaX = sprX; weaY = sprY+14; weaFrame = 15; break;
			}
			LocateSprite(weaX, weaY);
			SetSprite(SPRITE_WEAPON, weaFrame);	
			EnableSprite(SPRITE_WEAPON);			
		} else {
			DisableSprite(SPRITE_WEAPON);
		}
		
		// Progress enemies
		i = 0; slot = SPRITE_ENEMY;
		while (i<ENEMY_NUM && slot<SPRITE_NUM) {
			enemy = &enemies[i++];
			if (enemy->state && enemy->mapX > mapX && enemy->mapX < mapX+screenWidth
						     && enemy->mapX > mapY && enemy->mapY < mapY+screenHeight) {
				enmX = (enemy->mapX-mapX+screenCol1)*SCALE_X;
				enmY = (enemy->mapY-mapY+screenRow1)*SCALE_Y;
				deltaX  = sprX;  deltaY = sprY;
				deltaX -= enmX; deltaY -= enmY;
				if (ABS(deltaX) < 20 && ABS(deltaY) < 20) {
					if (sprX > enmX)
						enemy->offset = 0;
					else
						enemy->offset = 4;
					enmFrame = enemy->frame+enemy->offset+(togFrame+slot)%2u+2;					
				} else {
					enmFrame = enemy->frame+enemy->offset+(togFrame+slot)%2u;
			    }
				RecolorSprite(slot, 0, enemy->color); 
				LocateSprite(enmX, enmY);
				SetSprite(slot, enmFrame);	
				EnableSprite(slot++);
			}
		}
		while (slot<SPRITE_NUM)
			DisableSprite(slot++);		
	}	
}
	
int main (void)
{		
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	clrscr();

	// Load music track
	LoadMusic("dungeon.mus", MUSICRAM);
	
	// Run Game
	SplashScreen();
	GameInit();
	GameLoop();
		
    // Done
    return EXIT_SUCCESS;
}

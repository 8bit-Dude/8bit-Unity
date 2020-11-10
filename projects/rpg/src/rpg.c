
#include "unity.h"

// Memory tweaks
#if defined __ATARI__
	#undef  MUSICRAM
	#define MUSICRAM (0x9800)	// Moved Music RAM start to allow more space for sprites
#endif

// Sprite definitions
#if defined __APPLE2__
	#define spriteFrames 32
	#define spriteCols   7
	#define spriteRows   5
	unsigned char spriteColors[] = { };	//  Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	#define spriteFrames 32
	#define spriteCols   8
	#define spriteRows   18
	unsigned char spriteColors[] = {0x0c, 0x14, 0xbc, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // White (sword), Brown, Green, Pink (player), White (enemies) ...
#elif defined __ORIC__
	#define spriteFrames 32
	#define spriteCols   12
	#define spriteRows   6
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	// Matching more or less with above
#elif defined __CBM__
	#define spriteFrames 32
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { WHITE, YELLOW, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, RED, GREEN };  // 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	#define spriteFrames 32
	#define spriteCols   7
	#define spriteRows   9
	unsigned char* spriteColors = defaultColors;	// All sprites use the default palette
#endif

// Chracter Controller
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

// Sprite slots
#if defined __ATARI__	
	#define SPRITE_WEAPON   0
	#define SPRITE_PLAYER0  1
	#define SPRITE_PLAYER1  2
	#define SPRITE_PLAYER2  3
	#define SPRITE_ENEMY    4
	#define COLOR_SKELETON 0x0c
	#define COLOR_GOBLIN   0xbc
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

void gameInit(void)
{
	// Load charmap
	InitCharmap();		
	ClearCharmap();
	LoadCharset("charset1.dat");
	LoadCharmap("quedex.map");
	scrollHeight = CHR_ROWS-1;
	EnterCharmapMode();
	
	// Setup sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
#if defined __ATARI__
	EnableSprite(SPRITE_PLAYER0);
	EnableSprite(SPRITE_PLAYER1);
	EnableSprite(SPRITE_PLAYER2);
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

	// Start music
	LoadMusic("dungeon.mus", MUSICRAM);
	PlayMusic();

	// Show some text
	inkColor = RED;    PrintCharmap(0, CHR_ROWS-1,"H100");
	inkColor = CYAN;   PrintCharmap(5, CHR_ROWS-1,"M100");
	inkColor = YELLOW; PrintCharmap(10,CHR_ROWS-1,"$000");
}

void gameLoop(void)
{
	unsigned char mapXPRV, mapYPRV, mapX =  25, mapY =  28; // Previous and current map position
	unsigned int  sprXPRV, sprYPRV, sprX = 160, sprY = 100; // Backup and current sprite position
	unsigned char flagX, flagY1, flagY2;  // Coords of collision detection (accounting for model height)
	unsigned char i, slot, joy, action, motion, direction;
	unsigned char sprFrame, weaFrame, enmFrame; 
	unsigned int  weaX, weaY, enmX, enmY;
	signed int    deltaX, deltaY;
	clock_t gameClock;	
	Enemy* enemy;
	
	while (1) {
		// Check clock
		if (clock() <= gameClock+4) 
			continue;
		gameClock = clock();

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
					 if (sprX<240) sprX += STEP_X; 
				else if (mapX<(charmapWidth-41)) mapX += 1;
				else if (sprX<317) sprX += STEP_X; 
				motion = 1; direction = 2;
			} else
			if (!(joy & JOY_UP)) {
					 if (sprY>50)  sprY -= STEP_Y; 
				else if (mapY>1)   mapY -= 1;
				else if (sprY>2)   sprY -= STEP_Y; 
				sprFrame = 4 + gameClock%2;
				motion = 1; direction = 4;
			} else
			if (!(joy & JOY_DOWN)) {
					 if (sprY<150) sprY += STEP_Y; 
				else if (mapY<(charmapHeight-25)) mapY += 1;
				else if (sprY<185) sprY += STEP_Y;
				sprFrame = 6 + gameClock%2;
				motion = 1; direction = 6;
			} 
		}	
		
		// Check if new position is allowed?
		LocatePixel(sprX, sprY);
		flagX = mapX+XToCol(pixelX);
		flagY1 = mapY+YToRow(pixelY);
		flagY2 = mapY+YToRow(pixelY+5);
		if (!(GetCharFlags(flagX, flagY1) & CHAR_WALKABLE) ||
			!(GetCharFlags(flagX, flagY2) & CHAR_WALKABLE)) {
			sprX = sprXPRV; sprY = sprYPRV;
			mapX = mapXPRV; mapY = mapYPRV;
		} else {
			ScrollCharmap(mapX, mapY);
		}					
		
		// Update sprite
		LocateSprite(sprX, sprY);
		if (motion)
			sprFrame = gameClock%2;
		else
			sprFrame = 0;
		sprFrame += direction;
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
			if (enemy->state && enemy->mapX > mapX && enemy->mapX < mapX+39
						     && enemy->mapX > mapY && enemy->mapY < mapY+24) {
				enmX = (enemy->mapX-mapX)*SCALE_X;
				enmY = (enemy->mapY-mapY)*SCALE_Y;
				deltaX  = sprX;  deltaY = sprY;
				deltaX -= enmX; deltaY -= enmY;
				if (ABS(deltaX) < 20 && ABS(deltaY) < 20) {
					if (sprX > enmX)
						enemy->offset = 0;
					else
						enemy->offset = 4;
					enmFrame = enemy->frame+enemy->offset+(gameClock/2u+slot)%2+2;					
				} else {
					enmFrame = enemy->frame+enemy->offset+(gameClock/2u+slot)%2;
			    }
				RecolorSprite(slot, 0, enemy->color); 
				LocateSprite(enmX, enmY);
				SetSprite(slot, enmFrame);	
				EnableSprite(slot++);
			}
		}
		while (slot<SPRITE_NUM)
			DisableSprite(slot++);
		
		// Platform specific
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
	}	
}
	
int main (void)
{		
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	clrscr();
	
	// Run the game!
	gameInit();
	gameLoop();
		
    // Done
    return EXIT_SUCCESS;
}

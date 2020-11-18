
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

// Animation Key Frames
#define KEY_PLAYER     0
#define KEY_WEAPON     8
#define KEY_SKELETON  16
#define KEY_GOBLIN    24

// Animation Stance Frames
#define STANCE_RIGHT   0
#define STANCE_LEFT	   4


// Enemy data
# define ENEMY_NUM    2
# define ENEMY_NULL   0
# define ENEMY_GUARD  1
# define ENEMY_ATTACK 2
typedef struct {
	unsigned char state, color;
	unsigned char key, stance, frame;
	unsigned char mapX, mapY;
	unsigned int scrX, scrY;
	clock_t timer;
} Enemy;

Enemy enemies[ENEMY_NUM] = { { ENEMY_GUARD, COLOR_SKELETON, KEY_SKELETON, STANCE_LEFT,  255, 108, 20, 255, 255, 0 },
							 { ENEMY_GUARD, COLOR_GOBLIN,   KEY_GOBLIN,   STANCE_RIGHT, 255, 122, 21, 255, 255, 0 } };

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
	screenCol1 = 4; screenCol2 = CHR_COLS-4;
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
	LoadTileset("level1.tls", 40, 2, 2);
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
	unsigned int  scrX = 195, scrY =  50, scrXPRV, scrYPRV; // Current and previous screen position
	unsigned int  weaponX, weaponY;							// Weapon position
	unsigned char maxX, maxY, flagX, flagY1, flagY2;  // Coords of collision detection (accounting for model height)
	unsigned char i, j, slot, joy, action, motion, direction;
	unsigned char sprFrame, weaFrame, enmFrame, togglePlayer, toggleEnemy; 
	signed int    deltaX, deltaY;
	clock_t playerClock, enemyClock;	
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
	#if defined __APPLE2__
		clk += 1;	// Manually update clock on Apple 2			
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
		
		//////////////////////////
		// Check player clock
		if (clock() > playerClock) {
			playerClock = clock()+(TCK_PER_SEC/18);
			togglePlayer ^= 1;

			// Save previous location
			scrXPRV = scrX; scrYPRV = scrY;
			mapXPRV = mapX; mapYPRV = mapY;

			// Process Joystick
			motion = 0;
			joy = GetJoy(0);
			if (!(joy & JOY_BTN1)) {
				action = 1;
			} else {
				action = 0;
				if (!(joy & JOY_LEFT))  {
						 if (scrX>100) scrX -= STEP_X; 
					else if (mapX>1)   mapX -= 1;
					else if (scrX>3)   scrX -= STEP_X;
					motion = 1; direction = 0;
				} else 
				if (!(joy & JOY_RIGHT)) { 
						 if (scrX<220)  scrX += STEP_X; 
					else if (mapX<maxX) mapX += 1;
					else if (scrX<317)  scrX += STEP_X; 
					motion = 1; direction = 2;
				} else
				if (!(joy & JOY_UP)) {
						 if (scrY>60)  scrY -= STEP_Y; 
					else if (mapY>1)   mapY -= 1;
					else if (scrY>2)   scrY -= STEP_Y; 
					motion = 1; direction = 4;
				} else
				if (!(joy & JOY_DOWN)) {
						 if (scrY<125)  scrY += STEP_Y; 
					else if (mapY<maxY) mapY += 1;
					else if (scrY<185)  scrY += STEP_Y;
					motion = 1; direction = 6;
				} 
			}	
			
			// Check if new position is allowed?
			LocatePixel(scrX, scrY);
			flagX = mapX+XToCol(pixelX)-screenCol1;
			flagY1 = mapY+YToRow(pixelY)-screenRow1;
			flagY2 = mapY+YToRow(pixelY+5)-screenRow1;
			if (!(GetCharFlags(flagX, flagY1) & CHAR_WALKABLE) ||
				!(GetCharFlags(flagX, flagY2) & CHAR_WALKABLE)) {
				scrX = scrXPRV; scrY = scrYPRV;
				mapX = mapXPRV; mapY = mapYPRV;
			} else {
				ScrollCharmap(mapX, mapY);
			}					
			
			// Update sprite
			LocateSprite(scrX, scrY);
			sprFrame = direction;
			if (motion)
				sprFrame += togglePlayer;
		#if defined __ATARI__
			SetSprite(SPRITE_PLAYER0, sprFrame);		
			SetSprite(SPRITE_PLAYER1, sprFrame+spriteFrames);		
			SetSprite(SPRITE_PLAYER2, sprFrame+spriteFrames*2);		
		#else
			SetSprite(SPRITE_PLAYER, sprFrame);		
		#endif		
		
			// Process action
			if (action) {
				switch (direction) {
				case 0:
					weaponX = scrX-12; weaponY = scrY; weaFrame = 9; break;
				case 2:
					weaponX = scrX+12; weaponY = scrY; weaFrame = 11; break;
				case 4:
					weaponX = scrX; weaponY = scrY-14; weaFrame = 13; break;
				case 6:
					weaponX = scrX; weaponY = scrY+14; weaFrame = 15; break;
				}
				LocateSprite(weaponX, weaponY);
				SetSprite(SPRITE_WEAPON, weaFrame);	
				EnableSprite(SPRITE_WEAPON);			
			} else {
				DisableSprite(SPRITE_WEAPON);
			}

			// Display enemies
			i = 0; slot = SPRITE_ENEMY;
			while (i<ENEMY_NUM && slot<SPRITE_NUM) {
				enemy = &enemies[i++];
				if (enemy->frame == 255) 
					continue;					
				RecolorSprite(slot, 0, enemy->color); 
				LocateSprite(enemy->scrX, enemy->scrY);
				SetSprite(slot, enemy->frame);	
				EnableSprite(slot++);			
			}
			while (slot<SPRITE_NUM)
				DisableSprite(slot++);
		}

		//////////////////////////
		// Check enemy clock
		if (clock() > enemyClock) {
			enemyClock = clock()+(TCK_PER_SEC/6);
			toggleEnemy ^= 1;
		
			i = 0; slot = SPRITE_ENEMY;
			while (i<ENEMY_NUM && slot<SPRITE_NUM) {
				// Is valid?
				enemy = &enemies[i++];
				if (!enemy->state) 
					continue;			
				
				// Is visible?
				if (enemy->mapX <= mapX || enemy->mapX >= mapX+screenWidth ||
					enemy->mapY <= mapY || enemy->mapY >= mapY+screenHeight) {
					// Set NULL frame
					enemy->frame = 255;
				} else {
					// Update screen position
					enemy->scrX = (enemy->mapX-mapX+screenCol1)*SCALE_X;
					enemy->scrY = (enemy->mapY-mapY+screenRow1)*SCALE_Y;
					
					// Compute distance to player
					deltaX = scrX; deltaX -= enemy->scrX; 
					deltaY = scrY; deltaY -= enemy->scrY;
					
					// Has detected player?
					enemy->frame = enemy->key + enemy->stance + (toggleEnemy+slot)%2u;
					switch (enemy->state) {
					case ENEMY_GUARD:
						// Check if player within reach?
						if (ABS(deltaX) < 80 && ABS(deltaY) < 80)
							enemy->state = ENEMY_ATTACK;
						break;
						
					case ENEMY_ATTACK:
						// Check if player out of reach?
						if (ABS(deltaX) > 100 || ABS(deltaY) > 100) {
							enemy->state = ENEMY_GUARD;
							break;
						}
						
						// Set direction of frame
						if (scrX < enemy->scrX)
							enemy->stance = STANCE_LEFT;
						else
							enemy->stance = STANCE_RIGHT;

						// Check distance to player
						if (ABS(deltaX) > 18 || ABS(deltaY) > 18) {
							// Decide direction of motion
							if (ABS(deltaX) < ABS(deltaY))  {
								deltaX = 0; deltaY = SIGN(deltaY);
							} else {
								deltaY = 0; deltaX = SIGN(deltaX);
							}
													
							// Look for collisions with map
							if (GetCharFlags(enemy->mapX+deltaX, enemy->mapY+deltaX) & CHAR_WALKABLE) {
								// Look for collisions with other enemies
								motion = 1;
								for (j=0; j<ENEMY_NUM; j++) {
									if (i != j && enemies[j].frame != 255 && 
										enemies[j].mapX == (enemy->mapX+deltaX) &&
										enemies[j].mapY == (enemy->mapY+deltaY)) {
										motion = 0;
										break;
									}
								}
								
								// Update position!
								if (motion) {
									enemy->mapX += deltaX;
									enemy->mapY += deltaY;
								}
							}
							
						} else {
							// Check attack timer
							if (clock() > enemy->timer) {
								enemy->timer = clock() + TCK_PER_SEC;
								//BumpSFX();
							}
							enemy->frame += 2;								
						}
						break;
					}
				}
			}
		}	
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

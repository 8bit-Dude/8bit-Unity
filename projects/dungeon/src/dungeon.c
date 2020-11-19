
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
	unsigned char spriteColors[] = { 0x0c, 0x14, 0xbc, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // White (sword), Brown, Green, Pink (player)
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
#elif defined __ORIC__
	#define spriteFrames 32
	#define spriteCols   12
	#define spriteRows   17
	unsigned char charColors[] = {};    //  All colors are pre-assigned in the char sheet
	unsigned char spriteColors[] = { SPR_WHITE, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };	// White (sword), AIC (player), AIC (actors)
	unsigned char multiColorDef[] = { SPR_WHITE, 7, SPR_MAGENTA, 12 , SPR_YELLOW, 17 };	// Multicolor definition { color, row, ...  }
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
# define ACTOR_NUM    6
# define ACTOR_NULL   0
# define ACTOR_GUARD  1
# define ACTOR_ATTACK 2
# define ACTOR_GOLD   3
# define ACTOR_POTION 4
# define ACTOR_ARMOR  5
# define ACTOR_KEY    6
typedef struct {
	unsigned char state, color;
	unsigned char key, stance, frame;
	unsigned char mapX, mapY;
	unsigned int scrX, scrY;
	clock_t timer;
} Actor;

Actor actors[ACTOR_NUM] = { { ACTOR_GUARD,  COLOR_SKELETON, KEY_SKELETON, STANCE_LEFT,  255, 108, 20, 255, 255, 0 },
							{ ACTOR_GUARD,  COLOR_GOBLIN,   KEY_GOBLIN,   STANCE_RIGHT, 255, 122, 21, 255, 255, 0 },
							{ ACTOR_GOLD,   COLOR_GOLD,     KEY_GOLD,     0,            255, 118, 18, 255, 255, 0 },
							{ ACTOR_POTION, COLOR_POTION,   KEY_POTION,   0,            255, 104, 16, 255, 255, 0 },
							{ ACTOR_ARMOR,  COLOR_ARMOR,    KEY_ARMOR,    0,            255, 122, 15, 255, 255, 0 },
							{ ACTOR_KEY,    COLOR_KEY,      KEY_KEY,      0,            255, 112, 13, 255, 255, 0 } };

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
	inkColor = RED;    PrintStr(0, CHR_ROWS-1, life);
	inkColor = CYAN;   PrintStr(5, CHR_ROWS-1, mana);
	inkColor = YELLOW; PrintStr(10,CHR_ROWS-1, gold);
	inkColor = WHITE;  PrintStr(CHR_COLS-4, CHR_ROWS-1, kill);
}

void GameLoop(void)
{
	unsigned char mapX =  92, mapY =  0, mapXPRV, mapYPRV; // Current and previous map position
	unsigned int  scrX = 195, scrY = 50, scrXPRV, scrYPRV; // Current and previous screen position
	unsigned int  weaponX, weaponY, weaponActive;    		// Weapon position
	unsigned char maxX, maxY, flagX, flagY1, flagY2;  // Coords of collision detection (accounting for model height)
	unsigned char i, j, slot, joy, action, motion, direction, tmpX, tmpY;
	unsigned char sprFrame, weaFrame, togglePlayer, toggleActor; 
	signed int    deltaX, deltaY;
	clock_t playerClock, actorClock;	
	Actor* actor;
	
	// Helper variables
	maxX = worldWidth-screenWidth;
	maxY = worldHeight-screenHeight;
	
	// Scroll to start position
	ScrollCharmap(mapX, mapY);
	
	// Restart music playback
#if (defined __ATARI__) || (defined __CBM__) || (defined __LYNX__)
	//PlayMusic();
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
				// Process attack
				if (!weaponActive) {
					i = 0; slot = SPRITE_ENEMY;
					//while (i<ACTOR_NUM) { 					
						weaponActive = 1;
					//}
				}
				
				// Display weapon
				switch (direction) {
				case 0:
					weaponX = scrX-12; weaponY = scrY; weaFrame = 8; break;
				case 2:
					weaponX = scrX+12; weaponY = scrY; weaFrame = 9; break;
				case 4:
					weaponX = scrX; weaponY = scrY-14; weaFrame = 10; break;
				case 6:
					weaponX = scrX; weaponY = scrY+14; weaFrame = 11; break;
				}
				LocateSprite(weaponX, weaponY);
				SetSprite(SPRITE_WEAPON, weaFrame);	
				EnableSprite(SPRITE_WEAPON);			
			} else {
				weaponActive = 0;
				DisableSprite(SPRITE_WEAPON);
			}

			// Display actors
			i = 0; slot = SPRITE_ENEMY;
			while (i<ACTOR_NUM && slot<SPRITE_NUM) {
				actor = &actors[i++];
				if (actor->frame == 255) 
					continue;					
				RecolorSprite(slot, 0, actor->color); 
				LocateSprite(actor->scrX, actor->scrY);
				SetSprite(slot, actor->frame);	
				EnableSprite(slot++);			
			}
			while (slot<SPRITE_NUM)
				DisableSprite(slot++);
		}

		//////////////////////////
		// Check actor clock
		if (clock() > actorClock) {
			actorClock = clock()+(TCK_PER_SEC/6);
			toggleActor ^= 1;
		
			i = 0; slot = SPRITE_ENEMY;
			while (i<ACTOR_NUM && slot<SPRITE_NUM) {
				// Is valid?
				actor = &actors[i++];
				if (!actor->state) 
					continue;			
				
				// Is visible?
				if (actor->mapX <= mapX || actor->mapX >= mapX+screenWidth ||
					actor->mapY <= mapY || actor->mapY >= mapY+screenHeight) {
					// Set NULL frame
					actor->frame = 255;
				} else {
					// Update screen position
					actor->scrX = (actor->mapX-mapX+screenCol1)*SCALE_X;
					actor->scrY = (actor->mapY-mapY+screenRow1)*SCALE_Y;
					
					// Compute distance to player
					deltaX = scrX; deltaX -= actor->scrX; 
					deltaY = scrY; deltaY -= actor->scrY;
					
					// Setup base frame
					actor->frame = actor->key + actor->stance;
					if (actor-> state <= ACTOR_ATTACK)
						actor->frame += (toggleActor+slot)%2u;
					 
					// Has detected player?
					switch (actor->state) {
					case ACTOR_GUARD:
						// Check if player within reach?
						if (ABS(deltaX) < 80 && ABS(deltaY) < 80)
							actor->state = ACTOR_ATTACK;
						break;
						
					case ACTOR_ATTACK:
						// Check if player out of reach?
						if (ABS(deltaX) > 100 || ABS(deltaY) > 100) {
							actor->state = ACTOR_GUARD;
							break;
						}
						
						// Set direction of frame
						if (scrX < actor->scrX)
							actor->stance = STANCE_LEFT;
						else
							actor->stance = STANCE_RIGHT;

						// Check distance to player
						if (ABS(deltaX) > 18 || ABS(deltaY) > 18) {
							// Decide direction of motion
							if (ABS(deltaX) < ABS(deltaY))  {
								deltaX = 0; deltaY = SIGN(deltaY);
							} else {
								deltaY = 0; deltaX = SIGN(deltaX);
							}
													
							// Look for collisions with map
							tmpX = actor->mapX+deltaX;
							tmpY = actor->mapY+deltaY;
							if (GetCharFlags(tmpX, tmpY) & CHAR_WALKABLE) {
								// Look for collisions with other actors
								motion = 1;
								for (j=0; j<ACTOR_NUM; j++) {
									if ( i != j && actors[j].frame != 255 && actors[j].state <= ACTOR_ATTACK && 
										 actors[j].mapX == tmpX && actors[j].mapY == tmpY ) {
										motion = 0;
										break;
									}
								}
								
								// Update position!
								if (motion) {
									actor->mapX += deltaX;
									actor->mapY += deltaY;
								}
							}
							
						} else {
							// Check attack timer
							if (clock() > actor->timer) {
								actor->timer = clock() + TCK_PER_SEC;
								//BumpSFX();
							}
							actor->frame += 2;								
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
	//LoadMusic("dungeon.mus", MUSICRAM);
	
	// Run Game
	//SplashScreen();
	GameInit();
	GameLoop();
		
    // Done
    return EXIT_SUCCESS;
}

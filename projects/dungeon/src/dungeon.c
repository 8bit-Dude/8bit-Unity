
#include "unity.h"
#include "definitions.h"

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

// Actor variables (Enemies/Items)
typedef struct {
	unsigned char state, color, health;
	unsigned char key, stance, frame;
	unsigned char mapX, mapY;
	unsigned int scrX, scrY;
	clock_t timer;
} Actor;

Actor actors[ACTOR_NUM] = { { ACTOR_GUARD,  COLOR_SKELETON, 15, KEY_SKELETON, STANCE_LEFT,  255, 108, 20, 255, 255, 0 },
							{ ACTOR_GUARD,  COLOR_SKELETON, 15, KEY_SKELETON, STANCE_RIGHT, 255, 122, 21, 255, 255, 0 },
							{ ACTOR_GUARD,  COLOR_SKELETON, 15, KEY_SKELETON, STANCE_LEFT,  255, 118, 18, 255, 255, 0 },
							{ ACTOR_GUARD,  COLOR_GOBLIN,   10, KEY_GOBLIN,   STANCE_RIGHT, 255, 104, 16, 255, 255, 0 },
							{ ACTOR_GUARD,  COLOR_GOBLIN,   10, KEY_GOBLIN,   STANCE_LEFT,  255, 122, 15, 255, 255, 0 },
							{ ACTOR_GUARD,  COLOR_GOBLIN,   10, KEY_GOBLIN,   STANCE_RIGHT, 255, 112, 13, 255, 255, 0 } };

// Player variables
unsigned char health = 100;
unsigned char armor  = 0;
unsigned char gold   = 0;
unsigned char kills  = 0;

void DamageEnemy(Actor* actor) 
{
	unsigned char reward;

	// Apply damage and check if killed
	BumpSFX();
	actor->health -= 5;
	if (actor->health)
		return;
	kills += 1; PrintKills();		
	
	// Generate reward
	reward = rand()%4;
	if (reward<3) { 
		actor->state  = ACTOR_GOLD+reward;
		actor->key    = KEY_GOLD+reward;
		actor->color  = COLOR_GOLD;
		actor->stance = 0;
	} else {
		actor->state = ACTOR_NULL;
		actor->frame = 255;
	}
}

void PickupItem(Actor* actor)
{
	// Process reward
	BleepSFX(128);
	switch (actor->state) {
	case ACTOR_GOLD:
		gold += 5*(rand()%5);
		PrintGold();
		break;
	case ACTOR_POTION:
		health += 25;
		PrintHealth();
		break;
	case ACTOR_ARMOR:
		armor += 25;
		PrintArmor();
		break;
	case ACTOR_KEY:
		PrintKey();
		break;
	}

	// Disable actor
	actor->state = ACTOR_NULL;
	actor->frame = 255;	
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

	// Show stats
#if (defined __ORIC__)
	SetAttributes(-1, CHR_ROWS-1, RED);
	SetAttributes( 4, CHR_ROWS-1, CYAN);
	SetAttributes(CHR_COLS-10, CHR_ROWS-1, YELLOW);
	SetAttributes(CHR_COLS-5,  CHR_ROWS-1, WHITE);
#endif	
	PrintHealth(); PrintArmor();
	PrintGold(); PrintKills();
	
	// Setup SFX
	InitSFX();
}

void GameLoop(void)
{
	unsigned char mapX =  92, mapY =  0, mapXPRV, mapYPRV; // Current and previous map position
	unsigned int  scrX = 195, scrY = 50, scrXPRV, scrYPRV; // Current and previous screen position
	unsigned int  weaponX, weaponY;
	unsigned char maxX, maxY, flagX, flagY1, flagY2;  // Coords of collision detection (accounting for model height)
	unsigned char i, j, slot, joy, action, motion, direction, collision, tmpX, tmpY;
	unsigned char sprFrame, weaponFrame, togglePlayer, toggleActor, weaponProcessed; 
	signed int    deltaX, deltaY;
	clock_t playerClock, weaponClock, actorClock;	
	Actor* actor;
	
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
			if (!(joy & JOY_BTN1) || clock() < weaponClock) {
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

			// Check if colliding with other actors?
			i = 0; collision = 0;
			while (i<ACTOR_NUM) { 					
				actor = &actors[i++];
				if (actor->frame == 255) 	
					continue;
				deltaX = scrX; deltaX -= actor->scrX;
				deltaY = scrY; deltaY -= actor->scrY;
				if (ABS(deltaX) < 10 && ABS(deltaY) < 10) {
					if  (actor->state > ACTOR_ATTACK)
						PickupItem(actor);
					else
						collision = 1;
				}
			}			
			
			// Check if new position is allowed?
			LocatePixel(scrX, scrY);
			flagX = mapX+XToCol(pixelX)-screenCol1;
			flagY1 = mapY+YToRow(pixelY)-screenRow1;
			flagY2 = mapY+YToRow(pixelY+5)-screenRow1;
			if (!(GetCharFlags(flagX, flagY1) & CHAR_WALKABLE) || !(GetCharFlags(flagX, flagY2) & CHAR_WALKABLE))
				collision = 1;
			
			// Allow motion?
			if (collision) {
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
				// Locate weapon
				switch (direction) {
				case 0:
					weaponX = scrX-12; weaponY = scrY; weaponFrame = 8; break;
				case 2:
					weaponX = scrX+12; weaponY = scrY; weaponFrame = 9; break;
				case 4:
					weaponX = scrX; weaponY = scrY-14; weaponFrame = 10; break;
				case 6:
					weaponX = scrX; weaponY = scrY+14; weaponFrame = 11; break;
				}
				
				// Process attack
				if (!weaponProcessed) {
					weaponClock = clock()+(TCK_PER_SEC/4);
					weaponProcessed = 1;
					
					// Look for Enemy Hit
					i = 0;
					while (i<ACTOR_NUM) { 					
						actor = &actors[i++];
						if (actor->frame == 255 || actor->state > ACTOR_ATTACK) 	
							continue;
						deltaX = weaponX; deltaX -= actor->scrX;
						deltaY = weaponY; deltaY -= actor->scrY;
						if (ABS(deltaX) < 16 && ABS(deltaY) < 16) {
							DamageEnemy(actor); 
							break;
						}
					}
					
					// Look for Map Interactions

				}
				
				// Display weapon
				LocateSprite(weaponX, weaponY);
				SetSprite(SPRITE_WEAPON, weaponFrame);	
				EnableSprite(SPRITE_WEAPON);			
			} else {
				// Hide weapon
				weaponProcessed = 0;
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
						if (ABS(deltaX) > 20 || ABS(deltaY) > 20) {
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
								if (health >= 5) {
									health -= 5; 
									PrintHealth();
									//BumpSFX();
								}
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
	LoadMusic("dungeon.mus", MUSICRAM);
	
	// Run Game
	//SplashScreen();
	GameInit();
	GameLoop();
		
    // Done
    return EXIT_SUCCESS;
}

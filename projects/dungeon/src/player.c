
#include "definitions.h"

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

// Player stats
unsigned char health = 100;
unsigned char armor  = 0;
unsigned char gold   = 0;
unsigned char kills  = 0;

// Player controls
unsigned char joy, action, motion, direction, tile;
unsigned char mapX, mapY, mapXPRV, mapYPRV; // Current and previous map position
unsigned int  scrX, scrY, scrXPRV, scrYPRV; // Current and previous screen position of player
unsigned char flagX, flagY1, flagY2;  // Coords of collision detection (accounting for model height)
unsigned char togglePlayer, playerFrame, weaponFrame, weaponProcessed; 
unsigned int  weaponX, weaponY;
clock_t		  weaponClock;

// See actors.c
extern Actor* selActor;

// Map extent
extern unsigned char worldMaxX, worldMaxY;

void ProcessPlayer()
{
	unsigned char collision = 0;
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
			else if (mapX<worldMaxX) mapX += 1;
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
			else if (mapY<worldMaxY) mapY += 1;
			else if (scrY<185)  scrY += STEP_Y;
			motion = 1; direction = 6;
		} 
	}	

	// Check if colliding with other actors?
	if (CheckActorCollision(scrX, scrY, ACTOR_MAX)) {
		if (selActor->state > ACTOR_ATTACK)
			PickupReward(selActor);
		else
			collision = 1;
	}
				
	// Check if new position is allowed?
	flagX = mapX+(scrX*CMP_COLS)/320u-screenCol1;
	flagY1 = mapY+(scrY*CMP_ROWS)/200u-screenRow1;
	flagY2 = mapY+((scrY+5)*CMP_ROWS)/200u-screenRow1;
	if (GetFlag(flagX, flagY1) || GetFlag(flagX, flagY2))
		collision = 1;
	
	// Allow motion?
	if (collision) {
		scrX = scrXPRV; scrY = scrYPRV;
		mapX = mapXPRV; mapY = mapYPRV;
	} else {
	#if defined(__APPLE2__) || defined(__ORIC__)
		if (mapX/2u != mapXPRV/2u || mapY/2u != mapYPRV/2u) {
	#else
		if (mapX != mapXPRV || mapY != mapYPRV) {
	#endif
			ScrollCharmap(mapX, mapY);
		}
	}					
	
	// Update sprite
	LocateSprite(scrX, scrY);
	playerFrame = direction;
	if (motion)
		playerFrame += togglePlayer;
#if defined __ATARI__
	SetMultiColorSprite(SPRITE_PLAYER, playerFrame);		
#else
	SetSprite(SPRITE_PLAYER, playerFrame);		
#endif		

	// Weapon
	if (action) {		
		// Process and Display weapon
		ProcessWeapon();
		LocateSprite(weaponX, weaponY);
		SetSprite(SPRITE_WEAPON, weaponFrame);	
		EnableSprite(SPRITE_WEAPON);			
	} else {
		// Hide weapon
		weaponProcessed = 0;
		DisableSprite(SPRITE_WEAPON);
	}	
}

void TriggerAction()
{
	DrawCharmap(mapX, mapY);
	PlaySFX(SFX_BUMP, 16, 120, 2);	
}

void ProcessWeapon()
{
	// Locate weapon
	switch (direction) {
	case 0:
		weaponX = scrX-12; weaponY = scrY; weaponFrame = KEY_WEAPON; break;
	case 2:
		weaponX = scrX+12; weaponY = scrY; weaponFrame = KEY_WEAPON+1; break;
	case 4:
		weaponX = scrX; weaponY = scrY-14; weaponFrame = KEY_WEAPON+2; break;
	case 6:
		weaponX = scrX; weaponY = scrY+14; weaponFrame = KEY_WEAPON+3; break;
	}
	
	// Process attack
	if (!weaponProcessed) {
		// Lock weapon
		weaponClock = clock()+(TCK_PER_SEC/4);
		weaponProcessed = 1;
		
		// Look for Enemy Hit
		if (CheckActorCollision(weaponX, weaponY, ACTOR_ATTACK)) {
			DamageMonster(selActor);
		} else {
			// Look for Map Interactions
			flagX = mapX+(weaponX*CMP_COLS)/320u-screenCol1;
			flagY1 = mapY+(weaponY*CMP_ROWS)/200u-screenRow1;
			switch (GetFlag(flagX, flagY1)) {
			case FLAG_BARREL:
				GenerateReward(flagX, flagY1);
				SetTile(flagX, flagY1, TILE_FLOOR);
				TriggerAction();
				break;						
			case FLAG_TOMB:
				tile = GetTile(flagX, flagY1);
				if (tile == TILE_TOMB_SHUTL) {
					SetTile(flagX, flagY1, TILE_TOMB_OPENL);
					SetTile(flagX+2, flagY1, TILE_TOMB_OPENR);
					GenerateMonster(flagX+4, flagY1);
				} else 
				if (tile == TILE_TOMB_SHUTR) {
					SetTile(flagX-2, flagY1, TILE_TOMB_OPENL);
					SetTile(flagX, flagY1, TILE_TOMB_OPENR);
					GenerateMonster(flagX-4, flagY1);
				}
				TriggerAction();
				break;						
			case FLAG_GATE:
				tile = GetTile(flagX, flagY1);
				if (tile == TILE_GATE_SHUTL) {
					SetTile(flagX, flagY1, TILE_FLOOR);
					SetTile(flagX+2, flagY1, TILE_FLOOR);
				} else 
				if (tile == TILE_GATE_SHUTR) {
					SetTile(flagX-2, flagY1, TILE_FLOOR);
					SetTile(flagX, flagY1, TILE_FLOOR);
				}
				TriggerAction();
				break;						
			}	
		}
	}
}		

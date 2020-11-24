
#include "definitions.h"

// List of actors and PTR to selected one
Actor actors[ACTOR_NUM];
Actor* selActor;

// Animation frame toggle
unsigned char toggleActor;

// Variables for position checks, etc...
unsigned char i, j, slot;
signed int deltaX, deltaY, tmpX, tmpY;

// See player.c
extern unsigned char mapX, mapY, scrX, scrY;
extern unsigned char health, armor, gold, kills;

void LoadActors(const char* filename)
{	
	// Read nav file
	unsigned char offset, i=0;
#if (defined __APPLE2__)
	unsigned char navBuffer[128];
	FILE* fp = fopen(filename, "rb");
	fread(navBuffer, 1, 128, fp);
	fclose(fp);
#elif (defined __ATARI__)
	unsigned char navBuffer[128];
	if (FileOpen(filename))
		FileRead(navBuffer, 128);
#elif (defined __CBM__)
	unsigned char navBuffer[128];
	FILE* fp = fopen(filename, "rb");
	fread(navBuffer, 1, 128, fp);
	fclose(fp);
#elif (defined __LYNX__)
	unsigned char* navBuffer = (char*)SHAREDRAM;
	bzero(SHAREDRAM, 128);
	FileRead(filename);
#elif (defined __ORIC__)
	unsigned char navBuffer[128];
	FileRead(filename, navBuffer);
#endif	

	// Reset actor memory
	memset(actors, 255, ACTOR_MAX*16);

	// Decode start position
	mapX = 2*navBuffer[0];
	offset = MIN(mapX, screenWidth/2); 
	scrX = 8*(CROP_X+offset);
	mapX -= offset;
		
	mapY = 2*navBuffer[1];
	offset = MIN(mapY, screenHeight/2);
	scrY = 8*(CROP_Y+offset);
	mapY -= offset;
	
	// Decode actors
	offset = navBuffer[4];
	while (i<offset) {
		selActor = &actors[i]; i++;
		switch (navBuffer[5+i*5]) {
		case 0:
			selActor->state = ACTOR_KEY;
			selActor->color = COLOR_KEY;
			selActor->key   = KEY_KEY;
			break;
		case 1:
			selActor->state = ACTOR_GUARD;
			selActor->color = COLOR_GOBLIN;
			selActor->key   = KEY_GOBLIN;
			break;
		case 2:
			selActor->state = ACTOR_GUARD;
			selActor->color = COLOR_SKELETON;
			selActor->key   = KEY_SKELETON;
			break;		
		}
		switch (navBuffer[6+i*5]) {
		case 0:
			selActor->stance = STANCE_LEFT;
			break;
		case 1:
			selActor->stance = STANCE_RIGHT;
			break;
		}
		selActor->health = navBuffer[7+i*5];
		selActor->mapX = 2*navBuffer[8+i*5];
		selActor->mapY = 2*navBuffer[9+i*5];
	}
}

void ProcessActors()
{
	unsigned char collision;
	toggleActor ^= 1;

	// Process Actor Movements and Attacks
	i = 0; slot = SPRITE_ENEMY;
	while (i<ACTOR_NUM && slot<SPRITE_NUM) {
		// Is valid?
		selActor = &actors[i++];
		if (!selActor->state) 
			continue;			
		
		// Is visible?
		if (selActor->mapX <= mapX || selActor->mapX >= mapX+screenWidth ||
			selActor->mapY <= mapY || selActor->mapY >= mapY+screenHeight) {
			// Set NULL frame
			selActor->frame = 255;
		} else {
			// Update screen position
			selActor->scrX = (selActor->mapX-mapX+screenCol1)*SCALE_X;
			selActor->scrY = (selActor->mapY-mapY+screenRow1)*SCALE_Y;
			
			// Compute distance to player
			deltaX = scrX; deltaX -= selActor->scrX; 
			deltaY = scrY; deltaY -= selActor->scrY;
			
			// Setup base frame
			selActor->frame = selActor->key + selActor->stance;
			if (selActor-> state <= ACTOR_ATTACK)
				selActor->frame += (toggleActor+slot)%2u;
			 
			// Has detected player?
			switch (selActor->state) {
			case ACTOR_GUARD:
				// Check if player within reach?
				if (ABS(deltaX) < 80 && ABS(deltaY) < 80)
					selActor->state = ACTOR_ATTACK;
				break;
				
			case ACTOR_ATTACK:
				// Check if player out of reach?
				if (ABS(deltaX) > 100 || ABS(deltaY) > 100) {
					selActor->state = ACTOR_GUARD;
					break;
				}
				
				// Set direction of frame
				if (scrX < selActor->scrX)
					selActor->stance = STANCE_LEFT;
				else
					selActor->stance = STANCE_RIGHT;

				// Check distance to player
				if (ABS(deltaX) > 20 || ABS(deltaY) > 20) {
					// Decide direction of motion
					if (ABS(deltaX) < ABS(deltaY))  {
						deltaX = 0; deltaY = SIGN(deltaY);
					} else {
						deltaY = 0; deltaX = SIGN(deltaX);
					}
											
					// Look for collisions with map
					tmpX = selActor->mapX+deltaX;
					tmpY = selActor->mapY+deltaY;
					if (!GetFlag(tmpX, tmpY)) {
						// Look for collisions with other actors
						collision = 0;
						for (j=0; j<ACTOR_NUM; j++) {
							if (i != j && actors[j].frame != 255 && actors[j].state <= ACTOR_ATTACK && actors[j].mapX == tmpX && actors[j].mapY == tmpY ) {
								collision = 1;
								break;
							}
						}
						
						// Update position!
						if (!collision) {
							selActor->mapX += deltaX;
							selActor->mapY += deltaY;
						}
					}
					
				} else {
					// Check attack timer
					if (clock() > selActor->timer) {
						selActor->timer = clock() + TCK_PER_SEC;
						if (health >= 5) {
							health -= 5; 
							PrintHealth();
							//BumpSFX();
						}
					}
					selActor->frame += 2;								
				}
				break;
			}
			
			// Assign sprite slot
			RecolorSprite(slot, 0, selActor->color); 
			LocateSprite(selActor->scrX, selActor->scrY);
			SetSprite(slot, selActor->frame);	
			EnableSprite(slot++);				
		}
	}
	
	// Reset unused sprite slots		
	while (slot<SPRITE_NUM)
		DisableSprite(slot++);	
}

unsigned char CheckActorCollision(unsigned int scrX, unsigned int scrY, unsigned char flag)
{
	unsigned char i = 0;
	while (i<ACTOR_NUM) { 					
		selActor = &actors[i++];
		if (selActor->frame == 255 || selActor->state > flag) 	
			continue;
		deltaX = scrX; deltaX -= selActor->scrX;
		deltaY = scrY; deltaY -= selActor->scrY;
		if (ABS(deltaX) < 16 && ABS(deltaY) < 16)
			return 1;
	}
	return 0;
}

unsigned char FindActorSlot()
{
	// Find slot
	unsigned char i = 0;
	while (actors[i].state != ACTOR_NULL) {
		if (++i == ACTOR_NUM)
			return 0;
	}
	selActor = &actors[i];	
	return 1;
}

void GenerateMonster(unsigned char mapX, unsigned char mapY)
{
	unsigned char monster;
	
	// Find slot
	if (!FindActorSlot()) 
		return;		
	
	// Assign monster
	monster = rand()%2;
	switch (monster) {
	case 0:
		selActor->key    = KEY_SKELETON;
		selActor->color  = COLOR_SKELETON;
		selActor->health  = 15;
		break;
	case 1:
		selActor->key    = KEY_GOBLIN;
		selActor->color  = COLOR_GOBLIN;
		selActor->health  = 10;
		break;
	}	
	selActor->stance = STANCE_LEFT;
	selActor->state  = ACTOR_GUARD;		
	selActor->mapX	  = mapX;
	selActor->mapY	  = mapY;
}


void GenerateReward(unsigned char mapX, unsigned char mapY)
{
	unsigned char reward;
	
	// Find slot
	if (!FindActorSlot()) 
		return;	
	
	// Assign reward
	reward = rand()%4;
	if (reward<3) { 
		selActor->state  = ACTOR_GOLD+reward;
		selActor->key    = KEY_GOLD+reward;
		selActor->color  = COLOR_GOLD;
		selActor->stance = 0;
		selActor->mapX	  = mapX;
		selActor->mapY	  = mapY;
	}	
}

void DamageMonster(Actor* actor) 
{
	// Apply damage and check if killed
	BumpSFX();
	selActor->health -= 5;
	if (selActor->health)
		return;
	
	// Kill unit and generate reward
	selActor->state = ACTOR_NULL;
	selActor->frame = 255;
	kills += 1; PrintKills();
	GenerateReward(selActor->mapX, selActor->mapY);
}

void PickupReward(Actor* actor)
{
	// Process reward
	BleepSFX(128);
	switch (selActor->state) {
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
	selActor->state = ACTOR_NULL;
	selActor->frame = 255;	
}	

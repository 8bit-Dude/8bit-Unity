
#include "definitions.h"

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
  #pragma bss-name(push, "XRAM")  
#endif

// Game state: Position of unit and mouse
unsigned int mouseX, mouseY, unitX, unitY, wayX[MAX_WAYPOINTS], wayY[MAX_WAYPOINTS];
unsigned char numWay, *mouse, mouseL = 0, mouseAction = 0, currmusic[13];
unsigned char unitFrame = frameWaitLeft, waitFrame = frameWaitLeft;		

// Game state: Actions consist of tuplets (Scene ID, Action ID (lower 6bits) + Type (higher 2bits))
unsigned char actionLast, itemLast, itemOffset;
unsigned char gameActions[0x100];
unsigned char gameItems[MAX_ITEM*LEN_ITEM];
unsigned char gameScene[8];

// DO NOT CHANGE ORDER: This data is loaded sequentially from file!!!!!
unsigned char sceneID, music;
unsigned int startx[MAX_PLAYERS], starty[MAX_PLAYERS];
unsigned char num_polygon, num_animations, num_interacts, num_triggers, num_modifiers, num_paths;
signed int polygonX[MAX_POLYGON];
signed int polygonY[MAX_POLYGON];
Animation animations[MAX_ANIMATION]; 
Interact interacts[MAX_INTERACT]; 
Trigger triggers[MAX_TRIGGER];
Modifier modifiers[MAX_MODIFIER];
Path paths[MAX_PATH];
unsigned char strings[0x400];

// Chunks for scene animation
unsigned char  num_chunks;
unsigned char *chunkData[MAX_CHUNK];
unsigned char *chunkBckg[MAX_CHUNK];

#ifdef __NES__
  #pragma bss-name(pop)
#endif

// Fast tables for cos/sin/tan
const signed char cos[16] = {16,14,11,6,0,-6,-11,-14,-16,-14,-11,-6,0,6,11,14};
const signed char sin[16] = {0,6,11,14,16,14,11,6,0,-6,-11,-14,-16,-14,-11,-6};
const signed char tan[19] = {91,30,17,11,8,6,4,2,1,-1,-2,-4,-6,-8,-11,-17,-30,-91,-128};

// Fast atan2fast routine with 36 x 10 degs increments
unsigned char atan2fast(signed int dY, signed int dX)
{
	unsigned char j = 0;
	signed char tn;
	
	if (dY == 0) {
		 // Select value directly (Cannot divide by 0)
		if (dX >= 0)
			return 0;
		else
			return 8;
	} else {
		// Use simple division and check range
		dX = (8*dX)/dY;
		if (dX < -128)
			tn = -128;
		else 
		if (dX > 127)
			tn = 127;
		else
			tn = dX;
			
		// Check against reference table
		while (tn < tan[j])
			j++;
		
		// Compute angle
		dX = j*10;
		if (dY < 0)
			dX += 180;
		return (dX/23u);
	}	
}
	  
// Draw chunk to screen
void DrawChunk(unsigned char id)
{
	unsigned char* chunk = chunkData[id];
	SetChunk(chunk, chunk[0], chunk[1]);	
}

// Restore background to screen
void DrawBackg(unsigned char id)
{
	unsigned char* chunk = chunkBckg[id];
	SetChunk(chunk, chunk[0], chunk[1]);	
}

// Wait function (used in animations)
void Wait(unsigned char ticks) 
{	
	clock_t animClock = clock();
#ifdef __ORIC__
	ticks *= 2;	// Clock runs faster on the Oric
#endif
	while (clock()<animClock+ticks) { 
	#if defined(__APPLE2__)
		wait(1); clk += 2;  // Manually update clock on Apple 2
	#endif
	}
}

// Search for interactable object under the mouse cursor
unsigned char SearchScene(unsigned int searchX, unsigned int searchY) 
{
	unsigned char i;
	signed int deltaX, deltaY, radius;
	Interact* interact;
	for (i=0; i<num_interacts; i++) {
		interact = &interacts[i];
		if (!interact->flags) continue;
		deltaX = (searchX - interact->x)/2;
		deltaY = (searchY - interact->y)/2;
		radius = interact->r/2;
		if ( (deltaX * deltaX + deltaY * deltaY) < (radius * radius) ) {
			return i;
		}
	}
	return 255;
}

// Process modifier to interactable
void ProcessModifier(unsigned char index)
{
	unsigned char i;
	Interact *modified;
	Modifier *modifier = &modifiers[index];
	for (i=0; i<num_interacts; i++) {
		modified = &interacts[i];
		if (modifier->label == modified->label) {
			modified->flags    = modifier->flags;
			modified->chk      = modifier->chk;
			modified->bcg      = modifier->bcg;
			modified->path     = modifier->path;
			modified->question = modifier->question;
			modified->answer   = modifier->answer;
			break;
		}
	}	
}

// Process player path
void ProcessPath(unsigned char index)
{
	unsigned char d, i;
	signed int deltaX, deltaY;
	Path *path = &paths[index];
	while (1) {
		deltaX = path->x - unitX;
		deltaY = path->y - unitY;
		d = abs(deltaX) + abs(deltaY);
		i = path->speed;
		while (i < d) {
			DrawUnit(unitX+(deltaX*i)/d, unitY+(deltaY*i)/d, path->frame);
			i += path->speed;
			Wait(10);
		}
		if (unitX || unitY) {
			unitX = path->x; 
			unitY = path->y;
		}
		if (path->next == 255) break;
		path = &paths[path->next];
	}
}

// Record game actions
void RecordAction(unsigned char index)
{
	gameActions[actionLast++] = sceneID;
	gameActions[actionLast++] = index;	
}

// Replay game actions
void ReplayActions(void)
{
	unsigned char a, i;
	Interact* interact;
		
	for (i=0; i<actionLast; i+=2) {
		if (gameActions[i] == sceneID) {
			a = gameActions[i+1];
			if (a & 128) {
				// Chunk
				DrawChunk(a-128);
			} else
			if (a & 64) {
				// Picked item
				interact = &interacts[a-64];
				interact->flags = DISABLED;				
			} else {
				// Modifier
				ProcessModifier(a);
			}
		}
	}	
}

// Process interactable object
unsigned char *ProcessInteract(unsigned char target, unsigned char item)
{
	unsigned char i, modifID;
	signed int deltaX, deltaY;
	Trigger *trigger;
	Interact* interact = &interacts[target];
		
	// Update frame (if any)
	if (interact->frame != 255) {
		waitFrame = interact->frame;
		DrawUnit(unitX, unitY, waitFrame);				
	}

	// Are we using an item?
	if (item != 255) {
		
		// Find associated trigger (if any)
		for (i=0; i<num_triggers; i++) {
			trigger = &triggers[i];
			if ( interact->label == trigger->label && 
			    !strcmp(&gameItems[item*LEN_ITEM], &strings[trigger->item]) ) 
			{	
				// Display chunk (if any)
				if (trigger->chk != 255)
					DrawChunk(trigger->chk);

				// Process answer (if any)
				if (trigger->answer) {
					inkColor = INK_INTERACT;
					PrintMessage(trigger->answer);
					Wait(120);
				}					
				
				// Restore background (if any)
				if (trigger->bcg != 255)
					DrawBackg(trigger->bcg);
							
				// Check if item is converted?
				if (trigger->convert)
					ConvertItem(item, trigger->convert);
				else
					PopItem(item);
				
				// Check if modifier is triggered?
				modifID = trigger->modifier;
				if (modifID != 255) {
					ProcessModifier(modifID);
					RecordAction(modifID);
				}
				
				// Was chunk applied permanently?
				if (trigger->chk != 255 && trigger->bcg == 255)
					RecordAction(trigger->chk + 128);

				break;
			}
		}
		
		// Did we find a match?
		if (i == num_triggers) {
			inkColor = INK_GOBLIN;
			PrintMessage(255);
			Wait(120);
		}
		
	} else {		
		// If this a load scene interact?
		if (interact->flags & LOADSCENE)
			return &strings[interact->question];
	
		// Process question (if any)
		if (interact->question) {
			inkColor = INK_GOBLIN;
			PrintMessage(interact->question);
			Wait(120);
		}
		
		// Display chunk (if any)
		if (interact->chk != 255) {
			DrawChunk(interact->chk);
		}
		
		// Process answer (if any)
		if (interact->answer) {
			inkColor = INK_INTERACT;
			PrintMessage(interact->answer);		
		}		
		
		// Process path (if any)
		if (interact->path != 255) {
			ProcessPath(interact->path);
		} else {
			if (interact->bcg != 255 || interact->answer)
				Wait(120);	
		}
		
		// Restore background (if any)
		if (interact->bcg != 255)
			DrawBackg(interact->bcg);
		
		// Process pickable item
		if (interact->flags & PICKABLE) {
			PushItem(interact->label);
			interact->flags = DISABLED;
			RecordAction(target + 64);				
		}	
		
		// Was chunk applied permanently?
		if (interact->chk != 255 && interact->bcg == 255)
			RecordAction(interact->chk + 128);					
	}
		
	// Clean-up
	PrintMessage(0);
#if defined(__NES__)
	PrintInventory();
#endif	
	inkColor = INK_DEFAULT;
	return 0;
}

// Move to next waypoint
void ProcessWaypoint(void)
{
	// Home-in on waypoint
	unitX = wayX[0];
	unitY = wayY[0];				
	numWay--;
	
	// Shift down remaining waypoints
	if (numWay) {
		memcpy(&wayX[0], &wayX[1], MAX_WAYPOINTS-1);
		memcpy(&wayY[0], &wayY[1], MAX_WAYPOINTS-1);						
	}	
}

void PlayScene(void)
{
	clock_t animClock, gameClock;
	unsigned char angle, i, j, interI[4], interN;
	signed int deltaX, deltaY, interX[4], interY[4], interD[4], tmp;
	unsigned char sceneSearch = 0, sceneIndex = 255, sceneInteract = 255, sceneItem = 255, *scene;	
	unsigned int clickX, clickY;
	Animation *anim;
	
	// Initialize clocks
	animClock = gameClock = clock();
	
	while (1) {
	#if defined __APPLE2__
		clk += 2;  // Manually update clock on Apple 2
	#endif
		// Did we reach the ending?	
		if (!sceneID)
			continue;
		
		// Get mouse state
		mouse = GetMouse();
		if (!(mouse[2] & MOU_LEFT)) {
			mouseL = 1;
		} else {
			mouseL = 0; mouseAction = 0;
		}
		mouseX = 2*mouse[0];
		mouseY = mouse[1];
		
		// Update mouse pointer
		DrawPointer(mouseX, mouseY, mouseL);
		
		// Search scene
		if (!(mouse[2] & MOU_MOTION)) {
			if (mouseY <= INVENTORY_Y) {
				sceneSearch = SearchScene(mouseX-4, mouseY-4);	
			} else {
				sceneSearch = 255;
			}
			if (sceneSearch != sceneIndex) {
				sceneIndex = sceneSearch;
				PrintInteract(sceneItem, sceneIndex);
			}
		}
		
		// Process commands
		if (mouseL && !mouseAction) {
	 		// Record action
			mouseAction = 1;
			PlaySFX(SFX_BUMP, 32, 120, 2);
			
			// Reset navigation
			sceneInteract = 255;
			numWay = 0;
					
			// Is mouse cursor in inventory area?
			if (mouseY > INVENTORY_Y) {
				sceneItem = SelectItem(mouseX, mouseY);
				PrintInteract(sceneItem, 255);
				
			} else {
				// Save current interact index
				sceneInteract = sceneIndex;
				
				// Get click coordinates
				if (sceneInteract == 255) {
					clickX = mouseX;
					clickY = mouseY;
					if (sceneItem != 255) {
						sceneItem = 255;
						PrintInteract(sceneItem, sceneIndex);
					}
				} else { 
					clickX = interacts[sceneInteract].cx;
					clickY = interacts[sceneInteract].cy;
				}
			
				// Compute goal coordinates
				if (!unitX && !unitY) {
					// Unit is disabled: DO NOTHING!				
				} else {					
					// Check intersections with polygon
					interN = IntersectSegmentPolygon(unitX, unitY, clickX, clickY, num_polygon, polygonX, polygonY, interI, interX, interY);
					if (interN) {
						// Compute distance to unit
						for (i=0; i<interN; i++) {
							interD[i] = abs(unitX - interX[i]) + abs(unitY - interY[i]); 
						}
						
						// Sort intersections by distance
						for (i=0; i<interN; ++i) { 
							for (j=i+1; j<interN; ++j) { 
								if (interD[i] > interD[j]) {
									tmp = interI[i];  interI[i] = interI[j]; interI[j] = tmp;
									tmp = interX[i];  interX[i] = interX[j]; interX[j] = tmp;
									tmp = interY[i];  interY[i] = interY[j]; interY[j] = tmp;
									tmp = interD[i];  interD[i] = interD[j]; interD[j] = tmp;
								}
							}				
						}						
						
						// Add waypoints						
						tmp = 1;
						for (i=0; i<interN; i++) {
							
							// Move around the polygon edge
							if (!tmp) {
								j = interI[i-1];
								if (interI[i] > interI[i-1]) {
									while (j<interI[i]) {
										wayX[numWay] = polygonX[j];
										wayY[numWay] = polygonY[j];
										numWay++; j++; 
										if (numWay > (MAX_WAYPOINTS-2))
											break;
									} 
								} else {
									while (j>interI[i]) {
										wayX[numWay] = polygonX[j-1];
										wayY[numWay] = polygonY[j-1];
										numWay++; j--; 
										if (numWay > (MAX_WAYPOINTS-2))
											break;
									} 											
								}
							}
							tmp = !tmp;
							
							// Move to intersection point
							wayX[numWay] = interX[i];
							wayY[numWay] = interY[i];
							numWay++; 
							if (numWay > (MAX_WAYPOINTS-1))
								break;
						}
					}	

					// Then move to mouse cursor (if in allowed area)
					if (PointInsidePolygon(clickX, clickY, num_polygon, polygonX, polygonY)) {
						wayX[numWay] = clickX;
						wayY[numWay] = clickY;
						numWay++;	
					}
				}
			}
		}
				
		// Process unit motion
		if (clock()>gameClock+unitTicks) {
			gameClock = clock();
			
			if (numWay) {			
				// Move in steps of 3 max.
				deltaX = wayX[0] - unitX;
				deltaY = wayY[0] - unitY;
				if (unitStep >= abs(deltaX) && unitStep >= abs(deltaY)) {
					ProcessWaypoint();
					
				} else {
					// Move along vector
					angle = atan2fast(deltaY, deltaX);
					unitX += (unitStep*cos[angle])/16;
					unitY += (unitStep*sin[angle])/16;
					
					// Check that nothing went wrong...
					if (unitX > 320 || unitY > INVENTORY_Y)
						ProcessWaypoint();
				}
				
				// Update frame number
				if (deltaX > 0) {
					if (unitFrame < frameWalkRightBeg) unitFrame = frameWalkRightBeg;
					unitFrame += 1; if (unitFrame > frameWalkRightEnd) unitFrame = frameWalkRightBeg;
					waitFrame = frameWaitRight;
				} else {
					if (unitFrame > frameWalkLeftEnd) { unitFrame = frameWalkLeftBeg; }
					unitFrame += 1; if (unitFrame > frameWalkLeftEnd) unitFrame = frameWalkLeftBeg;
					waitFrame = frameWaitLeft;
				}
			} else {
				// Process trigger (if any) and set wait frame
				if (sceneInteract != 255) {
					scene = ProcessInteract(sceneInteract, sceneItem);
					if (scene)
						LoadScene(scene);
					if (!sceneID)
						continue;
					sceneInteract = 255;
					sceneItem = 255;
				}
				unitFrame = waitFrame;
			}
			DrawUnit(unitX, unitY, unitFrame);
		}
		
		// Process scene animations
		for (i=0; i<num_animations; i++) {
			anim = &animations[i];
			if (anim->counter < anim->ticks[anim->state]) {
				// Increase counter
				anim->counter += (clock()-animClock);
			} else {
				// Reset counter
				anim->counter = 0;

				// Move to next frame
				anim->state++;
				while (anim->state<4 && !anim->ticks[anim->state])
					anim->state++;
							
				// Check if reached last frame
				if (anim->state > 3)
					anim->state = 0;		

				// Apply frame
				if (anim->state < 3)
					DrawChunk(anim->frames[anim->state]);
				else
					DrawBackg(anim->frames[anim->state]);
			}
		}
		animClock = clock();
	}	
}

unsigned char CompareNames(const char* fname1, const char* fname2);

// Load and initialize scene state
void LoadScene(unsigned char* scene)
{	
	unsigned char fname[13];
	unsigned char i, l, *coords;

	// Disable music/sprites/bitmap
	if (currmusic[0])
		StopMusic();	
	DisableSprite(-1);
	HideBitmap();
	
	// Prepare filename
	l = strlen(scene);
	memcpy(fname, scene, l);
	memcpy(gameScene, scene, l);

	// Load chunks
	FreeChunk(0);
	memcpy(&fname[l], ".chk\0", 5);
	while (!FileExists(fname))
		PrintSwapDisk(fname);
	num_chunks = LoadChunks(chunkData, fname);
	
	// Load bitmap
	memcpy(&fname[l], ".img\0", 5);
	while (!FileExists(fname))
		PrintSwapDisk(fname);
	LoadBitmap(fname);
	
	// Load navigation
	memcpy(&fname[l], ".nav\0", 5);
	while (!FileExists(fname))
		PrintSwapDisk(fname);
	if (FileOpen(fname)) {
		FileRead(&sceneID, -1);
		FileClose();
	}

	// Backup Chunks BCG
	for (i=0; i<num_chunks; i++) {
		coords = chunkData[i]; 
		chunkBckg[i] = GetChunk(coords[0], coords[1], coords[2], coords[3]);
	}
	
	// Reset mouse/player position
	unitX = startx[0]; unitY = starty[0];
	numWay = 0;	
	
	// Check if game ended?
	if (sceneID) {
		// Refresh game state
		ReplayActions();
		
		// Assign ink/paper colors
		paperColor = PAPER_DEFAULT;
		inkColor = INK_DEFAULT;
	#if defined(__ORIC__)
		txtY = TXT_ROWS-2;
		SetAttributes(paperColor); txtY++;
		SetAttributes(paperColor); txtY--;
		txtX = 0; PrintBlanks(TXT_COLS, 2);	
	#endif

		// Display inventory
		PrintInventory();
	}
	
	// Enable music & bitmap
 	if (music) {
		ShowBitmap();
		if (CompareNames(&strings[music], currmusic)) {
			strcpy(currmusic, &strings[music]);
			while (!FileExists(currmusic))
				PrintSwapDisk(currmusic);
			LoadMusic(currmusic);
		}
		PlayMusic();
	} else {
		currmusic[0] = 0;
	}
	ShowBitmap();
	
	// Enable sprites
	EnableSprite(0);  // Mouse cursor
	if (unitX || unitY) {
		EnableSprite(1);  // Unit #1
	#if defined(__ATARI__) || defined(__CBM__ )
		EnableSprite(2);  // Unit #1 (extra color)
		EnableSprite(3);  // Unit #1 (extra color)
		EnableSprite(4);  // Unit #1 (extra color)
	#endif	
	}
}

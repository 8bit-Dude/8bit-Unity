
#include "definitions.h"

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
  #pragma bss-name(push, "XRAM")  
#endif

// Game state: Actions consist of tuplets (Scene ID, Action ID (lower 6bits) + Type (higher 2bits))
unsigned char actionLast, itemLast, itemOffset;
unsigned char gameActions[0x100];
unsigned char gameItems[MAX_ITEM*LEN_ITEM];
unsigned char gameScene[8];

// DO NOT CHANGE ORDER: This data is loaded sequentially from file!!!!!
unsigned char sceneID, music;
unsigned int startx[MAX_PLAYERS], starty[MAX_PLAYERS];
unsigned char num_polygon, num_interacts, num_triggers, num_modifiers, num_paths;
signed int polygonX[MAX_POLYGON];
signed int polygonY[MAX_POLYGON];
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
	  
// See goblin.c	  
extern unsigned char numWay, waitFrame;
extern unsigned int mouseX, mouseY, unitX, unitY;
	  
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

// Load and initialize scene state
void LoadScene(unsigned char* scene)
{	
	unsigned char fname[13];
	unsigned char i, l, *coords;

	// Disable sprites/bitmap/music
	DisableSprite(-1);
	HideBitmap();
	if (music)
		StopMusic();	
	
	// Prepare filename
	l = strlen(scene);
	memcpy(fname, scene, l);
	memcpy(gameScene, scene, l);
	
	// Load bitmap
	memcpy(&fname[l], ".img\0", 5);
	LoadBitmap(fname);
	
	// Load chunks/backgrounds
	FreeChunk(0);
	memcpy(&fname[l], ".chk\0", 5);
	num_chunks = LoadChunks(chunkData, fname);
	for (i=0; i<num_chunks; i++) {
		coords = chunkData[i]; 
		chunkBckg[i] = GetChunk(coords[0], coords[1], coords[2], coords[3]);
	}
	
	// Load navigation
	memcpy(&fname[l], ".nav\0", 5);
	if (FileOpen(fname)) {
		FileRead(&sceneID, -1);
		FileClose();
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
		LoadMusic(&strings[music]);
		PlayMusic();
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

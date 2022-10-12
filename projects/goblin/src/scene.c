
#include "definitions.h"

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
  #pragma bss-name(push, "XRAM")  
#endif

// DO NOT CHANGE ORDER: This data is loaded sequentially from file!!!!!
unsigned char music;
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
extern unsigned char waitFrame;
extern unsigned int unitX, unitY;
extern unsigned int goalX, goalY;

// See inventory.c
extern Item items[MAX_ITEM];
	  
// Initialize scene animations
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
		FileRead(&music, -1);
		FileClose();
	}
	
	// Assign ink/paper colors
	paperColor = PAPER_DEFAULT;
	inkColor = INK_DEFAULT;
#if defined(__ORIC__)
	txtY = TXT_ROWS-2;
	SetAttributes(paperColor); txtY++;
	SetAttributes(paperColor); txtY--;
	txtX = 0; PrintBlanks(TXT_COLS, 2);	
#endif	

	// Update player position
	goalX = unitX = startx[0]; 
	goalY = unitY = starty[0];

	// Music & bitmap
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
	signed int deltaX, deltaY;
	Interact* interact;
	for (i=0; i<MAX_INTERACT; i++) {
		interact = &interacts[i];
		if (!interact->flags) continue;
		deltaX = searchX - interact->x;
		deltaY = searchY - interact->y;
		if ( (deltaX * deltaX + deltaY * deltaY) < (interact->r * interact->r) ) {
			return i;
		}
	}
	return 255;
}

// Process interactable object
unsigned char *ProcessInteract(unsigned char target, unsigned char item)
{
	Path *path;
	Trigger *trigger;
	Modifier *modifier;
	Interact *modified;
	unsigned char i, j, d;
	signed int deltaX, deltaY;
	unsigned int targetLabel, itemLabel;
	Interact* interact = &interacts[target];
	
	// Check if we are close enough?
	if (unitX || unitY) {
		deltaX = interact->x; deltaX = ABS(deltaX-unitX); 
		deltaY = interact->y; deltaY = ABS(deltaY-unitY);
		if (deltaX > 30 || deltaY > 25) return 0;
	}
	
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
			if (items[item].label == trigger->item && interact->label == trigger->label) {
				
				// Check if modifier is triggered?
				if (trigger->modifier != 255) {
					modifier = &modifiers[trigger->modifier];
					for (j=0; j<num_interacts; j++) {
						modified = &interacts[j];
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
					PopItem(item);					
				}
		
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
			path = &paths[interact->path];
			while (1) {
				deltaX = path->x - unitX;
				deltaY = path->y - unitY;
				d = ABS(deltaX) + ABS(deltaY);
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
			goalX = unitX; 
			goalY = unitY;
		} else {
			if (interact->chk != 255 || interact->answer)
				Wait(120);	
		}
		
		// Restore background (if any)
		if (interact->bcg != 255)
			DrawBackg(interact->bcg);
		
		// Process pickable item
		if (interact->flags & PICKABLE) {
			PushItem(interact->label);
			interact->flags = DISABLED;
		}	
	}
		
	// Clean-up
	PrintMessage(0);
#if defined(__NES__)
	PrintInventory();
#endif	
	inkColor = INK_DEFAULT;
	return 0;
}

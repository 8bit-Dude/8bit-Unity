
#include "definitions.h"

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

// DO NOT CHANGE ORDER: This data is loaded sequentially from file
unsigned char num_polygon, num_chunks, num_interacts, num_triggers, num_paths;
signed int polygonX[MAX_POLYGON];
signed int polygonY[MAX_POLYGON];
unsigned int chunkNames[MAX_CHUNK];
Interact interacts[MAX_INTERACT]; 
Trigger triggers[MAX_TRIGGER];
Modifier modifiers[MAX_MODIFIER];
Path paths[MAX_PATH];
unsigned char strings[0x400];

// Current inventory
Item items[MAX_ITEM] = { { TXT_COLS-7,  TXT_ROWS-2, 0 },
						 { TXT_COLS-7,  TXT_ROWS-1, 0 } };

// Chunks for scene animation
unsigned char* chunkData[MAX_CHUNK];
unsigned char* chunkBckg[MAX_CHUNK];
	  
// See goblin.c	  
extern unsigned int unitX, unitY;
extern unsigned int goalX, goalY;
	  
// Initialize scene animations
void InitScene()
{	
	unsigned char i, *coords;
	
	// Load bitmap
	HideBitmap();
	LoadBitmap("scene1.img");
	ShowBitmap();
	
	// Load navigation
	if (FileOpen("scene1.nav")) {
		FileRead(&num_polygon, -1);
		FileClose();
	}

	// Prepare graphic animations
	for (i=0; i<num_chunks; i++) {
		LoadChunk(&chunkData[i], &strings[chunkNames[i]]);
	}

	// Grab background of some animations
	coords = chunkData[0]; GetChunk(&chunkBckg[0], coords[0], coords[1], coords[2], coords[3]);  // Grab Notable bakground
	coords = chunkData[1]; GetChunk(&chunkBckg[1], coords[0], coords[1], coords[2], coords[3]);  // Grab Old men bakground
	coords = chunkData[4]; GetChunk(&chunkBckg[4], coords[0], coords[1], coords[2], coords[3]);  // Grab Switch bakground
	
	// Assign ink/paper colors
#if (defined __ORIC__)
	paperColor = ORANGE;
	txtX = -1; txtY = TXT_ROWS-2;
	SetAttributes(paperColor); txtY++;
	SetAttributes(paperColor); txtY--;
	txtX = 0; PrintBlanks(TXT_COLS, 2);	
#else	
	paperColor = ORANGE;
	inkColor = WHITE;
#endif	
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
		wait(1); clk += 10;  // Manually update clock on Apple 2
	#endif
	}
}

// Manage inventory
void PushItem(unsigned int label)
{
	unsigned char i=0;
	while (i<MAX_ITEM) {
		if (!items[i].label) {
			// Copy reference to label
			items[i].label = label;
			break;
		}
		i++;
	}
	PrintInventory();
}

void PopItem(unsigned char index)
{
	items[index].label = 0;
	PrintInventory();
}

unsigned char SelectItem(unsigned int x, unsigned int y)
{
	unsigned char i=0;
	x = (x*TXT_COLS)/320;
	y = (y*TXT_ROWS)/200;
	while (i<MAX_ITEM) {
		if (items[i].label && x >= items[i].col && y == items[i].row) return i;
		i++;
	}
	return 255;
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
unsigned char ProcessInteract(unsigned char target, unsigned char item)
{
	clock_t timer;
	Path *path;
	Trigger *trigger;
	Modifier *modifier;
	Interact *modified;
	unsigned char i;
	signed int deltaX, deltaY;
	unsigned int targetLabel, itemLabel;
	Interact* interact = &interacts[target];
	
	// Check if we are close enough?
	deltaX = interact->x; deltaX = ABS(deltaX-unitX); 
	deltaY = interact->y; deltaY = ABS(deltaY-unitY);
	if (deltaX > 30 || deltaY > 25) return 0;

	// Are we using an item?
	if (item != 255) {
		// Find associated trigger (if any)
		targetLabel = interact->label;
		itemLabel   = items[item].label;
		for (i=0; i<num_triggers; i++) {
			trigger = &triggers[i];
			if (itemLabel == trigger->item && targetLabel == trigger->label) {
				// Check if modifier is triggered?
				if (trigger->modifier != 255) {
					modifier = &modifiers[trigger->modifier];
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
					PopItem(item);					
				}
				
				// Process answer (if any)
				if (trigger->answer) {
					inkColor = INK_INTERACT;
					PrintMessage(trigger->answer);
					Wait(120);
				}					
				break;
			}
		}
		
	} else {
		// Process question (if any)
		if (interact->question) {
			inkColor = INK_GOBLIN;
			PrintMessage(interact->question);
			Wait(120);
		}
		
		// Display chunk (if any)
		if (interact->chk != 255)
			DrawChunk(interact->chk);

		// Process path (if any)
		if (interact->path != 255) {
			timer = clock();
			path = &paths[interact->path];
			while (path->next != 255) {
				DrawUnit(path->x2, path->y2, frameWalkLeftBeg);
				Wait(30);
				path = &paths[path->next];
			}
			unitX = path->x2; unitY = path->y2;
			goalX = path->x2; goalY = path->y2;
			DrawUnit(unitX, unitY, frameWalkLeftBeg);
		}

		// Process answer (if any)
		if (interact->answer) {
			inkColor = INK_INTERACT;
			PrintMessage(interact->answer);
			Wait(120);			
		}
		
		// Restore background (if any)
		if (interact->bcg != 255)
			DrawBackg(interact->bcg);
		
		// Process pickable item
		if (interact->flags & PICKABLE) {
			PushItem(interact->label);
			interact->flags = INACTIVE;
		}	
	}
	
	// Check is quest is completed
	if (target == 6) return 1;
	
	// Clean-up
	PrintMessage(0);
#if defined(__NES__)
	PrintInventory();
#endif	
	inkColor = INK_DEFAULT;
	return 0;
}

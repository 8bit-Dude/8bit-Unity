
#include "interface.h"

void Wait(unsigned char ticks) 
{
	clock_t animClock = clock();
	while (clock()<animClock+ticks) { 
	#ifdef __APPLE2__
		wait(1); clk += 10;  // Manually update clock on Apple 2
	#endif
	} 
}

// Manage inventory
void PushItem(char* label)
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
	x = (x*CHR_COLS)/320;
	y = (y*CHR_ROWS)/200;
	while (i<MAX_ITEM) {
		if (items[i].label && x >= items[i].col && y == items[i].row) return i;
		i++;
	}
	return 255;
}

// Process interactable object
void ProcessInteract(unsigned char index, unsigned char item, unsigned char x, unsigned char y)
{
	signed int deltaX, deltaY;
	Interact* interact = &interacts[index];
	
	// Check if we are close enough?
	deltaX = interact->x; deltaX = ABS(deltaX-x); 
	deltaY = interact->y; deltaY = ABS(deltaY-y);
	if (deltaX > 30 || deltaY > 25) return;
	
	if (item != 255) {
		// Use item on interact
		if (!strcmp(items[item].label, "Flower") && (index < 2)) {
			inkColor = RED;
			PrintMessage("No, thank you...");
		} else if (!strcmp(items[item].label, "Bottle") && (index < 1)) {
			PopItem(item);
			interacts[4].flags |= PICKABLE;
			interacts[4].chunk1 = 0;
			interacts[4].chunk2 = chunks[3];
			interacts[4].answer = 0;
			inkColor = RED;
			PrintMessage("Just what I need for my thirst!\nWould you like some sausage?");
			Wait(60);
		} else {
			inkColor = YELLOW;
			PrintMessage("I can't do that...");
		}
		Wait(120);
	} else {
		// Process question (if any)
		if (interact->question) {
			inkColor = YELLOW;
			PrintMessage(interact->question);
			Wait(120);
		}
		
		// Process answer/chunk#1 (if any)
		if (interact->answer || interact->chunk1) {
			inkColor = RED;
			if (interact->chunk1) DrawChunk(interact->chunk1);
			if (interact->answer) PrintMessage(interact->answer);
			Wait(120);
		}
		
		// Process graphic chunk #2 (if any)
		if (interact->chunk2) {
			if (interact->chunk2) DrawChunk(interact->chunk2);
		}	
		
		// Process pickable item
		if (interact->flags & PICKABLE) {
			PushItem(interact->label);
			interact->flags = INACTIVE;
		}
	}
	
	// Clean-up
	PrintMessage("\0");
	inkColor = WHITE;
}

// Search for interactable object under the mouse cursor
unsigned char SearchScene(signed int searchX, signed int searchY) 
{
	unsigned char i;
	Interact* interact;
	for (i=0; i<MAX_INTERACT; i++) {
		interact = &interacts[i];
		if (!interact->flags) continue;
		if (((searchX - interact->x) * (searchX - interact->x) + 
		     (searchY - interact->y) * (searchY - interact->y) ) < (interact->r * interact->r)) {
			return i;
		}
	}
	return 255;
}

// Initialize scene animations
void InitScene()
{	
	// Prepare graphic chunks
	LoadChunk(&chunks[0], "chunk0.dat");	// Load Notable animation
	LoadChunk(&chunks[1], "chunk1.dat");	// Load Old men animation
	LoadChunk(&chunks[2], "chunk2.dat");	// Load Bottle removed
	LoadChunk(&chunks[3], "chunk3.dat");	// Load Sausage removed
	GrabChunkBG(&chunks[4], chunks[0]);		// Grab Notable bakground
	GrabChunkBG(&chunks[5], chunks[1]);		// Grab Old men background
#if (defined __ORIC__)
	chunks[2][3] = 11;	// For some reason, the last line corrupts the screen...
#endif

	// Assign graphic chunks to interactables
	interacts[1].chunk1 = chunks[1];
	interacts[1].chunk2 = chunks[5];
	interacts[2].chunk2 = chunks[2];
	interacts[4].chunk1 = chunks[0];
	interacts[4].chunk2 = chunks[4];

	// Assign ink/paper colors
	inkColor = WHITE;
	paperColor = ORANGE;
}

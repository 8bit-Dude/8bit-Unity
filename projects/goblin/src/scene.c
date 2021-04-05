
#include "definitions.h"

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

Interact interacts[MAX_INTERACT] = 
	{ {  55,  81, 15,  90,  93, 	ACTIVE,		 "Notable",  "Dear sir, you look powerful.\nPlease help me with my quest?\0", "Go away, I am busy!\0", 0, 0 },
	  { 222,  66, 15, 194,  86, 	ACTIVE,		 "Old Men",  "Hey villagers, do you know\nthe house of lord Tazaar?\0", "We are hungry!! But the\nnotable keeps all the food.", 0, 0 },
	  { 260,  70,  7, 240,  94, ACTIVE|PICKABLE, "Bottle",   0, 0, 0, 0 },
	  { 230, 134, 10, 208, 141, ACTIVE|PICKABLE, "Flower",   0, 0, 0, 0 },
	  {  69,  60,  7,  94,  93, 	ACTIVE, 	 "Sausage",  0, "Hey, don't touch that!", 0, 0 },
	  {  32,  77,  5,  18, 101, 	ACTIVE,		 "Switch",   0, 0, 0, 0 },
	  { 300, 170, 30, 300, 170,    INACTIVE,	 "Fountain", 0, "Well done little goblin!\nThe tech-demo ends here!", 0, 0 } };

Item items[MAX_ITEM] = 
	{ { TXT_COLS-7,  TXT_ROWS-2, INACTIVE, 0 },
	  { TXT_COLS-7,  TXT_ROWS-1, INACTIVE, 0 } };

// Chunks for scene animation
unsigned char* chunkAnim[6];
unsigned char* chunkBcgr[3];
	  
// Initialize scene animations
void InitScene()
{	
	// Load bitmap
	HideBitmap();
	LoadBitmap("scene1.img");
	ShowBitmap();

	// Prepare graphic animations
	LoadChunk(&chunkAnim[0], "notable.chk"); // Load Notable animation
	LoadChunk(&chunkAnim[1], "oldmen.chk");	 // Load Old men animation
	LoadChunk(&chunkAnim[2], "bottle.chk");	 // Load Bottle removed
	LoadChunk(&chunkAnim[3], "sausage.chk"); // Load Sausage removed
	LoadChunk(&chunkAnim[4], "switch.chk");	 // Switch animation

	// Grab background of some animations
	GrabBackground(&chunkBcgr[0], chunkAnim[0]);  // Grab Notable bakground
	GrabBackground(&chunkBcgr[1], chunkAnim[1]);  // Grab Old men background
	GrabBackground(&chunkBcgr[2], chunkAnim[4]);  // Grab Switch background

	// Assign animations to interactables
	interacts[1].chunk1 = chunkAnim[1];	 // Old men speaking
	interacts[1].chunk2 = chunkBcgr[1];
	interacts[2].chunk2 = chunkAnim[2];  // Bottle removed
	interacts[4].chunk1 = chunkAnim[0];  // Sausage attack
	interacts[4].chunk2 = chunkBcgr[0];
	interacts[5].chunk1 = chunkAnim[4];  // Swith animation
	interacts[5].chunk2 = chunkBcgr[2];
	
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

void Wait(unsigned char ticks) 
{	
	clock_t animClock = clock();
#ifdef __ORIC__
	ticks *= 2;	// Clock runs faster on the Oric
#endif
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
unsigned char ProcessInteract(unsigned char index, unsigned char item, unsigned int unitX, unsigned int unitY)
{
	signed int deltaX, deltaY;
	Interact* interact = &interacts[index];
	
	// Check if we are close enough?
	deltaX = interact->x; deltaX = ABS(deltaX-unitX); 
	deltaY = interact->y; deltaY = ABS(deltaY-unitY);
	if (deltaX > 30 || deltaY > 25) return 0;
	
	if (item != 255) {
		// Use item on interact
		if (!strcmp(items[item].label, "Flower") && (index < 2)) {
			inkColor = INK_INTERACT;
			PrintMessage("No, thank you...");
		} else if (!strcmp(items[item].label, "Bottle") && (index < 2)) {
			if (index == 1) {
				inkColor = INK_INTERACT;
				PrintMessage("We are not thirsty...\nWe are hungry!!");
				Wait(60);
			} else {
				PopItem(item);
				interacts[4].flags |= PICKABLE;
				interacts[4].chunk1 = 0;
				interacts[4].chunk2 = chunkAnim[3];	 // Sausage removed
				interacts[4].answer = 0;
				inkColor = INK_INTERACT;
				PrintMessage("Just what I need in this heat!\nWould you like some sausage?");
				Wait(60);
			}
		} else if (!strcmp(items[item].label, "Sausage") && (index == 1)) {
			PopItem(item);
			interacts[6].flags |= ACTIVE;  // Fountain interactable
			inkColor = INK_INTERACT;
			interacts[1].answer = "Thank you kind sir!\nGo right, to the fountain.\0";
			PrintMessage(interacts[1].answer);
			Wait(60);
		} else {
			inkColor = INK_GOBLIN;
			PrintMessage("I can't do that...");
		}
		Wait(120);
	} else {
		// Process question (if any)
		if (interact->question) {
			inkColor = INK_GOBLIN;
			PrintMessage(interact->question);
			Wait(120);
		}
		
		// Process answer/chunk#1 (if any)
		if (interact->answer || interact->chunk1) {
			inkColor = INK_INTERACT;
			if (index == 5) {
				// Jump animation
				PlaySFX(SFX_BLEEP, 64, 60, 2);
				DrawUnit(unitX, unitY-10, frameWalkLeftBeg);
				DrawChunk(interact->chunk1);
				Wait(10);
				DrawUnit(unitX, unitY-20, frameWalkLeftBeg);
				Wait(10);
				DrawUnit(unitX, unitY-25, frameWalkLeftBeg);
				Wait(10);
				DrawUnit(unitX, unitY-20, frameWalkLeftBeg);
				Wait(10);
				DrawChunk(interact->chunk2);
				DrawUnit(unitX, unitY-10, frameWalkLeftBeg);
				Wait(10);
				DrawUnit(unitX, unitY, frameWalkLeftBeg);
			} else {
				if (interact->chunk1) DrawChunk(interact->chunk1);
				if (interact->answer) PrintMessage(interact->answer);
				Wait(120);
			}
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
	
	// Check is quest is completed
	if (index == 6) return 1;
	
	// Clean-up
	PrintMessage("\0");
	inkColor = INK_DEFAULT;
	return 0;
}

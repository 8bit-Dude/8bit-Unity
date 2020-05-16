
#include "interface.h"

void Wait(unsigned char ticks) 
{
	clock_t animClock = clock();
	while (clock()<animClock+ticks) { 
	#ifdef __APPLE2__
		tick(); 
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

// Process interactable object
void ProcessInteract(unsigned char index)
{
	Interact* interact = &interacts[index];
	
	// Process question (if any)
	if (interact->question) {
		inkColor = YELLOW;
		PrintMessage(interact->question);
		Wait(120);
	}
	
	// Process answer/chunk#1 (if any)
	if (interact->answer || interact->chunk1) {
		inkColor = RED;
		if (interact->answer) PrintMessage(interact->answer);
		if (interact->chunk1) DrawSceneChunk(interact->chunk1);
		Wait(120);
	}
	
	// Process graphic chunk #2 (if any)
	if (interact->chunk2) {
		if (interact->chunk2) DrawSceneChunk(interact->chunk2);
	}	
	
	// Custom processing
	if (index == 2 || index == 3) {
		PushItem(interact->label);
		interact->flags = INACTIVE;		// Bottle can be clicked only once
	}
	inkColor = WHITE;
}

unsigned int sceneX[6] = { 55, 222, 260, 230, 69, 32 };
unsigned int sceneY[6] = { 81,  66,  70, 134, 60, 77 };
unsigned int sceneR[6] = { 15,  15,   7,  10,  7,  5 };

// Search for interactable object under the mouse cursor
unsigned char SearchScene(signed int searchX, signed int searchY) 
{
	unsigned char i;
	Interact* interact;
	for (i=0; i<MAX_INTERACT; i++) {
		interact = &interacts[i];
		if (!interact->flags) continue;
		//if (((searchX - interact->x) * (searchX - interact->x) + 
		//     (searchY - interact->y) * (searchY - interact->y) ) < (interact->r * interact->r)) {
		if (((searchX - sceneX[i]) * (searchX - sceneX[i]) + 
		     (searchY - sceneY[i]) * (searchY - sceneY[i]) ) < (sceneR[i] * sceneR[i])) {
			return i;
		}
	}
	return 255;
}
		
// Link labels, questions and answers to arrays
void InitScene()
{	
	// Load graphic chunks
#if defined __APPLE2__
	LoadSceneChunk(&chunks[0],  14, 50, 28, 46, "chunk0.dat");
	LoadSceneChunk(&chunks[1], 112, 58,  7, 18, "chunk1.dat");
	GrabSceneChunk(&chunks[3],  14, 50, 28, 46);
#elif defined __ATARI__
	LoadSceneChunk(&chunks[0],  16, 52, 28, 46, "chunk0.dat");
	LoadSceneChunk(&chunks[1], 124, 60, 12, 20, "chunk1.dat");
	GrabSceneChunk(&chunks[3],  16, 52, 28, 46);
#elif defined __C64__
	LoadSceneChunk(&chunks[0],  16, 48, 28, 48, "chunk0.dat");
	LoadSceneChunk(&chunks[1], 124, 76, 12, 24, "chunk1.dat");
	GrabSceneChunk(&chunks[3],  16, 48, 28, 48);
#elif defined __LYNX__
	LoadSceneChunk(&chunks[0],  16, 26, 28, 23, "chunk0.dat");
	LoadSceneChunk(&chunks[1], 126, 30,  8, 10, "chunk1.dat");
	GrabSceneChunk(&chunks[3],  16, 26, 28, 23);
#elif defined __ORIC__
	LoadSceneChunk(&chunks[0],  15, 26, 27, 22, "chunk0.dat");
	LoadSceneChunk(&chunks[1],  93, 32,  6,  6, "chunk1.dat");
	GrabSceneChunk(&chunks[3],  15, 26, 27, 22);
#endif	

	// Assign graphic chunks to interactables
	interacts[4].chunk1 = &chunks[0];
	interacts[4].chunk2 = &chunks[3];
	interacts[2].chunk2 = &chunks[1];

	// Assign ink/paper colors
	inkColor = WHITE;
	paperColor = ORANGE;
}

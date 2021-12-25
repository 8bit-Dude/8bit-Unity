
#include "definitions.h"

// See scene.c
extern Item items[MAX_ITEM];

// Print label in lower-left panel
void PrintInteract(unsigned char item, unsigned char *label)
{
	unsigned char *iLabel;
	txtX = 0; txtY = TXT_ROWS-2;
	if (item != 255) {
		iLabel = items[item].label;
		PrintStr("use"); txtX += 4; 
		PrintStr(iLabel); txtX += 1+strlen(iLabel); 
		PrintStr("on"); txtX += 3; 
	} 
	if (label)
		PrintStr(label);
}

// Print message in lower-left panel
void PrintMessage(unsigned char *msg)
{
	unsigned char i = 0;
	
	// Reset panel
	txtX = 0; txtY = TXT_ROWS-2;
	PrintBlanks(MSG_WIDTH, 2);
	
	// Print message across two lines, by taking into account line feed '\n'
	while (msg[i] != '\0') {
		if (msg[i] == '\n') {
			txtX = 0; txtY++;
		} else {
			PrintChr(msg[i]);
			txtX++;
		}
		++i;
	}
#if defined(__LYNX__) || defined(__NES__)
	UpdateDisplay(); // Refresh Lynx screen
#endif	
}

// Print inventory in lower-right panel
void PrintInventory(void)
{
	unsigned char i=0;
	Item* item;
	
	inkColor = INK_INVENTORY;
	while (i<MAX_ITEM) {
		item = &items[i];
		txtX = item->col; txtY = item->row;
		if (!item->label) {
			PrintStr("       ");
		} else {
			PrintStr(item->label);
		}
		i++;
	}
	inkColor = INK_DEFAULT;
}

// Draw mouse pointer
void DrawPointer(unsigned int x, unsigned int y, unsigned char pressed) 
{
#if defined __ATARI__
	LocateSprite(x+4, y);
#else		
	LocateSprite(x+4, y+4);
#endif	
	SetSprite(0, pressed);
#if defined __LYNX__
	UpdateDisplay(); // Refresh Lynx screen
#endif
}

// Draw player unit
void DrawUnit(unsigned int x, unsigned int y, unsigned char frame) 
{
#if (defined __CBM__) 
	LocateSprite(x, y-20);
#elif (defined __ATARI__)
	LocateSprite(x, y-16);
#else
	LocateSprite(x, y-10);
#endif
#if (defined __ATARI__) || (defined __CBM__)
	SetSprite(1, frame);	// Unit color #1
	SetSprite(2, frame+14);	// Unit color #2
	SetSprite(3, frame+28);	// Unit color #3
	SetSprite(4, frame+42);	// Unit color #4
#else
	SetSprite(1, frame);
#endif
}

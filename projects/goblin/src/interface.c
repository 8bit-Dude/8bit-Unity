
#include "definitions.h"

// See scene.c
extern Item items[MAX_ITEM];

// Print label in lower-left panel
void PrintInteract(unsigned char item, unsigned char *label)
{
	if (item != 255) {		
		PrintStr(0, TXT_ROWS-2, "use");
		PrintStr(4, TXT_ROWS-2, items[item].label);
		PrintStr(5+strlen(items[item].label), TXT_ROWS-2, "on");
		PrintStr(8+strlen(items[item].label), TXT_ROWS-2, label);
	} else {
		PrintStr(0, TXT_ROWS-2, label);
	}
}

// Print message in lower-left panel
void PrintMessage(unsigned char *msg)
{
	unsigned char i = 0;
	unsigned char col = 0, row = TXT_ROWS-2;
	
	// Reset panel
	PrintBlanks(0, TXT_ROWS-2, TXT_COLS-8, 2);
	
	// Print message across two lines, by taking into account line feed '\n'
	while (msg[i] != '\0') {
		if (msg[i] == '\n') {
			++row;
			col = 0;
		} else {
			PrintChr(col++, row, GetChr(msg[i]));
		}
		++i;
	}
#if defined __LYNX__
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
		if (!item->label) {
			PrintStr(item->col, item->row, "       ");
		} else {
			PrintStr(item->col, item->row, item->label);
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

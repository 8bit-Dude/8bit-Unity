
#include "chunks.h"

// Print message in lower-left panel
void PrintMessage(unsigned char *msg)
{
	unsigned char i = 0;
	unsigned char col = 0, row = CHR_ROWS-2;
	
	// Reset panel
	DrawPanel(0, CHR_ROWS-2, CHR_COLS-8, CHR_ROWS-1);
	
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
	
	inkColor = BLACK;
	while (i<MAX_ITEM) {
		item = &items[i];
		if (!item->label) {
			PrintStr(item->col, item->row, "       ");
		} else {
			PrintStr(item->col, item->row, item->label);
		}
		i++;
	}
	inkColor = WHITE;
}

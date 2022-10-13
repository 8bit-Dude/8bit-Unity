
#include "definitions.h"

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
  #pragma bss-name(push, "XRAM")  
#endif

// Current inventory
extern unsigned int gameItems[MAX_ITEM];
extern unsigned char itemLast, itemOffset;

// Manage inventory
void PushItem(unsigned int label)
{
	gameItems[itemLast++] = label;
	PrintInventory();
}

void PopItem(unsigned char index)
{
	// Shift down all items above index
	while (index<(itemLast-1)) {
		gameItems[index] = gameItems[index+1];
		index++;
	}
	gameItems[index] = 0; itemLast--;
	PrintInventory();
}

unsigned char SelectItem(unsigned int x, unsigned int y)
{
	unsigned char i=0;
	x = (x*TXT_COLS)/320;
	y = (y*TXT_ROWS)/200;
	if (x == TXT_COLS-1) {
		if (y == TXT_ROWS-2) 
			if (itemOffset > 0) 
				itemOffset--;
		if (y == TXT_ROWS-1) 
			if (itemOffset < (itemLast-1)) 
				itemOffset++;			
		PrintInventory();
	} else
	if (x > TXT_COLS-8) {
		if (y == TXT_ROWS-2) 
			if (itemOffset < itemLast)
				return itemOffset;
		if (y == TXT_ROWS-1) 
			if (itemOffset+1 < itemLast)			
				return itemOffset+1;
	}
	return 255;
}

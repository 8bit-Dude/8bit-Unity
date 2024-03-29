
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
extern unsigned char strings[];
extern unsigned char gameItems[MAX_ITEM*LEN_ITEM];
extern unsigned char itemLast, itemOffset;

// Manage inventory
void PushItem(unsigned int label)
{
	memcpy(&gameItems[(itemLast++)*LEN_ITEM], &strings[label], LEN_ITEM);
	if (itemLast > 2)
		itemOffset = itemLast-2;
	PrintInventory();
}

void PopItem(unsigned char index)
{
	// Shift down all items above index
	memcpy(&gameItems[index*LEN_ITEM], &gameItems[(index+1)*LEN_ITEM], (MAX_ITEM-1-index)*LEN_ITEM);
	gameItems[(MAX_ITEM-1)*LEN_ITEM] = 0; itemLast--;
	if (itemOffset && itemOffset >= itemLast)
		itemOffset--;
	PrintInventory();
}

void ConvertItem(unsigned char index, unsigned int label)
{
	// Convert the name of this item
	memcpy(&gameItems[index*LEN_ITEM], &strings[label], LEN_ITEM);
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

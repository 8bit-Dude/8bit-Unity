
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
Item items[MAX_ITEM] = { { TXT_COLS-7,  TXT_ROWS-2, 0 },
						 { TXT_COLS-7,  TXT_ROWS-1, 0 } };

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


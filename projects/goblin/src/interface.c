
#include "definitions.h"

// See scene.c
extern Item items[MAX_ITEM];
extern unsigned char strings[];

// Print label in lower-left panel
void PrintInteract(unsigned char item, unsigned int label)
{
	unsigned char *iLabel;
	inkColor = INK_DEFAULT;
	txtX = 0; txtY = TXT_ROWS-2;
	if (item != 255) {
		iLabel = &strings[items[item].label];
		PrintStr("use"); txtX += 4; 
		PrintStr(iLabel); txtX += 1+strlen(iLabel); 
		PrintStr("on"); txtX += 3; 
	} 
	if (label) {
		PrintStr(&strings[label]);
	}
}

// Print message in lower-left panel
void PrintMessage(unsigned int message)
{
	unsigned char *msg;
	
	// Find the string
	if (message == 255)
		msg = "I can't do that...";
	else
		msg = &strings[message];
	
	// Reset panel
	txtX = 0; txtY = TXT_ROWS-2;
	PrintBlanks(MSG_WIDTH, 2);
	
	// Print message across two lines, by taking into account line feed '\n'
	while (*msg != 0) {
		if (*msg == 10) {
			txtX = 0; txtY++;
		} else {
			PrintChr(*msg);
			txtX++;
		}
		msg++;
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
		if (item->label)
			PrintStr(&strings[item->label]);
		else
			PrintStr("       ");
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
	SetSprite(2, frame+SPRITEFRAMES*1);	// Unit color #2
	SetSprite(3, frame+SPRITEFRAMES*2);	// Unit color #3
	SetSprite(4, frame+SPRITEFRAMES*3);	// Unit color #4
#else
	SetSprite(1, frame);
#endif
#if defined __LYNX__
	UpdateDisplay(); // Refresh Lynx screen
#endif
}

// Introduction Screen
void SplashScreen(void)
{
	// Load and show banner
	LoadBitmap("banner.img");
	ShowBitmap();
	
	// Show credit/build
	txtX = TXT_COLS-12; txtY = TXT_ROWS-4;
#if defined(__ORIC__)
	inkColor = AIC;
#elif defined(__NES__)
	inkColor = INK_DEFAULT; 
#else		
	pixelX = 0; pixelY = 0;
	paperColor = GetPixel(); 
	inkColor = INK_DEFAULT; 
#endif
	PrintStr(" TECH DEMO  "); txtY++;
	PrintStr("BY 8BIT-DUDE"); txtY++;
	PrintStr(" 2022/01/20 ");
	PlayMusic();

	// Wait until key is pressed
	while (!kbhit()) {	
	#if defined __APPLE2__
		UpdateMusic();
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif
	}	
	
	// Stop splash music
	StopMusic();	
}

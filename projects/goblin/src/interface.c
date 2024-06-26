
#include "definitions.h"

// See scene.c
extern Interact interacts[MAX_INTERACT]; 
extern unsigned char gameItems[MAX_ITEM*LEN_ITEM];
extern unsigned char strings[];
extern unsigned int unitX, unitY;

// Print label in lower-left panel
void PrintInteract(unsigned char item, unsigned char interact)
{
	unsigned char *iLabel;
	inkColor = INK_DEFAULT;
	
	// Reset panel
	txtX = 0; txtY = TXT_ROWS-2;
	PrintBlanks(MSG_WIDTH, 2);

	// Show labels of current item and interact
	if (item != 255) {
		iLabel = &gameItems[item*LEN_ITEM];
		PrintStr("use"); txtX += 4; 
		PrintStr(iLabel); txtX += 1+strlen(iLabel); 
		PrintStr("on"); txtX += 3; 
	} 
	if (interact != 255) {
		PrintStr(&strings[interacts[interact].label]);
	}
#if defined(__LYNX__) || defined(__NES__)
	UpdateDisplay(); // Refresh Lynx screen
#endif	
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

extern unsigned char itemOffset;

// Print inventory in lower-right panel
void PrintInventory(void)
{
	unsigned char i;
	inkColor = INK_INVENTORY;
		
	// Print Items
	txtX = TXT_COLS-8; 
	txtY = TXT_ROWS-2;
	PrintBlanks(7, 2);
	for (i=0; i<2; i++) {
		PrintStr(&gameItems[(i+itemOffset)*LEN_ITEM]);
		txtY++;
	}
	
	// Print Buttons
	txtX = TXT_COLS-1; 
	txtY = TXT_ROWS-2;
	PrintChr(CHR_ARROW_UP); txtY++;
	PrintChr(CHR_ARROW_DOWN);
	inkColor = INK_DEFAULT;
#if defined(__LYNX__) || defined(__NES__)
	UpdateDisplay(); // Refresh Lynx screen
#endif	
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
	// Check that unit is enabled
	if (!unitX && !unitY) return;
	
#if (defined __CBM__) 
	LocateSprite(x, y-20);
#elif (defined __ATARI__)
	LocateSprite(x, y-16);
#elif (defined __LYNX__)
	LocateSprite(x, y-14);
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

// Swap Disk
void PrintSwapDisk(unsigned char *fname)
{
	ClearBitmap();
	inkColor = WHITE; paperColor = BLACK;
	txtY = 8; 
	txtX = 6; PrintStr("File not found:");
	txtX = 22; PrintStr(fname);
	txtY = 10;
	txtX = 7; PrintStr("Swap disk and press a key");
	ShowBitmap();
	cgetc();
	HideBitmap();
}

#if (defined __LYNX__)
	unsigned char* textBuffer = SHAREDRAM;
#else
	unsigned char textBuffer[64];
#endif

extern unsigned char currmusic[13];
unsigned char menumusic[] = "menu.mus";
unsigned char menuimage[] = "menu.img";
unsigned char menutext[] = "menu.txt";

// Menu Screen
void MainMenu(void)
{
	unsigned char l, i=0;

	// Show banner
	if (FileExists(menuimage))
		LoadBitmap(menuimage);
	else
		ClearBitmap();
	
	// Show credit/build
	if (FileExists(menutext)) {
		// Load file
		FileOpen(menutext);
		l = FileRead(textBuffer, 64);
		textBuffer[l] = 0;
		FileClose();
		
		// Display data
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
		while (textBuffer[i]) {
			if (textBuffer[i] == 10) {
				txtX = TXT_COLS-12; txtY++;
		    } else
			if (textBuffer[i] != 13) {	
				PrintChr(textBuffer[i]); txtX++;
			}
			i++;
		}
	}

	// Play title music
	if (FileExists(menumusic)) {
		strcpy(currmusic, menumusic);
		LoadMusic(menumusic);	
		PlayMusic();
	}

	// All loaded-up!
	ShowBitmap();
	
	// Wait until key is pressed
#if defined __APPLE2__
	while (!kbhit())	
		UpdateMusic();
#endif
	cgetc();
	
	// Stop splash music
	StopMusic();	
}

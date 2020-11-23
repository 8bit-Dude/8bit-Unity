
#include "definitions.h"

// App/Icons parameters
extern char appSel;
extern callback *homeCall;
extern callback *appCall[4];
unsigned char* appChunk[NUM_APPS];
unsigned char* icoChunk[NUM_ICOS];

// Version definitions
char version[] = "8BIT-OS 2020/11/22";

void LoadChunks()
{
	// Load app chunks
	LoadChunk(&appChunk[APP_FILES], "files.chk");	
	LoadChunk(&appChunk[APP_IMAGE], "image.chk");	
	LoadChunk(&appChunk[APP_MUSIC], "music.chk");	
	LoadChunk(&appChunk[APP_CHAT],  "chat.chk");	
	
	// Load icon chunks	
	LoadChunk(&icoChunk[ICO_PLAY],  "play.chk");	
#ifndef __APPLE2__
	LoadChunk(&icoChunk[ICO_STOP],  "stop.chk");	
	LoadChunk(&icoChunk[ICO_PREV],  "prev.chk");	
#endif
	LoadChunk(&icoChunk[ICO_NEXT],  "next.chk");		
}

void ClearScreen()
{
	unsigned char i;
	
	// Clear usable area
	paperColor = DESK_COLOR;
	PrintBlanks(0, 0, CHR_COLS, CHR_ROWS-1);
	
	// Reset callbacks
	ClearCallbacks();
	for (i=0; i<4; i++)
		appCall[i] = 0;
}

void DrawTaskBar()
{
	paperColor = WHITE; inkColor = BLACK; 
#if (defined __ORIC__)
	SetAttributes(-1, CHR_ROWS-1, paperColor);
#endif
	PrintBlanks(0, CHR_ROWS-1, CHR_COLS, 1);
	paperColor = BLACK; inkColor = WHITE;
	homeCall = Button(0, CHR_ROWS-1, 4, 1, "HOME");	
}

// App icons location
#if (defined __LYNX__)
  #define APP_VSPAN  7
#else
  #define APP_VSPAN  10
#endif
  #define APP_COL1   2
  #define APP_ROW1   1
  #define APP_HSPAN  10

void HomeScreen(void)
{
	unsigned char i, *chunk;
	
	// Clear screen and callbacks
	ClearScreen();
	DrawTaskBar();	
	
	// Register App icons
	for (i=0; i<4; i++)
		appCall[i] = Icon(APP_COL1+i*APP_HSPAN, APP_ROW1, appChunk[i]);

	// Add Taskbar Message
	paperColor = WHITE; inkColor = BLACK; 
	PrintStr(21, CHR_ROWS-1, version);		
}

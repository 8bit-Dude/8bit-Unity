
// App/Icons parameters
callback* homeCall;
callback* appCall[4];
unsigned char* appChunk[NUM_APPS];
unsigned char* icoChunk[NUM_ICOS];
char appSel = APP_HOME;

void ClearScreen()
{
	unsigned char i;
	
	// Clear usable area
	paperColor = DESK_COLOR;
	PrintBlanks(0, 0, CHR_COLS, CHR_ROWS-1);
	
	// Reset callbacks
	for (i=0; i<4; i++)
		appCall[i] = 0;
	ClearCallbacks();
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
  #define HOME_HEIGHT 6
  #define HOME_VSPAN  7
#else
  #define HOME_HEIGHT 8
  #define HOME_VSPAN  10
#endif
  #define HOME_COL1   1
  #define HOME_ROW1   1
  #define HOME_WIDTH  8
  #define HOME_HSPAN  10

void HomeScreen(void)
{
	unsigned char i, *chunk;
	
	// Clear screen and callbacks
	ClearScreen();
	
	// Display and register APP icons
	for (i=0; i<4; i++) {
		chunk = appChunk[i]; SetChunk(chunk, chunk[0], chunk[1]);
		appCall[i] = PushCallback(i*HOME_HSPAN+HOME_COL1, HOME_ROW1, HOME_WIDTH, HOME_HEIGHT, CALLTYPE_ICON, "");
	}
	
	// Add Taskbar
	DrawTaskBar();	
	paperColor = WHITE; inkColor = BLACK; 
	PrintStr(21, CHR_ROWS-1, "8BIT-OS 2020/10/03");		
}

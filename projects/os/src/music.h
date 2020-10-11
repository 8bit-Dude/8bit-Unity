
char musicSel = 127; playing = 0;
callback* musicCall[4];

#define MUS_LOGO_X      65
#define MUS_LOGO_Y      8
#define MUS_BUTT_COL1   7
#define MUS_BUTT_ROW1   11
#define MUS_BUTT_WIDTH  4
#define MUS_BUTT_HEIGHT 3
#define MUS_BUTT_HSPAN  7

void MusicTitle()
{
	unsigned char* track = fileNames[musicSel];
	
	paperColor = BLACK; inkColor = WHITE;
	PrintBlanks(10, 7, 20, 3); 
	PrintStr(20-strlen(track)/2, 8, track);	
}

void MusicScreen(void)
{	
	unsigned char i, col;
	
	// Select file?
	if (musicSel == 127)
		SelectFile(1, musicExt, &musicSel);
	
	// Clear screen and callbacks
	ClearScreen();
	
	// Show music icon and name
	SetChunk(appChunk[APP_MUSIC], MUS_LOGO_X, MUS_LOGO_Y);
	MusicTitle();
	
	// Display and register player buttons
	for (i=0; i<4; i++) {
		col = i*MUS_BUTT_HSPAN+MUS_BUTT_COL1;
		SetChunk(icoChunk[i], ColToX(col), RowToY(MUS_BUTT_ROW1));
		musicCall[i] = PushCallback(col, MUS_BUTT_ROW1, MUS_BUTT_WIDTH, MUS_BUTT_HEIGHT, CALLTYPE_ICON, "");
	}
	
	// Add Taskbar
	DrawTaskBar();		
}

void MusicCallback(callback* call)
{
	unsigned char dir = 1;
	
	if (call == musicCall[ICO_STOP]) {
		if (playing) {
			StopMusic();
			playing = 0;
			return;
		}
	}

	if (call == musicCall[ICO_PLAY]) {
		if (!playing) {
			LoadMusic(fileNames[musicSel], MUSICRAM);
			PlayMusic(MUSICRAM);			
			playing = 1;
			return;
		}
	}
	
	if (call == musicCall[ICO_PREV] || 
	    call == musicCall[ICO_NEXT]) {	
		if (call == musicCall[ICO_PREV])
			dir = -1;
		SelectFile(dir, musicExt, &musicSel);
		MusicTitle();
		if (playing) {
			StopMusic();
			LoadMusic(fileNames[musicSel], MUSICRAM);
			PlayMusic(MUSICRAM);
		}			
	}
}

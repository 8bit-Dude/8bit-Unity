
#if (defined __ORIC__)
  #define LOGO_X    102
#else
  #define LOGO_X    65	
#endif
#define LOGO_Y      8
#define BUTT_COL1   6
#define BUTT_ROW1   11
#define BUTT_HSPAN  7

callback* musicCall[4];

void PlayTrack(char *fname)
{
	if (musicPlaying)
		StopMusic();
	LoadMusic(fname);
	PlayMusic(MUSICRAM);			
	musicPlaying = 1;
}

void PauseTrack()
{
	if (musicPlaying)
		StopMusic();
}

void UnpauseTrack()
{
	if (musicPlaying)
		PlayMusic(MUSICRAM);
}

void StopTrack()
{
	if (musicPlaying)
		StopMusic();
	musicPlaying = 0;
}

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
	DrawTaskBar();		
	
	// Show music icon and name
	SetChunk(appChunk[APP_MUSIC], LOGO_X, LOGO_Y);
	MusicTitle();
	
	// Display and register player buttons
	for (i=0; i<4; i++)
		musicCall[i] = Icon(BUTT_COL1+i*BUTT_HSPAN, BUTT_ROW1, icoChunk[i]);
}

void MusicCallback(callback* call)
{
	unsigned char dir = 1;
	
	if (call == musicCall[ICO_STOP])
		StopTrack();

	if (call == musicCall[ICO_PLAY])
		PlayTrack(fileNames[musicSel]);
	
	if (call == musicCall[ICO_PREV] || 
	    call == musicCall[ICO_NEXT]) {	
		if (call == musicCall[ICO_PREV])
			dir = -1;
		SelectFile(dir, musicExt, &musicSel);
		PlayTrack(fileNames[musicSel]);
		MusicTitle();
	}
}

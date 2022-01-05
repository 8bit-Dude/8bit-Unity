
#include "definitions.h"

#if defined(__NES__)
 #pragma rodata-name("BANK0")
 #pragma code-name("BANK0")
#endif

#if (defined __NES__)
  #define LOGO_X     104	
  #define LOGO_Y     8
  #define BUTT_COL1  4
  #define BUTT_ROW1  11
  #define BUTT_HSPAN 7
#elif (defined __ORIC__)
  #define LOGO_X     102
  #define LOGO_Y     8
  #define BUTT_COL1  6
  #define BUTT_ROW1  11
  #define BUTT_HSPAN 7
#elif (defined __APPLE2__)
  #define LOGO_X     56
  #define LOGO_Y     8
  #define BUTT_COL1  6
  #define BUTT_ROW1  11
  #define BUTT_HSPAN 7
#else
  #define LOGO_X     65	
  #define LOGO_Y     8
  #define BUTT_COL1  6
  #define BUTT_ROW1  11
  #define BUTT_HSPAN 7
#endif

#define MUSIC_STOPPED 0
#define MUSIC_PLAYING 1
#define MUSIC_PAUSED  2

// See Unity
extern unsigned char* fileNames[];

// See home.c
extern unsigned char* chunkData[NUM_CHUNKS];

char musicSel = 127, musicState = 0;
callback* musicCall[4];

void PlayTrack(char *fname)
{
	if (musicState)
		StopMusic();
	LoadMusic(fname);
	PlayMusic();
#ifndef __APPLE2__	
	musicState = MUSIC_PLAYING;
#else
	if (kbhit())
		cgetc();
	while (!kbhit())
		UpdateMusic();
	cgetc();
#endif
}

void StopTrack()
{
	if (musicState)
		StopMusic();
	musicState = MUSIC_STOPPED;
}

void PauseTrack()
{
	if (musicState == MUSIC_PLAYING) {
		musicState = MUSIC_PAUSED;
		PauseMusic(1);
	}
}

void UnpauseTrack()
{
	if (musicState == MUSIC_PAUSED) {
		musicState = MUSIC_PLAYING;
		PauseMusic(0);
	}
}

void MusicTitle()
{
	unsigned char* track = fileNames[musicSel];
	
	paperColor = BLACK; inkColor = WHITE;
	txtX = (TXT_COLS)/2-10; txtY = 7;	
	PrintBlanks(20, 3); 
	txtX = (TXT_COLS)/2-strlen(track)/2u; txtY = 8;	
	PrintStr(track);	
}

void MusicScreen(void)
{	
	unsigned char i;
	
	// Select file?
	if (musicSel == 127)
		SelectFile(1, musicExt, &musicSel);
	
	// Clear screen and callbacks
	ClearScreen();
	DrawTaskBar();		
	
	// Show music icon and name
	SetChunk(chunkData[APP_MUSIC], LOGO_X, LOGO_Y);
	MusicTitle();
	
	// Display and register player buttons
	for (i=0; i<4; i++)
		musicCall[i] = Icon(BUTT_COL1+i*BUTT_HSPAN, BUTT_ROW1, chunkData[ICO_STOP+i]);
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
		MusicTitle();
		PlayTrack(fileNames[musicSel]);
	}
}

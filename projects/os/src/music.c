
#include "definitions.h"

#if (defined __ORIC__)
  #define LOGO_X    102
#elif (defined __APPLE2__)
  #define LOGO_X    56
#else
  #define LOGO_X    65	
#endif
#define LOGO_Y      8
#define BUTT_COL1   6
#define BUTT_ROW1   11
#define BUTT_HSPAN  7

// See Unity
extern unsigned char* fileNames[];

// See home.c
extern unsigned char* appChunk[NUM_APPS];
extern unsigned char* icoChunk[NUM_ICOS];

char musicSel = 127, musicPlaying = 0;

callback* musicCall[4];

void PlayTrack(char *fname)
{
	if (musicPlaying)
		StopMusic();
	LoadMusic(fname, MUSICRAM);
	PlayMusic();
#ifndef __APPLE2__	
	musicPlaying = 1;
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
	if (musicPlaying)
		StopMusic();
	musicPlaying = 0;
}

void MusicTitle()
{
	unsigned char* track = fileNames[musicSel];
	
	paperColor = BLACK; inkColor = WHITE;
	PrintBlanks(10, 7, 20, 3); 
	PrintStr(20-strlen(track)/2u, 8, track);	
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
		MusicTitle();
		PlayTrack(fileNames[musicSel]);
	}
}

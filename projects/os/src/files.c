
#include "definitions.h"

#if defined(__NES__)
 #pragma rodata-name("BANK0")
 #pragma code-name("BANK0")
#endif

// See Unity
extern unsigned char  fileNum;     
extern unsigned int   fileSizes[];  
extern unsigned char* fileNames[];

// See home.c
extern unsigned char* chunkData[NUM_CHUNKS];

// See image.c
extern char imageShowing;

#if defined(__NES__)
 #pragma bss-name(push, "XRAM")
#endif

unsigned char listNum = 0;
unsigned char listNames[16][17];
unsigned char listIds[16];

#if defined(__NES__)
 #pragma bss-name(pop)
#endif

void GetFileList()
{
	unsigned char i, len, dest, *ext;
	unsigned int size;
	
	// Get file list
	DirList();

	// Build media file list
	for (i=0; i<fileNum; i++) {
		// Check for allowed file extensions
		len = strlen(fileNames[i]);
		ext = &fileNames[i][len-3];
		if (strncmp(ext, imageExt, 3) && strncmp(ext, musicExt, 3) && strncmp(ext, textExt, 3))
			continue;
	#if defined(__NES__)
		if (!strncmp(fileNames[i], "bench.img", 9))
			continue;
	#endif	
		
		// Allocate string for name + size
		memset(listNames[listNum], 32, 15);
		listNames[listNum][16] = 0;
		listIds[listNum] = i;
		
		// Copy over name + size
		memcpy(listNames[listNum], fileNames[i], len);
		size = fileSizes[i];
		if (size<1000) {
			if (size < 10) {
				dest = 15;
			} else if (size < 100) {
				dest = 14;
			} else if (size < 1000) {
				dest = 13;
			}
			itoa(size, &listNames[listNum][dest], 10);			
		} else {
			size /= 1000u;
			if (size < 10) {
				dest = 14;
			} else {
				dest = 13;
			}
			itoa(size, &listNames[listNum][dest], 10);			
			listNames[listNum][15] = 'k';
		}
		
		// Increment counters
		listNum++;
	}
}

void SelectFile(char dir, unsigned char* extension, char* fileSel)
{
	unsigned char len;
	
	// Select file
	while (1) {
		// Move index (skip first file)
		*fileSel += dir;
		if (*fileSel > fileNum) *fileSel = fileNum-1;
		if (*fileSel == fileNum) *fileSel = 0;
						
		// Check if file extension matches
		len = strlen(fileNames[*fileSel]);
		if (!strncmp(&fileNames[*fileSel][len-3], extension, 3))
		#if defined(__NES__)
		  if (strncmp(fileNames[*fileSel], "bench.img", 9))
		#endif				
			break;
	}
}

#if (defined __APPLE2__)
  #define PREVIEW_X 91
  #define PREVIEW_Y 16
  #define BUTT_COL  26
  #define BUTT_ROW  9
#elif (defined __LYNX__)
  #define PREVIEW_X 100
  #define PREVIEW_Y 12
  #define BUTT_COL  26
  #define BUTT_ROW  9
#elif (defined __NES__)
  #define PREVIEW_X 168
  #define PREVIEW_Y 16
  #define BUTT_COL  22
  #define BUTT_ROW  9
#elif (defined __ORIC__)
  #define PREVIEW_X 154
  #define PREVIEW_Y 16
  #define BUTT_COL  26
  #define BUTT_ROW  9
#else
  #define PREVIEW_X 100
  #define PREVIEW_Y 16
  #define BUTT_COL  26
  #define BUTT_ROW  9
#endif

#if (defined __LYNX__)
	unsigned char* textBuffer = SHAREDRAM;
#else
	unsigned char textBuffer[256];
#endif
callback *callImg, *callMus; 
char *currFile, *currExt;

void PreviewImage(void)
{
	ClearScreen();
	PauseTrack();
	LoadBitmap(currFile);
	UnpauseTrack();
	DrawTaskBar();
	callImg = PushCallback(0, 0, TXT_COLS, TXT_ROWS, CALLTYPE_ICON);
	imageShowing = 1;
}

void PreviewText(void)
{
	unsigned char *addr, *end;
	unsigned char size;
	
	// Read text file
	PauseTrack();
	if (FileOpen(currFile)) {
		size = FileRead(textBuffer, 256);
		textBuffer[size] = 0;
		FileClose();
	}
	UnpauseTrack();

	// Display in preview box
	paperColor = DESK_COLOR; inkColor = BLACK;
	addr = textBuffer;
	end = addr+strlen(textBuffer);
	txtX=18; txtY=1; 
	while (addr<end) {
		if (*addr == '\n') {
			txtX = 18; txtY++;
		} else {
			PrintChr(*addr); txtX++; 
			if (txtX>TXT_COLS-2) { txtX = 18; txtY++; }
		} addr++;
	}
}

void FilesScreen(void)
{			
	// Clear screen
	ClearScreen();
	DrawTaskBar();		

	// Display Media files in ListBox
#if defined(__NES__)	
	inkColor = WHITE;
#else
	paperColor = DESK_COLOR; inkColor = BLACK;
#endif
	Panel(18, 0, TXT_COLS-19, TXT_ROWS-2, "Preview");	
	ListBox(1, 0, 16, TXT_ROWS-2, "Files", listNames, listNum);
	imageShowing = 0;
}

void FileCallback(callback* call)
{
	// Handle listbox calls
	if (call->type == CALLTYPE_LISTBOX) {		
		// Reset preview area
	#ifndef __NES__	
		paperColor = DESK_COLOR;
		txtX = 18; txtY = 1;
	  #if (defined __APPLE2__)
		PrintBlanks(TXT_COLS-20, TXT_ROWS-3);
	  #else
		PrintBlanks(TXT_COLS-19, TXT_ROWS-3);
	  #endif
	#endif
	
		// Clear preview callbacks
		if (callImg) { PopCallback(callImg); callImg = 0; }
		if (callMus) { PopCallback(callMus); callMus = 0; }
	
		// Get file name
		currFile = (char*)fileNames[listIds[call->value]];
		currExt = &currFile[strlen(currFile)-3];
		
		// Controls for Image File
		if (!strncmp(currExt, imageExt, 3)) {
			SetChunk(chunkData[APP_IMAGE], PREVIEW_X, PREVIEW_Y);
			callImg = Icon(BUTT_COL, BUTT_ROW, chunkData[ICO_PLAY]);
			return;
		}
		
		// Controls for Music File
		if (!strncmp(currExt, musicExt, 3)) {
			SetChunk(chunkData[APP_MUSIC], PREVIEW_X, PREVIEW_Y);	
			callMus = Icon(BUTT_COL, BUTT_ROW, chunkData[ICO_PLAY]);
			return;		
		}
		
		// Preview Text File
		if (!strncmp(currExt, textExt, 3)) {
			PreviewText(); 
			return;
		}
	} else 
	if (call == callImg) {
		if (imageShowing) {
			// Return to files screen
			FilesScreen();
		} else {
			// Show image and make entire screen callable
			PreviewImage();
		}
	} else 
	if (call == callMus) {
		PlayTrack(currFile);
	}
}

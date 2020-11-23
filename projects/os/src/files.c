
#include "definitions.h"

// See Unity
extern unsigned char  fileNum;     
extern unsigned int   fileSizes[];  
extern unsigned char* fileNames[];

// See home.c
extern unsigned char* appChunk[NUM_APPS];
extern unsigned char* icoChunk[NUM_ICOS];

// See image.c / music.c
extern char imageShowing;
extern char musicPlaying;

unsigned char  listNum = 0;
unsigned char* listNames[16];
unsigned char  listIds[16];

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
		
		// Allocate string for name + size
		listIds[listNum] = i;
		listNames[listNum] = malloc(17);
		memset(listNames[listNum], 32, 15);
		listNames[listNum][16] = 0;
		
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
			break;
	}
}

#if (defined __LYNX__)
  #define PREVIEW_X 100
  #define PREVIEW_Y 12
#elif (defined __ORIC__)
  #define PREVIEW_X 154
  #define PREVIEW_Y 16
#elif (defined __APPLE2__)
  #define PREVIEW_X 91
  #define PREVIEW_Y 16
#else
  #define PREVIEW_X 100
  #define PREVIEW_Y 16
#endif
  #define BUTT_COL 26
  #define BUTT_ROW 9

#if (defined __LYNX__)
	unsigned char* textBuffer;
#else
	unsigned char textBuffer[256];
#endif
callback *callImg, *callMus; 
char *currFile, *currExt;

void PreviewImage(void)
{
	ClearScreen();
	musicPaused = 1;
	LoadBitmap(currFile);
	musicPaused = 0;
	DrawTaskBar();
	callImg = PushCallback(0, 0, CHR_COLS, CHR_ROWS, CALLTYPE_ICON);
	imageShowing = 1;
}

void PreviewText(void)
{
	unsigned char x, y; 
	unsigned int addr, end;
	
	// Read text file
#if (defined __APPLE2__)
	// Try to open file
	FILE* fp = fopen(currFile, "rb");
	fread(textBuffer, 1, 256, fp);
	fclose(fp);
#elif (defined __ATARI__)
	musicPaused = 1;
	if (FileOpen(currFile))
		FileRead(textBuffer, 256);
	musicPaused = 0;
#elif (defined __CBM__)
	FILE* fp = fopen(currFile, "rb");
	fread(textBuffer, 1, 256, fp);
	fclose(fp);
#elif (defined __LYNX__)
	textBuffer = (char*)SHAREDRAM;
	bzero(SHAREDRAM, 256);
	FileRead(currFile);
#elif (defined __ORIC__)
	musicPaused = 1;
	FileRead(currFile, textBuffer);
	musicPaused = 0;
#endif

	// Display in preview box
	paperColor = DESK_COLOR; inkColor = BLACK;
	addr = (unsigned int)textBuffer;
	end = addr+strlen(textBuffer);
	x=18; y=1; 
	while (addr<end) {
		if (*(char*)(addr) == '\n') {
			x = 18; y+=1;
		} else {
			PrintChr(x, y, GetChr(*(char*)(addr)));
			x += 1; if (x>CHR_COLS-2) { x = 18; y+=1; }
		} addr++;
	}
}

void FilesScreen(void)
{			
	// Clear screen
	ClearScreen();
	DrawTaskBar();		

	// Display Media files in ListBox
	paperColor = DESK_COLOR; inkColor = BLACK;
	ListBox(1, 0, 16, CHR_ROWS-2, "Files", listNames, listNum);
	Panel(18, 0, CHR_COLS-19, CHR_ROWS-2, "Preview");	
	imageShowing = 0;
}

void FileCallback(callback* call)
{
	unsigned char i;
	
	// Handle listbox calls
	if (call->type == CALLTYPE_LISTBOX) {		
		// Reset preview area
		paperColor = DESK_COLOR;
	#if (defined __APPLE2__)
		PrintBlanks(18, 1, CHR_COLS-20, CHR_ROWS-3);
	#else
		PrintBlanks(18, 1, CHR_COLS-19, CHR_ROWS-3);
	#endif
	
		// Clear preview callbacks
		if (callImg) { PopCallback(callImg); callImg = 0; }
		if (callMus) { PopCallback(callMus); callMus = 0; }
	
		// Get file name
		currFile = (char*)fileNames[listIds[call->value]];
		currExt = &currFile[strlen(currFile)-3];
		
		// Controls for Image File
		if (!strncmp(currExt, imageExt, 3)) {
			SetChunk(appChunk[APP_IMAGE], PREVIEW_X, PREVIEW_Y);
			callImg = Icon(BUTT_COL, BUTT_ROW, icoChunk[ICO_PLAY]);
			return;
		}
		
		// Controls for Music File
		if (!strncmp(currExt, musicExt, 3)) {
			SetChunk(appChunk[APP_MUSIC], PREVIEW_X, PREVIEW_Y);	
			callMus = Icon(BUTT_COL, BUTT_ROW, icoChunk[ICO_PLAY]);
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
		if (musicPlaying)
			StopTrack();
		else
			PlayTrack(currFile);
	}
}

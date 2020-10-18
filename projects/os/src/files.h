
#if defined(__ATARI__) || defined(__LYNX__) || defined(__ORIC__)
  extern unsigned char  fileNum;     
  extern unsigned int   fileSizes[];  
  extern unsigned char* fileNames[];
#else
  unsigned char  fileNum = 0;
  unsigned int   fileSizes[32];  
  unsigned char* fileNames[32];
#endif

unsigned char  mediaNum = 0;
unsigned char* mediaNames[32];

void GetFileList()
{
	unsigned char i, len, dest, *ext;
	unsigned int size;
	
	// Retrieve file list
#if defined(__ATARI__) || defined(__ORIC__)
	FileList();
	
#elif defined(__APPLE2__)
	DIR *dir;
	struct dirent *dp;
	if ((dir = opendir(".")) != NULL) {
		do {
			dp = readdir(dir);
			if (dp != NULL) {
				fileNames[fileNum] = malloc(strlen(dp->d_name));
				strcpy(fileNames[fileNum], dp->d_name);
				fileNum++;
			}
		} while (dp != NULL);
		closedir(dir);
	}
#endif

	// Process media file list
	for (i=0; i<fileNum; i++) {
		// Check for allowed file extensions
		len = strlen(fileNames[i]);
		ext = &fileNames[i][len-3];
		if (strncmp(ext, imageExt, 3) && strncmp(ext, musicExt, 3) && strncmp(ext, textExt, 3))
			continue;
		
		// Allocate string for name + size
		mediaNames[mediaNum] = malloc(17);
		memset(mediaNames[mediaNum], 32, 15);
		mediaNames[mediaNum][16] = 0;
		
		// Copy over name + size
		memcpy(mediaNames[mediaNum], fileNames[i], len);
		size = fileSizes[i];
		if (size<1000) {
			if (size < 10) {
				dest = 15;
			} else if (size < 100) {
				dest = 14;
			} else if (size < 1000) {
				dest = 13;
			}
			itoa(size, &mediaNames[mediaNum][dest], 10);			
		} else {
			size /= 1000;
			if (size < 10) {
				dest = 14;
			} else {
				dest = 13;
			}
			itoa(size, &mediaNames[mediaNum][dest], 10);			
			mediaNames[mediaNum][15] = 'k';
		}
		
		// Increment counters
		mediaNum++;
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
char currFile[17];
callback *callImg, *callMus; 

void PlayTrack(char *fname);
void PauseTrack(void);
void UnpauseTrack(void);
void StopTrack(void);

void PreviewText(void)
{
	unsigned char x, y; 
	unsigned int addr, end;
	
	// Read text file
#if (defined __ATARI__)
	if (FileOpen(currFile))
		FileRead(textBuffer, 256);
#elif (defined __LYNX__)
	textBuffer = (char*)SHAREDRAM;
	bzero(SHAREDRAM, 256);
	FileRead(currFile);
#elif (defined __ORIC__)
	FileRead(currFile, textBuffer);
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
	ListBox(1, 0, 16, CHR_ROWS-2, "Files", mediaNames, mediaNum);
	Panel(18, 0, CHR_COLS-19, CHR_ROWS-2, "Preview");	
	imageShowing = 0;
}

void FileCallback(callback* call)
{
	unsigned char i;
	
	// Handle listbox calls
	if (call->type == CALLTYPE_LISTBOX) {		
		// Clear preview callbacks
		if (callImg) { PopCallback(callImg); callImg = 0; }
		if (callMus) { PopCallback(callMus); callMus = 0; }
	
		// Reset preview area
		paperColor = DESK_COLOR;
		PrintBlanks(18, 1, CHR_COLS-19, CHR_ROWS-3);

		// Get file name without size
		memcpy(currFile, call->label, 17);
		i=0; while (i<17) {
			if (currFile[i] == '.') {
				currFile[i+4] = 0; i++;
				break;
			} i++;
		}
		
		// Controls for Image File
		if (!strncmp(&currFile[i], imageExt, 3)) {
			SetChunk(appChunk[APP_IMAGE], PREVIEW_X, PREVIEW_Y);
			callImg = Icon(BUTT_COL, BUTT_ROW, icoChunk[ICO_PLAY]);
			return;
		}
		
		// Controls for Music File
		if (!strncmp(&currFile[i], musicExt, 3)) {
			SetChunk(appChunk[APP_MUSIC], PREVIEW_X, PREVIEW_Y);	
			callMus = Icon(BUTT_COL, BUTT_ROW, icoChunk[ICO_PLAY]);
			return;		
		}
		
		// Preview Text File
		if (!strncmp((char*)&currFile[i], textExt, 3)) {
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
			ClearScreen();
			//PauseTrack(); 
			LoadBitmap(currFile);
			//UnpauseTrack(); 
			DrawTaskBar();
			callImg = PushCallback(0, 0, CHR_COLS, CHR_ROWS, CALLTYPE_ICON, "");
			imageShowing = 1;
		}
	} else 
	if (call == callMus) {
		if (musicPlaying)
			StopTrack();
		else
			PlayTrack(currFile);
	}
}

#if defined(__LYNX__) || defined(__ORIC__)
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
#if defined(__ORIC__)
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
  #define FILE_TYPE_X 100
  #define FILE_TYPE_Y 16
#else
  #define FILE_TYPE_X 200
  #define FILE_TYPE_Y 32
  char text[440];
#endif

char fname[17];

void FilesScreen(void)
{			
	// Clear screen
	ClearScreen();

	// Display Media files in ListBox
	ListBox(1, 0, 16, CHR_ROWS-2, "Files", mediaNames, mediaNum);
	Panel(18, 0, CHR_COLS-19, CHR_ROWS-2, "Preview");	
	
	// Add Taskbar
	DrawTaskBar();		
}

void FileCallback(callback* call)
{
	unsigned char i;
	unsigned int x, y, addr, end;
	
	// Reset preview area
	paperColor = BLUE;
	PrintBlanks(18, 1, CHR_COLS-19, CHR_ROWS-3);

	// Get file name without size
	memcpy(fname, call->label, 17);
	i=0; while (i<17) {
		if (fname[i] == '.') {
			fname[i+4] = 0;
			break;
		} i++;
	}
	
	// Preview Image File
	if (!strncmp(&fname[i], ".img",4)) {
		// Create callback for player buttons
		SetChunk(appChunk[APP_IMAGE], FILE_TYPE_X, FILE_TYPE_Y);		
		return;
	}
	
	// Preview Music File
	if (!strncmp(&fname[i], ".mus",4)) {
		// Create callback for player buttons
		SetChunk(appChunk[APP_MUSIC], FILE_TYPE_X, FILE_TYPE_Y);	
		return;		
/*		if (playing)
			StopMusic();
		LoadMusic(fname, MUSICRAM);
		PlayMusic(MUSICRAM);
		playing = 1;   */
	}
	
	// Preview Text File
	if (!strncmp((char*)&fname[i], ".txt",4)) {
	#if (defined __LYNX__)
		addr = SHAREDRAM;
		end = addr+FileLoad(fname);
	#elif (defined __ORIC__)
		FileRead(fname, text);
		addr = (unsigned int)text;
		end = addr+strlen(text);
	#endif
		i=0; x=18; y=1; 
		while (addr<end) {
			if (*(char*)(addr) == '\n') {
				x = 18; y+=1;
			} else {
				PrintChr(x, y, GetChr(*(char*)(addr)));
				x += 1; if (x>38) { x = 18; y+=1; }
			} addr++;
		}
	}
}

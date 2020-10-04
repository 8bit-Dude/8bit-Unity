 
#include "unity.h"

// GUI parameters
#define DESK_COLOR BLUE

// App definitions
#define APP_HOME   0
#define APP_FILES  1
#define APP_BITMAP 2
#define APP_MUSIC  3
#define APP_CHAT   4
#define NUM_APPS   5

// Icon definitions
#define ICO_MUSIC  0
#define ICO_STOP   1
#define ICO_PLAY   2
#define ICO_PREV   3
#define ICO_NEXT   4
#define NUM_ICOS   5

// Icons Chunks
unsigned char* icon[NUM_ICOS];

// Mouse sprite definitions
#define spriteFrames 2
#if defined __APPLE2__	
	#define spriteCols   7
	#define spriteRows   7
	unsigned char spriteColors[] = { };  // Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	#define spriteCols   8
	#define spriteRows   6
	unsigned char spriteColors[] = { 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // Refer to atari palette in docs
#elif defined __CBM__
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { WHITE, BLUE, BROWN, PINK, GREEN, WHITE, WHITE, WHITE, BLACK, WHITE };	// 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	#define spriteCols   9
	#define spriteRows   13
	unsigned char spriteColors[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }; // Default palette
#elif defined __ORIC__
	#define spriteCols   12
	#define spriteRows   8
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };  // AIC color allows faster drawing!
#endif

// Mouse state
unsigned char mouseMove = 1;
unsigned char mouseButt = 0;
unsigned char mouseLock = 0;
unsigned int mouseX = 160;
unsigned int mouseY = 100;

void UpdateMouse()
{
	unsigned char joy;
	if (!mouseMove) return;
	
	// Read mouse state from joystick
	joy = GetJoy(0);
	if (joy & JOY_UP)    { mouseY+=4; if (mouseY>200) {mouseY = 200;} }
	if (joy & JOY_DOWN)  { mouseY-=4; if (mouseY>200) {mouseY = 0;} }
	if (joy & JOY_LEFT)  { mouseX+=4; if (mouseX>320) {mouseX = 320;} }
	if (joy & JOY_RIGHT) { mouseX-=4; if (mouseX>320) {mouseX = 0;} }
	if (joy & JOY_BTN1)  { mouseButt = 0; } else { mouseButt = 1; } 
	
	// Update 
	LocateSprite(mouseX+4, mouseY+4);
	if (mouseButt) SetSprite(0, 1); else SetSprite(0, 0);
}

///////////////////////////////////////////////////////////////////

unsigned char inputMode, inputCol, inputRow, inputLen, *inputBuffer;

#if defined __LYNX__ 
	#define kbhit KeyboardOverlayHit
	#define cgetc GetKeyboardOverlay
#endif

void SetInput(callback* call) 
{
	inputMode = 1;
	inputCol = call->colBeg;
	inputRow = call->rowBeg;
	inputLen = call->colEnd - inputCol - 1;
	inputBuffer = call->label;
#if defined __LYNX__ 
	SetKeyboardOverlay(60,70);
	ShowKeyboardOverlay();
	DisableSprite(0);
	mouseMove = 0;
	cgetc();
#endif
}

void UpdateInput()
{
	unsigned char lastKey;
	if (kbhit()) {
		lastKey = cgetc();
		if (inputMode) {
			paperColor = WHITE; inkColor = BLACK;
			if (InputUpdate(inputCol, inputRow, inputBuffer, inputLen, lastKey)) {
				inputMode = 0;
			#if defined __LYNX__
				HideKeyboardOverlay();	
				EnableSprite(0);			
				mouseMove = 1;
			#endif		
			}			
		}
	}
}

///////////////////////////////////////////////////////////////////

#if defined(__LYNX__)
  #define FIRST_FILE 1
#elif defined(__ORIC__)
  #define FIRST_FILE 3
#endif

#if defined(__LYNX__) || defined(__ORIC__)
	extern unsigned char  fileNum;     
	extern unsigned int   fileSizes[];  
	extern unsigned char* fileNames[];
#else
	unsigned char  fileNum;
	unsigned int   fileSizes[32];  
	unsigned char* fileNames[32];
#endif

void GetFileList()
{
	// Retrieve file list
#if defined(__ORIC__)
	FileList();
	
#elif defined(__APPLE2__) || defined(__ATARI__) || defined(__CBM__)
	DIR *dir;
	struct dirent *dp;
	fileNum = 0;
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
}

void SelectFile(char dir, unsigned char* extension, char* fileSel)
{
	unsigned char len;
	
	// Select file
	while (1) {
		// Move index (skip first file)
		*fileSel += dir;
		if (*fileSel < FIRST_FILE) *fileSel = fileNum-1;
		if (*fileSel >= fileNum) *fileSel = FIRST_FILE;
				
		// Check file type
		len = strlen(fileNames[*fileSel]);
				
		// Check extensions
		if (!strncmp(&fileNames[*fileSel][len-3], extension, 3)) 
			break;
	}
}

///////////////////////////////////////////////////////////////////

callback *home, *files, *bitmap, *music, *chat;

void ClearScreen()
{
	// Clear usable area
	paperColor = DESK_COLOR;
	PrintBlanks(0, 0, CHR_COLS, CHR_ROWS-1);
	
	// Reset callbacks
	home = files = bitmap = music = chat = 0;
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
	home = Button(0, CHR_ROWS-1, 4, 1, "HOME");	
}

void HomeScreen(void)
{
	unsigned char i, height, row1, row2;
#if (defined __LYNX__)
	height = 6; row1 = 1; row2 = 8;
#else
	height = 8; row1 = 1; row2 = 11;
#endif
	
	// Clear screen and callbacks
	ClearScreen();
	
	// Load Home screen
	LoadBitmap("apps.img");
	
	// Setup icons callbacks
	files  = PushCallback( 1, row1, 8, height, CALLTYPE_ICON, "FILES");
	bitmap = PushCallback(11, row1, 8, height, CALLTYPE_ICON, "BITMAP");
	music  = PushCallback(21, row1, 8, height, CALLTYPE_ICON, "MUSIC");
	chat   = PushCallback(31, row1, 8, height, CALLTYPE_ICON, "CHAT");
	
	// Add Taskbar
	DrawTaskBar();	
	paperColor = WHITE; inkColor = BLACK; 
	PrintStr(21, CHR_ROWS-1, "8BIT-OS 2020/10/03");		
}

///////////////////////////////////////////////////////////////////

char bitmapSel = 127;
callback *bPrev, *bNext;

void BitmapCallback(callback* call)
{	
	// Search direction
	char dir = 1;
	if (call == bPrev)
		dir = -1;

	// Clear screen and callbacks first!
	ClearScreen();
	
	// Select file, and display it
	SelectFile(dir, "img", &bitmapSel);
	LoadBitmap(fileNames[bitmapSel]);
	
	// Add Taskbar
	DrawTaskBar();
	
	// Display file name
	paperColor = WHITE; inkColor = BLACK; 
	PrintStr(14, CHR_ROWS-1, fileNames[bitmapSel]);		
	
	// Add Controllers
	paperColor = BLACK; inkColor = WHITE;
	bPrev = Button(10, CHR_ROWS-1, 3, 1, " ( ");	
	bNext = Button(27, CHR_ROWS-1, 3, 1, " ) ");	
}

void BitmapScreen(void)
{		
	BitmapCallback(0);
}

///////////////////////////////////////////////////////////////////

char musicSel = 127; dir = 1; playing = 0;
callback* mPrev, *mStop, *mPlay, *mNext;

void MusicTitle()
{
	unsigned char* track = fileNames[musicSel];
	
	paperColor = BLACK; inkColor = WHITE;
	PrintBlanks(10, 7, 20, 3); 
	PrintStr(20-strlen(track)/2, 8, track);	
}

void MusicCallback(callback* call)
{
	if (call == mStop) {
		if (playing) {
			StopMusic();
			playing = 0;
			return;
		}
	}

	if (call == mPlay) {
		if (!playing) {
			LoadMusic(fileNames[musicSel], MUSICRAM);
			PlayMusic(MUSICRAM);			
			playing = 1;
			return;
		}
	}
	
	if (call == mPrev || call == mNext) {	
		if (call == mPrev)
			dir = -1;
		else 
			dir = 1;
		SelectFile(dir, "mus", &musicSel);
		MusicTitle();
		if (playing) {
			StopMusic();
			LoadMusic(fileNames[musicSel], MUSICRAM);
			PlayMusic(MUSICRAM);
		}			
	}
}

#if (defined __LYNX__)
  #define MUS_COL  65
  #define MUS_ROW  8
  #define ICO_COL1 28
  #define ICO_COL2 58
  #define ICO_COL3 88
  #define ICO_COL4 118
  #define ICO_ROW  66
#elif (defined __ORIC__)
  #define MUS_COL  114
  #define MUS_ROW  8
  #define ICO_COL1 42
  #define ICO_COL2 84
  #define ICO_COL3 126
  #define ICO_COL4 168
  #define ICO_ROW  88
#endif

void MusicScreen(void)
{		
	// Clear screen and callbacks
	ClearScreen();
	
	// Select file?
	if (musicSel == 127)
		SelectFile(1, "mus", &musicSel);

	// Show music icon and name
	SetChunk(icon[ICO_MUSIC], MUS_COL, MUS_ROW);
	MusicTitle();
	
	// Create callback for player buttons
	SetChunk(icon[ICO_PREV], ICO_COL1, ICO_ROW);
	SetChunk(icon[ICO_STOP], ICO_COL2, ICO_ROW);
	SetChunk(icon[ICO_PLAY], ICO_COL3, ICO_ROW);
	SetChunk(icon[ICO_NEXT], ICO_COL4, ICO_ROW);
	mPrev = PushCallback( 7, 11, 4, 3, CALLTYPE_ICON, "PREV");
	mStop = PushCallback(14, 11, 4, 3, CALLTYPE_ICON, "STOP");
	mPlay = PushCallback(21, 11, 4, 3, CALLTYPE_ICON, "PLAY");
	mNext = PushCallback(28, 11, 4, 3, CALLTYPE_ICON, "NEXT");	
	
	// Add Taskbar
	DrawTaskBar();		
}

///////////////////////////////////////////////////////////////////

#if (defined __LYNX__)
  #define BMP_SIZE 8365
#elif (defined __ORIC__)
  #define BMP_SIZE 8000
  char text[8];	// TODO: INCREASE SPARE RAM!
#endif

unsigned char* fileList[32];
char fname[17], frame[BMP_SIZE];

void FileCallback(callback* call)
{
	unsigned char i;
	unsigned int x, y, addr, end;
	
	// Get file name without size
	memcpy(fname, call->label, 17);
	i=0; while (i<17) {
		if (fname[i] == '.') {
			fname[i+4] = 0;
			break;
		} i++;
	}
	
	// Display file name
	paperColor = WHITE;
	PrintBlanks(23, CHR_ROWS-1, 16, 1); 
	PrintStr(23, CHR_ROWS-1, fname);	
	
	// Reset preview area
	paperColor = BLUE;
	PrintBlanks(18, 1, CHR_COLS-19, CHR_ROWS-3);
	
	// Preview Image File
	if (!strncmp(&fname[i], ".img",4)) {
	#if (defined __LYNX__)
		// Backup current frame and load image
		memcpy(frame, (char*)(BITMAPRAM), BMP_SIZE);
		autoRefresh = 0;
		LoadBitmap(fname);	
		autoRefresh = 1;
		
		// Copy every second col/row to backup frame
		for (y=0; y<51; y++) { 
			for (x=0; x<40; x++) {
				POKE(&frame[(y+8)*82+(x+37)+1], *(char*)(BITMAPRAM+y*164+x*2+1)); 
			}
		}		
		
		// Restore backup
		memcpy((char*)(BITMAPRAM), frame, BMP_SIZE);								
		
	#elif (defined __ORIC__)
		// Load image to frame
		FileRead(fname, frame);
		
		// Copy every second group of 6x2 pixels
		for (y=0; y<100; y++) { 
			for (x=1; x<20; x++) {
				POKE((char*)(BITMAPRAM+y*40+x+440+19), frame[y*80+x*2]); 
				POKE((char*)(BITMAPRAM+y*40+x+480+19), frame[y*80+x*2+40]); 
			}
		}		
	#endif
		return;
	}
	
	// Preview Music File
	if (!strncmp(&fname[i], ".mus",4)) {
		if (playing)
			StopMusic();
		LoadMusic(fname, MUSICRAM);
		PlayMusic(MUSICRAM);
		playing = 1;
		return;
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

void FilesScreen(void)
{			
	unsigned char i, j = 0, len, dest;
	unsigned int size;
	
	// Clear screen
	ClearScreen();
	
	// Create list for display
	inkColor = BLACK;
	for (i=FIRST_FILE; i< fileNum; i++) {
		// Check extensions
		len = strlen(fileNames[i]);
		if (!strncmp(&fileNames[i][len-3], "chk", 3))
			continue;
		
		// Allocate string for name + size
		fileList[j] = malloc(17);
		memset(fileList[j], 32, 15);
		fileList[j][16] = 0;
		
		// Copy over name + size
		memcpy(fileList[j], fileNames[i], len);
		size = fileSizes[i];
		if (size<1000) {
			if (size < 10) {
				dest = 15;
			} else if (size < 100) {
				dest = 14;
			} else if (size < 1000) {
				dest = 13;
			}
			itoa(size, &fileList[j][dest], 10);			
		} else {
			size /= 1000;
			if (size < 10) {
				dest = 14;
			} else {
				dest = 13;
			}
			itoa(size, &fileList[j][dest], 10);			
			fileList[j][15] = 'k';
		}
		
		// Increment counters
		j++;
	}

	// List directory
	ListBox(1, 0, 16, CHR_ROWS-2, "Files", fileList, j);
	Panel(18, 0, CHR_COLS-19, CHR_ROWS-2, "Preview");	
	
	// Add Taskbar
	DrawTaskBar();		
}

///////////////////////////////////////////////////////////////////

#define REQ_LOGIN 	1
#define REQ_PAGE 	2
#define REQ_RECV 	3
#define REQ_SEND 	4
#define REQ_HEADER  85
#define REQ_ERROR   170

unsigned char lineX1, lineX2, lineY, chatConnected, chatLogged, chatLen;
unsigned int chatList[4] = {0, 0, 0, 0};
unsigned char chatRequest[138];
unsigned char* chatUser = &chatRequest[4];
unsigned char* chatPass = &chatRequest[14];
unsigned char* chatBuffer = &chatRequest[24];

callback *callUser, *callPass, *callLogin, *callMessage, *callSend, *callScroll;

void ChatPage(unsigned int page)
{
	chatRequest[0] = REQ_PAGE;
	POKEW(&chatRequest[1], page);
	chatRequest[3] = 3;
	SendTCP(chatRequest, 4);	
}

void ChatSend()
{
	// Make message length checks
	unsigned char len;
	len = strlen(chatBuffer);
	if (len < 8) {
		paperColor = DESK_COLOR; inkColor = BLACK;
		PrintStr(10,1,"Min Length: 8 chars!"); 		
		sleep(1); PrintBlanks(10,1, 20, 1);
		Line(lineX1, lineX2, lineY, lineY);
		return;
	}
	
	// Send message to server
	chatRequest[0] = REQ_SEND;
	chatRequest[1] = strlen(chatUser);
	chatRequest[2] = strlen(chatPass);
	chatRequest[3] = strlen(chatBuffer);
	SendTCP(chatRequest, 24+strlen(chatBuffer));
	
	// Clear previous message
	paperColor = WHITE;
	PrintBlanks(0,0,36,1);
	chatBuffer[0] = 0;
	
	// Refresh messages
	ChatPage(0);	
}

void ChatLogin()
{
	chatRequest[0] = REQ_LOGIN; 
	chatRequest[1] = strlen(chatUser);
	chatRequest[2] = strlen(chatPass);
	SendTCP(chatRequest, 24);	
}

void ChatMessage(unsigned char index, unsigned char* packet)
{
	unsigned char i, l, line, buffer[29];

	// Find message slot
	paperColor = DESK_COLOR;
	line = index*5+2;
	
	// Display user/date
	inkColor = WHITE;
	PrintStr(0, line, &packet[4]);
	i = 4 + 1 + strlen(&packet[4]);
	
	PrintStr(0, line+1, &packet[i]);
	i = i + 1 + strlen(&packet[i]);
	
	// Display message
	inkColor = BLACK;
	l = i + strlen(&packet[i]);
	while (i < l) {
		memcpy(buffer, &packet[i], 28);
		PrintStr(CHR_COLS-29, line, buffer);
		i += 28; line++;
	}
}

void ChatCallback(callback* call)
{
	if (call == callLogin) {
		ChatLogin();
		return;
	}
	
	if (call == callSend) {
		ChatSend();
		return;
	}
}

void ChatScreen(void)
{			
	unsigned char i;

	// DEBUG
	strcpy(chatUser, "8BIT-DUDE");
	strcpy(chatPass, "UNI-Z33K0");	
	
	// Clear screen
	ClearScreen();
	
	// Setup connection to chat server?
	if (!chatConnected) {
		OpenTCP(199, 47, 196, 106, 1999);
		chatConnected = 1;
	}
	
	// Login and Message screen?		
	if (!chatLogged) {
		// Panel/Labels
		paperColor = DESK_COLOR; inkColor = BLACK;	
		Panel(10, 3, 19, 8, "");	
		PrintStr(12, 5, "User:");
		PrintStr(12, 7, "Pass:");
		
		// Inputs
		paperColor = WHITE;
		callUser = Input(17, 5, 10, 1, chatUser);
		callPass = Input(17, 7, 10, 1, chatPass);		
		
		// Controls
		paperColor = BLACK; inkColor = WHITE;	
		callLogin = Button(17, 10, 7, 1, " Login ");
		
	} else {
		// Add text input, send button, and scrollbar
		paperColor = BLACK; inkColor = WHITE;	
		callSend = Button(CHR_COLS-4, 0, 7, 1, "Send");
		
		paperColor = WHITE; inkColor = BLACK;	
		callMessage = Input(0, 0, CHR_COLS-4, 1, chatBuffer);
			
		paperColor = DESK_COLOR;
		callScroll = ScrollBar(CHR_COLS-1, 1, CHR_ROWS-3, 0, "chat");

		// Add separators
		lineX1 = ColToX(0)+2;
		lineX2 = ColToX(CHR_COLS-2)+2;
		for (i=0; i<3; i++) {
			lineY = RowToY(5*(2-i)+1)+3;
			Line(lineX1, lineX2, lineY, lineY);
		}
		
		// Get latest page
		ChatPage(0);		
	}
	
	// Add Taskbar
	DrawTaskBar();		
}

void ChatPacket(unsigned char *packet)
{
	// Process received packets
	unsigned char i;
	switch 	(packet[0]) {
	case REQ_LOGIN:
		// Check if login was OK
		if (packet[1] == 'O') {
			chatLogged = 1;	
			ChatScreen();
		} else {
			PrintStr(10, 10, packet[1]);
		}
	case REQ_PAGE:
		// Save chat list and request first message
		chatLen = packet[4];
		for (i=0; i<chatLen; i++)
			chatList[i] = packet[5+2*i];
		chatRequest[0] = REQ_RECV;			
		chatRequest[1] = chatList[0]; 
		chatRequest[2] = 0; 
		chatRequest[3] = PLATFORM;		
		SendTCP(chatRequest, 4);
		break;
	
	case REQ_RECV:
		// Check corresponding entry in list
		i = 0;
		while (i<4) {
			if ((unsigned int)packet[1] == chatList[i]) break;
			i++;
		}

		// Display message and request next entry
		ChatMessage(i, packet);
		if (i<chatLen-1) {
			chatRequest[1] = chatList[i+1]; 
			SendTCP(chatRequest, 4);
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////

unsigned char app = APP_HOME;

void ProcessCallback(callback* call)
{
	// Handle input boxes here
	if (call->type == CALLTYPE_INPUT) { 
		SetInput(call);
		return;
	} 
		
	// Callbacks to Apps
	if (call == home) {
		app = APP_HOME;
		HomeScreen();
		return;
	}
	
	if (call == files) {
		app = APP_FILES;
		FilesScreen();
		return;
	}
	
	if (call == bitmap) {
		app = APP_BITMAP;
		BitmapScreen();
		return;
	}
	
	if (call == music) {
		app = APP_MUSIC;
		MusicScreen();
		return;
	}
	
	if (call == chat) {
		app = APP_CHAT;
		ChatScreen();
		return;
	}
	
	// Callbacks from Apps
	switch (app) {
	case APP_FILES:
		FileCallback(call);
		break;
	case APP_BITMAP:
		BitmapCallback(call);
		break;
	case APP_MUSIC:
		MusicCallback(call);
		break;
	case APP_CHAT:
		ChatCallback(call);
		break;								
	}
}

void ProcessPacket(unsigned char* packet)
{
	// Packets from Apps
	switch (app) {
	case APP_CHAT:
		ChatPacket(packet);
		break;								
	}	
}

int main (void)
{
	callback* call;
	unsigned char network;
	unsigned char *packet;
	clock_t timer;

	// Fetch File List First
	GetFileList();

	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
		
	// Init systems
	InitBitmap();
	ClearBitmap();
	InitSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
	
	// Try to init network
	network = InitNetwork();
	if (!network)
		SlotTCP(0);
	
	// Load chunks of various apps
	LoadChunk(&icon[ICO_MUSIC], "music.chk");	
	LoadChunk(&icon[ICO_STOP],  "stop.chk");	
	LoadChunk(&icon[ICO_PLAY],  "play.chk");	
	LoadChunk(&icon[ICO_PREV],  "prev.chk");	
	LoadChunk(&icon[ICO_NEXT],  "next.chk");	

	// Enter bitmap mode
	HomeScreen();
	EnterBitmapMode();
	EnableSprite(0);

	// Main loop
	while (1) {
		if (clock() > timer) {
			// Update timer and interface
			timer = clock();
			UpdateMouse();
			UpdateInput();
			
			// Check for packets
			if (!network) {
				packet = RecvTCP(0);
				if ((int)packet) {
				#if defined(__ORIC__)
					DisableSprite(0);
				#endif
					ProcessPacket(packet);
				#if defined(__ORIC__)
					EnableSprite(0);
				#endif
				}
			}

			// Check callbacks
			if (mouseButt) {
				if (!mouseLock && !inputMode) {
					call = CheckCallbacks((mouseX*CHR_COLS)/320, (mouseY*CHR_ROWS)/200);
					if (call) {
					#if defined(__ORIC__)
						DisableSprite(0);
					#endif
						ProcessCallback(call);
					#if defined(__ORIC__)
						EnableSprite(0);
					#endif
					}
					mouseLock = 1;
				}
			} else {
				mouseLock = 0;
			}
		}
		// Platform dependent actions
	#if defined __APPLE2__
		clk += 1;  // Manually update clock on Apple 2
	#elif defined __LYNX__
		UpdateDisplay();
	#endif
	}

    // Done
    return EXIT_SUCCESS;
}
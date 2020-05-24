 
#include "unity.h"

// Mouse sprite definition
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

unsigned char stateMenu = 0;
unsigned char mouseL = 0;
unsigned char mouseC = 0;
unsigned int mouseX = 160;
unsigned int mouseY = 100;

void UpdateMouse()
{
	unsigned char joy;
	
	// Read mouse state from joystick
	joy = GetJoy(0);
	if (joy & JOY_UP)    { mouseY+=2; if (mouseY>200) {mouseY = 200;} }
	if (joy & JOY_DOWN)  { mouseY-=2; if (mouseY>200) {mouseY = 0;} }
	if (joy & JOY_LEFT)  { mouseX+=2; if (mouseX>320) {mouseX = 320;} }
	if (joy & JOY_RIGHT) { mouseX-=2; if (mouseX>320) {mouseX = 0;} }
	if (joy & JOY_BTN1)  { mouseL = 0; } else { mouseL = 1; } 
	
	// Update 
	LocateSprite(mouseX, mouseY);
	if (mouseL) SetSprite(0, 1); else SetSprite(0, 0);
}

#if defined(__LYNX__)
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
#if defined(__APPLE2__) || defined(__ATARI__) || defined(__CBM__)
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

callback *home, *files, *bitmap, *music, *browser, *text, *paint;
unsigned char deskColor;

void DrawTaskBar()
{
	paperColor = WHITE; 
	inkColor = BLACK; 
#if (defined __ORIC__)
	SetAttributes(-1, CHR_ROWS-1, paperColor);
#endif
	PrintBlanks(0, CHR_ROWS-1, CHR_COLS, 1);
	PrintStr( 1, CHR_ROWS-1, ")(");		
	home = PushCallback( 0, CHR_ROWS-1, 4, 1, CALLTYPE_ICON, "HOME");	
}

void ClearScreen()
{
	// Clear usable area
	paperColor = deskColor;
	PrintBlanks(0, 0, CHR_COLS, CHR_ROWS-1);
	
	// Reset callbacks
	files = bitmap = music = browser = text = paint = 0;
	ClearCallbacks();
}

unsigned char* fileList[32];

void FilesScreen(void)
{			
	unsigned char i, dest;
	unsigned int size;
	
	// Clear screen
	ClearScreen();
	
	// Create list for display
	for (i=0; i< fileNum; i++) {
		// Allocate string for name + size
		fileList[i] = malloc(17);
		memset(fileList[i], 32, 15);
		fileList[i][16] = 0;
		
		// Copy over name + size
		memcpy(fileList[i], fileNames[i], strlen(fileNames[i]));
		size = fileSizes[i];
		if (size<1000) {
			if (size < 10) {
				dest = 15;
			} else if (size < 100) {
				dest = 14;
			} else if (size < 1000) {
				dest = 13;
			}
			itoa(size, &fileList[i][dest], 10);			
		} else {
			size /= 1000;
			if (size < 10) {
				dest = 14;
			} else {
				dest = 13;
			}
			itoa(size, &fileList[i][dest], 10);			
			fileList[i][15] = 'k';
		}
	}

	// List directory
	ListBox(1, 0, 16, CHR_ROWS-2, "Files", fileList, fileNum);
	Panel(18, 0, CHR_COLS-19, CHR_ROWS-2, "Preview");	
	
	// Add Taskbar
	DrawTaskBar();		
}

void BitmapScreen(void)
{		
	// Clear screen
	ClearScreen();
	
	// Add Taskbar
	DrawTaskBar();		
}

void MusicScreen(void)
{		
	// Clear screen
	ClearScreen();
	
	// Add Taskbar
	DrawTaskBar();		
}

void HomeScreen(void)
{
	unsigned char height, row1, row2;
#if (defined __LYNX__)
	height = 6; row1 = 1; row2 = 8;
#else
	height = 8; row1 = 1; row2 = 11;
#endif
	
	// Load Apps Bitmap
	LoadBitmap("apps.img");
	
	// Fetch Desk Colors
	pixelX = 0; pixelY = 0;
	deskColor = GetPixel();
	
	// Setup Callbacks
	ClearCallbacks();
	files   = PushCallback( 1, row1, 8, height, CALLTYPE_ICON, "FILES");
	bitmap  = PushCallback(11, row1, 8, height, CALLTYPE_ICON, "BITMAP");
	music   = PushCallback(21, row1, 8, height, CALLTYPE_ICON, "MUSIC");
	browser = PushCallback(31, row1, 8, height, CALLTYPE_ICON, "BROWSER");
	text    = PushCallback( 1, row2, 8, height, CALLTYPE_ICON, "TEXT");
	paint   = PushCallback(11, row2, 8, height, CALLTYPE_ICON, "PAINT");
	
	// Add Taskbar
	DrawTaskBar();	
	PrintStr(18, CHR_ROWS-1, "8BIT-BENCH 2020/05/23");		
}

int main (void)
{
	callback* call;
	clock_t timer = clock();
	
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	
	// Init systems
	InitBitmap();
	InitSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
	EnableSprite(0);
	GetFileList();

	// Enter bitmap mode
	HomeScreen();
	EnterBitmapMode();

	// Main loop
	while (1) {
		if (clock() > timer) {
			// Update timer
			timer = clock();
			
			// Update mouse cursor
			UpdateMouse();
			
			// Check callbacks
			if (mouseL) {
				if (!mouseC) {
					call = CheckCallbacks(mouseX, mouseY);
					if (call) {
						// Display action name
						paperColor = WHITE;
						PrintBlanks(5, CHR_ROWS-1, 8, 1); 
						PrintStr(5, CHR_ROWS-1, call->label);
						
						// Switch screen if needed
						if (call == home) {
							HomeScreen();
						} else if (call == files) {
							FilesScreen();
						} else if (call == bitmap) {
							BitmapScreen();
						} else if (call == music) {
							MusicScreen();
						}				
					}
					mouseC = 1;
				}
			} else {
				mouseC = 0;
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

#include "definitions.h"

// See home.c
char appSel = APP_HOME;
callback *homeCall, *appCall[4];

// See mouse.c
extern unsigned char* mouse;

// See network.c
extern unsigned char netConnected;

void ProcessCallback(callback* call)
{
	// Callbacks to Apps
	if (call == homeCall) { HomeScreen(); return; }
	if (call == appCall[APP_FILES]) { appSel = APP_FILES; FilesScreen(); return; }
	if (call == appCall[APP_IMAGE]) { appSel = APP_IMAGE; ImageScreen(); return; }
	if (call == appCall[APP_MUSIC]) { appSel = APP_MUSIC; MusicScreen(); return; }
	if (call == appCall[APP_CHAT])  { appSel = APP_CHAT;  ChatScreen();  return; }
	
	// Callbacks from Apps
	switch (appSel) {
	case APP_FILES:
		FileCallback(call);
		break;
	case APP_IMAGE:
		ImageCallback(call);
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
	switch (appSel) {
	case APP_CHAT:
		ChatPacket(packet);
		break;								
	}	
}

int main(void)
{
	clock_t timer;
	callback* call;
	unsigned char *packet;
	unsigned char mouseLock = 0;	

	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	
	// Get files and chunks
	GetFileList();
	LoadChunks();

	// Enter bitmap mode
	InitBitmap();
	ClearBitmap();
	HomeScreen();
	EnterBitmapMode();

	// Init systems
	InitMouse();

	// Main loop
	while (1) {
		if (clock() > timer) {
			// Update timer
			timer = clock();
			
			// Update Input Box (if active)
			if (ProcessInput()) {
				DisableSprite(0);
			} else {
				// Otherwise update Mouse
				EnableSprite(0);

				// Check callbacks
				if (!(mouse[2] & MOU_LEFT)) {
					if (!mouseLock) {
					#if defined(__APPLE2__) || defined(__ORIC__)
						DisableSprite(0);
					#endif
						call = CheckCallbacks((mouse[0]*CHR_COLS)/160u, (mouse[1]*CHR_ROWS)/200u);
						if (call)
							ProcessCallback(call);
					#if defined(__APPLE2__) || defined(__ORIC__)
						EnableSprite(0);
					#endif			
						mouseLock = 1;
					}
				} else {
					mouseLock = 0;
				}
				
				// Update mouse
				ProcessMouse();				
			}
			
			// Check for packets
			if (netConnected) {
				packet = RecvTCP(0);
				if ((int)packet) {
				#if defined(__APPLE2__) || defined(__ORIC__)
					DisableSprite(0);
				#endif					
					ProcessPacket(packet);
				#if defined(__APPLE2__) || defined(__ORIC__)
					EnableSprite(0);
				#endif			
				}
			}			
		}
		// Platform dependent actions
	#if defined __APPLE2__
		clk++;  // Manually update clock on Apple 2
	#elif defined __LYNX__
		UpdateDisplay();
	#endif
	}

    // Done
    return EXIT_SUCCESS;
}


#include "definitions.h"

#if defined(__NES__)
 #pragma rodata-name("BANK0")
 #pragma code-name("BANK0")
#endif

// See mouse.c
extern unsigned char* mouse;

// See network.c
extern unsigned char netConnected;

// See home.c
char appSel = -1;
callback *homeCall, *appCall[4];

// App/Icon Chunk Pointers
unsigned char* chunkData[NUM_CHUNKS];

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
		return;
	case APP_IMAGE:
		ImageCallback(call);
		return;
	case APP_MUSIC:
		MusicCallback(call);
		return;
	case APP_CHAT:
		ChatCallback(call);
		return;								
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
	LoadChunks(chunkData, "icons.chk");	
	
	// Init systems
	InitMouse();	

	// Enter bitmap mode
	InitBitmap();
	ClearBitmap();
	ShowBitmap();
	HomeScreen();

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
						call = CheckCallbacks((mouse[0]*TXT_COLS)/160u, (mouse[1]*TXT_ROWS)/200u);
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

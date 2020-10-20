
#include "unity.h"

// Main interface
#include "definitions.h"
#include "mouse.h"
#include "home.h"

// Individual Apps
#include "files.h"
#include "image.h"
#include "music.h"
#include "chat.h"

#ifdef __APPLE2__
  #pragma code-name("LOWCODE")
#endif

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

#if defined(__APPLE2__) || defined(__CBM__)
  // IP65 bug requires reconnection after each packet
  unsigned char packetBackup[140];
#endif

#ifdef __APPLE2__
  #pragma code-name("CODE")
#endif

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

	// Load mouse sprite
	InitSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
	EnableSprite(0);

	// Try to init 
	netConnected = !InitNetwork();
	if (netConnected)
		SlotTCP(0);

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
						call = CheckCallbacks((mouse[0]*CHR_COLS)/160, (mouse[1]*CHR_ROWS)/200);
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
				#if defined(__APPLE2__) || defined(__CBM__)
					// IP65 bug requires reconnection after each packet
					memcpy(packetBackup, packet, 140); packet = packetBackup;
					CloseTCP();	OpenTCP(199, 47, 196, 106, 1999);
				#endif
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
		clk += 1;  // Manually update clock on Apple 2
	#elif defined __LYNX__
		UpdateDisplay();
	#endif
	}

    // Done
    return EXIT_SUCCESS;
}

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

void ProcessCallback(callback* call)
{
	// Handle input boxes here
	if (call->type == CALLTYPE_INPUT) { 
		SetInput(call);
		return;
	} 
		
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

int main (void)
{
	callback* call;
	unsigned char network;
	unsigned char *packet;
	clock_t timer;

	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	
	// Fetch File List First
	GetFileList();
	
	// Load app chunks
	LoadChunk(&appChunk[APP_FILES], "files.chk");	
	LoadChunk(&appChunk[APP_IMAGE], "image.chk");	
	LoadChunk(&appChunk[APP_MUSIC], "music.chk");	
	LoadChunk(&appChunk[APP_CHAT],  "chat.chk");	
	
	// Load icon chunks	
	LoadChunk(&icoChunk[ICO_STOP],  "stop.chk");	
	LoadChunk(&icoChunk[ICO_PLAY],  "play.chk");	
	LoadChunk(&icoChunk[ICO_PREV],  "prev.chk");	
	LoadChunk(&icoChunk[ICO_NEXT],  "next.chk");	

	// Enter bitmap mode
	InitBitmap();
	ClearBitmap();
	HomeScreen();
	EnterBitmapMode();

	// Load mouse sprite
	InitSprites(spriteFrames, spriteCols, spriteRows, spriteColors);
	EnableSprite(0);

	// Try to init 
#ifndef __APPLE2__	
	network = !InitNetwork();
#endif
	if (network)
		SlotTCP(0);

	// Main loop
	while (1) {
		if (clock() > timer) {
			// Update timer and interface
			timer = clock();
			UpdateMouse();
			UpdateInput();
			
			// Check for packets
			if (network) {
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
			if (mouseState[2] & MOU_LEFT) {
				if (!mouseLock && !inputMode) {
					call = CheckCallbacks((mouseState[0]*CHR_COLS)/160, (mouseState[1]*CHR_ROWS)/200);
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
/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2021/05/13
 *	
 * Copyright (c) 2021 Anthony Beaucamp
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented * you must not
 *   claim that you wrote the original software. If you use this software in a
 *   product, an acknowledgment in the product documentation would be
 *   appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not
 *   be misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any distribution.
 *
 *   4. The names of this software and/or it's copyright holders may not be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 *	Credits: 
 *		* Oliver Schmidt for his IP65 network interface
 *		* Christian Groessler for helping optimize the memory maps on Commodore and Atari
 *		* Bill Buckels for his Apple II Double Hi-Res bitmap code
 *		* Karri Kaksonen for his many contributions on the Lynx
 */

// CC65 includes
#include <string.h>
#include <conio.h>
#include <dirent.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Platform IDs
#if defined __CBM__
	#define PLATFORM   0	
    #define SPRITE_NUM 8
    #include "targets/c64/platform.h"		

#elif defined __ATARI__
	#define PLATFORM   1
    #define SPRITE_NUM 16
    #include "targets/atari/platform.h"	

#elif defined __APPLE2__
	#define PLATFORM   2
    #define SPRITE_NUM 8
    #include "targets/apple2/platform.h"		

#elif defined __ATMOS__
	#define PLATFORM   3
    #define SPRITE_NUM 8
    #include "targets/oric/platform.h"	

#elif defined __LYNX__
	#define PLATFORM   4
    #define SPRITE_NUM 8  
    #include "targets/lynx/platform.h"	

#elif defined __NES__
	#define PLATFORM   5
    #define SPRITE_NUM 8
    #include "targets/nes/platform.h"	
#endif

// Unity Modules
#include "bitmap/bitmap.h"
#include "charmap/charmap.h"
#include "sprites/sprites.h"
#include "strings/chars.h"

// Adaptors
#if defined __HUB__
  #include "adaptors/hub.h"  
#elif defined __FUJINET__	
  // Nothing
#else
  #include "adaptors/ip65.h"
#endif

// Bitmap functions (see bitmap.c)
void InitBitmap(void);
void ShowBitmap(void);
void HideBitmap(void);
void ClearBitmap(void);
void LoadBitmap(char *filename);

// Pixel setting functions (see pixel.c)
unsigned char GetPixel(void);
void SetPixel(unsigned char color);
void LocatePixel(unsigned int x, unsigned int y);
extern unsigned char pixelX, pixelY; // (see LocatePixel())

// Bitmap chunks functions (see chunks.c)
// 	Apple:  X/W must be multiples of 7 (e.g. 0,7,14,21...) |              No restrictions
//	Atari:  X/W must be multiples of 4 (e.g. 0,4,8,12... ) |              No restrictions
//	C64:    X/W must be multiples of 4 (e.g. 0,4,8,12... ) |  Y/H must be multiples of 8  (e.g. 0,8,16,24...)
//	Lynx:   X/W must be multiples of 2 (e.g. 0,2,4,6... )  |              No restrictions
//	NES:    X/W must be multiples of 8 (e.g. 0,8,16,24... )|  Y/H must be multiples of 8  (e.g. 0,8,16,24...)
// 	Oric:   X/W must be multiples of 6 (e.g. 0,6,12,18...) |  			  No restrictions
unsigned char LoadChunks(unsigned char *chunks[], char *filename);
unsigned char *GetChunk(unsigned char x, unsigned char y, unsigned char w, unsigned char h);
void SetChunk(unsigned char* chunk, unsigned char x, unsigned char y);
void FreeChunk(unsigned char* chunk);

// Charmap functions (see charmap.c)
void InitCharmap(unsigned char col1, unsigned char col2, unsigned char row1, unsigned char row2);
void ShowCharmap(void);
void HideCharmap(void);
void FreeCharmap(void);
void ClearCharmap(void);
void LoadCharmap(char *filename, unsigned int w, unsigned int h);
void LoadTileset(char *filename, unsigned int n);
void LoadCharset(char* filename);
void DrawCharmap(unsigned char x, unsigned char y);
void ScrollCharmap(unsigned char x, unsigned char y);
void PrintCharmap(unsigned char x, unsigned char y, unsigned char chr);
void SetTile(unsigned char x, unsigned char y, unsigned char tile);
unsigned int DecodeTiles(void);
unsigned char GetTile(unsigned char x, unsigned char y);
unsigned char GetFlag(unsigned char x, unsigned char y);
extern unsigned char screenCol1, screenCol2, screenWidth;
extern unsigned char screenRow1, screenRow2, screenHeight;
extern unsigned char worldWidth, worldHeight, worldMaxX, worldMaxY;

// Console specific functions
#if defined(__LYNX__) || defined(__NES__)
  void UpdateDisplay(void);
  void ShowKeyboardOverlay(void);
  void HideKeyboardOverlay(void);
  void SetKeyboardOverlay(unsigned char x, unsigned char y);
  unsigned char GetKeyboardOverlay(void); 
  unsigned char KeyboardOverlayHit(void);
#endif

// File system functions (see /targets/xxx/files.c)
void DirList(void);
unsigned char FileExists(const char* fname);
unsigned int FileOpen(const char* fname);
signed int FileRead(char* buffer, signed int len);
void FileClose(void);

// Printing functions (see print.c)
extern unsigned char txtX, txtY;
extern unsigned char inkColor, paperColor;
#if defined __ORIC__
  void SetAttributes(unsigned char color);
#endif
void BackupCursor(void);
void RestoreCursor(void);
void PrintBlanks(unsigned char width, unsigned char height);
void PrintChr(char chr);
void PrintNum(unsigned int num);
void PrintStr(const char *buffer);
void PrintLogo(unsigned char index);
void CopyStr(unsigned char col1, unsigned char row1, unsigned char col2, unsigned char row2, unsigned char len);
unsigned char InputStr(unsigned char width, char *buffer, unsigned char len, unsigned char key);
extern unsigned char maskInput;

// Joystick/Mouse definitions
#define JOY_UP      1
#define JOY_DOWN    2
#define JOY_LEFT    4
#define JOY_RIGHT   8
#define JOY_BTN1   16
#define JOY_BTN2   32
#define MOU_LEFT    1
#define MOU_MIDDLE  2
#define MOU_RIGHT   4
#define MOU_UP      8
#define MOU_DOWN   16
#define MOU_MOTION 32
#if defined(__APPLE2__) || (__ATARIXL__)
  #if defined __HUB__
    #define JOY_MAX 5
  #else	
    #define JOY_MAX 2
  #endif	 
#elif defined(__ATARI__)
  #if defined __HUB__
    #define JOY_MAX 7
  #else	
    #define JOY_MAX 4	 
  #endif	 
#elif defined(__CBM__)
  #if defined __HUB__
    #define JOY_MAX 5
  #else
    #define JOY_MAX 4
  #endif
#elif defined(__LYNX__)
    #define JOY_MAX 4
#elif defined __NES__
    #define JOY_MAX 5
#elif defined(__ORIC__)
    #define JOY_MAX 5
#endif

// Joystick functions
void InitJoy(void);
unsigned char GetJoy(unsigned char);
unsigned char* GetMouse(void);
#if defined __APPLE2__
  unsigned char GetButton(unsigned char);
  unsigned char GetPaddle(unsigned char);	// Fast reading of paddle 0, 1, 2 or 3 (Note: cannot read another paddle for 3 msec)
  void GetPaddles(unsigned char);			// Normal reading of both paddles in joystick 0 or 1
  extern unsigned char paddleX, paddleY;	// Paddle values received from GetPaddles()
#endif

// 2D geometry functions
unsigned char IntersectSegments(signed int seg1X1, signed int seg1Y1, signed int seg1X2, signed int seg1Y2,
							    signed int seg2X1, signed int seg2Y1, signed int seg2X2, signed int seg2Y2,
								signed int *intX,  signed int *intY);
unsigned char IntersectSegmentPolygon(signed int segX1, signed int segY1, signed int segX2, signed int segY2, 
									  unsigned char polyN, signed int *polyX, signed int *polyY, 
									  unsigned char *intI, signed int *intX, signed int *intY);								
unsigned char PointInsidePolygon(signed int pX, signed int pY, unsigned char polyN, signed int *polyX, signed int *polyY);

// Math functions
#define MIN(a,b) (a>b ? b : a)
#define MAX(a,b) (a>b ? a : b)
#define ABS(a)   (a<0 ? -a : a)
#define SIGN(a)  (a<0 ? -1 : 1)
#define DOT(a,b) (a[0]*b[0]+a[1]*b[1])
#define BYTE4(a,b,c,d) ((a<<6) | (b<<4) | (c<<2) | d)
signed int dot(signed char *v1, signed char *v2);
unsigned char atan2(unsigned char y, unsigned char x);

// Music functions (see music.c)
// Apple: ElectDuet (see apple/DUET.s) 
// Atari: RMT track (see atari/POKEY.s)
// C64:   SID track (see c64/SID.s)
// Lynx:  Chipper   (see lynx/CHIPPER.s)
// Oric:  YM track  (see oric/MYM.s)
void LoadMusic(const char* filename);
void PlayMusic(void);
void StopMusic(void);
void PauseMusic(unsigned char state);
#if defined __APPLE2__
  void UpdateMusic(void);
#endif

// SFX functions (see sfx.c)
#define SFX_BLEEP   0
#define SFX_BUMP    1
#define SFX_ENGINE  2
#define SFX_GUN  	3
#define SFX_INJURY  4
#define SFX_SCREECH 5
void InitSFX(void);
void StopSFX(void);
void PlaySFX(unsigned char index, unsigned char pitch, unsigned char volume, unsigned char channel);

// Sprite handling functions
void LoadSprites(unsigned char* filename, const unsigned char *spriteColors);
void EnableSprite(unsigned char index);
void DisableSprite(signed char index);
void LocateSprite(unsigned int x, unsigned int y);
void SetSprite(unsigned char index, unsigned int frame);
void RecolorSprite(unsigned char index, unsigned char offset, unsigned char color);
#if defined __APPLE2__
  void CropSprite(unsigned char index, unsigned char rows);
#elif defined __ATARI__
  void EnableMultiColorSprite(unsigned char index);
  void DisableMultiColorSprite(unsigned char index);
  void SetMultiColorSprite(unsigned char index, unsigned int frame);
#elif defined __CBM__
  void DoubleHeightSprite(unsigned char index, unsigned char onoff);
  void DoubleWidthSprite(unsigned char index, unsigned char onoff);
#elif defined __LYNX__
  void ScaleSprite(unsigned char index, unsigned int xPercent, unsigned int yPercent);
#elif defined __ORIC__
  void CropSprite(unsigned char index, unsigned char rows);
  void MultiColorSprite(unsigned char index, unsigned char* multiColorDef);
#endif

// Screen coordinates of current sprite (see LocateSprite())
#if defined __CBM__
	extern unsigned int spriteX;
	extern unsigned char spriteY;	
#else
	extern unsigned char spriteX, spriteY;
	extern unsigned char sprCushion;
#endif

// Sprite collisions: Hardware on C64 vs. Software on Apple/Atari/Oric/Lynx 
#if defined(__CBM__)
  unsigned char COLLISIONS(unsigned char i);
  #define COLLIDING(collisions,i) (collisions & (1<<i)) 
#elif defined(__ATARI__)
  #define COLLISIONS(i) (sprCollision[i])
  #define COLLIDING(collisions,i) (collisions & (1<<i&7)) 
#else
  #define COLLISIONS(i) (sprCollision[i])
  #define COLLIDING(collisions,i) (collisions & (1<<i)) 
#endif

// Scaling functions (char <> bitmap)
unsigned char ColToX(unsigned char col);
unsigned char RowToY(unsigned char row);
unsigned char XToCol(unsigned char x);
unsigned char YToRow(unsigned char y);

// Simple menu functions
extern unsigned char cursorLocked, cursorMoved, cursorCol, cursorRow, cursorHeight, cursorPos, *cursorActions;
void InitMenu(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char padding, unsigned char **labels, unsigned char *actions);
unsigned char UpdateMenu(void);

// Widget functions
#ifndef CALLDEF
#define CALLDEF
typedef struct {
	unsigned char colBeg, colEnd, rowBeg, rowEnd;
	unsigned char *buffer, value, ink, paper, type;
	unsigned int next;
} callback;
#endif
#define CALLTYPE_ICON  	   1
#define CALLTYPE_BUTTON    2
#define CALLTYPE_INPUT     3
#define CALLTYPE_LISTBOX   4
#define CALLTYPE_SCROLLBAR 5
callback* Button(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* label);
callback* Icon(unsigned char col, unsigned char row, unsigned char* chunk);
callback* Input(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* buffer, unsigned char len);
void ListBox(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title, unsigned char* labels, unsigned char len);
void Panel(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title);
void Line(unsigned char x1, unsigned char x2, unsigned char y1, unsigned char y2);
callback* ScrollBar(unsigned char col, unsigned char row, unsigned char height, unsigned int *range);
callback* PushCallback(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char type);
callback* CheckCallbacks(unsigned char col, unsigned char row);
void PopCallback(callback* call);
unsigned char SliderPos(callback* call);
unsigned char ProcessInput(void);
void ClearCallbacks(void);

// Network functions (see net-base.c, net-ip.c, net-easy.c, net-udp.c, net-tcp.c, net-url.c, net-web.c)
#define NETWORK_OK  0
#define ADAPTOR_ERR 1
#define DHCP_ERR    2
unsigned char InitNetwork(void);							// Initialize network adapter
unsigned char GetLocalIP(unsigned char* ip);				// Fetch local IP

// Easynet protocols
#define EASY_TCP     1
#define EASY_UDP     2

// Easynet return states
#define EASY_OK      0
#define EASY_CRED    1
#define EASY_FULL    2
#define EASY_TIMEOUT 3
#define EASY_BUFFER 256

// Easynet variables and functions
extern unsigned char easyProt, easyLive, easySlot;
unsigned char EasyHost(unsigned char protocol, unsigned char slots, unsigned int *ID, unsigned int *PASS); 	// protocol: EASY_TCP or EASY_UDP (ID and PASS generated automatically)
unsigned char EasyJoin(unsigned char protocol,  unsigned int *ID, unsigned int *PASS);						// 				   " "		      (ID and PASS must match host)		  	 
void EasySend(unsigned char *buffer, unsigned char len);
unsigned char *EasyRecv(unsigned char timeout, unsigned char* len);
void EasyQuit(void);

// TCP functions
void SlotTCP(unsigned char slot);							// Set TCP slot (0~15)
void OpenTCP(unsigned char *ip, unsigned int svPort);		// Open connection on current TCP slot (local port allocated automatically)
void CloseTCP(void);										// Close current TCP slot
void SendTCP(unsigned char* buffer, unsigned char length);  // Send contents of buffer on current TCP slot
unsigned char* RecvTCP(unsigned int timeOut);				// Check all slots for incoming TCP packet (within time-out period)

// UDP functions
void SlotUDP(unsigned char slot);							// Set UDP slot (0~15)
void OpenUDP(unsigned char *ip, unsigned int svPort,     	// Open connection on current UDP slot (local port allocated on clPort)
								unsigned int clPort);
void CloseUDP(void);										// Close current UDP slot
void SendUDP(unsigned char* buffer, unsigned char length);  // Send contents of buffer on current UDP slot
unsigned char* RecvUDP(unsigned int timeOut);				// Check all slots for incoming UDP packet (within time-out period)

// URL functions
void GetURL(unsigned char* url);									// Request URL file
unsigned char* ReadURL(unsigned char size, unsigned int timeOut);	// Read chunk of URL file

// WEB server functions
void OpenWEB(unsigned int port, unsigned int timeOut);		// Start WEB server on specified port (time-out in millisecs)
void CloseWEB(void);										// Close WEB server
void HeaderWEB(unsigned char* buffer, unsigned char length);// Header of reply to current WEB client
void BodyWEB(unsigned char* buffer, unsigned char length);  // Body of reply to current WEB client
void SendWEB(void);											// Send reply to current WEB client
unsigned char* RecvWEB(unsigned int timeOut);				// Check WEB server for incoming packet (within time-out period)

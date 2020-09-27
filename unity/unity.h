/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2019/07/12
 *	
 * Copyright (c) 2019 Anthony Beaucamp.
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
#include <conio.h>
#include <dirent.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "chars.h"

// Platform IDs
#if defined __CBM__
	#define PLATFORM   0	
    #include "C64/platform.h"		
#elif defined __ATARI__
	#define PLATFORM   1
    #include "Atari/platform.h"	
#elif defined __APPLE2__
	#define PLATFORM   2
    #include "Apple/platform.h"		
#elif defined __ATMOS__
	#define __HUB__
	#define PLATFORM   3
    #include "Oric/platform.h"	
#elif defined __LYNX__
	#define __HUB__
	#define PLATFORM   4
    #include "Lynx/platform.h"	
#endif

// Bitmap functions (see bitmap.c)
void InitBitmap(void);
void EnterBitmapMode(void);
void ExitBitmapMode(void);
void ClearBitmap(void);
void LoadBitmap(char *filename);
unsigned char GetPixel(void);
void SetPixel(unsigned char color);
void LocatePixel(unsigned int x, unsigned int y);
extern unsigned char pixelX, pixelY; // (see LocatePixel())

// Bitmap chunks functions (see chunks.c)
// 	Apple:  X/W must be multiples of 7 (e.g. 0,7,14,21...) |              No restrictions
//	Atari:  X/W must be multiples of 4 (e.g. 0,4,8,12... ) |              No restrictions
//	C64:    X/W must be multiples of 4 (e.g. 0,4,8,12... ) |  Y/H must be multiples of 8  (e.g. 0,8,16,24...)
//	Lynx:   X/W must be multiples of 2 (e.g. 0,2,4,6... )  |              No restrictions
// 	Oric:   X/W must be multiples of 6 (e.g. 0,6,12,18...) |  			  No restrictions
void GetChunk(unsigned char** chunk, unsigned char x, unsigned char y, unsigned char w, unsigned char h);
void SetChunk(unsigned char* chunk, unsigned char x, unsigned char y);
void LoadChunk(unsigned char** chunk, char *filename);

// C64 specific functions (see C64/ROM.s)
#ifdef __CBM__
  void DisableRom();	// Disable ROM before using GetColor()
  void EnableRom();	// Enable ROM after using GetColor()
#endif

// Lynx specific functions (see Lynx/display.c)
#if defined __LYNX__
  void UpdateDisplay(void);
  void ShowKeyboardOverlay(void);
  void HideKeyboardOverlay(void);
  void SetKeyboardOverlay(unsigned char x, unsigned char y);
  unsigned char GetKeyboardOverlay(void); 
  unsigned char KeyboardOverlayHit(void); 
#endif

// Printing functions (see print.c)
extern unsigned char inkColor, paperColor;
#if defined __ORIC__
  void SetAttributes(signed char col, unsigned char row, unsigned char color);
#endif
void PrintBlanks(unsigned char col, unsigned char row, unsigned char width, unsigned char height);
void PrintChr(unsigned char col, unsigned char row, const char *chr);
void PrintNum(unsigned char col, unsigned char row, unsigned int num);
void PrintStr(unsigned char col, unsigned char row, const char *buffer);
void PrintLogo(unsigned char col, unsigned char row, unsigned char index);
void PrintBuffer(char *buffer);
void InputStr(unsigned char col, unsigned char row, char *buffer, unsigned char len);
unsigned char InputUpdate(unsigned char col, unsigned char row, char *buffer, unsigned char len, unsigned char key);
const char *GetChr(unsigned char chr);

// Joystick/Mouse definitions
#define JOY_UP      1
#define JOY_DOWN    2
#define JOY_LEFT    4
#define JOY_RIGHT   8
#define JOY_BTN1    16
#define JOY_BTN2    32
#define MOUSE_LEFT  64
#define MOUSE_MIDDL 64
#define MOUSE_RIGHT 128

// Joystick functions
#if (defined __ORIC__) || (defined __CBM__)
  #define JOY_MAX 4
  void InitJoy(void);
  unsigned char GetJoy(unsigned char);
#elif (defined __LYNX__)
  #define JOY_MAX 4
  #define InitJoy() (PEEK(0))	// Dummy function
  unsigned char GetJoy(unsigned char);
#else
  #define JOY_MAX 2
  #define InitJoy() (PEEK(0))	// Dummy function
  #if defined __ATARI__
    #define GetJoy(i) (PEEK(0x0278+i)+(PEEK(0x0284+i)<<4))
  #else if defined __APPLE2__
    unsigned char GetJoy(unsigned char);		
    unsigned char GetPaddle(unsigned char);
    #define GetButton(i) (PEEK(0xC061+i)>127)
  #endif
#endif

// 2D geometry functions
unsigned char IntersectSegments(signed int seg1X1, signed int seg1Y1, signed int seg1X2, signed int seg1Y2,
							    signed int seg2X1, signed int seg2Y1, signed int seg2X2, signed int seg2Y2,
								signed int *intX,  signed int *intY);
unsigned char IntersectSegmentPolygon(signed int segX1, signed int segY1, signed int segX2, signed int segY2, 
									  unsigned char vN, signed int *vX, signed int *vY, signed int *intX, signed int *intY);								
unsigned char PointInsidePolygon(signed int pX, signed int pY, unsigned char vN, signed int *vX, signed int *vY);

// Math functions (see math.s)
#define MIN(a,b) (a>b ? b : a)
#define MAX(a,b) (a>b ? a : b)
#define ABS(a)   (a<0 ? -a : a)
#define SIGN(a)  (a<0 ? -1 : 1)
#define DOT(a,b) (a[0]*b[0]+a[1]*b[1])
unsigned char atan2(unsigned char y, unsigned char x);

// Music functions
// Apple: ElectricDuet (see Apple/DUET.s) 
// Atari: RMT track (see Atari/POKEY.s)
// C64:   SID track (see C64/SID.s)
// Lynx:  Chipper   (see Lynx/CHIPPER.s)
// Oric:  YM track  (see Oric/MYM.s)
void LoadMusic(const char* filename, unsigned int address);
void PlayMusic(unsigned int address);
void StopMusic(void);

// SFX functions (see sfx.c)
void InitSFX(void);
void StopSFX(void);
void BleepSFX(unsigned char tone);
void BumpSFX(void);
void EngineSFX(unsigned int channel, unsigned int rpm);
void ScreechSFX(unsigned char channel, unsigned char pitch);

// Sprite handling functions
#if defined __APPLE2__
  #define SPRITE_NUM 8
#elif defined __ATARI__
  #define SPRITE_NUM 10
#elif defined __ORIC__
  #define SPRITE_NUM 8
#elif defined __CBM__
  #define SPRITE_NUM 8
#elif defined __LYNX__
  #define SPRITE_NUM 8  
#endif
void InitSprites(unsigned char frames, unsigned char cols, unsigned char rows, unsigned char *spriteColors);
void EnableSprite(signed char index);
void DisableSprite(signed char index);
void LocateSprite(unsigned int x, unsigned int y);
void SetSprite(unsigned char index, unsigned char frame);
void RecolorSprite(unsigned char index, unsigned char number, unsigned char color);
#if defined __APPLE2__
  void CropSprite(unsigned char index, unsigned char rows);
#elif defined __ATARI__
  void DoubleHeightSprite(unsigned char index, unsigned char onoff);
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
#endif

// Sprite collisions: Hardware on Atari/C64 vs. Software on Apple/Oric/Lynx 
#if defined __ATARI__	
  #define COLLISIONS(i) PEEK(53260+i); POKE(53278,0)	// On Atari, reset collisions by poking 0 into 53278
#elif defined __CBM__
  #define COLLISIONS(i) (PEEK(53278))		// On C64, all collisions are contained within a single register
#else
  extern unsigned char sprCollision[SPRITE_NUM];
  #define COLLISIONS(i) (sprCollision[i])
#endif
#define COLLIDING(collisions,i) ((collisions >> i) & 1) 

// Widget handling functions
#ifndef CALLDEF
#define CALLDEF
typedef struct {
	unsigned char colBeg, colEnd, rowBeg, rowEnd;
	unsigned char id, type, *label;
	unsigned int next;
} callback;
#endif
#define CALLTYPE_ICON  	 1
#define CALLTYPE_BUTTON  2
#define CALLTYPE_INPUT   3
#define CALLTYPE_LISTBOX 4
callback* Button(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* label);
callback* Input(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* label);
void ListBox(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title, unsigned char* labels[], unsigned char len);
void Panel(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title);
callback* PushCallback(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char type, unsigned char* label);
callback* CheckCallbacks(unsigned char col, unsigned char row);
void PopCallback(callback* call);
void ClearCallbacks(void);

// Network functions (see net-init.c, net-udp.c, net-tcp.c, net-web.c)
#define NETWORK_OK  0
#define ADAPTOR_ERR 1
#define DHCP_ERR    2
unsigned char InitNetwork(void);							// Initialize network adapter
unsigned char GetLocalIP(unsigned char* ip);				// Fetch local IP

void SlotTCP(unsigned char slot);							// Set TCP slot (0~15)
void OpenTCP(unsigned char ip1, unsigned char ip2, 			// Open connection on current TCP slot (local port allocated automatically)
			 unsigned char ip3, unsigned char ip4, 
			 unsigned int svPort);
void CloseTCP(void);										// Close current TCP slot
void SendTCP(unsigned char* buffer, unsigned char length);  // Send contents of buffer on current TCP slot
unsigned int RecvTCP(unsigned int timeOut);					// Check all slots for incoming TCP packet (within time-out period)

void SlotUDP(unsigned char slot);							// Set UDP slot (0~15)
void OpenUDP(unsigned char ip1, unsigned char ip2, 			// Open connection on current UDP slot (local port allocated on clPort)
			 unsigned char ip3, unsigned char ip4, 
			 unsigned int svPort, unsigned int clPort);
void CloseUDP(void);										// Close current UDP slot
void SendUDP(unsigned char* buffer, unsigned char length);  // Send contents of buffer on current UDP slot
unsigned int RecvUDP(unsigned int timeOut);					// Check all slots for incoming UDP packet (within time-out period)

void OpenWEB(unsigned int port, unsigned int timeOut);		// Start WEB server on specified port (time-out in millisecs)
void CloseWEB(void);										// Close WEB server
void HeaderWEB(unsigned char* buffer, unsigned char length);// Header of reply to current WEB client
void BodyWEB(unsigned char* buffer, unsigned char length);  // Body of reply to current WEB client
void SendWEB(void);											// Send reply to current WEB client
unsigned int RecvWEB(unsigned int timeOut);					// Check WEB server for incoming packet (within time-out period)

// 8bit-Hub support (see http://www.8bit-unity.com/8bit-Hub)
#if defined __HUB__
  void UpdateHub(void);
  unsigned char QueueHub(unsigned char packetCmd, unsigned char* packetBuffer, unsigned char packetLen);
  extern unsigned char recvLen, recvHub[256];
  extern unsigned char sendLen, sendHub[256];
  extern unsigned char hubState[7];
#endif

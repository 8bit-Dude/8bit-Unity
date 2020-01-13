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
 */

// CC65 includes
#include <conio.h>
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

// Useful functions
#define MIN(a,b) (a>b ? b : a)

// Colors for printing
extern unsigned char inkColor, paperColor;
#if defined __ATMOS__
  void SetInk(unsigned char col, unsigned char row);
#endif

// Bitmap functions (see bitmap.c)
void InitBitmap(void);
void EnterBitmapMode(void);
void ExitBitmapMode(void);
void ClearBitmap(void);
void LoadBitmap(char *filename);
void LocatePixel(unsigned int x, unsigned int y);
unsigned char GetPixel(void);
void SetPixel(unsigned char color);
void DrawPanel(unsigned char colBeg, unsigned char rowBeg, unsigned char colEnd, unsigned char rowEnd);
void PrintChr(unsigned char col, unsigned char row, const char *chr);
void PrintNum(unsigned char col, unsigned char row, unsigned int num);
void PrintStr(unsigned char col, unsigned char row, const char *buffer);
void PrintLogo(unsigned char col, unsigned char row, unsigned char index);
void PrintBuffer(char *buffer);
void InputStr(unsigned char col, unsigned char row, char *buffer, unsigned char len);
unsigned char InputUpdate(unsigned char col, unsigned char row, char *buffer, unsigned char len, unsigned char key);
const char *GetChr(unsigned char chr);

// Screen coordinates of current pixel (see LocatePixel())
extern unsigned char pixelX, pixelY;

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
#if (defined __CBM__) || (defined __ATMOS__)
  #define JOY_MAX 4
  void InitJoy(void);
  unsigned char GetJoy(unsigned char);
#elif (defined __LYNX__)
  #define JOY_MAX 3
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

// Math functions (see math.s)
#define DOT(a,b) (a[0]*b[0]+a[1]*b[1])
unsigned char atan2(unsigned char y, unsigned char x);

// Network functions (see network.c)
#define NETWORK_OK  0
#define ADAPTOR_ERR 1
#define DHCP_ERR    2
unsigned char InitNetwork(void);							// Initialize network interface and get IP from DHCP
void InitUDP(unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4, unsigned int svPort, unsigned int clPort);	// Setup UDP connection
void SendUDP(unsigned char* buffer, unsigned char length);  // Send UDP packet (of specified length)
unsigned int RecvUDP(unsigned int timeOut);				// Fetch UDP packet (within time-out period)

// Music functions
// Apple: ElectricDuet (see Apple/DUET.s) 
// Atari: RMT track (see Atari/POKEY.s)
// C64:   SID track (see C64/SID.s)
// Lynx:  Chipper   (see Lynx/CHIPPER.s)
// Oric:  YM track  (see Oric/MYM.s)
#if defined __APPLE2__
  extern unsigned char sfxOutput;	// 0 = Speaker, 1 = Mockingboard (Slot 4)
  void InitMocking(void);
#endif
void LoadMusic(const char* filename, unsigned int address);
void PlayMusic(unsigned int address);
void StopMusic(void);

// SFX functions (see sfx.c)
void InitSFX(void);
void StopSFX(void);
void EngineSFX(int channel, int vel);
void BleepSFX(unsigned char tone);
void BumpSFX(void);

// Sprite handling functions
#if defined __APPLE2__
  #define SPRITE_NUM 8
#elif defined __ATARI__
  #define SPRITE_NUM 10
#elif defined __ATMOS__
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
  extern unsigned char sprCOL[SPRITE_NUM];
  #define COLLISIONS(i) (sprCOL[i])
#endif
#define COLLIDING(collisions,i) ((collisions >> i) & 1) 

// 8bit-Hub support (see http://www.8bit-unity.com/8bit-Hub)
#if defined __HUB__
  void InitHub(void);
  unsigned char NextID(void);
  void UpdateHub(unsigned char timeout);
  extern unsigned char recvLen, recvHub[192];
  extern unsigned char sendLen, sendHub[192];
  extern unsigned char hubState[5];
#endif

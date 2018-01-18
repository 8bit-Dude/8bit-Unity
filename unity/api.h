/*
	API of the "8bit-Unity" SDK for CC65
	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
	
	Author: Anthony Beaucamp
	Last modified: 2018/01/18
	
	Credits: 
		* Oliver Schmidt for programming the IP65 network interface
		* Christian Groessler for helping optimize the memory maps on Commodore and Atari
*/

// CC65 includes
#include <conio.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Debugging flags
#define DEBUG_FPS
//#define DEBUG_NAV

// Platform IDs
#if defined __CBM__
	#define PLATFORM   0
#elif defined __ATARI__
	#define PLATFORM   1
#elif defined __APPLE2__
	#define PLATFORM   2
	#define CLK_TCK   60	// Missing definition
	#define CH_DEL	   0
#endif

// Memory locations
#if defined __CBM__
	// C64 Memory locations
	#define VIDEOBANK  3
	#define SCREENLOC  0
	#define BITMAPLOC  8
	#define SPRITELOC  28
	#define SPRITELEN  64 		// Byte length of 1 sprite;
	
	#define COLORRAM   (0xD800) // Fixed location
	#define SCREENRAM  (VIDEOBANK * 0x4000 + SCREENLOC * 0x0400) // 0xC000
	#define BITMAPRAM  (VIDEOBANK * 0x4000 + BITMAPLOC * 0x0400) // 0xE000
	#define SPRITEPTR  (SCREENRAM + 0x03F8)						 // 0xC3F8 (sprite position/frame)
	#define SPRITERAM  (VIDEOBANK * 0x4000 + SPRITELOC * 0x0040) // 0xC700 (sprites.prg is loaded at this address)
#elif defined __ATARI__
	// Atari Memory locations
	#define SPRITELEN  13 		// Byte length of 1 sprite;
	#define PMG5	   (0x8f0b) // 5th sprite flicker routine
	#define PMGRAM     (0x9800) // Player missile memory
	#define SPRITERAM1 (0x9700)	// Sprite data
	#define SPRITERAM2 (0x98A0)	// 5th sprite data
	
	#define STARTBMP   (0x8E10) // Start Bitmap routine
	#define STOPBMP    (0x8E65) // Stop Bitmap routine
	#define PALETTERAM (0x7000) // Palette data
	#define PALETTETOG (0xBFA4) // Palette toggle (RGB/BW)
	#define BITMAPRAM1 (0x7010) // Colour data
	#define BITMAPRAM2 (0xA010) // Shade data
#elif defined __APPLE2__
	// Apple Memory locations
	#define BITMAPRAM  (0x2000)
#endif

// Color definitions
#if defined __CBM__
	// C64 Colors
	#define BLACK  		COLOR_BLACK
	#define BLUE   		COLOR_BLUE
	#define DARKGRAY   	COLOR_GRAY2	
	#define GRAY   		COLOR_GRAY3	
	#define GREEN  		COLOR_GREEN
	#define LIGHTGREEN  COLOR_LIGHTGREEN
	#define PURPLE 		COLOR_PURPLE
	#define RED    		COLOR_RED
	#define YELLOW 		COLOR_YELLOW
	#define WHITE  		COLOR_WHITE
#elif defined __ATARI__
	// Atari Colors
	#define BLACK  		0
	#define BLUE   		9
	#define DARKGRAY   	1
	#define GRAY   		2
	#define GREEN  	   15
	#define PURPLE     11
	#define RED    		6
	#define YELLOW 		7
	#define WHITE  		3
#elif defined __APPLE2__
	// Apple Colors
	#define BLACK   	0
	#define RED     	1
	#define DARKKBLUE	2
	#define PURPLE  	3
	#define LIGHTGREEN	4
	#define GRAY   		5
	#define DARKGRAY   	5
	#define BLUE		6
	#define LIGHTTBLUE 	7
	#define BROWN   	8
	#define ORANGE  	9
	#define GREY    	10
	#define PINK    	11
	#define GREEN		12
	#define YELLOW  	13
	#define AQUA    	14
	#define WHITE   	15
#endif	

// Bitmap functions (see bitmap.c)
void EnterBitmapMode(void);
void ExitBitmapMode(void);
void ClearBitmap(void);
void LoadBitmap(char *filename);
unsigned char GetColor(unsigned int x, unsigned int y);
void SetColor(unsigned int x, unsigned int y, unsigned char color);
void PrintChr(unsigned char col, unsigned char row, const char *matrix);
void PrintStr(unsigned char col, unsigned char row, const char *buffer);
void PrintLogo(unsigned char col, unsigned char row, unsigned char index);
void PrintInput(unsigned char col, unsigned char row, char *buffer, unsigned char len);
void PrintHeader(const char *buffer);
char KeyStr(char *buffer, unsigned char len, unsigned char key);
void InputStr(unsigned char col, unsigned char row, char *buffer, unsigned char len);
#ifdef DEBUG_FPS
void DrawFPS(unsigned long  f);
extern clock_t tFPS;
#endif

// C64 specific functions (see C64/ROM.s)
#ifdef __CBM__
extern void DisableRom();	// Disable ROM before using GetColor()
extern void EnableRom();	// Enable ROM after using GetColor()
#endif

// Character data (see char.s)
extern const char charBlank[3];
extern const char charDigit[30];
extern const char charLetter[78];
extern const char charBracket[6];
extern const char charColon[3];
extern const char charComma[3];
extern const char charDot[3];
extern const char charExclaim[3];
extern const char charHyphen[3];
extern const char charQuestion[3];
extern const char charQuote[3];
extern const char charSlash[3];
extern const char charUnderbar[3];

// Colors for printing
extern unsigned char bg, bgChat, fgCol, bgCol;

// Joystick definitions
#undef JOY_LEFT
#undef JOY_RIGHT
#undef JOY_FIRE
#define JOY_LEFT  2
#define JOY_RIGHT 3
#define JOY_FIRE  4

// Joystick functions
#if defined __CBM__
	// Joystick 1&2 support
	#define getJoy(i) (PEEK(56320+i))
		
	// Joystick 3&4 support (see C64/JOY34.s)
	void initJoy34();
	void updateJoy34();
	unsigned char getJoy3();
	unsigned char getJoy4();
#elif defined __ATARI__
	// Joystick 1&2 support
	#define getJoy(i) (PEEK(0x0278+i)+(PEEK(0x0284+i)<<4))
	
	// Joystick 3&4 support (not implemented)	
	void initJoy34() {}
	void updateJoy34() {}
	unsigned char getJoy3() {}
	unsigned char getJoy4() {}
#elif defined __APPLE2__
	// Joystick 1&2 support
	#define getJoy(i) (0)
	
	// Joystick 3&4 support (not implemented)	
	void initJoy34() {}
	void updateJoy34() {}
	unsigned char getJoy3() {}
	unsigned char getJoy4() {}
#endif

// Math functions (see math.s)
unsigned char atan2(unsigned char y, unsigned char x);

// Dot product of 2 vectors
#define DOT(a,b) (a[0]*b[0]+a[1]*b[1])

// Network functions (see IP65.lib)
#ifndef __APPLE2__			
	unsigned char ip65_init(void);
	unsigned char ip65_process(void);
	unsigned char dhcp_init(void);
	unsigned char __fastcall__ udp_send(const unsigned char* buf, unsigned int len, unsigned long dest, unsigned int dest_port, unsigned int src_port);
	unsigned char __fastcall__ udp_add_listener(unsigned int port, void (*callback)(void));
	unsigned char __fastcall__ udp_remove_listener(unsigned int port);
	extern unsigned char udp_recv_buf[192];   // Buffer with data received
#else
	// Temporary name holders until freeing-up memory
	unsigned char ip65_init(void) { return 0; }
	unsigned char ip65_process(void) { return 0; }
	unsigned char dhcp_init(void) { return 0; }
	unsigned char udp_send(const unsigned char* buf, unsigned int len, unsigned long dest, unsigned int dest_port, unsigned int src_port) { return 0; }	
	unsigned char udp_add_listener(unsigned int port, void (*callback)(void)) { return 0; };	
	unsigned char udp_remove_listener(unsigned int port) { return 0; };	
	unsigned char udp_recv_buf[192];
#endif

// Music functions
#if defined __CBM__
	// SID music player (see C64/SID.s)
	void PlayMusic();
	void StopMusic();
#elif defined __ATARI__
	// RMT/SFX players (see Atari/POKEY.s)
	void PlayMusic();
	void StopMusic();
	extern unsigned char sampleCount;
	extern unsigned char sampleFreq;
	extern unsigned char sampleCtrl;
#elif defined __APPLE2__
	// Nothing for now
	void PlayMusic() {}
	void StopMusic() {}
#endif

// SFX functions
void InitSFX(void);
void EngineSFX(int channel, int vel);
void BleepLowSFX(void);
void BleepHighSFX(void);
void BumpSFX(void);

// Sprite functions
void InitSprites(unsigned char *colors);
void EnableSprite(signed char index);
void DisableSprite(signed char index);
void SetSprite(unsigned char index, unsigned int frame, unsigned int x, unsigned int y);

// Sprite collision functions
#if defined __CBM__
	// On C64, all collisions are contained within a single register
	#define COLLISIONS(i) (PEEK(53278))
	#define COLLIDING(collisions,i) ((collisions >> i) & 1) 
#elif defined __ATARI__	
	// On Atari, we need to reset collisions by poking 0 into 53278
	#define COLLISIONS(i) (PEEK(53260+i)+(1<<i)); POKE(53278,0)
	#define COLLIDING(collisions,i) ((collisions >> i) & 1) 
#elif defined __APPLE2__	
	// On Apple, there is no collision detection at present
	#define COLLISIONS(i) (0)
	#define COLLIDING(collisions,i) (0) 
#endif




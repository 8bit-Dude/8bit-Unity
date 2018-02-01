/*
	API of the "8bit-Unity" SDK for CC65
	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
	
	Author: Anthony Beaucamp
	Last modified: 2018/01/18
	
	Credits: 
		* Oliver Schmidt for his IP65 network interface
		* Christian Groessler for helping optimize the memory maps on Commodore and Atari
		* Bill Buckels for his Apple II Double Hi-Res bitmap code
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
//#define DEBUG_FPS

// Platform IDs/Specs
#if defined __CBM__
	#define PLATFORM   0
	#define LAST_LINE 24	
#elif defined __ATARI__
	#define PLATFORM   1
	#define LAST_LINE 23	
#elif defined __APPLE2__
	#define PLATFORM   2
	#define LAST_LINE 23	
	#define CH_DEL  0x08
#endif

// Memory locations
#if defined __CBM__
	// C64 Memory locations
	#define VIDEOBANK  3
	#define SCREENLOC  0
	#define BITMAPLOC  8
	#define SPRITELOC  28
	#define COLORRAM   (0xD800) // Fixed location
	#define SCREENRAM  (VIDEOBANK * 0x4000 + SCREENLOC * 0x0400) // 0xC000
	#define BITMAPRAM  (VIDEOBANK * 0x4000 + BITMAPLOC * 0x0400) // 0xE000
	#define SPRITEPTR  (SCREENRAM + 0x03F8)						 // 0xC3F8 (sprite position/frame)
	#define SPRITERAM  (VIDEOBANK * 0x4000 + SPRITELOC * 0x0040) // 0xC700 (sprites.prg is loaded at this address)
#elif defined __ATARI__
	// Atari Memory locations
	#define PMGRAM     (0x9800) // Player missile memory
	#define SPRITERAM1 (0x9700)	// Sprite data
	#define SPRITERAM2 (0x98A0)	// 5th sprite data
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
	#define DRKGRAY   	COLOR_GRAY1	
	#define MEDGRAY 	COLOR_GRAY2	
	#define LITGRAY   	COLOR_GRAY3	
	#define GREEN  		COLOR_GREEN
	#define LITGREEN    COLOR_LIGHTGREEN
	#define PURPLE 		COLOR_PURPLE
	#define RED    		COLOR_RED
	#define YELLOW 		COLOR_YELLOW
	#define WHITE  		COLOR_WHITE
#elif defined __ATARI__
	// Atari Colors
	#define BLACK  		0
	#define BLUE   		9
	#define DRKGRAY   	1
	#define MEDGRAY   	2
	#define LITGRAY     3
	#define GREEN  	   15
	#define PURPLE     11
	#define RED    		6
	#define YELLOW 		7
	#define WHITE  		3
#elif defined __APPLE2__
	// Apple Colors
	#define BLACK   	0
	#define RED     	1
	#define DRKBLUE	    2
	#define PURPLE  	3
	#define DRKGREEN	4
	#define MEDGRAY   	5
	#define LITGRAY   	5
	#define MEDBLUE		6
	#define BLUE		6
	#define LITBLUE 	7
	#define BROWN   	8
	#define ORANGE  	9
	#define DRKGREY 	10
	#define PINK    	11
	#define MEDGREEN	12
	#define GREEN		12
	#define YELLOW  	13
	#define LITGREEN   	14
	#define WHITE   	15
#endif

// Keyboard definitions
	#define KEY_SP		' '
#if defined __APPLE2__
	#define KEY_A		'A'
	#define KEY_B		'B'
	#define KEY_C		'C'
	#define KEY_D		'D'
	#define KEY_F		'F'
	#define KEY_G		'G'
	#define KEY_H		'H'
	#define KEY_I		'I'
	#define KEY_J		'J'
	#define KEY_L		'L'
	#define KEY_M		'M'
	#define KEY_O		'O'
	#define KEY_Q		'Q'	
	#define KEY_W		'W'
#else
	#define KEY_A		'a'	
	#define KEY_B		'b'	
	#define KEY_C		'c'	
	#define KEY_D		'd'	
	#define KEY_F		'f'
	#define KEY_G		'g'
	#define KEY_H		'h'
	#define KEY_I		'i'
	#define KEY_J		'j'
	#define KEY_L		'l'
	#define KEY_M		'm'
	#define KEY_O		'o'
	#define KEY_Q		'q'	
	#define KEY_W		'w'
#endif

// Bitmap functions (see bitmap.c)
void InitBitmap(void);
void EnterBitmapMode(void);
void ExitBitmapMode(void);
void ClearBitmap(void);
void LoadBitmap(char *filename);
unsigned char GetColor(unsigned int x, unsigned int y);
void SetColor(unsigned int x, unsigned int y, unsigned char color);
void DrawPanel(unsigned char colBeg, unsigned char rowBeg, unsigned char colEnd, unsigned char rowEnd);
void PrintChr(unsigned char col, unsigned char row, const char *matrix);
void PrintStr(unsigned char col, unsigned char row, const char *buffer);
void PrintLogo(unsigned char col, unsigned char row, unsigned char index);
void PrintInput(unsigned char col, unsigned char row, char *buffer, unsigned char len);
void PrintHeader(const char *buffer);
char KeyStr(char *buffer, unsigned char len, unsigned char key);
void InputStr(unsigned char col, unsigned char row, char *buffer, unsigned char len);
#ifdef DEBUG_FPS
void DrawFPS(unsigned long  f);
extern clock_t fpsClock;
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
extern unsigned char fgCol, bgCol, bgHeader;

// Joystick definitions
#if defined __APPLE2__
	#define JOY_LEFT  4
	#define JOY_RIGHT 8
	#define JOY_UP    16
	#define JOY_DOWN  32
	#define JOY_FIRE  64
#else 
	#define JOY_UP    1
	#define JOY_DOWN  2
	#define JOY_LEFT  4
	#define JOY_RIGHT 8
	#define JOY_FIRE  16
#endif

// Joystick functions
#if defined __CBM__
	#define JOY_MAX	 4
	// Joystick 1&2
	#define GetJoy(i) (PEEK(56321-(i)))		
	// Joystick 3&4 (see C64/JOY34.s)	
	void InitJoy34(void);
	unsigned char GetJoy3(void);
	unsigned char GetJoy4(void);
#else
	#define JOY_MAX	 2
	// Joystick 1&2
	#if defined __ATARI__
		#define GetJoy(i) (PEEK(0x0278+i)+(PEEK(0x0284+i)<<4))
	#else if defined __APPLE2__
		#define GetJoy(i) (GetJoy12(i))
		unsigned char GetJoy12(unsigned char);
	#endif
	// Joystick 3&4 (placeholders)
	#define InitJoy34()   (0)
	#define GetJoy3() 	  (255)
	#define GetJoy4()     (255)
#endif

// Math functions (see math.s)
#define DOT(a,b) (a[0]*b[0]+a[1]*b[1])
unsigned char atan2(unsigned char y, unsigned char x);

// Network functions (see IP65.lib)
/* #ifndef __APPLE2__			 */
	unsigned char ip65_init(void);
	unsigned char ip65_process(void);
	unsigned char dhcp_init(void);
	unsigned long __fastcall__ parse_dotted_quad(char* quad);
	unsigned char __fastcall__ udp_send(const unsigned char* buf, unsigned int len, unsigned long dest, unsigned int dest_port, unsigned int src_port);
	unsigned char __fastcall__ udp_add_listener(unsigned int port, void (*callback)(void));
	unsigned char __fastcall__ udp_remove_listener(unsigned int port);
	extern unsigned char udp_recv_buf[192];   // Buffer with data received
/* #else
	// Temporary name holders until freeing-up memory
	unsigned char ip65_init(void) { return 0; }
	unsigned char ip65_process(void) { return 0; }
	unsigned char dhcp_init(void) { return 0; }
	unsigned char udp_send(const unsigned char* buf, unsigned int len, unsigned long dest, unsigned int dest_port, unsigned int src_port) { return 0; }	
	unsigned char udp_add_listener(unsigned int port, void (*callback)(void)) { return 0; };	
	unsigned char udp_remove_listener(unsigned int port) { return 0; };	
	unsigned char udp_recv_buf[192];
#endif */

// Music functions
// C64: SID music player (see C64/SID.s)
// Atari: RMT/SFX players (see Atari/POKEY.s)
// Apple: Not implemented yet
void PlayMusic(void);
void StopMusic(void);

// SFX functions (see sfx.c)
void InitSFX(void);
void EngineSFX(int channel, int vel);
void BleepLowSFX(void);
void BleepHighSFX(void);
void BumpSFX(void);

// Sprite definitions
#if defined __CBM__
	#define SPRITE_NUM	   8
	#define SPRITE_LENGTH 64 		// Byte length of 1 sprite
#elif defined __ATARI__	
	#define SPRITE_NUM	   5
	#define SPRITE_LENGTH 13 		// Byte length of 1 sprite
#elif defined __APPLE2__
	#define SPRITE_NUM	   4
	#define SPRITE_WIDTH   4		// 4 bytes = 7 pixels
	#define SPRITE_HEIGHT  5
	#define SPRITE_FRAMES 16
	#define SPRITE_LENGTH SPRITE_WIDTH*SPRITE_HEIGHT	// Byte length of 1 sprite;
#endif	
	
// Sprite functions
#if defined __APPLE2__
	void InitSprites(const char *filename);
	unsigned char GetBGColor(unsigned char index);
#else
	void InitSprites(unsigned char *colors);
#endif	
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
	// On Apple, collisions are prevented at draw time
	#define COLLISIONS(i) (0)
	#define COLLIDING(collisions,i) (sprCOL[i])
	extern unsigned char sprCOL[SPRITE_NUM];
#endif

// Apple DHR functions, for code optimization (see Apple/DHR.c)
#if defined __APPLE2__
extern unsigned char *dhrmain, *dhraux, *dhrptr, dhrpixel;
void SetDHRPointer(unsigned int x, unsigned int y);
void SetDHRColor(unsigned char color);
unsigned char GetDHRColor(void);
#endif

// Workaround for missing Apple clock
#if defined __APPLE2__
	#define CLK_TCK 59	// Missing definition
	void tick(void);
	clock_t clock(void);
	unsigned sleep(unsigned seconds);
	extern clock_t clk;	
#endif

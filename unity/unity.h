/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2019/05/05
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

// Platform IDs
#if defined __CBM__
	#define PLATFORM   0
	#define LAST_LINE 24	
#elif defined __ATARI__
	#define PLATFORM   1
	#define LAST_LINE 24	
#elif defined __APPLE2__
	#define PLATFORM   2
	#define LAST_LINE 23	
	#define CH_DEL  0x08
#elif defined __ATMOS__
	#define PLATFORM   3
	#define LAST_LINE 24	
#endif

// Memory locations
#if defined __CBM__
	// C64 Memory locations
	#define VIDEOBANK  3
	#define SCREENLOC  0
	#define BITMAPLOC  8
	#define SPRITELOC  28
	#define MUSICRAM   (0xb000) 								 // B000-BFFF (SID sound track: ALSO EDIT SID.S WHEN CHANGING THIS VALUE!)
	#define SCREENRAM  (VIDEOBANK * 0x4000 + SCREENLOC * 0x0400) // C000-C3FF (char data)
	#define COLORRAM   (0xd800) 								 // D800-DBFF (color data; fixed location)
	#define BITMAPRAM  (VIDEOBANK * 0x4000 + BITMAPLOC * 0x0400) // E000-FFFF (bitmap data)
	#define SPRITEPTR  (SCREENRAM + 0x03f8)						 // C3F8-???? (sprite control)
	#define SPRITERAM  (VIDEOBANK * 0x4000 + SPRITELOC * 0x0040) // C700-CFFF (sprites.prg loaded here)
#elif defined __ATARI__
	// Atari Memory locations
	#define DLIRAM	   (0x6550) // 6550-66df (DLI list and sprite flicker routine)
								// 6f50-700c (START/STOP routines for bitmap mode)
	#define RMTPLAYER  (0x66e0) // 66e0-6f4d (RMT music player; JSR to 0x6A00)
	#define BITMAPRAM1 (0x7010) // 7010-8f50 (bitmap frame 1)
	#define MUSICRAM   (0x9000) // 9000-96ff (RMT sound track)
	#define SPRITERAM  (0x9700)	// 9700-9aff (sprite data loaded here, overlaps with unused part of PMGRAM)
	#define PMGRAM     (0x9800) // 9800-9fff (player missile memory)
	#define PALETTERAM (0xa000) // a000-a003 (palette data)
	#define BITMAPRAM2 (0xa010) // a010-bf50 (bitmap frame 2)
	// External Routines/Variables 
	#define FLICKDATA  (0x6622) // Sprite flicker variables (see DLI.a65)
	#define BLENDTOG   (0x6f50) // Toggle for frame blending ON/OFF (see DLI.a65)
	#define STARTBMP   (0x6f5b) // Start Bitmap routine (see DLI.a65)
	#define STOPBMP    (0x6fa4) // Stop Bitmap routine (see DLI.a65)	
#elif defined __APPLE2__
	// Apple Memory locations
	#define BITMAPRAM  (0x2000)
	#define MUSICRAM   (0xa800) // A800-AAFF (electric duet track loaded here)
	#define SPRITERAM  (0xab00)	// AB00-BEFF (sprites.app loaded here)
#elif defined __ATMOS__
	// Atmos Memory locations
	#define MUSICRAM   (0x9C00) // 9C00-9fff
	#define SPRITERAM  (0x9C00)	// 9C00-9fff
	#define BITMAPRAM  (0xA000) // A000-Bfff
#endif

// Screen/Palette definitions
#if defined __CBM__
	// C64 Screen (Multi-Color Mode)
	#define BMPWIDTH  160
	#define BMPHEIGHT 200
	#define BMPCOLORS 16
	// C64 Palette
	#define BLACK  	0
	#define WHITE  	1
	#define RED    	2
	#define CYAN   	3
	#define PURPLE 	4
	#define GREEN  	5
	#define BLUE   	6
	#define YELLOW 	7
	#define ORANGE 	8
	#define BROWN 	9
	#define PINK 	10
	#define DGREY   11	
	#define MGREY 	12
	#define GREY    12	
	#define LGREEN  13
	#define LBLUE   14
	#define LGREY   15	
#elif defined __ATARI__
	// Atari Screen (INP Mode)
	#define BMPWIDTH  160
	#define BMPHEIGHT 200
	#define BMPCOLORS 11
	// Atari Palette
	#define BLACK  	0
	#define DBLUE   2
	#define DGREEN  3
	#define BROWN 	4
	#define RED    	5
	#define ORANGE 	7
	#define GREY   	9
	#define WHITE  	9
	#define BLUE   	10
	#define GREEN  	11
	#define YELLOW 	15
#elif defined __APPLE2__
	// Apple Screen (DHR Mode)
	#define BMPWIDTH  140
	#define BMPHEIGHT 192
	#define BMPCOLORS 16
	// Apple Palette
	#define BLACK   0
	#define DBLUE	1
	#define DGREEN	2
	#define BLUE	3
	#define BROWN	4
	#define GREY	5
	#define GREEN	6
	#define LGREEN	7
	#define RED 	8
	#define PURPLE  9
	#define LGREY	10
	#define LBLUE	11
	#define ORANGE	12
	#define PINK	13
	#define YELLOW  14
	#define WHITE   15
#elif defined __ATMOS__	
	// Oric Screen (AIC Mode)
	#define BMPWIDTH  117
	#define BMPHEIGHT 100
	#define BMPCOLORS 20
	// Oric Palette
	#define BLACK   0
	#define MGREEN	1
	#define DGREEN	2
	#define CYAN	3
	#define LGREEN	4
	#define GREY	5
	#define MBLUE	6
	#define DBLUE	7
	#define LBLUE 	8
	#define AQUA    9
	#define GREY2	10
	#define BROWN	11
	#define YELLOW	12
	#define RED		13
	#define ORANGE  14
	#define WHITE   15
	#define DPINK   16
	#define LPURPLE 17
	#define LPINK   18
	#define PURPLE  19
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
	#define KEY_S		'S'	
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
	#define KEY_S		's'	
	#define KEY_W		'w'
#endif

// Location of current pixel (see LocatePixel(), SetPixel(), GetPixel())
extern unsigned char bmpX, bmpY;

// Colors for printing
extern unsigned char inkColor, paperColor, paperHeader;

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
void PrintNum(unsigned char col, unsigned char row, unsigned char num);
void PrintStr(unsigned char col, unsigned char row, const char *buffer);
void PrintLogo(unsigned char col, unsigned char row, unsigned char index);
void PrintHeader(const char *buffer);
void InputStr(unsigned char col, unsigned char row, char *buffer, unsigned char len);
unsigned char InputUpdate(unsigned char col, unsigned char row, char *buffer, unsigned char len, unsigned char key);
const char *GetChr(unsigned char chr);

// C64 specific functions (see C64/ROM.s)
#ifdef __CBM__
  extern void DisableRom();	// Disable ROM before using GetColor()
  extern void EnableRom();	// Enable ROM after using GetColor()
#endif

// Apple Double-HiRes functions (see Apple/DHR.c)
#if defined __APPLE2__
  extern unsigned char *dhrmain, *dhraux, *dhrptr, dhrpixel;
  void SetDHRPointer(unsigned int x, unsigned int y);
  void SetDHRColor(unsigned char color);
  unsigned char GetDHRColor(void);
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

// Joystick definitions
#define JOY_UP    1
#define JOY_DOWN  2
#define JOY_LEFT  4
#define JOY_RIGHT 8
#define JOY_FIRE  16

// Joystick functions
#if defined __CBM__
  #define JOY_MAX 4
  void InitJoy34(void);
  unsigned char GetJoy(unsigned char);
#elif defined __ATMOS__
  #define JOY_MAX 2
  unsigned char GetKey(unsigned char);
  unsigned char GetJoy(unsigned char);
#else
  #define JOY_MAX 2
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
#define NETW_OK  0
#define ADAP_ERR 1
#define DHCP_ERR 2
#define ParseIP(a,b,c,d) (a<<24+b<<16+c<<8+d)
extern unsigned long udp_send_ip;
extern unsigned int  udp_send_port;
extern unsigned char udp_send_buf[32];
extern unsigned char udp_recv_buf[192];
unsigned char InitNetwork(void);				// Initialize network interface and get IP from DHCP
void ListenUDP(unsigned int port);				// Set listening port for incoming UDP packets
void SetUDPTarget(char *IP, unsigned int port);	// Set UDP target's IP and Port
void SendUDPPacket(unsigned char length);		// Send contents of udp_send_buf[] to UDP target
void RecvUDPPacket(unsigned char timeOut);		// Fetch UDP packet in udp_recv`_buf[] (within time-out period)
unsigned long __fastcall__ parse_dotted_quad(char* quad);

// Music functions
// Apple: Electric Duet player (see Apple/DUET.s) 
// Atari: RMT/SFX player (see Atari/POKEY.s)
// C64:   SID music player (see C64/SID.s)
#if defined __APPLE2__
  extern unsigned char sfxOutput;	// 0 = Speaker, 1 = Mockingboard (Slot 4)
  void InitMocking(void);
#endif
void PlayMusic(unsigned int address);
void StopMusic(void);

// SFX functions (see sfx.c)
void InitSFX(void);
void StopSFX(void);
void EngineSFX(int channel, int vel);
void BleepSFX(unsigned char tone);
void BumpSFX(void);

// Sprite functions
#if defined __APPLE2__
  #define SPRITE_NUM 4
  void InitSprites(unsigned char height, unsigned char frames);	
#elif defined __ATMOS__
  #define SPRITE_NUM 4
  void InitSprites(unsigned char height, unsigned char *spriteColors);
#elif defined __ATARI__
  #define SPRITE_NUM 8
  void InitSprites(unsigned char height, unsigned char *spriteColors);
#elif defined __CBM__
  #define SPRITE_NUM 8
  void InitSprites(unsigned char *spriteColors, unsigned char *sharedColors);
#endif
void EnableSprite(signed char index);
void DisableSprite(signed char index);
void LocateSprite(unsigned int x, unsigned int y);
void UpdateSprite(unsigned char index, unsigned char frame);

// Sprite collision functions
#if defined __CBM__
  // On C64, all collisions are contained within a single register
  #define COLLISIONS(i) (PEEK(53278))
  #define COLLIDING(collisions,i) ((collisions >> i) & 1) 
#elif defined __ATARI__	
  // On Atari, we need to reset collisions by poking 0 into 53278
  #define COLLISIONS(i) (PEEK(53260+i)+(1<<i)); POKE(53278,0)
  #define COLLIDING(collisions,i) ((collisions >> i) & 1) 
#elif defined __APPLE2__ or defined __ATMOS__
  // On Apple and Atmos, collisions are prevented at draw time
  #define COLLISIONS(i) (0)
  #define COLLIDING(collisions,i) (sprCOL[i])
  extern unsigned char sprCOL[SPRITE_NUM];
#endif

// Workaround for missing Apple clock
#if defined __APPLE2__
  #define CLK_TCK 59	// Missing definition
  void tick(void);
  clock_t clock(void);
  unsigned sleep(unsigned seconds);
  extern clock_t clk;	
#endif

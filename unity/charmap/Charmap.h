/*
 * Copyright (c) 2020 Anthony Beaucamp.
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
 */

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

// Zero Page pointers (for tile decoding and scrolling)
#if defined(__APPLE2__)
  #define charPtrZP 0xce
  #define decPtr1ZP 0xfb
  #define decPtr2ZP 0xfd
  #define scrPtr1ZP 0xfb
  #define scrPtr2ZP 0xfd
  #define scrRow1ZP 0xfb
  #define scrCol1ZP 0xfc
  #define scrRow2ZP 0xfd
  #define scrCol2ZP 0xfe  
#elif defined(__ATARI__)
  #define charatrDataZP 0xe0
  #define charPtrZP 0xe2
  #define decPtr1ZP 0xe4
  #define decPtr2ZP 0xe6
  #define scrPtrZP  0xe4
  #define scrPtr1ZP 0xe4
  #define scrPtr2ZP 0xe6
#elif defined(__CBM__)
  #define charatrDataZP 0x61
  #define charPtrZP 0x63
  #define scrPtrZP  0xfb
  #define colPtrZP  0xfd
  #define decPtr1ZP 0xfb
  #define decPtr2ZP 0xfd
  #define scrPtr1ZP 0x61
  #define scrPtr2ZP 0x63
  #define clrPtr1ZP 0xfb
  #define clrPtr2ZP 0xfd
#elif defined(__LYNX__)
  #define charPtrZP 0xb5
  #define decPtr1ZP 0xb7
  #define decPtr2ZP 0xb9
  #define scrPtrZP  0xb7
  #define scrPtr1ZP 0xb7
  #define scrPtr2ZP 0xb9
#elif defined(__NES__)
  #define charPtrZP 0xf0
  #define decPtr1ZP 0xf2
  #define decPtr2ZP 0xf4 
#elif defined(__ORIC__)
  #define charPtrZP 0xb2
  #define decPtr1ZP 0xb4
  #define decPtr2ZP 0xb6 
  #define scrPtrZP  0xb4
  #define scrPtr1ZP 0xb4
  #define scrPtr2ZP 0xb6 
#endif

// Byte size of screen elements
#if defined(__APPLE2__) 
  #define CHAR_WIDTH       1u
  #define CHAR_HEIGHT	   8u
  #define LINE_SIZE	  	  40u
  #define ROW_SIZE		 320u   
#elif defined(__ATARI__) || defined(__CBM__)
  #define CHAR_WIDTH       1u
  #define CHAR_HEIGHT      1u
  #define LINE_SIZE	  	  40u
  #define ROW_SIZE		  40u 
#elif defined(__LYNX__)
  #define CHAR_WIDTH       2u
  #define CHAR_HEIGHT      4u
  #define LINE_SIZE	  	  82u
  #define ROW_SIZE		 328u 
#elif defined(__NES__)
  #define CHAR_WIDTH       1u
  #define CHAR_HEIGHT      1u
  #define LINE_SIZE	  	  32u	
  #define ROW_SIZE		  32u
#elif defined(__ORIC__)
  #define CHAR_WIDTH       1u
  #define CHAR_HEIGHT      8u
  #define LINE_SIZE	  	  40u	
  #define ROW_SIZE		 320u
#endif

// Tile size
#if defined(__TILE_NONE__)
  #define TILE_WIDTH  1u
  #define TILE_HEIGHT 1u
#elif defined(__TILE_2X2__)
  #define TILE_WIDTH  2u
  #define TILE_HEIGHT 2u
#elif defined(__TILE_3X3__)
  #define TILE_WIDTH  3u
  #define TILE_HEIGHT 3u
#elif defined(__TILE_4X4__)
  #define TILE_WIDTH  4u
  #define TILE_HEIGHT 4u
#endif

// Assembly functions (see tiles.s and scroll.s)
void __fastcall__ DecodeTiles2x2(void);
void __fastcall__ Scroll(void);

// Map size and location properties
extern unsigned char charmapWidth, charmapHeight;
extern unsigned char worldWidth, worldHeight, worldX, worldY, worldMaxX, worldMaxY;
extern unsigned char screenCol1, screenCol2, screenWidth;
extern unsigned char screenRow1, screenRow2, screenHeight;
extern unsigned char lineBlock;

// Decoding properties
extern unsigned char blockWidth, lineWidth, decodeWidth, decodeHeight;
extern unsigned char tileX, tileY, tileCols, tileRows;
extern unsigned char scrollCols, scrollRows, scrollDirX, scrollDirY;
  
// Pointers to various data sets
#if defined(__NES__)
  extern unsigned char charmapData[CHARMAPRAM], charflagData[CHARFLAGRAM], decodeData[DECODERAM]; 
#else
  extern unsigned char *charmapData, *charflagData, *decodeData;
#endif
extern unsigned char *charatrData, *charsetData, *colorData, *screenData, *tilesetData;
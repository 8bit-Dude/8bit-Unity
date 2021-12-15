/*
 * Copyright (c) 2018 Anthony Beaucamp.
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
 
#include "unity.h"

#ifdef __APPLE2__
  #pragma code-name("LOWCODE")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif
  
// Define logos
#if defined __APPLE2__
  #if defined __DHR__
	const unsigned char logos[7][5][3] = { { { 0,11, 0}, {11, 0,11}, {11, 0, 0}, {11, 0,12}, { 0,11, 0} },   // C64
										   { { 0, 4, 0}, { 0,12, 0}, { 0,14, 0}, { 6, 6, 6}, {11,11,11} },   // ATR
										   { { 0, 0, 6}, { 0,14, 0}, {12,12,12}, { 9, 9, 9}, { 0, 3, 1} },   // APP
										   { { 0,15,12}, {15,12,15}, {15,12,15}, {12,15, 0}, {12,12,12} },   // ORI
										   { {12, 0,12}, {12,12, 0}, { 0,12, 0}, { 0,12,12}, {12, 0,12} },   // LNX
										   { {12, 0,12}, {12,12,12}, {12,12,12}, {12,12,12}, {12, 0,12} },   // NES
										   { {11,15, 0}, {11,15,11}, {11,11,11}, {11,15,11}, {11,11,11} } }; // FLP
  #else
	const unsigned char logos[7][5][3] = { { { 0, 3, 0}, { 3, 0, 3}, { 3, 0, 0}, { 3, 0, 4}, { 0, 3, 0} },   // C64
										   { { 0, 4, 0}, { 0, 4, 0}, { 0, 2, 0}, { 2, 2, 2}, { 3, 3, 3} },   // ATR
										   { { 0, 0, 2}, { 0, 2, 0}, { 4, 4, 4}, { 0, 1, 1}, { 0, 3, 3} },   // APP
										   { { 0, 5, 4}, { 5, 4, 5}, { 5, 4, 5}, { 4, 5, 0}, { 4, 4, 4} },   // ORI
										   { { 4, 0, 4}, { 4, 4, 0}, { 0, 4, 0}, { 0, 4, 4}, { 4, 0, 4} },   // LNX
										   { { 4, 0, 4}, { 4, 4, 4}, { 4, 4, 4}, { 4, 4, 4}, { 4, 0, 4} },   // NES
										   { { 3, 5, 0}, { 3, 5, 3}, { 3, 3, 3}, { 3, 5, 3}, { 3, 3, 3} } }; // FLP
  #endif
									 
#elif defined __ATARI__															// (1=Red, 2=Blue, 3=Green)
	const unsigned char logos[7][8] = { {0,0,0, 32,136,128,132, 32}, 	// C64: (0,2,0,0) (2,0,2,0) (2,0,0,0) (2,0,1,0) (0,2,0,0)
										{0,0,0, 16, 16, 48,252,168},	// ATR: (0,1,0,0) (0,1,0,0) (0,3,0,0) (3,3,3,0) (2,2,2,0)
										{0,0,0, 12, 48, 84,168, 32},  	// APP: (0,0,3,0) (0,3,0,0) (1,1,1,0) (2,2,2,0) (0,2,0,0)
										{0,0,0, 52,220,220,112, 84},  	// ORI: (0,3,1,0) (3,1,3,0) (3,1,3,0) (1,3,0,0) (1,1,1,0)
										{0,0,0, 76, 80, 16, 52, 76},  	// LNX: (1,0,3,0) (1,1,0,0) (0,1,0,0) (0,3,1,0) (1,0,3,0)
										{0,0,0, 68, 84, 84, 84, 68},  	// NES: (1,0,1,0) (1,1,1,0) (1,1,1,0) (1,1,1,0) (1,0,1,0)
										{0,0,0,188,190,170,190,170} };	// FLP: (2,3,3,0) (2,3,3,2) (2,2,2,2) (2,3,3,2) (2,2,2,2)
								  
#elif defined __CBM__															// (1=Green, 2=Red, 3=Blue) 
	const unsigned char logos[7][8] = { {0,0,0, 48,204,192,200, 48}, 	// C64: (0,3,0,0) (3,0,3,0) (3,0,0,0) (3,0,2,0) (0,3,0,0)
										{0,0,0, 32, 32, 16, 84,252},	// ATR: (0,2,0,0) (0,2,0,0) (0,1,0,0) (1,1,1,0) (3,3,3,0)
										{0,0,0,  4, 16,168,252, 48},	// APP: (0,0,1,0) (0,1,0,0) (2,2,2,0) (3,3,3,0) (0,3,0,0)
										{0,0,0, 24,100,100,144,168},	// ORI: (0,1,2,0) (1,2,1,0) (1,2,1,0) (2,1,0,0) (2,2,2,0)
										{0,0,0,132,160, 32, 24,132},	// LNX: (2,0,1,0) (2,2,0,0) (0,2,0,0) (0,1,2,0) (2,0,1,0)
										{0,0,0,136,168,168,168,136},  	// NES: (2,0,2,0) (2,2,2,0) (2,2,2,0) (2,2,2,0) (2,0,2,0)
										{0,0,0,212,215,255,215,255} };	// FLP: (3,1,1,0) (3,1,1,3) (3,3,3,3) (3,1,1,3) (3,3,3,3)

#elif defined __LYNX__							// (2=Blue, 5=Green, 8=Orange, 9=Yellow, b=Red, d=White) 
	const unsigned char logos[7][6][2] = { {{0xff,0xff},{0xf2,0xff},{0x2f,0x2f},{0x2f,0xff},{0x2f,0xbf},{0xf2,0xff}}, 	// C64
										   {{0xff,0xff},{0xfb,0xff},{0xf8,0xff},{0xf9,0xff},{0x55,0x5f},{0x22,0x2f}}, 	// ATR
										   {{0xff,0xff},{0xff,0x5f},{0xf5,0xff},{0x88,0x8f},{0xbb,0xbf},{0xf2,0xff}}, 	// APP
								    	   {{0xff,0xff},{0xfd,0xbf},{0xdb,0xdf},{0xdb,0xdf},{0xbd,0xff},{0xbb,0xbf}}, 	// ORI
										   {{0xff,0xff},{0xbf,0x9f},{0xbb,0xff},{0xf9,0xbf},{0xf9,0x9f},{0xbf,0x9f}}, 	// LNX
										   {{0xff,0xff},{0xbf,0xbf},{0xbb,0xbf},{0xbb,0xbf},{0xbb,0xbf},{0xbf,0xbf}}, 	// NES
										   {{0xff,0xff},{0x2d,0xdf},{0x2d,0xd2},{0x22,0x22},{0x2d,0xd2},{0x22,0x22}} }; // FLP								  

#elif defined __ORIC__
	const unsigned char logos[7][8] = { {0,0,0,12,18,16,22,12}, 	// C64: (0,0,1,1,0,0) (0,1,0,0,1,0) (0,1,0,0,0,0) (0,1,0,1,1,0) (0,0,1,1,0,0)
										{0,0,0, 4, 4,14,14,21}, 	// ATR: (0,0,0,1,0,0) (0,0,0,1,0,0) (0,0,1,1,1,0) (0,0,1,1,1,0) (0,1,0,1,0,1)
										{0,0,0, 3, 4,31,31,14}, 	// APP: (0,0,0,0,1,1) (0,0,0,1,0,0) (0,1,1,1,1,1) (0,1,1,1,1,1) (0,0,1,1,1,0)
										{0,0,0,13,18,22,10,31}, 	// ORI: (0,0,1,1,0,1) (0,1,0,0,1,0) (0,1,0,1,1,0) (0,0,1,0,1,0) (0,1,1,1,1,1)
										{0,0,0,27,28, 4, 7,27}, 	// LNX: (0,1,1,0,1,1) (0,1,1,1,0,0) (0,0,0,1,0,0) (0,0,0,1,1,1) (0,1,1,0,1,1)
										{0,0,0,18,26,30,22,18}, 	// NES: (0,1,0,0,1,0) (0,1,1,0,1,0) (0,1,1,1,1,0) (0,1,0,1,1,0) (0,1,0,0,1,0)
										{0,0,0,30,19,31,31,31} };	// FLP: (0,1,1,1,1,0) (0,1,0,0,1,1) (0,1,1,1,1,1) (0,1,1,1,1,1) (0,1,1,1,1,1)
#endif

// Print multicolor logos of the various unity platforms
void PrintLogo(unsigned char index)
{
#if defined __APPLE2__
	unsigned int x,y;
	unsigned char i,j,n;
	
	// Compute location of character
	if (txtX&1) { n=4; } else { n=3; }
	x = (txtX*35)/10u; y = (txtY*8);
	
	// Set character over 3/4 pixels out of 7 in a cell
	for (i=0; i<5; ++i) {
		SetHiresPointer(x, y+i+3);
		for (j=0; j<n; j++) {
			if (j<3) {
			  #if defined __DHR__		
				SetColorDHR(logos[index][i][j]);
			  #else
				SetColorSHR(logos[index][i][j]);
			  #endif
			} else {
			  #if defined __DHR__		
				SetColorDHR(BLACK);
			  #else
				SetColorSHR(BLACK);
			  #endif
			}
			hiresPixel++;
		}
	}
	
#elif defined __ATARI__
	unsigned int addr1, addr2;
	unsigned char i;

	// Get memory addresses
	addr1 = BITMAPRAM1+txtY*320+txtX;
  #ifdef __ATARIXL__	
	addr2 = BITMAPRAM2+txtY*320+txtX;		
  #endif
  
	// Set Character data
	for (i=0; i<8; ++i) {
		POKE(addr1, logos[index][i]); addr1 += 40;
	  #ifdef __ATARIXL__	
		POKE(addr2, logos[index][i]); addr2 += 40;
	  #endif
	}
	
#elif defined __CBM__
	unsigned int addr1, addr2, addr3;
	unsigned char i;
	
	// Get memory addresses
	addr1 = BITMAPRAM + txtY*320 + txtX*8;
	addr2 = SCREENRAM + txtY*40 + txtX;
	addr3 = COLORRAM + txtY*40 + txtX;
	
	// Set logo colors
	POKE(addr2, GREEN << 4 | RED);
	POKE(addr3, BLUE);

	// Set Character data
	memcpy(addr1, logos[index], 8);	
	
#elif defined __LYNX__	
	unsigned int addr;
	unsigned char i;
								  
	// Set Character data
	addr = BITMAPRAM+1 + txtY*492 + txtX*2;
	for (i=0; i<6; ++i) {
		memcpy(addr, logos[index][i], 2);
		addr += 82;
	}

#elif defined __NES__
	PrintChr(index);
	
#elif defined __ORIC__
	unsigned int addr;
	unsigned char i;
	
	// Set Character data
	addr = BITMAPRAM+1 + txtY*320 + txtX;
	for (i=0; i<8; ++i) {
		POKE((char*)addr, 64+logos[index][i]); 
		addr += 40;
	}			
#endif
}

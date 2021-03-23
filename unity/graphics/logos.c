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

// Define logos
#if defined __APPLE2__
	unsigned char logos[6][5][3] = { { { 0,11, 0}, {11, 0,11}, {11, 0, 0}, {11, 0,12}, { 0,11, 0} },   // C64
									 { { 0, 7, 0}, { 7, 0, 7}, { 9, 9, 9}, { 4, 0, 4}, {11, 0,11} },   // ATR
									 { { 0, 0, 7}, { 0, 7, 0}, { 9, 9, 9}, { 4, 4, 4}, { 0,11, 0} },   // APP
									 { { 0,15,12}, {15,12,15}, {15,12,15}, {12,15, 0}, {12,12,12} },   // ORI
									 { {12, 0,12}, {12,12, 0}, { 0,12, 0}, { 0,12,12}, {12, 0,12} },   // LNX
									 { {11, 7, 0}, {11, 7,11}, {11,11,11}, {11, 7,11}, {11,11,11} } }; // FLP
									 
#elif defined __ATARI__															// (1=Red, 2=Blue, 3=Green)
	unsigned char logos[6][8] = { {0,0,0, 32,136,128,132, 32}, 		// C64: (0,2,0,0) (2,0,2,0) (2,0,0,0) (2,0,1,0) (0,2,0,0)
								  {0,0,0, 48,204, 84, 68,136},		// ATR: (0,3,0,0) (3,0,3,0) (1,1,1,0) (1,0,1,0) (2,0,2,0)
								  {0,0,0, 12, 48, 84,168, 32},  	// APP: (0,0,3,0) (0,3,0,0) (1,1,1,0) (2,2,2,0) (0,2,0,0)
								  {0,0,0, 52,220,220,112, 84},  	// ORI: (0,3,1,0) (3,1,3,0) (3,1,3,0) (1,3,0,0) (1,1,1,0)
								  {0,0,0, 68, 80, 16, 20, 68},  	// LNX: (1,0,1,0) (1,1,0,0) (0,1,0,0) (0,1,1,0) (1,0,1,0)
								  {0,0,0,188,190,170,190,170} };	// FLP: (2,3,3,0) (2,3,3,2) (2,2,2,2) (2,3,3,2) (2,2,2,2)
								  
#elif defined __CBM__															// (1=Green, 2=Red, 3=Blue) 
	unsigned char logos[6][8] = { {0,0,0, 48,204,192,200, 48}, 		// C64: (0,3,0,0) (3,0,3,0) (3,0,0,0) (3,0,2,0) (0,3,0,0)
								  {0,0,0, 16, 68,168,204,204},		// ATR: (0,1,0,0) (1,0,1,0) (2,2,2,0) (3,0,3,0) (3,0,3,0)
								  {0,0,0,  4, 16,168,252, 48},		// APP: (0,0,1,0) (0,1,0,0) (2,2,2,0) (3,3,3,0) (0,3,0,0)
								  {0,0,0, 24,100,100,144,168},		// ORI: (0,1,2,0) (1,2,1,0) (1,2,1,0) (2,1,0,0) (2,2,2,0)
								  {0,0,0,136,160, 32, 40,136},		// LNX: (2,0,2,0) (2,2,0,0) (0,2,0,0) (0,2,2,0) (2,0,2,0)
								  {0,0,0,212,215,255,215,255} };	// FLP: (3,1,1,0) (3,1,1,3) (3,3,3,3) (3,1,1,3) (3,3,3,3)

#elif defined __LYNX__							// (2=Blue, 5=Green, 8=Orange, b=Red, d=White) 
	unsigned char logos[6][6][2] = { {{0xff,0xff},{0xf2,0xff},{0x2f,0x2f},{0x2f,0xff},{0x2f,0xbf},{0xf2,0xff}}, 	// C64
									 {{0xff,0xff},{0xf5,0xff},{0x5f,0x5f},{0x88,0x8f},{0xbf,0xbf},{0x2f,0x2f}}, 	// ATR
									 {{0xff,0xff},{0xff,0x5f},{0xf5,0xff},{0x88,0x8f},{0xbb,0xbf},{0xf2,0xff}}, 	// APP
								     {{0xff,0xff},{0xfd,0xbf},{0xdb,0xdf},{0xdb,0xdf},{0xbd,0xff},{0xbb,0xbf}}, 	// ORI
									 {{0xff,0xff},{0x8f,0x8f},{0x88,0xff},{0xf8,0xff},{0xf8,0x8f},{0x8f,0x8f}}, 	// LNX
									 {{0xff,0xff},{0x2d,0xdf},{0x2d,0xd2},{0x22,0x22},{0x2d,0xd2},{0x22,0x22}} }; 	// FLP								  

#elif defined __ORIC__
	unsigned char logos[6][8] = { {0,0,0,12,18,16,23,12}, 	// C64: (0,0,1,1,0,0) (0,1,0,0,1,0) (0,1,0,0,0,0) (0,1,0,1,1,1) (0,0,1,1,0,0)
								  {0,0,0,12,18,30,18,18}, 	// ATR: (0,0,1,1,0,0) (0,1,0,0,1,0) (0,1,1,1,1,0) (0,1,0,0,1,0) (0,1,0,0,1,0)
								  {0,0,0, 2,12,30,30,12}, 	// APP: (0,0,0,0,1,0) (0,0,1,1,0,0) (0,1,1,1,1,0) (0,1,1,1,1,0) (0,0,1,1,0,0)
								  {0,0,0,13,18,22,28,30}, 	// ORI: (0,0,1,1,0,1) (0,1,0,0,1,0) (0,1,0,1,1,0) (0,1,1,1,0,0) (0,1,1,1,1,0)
								  {0,0,0,27,28, 4, 7,27}, 	// LNX: (0,1,1,0,1,1) (0,1,1,1,0,0) (0,0,0,1,0,0) (0,0,0,1,1,1) (0,1,1,0,1,1)
								  {0,0,0,30,18,30,30,30} };	// FLP: (0,1,1,1,1,0) (0,1,0,0,1,0) (0,1,1,1,1,0) (0,1,1,1,1,0) (0,1,1,1,1,0)
#endif

// Print multicolor logos of the various unity platforms
void PrintLogo(unsigned char col, unsigned char row, unsigned char index)
{
#if defined __APPLE2__
	unsigned int x,y;
	unsigned char i,j,n;
	
	// Compute location of character
	if (col&1) { n=4; } else { n=3; }
	x = (col*35)/10u; y = (row*8);
	
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
	addr1 = BITMAPRAM1+row*320+col;
	addr2 = BITMAPRAM2+row*320+col;		
	
	// Set Character data
	for (i=0; i<8; ++i) {
		POKE(addr1, logos[index][i]); addr1 += 40;
		POKE(addr2, logos[index][i]); addr2 += 40;
	}
	
#elif defined __CBM__
	unsigned int addr1, addr2, addr3;
	unsigned char i;
	
	// Get memory addresses
	addr1 = BITMAPRAM + row*320 + col*8;
	addr2 = SCREENRAM + row*40 + col;
	addr3 = COLORRAM + row*40 + col;
	
	// Set logo colors
	POKE(addr2, GREEN << 4 | RED);
	POKE(addr3, BLUE);

	// Set Character data
	memcpy(addr1, logos[index], 8);	
	
#elif defined __LYNX__	
	unsigned int addr;
	unsigned char i;
								  
	// Set Character data
	addr = BITMAPRAM+1 + row*492 + col*2;
	for (i=0; i<6; ++i) {
		memcpy(addr, logos[index][i], 2);
		addr += 82;
	}
	
#elif defined __ORIC__
	unsigned int addr;
	unsigned char i;
	
	// Set Character data
	addr = BITMAPRAM+1 + row*320 + col;
	for (i=0; i<8; ++i) {
		POKE((char*)addr, 64+logos[index][i]); 
		addr += 40;
	}			
#endif
}

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

#include "unity.h"

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

// Scroll step size
signed char stepX, stepY;

#if (defined __APPLE2__) || (defined __ORIC__)
  void RestoreSprBG(unsigned char index);
#endif

void ShiftSprites(void) 
{
	unsigned char i;
#if (defined __APPLE2__) || (defined __ORIC__)
	unsigned char nX, nY;
#endif
	for (i=0; i<SPRITE_NUM; i++)
		if (sprDrawn[i]) {
			if (!(sprFlags[i] & SPR_SCROLL)) {
		#if (defined __APPLE2__) || (defined __ORIC__)
				RestoreSprBG(i);
				sprDrawn[i] = 0;
		#endif
			} else {
		#if (defined __APPLE2__) || (defined __ORIC__)
				nY = sprY[i] - stepY*8;
				if (nY < screenRow1*8 || nY >= screenRow2*8) {	
					RestoreSprBG(i);
					sprDrawn[i] = 0;
					continue;
				} else {
					sprY[i] = nY;
				}
			#if (defined __APPLE2__)
				nX = sprHiresX[i] - stepX;
			#else
				nX = sprX[i] - stepX;
			#endif	
				if (nX < screenCol1 || nX >= screenCol2) {
					RestoreSprBG(i);
					sprDrawn[i] = 0;
					continue;
				} else {
			#if (defined __APPLE2__)
					sprHiresX[i] = nX;
					sprX[i] = (sprHiresX[i]*7)/2u;
			#else
					sprX[i] = nX;
			#endif	
				}
		#else
				spriteX = sprX[i] - stepX*4;
			#if (defined __LYNX__)
				spriteY = sprY[i] - stepY*4;
			#else
				spriteY = sprY[i] - stepY*8;
			#endif
				SetSprite(i, sprFrame[i]);
		#endif
		}
	}
}

void ScrollCharmap(unsigned char x, unsigned char y)
{	
#if defined(__NES__) || defined(__ATARI__)
	// Compute step to shift sprites
	stepX = x - worldX; 
	stepY = y - worldY;
	ShiftSprites();
	DrawCharmap(x, y);
#else
	unsigned int src, srcOff, dstOff;
	unsigned int cpyDst = 0, cpySrc = 0;
	unsigned char tmp1;
	
	// Platform specific handling
#if defined(__APPLE2__)
	unsigned char tmp2;
	x = 2*(x/2u)+1;
  #if defined(__DHR__)
	clk += 44;
  #else
	clk += 24;  
  #endif	
#endif

	// Compute step
	stepX = x - worldX; 
	stepY = y - worldY;
	worldX = x; worldY = y;
	
	// Decode tiles (if necessary)
	src = DecodeTiles();
	
	// Init copy parameters
	scrollCols = lineBlock;
	scrollDirX = 1;
	scrollDirY = 1;
	
	// Determine screen area
	if (stepY) {
		if (stepY < 0) {
			cpySrc = cpyDst = screenHeight*ROW_SIZE - LINE_SIZE;
			scrollDirY = -1;
		}
		cpySrc += stepY*ROW_SIZE;
	}
	if (stepX) {
		if (stepX < 0) {
			cpyDst += (-stepX)*CHAR_WIDTH;
			scrollDirX = -1;
		} else {
			cpySrc += stepX*CHAR_WIDTH;
		}
		scrollCols -= ABS(stepX)*CHAR_WIDTH;
	}
	
	// Shift position of soft sprites
	ShiftSprites();
	
	// Scroll screen area
	scrollRows = (screenHeight-ABS(stepY))*CHAR_HEIGHT;
#if defined __APPLE2__	
	POKE(scrRow1ZP, screenRow1*CHAR_HEIGHT+(cpySrc/LINE_SIZE));
	POKE(scrRow2ZP, screenRow1*CHAR_HEIGHT+(cpyDst/LINE_SIZE));
	POKE(scrCol1ZP, screenCol1+(cpySrc%LINE_SIZE));
	POKE(scrCol2ZP, screenCol1+(cpyDst%LINE_SIZE));	
#else
	POKEW(scrPtr1ZP, screenData+cpySrc);
	POKEW(scrPtr2ZP, screenData+cpyDst);
  #if defined __CBM__		
	POKEW(clrPtr1ZP, colorData+cpySrc);
	POKEW(clrPtr2ZP, colorData+cpyDst);
  #endif
#endif  
	Scroll();
	
	// Restore non-scrollable sprites
#if (defined __APPLE2__) || (defined __ORIC__)
	for (tmp1=0; tmp1<SPRITE_NUM; tmp1++)
		if (sprDrawn[tmp1] && !(sprFlags[tmp1] & SPR_SCROLL)) {
			spriteX = sprX[tmp1];
			spriteY = sprY[tmp1];
			SetSprite(tmp1, sprFrame[tmp1]);
		}
#endif
		
		
	// Blit new area on Top/Bottom
	if (stepY) {
		// Assign charmap pointer
		if (stepY > 0) {
			srcOff = (screenHeight-stepY);
			dstOff = srcOff*ROW_SIZE;
			srcOff *= blockWidth;
		} else {
			srcOff = 0;
			dstOff = 0;
		}
		POKEW(charPtrZP, src+srcOff);
		
		// Assign screen information
	#if defined __APPLE2__	
		tmp1 = screenRow1;
		tmp2 = screenRow2;
		screenRow1 += dstOff/ROW_SIZE;
		screenRow2 = screenRow1+ABS(stepY);
	#else
		tmp1 = screenHeight;
		screenHeight = ABS(stepY);
		POKEW(scrPtrZP, (unsigned int)screenData+dstOff);
	  #if defined __CBM__	
		POKEW(charatrDataZP, charatrData);
		POKEW(colPtrZP, (unsigned int)colorData+dstOff);
	  #endif
	#endif
		BlitCharmap();	

		// Restore screen information
	#if defined __APPLE2__	
		screenRow1 = tmp1;
		screenRow2 = tmp2;
	#else
		screenHeight = tmp1;
	#endif
	}
	
	// Blit new area on Left/Right
	if (stepX) {
		// Assign charmap pointer
		if (stepX > 0) {
			srcOff = (screenWidth-stepX);
			dstOff = srcOff*CHAR_WIDTH;
		} else {
			srcOff = 0;
			dstOff = 0;
		}
		POKEW(charPtrZP, src+srcOff);
		
		// Assign screen information		
		tmp1 = screenWidth;
		screenWidth = ABS(stepX);
	#if defined __APPLE2__	
		tmp2 = screenCol1;
		screenCol1 += dstOff%LINE_SIZE;
	#else
		POKEW(scrPtrZP, (unsigned int)screenData+dstOff);
	  #if defined __CBM__	
		POKEW(charatrDataZP, charatrData);
		POKEW(colPtrZP, (unsigned int)colorData+dstOff);
	  #endif
	#endif
		BlitCharmap();			
		
		// Restore screen information
		screenWidth = tmp1;
	#if defined __APPLE2__	
		screenCol1 = tmp2;
	#endif
	}		
#endif	
}

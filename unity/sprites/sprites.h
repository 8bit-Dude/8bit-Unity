/*
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
 */

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

// Platform specific definitions
#if defined __APPLE2__
    #define BYTEWIDTH 2		// 2 bytes = 7 pixels wide (4 bytes between AUX/MAIN in DHR)
	#define FRAMEBLOCK (SPRITEFRAMES*SPRITEHEIGHT*BYTEWIDTH)
	extern unsigned char *sprData, *sprBG[SPRITE_NUM], sprRows[SPRITE_NUM], sprHiresX[SPRITE_NUM];
#elif defined __ATARI__	
	#define BANK_NUM   3	// 3 banks of 4 single color sprites (or 2 multicolor sprites)
	extern unsigned char sprRows, sprPads, sprDLIs, sprBank[BANK_NUM];
	extern unsigned char sprDrawn[SPRITE_NUM], sprX[SPRITE_NUM], sprY[SPRITE_NUM];
	extern unsigned char sprLine[SPRITE_NUM], sprOff[SPRITE_NUM], sprColor[SPRITE_NUM];
	extern unsigned int  sprSrc[SPRITE_NUM], sprDst[SPRITE_NUM];
	extern unsigned char *sprData, sprYOffset;
#elif defined __LYNX__
	#define FRAMESIZE  (SPRITEHEIGHT*((SPRITEWIDTH+(SPRITEWIDTH&1))/2+1)+1)	
	extern unsigned char spriteData; 
	extern SCB_REHV_PAL sprSCB[];
	extern SCB_REHV_PAL *scb;
#elif defined __NES__
	extern unsigned char metaSprites[136];
#elif defined __ORIC__	
	#define BYTEWIDTH 2		// 2 bytes = 12 pixels wide
	#define FRAMEBLOCK (SPRITEFRAMES*SPRITEHEIGHT*BYTEWIDTH)
	extern unsigned char *sprBG[SPRITE_NUM], sprRows[SPRITE_NUM], sprOverlap[SPRITE_NUM];
	extern unsigned int  scrAddr[SPRITE_NUM], sprCOLOR; 
	extern unsigned char *sprMULTICOLOR[SPRITE_NUM];	
	extern unsigned char ink1[20];	// see bitmap.c
#endif

// Sprite status
#if defined(__CBM__)
  extern unsigned char sprCollision;
#else 
  extern unsigned char sprCollision[SPRITE_NUM];
#endif
extern unsigned char sprMask[SPRITE_NUM], sprDrawn[SPRITE_NUM];
extern unsigned char sprX[SPRITE_NUM], sprY[SPRITE_NUM], sprColor[SPRITE_NUM];
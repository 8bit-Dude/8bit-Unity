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
 */

// Keyboard definitions
	#define KB_SP		' '
#if defined(__APPLE2__) || defined(__LYNX__)
	#define KB_A	'A'
	#define KB_B	'B'
	#define KB_C	'C'
	#define KB_D	'D'
	#define KB_E	'E'
	#define KB_F	'F'
	#define KB_G	'G'
	#define KB_H	'H'
	#define KB_I	'I'
	#define KB_J	'J'
	#define KB_K	'K'
	#define KB_L	'L'
	#define KB_M	'M'
	#define KB_N	'N'
	#define KB_O	'O'
	#define KB_P	'P'
	#define KB_Q	'Q'	
	#define KB_R	'R'	
	#define KB_S	'S'	
	#define KB_T	'T'	
	#define KB_U	'U'	
	#define KB_V	'V'	
	#define KB_W	'W'
	#define KB_X	'X'
	#define KB_Y	'Y'
	#define KB_Z	'Z'
#elif defined(__ATARI__) || defined(__CBM__) || defined(__ATMOS__)
	#define KB_A	'a'	
	#define KB_B	'b'	
	#define KB_C	'c'	
	#define KB_D	'd'	
	#define KB_E	'e'
	#define KB_F	'f'
	#define KB_G	'g'
	#define KB_H	'h'
	#define KB_I	'i'
	#define KB_J	'j'
	#define KB_K	'k'
	#define KB_L	'l'
	#define KB_M	'm'
	#define KB_N	'n'
	#define KB_O	'o'
	#define KB_P	'p'
	#define KB_Q	'q'	
	#define KB_R	'r'	
	#define KB_S	's'	
	#define KB_T	't'	
	#define KB_U	'u'	
	#define KB_V	'v'	
	#define KB_W	'w'
	#define KB_X	'x'
	#define KB_Y	'y'
	#define KB_Z	'z'
#endif

// Text data
extern const char charBlank[3];
extern const char charDigit[30];
extern const char charLetter[78];
extern const char charBracket[6];
extern const char charColon[3];
extern const char charComma[3];
extern const char charDot[3];
extern const char charDollar[3];
extern const char charExclaim[3];
extern const char charHyphen[3];
extern const char charPlus[3];
extern const char charQuestion[3];
extern const char charQuote[3];
extern const char charBwSlash[3];
extern const char charFwSlash[3];
extern const char charStar[3];
extern const char charUnderbar[3];

// Widget data
extern const char charArrowDown[3];
extern const char charArrowLeft[3];
extern const char charArrowRight[3];
extern const char charArrowUp[3];
extern const char charLineHorz[3];
extern const char charLineVert[3];
extern const char charSliderHorz[3];
extern const char charSliderVert[3];

// Icon data
extern const char charAmmo[3];
extern const char charBlock[3];
extern const char charDeath[3];
extern const char charHeart[3];
extern const char charPotion[3];
extern const char charShield[3];

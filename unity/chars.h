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
	#define KEY_SP		' '
#if defined(__APPLE2__) || defined(__LYNX__)
	#define KEY_A		'A'
	#define KEY_B		'B'
	#define KEY_C		'C'
	#define KEY_D		'D'
	#define KEY_E		'E'
	#define KEY_F		'F'
	#define KEY_G		'G'
	#define KEY_H		'H'
	#define KEY_I		'I'
	#define KEY_J		'J'
	#define KEY_K		'K'
	#define KEY_L		'L'
	#define KEY_M		'M'
	#define KEY_N		'N'
	#define KEY_O		'O'
	#define KEY_P		'P'
	#define KEY_Q		'Q'	
	#define KEY_R		'R'	
	#define KEY_S		'S'	
	#define KEY_T		'T'	
	#define KEY_U		'U'	
	#define KEY_V		'V'	
	#define KEY_W		'W'
	#define KEY_X		'X'
	#define KEY_Y		'Y'
	#define KEY_Z		'Z'
#elif defined(__CBM__) || defined(__ATMOS__)
	#define KEY_A		'a'	
	#define KEY_B		'b'	
	#define KEY_C		'c'	
	#define KEY_D		'd'	
	#define KEY_E		'e'
	#define KEY_F		'f'
	#define KEY_G		'g'
	#define KEY_H		'h'
	#define KEY_I		'i'
	#define KEY_J		'j'
	#define KEY_K		'k'
	#define KEY_L		'l'
	#define KEY_M		'm'
	#define KEY_N		'n'
	#define KEY_O		'o'
	#define KEY_P		'p'
	#define KEY_Q		'q'	
	#define KEY_R		'r'	
	#define KEY_S		's'	
	#define KEY_T		't'	
	#define KEY_U		'u'	
	#define KEY_V		'v'	
	#define KEY_W		'w'
	#define KEY_X		'x'
	#define KEY_Y		'y'
	#define KEY_Z		'z'
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
extern const char charPlus[3];
extern const char charQuestion[3];
extern const char charQuote[3];
extern const char charBwSlash[3];
extern const char charFwSlash[3];
extern const char charStar[3];
extern const char charUnderbar[3];
extern const char charArrowDown[3];
extern const char charArrowLeft[3];
extern const char charArrowRight[3];
extern const char charArrowUp[3];
extern const char charLineHorz[3];
extern const char charLineVert[3];
extern const char charSliderHorz[3];
extern const char charSliderVert[3];

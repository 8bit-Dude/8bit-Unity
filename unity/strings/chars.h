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
	#define KB_EN	CH_ENTER
	#define KB_SP	' '
#if defined(__APPLE2__) || defined(__LYNX__) || defined(__NES__)
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

// Codes of special Characters
#define CHR_DATA_TOP  	18
#define CHR_ARROW_DOWN	18
#define CHR_ARROW_LEFT	19
#define CHR_ARROW_RIGHT	20
#define CHR_ARROW_UP	21
#define CHR_LINE_HORZ	22
#define CHR_LINE_VERT	23
#define CHR_SLIDER_HORZ	24
#define CHR_SLIDER_VERT	25
#define CHR_HEART		26
#define CHR_POTION  	27
#define CHR_SHIELD  	28
#define CHR_DEATH   	29
#define CHR_AMMO    	30
#define CHR_BLOCK   	31
#define CHR_DIGIT   	48
#define CHR_LETTER   	65
	
// Character data (encoded as 3 bytes per char)
#ifndef __NES__
  extern const char charData[];
#endif

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
 
// Graphical scaling functions

#include "unity.h"

unsigned char ColToX(unsigned char col)
{
#if defined __APPLE2__	// DHR Mode: 140 x 192
	return (col*35u)/10u;
#elif defined __ATARI__	// INP Mode: 160 x 200
	return col*4u;
#elif defined __ORIC__	// AIC Mode: 117 x 100 equivalent pixels
	return col*3u;	
#elif defined __CBM__	// MLC Mode: 160 x 200
	return col*4u;
#elif defined __LYNX__	// STD Mode: 160 x 102
	return col*4u;
#endif
}

unsigned char XToCol(unsigned char x)
{
#if defined __APPLE2__	// DHR Mode: 140 x 192
	return (x*10u)/35u;
#elif defined __ATARI__	// INP Mode: 160 x 200
	return x/4u;
#elif defined __ORIC__	// AIC Mode: 117 x 100 equivalent pixels
	return x/3u;	
#elif defined __CBM__	// MLC Mode: 160 x 200
	return x/4u;
#elif defined __LYNX__	// STD Mode: 160 x 102
	return x/4u;
#endif
}

unsigned char RowToY(unsigned char row)
{
#if defined __APPLE2__	// DHR Mode: 140 x 192
	return row*8u;
#elif defined __ATARI__	// INP Mode: 160 x 200
	return row*8u;
#elif defined __ORIC__	// AIC Mode: 117 x 100 equivalent pixels
	return row*4u;
#elif defined __CBM__	// MLC Mode: 160 x 200
	return row*8u;
#elif defined __LYNX__	// STD Mode: 160 x 102
	return row*6u;
#endif
}

unsigned char YToRow(unsigned char y)
{
#if defined __APPLE2__	// DHR Mode: 140 x 192
	return y/8u;
#elif defined __ATARI__	// INP Mode: 160 x 200
	return y/8u;
#elif defined __ORIC__	// AIC Mode: 117 x 100 equivalent pixels
	return y/4u;
#elif defined __CBM__	// MLC Mode: 160 x 200
	return y/8u;
#elif defined __LYNX__	// STD Mode: 160 x 102
	return y/6u;
#endif
}

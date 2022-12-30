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
 
#include "widgets.h"

callback* ScrollBar(unsigned char col, unsigned char row, unsigned char height, unsigned int *range)
{
	callback* call;
	unsigned char i=0;	

	// Draw scrollbar
	txtX = col; txtY = row;
	PrintChr(CHR_ARROW_UP);
	while (++txtY < (row+height-1)) {
		PrintChr(CHR_LINE_VERT);
	}
	PrintChr(CHR_ARROW_DOWN);
	
	// Register callback
	call = PushCallback(col, row, 1, height, CALLTYPE_SCROLLBAR);
	call->buffer = (unsigned char*)range;

	// Show slider
	txtY = SliderPos(call);
	PrintChr(CHR_SLIDER_VERT);
	return call;
}

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

void ListBox(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title, unsigned char* labels, unsigned char len)
{
	unsigned char i=0, j=0;	
	unsigned char* elt;
	callback* call;
	
	// Draw panel
	Panel(col, row, width, height, title);
	
	// Print list and register callbacks
	txtX = col; txtY = row+1;
 	while (i<(height-1) && i<len) {
		// Add callback
		elt = &labels[j];
		PrintStr(elt);
		call = PushCallback(txtX, txtY, width, 1, CALLTYPE_LISTBOX);
		call->buffer = elt;
		call->value = i;
		
		// Search next string
		while (labels[++j]) if (j==255) return;
		while (!labels[++j]) if (j==255) return;
		txtY++; i++;
	}
}

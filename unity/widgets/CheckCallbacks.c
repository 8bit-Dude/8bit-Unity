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

callback* callHead = NULL;
callback* callList = NULL;

callback* CheckCallbacks(unsigned char col, unsigned char row)
{
	callback *call = callHead;
	unsigned int val, max;
	unsigned char inkBckup, paperBckup;
		
	// Check coordinates overlaps callback?
	while (call) {
		if (call->colBeg<=col && col<call->colEnd && call->rowBeg<=row && row<call->rowEnd) {
			// Backup current ink/paper
			inkBckup = inkColor; paperBckup = paperColor;
	
			// Trigger click action (if any)
			switch (call->type) {
			case CALLTYPE_INPUT:	
				inputCall = call;
				inputWidth = call->colEnd - call->colBeg;
			#if defined(__LYNX__) || defined(__NES__) 
			  #if defined(__LYNX__)
				SetKeyboardOverlay(54,70);
			  #endif
				ShowKeyboardOverlay();
			#endif		
				break;
				
			case CALLTYPE_LISTBOX:
				// Change highlight to new item			
				if (callList) {	
					txtX = callList->colBeg;
					txtY = callList->rowBeg;
			#if defined(__NES__)
					txtX--; PrintChr(CHR_LINE_VERT);
			#else				
					inkColor = callList->ink;
					paperColor = callList->paper;
					PrintStr(callList->buffer);
			#endif
				}
				txtX = call->colBeg;
				txtY = call->rowBeg;
			#if defined(__NES__)
				txtX--; PrintChr(CHR_SLIDER_VERT);
			#else				
				inkColor = call->paper;
				paperColor = call->ink;
				PrintStr(call->buffer);
			#endif
				callList = call;
				break;

			case CALLTYPE_SCROLLBAR:
				// Update position of slider
				inkColor = call->ink;
				paperColor = call->paper;
				txtX = call->colBeg;
				txtY = SliderPos(call);
				PrintChr(CHR_LINE_VERT);
				val = *(unsigned int*)&call->buffer[0];
				max = *(unsigned int*)&call->buffer[2];
				if (row == call->rowBeg) {
					if (val > 0) 
						(val)--;
				} else 
				if (row == call->rowEnd-1) {
					if (val < max-1)
						(val)++;
				} else 
				if (row == call->rowBeg+1) {
					val = 0;
				} else 
				if (row == call->rowEnd-2) {
					val = max-1;
				} else {
					val  = max;
					val *= row - call->rowBeg - 1;
					val /= call->rowEnd - call->rowBeg - 2;
				}
				*(unsigned int*)&call->buffer[0] = val;
				txtY = SliderPos(call);
				PrintChr(CHR_SLIDER_VERT);
				break;
				
			}
			
			// Restore current ink/paper
			inkColor = inkBckup; paperColor = paperBckup;			
			return call;
		}
		call = (callback*)call->next;
	}
	return 0;
}

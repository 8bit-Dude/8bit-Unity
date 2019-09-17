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
 
#include "../unity.h"

// See bitmap.c and sprites.c
extern SCB_REHV_PAL bitmapTGI;
extern LynxSprite spriteSlot[SPRITE_NUM];
extern unsigned char spriteCols[SPRITE_NUM];

void UpdateDisplay(void)
{
	unsigned char i, c;

	// Wait for previous drawing to complete then reset collisions
	while (tgi_busy()) {}
	tgi_clear();
	for (i=0; i<SPRITE_NUM; i++) { 
		spriteCols[i] = 0; 
	}	
	
	// Send bitmap and sprites to Suzy
	tgi_sprite(&bitmapTGI);
	for (i=0; i<SPRITE_NUM; i++) {
		if (spriteSlot[i].scb.data) { 
			tgi_sprite(&spriteSlot[i].scb);
			c = spriteSlot[i].collisions;
			if (c) {
				c -= 1;
				spriteCols[i] |= 1 << c;
				spriteCols[c] |= 1 << i;
			}
		}
	}
	tgi_updatedisplay();
}

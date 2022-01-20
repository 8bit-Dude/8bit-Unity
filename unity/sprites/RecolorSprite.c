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

#include "unity.h"
#include "Sprites.h"

void RecolorSprite(unsigned char index, unsigned char offset, unsigned char color)
{
#if defined __ATARI__
	sprColor[index] = color;
#elif defined __CBM__
	POKE(53287+index, color);
#elif defined __LYNX__
	sprSCB[index].penpal[offset] = color;
#elif defined __NES__
	unsigned char i, base = index*17+3;
	for (i=0; i<4; i++) {
		metaSprites[base] = color;
		base += 4;
	}
#elif defined __ORIC__
	POKE(sprCOLOR+index, color);
#endif
}

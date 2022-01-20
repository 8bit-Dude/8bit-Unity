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

#include "unity.h"
#include "charmap.h"

void SetTile(unsigned char x, unsigned char y, unsigned char tile)
{
#if defined __TILE_NONE__
	charmapData[charmapWidth*y + x] = tile;
#else	
  #if defined __TILE_2X2__
	charmapData[charmapWidth*(y/2u) + x/2u] = tile;
  #elif defined __TILE_3X3__
	charmapData[charmapWidth*(y/3u) + x/3u] = tile;
  #elif defined __TILE_4X4__
	charmapData[charmapWidth*(y/4u) + x/4u] = tile;
  #endif
	tileX = 255; tileY = 255;	// Trick to force re-decoding
#endif
}

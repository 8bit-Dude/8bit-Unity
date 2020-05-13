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
 
#define BASE_DECIMALS 100 

unsigned char IntersectSegments(signed int s1x1, signed int s1y1, signed int s1x2, signed int s1y2,
							    signed int s2x1, signed int s2y1, signed int s2x2, signed int s2y2,
								signed int *ix,  signed int *iy)
{
    signed long deltas1x, deltas1y, deltas2x, deltas2y;
	signed long denom, numer1, numer2, ratio1, ratio2;

	deltas1x = s1x2 - s1x1; 
	deltas1y = s1y2 - s1y1; 
	deltas2x = s2x2 - s2x1; 
	deltas2y = s2y2 - s2y1; 
	
	denom = (deltas1x * deltas2y) - (deltas1y * deltas2x);

    // Segments are parallel 
    if (denom == 0) return 0;

    numer1 = ((s1y1 - s2y1) * deltas2x) - ((s1x1 - s2x1) * deltas2y);
    numer2 = ((s1y1 - s2y1) * deltas1x) - ((s1x1 - s2x1) * deltas1y);

    ratio1 = (BASE_DECIMALS*numer1) / denom;
	if (ratio1 < 0 || ratio1 > BASE_DECIMALS) return 0;
	
    ratio2 = (BASE_DECIMALS*numer2) / denom;
    if (ratio2 < 0 || ratio2 > BASE_DECIMALS) return 0;

    // Find intersection point
    *ix = s1x1 + (ratio1 * deltas1x)/BASE_DECIMALS;
    *iy = s1y1 + (ratio1 * deltas1y)/BASE_DECIMALS;
    return 1;
}

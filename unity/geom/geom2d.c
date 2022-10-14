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
 
#define BASE_DECIMALS 100

unsigned char IntersectSegments(signed int seg1X1, signed int seg1Y1, signed int seg1X2, signed int seg1Y2,
							    signed int seg2X1, signed int seg2Y1, signed int seg2X2, signed int seg2Y2,
								signed int *intX,  signed int *intY)
{
    signed long delta1X, delta1Y, delta2X, delta2Y;
	signed long denom, numer1, numer2, ratio1, ratio2;

	delta1X = seg1X2 - seg1X1; 
	delta1Y = seg1Y2 - seg1Y1; 
	delta2X = seg2X2 - seg2X1; 
	delta2Y = seg2Y2 - seg2Y1; 
	
	denom = (delta1X * delta2Y) - (delta1Y * delta2X);

    // Segments are parallel 
    if (denom == 0) return 0;

    numer1 = ((seg1Y1 - seg2Y1) * delta2X) - ((seg1X1 - seg2X1) * delta2Y);
    numer2 = ((seg1Y1 - seg2Y1) * delta1X) - ((seg1X1 - seg2X1) * delta1Y);

    ratio1 = (BASE_DECIMALS*numer1) / denom;
	if (ratio1 < 0 || ratio1 > BASE_DECIMALS) return 0;
	
    ratio2 = (BASE_DECIMALS*numer2) / denom;
    if (ratio2 < 0 || ratio2 > BASE_DECIMALS) return 0;

    // Find intersection point
    *intX = seg1X1 + (ratio1 * delta1X)/BASE_DECIMALS;
    *intY = seg1Y1 + (ratio1 * delta1Y)/BASE_DECIMALS;
    return 1;
}

// Find intersection points with polygon
unsigned char IntersectSegmentPolygon(signed int segX1, signed int segY1, signed int segX2, signed int segY2, 
									  unsigned char polyN, signed int *polyX, signed int *polyY, 
									  unsigned char *intI, signed int *intX, signed int *intY)
{
	unsigned char i, count = 0;
	signed int tmpX, tmpY; 
	for (i=1; i<polyN; i++) {
		if (IntersectSegments(segX1, segY1, segX2, segY2, polyX[i-1], polyY[i-1], polyX[i], polyY[i], &tmpX, &tmpY)) {
			intI[count] = i;
			intX[count] = tmpX;
			intY[count] = tmpY;
			count += 1;
		}
	}
	return count;
}

unsigned char PointInsidePolygon(signed int pX, signed int pY, unsigned char polyN, signed int *polyX, signed int *polyY)
{
	unsigned char i, j, inside = 0;
	signed long casting;
	for (i = 0, j = polyN-1; i < polyN; j = i++) {
		if ((polyY[i]>pY) != (polyY[j]>pY)) {
			casting  = polyX[j]-polyX[i];
			casting *= pY-polyY[i];
			casting /= polyY[j]-polyY[i];
			if (pX < polyX[i]+casting) inside = !inside;
		}
	}
	return inside;
}

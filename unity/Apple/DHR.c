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
 *
 * Adapted from: Bill Buckels
 */
 
//	Helper functions for Apple II Double Hi-Res Mode
//	Last modified: 2019/05/21

#ifdef __APPLE2__
  #pragma code-name("LOWCODE")
#endif

// Addresses for AUX bank copying
unsigned char *dhrmain = (unsigned char*)0xC054;
unsigned char *dhraux = (unsigned char*)0xC055;	
unsigned char *dhrptr, dhrpixel;
unsigned int DHRLine(unsigned char y);	// (see Apple/sprite.c)

void SetDHRPointer(unsigned int x, unsigned int y)
{
	// Compute 7 pixels block address
	unsigned int xBlock;
	dhrpixel = (x%7);
	xBlock = (x/7)*2;
	if (dhrpixel > 3) { ++xBlock; }
	dhrptr = DHRLine(y) + xBlock;
}

void SetDHRColor(unsigned char color)
{
	// Use bitmasks to assign the relevant pixel
	switch(dhrpixel) {
		case 0: *dhraux = 0; // select auxilliary memory 
				*dhrptr &= 0x70;
				*dhrptr |= color;
				*dhrmain = 0; // reset to main memory 
				break;
		case 1: *dhraux = 0; // select auxilliary memory 
				*dhrptr &= 0x0f;
				*dhrptr |= (color &  7) << 4;
				*dhrmain = 0; // reset to main memory 
				*dhrptr &= 0x7e;
				*dhrptr |= (color &  8) >> 3;
				break;
		case 2: *dhrptr &= 0x61;
				*dhrptr |= color << 1;
				break;
		case 3: *dhrptr &= 0x1f;
				*dhrptr |= (color &  3) << 5;
				*dhraux = 0; // select auxilliary memory 
				*dhrptr++;   // advance offset in frame 
				*dhrptr &= 0x7c;
				*dhrptr |= (color & 12) >> 2;
				*dhrmain = 0; // reset to main memory 
				break;
		case 4: *dhraux = 0; // select auxilliary memory 
				*dhrptr &= 0x43;
				*dhrptr |= color << 2;
				*dhrmain = 0; // reset to main memory 
				break;
		case 5: *dhraux = 0; // select auxilliary memory 
				*dhrptr &= 0x3f;
				*dhrptr |= (color &  1) << 6;
				*dhrmain = 0; // reset to main memory 
				*dhrptr &= 0x78;
				*dhrptr |= (color & 14) >> 1;
				break;
		case 6: *dhrptr &= 0x07;
				*dhrptr |= color << 3;
				break;
	}
}

unsigned char GetDHRColor()
{
	unsigned char color;

	// Use bitmasks to retrieve the relevant pixel
	switch(dhrpixel) {
		case 0: *dhraux = 0; // select auxilliary memory 
				color = *dhrptr & 15;
				*dhrmain = 0; // reset to main memory 
				break;
		case 1: *dhraux = 0; // select auxilliary memory 
				color = (*dhrptr & 112) >> 4;
				*dhrmain = 0; // reset to main memory 
				color |= (*dhrptr & 1) << 3;
				break;
		case 2: color = (*dhrptr & 30) >> 1;
				break;
		case 3: color = (*dhrptr & 96) >> 5;
				*dhraux = 0; // select auxilliary memory 
				*dhrptr++;      // advance off in frame 
				color |= (*dhrptr & 3) << 2;
				*dhrmain = 0; // reset to main memory 
				break;
		case 4: *dhraux = 0; // select auxilliary memory 
				color = (*dhrptr & 60) >> 2;
				*dhrmain = 0; // reset to main memory 
				break;
		case 5: *dhraux = 0; // select auxilliary memory 
				color = (*dhrptr & 64) >> 6;
				*dhrmain = 0; // reset to main memory 
				color |= (*dhrptr & 7) << 1;
				break;
		case 6: color = (*dhrptr & 120) >> 3;
				break;
	}

	return color;
}

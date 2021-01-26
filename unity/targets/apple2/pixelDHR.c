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

extern unsigned char *hiresPtr, hiresPixel;

unsigned char *dhrmain = (unsigned char*)0xC054;
unsigned char *dhraux = (unsigned char*)0xC055;	

void SetColorDHR(unsigned char color)
{
	// Use bitmasks to assign the relevant pixel
	switch(hiresPixel) {
		case 0: *dhraux = 0; // select auxilliary memory 
				*hiresPtr &= 0x70;
				*hiresPtr |= color;
				*dhrmain = 0; // reset to main memory 
				break;
		case 1: *dhraux = 0; // select auxilliary memory 
				*hiresPtr &= 0x0f;
				*hiresPtr |= (color &  7) << 4;
				*dhrmain = 0; // reset to main memory 
				*hiresPtr &= 0x7e;
				*hiresPtr |= (color &  8) >> 3;
				break;
		case 2: *hiresPtr &= 0x61;
				*hiresPtr |= color << 1;
				break;
		case 3: *hiresPtr &= 0x1f;
				*hiresPtr |= (color &  3) << 5;
				*dhraux = 0; // select auxilliary memory 
				*hiresPtr++;   // next byte
				*hiresPtr &= 0x7c;
				*hiresPtr |= (color & 12) >> 2;
				*dhrmain = 0; // reset to main memory 
				break;
		case 4: *dhraux = 0; // select auxilliary memory 
				*hiresPtr &= 0x43;
				*hiresPtr |= color << 2;
				*dhrmain = 0; // reset to main memory 
				break;
		case 5: *dhraux = 0; // select auxilliary memory 
				*hiresPtr &= 0x3f;
				*hiresPtr |= (color &  1) << 6;
				*dhrmain = 0; // reset to main memory 
				*hiresPtr &= 0x78;
				*hiresPtr |= (color & 14) >> 1;
				break;
		case 6: *hiresPtr &= 0x07;
				*hiresPtr |= color << 3;
				break;
	}
}

unsigned char GetColorDHR()
{
	unsigned char color;

	// Use bitmasks to retrieve the relevant pixel
	switch (hiresPixel) {
		case 0: *dhraux = 0; // select auxilliary memory 
				color = *hiresPtr & 15;
				*dhrmain = 0; // reset to main memory 
				break;
		case 1: *dhraux = 0; // select auxilliary memory 
				color = (*hiresPtr & 112) >> 4;
				*dhrmain = 0; // reset to main memory 
				color |= (*hiresPtr & 1) << 3;
				break;
		case 2: color = (*hiresPtr & 30) >> 1;
				break;
		case 3: color = (*hiresPtr & 96) >> 5;
				*dhraux = 0; // select auxilliary memory 
				*hiresPtr++;      // next byte
				color |= (*hiresPtr & 3) << 2;
				*dhrmain = 0; // reset to main memory 
				break;
		case 4: *dhraux = 0; // select auxilliary memory 
				color = (*hiresPtr & 60) >> 2;
				*dhrmain = 0; // reset to main memory 
				break;
		case 5: *dhraux = 0; // select auxilliary memory 
				color = (*hiresPtr & 64) >> 6;
				*dhrmain = 0; // reset to main memory 
				color |= (*hiresPtr & 7) << 1;
				break;
		case 6: color = (*hiresPtr & 120) >> 3;
				break;
	}

	return color;
}

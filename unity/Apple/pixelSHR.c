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

void SetColorSHR(unsigned char color)
{
	// Use color groups
	switch (color) {
		case 1:
		case 2:
			*hiresPtr &= ~0x80;	
			break;
		case 3:
		case 4:
			*hiresPtr |= 0x80;
			break;
	}
	if (color > 2) color -= 2;		

	// Use bitmasks to assign the relevant pixel
	switch (hiresPixel) {
		case 0: *hiresPtr &= ~0x03;
				*hiresPtr |= color;
				break;
		case 1: *hiresPtr &= ~0x0c;
				*hiresPtr |= color << 2;
				break;
		case 2: *hiresPtr &= ~0x30;
				*hiresPtr |= color << 4;
				break;
		case 3: *hiresPtr &= ~0x40;
				*hiresPtr |= (color & 0x01) << 6;
				*hiresPtr++;   // next byte
				*hiresPtr &= ~0x01;
				*hiresPtr |= (color & 0x02) >> 1;
				break;
		case 4: *hiresPtr &= ~0x06;
				*hiresPtr |= color << 1;
				break;
		case 5: *hiresPtr &= ~0x18;
				*hiresPtr |= color << 3;
				break;
		case 6: *hiresPtr &= ~0x60;
				*hiresPtr |= color << 5;
				break;
	}
}

unsigned char GetColorSHR()
{
	unsigned char color;

	// Use bitmasks to retrieve the relevant pixel
	switch (hiresPixel) {
		case 0: color = (*hiresPtr & 0x03);
				break;
		case 1: color = (*hiresPtr & 0x0c) >> 2;
				break;
		case 2: color = (*hiresPtr & 0x30) >> 4;
				break;
		case 3: color = (*hiresPtr & 0x40) >> 6;
				*hiresPtr++;      // next byte
				color |= (*hiresPtr & 0x01) << 1;
				break;
		case 4: color = (*hiresPtr & 0x06) >> 1;
				break;
		case 5: color = (*hiresPtr & 0x18) >> 3;
				break;
		case 6: color = (*hiresPtr & 0x60) >> 5;
				break;
	}
	
	// Use color groups
	if (color == 3)
		color = 5;
	else if (*hiresPtr & 0x80)
		color += 2;
	return color;
}

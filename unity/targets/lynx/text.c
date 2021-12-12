/*
 * Copyright (c) 2021 Anthony Beaucamp.
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

#include "../../unity.h"
#include <stdarg.h>

// Workaround for missing text printing

void screensize(unsigned char *xSize, unsigned char *ySize) 
{
	*xSize = 40; *ySize = 17;
}

void gotoxy(unsigned char col, unsigned char row) 
{
	txtX = col; txtY = row;
}

int cprintf(const char* format, ...) 
{
	va_list ap;
	unsigned char value;
	unsigned char buffer[40];
	
	va_start(ap, format);
	value = va_arg(ap, unsigned char);
	sprintf(buffer, format, value);
	va_end(ap);
	
	PrintStr(buffer);
	txtX += strlen(buffer);
	while (txtX > 39) {
		txtX -= 40;
		txtY++;
	}
}

int scanf(const char *format, ...)
{
	unsigned char buffer[16];
	va_list vl;
	
	// Reset buffer
	buffer[0] = 0;

	// Run input loop
	ShowKeyboardOverlay();
	while (1) {
		if (KeyboardOverlayHit() && InputStr(16, buffer, 16, GetKeyboardOverlay()))
			break;
		UpdateDisplay();
	}
	HideKeyboardOverlay();
	
	// Decode arguments
	va_start(vl, format);	
	sscanf(buffer, format, vl);
	va_end(vl);
	return 1; 
}

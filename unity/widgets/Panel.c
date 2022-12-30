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

#if defined __NES__
void Border(unsigned char x1, unsigned char x2, unsigned char y1, unsigned char y2) 
{
	txtY = y1;
	while (txtY < y2) {
		txtX = x2; PrintChr(CHR_LINE_VERT);
		txtX = x1; PrintChr(CHR_LINE_VERT);
		txtY++;
	}
	PrintChr(CHR_CORNER_BL); 
	txtX++;
	while (txtX < x2) {
		PrintChr(CHR_LINE_HORZ);
		txtX++;
	}
	PrintChr(CHR_CORNER_BR); 	
}	
#endif

void Line(unsigned char x1, unsigned char x2, unsigned char y1, unsigned char y2) 
{
	if (x1 == x2) {
		pixelX = x1;
		for (pixelY=y1; pixelY<y2; pixelY++) {
		  #if defined __NES__	
			txtX = pixelX; txtY = pixelY;
			PrintChr(CHR_LINE_VERT);
		  #else
			SetPixel(inkColor); 
		  #endif
		}
	} else {
		pixelY = y1;
		for (pixelX=x1; pixelX<x2; pixelX++) {
		  #if defined __NES__			
			txtX = pixelX; txtY = pixelY;
			PrintChr(CHR_LINE_HORZ);
		  #else
			SetPixel(inkColor); 
		  #endif
		}
	}
}

void Panel(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title)
{
	unsigned char xC, xCW, yR, yRH;
	unsigned char ink, paper;
		
	// Clear area
	txtX = col; txtY = row;
	PrintBlanks(width, height);
	
#if defined __NES__
	// Create border
	Border(col-1, col+width, row, row+height);
	txtX = col; txtY = row;
#else	
	// Create border
	xC = ColToX(col); xCW = ColToX(col+width);
	yR = RowToY(row); yRH = RowToY(row+height); 
	Line(xC, xCW, yRH+1, yRH+1);
	Line(xC-1, xC-1, yR, yRH+2);
	Line(xCW, xCW, yR, yRH+2);
#endif
	
	// Add Title
	ink = inkColor; paper = paperColor;
	inkColor = paper; paperColor = ink;
	PrintBlanks(width, 1);
	PrintStr(title);	
	inkColor = ink; paperColor = paper;
}

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

#pragma bss-name(push, "XRAM")
  // extra RAM at $6000-$7fff
  unsigned char bankLevel;
  unsigned char bankBuffer[MAX_BANK_DEPTH];
  unsigned char vram_list[256];
  unsigned char vram_list_index = 0;
  unsigned char vram_list_length;
  unsigned char vram_attr[64];
  unsigned char vram_attr_index = 0;
  unsigned char vram_attr_color;
  unsigned char vram_attr_horz;
  unsigned char vram_attr_vert;
  unsigned char autoRefresh = 1;
#pragma bss-name(pop)

#pragma rodata-name("BANK0")
#pragma code-name("BANK0")

void __fastcall__ SetVramAddr(unsigned int vaddr)
{
	vram_list[vram_list_index++] = MSB(vaddr)|NT_UPD_HORZ;
	vram_list[vram_list_index++] = LSB(vaddr);
	vram_list_length = vram_list_index++;
	vram_list[vram_list_length] = 0;
}

void __fastcall__ SetVramName(void) 
{
	// Set VRAM address in Name Table
	SetVramAddr(NTADR_A(txtX,(txtY+2)));
}

void __fastcall__ SetVramAttr(void) 
{
	// Set VRAM address in Attribute Table
	SetVramAddr(get_at_addr(0,txtX*8,(txtY+2)*8));
	
	// Prepare attribute variables
	vram_attr_color = (inkColor << 6) | (inkColor << 4) | (inkColor << 2) | inkColor;
	vram_attr_index = 8*((txtY+2)/4u) + txtX/4u;
	vram_attr_vert = ((txtY+2)%4);
	vram_attr_horz = (txtX%4); 
}

void CheckVramQueue(void)
{
	// Check if there is still space...
	if (vram_list_index == 255) {
		// Flush current queue
		UpdateDisplay();
		
		// Restart new queue with offset location
		vram_list[vram_list_index++] = vram_list[vram_list_length-2];
		vram_list[vram_list_index++] = vram_list[vram_list_length-1]+vram_list[vram_list_length];
		vram_list_length = vram_list_index++;
		vram_list[vram_list_length] = 0;
	}	
}

void __fastcall__ SetVramChar(unsigned char chr)
{		
	// Push to VRAM list
	CheckVramQueue();
	vram_list[vram_list_index++] = chr;
	vram_list[vram_list_length] += 1;
}

void __fastcall__ SetVramColor(unsigned char forcePush) 
{
	// Compute attribute cell mask
	unsigned char mask = 0b11111111;
	if (vram_attr_vert<2) mask &= 0b00001111; else mask &= 0b11110000;
	if (vram_attr_horz<2) mask &= 0b00110011; else mask &= 0b11001100;	
	
	// Update attribute data (shadow)
	vram_attr[vram_attr_index] = (vram_attr[vram_attr_index] & ~mask) | (vram_attr_color & mask);
	
	// Push to VRAM list (when reaching edge of cell)
	if (vram_attr_horz==3 || forcePush) {
		CheckVramQueue();
		vram_list[vram_list_index++] = vram_attr[vram_attr_index++];
		vram_list[vram_list_length] += 1;
		vram_attr_horz = 0;
	} else {
		vram_attr_horz++;
	}
}

void __fastcall__ UpdateDisplay(void)
{
	// Flush VRAM queue
	if (vram_list_index) {
		vram_list[vram_list_index] = NT_UPD_EOF;
		set_vram_update(vram_list);
		ppu_wait_nmi();
		vram_list_index = 0;
	}
}

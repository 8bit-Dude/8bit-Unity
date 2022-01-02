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

#define VRAM_BUFFER_LEN 103  // Max. length that NMI can handle within VBLANK

#pragma bss-name(push, "XRAM")
  unsigned int  vram_addr;
  unsigned char vram_list[VRAM_BUFFER_LEN];		
  unsigned char vram_list_index = 0;
  unsigned char vram_list_length;
  unsigned char vram_attr[64];
  unsigned char vram_attr_index = 0;
  unsigned char vram_attr_color;
  unsigned char vram_attr_horz;
  unsigned char vram_attr_vert;
  unsigned char autoRefresh = 1;
  unsigned char palBG[16]={ 0x0f, 0x00, 0x10, 0x30, 	// Grays
							0x0f, 0x01, 0x11, 0x21, 	// Blues
							0x0f, 0x06, 0x16, 0x26, 	// Reds
							0x0f, 0x09, 0x19, 0x29 };  	// Greens
#pragma bss-name(pop)

#pragma rodata-name("BANK0")
#pragma code-name("BANK0")

void UpdateKeyboardOverlay(void);

void __fastcall__ UpdateDisplay(void)
{
	// Update virtual keyboard (if needed)
	UpdateKeyboardOverlay();
	
	// Flush VRAM queue
	if (vram_list_index) {
		vram_list[vram_list_index] = NT_UPD_EOF;
		set_vram_update(vram_list);
		ppu_wait_nmi();
		vram_list_index = 0;
	}
}

void __fastcall__ SetVramAddr(void)
{
	// Flush current queue?
	if (vram_list_index > VRAM_BUFFER_LEN-4)
		UpdateDisplay();
	
	// Setup next write command
	vram_list[vram_list_index++] = MSB(vram_addr)|NT_UPD_HORZ;
	vram_list[vram_list_index++] = LSB(vram_addr);
	vram_list_length = vram_list_index++;
	vram_list[vram_list_length] = 0;
}

void __fastcall__ SetVramName(void) 
{	
	// Set VRAM address in Name Table
	vram_addr = NTADR_A(txtX,(txtY+3));
	SetVramAddr();
}

void __fastcall__ SetVramAttr(void) 
{
	unsigned char y = txtY+3;
	
	// Set VRAM address in Attribute Table
	vram_addr = get_at_addr(0,txtX*8,y*8);
	SetVramAddr();
	
	// Prepare attribute variables
	vram_attr_color = (inkColor << 6) | (inkColor << 4) | (inkColor << 2) | inkColor;
	vram_attr_index = 8*(y/4u) + txtX/4u;
	vram_attr_vert = (y%4);
	vram_attr_horz = (txtX%4); 
}

void CheckVramBuffer(void)
{
	// Is VRAM buffer full?
	if (vram_list_index == VRAM_BUFFER_LEN-1) {
		// Start new queue at offset location (this will flush previous contents)
		vram_addr += vram_list[vram_list_length];
		SetVramAddr();
	}	
}

void __fastcall__ SetVramChar(unsigned char chr)
{		
	// Write to VRAM buffer
	CheckVramBuffer();
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
	
	// Push to VRAM buffer when reaching edge of cell
	if (vram_attr_horz==3 || forcePush) {
		SetVramChar(vram_attr[vram_attr_index++]);
		vram_attr_horz = 0;
	} else {
		vram_attr_horz++;
	}
}

void __fastcall__ FillVram(unsigned char* data)
{
	ppu_off();
	bzero(vram_attr, 64);	// Reset color attributes (shadow)
	vram_adr(NAMETABLE_A);	// Go to top of nametable
	if (!data) {
		vram_fill(0, 0x400); // Fill with 0s
	} else {
		vram_unrle(data);	 // Decompress name-table
	}
	ppu_on_all();	
}
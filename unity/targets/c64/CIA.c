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
 
#include "unity.h"
 
unsigned char byte;
unsigned char cnt;

void OutputMode()
{
	POKE(56579, 0b11110011);	// Switch pins 4-7 to output
	POKE(56577, 0b00000001); 	// STROBE ON	
	cnt--; cnt--; cnt--; cnt--; 
	cnt--; cnt--; cnt--; cnt--;
}

void InputMode()
{
	POKE(56579, 0b00000011);	// Switch pins 4-7 to input
	POKE(56577, 0b00000011); 	// STROBE ON	
	cnt--; cnt--; cnt--; cnt--; 
	cnt--; cnt--; cnt--; cnt--;
}
 
unsigned char SendByte()
{
	// Send 1 byte as 2 x 4bits
	cnt = 255;
	while (cnt) {  // Countdown i to 0
		if (PEEK(56577) & 0b00000100) {	// Ready?
			POKE(56577, (byte&0b00001111)<<4);  // STROBE OFF & Write Bits 1-4
			cnt--; cnt--; cnt--; cnt--; 		// Wait some cycles...
			cnt--; cnt--; cnt--; cnt--; 		// Wait some cycles...
			POKE(56577,    	  0b00000001);  	// STROBE ON
			cnt--; cnt--; cnt--; cnt--;         // Wait some cycles...
			cnt--; cnt--; cnt--; cnt--; 		// Wait some cycles...
			
			POKE(56577, (byte&0b11110000));     // STROBE OFF & Write Bits 5-8
			cnt--; cnt--; cnt--; cnt--; 		// Wait some cycles...
			cnt--; cnt--; cnt--; cnt--; 		// Wait some cycles...
			POKE(56577,    	  0b00000001);  	// STROBE ON		
			cnt--; cnt--; cnt--; cnt--; 		// Wait some cycles...
			cnt--; cnt--; cnt--; cnt--; 		// Wait some cycles...
			return 1;
		}
		cnt--;
	}
	return 0;
}

unsigned char RecvByte()
{
	// Receive 1 byte as 2 x 4bits
	cnt = 255;
	while (cnt) {  // Countdown i to 0
		if (PEEK(56577) & 0b00000100) {	// Ready?
			byte = 0;
			POKE(56577, 0b00000010);  				// STROBE OFF
			cnt--; cnt--; cnt--; cnt--; 			// Wait some cycles...
			cnt--; cnt--; cnt--; cnt--; 			// Wait some cycles...
			byte |= ((PEEK(56577)&0b11110000)>>4);  // Read Bits 1-4
			POKE(56577, 0b00000011);  				// STROBE ON
			cnt--; cnt--; cnt--; cnt--; 			// Wait some cycles...
			cnt--; cnt--; cnt--; cnt--; 			// Wait some cycles...
						
			POKE(56577, 0b00000010);  				// STROBE OFF
			cnt--; cnt--; cnt--; cnt--; 			// Wait some cycles...
			cnt--; cnt--; cnt--; cnt--; 			// Wait some cycles...
			byte |= ((PEEK(56577)&0b11110000));	    // Read Bits 5-8
			POKE(56577, 0b00000011);  				// STROBE ON
			cnt--; cnt--; cnt--; cnt--; 			// Wait some cycles...
			cnt--; cnt--; cnt--; cnt--; 			// Wait some cycles...
			return 1;
		}
		cnt--;
	}
	return 0;
}

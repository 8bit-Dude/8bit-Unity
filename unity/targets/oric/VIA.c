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
  
void SendByte()
{
	// Send 1 byte to HUB
	POKE(0x0301, byte);			// Write to Printer Port
	*((char*)0x300) &= 0xEF; 	// 11101111 - Send STROBE (falling signal)
	cnt++; cnt++; cnt++; 		// Wait some cycles...
	*((char*)0x300) |= 0x10; 	// 00010000 - Reset STROBE
}

unsigned char RecvByte()
{
	// Recv 1 byte from HUB
	cnt = 255;
	cnt--; cnt--; cnt--; cnt--; cnt--; cnt--; cnt--; cnt--; 
	cnt--; cnt--; cnt--; cnt--; cnt--; cnt--; cnt--; cnt--;
	while (cnt) {  // Countdown i to 0
		if (PEEK(0x030d)&2) {						// Look for ACKNOW on CA1 then read Printer Port
			byte = PEEK(0x0301); 
			return 1; 
		} 	
		cnt--;
	}
	return 0;
}


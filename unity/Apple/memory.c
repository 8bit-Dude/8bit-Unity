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
 *
 * Adapted from: Bill Buckels
 */
 
//	Helper functions for Apple II Aux Memory
//	Last modified: 2020/01/14

#include <peekpoke.h>

#ifdef __APPLE2__
  #pragma code-name("LC")	// MUST BE IN LANGUAGE CARD!!! (Area of memory not banked out during switches between AUX/MAIN memory)
#endif

void MainToAux(unsigned char* src, unsigned int size)
{
	POKEW(0x3c, src);			// Src addr in MAIN (beg)
	POKEW(0x3e, src+size);		// Src addr in MAIN (end)
	POKEW(0x42, src);			// Dst addr in AUX (same as src)
	__asm__("sec");				// Set carry flag: Copy MAIN -> AUX
	__asm__("jsr $c311");		// Call AUXMOVE
}

void AuxToAux(unsigned char* dst, unsigned char* src, unsigned char size)
{
	// Copy pointers and size to ZP
	POKEW(0x3c, src);			// Src addr in AUX
	POKEW(0x3e, dst);			// Dst addr in AUX
	POKE(0x42, size);			// Data size
	
	// Switch to AUX memory
	__asm__("sta $C000");   	// Turn 80STORE off
	__asm__("sta $C003");   	// Read from AUX
	__asm__("sta $C005");   	// Write to AUX	

	// Copy data
	__asm__("ldy #0");
	__asm__("loopCopy:");
	__asm__("   lda ($3c),y");
	__asm__("   sta ($3e),y");
	__asm__("   iny");
	__asm__("   cpy $42");
	__asm__("   bne loopCopy");

	// Switch to MAIN memory
	__asm__("sta $C002");   	// Read from MAIN
	__asm__("sta $C004");   	// Write to MAIN
	__asm__("sta $C001");   	// Turn 80STORE on	
}

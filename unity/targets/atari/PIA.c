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
	POKE(54018, 0b00111000);	// Switch to output Mode
	POKE(54016, 0b11110000);	// Set PIA State
	POKE(54018, 0b00001100);
	POKE(54016, 0b10000000); 	// STROBE ON	
	//cnt--; cnt--; cnt--; cnt--; // Wait some cycles...
}

void InputMode()
{
	POKE(54018, 0b00111000);	// Switch to receive Mode
	POKE(54016, 0b11000000);	// Set PIA State
	POKE(54018, 0b00001100);
	POKE(54016, 0b11000000);	// STROBE ON	
	//cnt--; cnt--; cnt--; cnt--; // Wait some cycles...
}
 
unsigned char SendByte()
{
	// Send 1 byte as 4 x 2bits
	cnt = 255;
	while (cnt) {  // Countdown i to 0
		if (PEEK(53265)) {	// Ready? (Check JOY1 Trigger)
			POKE(54016, (byte&0b00000011)<<4);  // STROBE OFF & Write Bits 1/2
			cnt--; cnt--; cnt--; 
			//cnt--; // Wait some cycles...
			POKE(54016,    	  0b10000000);  	// STROBE ON
			cnt--; cnt--; 
			//cnt--; cnt--; // Wait some cycles...
			
			POKE(54016, (byte&0b00001100)<<2);  // STROBE OFF & Write Bits 3/4
			cnt--; cnt--; cnt--; 
			//cnt--; // Wait some cycles...
			POKE(54016,    	  0b10000000);  	// STROBE ON
			cnt--; cnt--; 
			//cnt--; cnt--; // Wait some cycles...
			
			POKE(54016, (byte&0b00110000));     // STROBE OFF & Write Bits 5/6
			cnt--; cnt--; cnt--; 
			//cnt--; // Wait some cycles...
			POKE(54016,    	  0b10000000);  	// STROBE ON		
			cnt--; cnt--; 
			//cnt--; cnt--; // Wait some cycles...
			
			POKE(54016, (byte&0b11000000)>>2);  // STROBE OFF & Write Bits 7/8
			cnt--; cnt--; cnt--; 
			//cnt--; // Wait some cycles...
			POKE(54016,    	  0b10000000);  	// STROBE ON		
			cnt--; cnt--; 
			//cnt--; cnt--; // Wait some cycles...
			return 1;
		}
		cnt--;
	}
	return 0;
}

unsigned char RecvByte()
{
	// Receive 1 byte as 4 x 2bits
	cnt = 255;
	while (cnt) {  // Countdown i to 0
		if (PEEK(53265)) {	// Ready? (JOY1 Trigger)
			byte = 0;
			POKE(54016, 0b01000000);  				// STROBE OFF
			cnt--; cnt--; cnt--; 
			//cnt--; 			// Wait some cycles...
			byte |= ((PEEK(54016)&0b00110000)>>4);  // Read Bits 1/2
			POKE(54016, 0b11000000);  				// STROBE ON
			cnt--; cnt--; 
			//cnt--; cnt--; 			// Wait some cycles...
			
			POKE(54016, 0b01000000); 				// STROBE OFF
			cnt--; cnt--; cnt--; 
			//cnt--; 			// Wait some cycles...
			byte |= ((PEEK(54016)&0b00110000)>>2);  // Read Bits 3/4
			POKE(54016, 0b11000000);  				// STROBE ON
			cnt--; cnt--; 
			//cnt--; cnt--; 			// Wait some cycles...
			
			POKE(54016, 0b01000000);  				// STROBE OFF
			cnt--; cnt--; cnt--; 
			//cnt--; 			// Wait some cycles...
			byte |= ((PEEK(54016)&0b00110000));	    // Read Bits 5/6
			POKE(54016, 0b11000000);  				// STROBE ON
			cnt--; cnt--; 
			//cnt--; cnt--; 			// Wait some cycles...		
			
			POKE(54016, 0b01000000);  				// STROBE OFF
			cnt--; cnt--; cnt--; 
			//cnt--; 			// Wait some cycles...
			byte |= ((PEEK(54016)&0b00110000)<<2);  // Read Bits 7/8
			POKE(54016, 0b11000000);  				// STROBE ON
			cnt--; cnt--; 
			//cnt--; cnt--; 			// Wait some cycles...
			return 1;
		}
		cnt--;
	}
	return 0;
}


/*
void SendByte(unsigned char value)
{
	unsigned char i = 255;
	while (1) {  // Countdown i to 0
		if (PEEK(53265)) {	// Ready? (JOY1 Trigger)
			POKE(54016, (value&0b00000011)<<4); // STROBE OFF & Write Bits 1/2
			i--; i--;  							// Wait some cycles...
			POKE(54016,    	  0b10000000);  	// STROBE ON
			POKE(54016, (value&0b00001100)<<2); // STROBE OFF & Write Bits 3/4
			i--; i--;  							// Wait some cycles...
			POKE(54016,    	  0b10000000);  	// STROBE ON
			POKE(54016, (value&0b00110000));    // STROBE OFF & Write Bits 5/6
			i--; i--;  							// Wait some cycles...
			POKE(54016,    	  0b10000000);  	// STROBE ON		
			POKE(54016, (value&0b11000000)>>2); // STROBE OFF & Write Bits 7/8
			i--; i--;  							// Wait some cycles...
			POKE(54016,    	  0b10000000);  	// STROBE ON		
			return;
		}
		if (!i--) return;
	}	
}

unsigned char RecvByte(unsigned char* value)
{
	unsigned char i = 255;
	*value = 0;
	while (1) {  // Countdown i to 0
		if (PEEK(53265)) {	// Ready? (JOY1 Trigger)
			POKE(54016, 0b01000000);  				// STROBE OFF
			i--; i--;     		  					// Wait some cycles...
			*value |= ((PEEK(54016)&0b00110000)>>4);// Read Bits 1/2
			POKE(54016, 0b11000000);  				// STROBE ON
			i--; i--;     		  					// Wait some cycles...
			POKE(54016, 0b01000000); 				// STROBE OFF
			i--; i--;     		  					// Wait some cycles...
			*value |= ((PEEK(54016)&0b00110000)>>2);// Read Bits 3/4
			POKE(54016, 0b11000000);  				// STROBE ON
			i--; i--;     		  					// Wait some cycles...
			POKE(54016, 0b01000000);  				// STROBE OFF
			i--; i--;     		  					// Wait some cycles...
			*value |= ((PEEK(54016)&0b00110000));	// Read Bits 5/6
			POKE(54016, 0b11000000);  				// STROBE ON
			i--; i--;     		  					// Wait some cycles...
			POKE(54016, 0b01000000);  				// STROBE OFF
			i--; i--;     		  					// Wait some cycles...
			*value |= ((PEEK(54016)&0b00110000)<<2);// Read Bits 7/8
			POKE(54016, 0b11000000);  				// STROBE ON
			return 1;
		}
		if (!i--) return 0;
	}	
}
*/
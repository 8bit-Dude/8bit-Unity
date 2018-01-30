/*
	Workaround for the missing Apple II clock
	
	Author: Anthony Beaucamp
	Last modified: 2018/01/29
*/

#include "unity.h"

clock_t clk;

// Return clock state
clock_t clock()
{
	return clk;
}

// Sleep for x seconds
unsigned sleep(unsigned seconds)
{
	unsigned int i;
	while (seconds) {
		i = 0;
		while (i<8192) { i++; }
		seconds--;
	}
	clk += CLK_TCK;
	return 1;
}

// Wait for 1 clock tick
void tick()
{
	unsigned int i;
	while (i<128) { i++; }
	clk += 1;		
}

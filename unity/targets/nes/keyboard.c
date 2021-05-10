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

unsigned char key = 0;

char cgetc(void)
{
	unsigned char res;
	while (!key) kbhit();
	res = key; 
	while (kbhit());
	return res;
}

unsigned char kbhit(void)
{
	unsigned char i;
	i = GetJoy(0);
	if (!(i & 64)) {
		key = KB_SP;
		return 1;
	}
	if (!(i & 128)) {
		key = KB_EN;
		return 1;
	}
	key = 0;
	return 0;
}

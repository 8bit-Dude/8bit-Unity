/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2021/12/25
 *
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

#include <string.h>
#include "unity.h"

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

// Externals: see prodos.s
unsigned char mli(unsigned char call, void* ptr_parameter);

// MLI functions
#define CREATE   0xC0
#define OPEN     0xC8
#define READ     0xCA
#define WRITE    0xCB
#define CLOSE    0xCC
#define FLUSH    0xCD
#define SET_MARK 0xCE

// File handle
unsigned char* ref_num;
//static unsigned char Error = 0u;

typedef struct {
	unsigned char param_count;
	char* pathname;
	unsigned char* io_buffer;
	unsigned char ref_num;
} open_param_t;

unsigned int FileOpen(const char* fname)
{
	open_param_t param;
	char Filename[16];
	Filename[0] = strlen(fname);	// Encode filename for MLI
	strcpy(&Filename[1], fname );	
	param.param_count = 3u;
	param.pathname = Filename;
	param.io_buffer = (unsigned char*)FILERAM;
	if (!mli( OPEN, &param )) {
		ref_num = param.ref_num;
		return 1;
	}
	return 0;
}

typedef struct {
	unsigned char param_count;
	unsigned char ref_num;
	unsigned char* data_buffer;
	unsigned int request_count;
	unsigned int trans_count;
} read_param_t;

signed int FileRead(char* buffer, signed int len)
{
	read_param_t param;
	param.param_count = 4u;
	param.ref_num = ref_num;
	param.data_buffer = buffer;
	param.request_count = len;
	mli( READ, &param );
	return param.trans_count;
	//Error = mli( READ, &param );
}

typedef struct {
	unsigned char param_count;
	unsigned char ref_num;
} close_param_t;

/*
signed int FileWrite(char* buffer, signed int len)
{
}
*/

void FileClose( void )
{
	close_param_t param;
	param.param_count = 1u;
	param.ref_num = ref_num;
	mli( CLOSE, &param );
	//Error = mli( CLOSE, &param );
}

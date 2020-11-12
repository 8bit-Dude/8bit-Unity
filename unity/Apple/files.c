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

#include <string.h>

#ifdef __APPLE2__
  #pragma code-name("CODE")
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

const char* VOLUME_NAME = "/SYSTEM/";

// File handle
unsigned char* ref_num;
char Filename[65];
static unsigned char Io[ 1024+256 ]; //I need an adress aligned on 256: I'll use the 1st one inside Io
static unsigned char Error = 0u;

void EncodePath( const char* p_filename )
{
  strcpy(Filename[1], VOLUME_NAME );
  Filename[0] = strlen(VOLUME_NAME);
  strcpy(Filename[1+Filename[0]], p_filename );
  Filename[0] += strlen(p_filename);
}

typedef struct {
	unsigned char param_count;
	char* pathname;
	unsigned char* io_buffer;
	unsigned char ref_num;
} open_param_t;

unsigned char FileOpen( const char* p_filename )
{
	open_param_t param;
	EncodePath( p_filename );
	param.param_count = 3u;
	param.pathname = Filename;
	param.io_buffer = (unsigned char*)((unsigned int)(Io)+256-(unsigned int)(Io)%256); //Getting the 256byte aligned adrress
	Error = mli( OPEN, &param );
	if(!Error)
		ref_num = param.ref_num;
	return Error;
}

typedef struct {
	unsigned char param_count;
	unsigned char ref_num;
	unsigned char* data_buffer;
	unsigned int request_count;
	unsigned int trans_count;
} read_param_t;

unsigned int FileRead( char* buffer, unsigned int len )
{
	read_param_t param;
	param.param_count = 4u;
	param.ref_num = ref_num;
	param.data_buffer = buffer;
	param.request_count = len;
	Error = mli( READ, &param );
	return param.trans_count;
}

typedef struct {
	unsigned char param_count;
	unsigned char ref_num;
} close_param_t;

void file_close( void )
{
	close_param_t param;
	param.param_count = 1u;
	param.ref_num = ref_num;
	Error = mli( CLOSE, &param );
}

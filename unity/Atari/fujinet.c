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

#include <unity.h>

#define TIMEOUT 0x01 // 0x1f == approx 30 seconds

#define DFUJI   0x71
#define DREAD   0x40
#define DWRITE  0x80
#define DUPDATE 0xC0

#define OREAD   0x04
#define OWRITE  0x08
#define OUPDATE 0x0C

char dcbBackup[12];
char *fujiBuffer = (char*)0x0400;	// Memory page shared with xBios buffer

void BackupDCB()
{
	memcpy(dcbBackup, 0x300, 12);
}

void RestoreDCB()
{
	memcpy(0x300, dcbBackup, 12);
}

unsigned char FujiOpen(unsigned char trans) 
{
	BackupDCB();
	OS.dcb.ddevic = DFUJI;
	OS.dcb.dunit  = 1;
	OS.dcb.dcomnd = 'O';
	OS.dcb.dstats = DWRITE;
	OS.dcb.dbuf   = fujiBuffer;
	OS.dcb.dtimlo = TIMEOUT;
	OS.dcb.dbyt   = 256;
	OS.dcb.daux1  = OUPDATE;
	OS.dcb.daux2  = trans;
#ifdef __ATARIXL__	
	enable_rom();
#endif
	__asm__("JSR $E459");
#ifdef __ATARIXL__	
	restore_rom();
#endif
	RestoreDCB();
	return (OS.dcb.dstats == 1);
}

void FujiWrite(unsigned char length)
{
	BackupDCB();
	OS.dcb.ddevic = DFUJI;
	OS.dcb.dunit  = 1;
	OS.dcb.dcomnd = 'W';
	OS.dcb.dstats = DWRITE;
	OS.dcb.dbuf   = fujiBuffer;
	OS.dcb.dtimlo = TIMEOUT;
	OS.dcb.dbyt   = length;
	OS.dcb.daux   = length;
#ifdef __ATARIXL__	
	enable_rom();
#endif
	__asm__("JSR $E459");
#ifdef __ATARIXL__	
	restore_rom();
#endif
	RestoreDCB();
}

unsigned char FujiRead() 
{
	// Check status
	BackupDCB();
	OS.dvstat[0] = 0;
	OS.dcb.ddevic = DFUJI;
	OS.dcb.dunit  = 1;
	OS.dcb.dcomnd = 'S';
	OS.dcb.dstats = DREAD;
	OS.dcb.dbuf   = OS.dvstat;
	OS.dcb.dtimlo = TIMEOUT;
	OS.dcb.dbyt   = 4;
	OS.dcb.daux   = 0;
#ifdef __ATARIXL__	
	enable_rom();
#endif
	__asm__("JSR $E459");
	if (OS.dvstat[0]) {
		// Read actual data
		OS.dcb.ddevic = DFUJI;
		OS.dcb.dunit  = 1;
		OS.dcb.dcomnd = 'R';
		OS.dcb.dstats = DREAD;
		OS.dcb.dbuf   = fujiBuffer;
		OS.dcb.dtimlo = TIMEOUT;
		OS.dcb.dbyt   = OS.dvstat[0];
		OS.dcb.daux   = OS.dvstat[0];
		__asm__("JSR $E459");
	}
#ifdef __ATARIXL__	
	restore_rom();
#endif
	RestoreDCB();
	
	// Flag interrupt as serviced
	fujiReady = 0; PIA.pactl |= 1; 
	fujiBuffer[OS.dvstat[0]] = 0;	
	return OS.dvstat[0];
}

void FujiClose()
{
	BackupDCB();
	OS.dcb.ddevic = DFUJI;
	OS.dcb.dunit  = 1;
	OS.dcb.dcomnd = 'C';
	OS.dcb.dstats = 0x00;
	OS.dcb.dbuf   = 0;
	OS.dcb.dtimlo = TIMEOUT;
	OS.dcb.dbyt   = 0;
	OS.dcb.daux1  = 0;
	OS.dcb.daux2  = 0;
#ifdef __ATARIXL__	
	enable_rom();
#endif
	__asm__("JSR $E459");
#ifdef __ATARIXL__	
	restore_rom();
#endif
	RestoreDCB();
}

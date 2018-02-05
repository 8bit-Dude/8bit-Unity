
#include "unity.h"

#ifdef __APPLE2__
#pragma code-name("LC")
#endif

// Helper functions
#define BYTE4(a,b,c,d) ((a<<6) | (b<<4) | (c<<2) | d)

// Colors for printing
unsigned char headerBG = 0;
unsigned char colorFG, colorBG;

// C64 specific variables & functions
#ifdef __CBM__
	int vicconf[3];
	unsigned char bg = 0;
	unsigned char pow2 = (2 | 2<<2 | 2<<4 | 2<<6);
	void SwitchBank(char bank) {
		// notice that 0 selects vic bank 3, 1 selects vic bank 2, etc.
		POKE(0xDD00, (PEEK(0xDD00) & 252 | (3 - bank))); // switch VIC base
	}
#endif

// Atari specific variables & functions
#ifdef __ATARI__
	#define STARTBMP (0x8E10) // Start Bitmap routine (see DLI.a65)
	#define STOPBMP  (0x8E4D) // Stop Bitmap routine
	unsigned char colorFG1,colorFG2;
	unsigned char colorBG1,colorBG2;
	unsigned char bgByte1,bgByte2;
#endif

// Performance Drawing
#ifdef DEBUG_FPS
clock_t fpsClock;
void DrawFPS(unsigned long  f)
{
    unsigned int fps,dmp1,dmp2;
	
	// Calculate stats
	fpsClock = clock() - fpsClock;
	fps = ( (f-60*(f/60)) * CLK_TCK) / fpsClock;
	colorFG = WHITE;

	// Output stats
	dmp1 = fps/10;
	PrintChr(38, 0, &charDigit[dmp1*3]);
	dmp2 = (fps-dmp1*10);
	PrintChr(39, 0, &charDigit[dmp2*3]);				
	fpsClock = clock();	
}
#endif

// Initialize Bitmap Screen
void InitBitmap() {
#if defined __CBM__
	// Backup VIC config
	vicconf[0] = PEEK(53272);
	vicconf[1] = PEEK(53265);
	vicconf[2] = PEEK(53270);
#elif defined __ATARI__
	// Set default palette
	POKE(PALETTERAM+0, 0x00);
	POKE(PALETTERAM+1, 0x02);
	POKE(PALETTERAM+2, 0x05);
	POKE(PALETTERAM+3, 0x08);
	POKE(PALETTERAM+4, 0x00);
	POKE(PALETTERAM+5, 0x12);
	POKE(PALETTERAM+6, 0x82);
	POKE(PALETTERAM+7, 0xc2);
#elif defined __APPLE2__
	// Prepare Double Hi-Res Mode
	asm("sta $c052"); // TURN ON FULLSCREEN       
	asm("sta $c057"); // TURN ON HI-RES           
	asm("sta $c001"); // TURN ON 80 STORE
#endif	
}

// Switch from Text mode to Bitmap mode
void EnterBitmapMode()
{		
#if defined __CBM__
	// Set data direction flag and Switch bank
	POKE(0xDD02, (PEEK(0xDD02) | 3));
	SwitchBank(VIDEOBANK);

	// Enter multicolor mode
	POKE(0xD018, SCREENLOC*16 + BITMAPLOC);	// 53272: address of screen and bitmap RAM
	POKE(0xD011, PEEK(0xD011) | 32);		// 53265: set bitmap mode
	POKE(0xD016, PEEK(0xD016) | 16);		// 53270: set multicolor mode
#elif defined __ATARI__
	__asm__("jsr %w", STARTBMP);			// Switch ON graphic mode and antic
	POKE(559, 32+16+8+4+2); // ANTIC: DMA Screen + Enable P/M + DMA Players + DMA Missiles + Single resolution
#elif defined __APPLE2__
	// Switch ON Double Hi-Res Mode
	asm("sta $c00d"); // TURN ON 80 COLUMN MODE	  
    asm("sta $c050"); // TURN ON GRAPHICS         
    asm("sta $c05e"); // TURN ON DOUBLE HI-RES    
#endif
}

// Switch from Bitmap mode to Text mode
void ExitBitmapMode()
{
#if defined __CBM__
	// Return VIC and Bank back to previous state
	POKE(53272, vicconf[0]);
	POKE(53265, vicconf[1]);
	POKE(53270, vicconf[2]);
	SwitchBank(0);
#elif defined __ATARI__
    // Switch OFF graphic mode and antic
	__asm__("jsr %w", STOPBMP);
	POKE(559, 0);
#elif defined __APPLE2__
    // Switch OFF Double Hi-Res Mode
    asm("sta $c051"); // TEXT - HIDE GRAPHICS
    asm("sta $c05f"); // TURN OFF DOUBLE RES
	asm("sta $c00c"); // TURN OFF 80 COLUMN MODE	  
#endif
}

unsigned char GetColor(unsigned int x, unsigned int y)
{
#if defined __CBM__
	unsigned char index;
	unsigned int addr, offset;
	
	// Check color index
	DisableRom();
	addr = BITMAPRAM + 40*(y&248)+(y&7)+(x&504);
	index = (PEEK(addr) >> (2*(3-(x/2)%4))) & 3;
	EnableRom();
	
	// Is background color?
	if (index==0) { return bg; }
	
	// Analyze color index
	offset = (y/8)*40+(x/8);
	if (index==1) {	// Upper bits of screen RAM
		addr = SCREENRAM + offset;
		return (PEEK(addr) & 0xF0) >> 4;		
	}
	if (index==2) {	// Lower bits of screen RAM
		addr = SCREENRAM + offset;
		return (PEEK(addr) & 0x0F);
	}
	if (index==3) { // Lower bits of color RAM
		addr = COLORRAM + offset;
		return (PEEK(addr) & 0x0F);
	}
#elif defined __ATARI__
	unsigned int offset;
	unsigned char shift;
	
	// Compute pixel location
	offset = 40*((y*192)/200)+x/8;
	shift = 6 - 2*((x/2)%4);

	// Dual buffer (colour/shade)
	return ( ((PEEK((char*)BITMAPRAM1+offset) & ( 3 << shift )) >> shift) +
			 ((PEEK((char*)BITMAPRAM2+offset) & ( 3 << shift )) >> shift) * 4 );
#elif defined __APPLE2__
	// Rescale coords
 	x = (140*x)/320;
	y = (192*y)/200;
	SetDHRPointer(x,y);
	return GetDHRColor();
#endif	
}

void SetColor(unsigned int x, unsigned int y, unsigned char color)
{
#if defined __CBM__
	unsigned int addr, offset;
	unsigned char shift;
	
	// Set index to 3
	DisableRom();
	offset = 40*(y&248)+(y&7)+(x&504);
	shift = (2*(3-(x/2)%4));
	POKE(BITMAPRAM+offset, PEEK(BITMAPRAM+offset) | 3 << shift);
	EnableRom();
	
	// Set color in COLORAM
	offset = (y/8)*40+(x/8);
	POKE(COLORRAM+offset, color);
#elif defined __ATARI__
	unsigned int offset;
	unsigned char shift;	

	// Compute pixel location
	offset = 40*((y*192)/200)+x/8;
	shift = 6 - 2*((x/2)%4);

	// Dual buffer (colour/shade)
	POKE((char*)BITMAPRAM1+offset, PEEK((char*)BITMAPRAM1+offset) | ( (color%4) << shift ));
	POKE((char*)BITMAPRAM2+offset, PEEK((char*)BITMAPRAM2+offset) | ( (color/4) << shift ));
#elif defined __APPLE2__
	// Rescale coordinates
	x = (140*x)/320;
	y = (192*y)/200;
	SetDHRPointer(x,y);
	SetDHRColor(color);	
#endif
}

// Load bitmap from file
void LoadBitmap(char *filename) 
{
	unsigned int i;
	FILE* fp;
	
	// Open Map File
	fp = fopen(filename, "rb");
	
#if defined __CBM__
	// Consume two bytes of header
	fgetc(fp);
	fgetc(fp);
	
	// 8000 bytes bitmap ram
	fread((char*)(BITMAPRAM), 1, 8000, fp);

	// 1000 bytes char ram
	fread((char*)(SCREENRAM), 1, 1000, fp);
	
	// 1000 bytes color ram
	fread((char*)(COLORRAM), 1, 1000, fp);
	
	// 1 byte background color
	bg = (char) fgetc(fp);	
#elif defined __ATARI__
	// 9 bytes palette ram
	fread((char*)PALETTERAM, 1, 9, fp);
	
	// 7680 bytes RAM1 (color)
	fread((char*)BITMAPRAM1, 1, 7680, fp);
	
	// 7680 bytes RAM2 (luminance)
	fread((char*)BITMAPRAM2, 1, 7680, fp);	
#elif defined __APPLE2__
	// Read 8192 bytes to AUX
	*dhraux = 0;
	fread((char*)BITMAPRAM, 1, 8192, fp);
	
	// Read 8192 bytes to MAIN
	*dhrmain = 0;
	fread((char*)BITMAPRAM, 1, 8192, fp);
#endif
	// Close file
	fclose(fp);

	// Chat background color
	headerBG = GetColor(0, 0);
}

// Clear entire bitmap screen
void ClearBitmap()
{
#if defined __CBM__
	bzero((char*)BITMAPRAM, 8000);
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)COLORRAM, 1000);
#elif defined __ATARI__
	bzero((char*)BITMAPRAM1, 7680);
	bzero((char*)BITMAPRAM2, 7680);
#elif defined __APPLE2__
    // clear main and aux screen memory	
	*dhraux = 0;
    bzero((char *)BITMAPRAM, 8192);
	*dhrmain = 0;
    bzero((char *)BITMAPRAM, 8192);
#endif
}

#ifdef __ATARIXL__
#pragma code-name("SHADOW_RAM2")
#endif

// Draw panel using the background color
void DrawPanel(unsigned char colBeg, unsigned char rowBeg, unsigned char colEnd, unsigned char rowEnd)
{
	// Black-out menu area
	unsigned char j;
	unsigned int span;
	span = colEnd-colBeg+1;
	rowEnd++;
#if defined __CBM__
	for (j=rowBeg; j<rowEnd; ++j) {
		memset((char*)(BITMAPRAM+320*j+colBeg*8), pow2, span*8);
		memset((char*)(SCREENRAM+40*j+colBeg), colorBG, span);		
	}
#elif defined __ATARI__
	colorBG1 = colorBG%4;
	colorBG2 = colorBG/4;
	bgByte1 = BYTE4(colorBG1,colorBG1,colorBG1,colorBG1);
	bgByte2 = BYTE4(colorBG2,colorBG2,colorBG2,colorBG2);
	for (j=rowBeg*8; j<rowEnd*8; ++j) {
		memset((char*)(BITMAPRAM1+j*40+colBeg), bgByte1, span);
		memset((char*)(BITMAPRAM2+j*40+colBeg), bgByte2, span);
	}	
#elif defined __APPLE2__
	for (j=rowBeg*8; j<rowEnd*8; ++j) {
		SetDHRPointer((colBeg*35)/10, j);
		*dhraux = 0;
		bzero(dhrptr, span);
		*dhrmain = 0;
		bzero(dhrptr, span);
	}
#endif
}

// Print multicolor logo for the various platforms...
void PrintLogo(unsigned char col, unsigned char row, unsigned char index)
{
#if defined __CBM__
	// Define logos
	unsigned char logos[4][8] = { {0,0,0, 16, 68, 64, 72, 16}, 		// C64: (0,1,0,0) (1,0,1,0) (1,0,0,0) (1,0,2,0) (0,1,0,0)
								  {0,0,0, 16, 68,168,136,204},		// ATR: (0,1,0,0) (1,0,1,0) (2,2,2,0) (2,0,2,0) (3,0,3,0)
								  {0,0,0,  4, 16,168,168, 48},		// APP: (0,0,1,0) (0,1,0,0) (2,2,2,0) (2,2,2,0) (0,3,0,0)
								  {0,0,0,212,215,255,215,255} };	// FLP: (3,1,1,0) (3,1,1,3) (3,3,3,3) (3,1,1,3) (3,3,3,3)
	unsigned int addr1, addr2;
	unsigned char i;
	
	// Get memory addresses
	addr1 = BITMAPRAM + 40*((row*8)&248)+((col*8)&504);
	addr2 = SCREENRAM + row*40+col;
	
	// Set logo colors
	if (index == 0)	{	// C64
		POKE(addr2, BLUE << 4 | RED);
	} else {
		POKE(addr2, GREEN << 4 | PURPLE);
		POKE(COLORRAM + row*40+col, BLUE);
	}

	// Set Character data
	for (i=0; i<8; ++i) {
		POKE(addr1+i, logos[index][i]);
	}
#elif defined __ATARI__
	// Define logos
	unsigned char logos1[4][8] = { {0,0,0, 48,204,192,204, 48}, 	// C64: (0,3,0,0) (3,0,3,0) (3,0,0,0) (3,0,3,0) (0,3,0,0)	LUMINANCE
								   {0,0,0, 48,204,252,204,204},		// ATR: (0,3,0,0) (3,0,3,0) (3,3,3,0) (3,0,3,0) (3,0,3,0)
								   {0,0,0, 12, 48,252,252, 48},  	// APP: (0,0,3,0) (0,3,0,0) (3,3,3,0) (3,3,3,0) (0,3,0,0)
								   {0,0,0,252,255,255,255,255} };	// FLP: (3,3,3,0) (3,3,3,3) (3,3,3,3) (3,3,3,3) (3,3,3,3)
	unsigned char logos2[4][8] = { {0,0,0, 32,136,128,132, 32}, 	// C64: (0,2,0,0) (2,0,2,0) (2,0,0,0) (2,0,1,0) (0,2,0,0)	CHROMA
								   {0,0,0, 48,204, 84, 68,136},		// ATR: (0,3,0,0) (3,0,3,0) (1,1,1,0) (1,0,1,0) (2,0,2,0)
								   {0,0,0, 12, 32, 84, 84,168},  	// APP: (0,0,3,0) (0,3,0,0) (1,1,1,0) (1,1,1,0) (0,2,0,0)
								   {0,0,0,128,130,170,130,170} };	// FLP: (2,0,0,0) (2,0,0,2) (2,2,2,2) (2,0,0,2) (2,2,2,2)
	unsigned int addr1, addr2;
	unsigned char i;

	// Get memory addresses
	addr1 = BITMAPRAM1+row*320+col;
	addr2 = BITMAPRAM2+row*320+col;		
	
	// Set Character data
	for (i=0; i<8; ++i) {
		POKE(addr1+i*40, logos1[index][i]);
		POKE(addr2+i*40, logos2[index][i]);
	}	
#elif defined __APPLE2__
	// Define logos
	unsigned char logos[4][5][3] = { { { 0, 2, 0}, { 2, 0, 2}, { 2, 0, 0}, { 2, 0, 1}, { 0, 2, 0} },   // C64
									 { { 0,12, 0}, {12, 0,12}, {11,11,11}, {11, 0,11}, { 7, 0, 7} },   // ATR
								     { { 0, 0,12}, { 0,12, 0}, {11,11,11}, {11,11,11}, { 0, 7, 0} },   // APP
								     { { 7, 5, 0}, { 7, 5, 7}, { 7, 7, 7}, { 7, 5, 7}, { 7, 7, 7} } }; // FLP
	unsigned int x,y;
	unsigned char i,j,n;
	
	// Compute location of character
	if (col%2) { n=4; } else { n=3; }
	x = (col*35)/10; y = (row*8);
	
	// Set character over 3/4 pixels out of 7 in a cell
	for (i=0; i<5; ++i) {
		SetDHRPointer(x, y+i+3);
		for (j=0; j<n; j++) {
			if (j<3) {
				SetDHRColor(logos[index][i][j]);
			} else {
				SetDHRColor(BLACK);
			}
			dhrpixel++;
		}
	}
#endif
}

// Print character using background and foreground colors
void PrintChr(unsigned char col, unsigned char row, const char *matrix)
{
#if defined __CBM__
	// Set Character inside 4*8 cell
	unsigned char i;
	unsigned int addr;
	addr = BITMAPRAM + 40*((row*8)&248)+((col*8)&504);
	if (matrix == &charBlank[0]) {
		memset((char*)addr, pow2, 8);
	} else {
		POKE(addr, pow2);
		for (i=0; i<3; ++i) {
			POKE(addr+2*i+1, BYTE4((((matrix[i]>>7)&1) ? 1 : 2), (((matrix[i]>>6)&1) ? 1 : 2), (((matrix[i]>>5)&1) ? 1 : 2), 2));
			POKE(addr+2*i+2, BYTE4((((matrix[i]>>3)&1) ? 1 : 2), (((matrix[i]>>2)&1) ? 1 : 2), (((matrix[i]>>1)&1) ? 1 : 2), 2));
		}
		POKE(addr+7, pow2);
	}
	
	// Set Color
	addr = SCREENRAM + row*40+col;
	POKE(addr, colorFG << 4 | colorBG);
#elif defined __ATARI__	
	// Set Character across double buffer
	unsigned char i;
	unsigned int addr1,addr2;
	colorFG1 = colorFG%4; colorFG2 = colorFG/4;
	colorBG1 = colorBG%4; colorBG2 = colorBG/4;
	bgByte1 = BYTE4(colorBG1,colorBG1,colorBG1,colorBG1);
	bgByte2 = BYTE4(colorBG2,colorBG2,colorBG2,colorBG2);	
	addr1 = BITMAPRAM1+row*320+col;
	addr2 = BITMAPRAM2+row*320+col;
	if (matrix == &charBlank[0]) {
		for (i=0; i<8; ++i) {
			POKE((char*)addr1+i*40, bgByte1);
			POKE((char*)addr2+i*40, bgByte2);
		}
	} else {
		POKE((char*)addr1+0*40, bgByte1);
		POKE((char*)addr2+0*40, bgByte2);
		for (i=0; i<3; ++i) {
			POKE((char*)addr1+(i*2+1)*40, BYTE4((((matrix[i]>>7)&1) ? colorFG1 : colorBG1), (((matrix[i]>>6)&1) ? colorFG1 : colorBG1), (((matrix[i]>>5)&1) ? colorFG1 : colorBG1), colorBG1));
			POKE((char*)addr1+(i*2+2)*40, BYTE4((((matrix[i]>>3)&1) ? colorFG1 : colorBG1), (((matrix[i]>>2)&1) ? colorFG1 : colorBG1), (((matrix[i]>>1)&1) ? colorFG1 : colorBG1), colorBG1));
			POKE((char*)addr2+(i*2+1)*40, BYTE4((((matrix[i]>>7)&1) ? colorFG2 : colorBG2), (((matrix[i]>>6)&1) ? colorFG2 : colorBG2), (((matrix[i]>>5)&1) ? colorFG2 : colorBG2), colorBG2));
			POKE((char*)addr2+(i*2+2)*40, BYTE4((((matrix[i]>>3)&1) ? colorFG2 : colorBG2), (((matrix[i]>>2)&1) ? colorFG2 : colorBG2), (((matrix[i]>>1)&1) ? colorFG2 : colorBG2), colorBG2));
		}
		POKE((char*)addr1+7*40, bgByte1);
		POKE((char*)addr2+7*40, bgByte2);
	}
#elif defined __APPLE2__
	// Set Character over 3/4 pixels out of 7 in a cell
	unsigned int x,y;
	unsigned char i,j,n;
	if (col%2) { n=4; } else { n=3; }
	x = (col*35)/10; y = (row*8);
	SetDHRPointer(x, y);	
	for (j=0; j<n; j++) {
		SetDHRColor(colorBG);
		dhrpixel++;
	}
	for (i=0; i<3; ++i) {
		SetDHRPointer(x, y+i*2+1);
		for (j=0; j<n; j++) {
			SetDHRColor(((matrix[i]>>(7-j))&1) ? colorFG : colorBG);
			dhrpixel++;
		}
		SetDHRPointer(x, y+i*2+2);
		for (j=0; j<n; j++) {
			SetDHRColor(((matrix[i]>>(3-j))&1) ? colorFG : colorBG);
			dhrpixel++;
		}
	}
	SetDHRPointer(x, y+7);
	for (j=0; j<n; j++) {
		SetDHRColor(colorBG);
		dhrpixel++;
	}

	// Update clock (slow function)
	clk += 2;
#endif
}

// Parse string and print characters one-by-one (slow)
void PrintStr(unsigned char col, unsigned char row, const char *buffer)
{
	const char *chr;
	unsigned char i;
	for (i=0; i<strlen(buffer); ++i) {
		// Select the correct bitmask
		if (buffer[i] == 95) 	  { chr = &charUnderbar[0]; }
#if defined __CBM__
		else if (buffer[i] > 192) { chr = &charLetter[(buffer[i]-193)*3]; }		// Upper case (C64)
		else if (buffer[i] > 64)  { chr = &charLetter[(buffer[i]-65)*3]; }		// Lower case (C64)
#else
		else if (buffer[i] > 96)  { chr = &charLetter[(buffer[i]-97)*3]; }	// Lower case (Apple/Atari)
		else if (buffer[i] > 64)  { chr = &charLetter[(buffer[i]-65)*3]; }	// Upper case (Apple/Atari)
#endif
		else if (buffer[i] == 63) { chr = &charQuestion[0]; }
		else if (buffer[i] == 58) { chr = &charColon[0]; }
		else if (buffer[i] > 47)  { chr = &charDigit[(buffer[i]-48)*3]; }
		else if (buffer[i] == 47) { chr = &charSlash[0]; }
		else if (buffer[i] == 46) { chr = &charDot[0]; }
		else if (buffer[i] == 45) { chr = &charHyphen[0]; }
		else if (buffer[i] == 44) { chr = &charComma[0]; }
		else if (buffer[i] > 39)  { chr = &charBracket[(buffer[i]-40)*3]; }
		else if (buffer[i] == 39) { chr = &charQuote[0]; }
		else if (buffer[i] == 33) { chr = &charExclaim[0]; }
		else if (buffer[i] == 32) { chr = &charBlank[0]; }
		PrintChr(col+i, row, chr);
	}
}

// Rolling buffer at the top of the screen, that moves text leftward when printing
void PrintHeader(const char *buffer)
{
	unsigned char len, i;
	len = strlen(buffer);
	colorBG = headerBG;
#if defined __CBM__
	// Roll bitmap and screen ram
	DisableRom();
	memcpy((char*)BITMAPRAM, (char*)(BITMAPRAM+len*8), (40-len)*8);
	EnableRom();
	memcpy((char*)SCREENRAM, (char*)(SCREENRAM+len), (40-len));
#elif defined __ATARI__
	// Roll chroma and luma buffers
	for (i=0; i<8; ++i) {
		memcpy((char*)BITMAPRAM1+i*40, (char*)(BITMAPRAM1+len)+i*40, (40-len));
		memcpy((char*)BITMAPRAM2+i*40, (char*)(BITMAPRAM2+len)+i*40, (40-len));
	}
#elif defined __APPLE2__
	// Always move 7 pixels at a time!
	len = 2*(len/2+len%2);
	for (i=0; i<8; ++i) {
		SetDHRPointer(0, i);		
		*dhraux = 0;
		memcpy((char*)(dhrptr), (char*)(dhrptr+len), (40-len));
		*dhrmain = 0;
		memcpy((char*)(dhrptr), (char*)(dhrptr+len), (40-len));
	}
	if (strlen(buffer)%2) { PrintChr(39, 0, charBlank); }
#endif
	// Print new message
	PrintStr(40-len, 0, buffer);
	colorBG = COLOR_BLACK;
}

// Interactive text input function
unsigned char InputUpdate(unsigned char col, unsigned char row, char *buffer, unsigned char len, unsigned char key)
{
	unsigned char curlen;
	
	// Was a new key received?
	if (!key) {
		// Initialize input fied
		DrawPanel(col, row, col+len, row);
		PrintStr(col, row, buffer);
	} else {
		// Check current length of input
		curlen = strlen(buffer);

		// Process Letter keys
#if defined __ATARI__	
		if (key == 32 | key == 33 | (key > 38 & key < 42) | (key > 43 & key < 59) | key == 63 | (key > 96 & key < 123)) {	// Atari
#else
		if (key == 32 | key == 33 | (key > 38 & key < 42) | (key > 43 & key < 59) | key == 63 | (key > 64 & key < 91)) {	// Apple/C64
#endif
			if (curlen < len) { 
				buffer[curlen] = key;
				buffer[curlen+1] = 0; 
				PrintStr(col+curlen, row, &buffer[curlen]);
			}
		}
		
		// Process Delete key
		if (key == CH_DEL) {
			if (curlen > 0) {
				buffer[curlen-1] = 0;
				PrintChr(col+curlen, row, &charBlank[0]);
			}
		}

		// Return key
		if (key == CH_ENTER) { return 1; }
	}
	
	// Show cursor
	PrintChr(col+strlen(buffer), row, &charUnderbar[0]);
	return 0;
}

void InputStr(unsigned char col, unsigned char row, char *buffer, unsigned char len)
{
	char i;
	
	// Print initial condition
	InputUpdate(col, row, buffer, len, 0);
	
	// Run input loop
	while (1) {
		if (InputUpdate(col, row, buffer, len, cgetc())) { return; }
	}
}

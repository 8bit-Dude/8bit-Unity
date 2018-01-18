
#include "api.h"

#define BYTE2(a,b) ((a<<4) | b)
#define BYTE4(a,b,c,d) ((a<<6) | (b<<4) | (c<<2) | d)

// Colors for printing
unsigned char bg = 0;
unsigned char bgChat = 0;
unsigned char fgCol, bgCol;

// C64 specific functions
#ifdef __CBM__
	int vicconf[3];
	unsigned char pow2 = (2 | 2<<2 | 2<<4 | 2<<6);
	void SwitchBank(char bank) 
	{
		// notice that 0 selects vic bank 3, 1 selects vic bank 2, etc.
		POKE(0xDD00, (PEEK(0xDD00) & 252 | (3 - bank))); // switch VIC base
	}
#endif

// Apple II specific functions
#ifdef __APPLE2__
	// Buffer for HGR loading
	char DHRLoad[1024];
	
	// HGR base array: provides base address for 192 page1 hires scanlines
	unsigned HB[192]={
		0x2000, 0x2400, 0x2800, 0x2C00, 0x3000, 0x3400, 0x3800, 0x3C00,
		0x2080, 0x2480, 0x2880, 0x2C80, 0x3080, 0x3480, 0x3880, 0x3C80,
		0x2100, 0x2500, 0x2900, 0x2D00, 0x3100, 0x3500, 0x3900, 0x3D00,
		0x2180, 0x2580, 0x2980, 0x2D80, 0x3180, 0x3580, 0x3980, 0x3D80,
		0x2200, 0x2600, 0x2A00, 0x2E00, 0x3200, 0x3600, 0x3A00, 0x3E00,
		0x2280, 0x2680, 0x2A80, 0x2E80, 0x3280, 0x3680, 0x3A80, 0x3E80,
		0x2300, 0x2700, 0x2B00, 0x2F00, 0x3300, 0x3700, 0x3B00, 0x3F00,
		0x2380, 0x2780, 0x2B80, 0x2F80, 0x3380, 0x3780, 0x3B80, 0x3F80,
		0x2028, 0x2428, 0x2828, 0x2C28, 0x3028, 0x3428, 0x3828, 0x3C28,
		0x20A8, 0x24A8, 0x28A8, 0x2CA8, 0x30A8, 0x34A8, 0x38A8, 0x3CA8,
		0x2128, 0x2528, 0x2928, 0x2D28, 0x3128, 0x3528, 0x3928, 0x3D28,
		0x21A8, 0x25A8, 0x29A8, 0x2DA8, 0x31A8, 0x35A8, 0x39A8, 0x3DA8,
		0x2228, 0x2628, 0x2A28, 0x2E28, 0x3228, 0x3628, 0x3A28, 0x3E28,
		0x22A8, 0x26A8, 0x2AA8, 0x2EA8, 0x32A8, 0x36A8, 0x3AA8, 0x3EA8,
		0x2328, 0x2728, 0x2B28, 0x2F28, 0x3328, 0x3728, 0x3B28, 0x3F28,
		0x23A8, 0x27A8, 0x2BA8, 0x2FA8, 0x33A8, 0x37A8, 0x3BA8, 0x3FA8,
		0x2050, 0x2450, 0x2850, 0x2C50, 0x3050, 0x3450, 0x3850, 0x3C50,
		0x20D0, 0x24D0, 0x28D0, 0x2CD0, 0x30D0, 0x34D0, 0x38D0, 0x3CD0,
		0x2150, 0x2550, 0x2950, 0x2D50, 0x3150, 0x3550, 0x3950, 0x3D50,
		0x21D0, 0x25D0, 0x29D0, 0x2DD0, 0x31D0, 0x35D0, 0x39D0, 0x3DD0,
		0x2250, 0x2650, 0x2A50, 0x2E50, 0x3250, 0x3650, 0x3A50, 0x3E50,
		0x22D0, 0x26D0, 0x2AD0, 0x2ED0, 0x32D0, 0x36D0, 0x3AD0, 0x3ED0,
		0x2350, 0x2750, 0x2B50, 0x2F50, 0x3350, 0x3750, 0x3B50, 0x3F50,
		0x23D0, 0x27D0, 0x2BD0, 0x2FD0, 0x33D0, 0x37D0, 0x3BD0, 0x3FD0};

	// Addresses for AUX bank copying
	unsigned *src  = (unsigned *)0x3c;
	unsigned *dest = (unsigned *)0x42;	
	void MainToAux(unsigned src0, unsigned src1, unsigned dest0)
	{
		// move a block of data from main to auxiliary memory
		src[0] = src0;
		src[1] = src1;
		dest[0] = dest0;
		asm("sec");
		asm("jsr $c311");
	}
#endif

void EnterBitmapMode()
{	
#if defined __CBM__
	// Backup VIC config
	vicconf[0] = PEEK(53272);
	vicconf[1] = PEEK(53265);
	vicconf[2] = PEEK(53270);
	
	// Set data direction flag
	POKE(0xDD02, (PEEK(0xDD02) | 3));

	// now that we copied the bitmap data, switch bank
	SwitchBank(VIDEOBANK);

	// multicolor on
	POKE(0xD018, SCREENLOC*16 + BITMAPLOC);	// 53272: address of screen and bitmap RAM
	POKE(0xD011, PEEK(0xD011) | 32);		// 53265: set bitmap mode
	POKE(0xD016, PEEK(0xD016) | 16);		// 53270: set multicolor mode
#elif defined __ATARI__
    // Switch ON graphic mode and antic
	__asm__("jsr %w", STARTBMP);
	POKE(559, 32+16+8+4+2); // DMA Screen + Enable P/M + DMA Players + DMA Missiles + Single resolution  (shadow for 54272)
#elif defined __APPLE2__
	// Switch ON Double Hi-Res Mode
    asm("sta $c050"); /* GRAPHICS                 */
    //asm("sta $c054"); /* PAGE ONE                 */
    asm("sta $c052"); /* GRAPHICS ONLY, NOT MIXED */
    asm("sta $c057"); /* HI-RES                   */
    asm("sta $c05e"); /* TURN ON DOUBLE RES       */
    asm("sta $c00d"); /* 80 COLUMN MODE	          */
    //asm("sta $c001"); /* TURN ON 80 STORE  */
#endif
}

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
    asm("sta $c000"); /* TURN OFF 80 STORE        */
    asm("sta $c051"); /* TEXT - HIDE GRAPHICS */
    asm("sta $c05f"); /* TURN OFF DOUBLE RES  */
    //asm("sta $c054"); /* PAGE ONE             */
    //asm("sta $c00d"); /* 80 COLUMN MODE	      */
#endif
}

void ClearBitmap()
{
#if defined __APPLE2__
    /* clear main and aux screen memory */
    memset((char *)BITMAPRAM,0,8192);
    MainToAux(BITMAPRAM,BITMAPRAM+8191,8192);
#endif
}

void LoadBitmap(char *filename) 
{
#if defined __APPLE2__
	unsigned src0, src1, dest;
#endif
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
	// 9 bytes palette + 7680 bytes RAM1 + 7680 bytes RAM2
	fread((char*)PALETTERAM, 1, 9, fp);
	fread((char*)BITMAPRAM1, 1, 7680, fp);
	fread((char*)BITMAPRAM2, 1, 7680, fp);	
	bg = BLACK;
#elif defined __APPLE2__
	// Read 8192+8192 of memory
	src0 = (unsigned)&DHRLoad[0];
	src1 = src0 + 1023;
	dest = BITMAPRAM;

	// Aux Memory Part
	for (i = 0;i < 8; i++) {
		fread(DHRLoad,1,1024,fp);
		MainToAux(src0,src1,dest);
		dest += 1024;
	}

	// Main Memory Part
	dest = BITMAPRAM;
	for (i = 0;i < 8; i++) {
		fread(DHRLoad,1,1024,fp);
		memcpy((char *)dest,(char *)&DHRLoad[0],1024);
		dest += 1024;
	}
#endif
	// Close file
	fclose(fp);

	// Set the border color
	bordercolor(bg);
	bgcolor(bg);
	
	// chat background color
	bgChat = GetColor(0, 0);	
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
	offset = y*40+x/8;
	shift = 6 - 2*((x/2)%4);

	// Dual buffer (colour/shade)
	return ( ((PEEK((char*)BITMAPRAM1+offset) & ( 3 << shift )) >> shift) +
			 ((PEEK((char*)BITMAPRAM2+offset) & ( 3 << shift )) >> shift) * 4 );
#elif defined __APPLE2__
	return GRAY;
#endif	
}

void SetColor(unsigned int x, unsigned int y, unsigned char color)
{
#if defined __CBM__
	// Set pixel
	POKE(BITMAPRAM + 40*(y&248)+(y&7)+(x&504), 0);
#elif defined __APPLE2__
	// Call Assembly Function
	//DHRSetColor(x,y,color);
#endif
}

void PrintLogo(unsigned char col, unsigned char row, unsigned char index)
{
	unsigned int addr1, addr2;
	unsigned char i;
#if defined __CBM__
	// Define logos
	unsigned char logos[3][8] = { {0,0,0,16,68, 64, 72, 16}, 	// C64: (0,1,0,0) (1,0,1,0) (1,0,0,0) (1,0,2,0) (0,1,0,0)
								  {0,0,0,16,68,168,136,204},	// ATR: (0,1,0,0) (1,0,1,0) (2,2,2,0) (2,0,2,0) (3,0,3,0)
								  {0,0,0, 4,16,168,168,252} };	// APP: (0,0,1,0) (0,1,0,0) (2,2,2,0) (2,2,2,0) (3,3,3,0)
	
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
	unsigned char logos1[3][8] = { {0,0,0,48,204,192,204, 48}, 	// C64: (0,3,0,0) (3,0,3,0) (3,0,0,0) (3,0,3,0) (0,3,0,0)	LUMINANCE
								   {0,0,0,48,204,252,204,204},	// ATR: (0,3,0,0) (3,0,3,0) (3,3,3,0) (3,0,3,0) (3,0,3,0)
								   {0,0,0,12, 48,252,252,252} };// APP: (0,0,3,0) (0,3,0,0) (3,3,3,0) (3,3,3,0) (3,3,3,0)
	unsigned char logos2[3][8] = { {0,0,0,32,136,128,132, 32}, 	// C64: (0,2,0,0) (2,0,2,0) (2,0,0,0) (2,0,1,0) (0,2,0,0)	CHROMA
								   {0,0,0,48,204, 84, 68,136},	// ATR: (0,3,0,0) (3,0,3,0) (1,1,1,0) (1,0,1,0) (2,0,2,0)
								   {0,0,0,12, 48, 84, 84,168} };// APP: (0,0,3,0) (0,3,0,0) (1,1,1,0) (1,1,1,0) (2,2,2,0)

	// Get memory addresses
	addr1 = BITMAPRAM1+row*320+col;
	addr2 = BITMAPRAM2+row*320+col;		
	
	// Set Character data
	for (i=0; i<8; ++i) {
		POKE(addr1+i*40, logos1[index][i]);
		POKE(addr2+i*40, logos2[index][i]);
	}		
#endif
}

void PrintChr(unsigned char col, unsigned char row, const char *matrix)
{
	unsigned int addr1, addr2;
	unsigned char i;

#if defined __CBM__
	// Set Character inside 4*8 cell
	addr1 = BITMAPRAM + 40*((row*8)&248)+((col*8)&504);
	if (matrix == &charBlank[0]) {
		memset((char*)addr1, pow2, 8);
	} else {
		POKE(addr1, pow2);
		for (i=0; i<3; ++i) {
			POKE(addr1+2*i+1, BYTE4((((matrix[i]>>7)&1) ? 1 : 2), (((matrix[i]>>6)&1) ? 1 : 2), (((matrix[i]>>5)&1) ? 1 : 2), 2));
			POKE(addr1+2*i+2, BYTE4((((matrix[i]>>3)&1) ? 1 : 2), (((matrix[i]>>2)&1) ? 1 : 2), (((matrix[i]>>1)&1) ? 1 : 2), 2));
		}
		POKE(addr1+7, pow2);
	}
	
	// Set Color
	addr2 = SCREENRAM + row*40+col;
	POKE(addr2, fgCol << 4 | bgCol);
#elif defined __ATARI__	
	// Set Character across double buffer
	unsigned char fgCol1,fgCol2;
	unsigned char bgCol1,bgCol2;
	fgCol1 = fgCol%4;
	fgCol2 = fgCol/4;
	bgCol1 = bgCol%4;
	bgCol2 = bgCol/4;
	addr1 = BITMAPRAM1+row*320+col;
	addr2 = BITMAPRAM2+row*320+col;
	if (matrix == &charBlank[0]) {
		for (i=0; i<8; ++i) {
			POKE((char*)addr1+i*40, BYTE4(bgCol1,bgCol1,bgCol1,bgCol1));
			POKE((char*)addr2+i*40, BYTE4(bgCol2,bgCol2,bgCol2,bgCol2));
		}
	} else {
		POKE((char*)addr1+0*40, BYTE4(bgCol1,bgCol1,bgCol1,bgCol1));
		POKE((char*)addr2+0*40, BYTE4(bgCol2,bgCol2,bgCol2,bgCol2));
		for (i=0; i<3; ++i) {
			POKE((char*)addr1+(i*2+1)*40, BYTE4((((matrix[i]>>7)&1) ? fgCol1 : bgCol1), (((matrix[i]>>6)&1) ? fgCol1 : bgCol1), (((matrix[i]>>5)&1) ? fgCol1 : bgCol1), bgCol1));
			POKE((char*)addr1+(i*2+2)*40, BYTE4((((matrix[i]>>3)&1) ? fgCol1 : bgCol1), (((matrix[i]>>2)&1) ? fgCol1 : bgCol1), (((matrix[i]>>1)&1) ? fgCol1 : bgCol1), bgCol1));
			POKE((char*)addr2+(i*2+1)*40, BYTE4((((matrix[i]>>7)&1) ? fgCol2 : bgCol2), (((matrix[i]>>6)&1) ? fgCol2 : bgCol2), (((matrix[i]>>5)&1) ? fgCol2 : bgCol2), bgCol2));
			POKE((char*)addr2+(i*2+2)*40, BYTE4((((matrix[i]>>3)&1) ? fgCol2 : bgCol2), (((matrix[i]>>2)&1) ? fgCol2 : bgCol2), (((matrix[i]>>1)&1) ? fgCol2 : bgCol2), bgCol2));
		}
		POKE((char*)addr1+7*40, BYTE4(bgCol1,bgCol1,bgCol1,bgCol1));
		POKE((char*)addr2+7*40, BYTE4(bgCol2,bgCol2,bgCol2,bgCol2));
	}
#elif defined __APPLE2__	
	unsigned x1 = col;
	unsigned y1 = 8*row;
	unsigned destoffset; 
	unsigned char buff[4];

	
	//asm("sta $c054"); /* PAGE ONE                 */
	//asm("sta $c055"); /* PAGE TWO                 */

/*		
    // rasters are split between auxiliary memory and main memory
	while (y1 < y2) {
		// assign auxiliary memory
		src[0] = (unsigned) &buff[0];
		src[1] = (unsigned) &buff[0];
		dest[0] = destoffset = (HB[y1] + x1);
		asm("sec");
		asm("jsr $c311");
		
		// assign main memory
		memcpy((char *)destoffset,&buff[0],1);
		y1++;
	} */
#endif
}

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
#elif defined __ATARI__
		else if (buffer[i] > 96)  { chr = &charLetter[(buffer[i]-97)*3]; }	// Lower case (Atari)
		else if (buffer[i] > 64)  { chr = &charLetter[(buffer[i]-65)*3]; }	// Upper case (Atari)
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

void PrintHeader(const char *buffer)
{
#if defined __CBM__
	// Rolling message buffer
	DisableRom();
	memcpy((char*)BITMAPRAM, (char*)(BITMAPRAM+strlen(buffer)*8), (40-strlen(buffer))*8);
	EnableRom();
	memcpy((char*)SCREENRAM, (char*)(SCREENRAM+strlen(buffer)), (40-strlen(buffer)));
#elif defined __ATARI__
	unsigned char i;
	for (i=0; i<8; ++i) {
		memcpy((char*)BITMAPRAM1+i*40, (char*)(BITMAPRAM1+strlen(buffer))+i*40, (40-strlen(buffer)));
		memcpy((char*)BITMAPRAM2+i*40, (char*)(BITMAPRAM2+strlen(buffer))+i*40, (40-strlen(buffer)));
	}
#endif
	// Print new message
	bgCol = bgChat;
	PrintStr(40-strlen(buffer), 0, buffer);
	bgCol = COLOR_BLACK;
}

void PrintInput(unsigned char col, unsigned char row, char *buffer, unsigned char len)
{
	// Refresh input field
	char i;
	for (i=0; i<(len+1); ++i) {
		PrintChr(col+i, row, &charBlank[0]);
	}
	PrintStr(col, row, buffer);
	PrintChr(col+strlen(buffer), row, &charUnderbar[0]);
}

char KeyStr(char *buffer, unsigned char len, unsigned char key)
{
	// Letter key
#if defined __CBM__	
	if (key == 32 | key == 33 | (key > 38 & key < 42) | (key > 43 & key < 59) | key == 63 | (key > 64 & key < 91)) {
#elif defined __ATARI__
	if (key == 32 | key == 33 | (key > 38 & key < 42) | (key > 43 & key < 59) | key == 63 | (key > 96 & key < 123)) {
#elif defined __APPLE2__
	if (key == 32 | key == 33 | (key > 38 & key < 42) | (key > 43 & key < 59) | key == 63 | (key > 96 & key < 123)) {
#endif
		if (strlen(buffer) < len) { 
			buffer[strlen(buffer)+1] = 0; 
			buffer[strlen(buffer)] = key; 
		}
		return 2;		
	}
	// Delete key
	if (key == CH_DEL) {
		if (strlen(buffer) > 0) { 
			buffer[strlen(buffer)-1] = 0; 
		}
		return 2;
	// Return key
	} else if (key == CH_ENTER) { 
		return 1;	
	} else {
		return 0;
	}
}

void InputStr(unsigned char col, unsigned char row, char *buffer, unsigned char len)
{
	char i;
	
	// Print initial condition
	PrintInput(col, row, buffer, len);
	
	// Run input loop
	while (1) {
		if (kbhit()) {
			i = KeyStr(buffer, len, cgetc());
			if (i == 2) {
				PrintInput(col, row, buffer, len);
			} else if (i == 1) {
				return;
			}
		}
	}
}

// Performance Drawing
#ifdef DEBUG_FPS
clock_t tFPS;
void DrawFPS(unsigned long  f)
{
    unsigned int fps,dmp1,dmp2;
	
	// Calculate stats
	tFPS = clock() - tFPS;
	fps = ( (f-60*(f/60)) * CLK_TCK) / tFPS;
	fgCol = WHITE;

	// Output stats
	dmp1 = fps/10;
	PrintChr(38, 0, &charDigit[dmp1*3]);
	dmp2 = (fps-dmp1*10);
	PrintChr(39, 0, &charDigit[dmp2*3]);				
	tFPS = clock();	
}
#endif

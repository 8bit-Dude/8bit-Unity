
#include "unity.h"

int main (void)
{
    unsigned char xSize, ySize, key;

	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	clrscr();

    // Ask for the screen size
    screensize(&xSize, &ySize);

    // Top line
    cputc(CH_ULCORNER);
    chline(xSize - 2);
    cputc(CH_URCORNER);

    // Vertical line, left-side 
    cvlinexy(0, 1, ySize - 2);

    // Bottom line
    cputc(CH_LLCORNER);
    chline(xSize - 2);
    cputc(CH_LRCORNER);

    // Vertical line, right-side 
    cvlinexy(xSize - 1, 1, ySize - 2);
	
    // Display selection menu
    gotoxy(11, 8);  cprintf("Network Interface:");
    gotoxy(13, 10); cprintf("0 - None");		
#if defined __HUB__	
    gotoxy(13, 11); cprintf("1 - 8bit-Hub");	
#endif
#if defined __IP65__	
    gotoxy(13, 12); cprintf("2 - RR-Net");	
#endif
    gotoxy(10, 14);  cprintf("< 8bit-Unity Loader >");
	
	//////////////////////////////////////////////////
	// Create file load & run routine in higher memory
	
	// Call kernal loaded
	__asm__("lda #$A9");	// LDA
	__asm__("sta $C000");
	__asm__("lda #$00");	// $00
	__asm__("sta $C001");
	__asm__("lda #$A2");	// LDX
	__asm__("sta $C002");
	__asm__("lda #$01");	// $01
	__asm__("sta $C003");			
	__asm__("lda #$A0");	// LDY
	__asm__("sta $C004");
	__asm__("lda #$08");	// $08
	__asm__("sta $C005");			
	__asm__("lda #$20"); 	// JSR
	__asm__("sta $C006");
	__asm__("lda #$D5");	// Kernal Loader ($FFD5)
	__asm__("sta $C007");
	__asm__("lda #$FF");
	__asm__("sta $C008");
	
	// Turn-on basic
	__asm__("lda #$A9");	// LDA
	__asm__("sta $C009");
	__asm__("lda #$37");	// $36
	__asm__("sta $C00a");
	__asm__("lda #$85");	// STA
	__asm__("sta $C00b");
	__asm__("lda #$01");	// $01
	__asm__("sta $C00c");	

	// Run basic program
 	__asm__("lda #$20"); 	// JSR
	__asm__("sta $C00d");
	__asm__("lda #$59");	// Basic Loader ($A659)
	__asm__("sta $C00e");
	__asm__("lda #$A6");
	__asm__("sta $C00f");
	__asm__("lda #$4C"); 	// JMP
	__asm__("sta $C010");
	__asm__("lda #$AE");	// Basic Loader ($A7AE)
	__asm__("sta $C011");
	__asm__("lda #$A7");
	__asm__("sta $C012");	 
	
	
	while (1) {
		// Run selected debug
		key = cgetc();
		
		switch (key) {
		case '0':
			gotoxy(7, 17); cprintf(" Loading None version...");
			cbm_k_setlfs(0, 8, 0);
			cbm_k_setnam("hub.prg");
			__asm__("jmp $C000");	// Execute
#if defined __HUB__				
		case '1':
			gotoxy(5, 17); cprintf(" Loading 8bit-Hub version...");
			cbm_k_setlfs(0, 8, 0);
			cbm_k_setnam("hub.prg");
			__asm__("jmp $C000");	// Execute
#endif			
#if defined __IP65__				
		case '2':
			gotoxy(5, 17); cprintf("  Loading RR-Net version...");
			cbm_k_setlfs(0, 8, 0);
			cbm_k_setnam("rrnet.prg");
			__asm__("jmp $C000");	// Execute
#endif
		}
	}
		
    // Done
    return EXIT_SUCCESS;
}

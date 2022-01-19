
#include "unity.h"

const unsigned char hubPRG[] = "hub.prg";
const unsigned char rrnetPRG[] = "rrnet.prg";
const unsigned char ultimatePRG[] = "ultimate.prg";

int main (void)
{
    unsigned char xSize, ySize, key, *prg;

	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	clrscr();

	// Install Krill loader on drive
	//__asm__("  jsr $A000");
	//__asm__("  lda $ff");	
	//__asm__("  sta $fe");	
	
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
#if defined __ULTIMATE__	
    gotoxy(13, 13); cprintf("3 - Ultimate");	
#endif
    gotoxy(10, 15);  cprintf("< 8bit-Unity Loader >");
	
	//////////////////////////////////////////////////////
	// Create file loader & run routine in higher memory
	
	//if (PEEK(0xfe)) {
		// Use Kernal Loader
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
		__asm__("lda #$D5");	// Kernal Load ($FFD5)
		__asm__("sta $C007");
		__asm__("lda #$FF");
		__asm__("sta $C008");
	//} else {
	//	// Use Krill Loader
	//	__asm__("lda #$20"); 	// JSR
	//	__asm__("sta $C003");
	//	__asm__("lda #$00");	// Krill Open ($BE00)
	//	__asm__("sta $C004");
	//	__asm__("lda #$BE");
	//	__asm__("sta $C005");
	//	__asm__("lda #$20"); 	// JSR
	//	__asm__("sta $C006");
	//	__asm__("lda #$07");	// Krill Load ($BE07)
	//	__asm__("sta $C007");
	//	__asm__("lda #$BE");
	//	__asm__("sta $C008");
	//}
	
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
		gotoxy(7, 18); 
		switch (key) {
		case '0':
			cprintf("  Loading None version...  ");
			prg = hubPRG;
			break;
#if defined __HUB__				
		case '1':
			cprintf("Loading 8bit-Hub version...");
			prg = hubPRG;
			break;
#endif			
#if defined __IP65__				
		case '2':
			cprintf(" Loading RR-Net version... ");
			prg = rrnetPRG;
			break;
#endif
#if defined __ULTIMATE__				
		case '3':
			cprintf("Loading Ultimate version...");
			prg = ultimatePRG;
			break;
#endif
		default:
			continue;
		}
		
		//if (PEEK(0xfe)) {		
			cbm_k_setlfs(0, 8, 0);
			cbm_k_setnam(prg);
			__asm__("jmp $C000");	// Execute
		//} else {
		//	__asm__("lda #$7f"); 	// disable KERNAL
		//	__asm__("sta $dc0d"); 	// timer interrupts			
		//	__asm__("ldx #<_hubPRG");
		//	__asm__("ldy #>_hubPRG");
		//	__asm__("jmp $C003");	// Execute
		//}		
	}
		
    // Done
    return EXIT_SUCCESS;
}

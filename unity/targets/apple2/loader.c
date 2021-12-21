
#include "unity.h"

extern unsigned char* ref_num;

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
    gotoxy(13, 12); cprintf("2 - Uthernet");	
#endif
    gotoxy(10, 14);  cprintf("< 8bit-Unity Loader >");
		
	//////////////////////////////////////////////////
	// Create file load & run routine in higher memory
	
	// Load file with MLI
	__asm__("lda #$20");	// JSR
	__asm__("sta $9000");
	__asm__("lda #$00");	// ProDOS ($BF00)
	__asm__("sta $9001");
	__asm__("lda #$BF");	
	__asm__("sta $9002");
	__asm__("lda #$CA");	// READ ($CA)
	__asm__("sta $9003");			
	__asm__("lda #$0F");	// MLI Addr
	__asm__("sta $9004");
	__asm__("lda #$90");	// MLI Addr+1
	__asm__("sta $9005");

	// Close file with MLI
	__asm__("lda #$20");	// JSR
	__asm__("sta $9006");
	__asm__("lda #$00");	// ProDOS ($BF00)
	__asm__("sta $9007");
	__asm__("lda #$BF");	
	__asm__("sta $9008");
	__asm__("lda #$CC");	// CLOSE ($CC)
	__asm__("sta $9009");			
	__asm__("lda #$17");	// MLI Addr
	__asm__("sta $900A");
	__asm__("lda #$90");	// MLI Addr+1
	__asm__("sta $900B");
	
	// Run decruncher
	__asm__("lda #$4C"); 	// JMP
	__asm__("sta $900C");
	__asm__("lda #$03");	// Decrunch ($0803)
	__asm__("sta $900D");
	__asm__("lda #$08");
	__asm__("sta $900E");
	
	// MLI parameters (READ)
	__asm__("lda #$04"); 	// 4 params
	__asm__("sta $900F");
	__asm__("lda #$01");	// Refnum
	__asm__("sta $9010");
	__asm__("lda #$03");	// Buffer ($0801)
	__asm__("sta $9011");
	__asm__("lda #$08");
	__asm__("sta $9012");
	__asm__("lda #$ff");	// Requested Size ($ffff)
	__asm__("sta $9013");
	__asm__("lda #$ff");
	__asm__("sta $9014");	
	__asm__("lda #$00");	// Returned Size ($0000)
	__asm__("sta $9015");
	__asm__("lda #$00");
	__asm__("sta $9016");

	// MLI parameters (CLOSE)
	__asm__("lda #$01"); 	// 4 params
	__asm__("sta $9017");
	__asm__("lda #$01");	// Refnum
	__asm__("sta $9018");
	
	while (1) {
		// Run selected debug
		key = cgetc();
		
		switch (key) {
		case '0':
			gotoxy(7, 17); cprintf(" Loading None version...");
			if (FileOpen("hub")) {			
				__asm__("jmp $9000");
			}
			break;
#if defined __HUB__				
		case '1':
			gotoxy(5, 17); cprintf(" Loading 8bit-Hub version...");
			if (FileOpen("hub")) {			
				__asm__("jmp $9000");
			}
			break;
#endif			
#if defined __IP65__				
		case '2':
			gotoxy(5, 17); cprintf(" Loading Uthernet version...");
			if (FileOpen("uthernet")) {
				__asm__("jmp $9000");
			}
			break;
#endif
		}
	}
		
    // Done
    return EXIT_SUCCESS;
}

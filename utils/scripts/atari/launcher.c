
#include "unity.h"

// Externals: see xbios.s
extern void FileSet(const char* fname);
extern unsigned char __fastcall__ xbios_run_file(void);

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
	
    // Write the greeting in the mid of the screen
    gotoxy(11, 8);  cprintf("Network Interface:");
#if defined __HUB__	
    gotoxy(13, 10); cprintf("1 - 8bit-Hub");	
#endif
#if defined __IP65__	
    gotoxy(13, 11); cprintf("2 - DragonCart");	
#endif
#if defined __FUJINET__	
    gotoxy(13, 12); cprintf("3 - Fujinet");	
#endif
    gotoxy(11, 14);  cprintf("< 8bit-Launcher >");
	
	while (1) {
		// Run selected debug
		key = cgetc();
		
		switch (key) {
#if defined __HUB__				
		case '1':
			gotoxy(5, 17); cprintf(" Loading 8bit-Hub version...");
			FileSet("hub.xex");
			xbios_run_file();
			break;
#endif			
#if defined __FUJINET__				
		case '3':
			gotoxy(5, 17); cprintf("  Loading Fujinet version...");
			FileSet("hub.xex");
			xbios_run_file();
			break;
#endif
#if defined __IP65__				
		default:
			gotoxy(5, 17);  cprintf("Loading DragonCart version...");
			FileSet("dragon.xex");
			xbios_run_file();
		}
#endif
	}
		
    // Done
    return EXIT_SUCCESS;
}

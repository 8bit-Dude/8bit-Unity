
#include "unity.h"

char healthHeader[] = { 1 , 32, 32, 32, 0 };
char armorHeader[]  = { 3 , 32, 32, 32, 0 };
char goldHeader[]   = {'$', 32, 32, 32, 0 };
char killsHeader[]  = { 4 , 32, 32, 32, 0 };

extern unsigned char health, armor, gold, kills;

void SplashScreen(void)
{	
	// Load and show banner
	InitBitmap();
	LoadBitmap("menu.img");
	ShowBitmap();
	
	// Show credit/build
	paperColor = BLACK; 
	PrintBlanks(CHR_COLS-13, CHR_ROWS-4, 12, 3);
#if (defined __ORIC__)
	inkColor = AIC;
#else		
	inkColor = RED; 
#endif
	PrintStr(CHR_COLS-12, CHR_ROWS-4, "8BIT-DUNGEON");		
	PrintStr(CHR_COLS-12, CHR_ROWS-3, " TECH DEMO  ");		
	PrintStr(CHR_COLS-12, CHR_ROWS-2, " 2021/01/16 ");
	
	// Start music
	PlayMusic();

	// Wait until key is pressed
	while (!kbhit()) {	
	#if defined __APPLE2__
		UpdateMusic();
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif
	}	
	
	// Exit banner screen
	HideBitmap();
	StopMusic();	
}

void PrintHealth(void)
{
	inkColor = RED;    
	PrintStr(0, CHR_ROWS-1, healthHeader); 
	PrintNum(1,  CHR_ROWS-1, health);
}

void PrintArmor(void)
{
	inkColor = CYAN;   
	PrintStr(5, CHR_ROWS-1, armorHeader);  
	PrintNum(6,  CHR_ROWS-1, armor);
}

void PrintGold(void)
{
	inkColor = YELLOW; 
	PrintStr(CHR_COLS-9, CHR_ROWS-1, goldHeader); 
	PrintNum(CHR_COLS-8, CHR_ROWS-1, gold);
}

void PrintKills(void)
{
	inkColor = WHITE;  
	PrintStr(CHR_COLS-4, CHR_ROWS-1, killsHeader); 
	PrintNum(CHR_COLS-3, CHR_ROWS-1, kills);	
}

void PrintKey(void)
{
	inkColor = YELLOW;
	PrintStr(10, CHR_ROWS-1, "KEY");  	
}

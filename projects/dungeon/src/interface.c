
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
	PrintBlanks(TXT_COLS-13, TXT_ROWS-4, 12, 3);
#if (defined __ORIC__)
	inkColor = AIC;
#else		
	inkColor = RED; 
#endif
	PrintStr(TXT_COLS-12, TXT_ROWS-4, "8BIT-DUNGEON");		
	PrintStr(TXT_COLS-12, TXT_ROWS-3, " TECH DEMO  ");		
	PrintStr(TXT_COLS-12, TXT_ROWS-2, " 2021/01/16 ");
	
	// Start music
#ifdef __CBM__	
	LoadMusic("title.mus");
#else
	LoadMusic("dungeon.mus");
#endif	
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
#ifdef __CBM__	
	LoadMusic("dungeon.mus");
#endif
}

void PrintHealth(void)
{
	inkColor = RED;    
	PrintStr(0, TXT_ROWS-1, healthHeader); 
	PrintNum(1,  TXT_ROWS-1, health);
}

void PrintArmor(void)
{
	inkColor = CYAN;   
	PrintStr(5, TXT_ROWS-1, armorHeader);  
	PrintNum(6,  TXT_ROWS-1, armor);
}

void PrintGold(void)
{
	inkColor = YELLOW; 
	PrintStr(TXT_COLS-9, TXT_ROWS-1, goldHeader); 
	PrintNum(TXT_COLS-8, TXT_ROWS-1, gold);
}

void PrintKills(void)
{
	inkColor = WHITE;  
	PrintStr(TXT_COLS-4, TXT_ROWS-1, killsHeader); 
	PrintNum(TXT_COLS-3, TXT_ROWS-1, kills);	
}

void PrintKey(void)
{
	inkColor = YELLOW;
	PrintStr(10, TXT_ROWS-1, "KEY");  	
}

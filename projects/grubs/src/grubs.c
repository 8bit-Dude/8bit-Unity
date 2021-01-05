
#include <cc65.h>
#include "unity.h"

#define MAX_GRUBS 4
#define MAX_PROJS 1

#define STATE_PHYS 1
#define STATE_JUMP 2
#define STATE_MOVE 4
#define STATE_WEAP 8
#define STATE_LOAD 16

#define PROJ_NULL  0
#define PROJ_LOAD  1
#define PROJ_MOVE  2

#define KEYFRAME_MLEFT  0
#define KEYFRAME_JLEFT  3
#define KEYFRAME_MRIGHT 4
#define KEYFRAME_JRIGHT 7
#define KEYFRAME_DEAD   15
#define KEYFRAME_WEAP   8
#define KEYFRAME_PROJ   14

#define NUMFRAME_MOVE   3
#define NUMFRAME_JUMP   1
#define NUMFRAME_WEAP   3

#define EXPLODE YELLOW
#define HEALTH_LOW RED
#define HEALTH_MED ORANGE
#if defined __APPLE2__
	#define SKY 		LBLUE
	#define HEALTH_HIGH LGREEN
	#define GRND_OFFST	7
#elif defined __ATARI__
	#define SKY 		BLUE
	#define HEALTH_HIGH GREEN
	#define GRND_OFFST	4
#elif defined __CBM__
	#define SKY 		CYAN
	#define HEALTH_HIGH GREEN
	#define GRND_OFFST	4
#elif defined __ORIC__
	#define SKY 		BLACK
	#define HEALTH_HIGH MGREEN
	#define GRND_OFFST	5
#elif defined __LYNX__
	#define SKY 		LBLUE
	#define HEALTH_HIGH GREEN
	#define GRND_OFFST	7
#endif

// Grub structure
typedef struct {
	unsigned int x, y;
	signed char zAcc;
	unsigned int wAng;
	unsigned char health, index, state;
	unsigned char keyFrame, motionFrame, numFrames;
	unsigned char weapFrame;
} Grub;

// Projectile structure
typedef struct {
	signed int x, y;
	signed char xVel, yVel;
	unsigned char index, owner, state;
} Proj;

clock_t gameClock, projClock;
unsigned char curGrub = 0;
Grub grubs[MAX_GRUBS];
Proj projs[MAX_PROJS];

unsigned int xStart[4] = { 60, 120, 180, 240 };
//unsigned int xStart[4] = { 100, 120, 140, 160 };
unsigned char *names[4] = { "SAM", "JOE", "TOM", "LUC" };

// Sprite definitions
#define spriteFrames 16
#if defined __APPLE2__
	#define spriteCols   7
	#define spriteRows   13
	unsigned char spriteColors[] = { };	 // Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	#define spriteCols   8
	#define spriteRows   13
	unsigned char spriteColors[] = { 0x2a, 0x2a, 0x2a, 0x2a, 0x14, 0x10, 0x10, 0x10, 0x10, 0x10};
#elif defined __ORIC__
	#define spriteCols   12
	#define spriteRows   13
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };  // AIC color for faster drawing!
#elif defined __CBM__
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { 0, 0, 0, 0, PINK, PINK, PINK, PINK, RED, WHITE};
#elif defined __LYNX__
	#define spriteCols   7
	#define spriteRows   11
	unsigned char spriteColors[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }; // Default palette	
#endif

// Effect masks
unsigned char exploMask[15][9] = {{0, 0, 0, 0, 1, 0, 0, 0, 0},
								  {0, 0, 1, 1, 1, 1, 1, 0, 0},
								  {0, 0, 1, 1, 1, 1, 1, 0, 0},
								  {0, 1, 1, 1, 1, 1, 1, 1, 0},
								  {0, 1, 1, 1, 1, 1, 1, 1, 0},
								  {0, 1, 1, 1, 1, 1, 1, 1, 0},
								  {0, 1, 1, 1, 1, 1, 1, 1, 0},
								  {1, 1, 1, 1, 1, 1, 1, 1, 1},
								  {0, 1, 1, 1, 1, 1, 1, 1, 0},
								  {0, 1, 1, 1, 1, 1, 1, 1, 0},
								  {0, 1, 1, 1, 1, 1, 1, 1, 0},
								  {0, 1, 1, 1, 1, 1, 1, 1, 0},
								  {0, 0, 1, 1, 1, 1, 1, 0, 0},
								  {0, 0, 1, 1, 1, 1, 1, 0, 0},
								  {0, 0, 0, 0, 1, 0, 0, 0, 0}};

// Function for printing health
void PrintHealth(unsigned char i)
{
	unsigned char buffer[4];
	if (grubs[i].health) {
		inkColor = HEALTH_LOW; paperColor = BLACK;		
		if (grubs[i].health > 33) { inkColor = HEALTH_MED; }
		if (grubs[i].health > 66) { inkColor = HEALTH_HIGH; }
		sprintf(&buffer[0], "%i", grubs[i].health);
		PrintStr((i+1)*8+4, CHR_ROWS-1, "   ");	
		PrintStr((i+1)*8+4, CHR_ROWS-1, &buffer[0]);	
		inkColor = WHITE; paperColor = BLACK;
	} else {
		inkColor = WHITE; paperColor = BLACK;
		PrintStr((i+1)*8+4, CHR_ROWS-1, "---");	
	}
}

// Setup initial condition of Grubs
void InitGrubs()
{
	unsigned char i;
	Grub *grub;
	
	// Set initial position and state
	inkColor = WHITE; paperColor = BLACK;
	for (i=0; i<MAX_GRUBS; i++) {
		// Set state
		grub = &grubs[i];
		grub->x = xStart[i];
	#if defined __CBM__				
		grub->index = 4+i;	// Using sprites 0-3 for weapons/missiles, 4-7 for players
	#else
		grub->index = i;
	#endif	
		grub->state = 0;
		grub->health = 100;
		grub->wAng = 180;
		grub->keyFrame = 0;
		grub->numFrames = 1;
		
		// Drop grubs on the ground
		LocatePixel(grub->x, grub->y+GRND_OFFST);
		while (GetPixel() == SKY) { 
			grub->y++; 
			LocatePixel(grub->x, grub->y+GRND_OFFST);
		}
		LocateSprite(grub->x, grub->y);
		SetSprite(grub->index, 0);
		EnableSprite(grub->index);
	#if defined __ATARI__
		SetSprite(5+grub->index, 16);	// Frame 0-3 shows body colour, Frame 5-8 shows outline of body
		EnableSprite(5+grub->index);
	#endif		
		
		// Print name/health
		PrintStr((i+1)*8+0, CHR_ROWS-1, names[i]);
		PrintStr((i+1)*8+3, CHR_ROWS-1, ":");
		PrintHealth(i);
	}		
}

// Select next available Grub
void NextGrub()
{
	while (1) {
		curGrub += 1; 
		if (curGrub>=MAX_GRUBS) { curGrub = 0; }
		if (grubs[curGrub].health) { return; }
	}
}

// Update Grub sprite
void DrawGrub(Grub *grub)
{
	// Set grub sprite and weapon sprite (if needed)
	LocateSprite(grub->x, grub->y);
	if (grub->state & STATE_WEAP) {
	#if defined __CBM__
		SetSprite(grub->index, grub->keyFrame+grub->motionFrame);	
		SetSprite(0, grub->weapFrame);
		EnableSprite(0);
	#else
		SetSprite(grub->index, grub->weapFrame);	
	  #if defined __ATARI__
		SetSprite(grub->index+5, grub->weapFrame+16);
	  #endif	
	#endif	
	} else {
		SetSprite(grub->index, grub->keyFrame+grub->motionFrame);	
	#if defined __ATARI__
		SetSprite(grub->index+5, grub->keyFrame+grub->motionFrame+16);
	#elif defined __CBM__
		DisableSprite(0);
	#endif	
	}
}
	
// Process Grub motion
void ProcessGrub(Grub *grub, signed char xMove, signed char yMove)
{
	signed char yOffset;
#if (defined __APPLE2__) || (defined __ORIC__)
	unsigned char i, collisions;
#endif	
	// Process motion
	grub->x += xMove; yOffset = yMove;
	LocatePixel(grub->x, grub->y+yOffset+GRND_OFFST);
	paperColor = GetPixel();
	while (yOffset < grub->zAcc && paperColor == SKY) { 
		yOffset++; 
		LocatePixel(grub->x, grub->y+yOffset+GRND_OFFST);
		paperColor = GetPixel();
	}
	if (!xMove & !yOffset) {
		grub->state &= ~STATE_MOVE;		
	} else {
		grub->state |= STATE_MOVE;
		if (yOffset == yMove) { grub->x -= xMove; }
						 else { grub->y += yOffset; }
	}
	
	// Keep within the screen
	if (grub->x < 2) { grub->x = 2; }
	if (grub->y < 2) { grub->y = 2; }
	if (grub->x > 317) { grub->x = 317; }
	if (grub->y > 197) { grub->y = 197; }
	
	// Still jumping?
	if ((grub->state & STATE_JUMP) && paperColor != SKY) { 
		grub->state &= ~STATE_JUMP; 
		grub->numFrames = NUMFRAME_MOVE;
		if (grub->keyFrame == KEYFRAME_JLEFT) { grub->keyFrame = KEYFRAME_MLEFT; } 
										 else { grub->keyFrame = KEYFRAME_MRIGHT; }
	}
	
	// Process gravity
	grub->zAcc += 1;
	if (grub->zAcc > 5) { grub->zAcc = 5; }
	
	// Enable weapon?
	grub->state &= ~STATE_WEAP;
	if (!(grub->state & STATE_PHYS)) {
		if (!(grub->state & STATE_JUMP)) {
			if (!(grub->state & STATE_MOVE)) {
				grub->state |= STATE_WEAP;
				if (grub->keyFrame == KEYFRAME_MLEFT) {
					if (grub->wAng < 160) {
						grub->weapFrame = KEYFRAME_WEAP+0;
					} else if (grub->wAng > 200) {
						grub->weapFrame = KEYFRAME_WEAP+2;
					} else {
						grub->weapFrame = KEYFRAME_WEAP+1;
					}
				} else {
					if (grub->wAng < 160) {
						grub->weapFrame = KEYFRAME_WEAP+3;
					} else if (grub->wAng > 200) {
						grub->weapFrame = KEYFRAME_WEAP+5;
					} else {
						grub->weapFrame = KEYFRAME_WEAP+4;
					}
				}				
			}
		}
	}

	// Compute sprite position and frame
	grub->motionFrame = (grub->motionFrame+1) % grub->numFrames;
	DrawGrub(grub);
	
#if (defined __APPLE2__) || (defined __ORIC__)
	// Manually redraw colliding sprites
	collisions = COLLISIONS(grub->index);
	for (i=0; i<MAX_GRUBS; i++) {
		if (i != grub->index && COLLIDING(collisions, i)) {
			DrawGrub(&grubs[i]);
		}
	}
#endif
}

// Process input for selected Grub
void ProcessControls(Grub *grub, Proj *proj)
{
	unsigned char joy;
	signed char xMove = 0;
	
	// Check joystick
	joy = GetJoy(0);
	grub->numFrames = 1;
	if (!(joy & JOY_BTN1)) {
		if (!(grub->state & STATE_JUMP))  {
			// Set grub and projectile state
			grub->state |= STATE_LOAD;
			proj->state = PROJ_LOAD;
			proj->owner = grub->index;
			
			// Adjust position of projectile
			if (!(joy & JOY_UP))  { 
				grub->wAng -= 4;
				if (grub->wAng < 135) { grub->wAng = 135; }
			} 
			if (!(joy & JOY_DOWN))  { 
				grub->wAng += 4;
				if (grub->wAng > 260) { grub->wAng = 260; }
			}
			if (!(joy & JOY_LEFT))  { grub->keyFrame = KEYFRAME_MLEFT; }
			if (!(joy & JOY_RIGHT)) { grub->keyFrame = KEYFRAME_MRIGHT; }		
			
			// Move projectile around Grub
			if (grub->keyFrame == KEYFRAME_MLEFT) { 
				proj->x = grub->x+(5*_cos(grub->wAng))/128;
			} else { 
				proj->x = grub->x-(5*_cos(grub->wAng))/128; 
			}
			proj->y = grub->y + (5*_sin(grub->wAng))/128 + 2;
		}
	} else {
		// Release projectile?
		if (grub->state & STATE_LOAD) {
			grub->state &= ~STATE_LOAD;
			proj->state = PROJ_MOVE;
			if (grub->keyFrame == KEYFRAME_MLEFT) { 
				proj->xVel =  (4*_cos(grub->wAng))/128;
			} else { 
				proj->xVel = -(4*_cos(grub->wAng))/128; 
			}
			proj->yVel = (4*_sin(grub->wAng))/128;
			PlaySFX(SFX_BUMP, 64, 60, 2);
		}
		
		// Process motion
		if (!(joy & JOY_UP) && !(grub->state & STATE_JUMP)) {
			PlaySFX(SFX_BUMP, 32, 120, 2);
			grub->zAcc = -3;
			grub->state |= STATE_JUMP; 
			grub->numFrames = NUMFRAME_JUMP;
			if (grub->keyFrame == KEYFRAME_MLEFT) { grub->keyFrame = KEYFRAME_JLEFT; } 
											 else { grub->keyFrame = KEYFRAME_JRIGHT; }
		}				
		if (!(joy & JOY_LEFT))  { 
			xMove = -1;
			if (grub->state & STATE_JUMP) { grub->keyFrame = KEYFRAME_JLEFT; grub->numFrames = NUMFRAME_JUMP; }
									 else { grub->keyFrame = KEYFRAME_MLEFT; grub->numFrames = NUMFRAME_MOVE; }
		}
		if (!(joy & JOY_RIGHT)) { 
			xMove = 1; 
			if (grub->state & STATE_JUMP) { grub->keyFrame = KEYFRAME_JRIGHT; grub->numFrames = NUMFRAME_JUMP; }
									 else { grub->keyFrame = KEYFRAME_MRIGHT; grub->numFrames = NUMFRAME_MOVE; }
		}
	}	
	
	// Now process the Grub
	ProcessGrub(grub, xMove, -4);
}

// Display explosion and process all grubs
void Explosion(unsigned int x, unsigned int y)
{
	unsigned char i,j,pass,color;
	signed int xOff, yOff;
	unsigned int sqDist;
	Grub *grub;
	
	// Explode in 2 steps
	PlaySFX(SFX_BUMP, 32, 120, 2);
	for (pass=0; pass<2; pass++) {
		// 2 passes: yellow then sky color
		if (!pass) { color = EXPLODE; } else { color = SKY; }
		
		// Scan horizontally (safely)
		for (j=0; j<15; j++) {
			yOff = y+j-7;
			if (yOff > 191) { 
				break; 
			} else if (yOff > 0) {
				// Scan horizontally (safely)
				for (i=0; i<9; i++) {
					xOff = x+2*i-8;
					if (xOff > 319) { 
						break; 
					} else if (xOff > 0) { 
						// Check explosion mask
						if (exploMask[j][i]) {
							LocatePixel(xOff, yOff);
							SetPixel(color);
						}
					}
				}
			}
		}
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
	}
	
	// Process damage
	for (i=0; i<MAX_GRUBS; i++) {
		grub = &grubs[i];
		if (grub->health) {
			xOff = grub->x; yOff = grub->y;
			sqDist = ((xOff-x)*(xOff-x)+(yOff-y)*(yOff-y));
			if (sqDist<162) {
				if (grub->health > 34) { grub->health -= 34; } else {grub->health = 0; }
				if (!grub->health) {
					grub->keyFrame = KEYFRAME_DEAD;
					grub->numFrames = 1;
				}
				PrintHealth(i);
			}
		}
	}
	
	// Update all grubs until they stop moving
	pass = 1;
	while (pass) {
		pass = 0;
		for (i=0; i<MAX_GRUBS; i++) {
			grub = &grubs[i];
			grub->state |= STATE_PHYS;
			ProcessGrub(grub, 0, 0);
			pass |= (grub->state & STATE_MOVE);
			grub->state &= ~STATE_PHYS;
		}
		while (clock()-gameClock < 8) { 
		#if defined __APPLE2__
			clk += 1;  // Manually update clock on Apple 2
		#elif defined __LYNX__
			UpdateDisplay(); // Refresh Lynx screen
		#endif
		}
		gameClock = clock();		
	}
}

// Setup initial condition of projectiles
void InitProjs()
{
	unsigned char i;
	for (i=0; i<MAX_PROJS; i++) {
		// Set sprite index of proj.
	#if defined __CBM__		
		projs[i].index = 1+i;	// Lower index for drawing priority (weap/proj on top of grubs)
	#else
		projs[i].index = 4+i;
	#endif
	}
}

// Update the position of a projectile
void ProcessProj(Proj *proj)
{
	unsigned int x, y;
	signed char steps, fraction = 0;
	switch (proj->state) {
		case PROJ_NULL:
			break;
		case PROJ_LOAD:
			LocateSprite(proj->x, proj->y);
			SetSprite(proj->index, KEYFRAME_PROJ);		
			EnableSprite(proj->index);
		#if defined __ATARI__ 	// Assign sprite for second color
			SetSprite(proj->index+5, KEYFRAME_PROJ+16);		
			EnableSprite(proj->index+5);
		#endif
			break;
		case PROJ_MOVE:
			if (abs(proj->xVel) > abs(proj->yVel)) { steps = abs(proj->xVel); } 
											  else { steps = abs(proj->yVel); }
			while (fraction<=steps) {
				// Move one step
				x = proj->x + (proj->xVel*fraction)/steps;
				y = proj->y + (proj->yVel*fraction)/steps;
				
				// Gone too far left/right/down?
				if (x < 1 || x > 318 || y > 191) {
					proj->state = PROJ_NULL;
					DisableSprite(proj->index);
				#if defined __ATARI__ 
					DisableSprite(5+proj->index);
				#endif
					NextGrub();
					return;
				}
				// Disappeared at top of screen?
				if (y < 1) {
					DisableSprite(proj->index);
				#if defined __ATARI__ 
					DisableSprite(5+proj->index);
				#endif
					continue;
				}
				// Check if we hit the ground or another grub
				LocatePixel(x, y);
				if (GetPixel() != SKY) { // || (COLLISIONS(proj->index) && !COLLISIONS(proj->owner))) {
					// Destroy proj. and process explosion
					proj->state = PROJ_NULL;
					DisableSprite(proj->index);
				#if defined __ATARI__ 
					DisableSprite(5+proj->index);
				#endif
					Explosion(x, y);
					NextGrub();
					return;					
				} else {
					// Move proj. to next trajectory point
					LocateSprite(x, y);
					SetSprite(proj->index, KEYFRAME_PROJ);
					EnableSprite(proj->index);
				#if defined __ATARI__ 
					SetSprite(5+proj->index, KEYFRAME_PROJ+16);		
					EnableSprite(5+proj->index);
				#endif
				}
				fraction++;
			}
			// Update proj.
			proj->x = x;
			proj->y = y;
			proj->yVel++;
	}
}

void SplashScreen(void)
{
	// Load and show banner
	ExitBitmapMode();
	LoadBitmap("banner.img");
	EnterBitmapMode();
	
	// Show credit/build
	paperColor = SKY; 
	inkColor = WHITE; 
	PrintStr(CHR_COLS-12, CHR_ROWS-4, "TECH DEMO");		
	PrintStr(CHR_COLS-13, CHR_ROWS-3, "BY 8BIT-DUDE");		
	PrintStr(CHR_COLS-12, CHR_ROWS-2,  "2019/09/18");

	// Wait until key is pressed
	while (!kbhit()) {	
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif
	}	
}

int main(void) 
{
	Grub *grub;
	Proj *proj;
	
	// Reset screen
	clrscr();
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);

	// Initialize sfx/bitmap
	InitBitmap();
	
	// Setup sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteFrames, spriteCols, spriteRows, spriteColors);	
	
	// Show splash screen
	SplashScreen();
		
	// Load and show playfield
	ExitBitmapMode();
	LoadBitmap("pumpkins.img");
	EnterBitmapMode();

	// Setup sound
	InitSFX();
	
	// Setup grubs/projectiles
	InitGrubs();
	InitProjs();
	
	// Run game indefinetely
	gameClock = clock();	
	projClock = clock();	
	while (1) {
	#if defined __APPLE2__
		clk += 1;  // Manually update clock on Apple 2
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif
		// Process projectiles?
		if (clock()-projClock > 2) {
			projClock = clock();
			ProcessProj(&projs[0]);
		}			
		
		// Process input on selected grub?
		if (clock()-gameClock > (TCK_PER_SEC/10)) {
			gameClock = clock();
			grub = &grubs[curGrub];
			proj = &projs[0];
			ProcessControls(grub, proj);
		}
	}
	
	// Black-out screen and stop SFX
	DisableSprite(-1);	// "-1" disables all sprites
	ExitBitmapMode();
	StopSFX();
		
    // Done
    return EXIT_SUCCESS;	
}


#include <cc65.h>
#include "unity.h"
#include "definitions.h"

clock_t gameClock, projClock;
unsigned char curGrub = 0;
Grub grubs[MAX_GRUBS];
Proj projs[MAX_PROJS];

unsigned int xStart[4] = { 60, 120, 180, 240 };
//unsigned int xStart[4] = { 100, 120, 140, 160 };
unsigned char *names[4] = { "SAM", "JOE", "TOM", "LUC" };

// Sprite definitions
#if defined __APPLE2__
	unsigned char spriteColors[] = { };	 // Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	unsigned char spriteColors[] = { 0x2a, 0x10, 0x2a, 0x10, 0x2a, 0x10, 0x2a, 0x10, 0x0e, 0x14, 0x00, 0x00};
#elif defined __CBM__
	unsigned char spriteColors[] = { 0, 0, 0, 0, PINK, PINK, PINK, PINK, RED, WHITE};
#elif defined __LYNX__
	unsigned char spriteColors[] = 0;  //  All sprites use the default palette
#elif defined __NES__
  const unsigned char spriteColors[] = { SPR_VOID, SPR_BLUE,   SPR_CYAN, SPR_GREY,  
										 SPR_VOID, SPR_RED,    SPR_CYAN, SPR_GREY, 
										 SPR_VOID, SPR_GREEN,  SPR_CYAN, SPR_GREY, 
										 SPR_VOID, SPR_YELLOW, SPR_CYAN, SPR_GREY }; // 4 palettes of 3 colors (1st color is unused)
#elif defined __ORIC__
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };  // AIC color for faster drawing!
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
	txtX = (i+1)*8+4; txtY = TXT_ROWS-1;
	if (grubs[i].health) {
		inkColor = HEALTH_LOW; paperColor = BLACK;		
		if (grubs[i].health > 33) { inkColor = HEALTH_MED; }
		if (grubs[i].health > 66) { inkColor = HEALTH_HIGH; }
		PrintStr("   ");	
		PrintNum(grubs[i].health);
		inkColor = WHITE; paperColor = BLACK;
	} else {
		inkColor = WHITE; paperColor = BLACK;
		PrintStr("---");	
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
	#if defined __ATARI__
		SetMultiColorSprite(2*grub->index, 0);
		EnableMultiColorSprite(2*grub->index);
	#else
		SetSprite(grub->index, 0);
		EnableSprite(grub->index);
	#endif		
		
		// Print name/health
		txtX = (i+1)*8; txtY = TXT_ROWS-1;
		PrintStr(names[i]); txtX+=3;
		PrintStr(":");
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
	#elif defined __ATARI__
		SetMultiColorSprite(2*grub->index, grub->weapFrame);
	#else
		SetSprite(grub->index, grub->weapFrame);	
	#endif	
	} else {
	#if defined __CBM__
		SetSprite(grub->index, grub->keyFrame+grub->motionFrame);	
		DisableSprite(0);
	#elif defined __ATARI__
		SetMultiColorSprite(2*grub->index, grub->keyFrame+grub->motionFrame);
	#else
		SetSprite(grub->index, grub->keyFrame+grub->motionFrame);	
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
	PlaySFX(SFX_BUMP, 8, 120, 2);
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
		#if defined __ATARI__ 
			SetMultiColorSprite(2*proj->index, KEYFRAME_PROJ);
			EnableMultiColorSprite(2*proj->index);
		#else
			SetSprite(proj->index, KEYFRAME_PROJ);
			EnableSprite(proj->index);
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
				#if defined __ATARI__ 
					DisableMultiColorSprite(2*proj->index);
				#else
					DisableSprite(proj->index);
				#endif
					NextGrub();
					return;
				}
				// Disappeared at top of screen?
				if (y < 1) {
				#if defined __ATARI__ 
					DisableMultiColorSprite(2*proj->index);
				#else
					DisableSprite(proj->index);
				#endif
					continue;
				}
				// Check if we hit the ground or another grub
				LocatePixel(x, y);
				if (GetPixel() != SKY) { // || (COLLISIONS(proj->index) && !COLLISIONS(proj->owner))) {
					// Destroy proj. and process explosion
					proj->state = PROJ_NULL;
				#if defined __ATARI__ 
					DisableMultiColorSprite(2*proj->index);
				#else
					DisableSprite(proj->index);
				#endif
					Explosion(x, y);
					NextGrub();
					return;					
				} else {
					// Move proj. to next trajectory point
					LocateSprite(x, y);
				#if defined __ATARI__ 
					SetMultiColorSprite(2*proj->index, KEYFRAME_PROJ);
					EnableMultiColorSprite(2*proj->index);				
				#else
					SetSprite(proj->index, KEYFRAME_PROJ);
					EnableSprite(proj->index);
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
	LoadBitmap("banner.img");
	ShowBitmap();
	
	// Show credit/build
	paperColor = SKY; 
	inkColor = WHITE; 
	txtX = TXT_COLS-13; txtY = TXT_ROWS-4; 
	PrintStr(" TECH DEMO"); txtY++;
	PrintStr("BY 8BIT-DUDE"); txtY++;
	PrintStr(" 2021/04/18");

	// Wait until key is pressed
	while (!kbhit()) {	
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif
	}	
	
	// Exit bitmap mode again
	HideBitmap();
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
	LoadSprites("sprites.dat", spriteColors);	
	
	// Show splash screen
	SplashScreen();
		
	// Load and show playfield
	LoadBitmap("pumpkins.img");
	ShowBitmap();

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
	HideBitmap();
	StopSFX();
		
    // Done
    return EXIT_SUCCESS;	
}

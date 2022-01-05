
#include "definitions.h"

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

// Sprite definitions
#if defined __APPLE2__	
	unsigned char spriteColors[] = { };  // Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	unsigned char spriteColors[] = { 0x0e, 0x80, 0x10, 0x2a, 0xba, 0x00, 0x00, 0x00, 0x00, 0x00 };  // Refer to atari palette in docs
#elif defined __ORIC__
	unsigned char spriteColors[] = { SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC, SPR_AIC };  // AIC color allows faster drawing!
	unsigned char multiColorDef[] = { SPR_WHITE, 8, SPR_GREEN, 14, SPR_MAGENTA, 24 };	// Multicolor definition { color, row, ... }
#elif defined __CBM__
	unsigned char spriteColors[] = { WHITE, BLUE, BROWN, PINK, GREEN, WHITE, WHITE, WHITE, BLACK, WHITE };	// 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	unsigned char *spriteColors = 0; //  All sprites use the default palette
#elif defined __NES__
    const unsigned char spriteColors[] = { SPR_VOID, SPR_BLACK, SPR_GREEN, SPR_YELLOW, 
										   SPR_VOID, SPR_BLACK, SPR_GREEN, SPR_YELLOW, 
										   SPR_VOID, SPR_VOID, SPR_VOID, SPR_VOID, 
										   SPR_VOID, SPR_VOID, SPR_VOID, SPR_VOID }; // 4 palettes of 3 colors (1st color is unused)
#endif

// Accessible polygon in scene
extern signed int polygonX[];
extern signed int polygonY[];

// See scene.c
extern Interact interacts[MAX_INTERACT];

// Game state variables
unsigned char *mouse, mouseL = 0, mouseAction = 0, intersect;
unsigned int mouseX = 160, mouseY = 100, clickX = 160, clickY = 100;
unsigned int goalX = 180, goalY = 130, unitX = 180, unitY = 130;	
unsigned char sceneSearch, sceneIndex = 255, sceneInteract = 255, sceneItem = 255;
unsigned char unitFrame = frameWaitLeft, waitFrame = frameWaitLeft;

void GameLoop(void)
{
	signed int interX, interY, deltaX, deltaY;
	clock_t gameClock = clock();
	
	while (1) {
	#if defined __APPLE2__
		clk += 1;  // Manually update clock on Apple 2
	#endif		
		// Get mouse state
		mouse = GetMouse();
		if (!(mouse[2] & MOU_LEFT)) {
			mouseL = 1;
		} else {
			mouseL = 0; mouseAction = 0;
		}
		mouseX = 2*mouse[0];
		mouseY = mouse[1];
		
		// Update mouse pointer
		DrawPointer(mouseX, mouseY, mouseL);
		
		// Search scene
		if (!(mouse[2] & MOU_MOTION)) {
			sceneSearch = SearchScene(mouseX, mouseY);			
			if (sceneSearch != sceneIndex) {
				sceneIndex = sceneSearch;
				if (sceneIndex == 255) {
					txtX = 0; txtY = TXT_ROWS-2;
					PrintBlanks(TXT_COLS-8, 1);
				} else {
					PrintInteract(sceneItem, interacts[sceneIndex].label);
				}
			}
		}
		
		// Process commands
		if (mouseL && !mouseAction) {
	 		// Record action
			mouseAction = 1;
			PlaySFX(SFX_BUMP, 32, 120, 2);
					
			// Is mouse cursor in inventory area?
			if (mouseY > INVENTORY_Y) {
				sceneItem = SelectItem(mouseX, mouseY);
				if (sceneItem != 255) {
					PrintInteract(sceneItem, 0);
				} else {
					txtX = 0; txtY = TXT_ROWS-2;
					PrintBlanks(TXT_COLS-8, 2);
				}
			} else {
				// Get click coordinates
				if (sceneIndex == 255) {
					clickX = mouseX;
					clickY = mouseY;
				} else { 
					clickX = interacts[sceneIndex].cx;
					clickY = interacts[sceneIndex].cy;
				}
			
				// Compute goal coordinates
				intersect = IntersectSegmentPolygon(unitX, unitY, clickX, clickY, MAX_POLYGON, polygonX, polygonY, &interX, &interY);
				if (intersect && (unitX != interX || unitY != interY)) { 	// Check that we are not stuck on a polygon segment
					goalX = interX;
					goalY = interY;
				} else {
					// Move directly to mouse cursor (if in allowed area, and not crossing other parts of polygon)
					if (intersect < 2 && PointInsidePolygon(clickX, clickY, MAX_POLYGON, polygonX, polygonY)) {
						goalX = clickX;
						goalY = clickY;
					}
				}
				
				// Save index, for interaction when we reach position
				sceneInteract = sceneIndex;
			}
		}
				
		// Process unit motion
		if (clock()>gameClock+unitTicks) {
			gameClock = clock();
			deltaX = goalX - unitX;
			deltaY = goalY - unitY;
			if (deltaX || deltaY) {
				// Move in steps of 3 max.
				unitX += SIGN(deltaX) * MIN(ABS(deltaX), unitStep); 
				unitY += SIGN(deltaY) * MIN(ABS(deltaY), unitStep);
				if (unitX > 320) unitX = 0;
				if (unitY > 200) unitY = 0;
				
				// Update frame number
				if (deltaX > 0) {
					if (unitFrame < frameWalkRightBeg) unitFrame = frameWalkRightBeg;
					unitFrame += 1; if (unitFrame > frameWalkRightEnd) unitFrame = frameWalkRightBeg;
					waitFrame = frameWaitRight;
				} else {
					if (unitFrame > frameWalkLeftEnd) { unitFrame = frameWalkLeftBeg; }
					unitFrame += 1; if (unitFrame > frameWalkLeftEnd) unitFrame = frameWalkLeftBeg;
					waitFrame = frameWaitLeft;
				}
			} else {
				// Process trigger (if any) and set wait frame
				if (sceneInteract != 255) {
					if (ProcessInteract(sceneInteract, sceneItem)) return;
					sceneInteract = 255;
					sceneItem = 255;
				}
				unitFrame = waitFrame;
			}
			DrawUnit(unitX, unitY, unitFrame);
		}
	}	
}

int main(void) 
{
	// Set text mode colors
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	
	// Initialize sfx/bitmap
	InitSFX();
	InitBitmap();
	
	// Setup sprites
	LoadSprites("sprites.dat");
	SetupSprites(spriteColors);
	
	// Enable sprites
	EnableSprite(0);  // Mouse cursor
	EnableSprite(1);  // Unit #1
#if defined __APPLE2__
	CropSprite(0,8); // Mouse cursor only occupies third of frame
#elif defined __ATARI__
	EnableSprite(2);  // Unit #1 (extra color)
	EnableSprite(3);  // Unit #1 (extra color)
	EnableSprite(4);  // Unit #1 (extra color)
#elif defined __ORIC__
	CropSprite(0,8);  // Mouse cursor only occupies half frame
	MultiColorSprite(1, multiColorDef);	// Make sprite 1 multi-colored
#elif defined __CBM__ 
	EnableSprite(2);  // Unit #1 (extra color)	
	EnableSprite(3);  // Unit #1 (extra color)
	EnableSprite(4);  // Unit #1 (extra color)
	DoubleHeightSprite(1, 1);
	DoubleHeightSprite(2, 1);
	DoubleHeightSprite(3, 1);
	DoubleHeightSprite(4, 1);
#endif	

	// Load music
	LoadMusic("goblin.mus");
	
	// Show splash screen
	SplashScreen();

	// Prepare scene
	LoadScene("scene01");
	
	// Run game loop
	PlayMusic();
	GameLoop();
	StopMusic();
	
	// Black-out screen and clear key
	DisableSprite(-1);	// "-1" disables all sprites
	HideBitmap();
	StopSFX();
	
    // Done
    return EXIT_SUCCESS;	
}

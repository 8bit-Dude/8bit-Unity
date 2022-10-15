
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

// Fast tables for cos/sin/tan
const signed char cos[16] = {16,14,11,6,0,-6,-11,-14,-16,-14,-11,-6,0,6,11,14};
const signed char sin[16] = {0,6,11,14,16,14,11,6,0,-6,-11,-14,-16,-14,-11,-6};
const signed char tan[19] = {91,30,17,11,8,6,4,2,1,-1,-2,-4,-6,-8,-11,-17,-30,-91,-128};

// Fast atan2fast routine with 36 x 10 degs increments
unsigned char atan2fast(signed int dY, signed int dX)
{
	unsigned char j = 0;
	signed char tn;
	
	if (dY == 0) {
		 // Select value directly (Cannot divide by 0)
		if (dX >= 0)
			return 0;
		else
			return 8;
	} else {
		// Use simple division and check range
		dX = (8*dX)/dY;
		if (dX < -128)
			tn = -128;
		else 
		if (dX > 127)
			tn = 127;
		else
			tn = dX;
			
		// Check against reference table
		while (tn < tan[j])
			j++;
		
		// Compute angle
		dX = j*10;
		if (dY < 0)
			dX += 180;
		return (dX/23u);
	}	
}


// Accessible polygon in scene
extern unsigned char num_polygon;
extern signed int polygonX[];
extern signed int polygonY[];

// See scene.c
extern unsigned char itemLast, gameItems[], strings[];
extern Interact interacts[];

// Game state variables
unsigned int unitX = 0, unitY = 0, wayX[MAX_WAYPOINTS], wayY[MAX_WAYPOINTS];	
unsigned int mouseX = 160, mouseY = 100, clickX = 160, clickY = 100;
unsigned char numWay, *mouse, mouseL = 0, mouseAction = 0;
unsigned char sceneSearch = 0, sceneIndex = 255, sceneInteract = 255, sceneItem = 255;
unsigned char unitFrame = frameWaitLeft, waitFrame = frameWaitLeft;

void ProcessWaypoint(void)
{
	// Home-in on waypoint
	unitX = wayX[0];
	unitY = wayY[0];				
	numWay--;
	
	// Shift down remaining waypoints
	if (numWay) {
		memcpy(&wayX[0], &wayX[1], MAX_WAYPOINTS-1);
		memcpy(&wayY[0], &wayY[1], MAX_WAYPOINTS-1);						
	}	
}					

void GameLoop(void)
{
	clock_t gameClock = clock();
	unsigned char angle, i, j, interI[4], interN, *scene;
	signed int deltaX, deltaY, interX[4], interY[4], interD[4], tmp;
	
	while (1) {
	#if defined __APPLE2__
		clk += 2;  // Manually update clock on Apple 2
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
			if (mouseY <= INVENTORY_Y) {
				sceneSearch = SearchScene(mouseX-4, mouseY-4);	
			} else {
				sceneSearch = 255;
			}
			if (sceneSearch != sceneIndex) {
				sceneIndex = sceneSearch;
				PrintInteract(sceneItem, sceneIndex);
			}
		}
		
		// Process commands
		if (mouseL && !mouseAction) {
	 		// Record action
			mouseAction = 1;
			PlaySFX(SFX_BUMP, 32, 120, 2);
			
			// Reset navigation
			sceneInteract = 255;
			numWay = 0;
					
			// Is mouse cursor in inventory area?
			if (mouseY > INVENTORY_Y) {
				sceneItem = SelectItem(mouseX, mouseY);
				PrintInteract(sceneItem, 255);
				
			} else {
				// Save current interact index
				sceneInteract = sceneIndex;
				
				// Get click coordinates
				if (sceneInteract == 255) {
					clickX = mouseX;
					clickY = mouseY;
					if (sceneItem != 255) {
						sceneItem = 255;
						PrintInteract(sceneItem, sceneIndex);
					}
				} else { 
					clickX = interacts[sceneInteract].cx;
					clickY = interacts[sceneInteract].cy;
				}
			
				// Compute goal coordinates
				if (!unitX && !unitY) {
					// Unit is disabled: DO NOTHING!				
				} else {					
					// Check intersections with polygon
					interN = IntersectSegmentPolygon(unitX, unitY, clickX, clickY, num_polygon, polygonX, polygonY, interI, interX, interY);
					if (interN) {
						// Compute distance to unit
						for (i=0; i<interN; i++) {
							interD[i] = abs(unitX - interX[i]) + abs(unitY - interY[i]); 
						}
						
						// Sort intersections by distance
						for (i=0; i<interN; ++i) { 
							for (j=i+1; j<interN; ++j) { 
								if (interD[i] > interD[j]) {
									tmp = interI[i];  interI[i] = interI[j]; interI[j] = tmp;
									tmp = interX[i];  interX[i] = interX[j]; interX[j] = tmp;
									tmp = interY[i];  interY[i] = interY[j]; interY[j] = tmp;
									tmp = interD[i];  interD[i] = interD[j]; interD[j] = tmp;
								}
							}				
						}						
						
						// Add waypoints						
						tmp = 1;
						for (i=0; i<interN; i++) {
							
							// Move around the polygon edge
							if (!tmp) {
								j = interI[i-1];
								if (interI[i] > interI[i-1]) {
									while (j<interI[i]) {
										wayX[numWay] = polygonX[j];
										wayY[numWay] = polygonY[j];
										numWay++; j++; 
										if (numWay > (MAX_WAYPOINTS-2))
											break;
									} 
								} else {
									while (j>interI[i]) {
										wayX[numWay] = polygonX[j-1];
										wayY[numWay] = polygonY[j-1];
										numWay++; j--; 
										if (numWay > (MAX_WAYPOINTS-2))
											break;
									} 											
								}
							}
							tmp = !tmp;
							
							// Move to intersection point
							wayX[numWay] = interX[i];
							wayY[numWay] = interY[i];
							numWay++; 
							if (numWay > (MAX_WAYPOINTS-1))
								break;
						}
					}	

					// Then move to mouse cursor (if in allowed area)
					if (PointInsidePolygon(clickX, clickY, num_polygon, polygonX, polygonY)) {
						wayX[numWay] = clickX;
						wayY[numWay] = clickY;
						numWay++;	
					}
				}
			}
		}
				
		// Process unit motion
		if (clock()>gameClock+unitTicks) {
			gameClock = clock();
			
			if (numWay) {			
				// Move in steps of 3 max.
				deltaX = wayX[0] - unitX;
				deltaY = wayY[0] - unitY;
				if (unitStep >= abs(deltaX) && unitStep >= abs(deltaY)) {
					ProcessWaypoint();
					
				} else {
					// Move along vector
					angle = atan2fast(deltaY, deltaX);
					unitX += (unitStep*cos[angle])/16;
					unitY += (unitStep*sin[angle])/16;
					
					// Check that nothing went wrong...
					if (unitX > 320 || unitY > INVENTORY_Y)
						ProcessWaypoint();
				}
				
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
					scene = ProcessInteract(sceneInteract, sceneItem);
					if (scene)
						LoadScene(scene);
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
	clrscr();
	
	// Initialize sfx/bitmap
	InitSFX();
	InitBitmap();
	
	// Setup sprites
	LoadSprites("sprites.dat", spriteColors);
#if defined __APPLE2__
	CropSprite(0,8); // Mouse cursor only occupies third of frame	
#elif defined __ORIC__
	CropSprite(0,8);  // Mouse cursor only occupies half frame
	MultiColorSprite(1, multiColorDef);	// Make sprite 1 multi-colored
#elif defined __CBM__ 
	DoubleHeightSprite(1, 1);
	DoubleHeightSprite(2, 1);
	DoubleHeightSprite(3, 1);
	DoubleHeightSprite(4, 1);
#endif
	
	// Show splash screen
	SplashScreen();

	// Load first scene
	LoadScene("scene01");
	
	//// HACK FOR TESTING SCENES ////
/* 	itemLast = 2;
	strcpy(&gameItems[0], "Sausage");
	strcpy(&gameItems[9], "Bottle");
	LoadScene("scene03"); */
	//// HACK FOR TESTING SCENES ////
	
	// Game loop	
	GameLoop();
	
	// Black-out screen and clear key
	DisableSprite(-1);	// "-1" disables all sprites
	HideBitmap();
	StopMusic();
	StopSFX();
	
    // Done
    return EXIT_SUCCESS;	
}

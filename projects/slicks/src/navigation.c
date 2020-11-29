
#include "definitions.h"

// See game.c
extern unsigned char gameLineUp[4];

// Navigation variables
Vehicle cars[MAX_PLAYERS];
Waypoint ways[MAX_WAYPOINTS];
Ramp ramps[MAX_RAMPS];
unsigned char numWays;
unsigned char numRamps;

// Lineup positions
unsigned int lineupX[MAX_PLAYERS];
unsigned int lineupY[MAX_PLAYERS];
unsigned int lineupAng[MAX_PLAYERS];

// Function to load *.nav files
void LoadNavigation(char *filename)
{
	unsigned char i,n;
#if (defined __ATARI__) || (defined __LYNX__) || (defined __ORIC__)
	// Read entire file contents
  #if defined __ATARI__
	unsigned char buffer[160];
	FileOpen(filename);
	FileRead(buffer, 160);
  #elif defined __ORIC__
	unsigned char buffer[160];
	FileRead(filename, buffer);
  #elif defined __LYNX__
	unsigned char *buffer = (unsigned char*)SHAREDRAM;
	FileRead(filename);
  #endif
	// Read Lineup
	n = 1;
	for (i=0; i<buffer[0]; ++i) {
		// Data is encoded as uint16
		lineupX[i] = PEEKW(&buffer[n]); n += 2;
		lineupY[i] = PEEKW(&buffer[n]); n += 2;
		lineupAng[i] = PEEKW(&buffer[n]); n += 2;
	}	
	
	// Read Navigation Cylinders
	numWays = buffer[n]; n += 1;
	for (i=0; i<numWays; ++i) {
		// Data is encoded as uint16
		ways[i].x = PEEKW(&buffer[n]); n += 2;
		ways[i].y = PEEKW(&buffer[n]); n += 2;
		memcpy(&ways[i].v[0][0], &buffer[n], 8); n += 8;
	#ifdef DEBUG_NAV
		// Display waypoints (debugging)
		LocatePixel(ways[i].x/8u, ways[i].y/8u); SetPixel(BLACK);
        LocatePixel(ways[i].x/8u+ways[i].v[0][0], ways[i].y/8u+ways[i].v[0][1]); SetPixel(BLACK); 
        LocatePixel(ways[i].x/8u+ways[i].v[1][0], ways[i].y/8u+ways[i].v[1][1]); SetPixel(BLACK);         
	#endif		
	}	
	
	// Read Jump Ramps
	numRamps = buffer[n]; n += 1;
	for (i=0; i<numRamps; ++i) {
		// Data is encoded as uint16
		memcpy(&ramps[i].x[0], &buffer[n], 4); n += 4;
		memcpy(&ramps[i].y[0], &buffer[n], 4); n += 4;
	}    
#else	
	// Try to open file
	FILE* fp = fopen(filename, "rb");
	
	// Read Lineup
	fread(&n, 1, 1, fp);
	for (i=0; i<n; ++i) {
		// Data is encoded as uint16
		fread(&lineupX[i], 2, 1, fp);
		fread(&lineupY[i], 2, 1, fp);
		fread(&lineupAng[i], 2, 1, fp);
	}

	// Read Navigation Cylinders
	fread(&numWays, 1, 1, fp);
	for (i=0; i<numWays; ++i) {
		// Data is encoded as uint16
		fread(&ways[i].x, 2, 1, fp);
		fread(&ways[i].y, 2, 1, fp);
		fread(&ways[i].v[0][0], 2, 4, fp);
	#ifdef DEBUG_NAV
		// Display waypoints (debugging)
		LocatePixel(ways[i].x/8u, ways[i].y/8u); SetPixel(BLACK);
        LocatePixel(ways[i].x/8u+ways[i].v[0][0], ways[i].y/8u+ways[i].v[0][1]); SetPixel(BLACK); 
        LocatePixel(ways[i].x/8u+ways[i].v[1][0], ways[i].y/8u+ways[i].v[1][1]); SetPixel(BLACK);         
	#endif	
	}
    
	// Read Jump Ramps
	fread(&numRamps, 1, 1, fp);
	for (i=0; i<numRamps; ++i) {
		// Data is encoded as uint16
		fread(&ramps[i].x[0], 2, 2, fp);
		fread(&ramps[i].y[0], 2, 2, fp);
	}    
	
	// Release file
	fclose(fp);
#endif
}

// Reset cars to Line-up positions 
void ResetLineUp()
{
	unsigned char i,j;
	for (i=0; i<MAX_PLAYERS; ++i) {
		j = gameLineUp[i];
		cars[i].x2 = 8*lineupX[j]; 
		cars[i].y2 = 8*lineupY[j];
		cars[i].ang1 = lineupAng[j];
		cars[i].ang2 = lineupAng[j];
		cars[i].vel = 0;		
        cars[i].way = 0;
        cars[i].lap = -1;
        cars[i].joy = 0;             
	}
}

// Functions to check navigation around cylinders
static int iVec, v1[2], v2[2], v90[2];
unsigned char dx, dy;
Waypoint *way;

char CheckWaypoint(Vehicle *car)
{
	// Initiate variables
	way = &ways[car->way/2u];
	iVec = car->way%2;
	
	// Compute vectors with Waypoint centre
	v1[0] = (car->x1 - way->x)/8;
	v1[1] = (car->y1 - way->y)/8;
	v2[0] = (car->x2 - way->x)/8;
	v2[1] = (car->y2 - way->y)/8;				
		
	// Check dot products against Waypoint vector
	if ( (DOT(v1, way->v[iVec]) > 0) && (DOT(v2, way->v[iVec]) > 0) ) {
		// Compute 90 deg rotated vector
		v90[0] = -way->v[iVec][1];
		v90[1] =  way->v[iVec][0];

		// Check dot products with 90 deg rotated vector
		if ((DOT(v1, v90) >= 0) & (DOT(v2, v90) <= 0)) { return 1; }
		if ((DOT(v1, v90) <= 0) & (DOT(v2, v90) >= 0)) { return 1; }
	}
	return 0;
}

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

int GetWaypointAngle(Vehicle *car)
{
	// Get target
	way = &ways[car->way/2u];
	iVec = car->way%2;
	dx = 128 + (way->x + 8*way->v[iVec][0] - car->x2) / 16u;
	dy = 128 - (way->y + 8*way->v[iVec][1] - car->y2) / 16u;
	return (45*(unsigned int)atan2(dy,dx))/32u;
}

// Function to check ramp logics
Ramp *ramp;
char CheckRamps(Vehicle *car)
{
    // Gotta check them all...
    unsigned char i;
	for (i=0; i<numRamps; ++i) {
        ramp = &ramps[i];
        if (car->x2 < ramp->x[0] & car->x2 < ramp->x[1]) { continue; }
        if (car->x2 > ramp->x[0] & car->x2 > ramp->x[1]) { continue; }
        if (car->y2 < ramp->y[0] & car->y2 < ramp->y[1]) { continue; }
        if (car->y2 > ramp->y[0] & car->y2 > ramp->y[1]) { continue; }
        return 1;
    }
    return 0;
}

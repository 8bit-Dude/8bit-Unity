
#include "definitions.h"

// See game.c
extern unsigned char gameLineUp[4];

#if defined __NES__
 #pragma bss-name(push, "XRAM")
#endif

// Navigation variables
Waypoint *way;
Vehicle cars[MAX_PLAYERS];
Waypoint ways[MAX_WAYPOINTS];
Ramp ramps[MAX_RAMPS];
unsigned char numWays;
unsigned char numRamps;
signed char *vWay;

// Lineup positions
unsigned int lineupX[MAX_PLAYERS];
unsigned int lineupY[MAX_PLAYERS];
unsigned int lineupAng[MAX_PLAYERS];

#if defined __LYNX__
  unsigned char *buffer = (unsigned char*)SHAREDRAM;
#else	
  unsigned char buffer[128];
#endif

#if defined __NES__
 #pragma bss-name(pop) 
#endif

// Function to load *.nav files
void LoadNavigation(char *filename)
{
	// Read nav file contents
	unsigned char n,*p;
  #if defined __APPLE2__
	if (FileOpen(filename)) {
		FileRead(buffer, 128);
		FileClose();
	}
  #elif defined __ATARI__
	FileOpen(filename);
	FileRead(buffer, 128);
  #elif defined __LYNX__
	FileRead(filename);
  #elif defined(__NES__) || defined(__ORIC__)
	FileRead(filename, buffer);
  #elif defined __CBM__
	FILE* fp = fopen(filename, "rb");
	fread(buffer, 1, 128, fp);
	fclose(fp);
  #endif
  
	// Read Lineup
	p = buffer;
	n = *p*2; p++;
	memcpy(lineupX, p, n); p += n;
	memcpy(lineupY, p, n); p += n;
	memcpy(lineupAng, p, n); p += n;
	
	// Read Navigation Cylinders
	numWays = *p; p++;
	memcpy(&ways[0].x, p, numWays*8); p += numWays*8;
	
	// Read Jump Ramps
	numRamps = *p; p++;
	memcpy(&ramps[0].x[0], p, numRamps*8);
	
#ifdef DEBUG_NAV
	for (n=0; n<numWays; ++n) {
		// Display waypoints (debugging)
		LocatePixel(ways[n].x/8u, ways[n].y/8u); SetPixel(BLACK);
		LocatePixel(ways[n].x/8u+ways[n].v[0][0], ways[n].y/8u+ways[n].v[0][1]); SetPixel(BLACK); 
		LocatePixel(ways[n].x/8u+ways[n].v[1][0], ways[n].y/8u+ways[n].v[1][1]); SetPixel(BLACK);         
	}
#endif
}

// Reset cars to Line-up positions 
void ResetLineUp()
{
	unsigned char i,j;
	Vehicle *car;
	for (i=0; i<MAX_PLAYERS; ++i) {
		j = gameLineUp[i];
		car = &cars[i];
		car->x2 =   lineupX[j]; 
		car->y2 =   lineupY[j];
		car->ang1 = lineupAng[j];
		car->ang2 = lineupAng[j];
		car->vel = 0;		
        car->way = 0;
        car->lap = -1;
        car->joy = 0;             
	}
}

// Functions to check navigation around cylinders
signed char v1[2], v2[2], dist[2], cross[2];

char CheckWaypoint(Vehicle *car)
{
	signed int dotCross1, dotCross2;
	
	// Fetch waypoint
	GetWaypoint(car);
	
	// Check waypoint vector dot products against old position
	v1[0] = (car->x1 - way->x)/16u;
	v1[1] = (car->y1 - way->y)/16u;	
	if ( (dot(v1, vWay) >= 0) ) {
		
		// Check waypoint vector dot products against new position
		v2[0] = (car->x2 - way->x)/16u;
		v2[1] = (car->y2 - way->y)/16u;				
		if ( (dot(v2, vWay) >= 0) ) {
			
			// Check proximty to waypoint
			dist[0] = v2[0] - vWay[0]/2u;
			dist[1] = v2[1] - vWay[1]/2u;
			if (dot(dist, dist) < 50)
				return 1;
			
			// Compute dot products with 90 deg rotated vector
			cross[0] = -vWay[1];
			cross[1] =  vWay[0];
			dotCross1 = dot(v1, cross);
			dotCross2 = dot(v2, cross);

			// Check dot products with 90 deg rotated vector
			if (dotCross1 >= 0 && dotCross2 <= 0) { 
			#ifdef DEBUG_NAV
				txtX = 0; txtY = 0;
				PrintBlanks(2, 0);
				PrintNum(car->way+1);
			#endif
				return 1; 
			}
			if (dotCross1 <= 0 && dotCross2 >= 0) { 
			#ifdef DEBUG_NAV
				txtX = 0; txtY = 0;
				PrintBlanks(2, 0);
				PrintNum(car->way+1);
			#endif
				return 1; 
			}
		}
	}
	return 0;
}

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

// Function to check ramp logics
Ramp *ramp;
char CheckRamps(Vehicle *car)
{
    // Gotta check them all...
    unsigned char i;
	for (i=0; i<numRamps; ++i) {
        ramp = &ramps[i];
        if (car->x2 < ramp->x[0] && car->x2 < ramp->x[1]) { continue; }
        if (car->x2 > ramp->x[0] && car->x2 > ramp->x[1]) { continue; }
        if (car->y2 < ramp->y[0] && car->y2 < ramp->y[1]) { continue; }
        if (car->y2 > ramp->y[0] && car->y2 > ramp->y[1]) { continue; }
        return 1;
    }
    return 0;
}

void GetWaypoint(Vehicle *car)
{
	// Prepare waypoint variables
	way = &ways[car->way/2u];
	vWay = way->v[car->way&1];
}

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

int GetWaypointAngle(Vehicle *car)
{
	unsigned char dx = 128, dy = 128;	
	GetWaypoint(car);
	dx += (way->x + 8*vWay[0] - car->x2)/16u;
	dy -= (way->y + 8*vWay[1] - car->y2)/16u;
	return (45*(unsigned int)atan2(dy,dx))/32u;
}

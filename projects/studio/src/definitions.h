
#include "unity.h"
#include <cc65.h>

#define MAXMESH 16
#define MAXVERT 256
#define MAXFACE 256

#define PRIM_BOX 1
#define PRIM_CON 2
#define PRIM_CYL 3
#define PRIM_SPH 4
#define PRIM_TOR 5

#ifndef DEFINE_FIX8
#define DEFINE_FIX8
typedef signed int fix8;
#endif

#define Fix8(val) (val*256)

// See mouse.c
void InitMouse(void);
void UpdateMouse(void);
extern unsigned char* mouse;

// See primitives.c
void Push(unsigned char prim);
void Pop(void);

// See scene.c
void UpdateCamera(void);
void Transform(unsigned char index);
void Rasterize(unsigned char index);
void RasterizeAll(void);
void RenderAll(void);
extern unsigned char meshNum, meshCur, cFaces, cVerts;
extern unsigned char names[MAXMESH][8], nVerts[MAXMESH], nFaces[MAXMESH];
extern fix8 trsf[MAXMESH][9];
extern unsigned char faces[MAXFACE][3]; 
extern fix8 verts[MAXVERT][3];
extern fix8 trans[MAXVERT][3];
extern fix8 norms[MAXFACE][3];
extern fix8 camRotX, camRotZ, camZoom;

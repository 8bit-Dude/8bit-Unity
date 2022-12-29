
#include "definitions.h"

char nameBox[] = "  BOX  ";
fix8 vertBox[8][3]  = {{-128,-128,-128},{-128,128,-128},{128,128,-128},{128,-128,-128},
					   {-128,-128, 128},{-128,128, 128},{128,128, 128},{128,-128, 128}};
char faceBox[12][3] = {{0,1,2},{0,2,3},{0,5,1},{0,4,5}, 
					   {4,6,5},{4,7,6},{3,2,6},{3,6,7}, 
					   {3,4,0},{3,7,4},{1,5,2},{2,5,6}};	
							  
char nameCyl[] = "CYLINDE";
fix8 vertCyl[16][3] = {{128,0,-128},{91,91,-128},{0,128,-128},{-91,91,-128},{-128,0,-128},{-91,-91,-128},{0,-128,-128},{91,-91,-128},
					   {128,0, 128},{91,91, 128},{0,128, 128},{-91,91, 128},{-128,0, 128},{-91,-91, 128},{0,-128, 128},{91,-91, 128}};
char faceCyl[28][3] = {{1,8,0},{9,8,1},{2,9,1},{10,9,2},{3,10,2},{11,10,3},{4,11,3},{12,11,4},
							  {5,12,4},{13,12,5},{6,13,5},{14,13,6},{7,14,6},{15,14,7},{0,15,7},{8,15,0},
							  {0,4,1},{1,4,3},{1,3,2},{5,4,0},{7,5,0},{6,5,7},
							  {9,12,8},{11,12,9},{11,10,9},{8,12,13},{8,13,15},{15,13,14}};	

char nameSph[] = "SPHERE ";
fix8 vertSph[12][3] = {{-67,0,109},{ 67,0,109},{-67,0,-109},{ 67,0,-109},
					   {0,109, 67},{0,109,-67},{0,-109, 67},{0,-109,-67},
					   {109, 67,0},{-109,67,0},{ 109,-67,0},{-109,-67,0}};
char faceSph[20][3] = {{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
					   {8,10,1},{8,3,10},{5,3,8}, {5,2,3}, {2,7,3},
					   {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
					   {6,1,10},{9,0,11},{9,11,2},{9,2,5}, {7,2,11}};

void Push(unsigned char prim)
{	
	unsigned char i, fn, vn, *f, *v, *n;
		
	switch (prim) {
	case PRIM_BOX:
		vn =  8; fn = 12; v = vertBox; f = faceBox; n = nameBox;
		break;
	case PRIM_CYL:
		vn = 16; fn = 28; v = vertCyl; f = faceCyl; n = nameCyl;
		break;	
	case PRIM_SPH:
		vn = 12; fn = 20; v = vertSph; f = faceSph; n = nameSph;
		break;		
	}
	
	// Copy faces/vertices
	memcpy(verts[cVerts], v, 6*vn);	// 3 * fix8
	memcpy(faces[cFaces], f, 3*fn); // 3 * int
	for (i=cFaces; i<cFaces+fn; i++) {
		faces[i][0] += cVerts;
		faces[i][1] += cVerts;
		faces[i][2] += cVerts;
	}
	
	// Update faces/vertices pointers
	cVerts += vn; cFaces += fn;
	nVerts[meshNum] = cVerts;
	nFaces[meshNum] = cFaces;
	strcpy(names[meshNum], n);
	meshCur = meshNum;
	meshNum++;
} 

void Pop(void)
{
	unsigned char i, minF, dropF, nF, nV;
	
	// Check if any work is needed?
	if (!meshNum) return;
	if (meshCur == meshNum-1) {
		meshCur--;
		meshNum--;
		return;
	}
	
	// Compute dropped vertices/faces
	nV = nVerts[meshNum-1]-nVerts[meshCur];
	nF = nFaces[meshNum-1]-nFaces[meshCur];
	if (meshCur) {
		dropF = nFaces[meshCur]-nFaces[meshCur-1];
		minF = nFaces[meshCur-1];
	} else {
		dropF = nFaces[0];
		minF = 0;
	}
	
	// Shift mesh data
	memcpy(verts[meshCur], verts[meshCur+1], 6*nV);
	memcpy(trans[meshCur], trans[meshCur+1], 6*nV);
	memcpy(faces[meshCur], faces[meshCur+1], 3*nF);
	memcpy(norms[meshCur], norms[meshCur+1], 6*nF);
	memcpy(names[meshCur], names[meshCur+1], 6*(meshNum-meshCur-1));
	
	// Shift mesh data ranges and names
	cVerts -= nV; cFaces -= nF;
	for (i=meshCur; i<meshNum-1; i++) {
		nVerts[i] = nVerts[i+1] - nV;
		nFaces[i] = nFaces[i+1] - nF;
	}		
	
	// Shift face indices
	for (i=minF; i<(minF+nF); i++) {
		faces[i][0] -= dropF;
		faces[i][1] -= dropF;
		faces[i][2] -= dropF;
	}	
	
	// Update counters
	if (meshCur) meshCur--;
	meshNum--;
}

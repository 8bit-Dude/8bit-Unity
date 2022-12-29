
#include "definitions.h"

callback *callAdd, *callRem, *callEdit, *callTmp, *callOK;
callback *callBox, *callCon, *callCyl, *callSph, *callTor;
callback *callRL, *callRR, *callRU, *callRD, *callZM, *callZP;

void ResetCalls(void)
{
	ClearCallbacks();
	callAdd = 0; callRem = 0; callEdit = 0; callOK = 0;
	callBox = 0; callCon = 0; callCyl = 0; callSph = 0; callTor = 0;
	callRL = 0; callRR = 0; callRU = 0; callRD = 0; callZM = 0; callZP = 0; 
}

unsigned char up[] = { CHR_ARROW_UP,    0 };
unsigned char dn[] = { CHR_ARROW_DOWN,  0 };
unsigned char lf[] = { CHR_ARROW_LEFT,  0 };
unsigned char rg[] = { CHR_ARROW_RIGHT, 0 };

void DrawGUI(void)
{
	// Reset Callbacks
	ResetCalls();

	// Set GUI Style
	paperColor = BLACK; 
	inkColor = WHITE;
	
	// Show Mesh List
	ListBox(TXT_COLS-8, 0, 7, 9, "Meshes", names, meshNum);
	if (meshNum) callTmp = CheckCallbacks(33, meshCur+1);	//	Select current mesh

	// Show Command Buttons
	callAdd  = Button(TXT_COLS-8, 10, 3, 1, "ADD");
	callRem  = Button(TXT_COLS-4, 10, 3, 1, "REM");
	callEdit = Button(TXT_COLS-6, 12, 4, 1, "EDIT");
	
	// Show Transform Buttons
	txtX = TXT_COLS-13, txtY = 0; PrintChr('-');
	callRL = Button(TXT_COLS-14, 0, 1, 1, lf);
	callRR = Button(TXT_COLS-12, 0, 1, 1, rg);
	callRU = Button(TXT_COLS-10,  2, 1, 1, up);
	callRD = Button(TXT_COLS-10,  3, 1, 1, dn);
	callZM = Button(TXT_COLS-11, 1, 1, 1, "-");
	callZP = Button(TXT_COLS-10, 0, 1, 1, "+");
}

void PrimitivePanel(void)
{
	// Reset Callbacks
	ResetCalls();
	
	// Panel
	paperColor = BLACK; inkColor = WHITE;
	Panel(10, 4, 20, 6, "");
	callBox = Button(12, 6, 8, 1, "  BOX   ");
	callCon = Button(12, 7, 8, 1, "  CONE  ");
	callCyl = Button(12, 8, 8, 1, "CYLINDER");
	callSph = Button(21, 6, 8, 1, " SPHERE ");
	callTor = Button(21, 7, 8, 1, " TORUS  ");
}

unsigned char trsfStr[9][4] = { "0", "0", "0", "0", "0", "0", "10", "10", "10" };

void TransformPanel(void)
{
	unsigned char i,j;

	// Reset Callbacks
	ResetCalls();
	
	// Panel
	paperColor = BLACK; inkColor = WHITE;
	Panel(10, 4, 20, 6, "");	
	
	// Labels
	txtX = 11; txtY = 5;
	PrintStr("Pos:"); txtY += 1;
	PrintStr("Rot:"); txtY += 1;
	PrintStr("Dim:"); txtY += 1;
		
	// Inputs
	paperColor = WHITE; inkColor = BLACK;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			callTmp = Input(16+5*j, 5+i, 3, 1, trsfStr[i*3+j], 3);
		}
	}
	
	// Controls
	paperColor = BLACK; inkColor = WHITE;	
	callOK = Button(19, 9, 7, 1, " OK");	
}

void EncodeTransform(void)
{
	unsigned char i;
	fix8 val;	
	for (i=0; i<9; i++) {
		val = trsf[meshCur][i];
		if (i<3 || i>5) val /= 256u;	
		sprintf(trsfStr[i], "%d", val); 
	}
}

unsigned char primitive;

void DecodeTransform(void)
{
	unsigned char i, m;
	fix8 val;
	if (primitive)
		m = meshNum;
	else
		m = meshCur;
	for (i=0; i<9; i++) {
		sscanf(trsfStr[i], "%d", &val); 
		if (i<3 || i>5) val *= 256u;	
		trsf[m][i] = val;
	}
}

void ProcessCallback(callback* call)
{
	unsigned char update = 0, redraw = 0;
	
	if (call == callAdd) {
		PrimitivePanel();
	} else
	if (call == callBox) {
		primitive = PRIM_BOX;
		TransformPanel();
	} else 
	if (call == callCon) {
		primitive = PRIM_CON;
		TransformPanel();
	} else 
	if (call == callCyl) {
		primitive = PRIM_CYL;
		TransformPanel();
	} else 
	if (call == callSph) {
		primitive = PRIM_SPH;
		TransformPanel();
	} else 
	if (call == callTor) {
		primitive = PRIM_TOR;
		TransformPanel();
	} else
	if (call == callEdit) {
		if (!meshNum) return;
		EncodeTransform();
		TransformPanel();
	} else 
	if (call == callOK) { 	
		DecodeTransform();
		if (primitive) {
			Push(primitive);
			primitive = 0;
		}
		Transform(meshCur);
		Rasterize(meshCur);		
		redraw = 1;
	} else
	if (call == callRem) {
		if (!meshNum) return;
		Pop(); redraw = 1;
	} else
	if (call == callRL) {
		camRotZ += 20; update = 1;
		camRotZ %= 360;
	} else 
	if (call == callRR) {
		camRotZ -= 20; update = 1;
		camRotZ %= 360;
	} else
	if (call == callRU) {
		camRotX += 20; update = 1;
		camRotX %= 360;
	} else 
	if (call == callRD) {
		camRotX -= 20; update = 1;
		camRotX %= 360;
	} else
	if (call == callZM) {
		camZoom *= 2u; update = 1;
	} else 
	if (call == callZP) {
		camZoom /= 2u; update = 1;
	} 

	// Update scene?
	if (update) {
		UpdateCamera();	
		RasterizeAll();
		redraw = 1;
	}

	// Redraw scene?
	if (redraw) {
		ClearBitmap();
		RenderAll();
		DrawGUI(); 
	}		
}

int main(void) 
{
	callback *call;
	unsigned char mouseLock = 0;	

	// Init systems
	InitMouse();	

	// Init Screen
	InitBitmap();
	ClearBitmap();
	ShowBitmap();
		
	// Prepare camera
	UpdateCamera();
	DrawGUI();
	
	while (1) {		
		// Update Input Box (if active)
		if (ProcessInput()) {
			DisableSprite(0);
		} else {
			// Otherwise update Mouse
			EnableSprite(0);

			// Check callbacks
			if (!(mouse[2] & MOU_LEFT)) {
				if (!mouseLock) {
				#if defined(__APPLE2__) || defined(__ORIC__)
					DisableSprite(0);
				#endif
					call = CheckCallbacks((mouse[0]*TXT_COLS)/160u, (mouse[1]*TXT_ROWS)/200u);
					if (call) {
						if (call->type == CALLTYPE_LISTBOX) {
							meshCur = call->value;
						} else
						if (call->type != CALLTYPE_INPUT) {
							ProcessCallback(call);					
						}
					}
				#if defined(__APPLE2__) || defined(__ORIC__)
					EnableSprite(0);
				#endif			
					mouseLock = 1;
				}
			} else {
				mouseLock = 0;
			}
			
			// Update mouse position
			UpdateMouse();				
		}

	  #if defined(__LYNX__) || defined(__NES__)
		UpdateDisplay(); // Manually refresh Display
	  #endif			
	}
}

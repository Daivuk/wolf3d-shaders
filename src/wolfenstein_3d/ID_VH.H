// ID_VH.H


#define WHITE			15			// graphics mode independant colors
#define BLACK			0
#define FIRSTCOLOR		1
#define SECONDCOLOR		12
#define F_WHITE			15
#define F_BLACK			0
#define F_FIRSTCOLOR	1
#define F_SECONDCOLOR	12

//===========================================================================

#define MAXSHIFTS	1

typedef struct
{
  int16_t	width,
	height,
	orgx,orgy,
	xl,yl,xh,yh,
	shifts;
} spritetabletype;

typedef	struct
{
	uint16_t	sourceoffset[MAXSHIFTS];
	uint16_t	planesize[MAXSHIFTS];
	uint16_t	width[MAXSHIFTS];
	byte*		data;
} spritetype;		// the memptr for each sprite points to this

typedef struct
{
	int16_t width,height;
} pictabletype;


typedef struct
{
	int16_t height;
	int16_t location[256];
	char width[256];
} fontstruct;


//===========================================================================


extern	pictabletype *pictable;
extern	pictabletype *picmtable;
extern	spritetabletype *spritetable;

extern	byte	fontcolor;
extern	int16_t	fontnumber;
extern	int16_t	px,py;

//
// Double buffer management routines
//

void VW_InitDoubleBuffer (void);
int16_t	 VW_MarkUpdateBlock (int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void VW_UpdateScreen (void);

//
// mode independant routines
// coordinates in pixels, rounded to best screen res
// regions marked in double buffer
//

void VWB_DrawTile8 (int16_t x, int16_t y, int16_t tile);
void VWB_DrawTile8M (int16_t x, int16_t y, int16_t tile);
void VWB_DrawTile16 (int16_t x, int16_t y, int16_t tile);
void VWB_DrawTile16M (int16_t x, int16_t y, int16_t tile);
void VWB_DrawPic (int16_t x, int16_t y, int16_t chunknum, int16_t w = -1, int16_t h = -1);
void VWB_DrawMPic(int16_t x, int16_t y, int16_t chunknum);
void VWB_Bar (int16_t x, int16_t y, int16_t width, int16_t height, int16_t color);

void VWB_DrawPropString	 (char *string);
void VWB_DrawMPropString (char *string);
void VWB_DrawSprite (int16_t x, int16_t y, int16_t chunknum);
void VWB_Plot (int16_t x, int16_t y, int16_t color);
void VWB_Hlin (int16_t x1, int16_t x2, int16_t y, int16_t color);
void VWB_Vlin (int16_t y1, int16_t y2, int16_t x, int16_t color);


//
// wolfenstein EGA compatability stuff
//
extern byte gamepal[256][3];

void VH_SetDefaultColors (void);

#define VW_Startup		VL_Startup
#define VW_Shutdown		VL_Shutdown
#define VW_SetCRTC		VL_SetCRTC
#define VW_SetScreen	VL_SetScreen
#define VW_Bar			VL_Bar
#define VW_Plot			VL_Plot
#define VW_Hlin(x,z,y,c)	VL_Hlin(x,y,(z)-(x)+1,c)
#define VW_Vlin(y,z,x,c)	VL_Vlin(x,y,(z)-(y)+1,c)
#define VW_DrawPic		VH_DrawPic
#define VW_SetSplitScreen	VL_SetSplitScreen
#define VW_SetLineWidth		VL_SetLineWidth
#define VW_ColorBorder	VL_ColorBorder
#define VW_WaitVBL		VL_WaitVBL
#define VW_FadeIn()		VL_FadeIn(0,255,(byte*)gamepal,30);
#define VW_FadeOut()	VL_FadeOut(0,255,0,0,0,30);
#define VW_ScreenToScreen	VL_ScreenToScreen
#define VW_SetDefaultColors	VH_SetDefaultColors
void	VW_MeasurePropString (char *string, word *width, word *height);
#define EGAMAPMASK(x)	VGAMAPMASK(x)
#define EGAWRITEMODE(x)	VGAWRITEMODE(x)

//#define VW_MemToScreen	VL_MemToLatch

#define MS_Quit			Quit


#define LatchDrawChar(x,y,p) VL_LatchToScreen(latchpics[0]+(p)*16,2,8,x,y)
#define LatchDrawTile(x,y,p) VL_LatchToScreen(latchpics[1]+(p)*64,4,16,x,y)

void LatchDrawPic (uint16_t x, uint16_t y, uint16_t picnum);
void 	LoadLatchMem (void);
_boolean 	FizzleFade (uint16_t source, uint16_t dest,
	uint16_t width,uint16_t height, uint16_t frames,_boolean abortable);


#define NUMLATCHPICS	100
extern	uint16_t	latchpics[NUMLATCHPICS];
extern	uint16_t freelatch;


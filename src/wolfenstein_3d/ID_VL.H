// ID_VL.H

// wolf compatability

#define MS_Quit	Quit

void Quit (char *error);

//===========================================================================


#define SC_INDEX			0x3C4
#define SC_RESET			0
#define SC_CLOCK			1
#define SC_MAPMASK			2
#define SC_CHARMAP			3
#define SC_MEMMODE			4

#define CRTC_INDEX			0x3D4
#define CRTC_H_TOTAL		0
#define CRTC_H_DISPEND		1
#define CRTC_H_BLANK		2
#define CRTC_H_ENDBLANK		3
#define CRTC_H_RETRACE		4
#define CRTC_H_ENDRETRACE 	5
#define CRTC_V_TOTAL		6
#define CRTC_OVERFLOW		7
#define CRTC_ROWSCAN		8
#define CRTC_MAXSCANLINE 	9
#define CRTC_CURSORSTART 	10
#define CRTC_CURSOREND		11
#define CRTC_STARTHIGH		12
#define CRTC_STARTLOW		13
#define CRTC_CURSORHIGH		14
#define CRTC_CURSORLOW		15
#define CRTC_V_RETRACE		16
#define CRTC_V_ENDRETRACE 	17
#define CRTC_V_DISPEND		18
#define CRTC_OFFSET			19
#define CRTC_UNDERLINE		20
#define CRTC_V_BLANK		21
#define CRTC_V_ENDBLANK		22
#define CRTC_MODE			23
#define CRTC_LINECOMPARE 	24


#define GC_INDEX			0x3CE
#define GC_SETRESET			0
#define GC_ENABLESETRESET 	1
#define GC_COLORCOMPARE		2
#define GC_DATAROTATE		3
#define GC_READMAP			4
#define GC_MODE				5
#define GC_MISCELLANEOUS 	6
#define GC_COLORDONTCARE 	7
#define GC_BITMASK			8

#define ATR_INDEX			0x3c0
#define ATR_MODE			16
#define ATR_OVERSCAN		17
#define ATR_COLORPLANEENABLE 18
#define ATR_PELPAN			19
#define ATR_COLORSELECT		20

#define	STATUS_REGISTER_1    0x3da

#define PEL_WRITE_ADR		0x3c8
#define PEL_READ_ADR		0x3c7
#define PEL_DATA			0x3c9


//===========================================================================

#define SCREENSEG		0xa000

#define SCREENWIDTH		80			// default screen width in bytes
#define MAXSCANLINES	200			// size of ylookup table

#define CHARWIDTH		2
#define TILEWIDTH		4

//===========================================================================

extern	uintptr_t	bufferofs;			// all drawing is reletive to this
extern	uint16_t	displayofs,pelpan;	// last setscreen coordinates

extern	uint16_t	screenseg;			// set to 0xa000 for asm convenience

extern	uint16_t	linewidth;
extern	uint16_t	ylookup[MAXSCANLINES];

extern	_boolean		screenfaded;
extern	uint16_t	bordercolor;

//===========================================================================

//
// VGA hardware routines
//

#define VGAWRITEMODE(x) asm{\
cli;\
mov dx,GC_INDEX;\
mov al,GC_MODE;\
out dx,al;\
inc dx;\
in al,dx;\
and al,252;\
or al,x;\
out dx,al;\
sti;}

#define VGAMAPMASK(x) asm{cli;mov dx,SC_INDEX;mov al,SC_MAPMASK;mov ah,x;out dx,ax;sti;}
#define VGAREADMAP(x) asm{cli;mov dx,GC_INDEX;mov al,GC_READMAP;mov ah,x;out dx,ax;sti;}


void VL_Startup (void);
void VL_Shutdown (void);

void VL_SetVGAPlane (void);
void VL_SetTextMode (void);
void VL_DePlaneVGA (void);
void VL_SetVGAPlaneMode (void);
void VL_ClearVideo (byte color);

void VL_SetLineWidth (uint16_t width);
void VL_SetSplitScreen (int16_t linenum);

void VL_WaitVBL (int16_t vbls);
void VL_CrtcStart (int16_t crtc);
void VL_SetScreen (int16_t crtc, int16_t pelpan);

void VL_FillPalette (int16_t red, int16_t green, int16_t blue);
void VL_SetColor	(int16_t color, int16_t red, int16_t green, int16_t blue);
void VL_GetColor	(int16_t color, int16_t *red, int16_t *green, int16_t *blue);
void VL_SetPalette (byte *palette);
void VL_GetPalette (byte *palette);
void VL_FadeOut (int16_t start, int16_t end, int16_t red, int16_t green, int16_t blue, int16_t steps);
void VL_FadeIn (int16_t start, int16_t end, byte *palette, int16_t steps);
void VL_ColorBorder (int16_t color);

void VL_Plot (int16_t x, int16_t y, int16_t color);
void VL_Hlin (uint16_t x, uint16_t y, uint16_t width, uint16_t color);
void VL_Vlin (int16_t x, int16_t y, int16_t height, int16_t color);
void VL_Bar (int16_t x, int16_t y, int16_t width, int16_t height, int16_t color);

void VL_MungePic (byte *source, uint16_t width, uint16_t height);
void VL_DrawPicBare (int16_t x, int16_t y, byte *pic, int16_t width, int16_t height);
void VL_MemToLatch (byte *source, int16_t width, int16_t height, uint16_t dest);
void VL_ScreenToScreen (uint16_t source, uint16_t dest,int16_t width, int16_t height);
void VL_MemToScreen (byte *source, int16_t width, int16_t height, int16_t x, int16_t y);
void VL_MaskedToScreen (byte *source, int16_t width, int16_t height, int16_t x, int16_t y);

void VL_DrawTile8String (char *str, char *tile8ptr, int16_t printx, int16_t printy);
void VL_DrawLatch8String (char *str, uint16_t tile8ptr, int16_t printx, int16_t printy);
void VL_SizeTile8String (char *str, int16_t *width, int16_t *height);
void VL_DrawPropString (char *str, uint16_t tile8ptr, int16_t printx, int16_t printy);
void VL_SizePropString (char *str, int16_t *width, int16_t *height, char *font);

void VL_TestPaletteSet (void);


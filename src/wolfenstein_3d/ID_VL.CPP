// ID_VL.C

// #include <dos.h>
// #include <alloc.h>
// #include <mem.h>
#include <string.h>
#include "ID_HEADS.H"
#include "ID_VL.H"
#include "ws_main.h"
#include <thread>
#include <chrono>
#pragma hdrstop

//
// SC_INDEX is expected to stay at SC_MAPMASK for proper operation
//

uintptr_t	bufferofs;
uint16_t	displayofs,pelpan;

uint16_t	screenseg=SCREENSEG;		// set to 0xa000 for asm convenience

uint16_t	linewidth;
uint16_t	ylookup[MAXSCANLINES];

_boolean		screenfaded;
uint16_t	bordercolor;

_boolean		fastpalette;				// if _true, use outsb to set

byte			palette1[256][3], palette2[256][3];
byte            gamepal[256][3];

//===========================================================================

// asm

int16_t	 VL_VideoID (void);
void VL_SetCRTC (int16_t crtc);
void VL_SetScreen (int16_t crtc, int16_t pelpan) {}
void VL_WaitVBL (int16_t vbls)
{
    VW_UpdateScreen();
    //auto waitTime = std::chrono::milliseconds(vbls * 8);
    //auto timeBefore = std::chrono::high_resolution_clock::now();
    //auto timeAfter = std::chrono::high_resolution_clock::now();
    //auto diff = timeAfter - timeBefore;
    //while (diff < waitTime)
    //{
    //    ws_update_sdl();
    //    timeAfter = std::chrono::high_resolution_clock::now();
    //    diff = timeAfter - timeBefore;
    //}
}

//===========================================================================


/*
=======================
=
= VL_Startup
=
=======================
*/

#if 0
void	VL_Startup (void)
{
	if ( !MS_CheckParm ("HIDDENCARD") && VL_VideoID () != 5)
		MS_Quit ("You need a VGA graphics card to run this!");

	asm	cld;				// all string instructions assume forward
}

#endif

/*
=======================
=
= VL_Startup	// WOLFENSTEIN HACK
=
=======================
*/

static	char *ParmStrings[] = {(char*)"HIDDENCARD",(char*)""};

void	VL_Startup (void)
{
// 	int16_t i,videocard;

// 	asm	cld;

// 	videocard = VL_VideoID ();
// 	for (i = 1;i < _argc;i++)
// 		if (US_CheckParm(_argv[i],ParmStrings) == 0)
// 		{
// 			videocard = 5;
// 			break;
// 		}

// 	if (videocard != 5)
// Quit ("Improper video card!  If you really have a VGA card that I am not \n"
// 	  "detecting, use the -HIDDENCARD command line parameter!");

}



/*
=======================
=
= VL_Shutdown
=
=======================
*/

void	VL_Shutdown (void)
{
	VL_SetTextMode ();
}


/*
=======================
=
= VL_SetVGAPlaneMode
=
=======================
*/

void	VL_SetVGAPlaneMode (void)
{
// asm	mov	ax,0x13
// asm	int16_t	0x10
// 	VL_DePlaneVGA ();
// 	VGAMAPMASK(15);
 	VL_SetLineWidth (40);
}


/*
=======================
=
= VL_SetTextMode
=
=======================
*/

void	VL_SetTextMode (void)
{
// asm	mov	ax,3
// asm	int16_t	0x10
}

//===========================================================================

/*
=================
=
= VL_ClearVideo
=
= Fill the entire video buffer with a given color
=
=================
*/

void VL_ClearVideo (byte color)
{
// asm	mov	dx,GC_INDEX
// asm	mov	al,GC_MODE
// asm	out	dx,al
// asm	inc	dx
// asm	in	al,dx
// asm	and	al,0xfc				// write mode 0 to store directly to video
// asm	out	dx,al

// asm	mov	dx,SC_INDEX
// asm	mov	ax,SC_MAPMASK+15*256
// asm	out	dx,ax				// write through all four planes

// asm	mov	ax,SCREENSEG
// asm	mov	es,ax
// asm	mov	al,[color]
// asm	mov	ah,al
// asm	mov	cx,0x8000			// 0x8000 words, clearing 8 video bytes/word
// asm	xor	di,di
// asm	rep	stosw
}


/*
=============================================================================

			VGA REGISTER MANAGEMENT ROUTINES

=============================================================================
*/


/*
=================
=
= VL_DePlaneVGA
=
=================
*/

void VL_DePlaneVGA (void)
{

// //
// // change CPU addressing to non linear mode
// //

// //
// // turn off chain 4 and odd/even
// //
// 	outportb (SC_INDEX,SC_MEMMODE);
// 	outportb (SC_INDEX+1,(inportb(SC_INDEX+1)&~8)|4);

// 	outportb (SC_INDEX,SC_MAPMASK);		// leave this set throughought

// //
// // turn off odd/even and set write mode 0
// //
// 	outportb (GC_INDEX,GC_MODE);
// 	outportb (GC_INDEX+1,inportb(GC_INDEX+1)&~0x13);

// //
// // turn off chain
// //
// 	outportb (GC_INDEX,GC_MISCELLANEOUS);
// 	outportb (GC_INDEX+1,inportb(GC_INDEX+1)&~2);

// //
// // clear the entire buffer space, because int16_t 10h only did 16 k / plane
// //
// 	VL_ClearVideo (0);

// //
// // change CRTC scanning from doubleword to byte mode, allowing >64k scans
// //
// 	outportb (CRTC_INDEX,CRTC_UNDERLINE);
// 	outportb (CRTC_INDEX+1,inportb(CRTC_INDEX+1)&~0x40);

// 	outportb (CRTC_INDEX,CRTC_MODE);
// 	outportb (CRTC_INDEX+1,inportb(CRTC_INDEX+1)|0x40);
}

//===========================================================================

/*
====================
=
= VL_SetLineWidth
=
= Line witdh is in WORDS, 40 words is normal width for vgaplanegr
=
====================
*/

void VL_SetLineWidth (uint16_t width)
{
	int16_t i,offset;

// //
// // set wide virtual screen
// //
	// outport (CRTC_INDEX,CRTC_OFFSET+width*256);

//
// set up lookup tables
//
	linewidth = width*2;

	offset = 0;

	for (i=0;i<MAXSCANLINES;i++)
	{
		ylookup[i]=offset;
		offset += linewidth;
	}
}

/*
====================
=
= VL_SetSplitScreen
=
====================
*/

void VL_SetSplitScreen (int16_t linenum)
{
	// VL_WaitVBL (1);
	// linenum=linenum*2-1;
	// outportb (CRTC_INDEX,CRTC_LINECOMPARE);
	// outportb (CRTC_INDEX+1,linenum % 256);
	// outportb (CRTC_INDEX,CRTC_OVERFLOW);
	// outportb (CRTC_INDEX+1, 1+16*(linenum/256));
	// outportb (CRTC_INDEX,CRTC_MAXSCANLINE);
	// outportb (CRTC_INDEX+1,inportb(CRTC_INDEX+1) & (255-64));
}


/*
=============================================================================

						PALETTE OPS

		To avoid snow, do a WaitVBL BEFORE calling these

=============================================================================
*/


/*
=================
=
= VL_FillPalette
=
=================
*/

void VL_FillPalette (int16_t red, int16_t green, int16_t blue)
{
	// int16_t	i;

	// outportb (PEL_WRITE_ADR,0);
	// for (i=0;i<256;i++)
	// {
	// 	outportb (PEL_DATA,red);
	// 	outportb (PEL_DATA,green);
	// 	outportb (PEL_DATA,blue);
	// }
}

//===========================================================================

/*
=================
=
= VL_SetColor
=
=================
*/

void VL_SetColor	(int16_t color, int16_t red, int16_t green, int16_t blue)
{
	// outportb (PEL_WRITE_ADR,color);
	// outportb (PEL_DATA,red);
	// outportb (PEL_DATA,green);
	// outportb (PEL_DATA,blue);
}

//===========================================================================

/*
=================
=
= VL_GetColor
=
=================
*/

void VL_GetColor	(int16_t color, int16_t *red, int16_t *green, int16_t *blue)
{
	// outportb (PEL_READ_ADR,color);
	// *red = inportb (PEL_DATA);
	// *green = inportb (PEL_DATA);
	// *blue = inportb (PEL_DATA);
}

//===========================================================================

/*
=================
=
= VL_SetPalette
=
= If fast palette setting has been tested for, it is used
= (some cards don't like outsb palette setting)
=
=================
*/

//void VL_SetPalette (byte  *palette)
//{
// 	int16_t	i;

// //	outportb (PEL_WRITE_ADR,0);
// //	for (i=0;i<768;i++)
// //		outportb(PEL_DATA,*palette++);

// 	asm	mov	dx,PEL_WRITE_ADR
// 	asm	mov	al,0
// 	asm	out	dx,al
// 	asm	mov	dx,PEL_DATA
// 	asm	lds	si,[palette]

// 	asm	test	[ss:fastpalette],1
// 	asm	jz	slowset
// //
// // set palette fast for cards that can take it
// //
// 	asm	mov	cx,768
// 	asm	rep outsb
// 	asm	jmp	done

// //
// // set palette slowly for some video cards
// //
// slowset:
// 	asm	mov	cx,256
// setloop:
// 	asm	lodsb
// 	asm	out	dx,al
// 	asm	lodsb
// 	asm	out	dx,al
// 	asm	lodsb
// 	asm	out	dx,al
// 	asm	loop	setloop

// done:
// 	asm	mov	ax,ss
// 	asm	mov	ds,ax

//}


//===========================================================================

/*
=================
=
= VL_GetPalette
=
= This does not use the port string instructions,
= due to some incompatabilities
=
=================
*/

//byte current_palette[768] = {0};

void VL_GetPalette (byte  *palette)
{
    //memcpy(palette, current_palette, 768);
	 int16_t	i;

	 outportb (PEL_READ_ADR,0);
	 for (i=0;i<768;i++)
	 	*palette++ = (byte)inportb(PEL_DATA);
}


//===========================================================================

/*
=================
=
= VL_FadeOut
=
= Fades the current palette to the given color in the given number of steps
=
=================
*/

void VL_FadeOut (int16_t start, int16_t end, int16_t red, int16_t green, int16_t blue, int16_t steps)
{
    extern float fade_val;
 	int16_t		i,j,orig,delta;
 	byte	 *origptr,  *newptr;

    fade_val = 1.0f;

 	VL_WaitVBL(1);
 	//VL_GetPalette (&palette1[0][0]);
 	//memcpy (palette2,palette1,768);
 //
 // fade through intermediate frames
 //
 	for (i=0;i<steps;i++)
 	{
 		//origptr = &palette1[start][0];
 		//newptr = &palette2[start][0];
 		//for (j=start;j<=end;j++)
 		//{
 		//	orig = *origptr++;
 		//	delta = red-orig;
 		//	*newptr++ = orig + delta * i / steps;
 		//	orig = *origptr++;
 		//	delta = green-orig;
 		//	*newptr++ = orig + delta * i / steps;
 		//	orig = *origptr++;
 		//	delta = blue-orig;
 		//	*newptr++ = orig + delta * i / steps;
 		//}

 		VL_WaitVBL(1);
        fade_val = 1.0f - (float)i / (float)steps;
 		//VL_SetPalette (&palette2[0][0]);
 	}

 //
 // final color
 //
 	//VL_FillPalette (red,green,blue);
    fade_val = 0.0f;
 	screenfaded = _true;
}


/*
=================
=
= VL_FadeIn
=
=================
*/

void VL_FadeIn (int16_t start, int16_t end, byte  *palette, int16_t steps)
{
    extern float fade_val;
 	int16_t		i,j,delta;

    fade_val = 0.0f;

 	VL_WaitVBL(1);
 	//VL_GetPalette (&palette1[0][0]);
 	//memcpy (&palette2[0][0],&palette1[0][0],sizeof(palette1));

 	start *= 3;
 	end = end*3+2;

 //
 // fade through intermediate frames
 //
 	for (i=0;i<steps;i++)
 	{
 		//for (j=start;j<=end;j++)
 		//{
 		//	delta = palette[j]-palette1[0][j];
 		//	palette2[0][j] = palette1[0][j] + delta * i / steps;
 		//}

 		VL_WaitVBL(1); 
        fade_val = (float)i / (float)steps;
 		//VL_SetPalette (&palette2[0][0]);
 	}

 //
 // final color
 //
 	//VL_SetPalette (palette);
 	screenfaded = _false;
    fade_val = 1.0f;
}



/*
=================
=
= VL_TestPaletteSet
=
= Sets the palette with outsb, then reads it in and compares
= If it compares ok, fastpalette is set to _true.
=
=================
*/

void VL_TestPaletteSet (void)
{
	 int16_t	i;

	 for (i=0;i<768;i++)
	 	palette1[0][i] = i;

	 fastpalette = _true;
	 VL_SetPalette (&palette1[0][0]);
	 VL_GetPalette (&palette2[0][0]);
	 if (memcmp (&palette1[0][0],&palette2[0][0],768))
	 	fastpalette = _false;
}


/*
==================
=
= VL_ColorBorder
=
==================
*/

void VL_ColorBorder (int16_t color)
{
	// _AH=0x10;
	// _AL=1;
	// _BH=color;
	// geninterrupt (0x10);
	// bordercolor = color;
}



/*
=============================================================================

							PIXEL OPS

=============================================================================
*/

byte	pixmasks[4] = {1,2,4,8};
byte	leftmasks[4] = {15,14,12,8};
byte	rightmasks[4] = {1,3,7,15};


/*
=================
=
= VL_Plot
=
=================
*/

void VL_Plot (int16_t x, int16_t y, int16_t color)
{
	// byte mask;

	// mask = pixmasks[x&3];
	// VGAMAPMASK(mask);
	// *(byte  *)MK_FP(SCREENSEG,bufferofs+(ylookup[y]+(x>>2))) = color;
	// VGAMAPMASK(15);
}


/*
=================
=
= VL_Hlin
=
=================
*/

//void VL_Hlin (uint16_t x, uint16_t y, uint16_t width, uint16_t color)
//{
//	// uint16_t		xbyte;
//	// byte			 *dest;
//	// byte			leftmask,rightmask;
//	// int16_t				midbytes;
//
//	// xbyte = x>>2;
//	// leftmask = leftmasks[x&3];
//	// rightmask = rightmasks[(x+width-1)&3];
//	// midbytes = ((x+width+3)>>2) - xbyte - 2;
//
//	// dest = MK_FP(SCREENSEG,bufferofs+ylookup[y]+xbyte);
//
//	// if (midbytes<0)
//	// {
//	// // all in one byte
//	// 	VGAMAPMASK(leftmask&rightmask);
//	// 	*dest = color;
//	// 	VGAMAPMASK(15);
//	// 	return;
//	// }
//
//	// VGAMAPMASK(leftmask);
//	// *dest++ = color;
//
//	// VGAMAPMASK(15);
//	// _fmemset (dest,color,midbytes);
//	// dest+=midbytes;
//
//	// VGAMAPMASK(rightmask);
//	// *dest = color;
//
//	// VGAMAPMASK(15);
//}


/*
=================
=
= VL_Vlin
=
=================
*/

//void VL_Vlin (int16_t x, int16_t y, int16_t height, int16_t color)
//{
//	// byte	 *dest,mask;
//
//	// mask = pixmasks[x&3];
//	// VGAMAPMASK(mask);
//
//	// dest = MK_FP(SCREENSEG,bufferofs+ylookup[y]+(x>>2));
//
//	// while (height--)
//	// {
//	// 	*dest = color;
//	// 	dest += linewidth;
//	// }
//
//	// VGAMAPMASK(15);
//}


/*
=================
=
= VL_Bar
=
=================
*/

//void VL_Bar (int16_t x, int16_t y, int16_t width, int16_t height, int16_t color)
//{
//	// byte	 *dest;
//	// byte	leftmask,rightmask;
//	// int16_t		midbytes,linedelta;
//
//	// leftmask = leftmasks[x&3];
//	// rightmask = rightmasks[(x+width-1)&3];
//	// midbytes = ((x+width+3)>>2) - (x>>2) - 2;
//	// linedelta = linewidth-(midbytes+1);
//
//	// dest = MK_FP(SCREENSEG,bufferofs+ylookup[y]+(x>>2));
//
//	// if (midbytes<0)
//	// {
//	// // all in one byte
//	// 	VGAMAPMASK(leftmask&rightmask);
//	// 	while (height--)
//	// 	{
//	// 		*dest = color;
//	// 		dest += linewidth;
//	// 	}
//	// 	VGAMAPMASK(15);
//	// 	return;
//	// }
//
//	// while (height--)
//	// {
//	// 	VGAMAPMASK(leftmask);
//	// 	*dest++ = color;
//
//	// 	VGAMAPMASK(15);
//	// 	_fmemset (dest,color,midbytes);
//	// 	dest+=midbytes;
//
//	// 	VGAMAPMASK(rightmask);
//	// 	*dest = color;
//
//	// 	dest+=linedelta;
//	// }
//
//	// VGAMAPMASK(15);
//}

/*
============================================================================

							MEMORY OPS

============================================================================
*/

/*
=================
=
= VL_MemToLatch
=
=================
*/

void VL_MemToLatch (byte  *source, int16_t width, int16_t height, uint16_t dest)
{
// 	uint16_t	count;
// 	byte	plane,mask;

// 	count = ((width+3)/4)*height;
// 	mask = 1;
// 	for (plane = 0; plane<4 ; plane++)
// 	{
// 		VGAMAPMASK(mask);
// 		mask <<= 1;

// asm	mov	cx,count
// asm mov ax,SCREENSEG
// asm mov es,ax
// asm	mov	di,[dest]
// asm	lds	si,[source]
// asm	rep movsb
// asm mov	ax,ss
// asm	mov	ds,ax

// 		source+= count;
// 	}
}


//===========================================================================


/*
=================
=
= VL_MemToScreen
=
= Draws a block of data to the screen.
=
=================
*/

void VL_MemToScreen (byte  *source, int16_t width, int16_t height, int16_t x, int16_t y)
{
	// byte     *screen, *dest,mask;
	// int16_t		plane;

	// width>>=2;
	// dest = MK_FP(SCREENSEG,bufferofs+ylookup[y]+(x>>2) );
	// mask = 1 << (x&3);

	// for (plane = 0; plane<4; plane++)
	// {
	// 	VGAMAPMASK(mask);
	// 	mask <<= 1;
	// 	if (mask == 16)
	// 		mask = 1;

	// 	screen = dest;
	// 	for (y=0;y<height;y++,screen+=linewidth,source+=width)
	// 		_fmemcpy (screen,source,width);
	// }
}

//==========================================================================


/*
=================
=
= VL_MaskedToScreen
=
= Masks a block of main memory to the screen.
=
=================
*/

void VL_MaskedToScreen (byte  *source, int16_t width, int16_t height, int16_t x, int16_t y)
{
// 	byte     *screen, *dest,mask;
// 	byte	 *maskptr;
// 	int16_t		plane;

// 	width>>=2;
// 	dest = MK_FP(SCREENSEG,bufferofs+ylookup[y]+(x>>2) );
// //	mask = 1 << (x&3);

// //	maskptr = source;

// 	for (plane = 0; plane<4; plane++)
// 	{
// 		VGAMAPMASK(mask);
// 		mask <<= 1;
// 		if (mask == 16)
// 			mask = 1;

// 		screen = dest;
// 		for (y=0;y<height;y++,screen+=linewidth,source+=width)
// 			_fmemcpy (screen,source,width);
// 	}
}

//==========================================================================

/*
=================
=
= VL_LatchToScreen
=
=================
*/

void VL_LatchToScreen (uint16_t source, int16_t width, int16_t height, int16_t x, int16_t y)
{
// 	VGAWRITEMODE(1);
// 	VGAMAPMASK(15);

// asm	mov	di,[y]				// dest = bufferofs+ylookup[y]+(x>>2)
// asm	shl	di,1
// asm	mov	di,[WORD PTR ylookup+di]
// asm	add	di,[bufferofs]
// asm	mov	ax,[x]
// asm	shr	ax,2
// asm	add	di,ax

// asm	mov	si,[source]
// asm	mov	ax,[width]
// asm	mov	bx,[linewidth]
// asm	sub	bx,ax
// asm	mov	dx,[height]
// asm	mov	cx,SCREENSEG
// asm	mov	ds,cx
// asm	mov	es,cx

// drawline:
// asm	mov	cx,ax
// asm	rep movsb
// asm	add	di,bx
// asm	dec	dx
// asm	jnz	drawline

// asm	mov	ax,ss
// asm	mov	ds,ax

// 	VGAWRITEMODE(0);
}


//===========================================================================
void VL_ScreenToScreen (uint16_t source, uint16_t dest,int16_t width, int16_t height) {}
#if 0

/*
=================
=
= VL_ScreenToScreen
=
=================
*/

void VL_ScreenToScreen (uint16_t source, uint16_t dest,int16_t width, int16_t height)
{
	VGAWRITEMODE(1);
	VGAMAPMASK(15);

asm	mov	si,[source]
asm	mov	di,[dest]
asm	mov	ax,[width]
asm	mov	bx,[linewidth]
asm	sub	bx,ax
asm	mov	dx,[height]
asm	mov	cx,SCREENSEG
asm	mov	ds,cx
asm	mov	es,cx

drawline:
asm	mov	cx,ax
asm	rep movsb
asm	add	si,bx
asm	add	di,bx
asm	dec	dx
asm	jnz	drawline

asm	mov	ax,ss
asm	mov	ds,ax

	VGAWRITEMODE(0);
}


#endif

/*
=============================================================================

						STRING OUTPUT ROUTINES

=============================================================================
*/




/*
===================
=
= VL_DrawTile8String
=
===================
*/

void VL_DrawTile8String (char *str, char  *tile8ptr, int16_t printx, int16_t printy)
{
	// int16_t		i;
	// uint16_t	 *dest, *screen, *src;

	// dest = MK_FP(SCREENSEG,bufferofs+ylookup[printy]+(printx>>2));

	// while (*str)
	// {
	// 	src = (uint16_t  *)(tile8ptr + (*str<<6));
	// 	// each character is 64 bytes

	// 	VGAMAPMASK(1);
	// 	screen = dest;
	// 	for (i=0;i<8;i++,screen+=linewidth)
	// 		*screen = *src++;
	// 	VGAMAPMASK(2);
	// 	screen = dest;
	// 	for (i=0;i<8;i++,screen+=linewidth)
	// 		*screen = *src++;
	// 	VGAMAPMASK(4);
	// 	screen = dest;
	// 	for (i=0;i<8;i++,screen+=linewidth)
	// 		*screen = *src++;
	// 	VGAMAPMASK(8);
	// 	screen = dest;
	// 	for (i=0;i<8;i++,screen+=linewidth)
	// 		*screen = *src++;

	// 	str++;
	// 	printx += 8;
	// 	dest+=2;
	// }
}



/*
===================
=
= VL_DrawLatch8String
=
===================
*/

void VL_DrawLatch8String (char *str, uint16_t tile8ptr, int16_t printx, int16_t printy)
{
// 	int16_t		i;
// 	uint16_t	src,dest;

// 	dest = bufferofs+ylookup[printy]+(printx>>2);

// 	VGAWRITEMODE(1);
// 	VGAMAPMASK(15);

// 	while (*str)
// 	{
// 		src = tile8ptr + (*str<<4);		// each character is 16 latch bytes

// asm	mov	si,[src]
// asm	mov	di,[dest]
// asm	mov	dx,[linewidth]

// asm	mov	ax,SCREENSEG
// asm	mov	ds,ax

// asm	lodsw
// asm	mov	[di],ax
// asm	add	di,dx
// asm	lodsw
// asm	mov	[di],ax
// asm	add	di,dx
// asm	lodsw
// asm	mov	[di],ax
// asm	add	di,dx
// asm	lodsw
// asm	mov	[di],ax
// asm	add	di,dx
// asm	lodsw
// asm	mov	[di],ax
// asm	add	di,dx
// asm	lodsw
// asm	mov	[di],ax
// asm	add	di,dx
// asm	lodsw
// asm	mov	[di],ax
// asm	add	di,dx
// asm	lodsw
// asm	mov	[di],ax
// asm	add	di,dx

// asm	mov	ax,ss
// asm	mov	ds,ax

// 		str++;
// 		printx += 8;
// 		dest+=2;
// 	}

// 	VGAWRITEMODE(0);
}


/*
===================
=
= VL_SizeTile8String
=
===================
*/

void VL_SizeTile8String (char *str, int16_t *width, int16_t *height)
{
	// *height = 8;
	// *width = 8*strlen(str);
}










// WL_DRAW.C

#include "WL_DEF.H"
#include "ws.h"
// #include <DOS.H>
#pragma hdrstop

//#define DEBUGWALLS
//#define DEBUGTICS

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

// the door is the last picture before the sprites
#define DOORWALL	(PMSpriteStart-8)

#define ACTORSIZE	0x4000

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/


#ifdef DEBUGWALLS
uint16_t screenloc[3]= {0,0,0};
#else
uint16_t screenloc[3]= {PAGE1START,PAGE2START,PAGE3START};
#endif
uint16_t freelatch = FREESTART;

int32_t 	lasttimecount;
int32_t 	frameon;

uint16_t	wallheight[MAXVIEWWIDTH];

fixed	tileglobal	= TILEGLOBAL;
fixed	mindist		= MINDIST;


//
// math tables
//
int16_t			pixelangle[MAXVIEWWIDTH];
int32_t		 finetangent[FINEANGLES/4];
fixed 		 sintable[ANGLES+ANGLES/4], *costable = sintable+(ANGLES/4);

//
// refresh variables
//
fixed	viewx,viewy;			// the focal point
int16_t		viewangle;
fixed	viewsin,viewcos;
word    xspot,yspot;



fixed	FixedByFrac (fixed a, fixed b);
void	TransformActor (objtype *ob);
void	BuildTables (void);
void	ClearScreen (void);
int16_t		CalcRotate (objtype *ob);
void	DrawScaleds (void);
void	CalcTics (void);
void	FixOfs (void);
void	ThreeDRefresh (void);



//
// wall optimization variables
//
int16_t		lastside;		// _true for vertical
int32_t	lastintercept;
int16_t		lasttilehit;


//
// ray tracing variables
//
int16_t			focaltx,focalty,viewtx,viewty;

int16_t			midangle,angle;
uint16_t	xpartial,ypartial;
uint16_t	xpartialup,xpartialdown,ypartialup,ypartialdown;
uint16_t	xinttile,yinttile;

uint16_t	tilehit;
uint16_t	pixx;

int16_t		xtile,ytile;
int16_t		xtilestep,ytilestep;
int32_t	xintercept,yintercept;
int32_t	xstep,ystep;
int texdelta;

int16_t		horizwall[MAXWALLTILES],vertwall[MAXWALLTILES];


/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/


// void AsmRefresh (void);			// in WL_DR_A.ASM

/*
============================================================================

			   3 - D  DEFINITIONS

============================================================================
*/


//==========================================================================


/*
========================
=
= FixedByFrac
=
= multiply a 16/16 bit, 2's complement fixed point number by a 16 bit
= fraction, passed as a signed magnitude 32 bit number
=
========================
*/

#pragma warn -rvl			// I stick the return value in with ASMs

fixed FixedByFrac (fixed a, fixed b)
{
    return (fixed)(((int64_t)a * (int64_t)b) / 65536);
    //return (fixed)(((int64_t)a * b + 0x8000) >> 16);
//
// setup
//
// asm	mov	si,[WORD PTR b+2]	// sign of result = sign of fraction

// asm	mov	ax,[WORD PTR a]
// asm	mov	cx,[WORD PTR a+2]

// asm	or	cx,cx
// asm	jns	aok:				// negative?
// asm	neg	cx
// asm	neg	ax
// asm	sbb	cx,0
// asm	xor	si,0x8000			// toggle sign of result
// aok:

// //
// // multiply  cx:ax by bx
// //
// asm	mov	bx,[WORD PTR b]
// asm	mul	bx					// fraction*fraction
// asm	mov	di,dx				// di is low word of result
// asm	mov	ax,cx				//
// asm	mul	bx					// units*fraction
// asm add	ax,di
// asm	adc	dx,0

// //
// // put result dx:ax in 2's complement
// //
// asm	test	si,0x8000		// is the result negative?
// asm	jz	ansok:
// asm	neg	dx
// asm	neg	ax
// asm	sbb	dx,0

// ansok:;

}

#pragma warn +rvl

//==========================================================================

/*
========================
=
= TransformActor
=
= Takes paramaters:
=   gx,gy		: globalx/globaly of point
=
= globals:
=   viewx,viewy		: point of view
=   viewcos,viewsin	: sin/cos of viewangle
=   scale		: conversion from global value to screen value
=
= sets:
=   screenx,transx,transy,screenheight: projected edge location and size
=
========================
*/


//
// transform actor
//
 void TransformActor (objtype *ob)
 {
 	int16_t ratio;
 	fixed gx,gy,gxt,gyt,nx,ny;
 	int32_t	temp;

 //
 // translate point to view centered coordinates
 //
 	gx = ob->x-viewx;
 	gy = ob->y-viewy;

 //
 // calculate newx
 //
 	gxt = FixedByFrac(gx,viewcos);
 	gyt = FixedByFrac(gy,viewsin);
 	nx = gxt-gyt-ACTORSIZE;		// fudge the shape forward a bit, because
 								// the midpoint could put parts of the shape
 								// into an adjacent wall

 //
 // calculate newy
 //
 	gxt = FixedByFrac(gx,viewsin);
 	gyt = FixedByFrac(gy,viewcos);
 	ny = gyt+gxt;

 //
 // calculate perspective ratio
 //
 	ob->transx = nx;
 	ob->transy = ny;

 	if (nx<mindist)			// too close, don't overflow the divide
 	{
 	  ob->viewheight = 0;
 	  return;
 	}

 	ob->viewx = centerx + ny*scale/nx;	// DEBUG: use assembly divide

 //
 // calculate height (heightnumerator/(nx>>8))
 //
 	// asm	mov	ax,[WORD PTR heightnumerator]
 	// asm	mov	dx,[WORD PTR heightnumerator+2]
 	// asm	idiv	[WORD PTR nx+1]			// nx>>8
 	// asm	mov	[WORD PTR temp],ax
 	// asm	mov	[WORD PTR temp+2],dx

     temp = heightnumerator/(nx>>8);

 	ob->viewheight = temp;
 }

//==========================================================================

/*
========================
=
= TransformTile
=
= Takes paramaters:
=   tx,ty		: tile the object is centered in
=
= globals:
=   viewx,viewy		: point of view
=   viewcos,viewsin	: sin/cos of viewangle
=   scale		: conversion from global value to screen value
=
= sets:
=   screenx,transx,transy,screenheight: projected edge location and size
=
= Returns _true if the tile is withing getting distance
=
========================
*/

 _boolean TransformTile (int16_t tx, int16_t ty, int16_t *dispx, int16_t *dispheight)
 {
 	int16_t ratio;
 	fixed gx,gy,gxt,gyt,nx,ny;
 	int32_t	temp;

 //
 // translate point to view centered coordinates
 //
 	gx = ((int32_t)tx<<TILESHIFT)+0x8000-viewx;
 	gy = ((int32_t)ty<<TILESHIFT)+0x8000-viewy;

 //
 // calculate newx
 //
 	gxt = FixedByFrac(gx,viewcos);
 	gyt = FixedByFrac(gy,viewsin);
 	nx = gxt-gyt-0x2000;		// 0x2000 is size of object

 //
 // calculate newy
 //
 	gxt = FixedByFrac(gx,viewsin);
 	gyt = FixedByFrac(gy,viewcos);
 	ny = gyt+gxt;


 //
 // calculate perspective ratio
 //
 	if (nx<mindist)			// too close, don't overflow the divide
 	{
 		*dispheight = 0;
 		return _false;
 	}

 	*dispx = centerx + ny*scale/nx;	// DEBUG: use assembly divide

 //
 // calculate height (heightnumerator/(nx>>8))
 //
 	// asm	mov	ax,[WORD PTR heightnumerator]
 	// asm	mov	dx,[WORD PTR heightnumerator+2]
 	// asm	idiv	[WORD PTR nx+1]			// nx>>8
 	// asm	mov	[WORD PTR temp],ax
 	// asm	mov	[WORD PTR temp+2],dx

     temp = (heightnumerator/(nx>>8));
 	*dispheight = temp;

 //
 // see if it should be grabbed
 //
 	if (nx<TILEGLOBAL && ny>-TILEGLOBAL/2 && ny<TILEGLOBAL/2)
 		return _true;
 	else
 		return _false;
 }

//==========================================================================

/*
====================
=
= CalcHeight
=
= Calculates the height of xintercept,yintercept from viewx,viewy
=
====================
*/

// #pragma warn -rvl			// I stick the return value in with ASMs

// int16_t	CalcHeight (void)
// {
// 	int16_t	transheight;
// 	int16_t ratio;
// 	fixed gxt,gyt,nx,ny;
// 	int32_t	gx,gy;

// 	gx = xintercept-viewx;
// 	gxt = FixedByFrac(gx,viewcos);

// 	gy = yintercept-viewy;
// 	gyt = FixedByFrac(gy,viewsin);

// 	nx = gxt-gyt;

//   //
//   // calculate perspective ratio (heightnumerator/(nx>>8))
//   //
// 	if (nx<mindist)
// 		nx=mindist;			// don't let divide overflow

//     return (heightnumerator/(nx>>8));
// 	// asm	mov	ax,[WORD PTR heightnumerator]
// 	// asm	mov	dx,[WORD PTR heightnumerator+2]
// 	// asm	idiv	[WORD PTR nx+1]			// nx>>8
// }


//==========================================================================

/*
===================
=
= ScalePost
=
===================
*/

int32_t		postsource;
uint16_t	postx;
uint16_t	postwidth;

// void ScalePost (void)		// VGA version
// {
// // 	asm	mov	ax,SCREENSEG
// // 	asm	mov	es,ax

// // 	asm	mov	bx,[postx]
// // 	asm	shl	bx,1
// // 	asm	mov	bp,WORD PTR [wallheight+bx]		// fractional height (low 3 bits frac)
// // 	asm	and	bp,0xfff8				// bp = heightscaler*4
// // 	asm	shr	bp,1
// // 	asm	cmp	bp,[maxscaleshl2]
// // 	asm	jle	heightok
// // 	asm	mov	bp,[maxscaleshl2]
// // heightok:
// // 	asm	add	bp,OFFSET fullscalefarcall
// // 	//
// // 	// scale a byte wide strip of wall
// // 	//
// // 	asm	mov	bx,[postx]
// // 	asm	mov	di,bx
// // 	asm	shr	di,2						// X in bytes
// // 	asm	add	di,[bufferofs]

// // 	asm	and	bx,3
// // 	asm	shl	bx,3						// bx = pixel*8+pixwidth
// // 	asm	add	bx,[postwidth]

// // 	asm	mov	al,BYTE PTR [mapmasks1-1+bx]	// -1 because no widths of 0
// // 	asm	mov	dx,SC_INDEX+1
// // 	asm	out	dx,al						// set bit mask register
// // 	asm	lds	si,DWORD PTR [postsource]
// // 	asm	call DWORD PTR [bp]				// scale the line of pixels

// // 	asm	mov	al,BYTE PTR [ss:mapmasks2-1+bx]   // -1 because no widths of 0
// // 	asm	or	al,al
// // 	asm	jz	nomore

// // 	//
// // 	// draw a second byte for vertical strips that cross two bytes
// // 	//
// // 	asm	inc	di
// // 	asm	out	dx,al						// set bit mask register
// // 	asm	call DWORD PTR [bp]				// scale the line of pixels

// // 	asm	mov	al,BYTE PTR [ss:mapmasks3-1+bx]	// -1 because no widths of 0
// // 	asm	or	al,al
// // 	asm	jz	nomore
// // 	//
// // 	// draw a third byte for vertical strips that cross three bytes
// // 	//
// // 	asm	inc	di
// // 	asm	out	dx,al						// set bit mask register
// // 	asm	call DWORD PTR [bp]				// scale the line of pixels


// // nomore:
// // 	asm	mov	ax,ss
// // 	asm	mov	ds,ax
// }

void  FarScalePost (void)				// just so other files can call
{
	// ScalePost ();
}


/*
====================
=
= HitVertWall
=
= tilehit bit 7 is 0, because it's not a door tile
= if bit 6 is 1 and the adjacent tile is a door tile, use door side pic
=
====================
*/

// void HitVertWall (void)
// {
// 	int16_t			wallpic;
// 	uint16_t	texture;

// 	texture = (yintercept>>4)&0xfc0;
// 	if (xtilestep == -1)
// 	{
// 		texture = 0xfc0-texture;
// 		xintercept += TILEGLOBAL;
// 	}
// 	wallheight[pixx] = CalcHeight();

// 	if (lastside==1 && lastintercept == xtile && lasttilehit == tilehit)
// 	{
// 		// in the same wall type as last time, so check for optimized draw
// 		if (texture == (uint16_t)postsource)
// 		{
// 		// wide scale
// 			postwidth++;
// 			wallheight[pixx] = wallheight[pixx-1];
// 			return;
// 		}
// 		else
// 		{
// 			ScalePost ();
// 			postsource = texture;
// 			postwidth = 1;
// 			postx = pixx;
// 		}
// 	}
// 	else
// 	{
// 	// _new wall
// 		if (lastside != -1)				// if not the first scaled post
// 			ScalePost ();

// 		lastside = _true;
// 		lastintercept = xtile;

// 		lasttilehit = tilehit;
// 		postx = pixx;
// 		postwidth = 1;

// 		if (tilehit & 0x40)
// 		{								// check for adjacent doors
// 			ytile = yintercept>>TILESHIFT;
// 			if ( tilemap[xtile-xtilestep][ytile]&0x80 )
// 				wallpic = DOORWALL+3;
// 			else
// 				wallpic = vertwall[tilehit & ~0x40];
// 		}
// 		else
// 			wallpic = vertwall[tilehit];

// 		*( ((uintptr_t *)&postsource)+1) = (uintptr_t)PM_GetPage(wallpic);
// 		postsource = texture;

// 	}
// }


/*
====================
=
= HitHorizWall
=
= tilehit bit 7 is 0, because it's not a door tile
= if bit 6 is 1 and the adjacent tile is a door tile, use door side pic
=
====================
*/

// void HitHorizWall (void)
// {
// 	int16_t			wallpic;
// 	uint16_t	texture;

// 	texture = (xintercept>>4)&0xfc0;
// 	if (ytilestep == -1)
// 		yintercept += TILEGLOBAL;
// 	else
// 		texture = 0xfc0-texture;
// 	wallheight[pixx] = CalcHeight();

// 	if (lastside==0 && lastintercept == ytile && lasttilehit == tilehit)
// 	{
// 		// in the same wall type as last time, so check for optimized draw
// 		if (texture == (uint16_t)postsource)
// 		{
// 		// wide scale
// 			postwidth++;
// 			wallheight[pixx] = wallheight[pixx-1];
// 			return;
// 		}
// 		else
// 		{
// 			ScalePost ();
// 			(uint16_t)postsource = texture;
// 			postwidth = 1;
// 			postx = pixx;
// 		}
// 	}
// 	else
// 	{
// 	// _new wall
// 		if (lastside != -1)				// if not the first scaled post
// 			ScalePost ();

// 		lastside = 0;
// 		lastintercept = ytile;

// 		lasttilehit = tilehit;
// 		postx = pixx;
// 		postwidth = 1;

// 		if (tilehit & 0x40)
// 		{								// check for adjacent doors
// 			xtile = xintercept>>TILESHIFT;
// 			if ( tilemap[xtile][ytile-ytilestep]&0x80 )
// 				wallpic = DOORWALL+2;
// 			else
// 				wallpic = horizwall[tilehit & ~0x40];
// 		}
// 		else
// 			wallpic = horizwall[tilehit];

// 		*( ((uint16_t *)&postsource)+1) = (uint16_t)PM_GetPage(wallpic);
// 		(uint16_t)postsource = texture;
// 	}

// }

//==========================================================================

/*
====================
=
= HitHorizDoor
=
====================
*/

// void HitHorizDoor (void)
// {
// 	uint16_t	texture,doorpage,doornum;

// 	doornum = tilehit&0x7f;
// 	texture = ( (xintercept-doorposition[doornum]) >> 4) &0xfc0;

// 	wallheight[pixx] = CalcHeight();

// 	if (lasttilehit == tilehit)
// 	{
// 	// in the same door as last time, so check for optimized draw
// 		if (texture == (uint16_t)postsource)
// 		{
// 		// wide scale
// 			postwidth++;
// 			wallheight[pixx] = wallheight[pixx-1];
// 			return;
// 		}
// 		else
// 		{
// 			ScalePost ();
// 			(uint16_t)postsource = texture;
// 			postwidth = 1;
// 			postx = pixx;
// 		}
// 	}
// 	else
// 	{
// 		if (lastside != -1)				// if not the first scaled post
// 			ScalePost ();			// draw last post
// 	// first pixel in this door
// 		lastside = 2;
// 		lasttilehit = tilehit;
// 		postx = pixx;
// 		postwidth = 1;

// 		switch (doorobjlist[doornum].lock)
// 		{
// 		case dr_normal:
// 			doorpage = DOORWALL;
// 			break;
// 		case dr_lock1:
// 		case dr_lock2:
// 		case dr_lock3:
// 		case dr_lock4:
// 			doorpage = DOORWALL+6;
// 			break;
// 		case dr_elevator:
// 			doorpage = DOORWALL+4;
// 			break;
// 		}

// 		*( ((uint16_t *)&postsource)+1) = (uint16_t)PM_GetPage(doorpage);
// 		(uint16_t)postsource = texture;
// 	}
// }

//==========================================================================

/*
====================
=
= HitVertDoor
=
====================
*/

// void HitVertDoor (void)
// {
// 	uint16_t	texture,doorpage,doornum;

// 	doornum = tilehit&0x7f;
// 	texture = ( (yintercept-doorposition[doornum]) >> 4) &0xfc0;

// 	wallheight[pixx] = CalcHeight();

// 	if (lasttilehit == tilehit)
// 	{
// 	// in the same door as last time, so check for optimized draw
// 		if (texture == (uint16_t)postsource)
// 		{
// 		// wide scale
// 			postwidth++;
// 			wallheight[pixx] = wallheight[pixx-1];
// 			return;
// 		}
// 		else
// 		{
// 			ScalePost ();
// 			(uint16_t)postsource = texture;
// 			postwidth = 1;
// 			postx = pixx;
// 		}
// 	}
// 	else
// 	{
// 		if (lastside != -1)				// if not the first scaled post
// 			ScalePost ();			// draw last post
// 	// first pixel in this door
// 		lastside = 2;
// 		lasttilehit = tilehit;
// 		postx = pixx;
// 		postwidth = 1;

// 		switch (doorobjlist[doornum].lock)
// 		{
// 		case dr_normal:
// 			doorpage = DOORWALL;
// 			break;
// 		case dr_lock1:
// 		case dr_lock2:
// 		case dr_lock3:
// 		case dr_lock4:
// 			doorpage = DOORWALL+6;
// 			break;
// 		case dr_elevator:
// 			doorpage = DOORWALL+4;
// 			break;
// 		}

// 		*( ((uint16_t *)&postsource)+1) = (uint16_t)PM_GetPage(doorpage+1);
// 		(uint16_t)postsource = texture;
// 	}
// }

//==========================================================================


/*
====================
=
= HitHorizPWall
=
= A pushable wall in action has been hit
=
====================
*/

// void HitHorizPWall (void)
// {
// 	int16_t			wallpic;
// 	uint16_t	texture,offset;

// 	texture = (xintercept>>4)&0xfc0;
// 	offset = pwallpos<<10;
// 	if (ytilestep == -1)
// 		yintercept += TILEGLOBAL-offset;
// 	else
// 	{
// 		texture = 0xfc0-texture;
// 		yintercept += offset;
// 	}

// 	wallheight[pixx] = CalcHeight();

// 	if (lasttilehit == tilehit)
// 	{
// 		// in the same wall type as last time, so check for optimized draw
// 		if (texture == (uint16_t)postsource)
// 		{
// 		// wide scale
// 			postwidth++;
// 			wallheight[pixx] = wallheight[pixx-1];
// 			return;
// 		}
// 		else
// 		{
// 			ScalePost ();
// 			(uint16_t)postsource = texture;
// 			postwidth = 1;
// 			postx = pixx;
// 		}
// 	}
// 	else
// 	{
// 	// _new wall
// 		if (lastside != -1)				// if not the first scaled post
// 			ScalePost ();

// 		lasttilehit = tilehit;
// 		postx = pixx;
// 		postwidth = 1;

// 		wallpic = horizwall[tilehit&63];

// 		*( ((uint16_t *)&postsource)+1) = (uint16_t)PM_GetPage(wallpic);
// 		(uint16_t)postsource = texture;
// 	}

// }


/*
====================
=
= HitVertPWall
=
= A pushable wall in action has been hit
=
====================
*/

// void HitVertPWall (void)
// {
// 	int16_t			wallpic;
// 	uint16_t	texture,offset;

// 	texture = (yintercept>>4)&0xfc0;
// 	offset = pwallpos<<10;
// 	if (xtilestep == -1)
// 	{
// 		xintercept += TILEGLOBAL-offset;
// 		texture = 0xfc0-texture;
// 	}
// 	else
// 		xintercept += offset;

// 	wallheight[pixx] = CalcHeight();

// 	if (lasttilehit == tilehit)
// 	{
// 		// in the same wall type as last time, so check for optimized draw
// 		if (texture == (uint16_t)postsource)
// 		{
// 		// wide scale
// 			postwidth++;
// 			wallheight[pixx] = wallheight[pixx-1];
// 			return;
// 		}
// 		else
// 		{
// 			ScalePost ();
// 			(uint16_t)postsource = texture;
// 			postwidth = 1;
// 			postx = pixx;
// 		}
// 	}
// 	else
// 	{
// 	// _new wall
// 		if (lastside != -1)				// if not the first scaled post
// 			ScalePost ();

// 		lasttilehit = tilehit;
// 		postx = pixx;
// 		postwidth = 1;

// 		wallpic = vertwall[tilehit&63];

// 		*( ((uint16_t *)&postsource)+1) = (uint16_t)PM_GetPage(wallpic);
// 		(uint16_t)postsource = texture;
// 	}

// }

//==========================================================================

//==========================================================================

#if 0
/*
=====================
=
= ClearScreen
=
=====================
*/

void ClearScreen (void)
{
 uint16_t floor=egaFloor[gamestate.episode*10+mapon],
	  ceiling=egaCeiling[gamestate.episode*10+mapon];

  //
  // clear the screen
  //
asm	mov	dx,GC_INDEX
asm	mov	ax,GC_MODE + 256*2		// read mode 0, write mode 2
asm	out	dx,ax
asm	mov	ax,GC_BITMASK + 255*256
asm	out	dx,ax

asm	mov	dx,40
asm	mov	ax,[viewwidth]
asm	shr	ax,3
asm	sub	dx,ax					// dx = 40-viewwidth/8

asm	mov	bx,[viewwidth]
asm	shr	bx,4					// bl = viewwidth/16
asm	mov	bh,BYTE PTR [viewheight]
asm	shr	bh,1					// half height

asm	mov	ax,[ceiling]
asm	mov	es,[screenseg]
asm	mov	di,[bufferofs]

toploop:
asm	mov	cl,bl
asm	rep	stosw
asm	add	di,dx
asm	dec	bh
asm	jnz	toploop

asm	mov	bh,BYTE PTR [viewheight]
asm	shr	bh,1					// half height
asm	mov	ax,[floor]

bottomloop:
asm	mov	cl,bl
asm	rep	stosw
asm	add	di,dx
asm	dec	bh
asm	jnz	bottomloop


asm	mov	dx,GC_INDEX
asm	mov	ax,GC_MODE + 256*10		// read mode 1, write mode 2
asm	out	dx,ax
asm	mov	al,GC_BITMASK
asm	out	dx,al

}
#endif
//==========================================================================

uint16_t vgaCeiling[]=
{
#ifndef SPEAR
 0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0xbfbf,
 0x4e4e,0x4e4e,0x4e4e,0x1d1d,0x8d8d,0x4e4e,0x1d1d,0x2d2d,0x1d1d,0x8d8d,
 0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x2d2d,0xdddd,0x1d1d,0x1d1d,0x9898,

 0x1d1d,0x9d9d,0x2d2d,0xdddd,0xdddd,0x9d9d,0x2d2d,0x4d4d,0x1d1d,0xdddd,
 0x7d7d,0x1d1d,0x2d2d,0x2d2d,0xdddd,0xd7d7,0x1d1d,0x1d1d,0x1d1d,0x2d2d,
 0x1d1d,0x1d1d,0x1d1d,0x1d1d,0xdddd,0xdddd,0x7d7d,0xdddd,0xdddd,0xdddd
#else
 0x6f6f,0x4f4f,0x1d1d,0xdede,0xdfdf,0x2e2e,0x7f7f,0x9e9e,0xaeae,0x7f7f,
 0x1d1d,0xdede,0xdfdf,0xdede,0xdfdf,0xdede,0xe1e1,0xdcdc,0x2e2e,0x1d1d,0xdcdc
#endif
};

/*
=====================
=
= VGAClearScreen
=
=====================
*/

// void VGAClearScreen (void)
// {
//  uint16_t ceiling=vgaCeiling[gamestate.episode*10+mapon];

//   //
//   // clear the screen
//   //
// asm	mov	dx,SC_INDEX
// asm	mov	ax,SC_MAPMASK+15*256	// write through all planes
// asm	out	dx,ax

// asm	mov	dx,80
// asm	mov	ax,[viewwidth]
// asm	shr	ax,2
// asm	sub	dx,ax					// dx = 40-viewwidth/2

// asm	mov	bx,[viewwidth]
// asm	shr	bx,3					// bl = viewwidth/8
// asm	mov	bh,BYTE PTR [viewheight]
// asm	shr	bh,1					// half height

// asm	mov	es,[screenseg]
// asm	mov	di,[bufferofs]
// asm	mov	ax,[ceiling]

// toploop:
// asm	mov	cl,bl
// asm	rep	stosw
// asm	add	di,dx
// asm	dec	bh
// asm	jnz	toploop

// asm	mov	bh,BYTE PTR [viewheight]
// asm	shr	bh,1					// half height
// asm	mov	ax,0x1919

// bottomloop:
// asm	mov	cl,bl
// asm	rep	stosw
// asm	add	di,dx
// asm	dec	bh
// asm	jnz	bottomloop
// }

//==========================================================================

/*
=====================
=
= CalcRotate
=
=====================
*/

int16_t	CalcRotate (objtype *ob)
{
	int16_t	angle,viewangle;

	// this isn't exactly correct, as it should vary by a trig value,
	// but it is close enough with only eight rotations

	viewangle = player->angle + (centerx - ob->viewx)/8;

	if (ob->obclass == rocketobj || ob->obclass == hrocketobj)
		angle =  (viewangle-180)- ob->angle;
	else
		angle =  (viewangle-180)- dirangle[ob->dir];

	angle+=ANGLES/16;
	while (angle>=ANGLES)
		angle-=ANGLES;
	while (angle<0)
		angle+=ANGLES;

	if (ob->state->rotate == (_boolean)2)             // 2 rotation pain frame
		return 4*(angle/(ANGLES/2));        // seperated by 3 (art layout...)

	return angle/(ANGLES/8);
}


/*
=====================
=
= DrawScaleds
=
= Draws all objects that are visable
=
=====================
*/

#define MAXVISABLE	1024 // Increased the max so we can render the whole thing

typedef struct
{
	int16_t	viewx,
		viewheight,
		shapenum;
    fixed x, y;
} visobj_t;

visobj_t	vislist[MAXVISABLE],*visptr,*visstep,*farthest;

void AsmRefresh()
{
    int32_t xstep,ystep;
    longword xpartial,ypartial;
    _boolean playerInPushwallBackTile = (tilemap[focaltx][focalty] == 64) ? _true : _false;

    for(pixx=0;pixx<viewwidth;pixx++)
    {
        short angl=midangle+pixelangle[pixx];
        if(angl<0) angl+=FINEANGLES;
        if(angl>=3600) angl-=FINEANGLES;
        if(angl<900)
        {
            xtilestep=1;
            ytilestep=-1;
            xstep=finetangent[900-1-angl];
            ystep=-finetangent[angl];
            xpartial=xpartialup;
            ypartial=ypartialdown;
        }
        else if(angl<1800)
        {
            xtilestep=-1;
            ytilestep=-1;
            xstep=-finetangent[angl-900];
            ystep=-finetangent[1800-1-angl];
            xpartial=xpartialdown;
            ypartial=ypartialdown;
        }
        else if(angl<2700)
        {
            xtilestep=-1;
            ytilestep=1;
            xstep=-finetangent[2700-1-angl];
            ystep=finetangent[angl-1800];
            xpartial=xpartialdown;
            ypartial=ypartialup;
        }
        else if(angl<3600)
        {
            xtilestep=1;
            ytilestep=1;
            xstep=finetangent[angl-2700];
            ystep=finetangent[3600-1-angl];
            xpartial=xpartialup;
            ypartial=ypartialup;
        }
        yintercept=FixedByFrac(ystep,xpartial)+viewy;
        xtile=focaltx+xtilestep;
        xspot=(word)((xtile<<6)+((uint32_t)yintercept>>16));
        xintercept=FixedByFrac(xstep,ypartial)+viewx;
        ytile=focalty+ytilestep;
        yspot=(word)((((uint32_t)xintercept>>16)<<6)+ytile);
        texdelta=0;

        // Special treatment when player is in back tile of pushwall
        if(playerInPushwallBackTile)
        {
            if(    pwalldir == di_east && xtilestep ==  1
                || pwalldir == di_west && xtilestep == -1)
            {
                int32_t yintbuf = yintercept - ((ystep * (64 - pwallpos)) >> 6);
                if((yintbuf >> 16) == focalty)   // ray hits pushwall back?
                {
                    continue;
                }
            }
            else if(pwalldir == di_south && ytilestep ==  1
                ||  pwalldir == di_north && ytilestep == -1)
            {
                int32_t xintbuf = xintercept - ((xstep * (64 - pwallpos)) >> 6);
                if((xintbuf >> 16) == focaltx)   // ray hits pushwall back?
                {
                    continue;
                }
            }
        }

        do
        {
            if(ytilestep==-1 && (yintercept>>16)<=ytile) goto horizentry;
            if(ytilestep==1 && (yintercept>>16)>=ytile) goto horizentry;
vertentry:
            if((uint32_t)yintercept>mapheight*65536-1 || (word)xtile>=mapwidth)
            {
                break;
            }
            if(xspot>=64*64) break;
            tilehit=((byte *)tilemap)[xspot];
            if(tilehit)
            {
                if(tilehit&0x80)
                {
                    int32_t yintbuf=yintercept+(ystep>>1);
                    if((yintbuf>>16)!=(yintercept>>16))
                        goto passvert;
                    if((word)yintbuf<doorposition[tilehit&0x7f])
                        goto passvert;
                }
                else
                {
                    if(tilehit==64)
                    {
                        if(pwalldir==di_west || pwalldir==di_east)
                        {
	                        int32_t yintbuf;
                            int pwallposnorm;
                            int pwallposinv;
                            if(pwalldir==di_west)
                            {
                                pwallposnorm = 64-pwallpos;
                                pwallposinv = pwallpos;
                            }
                            else
                            {
                                pwallposnorm = pwallpos;
                                pwallposinv = 64-pwallpos;
                            }
                            if(pwalldir == di_east && xtile==pwallx && ((uint32_t)yintercept>>16)==pwally
                                || pwalldir == di_west && !(xtile==pwallx && ((uint32_t)yintercept>>16)==pwally))
                            {
                                yintbuf=yintercept+((ystep*pwallposnorm)>>6);
                                if((yintbuf>>16)!=(yintercept>>16))
                                    goto passvert;
                            }
                            else
                            {
                                yintbuf=yintercept+((ystep*pwallposinv)>>6);
                                if((yintbuf>>16)!=(yintercept>>16))
                                    goto passvert;
                            }
                        }
                        else
                        {
                            int pwallposi = pwallpos;
                            if(pwalldir==di_north) pwallposi = 64-pwallpos;
                            if(pwalldir==di_south && (word)yintercept<(pwallposi<<10)
                                || pwalldir==di_north && (word)yintercept>(pwallposi<<10))
                            {
                                if(((uint32_t)yintercept>>16)==pwally && xtile==pwallx)
                                {
                                    if(pwalldir==di_south && (int32_t)((word)yintercept)+ystep<(pwallposi<<10)
                                            || pwalldir==di_north && (int32_t)((word)yintercept)+ystep>(pwallposi<<10))
                                        goto passvert;
                                }
                            }
                            else
                            {
                                if(((uint32_t)yintercept>>16)==pwally && xtile==pwallx)
                                {
                                }
                                else
                                {
                                    if(pwalldir==di_south && (int32_t)((word)yintercept)+ystep>(pwallposi<<10)
                                            || pwalldir==di_north && (int32_t)((word)yintercept)+ystep<(pwallposi<<10))
                                        goto passvert;
                                }
                            }
                        }
                    }
                }
                break;
            }
passvert:
            *((byte *)spotvis+xspot)=1;
            xtile+=xtilestep;
            yintercept+=ystep;
            xspot=(word)((xtile<<6)+((uint32_t)yintercept>>16));
        }
        while(1);
        continue;

        do
        {
            if(xtilestep==-1 && (xintercept>>16)<=xtile) goto vertentry;
            if(xtilestep==1 && (xintercept>>16)>=xtile) goto vertentry;
horizentry:
            if((uint32_t)xintercept>mapwidth*65536-1 || (word)ytile>=mapheight)
            {
                break;
            }
            if(yspot>=64*64) break;
            tilehit=((byte *)tilemap)[yspot];
            if(tilehit)
            {
                if(tilehit&0x80)
                {
                    int32_t xintbuf=xintercept+(xstep>>1);
                    if((xintbuf>>16)!=(xintercept>>16))
                        goto passhoriz;
                    if((word)xintbuf<doorposition[tilehit&0x7f])
                        goto passhoriz;
                }
                else
                {
                    if(tilehit==64)
                    {
                        if(pwalldir==di_north || pwalldir==di_south)
                        {
                            int32_t xintbuf;
                            int pwallposnorm;
                            int pwallposinv;
                            if(pwalldir==di_north)
                            {
                                pwallposnorm = 64-pwallpos;
                                pwallposinv = pwallpos;
                            }
                            else
                            {
                                pwallposnorm = pwallpos;
                                pwallposinv = 64-pwallpos;
                            }
                            if(pwalldir == di_south && ytile==pwally && ((uint32_t)xintercept>>16)==pwallx
                                || pwalldir == di_north && !(ytile==pwally && ((uint32_t)xintercept>>16)==pwallx))
                            {
                                xintbuf=xintercept+((xstep*pwallposnorm)>>6);
                                if((xintbuf>>16)!=(xintercept>>16))
                                    goto passhoriz;
                            }
                            else
                            {
                                xintbuf=xintercept+((xstep*pwallposinv)>>6);
                                if((xintbuf>>16)!=(xintercept>>16))
                                    goto passhoriz;
                            }
                        }
                        else
                        {
                            int pwallposi = pwallpos;
                            if(pwalldir==di_west) pwallposi = 64-pwallpos;
                            if(pwalldir==di_east && (word)xintercept<(pwallposi<<10)
                                    || pwalldir==di_west && (word)xintercept>(pwallposi<<10))
                            {
                                if(((uint32_t)xintercept>>16)==pwallx && ytile==pwally)
                                {
                                    if(pwalldir==di_east && (int32_t)((word)xintercept)+xstep<(pwallposi<<10)
                                            || pwalldir==di_west && (int32_t)((word)xintercept)+xstep>(pwallposi<<10))
                                        goto passhoriz;
                                }
                            }
                            else
                            {
                                if(((uint32_t)xintercept>>16)==pwallx && ytile==pwally)
                                {
                                }
                                else
                                {
                                    if(pwalldir==di_east && (int32_t)((word)xintercept)+xstep>(pwallposi<<10)
                                            || pwalldir==di_west && (int32_t)((word)xintercept)+xstep<(pwallposi<<10))
                                        goto passhoriz;
                                }
                            }
                        }
                    }
                }
                break;
            }
passhoriz:
            *((byte *)spotvis+yspot)=1;
            ytile+=ytilestep;
            xintercept+=xstep;
            yspot=(word)((((uint32_t)xintercept>>16)<<6)+ytile);
        }
        while(1);
    }
}

void Pickups(void)
{
 	int16_t 		i,j,least,numvisable,height;
 	memptr		shape;
 	byte		*tilespot,*visspot;
 	int16_t			shapenum;
 	uint16_t	spotloc;

 	statobj_t	*statptr;
 	objtype		*obj;

 	visptr = &vislist[0];

 //
 // place static objects
 //
 	for (statptr = &statobjlist[0] ; statptr !=laststatobj ; statptr++)
 	{
 		if ((visptr->shapenum = statptr->shapenum) == -1)
 			continue;						// object has been deleted

 		// if (!*statptr->visspot)
 		// 	continue;						// not visable

 		//if (TransformTile (statptr->tilex,statptr->tiley
 		//	,&visptr->viewx,&visptr->viewheight) && statptr->flags & FL_BONUS)
        if (player->tilex == statptr->tilex && 
            player->tiley == statptr->tiley && 
            statptr->flags & FL_BONUS)
 		{
 			GetBonus (statptr);
 			continue;
 		}
    }
}

void DrawScaleds (void)
{
 	int16_t 		i,j,least,numvisable,height;
 	memptr		shape;
 	byte		*tilespot,*visspot;
 	int16_t			shapenum;
 	uint16_t	spotloc;

 	statobj_t	*statptr;
 	objtype		*obj;

 	visptr = &vislist[0];

 //
 // place static objects
 //
 	for (statptr = &statobjlist[0] ; statptr !=laststatobj ; statptr++)
 	{
 		if ((visptr->shapenum = statptr->shapenum) == -1)
 			continue;						// object has been deleted

 		// if (!*statptr->visspot)
 		// 	continue;						// not visable

 		if (TransformTile (statptr->tilex,statptr->tiley
 			,&visptr->viewx,&visptr->viewheight) && statptr->flags & FL_BONUS)
 		{
 			// GetBonus (statptr);
 			// continue;
 		}

 		// if (!visptr->viewheight)
 		// 	continue;						// to close to the object

        visptr->x = (fixed)statptr->tilex * 65536 + 32768;
        visptr->y = (fixed)statptr->tiley * 65536 + 32768;

 		if (visptr < &vislist[MAXVISABLE-1])	// don't let it overflow
 			visptr++;
 	}

 //
 // place active objects
 //
 	for (obj = player->next;obj;obj=obj->next)
 	{
 		if (!(visptr->shapenum = obj->state->shapenum))
 			continue;						// no shape

 		spotloc = (obj->tilex<<6)+obj->tiley;	// optimize: keep in struct?
 		visspot = &spotvis[0][0]+spotloc;
 		tilespot = &tilemap[0][0]+spotloc;

 		/*
 		 could be in any of the nine surrounding tiles
 		*/
 		// if (*visspot
 		// || ( *(visspot-1) && !*(tilespot-1) )
 		// || ( *(visspot+1) && !*(tilespot+1) )
 		// || ( *(visspot-65) && !*(tilespot-65) )
 		// || ( *(visspot-64) && !*(tilespot-64) )
 		// || ( *(visspot-63) && !*(tilespot-63) )
 		// || ( *(visspot+65) && !*(tilespot+65) )
 		// || ( *(visspot+64) && !*(tilespot+64) )
 		// || ( *(visspot+63) && !*(tilespot+63) ) )
 		{
 			obj->active = (activetype)_true;
 			TransformActor (obj);
 			// if (!obj->viewheight)
 			// 	continue;						// too close or  away

            visptr->x = obj->x;
            visptr->y = obj->y;
 			visptr->viewx = obj->viewx;
 			visptr->viewheight = obj->viewheight;
 			if (visptr->shapenum == -1)
 				visptr->shapenum = obj->temp1;	// special shape

 			if (obj->state->rotate)
 				visptr->shapenum += CalcRotate (obj);

 			if (visptr < &vislist[MAXVISABLE-1])	// don't let it overflow
 				visptr++;
 			obj->flags |= FL_VISABLE;
 		}
 		// else
 		// 	obj->flags &= ~FL_VISABLE;
 	}

 //
 // draw from back to front
 //TODO: In our case, front to back is more efficient
 //
 	numvisable = visptr-&vislist[0];

 	if (!numvisable)
 		return;									// no visable objects

 	for (i = 0; i<numvisable; i++)
 	{
 		// least = 32000;
 		// for (visstep=&vislist[0] ; visstep<visptr ; visstep++)
 		// {
 		// 	height = visstep->viewheight;
 		// 	if (height < least)
 		// 	{
 		// 		least = height;
 		// 		farthest = visstep;
 		// 	}
 		// }
 		//
 		// draw farthest
 		//
 		//ScaleShape(farthest->viewx,farthest->shapenum,farthest->viewheight);
        auto s = vislist + i;
        ws_draw_sprite(s->x, s->y, s->shapenum);

 		// farthest->viewheight = 32000;
 	}
}

//==========================================================================

/*
==============
=
= DrawPlayerWeapon
=
= Draw the player's hands
=
==============
*/

int16_t	weaponscale[NUMWEAPONS] = {SPR_KNIFEREADY,SPR_PISTOLREADY
	,SPR_MACHINEGUNREADY,SPR_CHAINREADY};

void DrawPlayerWeapon (void)
{
 	int16_t	shapenum;

 #ifndef SPEAR
 	if (gamestate.victoryflag)
 	{
 		if (player->state == &s_deathcam && (TimeCount&32) )
 			SimpleScaleShape(viewwidth/2,SPR_DEATHCAM,viewheight+1);
 		return;
 	}
 #endif

 	if (gamestate.weapon != (weapontype)-1)
 	{
 		shapenum = weaponscale[gamestate.weapon]+gamestate.weaponframe;
 		SimpleScaleShape(viewwidth/2,shapenum,viewheight+1);
 	}

 	if (demorecord || demoplayback)
 		SimpleScaleShape(viewwidth/2,SPR_DEMO,viewheight+1);
}


//==========================================================================


/*
=====================
=
= CalcTics
=
=====================
*/

void CalcTics (void)
{
	int32_t	newtime,oldtimecount;

//
// calculate tics since last refresh for adaptive timing
//
	if (lasttimecount > TimeCount)
		TimeCount = lasttimecount;		// if the game was paused a LONG time

	do
	{
		newtime = TimeCount;
		tics = newtime-lasttimecount;
        ws_update_sdl();
	} while (!tics);			// make sure at least one tic passes

	lasttimecount = newtime;

#ifdef FILEPROFILE
		strcpy (scratch,"\tTics:");
		itoa (tics,str,10);
		strcat (scratch,str);
		strcat (scratch,"\n");
		file_write (profilehandle,scratch,strlen(scratch));
#endif

	if (tics>MAXTICS)
	{
		TimeCount -= (tics-MAXTICS);
		tics = MAXTICS;
	}
}


//==========================================================================


/*
========================
=
= FixOfs
=
========================
*/

void	FixOfs (void)
{
	// VW_ScreenToScreen (displayofs,bufferofs,viewwidth/8,viewheight);
}


//==========================================================================


/*
====================
=
= WallRefresh
=
====================
*/

inline int wall_at(int x, int y)
{
    if (tilemap[x][y] & 0x80) return 0;
    return tilemap[x][y];
}

inline _boolean is_door_at(int x, int y)
{
    return 
        ((tilemap[x][y] & 0x80) && !(tilemap[x][y] & 0x40))
         ? _true : _false;
}

inline float door_percent(int x, int y)
{
    auto doorNum = tilemap[x][y] & 0x3F;
    return (float)doorposition[doorNum] / (float)0xffff;
}

inline bool has_floor_at(int x, int y)
{
    if (pwallstate && pwallx == x && pwally == y) return true;
    byte num = tilemap[x][y];
    return !num || ((num & 0x80) && !(num & 0x40));
}

void ws_flush();
void WallRefresh (void)
{
//
// set up variables for this view
//
	viewangle = player->angle;
	midangle = viewangle*(FINEANGLES/ANGLES);
	viewsin = sintable[viewangle];
	viewcos = costable[viewangle];
	viewx = player->x - FixedByFrac(focallength,viewcos);
	viewy = player->y + FixedByFrac(focallength,viewsin);

	focaltx = viewx>>TILESHIFT;
	focalty = viewy>>TILESHIFT;

	viewtx = player->x >> TILESHIFT;
	viewty = player->y >> TILESHIFT;

	xpartialdown = viewx&(TILEGLOBAL-1);
	xpartialup = (uint16_t)(TILEGLOBAL-xpartialdown);
	ypartialdown = viewy&(TILEGLOBAL-1);
	ypartialup = (uint16_t)(TILEGLOBAL-ypartialdown);

	lastside = -1;			// the first pixel is on a _new wall
	// AsmRefresh ();
	 //ScalePost ();			// no more optimization on last post

    ws_update_camera();

    //TODO: Batch this for static walls
    for (int x = 1; x < 63; ++x)
    {
        for (int y = 1; y < 63; ++y)
        {
            if (!wall_at(x, y))
            {
                if (int w = wall_at(x, y - 1))
                {
                    if (is_door_at(x, y))
                        w = DOORWALL + 3;
                    else
                        w = horizwall[w & 63];
                    ws_draw_wall((float)x, (float)y, SOUTH, w, false, 
                        wall_at(x - 1, y) ? 1 : (wall_at(x - 1, y - 1) ? 0 : -1), 
                        wall_at(x + 1, y) ? 1 : (wall_at(x + 1, y - 1) ? 0 : -1));
                }
                if (int w = wall_at(x - 1, y))
                {
                    if (is_door_at(x, y))
                        w = DOORWALL + 2;
                    else
                        w = vertwall[w & 63];
                    ws_draw_wall((float)x, (float)y + 1.0f, EAST, w, false,
                        wall_at(x, y + 1) ? 1 : (wall_at(x - 1, y + 1) ? 0 : -1),
                        wall_at(x, y - 1) ? 1 : (wall_at(x - 1, y - 1) ? 0 : -1));
                }
                if (int w = wall_at(x, y + 1))
                {
                    if (is_door_at(x, y))
                        w = DOORWALL + 3;
                    else
                        w = horizwall[w & 63];
                    ws_draw_wall((float)x + 1.0f, (float)y + 1.0f, NORTH, w, false,
                        wall_at(x + 1, y) ? 1 : (wall_at(x + 1, y + 1) ? 0 : -1),
                        wall_at(x - 1, y) ? 1 : (wall_at(x - 1, y + 1) ? 0 : -1));
                }
                if (int w = wall_at(x + 1, y))
                {
                    if (is_door_at(x, y))
                        w = DOORWALL + 2;
                    else
                        w = vertwall[w & 63];
                    ws_draw_wall((float)x + 1.0f, (float)y, WEST, w, false,
                        wall_at(x, y - 1) ? 1 : (wall_at(x + 1, y - 1) ? 0 : -1),
                        wall_at(x, y + 1) ? 1 : (wall_at(x + 1, y + 1) ? 0 : -1));
                }

                if (is_door_at(x, y))
                {
                    auto percent = door_percent(x, y);
                    auto tilehit = tilemap[x][y];
                    uint16_t wallpic, doornum;
                    doornum = tilehit & 0x7f;

                    switch (doorobjlist[doornum].lock)
                    {
                    case dr_normal:
                        wallpic = (PMSpriteStart - 8);
                        break;
                    case dr_lock1:
                    case dr_lock2:
                    case dr_lock3:
                    case dr_lock4:
                        wallpic = (PMSpriteStart - 8) + 6;
                        break;
                    case dr_elevator:
                        wallpic = (PMSpriteStart - 8) + 4;
                        break;
                    }

                    if (wall_at(x - 1, y))
                    {
                        ws_draw_wall((float)x + percent, (float)y + 0.5f, SOUTH, wallpic, true, 0, 0);
                        ws_draw_wall((float)x + 1.0f + percent, (float)y + 0.5f, NORTH, wallpic, true, 0, 0);
                    }
                    else if (wall_at(x, y - 1))
                    {
                        ws_draw_wall((float)x + 0.5f, (float)y + 1.0f + percent, EAST, wallpic, true, 0, 0);
                        ws_draw_wall((float)x + 0.5f, (float)y + percent, WEST, wallpic, true, 0, 0);
                    }
                }
            }
        }
    }

    extern uint16_t vgaCeiling[];
    int ceiling = vgaCeiling[gamestate.episode*10+mapon] & 0xFF;

    // Push walls
    if (pwallstate)
    {
        float percent = (float)pwallpos / 64.0f;
        const float DIROFS[4][2] = {
            {0.0f, -1.0f},
            {1.0f, 0.0f},
            {0.0f, 1.0f},
            {-1.0f, -0.0f} };
        const int DIROFSi[4][2] = {
            {0, -1},
            {1, 0},
            {0, 1},
            {-1, -0} };
        auto &ofs = DIROFS[pwalldir];
        int n = wall_at(pwallx + DIROFSi[pwalldir][0], pwally + DIROFSi[pwalldir][1]);
        int w = horizwall[n & 63];
        ws_draw_wall((float)pwallx + ofs[0] * percent, (float)pwally + 1.0f + ofs[1] * percent, SOUTH, w);
        ws_draw_wall((float)pwallx + 1.0f + ofs[0] * percent, (float)pwally + ofs[1] * percent, NORTH, w);
        w = vertwall[n & 63];
        ws_draw_wall((float)pwallx + 1.0f + ofs[0] * percent, (float)pwally + 1.0f + ofs[1] * percent, EAST, w);
        ws_draw_wall((float)pwallx + ofs[0] * percent, (float)pwally + ofs[1] * percent, WEST, w);
    }

    // Draw ceilling and floor tile by tile (We could have gone with a full map plane, but then we would be depth testing in useless places and its not
    // as nice to look at in freecam. + we need the AO to be done per tile
    bool neighbors[] = { false, false, false, false, false, false, false, false };
    for (int x = 1; x < 63; ++x)
    {
        for (int y = 1; y < 63; ++y)
        {
            if (has_floor_at(x, y))
            {
                neighbors[0] = !has_floor_at(x, y + 1);
                neighbors[1] = !has_floor_at(x + 1, y + 1);
                neighbors[2] = !has_floor_at(x + 1, y);
                neighbors[3] = !has_floor_at(x + 1, y - 1);
                neighbors[4] = !has_floor_at(x, y - 1);
                neighbors[5] = !has_floor_at(x - 1, y - 1);
                neighbors[6] = !has_floor_at(x - 1, y);
                neighbors[7] = !has_floor_at(x - 1, y + 1);

                ws_draw_ceiling(x, y, ceiling, neighbors);
                ws_draw_floor(x, y, 0x19, neighbors);
            }
        }
    }

    // Door overlays
    ws_flush();
    glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    for (int x = 1; x < 63; ++x)
    {
        for (int y = 1; y < 63; ++y)
        {
            if (is_door_at(x, y))
            {
                neighbors[0] = !has_floor_at(x, y + 1);
                neighbors[1] = !has_floor_at(x + 1, y + 1);
                neighbors[2] = !has_floor_at(x + 1, y);
                neighbors[3] = !has_floor_at(x + 1, y - 1);
                neighbors[4] = !has_floor_at(x, y - 1);
                neighbors[5] = !has_floor_at(x - 1, y - 1);
                neighbors[6] = !has_floor_at(x - 1, y);
                neighbors[7] = !has_floor_at(x - 1, y + 1);

                float percent = door_percent(x, y);
                ws_draw_door_ceiling(x, y, ceiling, neighbors, percent);
                ws_draw_door_floor(x, y, 0x19, neighbors, percent);
            }
        }
    }
    ws_flush();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
}

//==========================================================================

/*
========================
=
= ThreeDRefresh
=
========================
*/
void VGAClearScreen(void);
void	ThreeDRefresh (void)
{
    memset(spotvis,0,64*64);

 	int16_t tracedir;

 // this wouldn't need to be done except for my debugger/video wierdness
 	// outportb (SC_INDEX,SC_MAPMASK);

 //
 // clear out the traced array
 //
 // asm	mov	ax,ds
 // asm	mov	es,ax
 // asm	mov	di,OFFSET spotvis
 // asm	xor	ax,ax
 // asm	mov	cx,2048							// 64*64 / 2
 // asm	rep stosw

 	bufferofs += screenofs;

 //
 // follow the walls from there to the right, drawwing as we go
 //
 	 VGAClearScreen ();

 	WallRefresh ();

 //
 // draw all the scaled images
 //
DrawScaleds();			// draw scaled stuff
ws_finish_draw_3d();
Pickups(); // This is like DrawScaleds for statics, but it does the pickup hud update
DrawPlayerWeapon ();	// draw player's hands

 //
 // show screen and time last cycle
 //
 	if (fizzlein)
 	{
 		FizzleFade(bufferofs,displayofs+screenofs,viewwidth,viewheight,20,_false);
 		fizzlein = _false;

 		lasttimecount = TimeCount = 0;		// don't make a big tic count

 	}

 	bufferofs -= screenofs;
 	displayofs = bufferofs;

// 	asm	cli
// 	asm	mov	cx,[displayofs]
// 	asm	mov	dx,3d4h		// CRTC address register
// 	asm	mov	al,0ch		// start address high register
// 	asm	out	dx,al
// 	asm	inc	dx
// 	asm	mov	al,ch
// 	asm	out	dx,al   	// set the high byte
// 	asm	sti

 	bufferofs += SCREENSIZE;
 	if (bufferofs > PAGE3START)
 		bufferofs = PAGE1START;

 	frameon++;
	//PM_NextFrame();
}


//===========================================================================


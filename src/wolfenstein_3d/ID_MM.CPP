// NEWMM.C

/*
=============================================================================

		   ID software memory manager
		   --------------------------

Primary coder: John Carmack

RELIES ON
---------
Quit (char *error) function


WORK TO DO
----------
MM_SizePtr to change the size of a given pointer

Multiple purge levels utilized

EMS / XMS unmanaged routines

=============================================================================
*/

#include "ID_HEADS.H"
#pragma hdrstop

#pragma warn -pro
#pragma warn -use

/*
=============================================================================

							LOCAL INFO

=============================================================================
*/

#define LOCKBIT		0x80	// if set in attributes, block cannot be moved
#define PURGEBITS	3		// 0-3 level, 0= unpurgable, 3= purge first
#define PURGEMASK	0xfffc
#define BASEATTRIBUTES	0	// unlocked, non purgable

#define MAXUMBS		10

#define farfree free

typedef struct mmblockstruct
{
	uint16_t	start,length;
	uint16_t	attributes;
	memptr		*useptr;	// pointer to the segment start
	struct mmblockstruct *next;
} mmblocktype;


//#define GETNEWBLOCK {if(!(mmnew=mmfree))Quit("MM_GETNEWBLOCK: No free blocks!")\
//	;mmfree=mmfree->next;}

#define GETNEWBLOCK {if(!mmfree)MML_ClearBlock();mmnew=mmfree;mmfree=mmfree->next;}

#define FREEBLOCK(x) {*x->useptr=NULL;x->next=mmfree;mmfree=x;}

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

mminfotype	mminfo;
memptr		bufferseg;
_boolean		mmerror;

void		(* beforesort) (void);
void		(* aftersort) (void);

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

_boolean		mmstarted;

void    	*farheap;
void		*nearheap;

mmblocktype	 mmblocks[MAXBLOCKS]
			, *mmhead, *mmfree, *mmrover, *mmnew;

_boolean		bombonerror;

//uint16_t	totalEMSpages,freeEMSpages,EMSpageframe,EMSpagesmapped,EMShandle;

void		(* XMSaddr) (void);		//  pointer to XMS driver

uint16_t	numUMBs,UMBbase[MAXUMBS];

//==========================================================================

//
// local prototypes
//

_boolean		MML_CheckForEMS (void);
void 		MML_ShutdownEMS (void);
void 		MM_MapEMS (void);
// _boolean 	MML_CheckForXMS (void);
void 		MML_ShutdownXMS (void);
void		MML_UseSpace (uint16_t segstart, uint16_t seglength);
void 		MML_ClearBlock (void);

//==========================================================================

/*
======================
=
= MML_CheckForXMS
=
= Check for XMM driver
=
=======================
*/

// _boolean MML_CheckForXMS (void)
// {
// 	numUMBs = 0;

// asm {
// 	mov	ax,0x4300
// 	int16_t	0x2f				// query status of installed diver
// 	cmp	al,0x80
// 	je	good
// 	}

// 	return _false;
// good:
// 	return _true;
// }


// /*
// ======================
// =
// = MML_SetupXMS
// =
// = Try to allocate all upper memory block
// =
// =======================
// */

// void MML_SetupXMS (void)
// {
// 	uint16_t	base,size;

// asm	{
// 	mov	ax,0x4310
// 	int16_t	0x2f
// 	mov	[WORD PTR XMSaddr],bx
// 	mov	[WORD PTR XMSaddr+2],es		// function pointer to XMS driver
// 	}

// getmemory:
// asm	{
// 	mov	ah,XMS_ALLOCUMB
// 	mov	dx,0xffff					// try for largest block possible
// 	call	[DWORD PTR XMSaddr]
// 	or	ax,ax
// 	jnz	gotone

// 	cmp	bl,0xb0						// error: smaller UMB is available
// 	jne	done;

// 	mov	ah,XMS_ALLOCUMB
// 	call	[DWORD PTR XMSaddr]		// DX holds largest available UMB
// 	or	ax,ax
// 	jz	done						// another error...
// 	}

// gotone:
// asm	{
// 	mov	[base],bx
// 	mov	[size],dx
// 	}
// 	MML_UseSpace (base,size);
// 	mminfo.XMSmem += size*16;
// 	UMBbase[numUMBs] = base;
// 	numUMBs++;
// 	if (numUMBs < MAXUMBS)
// 		goto getmemory;

// done:;
// }


// /*
// ======================
// =
// = MML_ShutdownXMS
// =
// ======================
// */

// void MML_ShutdownXMS (void)
// {
// 	int16_t	i;
// 	uint16_t	base;

// 	for (i=0;i<numUMBs;i++)
// 	{
// 		base = UMBbase[i];

// asm	mov	ah,XMS_FREEUMB
// asm	mov	dx,[base]
// asm	call	[DWORD PTR XMSaddr]
// 	}
// }

//==========================================================================

/*
======================
=
= MML_UseSpace
=
= Marks a range of paragraphs as usable by the memory manager
= This is used to mark space for the  heap,  heap, ems page frame,
= and upper memory blocks
=
======================
*/

void MML_UseSpace (uint16_t segstart, uint16_t seglength)
{
// 	mmblocktype  *scan, *last;
// 	uint16_t	oldend;
// 	int32_t		extra;

// 	scan = last = mmhead;
// 	mmrover = mmhead;		// reset rover to start of memory

// //
// // search for the block that contains the range of segments
// //
// 	while (scan->start+scan->length < segstart)
// 	{
// 		last = scan;
// 		scan = scan->next;
// 	}

// //
// // take the given range out of the block
// //
// 	oldend = scan->start + scan->length;
// 	extra = oldend - (segstart+seglength);
// 	if (extra < 0)
// 		Quit ("MML_UseSpace: Segment spans two blocks!");

// 	if (segstart == scan->start)
// 	{
// 		last->next = scan->next;			// unlink block
// 		FREEBLOCK(scan);
// 		scan = last;
// 	}
// 	else
// 		scan->length = segstart-scan->start;	// shorten block

// 	if (extra > 0)
// 	{
// 		GETNEWBLOCK;
// 		mmnew->useptr = NULL;

// 		mmnew->next = scan->next;
// 		scan->next = mmnew;
// 		mmnew->start = segstart+seglength;
// 		mmnew->length = extra;
// 		mmnew->attributes = LOCKBIT;
// 	}

}

//==========================================================================

/*
====================
=
= MML_ClearBlock
=
= We are out of blocks, so free a purgable block
=
====================
*/

 void MML_ClearBlock (void)
 {
// 	mmblocktype  *scan, *last;

// 	scan = mmhead->next;

// 	while (scan)
// 	{
// 		if (!(scan->attributes&LOCKBIT) && (scan->attributes&PURGEBITS) )
// 		{
// 			MM_FreePtr(scan->useptr);
// 			return;
// 		}
// 		scan = scan->next;
// 	}

// 	Quit ("MM_ClearBlock: No purgable blocks!");
 }


//==========================================================================

/*
===================
=
= MM_Startup
=
= Grabs all space from turbo with malloc/farmalloc
= Allocates bufferseg misc buffer
=
===================
*/

static	char *ParmStrings[] = {(char*)"noems",(char*)"noxms",(char*)""};

void MM_Startup (void)
{
 	int16_t i;
 	uint32_t length;
 	void  	*start;
 	uint16_t 	segstart,seglength,endfree;

 	if (mmstarted)
 		MM_Shutdown ();


 	mmstarted = _true;
 	bombonerror = _true;
 //
 // set up the linked list (everything in the free list;
 //
 	mmhead = NULL;
 	mmfree = &mmblocks[0];
 	for (i=0;i<MAXBLOCKS-1;i++)
 		mmblocks[i].next = &mmblocks[i+1];
 	mmblocks[i].next = NULL;

 //
 // locked block of all memory until we punch out free space
 //
 	GETNEWBLOCK;
 	mmhead = mmnew;				// this will allways be the first node
 	mmnew->start = 0;
 	mmnew->length = 0xffff;
 	mmnew->attributes = LOCKBIT;
 	mmnew->next = NULL;
 	mmrover = mmhead;


 //
 // get all available  conventional memory segments
 //
 	length=64 * 1024 * 1024;
 	nearheap = malloc(length);

 	//length -= 16-(FP_OFF(start)&15);
 	//length -= SAVENEARHEAP;
 	//seglength = length / 16;			// now in paragraphs
 	//segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
 	//MML_UseSpace (segstart,seglength);
 	mminfo.nearheap = length; // Pretend we have 64 MB

 //
 // get all available  conventional memory segments
 //
 	length= 64 * 1024 * 1024;
    farheap = malloc(length);
 	//start = farheap = farmalloc(length);
 	//length -= 16-(FP_OFF(start)&15);
 	//length -= SAVEFARHEAP;
 	//seglength = length / 16;			// now in paragraphs
 	//segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
 	//MML_UseSpace (segstart,seglength);
 	mminfo.farheap = length;
 	mminfo.mainmem = mminfo.nearheap + mminfo.farheap;

 //
 // allocate the misc buffer
 //
 	mmrover = mmhead;		// start looking for space after low block

	MM_GetPtr (&bufferseg,BUFFERSIZE);
}

//==========================================================================

/*
====================
=
= MM_Shutdown
=
= Frees all conventional, EMS, and XMS allocated
=
====================
*/

void MM_Shutdown (void)
{
//   if (!mmstarted)
// 	return;

//   farfree (farheap);
//   free (nearheap);
//  MML_ShutdownXMS ();
}

//==========================================================================

/*
====================
=
= MM_GetPtr
=
= Allocates an unlocked, unpurgable block
=
====================
*/

void MM_GetPtr (memptr *baseptr,uint32_t size)
{
    auto ptr = malloc(size);
    if (!ptr) Quit ((char*)"MM_GetPtr: Out of memory!");
    *baseptr = ptr;
    mmerror = _false;
// 	mmblocktype  *scan, *lastscan, *endscan
// 				, *purge, *next;
// 	int16_t			search;
// 	uint16_t	needed,startseg;

// 	needed = (size+15)/16;		// convert size from bytes to paragraphs

// 	GETNEWBLOCK;				// fill in start and next after a spot is found
// 	mmnew->length = needed;
// 	mmnew->useptr = baseptr;
// 	mmnew->attributes = BASEATTRIBUTES;

// tryagain:
// 	for (search = 0; search<3; search++)
// 	{
// 	//
// 	// first search:	try to allocate right after the rover, then on up
// 	// second search: 	search from the head pointer up to the rover
// 	// third search:	compress memory, then scan from start
// 		if (search == 1 && mmrover == mmhead)
// 			search++;

// 		switch (search)
// 		{
// 		case 0:
// 			lastscan = mmrover;
// 			scan = mmrover->next;
// 			endscan = NULL;
// 			break;
// 		case 1:
// 			lastscan = mmhead;
// 			scan = mmhead->next;
// 			endscan = mmrover;
// 			break;
// 		case 2:
// 			MM_SortMem ();
// 			lastscan = mmhead;
// 			scan = mmhead->next;
// 			endscan = NULL;
// 			break;
// 		}

// 		startseg = lastscan->start + lastscan->length;

// 		while (scan != endscan)
// 		{
// 			if (scan->start - startseg >= needed)
// 			{
// 			//
// 			// got enough space between the end of lastscan and
// 			// the start of scan, so throw out anything in the middle
// 			// and allocate the _new block
// 			//
// 				purge = lastscan->next;
// 				lastscan->next = mmnew;
// 				mmnew->start = *(uint16_t *)baseptr = startseg;
// 				mmnew->next = scan;
// 				while ( purge != scan)
// 				{	// free the purgable block
// 					next = purge->next;
// 					FREEBLOCK(purge);
// 					purge = next;		// purge another if not at scan
// 				}
// 				mmrover = mmnew;
// 				return;	// good allocation!
// 			}

// 			//
// 			// if this block is purge level zero or locked, skip past it
// 			//
// 			if ( (scan->attributes & LOCKBIT)
// 				|| !(scan->attributes & PURGEBITS) )
// 			{
// 				lastscan = scan;
// 				startseg = lastscan->start + lastscan->length;
// 			}


// 			scan=scan->next;		// look at next line
// 		}
// 	}

// 	if (bombonerror)
// 	{

// extern char configname[];
// extern	_boolean	insetupscaling;
// extern	int16_t	viewsize;
// _boolean SetViewSize (uint16_t width, uint16_t height);
// #define HEIGHTRATIO		0.50
// //
// // wolf hack -- size the view down
// //
// 		if (!insetupscaling && viewsize>10)
// 		{
// mmblocktype	 *savedmmnew;
// 			savedmmnew = mmnew;
// 			viewsize -= 2;
// 			SetViewSize (viewsize*16,viewsize*16*HEIGHTRATIO);
// 			mmnew = savedmmnew;
// 			goto tryagain;
// 		}

// //		unlink(configname);
// 		Quit ("MM_GetPtr: Out of memory!");
// 	}
// 	else
// 		mmerror = _true;
}

//==========================================================================

/*
====================
=
= MM_FreePtr
=
= Deallocates an unlocked, purgable block
=
====================
*/

void MM_FreePtr (memptr *baseptr)
{
    free(*baseptr);
    // delete[] (uint8_t*)*baseptr;
    *baseptr = nullptr;
	// mmblocktype  *scan, *last;

	// last = mmhead;
	// scan = last->next;

	// if (baseptr == mmrover->useptr)	// removed the last allocated block
	// 	mmrover = mmhead;

	// while (scan->useptr != baseptr && scan)
	// {
	// 	last = scan;
	// 	scan = scan->next;
	// }

	// if (!scan)
	// 	Quit ("MM_FreePtr: Block not found!");

	// last->next = scan->next;

	// FREEBLOCK(scan);
}
//==========================================================================

/*
=====================
=
= MM_SetPurge
=
= Sets the purge level for a block (locked blocks cannot be made purgable)
=
=====================
*/

void MM_SetPurge (memptr *baseptr, int16_t purge)
{
	// mmblocktype  *start;

	// start = mmrover;

	// do
	// {
	// 	if (mmrover->useptr == baseptr)
	// 		break;

	// 	mmrover = mmrover->next;

	// 	if (!mmrover)
	// 		mmrover = mmhead;
	// 	else if (mmrover == start)
	// 		Quit ("MM_SetPurge: Block not found!");

	// } while (1);

	// mmrover->attributes &= ~PURGEBITS;
	// mmrover->attributes |= purge;
}

//==========================================================================

/*
=====================
=
= MM_SetLock
=
= Locks / unlocks the block
=
=====================
*/

void MM_SetLock (memptr *baseptr, _boolean locked)
{
	// mmblocktype  *start;

	// start = mmrover;

	// do
	// {
	// 	if (mmrover->useptr == baseptr)
	// 		break;

	// 	mmrover = mmrover->next;

	// 	if (!mmrover)
	// 		mmrover = mmhead;
	// 	else if (mmrover == start)
	// 		Quit ("MM_SetLock: Block not found!");

	// } while (1);

	// mmrover->attributes &= ~LOCKBIT;
	// mmrover->attributes |= locked*LOCKBIT;
}

//==========================================================================

/*
=====================
=
= MM_SortMem
=
= Throws out all purgable stuff and compresses movable blocks
=
=====================
*/

void MM_SortMem (void)
{
	// mmblocktype  *scan, *last, *next;
	// uint16_t	start,length,source,dest;
	// int16_t			playing;

	// //
	// // lock down a currently playing sound
	// //
	// playing = SD_SoundPlaying ();
	// if (playing)
	// {
	// 	switch (SoundMode)
	// 	{
	// 	case sdm_PC:
	// 		playing += STARTPCSOUNDS;
	// 		break;
	// 	case sdm_AdLib:
	// 		playing += STARTADLIBSOUNDS;
	// 		break;
	// 	}
	// 	MM_SetLock(&(memptr)audiosegs[playing],_true);
	// }


	// SD_StopSound();

	// if (beforesort)
	// 	beforesort();

	// scan = mmhead;

	// last = NULL;		// shut up compiler warning

	// while (scan)
	// {
	// 	if (scan->attributes & LOCKBIT)
	// 	{
	// 	//
	// 	// block is locked, so try to pile later blocks right after it
	// 	//
	// 		start = scan->start + scan->length;
	// 	}
	// 	else
	// 	{
	// 		if (scan->attributes & PURGEBITS)
	// 		{
	// 		//
	// 		// throw out the purgable block
	// 		//
	// 			next = scan->next;
	// 			FREEBLOCK(scan);
	// 			last->next = next;
	// 			scan = next;
	// 			continue;
	// 		}
	// 		else
	// 		{
	// 		//
	// 		// push the non purgable block on top of the last moved block
	// 		//
	// 			if (scan->start != start)
	// 			{
	// 				length = scan->length;
	// 				source = scan->start;
	// 				dest = start;
	// 				while (length > 0xf00)
	// 				{
	// 					movedata(source,0,dest,0,0xf00*16);
	// 					length -= 0xf00;
	// 					source += 0xf00;
	// 					dest += 0xf00;
	// 				}
	// 				movedata(source,0,dest,0,length*16);

	// 				scan->start = start;
	// 				*(uint16_t *)scan->useptr = start;
	// 			}
	// 			start = scan->start + scan->length;
	// 		}
	// 	}

	// 	last = scan;
	// 	scan = scan->next;		// go to next block
	// }

	// mmrover = mmhead;

	// if (aftersort)
	// 	aftersort();

	// if (playing)
	// 	MM_SetLock(&(memptr)audiosegs[playing],_false);
}


//==========================================================================

/*
=====================
=
= MM_ShowMemory
=
=====================
*/

void MM_ShowMemory (void)
{
	// mmblocktype  *scan;
	// uint16_t color,temp,x,y;
	// int32_t	end,owner;
	// char    scratch[80],str[10];

	// temp = bufferofs;
	// bufferofs = displayofs;
	// scan = mmhead;

	// end = -1;

	// while (scan)
	// {
	// 	if (scan->attributes & PURGEBITS)
	// 		color = 5;		// dark purple = purgable
	// 	else
	// 		color = 9;		// medium blue = non purgable
	// 	if (scan->attributes & LOCKBIT)
	// 		color = 12;		// red = locked
	// 	if (scan->start<=end)
	// 		Quit ("MM_ShowMemory: Memory block order currupted!");
	// 	end = scan->length-1;
	// 	y = scan->start/320;
	// 	x = scan->start%320;
	// 	VW_Hlin(x,x+end,y,color);
	// 	VW_Plot(x,y,15);
	// 	if (scan->next && scan->next->start > end+1)
	// 		VW_Hlin(x+end+1,x+(scan->next->start-scan->start),y,0);	// black = free

	// 	scan = scan->next;
	// }

	// VW_FadeIn ();
	// IN_Ack();

	// bufferofs = temp;
}

//==========================================================================

/*
=====================
=
= MM_DumpData
=
=====================
*/

void MM_DumpData (void)
{
	// mmblocktype  *scan, *best;
	// int32_t	lowest,oldlowest;
	// uint16_t	owner;
	// char	lock,purge;
	// FILE	*dumpfile;


	// free (nearheap);
	// dumpfile = fopen ("MMDUMP.TXT","w");
	// if (!dumpfile)
	// 	Quit ("MM_DumpData: Couldn't open MMDUMP.TXT!");

	// lowest = -1;
	// do
	// {
	// 	oldlowest = lowest;
	// 	lowest = 0xffff;

	// 	scan = mmhead;
	// 	while (scan)
	// 	{
	// 		owner = (uint16_t)scan->useptr;

	// 		if (owner && owner<lowest && owner > oldlowest)
	// 		{
	// 			best = scan;
	// 			lowest = owner;
	// 		}

	// 		scan = scan->next;
	// 	}

	// 	if (lowest != 0xffff)
	// 	{
	// 		if (best->attributes & PURGEBITS)
	// 			purge = 'P';
	// 		else
	// 			purge = '-';
	// 		if (best->attributes & LOCKBIT)
	// 			lock = 'L';
	// 		else
	// 			lock = '-';
	// 		fprintf (dumpfile,"0x%p (%c%c) = %u\n"
	// 		,(uint16_t)lowest,lock,purge,best->length);
	// 	}

	// } while (lowest != 0xffff);

	// fclose (dumpfile);
	// Quit ("MMDUMP.TXT created.");
}

//==========================================================================


/*
======================
=
= MM_UnusedMemory
=
= Returns the total free space without purging
=
======================
*/

int32_t MM_UnusedMemory (void)
{
	// uint16_t free;
	// mmblocktype  *scan;

	// free = 0;
	// scan = mmhead;

	// while (scan->next)
	// {
	// 	free += scan->next->start - (scan->start + scan->length);
	// 	scan = scan->next;
	// }

	// return free*16l;
    return 128 * 1024 * 1024; // Just always report there's 128 MB left
}

//==========================================================================


/*
======================
=
= MM_TotalFree
=
= Returns the total free space with purging
=
======================
*/

int32_t MM_TotalFree (void)
{
	// uint16_t free;
	// mmblocktype  *scan;

	// free = 0;
	// scan = mmhead;

	// while (scan->next)
	// {
	// 	if ((scan->attributes&PURGEBITS) && !(scan->attributes&LOCKBIT))
	// 		free += scan->length;
	// 	free += scan->next->start - (scan->start + scan->length);
	// 	scan = scan->next;
	// }

	// return free*16l;
    return 128 * 1024 * 1024; // Just always report there's 128 MB left
}

//==========================================================================

/*
=====================
=
= MM_BombOnError
=
=====================
*/

void MM_BombOnError (_boolean bomb)
{
	bombonerror = bomb;
}



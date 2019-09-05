// ID_CA.H
//===========================================================================

#define NUMMAPS		60
#define MAPPLANES	2

#define UNCACHEGRCHUNK(chunk)	{MM_FreePtr(&grsegs[chunk]);grneeded[chunk]&=~ca_levelbit;}

//===========================================================================

typedef	struct
{
	long		planestart[3];
	unsigned	planelength[3];
	unsigned	width,height;
	char		name[16];
} maptype;

//===========================================================================

extern	char		audioname[13];

extern	byte 		*tinf;
extern	int			mapon;

extern	unsigned	*mapsegs[MAPPLANES];
extern	maptype		*mapheaderseg[NUMMAPS];
extern	byte		*audiosegs[NUMSNDCHUNKS];
extern	void		*grsegs[NUMCHUNKS];

extern	byte		grneeded[NUMCHUNKS];
extern	byte		ca_levelbit,ca_levelnum;

extern	char		*titleptr[8];

extern	int			profilehandle,debughandle;

extern	char		extension[5],
			gheadname[10],
			gfilename[10],
			gdictname[10],
			mheadname[10],
			mfilename[10],
			aheadname[10],
			afilename[10];

extern long	 *grstarts;	// array of offsets in egagraph, -1 for sparse
extern long	 *audiostarts;	// array of offsets in audio / audiot
//
// hooks for custom cache dialogs
//
extern	void	(*drawcachebox)		(char *title, unsigned numcache);
extern	void	(*updatecachebox)	(void);
extern	void	(*finishcachebox)	(void);

//===========================================================================

// just for the score box reshifting

void CAL_ShiftSprite (unsigned segment,unsigned source,unsigned dest,
	unsigned width, unsigned height, unsigned pixshift);

//===========================================================================

void CA_OpenDebug (void);
void CA_CloseDebug (void);
boolean CA_FarRead (int handle, byte *dest, long length);
boolean CA_FarWrite (int handle, byte *source, long length);
boolean CA_ReadFile (char *filename, memptr *ptr);
boolean CA_LoadFile (char *filename, memptr *ptr);
boolean CA_WriteFile (char *filename, void *ptr, long length);

long CA_RLEWCompress (unsigned *source, long length, unsigned *dest,
  unsigned rlewtag);

void CA_RLEWexpand (unsigned *source, unsigned *dest,long length,
  unsigned rlewtag);

void CA_Startup (void);
void CA_Shutdown (void);

void CA_SetGrPurge (void);
void CA_CacheAudioChunk (int chunk);
void CA_LoadAllSounds (void);

void CA_UpLevel (void);
void CA_DownLevel (void);

void CA_SetAllPurge (void);

void CA_ClearMarks (void);
void CA_ClearAllMarks (void);

#define CA_MarkGrChunk(chunk)	grneeded[chunk]|=ca_levelbit

void CA_CacheGrChunk (int chunk);
void CA_CacheMap (int mapnum);

void CA_CacheMarks (void);

void CA_CacheScreen (int chunk);

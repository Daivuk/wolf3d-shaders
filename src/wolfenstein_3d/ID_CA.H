// ID_CA.H
//===========================================================================

#define NUMMAPS		60
#define MAPPLANES	2

#define UNCACHEGRCHUNK(chunk)	{MM_FreePtr(&grsegs[chunk]);grneeded[chunk]&=~ca_levelbit;}

//===========================================================================

typedef	struct
{
	int32_t		planestart[3];
	uint16_t	planelength[3];
	uint16_t	width,height;
	char		name[16];
} maptype;

//===========================================================================

extern	char		audioname[13];

extern	byte 		*tinf;
extern	int16_t			mapon;

extern	uint16_t	*mapsegs[MAPPLANES];
extern	maptype		*mapheaderseg[NUMMAPS];
extern	byte		*audiosegs[NUMSNDCHUNKS];
extern	void		*grsegs[NUMCHUNKS];

extern	byte		grneeded[NUMCHUNKS];
extern	byte		ca_levelbit,ca_levelnum;

extern	char		*titleptr[8];

extern	int16_t			profilehandle,debughandle;

extern	char		extension[5],
			gheadname[10],
			gfilename[10],
			gdictname[10],
			mheadname[10],
			mfilename[10],
			aheadname[10],
			afilename[10];

extern int32_t	 *grstarts;	// array of offsets in egagraph, -1 for sparse
extern int32_t	 *audiostarts;	// array of offsets in audio / audiot
//
// hooks for custom cache dialogs
//
extern	void	(*drawcachebox)		(char *title, uint16_t numcache);
extern	void	(*updatecachebox)	(void);
extern	void	(*finishcachebox)	(void);

//===========================================================================

// just for the score box reshifting

void CAL_ShiftSprite (uint16_t segment,uint16_t source,uint16_t dest,
	uint16_t width, uint16_t height, uint16_t pixshift);

//===========================================================================

void CA_OpenDebug (void);
void CA_CloseDebug (void);
_boolean CA_FarRead (int16_t handle, byte *dest, int32_t length);
_boolean CA_FarWrite (int16_t handle, byte *source, int32_t length);
_boolean CA_ReadFile (char *filename, memptr *ptr);
_boolean CA_LoadFile (char *filename, memptr *ptr);
_boolean CA_WriteFile (char *filename, void *ptr, int32_t length);

int32_t CA_RLEWCompress (uint16_t *source, int32_t length, uint16_t *dest,
  uint16_t rlewtag);

void CA_RLEWexpand (uint16_t *source, uint16_t *dest,int32_t length,
  uint16_t rlewtag);

void CA_Startup (void);
void CA_Shutdown (void);

void CA_SetGrPurge (void);
void CA_CacheAudioChunk (int16_t chunk);
void CA_LoadAllSounds (void);

void CA_UpLevel (void);
void CA_DownLevel (void);

void CA_SetAllPurge (void);

void CA_ClearMarks (void);
void CA_ClearAllMarks (void);

#define CA_MarkGrChunk(chunk)	grneeded[chunk]|=ca_levelbit

void CA_CacheGrChunk (int16_t chunk);
void CA_CacheMap (int16_t mapnum);

void CA_CacheMarks (void);

void CA_CacheScreen (int16_t chunk);

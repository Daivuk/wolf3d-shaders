//
//	ID Engine
//	ID_US.h - Header file for the User Manager
//	v1.0d1
//	By Jason Blochowiak
//

#ifndef	__ID_US__
#define	__ID_US__

#include <stdio.h>

#ifdef	__DEBUG__
#define	__DEBUG_UserMgr__
#endif

//#define	HELPTEXTLINKED

#define	MaxX	320
#define	MaxY	200

#define	MaxHelpLines	500

#define	MaxHighName	57
#define	MaxScores	7
typedef	struct
		{
			char	name[MaxHighName + 1];
			int32_t	score;
			word	completed,episode;
		} HighScore;

#define	MaxGameName		32
#define	MaxSaveGames	6
typedef	struct
		{
			char	signature[4];
			word	*oldtest;
			_boolean	present;
			char	name[MaxGameName + 1];
		} SaveGame;

#define	MaxString	128	// Maximum input string size

typedef	struct
		{
			int16_t	x,y,
				w,h,
				px,py;
		} WindowRec;	// Record used to save & restore screen windows

typedef	enum
		{
			gd_Continue,
			gd_Easy,
			gd_Normal,
			gd_Hard
		} GameDiff;

//	Hack import for TED launch support
extern	_boolean		tedlevel;
extern	int16_t			tedlevelnum;
extern	void		TEDDeath(void);

extern	_boolean		ingame,		// Set by game code if a game is in progress
					abortgame,	// Set if a game load failed
					loadedgame,	// Set if the current game was loaded
					NoWait,
					HighScoresDirty;
extern	char		*abortprogram;	// Set to error msg if program is dying
extern	GameDiff	restartgame;	// Normally gd_Continue, else starts game
extern	word		PrintX,PrintY;	// Current printing location in the window
extern	word		WindowX,WindowY,// Current location of window
					WindowW,WindowH;// Current size of window

extern	_boolean		Button0,Button1,
					CursorBad;
extern	int16_t			CursorX,CursorY;

extern	void		(*USL_MeasureString)(char *,word *,word *),
					(*USL_DrawString)(char *);

extern	_boolean		(*USL_SaveGame)(int16_t),(*USL_LoadGame)(int16_t);
extern	void		(*USL_ResetGame)(void);
extern	SaveGame	Games[MaxSaveGames];
extern	HighScore	Scores[];

#define	US_HomeWindow()	{PrintX = WindowX; PrintY = WindowY;}

extern	void	US_Startup(void),
				US_Setup(void),
				US_Shutdown(void),
				US_InitRndT(_boolean randomize),
				US_SetLoadSaveHooks(_boolean (*load)(int16_t),
									_boolean (*save)(int16_t),
									void (*reset)(void)),
				US_TextScreen(void),
				US_UpdateTextScreen(void),
				US_FinishTextScreen(void),
				US_DrawWindow(word x,word y,word w,word h),
				US_CenterWindow(word,word),
				US_SaveWindow(WindowRec *win),
				US_RestoreWindow(WindowRec *win),
				US_ClearWindow(void),
				US_SetPrintRoutines(void (*measure)(char *,word *,word *),
									void (*print)(char *)),
				US_PrintCentered(char *s),
				US_CPrint(char *s),
				US_CPrintLine(char *s),
				US_Print(char *s),
				US_PrintUnsigned(longword n),
				US_PrintSigned(int32_t n),
				US_StartCursor(void),
				US_ShutCursor(void),
				US_CheckHighScore(int32_t score,word other),
				US_DisplayHighScores(int16_t which);
extern	_boolean	US_UpdateCursor(void),
				US_LineInput(int16_t x,int16_t y,char *buf,char *def,_boolean escok,
								int16_t maxchars,int16_t maxwidth);
extern	int16_t		US_CheckParm(char *parm,char **strings),
				US_RndT(void);

		void	USL_PrintInCenter(char *s,Rect r);
		char 	*USL_GiveSaveName(word game);
#endif

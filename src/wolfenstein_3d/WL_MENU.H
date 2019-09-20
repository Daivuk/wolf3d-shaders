#ifndef SPEAR_H_INCLUDED
#define SPEAR_H_INCLUDED

//
// WL_MENU.H
//
#ifdef SPEAR

#define BORDCOLOR	0x99
#define BORD2COLOR	0x93
#define DEACTIVE	0x9b
#define BKGDCOLOR	0x9d
//#define STRIPE		0x9c

#define MenuFadeOut()	VL_FadeOut(0,255,0,0,51,10)

#else

#define BORDCOLOR	0x29
#define BORD2COLOR	0x23
#define DEACTIVE	0x2b
#define BKGDCOLOR	0x2d
#define STRIPE		0x2c

#define MenuFadeOut()	VL_FadeOut(0,255,43,0,0,10)

#endif

#define READCOLOR	0x4a
#define READHCOLOR	0x47
#define VIEWCOLOR	0x7f
#define TEXTCOLOR	0x17
#define HIGHLIGHT	0x13
#define MenuFadeIn()	VL_FadeIn(0,255,(byte*)gamepal,10)


#define MENUSONG	WONDERIN_MUS

#ifndef SPEAR
#define INTROSONG	NAZI_NOR_MUS
#else
#define INTROSONG	XTOWER2_MUS
#endif

#define SENSITIVE	60
#define CENTER		SENSITIVE*2

#define MENU_X	76
#define MENU_Y	55
#define MENU_W	178
#ifndef SPEAR
#define MENU_H	13*9+6
#else
#define MENU_H	13*9+6
#endif

#define SM_X	40
#define SM_W	250

#define SM_Y1	63
#define SM_H1	4*13-7
#define SM_Y2	SM_Y1+5*13
#define SM_H2	4*13-7
#define SM_Y3	SM_Y2+5*13
#define SM_H3	3*13-7

#define CTL_X	24
#define CTL_Y	70
#define CTL_W	284
#define CTL_H	13*7-7

#define LSM_X	85
#define LSM_Y	55
#define LSM_W	175
#define LSM_H	10*13+10

#define NM_X	50
#define NM_Y	100
#define NM_W	225
#define NM_H	13*4+15

#define NE_X	10
#define NE_Y	23
#define NE_W	320-NE_X*2
#define NE_H	200-NE_Y*2

#define CST_X		20
#define CST_Y		48
#define CST_START	60
#define CST_SPC	60


//
// TYPEDEFS
//
typedef struct {
		int16_t x,y,amount,curpos,indent;
		} CP_iteminfo;

typedef struct {
		int16_t active;
		char string[36];
		void (* routine)(int16_t temp1);
		} CP_itemtype;

typedef struct {
		int16_t allowed[4];
		} CustomCtrls;

extern CP_itemtype MainMenu[], NewEMenu[];
extern CP_iteminfo MainItems;

//
// FUNCTION PROTOTYPES
//
void SetupControlPanel(void);
void CleanupControlPanel(void);

void DrawMenu(CP_iteminfo *item_i,CP_itemtype *items);
int16_t  HandleMenu(CP_iteminfo *item_i,
		CP_itemtype *items,
		void (*routine)(int16_t w));
void ClearMScreen(void);
void DrawWindow(int16_t x,int16_t y,int16_t w,int16_t h,int16_t wcolor);
void DrawOutline(int16_t x,int16_t y,int16_t w,int16_t h,int16_t color1,int16_t color2);
void WaitKeyUp(void);
void ReadAnyControl(ControlInfo *ci);
void TicDelay(int16_t count);
void CacheLump(int16_t lumpstart,int16_t lumpend);
void UnCacheLump(int16_t lumpstart,int16_t lumpend);
void StartCPMusic(int16_t song);
int16_t  Confirm(char *string);
void Message(char *string);
void CheckPause(void);
void ShootSnd(void);
void CheckSecretMissions(void);
void BossKey(void);

void DrawGun(CP_iteminfo *item_i,CP_itemtype *items,int16_t x,int16_t *y,int16_t which,int16_t basey,void (*routine)(int16_t w));
void DrawHalfStep(int16_t x,int16_t y);
void EraseGun(CP_iteminfo *item_i,CP_itemtype *items,int16_t x,int16_t y,int16_t which);
void SetTextColor(CP_itemtype *items,int16_t hlight);
void DrawMenuGun(CP_iteminfo *iteminfo);
void DrawStripes(int16_t y);

void DefineMouseBtns(void);
void DefineJoyBtns(void);
void DefineKeyBtns(void);
void DefineKeyMove(void);
void EnterCtrlData(int16_t index,CustomCtrls *cust,void (*DrawRtn)(int16_t),void (*PrintRtn)(int16_t),int16_t type);

void DrawMainMenu(void);
void DrawSoundMenu(void);
void DrawLoadSaveScreen(int16_t loadsave);
void DrawNewEpisode(void);
void DrawNewGame(void);
void DrawChangeView(int16_t view);
void DrawMouseSens(void);
void DrawCtlScreen(void);
void DrawCustomScreen(void);
void DrawLSAction(int16_t which);
void DrawCustMouse(int16_t hilight);
void DrawCustJoy(int16_t hilight);
void DrawCustKeybd(int16_t hilight);
void DrawCustKeys(int16_t hilight);
void PrintCustMouse(int16_t i);
void PrintCustJoy(int16_t i);
void PrintCustKeybd(int16_t i);
void PrintCustKeys(int16_t i);

void PrintLSEntry(int16_t w,int16_t color);
void TrackWhichGame(int16_t w);
void DrawNewGameDiff(int16_t w);
void FixupCustom(int16_t w);

void CP_NewGame(void);
void CP_Sound(void);
int16_t  CP_LoadGame(int16_t quick);
int16_t  CP_SaveGame(int16_t quick);
void CP_Control(void);
void CP_Graphics(void);
void CP_ChangeView(void);
void CP_ExitOptions(void);
void CP_Quit(void);
void CP_ViewScores(void);
int16_t  CP_EndGame(void);
int16_t  CP_CheckQuick(uint16_t scancode);
void CustomControls(void);
void MouseSensitivity(void);
void SoundVolume(void);
void MusicVolume(void);

void CheckForEpisodes(void);

//
// VARIABLES
//
extern int16_t SaveGamesAvail[10],StartGame,SoundStatus;
extern char SaveGameNames[10][32],SaveName[13];

enum {MOUSE,JOYSTICK,KEYBOARDBTNS,KEYBOARDMOVE};	// FOR INPUT TYPES

enum menuitems
{
	newgame,
	loadgame,
	savegame,
	soundmenu,
	control,
    graphics,
	//changeview,

#ifndef GOODTIMES
#ifndef SPEAR
	readthis,
#endif
#endif

	viewscores,
	backtodemo,
	quit
};

//
// WL_INTER
//
typedef struct {
		int16_t kill,secret,treasure;
		int32_t time;
		} LRstruct;

extern LRstruct LevelRatios[];

void Write (int16_t x,int16_t y,char *string);
void NonShareware(void);
int16_t GetYorN(int16_t x,int16_t y,int16_t pic);

#endif /* SPEAR_H_INCLUDED */

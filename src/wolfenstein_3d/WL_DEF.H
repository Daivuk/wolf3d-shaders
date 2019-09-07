//#define BETA
#define YEAR	1992
#define MONTH	9
#define DAY		30

#include "ID_HEADS.H"
#include <cmath>
// #include <MATH.H>
// #include <VALUES.H>

#include "WL_MENU.H"

#ifdef SPANISH
#include "SPANISH.H"
#else
#include "FOREIGN.H"
#endif

#ifdef SPEAR
#include "F_SPEAR.H"
#endif

/*
=============================================================================

							MACROS

=============================================================================
*/


#define COLORBORDER(color)		asm{mov	dx,STATUS_REGISTER_1;in al,dx;\
	mov dx,ATR_INDEX;mov al,ATR_OVERSCAN;out dx,al;mov al,color;out	dx,al;\
	mov	al,32;out dx,al};

#define MAPSPOT(x,y,plane)		(*(mapsegs[plane]+farmapylookup[y]+x))

#define SIGN(x) 	((x)>0?1:-1)
#define ABS(x) 		((int16_t)(x)>0?(x):-(x))
#define LABS(x) 	((int32_t)(x)>0?(x):-(x))

/*
=============================================================================

						 GLOBAL CONSTANTS

=============================================================================
*/

#define MAXACTORS		150				// max number of nazis, etc / map
#define MAXSTATS		400				// max number of lamps, bonus, etc
#define MAXDOORS		64				// max number of sliding doors
#define MAXWALLTILES	64				// max number of wall tiles

//
// tile constants
//

#define	ICONARROWS		90
#define PUSHABLETILE	98
#define EXITTILE		99				// at end of castle
#define AREATILE		107				// first of NUMAREAS floor tiles
#define NUMAREAS		37
#define ELEVATORTILE	21
#define AMBUSHTILE		106
#define	ALTELEVATORTILE	107

#define NUMBERCHARS	9


//----------------

#define EXTRAPOINTS		40000

#define PLAYERSPEED		3000
#define RUNSPEED   		6000

#define	SCREENSEG		0xa000

#define SCREENBWIDE		80

#define HEIGHTRATIO		0.50		// also defined in id_mm.c

#define BORDERCOLOR	3
#define FLASHCOLOR	5
#define FLASHTICS	4


#define PLAYERSIZE		MINDIST			// player radius
#define MINACTORDIST	0x10000l		// minimum dist from player center
										// to any actor center

#define NUMLATCHPICS	100


#define PI	3.141592657

#define GLOBAL1		(1l<<16)
#define TILEGLOBAL  GLOBAL1
#define PIXGLOBAL	(GLOBAL1/64)
#define TILESHIFT		16l
#define UNSIGNEDSHIFT	8

#define ANGLES		360					// must be divisable by 4
#define ANGLEQUAD	(ANGLES/4)
#define FINEANGLES	3600
#define ANG90		(FINEANGLES/4)
#define ANG180		(ANG90*2)
#define ANG270		(ANG90*3)
#define ANG360		(ANG90*4)
#define VANG90		(ANGLES/4)
#define VANG180		(VANG90*2)
#define VANG270		(VANG90*3)
#define VANG360		(VANG90*4)

#define MINDIST		(0x5800l)


#define	MAXSCALEHEIGHT	256				// largest scale on largest view

#define MAXVIEWWIDTH		320

#define MAPSIZE		64					// maps are 64*64 max
#define NORTH	0
#define EAST	1
#define SOUTH	2
#define WEST	3


#define STATUSLINES		40

#define SCREENSIZE		(SCREENBWIDE*208)
#define PAGE1START		0
#define PAGE2START		(SCREENSIZE)
#define PAGE3START		(SCREENSIZE*2u)
#define	FREESTART		(SCREENSIZE*3u)


#define PIXRADIUS		512

#define STARTAMMO		8


// object flag values

#define FL_SHOOTABLE	1
#define FL_BONUS		2
#define FL_NEVERMARK	4
#define FL_VISABLE		8
#define FL_ATTACKMODE	16
#define FL_FIRSTATTACK	32
#define FL_AMBUSH		64
#define FL_NONMARK		128


//
// sprite constants
//

enum	{
		SPR_DEMO,
		SPR_DEATHCAM,
//
// static sprites
//
		SPR_STAT_0,SPR_STAT_1,SPR_STAT_2,SPR_STAT_3,
		SPR_STAT_4,SPR_STAT_5,SPR_STAT_6,SPR_STAT_7,

		SPR_STAT_8,SPR_STAT_9,SPR_STAT_10,SPR_STAT_11,
		SPR_STAT_12,SPR_STAT_13,SPR_STAT_14,SPR_STAT_15,

		SPR_STAT_16,SPR_STAT_17,SPR_STAT_18,SPR_STAT_19,
		SPR_STAT_20,SPR_STAT_21,SPR_STAT_22,SPR_STAT_23,

		SPR_STAT_24,SPR_STAT_25,SPR_STAT_26,SPR_STAT_27,
		SPR_STAT_28,SPR_STAT_29,SPR_STAT_30,SPR_STAT_31,

		SPR_STAT_32,SPR_STAT_33,SPR_STAT_34,SPR_STAT_35,
		SPR_STAT_36,SPR_STAT_37,SPR_STAT_38,SPR_STAT_39,

		SPR_STAT_40,SPR_STAT_41,SPR_STAT_42,SPR_STAT_43,
		SPR_STAT_44,SPR_STAT_45,SPR_STAT_46,SPR_STAT_47,

#ifdef SPEAR
		SPR_STAT_48,SPR_STAT_49,SPR_STAT_50,SPR_STAT_51,
#endif

//
// guard
//
		SPR_GRD_S_1,SPR_GRD_S_2,SPR_GRD_S_3,SPR_GRD_S_4,
		SPR_GRD_S_5,SPR_GRD_S_6,SPR_GRD_S_7,SPR_GRD_S_8,

		SPR_GRD_W1_1,SPR_GRD_W1_2,SPR_GRD_W1_3,SPR_GRD_W1_4,
		SPR_GRD_W1_5,SPR_GRD_W1_6,SPR_GRD_W1_7,SPR_GRD_W1_8,

		SPR_GRD_W2_1,SPR_GRD_W2_2,SPR_GRD_W2_3,SPR_GRD_W2_4,
		SPR_GRD_W2_5,SPR_GRD_W2_6,SPR_GRD_W2_7,SPR_GRD_W2_8,

		SPR_GRD_W3_1,SPR_GRD_W3_2,SPR_GRD_W3_3,SPR_GRD_W3_4,
		SPR_GRD_W3_5,SPR_GRD_W3_6,SPR_GRD_W3_7,SPR_GRD_W3_8,

		SPR_GRD_W4_1,SPR_GRD_W4_2,SPR_GRD_W4_3,SPR_GRD_W4_4,
		SPR_GRD_W4_5,SPR_GRD_W4_6,SPR_GRD_W4_7,SPR_GRD_W4_8,

		SPR_GRD_PAIN_1,SPR_GRD_DIE_1,SPR_GRD_DIE_2,SPR_GRD_DIE_3,
		SPR_GRD_PAIN_2,SPR_GRD_DEAD,

		SPR_GRD_SHOOT1,SPR_GRD_SHOOT2,SPR_GRD_SHOOT3,

//
// dogs
//
		SPR_DOG_W1_1,SPR_DOG_W1_2,SPR_DOG_W1_3,SPR_DOG_W1_4,
		SPR_DOG_W1_5,SPR_DOG_W1_6,SPR_DOG_W1_7,SPR_DOG_W1_8,

		SPR_DOG_W2_1,SPR_DOG_W2_2,SPR_DOG_W2_3,SPR_DOG_W2_4,
		SPR_DOG_W2_5,SPR_DOG_W2_6,SPR_DOG_W2_7,SPR_DOG_W2_8,

		SPR_DOG_W3_1,SPR_DOG_W3_2,SPR_DOG_W3_3,SPR_DOG_W3_4,
		SPR_DOG_W3_5,SPR_DOG_W3_6,SPR_DOG_W3_7,SPR_DOG_W3_8,

		SPR_DOG_W4_1,SPR_DOG_W4_2,SPR_DOG_W4_3,SPR_DOG_W4_4,
		SPR_DOG_W4_5,SPR_DOG_W4_6,SPR_DOG_W4_7,SPR_DOG_W4_8,

		SPR_DOG_DIE_1,SPR_DOG_DIE_2,SPR_DOG_DIE_3,SPR_DOG_DEAD,
		SPR_DOG_JUMP1,SPR_DOG_JUMP2,SPR_DOG_JUMP3,



//
// ss
//
		SPR_SS_S_1,SPR_SS_S_2,SPR_SS_S_3,SPR_SS_S_4,
		SPR_SS_S_5,SPR_SS_S_6,SPR_SS_S_7,SPR_SS_S_8,

		SPR_SS_W1_1,SPR_SS_W1_2,SPR_SS_W1_3,SPR_SS_W1_4,
		SPR_SS_W1_5,SPR_SS_W1_6,SPR_SS_W1_7,SPR_SS_W1_8,

		SPR_SS_W2_1,SPR_SS_W2_2,SPR_SS_W2_3,SPR_SS_W2_4,
		SPR_SS_W2_5,SPR_SS_W2_6,SPR_SS_W2_7,SPR_SS_W2_8,

		SPR_SS_W3_1,SPR_SS_W3_2,SPR_SS_W3_3,SPR_SS_W3_4,
		SPR_SS_W3_5,SPR_SS_W3_6,SPR_SS_W3_7,SPR_SS_W3_8,

		SPR_SS_W4_1,SPR_SS_W4_2,SPR_SS_W4_3,SPR_SS_W4_4,
		SPR_SS_W4_5,SPR_SS_W4_6,SPR_SS_W4_7,SPR_SS_W4_8,

		SPR_SS_PAIN_1,SPR_SS_DIE_1,SPR_SS_DIE_2,SPR_SS_DIE_3,
		SPR_SS_PAIN_2,SPR_SS_DEAD,

		SPR_SS_SHOOT1,SPR_SS_SHOOT2,SPR_SS_SHOOT3,

//
// mutant
//
		SPR_MUT_S_1,SPR_MUT_S_2,SPR_MUT_S_3,SPR_MUT_S_4,
		SPR_MUT_S_5,SPR_MUT_S_6,SPR_MUT_S_7,SPR_MUT_S_8,

		SPR_MUT_W1_1,SPR_MUT_W1_2,SPR_MUT_W1_3,SPR_MUT_W1_4,
		SPR_MUT_W1_5,SPR_MUT_W1_6,SPR_MUT_W1_7,SPR_MUT_W1_8,

		SPR_MUT_W2_1,SPR_MUT_W2_2,SPR_MUT_W2_3,SPR_MUT_W2_4,
		SPR_MUT_W2_5,SPR_MUT_W2_6,SPR_MUT_W2_7,SPR_MUT_W2_8,

		SPR_MUT_W3_1,SPR_MUT_W3_2,SPR_MUT_W3_3,SPR_MUT_W3_4,
		SPR_MUT_W3_5,SPR_MUT_W3_6,SPR_MUT_W3_7,SPR_MUT_W3_8,

		SPR_MUT_W4_1,SPR_MUT_W4_2,SPR_MUT_W4_3,SPR_MUT_W4_4,
		SPR_MUT_W4_5,SPR_MUT_W4_6,SPR_MUT_W4_7,SPR_MUT_W4_8,

		SPR_MUT_PAIN_1,SPR_MUT_DIE_1,SPR_MUT_DIE_2,SPR_MUT_DIE_3,
		SPR_MUT_PAIN_2,SPR_MUT_DIE_4,SPR_MUT_DEAD,

		SPR_MUT_SHOOT1,SPR_MUT_SHOOT2,SPR_MUT_SHOOT3,SPR_MUT_SHOOT4,

//
// officer
//
		SPR_OFC_S_1,SPR_OFC_S_2,SPR_OFC_S_3,SPR_OFC_S_4,
		SPR_OFC_S_5,SPR_OFC_S_6,SPR_OFC_S_7,SPR_OFC_S_8,

		SPR_OFC_W1_1,SPR_OFC_W1_2,SPR_OFC_W1_3,SPR_OFC_W1_4,
		SPR_OFC_W1_5,SPR_OFC_W1_6,SPR_OFC_W1_7,SPR_OFC_W1_8,

		SPR_OFC_W2_1,SPR_OFC_W2_2,SPR_OFC_W2_3,SPR_OFC_W2_4,
		SPR_OFC_W2_5,SPR_OFC_W2_6,SPR_OFC_W2_7,SPR_OFC_W2_8,

		SPR_OFC_W3_1,SPR_OFC_W3_2,SPR_OFC_W3_3,SPR_OFC_W3_4,
		SPR_OFC_W3_5,SPR_OFC_W3_6,SPR_OFC_W3_7,SPR_OFC_W3_8,

		SPR_OFC_W4_1,SPR_OFC_W4_2,SPR_OFC_W4_3,SPR_OFC_W4_4,
		SPR_OFC_W4_5,SPR_OFC_W4_6,SPR_OFC_W4_7,SPR_OFC_W4_8,

		SPR_OFC_PAIN_1,SPR_OFC_DIE_1,SPR_OFC_DIE_2,SPR_OFC_DIE_3,
		SPR_OFC_PAIN_2,SPR_OFC_DIE_4,SPR_OFC_DEAD,

		SPR_OFC_SHOOT1,SPR_OFC_SHOOT2,SPR_OFC_SHOOT3,

#ifndef SPEAR
//
// ghosts
//
		SPR_BLINKY_W1,SPR_BLINKY_W2,SPR_PINKY_W1,SPR_PINKY_W2,
		SPR_CLYDE_W1,SPR_CLYDE_W2,SPR_INKY_W1,SPR_INKY_W2,

//
// hans
//
		SPR_BOSS_W1,SPR_BOSS_W2,SPR_BOSS_W3,SPR_BOSS_W4,
		SPR_BOSS_SHOOT1,SPR_BOSS_SHOOT2,SPR_BOSS_SHOOT3,SPR_BOSS_DEAD,

		SPR_BOSS_DIE1,SPR_BOSS_DIE2,SPR_BOSS_DIE3,

//
// schabbs
//
		SPR_SCHABB_W1,SPR_SCHABB_W2,SPR_SCHABB_W3,SPR_SCHABB_W4,
		SPR_SCHABB_SHOOT1,SPR_SCHABB_SHOOT2,

		SPR_SCHABB_DIE1,SPR_SCHABB_DIE2,SPR_SCHABB_DIE3,SPR_SCHABB_DEAD,
		SPR_HYPO1,SPR_HYPO2,SPR_HYPO3,SPR_HYPO4,

//
// fake
//
		SPR_FAKE_W1,SPR_FAKE_W2,SPR_FAKE_W3,SPR_FAKE_W4,
		SPR_FAKE_SHOOT,SPR_FIRE1,SPR_FIRE2,

		SPR_FAKE_DIE1,SPR_FAKE_DIE2,SPR_FAKE_DIE3,SPR_FAKE_DIE4,
		SPR_FAKE_DIE5,SPR_FAKE_DEAD,

//
// hitler
//
		SPR_MECHA_W1,SPR_MECHA_W2,SPR_MECHA_W3,SPR_MECHA_W4,
		SPR_MECHA_SHOOT1,SPR_MECHA_SHOOT2,SPR_MECHA_SHOOT3,SPR_MECHA_DEAD,

		SPR_MECHA_DIE1,SPR_MECHA_DIE2,SPR_MECHA_DIE3,

		SPR_HITLER_W1,SPR_HITLER_W2,SPR_HITLER_W3,SPR_HITLER_W4,
		SPR_HITLER_SHOOT1,SPR_HITLER_SHOOT2,SPR_HITLER_SHOOT3,SPR_HITLER_DEAD,

		SPR_HITLER_DIE1,SPR_HITLER_DIE2,SPR_HITLER_DIE3,SPR_HITLER_DIE4,
		SPR_HITLER_DIE5,SPR_HITLER_DIE6,SPR_HITLER_DIE7,

//
// giftmacher
//
		SPR_GIFT_W1,SPR_GIFT_W2,SPR_GIFT_W3,SPR_GIFT_W4,
		SPR_GIFT_SHOOT1,SPR_GIFT_SHOOT2,

		SPR_GIFT_DIE1,SPR_GIFT_DIE2,SPR_GIFT_DIE3,SPR_GIFT_DEAD,
#endif
//
// Rocket, smoke and small explosion
//
		SPR_ROCKET_1,SPR_ROCKET_2,SPR_ROCKET_3,SPR_ROCKET_4,
		SPR_ROCKET_5,SPR_ROCKET_6,SPR_ROCKET_7,SPR_ROCKET_8,

		SPR_SMOKE_1,SPR_SMOKE_2,SPR_SMOKE_3,SPR_SMOKE_4,
		SPR_BOOM_1,SPR_BOOM_2,SPR_BOOM_3,

//
// Angel of Death's DeathSparks(tm)
//
#ifdef SPEAR
		SPR_HROCKET_1,SPR_HROCKET_2,SPR_HROCKET_3,SPR_HROCKET_4,
		SPR_HROCKET_5,SPR_HROCKET_6,SPR_HROCKET_7,SPR_HROCKET_8,

		SPR_HSMOKE_1,SPR_HSMOKE_2,SPR_HSMOKE_3,SPR_HSMOKE_4,
		SPR_HBOOM_1,SPR_HBOOM_2,SPR_HBOOM_3,

		SPR_SPARK1,SPR_SPARK2,SPR_SPARK3,SPR_SPARK4,
#endif

#ifndef SPEAR
//
// gretel
//
		SPR_GRETEL_W1,SPR_GRETEL_W2,SPR_GRETEL_W3,SPR_GRETEL_W4,
		SPR_GRETEL_SHOOT1,SPR_GRETEL_SHOOT2,SPR_GRETEL_SHOOT3,SPR_GRETEL_DEAD,

		SPR_GRETEL_DIE1,SPR_GRETEL_DIE2,SPR_GRETEL_DIE3,

//
// fat face
//
		SPR_FAT_W1,SPR_FAT_W2,SPR_FAT_W3,SPR_FAT_W4,
		SPR_FAT_SHOOT1,SPR_FAT_SHOOT2,SPR_FAT_SHOOT3,SPR_FAT_SHOOT4,

		SPR_FAT_DIE1,SPR_FAT_DIE2,SPR_FAT_DIE3,SPR_FAT_DEAD,

//
// bj
//
		SPR_BJ_W1,SPR_BJ_W2,SPR_BJ_W3,SPR_BJ_W4,
		SPR_BJ_JUMP1,SPR_BJ_JUMP2,SPR_BJ_JUMP3,SPR_BJ_JUMP4,
#else
//
// THESE ARE FOR 'SPEAR OF DESTINY'
//

//
// Trans Grosse
//
		SPR_TRANS_W1,SPR_TRANS_W2,SPR_TRANS_W3,SPR_TRANS_W4,
		SPR_TRANS_SHOOT1,SPR_TRANS_SHOOT2,SPR_TRANS_SHOOT3,SPR_TRANS_DEAD,

		SPR_TRANS_DIE1,SPR_TRANS_DIE2,SPR_TRANS_DIE3,

//
// Wilhelm
//
		SPR_WILL_W1,SPR_WILL_W2,SPR_WILL_W3,SPR_WILL_W4,
		SPR_WILL_SHOOT1,SPR_WILL_SHOOT2,SPR_WILL_SHOOT3,SPR_WILL_SHOOT4,

		SPR_WILL_DIE1,SPR_WILL_DIE2,SPR_WILL_DIE3,SPR_WILL_DEAD,

//
// UberMutant
//
		SPR_UBER_W1,SPR_UBER_W2,SPR_UBER_W3,SPR_UBER_W4,
		SPR_UBER_SHOOT1,SPR_UBER_SHOOT2,SPR_UBER_SHOOT3,SPR_UBER_SHOOT4,

		SPR_UBER_DIE1,SPR_UBER_DIE2,SPR_UBER_DIE3,SPR_UBER_DIE4,
		SPR_UBER_DEAD,

//
// Death Knight
//
		SPR_DEATH_W1,SPR_DEATH_W2,SPR_DEATH_W3,SPR_DEATH_W4,
		SPR_DEATH_SHOOT1,SPR_DEATH_SHOOT2,SPR_DEATH_SHOOT3,SPR_DEATH_SHOOT4,

		SPR_DEATH_DIE1,SPR_DEATH_DIE2,SPR_DEATH_DIE3,SPR_DEATH_DIE4,
		SPR_DEATH_DIE5,SPR_DEATH_DIE6,SPR_DEATH_DEAD,

//
// Ghost
//
		SPR_SPECTRE_W1,SPR_SPECTRE_W2,SPR_SPECTRE_W3,SPR_SPECTRE_W4,
		SPR_SPECTRE_F1,SPR_SPECTRE_F2,SPR_SPECTRE_F3,SPR_SPECTRE_F4,

//
// Angel of Death
//
		SPR_ANGEL_W1,SPR_ANGEL_W2,SPR_ANGEL_W3,SPR_ANGEL_W4,
		SPR_ANGEL_SHOOT1,SPR_ANGEL_SHOOT2,SPR_ANGEL_TIRED1,SPR_ANGEL_TIRED2,

		SPR_ANGEL_DIE1,SPR_ANGEL_DIE2,SPR_ANGEL_DIE3,SPR_ANGEL_DIE4,
		SPR_ANGEL_DIE5,SPR_ANGEL_DIE6,SPR_ANGEL_DIE7,SPR_ANGEL_DEAD,

#endif

//
// player attack frames
//
		SPR_KNIFEREADY,SPR_KNIFEATK1,SPR_KNIFEATK2,SPR_KNIFEATK3,
		SPR_KNIFEATK4,

		SPR_PISTOLREADY,SPR_PISTOLATK1,SPR_PISTOLATK2,SPR_PISTOLATK3,
		SPR_PISTOLATK4,

		SPR_MACHINEGUNREADY,SPR_MACHINEGUNATK1,SPR_MACHINEGUNATK2,MACHINEGUNATK3,
		SPR_MACHINEGUNATK4,

		SPR_CHAINREADY,SPR_CHAINATK1,SPR_CHAINATK2,SPR_CHAINATK3,
		SPR_CHAINATK4,

		};


/*
=============================================================================

						   GLOBAL TYPES

=============================================================================
*/

typedef int32_t fixed;

typedef enum {
	di_north,
	di_east,
	di_south,
	di_west
} controldir_t;

typedef enum {
	dr_normal,
	dr_lock1,
	dr_lock2,
	dr_lock3,
	dr_lock4,
	dr_elevator
} door_t;

typedef enum {
	ac_badobject = -1,
	ac_no,
	ac_yes,
	ac_allways
} activetype;

typedef enum {
	nothing,
	playerobj,
	inertobj,
	guardobj,
	officerobj,
	ssobj,
	dogobj,
	bossobj,
	schabbobj,
	fakeobj,
	mechahitlerobj,
	mutantobj,
	needleobj,
	fireobj,
	bjobj,
	ghostobj,
	realhitlerobj,
	gretelobj,
	giftobj,
	fatobj,
	rocketobj,

	spectreobj,
	angelobj,
	transobj,
	uberobj,
	willobj,
	deathobj,
	hrocketobj,
	sparkobj
} classtype;

typedef enum {
	dressing,
	block,
	bo_gibs,
	bo_alpo,
	bo_firstaid,
	bo_key1,
	bo_key2,
	bo_key3,
	bo_key4,
	bo_cross,
	bo_chalice,
	bo_bible,
	bo_crown,
	bo_clip,
	bo_clip2,
	bo_machinegun,
	bo_chaingun,
	bo_food,
	bo_fullheal,
	bo_25clip,
	bo_spear
} stat_t;

typedef enum {
	east,
	northeast,
	north,
	northwest,
	west,
	southwest,
	south,
	southeast,
	nodir
} dirtype;


#define NUMENEMIES		22
typedef enum {
	en_guard,
	en_officer,
	en_ss,
	en_dog,
	en_boss,
	en_schabbs,
	en_fake,
	en_hitler,
	en_mutant,
	en_blinky,
	en_clyde,
	en_pinky,
	en_inky,
	en_gretel,
	en_gift,
	en_fat,
	en_spectre,
	en_angel,
	en_trans,
	en_uber,
	en_will,
	en_death
} enemy_t;


typedef struct	statestruct
{
	_boolean	rotate;
	int16_t		shapenum;			// a shapenum of -1 means get from ob->temp1
	int16_t		tictime;
	void	(*think) (),(*action) ();
	struct	statestruct	*next;
} statetype;


//---------------------
//
// trivial actor structure
//
//---------------------

typedef struct statstruct
{
	byte	tilex,tiley;
	byte	*visspot;
	int16_t		shapenum;			// if shapenum == -1 the obj has been removed
	byte	flags;
	byte	itemnumber;
} statobj_t;


//---------------------
//
// door actor structure
//
//---------------------

typedef struct doorstruct
{
	byte	tilex,tiley;
	_boolean	vertical;
	byte	lock;
	enum	{dr_open,dr_closed,dr_opening,dr_closing}	action;
	int16_t		ticcount;
} doorobj_t;


//--------------------
//
// thinking actor structure
//
//--------------------

typedef struct objstruct
{
	activetype	active;
	int16_t			ticcount;
	classtype	obclass;
	statetype	*state;

	byte		flags;				//	FL_SHOOTABLE, etc

	int32_t		distance;			// if negative, wait for that door to open
	dirtype		dir;

	fixed 		x,y;
	uint16_t	tilex,tiley;
	byte		areanumber;

	int16_t	 		viewx;
	uint16_t	viewheight;
	fixed		transx,transy;		// in global coord

	int16_t 		angle;
	int16_t			hitpoints;
	int32_t		speed;

	int16_t			temp1,temp2,temp3;
	struct		objstruct	*next,*prev;
} objtype;


#define NUMBUTTONS	8
enum	{
	bt_nobutton=-1,
	bt_attack=0,
	bt_strafe,
	bt_run,
	bt_use,
	bt_readyknife,
	bt_readypistol,
	bt_readymachinegun,
	bt_readychaingun
};


#define NUMWEAPONS	5
typedef enum	{
	wp_knife,
	wp_pistol,
	wp_machinegun,
	wp_chaingun
} weapontype;


typedef enum	{
	gd_baby,
	gd_easy,
	gd_medium,
	gd_hard
} _DifficultyMode;

//---------------
//
// gamestate structure
//
//---------------

typedef	struct
{
	int16_t			difficulty;
	int16_t			mapon;
	int32_t		oldscore,score,nextextra;
	int16_t			lives;
	int16_t			health;
	int16_t			ammo;
	int16_t			keys;
	weapontype		bestweapon,weapon,chosenweapon;

	int16_t			faceframe;
	int16_t			attackframe,attackcount,weaponframe;

	int16_t			episode,secretcount,treasurecount,killcount,
				secrettotal,treasuretotal,killtotal;
	int32_t		TimeCount;
	int32_t		killx,killy;
	_boolean		victoryflag;		// set during victory animations
} gametype;


typedef	enum	{
	ex_stillplaying,
	ex_completed,
	ex_died,
	ex_warped,
	ex_resetgame,
	ex_loadedgame,
	ex_victorious,
	ex_abort,
	ex_demodone,
	ex_secretlevel
} exit_t;


/*
=============================================================================

						 WL_MAIN DEFINITIONS

=============================================================================
*/

extern	_boolean		MS_CheckParm (char *string);

extern	char		str[80],str2[20];
extern	int16_t			tedlevelnum;
extern	_boolean		tedlevel;
extern	_boolean		nospr;
extern	_boolean		IsA386;

extern	byte	*scalermemory;

extern	fixed		focallength;
extern	uint16_t	viewangles;
extern	uint16_t	screenofs;
extern	int16_t		    viewwidth;
extern	int16_t			viewheight;
extern	int16_t			centerx;
extern	int16_t			shootdelta;

extern	int16_t			dirangle[9];

extern	_boolean         startgame,loadedgame,virtualreality;
extern	int16_t		mouseadjustment;
//
// math tables
//
extern	int16_t			pixelangle[MAXVIEWWIDTH];
extern	int32_t		finetangent[FINEANGLES/4];
extern	fixed 		sintable[],*costable;

//
// derived constants
//
extern	fixed 	scale,maxslope;
extern	int32_t	heightnumerator;
extern	int16_t		minheightdiv;

extern	char	configname[13];



void		HelpScreens (void);
void		OrderingInfo (void);
void		TEDDeath(void);
void		Quit (char *error);
void 		CalcProjection (int32_t focal);
_boolean		SetViewSize (uint16_t width, uint16_t height);
void		NewGame (int16_t difficulty,int16_t episode);
void 		NewViewSize (int16_t width);
_boolean 	LoadTheGame(int16_t file,int16_t x,int16_t y);
_boolean		SaveTheGame(int16_t file,int16_t x,int16_t y);
void 		ShowViewSize (int16_t width);
void		ShutdownId (void);


/*
=============================================================================

						 WL_GAME DEFINITIONS

=============================================================================
*/


extern	_boolean		ingame,fizzlein;
extern	uint16_t	latchpics[NUMLATCHPICS];
extern	gametype	gamestate;
extern	int16_t			doornum;

extern	char		demoname[13];

extern	int32_t		spearx,speary;
extern	uint16_t	spearangle;
extern	_boolean		spearflag;


void 	DrawPlayBorder (void);
void 	ScanInfoPlane (void);
void	SetupGameLevel (void);
void 	NormalScreen (void);
void 	DrawPlayScreen (void);
void 	FizzleOut (void);
void 	GameLoop (void);
void ClearMemory (void);
void PlayDemo (int16_t demonumber);
void RecordDemo (void);
void DrawAllPlayBorder (void);
void	DrawHighScores(void);
void DrawAllPlayBorderSides (void);


// JAB
#define	PlaySoundLocTile(s,tx,ty)	PlaySoundLocGlobal(s,(((int32_t)(tx) << TILESHIFT) + (1L << (TILESHIFT - 1))),(((int32_t)ty << TILESHIFT) + (1L << (TILESHIFT - 1))))
#define	PlaySoundLocActor(s,ob)		PlaySoundLocGlobal(s,(ob)->x,(ob)->y)
void	PlaySoundLocGlobal(word s,fixed gx,fixed gy);
void UpdateSoundLoc(void);


/*
=============================================================================

						 WL_PLAY DEFINITIONS

=============================================================================
*/

#ifdef SPEAR
extern	int32_t		funnyticount;		// FOR FUNNY BJ FACE
#endif

extern	exit_t		playstate;

extern	_boolean		madenoise;

extern	objtype 	objlist[MAXACTORS],*_new,*obj,*player,*lastobj,
					*objfreelist,*killerobj;
extern	statobj_t	statobjlist[MAXSTATS],*laststatobj;
extern	doorobj_t	doorobjlist[MAXDOORS],*lastdoorobj;

extern	uint16_t	farmapylookup[MAPSIZE];
extern	byte		*nearmapylookup[MAPSIZE];

extern	byte		tilemap[MAPSIZE][MAPSIZE];	// wall values only
extern	byte		spotvis[MAPSIZE][MAPSIZE];
extern	objtype		*actorat[MAPSIZE][MAPSIZE];

#define UPDATESIZE			(UPDATEWIDE*UPDATEHIGH)
extern	byte		update[UPDATESIZE];

extern	_boolean		singlestep,godmode,noclip;
extern	int16_t			extravbls;

//
// control info
//
extern	_boolean		mouseenabled,joystickenabled,joypadenabled,joystickprogressive;
extern	int16_t			joystickport;
extern	int16_t			dirscan[4];
extern	int16_t			buttonscan[NUMBUTTONS];
extern	int16_t			buttonmouse[4];
extern	int16_t			buttonjoy[4];

extern	_boolean		buttonheld[NUMBUTTONS];

extern	int16_t			viewsize;

//
// curent user input
//
extern	int16_t			controlx,controly;		// range from -100 to 100
extern	_boolean		buttonstate[NUMBUTTONS];

extern	_boolean		demorecord,demoplayback;
extern	char	    *demoptr, *lastdemoptr;
extern	memptr		demobuffer;



void	InitRedShifts (void);
void 	FinishPaletteShifts (void);

void	CenterWindow(word w,word h);
void 	InitActorList (void);
void 	GetNewActor (void);
void 	RemoveObj (objtype *gone);
void 	PollControls (void);
void 	StopMusic(void);
void 	StartMusic(void);
void	PlayLoop (void);
void StartDamageFlash (int16_t damage);
void StartBonusFlash (void);

/*
=============================================================================

							WL_INTER

=============================================================================
*/

void IntroScreen (void);
void PreloadGraphics(void);
void LevelCompleted (void);
void	CheckHighScore (int32_t score,word other);
void Victory (void);
void ClearSplitVWB (void);


/*
=============================================================================

							WL_DEBUG

=============================================================================
*/

int16_t DebugKeys (void);
void PicturePause (void);


/*
=============================================================================

						 WL_DRAW DEFINITIONS

=============================================================================
*/

extern	uint16_t screenloc[3];
extern	uint16_t freelatch;

extern	int32_t 	lasttimecount;
extern	int32_t 	frameon;
extern	_boolean	fizzlein;

extern	uint16_t	wallheight[MAXVIEWWIDTH];

extern	fixed	tileglobal;
extern	fixed	focallength;
extern	fixed	mindist;

//
// math tables
//
extern	int16_t			pixelangle[MAXVIEWWIDTH];
extern	int32_t		finetangent[FINEANGLES/4];
extern	fixed 		sintable[], *costable;

//
// derived constants
//
extern	fixed 	scale;
extern	int32_t	heightnumerator,mindist;

//
// refresh variables
//
extern	fixed	viewx,viewy;			// the focal point
extern	int16_t		viewangle;
extern	fixed	viewsin,viewcos;

extern	int32_t		postsource;
extern	uint16_t	postx;
extern	uint16_t	postwidth;


extern	int16_t		horizwall[],vertwall[];

extern	uint16_t	pwallpos;


fixed	FixedByFrac (fixed a, fixed b);
void	TransformActor (objtype *ob);
void	BuildTables (void);
void	ClearScreen (void);
int16_t		CalcRotate (objtype *ob);
void	DrawScaleds (void);
void	CalcTics (void);
void	FixOfs (void);
void	ThreeDRefresh (void);
void  FarScalePost (void);

/*
=============================================================================

						 WL_STATE DEFINITIONS

=============================================================================
*/
#define TURNTICS	10
#define SPDPATROL	512
#define SPDDOG		1500


extern	dirtype opposite[9];
extern	dirtype diagonal[9][9];


void	InitHitRect (objtype *ob, uint16_t radius);
void	SpawnNewObj (uint16_t tilex, uint16_t tiley, statetype *state);
void	NewState (objtype *ob, statetype *state);

_boolean TryWalk (objtype *ob);
void 	SelectChaseDir (objtype *ob);
void 	SelectDodgeDir (objtype *ob);
void	SelectRunDir (objtype *ob);
void	MoveObj (objtype *ob, int32_t move);
_boolean SightPlayer (objtype *ob);

void	KillActor (objtype *ob);
void	DamageActor (objtype *ob, uint16_t damage);

_boolean CheckLine (objtype *ob);
_boolean	CheckSight (objtype *ob);


/*
=============================================================================

						 WL_SCALE DEFINITIONS

=============================================================================
*/


#define COMPSCALECODESTART	(65*4)		// offset to start of code in comp scaler

typedef struct
{
	uint16_t	codeofs[65];
	uint16_t	width[65];
	byte*		code;
}	t_compscale;

typedef struct
{
	uint16_t	leftpix,rightpix;
	uint16_t	dataofs[64];
// table data after dataofs[rightpix-leftpix+1]
}	t_compshape;


extern	t_compscale  *scaledirectory[MAXSCALEHEIGHT+1];
extern	int32_t			fullscalefarcall[MAXSCALEHEIGHT+1];

extern	byte		bitmasks1[8][8];
extern	byte		bitmasks2[8][8];
extern	uint16_t	wordmasks[8][8];

extern	byte		mapmasks1[4][8];
extern	byte		mapmasks2[4][8];
extern	byte		mapmasks3[4][8];

extern	int16_t			maxscale,maxscaleshl2;

extern	_boolean	insetupscaling;

void SetupScaling (int16_t maxscaleheight);
void ScaleShape (int16_t xcenter, int16_t shapenum, uint16_t height);
void SimpleScaleShape (int16_t xcenter, int16_t shapenum, uint16_t height);

/*
=============================================================================

						 WL_AGENT DEFINITIONS

=============================================================================
*/

//
// player state info
//
extern	_boolean		running;
extern	int32_t		thrustspeed;
extern	uint16_t	plux,pluy;		// player coordinates scaled to uint16_t

extern	int16_t			anglefrac;
extern	int16_t			facecount;

void	SpawnPlayer (int16_t tilex, int16_t tiley, int16_t dir);
void 	DrawFace (void);
void	DrawHealth (void);
void	TakeDamage (int16_t points,objtype *attacker);
void	HealSelf (int16_t points);
void	DrawLevel (void);
void	DrawLives (void);
void	GiveExtraMan (void);
void	DrawScore (void);
void	GivePoints (int32_t points);
void	DrawWeapon (void);
void	DrawKeys (void);
void	GiveWeapon (int16_t weapon);
void	DrawAmmo (void);
void	GiveAmmo (int16_t ammo);
void	GiveKey (int16_t key);
void	GetBonus (statobj_t *check);

void	Thrust (int16_t angle, int32_t speed);

/*
=============================================================================

						 WL_ACT1 DEFINITIONS

=============================================================================
*/

extern	doorobj_t	doorobjlist[MAXDOORS],*lastdoorobj;
extern	int16_t			doornum;

extern	uint16_t	doorposition[MAXDOORS],pwallstate;

extern	byte		areaconnect[NUMAREAS][NUMAREAS];

extern	_boolean		areabyplayer[NUMAREAS];

extern uint16_t	pwallstate;
extern uint16_t	pwallpos;			// amount a pushable wall has been moved (0-63)
extern uint16_t	pwallx,pwally;
extern int16_t			pwalldir;


void InitDoorList (void);
void InitStaticList (void);
void SpawnStatic (int16_t tilex, int16_t tiley, int16_t type);
void SpawnDoor (int16_t tilex, int16_t tiley, _boolean vertical, int16_t lock);
void MoveDoors (void);
void MovePWalls (void);
void OpenDoor (int16_t door);
void PlaceItemType (int16_t itemtype, int16_t tilex, int16_t tiley);
void PushWall (int16_t checkx, int16_t checky, int16_t dir);
void OperateDoor (int16_t door);
void InitAreas (void);

/*
=============================================================================

						 WL_ACT2 DEFINITIONS

=============================================================================
*/

#define s_nakedbody s_static10

extern	statetype s_grddie1;
extern	statetype s_dogdie1;
extern	statetype s_ofcdie1;
extern	statetype s_mutdie1;
extern	statetype s_ssdie1;
extern	statetype s_bossdie1;
extern	statetype s_schabbdie1;
extern	statetype s_fakedie1;
extern	statetype s_mechadie1;
extern	statetype s_hitlerdie1;
extern	statetype s_greteldie1;
extern	statetype s_giftdie1;
extern	statetype s_fatdie1;

extern	statetype s_spectredie1;
extern	statetype s_angeldie1;
extern	statetype s_transdie0;
extern	statetype s_uberdie0;
extern	statetype s_willdie1;
extern	statetype s_deathdie1;


extern	statetype s_grdchase1;
extern	statetype s_dogchase1;
extern	statetype s_ofcchase1;
extern	statetype s_sschase1;
extern	statetype s_mutchase1;
extern	statetype s_bosschase1;
extern	statetype s_schabbchase1;
extern	statetype s_fakechase1;
extern	statetype s_mechachase1;
extern	statetype s_gretelchase1;
extern	statetype s_giftchase1;
extern	statetype s_fatchase1;

extern	statetype s_spectrechase1;
extern	statetype s_angelchase1;
extern	statetype s_transchase1;
extern	statetype s_uberchase1;
extern	statetype s_willchase1;
extern	statetype s_deathchase1;

extern	statetype s_blinkychase1;
extern	statetype s_hitlerchase1;

extern	statetype s_grdpain;
extern	statetype s_grdpain1;
extern	statetype s_ofcpain;
extern	statetype s_ofcpain1;
extern	statetype s_sspain;
extern	statetype s_sspain1;
extern	statetype s_mutpain;
extern	statetype s_mutpain1;

extern	statetype s_deathcam;

extern	statetype s_schabbdeathcam2;
extern	statetype s_hitlerdeathcam2;
extern	statetype s_giftdeathcam2;
extern	statetype s_fatdeathcam2;

void SpawnStand (enemy_t which, int16_t tilex, int16_t tiley, int16_t dir);
void SpawnPatrol (enemy_t which, int16_t tilex, int16_t tiley, int16_t dir);
void KillActor (objtype *ob);

void	US_ControlPanel(byte);

void SpawnDeadGuard (int16_t tilex, int16_t tiley);
void SpawnBoss (int16_t tilex, int16_t tiley);
void SpawnGretel (int16_t tilex, int16_t tiley);
void SpawnTrans (int16_t tilex, int16_t tiley);
void SpawnUber (int16_t tilex, int16_t tiley);
void SpawnWill (int16_t tilex, int16_t tiley);
void SpawnDeath (int16_t tilex, int16_t tiley);
void SpawnAngel (int16_t tilex, int16_t tiley);
void SpawnSpectre (int16_t tilex, int16_t tiley);
void SpawnGhosts (int16_t which, int16_t tilex, int16_t tiley);
void SpawnSchabbs (int16_t tilex, int16_t tiley);
void SpawnGift (int16_t tilex, int16_t tiley);
void SpawnFat (int16_t tilex, int16_t tiley);
void SpawnFakeHitler (int16_t tilex, int16_t tiley);
void SpawnHitler (int16_t tilex, int16_t tiley);

/*
=============================================================================

						 WL_TEXT DEFINITIONS

=============================================================================
*/

extern	char	helpfilename[],endfilename[];

extern	void	HelpScreens(void);
extern	void	EndText(void);

// Arguments
extern int16_t _argc;
extern char** _argv;

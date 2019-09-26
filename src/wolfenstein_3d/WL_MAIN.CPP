// // WL_MAIN.C

// // #include <conio.h>
#include "WL_DEF.H"
#include <string.h>
#include <chrono>
#include <tinyfiledialogs.h>
#include <cassert>
// #pragma hdrstop


/*
=============================================================================

						   WOLFENSTEIN 3-D

					  An Id Software production

						   by John Carmack

=============================================================================
*/

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/


#define FOCALLENGTH     (0x5700l)               // in global coordinates
#define VIEWGLOBAL      0x10000                 // globals visable flush to wall

#define VIEWWIDTH       256                     // size of view window
#define VIEWHEIGHT      144

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

char            str[80]={0},str2[20]={0};
int16_t				tedlevelnum=0;
_boolean         tedlevel=_false;
_boolean         nospr=_false;
_boolean         IsA386=_false;
int16_t                     dirangle[9] = {0,ANGLES/8,2*ANGLES/8,3*ANGLES/8,4*ANGLES/8,
	5*ANGLES/8,6*ANGLES/8,7*ANGLES/8,ANGLES};

//
// proejection variables
//
fixed           focallength=0;
uint16_t        screenofs=0;
int16_t             viewwidth=0;
int16_t             viewheight=0;
int16_t             centerx=0;
int16_t             shootdelta=0;                     // pixels away from centerx a target can be
fixed           scale,maxslope=0;
int32_t            heightnumerator=0;
int16_t                     minheightdiv=0;

_boolean         startgame=_false,loadedgame=_false,virtualreality=_false;
int16_t             mouseadjustment = 0;

char	configname[13]="CONFIG.";


/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

void            Quit (char *error);

/*
====================
=
= ReadConfig
=
====================
*/

void ReadConfig(void)
{
	int16_t                     file;
	SDMode          sd;
	SMMode          sm;
	SDSMode         sds;


	//if ( (file = file_open(configname,O_BINARY | O_RDONLY)) != -1)
	//{
	////
	//// valid config file
	////
	//	file_read(file,Scores,sizeof(HighScore) * MaxScores);

	//	file_read(file,&sd,sizeof(sd));
	//	file_read(file,&sm,sizeof(sm));
	//	file_read(file,&sds,sizeof(sds));

	//	file_read(file,&mouseenabled,sizeof(mouseenabled));
	//	file_read(file,&joystickenabled,sizeof(joystickenabled));
	//	file_read(file,&joypadenabled,sizeof(joypadenabled));
	//	file_read(file,&joystickprogressive,sizeof(joystickprogressive));
	//	file_read(file,&joystickport,sizeof(joystickport));

	//	file_read(file,&dirscan,sizeof(dirscan));
	//	file_read(file,&buttonscan,sizeof(buttonscan));
	//	file_read(file,&buttonmouse,sizeof(buttonmouse));
	//	file_read(file,&buttonjoy,sizeof(buttonjoy));

	//	file_read(file,&viewsize,sizeof(viewsize));
	//	file_read(file,&mouseadjustment,sizeof(mouseadjustment));

	//	file_close(file);

	//	if (sd == sdm_AdLib && !AdLibPresent && !SoundBlasterPresent)
	//	{
	//		sd = sdm_PC;
	//		sd = (SDMode)smm_Off; // sm?
	//	}

	//	if ((sds == sds_SoundBlaster && !SoundBlasterPresent) ||
	//		(sds == sds_SoundSource && !SoundSourcePresent))
	//		sds = sds_Off;

	//	if (!MousePresent)
	//		mouseenabled = _false;
	//	if (!JoysPresent[joystickport])
	//		joystickenabled = _false;

	//	MainMenu[6].active=1;
	//	MainItems.curpos=0;
	//}
	//else
	//{
	//
	// no config file, so select by hardware
	//
		//if (SoundBlasterPresent || AdLibPresent)
		{
			sd = sdm_AdLib;
			sm = smm_AdLib;
		}
		//else
		//{
		//	sd = sdm_PC;
		//	sm = smm_Off;
		//}

		if (SoundBlasterPresent)
			sds = sds_SoundBlaster;
		else if (SoundSourcePresent)
			sds = sds_SoundSource;
		else
			sds = sds_Off;

		if (MousePresent)
			mouseenabled = _true;

		joystickenabled = _false;
		joypadenabled = _false;
		joystickport = 0;
		joystickprogressive = _false;

		viewsize = 15;
		mouseadjustment=3;
	//}

	SD_SetMusicMode (sm);
	SD_SetSoundMode (sd);
	SD_SetDigiDevice (sds);

}


/*
====================
=
= WriteConfig
=
====================
*/

void WriteConfig(void)
{
    return; // Not yet
	int16_t                     file;

	file = file_open(configname,O_CREAT | O_BINARY | O_WRONLY,
				S_IREAD | S_IWRITE | S_IFREG);

	if (file != -1)
	{
		file_write(file,Scores,sizeof(HighScore) * MaxScores);

		file_write(file,&dirscan,sizeof(dirscan));
		file_write(file,&buttonscan,sizeof(buttonscan));
		file_write(file,&buttonmouse,sizeof(buttonmouse));
		file_write(file,&buttonjoy,sizeof(buttonjoy));

		file_write(file,&mouseadjustment,sizeof(mouseadjustment));

		file_close(file);
	}
}


//===========================================================================


/*
========================
=
= Patch386
=
= Patch ldiv to use 32 bit instructions
=
========================
*/

// const char    *JHParmStrings[] = {"no386",nil};
void Patch386 (void)
{
// extern void jabhack2(void);
// extern int16_t  CheckIs386(void);

// 	int16_t     i;

// 	for (i = 1;i < ws_argc;i++)
// 		if (US_CheckParm(ws_argv[i],JHParmStrings) == 0)
// 		{
// 			IsA386 = _false;
// 			return;
// 		}

// 	if (CheckIs386())
// 	{
// 		IsA386 = _true;
// 		jabhack2();
// 	}
// 	else
// 		IsA386 = _false;
}

//===========================================================================

/*
=====================
=
= NewGame
=
= Set up _new game to start from the beginning
=
=====================
*/

void NewGame (int16_t difficulty,int16_t episode)
{
	memset (&gamestate,0,sizeof(gamestate));
	gamestate.difficulty = difficulty;
	gamestate.weapon = gamestate.bestweapon
		= gamestate.chosenweapon = wp_pistol;
	gamestate.health = 100;
	gamestate.ammo = STARTAMMO;
	gamestate.lives = 3;
	gamestate.nextextra = EXTRAPOINTS;
	gamestate.episode=episode;

	startgame = _true;
}

//===========================================================================

void DiskFlopAnim(int16_t x,int16_t y)
{
 static char which=0;
 if (!x && !y)
   return;
 VWB_DrawPic(x,y,C_DISKLOADING1PIC+which);
 VW_UpdateScreen();
 which^=1;
}


int32_t DoChecksum(byte *source,uint16_t size,int32_t checksum)
{
 uint16_t i;

 for (i=0;i<size-1;i++)
   checksum += source[i]^source[i+1];

 return checksum;
}

#include <json/json.h>
#include <fstream>
#include <tinyfiledialogs.h>
#include <sstream>
#include <map>
#include <string>

/*
==================
=
= SaveTheGame
=
==================
*/

std::map<weapontype, std::string> WP_2_STR = {
    std::make_pair<weapontype, std::string>(wp_knife, "knife"),
    std::make_pair<weapontype, std::string>(wp_pistol, "pistol"),
    std::make_pair<weapontype, std::string>(wp_machinegun, "machinegun"),
    std::make_pair<weapontype, std::string>(wp_chaingun, "chaingun")
};
std::map<std::string, weapontype> STR_2_WP;

std::map<activetype, std::string> ACTIVETYPE_TO_STR = {
    std::make_pair<activetype, std::string>(ac_badobject, "badobject"),
    std::make_pair<activetype, std::string>(ac_no, "no"),
    std::make_pair<activetype, std::string>(ac_yes, "yes"),
    std::make_pair<activetype, std::string>(ac_allways, "allways")
};
std::map<std::string, activetype> STR_2_ACTIVETYPE;

std::map<classtype, std::string> CLASSTYPE_2_STR = {
    std::make_pair<classtype, std::string>(nothing, "nothing"),
    std::make_pair<classtype, std::string>(playerobj, "playerobj"),
    std::make_pair<classtype, std::string>(inertobj, "inertobj"),
    std::make_pair<classtype, std::string>(guardobj, "guardobj"),
    std::make_pair<classtype, std::string>(officerobj, "officerobj"),
    std::make_pair<classtype, std::string>(ssobj, "ssobj"),
    std::make_pair<classtype, std::string>(dogobj, "dogobj"),
    std::make_pair<classtype, std::string>(bossobj, "bossobj"),
    std::make_pair<classtype, std::string>(schabbobj, "schabbobj"),
    std::make_pair<classtype, std::string>(fakeobj, "fakeobj"),
    std::make_pair<classtype, std::string>(mechahitlerobj, "mechahitlerobj"),
    std::make_pair<classtype, std::string>(mutantobj, "mutantobj"),
    std::make_pair<classtype, std::string>(needleobj, "needleobj"),
    std::make_pair<classtype, std::string>(fireobj, "fireobj"),
    std::make_pair<classtype, std::string>(bjobj, "bjobj"),
    std::make_pair<classtype, std::string>(ghostobj, "ghostobj"),
    std::make_pair<classtype, std::string>(realhitlerobj, "realhitlerobj"),
    std::make_pair<classtype, std::string>(gretelobj, "gretelobj"),
    std::make_pair<classtype, std::string>(giftobj, "giftobj"),
    std::make_pair<classtype, std::string>(fatobj, "fatobj"),
    std::make_pair<classtype, std::string>(rocketobj, "rocketobj"),
    std::make_pair<classtype, std::string>(spectreobj, "spectreobj"),
    std::make_pair<classtype, std::string>(angelobj, "angelobj"),
    std::make_pair<classtype, std::string>(transobj, "transobj"),
    std::make_pair<classtype, std::string>(uberobj, "uberobj"),
    std::make_pair<classtype, std::string>(willobj, "willobj"),
    std::make_pair<classtype, std::string>(deathobj, "deathobj"),
    std::make_pair<classtype, std::string>(hrocketobj, "hrocketobj"),
    std::make_pair<classtype, std::string>(sparkobj, "sparkobj"),
};
std::map<std::string, classtype> STR_2_CLASSTYPE;

extern	statetype s_rocket;
extern	statetype s_smoke1;
extern	statetype s_smoke2;
extern	statetype s_smoke3;
extern	statetype s_smoke4;
extern	statetype s_boom2;
extern	statetype s_boom3;
extern	statetype s_hrocket;
extern	statetype s_hsmoke1;
extern	statetype s_hsmoke2;
extern	statetype s_hsmoke3;
extern	statetype s_hsmoke4;
extern	statetype s_hboom2;
extern	statetype s_hboom3;
extern	statetype s_grdstand;
extern	statetype s_grdpath1;
extern	statetype s_grdpath1s;
extern	statetype s_grdpath2;
extern	statetype s_grdpath3;
extern	statetype s_grdpath3s;
extern	statetype s_grdpath4;
extern	statetype s_grdpain;
extern	statetype s_grdpain1;
extern	statetype s_grdgiveup;
extern	statetype s_grdshoot1;
extern	statetype s_grdshoot2;
extern	statetype s_grdshoot3;
extern	statetype s_grdshoot4;
extern	statetype s_grdchase1;
extern	statetype s_grdchase1s;
extern	statetype s_grdchase2;
extern	statetype s_grdchase3;
extern	statetype s_grdchase3s;
extern	statetype s_grdchase4;
extern	statetype s_grddie1;
extern	statetype s_grddie1d;
extern	statetype s_grddie2;
extern	statetype s_grddie3;
extern	statetype s_grddie4;
extern	statetype s_blinkychase1;
extern	statetype s_blinkychase2;
extern	statetype s_inkychase1;
extern	statetype s_inkychase2;
extern	statetype s_pinkychase1;
extern	statetype s_pinkychase2;
extern	statetype s_clydechase1;
extern	statetype s_clydechase2;
extern	statetype s_dogpath1;
extern	statetype s_dogpath1s;
extern	statetype s_dogpath2;
extern	statetype s_dogpath3;
extern	statetype s_dogpath3s;
extern	statetype s_dogpath4;
extern	statetype s_dogjump1;
extern	statetype s_dogjump2;
extern	statetype s_dogjump3;
extern	statetype s_dogjump4;
extern	statetype s_dogjump5;
extern	statetype s_dogchase1;
extern	statetype s_dogchase1s;
extern	statetype s_dogchase2;
extern	statetype s_dogchase3;
extern	statetype s_dogchase3s;
extern	statetype s_dogchase4;
extern	statetype s_dogdie1;
extern	statetype s_dogdie1d;
extern	statetype s_dogdie2;
extern	statetype s_dogdie3;
extern	statetype s_dogdead;
extern	statetype s_ofcstand;
extern	statetype s_ofcpath1;
extern	statetype s_ofcpath1s;
extern	statetype s_ofcpath2;
extern	statetype s_ofcpath3;
extern	statetype s_ofcpath3s;
extern	statetype s_ofcpath4;
extern	statetype s_ofcpain;
extern	statetype s_ofcpain1;
extern	statetype s_ofcgiveup;
extern	statetype s_ofcshoot1;
extern	statetype s_ofcshoot2;
extern	statetype s_ofcshoot3;
extern	statetype s_ofcshoot4;
extern	statetype s_ofcchase1;
extern	statetype s_ofcchase1s;
extern	statetype s_ofcchase2;
extern	statetype s_ofcchase3;
extern	statetype s_ofcchase3s;
extern	statetype s_ofcchase4;
extern	statetype s_ofcdie1;
extern	statetype s_ofcdie2;
extern	statetype s_ofcdie3;
extern	statetype s_ofcdie4;
extern	statetype s_ofcdie5;
extern	statetype s_mutstand;
extern	statetype s_mutpath1;
extern	statetype s_mutpath1s;
extern	statetype s_mutpath2;
extern	statetype s_mutpath3;
extern	statetype s_mutpath3s;
extern	statetype s_mutpath4;
extern	statetype s_mutpain;
extern	statetype s_mutpain1;
extern	statetype s_mutgiveup;
extern	statetype s_mutshoot1;
extern	statetype s_mutshoot2;
extern	statetype s_mutshoot3;
extern	statetype s_mutshoot4;
extern	statetype s_mutchase1;
extern	statetype s_mutchase1s;
extern	statetype s_mutchase2;
extern	statetype s_mutchase3;
extern	statetype s_mutchase3s;
extern	statetype s_mutchase4;
extern	statetype s_mutdie1;
extern	statetype s_mutdie2;
extern	statetype s_mutdie3;
extern	statetype s_mutdie4;
extern	statetype s_mutdie5;
extern	statetype s_ssstand;
extern	statetype s_sspath1;
extern	statetype s_sspath1s;
extern	statetype s_sspath2;
extern	statetype s_sspath3;
extern	statetype s_sspath3s;
extern	statetype s_sspath4;
extern	statetype s_sspain;
extern	statetype s_sspain1;
extern	statetype s_ssshoot1;
extern	statetype s_ssshoot2;
extern	statetype s_ssshoot3;
extern	statetype s_ssshoot4;
extern	statetype s_ssshoot5;
extern	statetype s_ssshoot6;
extern	statetype s_ssshoot7;
extern	statetype s_ssshoot8;
extern	statetype s_ssshoot9;
extern	statetype s_sschase1;
extern	statetype s_sschase1s;
extern	statetype s_sschase2;
extern	statetype s_sschase3;
extern	statetype s_sschase3s;
extern	statetype s_sschase4;
extern	statetype s_ssdie1;
extern	statetype s_ssdie2;
extern	statetype s_ssdie3;
extern	statetype s_ssdie4;
extern	statetype s_bossstand;
extern	statetype s_bosschase1;
extern	statetype s_bosschase1s;
extern	statetype s_bosschase2;
extern	statetype s_bosschase3;
extern	statetype s_bosschase3s;
extern	statetype s_bosschase4;
extern	statetype s_bossdie1;
extern	statetype s_bossdie2;
extern	statetype s_bossdie3;
extern	statetype s_bossdie4;
extern	statetype s_bossshoot1;
extern	statetype s_bossshoot2;
extern	statetype s_bossshoot3;
extern	statetype s_bossshoot4;
extern	statetype s_bossshoot5;
extern	statetype s_bossshoot6;
extern	statetype s_bossshoot7;
extern	statetype s_bossshoot8;
extern	statetype s_gretelstand;
extern	statetype s_gretelchase1;
extern	statetype s_gretelchase1s;
extern	statetype s_gretelchase2;
extern	statetype s_gretelchase3;
extern	statetype s_gretelchase3s;
extern	statetype s_gretelchase4;
extern	statetype s_greteldie1;
extern	statetype s_greteldie2;
extern	statetype s_greteldie3;
extern	statetype s_greteldie4;
extern	statetype s_gretelshoot1;
extern	statetype s_gretelshoot2;
extern	statetype s_gretelshoot3;
extern	statetype s_gretelshoot4;
extern	statetype s_gretelshoot5;
extern	statetype s_gretelshoot6;
extern	statetype s_gretelshoot7;
extern	statetype s_gretelshoot8;
extern	statetype s_angelshoot1;
extern	statetype s_deathshoot1;
extern	statetype s_spark1;
extern	statetype s_transstand;
extern	statetype s_transchase1;
extern	statetype s_transchase1s;
extern	statetype s_transchase2;
extern	statetype s_transchase3;
extern	statetype s_transchase3s;
extern	statetype s_transchase4;
extern	statetype s_transdie0;
extern	statetype s_transdie01;
extern	statetype s_transdie1;
extern	statetype s_transdie2;
extern	statetype s_transdie3;
extern	statetype s_transdie4;
extern	statetype s_transshoot1;
extern	statetype s_transshoot2;
extern	statetype s_transshoot3;
extern	statetype s_transshoot4;
extern	statetype s_transshoot5;
extern	statetype s_transshoot6;
extern	statetype s_transshoot7;
extern	statetype s_transshoot8;
extern	statetype s_uberstand;
extern	statetype s_uberchase1;
extern	statetype s_uberchase1s;
extern	statetype s_uberchase2;
extern	statetype s_uberchase3;
extern	statetype s_uberchase3s;
extern	statetype s_uberchase4;
extern	statetype s_uberdie0;
extern	statetype s_uberdie01;
extern	statetype s_uberdie1;
extern	statetype s_uberdie2;
extern	statetype s_uberdie3;
extern	statetype s_uberdie4;
extern	statetype s_uberdie5;
extern	statetype s_ubershoot1;
extern	statetype s_ubershoot2;
extern	statetype s_ubershoot3;
extern	statetype s_ubershoot4;
extern	statetype s_ubershoot5;
extern	statetype s_ubershoot6;
extern	statetype s_ubershoot7;
extern	statetype s_willstand;
extern	statetype s_willchase1;
extern	statetype s_willchase1s;
extern	statetype s_willchase2;
extern	statetype s_willchase3;
extern	statetype s_willchase3s;
extern	statetype s_willchase4;
extern	statetype s_willdie1;
extern	statetype s_willdie2;
extern	statetype s_willdie3;
extern	statetype s_willdie4;
extern	statetype s_willdie5;
extern	statetype s_willdie6;
extern	statetype s_willshoot1;
extern	statetype s_willshoot2;
extern	statetype s_willshoot3;
extern	statetype s_willshoot4;
extern	statetype s_willshoot5;
extern	statetype s_willshoot6;
extern	statetype s_deathstand;
extern	statetype s_deathchase1;
extern	statetype s_deathchase1s;
extern	statetype s_deathchase2;
extern	statetype s_deathchase3;
extern	statetype s_deathchase3s;
extern	statetype s_deathchase4;
extern	statetype s_deathdie1;
extern	statetype s_deathdie2;
extern	statetype s_deathdie3;
extern	statetype s_deathdie4;
extern	statetype s_deathdie5;
extern	statetype s_deathdie6;
extern	statetype s_deathdie7;
extern	statetype s_deathdie8;
extern	statetype s_deathdie9;
extern	statetype s_deathshoot1;
extern	statetype s_deathshoot2;
extern	statetype s_deathshoot3;
extern	statetype s_deathshoot4;
extern	statetype s_deathshoot5;
extern	statetype s_angelstand;
extern	statetype s_angelchase1;
extern	statetype s_angelchase1s;
extern	statetype s_angelchase2;
extern	statetype s_angelchase3;
extern	statetype s_angelchase3s;
extern	statetype s_angelchase4;
extern	statetype s_angeldie1;
extern	statetype s_angeldie11;
extern	statetype s_angeldie2;
extern	statetype s_angeldie3;
extern	statetype s_angeldie4;
extern	statetype s_angeldie5;
extern	statetype s_angeldie6;
extern	statetype s_angeldie7;
extern	statetype s_angeldie8;
extern	statetype s_angeldie9;
extern	statetype s_angelshoot1;
extern	statetype s_angelshoot2;
extern	statetype s_angelshoot3;
extern	statetype s_angelshoot4;
extern	statetype s_angelshoot5;
extern	statetype s_angelshoot6;
extern	statetype s_angeltired;
extern	statetype s_angeltired2;
extern	statetype s_angeltired3;
extern	statetype s_angeltired4;
extern	statetype s_angeltired5;
extern	statetype s_angeltired6;
extern	statetype s_angeltired7;
extern	statetype s_spark1;
extern	statetype s_spark2;
extern	statetype s_spark3;
extern	statetype s_spark4;
extern	statetype s_spectrewait1;
extern	statetype s_spectrewait2;
extern	statetype s_spectrewait3;
extern	statetype s_spectrewait4;
extern	statetype s_spectrechase1;
extern	statetype s_spectrechase2;
extern	statetype s_spectrechase3;
extern	statetype s_spectrechase4;
extern	statetype s_spectredie1;
extern	statetype s_spectredie2;
extern	statetype s_spectredie3;
extern	statetype s_spectredie4;
extern	statetype s_spectrewake;
extern	statetype s_schabbstand;
extern	statetype s_schabbchase1;
extern	statetype s_schabbchase1s;
extern	statetype s_schabbchase2;
extern	statetype s_schabbchase3;
extern	statetype s_schabbchase3s;
extern	statetype s_schabbchase4;
extern	statetype s_schabbdie1;
extern	statetype s_schabbdie2;
extern	statetype s_schabbdie3;
extern	statetype s_schabbdie4;
extern	statetype s_schabbdie5;
extern	statetype s_schabbdie6;
extern	statetype s_schabbshoot1;
extern	statetype s_schabbshoot2;
extern	statetype s_needle1;
extern	statetype s_needle2;
extern	statetype s_needle3;
extern	statetype s_needle4;
extern	statetype s_schabbdeathcam;
extern	statetype s_giftstand;
extern	statetype s_giftchase1;
extern	statetype s_giftchase1s;
extern	statetype s_giftchase2;
extern	statetype s_giftchase3;
extern	statetype s_giftchase3s;
extern	statetype s_giftchase4;
extern	statetype s_giftdie1;
extern	statetype s_giftdie2;
extern	statetype s_giftdie3;
extern	statetype s_giftdie4;
extern	statetype s_giftdie5;
extern	statetype s_giftdie6;
extern	statetype s_giftshoot1;
extern	statetype s_giftshoot2;
extern	statetype s_needle1;
extern	statetype s_needle2;
extern	statetype s_needle3;
extern	statetype s_needle4;
extern	statetype s_giftdeathcam;
extern	statetype s_boom1;
extern	statetype s_boom2;
extern	statetype s_boom3;
extern	statetype s_fatstand;
extern	statetype s_fatchase1;
extern	statetype s_fatchase1s;
extern	statetype s_fatchase2;
extern	statetype s_fatchase3;
extern	statetype s_fatchase3s;
extern	statetype s_fatchase4;
extern	statetype s_fatdie1;
extern	statetype s_fatdie2;
extern	statetype s_fatdie3;
extern	statetype s_fatdie4;
extern	statetype s_fatdie5;
extern	statetype s_fatdie6;
extern	statetype s_fatshoot1;
extern	statetype s_fatshoot2;
extern	statetype s_fatshoot3;
extern	statetype s_fatshoot4;
extern	statetype s_fatshoot5;
extern	statetype s_fatshoot6;
extern	statetype s_needle1;
extern	statetype s_needle2;
extern	statetype s_needle3;
extern	statetype s_needle4;
extern	statetype s_fatdeathcam;
extern	statetype s_fakestand;
extern	statetype s_fakechase1;
extern	statetype s_fakechase1s;
extern	statetype s_fakechase2;
extern	statetype s_fakechase3;
extern	statetype s_fakechase3s;
extern	statetype s_fakechase4;
extern	statetype s_fakedie1;
extern	statetype s_fakedie2;
extern	statetype s_fakedie3;
extern	statetype s_fakedie4;
extern	statetype s_fakedie5;
extern	statetype s_fakedie6;
extern	statetype s_fakeshoot1;
extern	statetype s_fakeshoot2;
extern	statetype s_fakeshoot3;
extern	statetype s_fakeshoot4;
extern	statetype s_fakeshoot5;
extern	statetype s_fakeshoot6;
extern	statetype s_fakeshoot7;
extern	statetype s_fakeshoot8;
extern	statetype s_fakeshoot9;
extern	statetype s_fire1;
extern	statetype s_fire2;
extern	statetype s_mechachase1;
extern	statetype s_mechachase1s;
extern	statetype s_mechachase2;
extern	statetype s_mechachase3;
extern	statetype s_mechachase3s;
extern	statetype s_mechachase4;
extern	statetype s_mechadie1;
extern	statetype s_mechadie2;
extern	statetype s_mechadie3;
extern	statetype s_mechadie4;
extern	statetype s_mechashoot1;
extern	statetype s_mechashoot2;
extern	statetype s_mechashoot3;
extern	statetype s_mechashoot4;
extern	statetype s_mechashoot5;
extern	statetype s_mechashoot6;
extern	statetype s_hitlerchase1;
extern	statetype s_hitlerchase1s;
extern	statetype s_hitlerchase2;
extern	statetype s_hitlerchase3;
extern	statetype s_hitlerchase3s;
extern	statetype s_hitlerchase4;
extern	statetype s_hitlerdie1;
extern	statetype s_hitlerdie2;
extern	statetype s_hitlerdie3;
extern	statetype s_hitlerdie4;
extern	statetype s_hitlerdie5;
extern	statetype s_hitlerdie6;
extern	statetype s_hitlerdie7;
extern	statetype s_hitlerdie8;
extern	statetype s_hitlerdie9;
extern	statetype s_hitlerdie10;
extern	statetype s_hitlershoot1;
extern	statetype s_hitlershoot2;
extern	statetype s_hitlershoot3;
extern	statetype s_hitlershoot4;
extern	statetype s_hitlershoot5;
extern	statetype s_hitlershoot6;
extern	statetype s_hitlerdeathcam;
extern	statetype s_bjrun1;
extern	statetype s_bjrun1s;
extern	statetype s_bjrun2;
extern	statetype s_bjrun3;
extern	statetype s_bjrun3s;
extern	statetype s_bjrun4;
extern	statetype s_bjjump1;
extern	statetype s_bjjump2;
extern	statetype s_bjjump3;
extern	statetype s_bjjump4;
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

static std::map<statetype*, std::string> STATE_2_STR = {
    std::make_pair<statetype*, std::string>(&s_rocket, "rocket"),
    std::make_pair<statetype*, std::string>(&s_smoke1, "smoke1"),
    std::make_pair<statetype*, std::string>(&s_smoke2, "smoke2"),
    std::make_pair<statetype*, std::string>(&s_smoke3, "smoke3"),
    std::make_pair<statetype*, std::string>(&s_smoke4, "smoke4"),
    std::make_pair<statetype*, std::string>(&s_boom2, "boom2"),
    std::make_pair<statetype*, std::string>(&s_boom3, "boom3"),
    //std::make_pair<statetype*, std::string>(&s_hrocket, "hrocket"),
    //std::make_pair<statetype*, std::string>(&s_hsmoke1, "hsmoke1"),
    //std::make_pair<statetype*, std::string>(&s_hsmoke2, "hsmoke2"),
    //std::make_pair<statetype*, std::string>(&s_hsmoke3, "hsmoke3"),
    //std::make_pair<statetype*, std::string>(&s_hsmoke4, "hsmoke4"),
    //std::make_pair<statetype*, std::string>(&s_hboom2, "hboom2"),
    //std::make_pair<statetype*, std::string>(&s_hboom3, "hboom3"),
    std::make_pair<statetype*, std::string>(&s_grdstand, "grdstand"),
    std::make_pair<statetype*, std::string>(&s_grdpath1, "grdpath1"),
    std::make_pair<statetype*, std::string>(&s_grdpath1s, "grdpath1s"),
    std::make_pair<statetype*, std::string>(&s_grdpath2, "grdpath2"),
    std::make_pair<statetype*, std::string>(&s_grdpath3, "grdpath3"),
    std::make_pair<statetype*, std::string>(&s_grdpath3s, "grdpath3s"),
    std::make_pair<statetype*, std::string>(&s_grdpath4, "grdpath4"),
    std::make_pair<statetype*, std::string>(&s_grdpain, "grdpain"),
    std::make_pair<statetype*, std::string>(&s_grdpain1, "grdpain1"),
    //std::make_pair<statetype*, std::string>(&s_grdgiveup, "grdgiveup"),
    std::make_pair<statetype*, std::string>(&s_grdshoot1, "grdshoot1"),
    std::make_pair<statetype*, std::string>(&s_grdshoot2, "grdshoot2"),
    std::make_pair<statetype*, std::string>(&s_grdshoot3, "grdshoot3"),
    //std::make_pair<statetype*, std::string>(&s_grdshoot4, "grdshoot4"),
    std::make_pair<statetype*, std::string>(&s_grdchase1, "grdchase1"),
    std::make_pair<statetype*, std::string>(&s_grdchase1s, "grdchase1s"),
    std::make_pair<statetype*, std::string>(&s_grdchase2, "grdchase2"),
    std::make_pair<statetype*, std::string>(&s_grdchase3, "grdchase3"),
    std::make_pair<statetype*, std::string>(&s_grdchase3s, "grdchase3s"),
    std::make_pair<statetype*, std::string>(&s_grdchase4, "grdchase4"),
    std::make_pair<statetype*, std::string>(&s_grddie1, "grddie1"),
    //std::make_pair<statetype*, std::string>(&s_grddie1d, "grddie1d"),
    std::make_pair<statetype*, std::string>(&s_grddie2, "grddie2"),
    std::make_pair<statetype*, std::string>(&s_grddie3, "grddie3"),
    std::make_pair<statetype*, std::string>(&s_grddie4, "grddie4"),
    std::make_pair<statetype*, std::string>(&s_blinkychase1, "blinkychase1"),
    std::make_pair<statetype*, std::string>(&s_blinkychase2, "blinkychase2"),
    std::make_pair<statetype*, std::string>(&s_inkychase1, "inkychase1"),
    std::make_pair<statetype*, std::string>(&s_inkychase2, "inkychase2"),
    std::make_pair<statetype*, std::string>(&s_pinkychase1, "pinkychase1"),
    std::make_pair<statetype*, std::string>(&s_pinkychase2, "pinkychase2"),
    std::make_pair<statetype*, std::string>(&s_clydechase1, "clydechase1"),
    std::make_pair<statetype*, std::string>(&s_clydechase2, "clydechase2"),
    std::make_pair<statetype*, std::string>(&s_dogpath1, "dogpath1"),
    std::make_pair<statetype*, std::string>(&s_dogpath1s, "dogpath1s"),
    std::make_pair<statetype*, std::string>(&s_dogpath2, "dogpath2"),
    std::make_pair<statetype*, std::string>(&s_dogpath3, "dogpath3"),
    std::make_pair<statetype*, std::string>(&s_dogpath3s, "dogpath3s"),
    std::make_pair<statetype*, std::string>(&s_dogpath4, "dogpath4"),
    std::make_pair<statetype*, std::string>(&s_dogjump1, "dogjump1"),
    std::make_pair<statetype*, std::string>(&s_dogjump2, "dogjump2"),
    std::make_pair<statetype*, std::string>(&s_dogjump3, "dogjump3"),
    std::make_pair<statetype*, std::string>(&s_dogjump4, "dogjump4"),
    std::make_pair<statetype*, std::string>(&s_dogjump5, "dogjump5"),
    std::make_pair<statetype*, std::string>(&s_dogchase1, "dogchase1"),
    std::make_pair<statetype*, std::string>(&s_dogchase1s, "dogchase1s"),
    std::make_pair<statetype*, std::string>(&s_dogchase2, "dogchase2"),
    std::make_pair<statetype*, std::string>(&s_dogchase3, "dogchase3"),
    std::make_pair<statetype*, std::string>(&s_dogchase3s, "dogchase3s"),
    std::make_pair<statetype*, std::string>(&s_dogchase4, "dogchase4"),
    std::make_pair<statetype*, std::string>(&s_dogdie1, "dogdie1"),
    //std::make_pair<statetype*, std::string>(&s_dogdie1d, "dogdie1d"),
    std::make_pair<statetype*, std::string>(&s_dogdie2, "dogdie2"),
    std::make_pair<statetype*, std::string>(&s_dogdie3, "dogdie3"),
    std::make_pair<statetype*, std::string>(&s_dogdead, "dogdead"),
    std::make_pair<statetype*, std::string>(&s_ofcstand, "ofcstand"),
    std::make_pair<statetype*, std::string>(&s_ofcpath1, "ofcpath1"),
    std::make_pair<statetype*, std::string>(&s_ofcpath1s, "ofcpath1s"),
    std::make_pair<statetype*, std::string>(&s_ofcpath2, "ofcpath2"),
    std::make_pair<statetype*, std::string>(&s_ofcpath3, "ofcpath3"),
    std::make_pair<statetype*, std::string>(&s_ofcpath3s, "ofcpath3s"),
    std::make_pair<statetype*, std::string>(&s_ofcpath4, "ofcpath4"),
    std::make_pair<statetype*, std::string>(&s_ofcpain, "ofcpain"),
    std::make_pair<statetype*, std::string>(&s_ofcpain1, "ofcpain1"),
    //std::make_pair<statetype*, std::string>(&s_ofcgiveup, "ofcgiveup"),
    std::make_pair<statetype*, std::string>(&s_ofcshoot1, "ofcshoot1"),
    std::make_pair<statetype*, std::string>(&s_ofcshoot2, "ofcshoot2"),
    std::make_pair<statetype*, std::string>(&s_ofcshoot3, "ofcshoot3"),
    //std::make_pair<statetype*, std::string>(&s_ofcshoot4, "ofcshoot4"),
    std::make_pair<statetype*, std::string>(&s_ofcchase1, "ofcchase1"),
    std::make_pair<statetype*, std::string>(&s_ofcchase1s, "ofcchase1s"),
    std::make_pair<statetype*, std::string>(&s_ofcchase2, "ofcchase2"),
    std::make_pair<statetype*, std::string>(&s_ofcchase3, "ofcchase3"),
    std::make_pair<statetype*, std::string>(&s_ofcchase3s, "ofcchase3s"),
    std::make_pair<statetype*, std::string>(&s_ofcchase4, "ofcchase4"),
    std::make_pair<statetype*, std::string>(&s_ofcdie1, "ofcdie1"),
    std::make_pair<statetype*, std::string>(&s_ofcdie2, "ofcdie2"),
    std::make_pair<statetype*, std::string>(&s_ofcdie3, "ofcdie3"),
    std::make_pair<statetype*, std::string>(&s_ofcdie4, "ofcdie4"),
    std::make_pair<statetype*, std::string>(&s_ofcdie5, "ofcdie5"),
    std::make_pair<statetype*, std::string>(&s_mutstand, "mutstand"),
    std::make_pair<statetype*, std::string>(&s_mutpath1, "mutpath1"),
    std::make_pair<statetype*, std::string>(&s_mutpath1s, "mutpath1s"),
    std::make_pair<statetype*, std::string>(&s_mutpath2, "mutpath2"),
    std::make_pair<statetype*, std::string>(&s_mutpath3, "mutpath3"),
    std::make_pair<statetype*, std::string>(&s_mutpath3s, "mutpath3s"),
    std::make_pair<statetype*, std::string>(&s_mutpath4, "mutpath4"),
    std::make_pair<statetype*, std::string>(&s_mutpain, "mutpain"),
    std::make_pair<statetype*, std::string>(&s_mutpain1, "mutpain1"),
    //std::make_pair<statetype*, std::string>(&s_mutgiveup, "mutgiveup"),
    std::make_pair<statetype*, std::string>(&s_mutshoot1, "mutshoot1"),
    std::make_pair<statetype*, std::string>(&s_mutshoot2, "mutshoot2"),
    std::make_pair<statetype*, std::string>(&s_mutshoot3, "mutshoot3"),
    std::make_pair<statetype*, std::string>(&s_mutshoot4, "mutshoot4"),
    std::make_pair<statetype*, std::string>(&s_mutchase1, "mutchase1"),
    std::make_pair<statetype*, std::string>(&s_mutchase1s, "mutchase1s"),
    std::make_pair<statetype*, std::string>(&s_mutchase2, "mutchase2"),
    std::make_pair<statetype*, std::string>(&s_mutchase3, "mutchase3"),
    std::make_pair<statetype*, std::string>(&s_mutchase3s, "mutchase3s"),
    std::make_pair<statetype*, std::string>(&s_mutchase4, "mutchase4"),
    std::make_pair<statetype*, std::string>(&s_mutdie1, "mutdie1"),
    std::make_pair<statetype*, std::string>(&s_mutdie2, "mutdie2"),
    std::make_pair<statetype*, std::string>(&s_mutdie3, "mutdie3"),
    std::make_pair<statetype*, std::string>(&s_mutdie4, "mutdie4"),
    std::make_pair<statetype*, std::string>(&s_mutdie5, "mutdie5"),
    std::make_pair<statetype*, std::string>(&s_ssstand, "ssstand"),
    std::make_pair<statetype*, std::string>(&s_sspath1, "sspath1"),
    std::make_pair<statetype*, std::string>(&s_sspath1s, "sspath1s"),
    std::make_pair<statetype*, std::string>(&s_sspath2, "sspath2"),
    std::make_pair<statetype*, std::string>(&s_sspath3, "sspath3"),
    std::make_pair<statetype*, std::string>(&s_sspath3s, "sspath3s"),
    std::make_pair<statetype*, std::string>(&s_sspath4, "sspath4"),
    std::make_pair<statetype*, std::string>(&s_sspain, "sspain"),
    std::make_pair<statetype*, std::string>(&s_sspain1, "sspain1"),
    std::make_pair<statetype*, std::string>(&s_ssshoot1, "ssshoot1"),
    std::make_pair<statetype*, std::string>(&s_ssshoot2, "ssshoot2"),
    std::make_pair<statetype*, std::string>(&s_ssshoot3, "ssshoot3"),
    std::make_pair<statetype*, std::string>(&s_ssshoot4, "ssshoot4"),
    std::make_pair<statetype*, std::string>(&s_ssshoot5, "ssshoot5"),
    std::make_pair<statetype*, std::string>(&s_ssshoot6, "ssshoot6"),
    std::make_pair<statetype*, std::string>(&s_ssshoot7, "ssshoot7"),
    std::make_pair<statetype*, std::string>(&s_ssshoot8, "ssshoot8"),
    std::make_pair<statetype*, std::string>(&s_ssshoot9, "ssshoot9"),
    std::make_pair<statetype*, std::string>(&s_sschase1, "sschase1"),
    std::make_pair<statetype*, std::string>(&s_sschase1s, "sschase1s"),
    std::make_pair<statetype*, std::string>(&s_sschase2, "sschase2"),
    std::make_pair<statetype*, std::string>(&s_sschase3, "sschase3"),
    std::make_pair<statetype*, std::string>(&s_sschase3s, "sschase3s"),
    std::make_pair<statetype*, std::string>(&s_sschase4, "sschase4"),
    std::make_pair<statetype*, std::string>(&s_ssdie1, "ssdie1"),
    std::make_pair<statetype*, std::string>(&s_ssdie2, "ssdie2"),
    std::make_pair<statetype*, std::string>(&s_ssdie3, "ssdie3"),
    std::make_pair<statetype*, std::string>(&s_ssdie4, "ssdie4"),
    std::make_pair<statetype*, std::string>(&s_bossstand, "bossstand"),
    std::make_pair<statetype*, std::string>(&s_bosschase1, "bosschase1"),
    std::make_pair<statetype*, std::string>(&s_bosschase1s, "bosschase1s"),
    std::make_pair<statetype*, std::string>(&s_bosschase2, "bosschase2"),
    std::make_pair<statetype*, std::string>(&s_bosschase3, "bosschase3"),
    std::make_pair<statetype*, std::string>(&s_bosschase3s, "bosschase3s"),
    std::make_pair<statetype*, std::string>(&s_bosschase4, "bosschase4"),
    std::make_pair<statetype*, std::string>(&s_bossdie1, "bossdie1"),
    std::make_pair<statetype*, std::string>(&s_bossdie2, "bossdie2"),
    std::make_pair<statetype*, std::string>(&s_bossdie3, "bossdie3"),
    std::make_pair<statetype*, std::string>(&s_bossdie4, "bossdie4"),
    std::make_pair<statetype*, std::string>(&s_bossshoot1, "bossshoot1"),
    std::make_pair<statetype*, std::string>(&s_bossshoot2, "bossshoot2"),
    std::make_pair<statetype*, std::string>(&s_bossshoot3, "bossshoot3"),
    std::make_pair<statetype*, std::string>(&s_bossshoot4, "bossshoot4"),
    std::make_pair<statetype*, std::string>(&s_bossshoot5, "bossshoot5"),
    std::make_pair<statetype*, std::string>(&s_bossshoot6, "bossshoot6"),
    std::make_pair<statetype*, std::string>(&s_bossshoot7, "bossshoot7"),
    std::make_pair<statetype*, std::string>(&s_bossshoot8, "bossshoot8"),
    std::make_pair<statetype*, std::string>(&s_gretelstand, "gretelstand"),
    std::make_pair<statetype*, std::string>(&s_gretelchase1, "gretelchase1"),
    std::make_pair<statetype*, std::string>(&s_gretelchase1s, "gretelchase1s"),
    std::make_pair<statetype*, std::string>(&s_gretelchase2, "gretelchase2"),
    std::make_pair<statetype*, std::string>(&s_gretelchase3, "gretelchase3"),
    std::make_pair<statetype*, std::string>(&s_gretelchase3s, "gretelchase3s"),
    std::make_pair<statetype*, std::string>(&s_gretelchase4, "gretelchase4"),
    std::make_pair<statetype*, std::string>(&s_greteldie1, "greteldie1"),
    std::make_pair<statetype*, std::string>(&s_greteldie2, "greteldie2"),
    std::make_pair<statetype*, std::string>(&s_greteldie3, "greteldie3"),
    std::make_pair<statetype*, std::string>(&s_greteldie4, "greteldie4"),
    std::make_pair<statetype*, std::string>(&s_gretelshoot1, "gretelshoot1"),
    std::make_pair<statetype*, std::string>(&s_gretelshoot2, "gretelshoot2"),
    std::make_pair<statetype*, std::string>(&s_gretelshoot3, "gretelshoot3"),
    std::make_pair<statetype*, std::string>(&s_gretelshoot4, "gretelshoot4"),
    std::make_pair<statetype*, std::string>(&s_gretelshoot5, "gretelshoot5"),
    std::make_pair<statetype*, std::string>(&s_gretelshoot6, "gretelshoot6"),
    std::make_pair<statetype*, std::string>(&s_gretelshoot7, "gretelshoot7"),
    std::make_pair<statetype*, std::string>(&s_gretelshoot8, "gretelshoot8"),
    //std::make_pair<statetype*, std::string>(&s_angelshoot1, "angelshoot1"),
    //std::make_pair<statetype*, std::string>(&s_deathshoot1, "deathshoot1"),
    //std::make_pair<statetype*, std::string>(&s_spark1, "spark1"),
    //std::make_pair<statetype*, std::string>(&s_transstand, "transstand"),
    //std::make_pair<statetype*, std::string>(&s_transchase1, "transchase1"),
    //std::make_pair<statetype*, std::string>(&s_transchase1s, "transchase1s"),
    //std::make_pair<statetype*, std::string>(&s_transchase2, "transchase2"),
    //std::make_pair<statetype*, std::string>(&s_transchase3, "transchase3"),
    //std::make_pair<statetype*, std::string>(&s_transchase3s, "transchase3s"),
    //std::make_pair<statetype*, std::string>(&s_transchase4, "transchase4"),
    //std::make_pair<statetype*, std::string>(&s_transdie0, "transdie0"),
    //std::make_pair<statetype*, std::string>(&s_transdie01, "transdie01"),
    //std::make_pair<statetype*, std::string>(&s_transdie1, "transdie1"),
    //std::make_pair<statetype*, std::string>(&s_transdie2, "transdie2"),
    //std::make_pair<statetype*, std::string>(&s_transdie3, "transdie3"),
    //std::make_pair<statetype*, std::string>(&s_transdie4, "transdie4"),
    //std::make_pair<statetype*, std::string>(&s_transshoot1, "transshoot1"),
    //std::make_pair<statetype*, std::string>(&s_transshoot2, "transshoot2"),
    //std::make_pair<statetype*, std::string>(&s_transshoot3, "transshoot3"),
    //std::make_pair<statetype*, std::string>(&s_transshoot4, "transshoot4"),
    //std::make_pair<statetype*, std::string>(&s_transshoot5, "transshoot5"),
    //std::make_pair<statetype*, std::string>(&s_transshoot6, "transshoot6"),
    //std::make_pair<statetype*, std::string>(&s_transshoot7, "transshoot7"),
    //std::make_pair<statetype*, std::string>(&s_transshoot8, "transshoot8"),
    //std::make_pair<statetype*, std::string>(&s_uberstand, "uberstand"),
    //std::make_pair<statetype*, std::string>(&s_uberchase1, "uberchase1"),
    //std::make_pair<statetype*, std::string>(&s_uberchase1s, "uberchase1s"),
    //std::make_pair<statetype*, std::string>(&s_uberchase2, "uberchase2"),
    //std::make_pair<statetype*, std::string>(&s_uberchase3, "uberchase3"),
    //std::make_pair<statetype*, std::string>(&s_uberchase3s, "uberchase3s"),
    //std::make_pair<statetype*, std::string>(&s_uberchase4, "uberchase4"),
    //std::make_pair<statetype*, std::string>(&s_uberdie0, "uberdie0"),
    //std::make_pair<statetype*, std::string>(&s_uberdie01, "uberdie01"),
    //std::make_pair<statetype*, std::string>(&s_uberdie1, "uberdie1"),
    //std::make_pair<statetype*, std::string>(&s_uberdie2, "uberdie2"),
    //std::make_pair<statetype*, std::string>(&s_uberdie3, "uberdie3"),
    //std::make_pair<statetype*, std::string>(&s_uberdie4, "uberdie4"),
    //std::make_pair<statetype*, std::string>(&s_uberdie5, "uberdie5"),
    //std::make_pair<statetype*, std::string>(&s_ubershoot1, "ubershoot1"),
    //std::make_pair<statetype*, std::string>(&s_ubershoot2, "ubershoot2"),
    //std::make_pair<statetype*, std::string>(&s_ubershoot3, "ubershoot3"),
    //std::make_pair<statetype*, std::string>(&s_ubershoot4, "ubershoot4"),
    //std::make_pair<statetype*, std::string>(&s_ubershoot5, "ubershoot5"),
    //std::make_pair<statetype*, std::string>(&s_ubershoot6, "ubershoot6"),
    //std::make_pair<statetype*, std::string>(&s_ubershoot7, "ubershoot7"),
    //std::make_pair<statetype*, std::string>(&s_willstand, "willstand"),
    //std::make_pair<statetype*, std::string>(&s_willchase1, "willchase1"),
    //std::make_pair<statetype*, std::string>(&s_willchase1s, "willchase1s"),
    //std::make_pair<statetype*, std::string>(&s_willchase2, "willchase2"),
    //std::make_pair<statetype*, std::string>(&s_willchase3, "willchase3"),
    //std::make_pair<statetype*, std::string>(&s_willchase3s, "willchase3s"),
    //std::make_pair<statetype*, std::string>(&s_willchase4, "willchase4"),
    //std::make_pair<statetype*, std::string>(&s_willdie1, "willdie1"),
    //std::make_pair<statetype*, std::string>(&s_willdie2, "willdie2"),
    //std::make_pair<statetype*, std::string>(&s_willdie3, "willdie3"),
    //std::make_pair<statetype*, std::string>(&s_willdie4, "willdie4"),
    //std::make_pair<statetype*, std::string>(&s_willdie5, "willdie5"),
    //std::make_pair<statetype*, std::string>(&s_willdie6, "willdie6"),
    //std::make_pair<statetype*, std::string>(&s_willshoot1, "willshoot1"),
    //std::make_pair<statetype*, std::string>(&s_willshoot2, "willshoot2"),
    //std::make_pair<statetype*, std::string>(&s_willshoot3, "willshoot3"),
    //std::make_pair<statetype*, std::string>(&s_willshoot4, "willshoot4"),
    //std::make_pair<statetype*, std::string>(&s_willshoot5, "willshoot5"),
    //std::make_pair<statetype*, std::string>(&s_willshoot6, "willshoot6"),
    //std::make_pair<statetype*, std::string>(&s_deathstand, "deathstand"),
    //std::make_pair<statetype*, std::string>(&s_deathchase1, "deathchase1"),
    //std::make_pair<statetype*, std::string>(&s_deathchase1s, "deathchase1s"),
    //std::make_pair<statetype*, std::string>(&s_deathchase2, "deathchase2"),
    //std::make_pair<statetype*, std::string>(&s_deathchase3, "deathchase3"),
    //std::make_pair<statetype*, std::string>(&s_deathchase3s, "deathchase3s"),
    //std::make_pair<statetype*, std::string>(&s_deathchase4, "deathchase4"),
    //std::make_pair<statetype*, std::string>(&s_deathdie1, "deathdie1"),
    //std::make_pair<statetype*, std::string>(&s_deathdie2, "deathdie2"),
    //std::make_pair<statetype*, std::string>(&s_deathdie3, "deathdie3"),
    //std::make_pair<statetype*, std::string>(&s_deathdie4, "deathdie4"),
    //std::make_pair<statetype*, std::string>(&s_deathdie5, "deathdie5"),
    //std::make_pair<statetype*, std::string>(&s_deathdie6, "deathdie6"),
    //std::make_pair<statetype*, std::string>(&s_deathdie7, "deathdie7"),
    //std::make_pair<statetype*, std::string>(&s_deathdie8, "deathdie8"),
    //std::make_pair<statetype*, std::string>(&s_deathdie9, "deathdie9"),
    //std::make_pair<statetype*, std::string>(&s_deathshoot1, "deathshoot1"),
    //std::make_pair<statetype*, std::string>(&s_deathshoot2, "deathshoot2"),
    //std::make_pair<statetype*, std::string>(&s_deathshoot3, "deathshoot3"),
    //std::make_pair<statetype*, std::string>(&s_deathshoot4, "deathshoot4"),
    //std::make_pair<statetype*, std::string>(&s_deathshoot5, "deathshoot5"),
    //std::make_pair<statetype*, std::string>(&s_angelstand, "angelstand"),
    //std::make_pair<statetype*, std::string>(&s_angelchase1, "angelchase1"),
    //std::make_pair<statetype*, std::string>(&s_angelchase1s, "angelchase1s"),
    //std::make_pair<statetype*, std::string>(&s_angelchase2, "angelchase2"),
    //std::make_pair<statetype*, std::string>(&s_angelchase3, "angelchase3"),
    //std::make_pair<statetype*, std::string>(&s_angelchase3s, "angelchase3s"),
    //std::make_pair<statetype*, std::string>(&s_angelchase4, "angelchase4"),
    //std::make_pair<statetype*, std::string>(&s_angeldie1, "angeldie1"),
    //std::make_pair<statetype*, std::string>(&s_angeldie11, "angeldie11"),
    //std::make_pair<statetype*, std::string>(&s_angeldie2, "angeldie2"),
    //std::make_pair<statetype*, std::string>(&s_angeldie3, "angeldie3"),
    //std::make_pair<statetype*, std::string>(&s_angeldie4, "angeldie4"),
    //std::make_pair<statetype*, std::string>(&s_angeldie5, "angeldie5"),
    //std::make_pair<statetype*, std::string>(&s_angeldie6, "angeldie6"),
    //std::make_pair<statetype*, std::string>(&s_angeldie7, "angeldie7"),
    //std::make_pair<statetype*, std::string>(&s_angeldie8, "angeldie8"),
    //std::make_pair<statetype*, std::string>(&s_angeldie9, "angeldie9"),
    //std::make_pair<statetype*, std::string>(&s_angelshoot1, "angelshoot1"),
    //std::make_pair<statetype*, std::string>(&s_angelshoot2, "angelshoot2"),
    //std::make_pair<statetype*, std::string>(&s_angelshoot3, "angelshoot3"),
    //std::make_pair<statetype*, std::string>(&s_angelshoot4, "angelshoot4"),
    //std::make_pair<statetype*, std::string>(&s_angelshoot5, "angelshoot5"),
    //std::make_pair<statetype*, std::string>(&s_angelshoot6, "angelshoot6"),
    //std::make_pair<statetype*, std::string>(&s_angeltired, "angeltired"),
    //std::make_pair<statetype*, std::string>(&s_angeltired2, "angeltired2"),
    //std::make_pair<statetype*, std::string>(&s_angeltired3, "angeltired3"),
    //std::make_pair<statetype*, std::string>(&s_angeltired4, "angeltired4"),
    //std::make_pair<statetype*, std::string>(&s_angeltired5, "angeltired5"),
    //std::make_pair<statetype*, std::string>(&s_angeltired6, "angeltired6"),
    //std::make_pair<statetype*, std::string>(&s_angeltired7, "angeltired7"),
    //std::make_pair<statetype*, std::string>(&s_spark1, "spark1"),
    //std::make_pair<statetype*, std::string>(&s_spark2, "spark2"),
    //std::make_pair<statetype*, std::string>(&s_spark3, "spark3"),
    //std::make_pair<statetype*, std::string>(&s_spark4, "spark4"),
    //std::make_pair<statetype*, std::string>(&s_spectrewait1, "spectrewait1"),
    //std::make_pair<statetype*, std::string>(&s_spectrewait2, "spectrewait2"),
    //std::make_pair<statetype*, std::string>(&s_spectrewait3, "spectrewait3"),
    //std::make_pair<statetype*, std::string>(&s_spectrewait4, "spectrewait4"),
    //std::make_pair<statetype*, std::string>(&s_spectrechase1, "spectrechase1"),
    //std::make_pair<statetype*, std::string>(&s_spectrechase2, "spectrechase2"),
    //std::make_pair<statetype*, std::string>(&s_spectrechase3, "spectrechase3"),
    //std::make_pair<statetype*, std::string>(&s_spectrechase4, "spectrechase4"),
    //std::make_pair<statetype*, std::string>(&s_spectredie1, "spectredie1"),
    //std::make_pair<statetype*, std::string>(&s_spectredie2, "spectredie2"),
    //std::make_pair<statetype*, std::string>(&s_spectredie3, "spectredie3"),
    //std::make_pair<statetype*, std::string>(&s_spectredie4, "spectredie4"),
    //std::make_pair<statetype*, std::string>(&s_spectrewake, "spectrewake"),
    std::make_pair<statetype*, std::string>(&s_schabbstand, "schabbstand"),
    std::make_pair<statetype*, std::string>(&s_schabbchase1, "schabbchase1"),
    std::make_pair<statetype*, std::string>(&s_schabbchase1s, "schabbchase1s"),
    std::make_pair<statetype*, std::string>(&s_schabbchase2, "schabbchase2"),
    std::make_pair<statetype*, std::string>(&s_schabbchase3, "schabbchase3"),
    std::make_pair<statetype*, std::string>(&s_schabbchase3s, "schabbchase3s"),
    std::make_pair<statetype*, std::string>(&s_schabbchase4, "schabbchase4"),
    std::make_pair<statetype*, std::string>(&s_schabbdie1, "schabbdie1"),
    std::make_pair<statetype*, std::string>(&s_schabbdie2, "schabbdie2"),
    std::make_pair<statetype*, std::string>(&s_schabbdie3, "schabbdie3"),
    std::make_pair<statetype*, std::string>(&s_schabbdie4, "schabbdie4"),
    std::make_pair<statetype*, std::string>(&s_schabbdie5, "schabbdie5"),
    std::make_pair<statetype*, std::string>(&s_schabbdie6, "schabbdie6"),
    std::make_pair<statetype*, std::string>(&s_schabbshoot1, "schabbshoot1"),
    std::make_pair<statetype*, std::string>(&s_schabbshoot2, "schabbshoot2"),
    std::make_pair<statetype*, std::string>(&s_needle1, "needle1"),
    std::make_pair<statetype*, std::string>(&s_needle2, "needle2"),
    std::make_pair<statetype*, std::string>(&s_needle3, "needle3"),
    std::make_pair<statetype*, std::string>(&s_needle4, "needle4"),
    std::make_pair<statetype*, std::string>(&s_schabbdeathcam, "schabbdeathcam"),
    std::make_pair<statetype*, std::string>(&s_giftstand, "giftstand"),
    std::make_pair<statetype*, std::string>(&s_giftchase1, "giftchase1"),
    std::make_pair<statetype*, std::string>(&s_giftchase1s, "giftchase1s"),
    std::make_pair<statetype*, std::string>(&s_giftchase2, "giftchase2"),
    std::make_pair<statetype*, std::string>(&s_giftchase3, "giftchase3"),
    std::make_pair<statetype*, std::string>(&s_giftchase3s, "giftchase3s"),
    std::make_pair<statetype*, std::string>(&s_giftchase4, "giftchase4"),
    std::make_pair<statetype*, std::string>(&s_giftdie1, "giftdie1"),
    std::make_pair<statetype*, std::string>(&s_giftdie2, "giftdie2"),
    std::make_pair<statetype*, std::string>(&s_giftdie3, "giftdie3"),
    std::make_pair<statetype*, std::string>(&s_giftdie4, "giftdie4"),
    std::make_pair<statetype*, std::string>(&s_giftdie5, "giftdie5"),
    std::make_pair<statetype*, std::string>(&s_giftdie6, "giftdie6"),
    std::make_pair<statetype*, std::string>(&s_giftshoot1, "giftshoot1"),
    std::make_pair<statetype*, std::string>(&s_giftshoot2, "giftshoot2"),
    std::make_pair<statetype*, std::string>(&s_needle1, "needle1"),
    std::make_pair<statetype*, std::string>(&s_needle2, "needle2"),
    std::make_pair<statetype*, std::string>(&s_needle3, "needle3"),
    std::make_pair<statetype*, std::string>(&s_needle4, "needle4"),
    std::make_pair<statetype*, std::string>(&s_giftdeathcam, "giftdeathcam"),
    std::make_pair<statetype*, std::string>(&s_boom1, "boom1"),
    std::make_pair<statetype*, std::string>(&s_boom2, "boom2"),
    std::make_pair<statetype*, std::string>(&s_boom3, "boom3"),
    std::make_pair<statetype*, std::string>(&s_fatstand, "fatstand"),
    std::make_pair<statetype*, std::string>(&s_fatchase1, "fatchase1"),
    std::make_pair<statetype*, std::string>(&s_fatchase1s, "fatchase1s"),
    std::make_pair<statetype*, std::string>(&s_fatchase2, "fatchase2"),
    std::make_pair<statetype*, std::string>(&s_fatchase3, "fatchase3"),
    std::make_pair<statetype*, std::string>(&s_fatchase3s, "fatchase3s"),
    std::make_pair<statetype*, std::string>(&s_fatchase4, "fatchase4"),
    std::make_pair<statetype*, std::string>(&s_fatdie1, "fatdie1"),
    std::make_pair<statetype*, std::string>(&s_fatdie2, "fatdie2"),
    std::make_pair<statetype*, std::string>(&s_fatdie3, "fatdie3"),
    std::make_pair<statetype*, std::string>(&s_fatdie4, "fatdie4"),
    std::make_pair<statetype*, std::string>(&s_fatdie5, "fatdie5"),
    std::make_pair<statetype*, std::string>(&s_fatdie6, "fatdie6"),
    std::make_pair<statetype*, std::string>(&s_fatshoot1, "fatshoot1"),
    std::make_pair<statetype*, std::string>(&s_fatshoot2, "fatshoot2"),
    std::make_pair<statetype*, std::string>(&s_fatshoot3, "fatshoot3"),
    std::make_pair<statetype*, std::string>(&s_fatshoot4, "fatshoot4"),
    std::make_pair<statetype*, std::string>(&s_fatshoot5, "fatshoot5"),
    std::make_pair<statetype*, std::string>(&s_fatshoot6, "fatshoot6"),
    std::make_pair<statetype*, std::string>(&s_needle1, "needle1"),
    std::make_pair<statetype*, std::string>(&s_needle2, "needle2"),
    std::make_pair<statetype*, std::string>(&s_needle3, "needle3"),
    std::make_pair<statetype*, std::string>(&s_needle4, "needle4"),
    std::make_pair<statetype*, std::string>(&s_fatdeathcam, "fatdeathcam"),
    std::make_pair<statetype*, std::string>(&s_fakestand, "fakestand"),
    std::make_pair<statetype*, std::string>(&s_fakechase1, "fakechase1"),
    std::make_pair<statetype*, std::string>(&s_fakechase1s, "fakechase1s"),
    std::make_pair<statetype*, std::string>(&s_fakechase2, "fakechase2"),
    std::make_pair<statetype*, std::string>(&s_fakechase3, "fakechase3"),
    std::make_pair<statetype*, std::string>(&s_fakechase3s, "fakechase3s"),
    std::make_pair<statetype*, std::string>(&s_fakechase4, "fakechase4"),
    std::make_pair<statetype*, std::string>(&s_fakedie1, "fakedie1"),
    std::make_pair<statetype*, std::string>(&s_fakedie2, "fakedie2"),
    std::make_pair<statetype*, std::string>(&s_fakedie3, "fakedie3"),
    std::make_pair<statetype*, std::string>(&s_fakedie4, "fakedie4"),
    std::make_pair<statetype*, std::string>(&s_fakedie5, "fakedie5"),
    std::make_pair<statetype*, std::string>(&s_fakedie6, "fakedie6"),
    std::make_pair<statetype*, std::string>(&s_fakeshoot1, "fakeshoot1"),
    std::make_pair<statetype*, std::string>(&s_fakeshoot2, "fakeshoot2"),
    std::make_pair<statetype*, std::string>(&s_fakeshoot3, "fakeshoot3"),
    std::make_pair<statetype*, std::string>(&s_fakeshoot4, "fakeshoot4"),
    std::make_pair<statetype*, std::string>(&s_fakeshoot5, "fakeshoot5"),
    std::make_pair<statetype*, std::string>(&s_fakeshoot6, "fakeshoot6"),
    std::make_pair<statetype*, std::string>(&s_fakeshoot7, "fakeshoot7"),
    std::make_pair<statetype*, std::string>(&s_fakeshoot8, "fakeshoot8"),
    std::make_pair<statetype*, std::string>(&s_fakeshoot9, "fakeshoot9"),
    std::make_pair<statetype*, std::string>(&s_fire1, "fire1"),
    std::make_pair<statetype*, std::string>(&s_fire2, "fire2"),
    std::make_pair<statetype*, std::string>(&s_mechachase1, "mechachase1"),
    std::make_pair<statetype*, std::string>(&s_mechachase1s, "mechachase1s"),
    std::make_pair<statetype*, std::string>(&s_mechachase2, "mechachase2"),
    std::make_pair<statetype*, std::string>(&s_mechachase3, "mechachase3"),
    std::make_pair<statetype*, std::string>(&s_mechachase3s, "mechachase3s"),
    std::make_pair<statetype*, std::string>(&s_mechachase4, "mechachase4"),
    std::make_pair<statetype*, std::string>(&s_mechadie1, "mechadie1"),
    std::make_pair<statetype*, std::string>(&s_mechadie2, "mechadie2"),
    std::make_pair<statetype*, std::string>(&s_mechadie3, "mechadie3"),
    std::make_pair<statetype*, std::string>(&s_mechadie4, "mechadie4"),
    std::make_pair<statetype*, std::string>(&s_mechashoot1, "mechashoot1"),
    std::make_pair<statetype*, std::string>(&s_mechashoot2, "mechashoot2"),
    std::make_pair<statetype*, std::string>(&s_mechashoot3, "mechashoot3"),
    std::make_pair<statetype*, std::string>(&s_mechashoot4, "mechashoot4"),
    std::make_pair<statetype*, std::string>(&s_mechashoot5, "mechashoot5"),
    std::make_pair<statetype*, std::string>(&s_mechashoot6, "mechashoot6"),
    std::make_pair<statetype*, std::string>(&s_hitlerchase1, "hitlerchase1"),
    std::make_pair<statetype*, std::string>(&s_hitlerchase1s, "hitlerchase1s"),
    std::make_pair<statetype*, std::string>(&s_hitlerchase2, "hitlerchase2"),
    std::make_pair<statetype*, std::string>(&s_hitlerchase3, "hitlerchase3"),
    std::make_pair<statetype*, std::string>(&s_hitlerchase3s, "hitlerchase3s"),
    std::make_pair<statetype*, std::string>(&s_hitlerchase4, "hitlerchase4"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie1, "hitlerdie1"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie2, "hitlerdie2"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie3, "hitlerdie3"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie4, "hitlerdie4"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie5, "hitlerdie5"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie6, "hitlerdie6"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie7, "hitlerdie7"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie8, "hitlerdie8"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie9, "hitlerdie9"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie10, "hitlerdie10"),
    std::make_pair<statetype*, std::string>(&s_hitlershoot1, "hitlershoot1"),
    std::make_pair<statetype*, std::string>(&s_hitlershoot2, "hitlershoot2"),
    std::make_pair<statetype*, std::string>(&s_hitlershoot3, "hitlershoot3"),
    std::make_pair<statetype*, std::string>(&s_hitlershoot4, "hitlershoot4"),
    std::make_pair<statetype*, std::string>(&s_hitlershoot5, "hitlershoot5"),
    std::make_pair<statetype*, std::string>(&s_hitlershoot6, "hitlershoot6"),
    std::make_pair<statetype*, std::string>(&s_hitlerdeathcam, "hitlerdeathcam"),
    std::make_pair<statetype*, std::string>(&s_bjrun1, "bjrun1"),
    std::make_pair<statetype*, std::string>(&s_bjrun1s, "bjrun1s"),
    std::make_pair<statetype*, std::string>(&s_bjrun2, "bjrun2"),
    std::make_pair<statetype*, std::string>(&s_bjrun3, "bjrun3"),
    std::make_pair<statetype*, std::string>(&s_bjrun3s, "bjrun3s"),
    std::make_pair<statetype*, std::string>(&s_bjrun4, "bjrun4"),
    std::make_pair<statetype*, std::string>(&s_bjjump1, "bjjump1"),
    std::make_pair<statetype*, std::string>(&s_bjjump2, "bjjump2"),
    std::make_pair<statetype*, std::string>(&s_bjjump3, "bjjump3"),
    std::make_pair<statetype*, std::string>(&s_bjjump4, "bjjump4"),
    std::make_pair<statetype*, std::string>(&s_grddie1, "grddie1"),
    std::make_pair<statetype*, std::string>(&s_dogdie1, "dogdie1"),
    std::make_pair<statetype*, std::string>(&s_ofcdie1, "ofcdie1"),
    std::make_pair<statetype*, std::string>(&s_mutdie1, "mutdie1"),
    std::make_pair<statetype*, std::string>(&s_ssdie1, "ssdie1"),
    std::make_pair<statetype*, std::string>(&s_bossdie1, "bossdie1"),
    std::make_pair<statetype*, std::string>(&s_schabbdie1, "schabbdie1"),
    std::make_pair<statetype*, std::string>(&s_fakedie1, "fakedie1"),
    std::make_pair<statetype*, std::string>(&s_mechadie1, "mechadie1"),
    std::make_pair<statetype*, std::string>(&s_hitlerdie1, "hitlerdie1"),
    std::make_pair<statetype*, std::string>(&s_greteldie1, "greteldie1"),
    std::make_pair<statetype*, std::string>(&s_giftdie1, "giftdie1"),
    std::make_pair<statetype*, std::string>(&s_fatdie1, "fatdie1"),
    //std::make_pair<statetype*, std::string>(&s_spectredie1, "spectredie1"),
    //std::make_pair<statetype*, std::string>(&s_angeldie1, "angeldie1"),
    //std::make_pair<statetype*, std::string>(&s_transdie0, "transdie0"),
    //std::make_pair<statetype*, std::string>(&s_uberdie0, "uberdie0"),
    //std::make_pair<statetype*, std::string>(&s_willdie1, "willdie1"),
    //std::make_pair<statetype*, std::string>(&s_deathdie1, "deathdie1"),
    std::make_pair<statetype*, std::string>(&s_grdchase1, "grdchase1"),
    std::make_pair<statetype*, std::string>(&s_dogchase1, "dogchase1"),
    std::make_pair<statetype*, std::string>(&s_ofcchase1, "ofcchase1"),
    std::make_pair<statetype*, std::string>(&s_sschase1, "sschase1"),
    std::make_pair<statetype*, std::string>(&s_mutchase1, "mutchase1"),
    std::make_pair<statetype*, std::string>(&s_bosschase1, "bosschase1"),
    std::make_pair<statetype*, std::string>(&s_schabbchase1, "schabbchase1"),
    std::make_pair<statetype*, std::string>(&s_fakechase1, "fakechase1"),
    std::make_pair<statetype*, std::string>(&s_mechachase1, "mechachase1"),
    std::make_pair<statetype*, std::string>(&s_gretelchase1, "gretelchase1"),
    std::make_pair<statetype*, std::string>(&s_giftchase1, "giftchase1"),
    std::make_pair<statetype*, std::string>(&s_fatchase1, "fatchase1"),
    //std::make_pair<statetype*, std::string>(&s_spectrechase1, "spectrechase1"),
    //std::make_pair<statetype*, std::string>(&s_angelchase1, "angelchase1"),
    //std::make_pair<statetype*, std::string>(&s_transchase1, "transchase1"),
    //std::make_pair<statetype*, std::string>(&s_uberchase1, "uberchase1"),
    //std::make_pair<statetype*, std::string>(&s_willchase1, "willchase1"),
    //std::make_pair<statetype*, std::string>(&s_deathchase1, "deathchase1"),
    std::make_pair<statetype*, std::string>(&s_blinkychase1, "blinkychase1"),
    std::make_pair<statetype*, std::string>(&s_hitlerchase1, "hitlerchase1"),
    std::make_pair<statetype*, std::string>(&s_grdpain, "grdpain"),
    std::make_pair<statetype*, std::string>(&s_grdpain1, "grdpain1"),
    std::make_pair<statetype*, std::string>(&s_ofcpain, "ofcpain"),
    std::make_pair<statetype*, std::string>(&s_ofcpain1, "ofcpain1"),
    std::make_pair<statetype*, std::string>(&s_sspain, "sspain"),
    std::make_pair<statetype*, std::string>(&s_sspain1, "sspain1"),
    std::make_pair<statetype*, std::string>(&s_mutpain, "mutpain"),
    std::make_pair<statetype*, std::string>(&s_mutpain1, "mutpain1"),
    std::make_pair<statetype*, std::string>(&s_deathcam, "deathcam")
    //std::make_pair<statetype*, std::string>(&s_schabbdeathcam2, "schabbdeathcam2"),
    //std::make_pair<statetype*, std::string>(&s_hitlerdeathcam2, "hitlerdeathcam2"),
    //std::make_pair<statetype*, std::string>(&s_giftdeathcam2, "giftdeathcam2"),
    //std::make_pair<statetype*, std::string>(&s_fatdeathcam2, "fatdeathcam2")
};
std::map<std::string, statetype*> STR_2_STATE;

std::map<dirtype, std::string> DIR_2_STR = {
    std::make_pair<dirtype, std::string>(east, "east"),
    std::make_pair<dirtype, std::string>(northeast, "northeast"),
    std::make_pair<dirtype, std::string>(north, "north"),
    std::make_pair<dirtype, std::string>(northwest, "northwest"),
    std::make_pair<dirtype, std::string>(southwest, "southwest"),
    std::make_pair<dirtype, std::string>(south, "south"),
    std::make_pair<dirtype, std::string>(southeast, "southeast"),
    std::make_pair<dirtype, std::string>(nodir, "nodir")
};
std::map<std::string, dirtype> STR_2_DIR;

std::map<int, std::string> DOORACTION_2_STR = {
    std::make_pair<int, std::string>(doorobj_t::dr_open, "dr_open"),
    std::make_pair<int, std::string>(doorobj_t::dr_closed, "dr_closed"),
    std::make_pair<int, std::string>(doorobj_t::dr_opening, "dr_opening"),
    std::make_pair<int, std::string>(doorobj_t::dr_closing, "dr_closing")
};
std::map<std::string, int> STR_2_DOORACTION;

static void init_saveload_constants()
{
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    for (auto& kv : WP_2_STR)
    {
        STR_2_WP[kv.second] = kv.first;
    }

    for (auto& kv : ACTIVETYPE_TO_STR)
    {
        STR_2_ACTIVETYPE[kv.second] = kv.first;
    }

    for (auto& kv : CLASSTYPE_2_STR)
    {
        STR_2_CLASSTYPE[kv.second] = kv.first;
    }

    for (auto& kv : STATE_2_STR)
    {
        STR_2_STATE[kv.second] = kv.first;
    }

    for (auto& kv : DIR_2_STR)
    {
        STR_2_DIR[kv.second] = kv.first;
    }
}

static Json::Value gamestate_to_json(const gametype& gamestate)
{
    Json::Value json_gamestate;
    json_gamestate["difficulty"] = gamestate.difficulty;
    json_gamestate["mapon"] = gamestate.mapon;
    json_gamestate["oldscore"] = gamestate.oldscore;
    json_gamestate["score"] = gamestate.score;
    json_gamestate["nextextra"] = gamestate.nextextra;
    json_gamestate["lives"] = gamestate.lives;
    json_gamestate["health"] = gamestate.health;
    json_gamestate["ammo"] = gamestate.ammo;
    json_gamestate["keys"] = gamestate.keys;
    assert(WP_2_STR.find(gamestate.bestweapon) != WP_2_STR.end());
    json_gamestate["bestweapon"] = WP_2_STR[gamestate.bestweapon];
    assert(WP_2_STR.find(gamestate.weapon) != WP_2_STR.end());
    json_gamestate["weapon"] = WP_2_STR[gamestate.weapon];
    assert(WP_2_STR.find(gamestate.chosenweapon) != WP_2_STR.end());
    json_gamestate["chosenweapon"] = WP_2_STR[gamestate.chosenweapon];
    json_gamestate["faceframe"] = gamestate.faceframe;
    json_gamestate["attackframe"] = gamestate.attackframe;
    json_gamestate["attackcount"] = gamestate.attackcount;
    json_gamestate["weaponframe"] = gamestate.weaponframe;
    json_gamestate["episode"] = gamestate.episode;
    json_gamestate["secretcount"] = gamestate.secretcount;
    json_gamestate["treasurecount"] = gamestate.treasurecount;
    json_gamestate["killcount"] = gamestate.killcount;
    json_gamestate["secrettotal"] = gamestate.secrettotal;
    json_gamestate["treasuretotal"] = gamestate.treasuretotal;
    json_gamestate["killtotal"] = gamestate.killtotal;
    json_gamestate["TimeCount"] = gamestate.TimeCount;
    json_gamestate["killx"] = gamestate.killx;
    json_gamestate["killy"] = gamestate.killy;
    json_gamestate["victoryflag"] = gamestate.victoryflag ? true : false;
    return json_gamestate;
}

static Json::Value LevelRatio_to_json(const LRstruct& level_ratio)
{
    Json::Value json_LevelRatio;
    json_LevelRatio["killy"] = level_ratio.kill;
    json_LevelRatio["secret"] = level_ratio.secret;
    json_LevelRatio["treasure"] = level_ratio.treasure;
    json_LevelRatio["time"] = level_ratio.time;
    return json_LevelRatio;
}

static Json::Value LevelRatios_to_json(const LRstruct* level_ratios, int count)
{
    Json::Value json_LevelRatios(Json::arrayValue);
    for (int i = 0; i < count; ++i)
    {
        json_LevelRatios.append(LevelRatio_to_json(level_ratios[i]));
    }
    return json_LevelRatios;
}

static Json::Value tilemap_to_json(const byte* tilemap)
{
    Json::Value json_tilemap(Json::arrayValue);
    for (int i = 0; i < MAPSIZE * MAPSIZE; ++i)
    {
        json_tilemap.append(tilemap[i]);
    }
    return json_tilemap;
}

static Json::Value actorat_to_json(objtype** actorat)
{
    Json::Value json_actorat(Json::arrayValue);
    for (int i = 0; i < MAPSIZE * MAPSIZE; ++i)
    {
        auto pActorAt = actorat[i];
        auto val = (uintptr_t)pActorAt;
        if (val > 0xFFFF)
        {
            json_actorat.append((int)((uintptr_t)0x8000 + (uintptr_t)(objlist - pActorAt)));
        }
        else
        {
            json_actorat.append((int)val);
        }
    }
    return json_actorat;
}

static Json::Value areaconnect_to_json(const byte* areaconnect)
{
    Json::Value json_areaconnect(Json::arrayValue);
    for (int i = 0; i < NUMAREAS * NUMAREAS; ++i)
    {
        json_areaconnect.append(areaconnect[i]);
    }
    return json_areaconnect;
}

static Json::Value areabyplayer_to_json(const _boolean* areabyplayer)
{
    Json::Value json_areabyplayer(Json::arrayValue);
    for (int i = 0; i < NUMAREAS * NUMAREAS; ++i)
    {
        json_areabyplayer.append((byte)areabyplayer[i] ? true : false);
    }
    return json_areabyplayer;
}

static Json::Value object_to_json(const objtype& ob)
{
    Json::Value json_object;
    assert(ACTIVETYPE_TO_STR.find(ob.active) != ACTIVETYPE_TO_STR.end());
    json_object["active"] = ACTIVETYPE_TO_STR[ob.active];
    json_object["ticcount"] = ob.ticcount;
    assert(CLASSTYPE_2_STR.find(ob.obclass) != CLASSTYPE_2_STR.end());
    json_object["obclass"] = CLASSTYPE_2_STR[ob.obclass];
    printf("ob.state: %p\n", (void*)ob.state);
    assert(STATE_2_STR.find(ob.state) != STATE_2_STR.end());
    json_object["state"] = STATE_2_STR[ob.state];
    json_object["flags"] = ob.flags;
    json_object["distance"] = ob.distance;
    assert(DIR_2_STR.find(ob.dir) != DIR_2_STR.end());
    json_object["state"] = DIR_2_STR[ob.dir];
    json_object["x"] = ob.x;
    json_object["y"] = ob.y;
    json_object["tilex"] = ob.tilex;
    json_object["tiley"] = ob.tiley;
    json_object["areanumber"] = ob.areanumber;
    json_object["viewx"] = ob.viewx;
    json_object["viewheight"] = ob.viewheight;
    json_object["transx"] = ob.transx;
    json_object["transy"] = ob.transy;
    json_object["angle"] = ob.angle;
    json_object["hitpoints"] = ob.hitpoints;
    json_object["speed"] = ob.speed;
    json_object["temp1"] = ob.temp1;
    json_object["temp2"] = ob.temp2;
    json_object["temp3"] = ob.temp3;
    return json_object;
}

static Json::Value objects_to_json(objtype *ob, int16_t x, int16_t y)
{
    Json::Value json_objects(Json::arrayValue);
    for (; ob; ob = ob->next)
    {
        DiskFlopAnim(x, y);
        json_objects.append(object_to_json(*ob));
    }
    return json_objects;
}

static Json::Value statobj_to_json(const statobj_t& statobj)
{
    Json::Value json_statobj;
    json_statobj["tilex"] = statobj.tilex;
    json_statobj["tiley"] = statobj.tiley;
    json_statobj["shapenum"] = statobj.shapenum;
    json_statobj["flags"] = statobj.flags;
    json_statobj["itemnumber"] = statobj.itemnumber;
    return json_statobj;
}

static Json::Value statobjs_to_json(statobj_t *statobjlist)
{
    Json::Value json_sstatobjs(Json::arrayValue);
    for (int i = 0; i < MAXSTATS; ++i)
    {
        json_sstatobjs.append(statobj_to_json(statobjlist[i]));
    }
    return json_sstatobjs;
}

static Json::Value doorposition_to_json(uint16_t* doorposition)
{
    Json::Value json_doorposition(Json::arrayValue);
    for (int i = 0; i < MAXDOORS; ++i)
    {
        json_doorposition.append(doorposition[i]);
    }
    return json_doorposition;
}

static Json::Value doorobj_to_json(const doorobj_t& doorobj)
{
    Json::Value json_doorobj;
    json_doorobj["tilex"] = doorobj.tilex;
    json_doorobj["tiley"] = doorobj.tiley;
    json_doorobj["vertical"] = doorobj.vertical ? true : false;
    json_doorobj["lock"] = doorobj.lock;
    assert(DOORACTION_2_STR.find((int)doorobj.action) != DOORACTION_2_STR.end());
    json_doorobj["action"] = DOORACTION_2_STR[(int)doorobj.action];
    json_doorobj["ticcount"] = doorobj.ticcount;
    return json_doorobj;
}

static Json::Value doorobjlist_to_json(doorobj_t* doorobjlist)
{
    Json::Value json_doorobjlist(Json::arrayValue);
    for (int i = 0; i < MAXDOORS; ++i)
    {
        json_doorobjlist.append(doorobj_to_json(doorobjlist[i]));
    }
    return json_doorobjlist;
}


_boolean SaveTheGame(int16_t file,int16_t x,int16_t y)
{
    init_saveload_constants();

    Json::Value jsonDocument;

    //-----------------------
    DiskFlopAnim(x, y);
    jsonDocument["gamestate"] = gamestate_to_json(gamestate);
    DiskFlopAnim(x, y);
    jsonDocument["LevelRatios"] = LevelRatios_to_json(LevelRatios, 8);
    DiskFlopAnim(x, y);
    jsonDocument["tilemap"] = tilemap_to_json((byte*)tilemap);
    DiskFlopAnim(x, y);
    jsonDocument["actorat"] = actorat_to_json((objtype**)actorat);
    jsonDocument["areaconnect"] = areaconnect_to_json((byte*)areaconnect);
    jsonDocument["areabyplayer"] = areabyplayer_to_json((_boolean*)areabyplayer);
    jsonDocument["objects"] = objects_to_json(player, x, y);
    DiskFlopAnim(x, y);
    int laststatobji = (int)(laststatobj - statobjlist);
    jsonDocument["laststatobj"] = laststatobji;
    DiskFlopAnim(x, y);
    jsonDocument["statobjs"] = statobjs_to_json((statobj_t*)statobjlist);
	DiskFlopAnim(x,y);
    jsonDocument["doorposition"] = doorposition_to_json((uint16_t*)doorposition);
	DiskFlopAnim(x,y);
    jsonDocument["doorobjlist"] = doorobjlist_to_json((doorobj_t*)doorobjlist);
	DiskFlopAnim(x,y);
    jsonDocument["pwallstate"] = pwallstate;
    jsonDocument["pwallx"] = pwallx;
    jsonDocument["pwally"] = pwally;
    jsonDocument["pwalldir"] = pwalldir;
    jsonDocument["pwallpos"] = pwallpos;
    //-----------------------

    std::stringstream ss;
    ss << jsonDocument;
    auto str = ss.str();

    CA_FarWrite(file, (byte *)str.c_str(), (int32_t)str.size());




	//DiskFlopAnim(x,y);
	//CA_FarWrite (file,(byte *)&gamestate,sizeof(gamestate));
	//checksum = DoChecksum((byte *)&gamestate,sizeof(gamestate),checksum);

	//DiskFlopAnim(x,y);
//#ifdef SPEAR
//	CA_FarWrite (file,(byte *)&LevelRatios[0],sizeof(LRstruct)*20);
//	checksum = DoChecksum((byte *)&LevelRatios[0],sizeof(LRstruct)*20,checksum);
//#else
//	CA_FarWrite (file,(byte *)&LevelRatios[0],sizeof(LRstruct)*8);
//	checksum = DoChecksum((byte *)&LevelRatios[0],sizeof(LRstruct)*8,checksum);
//#endif

	//DiskFlopAnim(x,y);
	//CA_FarWrite (file,(byte *)tilemap,sizeof(tilemap));
	//checksum = DoChecksum((byte *)tilemap,sizeof(tilemap),checksum);
	//DiskFlopAnim(x,y);
	//CA_FarWrite (file,(byte *)actorat,sizeof(actorat));
	//checksum = DoChecksum((byte *)actorat,sizeof(actorat),checksum);

	//CA_FarWrite (file,(byte *)areaconnect,sizeof(areaconnect));
	//CA_FarWrite (file,(byte *)areabyplayer,sizeof(areabyplayer));

	//for (ob = player ; ob ; ob=ob->next)
	//{
	// DiskFlopAnim(x,y);
	// CA_FarWrite (file,(byte *)ob,sizeof(*ob));
	//}
	//nullobj.active = ac_badobject;          // end of file marker
	//DiskFlopAnim(x,y);
	//CA_FarWrite (file,(byte *)&nullobj,sizeof(nullobj));



	//DiskFlopAnim(x,y);
	//CA_FarWrite (file,(byte *)&laststatobj,sizeof(laststatobj));
	//checksum = DoChecksum((byte *)&laststatobj,sizeof(laststatobj),checksum);
	// DiskFlopAnim(x,y);
	// CA_FarWrite (file,(byte *)statobjlist,sizeof(statobjlist));
	// checksum = DoChecksum((byte *)statobjlist,sizeof(statobjlist),checksum);

	// DiskFlopAnim(x,y);
	// CA_FarWrite (file,(byte *)doorposition,sizeof(doorposition));
	// checksum = DoChecksum((byte *)doorposition,sizeof(doorposition),checksum);
	// DiskFlopAnim(x,y);
	// CA_FarWrite (file,(byte *)doorobjlist,sizeof(doorobjlist));
	// checksum = DoChecksum((byte *)doorobjlist,sizeof(doorobjlist),checksum);

	// DiskFlopAnim(x,y);
	// CA_FarWrite (file,(byte *)&pwallstate,sizeof(pwallstate));
	// checksum = DoChecksum((byte *)&pwallstate,sizeof(pwallstate),checksum);
	// CA_FarWrite (file,(byte *)&pwallx,sizeof(pwallx));
	// checksum = DoChecksum((byte *)&pwallx,sizeof(pwallx),checksum);
	// CA_FarWrite (file,(byte *)&pwally,sizeof(pwally));
	// checksum = DoChecksum((byte *)&pwally,sizeof(pwally),checksum);
	// CA_FarWrite (file,(byte *)&pwalldir,sizeof(pwalldir));
	// checksum = DoChecksum((byte *)&pwalldir,sizeof(pwalldir),checksum);
	// CA_FarWrite (file,(byte *)&pwallpos,sizeof(pwallpos));
	// checksum = DoChecksum((byte *)&pwallpos,sizeof(pwallpos),checksum);

	//
	// WRITE OUT CHECKSUM
	//
	//CA_FarWrite (file,(byte *)&checksum,sizeof(checksum));

	return(_true);
}

//===========================================================================

/*
==================
=
= LoadTheGame
=
==================
*/

_boolean LoadTheGame(int16_t file,int16_t x,int16_t y)
{
	int32_t checksum,oldchecksum;
	objtype *ob,nullobj;


	checksum = 0;

	DiskFlopAnim(x,y);
	CA_FarRead (file,(byte *)&gamestate,sizeof(gamestate));
	checksum = DoChecksum((byte *)&gamestate,sizeof(gamestate),checksum);

	DiskFlopAnim(x,y);
#ifdef SPEAR
	CA_FarRead (file,(byte *)&LevelRatios[0],sizeof(LRstruct)*20);
	checksum = DoChecksum((byte *)&LevelRatios[0],sizeof(LRstruct)*20,checksum);
#else
	CA_FarRead (file,(byte *)&LevelRatios[0],sizeof(LRstruct)*8);
	checksum = DoChecksum((byte *)&LevelRatios[0],sizeof(LRstruct)*8,checksum);
#endif

	DiskFlopAnim(x,y);
	SetupGameLevel ();

	DiskFlopAnim(x,y);
	CA_FarRead (file,(byte *)tilemap,sizeof(tilemap));
	checksum = DoChecksum((byte *)tilemap,sizeof(tilemap),checksum);
	DiskFlopAnim(x,y);
	CA_FarRead (file,(byte *)actorat,sizeof(actorat));
	checksum = DoChecksum((byte *)actorat,sizeof(actorat),checksum);

	CA_FarRead (file,(byte *)areaconnect,sizeof(areaconnect));
	CA_FarRead (file,(byte *)areabyplayer,sizeof(areabyplayer));



	InitActorList ();
	DiskFlopAnim(x,y);
	CA_FarRead (file,(byte *)player,sizeof(*player));

	while (1)
	{
	 DiskFlopAnim(x,y);
		CA_FarRead (file,(byte *)&nullobj,sizeof(nullobj));
		if (nullobj.active == ac_badobject)
			break;
		GetNewActor ();
	 // don't copy over the links
		memcpy (_new,&nullobj,sizeof(nullobj)-4);
	}



	DiskFlopAnim(x,y);
	CA_FarRead (file,(byte *)&laststatobj,sizeof(laststatobj));
	checksum = DoChecksum((byte *)&laststatobj,sizeof(laststatobj),checksum);
	DiskFlopAnim(x,y);
	CA_FarRead (file,(byte *)statobjlist,sizeof(statobjlist));
	checksum = DoChecksum((byte *)statobjlist,sizeof(statobjlist),checksum);

	DiskFlopAnim(x,y);
	CA_FarRead (file,(byte *)doorposition,sizeof(doorposition));
	checksum = DoChecksum((byte *)doorposition,sizeof(doorposition),checksum);
	DiskFlopAnim(x,y);
	CA_FarRead (file,(byte *)doorobjlist,sizeof(doorobjlist));
	checksum = DoChecksum((byte *)doorobjlist,sizeof(doorobjlist),checksum);

	DiskFlopAnim(x,y);
	CA_FarRead (file,(byte *)&pwallstate,sizeof(pwallstate));
	checksum = DoChecksum((byte *)&pwallstate,sizeof(pwallstate),checksum);
	CA_FarRead (file,(byte *)&pwallx,sizeof(pwallx));
	checksum = DoChecksum((byte *)&pwallx,sizeof(pwallx),checksum);
	CA_FarRead (file,(byte *)&pwally,sizeof(pwally));
	checksum = DoChecksum((byte *)&pwally,sizeof(pwally),checksum);
	CA_FarRead (file,(byte *)&pwalldir,sizeof(pwalldir));
	checksum = DoChecksum((byte *)&pwalldir,sizeof(pwalldir),checksum);
	CA_FarRead (file,(byte *)&pwallpos,sizeof(pwallpos));
	checksum = DoChecksum((byte *)&pwallpos,sizeof(pwallpos),checksum);

	CA_FarRead (file,(byte *)&oldchecksum,sizeof(oldchecksum));

	if (oldchecksum != checksum)
	{
	 Message((char*)(STR_SAVECHT1"\n"
			 STR_SAVECHT2"\n"
			 STR_SAVECHT3"\n"
			 STR_SAVECHT4));

	 IN_ClearKeysDown();
	 IN_Ack();

	 gamestate.score = 0;
	 gamestate.lives = 1;
	 gamestate.weapon =
	   gamestate.chosenweapon =
	   gamestate.bestweapon = wp_pistol;
	 gamestate.ammo = 8;
	}

	return _true;
}

//===========================================================================

/*
==========================
=
= ShutdownId
=
= Shuts down all ID_?? managers
=
==========================
*/

void ShutdownId (void)
{
	US_Shutdown ();
	SD_Shutdown ();
	PM_Shutdown ();
	IN_Shutdown ();
	VW_Shutdown ();
	CA_Shutdown ();
	MM_Shutdown ();
}


//===========================================================================

/*
==================
=
= BuildTables
=
= Calculates:
=
= scale                 projection constant
= sintable/costable     overlapping fractional tables
=
==================
*/
const   float   radtoint = (float)(FINEANGLES/2/PI);

void BuildTables (void)
{
  int16_t           i;
  float         angle,anglestep;
  double        tang;
  fixed         value;


//
// calculate fine tangents
//

	for (i=0;i<FINEANGLES/8;i++)
	{
		tang = std::tan( ((double)i+0.5)/(double)radtoint);
		finetangent[i] = (int32_t)(tang*TILEGLOBAL);
		finetangent[FINEANGLES/4-1-i] = (int32_t)(1/tang*TILEGLOBAL);
	}

//
// costable overlays sintable with a quarter phase shift
// ANGLES is assumed to be divisable by four
//
// The low word of the value is the fraction, the high bit is the sign bit,
// bits 16-30 should be 0
//

  angle = 0;
  anglestep = (float)(PI/2/ANGLEQUAD);
  for (i=0;i<=ANGLEQUAD;i++)
  {
      auto ss = sin(angle);
	value=(fixed)(GLOBAL1*ss);
	sintable[i]=
	  sintable[i+ANGLES]=
	  sintable[ANGLES/2-i] = value;
	sintable[ANGLES-i]=
	  sintable[ANGLES/2+i] = -value;
	angle += anglestep;
  }
}

//===========================================================================


/*
====================
=
= CalcProjection
=
= Uses focallength
=
====================
*/

void CalcProjection (int32_t focal)
{
	int16_t             i;
	int32_t            intang;
	float   angle;
	double  tang;
	double  planedist;
	double  globinhalf;
	int16_t             halfview;
	double  halfangle,facedist;


	focallength = focal;
	facedist = focal+MINDIST;
	halfview = viewwidth/2;                                 // half view in pixels

//
// calculate scale value for vertical height calculations
// and sprite x calculations
//
	scale = (fixed)(halfview*facedist/(VIEWGLOBAL/2));

//
// divide heightnumerator by a posts distance to get the posts height for
// the heightbuffer.  The pixel height is height>>2
//
	heightnumerator = (TILEGLOBAL*scale)>>6;
	minheightdiv = heightnumerator/0x7fff +1;

//
// calculate the angle offset from view angle of each pixel's ray
//

	for (i=0;i<halfview;i++)
	{
	// start 1/2 pixel over, so viewangle bisects two middle pixels
		tang = (int32_t)i*VIEWGLOBAL/viewwidth/facedist;
		angle = (float)atan(tang);
		intang = (int32_t)(angle*radtoint);
		pixelangle[halfview-1-i] = intang;
		pixelangle[halfview+i] = -intang;
	}

//
// if a point's abs(y/x) is greater than maxslope, the point is outside
// the view area
//
	maxslope = finetangent[pixelangle[0]];
	maxslope >>= 8;
}



//===========================================================================

/*
===================
=
= SetupWalls
=
= Map tile values to scaled pics
=
===================
*/

void SetupWalls (void)
{
	int16_t     i;

	for (i=1;i<MAXWALLTILES;i++)
	{
		horizwall[i]=(i-1)*2;
		vertwall[i]=(i-1)*2+1;
	}
}

//===========================================================================

/*
==========================
=
= SignonScreen
=
==========================
*/

void SignonScreen (void)                        // VGA version
{
	  //uint16_t        segstart,seglength;

	  VL_SetVGAPlaneMode ();
	  VL_TestPaletteSet ();
	  VL_SetPalette ((byte*)gamepal);

	  //if (!virtualreality)
	  //{
	  //	VW_SetScreen((int16_t)0x8000,0);
	  //	VL_MungePic ((byte*)&introscn,320,200);
	  //	VL_MemToScreen ((byte*)&introscn,320,200,0,0);
	  //	VW_SetScreen(0,0);
	  //}

/*
 reclaim the memory from the linked in signon screen
*/
	 //segstart = FP_SEG(&introscn);
	 //seglength = 64000/16;
	 //if (FP_OFF(&introscn))
	 //{
	 //	segstart++;
	 //	seglength--;
	 //}
	 //MML_UseSpace (segstart,seglength);
}


/*
==========================
=
= FinishSignon
=
==========================
*/

void FinishSignon (void)
{

#ifndef SPEAR
	VW_Bar (0,189,300,11,0/*peekb(0xa000,0)*/);
	WindowX = 0;
	WindowW = 320;
	PrintY = 190;

	#ifndef JAPAN
	SETFONTCOLOR(14,4);

	#ifdef SPANISH
	US_CPrint ((char*)"Oprima una tecla");
	#else
	US_CPrint ((char*)"Press a key");
	#endif

	#endif

	if (!NoWait)
		IN_Ack ();

	#ifndef JAPAN
	VW_Bar (0,189,300,11,0/*peekb(0xa000,0)*/);

	PrintY = 190;
	SETFONTCOLOR(10,4);

	#ifdef SPANISH
	US_CPrint ((char*)"pensando...");
	#else
	US_CPrint ((char*)"Working...");
	#endif

	#endif

	SETFONTCOLOR(0,15);
#else
	if (!NoWait)
		VW_WaitVBL(3*70);
#endif
}

//===========================================================================

/*
=================
=
= MS_CheckParm
=
=================
*/

_boolean isalpha(char c)
{
    return 
       ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9')) ? _true : _false;
}

_boolean MS_CheckParm (char *check)
{
	int16_t             i;
	char    *parm;

	for (i = 1;i<ws_argc;i++)
	{
		parm = ws_argv[i];

		while ( !isalpha(*parm) )       // skip - / \ etc.. in front of parm
			if (!*parm++)
				break;                          // hit end of string without an alphanum

		if ( !strcmp(check,parm) )
			return _true;
	}

	return _false;
}

//===========================================================================

/*
=====================
=
= InitDigiMap
=
=====================
*/

static  int16_t     wolfdigimap[] =
		{
			// These first sounds are in the upload version
#ifndef SPEAR
			HALTSND,                0,
			DOGBARKSND,             1,
			CLOSEDOORSND,           2,
			OPENDOORSND,            3,
			ATKMACHINEGUNSND,       4,
			ATKPISTOLSND,           5,
			ATKGATLINGSND,          6,
			SCHUTZADSND,            7,
			GUTENTAGSND,            8,
			MUTTISND,               9,
			BOSSFIRESND,            10,
			SSFIRESND,              11,
			DEATHSCREAM1SND,        12,
			DEATHSCREAM2SND,        13,
			DEATHSCREAM3SND,        13,
			TAKEDAMAGESND,          14,
			PUSHWALLSND,            15,

			LEBENSND,               20,
			NAZIFIRESND,            21,
			SLURPIESND,             22,

			YEAHSND,				32,

#ifndef UPLOAD
			// These are in all other episodes
			DOGDEATHSND,            16,
			AHHHGSND,               17,
			DIESND,                 18,
			EVASND,                 19,

			TOT_HUNDSND,            23,
			MEINGOTTSND,            24,
			SCHABBSHASND,           25,
			HITLERHASND,            26,
			SPIONSND,               27,
			NEINSOVASSND,           28,
			DOGATTACKSND,           29,
			LEVELDONESND,           30,
			MECHSTEPSND,			31,

			SCHEISTSND,				33,
			DEATHSCREAM4SND,		34,		// AIIEEE
			DEATHSCREAM5SND,		35,		// DEE-DEE
			DONNERSND,				36,		// EPISODE 4 BOSS DIE
			EINESND,				37,		// EPISODE 4 BOSS SIGHTING
			ERLAUBENSND,			38,		// EPISODE 6 BOSS SIGHTING
			DEATHSCREAM6SND,		39,		// FART
			DEATHSCREAM7SND,		40,		// GASP
			DEATHSCREAM8SND,		41,		// GUH-BOY!
			DEATHSCREAM9SND,		42,		// AH GEEZ!
			KEINSND,				43,		// EPISODE 5 BOSS SIGHTING
			MEINSND,				44,		// EPISODE 6 BOSS DIE
			ROSESND,				45,		// EPISODE 5 BOSS DIE

#endif
#else
//
// SPEAR OF DESTINY DIGISOUNDS
//
			HALTSND,                0,
			CLOSEDOORSND,           2,
			OPENDOORSND,            3,
			ATKMACHINEGUNSND,       4,
			ATKPISTOLSND,           5,
			ATKGATLINGSND,          6,
			SCHUTZADSND,            7,
			BOSSFIRESND,            8,
			SSFIRESND,              9,
			DEATHSCREAM1SND,        10,
			DEATHSCREAM2SND,        11,
			TAKEDAMAGESND,          12,
			PUSHWALLSND,            13,
			AHHHGSND,               15,
			LEBENSND,               16,
			NAZIFIRESND,            17,
			SLURPIESND,             18,
			LEVELDONESND,           22,
			DEATHSCREAM4SND,		23,		// AIIEEE
			DEATHSCREAM3SND,        23,		// DOUBLY-MAPPED!!!
			DEATHSCREAM5SND,		24,		// DEE-DEE
			DEATHSCREAM6SND,		25,		// FART
			DEATHSCREAM7SND,		26,		// GASP
			DEATHSCREAM8SND,		27,		// GUH-BOY!
			DEATHSCREAM9SND,		28,		// AH GEEZ!
			GETGATLINGSND,			38,		// Got Gat replacement

#ifndef SPEARDEMO
			DOGBARKSND,             1,
			DOGDEATHSND,            14,
			SPIONSND,               19,
			NEINSOVASSND,           20,
			DOGATTACKSND,           21,
			TRANSSIGHTSND,			29,		// Trans Sight
			TRANSDEATHSND,			30,		// Trans Death
			WILHELMSIGHTSND,		31,		// Wilhelm Sight
			WILHELMDEATHSND,		32,		// Wilhelm Death
			UBERDEATHSND,			33,		// Uber Death
			KNIGHTSIGHTSND,			34,		// Death Knight Sight
			KNIGHTDEATHSND,			35,		// Death Knight Death
			ANGELSIGHTSND,			36,		// Angel Sight
			ANGELDEATHSND,			37,		// Angel Death
			GETSPEARSND,			39,		// Got Spear replacement
#endif
#endif
			LASTSOUND
		};


void InitDigiMap (void)
{
	int16_t                     *map;

	for (map = wolfdigimap;*map != LASTSOUND;map += 2)
		DigiMap[map[0]] = map[1];


}


#ifndef SPEAR
CP_iteminfo	MusicItems={CTL_X,CTL_Y,6,0,32};
CP_itemtype MusicMenu[]=
	{
		{1,"Get Them!",0},
		{1,"Searching",0},
		{1,"P.O.W.",0},
		{1,"Suspense",0},
		{1,"War March",0},
		{1,"Around The Corner!",0},

		{1,"Nazi Anthem",0},
		{1,"Lurking...",0},
		{1,"Going After Hitler",0},
		{1,"Pounding Headache",0},
		{1,"Into the Dungeons",0},
		{1,"Ultimate Conquest",0},

		{1,"Kill the S.O.B.",0},
		{1,"The Nazi Rap",0},
		{1,"Twelfth Hour",0},
		{1,"Zero Hour",0},
		{1,"Ultimate Conquest",0},
		{1,"Wolfpack",0}
	};
#else
CP_iteminfo MusicItems={CTL_X,CTL_Y-20,9,0,32};
CP_itemtype  MusicMenu[]=
   {
		{1,"Funky Colonel Bill",0},
		{1,"Death To The Nazis",0},
		{1,"Tiptoeing Around",0},
		{1,"Is This THE END?",0},
		{1,"Evil Incarnate",0},
		{1,"Jazzin' Them Nazis",0},
		{1,"Puttin' It To The Enemy",0},
		{1,"The SS Gonna Get You",0},
		{1,"Towering Above",0}
	};
#endif

#ifndef SPEARDEMO
void DoJukebox(void)
{
	int16_t which,lastsong=-1;
	uint16_t start,songs[]=
		{
#ifndef SPEAR
			GETTHEM_MUS,
			SEARCHN_MUS,
			POW_MUS,
			SUSPENSE_MUS,
			WARMARCH_MUS,
			CORNER_MUS,

			NAZI_OMI_MUS,
			PREGNANT_MUS,
			GOINGAFT_MUS,
			HEADACHE_MUS,
			DUNGEON_MUS,
			ULTIMATE_MUS,

			INTROCW3_MUS,
			NAZI_RAP_MUS,
			TWELFTH_MUS,
			ZEROHOUR_MUS,
			ULTIMATE_MUS,
			PACMAN_MUS
#else
			XFUNKIE_MUS,             // 0
			XDEATH_MUS,              // 2
			XTIPTOE_MUS,             // 4
			XTHEEND_MUS,             // 7
			XEVIL_MUS,               // 17
			XJAZNAZI_MUS,            // 18
			XPUTIT_MUS,              // 21
			XGETYOU_MUS,             // 22
			XTOWER2_MUS              // 23
#endif
		};
	decltype(std::chrono::steady_clock::now()) time;



	IN_ClearKeysDown();
	if (!AdLibPresent && !SoundBlasterPresent)
		return;


	MenuFadeOut();

#ifndef SPEAR
#ifndef UPLOAD
    time = std::chrono::steady_clock::now();
	start = (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(time).time_since_epoch()).count()%3)*6;
#else
	start = 0;
#endif
#else
	start = 0;
#endif


	CA_CacheGrChunk (STARTFONT+1);
#ifdef SPEAR
	CacheLump (BACKDROP_LUMP_START,BACKDROP_LUMP_END);
#else
	CacheLump (CONTROLS_LUMP_START,CONTROLS_LUMP_END);
#endif
	CA_LoadAllSounds ();

	fontnumber=1;
	ClearMScreen ();
	VWB_DrawPic(112,184,C_MOUSELBACKPIC);
	DrawStripes (10);
	SETFONTCOLOR (TEXTCOLOR,BKGDCOLOR);

#ifndef SPEAR
	DrawWindow (CTL_X-2,CTL_Y-6,280,13*7,BKGDCOLOR);
#else
	DrawWindow (CTL_X-2,CTL_Y-26,280,13*10,BKGDCOLOR);
#endif

	DrawMenu (&MusicItems,&MusicMenu[start]);

	SETFONTCOLOR (READHCOLOR,BKGDCOLOR);
	PrintY=15;
	WindowX = 0;
	WindowY = 320;
	US_CPrint ((char*)"Robert's Jukebox");

	SETFONTCOLOR (TEXTCOLOR,BKGDCOLOR);
	VW_UpdateScreen();
	MenuFadeIn();

	do
	{
		which = HandleMenu(&MusicItems,&MusicMenu[start],NULL);
		if (which>=0)
		{
			if (lastsong >= 0)
				MusicMenu[start+lastsong].active = 1;

			StartCPMusic(songs[start + which]);
			MusicMenu[start+which].active = 2;
			DrawMenu (&MusicItems,&MusicMenu[start]);
			VW_UpdateScreen();
			lastsong = which;
		}
	} while(which>=0);

	MenuFadeOut();
	IN_ClearKeysDown();
#ifdef SPEAR
	UnCacheLump (BACKDROP_LUMP_START,BACKDROP_LUMP_END);
#else
	UnCacheLump (CONTROLS_LUMP_START,CONTROLS_LUMP_END);
#endif
}
#endif


/*
==========================
=
= InitGame
=
= Load a few things right away
=
==========================
*/

void InitGame (void)
{
	int16_t                     i,x,y;
	uint16_t        *blockstart;

	if (MS_CheckParm ((char*)"virtual"))
		virtualreality = _true;
	else
		virtualreality = _false;

	MM_Startup ();                  // so the signon screen can be freed

	//SignonScreen ();

	VW_Startup ();
	IN_Startup ();
	PM_Startup ();
	PM_UnlockMainMem ();
	SD_Startup ();
	CA_Startup ();
	US_Startup ();

// #ifndef SPEAR
// 	if (mminfo.mainmem < 235000L)
// #else
// 	if (mminfo.mainmem < 257000L && !MS_CheckParm("debugmode"))
// #endif
// 	{
// 		// memptr screen;

// 		// CA_CacheGrChunk (ERRORSCREEN);
// 		// screen = grsegs[ERRORSCREEN];
// 		// ShutdownId();
// 		// movedata ((uint16_t)screen,7+7*160,0xb800,0,17*160);
// 		// gotoxy (1,23);
//         Quit((char*)"Error screen?");
// 		exit(1);
// 	}


//
// build some tables
//
	InitDigiMap ();

	for (i=0;i<MAPSIZE;i++)
	{
		nearmapylookup[i] = &tilemap[0][0]+MAPSIZE*i;
		farmapylookup[i] = i*64;
	}

	for (i=0;i<PORTTILESHIGH;i++)
		uwidthtable[i] = UPDATEWIDE*i;

	blockstart = &blockstarts[0];
	for (y=0;y<UPDATEHIGH;y++)
		for (x=0;x<UPDATEWIDE;x++)
			*blockstart++ = SCREENWIDTH*16*y+x*TILEWIDTH;

	updateptr = &update[0];

	bufferofs = 0;
	displayofs = 0;
	 ReadConfig (); // We have our own config.json


//
// HOLDING DOWN 'M' KEY?
//
#ifndef SPEARDEMO
	if (Keyboard[sc_M])
	  DoJukebox();
	else
#endif
//
// draw intro screen stuff
//
	//if (!virtualreality)
	//	IntroScreen ();

//
// load in and lock down some basic chunks
//

	CA_CacheGrChunk(STARTFONT);
	MM_SetLock (&grsegs[STARTFONT],_true);

	LoadLatchMem ();
	BuildTables ();          // trig tables
	SetupWalls ();

#if 0
{
int16_t temp,i;
temp = viewsize;
	profilehandle = file_open("SCALERS.TXT", O_CREAT | O_WRONLY | O_TEXT);
for (i=1;i<20;i++)
	NewViewSize(i);
viewsize = temp;
file_close(profilehandle);
}
#endif

	NewViewSize (viewsize);


//
// initialize variables
//
	InitRedShifts ();
	//if (!virtualreality)
	//	FinishSignon();

	displayofs = PAGE1START;
	bufferofs = PAGE2START;

	if (virtualreality)
	{
		NoWait = _true;
		// geninterrupt(0x60); // ??
	}
}

//===========================================================================

/*
==========================
=
= SetViewSize
=
==========================
*/

_boolean SetViewSize (uint16_t width, uint16_t height)
{
	viewwidth = width&~15;                  // must be divisable by 16
	viewheight = height&~1;                 // must be even
	centerx = viewwidth/2-1;
	shootdelta = viewwidth/10;
	screenofs = ((200-STATUSLINES-viewheight)/2*SCREENWIDTH+(320-viewwidth)/8);

//
// calculate trace angles and projection constants
//
	CalcProjection (FOCALLENGTH);

//
// build all needed compiled scalers
//
//	MM_BombOnError (_false);
	SetupScaling ((int16_t)(viewwidth*1.5));
#if 0
	MM_BombOnError (_true);
	if (mmerror)
	{
		Quit ("Can't build scalers!");
		mmerror = _false;
		return _false;
	}
#endif
	return _true;
}


void ShowViewSize (int16_t width)
{
	int16_t     oldwidth,oldheight;

	oldwidth = viewwidth;
	oldheight = viewheight;

	viewwidth = width*16;
	viewheight = (int16_t)(width*16*HEIGHTRATIO);
	DrawPlayBorder ();

	viewheight = oldheight;
	viewwidth = oldwidth;
}


void NewViewSize (int16_t width)
{
	CA_UpLevel ();
	MM_SortMem ();
	viewsize = width;
	SetViewSize (width*16,(uint16_t)(width*16*HEIGHTRATIO));
	CA_DownLevel ();
}



//===========================================================================

/*
==========================
=
= Quit
=
==========================
*/

void Quit (char *error)
{
	if (!error)
	{
	 //WriteConfig (); // We have our own config.json
	}
	else
	{
        tinyfd_messageBox("Wolf3D Shaders", error, "ok", "error", 1) ;
    }

	if (error && *error)
	{
	  exit(1);
	}

	exit(0);
}

//===========================================================================



/*
=====================
=
= DemoLoop
=
=====================
*/

static char *ParmStrings[] = {(char*)"baby",(char*)"easy",(char*)"normal",(char*)"hard",(char*)""};
void PG13 (void);

void    DemoLoop (void)
{
	static int16_t LastDemo;
	int16_t     i,level;
	int32_t nsize;
	memptr	nullblock;

//
// check for launch from ted
//
// 	if (tedlevel)
// 	{
// 		NoWait = _true;
// 		NewGame(1,0);

// 		for (i = 1;i < ws_argc;i++)
// 		{
// 			if ( (level = US_CheckParm(ws_argv[i],ParmStrings)) != -1)
// 			{
// 			 gamestate.difficulty=level;
// 			 break;
// 			}
// 		}

// #ifndef SPEAR
// 		gamestate.episode = tedlevelnum/10;
// 		gamestate.mapon = tedlevelnum%10;
// #else
// 		gamestate.episode = 0;
// 		gamestate.mapon = tedlevelnum;
// #endif
	// 	GameLoop();
	// 	Quit (NULL);
	// }


//
// main game cycle
//


	nsize = (int32_t)40*1024;
	MM_GetPtr(&nullblock,nsize);

 #ifndef DEMOTEST

 	#ifndef UPLOAD

 		#ifndef GOODTIMES
 		#ifndef SPEAR
 		#ifndef JAPAN
 		if (!NoWait)
 			NonShareware();
 		#endif
 		#else

 			#ifndef GOODTIMES
 			#ifndef SPEARDEMO
 			CopyProtection();
 			#endif
 			#endif

 		#endif
 		#endif
 	#endif

 	StartCPMusic(INTROSONG);

 #ifndef JAPAN
 	if (!NoWait)
 		PG13 ();
 #endif

 #endif

	while (1)
	{
		while (!NoWait)
		{
//
// title page
//
			MM_SortMem ();
#ifndef DEMOTEST

#ifdef SPEAR
			CA_CacheGrChunk (TITLEPALETTE);

			 CA_CacheGrChunk (TITLE1PIC);
			 VWB_DrawPic (0,0,TITLE1PIC);
			 UNCACHEGRCHUNK (TITLE1PIC);

			 CA_CacheGrChunk (TITLE2PIC);
			 VWB_DrawPic (0,80,TITLE2PIC);
			 UNCACHEGRCHUNK (TITLE2PIC);
			 VW_UpdateScreen ();
 			VL_FadeIn(0,255,grsegs[TITLEPALETTE],30);

 			UNCACHEGRCHUNK (TITLEPALETTE);
#else
 			CA_CacheScreen (TITLEPIC);
 			VW_UpdateScreen ();
 			VW_FadeIn();
#endif
 			if (IN_UserInput(TickBase*15))
 				break;
 			VW_FadeOut();

 //credits page

 			CA_CacheScreen (CREDITSPIC);
 			VW_UpdateScreen();
 			VW_FadeIn ();
 			if (IN_UserInput(TickBase*10))
 				break;
 			VW_FadeOut ();
 //
 // high scores
 //
 			DrawHighScores ();
 			VW_UpdateScreen ();
 			VW_FadeIn ();

 			if (IN_UserInput(TickBase*10))
 				break;
#endif
 //
 // demo
 //

 			#ifndef SPEARDEMO
 			PlayDemo (LastDemo++%4);
 			#else
 			PlayDemo (0);
 			#endif

 			if (playstate == ex_abort)
 				break;
 			StartCPMusic(INTROSONG);
		}

 		VW_FadeOut ();

 #ifndef SPEAR
 		if (Keyboard[sc_Tab] && MS_CheckParm((char*)"goobers"))
 #else
 		if (Keyboard[sc_Tab] && MS_CheckParm((char*)"debugmode"))
 #endif
 			RecordDemo ();
 		else
 			US_ControlPanel (0);

		 if (startgame || loadedgame)
		 {
		 	GameLoop ();
		 	VW_FadeOut();
		 	StartCPMusic(INTROSONG);
		 }
	}
}


// //===========================================================================


// /*
// ==========================
// =
// = main
// =
// ==========================
// */

// char    *nosprtxt[] = {(char*)"nospr",nil};

// int16_t main (int16_t argc, char** argv)
// {
// 	int16_t     i;


// // #ifdef BETA
	
// // 	THIS IS FOR BETA ONLY!
	
// // 	struct dosdate_t d;

// // 	_dos_getdate(&d);
// // 	if (d.year > YEAR ||
// // 		(d.month >= MONTH && d.day >= DAY))
// // 	{
// // 	 printf("Sorry, BETA-TESTING is over. Thanks for you help.\n");
// // 	 exit(1);
// // 	}
// // #endif

// 	CheckForEpisodes();

// 	// Patch386 ();

// 	InitGame ();

// 	DemoLoop();

// 	Quit((char*)"Demo loop exited???");

//     return 0;
// }

void wolf3d_init()
{
	CheckForEpisodes();
	InitGame();
    DemoLoop();
    Quit((char*)"Demo loop exited???");
}

void wolf3d_update()
{
}

void wolf3d_shutdown()
{
    
}

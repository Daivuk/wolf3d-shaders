// WL_ACT2.C

#include "WL_DEF.H"
#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define PROJECTILESIZE	0xc000l

#define BJRUNSPEED	2048
#define BJJUMPSPEED	680


/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/



/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/


dirtype dirtable[9] = {northwest,north,northeast,west,nodir,east,
	southwest,south,southeast};

int16_t	starthitpoints[4][NUMENEMIES] =
	 //
	 // BABY MODE
	 //
	 {
	 {25,	// guards
	  50,	// officer
	  100,	// SS
	  1,	// dogs
	  850,	// Hans
	  850,	// Schabbs
	  200,	// fake hitler
	  800,	// mecha hitler
	  45,	// mutants
	  25,	// ghosts
	  25,	// ghosts
	  25,	// ghosts
	  25,	// ghosts

	  850,	// Gretel
	  850,	// Gift
	  850,	// Fat
	  5,	// en_spectre,
	  1450,	// en_angel,
	  850,	// en_trans,
	  1050,	// en_uber,
	  950,	// en_will,
	  1250	// en_death
	  },
	 //
	 // DON'T HURT ME MODE
	 //
	 {25,	// guards
	  50,	// officer
	  100,	// SS
	  1,	// dogs
	  950,	// Hans
	  950,	// Schabbs
	  300,	// fake hitler
	  950,	// mecha hitler
	  55,	// mutants
	  25,	// ghosts
	  25,	// ghosts
	  25,	// ghosts
	  25,	// ghosts

	  950,	// Gretel
	  950,	// Gift
	  950,	// Fat
	  10,	// en_spectre,
	  1550,	// en_angel,
	  950,	// en_trans,
	  1150,	// en_uber,
	  1050,	// en_will,
	  1350	// en_death
	  },
	 //
	 // BRING 'EM ON MODE
	 //
	 {25,	// guards
	  50,	// officer
	  100,	// SS
	  1,	// dogs

	  1050,	// Hans
	  1550,	// Schabbs
	  400,	// fake hitler
	  1050,	// mecha hitler

	  55,	// mutants
	  25,	// ghosts
	  25,	// ghosts
	  25,	// ghosts
	  25,	// ghosts

	  1050,	// Gretel
	  1050,	// Gift
	  1050,	// Fat
	  15,	// en_spectre,
	  1650,	// en_angel,
	  1050,	// en_trans,
	  1250,	// en_uber,
	  1150,	// en_will,
	  1450	// en_death
	  },
	 //
	 // DEATH INCARNATE MODE
	 //
	 {25,	// guards
	  50,	// officer
	  100,	// SS
	  1,	// dogs

	  1200,	// Hans
	  2400,	// Schabbs
	  500,	// fake hitler
	  1200,	// mecha hitler

	  65,	// mutants
	  25,	// ghosts
	  25,	// ghosts
	  25,	// ghosts
	  25,	// ghosts

	  1200,	// Gretel
	  1200,	// Gift
	  1200,	// Fat
	  25,	// en_spectre,
	  2000,	// en_angel,
	  1200,	// en_trans,
	  1400,	// en_uber,
	  1300,	// en_will,
	  1600	// en_death
	  }}
	  ;

void	A_StartDeathCam (objtype *ob);


void	T_Path (objtype *ob);
void	T_Shoot (objtype *ob);
void	T_Bite (objtype *ob);
void	T_DogChase (objtype *ob);
void	T_Chase (objtype *ob);
void	T_Projectile (objtype *ob);
void	T_Stand (objtype *ob);

void A_DeathScream (objtype *ob);

extern	statetype s_rocket;
extern	statetype s_smoke1;
extern	statetype s_smoke2;
extern	statetype s_smoke3;
extern	statetype s_smoke4;
extern	statetype s_boom2;
extern	statetype s_boom3;

void A_Smoke (objtype *ob);

statetype s_rocket	 	= {_true,SPR_ROCKET_1,3,(void(*)())T_Projectile,(void(*)())A_Smoke,&s_rocket};
statetype s_smoke1	 	= {_false,SPR_SMOKE_1,3,NULL,NULL,&s_smoke2};
statetype s_smoke2	 	= {_false,SPR_SMOKE_2,3,NULL,NULL,&s_smoke3};
statetype s_smoke3	 	= {_false,SPR_SMOKE_3,3,NULL,NULL,&s_smoke4};
statetype s_smoke4	 	= {_false,SPR_SMOKE_4,3,NULL,NULL,NULL};

statetype s_boom1	 	= {_false,SPR_BOOM_1,6,NULL,NULL,&s_boom2};
statetype s_boom2	 	= {_false,SPR_BOOM_2,6,NULL,NULL,&s_boom3};
statetype s_boom3	 	= {_false,SPR_BOOM_3,6,NULL,NULL,NULL};

#ifdef SPEAR

extern	statetype s_hrocket;
extern	statetype s_hsmoke1;
extern	statetype s_hsmoke2;
extern	statetype s_hsmoke3;
extern	statetype s_hsmoke4;
extern	statetype s_hboom2;
extern	statetype s_hboom3;

void A_Smoke (objtype *ob);

statetype s_hrocket	 	= {_true,SPR_HROCKET_1,3,T_Projectile,A_Smoke,&s_hrocket};
statetype s_hsmoke1	 	= {_false,SPR_HSMOKE_1,3,NULL,NULL,&s_hsmoke2};
statetype s_hsmoke2	 	= {_false,SPR_HSMOKE_2,3,NULL,NULL,&s_hsmoke3};
statetype s_hsmoke3	 	= {_false,SPR_HSMOKE_3,3,NULL,NULL,&s_hsmoke4};
statetype s_hsmoke4	 	= {_false,SPR_HSMOKE_4,3,NULL,NULL,NULL};

statetype s_hboom1	 	= {_false,SPR_HBOOM_1,6,NULL,NULL,&s_hboom2};
statetype s_hboom2	 	= {_false,SPR_HBOOM_2,6,NULL,NULL,&s_hboom3};
statetype s_hboom3	 	= {_false,SPR_HBOOM_3,6,NULL,NULL,NULL};

#endif

void	T_Schabb (objtype *ob);
void	T_SchabbThrow (objtype *ob);
void	T_Fake (objtype *ob);
void	T_FakeFire (objtype *ob);
void	T_Ghosts (objtype *ob);

void A_Slurpie (objtype *ob);
void A_HitlerMorph (objtype *ob);
void A_MechaSound (objtype *ob);

/*
=================
=
= A_Smoke
=
=================
*/

void A_Smoke (objtype *ob)
{
	GetNewActor ();
#ifdef SPEAR
	if (ob->obclass == hrocketobj)
		_new->state = &s_hsmoke1;
	else
#endif
		_new->state = &s_smoke1;
	_new->ticcount = 6;

	_new->tilex = ob->tilex;
	_new->tiley = ob->tiley;
	_new->x = ob->x;
	_new->y = ob->y;
	_new->obclass = inertobj;
	_new->active = (activetype)_true;

	_new->flags = FL_NEVERMARK;
}


/*
===================
=
= ProjectileTryMove
=
= returns _true if move ok
===================
*/

#define PROJSIZE	0x2000

_boolean ProjectileTryMove (objtype *ob)
{
	int16_t			xl,yl,xh,yh,x,y;
	objtype		*check;
	int32_t		deltax,deltay;

	xl = (ob->x-PROJSIZE) >>TILESHIFT;
	yl = (ob->y-PROJSIZE) >>TILESHIFT;

	xh = (ob->x+PROJSIZE) >>TILESHIFT;
	yh = (ob->y+PROJSIZE) >>TILESHIFT;

//
// check for solid walls
//
	for (y=yl;y<=yh;y++)
		for (x=xl;x<=xh;x++)
		{
			check = actorat[x][y];
			if (check && check<objlist)
				return _false;
		}

	return _true;
}



/*
=================
=
= T_Projectile
=
=================
*/

void T_Projectile (objtype *ob)
{
	int32_t	deltax,deltay;
	int16_t		damage;
	int32_t	speed;

	speed = (int32_t)ob->speed*tics;

	deltax = FixedByFrac(speed,costable[ob->angle]);
	deltay = -FixedByFrac(speed,sintable[ob->angle]);

	if (deltax>0x10000l)
		deltax = 0x10000l;
	if (deltay>0x10000l)
		deltay = 0x10000l;

	ob->x += deltax;
	ob->y += deltay;

	deltax = LABS(ob->x - player->x);
	deltay = LABS(ob->y - player->y);

	if (!ProjectileTryMove (ob))
	{
		if (ob->obclass == rocketobj)
		{
			PlaySoundLocActor(MISSILEHITSND,ob);
			ob->state = &s_boom1;
		}
#ifdef SPEAR
		else if (ob->obclass == hrocketobj)
		{
			PlaySoundLocActor(MISSILEHITSND,ob);
			ob->state = &s_hboom1;
		}
#endif
		else
			ob->state = NULL;		// mark for removal

		return;
	}

	if (deltax < PROJECTILESIZE && deltay < PROJECTILESIZE)
	{	// hit the player
		switch (ob->obclass)
		{
            default: break;
		case needleobj:
			damage = (US_RndT() >>3) + 20;
			break;
		case rocketobj:
		case hrocketobj:
		case sparkobj:
			damage = (US_RndT() >>3) + 30;
			break;
		case fireobj:
			damage = (US_RndT() >>3);
			break;
		}

		TakeDamage (damage,ob);
		ob->state = NULL;		// mark for removal
		return;
	}

	ob->tilex = ob->x >> TILESHIFT;
	ob->tiley = ob->y >> TILESHIFT;

}




/*
=============================================================================

							GUARD

=============================================================================
*/

//
// guards
//

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

statetype s_grdstand	= {_true,SPR_GRD_S_1,0,(void(*)())T_Stand,NULL,&s_grdstand};

statetype s_grdpath1 	= {_true,SPR_GRD_W1_1,20,(void(*)())T_Path,NULL,&s_grdpath1s};
statetype s_grdpath1s 	= {_true,SPR_GRD_W1_1,5,NULL,NULL,&s_grdpath2};
statetype s_grdpath2 	= {_true,SPR_GRD_W2_1,15,(void(*)())T_Path,NULL,&s_grdpath3};
statetype s_grdpath3 	= {_true,SPR_GRD_W3_1,20,(void(*)())T_Path,NULL,&s_grdpath3s};
statetype s_grdpath3s 	= {_true,SPR_GRD_W3_1,5,NULL,NULL,&s_grdpath4};
statetype s_grdpath4 	= {_true,SPR_GRD_W4_1,15,(void(*)())T_Path,NULL,&s_grdpath1};

statetype s_grdpain 	= {(_boolean)2,SPR_GRD_PAIN_1,10,NULL,NULL,&s_grdchase1};
statetype s_grdpain1 	= {(_boolean)2,SPR_GRD_PAIN_2,10,NULL,NULL,&s_grdchase1};

statetype s_grdshoot1 	= {_false,SPR_GRD_SHOOT1,20,NULL,NULL,&s_grdshoot2};
statetype s_grdshoot2 	= {_false,SPR_GRD_SHOOT2,20,NULL,(void(*)())T_Shoot,&s_grdshoot3};
statetype s_grdshoot3 	= {_false,SPR_GRD_SHOOT3,20,NULL,NULL,&s_grdchase1};

statetype s_grdchase1 	= {_true,SPR_GRD_W1_1,10,(void(*)())T_Chase,NULL,&s_grdchase1s};
statetype s_grdchase1s 	= {_true,SPR_GRD_W1_1,3,NULL,NULL,&s_grdchase2};
statetype s_grdchase2 	= {_true,SPR_GRD_W2_1,8,(void(*)())T_Chase,NULL,&s_grdchase3};
statetype s_grdchase3 	= {_true,SPR_GRD_W3_1,10,(void(*)())T_Chase,NULL,&s_grdchase3s};
statetype s_grdchase3s 	= {_true,SPR_GRD_W3_1,3,NULL,NULL,&s_grdchase4};
statetype s_grdchase4 	= {_true,SPR_GRD_W4_1,8,(void(*)())T_Chase,NULL,&s_grdchase1};

statetype s_grddie1		= {_false,SPR_GRD_DIE_1,15,NULL,(void(*)())A_DeathScream,&s_grddie2};
statetype s_grddie2		= {_false,SPR_GRD_DIE_2,15,NULL,NULL,&s_grddie3};
statetype s_grddie3		= {_false,SPR_GRD_DIE_3,15,NULL,NULL,&s_grddie4};
statetype s_grddie4		= {_false,SPR_GRD_DEAD,0,NULL,NULL,&s_grddie4};


#ifndef SPEAR
//
// ghosts
//
extern	statetype s_blinkychase1;
extern	statetype s_blinkychase2;
extern	statetype s_inkychase1;
extern	statetype s_inkychase2;
extern	statetype s_pinkychase1;
extern	statetype s_pinkychase2;
extern	statetype s_clydechase1;
extern	statetype s_clydechase2;

statetype s_blinkychase1 	= {_false,SPR_BLINKY_W1,10,(void(*)())T_Ghosts,NULL,&s_blinkychase2};
statetype s_blinkychase2 	= {_false,SPR_BLINKY_W2,10,(void(*)())T_Ghosts,NULL,&s_blinkychase1};

statetype s_inkychase1 		= {_false,SPR_INKY_W1,10,(void(*)())T_Ghosts,NULL,&s_inkychase2};
statetype s_inkychase2 		= {_false,SPR_INKY_W2,10,(void(*)())T_Ghosts,NULL,&s_inkychase1};

statetype s_pinkychase1 	= {_false,SPR_PINKY_W1,10,(void(*)())T_Ghosts,NULL,&s_pinkychase2};
statetype s_pinkychase2 	= {_false,SPR_PINKY_W2,10,(void(*)())T_Ghosts,NULL,&s_pinkychase1};

statetype s_clydechase1 	= {_false,SPR_CLYDE_W1,10,(void(*)())T_Ghosts,NULL,&s_clydechase2};
statetype s_clydechase2 	= {_false,SPR_CLYDE_W2,10,(void(*)())T_Ghosts,NULL,&s_clydechase1};
#endif

//
// dogs
//

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

statetype s_dogpath1 	= {_true,SPR_DOG_W1_1,20,(void(*)())T_Path,NULL,&s_dogpath1s};
statetype s_dogpath1s 	= {_true,SPR_DOG_W1_1,5,NULL,NULL,&s_dogpath2};
statetype s_dogpath2 	= {_true,SPR_DOG_W2_1,15,(void(*)())T_Path,NULL,&s_dogpath3};
statetype s_dogpath3 	= {_true,SPR_DOG_W3_1,20,(void(*)())T_Path,NULL,&s_dogpath3s};
statetype s_dogpath3s 	= {_true,SPR_DOG_W3_1,5,NULL,NULL,&s_dogpath4};
statetype s_dogpath4 	= {_true,SPR_DOG_W4_1,15,(void(*)())T_Path,NULL,&s_dogpath1};

statetype s_dogjump1 	= {_false,SPR_DOG_JUMP1,10,NULL,NULL,&s_dogjump2};
statetype s_dogjump2 	= {_false,SPR_DOG_JUMP2,10,NULL,(void(*)())T_Bite,&s_dogjump3};
statetype s_dogjump3 	= {_false,SPR_DOG_JUMP3,10,NULL,NULL,&s_dogjump4};
statetype s_dogjump4 	= {_false,SPR_DOG_JUMP1,10,NULL,NULL,&s_dogjump5};
statetype s_dogjump5 	= {_false,SPR_DOG_W1_1,10,NULL,NULL,&s_dogchase1};

statetype s_dogchase1 	= {_true,SPR_DOG_W1_1,10,(void(*)())T_DogChase,NULL,&s_dogchase1s};
statetype s_dogchase1s 	= {_true,SPR_DOG_W1_1,3,NULL,NULL,&s_dogchase2};
statetype s_dogchase2 	= {_true,SPR_DOG_W2_1,8,(void(*)())T_DogChase,NULL,&s_dogchase3};
statetype s_dogchase3 	= {_true,SPR_DOG_W3_1,10,(void(*)())T_DogChase,NULL,&s_dogchase3s};
statetype s_dogchase3s 	= {_true,SPR_DOG_W3_1,3,NULL,NULL,&s_dogchase4};
statetype s_dogchase4 	= {_true,SPR_DOG_W4_1,8,(void(*)())T_DogChase,NULL,&s_dogchase1};

statetype s_dogdie1		= {_false,SPR_DOG_DIE_1,15,NULL,(void(*)())A_DeathScream,&s_dogdie2};
statetype s_dogdie2		= {_false,SPR_DOG_DIE_2,15,NULL,NULL,&s_dogdie3};
statetype s_dogdie3		= {_false,SPR_DOG_DIE_3,15,NULL,NULL,&s_dogdead};
statetype s_dogdead		= {_false,SPR_DOG_DEAD,15,NULL,NULL,&s_dogdead};


//
// officers
//

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

statetype s_ofcstand	= {_true,SPR_OFC_S_1,0,(void(*)())T_Stand,NULL,&s_ofcstand};

statetype s_ofcpath1 	= {_true,SPR_OFC_W1_1,20,(void(*)())T_Path,NULL,&s_ofcpath1s};
statetype s_ofcpath1s 	= {_true,SPR_OFC_W1_1,5,NULL,NULL,&s_ofcpath2};
statetype s_ofcpath2 	= {_true,SPR_OFC_W2_1,15,(void(*)())T_Path,NULL,&s_ofcpath3};
statetype s_ofcpath3 	= {_true,SPR_OFC_W3_1,20,(void(*)())T_Path,NULL,&s_ofcpath3s};
statetype s_ofcpath3s 	= {_true,SPR_OFC_W3_1,5,NULL,NULL,&s_ofcpath4};
statetype s_ofcpath4 	= {_true,SPR_OFC_W4_1,15,(void(*)())T_Path,NULL,&s_ofcpath1};

statetype s_ofcpain 	= {(_boolean)2,SPR_OFC_PAIN_1,10,NULL,NULL,&s_ofcchase1};
statetype s_ofcpain1 	= {(_boolean)2,SPR_OFC_PAIN_2,10,NULL,NULL,&s_ofcchase1};

statetype s_ofcshoot1 	= {_false,SPR_OFC_SHOOT1,6,NULL,NULL,&s_ofcshoot2};
statetype s_ofcshoot2 	= {_false,SPR_OFC_SHOOT2,20,NULL,(void(*)())T_Shoot,&s_ofcshoot3};
statetype s_ofcshoot3 	= {_false,SPR_OFC_SHOOT3,10,NULL,NULL,&s_ofcchase1};

statetype s_ofcchase1 	= {_true,SPR_OFC_W1_1,10,(void(*)())T_Chase,NULL,&s_ofcchase1s};
statetype s_ofcchase1s 	= {_true,SPR_OFC_W1_1,3,NULL,NULL,&s_ofcchase2};
statetype s_ofcchase2 	= {_true,SPR_OFC_W2_1,8,(void(*)())T_Chase,NULL,&s_ofcchase3};
statetype s_ofcchase3 	= {_true,SPR_OFC_W3_1,10,(void(*)())T_Chase,NULL,&s_ofcchase3s};
statetype s_ofcchase3s 	= {_true,SPR_OFC_W3_1,3,NULL,NULL,&s_ofcchase4};
statetype s_ofcchase4 	= {_true,SPR_OFC_W4_1,8,(void(*)())T_Chase,NULL,&s_ofcchase1};

statetype s_ofcdie1		= {_false,SPR_OFC_DIE_1,11,NULL,(void(*)())A_DeathScream,&s_ofcdie2};
statetype s_ofcdie2		= {_false,SPR_OFC_DIE_2,11,NULL,NULL,&s_ofcdie3};
statetype s_ofcdie3		= {_false,SPR_OFC_DIE_3,11,NULL,NULL,&s_ofcdie4};
statetype s_ofcdie4		= {_false,SPR_OFC_DIE_4,11,NULL,NULL,&s_ofcdie5};
statetype s_ofcdie5		= {_false,SPR_OFC_DEAD,0,NULL,NULL,&s_ofcdie5};


//
// mutant
//

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

statetype s_mutstand	= {_true,SPR_MUT_S_1,0,(void(*)())T_Stand,NULL,&s_mutstand};

statetype s_mutpath1 	= {_true,SPR_MUT_W1_1,20,(void(*)())T_Path,NULL,&s_mutpath1s};
statetype s_mutpath1s 	= {_true,SPR_MUT_W1_1,5,NULL,NULL,&s_mutpath2};
statetype s_mutpath2 	= {_true,SPR_MUT_W2_1,15,(void(*)())T_Path,NULL,&s_mutpath3};
statetype s_mutpath3 	= {_true,SPR_MUT_W3_1,20,(void(*)())T_Path,NULL,&s_mutpath3s};
statetype s_mutpath3s 	= {_true,SPR_MUT_W3_1,5,NULL,NULL,&s_mutpath4};
statetype s_mutpath4 	= {_true,SPR_MUT_W4_1,15,(void(*)())T_Path,NULL,&s_mutpath1};

statetype s_mutpain 	= {(_boolean)2,SPR_MUT_PAIN_1,10,NULL,NULL,&s_mutchase1};
statetype s_mutpain1 	= {(_boolean)2,SPR_MUT_PAIN_2,10,NULL,NULL,&s_mutchase1};

statetype s_mutshoot1 	= {_false,SPR_MUT_SHOOT1,6,NULL,(void(*)())T_Shoot,&s_mutshoot2};
statetype s_mutshoot2 	= {_false,SPR_MUT_SHOOT2,20,NULL,NULL,&s_mutshoot3};
statetype s_mutshoot3 	= {_false,SPR_MUT_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_mutshoot4};
statetype s_mutshoot4 	= {_false,SPR_MUT_SHOOT4,20,NULL,NULL,&s_mutchase1};

statetype s_mutchase1 	= {_true,SPR_MUT_W1_1,10,(void(*)())T_Chase,NULL,&s_mutchase1s};
statetype s_mutchase1s 	= {_true,SPR_MUT_W1_1,3,NULL,NULL,&s_mutchase2};
statetype s_mutchase2 	= {_true,SPR_MUT_W2_1,8,(void(*)())T_Chase,NULL,&s_mutchase3};
statetype s_mutchase3 	= {_true,SPR_MUT_W3_1,10,(void(*)())T_Chase,NULL,&s_mutchase3s};
statetype s_mutchase3s 	= {_true,SPR_MUT_W3_1,3,NULL,NULL,&s_mutchase4};
statetype s_mutchase4 	= {_true,SPR_MUT_W4_1,8,(void(*)())T_Chase,NULL,&s_mutchase1};

statetype s_mutdie1		= {_false,SPR_MUT_DIE_1,7,NULL,(void(*)())A_DeathScream,&s_mutdie2};
statetype s_mutdie2		= {_false,SPR_MUT_DIE_2,7,NULL,NULL,&s_mutdie3};
statetype s_mutdie3		= {_false,SPR_MUT_DIE_3,7,NULL,NULL,&s_mutdie4};
statetype s_mutdie4		= {_false,SPR_MUT_DIE_4,7,NULL,NULL,&s_mutdie5};
statetype s_mutdie5		= {_false,SPR_MUT_DEAD,0,NULL,NULL,&s_mutdie5};


//
// SS
//

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

statetype s_ssstand	= {_true,SPR_SS_S_1,0,(void(*)())T_Stand,NULL,&s_ssstand};

statetype s_sspath1 	= {_true,SPR_SS_W1_1,20,(void(*)())T_Path,NULL,&s_sspath1s};
statetype s_sspath1s 	= {_true,SPR_SS_W1_1,5,NULL,NULL,&s_sspath2};
statetype s_sspath2 	= {_true,SPR_SS_W2_1,15,(void(*)())T_Path,NULL,&s_sspath3};
statetype s_sspath3 	= {_true,SPR_SS_W3_1,20,(void(*)())T_Path,NULL,&s_sspath3s};
statetype s_sspath3s 	= {_true,SPR_SS_W3_1,5,NULL,NULL,&s_sspath4};
statetype s_sspath4 	= {_true,SPR_SS_W4_1,15,(void(*)())T_Path,NULL,&s_sspath1};

statetype s_sspain 		= {(_boolean)2,SPR_SS_PAIN_1,10,NULL,NULL,&s_sschase1};
statetype s_sspain1 	= {(_boolean)2,SPR_SS_PAIN_2,10,NULL,NULL,&s_sschase1};

statetype s_ssshoot1 	= {_false,SPR_SS_SHOOT1,20,NULL,NULL,&s_ssshoot2};
statetype s_ssshoot2 	= {_false,SPR_SS_SHOOT2,20,NULL,(void(*)())T_Shoot,&s_ssshoot3};
statetype s_ssshoot3 	= {_false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot4};
statetype s_ssshoot4 	= {_false,SPR_SS_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_ssshoot5};
statetype s_ssshoot5 	= {_false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot6};
statetype s_ssshoot6 	= {_false,SPR_SS_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_ssshoot7};
statetype s_ssshoot7  	= {_false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot8};
statetype s_ssshoot8  	= {_false,SPR_SS_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_ssshoot9};
statetype s_ssshoot9  	= {_false,SPR_SS_SHOOT3,10,NULL,NULL,&s_sschase1};

statetype s_sschase1 	= {_true,SPR_SS_W1_1,10,(void(*)())T_Chase,NULL,&s_sschase1s};
statetype s_sschase1s 	= {_true,SPR_SS_W1_1,3,NULL,NULL,&s_sschase2};
statetype s_sschase2 	= {_true,SPR_SS_W2_1,8,(void(*)())T_Chase,NULL,&s_sschase3};
statetype s_sschase3 	= {_true,SPR_SS_W3_1,10,(void(*)())T_Chase,NULL,&s_sschase3s};
statetype s_sschase3s 	= {_true,SPR_SS_W3_1,3,NULL,NULL,&s_sschase4};
statetype s_sschase4 	= {_true,SPR_SS_W4_1,8,(void(*)())T_Chase,NULL,&s_sschase1};

statetype s_ssdie1		= {_false,SPR_SS_DIE_1,15,NULL,(void(*)())A_DeathScream,&s_ssdie2};
statetype s_ssdie2		= {_false,SPR_SS_DIE_2,15,NULL,NULL,&s_ssdie3};
statetype s_ssdie3		= {_false,SPR_SS_DIE_3,15,NULL,NULL,&s_ssdie4};
statetype s_ssdie4		= {_false,SPR_SS_DEAD,0,NULL,NULL,&s_ssdie4};


#ifndef SPEAR
//
// hans
//
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


statetype s_bossstand	= {_false,SPR_BOSS_W1,0,(void(*)())T_Stand,NULL,&s_bossstand};

statetype s_bosschase1 	= {_false,SPR_BOSS_W1,10,(void(*)())T_Chase,NULL,&s_bosschase1s};
statetype s_bosschase1s	= {_false,SPR_BOSS_W1,3,NULL,NULL,&s_bosschase2};
statetype s_bosschase2 	= {_false,SPR_BOSS_W2,8,(void(*)())T_Chase,NULL,&s_bosschase3};
statetype s_bosschase3 	= {_false,SPR_BOSS_W3,10,(void(*)())T_Chase,NULL,&s_bosschase3s};
statetype s_bosschase3s	= {_false,SPR_BOSS_W3,3,NULL,NULL,&s_bosschase4};
statetype s_bosschase4 	= {_false,SPR_BOSS_W4,8,(void(*)())T_Chase,NULL,&s_bosschase1};

statetype s_bossdie1	= {_false,SPR_BOSS_DIE1,15,NULL,(void(*)())A_DeathScream,&s_bossdie2};
statetype s_bossdie2	= {_false,SPR_BOSS_DIE2,15,NULL,NULL,&s_bossdie3};
statetype s_bossdie3	= {_false,SPR_BOSS_DIE3,15,NULL,NULL,&s_bossdie4};
statetype s_bossdie4	= {_false,SPR_BOSS_DEAD,0,NULL,NULL,&s_bossdie4};

statetype s_bossshoot1 	= {_false,SPR_BOSS_SHOOT1,30,NULL,NULL,&s_bossshoot2};
statetype s_bossshoot2 	= {_false,SPR_BOSS_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_bossshoot3};
statetype s_bossshoot3 	= {_false,SPR_BOSS_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_bossshoot4};
statetype s_bossshoot4 	= {_false,SPR_BOSS_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_bossshoot5};
statetype s_bossshoot5 	= {_false,SPR_BOSS_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_bossshoot6};
statetype s_bossshoot6 	= {_false,SPR_BOSS_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_bossshoot7};
statetype s_bossshoot7 	= {_false,SPR_BOSS_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_bossshoot8};
statetype s_bossshoot8 	= {_false,SPR_BOSS_SHOOT1,10,NULL,NULL,&s_bosschase1};


//
// gretel
//
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


statetype s_gretelstand	= {_false,SPR_GRETEL_W1,0,(void(*)())T_Stand,NULL,&s_gretelstand};

statetype s_gretelchase1 	= {_false,SPR_GRETEL_W1,10,(void(*)())T_Chase,NULL,&s_gretelchase1s};
statetype s_gretelchase1s	= {_false,SPR_GRETEL_W1,3,NULL,NULL,&s_gretelchase2};
statetype s_gretelchase2 	= {_false,SPR_GRETEL_W2,8,(void(*)())T_Chase,NULL,&s_gretelchase3};
statetype s_gretelchase3 	= {_false,SPR_GRETEL_W3,10,(void(*)())T_Chase,NULL,&s_gretelchase3s};
statetype s_gretelchase3s	= {_false,SPR_GRETEL_W3,3,NULL,NULL,&s_gretelchase4};
statetype s_gretelchase4 	= {_false,SPR_GRETEL_W4,8,(void(*)())T_Chase,NULL,&s_gretelchase1};

statetype s_greteldie1	= {_false,SPR_GRETEL_DIE1,15,NULL,(void(*)())A_DeathScream,&s_greteldie2};
statetype s_greteldie2	= {_false,SPR_GRETEL_DIE2,15,NULL,NULL,&s_greteldie3};
statetype s_greteldie3	= {_false,SPR_GRETEL_DIE3,15,NULL,NULL,&s_greteldie4};
statetype s_greteldie4	= {_false,SPR_GRETEL_DEAD,0,NULL,NULL,&s_greteldie4};

statetype s_gretelshoot1 	= {_false,SPR_GRETEL_SHOOT1,30,NULL,NULL,&s_gretelshoot2};
statetype s_gretelshoot2 	= {_false,SPR_GRETEL_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_gretelshoot3};
statetype s_gretelshoot3 	= {_false,SPR_GRETEL_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_gretelshoot4};
statetype s_gretelshoot4 	= {_false,SPR_GRETEL_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_gretelshoot5};
statetype s_gretelshoot5 	= {_false,SPR_GRETEL_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_gretelshoot6};
statetype s_gretelshoot6 	= {_false,SPR_GRETEL_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_gretelshoot7};
statetype s_gretelshoot7 	= {_false,SPR_GRETEL_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_gretelshoot8};
statetype s_gretelshoot8 	= {_false,SPR_GRETEL_SHOOT1,10,NULL,NULL,&s_gretelchase1};
#endif


/*
===============
=
= SpawnStand
=
===============
*/

void SpawnStand (enemy_t which, int16_t tilex, int16_t tiley, int16_t dir)
{
	uint16_t	 *map,tile;

	switch (which)
	{
        default: break;
	case en_guard:
		SpawnNewObj (tilex,tiley,&s_grdstand);
		_new->speed = SPDPATROL;
		if (!loadedgame)
		  gamestate.killtotal++;
		break;

	case en_officer:
		SpawnNewObj (tilex,tiley,&s_ofcstand);
		_new->speed = SPDPATROL;
		if (!loadedgame)
		  gamestate.killtotal++;
		break;

	case en_mutant:
		SpawnNewObj (tilex,tiley,&s_mutstand);
		_new->speed = SPDPATROL;
		if (!loadedgame)
		  gamestate.killtotal++;
		break;

	case en_ss:
		SpawnNewObj (tilex,tiley,&s_ssstand);
		_new->speed = SPDPATROL;
		if (!loadedgame)
		  gamestate.killtotal++;
		break;
	}


	map = mapsegs[0]+farmapylookup[tiley]+tilex;
	if (*map == AMBUSHTILE)
	{
		tilemap[tilex][tiley] = 0;

		if (*(map+1) >= AREATILE)
			tile = *(map+1);
		if (*(map-mapwidth) >= AREATILE)
			tile = *(map-mapwidth);
		if (*(map+mapwidth) >= AREATILE)
			tile = *(map+mapwidth);
		if ( *(map-1) >= AREATILE)
			tile = *(map-1);

		*map = tile;
		_new->areanumber = tile-AREATILE;

		_new->flags |= FL_AMBUSH;
	}

	_new->obclass = (classtype)(guardobj+which);
	_new->hitpoints = starthitpoints[gamestate.difficulty][which];
	_new->dir = (dirtype)(dir*2);
	_new->flags |= FL_SHOOTABLE;
}



/*
===============
=
= SpawnDeadGuard
=
===============
*/

void SpawnDeadGuard (int16_t tilex, int16_t tiley)
{
	SpawnNewObj (tilex,tiley,&s_grddie4);
	_new->obclass = inertobj;
}



#ifndef SPEAR
/*
===============
=
= SpawnBoss
=
===============
*/

void SpawnBoss (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	SpawnNewObj (tilex,tiley,&s_bossstand);
	_new->speed = SPDPATROL;

	_new->obclass = bossobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_boss];
	_new->dir = south;
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}

/*
===============
=
= SpawnGretel
=
===============
*/

void SpawnGretel (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	SpawnNewObj (tilex,tiley,&s_gretelstand);
	_new->speed = SPDPATROL;

	_new->obclass = gretelobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_gretel];
	_new->dir = north;
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}
#endif

/*
===============
=
= SpawnPatrol
=
===============
*/

void SpawnPatrol (enemy_t which, int16_t tilex, int16_t tiley, int16_t dir)
{
	switch (which)
	{
        default: break;
	case en_guard:
		SpawnNewObj (tilex,tiley,&s_grdpath1);
		_new->speed = SPDPATROL;
		if (!loadedgame)
		  gamestate.killtotal++;
		break;

	case en_officer:
		SpawnNewObj (tilex,tiley,&s_ofcpath1);
		_new->speed = SPDPATROL;
		if (!loadedgame)
		  gamestate.killtotal++;
		break;

	case en_ss:
		SpawnNewObj (tilex,tiley,&s_sspath1);
		_new->speed = SPDPATROL;
		if (!loadedgame)
		  gamestate.killtotal++;
		break;

	case en_mutant:
		SpawnNewObj (tilex,tiley,&s_mutpath1);
		_new->speed = SPDPATROL;
		if (!loadedgame)
		  gamestate.killtotal++;
		break;

	case en_dog:
		SpawnNewObj (tilex,tiley,&s_dogpath1);
		_new->speed = SPDDOG;
		if (!loadedgame)
		  gamestate.killtotal++;
		break;
	}

	_new->obclass = (classtype)(guardobj+which);
	_new->dir = (dirtype)(dir*2);
	_new->hitpoints = starthitpoints[gamestate.difficulty][which];
	_new->distance = tileglobal;
	_new->flags |= FL_SHOOTABLE;
	_new->active = (activetype)_true;

	actorat[_new->tilex][_new->tiley] = NULL;		// don't use original spot

	switch (dir)
	{
	case 0:
		_new->tilex++;
		break;
	case 1:
		_new->tiley--;
		break;
	case 2:
		_new->tilex--;
		break;
	case 3:
		_new->tiley++;
		break;
	}

	actorat[_new->tilex][_new->tiley] = _new;
}



/*
==================
=
= A_DeathScream
=
==================
*/

void A_DeathScream (objtype *ob)
{
#ifndef UPLOAD
#ifndef SPEAR
	if (mapon==9 && !US_RndT())
#else
	if ((mapon==18 || mapon==19) && !US_RndT())
#endif
	{
	 switch(ob->obclass)
	 {
         default: break;
	  case mutantobj:
	  case guardobj:
	  case officerobj:
	  case ssobj:
	  case dogobj:
		PlaySoundLocActor(DEATHSCREAM6SND,ob);
		return;
	 }
	}
#endif

	switch (ob->obclass)
	{
        default:break;
	case mutantobj:
		PlaySoundLocActor(AHHHGSND,ob);
		break;

	case guardobj:
		{
		 int16_t sounds[9]={ DEATHSCREAM1SND,
				 DEATHSCREAM2SND,
				 DEATHSCREAM3SND,
				 DEATHSCREAM4SND,
				 DEATHSCREAM5SND,
				 DEATHSCREAM7SND,
				 DEATHSCREAM8SND,
				 DEATHSCREAM9SND
				 };

		 #ifndef UPLOAD
		 PlaySoundLocActor(sounds[US_RndT()%8],ob);
		 #else
		 PlaySoundLocActor(sounds[US_RndT()%2],ob);
		 #endif
		}
		break;
	case officerobj:
		PlaySoundLocActor(NEINSOVASSND,ob);
		break;
	case ssobj:
		PlaySoundLocActor(LEBENSND,ob);	// JAB
		break;
	case dogobj:
		PlaySoundLocActor(DOGDEATHSND,ob);	// JAB
		break;
#ifndef SPEAR
	case bossobj:
		SD_PlaySound(MUTTISND);				// JAB
		break;
	case schabbobj:
		SD_PlaySound(MEINGOTTSND);
		break;
	case fakeobj:
		SD_PlaySound(HITLERHASND);
		break;
	case mechahitlerobj:
		SD_PlaySound(SCHEISTSND);
		break;
	case realhitlerobj:
		SD_PlaySound(EVASND);
		break;
	case gretelobj:
		SD_PlaySound(MEINSND);
		break;
	case giftobj:
		SD_PlaySound(DONNERSND);
		break;
	case fatobj:
		SD_PlaySound(ROSESND);
		break;
#else
	case spectreobj:
		SD_PlaySound(GHOSTFADESND);
		break;
	case angelobj:
		SD_PlaySound(ANGELDEATHSND);
		break;
	case transobj:
		SD_PlaySound(TRANSDEATHSND);
		break;
	case uberobj:
		SD_PlaySound(UBERDEATHSND);
		break;
	case willobj:
		SD_PlaySound(WILHELMDEATHSND);
		break;
	case deathobj:
		SD_PlaySound(KNIGHTDEATHSND);
		break;
#endif
	}
}


/*
=============================================================================

						 SPEAR ACTORS

=============================================================================
*/

#ifdef SPEAR

void T_Launch (objtype *ob);
void T_Will (objtype *ob);

extern	statetype s_angelshoot1;
extern	statetype s_deathshoot1;
extern	statetype s_spark1;

//
// trans
//
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


statetype s_transstand	= {_false,SPR_TRANS_W1,0,T_Stand,NULL,&s_transstand};

statetype s_transchase1 	= {_false,SPR_TRANS_W1,10,T_Chase,NULL,&s_transchase1s};
statetype s_transchase1s	= {_false,SPR_TRANS_W1,3,NULL,NULL,&s_transchase2};
statetype s_transchase2 	= {_false,SPR_TRANS_W2,8,T_Chase,NULL,&s_transchase3};
statetype s_transchase3 	= {_false,SPR_TRANS_W3,10,T_Chase,NULL,&s_transchase3s};
statetype s_transchase3s	= {_false,SPR_TRANS_W3,3,NULL,NULL,&s_transchase4};
statetype s_transchase4 	= {_false,SPR_TRANS_W4,8,T_Chase,NULL,&s_transchase1};

statetype s_transdie0	= {_false,SPR_TRANS_W1,1,NULL,A_DeathScream,&s_transdie01};
statetype s_transdie01	= {_false,SPR_TRANS_W1,1,NULL,NULL,&s_transdie1};
statetype s_transdie1	= {_false,SPR_TRANS_DIE1,15,NULL,NULL,&s_transdie2};
statetype s_transdie2	= {_false,SPR_TRANS_DIE2,15,NULL,NULL,&s_transdie3};
statetype s_transdie3	= {_false,SPR_TRANS_DIE3,15,NULL,NULL,&s_transdie4};
statetype s_transdie4	= {_false,SPR_TRANS_DEAD,0,NULL,NULL,&s_transdie4};

statetype s_transshoot1 	= {_false,SPR_TRANS_SHOOT1,30,NULL,NULL,&s_transshoot2};
statetype s_transshoot2 	= {_false,SPR_TRANS_SHOOT2,10,NULL,T_Shoot,&s_transshoot3};
statetype s_transshoot3 	= {_false,SPR_TRANS_SHOOT3,10,NULL,T_Shoot,&s_transshoot4};
statetype s_transshoot4 	= {_false,SPR_TRANS_SHOOT2,10,NULL,T_Shoot,&s_transshoot5};
statetype s_transshoot5 	= {_false,SPR_TRANS_SHOOT3,10,NULL,T_Shoot,&s_transshoot6};
statetype s_transshoot6 	= {_false,SPR_TRANS_SHOOT2,10,NULL,T_Shoot,&s_transshoot7};
statetype s_transshoot7 	= {_false,SPR_TRANS_SHOOT3,10,NULL,T_Shoot,&s_transshoot8};
statetype s_transshoot8 	= {_false,SPR_TRANS_SHOOT1,10,NULL,NULL,&s_transchase1};


/*
===============
=
= SpawnTrans
=
===============
*/

void SpawnTrans (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	if (SoundBlasterPresent && DigiMode != sds_Off)
		s_transdie01.tictime = 105;

	SpawnNewObj (tilex,tiley,&s_transstand);
	_new->obclass = transobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_trans];
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}


//
// uber
//
void T_UShoot (objtype *ob);

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


statetype s_uberstand	= {_false,SPR_UBER_W1,0,T_Stand,NULL,&s_uberstand};

statetype s_uberchase1 	= {_false,SPR_UBER_W1,10,T_Chase,NULL,&s_uberchase1s};
statetype s_uberchase1s	= {_false,SPR_UBER_W1,3,NULL,NULL,&s_uberchase2};
statetype s_uberchase2 	= {_false,SPR_UBER_W2,8,T_Chase,NULL,&s_uberchase3};
statetype s_uberchase3 	= {_false,SPR_UBER_W3,10,T_Chase,NULL,&s_uberchase3s};
statetype s_uberchase3s	= {_false,SPR_UBER_W3,3,NULL,NULL,&s_uberchase4};
statetype s_uberchase4 	= {_false,SPR_UBER_W4,8,T_Chase,NULL,&s_uberchase1};

statetype s_uberdie0	= {_false,SPR_UBER_W1,1,NULL,A_DeathScream,&s_uberdie01};
statetype s_uberdie01	= {_false,SPR_UBER_W1,1,NULL,NULL,&s_uberdie1};
statetype s_uberdie1	= {_false,SPR_UBER_DIE1,15,NULL,NULL,&s_uberdie2};
statetype s_uberdie2	= {_false,SPR_UBER_DIE2,15,NULL,NULL,&s_uberdie3};
statetype s_uberdie3	= {_false,SPR_UBER_DIE3,15,NULL,NULL,&s_uberdie4};
statetype s_uberdie4	= {_false,SPR_UBER_DIE4,15,NULL,NULL,&s_uberdie5};
statetype s_uberdie5	= {_false,SPR_UBER_DEAD,0,NULL,NULL,&s_uberdie5};

statetype s_ubershoot1 	= {_false,SPR_UBER_SHOOT1,30,NULL,NULL,&s_ubershoot2};
statetype s_ubershoot2 	= {_false,SPR_UBER_SHOOT2,12,NULL,T_UShoot,&s_ubershoot3};
statetype s_ubershoot3 	= {_false,SPR_UBER_SHOOT3,12,NULL,T_UShoot,&s_ubershoot4};
statetype s_ubershoot4 	= {_false,SPR_UBER_SHOOT4,12,NULL,T_UShoot,&s_ubershoot5};
statetype s_ubershoot5 	= {_false,SPR_UBER_SHOOT3,12,NULL,T_UShoot,&s_ubershoot6};
statetype s_ubershoot6 	= {_false,SPR_UBER_SHOOT2,12,NULL,T_UShoot,&s_ubershoot7};
statetype s_ubershoot7 	= {_false,SPR_UBER_SHOOT1,12,NULL,NULL,&s_uberchase1};


/*
===============
=
= SpawnUber
=
===============
*/

void SpawnUber (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	if (SoundBlasterPresent && DigiMode != sds_Off)
		s_uberdie01.tictime = 70;

	SpawnNewObj (tilex,tiley,&s_uberstand);
	_new->obclass = uberobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_uber];
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}


/*
===============
=
= T_UShoot
=
===============
*/

void T_UShoot (objtype *ob)
{
	int16_t	dx,dy,dist;

	T_Shoot (ob);

	dx = abs(ob->tilex - player->tilex);
	dy = abs(ob->tiley - player->tiley);
	dist = dx>dy ? dx : dy;
	if (dist <= 1)
		TakeDamage (10,ob);
}


//
// will
//
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


statetype s_willstand	= {_false,SPR_WILL_W1,0,T_Stand,NULL,&s_willstand};

statetype s_willchase1 	= {_false,SPR_WILL_W1,10,T_Will,NULL,&s_willchase1s};
statetype s_willchase1s	= {_false,SPR_WILL_W1,3,NULL,NULL,&s_willchase2};
statetype s_willchase2 	= {_false,SPR_WILL_W2,8,T_Will,NULL,&s_willchase3};
statetype s_willchase3 	= {_false,SPR_WILL_W3,10,T_Will,NULL,&s_willchase3s};
statetype s_willchase3s	= {_false,SPR_WILL_W3,3,NULL,NULL,&s_willchase4};
statetype s_willchase4 	= {_false,SPR_WILL_W4,8,T_Will,NULL,&s_willchase1};

statetype s_willdeathcam	= {_false,SPR_WILL_W1,1,NULL,NULL,&s_willdie1};

statetype s_willdie1	= {_false,SPR_WILL_W1,1,NULL,A_DeathScream,&s_willdie2};
statetype s_willdie2	= {_false,SPR_WILL_W1,10,NULL,NULL,&s_willdie3};
statetype s_willdie3	= {_false,SPR_WILL_DIE1,10,NULL,NULL,&s_willdie4};
statetype s_willdie4	= {_false,SPR_WILL_DIE2,10,NULL,NULL,&s_willdie5};
statetype s_willdie5	= {_false,SPR_WILL_DIE3,10,NULL,NULL,&s_willdie6};
statetype s_willdie6	= {_false,SPR_WILL_DEAD,20,NULL,NULL,&s_willdie6};

statetype s_willshoot1 	= {_false,SPR_WILL_SHOOT1,30,NULL,NULL,&s_willshoot2};
statetype s_willshoot2 	= {_false,SPR_WILL_SHOOT2,10,NULL,T_Launch,&s_willshoot3};
statetype s_willshoot3 	= {_false,SPR_WILL_SHOOT3,10,NULL,T_Shoot,&s_willshoot4};
statetype s_willshoot4 	= {_false,SPR_WILL_SHOOT4,10,NULL,T_Shoot,&s_willshoot5};
statetype s_willshoot5 	= {_false,SPR_WILL_SHOOT3,10,NULL,T_Shoot,&s_willshoot6};
statetype s_willshoot6 	= {_false,SPR_WILL_SHOOT4,10,NULL,T_Shoot,&s_willchase1};


/*
===============
=
= SpawnWill
=
===============
*/

void SpawnWill (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	if (SoundBlasterPresent && DigiMode != sds_Off)
		s_willdie2.tictime = 70;

	SpawnNewObj (tilex,tiley,&s_willstand);
	_new->obclass = willobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_will];
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}


/*
================
=
= T_Will
=
================
*/

void T_Will (objtype *ob)
{
	int32_t move;
	int16_t	dx,dy,dist;
	_boolean	dodge;

	dodge = _false;
	dx = abs(ob->tilex - player->tilex);
	dy = abs(ob->tiley - player->tiley);
	dist = dx>dy ? dx : dy;

	if (CheckLine(ob))						// got a shot at player?
	{
		if ( US_RndT() < (tics<<3) )
		{
		//
		// go into attack frame
		//
			if (ob->obclass == willobj)
				NewState (ob,&s_willshoot1);
			else if (ob->obclass == angelobj)
				NewState (ob,&s_angelshoot1);
			else
				NewState (ob,&s_deathshoot1);
			return;
		}
		dodge = _true;
	}

	if (ob->dir == nodir)
	{
		if (dodge)
			SelectDodgeDir (ob);
		else
			SelectChaseDir (ob);
		if (ob->dir == nodir)
			return;							// object is blocked in
	}

	move = ob->speed*tics;

	while (move)
	{
		if (ob->distance < 0)
		{
		//
		// waiting for a door to open
		//
			OpenDoor (-ob->distance-1);
			if (doorobjlist[-ob->distance-1].action != dr_open)
				return;
			ob->distance = TILEGLOBAL;	// go ahead, the door is now opoen
		}

		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}

		//
		// reached goal tile, so select another one
		//

		//
		// fix position to account for round off during moving
		//
		ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

		move -= ob->distance;

		if (dist <4)
			SelectRunDir (ob);
		else if (dodge)
			SelectDodgeDir (ob);
		else
			SelectChaseDir (ob);

		if (ob->dir == nodir)
			return;							// object is blocked in
	}

}


//
// death
//
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


statetype s_deathstand	= {_false,SPR_DEATH_W1,0,T_Stand,NULL,&s_deathstand};

statetype s_deathchase1 	= {_false,SPR_DEATH_W1,10,T_Will,NULL,&s_deathchase1s};
statetype s_deathchase1s	= {_false,SPR_DEATH_W1,3,NULL,NULL,&s_deathchase2};
statetype s_deathchase2 	= {_false,SPR_DEATH_W2,8,T_Will,NULL,&s_deathchase3};
statetype s_deathchase3 	= {_false,SPR_DEATH_W3,10,T_Will,NULL,&s_deathchase3s};
statetype s_deathchase3s	= {_false,SPR_DEATH_W3,3,NULL,NULL,&s_deathchase4};
statetype s_deathchase4 	= {_false,SPR_DEATH_W4,8,T_Will,NULL,&s_deathchase1};

statetype s_deathdeathcam	= {_false,SPR_DEATH_W1,1,NULL,NULL,&s_deathdie1};

statetype s_deathdie1	= {_false,SPR_DEATH_W1,1,NULL,A_DeathScream,&s_deathdie2};
statetype s_deathdie2	= {_false,SPR_DEATH_W1,10,NULL,NULL,&s_deathdie3};
statetype s_deathdie3	= {_false,SPR_DEATH_DIE1,10,NULL,NULL,&s_deathdie4};
statetype s_deathdie4	= {_false,SPR_DEATH_DIE2,10,NULL,NULL,&s_deathdie5};
statetype s_deathdie5	= {_false,SPR_DEATH_DIE3,10,NULL,NULL,&s_deathdie6};
statetype s_deathdie6	= {_false,SPR_DEATH_DIE4,10,NULL,NULL,&s_deathdie7};
statetype s_deathdie7	= {_false,SPR_DEATH_DIE5,10,NULL,NULL,&s_deathdie8};
statetype s_deathdie8	= {_false,SPR_DEATH_DIE6,10,NULL,NULL,&s_deathdie9};
statetype s_deathdie9	= {_false,SPR_DEATH_DEAD,0,NULL,NULL,&s_deathdie9};

statetype s_deathshoot1 	= {_false,SPR_DEATH_SHOOT1,30,NULL,NULL,&s_deathshoot2};
statetype s_deathshoot2 	= {_false,SPR_DEATH_SHOOT2,10,NULL,T_Launch,&s_deathshoot3};
statetype s_deathshoot3 	= {_false,SPR_DEATH_SHOOT4,10,NULL,T_Shoot,&s_deathshoot4};
statetype s_deathshoot4 	= {_false,SPR_DEATH_SHOOT3,10,NULL,T_Launch,&s_deathshoot5};
statetype s_deathshoot5 	= {_false,SPR_DEATH_SHOOT4,10,NULL,T_Shoot,&s_deathchase1};


/*
===============
=
= SpawnDeath
=
===============
*/

void SpawnDeath (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	if (SoundBlasterPresent && DigiMode != sds_Off)
		s_deathdie2.tictime = 105;

	SpawnNewObj (tilex,tiley,&s_deathstand);
	_new->obclass = deathobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_death];
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}

/*
===============
=
= T_Launch
=
===============
*/

void T_Launch (objtype *ob)
{
	int32_t	deltax,deltay;
	float	angle;
	int16_t		iangle;

	deltax = player->x - ob->x;
	deltay = ob->y - player->y;
	angle = atan2 (deltay,deltax);
	if (angle<0)
		angle = M_PI*2+angle;
	iangle = angle/(M_PI*2)*ANGLES;
	if (ob->obclass == deathobj)
	{
		T_Shoot (ob);
		if (ob->state == &s_deathshoot2)
		{
			iangle-=4;
			if (iangle<0)
				iangle+=ANGLES;
		}
		else
		{
			iangle+=4;
			if (iangle>=ANGLES)
				iangle-=ANGLES;
		}
	}

	GetNewActor ();
	_new->state = &s_rocket;
	_new->ticcount = 1;

	_new->tilex = ob->tilex;
	_new->tiley = ob->tiley;
	_new->x = ob->x;
	_new->y = ob->y;
	_new->obclass = rocketobj;
	switch(ob->obclass)
	{
	case deathobj:
		_new->state = &s_hrocket;
		_new->obclass = hrocketobj;
		PlaySoundLocActor (KNIGHTMISSILESND,_new);
		break;
	case angelobj:
		_new->state = &s_spark1;
		_new->obclass = sparkobj;
		PlaySoundLocActor (ANGELFIRESND,_new);
		break;
	default:
		PlaySoundLocActor (MISSILEFIRESND,_new);
	}

	_new->dir = nodir;
	_new->angle = iangle;
	_new->speed = 0x2000l;
	_new->flags = FL_NONMARK;
	_new->active = _true;
}



//
// angel
//
void A_Relaunch (objtype *ob);
void A_Victory (objtype *ob);
void A_StartAttack (objtype *ob);
void A_Breathing (objtype *ob);

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


statetype s_angelstand	= {_false,SPR_ANGEL_W1,0,T_Stand,NULL,&s_angelstand};

statetype s_angelchase1 	= {_false,SPR_ANGEL_W1,10,T_Will,NULL,&s_angelchase1s};
statetype s_angelchase1s	= {_false,SPR_ANGEL_W1,3,NULL,NULL,&s_angelchase2};
statetype s_angelchase2 	= {_false,SPR_ANGEL_W2,8,T_Will,NULL,&s_angelchase3};
statetype s_angelchase3 	= {_false,SPR_ANGEL_W3,10,T_Will,NULL,&s_angelchase3s};
statetype s_angelchase3s	= {_false,SPR_ANGEL_W3,3,NULL,NULL,&s_angelchase4};
statetype s_angelchase4 	= {_false,SPR_ANGEL_W4,8,T_Will,NULL,&s_angelchase1};

statetype s_angeldie1	= {_false,SPR_ANGEL_W1,1,NULL,A_DeathScream,&s_angeldie11};
statetype s_angeldie11	= {_false,SPR_ANGEL_W1,1,NULL,NULL,&s_angeldie2};
statetype s_angeldie2	= {_false,SPR_ANGEL_DIE1,10,NULL,A_Slurpie,&s_angeldie3};
statetype s_angeldie3	= {_false,SPR_ANGEL_DIE2,10,NULL,NULL,&s_angeldie4};
statetype s_angeldie4	= {_false,SPR_ANGEL_DIE3,10,NULL,NULL,&s_angeldie5};
statetype s_angeldie5	= {_false,SPR_ANGEL_DIE4,10,NULL,NULL,&s_angeldie6};
statetype s_angeldie6	= {_false,SPR_ANGEL_DIE5,10,NULL,NULL,&s_angeldie7};
statetype s_angeldie7	= {_false,SPR_ANGEL_DIE6,10,NULL,NULL,&s_angeldie8};
statetype s_angeldie8	= {_false,SPR_ANGEL_DIE7,10,NULL,NULL,&s_angeldie9};
statetype s_angeldie9	= {_false,SPR_ANGEL_DEAD,130,NULL,A_Victory,&s_angeldie9};

statetype s_angelshoot1 	= {_false,SPR_ANGEL_SHOOT1,10,NULL,A_StartAttack,&s_angelshoot2};
statetype s_angelshoot2 	= {_false,SPR_ANGEL_SHOOT2,20,NULL,T_Launch,&s_angelshoot3};
statetype s_angelshoot3 	= {_false,SPR_ANGEL_SHOOT1,10,NULL,A_Relaunch,&s_angelshoot2};

statetype s_angeltired 	= {_false,SPR_ANGEL_TIRED1,40,NULL,A_Breathing,&s_angeltired2};
statetype s_angeltired2	= {_false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired3};
statetype s_angeltired3	= {_false,SPR_ANGEL_TIRED1,40,NULL,A_Breathing,&s_angeltired4};
statetype s_angeltired4	= {_false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired5};
statetype s_angeltired5	= {_false,SPR_ANGEL_TIRED1,40,NULL,A_Breathing,&s_angeltired6};
statetype s_angeltired6	= {_false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired7};
statetype s_angeltired7	= {_false,SPR_ANGEL_TIRED1,40,NULL,A_Breathing,&s_angelchase1};

statetype s_spark1 	= {_false,SPR_SPARK1,6,T_Projectile,NULL,&s_spark2};
statetype s_spark2 	= {_false,SPR_SPARK2,6,T_Projectile,NULL,&s_spark3};
statetype s_spark3 	= {_false,SPR_SPARK3,6,T_Projectile,NULL,&s_spark4};
statetype s_spark4 	= {_false,SPR_SPARK4,6,T_Projectile,NULL,&s_spark1};


#pragma argsused
void A_Slurpie (objtype *ob)
{
 SD_PlaySound(SLURPIESND);
}

#pragma argsused
void A_Breathing (objtype *ob)
{
 SD_PlaySound(ANGELTIREDSND);
}

/*
===============
=
= SpawnAngel
=
===============
*/

void SpawnAngel (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;


	if (SoundBlasterPresent && DigiMode != sds_Off)
		s_angeldie11.tictime = 105;

	SpawnNewObj (tilex,tiley,&s_angelstand);
	_new->obclass = angelobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_angel];
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}


/*
=================
=
= A_Victory
=
=================
*/

#pragma argsused
void A_Victory (objtype *ob)
{
	playstate = ex_victorious;
}


/*
=================
=
= A_StartAttack
=
=================
*/

void A_StartAttack (objtype *ob)
{
	ob->temp1 = 0;
}


/*
=================
=
= A_Relaunch
=
=================
*/

void A_Relaunch (objtype *ob)
{
	if (++ob->temp1 == 3)
	{
		NewState (ob,&s_angeltired);
		return;
	}

	if (US_RndT()&1)
	{
		NewState (ob,&s_angelchase1);
		return;
	}
}




//
// spectre
//
void T_SpectreWait (objtype *ob);
void A_Dormant (objtype *ob);

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

statetype s_spectrewait1	= {_false,SPR_SPECTRE_W1,10,T_Stand,NULL,&s_spectrewait2};
statetype s_spectrewait2	= {_false,SPR_SPECTRE_W2,10,T_Stand,NULL,&s_spectrewait3};
statetype s_spectrewait3	= {_false,SPR_SPECTRE_W3,10,T_Stand,NULL,&s_spectrewait4};
statetype s_spectrewait4	= {_false,SPR_SPECTRE_W4,10,T_Stand,NULL,&s_spectrewait1};

statetype s_spectrechase1	= {_false,SPR_SPECTRE_W1,10,T_Ghosts,NULL,&s_spectrechase2};
statetype s_spectrechase2	= {_false,SPR_SPECTRE_W2,10,T_Ghosts,NULL,&s_spectrechase3};
statetype s_spectrechase3	= {_false,SPR_SPECTRE_W3,10,T_Ghosts,NULL,&s_spectrechase4};
statetype s_spectrechase4	= {_false,SPR_SPECTRE_W4,10,T_Ghosts,NULL,&s_spectrechase1};

statetype s_spectredie1	= {_false,SPR_SPECTRE_F1,10,NULL,NULL,&s_spectredie2};
statetype s_spectredie2	= {_false,SPR_SPECTRE_F2,10,NULL,NULL,&s_spectredie3};
statetype s_spectredie3	= {_false,SPR_SPECTRE_F3,10,NULL,NULL,&s_spectredie4};
statetype s_spectredie4	= {_false,SPR_SPECTRE_F4,300,NULL,NULL,&s_spectrewake};
statetype s_spectrewake	= {_false,SPR_SPECTRE_F4,10,NULL,A_Dormant,&s_spectrewake};

/*
===============
=
= SpawnSpectre
=
===============
*/

void SpawnSpectre (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	SpawnNewObj (tilex,tiley,&s_spectrewait1);
	_new->obclass = spectreobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_spectre];
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH; // |FL_NEVERMARK|FL_NONMARK;
	if (!loadedgame)
	  gamestate.killtotal++;
}


/*
===============
=
= A_Dormant
=
===============
*/

void A_Dormant (objtype *ob)
{
	int32_t	deltax,deltay;
	int16_t	xl,xh,yl,yh;
	int16_t	x,y;
	uint16_t	tile;

	deltax = ob->x - player->x;
	if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
		goto moveok;
	deltay = ob->y - player->y;
	if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
		goto moveok;

	return;
moveok:

	xl = (ob->x-MINDIST) >> TILESHIFT;
	xh = (ob->x+MINDIST) >> TILESHIFT;
	yl = (ob->y-MINDIST) >> TILESHIFT;
	yh = (ob->y+MINDIST) >> TILESHIFT;

	for (y=yl ; y<=yh ; y++)
		for (x=xl ; x<=xh ; x++)
		{
			tile = actorat[x][y];
			if (!tile)
				continue;
			if (tile<256)
				return;
			if (((objtype *)tile)->flags&FL_SHOOTABLE)
				return;
		}

	ob->flags |= FL_AMBUSH | FL_SHOOTABLE;
	ob->flags &= ~FL_ATTACKMODE;
	ob->dir = nodir;
	NewState (ob,&s_spectrewait1);
}


#endif

/*
=============================================================================

						 SCHABBS / GIFT / FAT

=============================================================================
*/

#ifndef SPEAR
/*
===============
=
= SpawnGhosts
=
===============
*/

void SpawnGhosts (int16_t which, int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	switch(which)
	{
	 case en_blinky:
	   SpawnNewObj (tilex,tiley,&s_blinkychase1);
	   break;
	 case en_clyde:
	   SpawnNewObj (tilex,tiley,&s_clydechase1);
	   break;
	 case en_pinky:
	   SpawnNewObj (tilex,tiley,&s_pinkychase1);
	   break;
	 case en_inky:
	   SpawnNewObj (tilex,tiley,&s_inkychase1);
	   break;
	}

	_new->obclass = ghostobj;
	_new->speed = SPDDOG;

	_new->dir = east;
	_new->flags |= FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}



void	T_Gift (objtype *ob);
void	T_GiftThrow (objtype *ob);

void	T_Fat (objtype *ob);
void	T_FatThrow (objtype *ob);

//
// schabb
//
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


statetype s_schabbstand	= {_false,SPR_SCHABB_W1,0,(void(*)())T_Stand,NULL,&s_schabbstand};

statetype s_schabbchase1 	= {_false,SPR_SCHABB_W1,10,(void(*)())T_Schabb,NULL,&s_schabbchase1s};
statetype s_schabbchase1s	= {_false,SPR_SCHABB_W1,3,NULL,NULL,&s_schabbchase2};
statetype s_schabbchase2 	= {_false,SPR_SCHABB_W2,8,(void(*)())T_Schabb,NULL,&s_schabbchase3};
statetype s_schabbchase3 	= {_false,SPR_SCHABB_W3,10,(void(*)())T_Schabb,NULL,&s_schabbchase3s};
statetype s_schabbchase3s	= {_false,SPR_SCHABB_W3,3,NULL,NULL,&s_schabbchase4};
statetype s_schabbchase4 	= {_false,SPR_SCHABB_W4,8,(void(*)())T_Schabb,NULL,&s_schabbchase1};

statetype s_schabbdeathcam	= {_false,SPR_SCHABB_W1,1,NULL,NULL,&s_schabbdie1};

statetype s_schabbdie1	= {_false,SPR_SCHABB_W1,10,NULL,(void(*)())A_DeathScream,&s_schabbdie2};
statetype s_schabbdie2	= {_false,SPR_SCHABB_W1,10,NULL,NULL,&s_schabbdie3};
statetype s_schabbdie3	= {_false,SPR_SCHABB_DIE1,10,NULL,NULL,&s_schabbdie4};
statetype s_schabbdie4	= {_false,SPR_SCHABB_DIE2,10,NULL,NULL,&s_schabbdie5};
statetype s_schabbdie5	= {_false,SPR_SCHABB_DIE3,10,NULL,NULL,&s_schabbdie6};
statetype s_schabbdie6	= {_false,SPR_SCHABB_DEAD,20,NULL,(void(*)())A_StartDeathCam,&s_schabbdie6};

statetype s_schabbshoot1 	= {_false,SPR_SCHABB_SHOOT1,30,NULL,NULL,&s_schabbshoot2};
statetype s_schabbshoot2 	= {_false,SPR_SCHABB_SHOOT2,10,NULL,(void(*)())T_SchabbThrow,&s_schabbchase1};

statetype s_needle1 	= {_false,SPR_HYPO1,6,(void(*)())T_Projectile,NULL,&s_needle2};
statetype s_needle2 	= {_false,SPR_HYPO2,6,(void(*)())T_Projectile,NULL,&s_needle3};
statetype s_needle3 	= {_false,SPR_HYPO3,6,(void(*)())T_Projectile,NULL,&s_needle4};
statetype s_needle4 	= {_false,SPR_HYPO4,6,(void(*)())T_Projectile,NULL,&s_needle1};


//
// gift
//
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


statetype s_giftstand	= {_false,SPR_GIFT_W1,0,(void(*)())T_Stand,NULL,&s_giftstand};

statetype s_giftchase1 	= {_false,SPR_GIFT_W1,10,(void(*)())T_Gift,NULL,&s_giftchase1s};
statetype s_giftchase1s	= {_false,SPR_GIFT_W1,3,NULL,NULL,&s_giftchase2};
statetype s_giftchase2 	= {_false,SPR_GIFT_W2,8,(void(*)())T_Gift,NULL,&s_giftchase3};
statetype s_giftchase3 	= {_false,SPR_GIFT_W3,10,(void(*)())T_Gift,NULL,&s_giftchase3s};
statetype s_giftchase3s	= {_false,SPR_GIFT_W3,3,NULL,NULL,&s_giftchase4};
statetype s_giftchase4 	= {_false,SPR_GIFT_W4,8,(void(*)())T_Gift,NULL,&s_giftchase1};

statetype s_giftdeathcam	= {_false,SPR_GIFT_W1,1,NULL,NULL,&s_giftdie1};

statetype s_giftdie1	= {_false,SPR_GIFT_W1,1,NULL,(void(*)())A_DeathScream,&s_giftdie2};
statetype s_giftdie2	= {_false,SPR_GIFT_W1,10,NULL,NULL,&s_giftdie3};
statetype s_giftdie3	= {_false,SPR_GIFT_DIE1,10,NULL,NULL,&s_giftdie4};
statetype s_giftdie4	= {_false,SPR_GIFT_DIE2,10,NULL,NULL,&s_giftdie5};
statetype s_giftdie5	= {_false,SPR_GIFT_DIE3,10,NULL,NULL,&s_giftdie6};
statetype s_giftdie6	= {_false,SPR_GIFT_DEAD,20,NULL,(void(*)())A_StartDeathCam,&s_giftdie6};

statetype s_giftshoot1 	= {_false,SPR_GIFT_SHOOT1,30,NULL,NULL,&s_giftshoot2};
statetype s_giftshoot2 	= {_false,SPR_GIFT_SHOOT2,10,NULL,(void(*)())T_GiftThrow,&s_giftchase1};


//
// fat
//
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


statetype s_fatstand	= {_false,SPR_FAT_W1,0,(void(*)())T_Stand,NULL,&s_fatstand};

statetype s_fatchase1 	= {_false,SPR_FAT_W1,10,(void(*)())T_Fat,NULL,&s_fatchase1s};
statetype s_fatchase1s	= {_false,SPR_FAT_W1,3,NULL,NULL,&s_fatchase2};
statetype s_fatchase2 	= {_false,SPR_FAT_W2,8,(void(*)())T_Fat,NULL,&s_fatchase3};
statetype s_fatchase3 	= {_false,SPR_FAT_W3,10,(void(*)())T_Fat,NULL,&s_fatchase3s};
statetype s_fatchase3s	= {_false,SPR_FAT_W3,3,NULL,NULL,&s_fatchase4};
statetype s_fatchase4 	= {_false,SPR_FAT_W4,8,(void(*)())T_Fat,NULL,&s_fatchase1};

statetype s_fatdeathcam	= {_false,SPR_FAT_W1,1,NULL,NULL,&s_fatdie1};

statetype s_fatdie1	= {_false,SPR_FAT_W1,1,NULL,(void(*)())A_DeathScream,&s_fatdie2};
statetype s_fatdie2	= {_false,SPR_FAT_W1,10,NULL,NULL,&s_fatdie3};
statetype s_fatdie3	= {_false,SPR_FAT_DIE1,10,NULL,NULL,&s_fatdie4};
statetype s_fatdie4	= {_false,SPR_FAT_DIE2,10,NULL,NULL,&s_fatdie5};
statetype s_fatdie5	= {_false,SPR_FAT_DIE3,10,NULL,NULL,&s_fatdie6};
statetype s_fatdie6	= {_false,SPR_FAT_DEAD,20,NULL,(void(*)())A_StartDeathCam,&s_fatdie6};

statetype s_fatshoot1 	= {_false,SPR_FAT_SHOOT1,30,NULL,NULL,&s_fatshoot2};
statetype s_fatshoot2 	= {_false,SPR_FAT_SHOOT2,10,NULL,(void(*)())T_GiftThrow,&s_fatshoot3};
statetype s_fatshoot3 	= {_false,SPR_FAT_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_fatshoot4};
statetype s_fatshoot4 	= {_false,SPR_FAT_SHOOT4,10,NULL,(void(*)())T_Shoot,&s_fatshoot5};
statetype s_fatshoot5 	= {_false,SPR_FAT_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_fatshoot6};
statetype s_fatshoot6 	= {_false,SPR_FAT_SHOOT4,10,NULL,(void(*)())T_Shoot,&s_fatchase1};


/*
===============
=
= SpawnSchabbs
=
===============
*/

void SpawnSchabbs (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	if (DigiMode != sds_Off)
		s_schabbdie2.tictime = 140;
	else
		s_schabbdie2.tictime = 5;

	SpawnNewObj (tilex,tiley,&s_schabbstand);
	_new->speed = SPDPATROL;

	_new->obclass = schabbobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_schabbs];
	_new->dir = south;
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}


/*
===============
=
= SpawnGift
=
===============
*/

void SpawnGift (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	if (DigiMode != sds_Off)
	  s_giftdie2.tictime = 140;
	else
	  s_giftdie2.tictime = 5;

	SpawnNewObj (tilex,tiley,&s_giftstand);
	_new->speed = SPDPATROL;

	_new->obclass = giftobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_gift];
	_new->dir = north;
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}


/*
===============
=
= SpawnFat
=
===============
*/

void SpawnFat (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	if (DigiMode != sds_Off)
	  s_fatdie2.tictime = 140;
	else
	  s_fatdie2.tictime = 5;

	SpawnNewObj (tilex,tiley,&s_fatstand);
	_new->speed = SPDPATROL;

	_new->obclass = fatobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_fat];
	_new->dir = south;
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}


/*
=================
=
= T_SchabbThrow
=
=================
*/

void T_SchabbThrow (objtype *ob)
{
	int32_t	deltax,deltay;
	float	angle;
	int16_t		iangle;

	deltax = player->x - ob->x;
	deltay = ob->y - player->y;
	angle = (float)atan2 (deltay,deltax);
	if (angle<0)
		angle = (float)(M_PI*2+angle);
	iangle = (int16_t)(angle/(M_PI*2)*ANGLES);

	GetNewActor ();
	_new->state = &s_needle1;
	_new->ticcount = 1;

	_new->tilex = ob->tilex;
	_new->tiley = ob->tiley;
	_new->x = ob->x;
	_new->y = ob->y;
	_new->obclass = needleobj;
	_new->dir = nodir;
	_new->angle = iangle;
	_new->speed = 0x2000l;

	_new->flags = FL_NONMARK;
	_new->active = (activetype)_true;

	PlaySoundLocActor (SCHABBSTHROWSND,_new);
}

/*
=================
=
= T_GiftThrow
=
=================
*/

void T_GiftThrow (objtype *ob)
{
	int32_t	deltax,deltay;
	float	angle;
	int16_t		iangle;

	deltax = player->x - ob->x;
	deltay = ob->y - player->y;
	angle = (float)atan2 (deltay,deltax);
	if (angle<0)
		angle = (float)(M_PI*2+angle);
	iangle = (int16_t)(angle/(M_PI*2)*ANGLES);

	GetNewActor ();
	_new->state = &s_rocket;
	_new->ticcount = 1;

	_new->tilex = ob->tilex;
	_new->tiley = ob->tiley;
	_new->x = ob->x;
	_new->y = ob->y;
	_new->obclass = rocketobj;
	_new->dir = nodir;
	_new->angle = iangle;
	_new->speed = 0x2000l;
	_new->flags = FL_NONMARK;
	_new->active = (activetype)_true;

	PlaySoundLocActor (MISSILEFIRESND,_new);
}



/*
=================
=
= T_Schabb
=
=================
*/

void T_Schabb (objtype *ob)
{
	int32_t move;
	int16_t	dx,dy,dist;
	_boolean	dodge;

	dodge = _false;
	dx = abs((int16_t)ob->tilex - (int16_t)player->tilex);
	dy = abs((int16_t)ob->tiley - (int16_t)player->tiley);
	dist = dx>dy ? dx : dy;

	if (CheckLine(ob))						// got a shot at player?
	{

		if ( US_RndT() < (tics<<3) )
		{
		//
		// go into attack frame
		//
			NewState (ob,&s_schabbshoot1);
			return;
		}
		dodge = _true;
	}

	if (ob->dir == nodir)
	{
		if (dodge)
			SelectDodgeDir (ob);
		else
			SelectChaseDir (ob);
		if (ob->dir == nodir)
			return;							// object is blocked in
	}

	move = ob->speed*tics;

	while (move)
	{
		if (ob->distance < 0)
		{
		//
		// waiting for a door to open
		//
			OpenDoor (-ob->distance-1);
			if (doorobjlist[-ob->distance-1].action != doorobj_t::dr_open)
				return;
			ob->distance = TILEGLOBAL;	// go ahead, the door is now opoen
		}

		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}

		//
		// reached goal tile, so select another one
		//

		//
		// fix position to account for round off during moving
		//
		ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

		move -= ob->distance;

		if (dist <4)
			SelectRunDir (ob);
		else if (dodge)
			SelectDodgeDir (ob);
		else
			SelectChaseDir (ob);

		if (ob->dir == nodir)
			return;							// object is blocked in
	}

}




/*
=================
=
= T_Gift
=
=================
*/

void T_Gift (objtype *ob)
{
	int32_t move;
	int16_t	dx,dy,dist;
	_boolean	dodge;

	dodge = _false;
	dx = abs((int16_t)ob->tilex - (int16_t)player->tilex);
	dy = abs((int16_t)ob->tiley - (int16_t)player->tiley);
	dist = dx>dy ? dx : dy;

	if (CheckLine(ob))						// got a shot at player?
	{

		if ( US_RndT() < (tics<<3) )
		{
		//
		// go into attack frame
		//
			NewState (ob,&s_giftshoot1);
			return;
		}
		dodge = _true;
	}

	if (ob->dir == nodir)
	{
		if (dodge)
			SelectDodgeDir (ob);
		else
			SelectChaseDir (ob);
		if (ob->dir == nodir)
			return;							// object is blocked in
	}

	move = ob->speed*tics;

	while (move)
	{
		if (ob->distance < 0)
		{
		//
		// waiting for a door to open
		//
			OpenDoor (-ob->distance-1);
			if (doorobjlist[-ob->distance-1].action != doorobj_t::dr_open)
				return;
			ob->distance = TILEGLOBAL;	// go ahead, the door is now opoen
		}

		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}

		//
		// reached goal tile, so select another one
		//

		//
		// fix position to account for round off during moving
		//
		ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

		move -= ob->distance;

		if (dist <4)
			SelectRunDir (ob);
		else if (dodge)
			SelectDodgeDir (ob);
		else
			SelectChaseDir (ob);

		if (ob->dir == nodir)
			return;							// object is blocked in
	}

}




/*
=================
=
= T_Fat
=
=================
*/

void T_Fat (objtype *ob)
{
	int32_t move;
	int16_t	dx,dy,dist;
	_boolean	dodge;

	dodge = _false;
	dx = abs((int16_t)ob->tilex - (int16_t)player->tilex);
	dy = abs((int16_t)ob->tiley - (int16_t)player->tiley);
	dist = dx>dy ? dx : dy;

	if (CheckLine(ob))						// got a shot at player?
	{

		if ( US_RndT() < (tics<<3) )
		{
		//
		// go into attack frame
		//
			NewState (ob,&s_fatshoot1);
			return;
		}
		dodge = _true;
	}

	if (ob->dir == nodir)
	{
		if (dodge)
			SelectDodgeDir (ob);
		else
			SelectChaseDir (ob);
		if (ob->dir == nodir)
			return;							// object is blocked in
	}

	move = ob->speed*tics;

	while (move)
	{
		if (ob->distance < 0)
		{
		//
		// waiting for a door to open
		//
			OpenDoor (-ob->distance-1);
			if (doorobjlist[-ob->distance-1].action != doorobj_t::dr_open)
				return;
			ob->distance = TILEGLOBAL;	// go ahead, the door is now opoen
		}

		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}

		//
		// reached goal tile, so select another one
		//

		//
		// fix position to account for round off during moving
		//
		ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

		move -= ob->distance;

		if (dist <4)
			SelectRunDir (ob);
		else if (dodge)
			SelectDodgeDir (ob);
		else
			SelectChaseDir (ob);

		if (ob->dir == nodir)
			return;							// object is blocked in
	}

}



/*
=============================================================================

							HITLERS

=============================================================================
*/


//
// fake
//
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

statetype s_fakestand	= {_false,SPR_FAKE_W1,0,(void(*)())T_Stand,NULL,&s_fakestand};

statetype s_fakechase1 	= {_false,SPR_FAKE_W1,10,(void(*)())T_Fake,NULL,&s_fakechase1s};
statetype s_fakechase1s	= {_false,SPR_FAKE_W1,3,NULL,NULL,&s_fakechase2};
statetype s_fakechase2 	= {_false,SPR_FAKE_W2,8,(void(*)())T_Fake,NULL,&s_fakechase3};
statetype s_fakechase3 	= {_false,SPR_FAKE_W3,10,(void(*)())T_Fake,NULL,&s_fakechase3s};
statetype s_fakechase3s	= {_false,SPR_FAKE_W3,3,NULL,NULL,&s_fakechase4};
statetype s_fakechase4 	= {_false,SPR_FAKE_W4,8,(void(*)())T_Fake,NULL,&s_fakechase1};

statetype s_fakedie1	= {_false,SPR_FAKE_DIE1,10,NULL,(void(*)())A_DeathScream,&s_fakedie2};
statetype s_fakedie2	= {_false,SPR_FAKE_DIE2,10,NULL,NULL,&s_fakedie3};
statetype s_fakedie3	= {_false,SPR_FAKE_DIE3,10,NULL,NULL,&s_fakedie4};
statetype s_fakedie4	= {_false,SPR_FAKE_DIE4,10,NULL,NULL,&s_fakedie5};
statetype s_fakedie5	= {_false,SPR_FAKE_DIE5,10,NULL,NULL,&s_fakedie6};
statetype s_fakedie6	= {_false,SPR_FAKE_DEAD,0,NULL,NULL,&s_fakedie6};

statetype s_fakeshoot1 	= {_false,SPR_FAKE_SHOOT,8,NULL,(void(*)())T_FakeFire,&s_fakeshoot2};
statetype s_fakeshoot2 	= {_false,SPR_FAKE_SHOOT,8,NULL,(void(*)())T_FakeFire,&s_fakeshoot3};
statetype s_fakeshoot3 	= {_false,SPR_FAKE_SHOOT,8,NULL,(void(*)())T_FakeFire,&s_fakeshoot4};
statetype s_fakeshoot4 	= {_false,SPR_FAKE_SHOOT,8,NULL,(void(*)())T_FakeFire,&s_fakeshoot5};
statetype s_fakeshoot5 	= {_false,SPR_FAKE_SHOOT,8,NULL,(void(*)())T_FakeFire,&s_fakeshoot6};
statetype s_fakeshoot6 	= {_false,SPR_FAKE_SHOOT,8,NULL,(void(*)())T_FakeFire,&s_fakeshoot7};
statetype s_fakeshoot7 	= {_false,SPR_FAKE_SHOOT,8,NULL,(void(*)())T_FakeFire,&s_fakeshoot8};
statetype s_fakeshoot8 	= {_false,SPR_FAKE_SHOOT,8,NULL,(void(*)())T_FakeFire,&s_fakeshoot9};
statetype s_fakeshoot9 	= {_false,SPR_FAKE_SHOOT,8,NULL,NULL,&s_fakechase1};

statetype s_fire1 	= {_false,SPR_FIRE1,6,NULL,(void(*)())T_Projectile,&s_fire2};
statetype s_fire2 	= {_false,SPR_FIRE2,6,NULL,(void(*)())T_Projectile,&s_fire1};

//
// hitler
//
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

statetype s_mechastand	= {_false,SPR_MECHA_W1,0,(void(*)())T_Stand,NULL,&s_mechastand};

statetype s_mechachase1 	= {_false,SPR_MECHA_W1,10,(void(*)())T_Chase,(void(*)())A_MechaSound,&s_mechachase1s};
statetype s_mechachase1s	= {_false,SPR_MECHA_W1,6,NULL,NULL,&s_mechachase2};
statetype s_mechachase2 	= {_false,SPR_MECHA_W2,8,(void(*)())T_Chase,NULL,&s_mechachase3};
statetype s_mechachase3 	= {_false,SPR_MECHA_W3,10,(void(*)())T_Chase,(void(*)())A_MechaSound,&s_mechachase3s};
statetype s_mechachase3s	= {_false,SPR_MECHA_W3,6,NULL,NULL,&s_mechachase4};
statetype s_mechachase4 	= {_false,SPR_MECHA_W4,8,(void(*)())T_Chase,NULL,&s_mechachase1};

statetype s_mechadie1	= {_false,SPR_MECHA_DIE1,10,NULL,(void(*)())A_DeathScream,&s_mechadie2};
statetype s_mechadie2	= {_false,SPR_MECHA_DIE2,10,NULL,NULL,&s_mechadie3};
statetype s_mechadie3	= {_false,SPR_MECHA_DIE3,10,NULL,(void(*)())A_HitlerMorph,&s_mechadie4};
statetype s_mechadie4	= {_false,SPR_MECHA_DEAD,0,NULL,NULL,&s_mechadie4};

statetype s_mechashoot1 	= {_false,SPR_MECHA_SHOOT1,30,NULL,NULL,&s_mechashoot2};
statetype s_mechashoot2 	= {_false,SPR_MECHA_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_mechashoot3};
statetype s_mechashoot3 	= {_false,SPR_MECHA_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_mechashoot4};
statetype s_mechashoot4 	= {_false,SPR_MECHA_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_mechashoot5};
statetype s_mechashoot5 	= {_false,SPR_MECHA_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_mechashoot6};
statetype s_mechashoot6 	= {_false,SPR_MECHA_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_mechachase1};


statetype s_hitlerchase1 	= {_false,SPR_HITLER_W1,6,(void(*)())T_Chase,NULL,&s_hitlerchase1s};
statetype s_hitlerchase1s	= {_false,SPR_HITLER_W1,4,NULL,NULL,&s_hitlerchase2};
statetype s_hitlerchase2 	= {_false,SPR_HITLER_W2,2,(void(*)())T_Chase,NULL,&s_hitlerchase3};
statetype s_hitlerchase3 	= {_false,SPR_HITLER_W3,6,(void(*)())T_Chase,NULL,&s_hitlerchase3s};
statetype s_hitlerchase3s	= {_false,SPR_HITLER_W3,4,NULL,NULL,&s_hitlerchase4};
statetype s_hitlerchase4 	= {_false,SPR_HITLER_W4,2,(void(*)())T_Chase,NULL,&s_hitlerchase1};

statetype s_hitlerdeathcam	= {_false,SPR_HITLER_W1,10,NULL,NULL,&s_hitlerdie1};

statetype s_hitlerdie1	= {_false,SPR_HITLER_W1,1,NULL,(void(*)())A_DeathScream,&s_hitlerdie2};
statetype s_hitlerdie2	= {_false,SPR_HITLER_W1,10,NULL,NULL,&s_hitlerdie3};
statetype s_hitlerdie3	= {_false,SPR_HITLER_DIE1,10,NULL,(void(*)())A_Slurpie,&s_hitlerdie4};
statetype s_hitlerdie4	= {_false,SPR_HITLER_DIE2,10,NULL,NULL,&s_hitlerdie5};
statetype s_hitlerdie5	= {_false,SPR_HITLER_DIE3,10,NULL,NULL,&s_hitlerdie6};
statetype s_hitlerdie6	= {_false,SPR_HITLER_DIE4,10,NULL,NULL,&s_hitlerdie7};
statetype s_hitlerdie7	= {_false,SPR_HITLER_DIE5,10,NULL,NULL,&s_hitlerdie8};
statetype s_hitlerdie8	= {_false,SPR_HITLER_DIE6,10,NULL,NULL,&s_hitlerdie9};
statetype s_hitlerdie9	= {_false,SPR_HITLER_DIE7,10,NULL,NULL,&s_hitlerdie10};
statetype s_hitlerdie10	= {_false,SPR_HITLER_DEAD,20,NULL,(void(*)())A_StartDeathCam,&s_hitlerdie10};

statetype s_hitlershoot1 	= {_false,SPR_HITLER_SHOOT1,30,NULL,NULL,&s_hitlershoot2};
statetype s_hitlershoot2 	= {_false,SPR_HITLER_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_hitlershoot3};
statetype s_hitlershoot3 	= {_false,SPR_HITLER_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_hitlershoot4};
statetype s_hitlershoot4 	= {_false,SPR_HITLER_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_hitlershoot5};
statetype s_hitlershoot5 	= {_false,SPR_HITLER_SHOOT3,10,NULL,(void(*)())T_Shoot,&s_hitlershoot6};
statetype s_hitlershoot6 	= {_false,SPR_HITLER_SHOOT2,10,NULL,(void(*)())T_Shoot,&s_hitlerchase1};



/*
===============
=
= SpawnFakeHitler
=
===============
*/

void SpawnFakeHitler (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;


	if (DigiMode != sds_Off)
	  s_hitlerdie2.tictime = 140;
	else
	  s_hitlerdie2.tictime = 5;

	SpawnNewObj (tilex,tiley,&s_fakestand);
	_new->speed = SPDPATROL;

	_new->obclass = fakeobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_fake];
	_new->dir = north;
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}


/*
===============
=
= SpawnHitler
=
===============
*/

void SpawnHitler (int16_t tilex, int16_t tiley)
{
	uint16_t	 *map,tile;

	if (DigiMode != sds_Off)
		s_hitlerdie2.tictime = 140;
	else
		s_hitlerdie2.tictime = 5;


	SpawnNewObj (tilex,tiley,&s_mechastand);
	_new->speed = SPDPATROL;

	_new->obclass = mechahitlerobj;
	_new->hitpoints = starthitpoints[gamestate.difficulty][en_hitler];
	_new->dir = south;
	_new->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if (!loadedgame)
	  gamestate.killtotal++;
}


/*
===============
=
= A_HitlerMorph
=
===============
*/

void A_HitlerMorph (objtype *ob)
{
	uint16_t	 *map,tile,hitpoints[4]={500,700,800,900};


	SpawnNewObj (ob->tilex,ob->tiley,&s_hitlerchase1);
	_new->speed = SPDPATROL*5;

	_new->x = ob->x;
	_new->y = ob->y;

	_new->distance = ob->distance;
	_new->dir = ob->dir;
	_new->flags = ob->flags | FL_SHOOTABLE;

	_new->obclass = realhitlerobj;
	_new->hitpoints = hitpoints[gamestate.difficulty];
}


////////////////////////////////////////////////////////
//
// A_MechaSound
// A_Slurpie
//
////////////////////////////////////////////////////////
void A_MechaSound (objtype *ob)
{
	if (areabyplayer[ob->areanumber])
		PlaySoundLocActor (MECHSTEPSND,ob);
}


#pragma argsused
void A_Slurpie (objtype *ob)
{
 SD_PlaySound(SLURPIESND);
}

/*
=================
=
= T_FakeFire
=
=================
*/

void T_FakeFire (objtype *ob)
{
	int32_t	deltax,deltay;
	float	angle;
	int16_t		iangle;

	deltax = player->x - ob->x;
	deltay = ob->y - player->y;
	angle = (float)(atan2 (deltay,deltax));
	if (angle<0)
		angle = (float)(M_PI*2+angle);
	iangle = (int16_t)(angle/(M_PI*2)*ANGLES);

	GetNewActor ();
	_new->state = &s_fire1;
	_new->ticcount = 1;

	_new->tilex = ob->tilex;
	_new->tiley = ob->tiley;
	_new->x = ob->x;
	_new->y = ob->y;
	_new->dir = nodir;
	_new->angle = iangle;
	_new->obclass = fireobj;
	_new->speed = 0x1200l;
	_new->flags = FL_NEVERMARK;
	_new->active = (activetype)_true;

	PlaySoundLocActor (FLAMETHROWERSND,_new);
}



/*
=================
=
= T_Fake
=
=================
*/

void T_Fake (objtype *ob)
{
	int32_t move;
	int16_t	dx,dy,dist;
	_boolean	dodge;

	if (CheckLine(ob))			// got a shot at player?
	{
		if ( US_RndT() < (tics<<1) )
		{
		//
		// go into attack frame
		//
			NewState (ob,&s_fakeshoot1);
			return;
		}
	}

	if (ob->dir == nodir)
	{
		SelectDodgeDir (ob);
		if (ob->dir == nodir)
			return;							// object is blocked in
	}

	move = ob->speed*tics;

	while (move)
	{
		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}

		//
		// reached goal tile, so select another one
		//

		//
		// fix position to account for round off during moving
		//
		ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

		move -= ob->distance;

		SelectDodgeDir (ob);

		if (ob->dir == nodir)
			return;							// object is blocked in
	}

}

#endif
/*
============================================================================

							STAND

============================================================================
*/


/*
===============
=
= T_Stand
=
===============
*/

void T_Stand (objtype *ob)
{
	SightPlayer (ob);
}


/*
============================================================================

								CHASE

============================================================================
*/

/*
=================
=
= T_Chase
=
=================
*/

void T_Chase (objtype *ob)
{
	int32_t move;
	int16_t	dx,dy,dist,chance;
	_boolean	dodge;

	if (gamestate.victoryflag)
		return;

	dodge = _false;
	if (CheckLine(ob))	// got a shot at player?
	{
		dx = abs((int16_t)ob->tilex - (int16_t)player->tilex);
		dy = abs((int16_t)ob->tiley - (int16_t)player->tiley);
		dist = dx>dy ? dx : dy;
		if (!dist || (dist==1 && ob->distance<0x4000) )
			chance = 300;
		else
			chance = (tics<<4)/dist;

		if ( US_RndT()<chance)
		{
		//
		// go into attack frame
		//
			switch (ob->obclass)
			{
                default:break;
			case guardobj:
				NewState (ob,&s_grdshoot1);
				break;
			case officerobj:
				NewState (ob,&s_ofcshoot1);
				break;
			case mutantobj:
				NewState (ob,&s_mutshoot1);
				break;
			case ssobj:
				NewState (ob,&s_ssshoot1);
				break;
#ifndef SPEAR
			case bossobj:
				NewState (ob,&s_bossshoot1);
				break;
			case gretelobj:
				NewState (ob,&s_gretelshoot1);
				break;
			case mechahitlerobj:
				NewState (ob,&s_mechashoot1);
				break;
			case realhitlerobj:
				NewState (ob,&s_hitlershoot1);
				break;
#else
			case angelobj:
				NewState (ob,&s_angelshoot1);
				break;
			case transobj:
				NewState (ob,&s_transshoot1);
				break;
			case uberobj:
				NewState (ob,&s_ubershoot1);
				break;
			case willobj:
				NewState (ob,&s_willshoot1);
				break;
			case deathobj:
				NewState (ob,&s_deathshoot1);
				break;
#endif
			}
			return;
		}
		dodge = _true;
	}

	if (ob->dir == nodir)
	{
		if (dodge)
			SelectDodgeDir (ob);
		else
			SelectChaseDir (ob);
		if (ob->dir == nodir)
			return;							// object is blocked in
	}

	move = ob->speed*tics;

	while (move)
	{
		if (ob->distance < 0)
		{
		//
		// waiting for a door to open
		//
			OpenDoor (-ob->distance-1);
			if (doorobjlist[-ob->distance-1].action != doorobj_t::dr_open)
				return;
			ob->distance = TILEGLOBAL;	// go ahead, the door is now opoen
		}

		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}

		//
		// reached goal tile, so select another one
		//

		//
		// fix position to account for round off during moving
		//
		ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

		move -= ob->distance;

		if (dodge)
			SelectDodgeDir (ob);
		else
			SelectChaseDir (ob);

		if (ob->dir == nodir)
			return;							// object is blocked in
	}

}


/*
=================
=
= T_Ghosts
=
=================
*/

void T_Ghosts (objtype *ob)
{
	int32_t move;


	if (ob->dir == nodir)
	{
		SelectChaseDir (ob);
		if (ob->dir == nodir)
			return;							// object is blocked in
	}

	move = ob->speed*tics;

	while (move)
	{
		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}

		//
		// reached goal tile, so select another one
		//

		//
		// fix position to account for round off during moving
		//
		ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

		move -= ob->distance;

		SelectChaseDir (ob);

		if (ob->dir == nodir)
			return;							// object is blocked in
	}

}

/*
=================
=
= T_DogChase
=
=================
*/

void T_DogChase (objtype *ob)
{
	int32_t 	move;
	int16_t		dist,chance;
	int32_t	dx,dy;


	if (ob->dir == nodir)
	{
		SelectDodgeDir (ob);
		if (ob->dir == nodir)
			return;							// object is blocked in
	}

	move = ob->speed*tics;

	while (move)
	{
	//
	// check for byte range
	//
		dx = player->x - ob->x;
		if (dx<0)
			dx = -dx;
		dx -= move;
		if (dx <= MINACTORDIST)
		{
			dy = player->y - ob->y;
			if (dy<0)
				dy = -dy;
			dy -= move;
			if (dy <= MINACTORDIST)
			{
				NewState (ob,&s_dogjump1);
				return;
			}
		}

		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}

		//
		// reached goal tile, so select another one
		//

		//
		// fix position to account for round off during moving
		//
		ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

		move -= ob->distance;

		SelectDodgeDir (ob);

		if (ob->dir == nodir)
			return;							// object is blocked in
	}

}



/*
============================================================================

								PATH

============================================================================
*/


/*
===============
=
= SelectPathDir
=
===============
*/

void SelectPathDir (objtype *ob)
{
	uint16_t spot;

	spot = MAPSPOT(ob->tilex,ob->tiley,1)-ICONARROWS;

	if (spot<8)
	{
	// _new direction
		ob->dir = (dirtype)spot;
	}

	ob->distance = TILEGLOBAL;

	if (!TryWalk (ob))
		ob->dir = nodir;
}


/*
===============
=
= T_Path
=
===============
*/

void T_Path (objtype *ob)
{
	int32_t 	move;
	int32_t 	deltax,deltay,size;

	if (SightPlayer (ob))
		return;

	if (ob->dir == nodir)
	{
		SelectPathDir (ob);
		if (ob->dir == nodir)
			return;					// all movement is blocked
	}


	move = ob->speed*tics;

	while (move)
	{
		if (ob->distance < 0)
		{
		//
		// waiting for a door to open
		//
			OpenDoor (-ob->distance-1);
			if (doorobjlist[-ob->distance-1].action != doorobj_t::dr_open)
				return;
			ob->distance = TILEGLOBAL;	// go ahead, the door is now opoen
		}

		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}

		if (ob->tilex>MAPSIZE || ob->tiley>MAPSIZE)
		{
			sprintf (str,"T_Path hit a wall at %u,%u, dir %u"
			,ob->tilex,ob->tiley,ob->dir);
			Quit (str);
		}



		ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
		move -= ob->distance;

		SelectPathDir (ob);

		if (ob->dir == nodir)
			return;					// all movement is blocked
	}
}


/*
=============================================================================

								FIGHT

=============================================================================
*/


/*
===============
=
= T_Shoot
=
= Try to damage the player, based on skill level and player's speed
=
===============
*/

void T_Shoot (objtype *ob)
{
	int16_t	dx,dy,dist;
	int16_t	hitchance,damage;

	hitchance = 128;

	if (!areabyplayer[ob->areanumber])
		return;

	if (!CheckLine (ob))			// player is behind a wall
	  return;

	dx = abs((int16_t)ob->tilex - (int16_t)player->tilex);
	dy = abs((int16_t)ob->tiley - (int16_t)player->tiley);
	dist = dx>dy ? dx:dy;

	if (ob->obclass == ssobj || ob->obclass == bossobj)
		dist = dist*2/3;					// ss are better shots

	if (thrustspeed >= RUNSPEED)
	{
		if (ob->flags&FL_VISABLE)
			hitchance = 160-dist*16;		// player can see to dodge
		else
			hitchance = 160-dist*8;
	}
	else
	{
		if (ob->flags&FL_VISABLE)
			hitchance = 256-dist*16;		// player can see to dodge
		else
			hitchance = 256-dist*8;
	}

// see if the shot was a hit

	if (US_RndT()<hitchance)
	{
		if (dist<2)
			damage = US_RndT()>>2;
		else if (dist<4)
			damage = US_RndT()>>3;
		else
			damage = US_RndT()>>4;

		TakeDamage (damage,ob);
	}

	switch(ob->obclass)
	{
	 case ssobj:
	   PlaySoundLocActor(SSFIRESND,ob);
	   break;
#ifndef SPEAR
	 case giftobj:
	 case fatobj:
	   PlaySoundLocActor(MISSILEFIRESND,ob);
	   break;
	 case mechahitlerobj:
	 case realhitlerobj:
	 case bossobj:
	   PlaySoundLocActor(BOSSFIRESND,ob);
	   break;
	 case schabbobj:
	   PlaySoundLocActor(SCHABBSTHROWSND,ob);
	   break;
	 case fakeobj:
	   PlaySoundLocActor(FLAMETHROWERSND,ob);
	   break;
#endif
	 default:
	   PlaySoundLocActor(NAZIFIRESND,ob);
	}

}


/*
===============
=
= T_Bite
=
===============
*/

void T_Bite (objtype *ob)
{
	int32_t	dx,dy;
	int16_t	hitchance,damage;


	PlaySoundLocActor(DOGATTACKSND,ob);	// JAB

	dx = player->x - ob->x;
	if (dx<0)
		dx = -dx;
	dx -= TILEGLOBAL;
	if (dx <= MINACTORDIST)
	{
		dy = player->y - ob->y;
		if (dy<0)
			dy = -dy;
		dy -= TILEGLOBAL;
		if (dy <= MINACTORDIST)
		{
		   if (US_RndT()<180)
		   {
			   TakeDamage (US_RndT()>>4,ob);
			   return;
		   }
		}
	}

	return;
}


#ifndef SPEAR
/*
============================================================================

							BJ VICTORY

============================================================================
*/


//
// BJ victory
//

void T_BJRun (objtype *ob);
void T_BJJump (objtype *ob);
void T_BJDone (objtype *ob);
void T_BJYell (objtype *ob);

void T_DeathCam (objtype *ob);

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


statetype s_bjrun1 	= {_false,SPR_BJ_W1,12,(void(*)())T_BJRun,NULL,&s_bjrun1s};
statetype s_bjrun1s	= {_false,SPR_BJ_W1,3, NULL,NULL,&s_bjrun2};
statetype s_bjrun2 	= {_false,SPR_BJ_W2,8,(void(*)())T_BJRun,NULL,&s_bjrun3};
statetype s_bjrun3 	= {_false,SPR_BJ_W3,12,(void(*)())T_BJRun,NULL,&s_bjrun3s};
statetype s_bjrun3s	= {_false,SPR_BJ_W3,3, NULL,NULL,&s_bjrun4};
statetype s_bjrun4 	= {_false,SPR_BJ_W4,8,(void(*)())T_BJRun,NULL,&s_bjrun1};


statetype s_bjjump1	= {_false,SPR_BJ_JUMP1,14,(void(*)())T_BJJump,NULL,&s_bjjump2};
statetype s_bjjump2	= {_false,SPR_BJ_JUMP2,14,(void(*)())T_BJJump,(void(*)())T_BJYell,&s_bjjump3};
statetype s_bjjump3	= {_false,SPR_BJ_JUMP3,14,(void(*)())T_BJJump,NULL,&s_bjjump4};
statetype s_bjjump4	= {_false,SPR_BJ_JUMP4,300,NULL,(void(*)())T_BJDone,&s_bjjump4};


statetype s_deathcam = {_false,0,0,NULL,NULL,NULL};


/*
===============
=
= SpawnBJVictory
=
===============
*/

void SpawnBJVictory (void)
{
	uint16_t	 *map,tile;

	SpawnNewObj (player->tilex,player->tiley+1,&s_bjrun1);
	_new->x = player->x;
	_new->y = player->y;
	_new->obclass = bjobj;
	_new->dir = north;
	_new->temp1 = 6;			// tiles to run forward
}



/*
===============
=
= T_BJRun
=
===============
*/

void T_BJRun (objtype *ob)
{
	int32_t 	move;

	move = BJRUNSPEED*tics;

	while (move)
	{
		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}


		ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
		move -= ob->distance;

		SelectPathDir (ob);

		if ( !(--ob->temp1) )
		{
			NewState (ob,&s_bjjump1);
			return;
		}
	}
}


/*
===============
=
= T_BJJump
=
===============
*/

void T_BJJump (objtype *ob)
{
	int32_t 	move;

	move = BJJUMPSPEED*tics;
	MoveObj (ob,move);
}


/*
===============
=
= T_BJYell
=
===============
*/

void T_BJYell (objtype *ob)
{
	PlaySoundLocActor(YEAHSND,ob);	// JAB
}


/*
===============
=
= T_BJDone
=
===============
*/

#pragma argsused
void T_BJDone (objtype *ob)
{
	playstate = ex_victorious;				// exit castle tile
}



//===========================================================================


/*
===============
=
= CheckPosition
=
===============
*/

_boolean	CheckPosition (objtype *ob)
{
	int16_t	x,y,xl,yl,xh,yh;
	objtype *check;

	xl = (ob->x-PLAYERSIZE) >>TILESHIFT;
	yl = (ob->y-PLAYERSIZE) >>TILESHIFT;

	xh = (ob->x+PLAYERSIZE) >>TILESHIFT;
	yh = (ob->y+PLAYERSIZE) >>TILESHIFT;

	//
	// check for solid walls
	//
	for (y=yl;y<=yh;y++)
		for (x=xl;x<=xh;x++)
		{
			check = actorat[x][y];
			if (check && check<objlist)
				return _false;
		}

	return _true;
}


/*
===============
=
= A_StartDeathCam
=
===============
*/

void	A_StartDeathCam (objtype *ob)
{
	int32_t	dx,dy;
	float	fangle;
	int32_t    xmove,ymove;
	int32_t	dist;
	int16_t		temp,i;

	FinishPaletteShifts ();

	VW_WaitVBL (100);

	if (gamestate.victoryflag)
	{
		playstate = ex_victorious;				// exit castle tile
		return;
	}

	gamestate.victoryflag = _true;
	VW_Bar (0,0,320,200-STATUSLINES,127);
	FizzleFade(bufferofs,displayofs,320,200-STATUSLINES,70,_false);

	PM_UnlockMainMem ();
	CA_UpLevel ();
	CacheLump(LEVELEND_LUMP_START,LEVELEND_LUMP_END);
	#ifdef JAPAN
	#ifndef JAPDEMO
	CA_CacheScreen(C_LETSSEEPIC);
	#endif
	#else
	Write(0,7,(char*)STR_SEEAGAIN);
	#endif
	CA_DownLevel ();
	PM_CheckMainMem ();

	VW_UpdateScreen ();

	IN_UserInput(300);

//
// line angle up exactly
//
	NewState (player,&s_deathcam);

	player->x = gamestate.killx;
	player->y = gamestate.killy;

	dx = ob->x - player->x;
	dy = player->y - ob->y;

	fangle = (float)atan2(dy,dx);			// returns -pi to pi
	if (fangle<0)
		fangle = (float)(M_PI*2+fangle);

	player->angle = (int16_t)(fangle/(M_PI*2)*ANGLES);

//
// try to position as close as possible without being in a wall
//
	dist = 0x14000l;
	do
	{
		xmove = FixedByFrac(dist,costable[player->angle]);
		ymove = -FixedByFrac(dist,sintable[player->angle]);

		player->x = ob->x - xmove;
		player->y = ob->y - ymove;
		dist += 0x1000;

	} while (!CheckPosition (player));
	plux = player->x >> UNSIGNEDSHIFT;			// scale to fit in uint16_t
	pluy = player->y >> UNSIGNEDSHIFT;
	player->tilex = player->x >> TILESHIFT;		// scale to tile values
	player->tiley = player->y >> TILESHIFT;

//
// go back to the game
//
	temp = bufferofs;
	for (i=0;i<3;i++)
	{
		bufferofs = screenloc[i];
		DrawPlayBorder ();
	}
	bufferofs = temp;

	fizzlein = _true;
	switch (ob->obclass)
	{
        default:break;
#ifndef SPEAR
	case schabbobj:
		NewState (ob,&s_schabbdeathcam);
		break;
	case realhitlerobj:
		NewState (ob,&s_hitlerdeathcam);
		break;
	case giftobj:
		NewState (ob,&s_giftdeathcam);
		break;
	case fatobj:
		NewState (ob,&s_fatdeathcam);
		break;
#endif
	}

}

#endif

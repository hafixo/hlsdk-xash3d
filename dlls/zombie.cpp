/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
//=========================================================
// Zombie
//=========================================================

// UNDONE: Don't flinch every time you get hit

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"

//
// Spawn Flags
//
#define SF_ZOMBIE_FASTMODE	1024

#define ZOMBIE_WORD_LENGTH	6

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define	ZOMBIE_AE_ATTACK_RIGHT		0x01
#define	ZOMBIE_AE_ATTACK_LEFT		0x02
#define	ZOMBIE_AE_ATTACK_BOTH		0x03

#define ZOMBIE_FLINCH_DELAY		2		// at most one flinch every n secs

class CZombie : public CBaseMonster
{
public:
	void Spawn();
	void Precache();
	void SetYawSpeed();
	int Classify();
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	int IgnoreConditions();

	float m_flNextFlinch;

	void PainSound();
	void AlertSound();
	void IdleSound();
	void AttackSound();

	static const char *pAttackSounds[];
	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];

	string_t szCustomAttackSounds[2];
	string_t szCustomIdleSounds[4];
	string_t szCustomAlertSounds[3];
	string_t szCustomPainSounds[2];

	// No range attacks
	BOOL CheckRangeAttack1( float flDot, float flDist ) { return FALSE; }
	BOOL CheckRangeAttack2( float flDot, float flDist ) { return FALSE; }
	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );

	void RunAI();
};

LINK_ENTITY_TO_CLASS( monster_zombie, CZombie )

const char *CZombie::pAttackHitSounds[] =
{
	"zombie/claw_strike1.wav",
	"zombie/claw_strike2.wav",
	"zombie/claw_strike3.wav",
};

const char *CZombie::pAttackMissSounds[] =
{
	"zombie/claw_miss1.wav",
	"zombie/claw_miss2.wav",
};

const char *CZombie::pAttackSounds[] =
{
	"zombie/zo_attack1.wav",
	"zombie/zo_attack2.wav",
};

const char *CZombie::pIdleSounds[] =
{
	"zombie/zo_idle1.wav",
	"zombie/zo_idle2.wav",
	"zombie/zo_idle3.wav",
	"zombie/zo_idle4.wav",
};

const char *CZombie::pAlertSounds[] =
{
	"zombie/zo_alert10.wav",
	"zombie/zo_alert20.wav",
	"zombie/zo_alert30.wav",
};

const char *CZombie::pPainSounds[] =
{
	"zombie/zo_pain1.wav",
	"zombie/zo_pain2.wav",
};

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int CZombie::Classify( void )
{
	return	CLASS_ALIEN_MONSTER;
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CZombie::SetYawSpeed( void )
{
	int ys;

	ys = 120;
#if 0
	switch ( m_Activity )
	{
	}
#endif
	pev->yaw_speed = ys;
}

int CZombie::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	// Take 30% damage from bullets
	if( bitsDamageType == DMG_BULLET )
	{
		Vector vecDir = pev->origin - (pevInflictor->absmin + pevInflictor->absmax) * 0.5;
		vecDir = vecDir.Normalize();
		float flForce = DamageForce( flDamage );
		pev->velocity = pev->velocity + vecDir * flForce;
		flDamage *= 0.3;
	}

	// HACK HACK -- until we fix this.
	if( IsAlive() )
		PainSound();
	return CBaseMonster::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

void CZombie::PainSound( void )
{
	if( RANDOM_LONG( 0, 5 ) < 2 )
	{
		const char *pszSound;
		int pitch = 95 + RANDOM_LONG( 0, 9 );
		int iRand = RANDOM_LONG( 0, ARRAYSIZE( pPainSounds ) - 1 );

		if( pev->message )
			pszSound = STRING( szCustomPainSounds[iRand] );
		else
			pszSound = pPainSounds[iRand];

		EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, pszSound, 1.0, ATTN_NORM, 0, pitch );
	}
}

void CZombie::AlertSound( void )
{
	const char *pszSound;
	int pitch = 95 + RANDOM_LONG( 0, 9 );
	int iRand = RANDOM_LONG( 0, ARRAYSIZE( pAlertSounds ) - 1 );

	if( pev->message )
		pszSound = STRING( szCustomAlertSounds[iRand] );
	else
		pszSound = pAlertSounds[iRand];

	EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, pszSound, 1.0, ATTN_NORM, 0, pitch );
}

void CZombie::IdleSound( void )
{
	const char *pszSound;
	int pitch = 95 + RANDOM_LONG( 0, 9 );
	int iRand = RANDOM_LONG( 0, ARRAYSIZE( pIdleSounds ) - 1 );

	// Play a random idle sound
	if( pev->message )
		pszSound = STRING( szCustomIdleSounds[iRand] );
	else
		pszSound = pIdleSounds[iRand];

	EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, pszSound, 1.0, ATTN_NORM, 0, pitch );
}

void CZombie::AttackSound( void )
{
	const char *pszSound;
	int pitch = 100 + RANDOM_LONG( -5, 5 );
	int iRand = RANDOM_LONG( 0, ARRAYSIZE( pAttackSounds ) - 1 );

	// Play a random attack sound
	if( pev->message )
		pszSound = STRING( szCustomAttackSounds[iRand] );
	else
		pszSound = pAttackSounds[iRand];

	EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, pszSound, 1.0, ATTN_NORM, 0, pitch );
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CZombie::HandleAnimEvent( MonsterEvent_t *pEvent )
{
	switch( pEvent->event )
	{
		case ZOMBIE_AE_ATTACK_RIGHT:
		{
			// do stuff for this event.
			//ALERT( at_console, "Slash right!\n" );
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, gSkillData.zombieDmgOneSlash, DMG_SLASH );
			if( pHurt )
			{
				if( pHurt->pev->flags & ( FL_MONSTER | FL_CLIENT ) )
				{
					pHurt->pev->punchangle.z = -18;
					pHurt->pev->punchangle.x = 5;
					pHurt->pev->velocity = pHurt->pev->velocity - gpGlobals->v_right * 100;
				}
				// Play a random attack hit sound
				EMIT_SOUND_DYN( ENT( pev ), CHAN_WEAPON, pAttackHitSounds[RANDOM_LONG( 0, ARRAYSIZE( pAttackHitSounds ) - 1 )], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG( -5 , 5 ) );
			}
			else // Play a random attack miss sound
				EMIT_SOUND_DYN( ENT( pev ), CHAN_WEAPON, pAttackMissSounds[RANDOM_LONG( 0, ARRAYSIZE( pAttackMissSounds ) - 1 )], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG( -5, 5 ) );

			if( RANDOM_LONG( 0, 1 ) )
				AttackSound();
		}
		break;
		case ZOMBIE_AE_ATTACK_LEFT:
		{
			// do stuff for this event.
			//ALERT( at_console, "Slash left!\n" );
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, gSkillData.zombieDmgOneSlash, DMG_SLASH );
			if( pHurt )
			{
				if( pHurt->pev->flags & ( FL_MONSTER | FL_CLIENT ) )
				{
					pHurt->pev->punchangle.z = 18;
					pHurt->pev->punchangle.x = 5;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * 100;
				}
				EMIT_SOUND_DYN( ENT( pev ), CHAN_WEAPON, pAttackHitSounds[RANDOM_LONG( 0, ARRAYSIZE( pAttackHitSounds ) - 1 )], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG( -5, 5 ) );
			}
			else
				EMIT_SOUND_DYN( ENT( pev ), CHAN_WEAPON, pAttackMissSounds[RANDOM_LONG( 0, ARRAYSIZE( pAttackMissSounds ) - 1 )], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG( -5, 5 ) );

			if( RANDOM_LONG( 0, 1 ) )
				AttackSound();
		}
		break;
		case ZOMBIE_AE_ATTACK_BOTH:
		{
			// do stuff for this event.
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, gSkillData.zombieDmgBothSlash, DMG_SLASH );
			if( pHurt )
			{
				if( pHurt->pev->flags & ( FL_MONSTER | FL_CLIENT ) )
				{
					pHurt->pev->punchangle.x = 5;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * -100;
				}
				EMIT_SOUND_DYN( ENT( pev ), CHAN_WEAPON, pAttackHitSounds[RANDOM_LONG( 0, ARRAYSIZE( pAttackHitSounds ) - 1 )], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG( -5, 5 ) );
			}
			else
				EMIT_SOUND_DYN( ENT( pev ), CHAN_WEAPON, pAttackMissSounds[RANDOM_LONG( 0, ARRAYSIZE( pAttackMissSounds ) - 1 )], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG( -5, 5 ) );

			if( RANDOM_LONG( 0, 1 ) )
				AttackSound();
		}
		break;
		default:
			CBaseMonster::HandleAnimEvent( pEvent );
			break;
	}
}

//=========================================================
// Spawn
//=========================================================
void CZombie::Spawn()
{
	Precache();

	if( !pev->model )
	{
		pev->model = MAKE_STRING( "models/zombie.mdl" );
	}

	SET_MODEL( ENT( pev ), STRING( pev->model ) );
	UTIL_SetSize( pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	pev->solid		= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= BLOOD_COLOR_RED;
	if( !pev->health )
		pev->health	= gSkillData.zombieHealth;
	pev->view_ofs		= VEC_VIEW;// position of the eyes relative to monster's origin.
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;
	m_afCapability		= bits_CAP_DOORS_GROUP;

	MonsterInit();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CZombie::Precache()
{
	size_t i;

	// Mapper can customize zombie's models and sounds
	PRECACHE_MODEL( pev->model ? STRING( pev->model ) : "models/zombie.mdl" );

	for( i = 0; i < ARRAYSIZE( pAttackHitSounds ); i++ )
		PRECACHE_SOUND( pAttackHitSounds[i] );

	for( i = 0; i < ARRAYSIZE( pAttackMissSounds ); i++ )
		PRECACHE_SOUND( pAttackMissSounds[i] );

	if( pev->message )
	{
		char szSound[32];

		strcpy( szSound, STRING( pev->message ) );

		for( i = 0; i < ARRAYSIZE( pAttackSounds ); i++ )
		{
			strcat( szSound, pAttackSounds[i] + ZOMBIE_WORD_LENGTH );
			szCustomAttackSounds[i] = ALLOC_STRING( szSound );
			PRECACHE_SOUND( STRING( szCustomAttackSounds[i] ) );
			szSound[strlen( STRING( pev->message ) )] = 0;
		}

		for( i = 0; i < ARRAYSIZE( pIdleSounds ); i++ )
		{
			strcat( szSound, pIdleSounds[i] + ZOMBIE_WORD_LENGTH );
			szCustomIdleSounds[i] = ALLOC_STRING( szSound );
			PRECACHE_SOUND( STRING( szCustomIdleSounds[i] ) );
			szSound[strlen( STRING( pev->message ) )] = 0;
		}

		for( i = 0; i < ARRAYSIZE( pAlertSounds ); i++ )
		{
			strcat( szSound, pAlertSounds[i] + ZOMBIE_WORD_LENGTH );
			szCustomAlertSounds[i] = ALLOC_STRING( szSound );
			PRECACHE_SOUND( STRING( szCustomAlertSounds[i] ) );
			szSound[strlen( STRING( pev->message ) )] = 0;
		}

		for( i = 0; i < ARRAYSIZE( pPainSounds ); i++ )
		{
			strcat( szSound, pPainSounds[i] + ZOMBIE_WORD_LENGTH );
			szCustomPainSounds[i] = ALLOC_STRING( szSound );
			PRECACHE_SOUND( STRING( szCustomPainSounds[i] ) );
			szSound[strlen( STRING( pev->message ) )] = 0;
		}
	}
	else
	{
		for( i = 0; i < ARRAYSIZE( pAttackSounds ); i++ )
			PRECACHE_SOUND( pAttackSounds[i] );

		for( i = 0; i < ARRAYSIZE( pIdleSounds ); i++ )
			PRECACHE_SOUND( pIdleSounds[i] );

		for( i = 0; i < ARRAYSIZE( pAlertSounds ); i++ )
			PRECACHE_SOUND( pAlertSounds[i] );

		for( i = 0; i < ARRAYSIZE( pPainSounds ); i++ )
			PRECACHE_SOUND( pPainSounds[i] );
	}
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
int CZombie::IgnoreConditions( void )
{
	int iIgnore = CBaseMonster::IgnoreConditions();

	if( ( m_Activity == ACT_MELEE_ATTACK1 ) || ( m_Activity == ACT_MELEE_ATTACK1 ) )
	{
#if 0
		if( pev->health < 20 )
			iIgnore |= ( bits_COND_LIGHT_DAMAGE| bits_COND_HEAVY_DAMAGE );
		else
#endif
		if( m_flNextFlinch >= gpGlobals->time )
			iIgnore |= ( bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE );
	}

	if( ( m_Activity == ACT_SMALL_FLINCH ) || ( m_Activity == ACT_BIG_FLINCH ) )
	{
		if( m_flNextFlinch < gpGlobals->time )
			m_flNextFlinch = gpGlobals->time + ZOMBIE_FLINCH_DELAY;
	}

	return iIgnore;
}

//========================================================
// RunAI - overridden for zombie because there are things
// that need to be checked every think.
//========================================================
void CZombie::RunAI( void )
{
	// first, do base class stuff
	CBaseMonster::RunAI();

	if( pev->spawnflags & SF_ZOMBIE_FASTMODE )
	{
		if( pev->gaitsequence == ACT_WALK )
		{
			pev->framerate = 1.5;
		}
	}
}

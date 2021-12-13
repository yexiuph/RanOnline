#pragma once

#include "./GLCharDefine.h"
#include "./GLCharData.h"

class GLARoundSlot;
struct NET_MSG_GENERIC;
struct SSTATEBLOW;

class GLACTOR
{
public:
	virtual EMCROW GetCrow () const = 0;
	virtual DWORD GetCtrlID () const = 0;
	virtual DWORD GetCharID () const { return 0; }
	virtual WORD GetSchool () const { return 0; }

	virtual DWORD GetPartyID () const = 0;

	virtual BOOL IsAction ( EMACTIONTYPE emCur ) const = 0;
	virtual BOOL IsValidBody () const = 0;
	virtual BOOL IsRunning () const = 0;
	virtual BOOL IsSafeTime () const { return FALSE; }
	virtual bool IsSafeZone () const { return false; }

	virtual const SRESIST& GETRESIST () const = 0;
	virtual DWORD GETHOLDBLOW () const = 0;

	virtual const D3DXVECTOR3& GetPosition () const = 0;
	virtual WORD GetBodyRadius () const = 0;

	virtual WORD	GetLevel () const = 0;

	virtual DWORD	GetNowHP () const = 0;
	virtual DWORD	GetMaxHP () const = 0;

	virtual WORD	GetNowMP () const = 0;
	virtual WORD	GetMaxMP () const = 0;

	virtual WORD	GetNowSP () const = 0;
	virtual WORD	GetMaxSP () const = 0;

	virtual int		GetDefense () const = 0;
	virtual int		GetBodyDefense () const		{ return 0; }
	virtual int		GetItemDefense () const		{ return 0; }

	virtual DAMAGE_SPEC GetDamageSpec() const	=  0;
	virtual DEFENSE_SKILL GetDefenseSkill() const = 0;

	virtual DWORD	GetBonusExp () const = 0;

	virtual int GetAvoid () const = 0;
	virtual EMBRIGHT GetBright () const = 0;

	virtual GLARoundSlot* GETSLOT ()			{ return NULL; }
	virtual SCONFTING* GETCONFTING ()			{ return NULL; }
	virtual DWORD GETCLIENTID ()				{ return 0; }

	virtual DWORD ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const BOOL bShock ) = 0;
	virtual void ReceivePushPull ( const D3DXVECTOR3 &vMovePos ) = 0;

	virtual void DamageReflectionProc ( int nDAMAGE, STARGETID sACTOR ) = 0;
	virtual void DefenseSkill( SNATIVEID sNativeID, WORD wLevel, STARGETID sACTOR ) = 0;

	virtual void STATEBLOW ( const SSTATEBLOW &sStateBlow ) = 0;
	virtual float GETSTATE_DAMAGE () const = 0;

	virtual void CURE_STATEBLOW ( DWORD dwCUREFLAG ) = 0;
	virtual void BUFF_REMOVE( DWORD dwBuffFlag ) = 0;

	virtual void VAR_BODY_POINT ( const EMCROW emACrow, const DWORD dwAID, const BOOL bPartySkill, int nvar_hp, int nvar_mp, int nvar_sp ) = 0;
	virtual BOOL RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, DWORD &dwSELECT ) = 0;
	virtual HRESULT SNDMSGAROUND ( NET_MSG_GENERIC *pnet ) = 0;

public:
	GLACTOR ()		{}
	~GLACTOR ()		{}
};
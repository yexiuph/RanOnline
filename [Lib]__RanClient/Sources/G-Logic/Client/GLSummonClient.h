#pragma once

#include "GLSummon.h"
#include "GLCOPY.h"

#include "../[Lib]__Engine/Sources/NaviMesh/Actor.h"
#include "DxSkinChar.h"
#include "GLCharacter.h"

#define _SKILLFACT_DEFAULTTIME		(-1.0f)

class GLSummonClient : public GLSUMMON, public GLCOPY
{
protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	Actor				m_actorMove;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vServerPos;
	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;

	GLCharacter*		m_pOwner;

	bool				m_bValid;
	bool				m_bSkillProcessing;

	STARGETID			m_sSkillTARGET;
	STARGETID			m_sTargetID;

	DWORD				m_dwActionFlag;
	EMACTIONTYPE		m_Action;				//	행동.

	float				m_fIdleTimer;
	float				m_fAttackTimer;
	WORD				m_wAniSub[3];

	SCROWATTACK*		m_pAttackProp;			//	공격 속성.

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	float				m_fHeight;

	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

	SNATIVEID			m_sSummonCrowID;

protected:
	float m_fattTIMER;
	int   m_nattSTEP;

protected:
	void StartAttackProc ();
	BOOL AttackProc ( float fElapsedTime );
	void AttackEffect ( const SANIMSTRIKE &sStrikeEff );

public:
	GLSummonClient(void);
	~GLSummonClient(void);

	DxSkinChar* GetSkinChar ()							{ return m_pSkinChar; }

	void SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget );
	void SKT_EFF_HOLDOUT ( STARGETID sTarget , DWORD dwDamageFlag );
	BOOL RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, const DWORD dwSELECT, float fAge = _SKILLFACT_DEFAULTTIME );

	BOOL IsVALID () { return m_bValid; }

	void TurnAction ( EMACTIONTYPE toAction );

	BOOL IsSTATE ( DWORD dwStateFlag )		 			{ return m_dwActionFlag&dwStateFlag; }	
	void SetSTATE ( DWORD dwStateFlag );
	void ReSetSTATE ( DWORD dwStateFlag )				{ m_dwActionFlag &= ~dwStateFlag; }
	void ReSetAllSTATE ()								{ m_dwActionFlag = 0; }

	HRESULT UpateAnimation ( float fTime, float fElapsedTime );

	void SetMoveState ( BOOL bRun );

	BOOL IsValidBody ();

	D3DXVECTOR3 GetRandomOwnerTarget ();

	float GetDirection ();

	HRESULT Create ( const PGLSUMMON pSummonData, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, NavigationMesh* pNavi, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UpdateAnimation ( float fTime, float fElapsedTime );
	HRESULT	UpdateSummonState ( float fTime, float fElapsedTime );
	bool	UpdateSummonPos( bool bTargetMove );

	// Default FrameWork
	HRESULT FrameMove( float fTime, float fElapsedTime );
	HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

	virtual BOOL IsACTION ( EMACTIONTYPE emCur )	{ return m_Action == emCur; }

	virtual WORD GetBodyRadius ();
	virtual float GetBodyHeight()			{ return m_fHeight; }
	virtual D3DXVECTOR3 GetPosBodyHeight ()	{ return D3DXVECTOR3( m_vPos.x, m_vPos.y+m_fHeight, m_vPos.z ); }

	virtual EMCROW GetCrow () const			{ return CROW_SUMMON; }
	virtual DWORD GetCtrlID () const		{ return m_dwGUID; }

	virtual const char* GetName () const	{ return m_pSummonCrowData->GetName(); }
	virtual GLDWDATA GetHp () const			{ return GLDWDATA(GETHP(),GETMAXHP()); }


	virtual void ReceiveDamage ( WORD wDamage, DWORD dwDamageFlag, STARGETID sACTOR );
	virtual void ReceiveAVoid ();
	virtual void ReceiveSwing ();

	virtual const D3DXVECTOR3 &GetPosition ()		{ return m_vPos; }

	BOOL	IsCollisionVolume ();	

	void	ReqSummonSetPosion();
	void	ReqPosionRemove();

protected:
	void MakeAniSubKey ( float fTime );

public:
	void	ReqGoto ( D3DXVECTOR3 vTarPos, bool bTracing );
	void	ReqStop ( bool bStopAttack = false );

	void	MsgProcess ( NET_MSG_GENERIC* nmg );
	void	MsgAttack ( NET_MSG_GENERIC* nmg );
	void	MsgSkillFact ( NET_MSG_GENERIC* nmg );

	void	MsgSlotExHold ( NET_MSG_GENERIC* nmg );
	void	MsgHoldToSlot ( NET_MSG_GENERIC* nmg );
	void	MsgRemoveSlotItem ( NET_MSG_GENERIC* nmg );
	void	MsgHpUpdate ( NET_MSG_GENERIC* nmg );

};

typedef GLSummonClient* PGLSUMMONCLIENT;


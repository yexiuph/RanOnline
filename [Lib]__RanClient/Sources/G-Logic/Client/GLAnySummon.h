#pragma once

#include "GLSummon.h"

#include "../[Lib]__Engine/Sources/NaviMesh/Actor.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLList.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLQuadTree.h"

#include "DxSkinChar.h"
#include "GLSkill.h"

#define _SKILLFACT_DEFAULTTIME		(-1.0f)

class GLAnySummon : public GLSUMMON, public GLCOPY
{

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	Actor				m_actorMove;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;	

	D3DXVECTOR3			m_vServerPos;

	D3DXVECTOR3			m_vTarPos;

	STARGETID			m_sTargetID;

public:
	SGLNODE<GLAnySummon*>*	m_pGlobNode;		//	맵 리스트 노드.
	CLIENTQUADNODE*			m_pQuadNode;		//	쿼드 트리 노드.
	SGLNODE<GLAnySummon*>*	m_pCellNode;		//	셀 리스트 노드.

	DWORD				m_dwActionFlag;
	EMACTIONTYPE		m_Action;				//	행동.
	WORD				m_wAniSub;

	SCROWATTACK*		m_pAttackProp;			//	공격 속성.

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	float				m_fHeight;

	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

protected:
	float m_fattTIMER;
	int   m_nattSTEP;

protected:
	void StartAttackProc ();
	BOOL AttackProc ( float fElapsedTime );
	void AttackEffect ( const SANIMSTRIKE &sStrikeEff );
	HRESULT UpdateAnimation ( float fTime, float fElapsedTime );

public:
	GLAnySummon(void);
	~GLAnySummon(void);

	void ASSIGN ( PSDROPSUMMON pDropSummon );

	DxSkinChar* GetSkinChar ()							{ return m_pSkinChar; }

	BOOL IsValidBody ();

	float GetDirection ();
	
	HRESULT Create ( PSDROPSUMMON pDropSummon, NavigationMesh* pNavi, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UpateAnimation ( float fTime, float fElapsedTime );
	HRESULT	UpdateTurnAction ( float fTime, float fElapsedTime );

	void TurnAction ( EMACTIONTYPE toAction );


	BOOL IsSTATE ( DWORD dwStateFlag )		 			{ return m_dwActionFlag&dwStateFlag; }
	void SetSTATE ( DWORD dwStateFlag );
	void ReSetSTATE ( DWORD dwStateFlag )				{ m_dwActionFlag &= ~dwStateFlag; }

	void MsgGoto ( NET_MSG_GENERIC* nmg );
	void MsgStop ( NET_MSG_GENERIC* nmg );
	void MsgUpdateMoveState ( NET_MSG_GENERIC* nmg );
	void MsgSkillFact ( NET_MSG_GENERIC* nmg );
	void MsgHpUpdate ( NET_MSG_GENERIC* nmg );
	void MsgAttack ( NET_MSG_GENERIC* nmg );
	
	// Default FrameWork
	HRESULT FrameMove( float fTime, float fElapsedTime );
	HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	HRESULT RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

	void SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget );
	void SKT_EFF_HOLDOUT ( STARGETID sTarget , DWORD dwDamageFlag );
	BOOL RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, const DWORD dwSELECT, float fAge = _SKILLFACT_DEFAULTTIME );

	void MsgProcess ( NET_MSG_GENERIC* nmg );

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

	BOOL	IsCollisionVolume ();	


	virtual const D3DXVECTOR3 &GetPosition ()		{ return m_vPos; }
};

typedef GLAnySummon* PGLANYSUMMON;
typedef CGLLIST<PGLANYSUMMON> GLANYSUMMONLIST;
typedef SGLNODE<PGLANYSUMMON> GLANYSUMMONNODE;




#ifndef GLCROWCLIENT_H_
#define GLCROWCLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLCrowData.h"
#include "GLCOPY.h"
#include "GLogicEx.h"

#include "../[Lib]__Engine/Sources/NaviMesh/Actor.h"
#include "DxSkinChar.h"

#ifndef GLLandManClient
	class GLLandManClient;
	class GLClientNode;
	typedef SQUADNODE<GLClientNode> CLIENTQUADNODE;
#endif //GLLandMan

#define _SKILLFACT_DEFAULTTIME		(-1.0f)

class GLCrowClient : public GLCROWLOGIC, public GLCOPY
{
protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	Actor				m_actorMove;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;
	float				m_fScale;

	D3DXVECTOR3			m_vServerPos;

	//	Note : AABB
protected:
	D3DXVECTOR3		m_vMaxOrg;
	D3DXVECTOR3		m_vMinOrg;

public:
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
	float			m_fHeight;

	//	Note : 제어 코드.
	//
public:
	DWORD				m_dwGlobID;				//	생성 메모리 인덱스용 ( 해당 GLLandMan내에서 유니크함 )
	DWORD				m_dwCeID;				//	셀 ID.

	GLLandManClient*		m_pLandManClient;	//	맵 인스턴스 포인터.
	SGLNODE<GLCrowClient*>*	m_pGlobNode;		//	맵 리스트 노드.
	CLIENTQUADNODE*			m_pQuadNode;		//	쿼드 트리 노드.
	SGLNODE<GLCrowClient*>*	m_pCellNode;		//	셀 리스트 노드.

	//	Note : 행동.
protected:
	float				m_fAge;					//	경과 시간.
	EMACTIONTYPE		m_Action;				//	행동.
	DWORD				m_dwActState;			//	행동 상태.

	STARGETID			m_sTargetID;			//	목표 유닛.

	WORD				m_wTARNUM;				//	타겟 숫자.
	STARID				m_sTARIDS[EMTARGET_NET];//	타겟들.
	D3DXVECTOR3			m_vTARPOS;				//	타겟 위치.

	SCROWATTACK*		m_pAttackProp;			//	공격 속성.

protected:
	float				m_fIdleTime;			//	휴식 시간.
	float				m_fStayTimer;			//	휴식 딜래이.
	float				m_fAttackTimer;			//	공격 딜래이.
	float				m_fTargetTimer;			//	타갯 검색 타이머.
	float				m_fMoveDelay;			//	이동 딜래이.

public:
	BOOL IsValidBody ();
	BOOL IsDie ();
	BOOL IsHaveVisibleBody ();

	BOOL IsVisibleDetect ();

public:
	BOOL IsSTATE ( DWORD dwState )					{ return m_dwActState&dwState; }
	virtual BOOL IsACTION ( EMACTIONTYPE emCur )	{ return m_Action == emCur; }

protected:
	void SetSTATE ( DWORD dwState )					{ m_dwActState |= dwState; }
	void ReSetSTATE ( DWORD dwState )				{ m_dwActState &= ~dwState; }

public:
	HRESULT CreateCrow ( GLLandManClient *pLandManClient, SDROP_CROW *pCrowDrop, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void SetPosition ( D3DXVECTOR3 &vPos );
	virtual const D3DXVECTOR3 &GetPosition ()		{ return m_vPos; }
	float GetDirection ();
	DxSkinChar* GetCharSkin ()						{ return m_pSkinChar; }

public:
	float GetMoveVelo ();
	void TurnAction ( EMACTIONTYPE toAction );

public:
	virtual EMCROW GetCrow () const			{ return GETCROW(); }
	virtual DWORD GetCtrlID () const		{ return m_dwGlobID; }
	virtual const char* GetName () const	{ return m_pCrowData->GetName(); }
	virtual GLDWDATA GetHp () const			{ return GLDWDATA(m_dwNowHP,m_pCrowData->m_dwHP); }

	virtual WORD GetBodyRadius ();
	virtual float GetBodyHeight()			{ return m_fHeight; }
	virtual D3DXVECTOR3 GetPosBodyHeight ()	{ return D3DXVECTOR3( m_vPos.x, m_vPos.y+m_fHeight, m_vPos.z ); }

public:
	virtual void MsgProcess ( NET_MSG_GENERIC* nmg );
	virtual void ReceiveDamage ( WORD wDamage, DWORD dwDamageFlag, STARGETID sACTOR );
	virtual void ReceiveAVoid ();
	virtual void ReceiveSwing ();

protected:
	HRESULT UpateAnimation ( float fTime, float fElapsedTime );
	
	BOOL RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, const DWORD dwSELECT, float fAge = _SKILLFACT_DEFAULTTIME );

protected:
	float m_fattTIMER;
	int m_nattSTEP;

protected:
	void StartAttackProc ();
	BOOL AttackProc ( float fElapsedTime );
	void AttackEffect ( const SANIMSTRIKE &sStrikeEff );

protected:
	void StartSkillProc ();
	BOOL SkillProc ( float fElapsedTime );

	void SKT_EFF_HOLDOUT ( STARGETID sTarget, DWORD dwDamageFlag );

	void SK_EFF_TARG ( const PGLSKILL pSkill, const SANIMSTRIKE &sStrikeEff, const STARGETID &sTarget );
	void SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget=NULL );
	void SK_EFF_TARZONE ( const STARGETID &sTarget, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF );
	void SkillEffect ( const SANIMSTRIKE &sStrikeEff );

public:
	BOOL IsCollisionVolume ();
	HRESULT FrameMove ( float fTime, float fElapsedTime );

	HRESULT RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );

public:
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();

public:
	GLCrowClient(void);
	~GLCrowClient(void);
};

typedef GLCrowClient* PGLCROWCLIENT;

typedef CGLLIST<PGLCROWCLIENT>	GLCROWCLIENTLIST;
typedef SGLNODE<PGLCROWCLIENT>	GLCROWCLIENTNODE;

#endif // GLCROWCLIENT_H_
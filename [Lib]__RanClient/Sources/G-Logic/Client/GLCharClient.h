#ifndef GLCHARCLIENT_H_
#define GLCHARCLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "./GLCharData.h"
#include "./GLCopy.h"
#include "./GLogicData.h"
#include "./GLPrivateMarket.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSkinChar.h"
#include "../[Lib]__Engine/Sources/NaviMesh/Actor.h"
//#include "./GLLandManClient.h"
//#include "s_CSMsgList.h"
#include "./GLVEHICLE.h"

#ifndef GLLandManClient
	class GLLandManClient;
	class GLClientNode;
	typedef SQUADNODE<GLClientNode> CLIENTQUADNODE;
#endif //GLLandMan

#define _SKILLFACT_DEFAULTTIME		(-1.0f)

class GLCharClient : public GLCOPY
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

	//	Note : 현제 상태.
	//
	float				m_fAge;
	EMACTIONTYPE		m_Action;
	DWORD				m_dwActState;			//	행동 상태.
	D3DXVECTOR3			m_vTarPos;

	float				m_fIdleTime;
	STARGETID			m_sTargetID;			//	목표 유닛.

	float				m_fMoveDelay;			//	이동 딜래이.

	DWORD				m_dwANISUBSELECT;
	DWORD				m_dwANISUBGESTURE;

	//	Note : AABB
	//
	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	float				m_fHeight;

	//	Note : 제어 코드.
	//
public:
	std::string			m_strName;
	EMCHARINDEX			m_CHARINDEX;
	DWORD				m_dwGaeaID;				//	생성 메모리 인덱스용 ( 해당 GLLandMan내에서 유니크함 )
	DWORD				m_dwCeID;				//	셀 ID.

	GLLandManClient*		m_pLandManClient;	//	맵 인스턴스 포인터.
	SGLNODE<GLCharClient*>*	m_pGlobNode;		//	맵 리스트 노드.
	CLIENTQUADNODE*			m_pQuadNode;		//	쿼드 트리 노드.
	SGLNODE<GLCharClient*>*	m_pCellNode;		//	셀 리스트 노드.

	DWORD					m_dwSummonGUID;					//  소환수 ID

	//	Note : 기초 대이터.
	//
protected:
	EMANI_MAINTYPE		m_emANIMAINSKILL;
	EMANI_SUBTYPE		m_emANISUBSKILL;

	EMANI_SUBTYPE		m_emANISUBTYPE;
	DWORD				m_dwRevData;
	SDROP_CHAR			m_CharData;

	SSKILLFACT			m_sSKILLFACT[SKILLFACT_SIZE];	//	스킬효과들.

	SLANDEFFECT			m_sLandEffect[EMLANDEFFECT_MULTI];	//  지형 이상 효과들

	SSTATEBLOW			m_sSTATEBLOWS[EMBLOW_MULTI];	//	상태이상.

	DWORD				m_dwTransfromSkill;					//  변신 스킬 버프 넘버

	float				m_fSTATE_MOVE;					//	상태 이상 이동속도. ( 1.0f == 100% )
	float				m_fSTATE_DELAY;					//	상태 이상 딜래이. ( 1.0f == 100% ) (+)

	bool				m_bSTATE_PANT;					//	상태이상 헐떡임.
	bool				m_bSTATE_STUN;					//	상태이상 기절.
	bool				m_bINVISIBLE;					//	투명.

	float				m_fSKILL_MOVE;					//	스킬 이동 속도.
	float				m_fATTVELO;						//	스킬 공격속도 변화.
	float				m_fITEM_MOVE_R;					//  아이템 이동 속도 비율	
	float				m_fITEMATTVELO_R;				//  아이템 공격속도 비율
	float				m_fITEM_MOVE;					//  아이템 이동 속도 절대치	
	float				m_fITEMATTVELO;				//  아이템 공격속도 절대치

	WORD				m_wTARNUM;
	D3DXVECTOR3			m_vTARPOS;
	STARID				m_sTARIDS[EMTARGET_NET];

	WORD				m_wACTIVESKILL_LEVEL;
	SNATIVEID			m_idACTIVESKILL;

protected:
	float				m_fattTIMER;
	int					m_nattSTEP;

public:
	GLPrivateMarket		m_sPMarket;						//	개인 상점.
	BOOL				m_bVehicle;
	GLVEHICLE			m_sVehicle;

	bool				m_bItemShopOpen;
	bool				m_bSafeZone;

public:
	virtual WORD GetBodyRadius ();

public:
	virtual EMCROW GetCrow () const			{ return CROW_PC; }
	virtual DWORD GetCtrlID () const		{ return m_dwGaeaID; }
	virtual DWORD GetCharID () const		{ return m_CharData.dwCharID; }
	virtual const char* GetName () const	{ return m_strName.c_str(); }
	virtual GLDWDATA GetHp () const			{ return m_CharData.sHP; }

	virtual void ReceiveDamage ( WORD wDamage, DWORD dwDamageFlag , STARGETID sACTOR );
	virtual void ReceiveAVoid ();
	virtual void ReceiveSwing ();
	virtual bool IsSafeZone () const		{ return m_bSafeZone; }

	const SITEMCLIENT& GET_SLOT_ITEM ( EMSLOT _slot )				{ return m_CharData.m_PutOnItems[_slot]; }
	BOOL VALID_SLOT_ITEM ( EMSLOT _slot );

	SITEM* GET_SLOT_ITEMDATA ( EMSLOT _slot );

	EMELEMENT GET_ITEM_ELMT ();

	SSKILLFACT* GET_SKILLFACT ( int nINDEX )						{ if ( nINDEX >=SKILLFACT_SIZE ) return NULL; return &m_sSKILLFACT[nINDEX]; }

	DWORD GETCLUBID ()												{ return m_CharData.dwGuild; }
	EMCROW GETCROW()												{ return CROW_PC; }
	DWORD GETPARTYID()												{ return m_CharData.dwParty; }
	DWORD GETALLIANCEID()											{ return m_CharData.dwAlliance; }

public:
	D3DXMATRIX GetMatrix ()											{ return m_matTrans; }

public:
	BOOL IsSTATE ( DWORD dwState )					{ return m_dwActState&dwState; }
	virtual BOOL IsACTION ( EMACTIONTYPE emCur )	{ return m_Action == emCur; }
	
	virtual BOOL IsPartyMem ()						{ return m_CharData.dwParty!=GAEAID_NULL; }
	virtual BOOL IsPartyMaster ()					{ return m_CharData.dwPMasterID==m_CharData.dwGaeaID; }
	virtual BOOL IsClubMaster ()					{ return m_CharData.dwGuildMaster==m_CharData.dwCharID; }
	virtual BOOL IsAllianceMasterClub()				{ return m_CharData.dwGuild == m_CharData.dwAlliance ; }
	virtual BOOL IsAllianceMaster()					{ return IsClubMaster() && IsAllianceMasterClub(); }
	virtual const char* GetClubName()				{ return m_CharData.szClubName; }
	BOOL IsCDCertify ()								{ return m_CharData.dwFLAGS&SDROP_CHAR::CLUB_CD; }

	BOOL IsValidBody ();
	BOOL IsDie ();

	BOOL IsVisibleDetect ();

	void UpdateSpecialSkill();

protected:
	void SetSTATE ( DWORD dwState )			{ m_dwActState |= dwState; }
	void ReSetSTATE ( DWORD dwState )		{ m_dwActState &= ~dwState; }

protected:
	float GetMoveVelo ();
	void TurnAction ( EMACTIONTYPE toAction );

public:
	void SetPosition ( const D3DXVECTOR3 &vPos );
	
	const D3DXVECTOR3 &GetPosition ()	{ return m_vPos; }
	float GetDirection ();

	WORD GETBODYRADIUS ()				{ return GLCONST_CHAR::wBODYRADIUS; }
	SDROP_CHAR& GetCharData ()			{ return m_CharData; }
	DxSkinChar* GetCharSkin ()			{ return m_pSkinChar; }

	virtual float GetBodyHeight()			{ return m_fHeight; }
	virtual D3DXVECTOR3 GetPosBodyHeight ();

	void SetUseArmSub( BOOL bSub )		{ m_CharData.m_bUseArmSub = bSub; }
	BOOL IsUseArmSub() const			{ return m_CharData.m_bUseArmSub; }

	EMSLOT GetCurRHand();
	EMSLOT GetCurLHand();

	BOOL IsCurUseArm( EMSLOT emSlot );

public:
	void DISABLESKEFF ( int i )					{ m_sSKILLFACT[i].sNATIVEID = NATIVEID_NULL(); }
	void DISABLEBLOW ( int i )					{ m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE; }
	void UPDATE_DATA ( float fTime, float fElapsedTime, BOOL bClient=FALSE );
	void UPDATE_ITEM ( );
	void DISABLEALLLANDEFF();
	void ADDLANDEFF( SLANDEFFECT landEffect, int iNum );

	// 지형 효과 업데이트
	void UpdateLandEffect();

public:
	float GETATTVELO ();
	float GETMOVEVELO ();
	float GETATT_ITEM();
	float GETMOVE_ITEM();

public:
	HRESULT Create ( GLLandManClient* pLandManClient, SDROP_CHAR* pCharData, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UpdateSuit ();

	BOOL RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, const DWORD dwSELECT, float fAge = _SKILLFACT_DEFAULTTIME );

protected:
	void ReSelectAnimation ();
	HRESULT UpateAnimation ( float fTime, float fElapsedTime );

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
		DWORD GET_PK_COLOR ();

public:
	BOOL IsCollisionVolume ();
	HRESULT FrameMove ( float fTime, float fElapsedTime );

	HRESULT RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );

	void	DisableSkillFact();

public:
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();

public:
	HRESULT MsgMoveState ( NET_MSG_GENERIC* nmg );
	HRESULT MsgGoto ( NET_MSG_GENERIC* nmg );

public:
	virtual void MsgProcess ( NET_MSG_GENERIC* nmg );

public:
	GLCharClient(void);
	~GLCharClient(void);
};

typedef GLCharClient* PGLCHARCLIENT;

typedef CGLLIST<PGLCHARCLIENT>	GLCHARCLIENTLIST;
typedef SGLNODE<PGLCHARCLIENT>	GLCHARCLIENTNODE;

#endif // GLCHARCLIENT_H_
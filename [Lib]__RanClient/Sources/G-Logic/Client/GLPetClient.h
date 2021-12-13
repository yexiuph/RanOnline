#pragma once

#include "GLPet.h"
#include "../[Lib]__Engine/Sources/NaviMesh/Actor.h"
#include "DxSkinChar.h"
#include "GLCharacter.h"

enum EM_FIELDITEM_STATE
{
	EM_FIELDITEM_STATE_REMOVED     = 0, // 제거됐음
	EM_FIELDITEM_STATE_INSERTOK    = 1, // 인벤에 삽입가능
	EM_FIELDITEM_STATE_INSERTFAIL  = 2, // 인벤에 삽입불가
	EM_FIELDITEM_STATE_GENERALFAIL  = 3, // 오류
};


class GLPetClient : public GLPET
{
protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	Actor				m_actorMove;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;

	GLCharacter*		m_pOwner;

	bool				m_bValid;
	bool				m_bSkillProcessing;

	STARGETID			m_sSkillTARGET;

	DWORD				m_dwActionFlag;
	DETECTMAP			m_vecDroppedItems;

	PETDELAY_MAP		m_SKILLDELAY;

	float				m_fIdleTimer;
	float				m_fAttackTimer;
	WORD				m_wAniSub[3];

	bool				m_bCannotPickUpPileItem; // 겹침가능 아이템을 줍기실패했는지
	bool				m_bCannotPickUpItem;	 // 일반 아이템을 줍기실패했는지

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	float				m_fHeight;

	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;


public:
	SNATIVEID			m_sPetCrowID;
	WORD				m_wColorTEMP;
	WORD				m_wStyleTEMP;

	BOOL				m_bEnableSytle;
	BOOL				m_bEnableColor;

	D3DXVECTOR3			m_vRandPos;

	HMODULE m_hCheckStrDLL;
	BOOL (_stdcall *m_pCheckString)(CString);

public:
	GLPetClient(void);
	~GLPetClient(void);

	DxSkinChar* GetSkinChar ()							{ return m_pSkinChar; }

	void SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget );

	BOOL	IsVALID () { return m_bValid; }
	D3DXVECTOR3 GetPosition () { return m_vPos; }

	BOOL IsSTATE ( DWORD dwStateFlag )		 			{ return m_dwActionFlag&dwStateFlag; }
	void SetSTATE ( DWORD dwStateFlag )					{ m_dwActionFlag |= dwStateFlag; }
	void ReSetSTATE ( DWORD dwStateFlag )				{ m_dwActionFlag &= ~dwStateFlag; }
	void ReSetAllSTATE ()								{ m_dwActionFlag = 0; }

	void SetMoveState ( BOOL bRun );

	bool IsUsePetSkinPack() { return m_sPetSkinPackState.bUsePetSkinPack; }

	D3DXVECTOR3 GetRandomOwnerTarget ();

	EM_FIELDITEM_STATE	IsInsertableInven ( STARGETID sTar );

	bool	CheckSkill ( SNATIVEID sSkillID );
	void	AccountSkill ( SNATIVEID sSkillID );
	void	UpdateSkillDelay ( float fElapsedTime );
	float	GetSkillDelayPercent ( SNATIVEID sSkillID );

	HRESULT Create ( const PGLPET pPetData, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, NavigationMesh* pNavi, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UpdateAnimation ( float fTime, float fElapsedTime );
	HRESULT	UpdatePetState ( float fTime, float fElapsedTime );
	HRESULT UpdateSuit ( BOOL bChangeStyle );

	// Default FrameWork
	HRESULT FrameMove( float fTime, float fElapsedTime );
	HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

	void MsgProcess ( NET_MSG_GENERIC* nmg );

protected:
	void MakeAniSubKey ( float fTime );

public:
	void	ReqGoto ( D3DXVECTOR3 vTarPos, bool bToPickUp );
	void	ReqStop ( bool bStopAttack = false );
	void	ReqUpdateVelocity ();
	void	ReqUpdateState ();
	void	ReqUpdateSkillState ();
	void	ReqGiveFood ( WORD wPosX, WORD wPosY );
	void	ReqInputName ( WORD wPosX, WORD wPosY );
	void	ReqRename ( const char* szCharName );
	void	ReqInputColor ( WORD wPosX, WORD wPosY );
	void	ReqChangeColor ( WORD wColor );
	void	ReqInputStyle ( WORD wPosX, WORD wPosY );
	void	ReqChangeStyle ( WORD wStyle );
	void	ReqChangeActiveSkill ( SNATIVEID sSkillID );
	void	ReqChangeAccessory ( EMSUIT emSUIT );
	void	ReqLearnSkill ( WORD wPosX, WORD wPosY );
	void    ReqLearnSkill ( DWORD dwChannel, WORD wPosX, WORD wPosY );
	void	ReqRemoveSlotItem ( EMSUIT emType );


	void	MsgGetRightOfItem ( NET_MSG_GENERIC* nmg );
	void	MsgUpdateFull ( NET_MSG_GENERIC* nmg );
	void	MsgRename ( NET_MSG_GENERIC* nmg );
	void	MsgChangeColor ( NET_MSG_GENERIC* nmg );
	void	MsgChangeStyle ( NET_MSG_GENERIC* nmg );
	void	MsgSlotExHold ( NET_MSG_GENERIC* nmg );
	void	MsgHoldToSlot ( NET_MSG_GENERIC* nmg );
	void	MsgSlotToHold ( NET_MSG_GENERIC* nmg );
	void	MsgSkillChange ( NET_MSG_GENERIC* nmg );
	void	MsgLearnSkill ( NET_MSG_GENERIC* nmg );
	void	MsgRemoveSlotItem ( NET_MSG_GENERIC* nmg );
	void	MsgAttack ( NET_MSG_GENERIC* nmg );
	void	MsgSad ( NET_MSG_GENERIC* nmg );
	void	MsgAccessoryDelete ( NET_MSG_GENERIC* nmg );
	void	MsgRemoveSkillFact ( NET_MSG_GENERIC* nmg );

	// Skill Functions
	void	ProceedSkill ();
	void	StartSkillProc ();
	void	GetItembySkill ( SNATIVEID sSkillID );
	void	GetAllItems ();
	void	GetRareItems ();
	void	GetPotions ();
	void	GetMoney ();
	void	GetStone ();

	// 스타일 & 컬러
	void StyleInitData();
	void StyleChange( WORD wStyle );
	void StyleEnd()						{ m_bEnableSytle = FALSE; }
	void ColorInitData();
	void ColorChange( WORD wColor );
	void ColorEnd()						{ m_bEnableColor = FALSE; }
	void StyleUpdate();
	void ColorUpdate();
};

typedef GLPetClient* PGLPETCLIENT;

inline D3DXVECTOR3 GLPetClient::GetRandomOwnerTarget ()
{
	D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPostision ();
	m_vRandPos = vRandPos;
	STARGETID sTargetID = m_pOwner->GetTargetID ();
	sTargetID.vPos += vRandPos;
	return sTargetID.vPos;
}

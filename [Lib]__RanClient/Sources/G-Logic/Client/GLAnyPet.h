#ifndef GLANYPET_H_
#define GLANYPET_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLPet.h"

#include "../[Lib]__Engine/Sources/NaviMesh/Actor.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLList.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLQuadTree.h"

#include "DxSkinChar.h"
#include "GLSkill.h"

#ifndef GLLandManClient
	class GLLandManClient;
	class GLClientNode;
	typedef SQUADNODE<GLClientNode> CLIENTQUADNODE;
#endif //GLLandMan

class GLAnyPet : public GLPET
{
protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	Actor				m_actorMove;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;

	D3DXVECTOR3			m_vTarPos;

public:
	SGLNODE<GLAnyPet*>*	m_pGlobNode;		//	맵 리스트 노드.
	CLIENTQUADNODE*		m_pQuadNode;		//	쿼드 트리 노드.
	SGLNODE<GLAnyPet*>*	m_pCellNode;		//	셀 리스트 노드.

	DWORD				m_dwActionFlag;
	WORD				m_wAniSub;

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	float				m_fHeight;

	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

public:
	GLAnyPet(void);
	~GLAnyPet(void);

	void ASSIGN ( PSDROPPET pDropPet );

	DxSkinChar* GetSkinChar ()							{ return m_pSkinChar; }

	HRESULT Create ( PSDROPPET pDropPet, NavigationMesh* pNavi, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UpateAnimation ( float fTime, float fElapsedTime );
	HRESULT	UpdatePetState ( float fTime, float fElapsedTime );
	HRESULT UpdateSuit ();

	D3DXVECTOR3 GetPosition ()							{ return m_vPos; }

	BOOL IsSTATE ( DWORD dwStateFlag )		 			{ return m_dwActionFlag&dwStateFlag; }
	void SetSTATE ( DWORD dwStateFlag )					{ m_dwActionFlag |= dwStateFlag; }
	void ReSetSTATE ( DWORD dwStateFlag )				{ m_dwActionFlag &= ~dwStateFlag; }

	bool IsUsePetSkinPack() { return m_sPetSkinPackState.bUsePetSkinPack; }

	void MsgGoto ( NET_MSG_GENERIC* nmg );
	void MsgStop ( NET_MSG_GENERIC* nmg );
	void MsgSkillChange ( NET_MSG_GENERIC* nmg );
	void MsgUpdateMoveState ( NET_MSG_GENERIC* nmg );
	void MsgRename ( NET_MSG_GENERIC* nmg );
	void MsgChangeColor ( NET_MSG_GENERIC* nmg );
	void MsgChangeStyle ( NET_MSG_GENERIC* nmg );
	void MsgChangeAccessory ( NET_MSG_GENERIC* nmg );
	void MsgLearnSkill ( NET_MSG_GENERIC* nmg );
	void MsgRemoveSlotItem ( NET_MSG_GENERIC* nmg );
	void MsgAttack ( NET_MSG_GENERIC* nmg );
	void MsgSad ( NET_MSG_GENERIC* nmg );
	void MsgFunny ( NET_MSG_GENERIC* nmg );
	void MsgAccessoryDelete ( NET_MSG_GENERIC* nmg );
	void MsgPetSkinPack ( NET_MSG_GENERIC* nmg );

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

	void MsgProcess ( NET_MSG_GENERIC* nmg );
};

typedef GLAnyPet* PGLANYPET;
typedef CGLLIST<PGLANYPET> GLANYPETLIST;
typedef SGLNODE<PGLANYPET> GLANYPETNODE;

#endif // GLANYPET_H_
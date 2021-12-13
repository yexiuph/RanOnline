#ifndef GLMATERIALCLIENT_H_
#define GLMATERIALCLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLCrowData.h"
#include "GLCOPY.h"
//#include "GLogicEx.h"

#include "../[Lib]__Engine/Sources/NaviMesh/Actor.h"
#include "DxSkinChar.h"

#ifndef GLLandManClient
	class GLLandManClient;
	class GLClientNode;
	typedef SQUADNODE<GLClientNode> CLIENTQUADNODE;
#endif //GLLandMan

class GLMaterialClient : public GLCOPY
{
public:
	PCROWDATA			m_pCrowData;					//	Crow 기본 정보.
	SNATIVEID			m_sNativeID;					//	Crow NativeID.

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	Actor				m_actorMove;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;

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

	GLLandManClient*			m_pLandManClient;	//	맵 인스턴스 포인터.
	SGLNODE<GLMaterialClient*>*	m_pGlobNode;		//	맵 리스트 노드.
	CLIENTQUADNODE*				m_pQuadNode;		//	쿼드 트리 노드.
	SGLNODE<GLMaterialClient*>*	m_pCellNode;		//	셀 리스트 노드.

	//	Note : 행동.
protected:
	float				m_fAge;					//	경과 시간.
	EMACTIONTYPE		m_Action;				//	행동.
	DWORD				m_dwActState;			//	행동 상태.

protected:
	float				m_fIdleTime;			//	휴식 시간.

public:
	BOOL IsValidBody ();
	BOOL IsDie ();
	BOOL IsHaveVisibleBody ();

public:
	BOOL IsSTATE ( DWORD dwState )					{ return m_dwActState&dwState; }
	virtual BOOL IsACTION ( EMACTIONTYPE emCur )	{ return m_Action == emCur; }

protected:
	void SetSTATE ( DWORD dwState )					{ m_dwActState |= dwState; }
	void ReSetSTATE ( DWORD dwState )				{ m_dwActState &= ~dwState; }

public:
	HRESULT CreateMaterial ( GLLandManClient *pLandManClient, SDROP_MATERIAL *pCrowDrop, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void SetPosition ( D3DXVECTOR3 &vPos );
	virtual const D3DXVECTOR3 &GetPosition ()		{ return m_vPos; }
	float GetDirection ();
	DxSkinChar* GetCharSkin ()						{ return m_pSkinChar; }

public:
	void TurnAction ( EMACTIONTYPE toAction );

public:
	virtual EMCROW GetCrow () const			{ return CROW_MATERIAL; }
	virtual DWORD GetCtrlID () const		{ return m_dwGlobID; }
	virtual const char* GetName () const	{ return m_pCrowData->GetName(); }
	virtual GLDWDATA GetHp () const			{ return GLDWDATA(); }

	virtual WORD GetBodyRadius ();
	virtual float GetBodyHeight()			{ return m_fHeight; }
	virtual D3DXVECTOR3 GetPosBodyHeight ()	{ return D3DXVECTOR3( m_vPos.x, m_vPos.y+m_fHeight, m_vPos.z ); }

public:
	virtual void MsgProcess ( NET_MSG_GENERIC* nmg );
	virtual void ReceiveDamage ( WORD wDamage, DWORD dwDamageFlag, STARGETID sACTOR ) {}
	virtual void ReceiveAVoid () {}
	virtual void ReceiveSwing () {}

protected:
	HRESULT UpateAnimation ( float fTime, float fElapsedTime );	

public:
	BOOL IsCollisionVolume ();
	HRESULT FrameMove ( float fTime, float fElapsedTime );

	HRESULT RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );

public:
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();

public:
	GLMaterialClient(void);
	~GLMaterialClient(void);
};

typedef GLMaterialClient* PGLMATERIALCLIENT;

typedef CGLLIST<PGLMATERIALCLIENT>	GLMATERIALCLIENTLIST;
typedef SGLNODE<PGLMATERIALCLIENT>	GLMATERIALCLIENTNODE;

#endif // GLMATERIALCLIENT_H_
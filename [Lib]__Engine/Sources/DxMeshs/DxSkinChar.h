#pragma once

#include "DxSkinMesh9.h"
#include "DxSkinObject.h"
#include "DxSkinAniMan.h"
#include "DxSkinPieceContainer.h"
#include "DxSkinCharData.h"
#include "DxCharPart.h"
#include "DxSkinEffResult.h"

#include "./DxEffAniPlayer.h"
#include "./DxEffAni.h"
#include "./DxEffKeepPlayer.h"

class DxEffKeepDataMain;

// Note : 캐릭터
class DxSkinChar : public DxSkinAniControl
{
private:
	enum		{ FILE_LENGTH = 64 };

	BOOL			m_bWorldObj;
	float			m_fScale;

	DxEffAniPlayer	m_EffAniPlayer;

//------------------------------------------------------------
//				E	f	f		K	e	e	p
private:
	DxEffKeepPlayer	m_EffKeepPlayer;

public:
	SKINEFFDATA		m_sSkinEffData;

public: 
	void CreateEff( DxEffKeepDataMain* pEff );
	void DeleteEff( DxEffKeepDataMain* pEff );
	void Reset();

public:
	static DWORD				g_dwLOD;
	static DWORD				g_dwHIGHDRAW_NUM;

public:
	TCHAR			m_szFileName[FILE_LENGTH];
	float			m_fHeight;
	D3DXVECTOR3		m_vMax, m_vMin;
	DxCharPart		m_PartArray[PIECE_SIZE];

private:
	DWORD			m_dwVertexNUM;

public:
	HRESULT ClearAll ();

public:
	HRESULT SetPiece ( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlag = 0x0, int nLevel=0, bool bCharacterData = FALSE );
	HRESULT SetPieceGrindLevel ( int nIndex, int nLevel );
	void SetHairColor( WORD wColor );
	WORD GetHairColor();

	PDXCHARPART GetPiece ( int i )		{ return &m_PartArray[i]; }
	void ResetPiece ( int i );
	void SetPartRend ( EMPIECECHAR emPIECE, BOOL bRender );

	const TCHAR* GetFileName()			{ return m_szFileName; }
	DxEffAniPlayer* GetEffAniPlayer()	{ return &m_EffAniPlayer; }
	void SetWorldObj( BOOL bUse )		{ m_bWorldObj = bUse; }
	BOOL GetWorldObj()					{ return m_bWorldObj; }

	void SetCharData ( DxSkinCharData* pCharData, LPDIRECT3DDEVICEQ pd3dDevice, bool bCharaterData = FALSE );
	void SetRadiusHeight( DxSkinCharData* pCharData );

	void GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
	{
		vMax = m_vMax;
		vMin = m_vMin;
	}

private:
	void CollectVertexNUM();	// 파츠가 바뀔경우 버텍스 갯수를 재수집한다.
	void AvailableVB_InitVB();	// VB가 유효한지 확인 후 VB를 초기화.

public:
	HRESULT FrameMove( const float fTime, const float fElapsedTime, const BOOL bContinue=TRUE, BOOL const bFreeze=FALSE, const BOOL bAttackMode=TRUE );
	HRESULT Render( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, const BOOL bShadow=FALSE, const BOOL bEff=TRUE, const BOOL bReverse = FALSE );
	HRESULT RenderShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, const BOOL bDay );
	HRESULT RenderReflect( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot );

	template<class TYPE>
	HRESULT RenderEX( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, const TCHAR* pBoneName, const TYPE& BONEROT )
	{
		D3DXMATRIXA16	matLocalRot, matScale;
		D3DXMatrixIdentity( &matScale );
		matScale._11 = m_fScale;
		matScale._22 = m_fScale;
		matScale._33 = m_fScale;
		D3DXMatrixMultiply( &matLocalRot, &matScale, &matRot );
	
		//	Note : Skin Animation.
		DxSkinAniControl::Render( matLocalRot, pBoneName, BONEROT );

		RenderSKIN( pd3dDevice, matLocalRot );

		return S_OK;
	}
	HRESULT RenderSKIN( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matLocalRot );
	void RenderBone( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

public:
	DxSkinChar(void);
	~DxSkinChar(void);
};





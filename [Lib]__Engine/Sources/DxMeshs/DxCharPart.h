#pragma once

#include "DxSkinMesh9.h"

#include "DxSkinMesh9.h"
#include "DxSkinObject.h"
#include "DxSkinAniMan.h"
#include "DxSkinPieceContainer.h"

struct SKINEFFDATA;

struct DxCharPart
{
private:
	enum		{ FILE_LENGTH = 64 };

	DxSkeleton*			m_pSkeleton;

	DWORD				m_dwFlags;	// 현재 파트의 효과의 Flag 들을 더하고 빼고 함.
	DWORD				m_dwFlag;	// Shadow 를 뿌릴지 안 뿌릴 지를 정하는 옵션

public:
	char				m_szFileName[FILE_LENGTH];

	DxSkinMesh9*		m_pSkinMesh;

	EMPIECECHAR			m_emType;	// 헤드, 손, 몸, 오른손, 왼손 등....
	EMPEACEZONEWEAPON	m_emWeaponWhereBack;

	PSMESHCONTAINER		m_pmcMesh;

	DWORD				m_dwMaterialNum;
	SMATERIAL_PIECE*	m_pMaterialPiece;

private:
	DWORD				m_dwVertexNUM;
	BOOL				m_bRender;
	BOOL				m_bAttackMode;
	DxBoneTrans*		m_pHandHeld;	// 무기위치 손..
	DxBoneTrans*		m_pSlotHeld;	// 무기위치 슬롯..

	//	Note : 추적 위치값 들.
	VECTRACE			m_vecTrace;

	//	Note : 스킨 조각에 부착된 효과들.
	GLEFFCHAR_VEC		m_vecEFFECT;

	//	Note : 개조 레벨.
	int					m_nGrindLevel;

	// Note : 헤어컬러
	DWORD				m_dwHairColor;

	// Note : 일반 메쉬일 경우 최종 Matrix	- Skin 메쉬일 경우는 필요 없는 정보이다.
	D3DXMATRIX			m_matCombine;

public:
	void SetGrindLevel ( int nLevel )		{ m_nGrindLevel = nLevel; }
	int GetGrindLevel () const				{ return m_nGrindLevel; }

	void SetHairColor( DWORD dwColor )		{ m_dwHairColor = dwColor; }
	DWORD GetHairColor() const				{ return m_dwHairColor; }

	const char* GetFileName()	{ return m_szFileName; }

	D3DXMATRIX&	GetMatCombine()				{ return m_matCombine; }

protected:
	void SumEffFlag ();

public:
	void AddEffList ( DxEffChar* pEffChar );
	void DelEffList ( DxEffChar* pEffChar );
	void DelEffList ( const char* szEffFile );

	BOOL FindEffList ( const char* szEffFile );

public:
	void SetPart ( DxSkinPiece* pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice, bool bCharaterData );
	void ReSetPart ();

	void ClearEffList ();

public:
	void SetRender ( BOOL bRender )			{ m_bRender = bRender; }
	void SetAttackMode ( BOOL bMode )		{ m_bAttackMode = bMode; }
	DWORD GetVertexNUM()					{ return m_dwVertexNUM; }	// 정점 갯수.

protected:
	HRESULT DrawMeshContainer ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOriginDraw=TRUE, BOOL bAlpha=FALSE, BOOL bShadow=FALSE, BOOL bAlphaTex=TRUE, float fHeight=0.f );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();

	HRESULT FrameMove ( float fTime, float fETime, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwKeyTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton, SKINEFFDATA& sSKINEFFDATA, BOOL bShadow=FALSE, BOOL bEff=TRUE );
	HRESULT RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton, float fHeight );
	void RenderGlow ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton );
	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton, const float fScale );
	HRESULT RenderGhosting ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinAniControl* pSkinAniControl, const D3DXMATRIX &_matCurAni );

public:
	SVERTEXINFLU* GetTracePos ( std::string strTrace );
	STRACOR* GetTracor ( std::string strTrace );

	//	pBoneMatrice -> (주의) 영향을 받는 본의 갯수가 여러개일 경우에 문제가 생김.
	//	랜더시 미리 계산되어 있으므로 값을 꺼내서 반환.
	HRESULT CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice=NULL );

public:
	DxCharPart ();
	~DxCharPart ();
};
typedef DxCharPart* PDXCHARPART;






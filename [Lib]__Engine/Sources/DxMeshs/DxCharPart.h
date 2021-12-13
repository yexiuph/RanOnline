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

	DWORD				m_dwFlags;	// ���� ��Ʈ�� ȿ���� Flag ���� ���ϰ� ���� ��.
	DWORD				m_dwFlag;	// Shadow �� �Ѹ��� �� �Ѹ� ���� ���ϴ� �ɼ�

public:
	char				m_szFileName[FILE_LENGTH];

	DxSkinMesh9*		m_pSkinMesh;

	EMPIECECHAR			m_emType;	// ���, ��, ��, ������, �޼� ��....
	EMPEACEZONEWEAPON	m_emWeaponWhereBack;

	PSMESHCONTAINER		m_pmcMesh;

	DWORD				m_dwMaterialNum;
	SMATERIAL_PIECE*	m_pMaterialPiece;

private:
	DWORD				m_dwVertexNUM;
	BOOL				m_bRender;
	BOOL				m_bAttackMode;
	DxBoneTrans*		m_pHandHeld;	// ������ġ ��..
	DxBoneTrans*		m_pSlotHeld;	// ������ġ ����..

	//	Note : ���� ��ġ�� ��.
	VECTRACE			m_vecTrace;

	//	Note : ��Ų ������ ������ ȿ����.
	GLEFFCHAR_VEC		m_vecEFFECT;

	//	Note : ���� ����.
	int					m_nGrindLevel;

	// Note : ����÷�
	DWORD				m_dwHairColor;

	// Note : �Ϲ� �޽��� ��� ���� Matrix	- Skin �޽��� ���� �ʿ� ���� �����̴�.
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
	DWORD GetVertexNUM()					{ return m_dwVertexNUM; }	// ���� ����.

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

	//	pBoneMatrice -> (����) ������ �޴� ���� ������ �������� ��쿡 ������ ����.
	//	������ �̸� ���Ǿ� �����Ƿ� ���� ������ ��ȯ.
	HRESULT CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice=NULL );

public:
	DxCharPart ();
	~DxCharPart ();
};
typedef DxCharPart* PDXCHARPART;






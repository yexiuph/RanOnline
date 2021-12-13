// DxEffectReflect.h: interface for the DxEffectReflect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTREFLECT_H__CE6E84D1_52C8_498F_839E_0C82BC6CA3CF__INCLUDED_)
#define AFX_DXEFFECTREFLECT_H__CE6E84D1_52C8_498F_839E_0C82BC6CA3CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxLandMan.h"
#include "./DxEffectFrame.h"

struct DxFrame;

struct REFLECT_PROPERTY
{
	DWORD			m_dwPower;
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
};

struct D3DREFLECTVERTEX
{
	D3DXVECTOR3	vPos;
	D3DXVECTOR2 vTex1;
	const static DWORD FVF = D3DFVF_XYZ|D3DFVF_TEX1;
};

class DxReflectTree
{
public:		// Tree 구성에 필요한 정보들
	union
	{
		struct { D3DXVECTOR3 m_vMax; };					// Save
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3 m_vMin; };					// Save
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	DxReflectTree *m_pLeftChild;
	DxReflectTree *m_pRightChild;

	float		m_fDisX;			// Save
	float		m_fDisY;			// Save
	float		m_fDisZ;			// Save

	LPD3DXMESH	m_pOcMesh;			// Temp

	D3DXVECTOR3*	m_pPosSRC;		// Save,	Play
	DWORD			m_dwVertNUM;	// Save,	Play
	DWORD			m_dwFaceNUM;	// Save,	Play

protected:
	DWORD		m_dwFileCur;	// Play

	LPDIRECT3DVERTEXBUFFERQ	m_pVB;	//		Play
	LPDIRECT3DINDEXBUFFERQ	m_pIB;	// Save, Play

public:
	DWORD	GetFaceNUM()	{ return m_dwFaceNUM; }
	BOOL	IsOcMesh()		{ return m_pOcMesh ? TRUE : FALSE; }
	BOOL	IsVB()			{ return m_pVB ? TRUE : FALSE; }

	BOOL	IsLoad()
	{
		if ( !m_pPosSRC )	return FALSE;
		if ( !m_pVB )		return FALSE;
		if ( !m_pIB )		return FALSE;
		return TRUE; 
	}

public:
	void CreateSampleMesh ( LPDIRECT3DDEVICEQ pd3dDevice );				// 샘플
	void CreateVBIB ( LPDIRECT3DDEVICEQ pd3dDevice );
	void CloneData ( LPDIRECT3DDEVICEQ pd3dDevice, DxReflectTree* pSrc );

	void Render ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void CleanUp();
	void CreateVB_TexUV ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3* pPos );

public:
	void Save ( CSerialFile& SFile );
	void Load ( CSerialFile& SFile );
	void DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

public:
	DxReflectTree () :
		m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		m_vMin(FLT_MAX,FLT_MAX,FLT_MAX),
		m_pLeftChild(NULL),
		m_pRightChild(NULL),
		m_fDisX(0.f),
		m_fDisY(0.f),
		m_fDisZ(0.f),
		m_pOcMesh(NULL),
		m_pPosSRC(NULL),
		m_dwVertNUM(0L),
		m_dwFaceNUM(0L),

		m_dwFileCur(0),

		m_pVB(NULL),
		m_pIB(NULL)
	{
	};

	~DxReflectTree ()
	{
		CleanUp();

		SAFE_DELETE(m_pLeftChild);
		SAFE_DELETE(m_pRightChild);
	};
};
typedef DxReflectTree* PDXREFLECTTREE;

#include "NsOCTree.h"

class DxReflectAABB
{
public:
	DxReflectTree*	m_pTree;

public:
	void Create ( LPDIRECT3DDEVICEQ pd3dDevice, BYTE* pVert, WORD* pIndex, DWORD dwFace, D3DXMATRIX& matWorld, const DWORD dwFVF );
	void CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, DxReflectTree* pTree );

protected:
	void CleanUp();
	void CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, PDXREFLECTTREE& pSrc, DxReflectTree* pDest );

public:
	void Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const BOOL bDynamicLoad );
	
protected:
	void Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxReflectTree* pTree, const BOOL bDynamicLoad );

public:
	void Save ( CSerialFile& SFile );
	void Load ( CSerialFile& SFile );
	void DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV );

protected:
	void Save ( CSerialFile& SFile, DxReflectTree* pTree );
	void Load ( CSerialFile& SFile, PDXREFLECTTREE& pTree );
	void DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV, DxReflectTree* pTree );

public:
	DxReflectAABB();
	~DxReflectAABB();
};

namespace NSREFLECT
{
	void DivideMesh ( PDXREFLECTTREE& pTree, D3DXVECTOR3* pVertex, DWORD dwFace );	// Tree 를 만든다.
	void MakeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxReflectTree* pTree );				// Tree 안에 있는 Mesh 데이터를 생성한다.
	void MakeOptimizeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxReflectTree* pTree );		// Tree 안에 있는 Mesh 데이터를 최적화 한다.
	void MakeVBIB ( LPDIRECT3DDEVICEQ pd3dDevice, DxReflectTree* pTree );				// 데이터에 맞는 VB 와 IB 로 생성후 Mesh는 지운다.
};

class DxEffectReflect : public DxEffectBase
{		
	//	Note : 이펙트 타입 정의.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSB;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_SAVE;

public:
	void SetProperty ( REFLECT_PROPERTY& Property )
	{
		m_Property = Property;
	}
	REFLECT_PROPERTY& GetProperty () { return m_Property; }

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			REFLECT_PROPERTY	m_Property;
		};
		struct
		{
			DWORD				m_dwPower;
			D3DXVECTOR3			m_vMax;
			D3DXVECTOR3			m_vMin;
		};
	};

protected:
	DWORD			m_dwColor;
	D3DXVECTOR3		m_vCenter;
	DxReflectAABB	m_sReflectAABB;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

protected:
	HRESULT	CreateVB( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pmsMeshs, D3DXMATRIX& matWorld );
	void	D3DXMatrixTexScaleBias( D3DXMATRIX& Out );
	HRESULT RenderReflection( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );
	HRESULT RenderReflectionEX( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );

public:
	DxEffectReflect();
	virtual ~DxEffectReflect();

public:
	virtual void GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
	virtual HRESULT CloneData( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	virtual void SaveBuffer( CSerialFile &SFile );
	virtual void LoadBufferSet( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(AFX_DXEFFECTREFLECT_H__CE6E84D1_52C8_498F_839E_0C82BC6CA3CF__INCLUDED_)

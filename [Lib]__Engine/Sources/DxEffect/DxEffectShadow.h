// DxEffectShadow.h: interface for the DxEffectShadowHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTSHADOW_H__INCLUDED_)
#define AFX_DXEFFECTSHADOW_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

#include <map>

struct DxFrame;
struct DxMeshes;
class DxFrameMesh;
class DxSetLandMan;

#define		USESTATICSHADOW	0x001
#define		USEDIRECTSHADOW	0x002
#define		USEPOINTSHADOW	0x004
#define		USECHANGESHADOW	0x008

struct SHADOW_PROPERTY		// Ver.101
{
	DWORD			m_dwFlag;
	D3DXVECTOR3		m_vViewMax;
	D3DXVECTOR3		m_vViewMin;
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
	DWORD			m_dwTexSize;				//  저장 할 경우의 텍스쳐 사이즈
	float			m_fShadowD;					//	저장 할 경우의 텍스쳐 밝기
	float			m_fShadowP;					//	저장 할 경우의 텍스쳐 밝기

	float			m_fViewMoveX;
	float			m_fViewMoveZ;

	char			m_szFilter[MAX_PATH];		// 필터
	char			m_szTexDayTime[MAX_PATH];	// 아침용 텍스쳐
	char			m_szTexNight[MAX_PATH];		// 저녁용 텍스쳐

	SHADOW_PROPERTY()
	{
		memset( m_szFilter, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexDayTime, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexNight, 0, sizeof(char)*MAX_PATH );
	};
};

struct SHADOW_PROPERTY_100		// Ver.100, 101
{
	DWORD			m_dwFlag;
	D3DXVECTOR3		m_vViewMax;
	D3DXVECTOR3		m_vViewMin;
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
	DWORD			m_dwTexSize;				//  저장 할 경우의 텍스쳐 사이즈
	float			m_fShadowD;					//	저장 할 경우의 텍스쳐 밝기
	float			m_fShadowP;					//	저장 할 경우의 텍스쳐 밝기
	char			m_szFilter[MAX_PATH];		// 필터
	char			m_szTexDayTime[MAX_PATH];	// 아침용 텍스쳐
	char			m_szTexNight[MAX_PATH];		// 저녁용 텍스쳐

	SHADOW_PROPERTY_100()
	{
		memset( m_szFilter, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexDayTime, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexNight, 0, sizeof(char)*MAX_PATH );
	};
};

#include "NsOCTree.h"

struct SHADOW_TEX1				
{
	D3DXVECTOR3			vPos;
	D3DXVECTOR2			vTex1;
	static const DWORD	FVF = D3DFVF_XYZ|D3DFVF_TEX1;
};

struct SHADOW_TEX1_COLOR
{
	D3DXVECTOR3			vPos;
	D3DCOLOR			vColor;
	D3DXVECTOR2			vTex1;
	static const DWORD	FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
};

class DxShadowTree
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

	DxShadowTree *m_pLeftChild;
	DxShadowTree *m_pRightChild;

	float		m_fDisX;			// Save
	float		m_fDisY;			// Save
	float		m_fDisZ;			// Save

	LPD3DXMESH	m_pOcMesh;			// Temp

	D3DXVECTOR3*	m_pPosSRC;		// Save,	Play
	DWORD			m_dwVertNUM;	// Save,	Play
	DWORD			m_dwFaceNUM;	// Save,	Play

protected:
	DWORD		m_dwFileCur;	// Play
	BOOL		m_bLoad;		// Play

	D3DXVECTOR3 m_vViewMin;		// Save,	Play
	D3DXVECTOR3 m_vViewMax;		// Save,	Play
	float		m_fMicroSize;	// Save,	Play

	LPDIRECT3DVERTEXBUFFERQ	m_pCharVB;		//		Play
	LPDIRECT3DVERTEXBUFFERQ	m_pStaticVB;	//		Play
	LPDIRECT3DINDEXBUFFERQ	m_pStaticIB;	// Save, Play

public:
	DWORD	GetFaceNUM()	{ return m_dwFaceNUM; }
	BOOL	IsLoad()		{ return m_bLoad; }
	BOOL	IsOcMesh()		{ return m_pOcMesh ? TRUE : FALSE; }
	BOOL	IsCharVB()		{ return m_pCharVB ? TRUE : FALSE; }
	BOOL	IsStaticVB()	{ return m_pStaticVB ? TRUE : FALSE; }

public:
	void CreateVBIB ( LPDIRECT3DDEVICEQ pd3dDevice );
	void CloneData ( LPDIRECT3DDEVICEQ pd3dDevice, DxShadowTree* pSrc );
	void MakeCharShadowUV ( const D3DXMATRIX& matDirect );
	void MakeStaticShadowUV ( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const float fSize );
	void StaticShadowDraw ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTex );
	void CharShadowDraw ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTex );

protected:
	void CleanUp();
	void CreateVB ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3* pPos );
	void MakeStaticShadowUV ( const D3DXVECTOR3* pPos, const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const float fSize );
	void SetShadowUV_Day ( SHADOW_TEX1_COLOR& pVert, const D3DXVECTOR3& sPos, const D3DXMATRIX& matDirect );

public:
	void Save ( CSerialFile& SFile );
	void Load ( CSerialFile& SFile );
	void DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

public:
	DxShadowTree () :
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
		m_bLoad(FALSE),

		m_vViewMin(FLT_MAX,FLT_MAX,FLT_MAX),
		m_vViewMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		m_fMicroSize(0.f),

		m_pCharVB(NULL),
		m_pStaticVB(NULL),
		m_pStaticIB(NULL)
	{
	};

	~DxShadowTree ()
	{
		CleanUp();

		SAFE_DELETE(m_pLeftChild);
		SAFE_DELETE(m_pRightChild);
	};
};
typedef DxShadowTree* PDXSHADOWTREE;

class DxShadowAABB
{
public:
	DxShadowTree*	m_pTree;

public:
	void Create ( LPDIRECT3DDEVICEQ pd3dDevice, VERTEX* pVert, WORD* pIndex, DWORD dwFace, D3DXMATRIX& matWorld );
	void CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, DxShadowTree* pTree );
	void MakeStaticShadowUV ( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const float fSize );

protected:
	void CleanUp();
	void CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, PDXSHADOWTREE& pDest, DxShadowTree* pSrc );
	void MakeStaticShadowUV ( DxShadowTree* pTree, const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const float fSize  );

public:
	void RenderChar ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, LPDIRECT3DTEXTUREQ pTex, const D3DXMATRIX& matDirect, const BOOL bDynamicLoad );
	void RenderStatic ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, LPDIRECT3DTEXTUREQ pTex, const BOOL bDynamicLoad );
	
protected:
	void RenderChar ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxShadowTree* pTree, 
					LPDIRECT3DTEXTUREQ pTex, const D3DXMATRIX& matDirect, const BOOL bDynamicLoad );
	void RenderStatic ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxShadowTree* pTree, 
					LPDIRECT3DTEXTUREQ pTex, const BOOL bDynamicLoad );

public:
	void Save ( CSerialFile& SFile );
	void Load ( CSerialFile& SFile );
	void DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV );

protected:
	void Save ( CSerialFile& SFile, DxShadowTree* pTree );
	void Load ( CSerialFile& SFile, PDXSHADOWTREE& pTree );
	void DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV, DxShadowTree* pTree );

public:
	DxShadowAABB();
	~DxShadowAABB();
};

namespace NSSHADOW
{
	void MakeShadow ( PDXSHADOWTREE& pTree, D3DXVECTOR3* pVertex, DWORD dwFace );	// Tree 를 만든다.
	void MakeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxShadowTree* pTree );			// Tree 안에 있는 Mesh 데이터를 생성한다.
	void MakeOptimizeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxShadowTree* pTree );	// Tree 안에 있는 Mesh 데이터를 최적화 한다.
	void MakeVBIB ( LPDIRECT3DDEVICEQ pd3dDevice, DxShadowTree* pTree );			// 데이터에 맞는 VB 와 IB 로 생성후 Mesh는 지운다.
};

class DxEffectShadow : public DxEffectBase
{
	//	Note : 이펙트 타입 정의.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

protected:
	static char			m_szPath[MAX_PATH];

public:
	static void			SetPath ( char* szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	static char*		GetPath ()					{ return m_szPath; }

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

public:
	void SetProperty ( SHADOW_PROPERTY& Property )
	{
		m_Property = Property;
	}
	SHADOW_PROPERTY& GetProperty () { return m_Property; }

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedColorSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectColorSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedFilerSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectFilerSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedAddSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectAddSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedBlurSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectBlurSB;


	DxShadowAABB	m_sShadowAABB;

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			SHADOW_PROPERTY	m_Property;
		};

		struct
		{
			DWORD			m_dwFlag;
			D3DXVECTOR3		m_vViewMax;
			D3DXVECTOR3		m_vViewMin;
			D3DXVECTOR3		m_vMax;
			D3DXVECTOR3		m_vMin;
			DWORD			m_dwTexSize;				//  저장 할 경우의 텍스쳐 사이즈
			float			m_fShadowD;					//	저장 할 경우의 텍스쳐 밝기
			float			m_fShadowP;					//	저장 할 경우의 텍스쳐 밝기

			float			m_fViewMoveX;
			float			m_fViewMoveZ;

			char			m_szFilter[MAX_PATH];		// 필터
			char			m_szTexDayTime[MAX_PATH];	// 아침용 텍스쳐
			char			m_szTexNight[MAX_PATH];		// 저녁용 텍스쳐
		};
	};

protected:
	LPDIRECT3DTEXTUREQ	m_pSSDayTex;
	LPDIRECT3DTEXTUREQ	m_pSSNightTex;
	LPDIRECT3DTEXTUREQ	m_pFilterTex;

	float			m_fDistance;
	DWORD			m_dwVertices;

	BOOL*			m_pUse;

protected:
	DxFrame*		m_pThisFrame;
	D3DXMATRIX		m_matWorld;

protected:
	LPDIRECT3DTEXTUREQ	m_pATexture;	// 1
	LPDIRECT3DSURFACEQ	m_pASurface;	// 1

	LPDIRECT3DTEXTUREQ	m_pBTexture;	// 2
	LPDIRECT3DSURFACEQ	m_pBSurface;	// 2

	LPDIRECT3DTEXTUREQ	m_pSShadowTexture;		// Static Shadow
	LPDIRECT3DSURFACEQ	m_pSShadowSurface;		// Static Shadow

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
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

public:
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

private:
	HRESULT CreateShadow ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, D3DXMATRIX	matWorld, DxFrame *pframeCur );

public:
	HRESULT	MakeStaticShadow ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFile, DxSetLandMan *pSetLandMan, DWORD dwTexSize );	// 정적 그림자
	VOID	ReSetTexUV ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh );											// UV 좌표에 그림자가 생성되어 있지 않을 경우
	VOID	ReSetTexUV ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DWORD dwTexSize );							// UV 좌표에 그림자가 생성되어 있지 않을 경우

	void	SetView_Max_Min ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur );

private:
	HRESULT ProjectionObjD	( LPDIRECT3DDEVICEQ pd3dDevice,	DxFrame *pframeShadow, BOOL bTest, DxSetLandMan *pSetLandMan );
	HRESULT ProjectionObjP	( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeShadow, DxSetLandMan *pSetLandMan );
	HRESULT ProjectionObj	( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame*		pframeCur, CLIPVOLUME *pCV, D3DXVECTOR3 *vDir );
	HRESULT PickingObjD		( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame*		pframeCur, DxFrame *pframeShadow, CLIPVOLUME *pCV, D3DXVECTOR3 *vDir, DxSetLandMan *pSetLandMan );
	HRESULT PickingObjP		( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame*		pframeCur, DxFrame *pframeShadow, CLIPVOLUME *pCV, DxSetLandMan *pSetLandMan );
	HRESULT PickingObjP		( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame*		pframeCur, DxFrame *pframeShadow, CLIPVOLUME *pCV, D3DXVECTOR3 *vPos, float fRange, DxSetLandMan *pSetLandMan );
	HRESULT RenderObjD		( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame*		pframeCur );
	HRESULT RenderObjP		( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame*		pframeCur );
	BOOL	CheckViewPort	( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame*		pframeCur, float fDistance, DWORD dwTexSize );
	HRESULT SetTexUV		( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame*		pframeCur );
	HRESULT ResetLocalMeshs ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame*		pframeCur, DWORD dwFVF );
	VOID	ImageBlur		( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID	ImageBlur		( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ pSrcSurface, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface );
	VOID	DeleteFaces		( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, D3DXVECTOR3 vDelVert );

public:
	DxEffectShadow();
	virtual ~DxEffectShadow();

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(AFX_DXEFFECTSHADOW_H__INCLUDED_)

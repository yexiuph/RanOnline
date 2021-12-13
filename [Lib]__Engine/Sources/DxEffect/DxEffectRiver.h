// Terrain0.h: interface for the DxEffectRiver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DXEFFCTRIVER_INCLUDED__)
#define __DXEFFCTRIVER_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"
#include "Collision.h"
#include <vector>

struct DxMeshes;

#define		USEDARK			0x001
#define		USEFLASH		0x002
#define		USEREFLECT		0x004
#define		USEREFLECT_NEW	0x008
#define		USESEE			0x010
#define		USESAMEHEIGHT	0x020

struct RAINDROP
{
	D3DXVECTOR3	vPos;			// 위치 값
	float		fRotateY;		// 회전된 값
	float		fSize;			// 물방울 크기
	float		fTime;			// 지나간 시간
	int			nTexNum;		// 텍스쳐 번호

	RAINDROP() :
		vPos(0.f,0.f,0.f),
		fRotateY(0.f),
		fTime(0.f),
		nTexNum(0)
	{
	}

	BOOL FrameMove ( float fETime );
};

class DXRAINDROPMAN
{
protected:
	struct VERTEXCOLOR			// 빗방울용.
	{ 
		D3DXVECTOR3 vPos;
		D3DCOLOR	dwColor;
		D3DXVECTOR2 vTex; 
		const static DWORD FVF;
	};

protected:
	LPDIRECT3DTEXTUREQ		m_pRainDropTEX;
	char					m_szRainDropTEX[MAX_PATH];

protected:
	typedef std::vector<RAINDROP>		RAINDROPVECTOR;
	typedef RAINDROPVECTOR::iterator	RAINDROPVECTOR_ITER;

	RAINDROPVECTOR				m_vecRainDrop;

protected:
	float			m_fAREA;		// 면적
	float			m_fWidth;		// 길이

public:
	void	SetMaxMin ( D3DXVECTOR3 vMax, D3DXVECTOR3 vMin );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects ();

	HRESULT FrameMove ( float fTime, float fElapsedtime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB );

public:
	DXRAINDROPMAN();
	~DXRAINDROPMAN();
};

struct RIVER_PROPERTY_100
{
	BOOL				m_bBase;
	BOOL				m_bRefract;
	BOOL				m_bReflect;

	int					m_nCol;
	int					m_nRow;
	float				m_fVel;					// 이동 속도
	float				m_fScale;				// 크기
	float				m_fBumpAlpha;			// 범프 반사 알파
	float				m_fAlpha;				// 일반 알파
	D3DXVECTOR3			m_vColor;				// 컬러

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	D3DXMATRIX			m_matFrameComb;

	char				m_szTexMove[MAX_PATH];		// 이동하는 Tex
	char				m_szTexRotate[MAX_PATH];	// 로테이션 하는 Tex

	RIVER_PROPERTY_100()
	{
		memset( m_szTexMove, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexRotate, 0, sizeof(char)*MAX_PATH );
	};
};

struct RIVER_PROPERTY_101_5	// Ver.101 & Ver.102 , 103, 104, 105
{
	BOOL				m_bBase;
	BOOL				m_bDark;
	BOOL				m_bReflect;

	float				m_fVel;					// 이동 속도
	float				m_fBumpAlpha;			// 범프 반사 알파
	float				m_fAlpha;				// 일반 알파
	D3DXVECTOR2			m_vScale;				// 크기
	D3DXVECTOR3			m_vColor;				// 컬러

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	D3DXMATRIX			m_matFrameComb;

	char				m_szTexMove[MAX_PATH];		// 이동하는 Tex
	char				m_szTexRotate[MAX_PATH];	// 로테이션 하는 Tex

	RIVER_PROPERTY_101_5()
	{
		memset( m_szTexMove, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexRotate, 0, sizeof(char)*MAX_PATH );
	};
};

struct RIVER_PROPERTY_106	// Ver.106
{
	DWORD				m_dwFlag;

	float				m_fBumpAlpha;			// 범프 반사 알파

	float				m_fVel;					// 이동 속도
	D3DXVECTOR2			m_vScale;				// 크기		
	D3DXVECTOR3			m_vColor;				// 어둠용	

	D3DXVECTOR2			m_vDarkVel;				// 어둠용 이동
	D3DXVECTOR2			m_vDarkScale;			// 어둠용 크기
	D3DXVECTOR3			m_vDarkColor;			// 어둠용 컬러

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	D3DXMATRIX			m_matFrameComb;

	char				m_szTexDark[MAX_PATH];		// 어둠의 Tex
	char				m_szTexFlash[MAX_PATH];	// 회전하는 Tex

	RIVER_PROPERTY_106()
	{
		memset( m_szTexDark, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexFlash, 0, sizeof(char)*MAX_PATH );
	};
};

struct RIVER_PROPERTY	// Ver.107
{
	DWORD				m_dwFlag;

	float				m_fBumpAlpha;			// 범프 반사 알파

	float				m_fVel;					// 이동 속도
	float				m_fScale;				// 크기		
	D3DXVECTOR3			m_vColor;				// 어둠용	

	float				m_fDarkScale;			// 어둠용 크기
	D3DXVECTOR2			m_vDarkVel;				// 어둠용 이동
	D3DXVECTOR3			m_vDarkColor;			// 어둠용 컬러

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	char				m_szTexDark[MAX_PATH];	// 어둠의 Tex
	char				m_szTexFlash[MAX_PATH];	// 회전하는 Tex

	RIVER_PROPERTY()
	{
		memset( m_szTexDark, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexFlash, 0, sizeof(char)*MAX_PATH );
	};
};

struct D3DWATERVERTEX		// 반사 범프 O		// 일렁이는 모습
{
	D3DXVECTOR3	vPos;
	D3DCOLOR	vColor;
	D3DXVECTOR2 vTex1;
	D3DXVECTOR2 vTex2;
	const static DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2;
};

class DxWaterTree
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

	DxWaterTree *m_pLeftChild;
	DxWaterTree *m_pRightChild;

	float		m_fDisX;			// Save
	float		m_fDisY;			// Save
	float		m_fDisZ;			// Save

	LPD3DXMESH	m_pOcMesh;			// Temp

	D3DXVECTOR3*	m_pPosSRC;		// Save,	Play
	D3DXVECTOR2*	m_pTexUV;		// Save,	Play
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
		if ( !m_pTexUV )	return FALSE;
		if ( !m_pVB )		return FALSE;
		if ( !m_pIB )		return FALSE;
		return TRUE; 
	}

public:
	void CreateSampleMesh ( LPDIRECT3DDEVICEQ pd3dDevice );				// 샘플
	void CreateVBIB ( LPDIRECT3DDEVICEQ pd3dDevice );
	void CloneData ( LPDIRECT3DDEVICEQ pd3dDevice, DxWaterTree* pSrc );

	void FrameMove ( const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1 );
	void FrameMove ( const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1, const D3DXVECTOR2& vAddTex2 );
	void Render ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void CleanUp();
	void CreateVB_TexUV ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3* pPos );

public:
	void Save ( CSerialFile& SFile );
	void Load ( CSerialFile& SFile );
	void DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

public:
	DxWaterTree () :
		m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		m_vMin(FLT_MAX,FLT_MAX,FLT_MAX),
		m_pLeftChild(NULL),
		m_pRightChild(NULL),
		m_fDisX(0.f),
		m_fDisY(0.f),
		m_fDisZ(0.f),
		m_pOcMesh(NULL),
		m_pPosSRC(NULL),
		m_pTexUV(NULL),
		m_dwVertNUM(0L),
		m_dwFaceNUM(0L),

		m_dwFileCur(0),

		m_pVB(NULL),
		m_pIB(NULL)
	{
	};

	~DxWaterTree ()
	{
		CleanUp();

		SAFE_DELETE(m_pLeftChild);
		SAFE_DELETE(m_pRightChild);
	};
};
typedef DxWaterTree* PDXWATERTREE;

#include "NsOCTree.h"

class DxWaterAABB
{
public:
	DxWaterTree*	m_pTree;

public:
	void Create ( LPDIRECT3DDEVICEQ pd3dDevice, BYTE* pVert, WORD* pIndex, DWORD dwFace, D3DXMATRIX& matWorld, const DWORD dwFVF );
	void CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, DxWaterTree* pTree );

protected:
	void CleanUp();
	void CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, PDXWATERTREE& pSrc, DxWaterTree* pDest );

public:
	void Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const BOOL bDynamicLoad,
					const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1 );
	void Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const BOOL bDynamicLoad,
					const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1, const D3DXVECTOR2& vAddTex2 );
	
protected:
	void Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxWaterTree* pTree, const BOOL bDynamicLoad,
					const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1);
	void Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxWaterTree* pTree, const BOOL bDynamicLoad,
					const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1, const D3DXVECTOR2& vAddTex2 );

public:
	void Save ( CSerialFile& SFile );
	void Load ( CSerialFile& SFile );
	void DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV );

protected:
	void Save ( CSerialFile& SFile, DxWaterTree* pTree );
	void Load ( CSerialFile& SFile, PDXWATERTREE& pTree );
	void DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV, DxWaterTree* pTree );

public:
	DxWaterAABB();
	~DxWaterAABB();
};

namespace NSWATER
{
	void DivideMesh ( PDXWATERTREE& pTree, D3DXVECTOR3* pVertex, DWORD dwFace );	// Tree 를 만든다.
	void MakeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxWaterTree* pTree );				// Tree 안에 있는 Mesh 데이터를 생성한다.
	void MakeOptimizeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxWaterTree* pTree );		// Tree 안에 있는 Mesh 데이터를 최적화 한다.
	void MakeVBIB ( LPDIRECT3DDEVICEQ pd3dDevice, DxWaterTree* pTree );				// 데이터에 맞는 VB 와 IB 로 생성후 Mesh는 지운다.
};

class DxEffectRiver : public DxEffectBase
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
	static	LPDIRECT3DVERTEXSHADER9			m_pReflectVS;
	static	LPDIRECT3DVERTEXDECLARATION9	m_pReflectDCRT;
	static	DWORD							m_dwReflectAVS[];

	static	LPDIRECT3DVERTEXSHADER9			m_pOceanWaterVS;
	static	DWORD							m_dwOceanWaterAVS[];

	static	LPDIRECT3DPIXELSHADER9			m_pOceanWaterPS;
	static	DWORD							m_dwOceanWaterAPS[];

	static	LPDIRECT3DVERTEXSHADER9			m_pRiverVS;
	static	LPDIRECT3DVERTEXDECLARATION9	m_pRiverDCRT;
	static	DWORD							m_dwRiverAVS[];

	static	LPDIRECT3DPIXELSHADER9			m_pRiverPS;
	static	DWORD							m_dwRiverAPS[];

	static LPDIRECT3DSTATEBLOCK9	m_pSB_Reflect;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_TnL;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Dark;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_WaveTex;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_VSPS;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Reflect_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_TnL_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Dark_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_WaveTex_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_VSPS_SAVE;

public:
	void SetProperty ( RIVER_PROPERTY& Property )
	{
		m_Property = Property;
	}
	RIVER_PROPERTY& GetProperty () { return m_Property; }

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			RIVER_PROPERTY		m_Property;
		};

		struct
		{
			DWORD				m_dwFlag;

			float				m_fBumpAlpha;			// 범프 반사 알파

			float				m_fVel;					// 이동 속도
			float				m_fScale;				// 크기		
			D3DXVECTOR3			m_vColor;				// 어둠용	

			float				m_fDarkScale;			// 어둠용 크기
			D3DXVECTOR2			m_vDarkVel;				// 어둠용 이동
			D3DXVECTOR3			m_vDarkColor;			// 어둠용 컬러

			D3DXVECTOR3			m_vMax;
			D3DXVECTOR3			m_vMin;

			char				m_szTexDark[MAX_PATH];	// 어둠의 Tex
			char				m_szTexFlash[MAX_PATH];	// 회전하는 Tex
		};
	};

	//	Note : 워터 버텍스 구조.
	//
public :
	struct VERTEXCOLOR			// 빗방울용.
	{ 
		D3DXVECTOR3 vPos;
		D3DCOLOR	dwColor;
		D3DXVECTOR2 vTex; 
		const static DWORD FVF;
	};

	struct D3DWAVETEX
	{
		D3DXVECTOR4			vPos;
		D3DXVECTOR2			vTex;
		const static DWORD	FVF;
	};

protected:
	const	static	DWORD			RAINDROP_MAXNUM;
	const	static	float			DEFAULT_SCALE;
	const	static	float			DEFAULT_SPEED;

	static	LPDIRECT3DVERTEXBUFFERQ	m_pWaveTexVB;
	static	LPDIRECT3DINDEXBUFFERQ	m_pWaveTexIB;
	static	LPDIRECT3DVERTEXBUFFERQ	m_pRainDropVB;
	static	LPDIRECT3DINDEXBUFFERQ	m_pRainDropIB;

	DxWaterAABB	m_sWaterAABB;

	D3DXMATRIX	m_matFrameComb;		// 예전 데이터 유지를 위해서

protected:
	LPDIRECT3DTEXTUREQ		m_pddsTexDark;
	LPDIRECT3DTEXTUREQ		m_pddsTexFlash;
	LPDIRECT3DTEXTUREQ		m_pNormalMap;
	char					m_szNormalMap[MAX_PATH];
	LPDIRECT3DTEXTUREQ		m_p1DMap;
	char					m_sz1DMap[MAX_PATH];

	D3DXVECTOR2				m_vAddTex1;			// 누적 값 - 이동
	D3DXVECTOR2				m_vAddTex2;			// 누적 값 - 회전
	D3DXVECTOR2				m_vAddTex_Dark;		// 누적 값 - 다크

	float					m_fTime;
	float					m_fElapsedTime;
	D3DXVECTOR3				m_vCenter;
	float					m_fBumpStart;
	float					m_fBumpDelta;
	float					m_fBumpMat;
	float					m_fSNOWSTOP;		// 눈이 오면 0, 눈이 안오면 1 이다... 눈올때 모든게 멈추네..

public:
	BOOL				m_bEnable_EDIT;
	BOOL				m_bPS14_EDIT;

	//	Note : 빗방울
	//
protected:
	DXRAINDROPMAN			m_cRainDrop;

protected:
	virtual HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );
public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

public:
	void	ReSetDiffuse ();

protected:
	HRESULT RenderEditMODE ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );
	HRESULT RenderGameMODE ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );

	void	D3DXMatrixTexScaleBias ( D3DXMATRIX& Out );

	HRESULT RenderTnL ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );
	HRESULT RenderDark ( LPDIRECT3DDEVICEQ pd3dDevice , DxLandMan* pLandMan);
	HRESULT RenderReflection ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );
	HRESULT RenderReflectionEX ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan);

	HRESULT Render_VS_PS ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );		// 바닷물
	HRESULT Render_VS_PS_2 ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );	// 강물

	HRESULT UpdateWaveTex	( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT	CreateVB ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pmsMeshs, D3DXMATRIX& matWorld );

public:
	DxEffectRiver ();
	virtual ~DxEffectRiver ();

	//	Note : OBJAABB
	//
public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(__DXEFFCTRIVER_INCLUDED__)

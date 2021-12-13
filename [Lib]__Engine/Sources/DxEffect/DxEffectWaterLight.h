// DxEffectShadow.h: interface for the DxEffectShadowHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTWATERLIGHT_H__INCLUDED_)
#define AFX_DXEFFECTWATERLIGHT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxFrame;
struct DxMeshes;

struct WATERLIGHT_PROPERTY_100
{
	bool		m_bBaseRender;			// 원본도 뿌릴 것 인가 ??
	float		m_fVel;					// 이동 속도
	float		m_fScale;				// 크기
	D3DXVECTOR3	m_vDir;					// 방향
	D3DXVECTOR3	m_vColor;				// 컬러
	int			m_iAlphaUp;		// 알파 값
	int			m_iAlphaMiddle;	// 알파 값
	int			m_iAlphaDown;	// 알파 값
	char		m_szTexMove[MAX_PATH];		// 이동하는 Tex
	char		m_szTexRotate[MAX_PATH];	// 로테이션 하는 Tex

	WATERLIGHT_PROPERTY_100()
	{
		memset( m_szTexMove, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexRotate, 0, sizeof(char)*MAX_PATH );
	}
};

struct WATERLIGHT_PROPERTY
{
	bool		m_bBaseRender;			// 원본도 뿌릴 것 인가 ??
	float		m_fVel;					// 이동 속도
	float		m_fScale;				// 크기
	D3DXVECTOR3	m_vDir;					// 방향
	D3DXVECTOR3	m_vColor;				// 컬러
	int			m_iAlphaUp;		// 알파 값
	int			m_iAlphaMiddle;	// 알파 값
	int			m_iAlphaDown;	// 알파 값

	float		m_fMaxX;
	float		m_fMinX;
	float		m_fMaxY;
	float		m_fMinY;
	float		m_fMaxZ;
	float		m_fMinZ;

	char		m_szTexMove[MAX_PATH];		// 이동하는 Tex
	char		m_szTexRotate[MAX_PATH];	// 로테이션 하는 Tex

	WATERLIGHT_PROPERTY()
	{
		memset( m_szTexMove, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexRotate, 0, sizeof(char)*MAX_PATH );
	}
};


class DxEffectWaterLight : public DxEffectBase
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

public:
	void SetProperty ( WATERLIGHT_PROPERTY& Property )
	{
		m_Property = Property;
	}
	WATERLIGHT_PROPERTY& GetProperty () { return m_Property; }


protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;


	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			WATERLIGHT_PROPERTY	m_Property;
		};

		struct
		{
			bool		m_bBaseRender;			// 원본도 뿌릴 것 인가 ??
			float		m_fVel;	// 이동 속도
			float		m_fScale;	// 크기
			D3DXVECTOR3	m_vDir;	// 방향
			D3DXVECTOR3	m_vColor;	// 컬러
			int			m_iAlphaUp;		// 알파 값
			int			m_iAlphaMiddle;	// 알파 값
			int			m_iAlphaDown;	// 알파 값

			float		m_fMaxX;
			float		m_fMinX;
			float		m_fMaxY;
			float		m_fMinY;
			float		m_fMaxZ;
			float		m_fMinZ;

			char		m_szTexMove[MAX_PATH];		// 이동하는 Tex
			char		m_szTexRotate[MAX_PATH];	// 로테이션 하는 Tex
		};
	};

protected:

	struct VERTEX
	{
		D3DXVECTOR3 Pos;       // vertex position
		D3DXVECTOR3 Nor;       // vertex normal
		D3DXVECTOR2 Tex1;
		const static DWORD	FVF;
	};

	struct WATERVERTEX
	{
		D3DXVECTOR3 Pos;       // vertex position
		D3DCOLOR	Diff;      
		D3DXVECTOR2 Tex1;
		D3DXVECTOR2 Tex2;
		const static DWORD	FVF;
	};

	struct DIFFVERTEX
	{
		WORD		nIndex;
		D3DCOLOR	Diff;
	};

protected:

	LPDIRECT3DVERTEXBUFFERQ	m_pVB;			// 저장
	LPDIRECT3DINDEXBUFFERQ	m_pIB;			// 저장

	DWORD	m_dwVertices;					// 저장
	DWORD	m_dwFaces;						// 저장

	DIFFVERTEX*			m_pDiffVertex;		// 저장
	DWORD				m_pDiffNum;			// 저장

	LPDIRECT3DTEXTUREQ	m_pddsTexMove;
	LPDIRECT3DTEXTUREQ	m_pddsTexRotate;

	D3DXVECTOR2*		m_pTexUV;			// 저장	// 텍스쳐 초기 값을 위해서

	D3DXVECTOR2			m_vAddTex1;		// 누적 값 - 이동
	D3DXVECTOR2			m_vAddTex2;		// 누적 값 - 회전

	float				m_fHighHeight;		// 저장	- 이 메쉬의 최고 위치
	float				m_fMiddleHeight;	// 저장	- 이 메쉬의 최고 위치
	float				m_fLowerHeight;		// 저장 - 이 메쉬의 최저 위치

	float	m_fElapsedTime;
	float	m_fTime;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : 
	//
public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

public:
	HRESULT ReMakeWaterLight	( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT SetBoundBox ( LPD3DXMESH pMesh );
	HRESULT CloneWaterLight ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs );
	HRESULT UpdateWater		( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffectWaterLight();
	virtual ~DxEffectWaterLight();

	//Note: OBJAABB 부부분 
public:
	virtual void GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(AFX_DXEFFECTWATERLIGHT_H__INCLUDED_)
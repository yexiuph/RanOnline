// DxEffectNature.h: interface for the DxEffectFire class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DxEFFECTNATURE_H__INCLUDED_)
#define AFX_DxEFFECTNATURE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

#define	NATURE_WAIT		0
#define	NATURE_UPFLY	1
#define	NATURE_DOWNFLY	2
#define	NATURE_WALK		3
#define	NATURE_LANDING	4

struct DxFrame;

struct NATURE_PROPERTY_100
{	
	float		m_fRate;
	DWORD		m_dwBaseNumber;

	float		m_fFlowerRate;
	float		m_fFlowerScale;

	float		m_fButterRate;
	float		m_fButterWidth;
	float		m_fButterHeight;
	float		m_fButterSpeed;
	float		m_fButterRange;
	float		m_fButterHeightUP;
	float		m_fButterHeightDOWN;

	float		m_fBugRate;
	int			m_iBugCol;
	int			m_iBugRow;
	float		m_fBugWidth;
	float		m_fBugHeight;
	float		m_fBugSpeed;
	float		m_fBugRange;
	float		m_fBugHeightUP;
	float		m_fBugHeightDOWN;

	char		m_szButterTex[MAX_PATH];
	char		m_szTexture[MAX_PATH];
	char		m_szBugTex[MAX_PATH];

	NATURE_PROPERTY_100()
	{
		memset( m_szButterTex, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
		memset( m_szBugTex, 0, sizeof(char)*MAX_PATH );
	};
};

struct NATURE_PROPERTY
{	
	float		m_fRate;
	DWORD		m_dwBaseNumber;

	float		m_fFlowerRate;
	float		m_fFlowerScale;

	float		m_fButterRate;
	float		m_fButterWidth;
	float		m_fButterHeight;
	float		m_fButterSpeed;
	float		m_fButterRange;
	float		m_fButterHeightUP;
	float		m_fButterHeightDOWN;

	float		m_fBugRate;
	int			m_iBugCol;
	int			m_iBugRow;
	float		m_fBugWidth;
	float		m_fBugHeight;
	float		m_fBugSpeed;
	float		m_fBugRange;
	float		m_fBugHeightUP;
	float		m_fBugHeightDOWN;

	char		m_szButterTex[MAX_PATH];
	char		m_szTexture[MAX_PATH];
	char		m_szBugTex[MAX_PATH];
	char		m_szShadowTex[MAX_PATH];

	NATURE_PROPERTY()
	{
		memset( m_szButterTex, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
		memset( m_szBugTex, 0, sizeof(char)*MAX_PATH );
		memset( m_szShadowTex, 0, sizeof(char)*MAX_PATH );
	};
};


class DxEffectNature : public DxEffectBase
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
	
	//	Note : 쉐이더.
	//
protected:	
	static char			m_strParticle[];
	static DWORD		m_dwParticle;

	static LPDIRECT3DSTATEBLOCK9	m_pSB_Effect;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Shadow;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Point;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Effect_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Shadow_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Point_SAVE;

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			NATURE_PROPERTY	m_Property;
		};
		struct
		{
			float		m_fRate;
			DWORD		m_dwBaseNumber;

			float		m_fFlowerRate;
			float		m_fFlowerScale;

			float		m_fButterRate;
			float		m_fButterWidth;
			float		m_fButterHeight;
			float		m_fButterSpeed;
			float		m_fButterRange;
			float		m_fButterHeightUP;
			float		m_fButterHeightDOWN;

			float		m_fBugRate;
			int			m_iBugCol;
			int			m_iBugRow;
			float		m_fBugWidth;
			float		m_fBugHeight;
			float		m_fBugSpeed;
			float		m_fBugRange;
			float		m_fBugHeightUP;
			float		m_fBugHeightDOWN;

			char		m_szButterTex[MAX_PATH];
			char		m_szTexture[MAX_PATH];
			char		m_szBugTex[MAX_PATH];
			char		m_szShadowTex[MAX_PATH];
		};
	};

public:
	void SetProperty ( NATURE_PROPERTY& Property )
	{
		m_Property = Property;
	}
	NATURE_PROPERTY& GetProperty () { return m_Property; }

protected:
	float	m_fTime;
	float	m_fElapsedTime;

	struct VERTEX
	{
		D3DXVECTOR3 p;       // vertex position
		D3DXVECTOR3 n;       // vertex normal
		D3DXVECTOR2 t;
	};

	struct BASEVERTEX
	{
		D3DXVECTOR3 p;       // vertex position
	};

	struct PARTICLE
	{
		D3DXVECTOR3 p;       // vertex position
		D3DCOLOR	d;       // vertex 
		D3DXVECTOR2 t;       // vertex 
		const static DWORD	FVF;
	};

	struct PARTICLEPOINT
	{
		D3DXVECTOR3		vPos;
		float			fSize;
		D3DXVECTOR4		Diffuse;
		static const DWORD FVF;
	};

	struct PARTICLE_BUTTERFLY
	{
		D3DXVECTOR3 vNowPos;		// 현재 위치
		D3DXVECTOR3 vTargetPos;		// 가고 싶은 곳
		D3DXVECTOR3	vPrevPos;		// 예전 위치
		int			iState;			// 앉아 있을때, 위로 아래로 날때	0, 1, 2
		int			iLeft;			// 왼쪽으로 가려는 값		// 다시 날때 값 바뀜
		int			iRight;			// 오른쪽으로 가려는 값		// 다시 날때 값 바뀜
		float		fSpeed;			// 이 녀석의 스피드			// 다시 날때 값 바뀜
		float		fDot3;			// 이 녀석이 나가는 방향	// 다시 날때 값 바뀜
		float		fBaseLenth;		// 녀석의 예전위치에서 가려고 하는곳 위치까지의 거리.
	};

	struct PARTICLE_BUG
	{
		D3DXVECTOR3 vNowPos;		// 현재 위치
		D3DXVECTOR3 vTargetPos;		// 가고 싶은 곳
		int			iState;			// 앉아 있을때, 위로 아래로 날때	0, 1, 2
		int			iLeft;			// 왼쪽으로 가려는 값		// 다시 날때 값 바뀜
		int			iRight;			// 오른쪽으로 가려는 값		// 다시 날때 값 바뀜
		float		fSpeed;			// 이 녀석의 스피드			// 다시 날때 값 바뀜
	};
/*
	struct LEAFVERTEX
	{
		BASEVERTEX*	pCurrent;
		BASEVERTEX* pNext;
	};
*/

/*
	struct PARTICLE_LEAF
	{
		D3DXVECTOR3		vNowPos;		// 현재 위치
		D3DXVECTOR3		vTargetPos;		// 가고 싶은 곳
		int				iState;			// 앉아 있을때, 위로 아래로 날때	0, 1, 2
		int				iLeft;			// 왼쪽으로 가려는 값		// 다시 날때 값 바뀜
		int				iRight;			// 오른쪽으로 가려는 값		// 다시 날때 값 바뀜
		float			fSpeed;			// 이 녀석의 스피드			// 다시 날때 값 바뀜
		int				Group;			// 그 룹 ... ^^;
	};
*/

//	LEAFVERTEX*	m_pLeafVert;

/*
	PARTICLE_LEAF*				m_pLeafVert;	// 나뭇잎 정보들
	LPDIRECT3DVERTEXBUFFERQ		m_pLeafVB;
	LPDIRECT3DINDEXBUFFERQ		m_pLeafIB;
	LPDIRECT3DTEXTUREQ			m_pLeafTex;
*/

	BASEVERTEX*	m_pBaseVert;		// 이동 할 수 있는 점들..

	DWORD		m_dwFlowerNum;
	DWORD		m_dwButterNum;
	DWORD		m_dwBugNum;

	PARTICLE_BUTTERFLY*			m_pButterVert;	// 나비 정보들
	LPDIRECT3DVERTEXBUFFERQ		m_pButterVB;
	LPDIRECT3DINDEXBUFFERQ		m_pButterIB;
	LPDIRECT3DTEXTUREQ			m_pButterTex;

	PARTICLE*					m_pButterShadowVert;	// 나비 그림자 들
	LPDIRECT3DVERTEXBUFFERQ		m_pButterShadowVB;
	LPDIRECT3DINDEXBUFFERQ		m_pButterShadowIB;

	LPDIRECT3DTEXTUREQ			m_pShadowTex;

	int							m_iNowSpriteBug;	// 지금 녀석의 스프라이트 번호
	float						m_fAniTimeBug;		// 이녀석의 프레임 돌아가는 속도
	float						m_fTimeBug;

	PARTICLE_BUG*				m_pBugVert;	// 벌레 정보들
	LPDIRECT3DVERTEXBUFFERQ		m_pBugVB;
	LPDIRECT3DINDEXBUFFERQ		m_pBugIB;
	LPDIRECT3DTEXTUREQ			m_pBugTex;
	//LPDIRECT3DVERTEXBUFFERQ		m_pFireLightVB;
	//LPDIRECT3DTEXTUREQ			m_pFireLightTex;

	int		m_AddRand;


public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : 
	//
public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );	
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

public:
	DWORD GetBugNum ()		{ return m_dwBugNum; }
	DWORD GetButterNum ()	{ return m_dwButterNum; }

	HRESULT CreateBugMesh		( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT CreateButterMesh	( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT CreateBaseMesh	( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, D3DXMATRIX matWorld );

	HRESULT UpdateBug		( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UpdateButterFly ( LPDIRECT3DDEVICEQ pd3dDevice );
	float	CalcSurfaceArea	(VERTEX* pFV[3]);

	D3DMATRIX BillboardLookAt ( D3DXVECTOR3 BillboardPos, D3DXVECTOR3 CarmeraPos );

public:
	DxEffectNature();
	virtual ~DxEffectNature();
};

#endif // !defined(AFX_DxEFFECTNATURE_H__INCLUDED_)

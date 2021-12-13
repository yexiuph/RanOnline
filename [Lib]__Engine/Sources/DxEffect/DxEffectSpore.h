// DxEffectSpore.h: interface for the DxEffectFire class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DxEFFECTSPORE_H__INCLUDED_)
#define AFX_DxEFFECTSPORE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CMemPool.h"

#include "./DxEffectFrame.h"

#define	NATURE_WAIT		0
#define	NATURE_UPFLY	1
#define	NATURE_DOWNFLY	2
#define	NATURE_WALK		3
#define	NATURE_LANDING	4

#define	USESPORE		0x0001
#define	USEFIREFLY		0x0002

struct DxFrame;

struct SPORE_PROPERTY_100
{	
	int				m_nFlat;

	int				m_nSec;

	float			m_fScale;
	float			m_fSpeed;

	BOOL			m_bRotate;
	float			m_fRotate;

	float			m_fAlphaRate1;
	float			m_fAlphaRate2;
	float			m_fAlphaStart;
	float			m_fAlphaMid1;
	float			m_fAlphaMid2;
	float			m_fAlphaEnd;

	D3DXVECTOR3		m_vColor;

	float			m_fMaxX;
	float			m_fMaxY;
	float			m_fMaxZ;
	float			m_fMinX;
	float			m_fMinY;
	float			m_fMinZ;

	char			m_szSporeTex[MAX_PATH];

	SPORE_PROPERTY_100()
	{
		memset( m_szSporeTex, 0, sizeof(char)*MAX_PATH );
	}
};

struct SPORE_PROPERTY
{
	DWORD			m_dwFlags;

	int				m_nFlat;

	int				m_nSec;

	float			m_fScale;
	float			m_fSpeed;

	BOOL			m_bRotate;
	float			m_fRotate;

	float			m_fAlphaRate1;
	float			m_fAlphaRate2;
	float			m_fAlphaStart;
	float			m_fAlphaMid1;
	float			m_fAlphaMid2;
	float			m_fAlphaEnd;

	D3DXVECTOR3		m_vColor;

	D3DXVECTOR4		m_vFireFly_Color_1;		// ó�� ��
	D3DXVECTOR4		m_vFireFly_Color_2;		// ������ ��		 ���Ĵ� ���� ��������.

	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;

	char			m_szSporeTex[MAX_PATH];
	char			m_szFireFlyTex[MAX_PATH];

	SPORE_PROPERTY::SPORE_PROPERTY () :
		m_nFlat(0),
		m_nSec(10),
		m_bRotate(FALSE),
		m_fRotate(1.f),
		m_fScale(3.f),
		m_fSpeed(2.0f),
		m_fAlphaRate1(25.f),
		m_fAlphaRate2(75.f),
		m_fAlphaStart(1.f),
		m_fAlphaMid1(1.f),
		m_fAlphaMid2(1.f),
		m_fAlphaEnd(0.f),
		m_vColor(255,255,255),
		m_vFireFly_Color_1(130,255,130,255),
		m_vFireFly_Color_2(0,255,0,0),
		m_vMax(0.f,0.f,0.f),
		m_vMin(0.f,0.f,0.f)
	{
		m_dwFlags = 0L;

		StringCchCopy( m_szSporeTex,	MAX_PATH, "_Na_Min.tga" );
		StringCchCopy( m_szFireFlyTex,	MAX_PATH, "_Eff_flare.tga" );
	}
};


class DxEffectSpore : public DxEffectBase
{
	//	Note : ����Ʈ Ÿ�� ����.
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
	static LPDIRECT3DSTATEBLOCK9		m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9		m_pPointStateBlock;

	static LPDIRECT3DSTATEBLOCK9		m_pFireFlySB_S;
	static LPDIRECT3DSTATEBLOCK9		m_pFireFlySB_D;

	static LPDIRECT3DVERTEXBUFFERQ		m_pSporeVB;
	static LPDIRECT3DVERTEXBUFFERQ		m_pFireFlyVB;
	static LPDIRECT3DINDEXBUFFERQ		m_pIB;

	//	Note : �Ӽ�.
	//
protected:
	union
	{
		struct
		{
			SPORE_PROPERTY	m_Property;
		};

		struct
		{
			DWORD			m_dwFlags;

			int				m_nFlat;

			int				m_nSec;

			float			m_fScale;
			float			m_fSpeed;

			BOOL			m_bRotate;
			float			m_fRotate;

			float			m_fAlphaRate1;
			float			m_fAlphaRate2;
			float			m_fAlphaStart;
			float			m_fAlphaMid1;
			float			m_fAlphaMid2;
			float			m_fAlphaEnd;

			D3DXVECTOR3		m_vColor;

			D3DXVECTOR4		m_vFireFly_Color_1;		// ó�� ��
			D3DXVECTOR4		m_vFireFly_Color_2;		// ������ ��		 ���Ĵ� ���� ��������.

			D3DXVECTOR3		m_vMax;
			D3DXVECTOR3		m_vMin;

			char			m_szSporeTex[MAX_PATH];
			char			m_szFireFlyTex[MAX_PATH];
		};
	};

public:
	void SetProperty ( SPORE_PROPERTY& Property )
	{
		m_Property = Property;
	}
	SPORE_PROPERTY& GetProperty () { return m_Property; }

protected:
	float	m_fTime;
	float	m_fElapsedTime;

	struct VERTEX
	{
		D3DXVECTOR3 p;       // vertex position
		D3DXVECTOR3 n;       // vertex normal
		D3DXVECTOR2 t;
	};


	struct PARTICLEVERTEX
	{
		D3DXVECTOR3 p;       // vertex position
		D3DCOLOR	d;       // vertex 
		D3DXVECTOR2 t;       // vertex 
		const static DWORD	FVF;
	};

	struct PARTICLE
	{
		D3DXVECTOR3 vNowPos;		// ���� ��ġ
		float		fPrevY;			// ���� �ڽ��� Y ��..
		int			iState;			// ������ �ϴ÷θ� ������ �� ��..
		int			iLeft;			// �������� ������ ��		// �ٽ� ���� �� �ٲ�
		int			iRight;			// ���������� ������ ��		// �ٽ� ���� �� �ٲ�
		float		fSpeed;			// �� �༮�� ���ǵ�			// �ٽ� ���� �� �ٲ�
		float		fLife;			// �� �༮�� ����			// �ٽ� ���� �� �ٲ�
		float		fTexRotate;		// �ؽ��� ȸ��
		int			nRand;			//
		D3DXVECTOR4	Diffuse;		//
		PARTICLE*	pNext;

		PARTICLE::PARTICLE()
		{
			pNext = NULL;
		}

		PARTICLE::~PARTICLE()
		{
			SAFE_DELETE ( pNext );
		}
	};

	struct FIREFLY
	{
		D3DXVECTOR3 vNowPos;		// ���� ��ġ
		D3DXVECTOR3 vDirect;		// ������ �ϴ� ����		- ���ؾ� �Ѵ�.
		float		fSpeed;			// �� �༮�� ���ǵ�
		float		fLife;			// �� �༮�� ����
		float		fChange;		// ������.
		float		fAplha;			// �� �༮�� ����
		float		fAplhaTime;		// ���� ���ϴ� �ð� 
		float		fSize;			// ó�� ������
		float		fRotate;		// ������ ȸ�� ������ �����ϸ� �� �� �̴�.
		D3DXVECTOR4 vColor;			// ó�� ��
		FIREFLY*	pNext;

		FIREFLY::FIREFLY() :
			vNowPos(0,0,0),
			vDirect(0,0,0),
			fSpeed(0),
			fLife(0),
			fChange(0),
			fAplha(0),
			fAplhaTime(0),
			fSize(0),
			fRotate(0),
			vColor(0,0,0,0)
		{
			pNext = NULL;
		}

		FIREFLY::~FIREFLY()
		{
			SAFE_DELETE ( pNext );
		}
	};

	float		m_fAlpha;
	float		m_fAlphaTime1;
	float		m_fAlphaTime2;
	float		m_fAlphaLDelta1;
	float		m_fAlphaLDelta2;
	float		m_fAlphaLDelta3;

	LPDIRECT3DTEXTUREQ	m_pSporeTex;
	LPDIRECT3DTEXTUREQ	m_pFireFlyTex;

	float				m_fSecOne;
	float				m_fAddSecOne;
	int					m_nAddNum;

	D3DXVECTOR4			m_vFireFly_Color_D;	// �÷��� ��Ÿ..!

	PARTICLE*			m_pParticleHead;
	FIREFLY*			m_pFireFlyHead;

	DWORD				m_dwFireFlyCount;

protected:
	static BOOL			VAILEDDEVICE;

	static const float	MAX_LIFE;
	static const DWORD	MAX_VERTEX;

	static const float	FIREFLY_SIZE;

	typedef CMemPool<DxEffectSpore::PARTICLE>	PARTICLEPOOL;
	static PARTICLEPOOL*						m_pParticlePool;

	typedef CMemPool<DxEffectSpore::FIREFLY>	FIREFLYPOOL;
	static FIREFLYPOOL*							m_pFireFlyPool;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

	static void PutSleepParticle ( PARTICLE* pParticle );
	static void PutSleepParticles ( PARTICLE* pParticles );

	static void PutSleepFireFly ( FIREFLY* pParticle );
	static void PutSleepFireFlys ( FIREFLY* pParticles );

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

	HRESULT RenderSpore ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderFireFly ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT ResetEdit ();

protected:
	HRESULT SetBoundBox ( LPD3DXMESH pMesh, D3DXMATRIX *matWorld );

	HRESULT CreateParticle ();
	HRESULT Update	( LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT CreateFireFly ();
	HRESULT UpdateFireFly ( LPDIRECT3DDEVICEQ pd3dDevice );

	float	CalcSurfaceArea	(VERTEX* pFV[3]);

public:
	DxEffectSpore();
	virtual ~DxEffectSpore();

//Note: OBJAABB �κκ� 
public:
	virtual void GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );

//	Note : DxFrameMesh �� ȿ���� DxLandMan �� ���� �� �� �뷮�� ���� ����Ÿ��
	//		�����ϱ� ���ؼ� ����Ѵ�.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(AFX_DxEFFECTSPORE_H__INCLUDED_)


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
	
	//	Note : ���̴�.
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

	//	Note : �Ӽ�.
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
		D3DXVECTOR3 vNowPos;		// ���� ��ġ
		D3DXVECTOR3 vTargetPos;		// ���� ���� ��
		D3DXVECTOR3	vPrevPos;		// ���� ��ġ
		int			iState;			// �ɾ� ������, ���� �Ʒ��� ����	0, 1, 2
		int			iLeft;			// �������� ������ ��		// �ٽ� ���� �� �ٲ�
		int			iRight;			// ���������� ������ ��		// �ٽ� ���� �� �ٲ�
		float		fSpeed;			// �� �༮�� ���ǵ�			// �ٽ� ���� �� �ٲ�
		float		fDot3;			// �� �༮�� ������ ����	// �ٽ� ���� �� �ٲ�
		float		fBaseLenth;		// �༮�� ������ġ���� ������ �ϴ°� ��ġ������ �Ÿ�.
	};

	struct PARTICLE_BUG
	{
		D3DXVECTOR3 vNowPos;		// ���� ��ġ
		D3DXVECTOR3 vTargetPos;		// ���� ���� ��
		int			iState;			// �ɾ� ������, ���� �Ʒ��� ����	0, 1, 2
		int			iLeft;			// �������� ������ ��		// �ٽ� ���� �� �ٲ�
		int			iRight;			// ���������� ������ ��		// �ٽ� ���� �� �ٲ�
		float		fSpeed;			// �� �༮�� ���ǵ�			// �ٽ� ���� �� �ٲ�
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
		D3DXVECTOR3		vNowPos;		// ���� ��ġ
		D3DXVECTOR3		vTargetPos;		// ���� ���� ��
		int				iState;			// �ɾ� ������, ���� �Ʒ��� ����	0, 1, 2
		int				iLeft;			// �������� ������ ��		// �ٽ� ���� �� �ٲ�
		int				iRight;			// ���������� ������ ��		// �ٽ� ���� �� �ٲ�
		float			fSpeed;			// �� �༮�� ���ǵ�			// �ٽ� ���� �� �ٲ�
		int				Group;			// �� �� ... ^^;
	};
*/

//	LEAFVERTEX*	m_pLeafVert;

/*
	PARTICLE_LEAF*				m_pLeafVert;	// ������ ������
	LPDIRECT3DVERTEXBUFFERQ		m_pLeafVB;
	LPDIRECT3DINDEXBUFFERQ		m_pLeafIB;
	LPDIRECT3DTEXTUREQ			m_pLeafTex;
*/

	BASEVERTEX*	m_pBaseVert;		// �̵� �� �� �ִ� ����..

	DWORD		m_dwFlowerNum;
	DWORD		m_dwButterNum;
	DWORD		m_dwBugNum;

	PARTICLE_BUTTERFLY*			m_pButterVert;	// ���� ������
	LPDIRECT3DVERTEXBUFFERQ		m_pButterVB;
	LPDIRECT3DINDEXBUFFERQ		m_pButterIB;
	LPDIRECT3DTEXTUREQ			m_pButterTex;

	PARTICLE*					m_pButterShadowVert;	// ���� �׸��� ��
	LPDIRECT3DVERTEXBUFFERQ		m_pButterShadowVB;
	LPDIRECT3DINDEXBUFFERQ		m_pButterShadowIB;

	LPDIRECT3DTEXTUREQ			m_pShadowTex;

	int							m_iNowSpriteBug;	// ���� �༮�� ��������Ʈ ��ȣ
	float						m_fAniTimeBug;		// �̳༮�� ������ ���ư��� �ӵ�
	float						m_fTimeBug;

	PARTICLE_BUG*				m_pBugVert;	// ���� ������
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

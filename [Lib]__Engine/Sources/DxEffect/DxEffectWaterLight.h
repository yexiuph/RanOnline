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
	bool		m_bBaseRender;			// ������ �Ѹ� �� �ΰ� ??
	float		m_fVel;					// �̵� �ӵ�
	float		m_fScale;				// ũ��
	D3DXVECTOR3	m_vDir;					// ����
	D3DXVECTOR3	m_vColor;				// �÷�
	int			m_iAlphaUp;		// ���� ��
	int			m_iAlphaMiddle;	// ���� ��
	int			m_iAlphaDown;	// ���� ��
	char		m_szTexMove[MAX_PATH];		// �̵��ϴ� Tex
	char		m_szTexRotate[MAX_PATH];	// �����̼� �ϴ� Tex

	WATERLIGHT_PROPERTY_100()
	{
		memset( m_szTexMove, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexRotate, 0, sizeof(char)*MAX_PATH );
	}
};

struct WATERLIGHT_PROPERTY
{
	bool		m_bBaseRender;			// ������ �Ѹ� �� �ΰ� ??
	float		m_fVel;					// �̵� �ӵ�
	float		m_fScale;				// ũ��
	D3DXVECTOR3	m_vDir;					// ����
	D3DXVECTOR3	m_vColor;				// �÷�
	int			m_iAlphaUp;		// ���� ��
	int			m_iAlphaMiddle;	// ���� ��
	int			m_iAlphaDown;	// ���� ��

	float		m_fMaxX;
	float		m_fMinX;
	float		m_fMaxY;
	float		m_fMinY;
	float		m_fMaxZ;
	float		m_fMinZ;

	char		m_szTexMove[MAX_PATH];		// �̵��ϴ� Tex
	char		m_szTexRotate[MAX_PATH];	// �����̼� �ϴ� Tex

	WATERLIGHT_PROPERTY()
	{
		memset( m_szTexMove, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexRotate, 0, sizeof(char)*MAX_PATH );
	}
};


class DxEffectWaterLight : public DxEffectBase
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

public:
	void SetProperty ( WATERLIGHT_PROPERTY& Property )
	{
		m_Property = Property;
	}
	WATERLIGHT_PROPERTY& GetProperty () { return m_Property; }


protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;


	//	Note : �Ӽ�.
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
			bool		m_bBaseRender;			// ������ �Ѹ� �� �ΰ� ??
			float		m_fVel;	// �̵� �ӵ�
			float		m_fScale;	// ũ��
			D3DXVECTOR3	m_vDir;	// ����
			D3DXVECTOR3	m_vColor;	// �÷�
			int			m_iAlphaUp;		// ���� ��
			int			m_iAlphaMiddle;	// ���� ��
			int			m_iAlphaDown;	// ���� ��

			float		m_fMaxX;
			float		m_fMinX;
			float		m_fMaxY;
			float		m_fMinY;
			float		m_fMaxZ;
			float		m_fMinZ;

			char		m_szTexMove[MAX_PATH];		// �̵��ϴ� Tex
			char		m_szTexRotate[MAX_PATH];	// �����̼� �ϴ� Tex
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

	LPDIRECT3DVERTEXBUFFERQ	m_pVB;			// ����
	LPDIRECT3DINDEXBUFFERQ	m_pIB;			// ����

	DWORD	m_dwVertices;					// ����
	DWORD	m_dwFaces;						// ����

	DIFFVERTEX*			m_pDiffVertex;		// ����
	DWORD				m_pDiffNum;			// ����

	LPDIRECT3DTEXTUREQ	m_pddsTexMove;
	LPDIRECT3DTEXTUREQ	m_pddsTexRotate;

	D3DXVECTOR2*		m_pTexUV;			// ����	// �ؽ��� �ʱ� ���� ���ؼ�

	D3DXVECTOR2			m_vAddTex1;		// ���� �� - �̵�
	D3DXVECTOR2			m_vAddTex2;		// ���� �� - ȸ��

	float				m_fHighHeight;		// ����	- �� �޽��� �ְ� ��ġ
	float				m_fMiddleHeight;	// ����	- �� �޽��� �ְ� ��ġ
	float				m_fLowerHeight;		// ���� - �� �޽��� ���� ��ġ

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

	//Note: OBJAABB �κκ� 
public:
	virtual void GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh �� ȿ���� DxLandMan �� ���� �� �� �뷮�� ���� ����Ÿ��
	//		�����ϱ� ���ؼ� ����Ѵ�.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(AFX_DXEFFECTWATERLIGHT_H__INCLUDED_)
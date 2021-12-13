//  interface for the DxEffectSpecular class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DXEFFCTNEON_INCLUDED__)
#define __DXEFFCTNEON_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxMeshes;

struct DXMATERIAL_NEON
{
	BOOL				bUse;
	D3DXVECTOR2			vMoveTex;		// �̵�
	D3DXVECTOR2			vScaleUV;		// ������ ��
	D3DCOLOR			vColor;
	LPDIRECT3DTEXTUREQ	pSrcTex;
	LPDIRECT3DTEXTUREQ	pNeonTex;
	char				szTexture[MAX_PATH];
	char				szNeonTex[MAX_PATH];

	DXMATERIAL_NEON () :
		bUse(FALSE),
		vColor(0xffffffff),
		vMoveTex(1.f,0.f),
		vScaleUV(1.f,1.f),
		pSrcTex(NULL),
		pNeonTex(NULL)
	{
		memset( szTexture, 0, sizeof(char)*MAX_PATH );
		memset( szNeonTex, 0, sizeof(char)*MAX_PATH );
	}

	~DXMATERIAL_NEON();
};

struct NEON_PROPERTY
{
	D3DXMATRIX		m_matFrameComb;

	DWORD			m_dwMaterials;

	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
};

class DxEffectNeon : public DxEffectBase
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
	static LPDIRECT3DSTATEBLOCK9	m_pSB;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_SAVE;

public:
	void SetProperty ( NEON_PROPERTY& Property )
	{
		m_Property = Property;
	}
	NEON_PROPERTY& GetProperty () { return m_Property; }

	//	Note : �Ӽ�.
	//
protected:
	union
	{
		struct
		{
			NEON_PROPERTY	m_Property;
		};

		struct
		{
			D3DXMATRIX		m_matFrameComb;

			DWORD			m_dwMaterials;

			D3DXVECTOR3		m_vMax;
			D3DXVECTOR3		m_vMin;
		};
	};

protected:
	float					m_fTime;
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

protected:
	struct OBJECT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR2		vTex;
		static const DWORD	FVF;
	};

	struct OBJECTNORMAL
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		D3DXVECTOR2		vTex;
		static const DWORD	FVF;
	};

	LPDIRECT3DVERTEXBUFFERQ		m_pVB;
	LPDIRECT3DVERTEXBUFFERQ		m_pSrcVB;
	LPDIRECT3DINDEXBUFFERQ		m_pIB;			// ����

	DWORD						m_dwVertices;					// ����
	DWORD						m_dwFaces;						// ����

	D3DXVECTOR3					m_vCenter;

	D3DXVECTOR2					m_vAddTex;		// ���� �� - �̵�

	DWORD						m_dwAttribTable;
	D3DXATTRIBUTERANGE*			m_pAttribTable;
	DXMATERIAL_NEON*			m_pMaterials;			// ��Ʈ���� ����

public:
	DXMATERIAL_NEON*			GetMaterials()	{ return m_pMaterials; }
	void						ResetTexture ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT DxAnalysis ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs );

public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

protected:
	HRESULT	CreateObject( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs );
	HRESULT	ChangeObject ( LPDIRECT3DDEVICEQ pd3dDevice, float fScale );

public:
	DxEffectNeon ();
	virtual ~DxEffectNeon ();

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh �� ȿ���� DxLandMan �� ���� �� �� �뷮�� ���� ����Ÿ��
	//		�����ϱ� ���ؼ� ����Ѵ�.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT SetBoundBox ();
	virtual void	GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual	BOOL	IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );

};

#endif // !defined(__DXEFFCTNEON_INCLUDED__)

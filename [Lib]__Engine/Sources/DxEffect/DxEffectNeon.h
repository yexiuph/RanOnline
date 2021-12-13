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
	D3DXVECTOR2			vMoveTex;		// 이동
	D3DXVECTOR2			vScaleUV;		// 스케일 용
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
	void SetProperty ( NEON_PROPERTY& Property )
	{
		m_Property = Property;
	}
	NEON_PROPERTY& GetProperty () { return m_Property; }

	//	Note : 속성.
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
	LPDIRECT3DINDEXBUFFERQ		m_pIB;			// 저장

	DWORD						m_dwVertices;					// 저장
	DWORD						m_dwFaces;						// 저장

	D3DXVECTOR3					m_vCenter;

	D3DXVECTOR2					m_vAddTex;		// 누적 값 - 이동

	DWORD						m_dwAttribTable;
	D3DXATTRIBUTERANGE*			m_pAttribTable;
	DXMATERIAL_NEON*			m_pMaterials;			// 매트리얼 정보

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

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT SetBoundBox ();
	virtual void	GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual	BOOL	IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );

};

#endif // !defined(__DXEFFCTNEON_INCLUDED__)

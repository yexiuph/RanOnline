// Terrain0.h: interface for the DxEffectWater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DXEFFECTWATER__INCLUDED__)
#define __DXEFFECTWATER__INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct WATER_PROPERTY_100
{
	float			m_SizeX, m_SizeZ;
	int				m_col, m_row;

	float			m_fWaveCycle;
	float			m_Velocity;
	float			m_HeightChange;

	float			m_Depth;
	D3DCOLOR		m_DiffuseColor;
	D3DFILLMODE		m_d3dFillMode;
	char			m_szTexture[MAX_PATH];

	WATER_PROPERTY_100()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct WATER_PROPERTY_101
{
	float			m_SizeX, m_SizeZ;
	int				m_col, m_row;

	float			m_fWaveCycle;
	float			m_Velocity;
	float			m_HeightChange;

	D3DXMATRIX		m_matFrameComb;

	float			m_Depth;
	D3DCOLOR		m_DiffuseColor;
	D3DFILLMODE		m_d3dFillMode;
	char			m_szTexture[MAX_PATH];

	WATER_PROPERTY_101()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

class DxEffectWater : public DxEffectBase
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

	//	Note : 워터 버텍스 구조.
	//
public :
	struct D3DWATERVERTEX
	{
		D3DXVECTOR3 vertex;
		D3DXVECTOR3 normal;
		DWORD diffuse;
		float tu, tv;

		D3DWATERVERTEX () {}

		const static DWORD FVF;
	};

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			WATER_PROPERTY_101	m_Property;
		};

		struct
		{
			float				m_SizeX, m_SizeZ;
			int					m_col, m_row;

			float				m_fWaveCycle;
			float				m_Velocity;
			float				m_HeightChange;

			D3DXMATRIX			m_matFrameComb;

			float				m_Depth;
			D3DCOLOR			m_DiffuseColor;
			D3DFILLMODE			m_d3dFillMode;
			char				m_szTexture[MAX_PATH];
		};
	};

	float					m_CellSizeX, m_CellSizeZ;

	//	Note : 랜더링 데이터
	//
protected:
	static	LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static	LPDIRECT3DSTATEBLOCK9	m_pDrawStateBlock;

	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	D3DMATERIALQ			m_Material;
	LPDIRECT3DTEXTUREQ		m_pddsTexture;

	DWORD					m_dwNumIndices;
	DWORD					m_dwNumVertices;
	LPDIRECT3DVERTEXBUFFERQ	m_pVB;
	LPDIRECT3DINDEXBUFFERQ	m_pIB;

	float					m_distance;
	float					*m_pActiveHeightArray;

public:
	void SetProperty ( WATER_PROPERTY_101& Property )
	{
		m_Property = Property;

		m_CellSizeX = m_SizeX / (float)m_col;
		m_CellSizeZ = m_SizeZ / (float)m_row;
	}
	WATER_PROPERTY_101& GetProperty () { return m_Property; }

	void SetDiffuseColor ( D3DCOLOR &Diffuse )		{ m_DiffuseColor = Diffuse; }
	void SetVelocity ( float velocity )				{ m_Velocity = velocity; }
	void SetWaveDepth ( float Depth )				{ m_Depth = Depth; }
	void SetWireMode ( BOOL bFlag )					{ bFlag?m_d3dFillMode=D3DFILL_WIREFRAME:m_d3dFillMode=D3DFILL_SOLID; }
	
	void SetColRow ( int col, int row )
	{
		m_col = col; m_row = row;

		m_CellSizeX = m_SizeX / (float)m_col;
		m_CellSizeZ = m_SizeZ / (float)m_row;
	}
	
	void SetSize ( float SizeX, float SizeZ )
	{
		m_SizeX = SizeX; m_SizeZ = SizeZ;

		m_CellSizeX = m_SizeX / (float)m_col;
		m_CellSizeZ = m_SizeZ / (float)m_row;
	}

	void SetTexture ( const char* szName );

protected:
	HRESULT CreateVertex ();
	HRESULT ApplyHeightArrayToVB ();
	float CalculateDisplacement ( float Height );

public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

public:
	DxEffectWater ();
	virtual ~DxEffectWater ();

	//	Note : OBJAABB
	//
public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
};

#endif // !defined(__DXEFFECTWATER__INCLUDED__)

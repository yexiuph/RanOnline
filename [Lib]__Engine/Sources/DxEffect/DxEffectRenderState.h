// DxEffectRenderState.h: interface for the DxEffectShadowHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTRENDERSTATE_H__INCLUDED_)
#define AFX_DXEFFECTRENDERSTATE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxOcMeshes;
struct DxFrame;

struct RENDERSTATE_PROPERTY_100
{
	DWORD	m_FillMode;
	DWORD	m_CullMode;
	DWORD	m_Lighting; 
	DWORD	m_ZWriteEnable;
	DWORD	m_AlphaBlendeEnable;
	DWORD	m_SrcBlend; 
	DWORD	m_DestBlend;
	DWORD	m_ColorOP_0;
	DWORD	m_ColorARG1_0;
	DWORD	m_ColorARG2_0;
	DWORD	m_AlphaOP_0;
	DWORD	m_AlphaARG1_0;
	DWORD	m_AlphaARG2_0;
};

struct RENDERSTATE_PROPERTY_101
{
	int		m_nType;
	DWORD	m_FillMode;
	DWORD	m_CullMode;
	DWORD	m_Lighting; 
	DWORD	m_ZWriteEnable;
	DWORD	m_AlphaBlendeEnable;
	DWORD	m_SrcBlend; 
	DWORD	m_DestBlend;
	DWORD	m_ColorOP_0;
	DWORD	m_ColorARG1_0;
	DWORD	m_ColorARG2_0;
	DWORD	m_AlphaOP_0;
	DWORD	m_AlphaARG1_0;
	DWORD	m_AlphaARG2_0;
};

struct RENDERSTATE_PROPERTY_102
{
	int		m_nType;

	float		m_fMaxX;
	float		m_fMinX;
	float		m_fMaxY;
	float		m_fMinY;
	float		m_fMaxZ;
	float		m_fMinZ;
	D3DXMATRIX	m_matFrameComb;

	DWORD	m_FillMode;
	DWORD	m_CullMode;
	DWORD	m_Lighting; 
	DWORD	m_ZWriteEnable;
	DWORD	m_AlphaBlendeEnable;
	DWORD	m_SrcBlend; 
	DWORD	m_DestBlend;
	DWORD	m_ColorOP_0;
	DWORD	m_ColorARG1_0;
	DWORD	m_ColorARG2_0;
	DWORD	m_AlphaOP_0;
	DWORD	m_AlphaARG1_0;
	DWORD	m_AlphaARG2_0;
};

//struct RENDERSTATE_PROPERTY_103	// RENDERSTATE_PROPERTY_102 과 같아서 안 만들었다.

struct RENDERSTATE_PROPERTY
{
	int			m_nType;
	DWORD		m_FillMode;
	DWORD		m_CullMode;
	DWORD		m_Lighting; 
	DWORD		m_ZWriteEnable;
	DWORD		m_AlphaBlendeEnable;
	DWORD		m_SrcBlend; 
	DWORD		m_DestBlend;
	DWORD		m_ColorOP_0;
	DWORD		m_ColorARG1_0;
	DWORD		m_ColorARG2_0;
	DWORD		m_AlphaOP_0;
	DWORD		m_AlphaARG1_0;
	DWORD		m_AlphaARG2_0;

	float		m_fSpeed;		// 적을수록 빠르다.
	float		m_fColorPower;	// 음..
	int			m_nRate;		// 0~100 까지
	BOOL		m_bLight;
	BOOL		m_bNearLight;
	D3DXVECTOR3	m_vMax;
	D3DXVECTOR3	m_vMin;
	D3DXMATRIX	m_matFrameComb;
};

class DxEffectRenderState : public DxEffectBase
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
	void SetProperty ( RENDERSTATE_PROPERTY& Property )
	{
		m_Property = Property;
	}
	RENDERSTATE_PROPERTY& GetProperty () { return m_Property; }

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			RENDERSTATE_PROPERTY	m_Property;
		};

		struct
		{
			int			m_nType;
			DWORD		m_FillMode;
			DWORD		m_CullMode;
			DWORD		m_Lighting; 
			DWORD		m_ZWriteEnable;
			DWORD		m_AlphaBlendeEnable;
			DWORD		m_SrcBlend; 
			DWORD		m_DestBlend;
			DWORD		m_ColorOP_0;
			DWORD		m_ColorARG1_0;
			DWORD		m_ColorARG2_0;
			DWORD		m_AlphaOP_0;
			DWORD		m_AlphaARG1_0;
			DWORD		m_AlphaARG2_0;

			float		m_fSpeed;		// 적을수록 빠르다.
			float		m_fColorPower;	// 음..
			int			m_nRate;		// 0~100 까지
			BOOL		m_bLight;
			BOOL		m_bNearLight;
			D3DXVECTOR3	m_vMax;
			D3DXVECTOR3	m_vMin;
			D3DXMATRIX	m_matFrameComb;
		};
	};

protected:

	struct VERTEX
	{
		D3DXVECTOR3 p;       // vertex position
		D3DXVECTOR3 n;       // vertex normal
		D3DXVECTOR2 t;
		const static DWORD	FVF;
	};

protected:
	DxOcMeshes*		m_pOcMesh;

	DXLIGHT*		m_pLight;

	BOOL			m_bUse;
	float			m_fElapsedTime;

	LPDIRECT3DDEVICEQ _pd3dDevice;

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

protected:
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT CloneRenderState ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, D3DXMATRIX	*matComb );

protected:
	HRESULT SetBoundBox ( LPD3DXMESH pMesh );
	void	GetLight ();

public:
	DxEffectRenderState();
	virtual ~DxEffectRenderState();

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

#endif // !defined(AFX_DXEFFECTRENDERSTATE_H__INCLUDED_)
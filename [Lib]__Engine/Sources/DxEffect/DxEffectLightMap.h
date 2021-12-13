// DxEffectLightMap.h: interface for the DxEffectLightMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTLIGHTMAP_H__03600E97_AAF0_4A78_B1BD_E80EF78F6E32__INCLUDED_)
#define AFX_DXEFFECTLIGHTMAP_H__03600E97_AAF0_4A78_B1BD_E80EF78F6E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxFrame;

struct LIGHTMAP_PROPERTY
{
	bool			m_bRender;
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
};


class DxEffectLightMap : public DxEffectBase
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

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			LIGHTMAP_PROPERTY	m_Property;
		};
		struct
		{
			bool			m_bRender;
			D3DXVECTOR3		m_vMax;
			D3DXVECTOR3		m_vMin;
		};
	};

public:
	void SetProperty ( LIGHTMAP_PROPERTY& Property )
	{
		m_Property = Property;
	}
	LIGHTMAP_PROPERTY& GetProperty () { return m_Property; }

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	BOOL		m_bUse0, m_bUse1, m_bUse2, m_bUse3, m_bUse4, m_bUse5, m_bUse6, m_bUse7;
	D3DLIGHTQ	m_sLight0, m_sLight1, m_sLight2, m_sLight3, m_sLight4, m_sLight5, m_sLight6, m_sLight7;

protected:
	HRESULT	CreateVB ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pmsMeshs );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );	
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

public:
	DxEffectLightMap();
	virtual ~DxEffectLightMap();
};

#endif // !defined(AFX_DXEFFECTLIGHTMAP_H__03600E97_AAF0_4A78_B1BD_E80EF78F6E32__INCLUDED_)

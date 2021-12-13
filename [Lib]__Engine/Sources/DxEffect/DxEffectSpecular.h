//  interface for the DxEffectSpecular class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DXEFFCTSPECULAR_INCLUDED__)
#define __DXEFFCTSPECULAR_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxMeshes;

struct SPECULAR_PROPERTY
{
	BOOL		m_bAll;
};

class DxEffectSpecular : public DxEffectBase
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
	static	LPDIRECT3DSTATEBLOCK9		m_pSavedStateBlock;
	static	LPDIRECT3DSTATEBLOCK9		m_pDrawStateBlock;
	static	LPDIRECT3DSTATEBLOCK9		m_pSavedALLSB;
	static	LPDIRECT3DSTATEBLOCK9		m_pDrawALLSB;

public:
	void SetProperty ( SPECULAR_PROPERTY& Property )
	{
		m_Property = Property;
	}
	SPECULAR_PROPERTY& GetProperty () { return m_Property; }

	//	Note : 속성.
	//
protected:
	union
	{
		SPECULAR_PROPERTY	m_Property;

		struct
		{
			BOOL		m_bAll;
		};
	};

protected:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

public:
	DxEffectSpecular ();
	virtual ~DxEffectSpecular ();

	//	Note : OBJAABB
	//
//public:
//	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
//	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
//
//public:
//	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(__DXEFFCTSPECULAR_INCLUDED__)

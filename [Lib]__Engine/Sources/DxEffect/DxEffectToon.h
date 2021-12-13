// DxEffectToon.h: interface for the DxEffectToon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTTOON_H__D0DA0A48_41B0_40B6_93AF_B264F8CE1CC7__INCLUDED_)
#define AFX_DXEFFECTTOON_H__D0DA0A48_41B0_40B6_93AF_B264F8CE1CC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxFrame;

struct CARTOON_PROPERTY
{
	float	m_fNum1;
	float	m_fNum2;
	float	m_fNum3;
	float	m_fNum4;
	char	m_szTexture[MAX_PATH];

	CARTOON_PROPERTY()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};


class DxEffectToon : public DxEffectBase
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
	static DWORD			m_dwEffect;
	static DWORD			dwToonVertexShader[];

	static LPDIRECT3DSTATEBLOCK9		m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9		m_pEffectStateBlock;

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			CARTOON_PROPERTY	m_Property;
		};
		struct
		{
			float	m_fNum1;
			float	m_fNum2;
			float	m_fNum3;
			float	m_fNum4;
			char	m_szTexture[MAX_PATH];
		};
	};

public:
	void SetProperty ( CARTOON_PROPERTY& Property )
	{
		m_Property = Property;
	}
	CARTOON_PROPERTY& GetProperty () { return m_Property; }

protected:
	LPDIRECT3DTEXTUREQ	m_pddsTexture;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

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
	DxEffectToon();
	virtual ~DxEffectToon();
};

#endif // !defined(AFX_DXEFFECTTOON_H__D0DA0A48_41B0_40B6_93AF_B264F8CE1CC7__INCLUDED_)

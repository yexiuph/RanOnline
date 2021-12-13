// DxEffectFur.h: interface for the DxEffectToon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTFUR_H__INCLUDED_)
#define AFX_DXEFFECTFUR_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxFrame;

struct FUR_PROPERTY
{
	int					m_iLayers;
	float				m_fTexX;
	float				m_fTexY;
	float				m_fOffset;
	char				m_szTexture[MAX_PATH];

	FUR_PROPERTY()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	}
};

class DxEffectFur : public DxEffectBase
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
	static DWORD					m_dwEffect;
	static DWORD					dwFurVertexShader[];
	static LPDIRECT3DSTATEBLOCK9	m_pSB;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_SAVE;

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			FUR_PROPERTY	m_Property;
		};
		struct
		{
			int				m_iLayers;
			float			m_fTexX;
			float			m_fTexY;
			float			m_fOffset;
			char			m_szTexture[MAX_PATH];
		};
	};

public:
	void SetProperty ( FUR_PROPERTY& Property )
	{
		m_Property = Property;
	}
	FUR_PROPERTY& GetProperty () { return m_Property; }

protected:	
	float				m_fTime;

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
	DxEffectFur();
	virtual ~DxEffectFur();
};

#endif // !defined(AFX_DXEFFECTFUR_H__INCLUDED_)

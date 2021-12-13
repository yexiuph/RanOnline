// DxEffectDot3.h: interface for the DxEffectDot3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTDOT3_H__03600E97_AAF0_4A78_B1BD_E80EF78F6E32__INCLUDED_)
#define AFX_DXEFFECTDOT3_H__03600E97_AAF0_4A78_B1BD_E80EF78F6E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxFrame;

struct DOT3_PROPERTY
{
	bool			m_bDot3;	// 내적 블렌딩 혹은 디테일 맵
	char			m_szTexDetail[MAX_PATH];
	char			m_szTexture[MAX_PATH];
	float			m_fPower;
	float			m_fTexX;
	float			m_fTexY;

	DOT3_PROPERTY()
	{
		memset( m_szTexDetail, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	}
};


class DxEffectDot3 : public DxEffectBase
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
	static DWORD		m_dwEffect;
	static DWORD		dwDot3VertexShader[];

	static LPDIRECT3DSTATEBLOCK9 m_pDetailSB;
	static LPDIRECT3DSTATEBLOCK9 m_pDetailSB_SAVE;
	static LPDIRECT3DSTATEBLOCK9 m_pEffectSB;
	static LPDIRECT3DSTATEBLOCK9 m_pEffectSB_SAVE;	

	//	Note : 속성.
protected:
	union
	{
		struct
		{
			DOT3_PROPERTY	m_Property;
		};
		struct
		{
			bool			m_bDot3;
			char			m_szTexDetail[MAX_PATH];
			char			m_szTexture[MAX_PATH];
			float			m_fPower;
			float			m_fTexX;
			float			m_fTexY;
		};
	};

public:
	void SetProperty ( DOT3_PROPERTY& Property )
	{
		m_Property = Property;
	}
	DOT3_PROPERTY& GetProperty () { return m_Property; }

protected:
	LPDIRECT3DTEXTUREQ	m_pddsTexDetail;
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
	DxEffectDot3();
	virtual ~DxEffectDot3();
};

#endif // !defined(AFX_DXEFFECTDOT3_H__03600E97_AAF0_4A78_B1BD_E80EF78F6E32__INCLUDED_)

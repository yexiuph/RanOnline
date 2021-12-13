#pragma once

#include "DxEffChar.h"

#include "./TextureManager.h"
#include "DxMethods.h"

#define	USETRANSLUCENT		0x0001		// 반투명 옵션을 켠다.

struct EFFCHAR_PROPERTY_ALPHA_100
{
	float	m_fPlayTime;
};

struct EFFCHAR_PROPERTY_ALPHA_101
{
	float	m_fWaitTime;
	float	m_fPlayTime;
	float	m_fHeight;	
};

struct EFFCHAR_PROPERTY_ALPHA : public EFFCHAR_PROPERTY
{
	DWORD	m_dwFlags;
	float	m_fWaitTime;
	float	m_fPlayTime;
	float	m_fHeight;
	float	m_fAlpha;	

	EFFCHAR_PROPERTY_ALPHA () :
		m_fWaitTime(2.f),
		m_fPlayTime(1.f),
		m_fHeight(50.f),
		m_fAlpha(0.f)
	{
		m_dwFlags = 0L;
	}
};

class DxEffCharAlpha : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_ALPHA; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_ALPHA m_Property;
		};
		struct
		{
			DWORD	m_dwFlags;
			float	m_fWaitTime;
			float	m_fPlayTime;
			float	m_fHeight;
			float	m_fAlpha;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_ALPHA*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	DWORD m_dwZWriteEnable;
	DWORD m_dwFogEnable;
	DWORD m_dwAlphaBlendEnable;
	DWORD m_dwSecBlend;
	DWORD m_dwDestBlend;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

protected:
	float	m_fElapsedTime;
	float	m_fTimeSum;

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_ALPHA; }
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA );
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharAlpha(void);
	~DxEffCharAlpha(void);
};

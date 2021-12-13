#pragma once

#include "DxEffChar.h"

#include "./TextureManager.h"
#include "DxMethods.h"

#define	USEALL			0x0001
#define	USETEXCOLOR		0x0002

struct EFFCHAR_PROPERTY_AMBIENT_100
{
	DWORD		m_dwColorOP;
	float		m_fFullTime;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cAmbient;
};

struct EFFCHAR_PROPERTY_AMBIENT_101
{
	DWORD		m_dwFlage;
	DWORD		m_dwColorOP;
	int			m_nFrame;
	float		m_fFullTime;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cAmbient;
};

struct EFFCHAR_PROPERTY_AMBIENT_102
{
	DWORD		m_dwFlage;
	DWORD		m_dwColorOP;
	float		m_fFullTime;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cAmbient;
};

struct EFFCHAR_PROPERTY_AMBIENT : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlage;
	DWORD		m_dwColorOP;			// 삭제 하자.
	float		m_fFullTime;
	float		m_fSrcColor;			// 삭제 하자.
	D3DXCOLOR	m_cColor;

	EFFCHAR_PROPERTY_AMBIENT () :
		m_dwFlage(0),
		m_dwColorOP(D3DTOP_MODULATE),
		m_fFullTime ( 0.06f ),
		m_fSrcColor ( 0.4f ),
		m_cColor(D3DCOLOR_ARGB(0,255,0,0))
	{
		m_dwFlage |= USEALL;
	}
};

class DxEffCharAmbient : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_AMBIENT; }
	virtual const char* GetName ()	{ return NAME; }

	D3DXCOLOR	  GetColor()		   { return m_cColor; }
	bool		  GetUseTextureColor() { if( m_dwFlage & USETEXCOLOR ) return TRUE; else return FALSE; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_AMBIENT m_Property;
		};
		struct
		{
			DWORD		m_dwFlage;
			DWORD		m_dwColorOP;
			float		m_fFullTime;
			float		m_fSrcColor;
			D3DXCOLOR	m_cColor;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_AMBIENT*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	float	m_fElapsedTime;

	DWORD	m_dwOldTexFactor;
	DWORD	m_dwOldColorARG1;
	DWORD	m_dwOldColorARG2;
	DWORD	m_dwOldColorOP;
	DWORD	m_dwOldAmbient;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_AMBIENT; }
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA );
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharAmbient(void);
	~DxEffCharAmbient(void);
};

#pragma once

#include "DxEffChar.h"

#define		TEXTURE_LENGTH	128

#define		USETEXTURE		0x0001

struct EFFCHAR_PROPERTY_USERCOLOR_100 : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	D3DXCOLOR	m_cColor;
};

struct EFFCHAR_PROPERTY_USERCOLOR : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlage;
	D3DXCOLOR	m_cColor;
	DWORD		m_dwMaterials;

	EFFCHAR_PROPERTY_USERCOLOR () :
		m_cColor(D3DCOLOR_ARGB(0,255,255,255)),
		m_dwMaterials(0L)
	{
		m_dwFlage = 0L;
	}
};

class DxEffCharUserColor : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_USERCOLOR m_Property;
		};
		struct
		{
			DWORD		m_dwFlage;
			D3DXCOLOR	m_cColor;
			DWORD		m_dwMaterials;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_USERCOLOR*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

private:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedBaseSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectBaseSB;

private:
	DXMATERIAL_CHAR_EFF*	m_pMaterials;
	DXMATERIAL_CHAR_EFF*	m_pTempMaterials;
	DWORD					m_dwTempMaterials;

private:
	DxSkinMesh9*		m_pSkinMesh;
	PSMESHCONTAINER		m_pmcMesh;

public:
	virtual void SetMaterials( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials );
	virtual DXMATERIAL_CHAR_EFF*		GetMaterials () { return m_pMaterials; }

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender=FALSE );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharUserColor(void);
	~DxEffCharUserColor(void);
};

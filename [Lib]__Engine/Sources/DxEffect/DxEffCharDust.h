#pragma once

#include "DxEffChar.h"

#include "./TextureManager.h"
#include "DxMethods.h"

struct EFFCHAR_PROPERTY_DUST : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_DUST ()
	{
		m_dwFlag = 0L;
		StringCchCopy( m_szTexture, MAX_PATH, "Rain.dds" );	//
	}
};

class DxEffCharDust : public DxEffChar
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
			EFFCHAR_PROPERTY_DUST m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			char			m_szTexture[MAX_PATH];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_DUST*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedBaseSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectBaseSB;

protected:
	DxSkinMesh9*			m_pSkinMesh;
	PSMESHCONTAINER			m_pmcMesh;

protected:
	LPDIRECT3DTEXTUREQ		m_pTexture;
	DXMATERIAL_CHAR_EFF*	m_pMaterials;
	DWORD					m_dwMaterials;

	DWORD					m_dwMipMapCount;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
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
	DxEffCharDust(void);
	~DxEffCharDust(void);
};

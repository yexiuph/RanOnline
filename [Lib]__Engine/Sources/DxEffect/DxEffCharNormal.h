#pragma once

#include "DxEffChar.h"

	#include "./TextureManager.h"
#include "DxMethods.h"
#include "DxMaterialHLSL.h"

struct EFFCHAR_PROPERTY_NORMAL : public EFFCHAR_PROPERTY
{
	DWORD	m_dwFlags;

	EFFCHAR_PROPERTY_NORMAL() :
		m_dwFlags(0L)
	{
	}
};

class DxEffCharNormal : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_NORMALMAP; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_NORMAL m_Property;
		};
		
		struct
		{
			DWORD	m_dwFlags;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_NORMAL*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}


protected:
	DxSkinMesh9*		m_pSkinMesh;
	PSMESHCONTAINER		m_pmcMesh;

public:
	DxMaterialHLSL		m_sMaterialHLSL;

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender=FALSE );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharNormal(void);
	~DxEffCharNormal(void);
};

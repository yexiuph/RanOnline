#pragma once

#include <string>

#include "DxEffChar.h"

#include "./TextureManager.h"
#include "DxMethods.h"

#define		ISTEXTURE1		0x0001
#define		ISTEXTURE2		0x0002
#define		ISTEXTURE3		0x0004
#define		ISTEXTURE4		0x0008
#define		USETEXTURE1		0x0010
#define		USETEXTURE2		0x0020
#define		USETEXTURE3		0x0040
#define		USETEXTURE4		0x0080

#define		NO_UPDOWN		0x0100

struct EFFCHAR_PROPERTY_TEXDIFF_100
{
	DWORD			m_dwFlag;
	DWORD			m_dwMaterials;

	float			m_fSpeed;
	std::string		m_strTex;
};

struct EFFCHAR_PROPERTY_TEXDIFF : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	DWORD			m_dwMaterials;

	DWORD			m_dwIntensity;	// 1X, 2X, 4X
	float			m_fSpeed;
	std::string		m_strTex;

	EFFCHAR_PROPERTY_TEXDIFF () :
		m_dwMaterials(0L),
		m_dwIntensity(0),		// ºûÀÇ ¼¼±â.
		m_fSpeed(1.f)
	{
		m_dwFlag = USETEXTURE1 | USETEXTURE2 | USETEXTURE3 | USETEXTURE4;
		m_dwFlag |= NO_UPDOWN;
	}
};

class DxEffCharTexDiff : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_LEVEL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_TEXDIFF m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			DWORD			m_dwMaterials;

			DWORD			m_dwIntensity;
			float			m_fSpeed;
			std::string		m_strTex;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_TEXDIFF*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB;

protected:
	BOOL	m_bUp;
	float	m_fTime;
	float	m_fRandSpeed;
	DWORD	m_dwColor;
	float	m_fColor;

protected:
	LPDIRECT3DTEXTUREQ	m_pTexture;

protected:
	DXMATERIAL_CHAR_EFF*	m_pMaterials;
	DXMATERIAL_CHAR_EFF*	m_pTempMaterials;
	DWORD					m_dwTempMaterials;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

public:
	virtual void SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials );
	virtual DXMATERIAL_CHAR_EFF*		GetMaterials () { return m_pMaterials; }

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender=FALSE );

	virtual DWORD GetStateOrder () const					{ return EMEFFSO_GLOW; }

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharTexDiff(void);
	~DxEffCharTexDiff(void);
};

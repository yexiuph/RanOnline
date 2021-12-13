#pragma once

#include "DxEffChar.h"

#include "./TextureManager.h"
#include "DxMethods.h"

#define		ISTEXTURE1		0x0001
#define		ISTEXTURE2		0x0002
#define		ISTEXTURE3		0x0004
#define		USETEXTURE1		0x0010
#define		USETEXTURE2		0x0020
#define		USETEXTURE3		0x0040
#define		USEDIFFUSE		0x0100
#define		USEDIFFUSE2		0x0200
#define		USEDIFFUSE3		0x0400

#define		USE_ALL_TEX		0x1000
#define		USE_SELECT_TEX	0x2000

#define		USE_GLOW_SRCTEX	0x10000
#define		USE_GLOW_USER	0x20000

#define		USE_GENERAL		0x100000
#define		USE_BURN		0x200000

struct EFFCHAR_PROPERTY_NEON_100
{
	DWORD		m_dwFlag;

	char		m_szSrcTex_1[MAX_PATH];
	char		m_szSrcTex_2[MAX_PATH];
	char		m_szSrcTex_3[MAX_PATH];

	char		m_szGlowTex_1[MAX_PATH];
	char		m_szGlowTex_2[MAX_PATH];
	char		m_szGlowTex_3[MAX_PATH];

	EFFCHAR_PROPERTY_NEON_100()
	{
		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );

		memset( m_szGlowTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szGlowTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szGlowTex_3, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_NEON_101
{
	DWORD		m_dwFlag;

	D3DCOLOR	m_cColor_1;
	D3DCOLOR	m_cColor_2;
	D3DCOLOR	m_cColor_3;

	char		m_szSrcTex_1[MAX_PATH];
	char		m_szSrcTex_2[MAX_PATH];
	char		m_szSrcTex_3[MAX_PATH];

	char		m_szGlowTex_1[MAX_PATH];
	char		m_szGlowTex_2[MAX_PATH];
	char		m_szGlowTex_3[MAX_PATH];

	EFFCHAR_PROPERTY_NEON_101()
	{
		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );

		memset( m_szGlowTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szGlowTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szGlowTex_3, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_NEON : public EFFCHAR_PROPERTY		// Ver.102,103_105
{
	DWORD		m_dwFlag;
	DWORD		m_dwMaterials;

	D3DCOLOR	m_cColor;

	char		m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_NEON () :
		m_dwMaterials(0),
		m_cColor(0xff8d8d8d)
	{
		m_dwFlag = 0L;
		m_dwFlag |= USE_ALL_TEX;
		m_dwFlag |= USE_GLOW_SRCTEX;
		m_dwFlag |= USE_BURN;
		StringCchCopy( m_szTexture, MAX_PATH, "1d_Lighting.bmp" );
	}
};

class DxEffCharNeon : public DxEffChar
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
			EFFCHAR_PROPERTY_NEON m_Property;
		};
		
		struct
		{
			DWORD		m_dwFlag;
			DWORD		m_dwMaterials;

			D3DCOLOR	m_cColor;

			char		m_szTexture[MAX_PATH];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_NEON*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;

protected:
	LPDIRECT3DTEXTUREQ			m_pGlowTex;
	std::string					m_szBlackTex;
	LPDIRECT3DTEXTUREQ			m_pBlackTex;
	LPDIRECT3DTEXTUREQ			m_pTempTex;		// Release 해 줄 필요가 없다.

protected:
	DXMATERIAL_CHAR_EFF*		m_pMaterials;
	DXMATERIAL_CHAR_EFF*		m_pTempMaterials;
	BOOL*						m_pMaterials_EffUse;
	DWORD						m_dwTempMaterials;

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

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_GLOW; }

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharNeon(void);
	~DxEffCharNeon(void);
};


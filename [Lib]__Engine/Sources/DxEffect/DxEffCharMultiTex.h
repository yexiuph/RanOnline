#pragma once

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
#define		USETEXMAP		0x0100

#define		USE_ALL_TEX		0x1000
#define		USE_SELECT_TEX	0x2000


struct EFFCHAR_PROPERTY_MULTITEX_100_1
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	int				m_nTexNum;

	D3DXCOLOR		m_cDiffuse1;
	D3DXCOLOR		m_cDiffuse2;

	D3DXVECTOR2		m_vTex01;
	D3DXVECTOR2		m_vTex02;

	char			m_szTex1[MAX_PATH];
	char			m_szTex2[MAX_PATH];

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_MULTITEX_100_1()
	{
		memset( m_szTex1, 0, sizeof(char)*MAX_PATH );
		memset( m_szTex2, 0, sizeof(char)*MAX_PATH );

		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );

		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_MULTITEX_102
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	int				m_nTexNum;

	D3DXCOLOR		m_cDiffuse1;
	D3DXCOLOR		m_cDiffuse2;

	D3DXVECTOR2		m_vTex01;
	D3DXVECTOR2		m_vTex02;

	D3DXVECTOR2		m_vTexUV01;
	D3DXVECTOR2		m_vTexUV02;

	char			m_szTex1[MAX_PATH];
	char			m_szTex2[MAX_PATH];

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_MULTITEX_102()
	{
		memset( m_szTex1, 0, sizeof(char)*MAX_PATH );
		memset( m_szTex2, 0, sizeof(char)*MAX_PATH );

		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );

		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_MULTITEX : public EFFCHAR_PROPERTY		// Ver.103,104_105
{
	DWORD			m_dwFlag;
	DWORD			m_dwMaterials;

	int				m_nBlend;
	int				m_nTexNum;

	D3DXCOLOR		m_cDiffuse1;
	D3DXCOLOR		m_cDiffuse2;

	D3DXVECTOR2		m_vTex01;
	D3DXVECTOR2		m_vTex02;

	D3DXVECTOR2		m_vTexUV01;
	D3DXVECTOR2		m_vTexUV02;

	char			m_szTex1[MAX_PATH];
	char			m_szTex2[MAX_PATH];

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_MULTITEX () :
		m_nBlend(0),
		m_dwMaterials(0),
		m_nTexNum(1),
		m_vTex01(0.1f,0.f),
		m_vTex02(0.f,0.1f),
		m_vTexUV01(1.f,1.f),
		m_vTexUV02(1.f,1.f),
		m_cDiffuse1(0xffdddddd),
		m_cDiffuse2(0xffdddddd)
	{
		m_dwFlag = 0L;
		m_dwFlag |= USE_ALL_TEX;

		StringCchCopy( m_szTexture, MAX_PATH, "1d_Lighting.bmp" );

		StringCchCopy( m_szTex1, MAX_PATH, "Rain.dds" );	//
		StringCchCopy( m_szTex2, MAX_PATH, "Rain.dds" );	//
	}
};

class DxEffCharMultiTex : public DxEffChar
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
			EFFCHAR_PROPERTY_MULTITEX m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			DWORD			m_dwMaterials;

			int				m_nBlend;
			int				m_nTexNum;

			D3DXCOLOR		m_cDiffuse1;
			D3DXCOLOR		m_cDiffuse2;

			D3DXVECTOR2		m_vTex01;
			D3DXVECTOR2		m_vTex02;

			D3DXVECTOR2		m_vTexUV01;
			D3DXVECTOR2		m_vTexUV02;

			char			m_szTex1[MAX_PATH];
			char			m_szTex2[MAX_PATH];

			char			m_szTexture[MAX_PATH];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_MULTITEX*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static LPDIRECT3DSTATEBLOCK9		m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9		m_pEffectStateBlock;

protected:
	D3DXVECTOR2			m_vSumTex01;
	D3DXVECTOR2			m_vSumTex02;

	LPDIRECT3DTEXTUREQ			m_pTex1;
	LPDIRECT3DTEXTUREQ			m_pTex2;

	LPDIRECT3DTEXTUREQ			m_pTexture;

protected:
	DXMATERIAL_CHAR_EFF*	m_pMaterials;
	DXMATERIAL_CHAR_EFF*	m_pTempMaterials;
	BOOL*					m_pMaterials_EffUse;
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

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharMultiTex(void);
	~DxEffCharMultiTex(void);
};

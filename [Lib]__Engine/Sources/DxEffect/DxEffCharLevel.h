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

struct EFFCHAR_PROPERTY_LEVEL_100
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];
	char			m_szSrcTex_4[MAX_PATH];	

	char			m_szTexture_1[MAX_PATH];
	char			m_szTexture_2[MAX_PATH];
	char			m_szTexture_3[MAX_PATH];
	char			m_szTexture_4[MAX_PATH];

	EFFCHAR_PROPERTY_LEVEL_100()
	{
		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_4, 0, sizeof(char)*MAX_PATH );

		memset( m_szTexture_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_4, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_LEVEL_101
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;

	D3DXCOLOR		m_cSpecular;

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];
	char			m_szSrcTex_4[MAX_PATH];		//지워야함

	char			m_szTexture_1[MAX_PATH];
	char			m_szTexture_2[MAX_PATH];
	char			m_szTexture_3[MAX_PATH];
	char			m_szTexture_4[MAX_PATH];	//지워야함

	EFFCHAR_PROPERTY_LEVEL_101()
	{
		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_4, 0, sizeof(char)*MAX_PATH );

		memset( m_szTexture_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_4, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_LEVEL_102_103
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;
	DWORD			m_dwMaterials;

	D3DXCOLOR		m_cSpecular;
};

struct EFFCHAR_PROPERTY_LEVEL : public EFFCHAR_PROPERTY	//Ver.104_105
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;
	DWORD			m_dwMaterials;

	float			m_fPower;
	D3DXCOLOR		m_cSpecular;

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_LEVEL () :
		m_dwColorOP(D3DTOP_MODULATE),
		m_dwMaterials(0L),
		m_fPower(14.f),
		m_cSpecular(0.8f,0.8f,0.8f,1.0f)
	{
		m_dwFlag = USETEXTURE1 | USETEXTURE2 | USETEXTURE3 | USETEXTURE4;

		//StringCchCopy( m_szTexture, MAX_PATH, "_Wa_water5.bmp" );
		StringCchCopy( m_szTexture, MAX_PATH, "Rain.dds" );
	}
};

class DxEffCharLevel : public DxEffChar
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
			EFFCHAR_PROPERTY_LEVEL m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			DWORD			m_dwColorOP;
			DWORD			m_dwMaterials;

			float			m_fPower;
			D3DXCOLOR		m_cSpecular;

			char			m_szTexture[MAX_PATH];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_LEVEL*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB_A;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB_A;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB_S;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB_S;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB_R;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB_R;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB_F;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB_F;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB_G;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB_G;

protected:
	struct	DXAMBIENT
	{
		BOOL			m_bUse;
		float			m_fBaseColor;
		float			m_fDynamicColor;
		D3DXCOLOR		m_cColor;

		DXAMBIENT () :
			m_bUse(FALSE),
			m_fBaseColor(1.f),
			m_fDynamicColor(0.f)
		{
			m_cColor.a = 1.f;
			m_cColor.r = 1.f;
			m_cColor.g = 1.f;
			m_cColor.b = 1.f;
		}
	};

	struct	DXSPECULAR
	{
		BOOL			m_bUse;
		float			m_fPower;
		DWORD			m_dwColorOP;
		D3DXCOLOR		m_cColor;

		DXSPECULAR () :
			m_bUse(FALSE),
			m_fPower(5.f),
			m_dwColorOP(D3DTOP_MODULATE)
		{
			D3DXCOLOR ( D3DCOLOR_ARGB ( 255, 255, 255, 255 ) );
		}
	};

	struct	DXFLOW
	{
		BOOL			m_bUse;
		D3DXCOLOR		m_cColor;

		DXFLOW () :
			m_bUse(FALSE)
		{
			m_cColor.a = 1.f;
			m_cColor.r = 1.f;
			m_cColor.g = 1.f;
			m_cColor.b = 1.f;
		}
	};

	struct	DXGLOW
	{
		BOOL			m_bUse;
		D3DXCOLOR		m_cColor;

		DXGLOW () :
			m_bUse(FALSE)
		{
			m_cColor.a = 1.f;
			m_cColor.r = 1.f;
			m_cColor.g = 1.f;
			m_cColor.b = 1.f;
		}
	};

	struct	DXOPTION
	{
		DXAMBIENT		m_Ambient;
		DXSPECULAR		m_Specular;
		DXSPECULAR		m_Reflect;
		DXFLOW			m_Flow;
		DXGLOW			m_Glow;
	};

	enum
	{
		LEVEL_LIMIT = 11,	//5,
		LEVEL_ARRAY = 12,	//6,	// LEVEL_LIMIT+1
	};

	DXOPTION			m_OpLEVEL [ LEVEL_ARRAY ];	// 기본 0 LEVEL 1~5 까지

protected:
	int					m_nTestLEVEL;

protected:
	DWORD				m_dwOldAmbient;

protected:
	float				m_fTime;
	float				m_fLevelTime;

protected:
	D3DXVECTOR2			m_vSumTex01;
	D3DXVECTOR2			m_vTexUV01;
	D3DXVECTOR2			m_vTex01;
	D3DXVECTOR2			m_vMove;
	BOOL				m_bFlow_USE;

protected:
	LPDIRECT3DTEXTUREQ	m_pTexture;
	std::string			m_szLevelFlowTex;
	LPDIRECT3DTEXTUREQ	m_pFlowTex;

protected:
	static BOOL			m_bFlowReflectUSE;

protected:
	DXMATERIAL_CHAR_EFF*	m_pMaterials;
	DXMATERIAL_CHAR_EFF*	m_pTempMaterials;
	DWORD					m_dwTempMaterials;

	DWORD					m_dwMipMapCount;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

public:
	void	SetTestLEVEL ( const int nLevel )		{ m_nTestLEVEL = nLevel; }

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

protected:
	HRESULT	Render_EX( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, int nLevel, const BOOL bPieceRender );

	HRESULT	Render_Ambient( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, const DXAMBIENT cData, const BOOL bPieceRender );
	HRESULT	Render_Specular( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, const DXSPECULAR cData, const BOOL bPieceRender );
	HRESULT	Render_Reflect( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, const DXSPECULAR cData, const BOOL bPieceRender );
	HRESULT	Render_Flow( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, const DXFLOW cData, const BOOL bPieceRender );
	HRESULT	Render_Glow( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, const DXGLOW cData, const BOOL bPieceRender );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharLevel(void);
	~DxEffCharLevel(void);
};

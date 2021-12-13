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

struct EFFCHAR_PROPERTY_DOT3 : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;

	CRDT_TYPE		m_dwDetail_1;				// °¢°¢ÀÇ »Ñ·Á¾ß µÇ´Â Çü½Äµé
	CRDT_TYPE		m_dwDetail_2;
	CRDT_TYPE		m_dwDetail_3;

	char			m_szTexture[MAX_PATH];		// ¹Ý»ç

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];

	char			m_szTexture_1[MAX_PATH];	// ½ºÆåÅ§·¯ ¸Ê
	char			m_szTexture_2[MAX_PATH];
	char			m_szTexture_3[MAX_PATH];

	char			m_szDot3Tex_1[MAX_PATH];	// ¹üÇÁ ¸Ê
	char			m_szDot3Tex_2[MAX_PATH];
	char			m_szDot3Tex_3[MAX_PATH];


	EFFCHAR_PROPERTY_DOT3 () :
		m_dwColorOP(D3DTOP_MODULATE)
	{
		m_dwFlag = USETEXTURE1 | USETEXTURE2 | USETEXTURE3;

		m_dwDetail_1 = CRDT_DEFAULT;
		m_dwDetail_2 = CRDT_DEFAULT;
		m_dwDetail_3 = CRDT_DEFAULT;

		StringCchCopy( m_szSrcTex_1, MAX_PATH, "" );
		StringCchCopy( m_szSrcTex_2, MAX_PATH, "" );
		StringCchCopy( m_szSrcTex_3, MAX_PATH, "" );

		StringCchCopy( m_szTexture_1, MAX_PATH, "1d_Lighting.bmp" );
		StringCchCopy( m_szTexture_2, MAX_PATH, "1d_Lighting.bmp" );
		StringCchCopy( m_szTexture_3, MAX_PATH, "1d_Lighting.bmp" );

		StringCchCopy( m_szDot3Tex_1, MAX_PATH, "Test_Day.dds" );
		StringCchCopy( m_szDot3Tex_2, MAX_PATH, "Test_Day.dds" );
		StringCchCopy( m_szDot3Tex_3, MAX_PATH, "Test_Day.dds" );

		StringCchCopy( m_szTexture, MAX_PATH, "Rain.dds" );
	}
};

class DxEffCharDot3 : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_DOT3; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_DOT3 m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			DWORD			m_dwColorOP;

			CRDT_TYPE		m_dwDetail_1;				// °¢°¢ÀÇ »Ñ·Á¾ß µÇ´Â Çü½Äµé
			CRDT_TYPE		m_dwDetail_2;
			CRDT_TYPE		m_dwDetail_3;

			char			m_szTexture[MAX_PATH];		// ¹Ý»ç

			char			m_szSrcTex_1[MAX_PATH];
			char			m_szSrcTex_2[MAX_PATH];
			char			m_szSrcTex_3[MAX_PATH];

			char			m_szTexture_1[MAX_PATH];	// ½ºÆåÅ§·¯ ¸Ê
			char			m_szTexture_2[MAX_PATH];
			char			m_szTexture_3[MAX_PATH];

			char			m_szDot3Tex_1[MAX_PATH];	// ¹üÇÁ ¸Ê
			char			m_szDot3Tex_2[MAX_PATH];
			char			m_szDot3Tex_3[MAX_PATH];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_DOT3*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

		//	Note : ½¦ÀÌ´õ.
	//
protected:	
	//static DWORD		m_dwEffect;
	//static DWORD		dwDot3VertexShader[];
	//static DWORD		m_dw2VertexEffect;
	//static DWORD		dw2VertexShader[];
	//static DWORD		m_dw2PixelEffect;
	//static DWORD		dw2PixelShader[];

	//static DWORD		m_dwSavedStateBlock;
	//static DWORD		m_dwDetailStateBlock;
	//static DWORD		m_dwEffectStateBlock;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;

	LPDIRECT3DDEVICEQ m_pd3dDevice;

	LPDIRECT3DTEXTUREQ			m_pTexture;		// ¹Ý»ç

	LPDIRECT3DTEXTUREQ			m_pTexture1;	// ½ºÆåÅ§·¯
	LPDIRECT3DTEXTUREQ			m_pTexture2;
	LPDIRECT3DTEXTUREQ			m_pTexture3;

	LPDIRECT3DTEXTUREQ			m_pDot3Tex1;	// ¹üÇÁ
	LPDIRECT3DTEXTUREQ			m_pDot3Tex2;
	LPDIRECT3DTEXTUREQ			m_pDot3Tex3;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender=FALSE );


public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharDot3(void);
	~DxEffCharDot3(void);
};

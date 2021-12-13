#pragma once

#include "DxEffChar.h"

#include "./TextureManager.h"
#include "DxMethods.h"

#define USE_OBLIQUE		0x0001	// 빗금 렌더링 사용함.

struct EFFCHAR_PROPERTY_TOON : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	float			m_fSize;
	float			m_fSizeADD;			//  사이즈 추가 < 랜덤 수치 >
	char			m_szToonTex[32];
	char			m_szObliqueTex[32];	// 사선 

	EFFCHAR_PROPERTY_TOON () :
		m_dwFlag(0L),
		m_fSize(1.f),
		m_fSizeADD(1.f)
	{
		StringCchCopy( m_szToonTex, MAX_PATH, "Toon.dds" );
		StringCchCopy( m_szObliqueTex, MAX_PATH, "Oblique.dds" );
	}
};

class DxEffCharToon : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_TOON; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_TOON m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			float			m_fSize;
			float			m_fSizeADD;			//  사이즈 추가 < 랜덤 수치 >
	
			char			m_szToonTex[32];
			char			m_szObliqueTex[32];	// 사선 
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_TOON*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static DWORD		m_dwToonVS_G;		// 일반 Mesh
	static DWORD		m_dwToonVSA_G[];	// 일반 Mesh
	static DWORD		m_dwToonVS[4];
	static DWORD		m_dwToonVSA_1[];
	static DWORD		m_dwToonVSA_2[];
	static DWORD		m_dwToonVSA_3[];
	static DWORD		m_dwToonVSA_4[];

	static LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedEdgeSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawEdgeSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedObliqueSB_1;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawObliqueSB_1;
	static LPDIRECT3DSTATEBLOCK9	m_pSavedObliqueSB_2;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawObliqueSB_2;
	static LPDIRECT3DSTATEBLOCK9	m_pSavedObliqueSB_3;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawObliqueSB_3;
	

	LPDIRECT3DTEXTUREQ			m_pToonTex;
	LPDIRECT3DTEXTUREQ			m_pObliqueTex;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

private:
	float		m_fObliqueTEX;

public:
	static HRESULT StaticCreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT StaticResetDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT StaticLostDevice();
	static void StaticDestroyDevice();

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );

	virtual DWORD GetStateOrder () const				{ return EMEFFSO_TOON; }
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA );
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	void RenderSHADOW( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE *pmtrlPiece );
	void RenderOBLIQUE( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE *pmtrlPiece );
	void RenderBASE( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE *pmtrlPiece );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharToon(void);
	~DxEffCharToon(void);
};

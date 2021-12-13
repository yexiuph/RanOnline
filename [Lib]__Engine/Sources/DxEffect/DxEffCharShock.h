#pragma once

#include "DxEffChar.h"

#include "./TextureManager.h"
#include "DxMethods.h"

#define STRING_NUMBER_256	256

struct EFFCHAR_PROPERTY_SHOCK_100
{
	BOOL			m_bUse;
	int				m_nBlend;
	char			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.

	DWORD			m_dwFlag;

	DWORD			m_dwLightning;	// 전기 갯수

	DWORD			m_dwDivision;	// 분할 갯수.

	float			m_fWidth_In;
	float			m_fWidth_Out;

	DWORD			m_dwMaxLenth;	// Out 휘도록
	DWORD			m_dwVelocity;	// In 휘도록

	float			m_fAlphaRate1;
	float			m_fAlphaRate2;

	float			m_fAlphaStart;
	float			m_fAlphaMid1;
	float			m_fAlphaMid2;
	float			m_fAlphaEnd;

	D3DXCOLOR		m_cColorStart;
	D3DXCOLOR		m_cColorEnd;

	char			m_szTexture_In[STRING_NUMBER_256];
	char			m_szTexture_Out[STRING_NUMBER_256];

	EFFCHAR_PROPERTY_SHOCK_100()
	{
		memset( m_szTraceUP, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceDOWN, 0, sizeof(char)*STRACE_NSIZE );

		memset( m_szTexture_In, 0, sizeof(char)*STRING_NUMBER_256 );
		memset( m_szTexture_Out, 0, sizeof(char)*STRING_NUMBER_256 );
	};
};


struct EFFCHAR_PROPERTY_SHOCK : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	float			m_fFullTime;
	D3DXCOLOR		m_cColor;

	EFFCHAR_PROPERTY_SHOCK () :
		m_fFullTime(0.06f),
		m_cColor(1.0f,0.6f,0.6f,0.6f)
	{
		m_dwFlag = 0L;
	}
};

class DxEffCharShock : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_SHOCK; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_SHOCK m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			float			m_fFullTime;
			D3DXCOLOR		m_cColor;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_SHOCK*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	DxSkinMesh9*		m_pSkinMesh;
	PSMESHCONTAINER		m_pmcMesh;

protected:
	float			m_fElapsedTime;

	float			m_fTarDirection;
	D3DXVECTOR3		m_vDirection;

	D3DLIGHTQ		m_sLight0;

	BOOL			m_bLightEnable7;
	D3DLIGHTQ		m_sLight7;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );

	virtual DWORD GetStateOrder () const				{ return EMEFFSO_SHOCK; }
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA );
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );
	virtual void SetDirection ( D3DXVECTOR3 *pDir, float fTarDir )	{ m_vDirection = *pDir; m_fTarDirection = fTarDir; }

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharShock(void);
	~DxEffCharShock(void);
};
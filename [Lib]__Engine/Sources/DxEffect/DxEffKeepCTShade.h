#pragma once

#include "./DxEffKeep.h"

struct EFFKEEP_PROPERTY_CTSHADE : public EFFKEEP_PROPERTY
{
	TCHAR	szShadeTex[32];

	EFFKEEP_PROPERTY_CTSHADE();
};

class DxEffKeepData_CTShade : public DxEffKeepData
{
public:
	static DWORD		VERSION;
	static DWORD		TYPEID;
	static char			NAME[64];

public:
	void SetProperty( EFFKEEP_PROPERTY *pProperty )	{}
	EFFKEEP_PROPERTY* GetProperty()					{ return &m_sProp; }

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

public:
	EFFKEEP_PROPERTY_CTSHADE	m_sProp;

public:
	virtual DxEffKeep* NEWOBJ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

public:
	DxEffKeepData_CTShade();
	~DxEffKeepData_CTShade();
};

class DxEffKeepCTShade : public DxEffKeep
{
public:
	EFFKEEP_PROPERTY_CTSHADE	m_sProp;
	LPDIRECT3DTEXTUREQ			m_pTexture;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects();

public:
	virtual HRESULT FrameMove ( float fElapsedTime, SKINEFFDATA& sSKINEFFDATA );
	virtual HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffKeepCTShade(void);
	~DxEffKeepCTShade(void);
};


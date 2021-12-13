#pragma once

#include <string>

#include "DxTexEff.h"

class CSerialFile;
struct DxMeshes;

struct TEXEFF_DIFFUSE_PROPERTY
{
	// Note : Save, Load, Edit
	BOOL				bSpeed;
	float				fSpeed;
	float				fAlpha;
	std::string			strTex;
	LPDIRECT3DTEXTUREQ	pTex;

	void Save( CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

	TEXEFF_DIFFUSE_PROPERTY();
	~TEXEFF_DIFFUSE_PROPERTY();
};

struct TEXEFF_DIFFUSE_PROPERTY_100
{
	// Note : Save, Load, Edit
	float				fSpeed;
	std::string			strTex;
	LPDIRECT3DTEXTUREQ	pTex;

	void Save( CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

	TEXEFF_DIFFUSE_PROPERTY_100();
	~TEXEFF_DIFFUSE_PROPERTY_100();
};

class DxTexEffDiffuse : public DxTexEffBase
{
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static char	NAME[];

protected:
	TEXEFF_DIFFUSE_PROPERTY	m_sProp;

	BOOL	m_bUp;
	float	m_fTime;
	float	m_fRandSpeed;
	DWORD	m_dwColor;

public:
	virtual DWORD GetTypeID()				{ return TYPEID; }
	virtual const char* GetName()			{ return NAME; }
	virtual void GetProperty( PBYTE &pProp ) { pProp = (PBYTE)&m_sProp; }
	virtual void SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp );

public:
	virtual void FrameMove( const float fElapsedTime );

public:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawSB;
	static void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib );
	void SetRenderBegin( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	TEXEFF_DIFFUSE_PROPERTY* GetProperty()	{ return &m_sProp; }

public:
	virtual void SavePSF( CSerialFile& SFile );
	virtual void LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

public:
	DxTexEffDiffuse();
	virtual ~DxTexEffDiffuse();
};

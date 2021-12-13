#pragma once

#include "DxBoneCollector.h"
#include "DxSkinAniMan.h"

class DxSkinMesh9;

class DxSkinObject : public DxSkinAniControl
{
protected:
	static char		m_szPath[MAX_PATH];

public:
	static void		SetPath ( char* szPath );
	static char*	GetPath ()	{ return m_szPath; }

protected:
	char			m_szSkinMesh[MAX_PATH];
	DxSkinMesh9*	m_pSkinMesh;

public:
	DxSkinMesh9*	GetSkinMesh ()		{ return m_pSkinMesh; }
	char*			GetSkinMeshName ()	{ return m_szSkinMesh; }

public:
	HRESULT FrameMove ( float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot );

public:
	BOOL CreateSkinMesh ( const char* szFile, const char *szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread );

public:
	BOOL LoadObject ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread );
	BOOL SaveObject ( const char* szFile );

	void ClearAll ();

public:
	DxSkinObject(void);
	~DxSkinObject(void);
};

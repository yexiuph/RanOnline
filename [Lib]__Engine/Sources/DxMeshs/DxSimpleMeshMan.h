#pragma once

#include <map>
#include <string>
#include <algorithm>

#include "./TextureManager.h"

class DxSimMesh
{
protected:
	enum
	{
		EM_BOUNDSPHERE		= 0x0001,
		EM_BOUNDBOX			= 0x0002
	};

	D3DXVECTOR3			m_vCenter;
	float				m_fRadius;

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

public:
	char				m_szFileName[MAX_PATH];
	DWORD				m_dwRef;
	BOOL				m_dwFlag;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	LPD3DXMESH			m_pLocalMesh;

	DWORD						m_dwNumMaterials; // Materials for the mesh
	D3DXMATERIAL*				m_pMaterials;
	LPDIRECT3DTEXTUREQ*			m_pTextures;
	TextureManager::EM_TEXTYPE* m_emTexType;
	BOOL						m_bUseMaterials;

	BOOL				m_bMorph;
	D3DXVECTOR3*		m_pVertex[3];
	D3DXVECTOR3*		m_pNormal[3];
	D3DXVECTOR2*		m_pTexUV;

public:
	HRESULT FrameMove ( float fTime );	// -1 ~ 1

	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture=NULL, D3DMATERIALQ* pMaterials=NULL );

	HRESULT RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matWld, D3DMATERIALQ* pMaterials=NULL );

public:
	LPD3DXMESH GetLocalMesh ()  { return m_pLocalMesh; }
	HRESULT GetBoundSphere ( D3DXVECTOR3 &vCenter, float &fRadius );
	HRESULT GetBoundBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	void	SetMoveTex ( D3DXVECTOR2& vTexVelSum );
	void	SetSequenceTex ( int nCol, int nRow, int nNowSprite );

public:
	void    UseMeshMaterials ( BOOL bFlag ) { m_bUseMaterials = bFlag; }
	HRESULT SetFVF ( DWORD dwFVF );

public:
	HRESULT Create ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF, const char* szFilename, const char* szFilename1="", const char* szFilename2="" );
	HRESULT Destroy ();

public:
	DxSimMesh ();
	virtual ~DxSimMesh ();
};

class DxSimpleMeshMan
{
public:
	typedef std::map<std::string,DxSimMesh*>			SIMPLEMAP;
	typedef std::map<std::string,DxSimMesh*>::iterator	SIMPLEMAP_ITER;

protected:
	char			m_szPath[MAX_PATH];

public:
	void			SetPath ( char* szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	char*			GetPath ()					{ return m_szPath; }

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	SIMPLEMAP			m_MeshMap;
	SIMPLEMAP			m_BlendMeshMap;

public:
	DxSimMesh*	Load ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF );
	DxSimMesh*	BlendLoad ( char** szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF );

	HRESULT		Release ( const char* szFile, DWORD dwFVF, BOOL bMorph=FALSE );
	HRESULT		Delete ( const char* szFile, DWORD dwFVF, BOOL bMorph=FALSE );

	DxSimMesh*	Find ( const char* szFile, BOOL bMorph=FALSE );

protected:
	void		ConvertFileName( char* szOut, int nStrLen, const char* szFile, DWORD dwFVF );

public:
	HRESULT CleanUp ();

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

protected:
	DxSimpleMeshMan(void);

public:
	~DxSimpleMeshMan(void);

public:
	static DxSimpleMeshMan& GetInstance();
};

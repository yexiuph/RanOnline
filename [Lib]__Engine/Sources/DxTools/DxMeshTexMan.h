#pragma once

#include <map>
#include <string>
#include <list>
#include "./DxSimpleMeshMan.h"

#define D3DFVF_ITEM_VERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

class DxMeshTex
{
public:
	enum
	{
		TEXTURE_SIZE_X = 128, TEXTURE_SIZE_Y = 128
	};

protected:
	std::string			m_strMeshName;
	BOOL				m_bDynamic;

	DxSimMesh*			m_pMesh;
	LPDIRECT3DTEXTUREQ	m_pMeshTexture;

	D3DXMATRIX			m_matView, m_matProj, m_matWorld, m_matTrans, m_matScaling;
	D3DXVECTOR3			m_vObjectCenter;
	float				m_fObjectRadius;

	float				m_fTotTime;
	float				m_fAngle;
	float				m_fScalingFactor;

public:
	DWORD				m_dwRef;
	struct TEXTUREPOS
	{
		float left;
		float top;
		float sizex;
		float sizey;
		TEXTUREPOS()
		{
			left=top=sizex=sizey=0.f;
		}
	}	m_sTexturePos;

protected:
	int SetupOrthoProjForRenderCopy(LPDIRECT3DDEVICEQ pD3DDevice);

public:
	HRESULT Create ( LPDIRECT3DDEVICEQ pd3dDevice, std::string strMeshName, BOOL bDynamic );
	HRESULT Delete ();

	HRESULT LoadMesh ( LPDIRECT3DDEVICEQ pd3dDevice, std::string strMeshName );

protected:
	HRESULT MakeTexture ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT ReleaseTexture ();

protected:
	HRESULT DrawTextureStatic ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	LPDIRECT3DTEXTUREQ GetTexture ()	{ return m_pMeshTexture; }
	HRESULT UpdateRender ( float fElapsedTime, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT ReSet ()					{ m_fAngle = 0.0f; }

public:
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();

public:
	DxMeshTex () :
		m_bDynamic(FALSE),
		m_pMesh(NULL),
		m_pMeshTexture(NULL),
		m_fObjectRadius(0),
		m_fScalingFactor(0),

		m_fTotTime(0),
		m_fAngle(0),

		m_dwRef(0)
	{
	}
	~DxMeshTex ()
	{
	}
};
typedef DxMeshTex* PDXMESHTEX;

class DxMeshTexMan
{
public:
	typedef std::map<std::string,DxMeshTex*>	MESHTEXMAP;
	typedef MESHTEXMAP::iterator				MESHTEXMAP_ITER;
	typedef std::list<LPDIRECT3DTEXTUREQ>		REQTEXTURE;
	typedef REQTEXTURE::iterator				REQTEXTURE_ITER;

protected:
	MESHTEXMAP		m_MeshTexMap;

protected:
	REQTEXTURE		m_ReqTexture;

	HRESULT MakeTexture ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ &pMeshTexture );
	HRESULT ReleaseTextureAll ();

public:
	HRESULT ReserveTexture ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void PushTexture ( LPDIRECT3DTEXTUREQ pTexture );
	LPDIRECT3DTEXTUREQ PopTexture ();

public:
	PDXMESHTEX CreateTexture ( LPDIRECT3DDEVICEQ pd3dDevice, std::string strMeshName, BOOL bDynamic );
	HRESULT ReleaseTexture ( std::string strMeshName );

	HRESULT DeleteAll ();

public:
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();

	HRESULT FrameMove ( float fTime, float fElapsedTime );

protected:
	DxMeshTexMan(void);

public:
	~DxMeshTexMan(void);

public:
	static DxMeshTexMan& GetInstance();
};

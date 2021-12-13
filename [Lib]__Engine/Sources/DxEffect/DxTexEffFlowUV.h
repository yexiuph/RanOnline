#pragma once

#include <string>

#include "DxVertexFVF.h"
#include "DxTexEff.h"

class CSerialFile;
struct DxMeshes;

#define	TEXEFFFLOWUV_NEWUV			0x0001
#define	TEXEFFFLOWUV_RANDOMSPEED	0x0002
#define	TEXEFFFLOWUV_MODULATE2X		0x0004

struct TEXEFF_FLOWUV_PROPERTY
{
	DWORD		dwFlag;
	float		fScale;
	float		fAlpha;
	D3DXVECTOR2	vSpeed;
	std::string	strAlphaTex;
	std::string	strFlowTex;

	float		fSpeed;		// Save는 되지만 Edit 에서는 안 쓰인다.
	D3DXVECTOR2	vDirect;	// Save는 되지만 Edit 에서는 안 쓰인다.

	LPDIRECT3DTEXTUREQ	pAlphaTex;
	LPDIRECT3DTEXTUREQ	pFlowTex;

	void Save( CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

	TEXEFF_FLOWUV_PROPERTY();
	~TEXEFF_FLOWUV_PROPERTY();
};

class DxTexEffFlowUV : public DxTexEffBase
{
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static char	NAME[];

protected:
	TEXEFF_FLOWUV_PROPERTY	m_sProp;

	BOOL		m_bUp;
	float		m_fTime;
	float		m_fRandSpeed;
	D3DXVECTOR2	m_vLastSpeed;
	DWORD		m_dwColor;

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
	void SetRenderBegin( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice );

	// Note : FrameMesh Mode
public:
	void FrameMoveMultiTex( LPDIRECT3DVERTEXBUFFERQ pVB, DWORD dwVert, VERTEX* pVertSrc );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, 
				LPD3DXATTRIBUTERANGE pAttrib, const DWORD dwAttrib, VERTEX* pVertSrc, DWORD dwVert );

public:
	// Note : Octree 메쉬에서의 개개의 VB를 사용하지 않고 공용 VB 사용하려고 이렇게 만들어 놓았다.
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces, VERTEXNORCOLORTEX* pVertSrc );

public:
	TEXEFF_FLOWUV_PROPERTY* GetProperty()	{ return &m_sProp; }

public:
	virtual void SavePSF( CSerialFile& SFile );
	virtual void LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

public:
	DxTexEffFlowUV();
	virtual ~DxTexEffFlowUV();
};



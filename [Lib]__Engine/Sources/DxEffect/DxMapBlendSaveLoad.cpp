#include "pch.h"
#include "./SerialFile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include "./TextureManager.h"

#include "./DxMapBlend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//												B	L	E	N	D		D	A	T	A
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxBlendData::SaveFile_Edit ( CSerialFile &SFile )
{
	SFile << m_dwGrid;
	SFile << m_dwFaces;
	SFile << m_dwVertices;

	VERTEX_XT2*	pVertices;
	m_pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
	SFile.WriteBuffer ( pVertices, sizeof(VERTEX_XT2)*m_dwVertices );
	m_pVB->Unlock ();

	WORD*	pIndices;
	m_pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
	SFile.WriteBuffer ( pIndices, sizeof(WORD)*3*m_dwFaces );
	m_pIB->Unlock ();

	SFile.WriteBuffer ( m_pTileUSE, sizeof(BOOL)*EM_GRID*EM_GRID );

	SFile.WriteBuffer ( m_pAlphaUSE, sizeof(BOOL)*EM_TEXSIZE*EM_TEXSIZE );

	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;

	m_pAlphaTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst     = (DWORD*)lrDst.pBits;
	SFile.WriteBuffer ( pDst, sizeof(DWORD)*EM_TEXSIZE*EM_TEXSIZE );
	m_pAlphaTex->UnlockRect(0);
}

void DxBlendData::LoadFile_Edit ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, const char* pName )
{
	m_bEnable = FALSE;

	SFile >> m_dwGrid;
	SFile >> m_dwFaces;
	SFile >> m_dwVertices;

	VERTEX_XT2*	pVertices;
	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEX_XT2), D3DUSAGE_WRITEONLY, VERTEX_XT2::FVF, D3DPOOL_MANAGED, &m_pVB, NULL );

	m_pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
	SFile.ReadBuffer ( pVertices, sizeof(VERTEX_XT2)*m_dwVertices );
	m_pVB->Unlock ();

	WORD*	pIndices;
	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( 3*m_dwFaces*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	m_pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
	SFile.ReadBuffer ( pIndices, sizeof(WORD)*3*m_dwFaces );
	m_pIB->Unlock ();

	SAFE_DELETE_ARRAY ( m_pTileUSE );
	m_pTileUSE = new BOOL[EM_GRID*EM_GRID];
	SFile.ReadBuffer ( m_pTileUSE, sizeof(BOOL)*EM_GRID*EM_GRID );

	SAFE_DELETE_ARRAY ( m_pAlphaUSE );
	m_pAlphaUSE = new BOOL[EM_TEXSIZE*EM_TEXSIZE];
	SFile.ReadBuffer ( m_pAlphaUSE, sizeof(BOOL)*EM_TEXSIZE*EM_TEXSIZE );

	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;

	SAFE_RELEASE ( m_pAlphaTex );
	D3DXCreateTexture ( pd3dDevice, EM_TEXSIZE, EM_TEXSIZE, 1, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pAlphaTex );

	m_pAlphaTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst     = (DWORD*)lrDst.pBits;
	SFile.ReadBuffer ( pDst, sizeof(DWORD)*EM_TEXSIZE*EM_TEXSIZE );
	m_pAlphaTex->UnlockRect(0);

	TextureManager::ReleaseTexture( m_szBaseTex.c_str(), m_pTex );

	m_szBaseTex = pName;
	TextureManager::LoadTexture ( pName, pd3dDevice, m_pTex, 0, 0, TRUE );

	m_bEnable = TRUE;
}

void DxBlendData::SaveFile_Game ( CSerialFile &SFile )
{
	SFile << m_dwGrid;
	SFile << m_dwFaces;
	SFile << m_dwVertices;

	VERTEX_XT2*	pVertices;
	m_pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
	SFile.WriteBuffer ( pVertices, sizeof(VERTEX_XT2)*m_dwVertices );
	m_pVB->Unlock ();

	WORD*	pIndices;
	m_pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
	SFile.WriteBuffer ( pIndices, sizeof(WORD)*3*m_dwFaces );
	m_pIB->Unlock ();

	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;

	m_pAlphaTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst     = (DWORD*)lrDst.pBits;
	SFile.WriteBuffer ( pDst, sizeof(DWORD)*EM_TEXSIZE*EM_TEXSIZE );
	m_pAlphaTex->UnlockRect(0);
}

void DxBlendData::LoadFile_Game ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, const char* pName )
{
	m_bEnable = FALSE;

	SFile >> m_dwGrid;
	SFile >> m_dwFaces;
	SFile >> m_dwVertices;

	VERTEX_XT2*	pVertices;
	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEX_XT2), D3DUSAGE_WRITEONLY, VERTEX_XT2::FVF, D3DPOOL_MANAGED, &m_pVB, NULL );

	m_pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
	SFile.ReadBuffer ( pVertices, sizeof(VERTEX_XT2)*m_dwVertices );
	m_pVB->Unlock ();

	WORD*	pIndices;
	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( 3*m_dwFaces*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	m_pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
	SFile.ReadBuffer ( pIndices, sizeof(WORD)*3*m_dwFaces );
	m_pIB->Unlock ();

	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;

	SAFE_RELEASE ( m_pAlphaTex );
	D3DXCreateTexture ( pd3dDevice, EM_TEXSIZE, EM_TEXSIZE, 1, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pAlphaTex );

	m_pAlphaTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst     = (DWORD*)lrDst.pBits;
	SFile.ReadBuffer ( pDst, sizeof(DWORD)*EM_TEXSIZE*EM_TEXSIZE );
	m_pAlphaTex->UnlockRect(0);

	TextureManager::ReleaseTexture( m_szBaseTex.c_str(), m_pTex );

	m_szBaseTex = pName;
	TextureManager::LoadTexture ( pName, pd3dDevice, m_pTex, 0, 0, TRUE );

	m_bEnable = TRUE;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//														B	L	E	N	D
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxBlend::SaveFile_Edit ( CSerialFile &SFile )
{
	SFile.WriteBuffer ( m_pTileTex, sizeof(TILE_TEX)*EM_TEXSIZE*EM_TEXSIZE );

	int nSize;
	nSize = (int)m_mapBlend.size();

	SFile << nSize;

	BLENDMAP_ITER iter = m_mapBlend.begin();
	for ( ; iter!=m_mapBlend.end(); ++iter )
	{
		SFile << (*iter).first;
		(*iter).second->SaveFile_Edit ( SFile );
	}
}

void DxBlend::LoadFile_Edit ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile )
{
	SAFE_DELETE_ARRAY ( m_pTileTex );
	m_pTileTex = new TILE_TEX[EM_TEXSIZE*EM_TEXSIZE];
	SFile.ReadBuffer ( m_pTileTex, sizeof(TILE_TEX)*EM_TEXSIZE*EM_TEXSIZE );

	int nSize;
	std::string	sName;
	SFile >> nSize;

	std::for_each ( m_mapBlend.begin(), m_mapBlend.end(), std_afunc::DeleteMapObject() );
	m_mapBlend.clear();
	for ( int i=0; i<nSize; ++i )
	{
		SFile >> sName;

		DxBlendData* pBlendData = new DxBlendData;
		pBlendData->LoadFile_Edit ( pd3dDevice, SFile, sName.c_str() );

		m_mapBlend.insert ( std::make_pair(sName.c_str(), pBlendData) );
	}
}

void DxBlend::SaveFile_Game ( CSerialFile &SFile )
{
	int nSize;
	nSize = (int)m_mapBlend.size();

	SFile << nSize;

	BLENDMAP_ITER iter = m_mapBlend.begin();
	for ( ; iter!=m_mapBlend.end(); ++iter )
	{
		SFile << (*iter).first;
		(*iter).second->SaveFile_Game ( SFile );
	}
}

void DxBlend::LoadFile_Game ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile )
{
	int nSize;
	std::string	sName;
	SFile >> nSize;

	std::for_each ( m_mapBlend.begin(), m_mapBlend.end(), std_afunc::DeleteMapObject() );
	m_mapBlend.clear();
	for ( int i=0; i<nSize; ++i )
	{
		SFile >> sName;

		DxBlendData* pBlendData = new DxBlendData;
		pBlendData->LoadFile_Game ( pd3dDevice, SFile, sName.c_str() );

		m_mapBlend.insert ( std::make_pair(sName.c_str(), pBlendData) );
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//															C	O	L	O	R	
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxColor::SaveFile_Edit ( CSerialFile &SFile )
{
	SFile.WriteBuffer ( m_pColor, sizeof(D3DXCOLOR)*EM_TEXSIZE*EM_TEXSIZE );

	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;

	m_pTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst     = (DWORD*)lrDst.pBits;
	SFile.WriteBuffer ( pDst, sizeof(DWORD)*EM_TEXSIZE*EM_TEXSIZE );
	m_pTex->UnlockRect(0);
}

void DxColor::LoadFile_Edit ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile )
{
	m_bEnable = FALSE;

	SAFE_DELETE_ARRAY ( m_pColor );
	m_pColor = new D3DXCOLOR[EM_TEXSIZE*EM_TEXSIZE];
	SFile.ReadBuffer ( m_pColor, sizeof(D3DXCOLOR)*EM_TEXSIZE*EM_TEXSIZE );

	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;

	SAFE_RELEASE ( m_pTex );
	D3DXCreateTexture ( pd3dDevice, EM_TEXSIZE, EM_TEXSIZE, 1, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTex );

	m_pTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst     = (DWORD*)lrDst.pBits;
	SFile.ReadBuffer ( pDst, sizeof(DWORD)*EM_TEXSIZE*EM_TEXSIZE );
	m_pTex->UnlockRect(0);

	m_bEnable = TRUE;
}

void DxColor::SaveFile_Game ( CSerialFile &SFile )
{
	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;

	m_pTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst     = (DWORD*)lrDst.pBits;
	SFile.WriteBuffer ( pDst, sizeof(DWORD)*EM_TEXSIZE*EM_TEXSIZE );
	m_pTex->UnlockRect(0);
}

void DxColor::LoadFile_Game ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile )
{
	m_bEnable = FALSE;

	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;

	SAFE_RELEASE ( m_pTex );
	D3DXCreateTexture ( pd3dDevice, EM_TEXSIZE, EM_TEXSIZE, 1, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTex );

	m_pTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst     = (DWORD*)lrDst.pBits;
	SFile.ReadBuffer ( pDst, sizeof(DWORD)*EM_TEXSIZE*EM_TEXSIZE );
	m_pTex->UnlockRect(0);

	m_bEnable = TRUE;
}


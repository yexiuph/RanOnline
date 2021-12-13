#include "pch.h"
#include "./DxViewPort.h"
#include "./SerialFile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include "./TextureManager.h"
#include "./StlFunctions.h"

#include "./DxEffectMan.h"

#include "./DxMapBlend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD		VERTEX_XT2::FVF		= D3DFVF_XYZ|D3DFVF_TEX2;
const DWORD		VERTEX_XNT2::FVF	= D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2;
// -----------------------------------------------------------------------------------------------------------------------------------------
//									B	L	E	N	D		D	A	T	A		C l a s s
// -----------------------------------------------------------------------------------------------------------------------------------------
DxBlendData::DxBlendData () :
	m_bEnable(FALSE),
	m_dwGrid(0),
	m_dwFaces(0),
	m_dwVertices(0),
	m_pVB(NULL),
	m_pIB(NULL),
	m_pTex(NULL),
	m_pAlphaTex(NULL),
	m_pTileUSE(NULL),
	m_pAlphaUSE(NULL)
{
}

DxBlendData::~DxBlendData ()
{
	m_bEnable = FALSE;

	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );
	SAFE_RELEASE ( m_pAlphaTex );

	TextureManager::ReleaseTexture( m_szBaseTex.c_str(), m_pTex );

	SAFE_DELETE_ARRAY ( m_pTileUSE );
	SAFE_DELETE_ARRAY ( m_pAlphaUSE );
}

void DxBlendData::InitData ( LPDIRECT3DDEVICEQ pd3dDevice, TILE* pTile )
{
	m_bEnable = FALSE;

	TextureManager::ReleaseTexture( m_szBaseTex.c_str(), m_pTex );

	SAFE_DELETE_ARRAY ( m_pTileUSE );
	SAFE_DELETE_ARRAY ( m_pAlphaUSE );

	m_szBaseTex = "_Eff_detail.bmp";
	TextureManager::LoadTexture ( m_szBaseTex.c_str(), pd3dDevice, m_pTex, 0, 0, TRUE );

	m_pAlphaUSE = new BOOL[EM_TEXSIZE*EM_TEXSIZE];
	for ( int z=0; z<EM_TEXSIZE; z++ )
	{
		for ( int x=0; x<EM_TEXSIZE; x++ )
		{
			m_pAlphaUSE[(z*EM_TEXSIZE)+x] = TRUE;
		}
	}

	m_dwGrid = 0;
	m_pTileUSE = new BOOL[EM_GRID*EM_GRID];
	for ( int z=0; z<EM_GRID; z++ )
	{
		for ( int x=0; x<EM_GRID; x++ )
		{
			++m_dwGrid;
			m_pTileUSE[(z*EM_GRID)+x] = TRUE;
		}
	}

	m_dwFaces		= m_dwGrid*2;
	m_dwVertices	= m_dwGrid*4;

	InitAlphaTex ( pd3dDevice, 0xffffffff );
	ModifyVB ( pd3dDevice, pTile );
	ModifyIB ( pd3dDevice );

	m_bEnable = TRUE;
}

void DxBlendData::InitData ( LPDIRECT3DDEVICEQ pd3dDevice, const char* cName )
{
	m_bEnable = FALSE;

	// Note : 알파 사용 초기화
	SAFE_DELETE_ARRAY ( m_pAlphaUSE );
	m_pAlphaUSE = new BOOL[EM_TEXSIZE*EM_TEXSIZE];
	for ( int z=0; z<EM_TEXSIZE; z++ )
	{
		for ( int x=0; x<EM_TEXSIZE; x++ )
		{
			m_pAlphaUSE[(z*EM_TEXSIZE)+x] = FALSE;
		}
	}

	// Note : 타일 초기화
	SAFE_DELETE_ARRAY ( m_pTileUSE );
	m_pTileUSE = new BOOL[EM_GRID*EM_GRID];
	for ( int z=0; z<EM_GRID; z++ )
	{
		for ( int x=0; x<EM_GRID; x++ )
		{
			m_pTileUSE[(z*EM_GRID)+x] = FALSE;
		}
	}

	// Note : 일반 텍스쳐 로드
	TextureManager::ReleaseTexture( m_szBaseTex.c_str(), m_pTex );

	m_szBaseTex = cName;
	TextureManager::LoadTexture ( cName, pd3dDevice, m_pTex, 0, 0, TRUE );

	// Note : 알파 텍스쳐 초기화
	InitAlphaTex ( pd3dDevice, 0x00ffffff );

	m_bEnable = TRUE;
}

void DxBlendData::InitAlphaTex ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwColor )
{
	m_bEnable = FALSE;

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

	for( int z=0; z<EM_TEXSIZE; z++ )
	{
		for( int x=0; x<EM_TEXSIZE; x++ )
		{
			pDst[x] = dwColor;
		}
		pDst = pDst+(lrDst.Pitch/(dwBPP/8));
	}

	m_pAlphaTex->UnlockRect(0);

	m_bEnable = TRUE;
}

void DxBlendData::ModifyAlphaTex ( int _X, int _Y, BOOL bUSE )
{
	// Note : 알파를 사용하는지 안하는지 체크.
	m_pAlphaUSE[_X+(_Y*EM_TEXSIZE)] = bUSE;

	// Note : 알파 텍스쳐를 수정.
	float fAlpha = 0.f;

	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;

	m_pAlphaTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst     = (DWORD*)lrDst.pBits;

	pDst = pDst + ((lrDst.Pitch/(dwBPP/8))*(_Y-1));

	for( int y=(_Y-1); y<=(_Y+1); y++ )
	{
		for( int x=(_X-1); x<=(_X+1); x++ )
		{
			if ( (x<=0) || (y<=0) || (x>=(EM_TEXSIZE-1)) || (y>=(EM_TEXSIZE-1)) )	continue;

			fAlpha = 0.f;
			//if ( m_pAlphaUSE[(y*EM_TEXSIZE)+x] )		fAlpha += 0.32f;
			//if ( m_pAlphaUSE[((y-1)*EM_TEXSIZE)+x] )	fAlpha += 0.10f;
			//if ( m_pAlphaUSE[((y+1)*EM_TEXSIZE)+x] )	fAlpha += 0.10f;
			//if ( m_pAlphaUSE[(y*EM_TEXSIZE)+x+1] )		fAlpha += 0.10f;
			//if ( m_pAlphaUSE[(y*EM_TEXSIZE)+x-1] )		fAlpha += 0.10f;
			//if ( m_pAlphaUSE[((y-1)*EM_TEXSIZE)+x-1] )	fAlpha += 0.07f;
			//if ( m_pAlphaUSE[((y+1)*EM_TEXSIZE)+x-1] )	fAlpha += 0.07f;
			//if ( m_pAlphaUSE[((y-1)*EM_TEXSIZE)+x+1] )	fAlpha += 0.07f;
			//if ( m_pAlphaUSE[((y+1)*EM_TEXSIZE)+x+1] )	fAlpha += 0.07f;
			if ( m_pAlphaUSE[(y*EM_TEXSIZE)+x] )		fAlpha += 1.00f;
			if ( m_pAlphaUSE[((y-1)*EM_TEXSIZE)+x] )	fAlpha += 0.30f;
			if ( m_pAlphaUSE[((y+1)*EM_TEXSIZE)+x] )	fAlpha += 0.30f;
			if ( m_pAlphaUSE[(y*EM_TEXSIZE)+x+1] )		fAlpha += 0.30f;
			if ( m_pAlphaUSE[(y*EM_TEXSIZE)+x-1] )		fAlpha += 0.30f;
			if ( m_pAlphaUSE[((y-1)*EM_TEXSIZE)+x-1] )	fAlpha += 0.20f;
			if ( m_pAlphaUSE[((y+1)*EM_TEXSIZE)+x-1] )	fAlpha += 0.20f;
			if ( m_pAlphaUSE[((y-1)*EM_TEXSIZE)+x+1] )	fAlpha += 0.20f;
			if ( m_pAlphaUSE[((y+1)*EM_TEXSIZE)+x+1] )	fAlpha += 0.20f;

			if ( fAlpha >= 1.f )	fAlpha = 1.f;

			pDst[x] = 0xffffff;
			pDst[x] += ((DWORD)(fAlpha*255.f))<<24;
		}
		pDst = pDst+(lrDst.Pitch/(dwBPP/8));
	}

	m_pAlphaTex->UnlockRect(0);
}

BOOL DxBlendData::CheckTileUSE ()
{
	m_dwGrid = 0;
	SAFE_DELETE_ARRAY ( m_pTileUSE );
	m_pTileUSE = new BOOL[EM_GRID*EM_GRID];
	for ( int z=0; z<EM_GRID; ++z )
	{
		for ( int x=0; x<EM_GRID; ++x )
		{
			if ( IsTileGrid ( x*4, z*4 ) )
			{
				++m_dwGrid;
				m_pTileUSE [ (z*EM_GRID)+x ] = TRUE;
			}
			else
			{
				m_pTileUSE [ (z*EM_GRID)+x ] = FALSE;
			}
		}
	}

	m_dwFaces		= m_dwGrid*2;
	m_dwVertices	= m_dwGrid*4;

	if ( m_dwFaces )	return TRUE;
	else				return FALSE;
}

BOOL DxBlendData::IsTileGrid ( int initX, int initZ )
{
	int nNUM;
	for ( int z=initZ; z<(initZ+8); ++z )
	{
		for ( int x=initX; x<(initX+8); ++x )
		{
			nNUM = (z*EM_TEXSIZE)+x;
			if ( m_pAlphaUSE[nNUM] )	return TRUE;
		}
	}
	return FALSE;
}

void DxBlendData::ModifyVB ( LPDIRECT3DDEVICEQ pd3dDevice, TILE* pTile )
{
	m_bEnable = FALSE;
	SAFE_RELEASE ( m_pVB );

	if ( !m_dwGrid )	return;

	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEX_XT2), D3DUSAGE_WRITEONLY, VERTEX_XT2::FVF, D3DPOOL_MANAGED, &m_pVB, NULL );

	VERTEX_XT2*	pVertices;
	m_pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

	DWORD	dwAdd = 0;
	for ( DWORD z=0; z<EM_GRID; ++z )
	{
		for ( DWORD x=0; x<EM_GRID; ++x )
		{
			if ( m_pTileUSE [ (z*EM_GRID)+x ] )
			{
				for ( DWORD i=0; i<4; ++i )
				{
					pVertices[(dwAdd*4)+i].vPos		= pTile[(z*EM_GRID)+x].sVertex[i].vPos;
					pVertices[(dwAdd*4)+i].vTex1	= pTile[(z*EM_GRID)+x].sVertex[i].vTex1;
					pVertices[(dwAdd*4)+i].vTex2	= pTile[(z*EM_GRID)+x].sVertex[i].vTex2;
				}

				++dwAdd;
			}
		}
	}
	m_pVB->Unlock ();

	m_bEnable = TRUE;
}

void DxBlendData::ModifyIB ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_bEnable = FALSE;
	SAFE_RELEASE ( m_pIB );

	if ( !m_dwFaces )	return;

	pd3dDevice->CreateIndexBuffer ( 3*m_dwFaces*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	WORD*	pIndices;
	m_pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );

	DWORD	dwAdd = 0;
	for ( DWORD y=0; y<EM_GRID; ++y )
	{
		for ( DWORD x=0; x<EM_GRID; ++x )
		{
			if ( m_pTileUSE[(y*EM_GRID)+x] )
			{
				*pIndices++ = (WORD)(2 + dwAdd);
				*pIndices++ = (WORD)(3 + dwAdd);
				*pIndices++ = (WORD)(0 + dwAdd);

				*pIndices++ = (WORD)(3 + dwAdd);
				*pIndices++ = (WORD)(1 + dwAdd);
				*pIndices++ = (WORD)(0 + dwAdd);

				dwAdd += 4;
			}
		}
	}
	m_pIB->Unlock ();

	m_bEnable = TRUE;
}

void DxBlendData::Modify_Height_PART ( TILE* pTile )
{
	VERTEX_XT2*	pVertices;
	m_pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

	DWORD	dwAdd = 0;
	for ( DWORD z=0; z<EM_GRID; ++z )
	{
		for ( DWORD x=0; x<EM_GRID; ++x )
		{
			if ( m_pTileUSE [ (z*EM_GRID)+x ] )
			{
				for ( DWORD i=0; i<4; ++i )
				{
					pVertices[(dwAdd*4)+i].vPos		= pTile[(z*EM_GRID)+x].sVertex[i].vPos;
				}

				++dwAdd;
			}
		}
	}
	m_pVB->Unlock ();
}

void DxBlendData::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pVB || !m_pIB || !m_dwFaces )	return;

	if( !m_pTex )	TextureManager::GetTexture( m_szBaseTex.c_str(), m_pTex );	

	pd3dDevice->SetTexture ( 0, m_pAlphaTex );
	pd3dDevice->SetTexture ( 1, m_pTex );

	pd3dDevice->SetFVF ( VERTEX_XT2::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof(VERTEX_XT2) );
	pd3dDevice->SetIndices ( m_pIB );
	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertices, 0, m_dwFaces );
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//											B	L	E	N	D			C l a s s
// -----------------------------------------------------------------------------------------------------------------------------------------
DxBlend::DxBlend() :
	m_pTileTex(NULL)
{
}

DxBlend::~DxBlend()
{
	std::for_each ( m_mapBlend.begin(), m_mapBlend.end(), std_afunc::DeleteMapObject() );
	m_mapBlend.clear();

	SAFE_DELETE_ARRAY ( m_pTileTex );
}

void DxBlend::Create ( LPDIRECT3DDEVICEQ pd3dDevice, TILE* pTile )
{
	std::for_each ( m_mapBlend.begin(), m_mapBlend.end(), std_afunc::DeleteMapObject() );	// 다 지움.
	m_mapBlend.clear();

	DxBlendData* pBlendLand = new DxBlendData;
	m_mapBlend.insert ( std::make_pair("_Eff_detail.bmp",pBlendLand) );					// 기본 생성.

	BLENDMAP_ITER _iter = m_mapBlend.begin();
	for ( ; _iter!=m_mapBlend.end(); ++_iter )
	{
		(*_iter).second->InitData ( pd3dDevice, pTile );
	}

	// 타일 정보
	SAFE_DELETE_ARRAY ( m_pTileTex );
	m_pTileTex = new TILE_TEX[EM_TEXSIZE*EM_TEXSIZE];
	for ( DWORD z=0; z<EM_TEXSIZE; ++z )
	{
		for ( DWORD x=0; x<EM_TEXSIZE; ++x )
		{
			m_pTileTex[x+(z*EM_TEXSIZE)].szTexture = "_Eff_detail.bmp";
		}
	}
}

void DxBlend::ModifyTex ( LPDIRECT3DDEVICEQ pd3dDevice, const char* strName, int nTempX, int nTempY )
{
	BLENDMAP_ITER iter;

	// Note : 텍스쳐 정보를 수정한다.
	m_pTileTex[nTempX+(nTempY*EM_TEXSIZE)].szTexture = strName;

	// Note : 이전 데이터가 없다면 생성
	iter = m_mapBlend.find ( strName );
	if ( iter == m_mapBlend.end() )
	{
		DxBlendData*	pBlendData = new DxBlendData;
		pBlendData->InitData ( pd3dDevice, strName );
		m_mapBlend.insert ( std::make_pair ( strName, pBlendData ) );
	}
	
	// Note : 알파를 사용하느냐 체크, 알파맵 수정
	iter = m_mapBlend.begin();
	for ( ; iter!=m_mapBlend.end(); ++iter )
	{
		if ( !strcmp ( (*iter).first.c_str(), strName ) )
		{
			(*iter).second->ModifyAlphaTex ( nTempX, nTempY, TRUE );
		}
		else
		{
			(*iter).second->ModifyAlphaTex ( nTempX, nTempY, FALSE );
		}
	}
}

void DxBlend::Modify ( LPDIRECT3DDEVICEQ pd3dDevice, TILE* pTile )
{
	BLENDMAP_ITER iter_DEL;
	BLENDMAP_ITER iter = m_mapBlend.begin();
	for ( ; iter!=m_mapBlend.end(); )
	{
		if ( (*iter).second->CheckTileUSE () )	{}
		else
		{
			iter_DEL = iter;
			++iter;
			m_mapBlend.erase ( iter_DEL );
			continue;
		}

		(*iter).second->ModifyVB ( pd3dDevice, pTile );
		(*iter).second->ModifyIB ( pd3dDevice );
		++iter;
	}
}

void DxBlend::Modify_Height ( TILE* pTile )
{
	BLENDMAP_ITER iter = m_mapBlend.begin();
	for ( ; iter!=m_mapBlend.end(); ++iter )
	{
		(*iter).second->Modify_Height_PART ( pTile );
	}
}

void DxBlend::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	BLENDMAP_ITER iter = m_mapBlend.begin();
	for ( ; iter!=m_mapBlend.end(); ++iter )
	{
		(*iter).second->Render ( pd3dDevice );
	}
}
BOOL DxBlend::IsEnable()
{
	BLENDMAP_ITER iter = m_mapBlend.begin();
	for ( ; iter!=m_mapBlend.end(); ++iter )
	{
		if ( !(*iter).second->IsEnable() )	return FALSE;
	}

	return TRUE;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//											C	O	L	O	R			C l a s s
// -----------------------------------------------------------------------------------------------------------------------------------------
DxColor::DxColor() :
	m_bEnable(FALSE),
	m_pTex(NULL),
	m_pColor(NULL)
{
}

DxColor::~DxColor()
{
	m_bEnable = FALSE;
	SAFE_RELEASE ( m_pTex );
	SAFE_DELETE_ARRAY ( m_pColor );
}

void DxColor::Create ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_bEnable = FALSE;

	SAFE_DELETE_ARRAY ( m_pColor );
	m_pColor = new D3DXCOLOR[EM_TEXSIZE*EM_TEXSIZE];
	for ( DWORD y=0; y<EM_TEXSIZE; ++y )
	{
		for ( DWORD x=0; x<EM_TEXSIZE; ++x )
		{
			m_pColor[(y*EM_TEXSIZE)+x] = D3DXCOLOR(0.5f,0.5f,0.5f,1.f);
		}
	}

	SAFE_RELEASE ( m_pTex );
	D3DXCreateTexture ( pd3dDevice, EM_TEXSIZE, EM_TEXSIZE, 1, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTex );

	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;
	m_pTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst       = (DWORD*)lrDst.pBits;

	DWORD dwColor = 0xff808080;
	for( DWORD y=0; y<EM_TEXSIZE; y++ )
	{
		for( DWORD x=0; x<EM_TEXSIZE; x++ )
		{
			pDst[x] = dwColor;
		}
		pDst = pDst+(lrDst.Pitch/(dwBPP/8));
	}

	m_pTex->UnlockRect(0);

	m_bEnable = TRUE;
}

void DxColor::ColorChange ( D3DXCOLOR _cColor, int _X, int _Y )
{
	if ( !m_pColor || !m_pTex )	return;

	// 컬러 데이터 수정.
	m_pColor[_X+(_Y*EM_TEXSIZE)] = _cColor;

	// 텍스쳐 수정.
	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	RECT			pRect;
	pRect.left		= 0;
	pRect.top		= 0; 
	pRect.right		= EM_TEXSIZE-1;
	pRect.bottom	= EM_TEXSIZE-1;

	m_pTex->LockRect ( 0, &lrDst, &pRect, 0 );
	DWORD* pDst       = (DWORD*)lrDst.pBits;

	pDst = pDst + ((lrDst.Pitch/(dwBPP/8))*(_Y-1));

	for( int y=(_Y-1); y<=(_Y+1); y++ )
	{
		for( int x=(_X-1); x<=(_X+1); x++ )
		{
			if ( (x<=0) || (y<=0) || (x>=(EM_TEXSIZE-1)) || (y>=(EM_TEXSIZE-1)) )	continue;

			D3DXCOLOR cColor = D3DXCOLOR ( 0.f, 0.f, 0.f, 1.f );
			cColor += m_pColor[((y+1)*EM_TEXSIZE)+x]*0.10f;
			cColor += m_pColor[((y-1)*EM_TEXSIZE)+x]*0.10f;
			cColor += m_pColor[(y*EM_TEXSIZE)+(x+1)]*0.10f;
			cColor += m_pColor[(y*EM_TEXSIZE)+(x-1)]*0.10f;
			cColor += m_pColor[((y+1)*EM_TEXSIZE)+(x+1)]*0.07f;
			cColor += m_pColor[((y-1)*EM_TEXSIZE)+(x+1)]*0.07f;
			cColor += m_pColor[((y+1)*EM_TEXSIZE)+(x-1)]*0.07f;
			cColor += m_pColor[((y-1)*EM_TEXSIZE)+(x-1)]*0.07f;
			cColor += m_pColor[(y*EM_TEXSIZE)+x]*0.32f;

			DWORD	dwColor = 0xff000000;
			dwColor += ((DWORD)(cColor.r*255.f))<<16;
			dwColor += ((DWORD)(cColor.g*255.f))<<8;
			dwColor += (DWORD)(cColor.b*255.f);
			pDst[x] = dwColor;
		}
		pDst = pDst+(lrDst.Pitch/(dwBPP/8));
	}

	m_pTex->UnlockRect(0);
}

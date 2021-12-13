#include "pch.h"

#include "./TextureManager.h"
#include "DxBackUpRendTarget.h"
#include "./SubPath.h"
#include "DxSurfaceTex.h"

#include "DxEffToTexMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEffToTexMan& DxEffToTexMan::GetInstance()
{
	static DxEffToTexMan Instance;
	return Instance;
}

DxEffToTexMan::DATA::DATA() :
	pSuf(NULL),
	pTex(NULL)
{ 
}

DxEffToTexMan::DATA::~DATA()
{
	SAFE_RELEASE( pSuf );
	SAFE_RELEASE( pTex );
}

DxEffToTexMan::DxEffToTexMan() :
	m_nTexNumber(0),
	m_nTexSize(0),
	m_bTexRepeat(FALSE),
	m_pMainSuf(NULL),
	m_pMainTex(NULL)
{
}

DxEffToTexMan::~DxEffToTexMan()
{
	CleanUp();
}

void DxEffToTexMan::CleanUp()
{
	VECDATA_ITER iter = m_vecData.begin();
	for( ; iter!=m_vecData.end(); ++iter )
	{
		SAFE_DELETE( (*iter) );
	}
	m_vecData.clear();
}

void DxEffToTexMan::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	LPDIRECT3DSURFACEQ	pBackBuffer;
	D3DSURFACE_DESC		d3dsdBackBuffer;

	pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

    pBackBuffer->GetDesc( &d3dsdBackBuffer );
	DWORD		dwWidth		= d3dsdBackBuffer.Width;
	DWORD		dwHeight	= d3dsdBackBuffer.Height;
	D3DFORMAT	cMainFormat	= d3dsdBackBuffer.Format;
    pBackBuffer->Release();

	hr = pd3dDevice->CreateTexture( dwWidth, dwHeight, 1, D3DUSAGE_RENDERTARGET, cMainFormat, D3DPOOL_DEFAULT, &m_pMainTex, NULL );
	if( FAILED(hr) )	return;
	hr = m_pMainTex->GetSurfaceLevel( 0, &m_pMainSuf );
	if( FAILED(hr) )	return;
}

void DxEffToTexMan::OnLostDevice()
{
	SAFE_RELEASE( m_pMainSuf );
	SAFE_RELEASE( m_pMainTex );

	CleanUp();
}

void DxEffToTexMan::InsertData( LPDIRECT3DDEVICEQ pd3dDevice )
{
	int nAllCount = 0;
	if( m_nTexSize==0 )
	{
		if( m_nTexNumber==0 )		nAllCount = 16;
		else if( m_nTexNumber==1 )	nAllCount = 32;
		else if( m_nTexNumber==2 )	nAllCount = 64;
	}
	else if( m_nTexSize==1 )
	{
		if( m_nTexNumber==0 )		nAllCount = 16;
		else if( m_nTexNumber==1 )	nAllCount = 25;
		else if( m_nTexNumber==2 )	nAllCount = 36;
	}

	// Note : 너무 많다면 없앤다.
	while( m_vecData.size() >= (DWORD)nAllCount )
	{
		VECDATA_ITER iter = m_vecData.begin();
		SAFE_DELETE( (*iter) );
		m_vecData.erase( iter );
	}

	D3DSURFACE_DESC	sDesc;
	LPDIRECT3DSURFACEQ pBackBuffer;
	pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc ( &sDesc );
	pBackBuffer->Release();

	HRESULT hr = S_OK;
	hr = pd3dDevice->StretchRect( pBackBuffer, NULL, m_pMainSuf, NULL, D3DTEXF_POINT );
	if( FAILED(hr) )	return;


	int nTexSize(128);
	if( m_nTexSize==0 )			nTexSize = 128;
	else if( m_nTexSize==1 )
	{
		//nTexSize = 256;

		if( m_nTexNumber==0 )		nTexSize = 256;
		else if( m_nTexNumber==1 )	nTexSize = 204;
		else if( m_nTexNumber==2 )	nTexSize = 170;
	}


	DATA* pData = new DATA;
	hr = pd3dDevice->CreateTexture( nTexSize, nTexSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pData->pTex, NULL );
	if( FAILED(hr) )	return;
	hr = pData->pTex->GetSurfaceLevel( 0, &pData->pSuf );
	if( FAILED(hr) )	return;

	// 리스트에 128 맵을 삽입
	m_vecData.push_back( pData );

	// Note : Image Copy
	DxImageMap( pd3dDevice, m_pMainTex, pData->pSuf, nTexSize );
}

void DxEffToTexMan::EndEffToTex( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	int nAllCount = 0;
	if( m_nTexSize==0 )
	{
		if( m_nTexNumber==0 )		nAllCount = 16;
		else if( m_nTexNumber==1 )	nAllCount = 32;
		else if( m_nTexNumber==2 )	nAllCount = 64;
	}
	else if( m_nTexSize==1 )
	{
		if( m_nTexNumber==0 )		nAllCount = 16;
		else if( m_nTexNumber==1 )	nAllCount = 25;
		else if( m_nTexNumber==2 )	nAllCount = 36;
	}

	if( m_vecData.size() != nAllCount )
	{
		AfxMessageBox( "갯수가 다 채워지지 않았습니다." );
		return;
	}

	// Note : 반복되는 Texture를 만들기 위해..
	if( m_bTexRepeat )
	{
		int nTexSize(128);
		if( m_nTexSize==0 )			nTexSize = 128;
		else if( m_nTexSize==1 )
		{
			if( m_nTexNumber==0 )		nTexSize = 256;
			else if( m_nTexNumber==1 )	nTexSize = 204;
			else if( m_nTexNumber==2 )	nTexSize = 170;
		}

		// Note : 알파값을 집어 넣어준다.
		int nCount = 0;
		VECDATA		vecTempData;
		VECDATA_ITER iter = m_vecData.begin();
		for( ; iter!=m_vecData.end(); ++iter, ++nCount )
		{
			DATA* pData = new DATA;
			hr = pd3dDevice->CreateTexture( nTexSize, nTexSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pData->pTex, NULL );
			if( FAILED(hr) )	return;
			hr = pData->pTex->GetSurfaceLevel( 0, &pData->pSuf );
			if( FAILED(hr) )	return;

			// 리스트에 맵을 삽입
			vecTempData.push_back( pData );

			// Note : Image Copy
			DWORD dwColor = ComputeAlpha( nCount );
			DxImageMap( pd3dDevice, (*iter)->pTex, pData->pSuf, dwColor, nTexSize );
		}

		// Note : 알파는 다 정리 했으니 값을 섞는다.
		nCount = 0;
		int nTemp = 0;
		int nSize = m_vecData.size();
		iter = m_vecData.begin();
		for( ; iter!=m_vecData.end(); ++iter, ++nCount )
		{
			nTemp = nCount + (int)(nSize*0.5f);
			if( nTemp >= (nSize-1) )	nTemp -= (nSize-1);
			DxImageMap( pd3dDevice, vecTempData[nCount]->pTex, vecTempData[nTemp]->pTex, (*iter)->pSuf, nTexSize );
		}

		// Note : 데이터 정리
		iter = vecTempData.begin();
		for( ; iter!=vecTempData.end(); ++iter )	SAFE_DELETE( (*iter) );
		vecTempData.clear();
	}

	if( m_nTexSize==0 )			EndEffToTex128( pd3dDevice );
	else if( m_nTexSize==1 )	EndEffToTex256( pd3dDevice );
}

DWORD DxEffToTexMan::ComputeAlpha( int nCount )
{
	int nAllCount = 0;
	if( m_nTexSize==0 )
	{
		if( m_nTexNumber==0 )		nAllCount = 16;
		else if( m_nTexNumber==1 )	nAllCount = 32;
		else if( m_nTexNumber==2 )	nAllCount = 64;
	}
	else if( m_nTexSize==1 )
	{
		if( m_nTexNumber==0 )		nAllCount = 16;
		else if( m_nTexNumber==1 )	nAllCount = 25;
		else if( m_nTexNumber==2 )	nAllCount = 36;
	}
	int nAllCount_Half = (int)(nAllCount*0.5f);

	// Note : 알파를 계산
	float fAlpha(0.f);
	if( m_nTexSize==1 && m_nTexNumber==1 )
	{
		if( nCount < nAllCount_Half )
		{
			fAlpha = (float)(nCount+1)/(nAllCount_Half+1);
		}
		else
		{
			nCount -= (nAllCount-1);
			nCount = abs(nCount);
			fAlpha = (float)(nCount+1)/(nAllCount_Half+1);
		}
	}
	else
	{
		if( nCount < nAllCount_Half )
		{
			fAlpha = (float)(nCount+1)/nAllCount_Half;
		}
		else
		{
			nCount -= (nAllCount-1);
			nCount = abs(nCount);
			fAlpha = (float)(nCount+1)/nAllCount_Half;
		}
	}

	DWORD dwColor = DWORD(fAlpha*255.f);
	dwColor = (0xff<<24) + (dwColor<<16) + (dwColor<<8) + dwColor;

	return dwColor;
}

void DxEffToTexMan::EndEffToTex128( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : Base Job
	int nTotalCol, nTotalRow;
	LPDIRECT3DTEXTUREQ	pTex;
	LPDIRECT3DSURFACEQ	pSuf;

	if( m_nTexNumber==0 )		// 16개	512*512		4*4
	{
		nTotalCol = 4;
		nTotalRow = 4;
		pd3dDevice->CreateTexture( 512, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex, NULL );
		pTex->GetSurfaceLevel( 0, &pSuf );
	}
	else if( m_nTexNumber==1 )	// 32개	1024*512	8*4
	{
		nTotalCol = 8;
		nTotalRow = 4;
		pd3dDevice->CreateTexture( 1024, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex, NULL );
		pTex->GetSurfaceLevel( 0, &pSuf );
	}
	else if( m_nTexNumber==2 )	// 64개	1024*1024	8*8
	{
		nTotalCol = 8;
		nTotalRow = 8;
		pd3dDevice->CreateTexture( 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex, NULL );
		pTex->GetSurfaceLevel( 0, &pSuf );
	}

	// 어둡게 만듬.
	DxBackUpRendTarget sTarget( pd3dDevice );
	pd3dDevice->SetRenderTarget( 0, pSuf );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.f, 0L );
	sTarget.RestoreTarget( pd3dDevice );

	// Note : Texture Copy
	int		nCol, nRow;
	int		nCount = 0;
	POINT	sPoint;
	RECT	sRect;
	RECT	sRect2;

	int nAllCount = nTotalCol*nTotalRow;
	for( int i=0; i<nAllCount; ++i )
	{
		nCol = i%nTotalCol;
		nRow = i/nTotalCol;
		sPoint.x = nCol*128;
		sPoint.y = nRow*128;

		sRect.left		= 0;
		sRect.top		= 0;
		sRect.right		= 128;
		sRect.bottom	= 128;

		sRect2.left		= sPoint.x;
		sRect2.top		= sPoint.y;
		sRect2.right	= sPoint.x+128;
		sRect2.bottom	= sPoint.y+128;

		pd3dDevice->StretchRect( m_vecData[i]->pSuf, &sRect, pSuf, &sRect2, D3DTEXF_POINT );
	}

	// Note : Make Texture
	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, SUBPATH::APP_ROOT );
	StringCchCat( szPathName, MAX_PATH, SUBPATH::TEXTURE_FILE_EFF );
	StringCchCat( szPathName, MAX_PATH, "_Sequence.dds" );

	D3DXSaveTextureToFile( szPathName, D3DXIFF_DDS, pTex, NULL );

	if( m_bTexRepeat )	MakeSequenceRepeat( pd3dDevice, pTex );	// Image Repeat

	TextureManager::ReScanTexture ();		// Texture ReScan

	SAFE_RELEASE( pTex );
	SAFE_RELEASE( pSuf );
}

void DxEffToTexMan::EndEffToTex256( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : Base Job
	int nTotalCol, nTotalRow;
	LPDIRECT3DTEXTUREQ	pTex;
	LPDIRECT3DSURFACEQ	pSuf;

	int nTexSizeX = 256;
	int nTexSizeY = 256;

	if( m_nTexNumber==0 )		// 16개	512*512		4*4
	{
		nTotalCol = 4;
		nTotalRow = 4;
		pd3dDevice->CreateTexture( 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex, NULL );
		pTex->GetSurfaceLevel( 0, &pSuf );
	}
	else if( m_nTexNumber==1 )	// 25개
	{
		nTotalCol = 5;
		nTotalRow = 5;	// 204 * 5 = 1020
		nTexSizeX = 204;
		nTexSizeY = 204;
		pd3dDevice->CreateTexture( 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex, NULL );
		pTex->GetSurfaceLevel( 0, &pSuf );
	}
	else if( m_nTexNumber==2 )	// 36개
	{
		nTotalCol = 6;	// 204 * 5 = 1020
		nTotalRow = 6;	// 170 * 6 = 1020
		nTexSizeX = 170;
		nTexSizeY = 170;
		pd3dDevice->CreateTexture( 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex, NULL );
		pTex->GetSurfaceLevel( 0, &pSuf );
	}

	// 어둡게 만듬.
	DxBackUpRendTarget sTarget( pd3dDevice );
	pd3dDevice->SetRenderTarget( 0, pSuf );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.f, 0L );
	sTarget.RestoreTarget( pd3dDevice );

	// Note : Texture Copy
	int		nCol, nRow;
	int		nCount = 0;
	POINT	sPoint;
	RECT	sRect;
	RECT	sRect2;
	
	int nAllCount = nTotalCol*nTotalRow;
	for( int i=0; i<nAllCount; ++i )
	{
		nCol = i%nTotalCol;
		nRow = i/nTotalCol;
		sPoint.x = nCol*nTexSizeX;
		sPoint.y = nRow*nTexSizeY;

		sRect.left		= 0;
		sRect.top		= 0;
		sRect.right		= nTexSizeX;
		sRect.bottom	= nTexSizeY;

		sRect2.left		= sPoint.x;
		sRect2.top		= sPoint.y;
		sRect2.right	= sPoint.x+nTexSizeX;
		sRect2.bottom	= sPoint.y+nTexSizeY;

		pd3dDevice->StretchRect( m_vecData[i]->pSuf, &sRect, pSuf, &sRect2, D3DTEXF_POINT );
	}

	// Note : Make Texture
	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, SUBPATH::APP_ROOT );
	StringCchCat( szPathName, MAX_PATH, SUBPATH::TEXTURE_FILE_EFF );
	StringCchCat( szPathName, MAX_PATH, "_Sequence.dds" );

	D3DXSaveTextureToFile( szPathName, D3DXIFF_DDS, pTex, NULL );

	TextureManager::ReScanTexture ();		// Texture ReScan

	if( m_bTexRepeat )	MakeSequenceRepeat( pd3dDevice, pTex );	// Image Repeat

	SAFE_RELEASE( pTex );
	SAFE_RELEASE( pSuf );
}

void DxEffToTexMan::MakeSequenceRepeat( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTex )
{
	// Note : 이때 일 때만 홀수이다.
	if( (m_nTexSize==1) && (m_nTexNumber==1) )
	{
	}
}

void DxEffToTexMan::SequnceMake_00( int nCount, BOOL bFace, int nBaseCol, VERTEXCOLORRHW sVertex[] )	// 짝수
{
	int nCol = 	nCount % nBaseCol;
	int nRow = 	nCount / nBaseCol;
	float fCol = (float)nCol/nBaseCol;
	float fRow = (float)nRow/nBaseCol;
	float fOne = 1.f/nBaseCol;
	float fCol_Add = fCol + fOne;
	float fRow_Add = fRow + fOne;

	int nBaseCount = nBaseCol*nBaseCol;
	int nBaseCol_H = int(nBaseCol*0.5f);

	float fAlpha(0.f);
	if( nCount < nBaseCol_H )
	{
		fAlpha = (float)nCount/(nBaseCol_H-1);
	}
	else
	{
		nCount -= (nBaseCount-1);
		nCount = abs(nCount);
		fAlpha = (float)nCount/(nBaseCol_H-1);
	}

    for( DWORD i=0; i<4; ++i )
	{
		sVertex[i].dwColor = (DWORD(fAlpha*255.f)<<24) + 0xffffff;
	}

	float fRate = 1020.f/1024.f;

	fCol		*= fRate;
	fRow		*= fRate;
	fCol_Add	*= fRate;
	fRow_Add	*= fRate;

	if( bFace )
	{
		sVertex[0].vTex = D3DXVECTOR2( fCol,		fRow );
		sVertex[1].vTex = D3DXVECTOR2( fCol_Add,	fRow );
		sVertex[2].vTex = D3DXVECTOR2( fCol,		fRow_Add );
		sVertex[3].vTex = D3DXVECTOR2( fCol_Add,	fRow_Add );
	}
	else
	{
		sVertex[0].vTex = D3DXVECTOR2( fCol_Add,	fRow );
		sVertex[1].vTex = D3DXVECTOR2( fCol,		fRow );
		sVertex[2].vTex = D3DXVECTOR2( fCol_Add,	fRow_Add );
		sVertex[3].vTex = D3DXVECTOR2( fCol,		fRow_Add );
	}
}

void DxEffToTexMan::SequnceMake_01( int nCount, BOOL bFace, int nBaseCol, VERTEXCOLORRHW sVertex[] )	// 홀수 
{
	int nCol = 	nCount % nBaseCol;
	int nRow = 	nCount / nBaseCol;
	float fCol = (float)nCol/nBaseCol;
	float fRow = (float)nRow/nBaseCol;
	float fOne = 1.f/nBaseCol;
	float fCol_Add = fCol + fOne;
	float fRow_Add = fRow + fOne;

	int nBaseCount = nBaseCol*nBaseCol;
	int nBaseCol_H = int(nBaseCol*0.5f);

	float fAlpha(0.f);
	if( nCount < nBaseCol_H )
	{
		fAlpha = (float)nCount/(nBaseCol_H);
	}
	else
	{
		nCount -= (nBaseCount-1);
		nCount = abs(nCount);
		fAlpha = (float)nCount/(nBaseCol_H);
	}

    for( DWORD i=0; i<4; ++i )
	{
		sVertex[i].dwColor = (DWORD(fAlpha*255.f)<<24) + 0xffffff;
	}

	float fRate = 1020.f/1024.f;

	fCol		*= fRate;
	fRow		*= fRate;
	fCol_Add	*= fRate;
	fRow_Add	*= fRate;

	if( bFace )
	{
		sVertex[0].vTex = D3DXVECTOR2( fCol,		fRow );
		sVertex[1].vTex = D3DXVECTOR2( fCol_Add,	fRow );
		sVertex[2].vTex = D3DXVECTOR2( fCol,		fRow_Add );
		sVertex[3].vTex = D3DXVECTOR2( fCol_Add,	fRow_Add );
	}
	else
	{
		sVertex[0].vTex = D3DXVECTOR2( fCol_Add,	fRow );
		sVertex[1].vTex = D3DXVECTOR2( fCol,		fRow );
		sVertex[2].vTex = D3DXVECTOR2( fCol_Add,	fRow_Add );
		sVertex[3].vTex = D3DXVECTOR2( fCol,		fRow_Add );
	}
}


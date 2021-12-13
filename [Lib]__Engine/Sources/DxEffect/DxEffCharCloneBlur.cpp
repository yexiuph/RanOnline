#include "pch.h"

#include "./DxEffectMan.h"

#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./DxEffCharCloneBlur.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffCharCloneBlur::BLURTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
DWORD		DxEffCharCloneBlur::TYPEID			= EMEFFCHAR_CLONEBLUR;
DWORD		DxEffCharCloneBlur::VERSION			= 0x0100;
char		DxEffCharCloneBlur::NAME[MAX_PATH]	= "2.캐릭터 잔상 2";

LPDIRECT3DSTATEBLOCK9		DxEffCharCloneBlur::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharCloneBlur::m_pEffectStateBlock	= NULL;

DxEffCharCloneBlur::DxEffCharCloneBlur(void) :
	DxEffChar(),
	m_dwCount(0),
	m_nAlphaDeltaUP(0),
	m_nAlphaDeltaDOWN(0),
	m_vTransUP(0.f,0.f,0.f),
	m_vTransDOWN(0.f,0.f,0.f),
	m_vPrevUP(0.f,0.f,0.f),
	m_vPrevDOWN(0.f,0.f,0.f),
	m_vPrevPrevUP(0.f,0.f,0.f),
	m_vPrevPrevDOWN(0.f,0.f,0.f),
	m_fTime(0.f),
	m_fThisTime(0.f),
	m_fFPSSumTime(0.f),
	m_fElapsedTime(0.0f),
	m_nNowFaces(0),
	m_nPlayFaces(0),
	m_pVB(NULL),
	m_pIB(NULL),
	m_iBlurNum(0),
	m_iAdd(0),
	m_pd3dDevice(NULL),
	m_pddsTexture(NULL)
{
	memset( m_szTraceUP, 0, sizeof(char)*STRACE_NSIZE );
	memset( m_szTraceDOWN, 0, sizeof(char)*STRACE_NSIZE );
}

DxEffCharCloneBlur::~DxEffCharCloneBlur(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharCloneBlur::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharCloneBlur *pEffChar = new DxEffCharCloneBlur;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharCloneBlur::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, 0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_BORDER );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}


	return S_OK;
}

HRESULT DxEffCharCloneBlur::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharCloneBlur::OneTimeSceneInit ()
{

	return S_OK;
}

HRESULT DxEffCharCloneBlur::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	D3DXVECTOR3	vNormal;

	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
	//
	if ( m_szTraceUP[0] == NULL )	return E_FAIL;
	if ( m_szTraceDOWN[0] == NULL )	return E_FAIL;
	
	if ( m_pCharPart )
	{
		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return E_FAIL;

		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
	}
	else if ( m_pSkinPiece )
	{
		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return E_FAIL;

		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
	}

	//	Note : 텍스쳐 로드
	//
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0, TRUE );

	CreateBlurVB ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharCloneBlur::CreateBlurVB( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;


	BLURTEX*		pVertexC;
	WORD*			pIndices;

	SAFE_RELEASE ( m_pVB );

	if ( m_dwFaces==0 )	return S_OK;

	pd3dDevice->CreateVertexBuffer ( (m_dwFaces*4)*sizeof(BLURTEX), D3DUSAGE_DYNAMIC, BLURTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL  );

	if ( !m_pVB )		return S_OK;

	hr = m_pVB->Lock ( 0, 0, (VOID**)&pVertexC, D3DLOCK_DISCARD );
	if ( FAILED(hr) )	return E_FAIL;
	

	for( DWORD i=0; i<m_dwFaces; i++ )
	{
		pVertexC->p = m_vTransUP;
		pVertexC->d = D3DCOLOR_ARGB( 255, 255, 255, 255 );
		pVertexC->t = D3DXVECTOR2( 0.f, 0.0f );
		pVertexC++;

		pVertexC->p = m_vTransDOWN;
		pVertexC->d = D3DCOLOR_ARGB( 255, 255, 255, 255 );
		pVertexC->t = D3DXVECTOR2( 1.f, 0.0f );
		pVertexC++;

		pVertexC->p = m_vTransUP;
		pVertexC->d = D3DCOLOR_ARGB( 255, 255, 255, 255 );
		pVertexC->t = D3DXVECTOR2( 0.f, 1.0f );
		pVertexC++;

		pVertexC->p = m_vTransDOWN;
		pVertexC->d = D3DCOLOR_ARGB( 255, 255, 255, 255 );
		pVertexC->t = D3DXVECTOR2( 1.f, 1.0f );
		pVertexC++;
	}

	m_pVB->Unlock();


	SAFE_RELEASE ( m_pIB );
	hr = pd3dDevice->CreateIndexBuffer ( (m_dwFaces*6)*sizeof(WORD), D3DUSAGE_WRITEONLY,	D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( FAILED(hr) )	return hr;

	hr = m_pIB->Lock ( 0, 0, (VOID**)&pIndices, 0L );
	if ( FAILED(hr) )	return E_FAIL;

	for( DWORD i=0; i<m_dwFaces; i++ )
	{
		*pIndices++ = (WORD)(i*4+0);
		*pIndices++ = (WORD)(i*4+1);
		*pIndices++ = (WORD)(i*4+2);

   		*pIndices++ = (WORD)(i*4+1);
		*pIndices++ = (WORD)(i*4+3);
		*pIndices++ = (WORD)(i*4+2);
	}

	m_pIB->Unlock();


	//	Note : 알파 빼야 할 값을 지정
	//
	int FirstAlpha1 = ((m_vColorUP&0xff000000)>>24);
	m_nAlphaDeltaUP = FirstAlpha1 / m_dwFaces + 1;
	int FirstAlpha2 = ((m_vColorDOWN&0xff000000)>>24);
	m_nAlphaDeltaDOWN = FirstAlpha2 / m_dwFaces + 1;

	//	Note : 보간을 할때 사용 한다.
	//
	if ( m_bBezier )
	{
		m_nAlphaDeltaUP *= 2;
		m_nAlphaDeltaDOWN *= 2;
	}

	return S_OK;
}

HRESULT DxEffCharCloneBlur::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffCharCloneBlur::InvalidateDeviceObjects ()
{
	m_pd3dDevice = NULL;

	return S_OK;
}

HRESULT DxEffCharCloneBlur::DeleteDeviceObjects ()
{
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );

	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	return S_OK;
}

HRESULT DxEffCharCloneBlur::FinalCleanup ()
{

	return S_OK;
}

HRESULT DxEffCharCloneBlur::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fTime = fTime;
	m_fElapsedTime = fElapsedTime;

	return S_OK;
}

void DxEffCharCloneBlur::Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	if ( !m_pVB )	return;

	HRESULT hr = S_OK;

	//	Note : 초기화 해야 할 상황.. 1.맨 처음 보일때, 2.보였다 사라진후 다시 보일때..
	//
	ResetBlur ();


	m_fFPSSumTime += m_fElapsedTime;

	if ( m_fFPSSumTime > 0.03f )
	{
		m_fFPSSumTime = 0.0f;

		if ( m_CurAniMType == AN_ATTACK )	m_bUse = true;
		else								m_bUse = false;

		CreateVertex ();
		UpdateDiffuse ();
	}

	//	Note : Render ( 잔상 )
	//
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	DWORD	dwFVFSize;

	dwFVFSize = BLURTEX::FVF;
	dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

	// Note : Get Texture
	if( !m_pddsTexture )	TextureManager::GetTexture( m_szTexture, m_pddsTexture );

	pd3dDevice->SetFVF ( BLURTEX::FVF );
	pd3dDevice->SetTexture ( 0, m_pddsTexture );

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );
	DWORD	dwSrcBlend, dwDestBlend, dwColorOP, dwZWriteEnable, dwAlphaBlendEnable,
			dwAlpgaTestEnable, dwAlphaRef, dwAlphaFunc;

	pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
	pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHATESTENABLE,		&dwAlpgaTestEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAREF,			&dwAlphaRef );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAFUNC,			&dwAlphaFunc );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,	&dwColorOP );

	//	Note : 블렌딩 요소 셋팅
	//
	switch ( m_nBlend )
	{
	case 1 :
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		break;
	case 2:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

		break;
	case 3:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );

		break;			
	case 4:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		break;
	case 5:
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}
		break;
	}



	pd3dDevice->SetStreamSource ( 0, m_pVB, 0, dwFVFSize );
	pd3dDevice->SetIndices ( m_pIB );


	if ( m_nPlayFaces == m_dwFaces )	// 다 되었을 때. 
	{
		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwFaces*4, 0, m_dwFaces*2 );
	}
	else if ( m_nPlayFaces == 0 )
	{
	}
	else
	{
		int iFaces;
		int iRemain = m_nPlayFaces;			// 전체 써야할 면 갯수
		iFaces = m_dwFaces-m_nNowFaces;		// 뒷편의 남은 면 갯수.
		iFaces = (iRemain < iFaces) ? iRemain : iFaces;
		iRemain -= iFaces;
		if ( iRemain > 0)	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, iRemain*4, 0, iRemain*2 );
		if ( iFaces > 0)	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, m_nNowFaces*4, iFaces*4, m_nNowFaces*6, iFaces*2 );
	}
	
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,		dwAlpgaTestEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC,			dwAlphaFunc );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	dwColorOP );

	m_pSavedStateBlock->Apply();
}

HRESULT DxEffCharCloneBlur::UpdateDiffuse ()
{
	BLURTEX*	pVetices;

	m_pVB->Lock ( 0, 0, (VOID**)&pVetices , D3DLOCK_DISCARD );

	if ( m_nPlayFaces == m_dwFaces )	// 다 되었을 때. 
	{
		for ( DWORD i=0; i<m_dwFaces; i ++ )
		{
			int		nAlphaUP	= pVetices[ i*4+0 ].d >> 24;
			DWORD	dwColorUP	= pVetices[ i*4+0 ].d & 0xffffff;
			nAlphaUP			= nAlphaUP - m_nAlphaDeltaUP;
			if ( nAlphaUP < 0 )	nAlphaUP = 0;
			pVetices[ i*4+0 ].d = (nAlphaUP<<24) + dwColorUP;
			pVetices[ i*4+1 ].d = (nAlphaUP<<24) + dwColorUP;

			int		nAlphaDOWN	= pVetices[ i*4+2 ].d >> 24;
			DWORD	dwColorDOWN	= pVetices[ i*4+2 ].d & 0xffffff;
			nAlphaDOWN			= nAlphaDOWN - m_nAlphaDeltaDOWN;
			if ( nAlphaDOWN < 0 )	nAlphaDOWN = 0;
			pVetices[ i*4+2 ].d = (nAlphaDOWN<<24) + dwColorDOWN;
			pVetices[ i*4+3 ].d = (nAlphaDOWN<<24) + dwColorDOWN;
		}
	}
	else if ( m_nPlayFaces == 0 )	{}
	else
	{
		int iFaces;
		int iRemain = m_nPlayFaces;			// 전체 써야할 면 갯수
		iFaces = m_dwFaces-m_nNowFaces;		// 뒷편의 남은 면 갯수.
		iFaces = (iRemain < iFaces) ? iRemain : iFaces;
		iRemain -= iFaces;

		for ( int i=0; i<iRemain; i ++ )
		{
			int		nAlphaUP	= pVetices[ i*4+0 ].d >> 24;
			DWORD	dwColorUP	= pVetices[ i*4+0 ].d & 0xffffff;
			nAlphaUP			= nAlphaUP - m_nAlphaDeltaUP;
			if ( nAlphaUP < 0 )	nAlphaUP = 0;
			pVetices[ i*4+0 ].d = (nAlphaUP<<24) + dwColorUP;
			pVetices[ i*4+1 ].d = (nAlphaUP<<24) + dwColorUP;

			int		nAlphaDOWN	= pVetices[ i*4+2 ].d >> 24;
			DWORD	dwColorDOWN	= pVetices[ i*4+2 ].d & 0xffffff;
			nAlphaDOWN			= nAlphaDOWN - m_nAlphaDeltaDOWN;
			if ( nAlphaDOWN < 0 )	nAlphaDOWN = 0;
			pVetices[ i*4+2 ].d = (nAlphaDOWN<<24) + dwColorDOWN;
			pVetices[ i*4+3 ].d = (nAlphaDOWN<<24) + dwColorDOWN;
		}

		for ( int i=m_nNowFaces; i<m_nNowFaces+iFaces; i ++ )
		{
			int		nAlphaUP	= pVetices[ i*4+0 ].d >> 24;
			DWORD	dwColorUP	= pVetices[ i*4+0 ].d & 0xffffff;
			nAlphaUP			= nAlphaUP - m_nAlphaDeltaUP;
			if ( nAlphaUP < 0 )	nAlphaUP = 0;
			pVetices[ i*4+0 ].d = (nAlphaUP<<24) + dwColorUP;
			pVetices[ i*4+1 ].d = (nAlphaUP<<24) + dwColorUP;

			int		nAlphaDOWN	= pVetices[ i*4+2 ].d >> 24;
			DWORD	dwColorDOWN	= pVetices[ i*4+2 ].d & 0xffffff;
			nAlphaDOWN			= nAlphaDOWN - m_nAlphaDeltaDOWN;
			if ( nAlphaDOWN < 0 )	nAlphaDOWN = 0;
			pVetices[ i*4+2 ].d = (nAlphaDOWN<<24) + dwColorDOWN;
			pVetices[ i*4+3 ].d = (nAlphaDOWN<<24) + dwColorDOWN;
		}
	}

	m_pVB->Unlock ();

	return S_OK;
}

HRESULT	DxEffCharCloneBlur::CreateVertex ()
{
	if ( !m_bUse )
	{
		m_nPlayFaces--;
		if ( m_nPlayFaces < 0 )	m_nPlayFaces = 0;

		//	Note : 보간을 할때 사용 한다.
		//
		if ( m_bBezier )
		{
			m_nPlayFaces--;
			if ( m_nPlayFaces < 0 )	m_nPlayFaces = 0;
		}

		return S_OK;
	}

	HRESULT	hr;

	D3DXVECTOR3		vTempUP;
	D3DXVECTOR3		vBezierPosUP, vBezierPosDOWN;
	D3DXVECTOR3		 vNormal;

	BLURTEX*	pVetices;

	if ( m_pCharPart )
	{
		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return E_FAIL;

		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
	}
	else if ( m_pSkinPiece )
	{
		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return E_FAIL;

		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
	}
	
	if ( m_vTransUP == m_vPrevUP )	return S_OK;

	//	Note : 보간을 할때 사용 한다.
	//
	if ( m_bBezier )
	{
		vBezierPosUP = DxBezier ( m_vTransUP, m_vPrevUP, m_vPrevPrevUP );
		vBezierPosDOWN = DxBezier ( m_vTransDOWN, m_vPrevDOWN, m_vPrevPrevDOWN );
	}

	m_pVB->Lock ( 0, 0, (VOID**)&pVetices , D3DLOCK_DISCARD );


	//	Note : 새로운 점을 생성
	//
	if ( m_nPlayFaces == 0 )	{}
	else
	{
		//	Note : 보간을 할때 사용 한다.
		//
		if ( m_bBezier )
		{
			vTempUP = vBezierPosUP - m_vPrevUP;

			D3DXVec3Normalize ( &vTempUP, &vTempUP );

			pVetices[ m_nNowFaces*4+0 ].p = vBezierPosUP - vTempUP*m_fWidth;
			pVetices[ m_nNowFaces*4+1 ].p = vBezierPosUP;
			pVetices[ m_nNowFaces*4+2 ].p = vBezierPosDOWN - vTempUP*m_fWidth;
			pVetices[ m_nNowFaces*4+3 ].p = vBezierPosDOWN;

			int		nAlphaUP	= m_vColorUP >> 24;
			DWORD	dwColorUP	= m_vColorUP & 0xffffff;
			nAlphaUP			= nAlphaUP - m_nAlphaDeltaUP/2;
			if ( nAlphaUP < 0 )	nAlphaUP = 0;
			pVetices[ m_nNowFaces*4+0 ].d = (nAlphaUP<<24) + dwColorUP;
			pVetices[ m_nNowFaces*4+1 ].d = (nAlphaUP<<24) + dwColorUP;

			int		nAlphaDOWN	= m_vColorDOWN >> 24;
			DWORD	dwColorDOWN	= m_vColorDOWN & 0xffffff;
			nAlphaDOWN			= nAlphaDOWN - m_nAlphaDeltaDOWN/2;
			if ( nAlphaDOWN < 0 )	nAlphaDOWN = 0;
			pVetices[ m_nNowFaces*4+2 ].d = (nAlphaDOWN<<24) + dwColorDOWN;
			pVetices[ m_nNowFaces*4+3 ].d = (nAlphaDOWN<<24) + dwColorDOWN;

			m_nNowFaces++;

			if ( (int)m_dwFaces <= m_nNowFaces )	// 만약 값이 넘어 간다면...
			{
				m_nNowFaces = 0;
			}
		}

		if ( m_bBezier )	vTempUP = m_vTransUP - vBezierPosUP;
		else				vTempUP = m_vTransUP - m_vPrevUP;

		D3DXVec3Normalize ( &vTempUP, &vTempUP );

		pVetices[ m_nNowFaces*4+0 ].p = m_vTransUP - vTempUP*m_fWidth;
		pVetices[ m_nNowFaces*4+1 ].p = m_vTransUP;
		pVetices[ m_nNowFaces*4+2 ].p = m_vTransDOWN - vTempUP*m_fWidth;
		pVetices[ m_nNowFaces*4+3 ].p = m_vTransDOWN;

		pVetices[ m_nNowFaces*4+0 ].d = m_vColorUP;
		pVetices[ m_nNowFaces*4+1 ].d = m_vColorUP;
		pVetices[ m_nNowFaces*4+2 ].d = m_vColorDOWN;
		pVetices[ m_nNowFaces*4+3 ].d = m_vColorDOWN;

		m_nNowFaces++;

		if ( (int)m_dwFaces <= m_nNowFaces )	// 만약 값이 넘어 간다면...
		{
			m_nNowFaces = 0;
		}
	}

	m_pVB->Unlock ();

	m_nPlayFaces++;
	if ( m_nPlayFaces > (int)m_dwFaces )	m_nPlayFaces = m_dwFaces;

	//	Note : 보간을 할때 사용 한다.
	//
	if ( m_bBezier )
	{
		m_nPlayFaces++;
		if ( m_nPlayFaces > (int)m_dwFaces )	m_nPlayFaces = m_dwFaces;
	}

	//	이전 프레임 값 넣기
	m_vPrevPrevUP = m_vPrevUP;
	m_vPrevPrevDOWN = m_vPrevDOWN;

	m_vPrevUP = m_vTransUP;
	m_vPrevDOWN = m_vTransDOWN;

	return S_OK;
}

HRESULT	DxEffCharCloneBlur::ResetBlur ()
{
	HRESULT	hr;
	D3DXVECTOR3	vNormal;

	if ( m_fThisTime != 0.f )	m_fThisTime += m_fElapsedTime;

	if ( m_fTime - m_fThisTime > 0.3f || m_fThisTime == 0.f)
	{
		if ( m_pCharPart )
		{
			hr = m_pCharPart->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
			if ( FAILED(hr) )	return E_FAIL;

			hr = m_pCharPart->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
			if ( FAILED(hr) )	return E_FAIL;
		}
		else if ( m_pSkinPiece )
		{
			hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
			if ( FAILED(hr) )	return E_FAIL;

			hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
			if ( FAILED(hr) )	return E_FAIL;
		}

		//	이전 프레임 값 넣기
		m_vPrevPrevUP = m_vPrevUP = m_vTransUP;
		m_vPrevPrevDOWN = m_vPrevDOWN = m_vTransDOWN;

		m_nPlayFaces = 0;		// 초기화 아무것도 안뿌리게 해 준다.

		BLURTEX*	pVetices;

		m_pVB->Lock ( 0, 0, (VOID**)&pVetices , D3DLOCK_DISCARD );
		for ( DWORD i=0; i<m_dwFaces; i ++ )
		{
			pVetices[ i*4+0 ].p = m_vTransUP;
			pVetices[ i*4+1 ].p = m_vTransUP;
			pVetices[ i*4+2 ].p = m_vTransDOWN;
			pVetices[ i*4+3 ].p = m_vTransDOWN;

			pVetices[ i*4+0 ].d = 0x00000000;
			pVetices[ i*4+1 ].d = 0x00000000;
			pVetices[ i*4+2 ].d = 0x00000000;
			pVetices[ i*4+3 ].d = 0x00000000;
		}
		m_pVB->Unlock ();

		m_fThisTime = m_fTime;
	}

	return S_OK;
}

HRESULT	DxEffCharCloneBlur::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;

	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharCloneBlur::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}


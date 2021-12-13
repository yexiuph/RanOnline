#include "pch.h"
#include "./TextUtil.h"
#include "./TextGroup.h"
#include "./TextObject.h"
#include "./TextNode.h"
#include "./TextTexture.h"
#include "./TextTexture16.h"
#include "./TextTexture32.h"

#include "./D3DFontX.h"

#include "../[Lib]__MfcEx/Sources/GetWinVer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	const INT T_SIZE_SMALL = 256;				// 작은 텍스쳐 크기
	const INT T_SIZE_BIG = 512;					// 큰 텍스쳐 크기
	const INT T_SELECT = 32;					// T_SELECT 기준으로 텍스쳐 크기 결정
	const INT T_MAX_COUNT = 30;					// 최대 텍스쳐 갯수
	const INT T_INIT_COUNT = 2;					// 데이터 없어도 살려둘 최소한의 텍스쳐 갯수 ( 폰트별 )
	const DWORD O_ELAPS_FRAME = 2000;			// 오래된 텍스트 삭제할 타이밍
	const DWORD T_ELAPS_FRAME = 5000;			// 비어 있는 텍스쳐 삭제할 타이밍
}

BOOL CTextUtil::m_bUsage = FALSE;
BOOL CTextUtil::m_bUsageRenderQueue = FALSE;	// 사용하려면 크리티컬 섹션 써야함
BOOL CTextUtil::m_bBlurFilter = TRUE;
INT CTextUtil::m_iSpaceSize = 8;
INT CTextUtil::m_iSpaceSizeHalf = 4;

CTextUtil* CTextUtil::m_cpTextUtil = NULL;

CTextUtil::CTextUtil()
	: m_pd3dDevice( NULL )
	, m_pSavedSB( NULL )
	, m_pTextureSB( NULL )

	, m_iTextureSizeSmall( T_SIZE_SMALL )
	, m_iTextureSizeBig( T_SIZE_BIG )

	, m_dwLastTick( 0 )
{
	#if defined( KRT_PARAM ) || defined ( _RELEASED )
		m_bUsage = TRUE;
	#endif
}

CTextUtil::~CTextUtil()
{
	for( TEXTURE_ITOR ti = m_mmapTexture.begin(); ti != m_mmapTexture.end(); ++ti )
		SAFE_DELETE( ti->second );
	m_mmapTexture.clear();

	for( GROUP_LIST::iterator gi = m_listGroup.begin(); gi != m_listGroup.end(); ++gi )
		SAFE_DELETE( *gi );
	m_listGroup.clear();
}

VOID CTextUtil::OneTimeSceneInit()
{
	INT nWinVer;
	TCHAR szWinVer[ 50 ], szMajorMinorBuild[ 50 ];
	if( GetWinVer( szWinVer, &nWinVer, szMajorMinorBuild ) )
	{
		if( nWinVer < WNTFIRST )
			m_bUsage = FALSE;
	}
}

VOID CTextUtil::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice, SIZE* sizeTexture )
{
	if( !m_bUsage )
		return;

	m_pd3dDevice = pd3dDevice;

	m_iTextureSizeSmall = min( T_SIZE_SMALL, sizeTexture->cx );
	m_iTextureSizeSmall = min( m_iTextureSizeSmall, sizeTexture->cy );

	m_iTextureSizeBig = min( T_SIZE_BIG, sizeTexture->cx );
	m_iTextureSizeBig = min( m_iTextureSizeBig, sizeTexture->cy );

	for( TEXTURE_ITOR ti = m_mmapTexture.begin(); ti != m_mmapTexture.end(); ++ti )
		ti->second->InitDeviceObjects( pd3dDevice );
}

VOID CTextUtil::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bUsage )
		return;

	m_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		m_pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
		pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

		if( which == 0 )	m_pd3dDevice->EndStateBlock( &m_pSavedSB );
		else				m_pd3dDevice->EndStateBlock( &m_pTextureSB );
	}

	for( TEXTURE_ITOR ti = m_mmapTexture.begin(); ti != m_mmapTexture.end(); ++ti )
		ti->second->RestoreDeviceObjects( pd3dDevice );

	m_dwLastTick = GetTickCount();
}

VOID CTextUtil::FrameMove()
{
	if( !m_pd3dDevice )
		return;

	m_dwLastTick = GetTickCount();
	static DWORD dwLastDeleteO = m_dwLastTick;
	static DWORD dwLastDeleteT = m_dwLastTick;

	if( m_dwLastTick - dwLastDeleteO > O_ELAPS_FRAME )
	{
		dwLastDeleteO = m_dwLastTick;

        for( GROUP_LIST::iterator gi = m_listGroup.begin(); gi != m_listGroup.end(); ++gi )
			( *gi )->FrameMovePrev();
		for( TEXTURE_ITOR ti = m_mmapTexture.begin(); ti != m_mmapTexture.end(); ++ti )
			ti->second->FrameMove();
		for( GROUP_LIST::iterator gi = m_listGroup.begin(); gi != m_listGroup.end(); ++gi )
			( *gi )->FrameMoveNext();
	}
	else
	{
		for( TEXTURE_ITOR ti = m_mmapTexture.begin(); ti != m_mmapTexture.end(); ++ti )
			ti->second->FrameMove();
	}

	if( m_dwLastTick - dwLastDeleteT > T_ELAPS_FRAME )
	{
		dwLastDeleteT = m_dwLastTick;

		TEXTURE_ITOR ti = m_mmapTexture.begin();
		while( ti != m_mmapTexture.end() )
		{
			if( ti->second->IsEmpty() == FALSE )
			{
				++ti;
				continue;
			}

			if( m_mmapTexture.count( ti->first ) <= T_INIT_COUNT )
			{
				++ti;
				continue;
			}

			ti->second->DeleteDeviceObjects();
			SAFE_DELETE( ti->second );
			m_mmapTexture.erase( ti++ );
		}
	}
}

VOID CTextUtil::Render( BOOL bPrev )
{
	if( !m_pd3dDevice )
		return;

	if( bPrev )
	{
		m_pSavedSB->Capture();
		m_pTextureSB->Apply();
	}
	else
	{
		m_pSavedSB->Apply();
	}
}

VOID CTextUtil::InvalidateDeviceObjects()
{
	if( !m_bUsage )
		return;

	ClearData();

	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pTextureSB );

	m_pd3dDevice = NULL;
}

VOID CTextUtil::DeleteDeviceObjects()
{
	if( !m_bUsage )
		return;

	ClearData();

	m_pd3dDevice = NULL;
}

VOID CTextUtil::ClearData()
{
	for( TEXTURE_ITOR ti = m_mmapTexture.begin(); ti != m_mmapTexture.end(); ++ti )
	{
		ti->second->DeleteDeviceObjects();
		SAFE_DELETE( ti->second );
	}
	m_mmapTexture.clear();

	for( GROUP_LIST::iterator gi = m_listGroup.begin(); gi != m_listGroup.end(); ++gi )
		SAFE_DELETE( *gi );
	m_listGroup.clear();
}

VOID CTextUtil::PushText( const TCHAR* strText, CD3DFontX* pFont )
{
	if( !m_pd3dDevice )
		return;

	CTextGroup* pGroup = GetTextGroup( pFont );
	if( !pGroup )
		pGroup = CreateTextGroup( pFont );

	if( !pGroup )
		return;

	CTextObject* pObject = pGroup->GetTextObject( strText );
	if( pObject )
		return;

	pObject = pGroup->CreateTextObject( strText );
	if( !pObject )
		return;

	INT iHeightScreen = pFont->GetHeightScreen();

	TEXTURE_RANGE mmapRange = m_mmapTexture.equal_range( iHeightScreen );
	for( TEXTURE_ITOR ci = mmapRange.first; ci != mmapRange.second; ++ci )
	{
		if( ci->second->MarkTextPart( pObject ) )
			return;
	}

	CTextTexture* pTexture = CreateTextTexture( iHeightScreen );
	if( !pTexture )
	{
		pGroup->DeleteTextObject( pObject );
		return;
	}

	if( pTexture->MarkTextPart( pObject ) )
		return;

	pGroup->DeleteTextObject( pObject );
}

BOOL CTextUtil::DrawText( const TCHAR* strText, CD3DFontX* pFont, D3DCOLOR dwColor, FLOAT fX, FLOAT fY )
{
	if( !m_pd3dDevice )
		return FALSE;

	CTextGroup* pGroup = GetTextGroup( pFont );
	if( pGroup && pGroup->DrawText( strText, dwColor, fX, fY ) )
		return TRUE;

	if( strText && strlen( strText ) > 0 )
		PushText( strText, pFont );

	return FALSE;
}

CTextGroup* CTextUtil::CreateTextGroup( CD3DFontX* pFont )
{
	if( !m_pd3dDevice )
		return NULL;

	CTextGroup* pGroup = new CTextGroup;
	pGroup->SetFont( pFont );
	m_listGroup.push_back( pGroup );

	return pGroup;
}

CTextGroup* CTextUtil::GetTextGroup( CD3DFontX* pFont )
{
	for( GROUP_LIST::iterator gi = m_listGroup.begin(); gi != m_listGroup.end(); ++gi )
	{
		if( ( *gi )->GetFont() == pFont )
			return *gi;
	}

	return NULL;
}

CTextTexture* CTextUtil::CreateTextTexture( INT iHeight )
{
	if( !m_pd3dDevice )
		return NULL;

	if( m_mmapTexture.size() > T_MAX_COUNT )
		return NULL;

	CTextTexture* pTexture = NULL;
	if( IsTextureBig( iHeight ) )
		pTexture = new CTextTexture32;
	else
		pTexture = new CTextTexture16;

	pTexture->SetHeight( iHeight );
	if( !pTexture->InitDeviceObjects( m_pd3dDevice ) )
	{
		SAFE_DELETE( pTexture );
		return NULL;
	}

	m_mmapTexture.insert( std::make_pair( iHeight, pTexture ) );

	return pTexture;
}

BOOL CTextUtil::IsTextureBig( INT iFontSize )
{
	if( iFontSize < T_SELECT )
		return FALSE;

	return TRUE;
}

INT CTextUtil::GetTextureSize( INT iFontSize )
{
	if( IsTextureBig( iFontSize ) )
		return m_iTextureSizeBig;

	return m_iTextureSizeSmall;
}

VOID CTextUtil::SaveTextureToFile( TCHAR* pszPath )
{
	INT iCnt = 0;
	for( TEXTURE_ITOR ti = m_mmapTexture.begin(); ti != m_mmapTexture.end(); ++ti, ++iCnt )
	{
		CString strPath;
		strPath.Format( _T( "%s_TextTexture_%02d.bmp" ), pszPath, iCnt );
		ti->second->SaveTextureToFile( strPath.GetString() );
	}
}

CTextUtil* CTextUtil::Get()
{
	if( m_cpTextUtil == NULL )
		m_cpTextUtil = new CTextUtil;

	return m_cpTextUtil;
}

VOID CTextUtil::CleanUp()
{
	SAFE_DELETE( m_cpTextUtil );
}
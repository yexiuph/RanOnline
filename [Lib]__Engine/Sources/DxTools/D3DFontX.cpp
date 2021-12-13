#include "pch.h"
#include "./D3DFont.h"
#include "./D3DFontX.h"
#include "../TextTexture/TextUtil.h"

#include "../[Lib]__MfcEx/Sources/GetWinVer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	const DWORD g_dwExtentFlags = DT_SINGLELINE | DT_EXPANDTABS | DT_CALCRECT;
	const DWORD g_dwDrawFlags = DT_SINGLELINE | DT_EXPANDTABS | DT_NOCLIP;

	const POINT g_ptOffset[ 2 ] =
	{
		{ 1, 0 }, { 1, 1 }
	};

	const DWORD g_colorFontDX[ 2 ] =
	{
		D3DCOLOR_ARGB( 255, 50, 50, 50 ), D3DCOLOR_ARGB( 255, 10, 10, 10 )
	};

	const COLORREF g_colorFontDC[ 3 ] =
	{
		RGB( 50, 50, 50 ), RGB( 10, 10, 10 ), RGB( 255, 255, 255 )
	};

	const TCHAR g_szNull[ 2 ] = _T( "i" );

	const LONG g_lfWeight = FW_NORMAL;
	const BYTE g_lfQuality = ANTIALIASED_QUALITY;
}

namespace
{
	// 성조
	const int g_nSungjo = 5;
	static const TCHAR g_szSungjo[ g_nSungjo ] =
	{
		(TCHAR)0xcc, (TCHAR)0xd2, (TCHAR)0xde, (TCHAR)0xec, (TCHAR)0xf2
	};

	// 베트남에 추가되어 있는 언어
	const int g_nExAlphabet = 17;
	static const TCHAR g_szExAlphabet[ g_nExAlphabet ] =
	{
		(TCHAR)0xc3, (TCHAR)0xe3, (TCHAR)0xc2, (TCHAR)0xe2, (TCHAR)0xca, (TCHAR)0xea, (TCHAR)0xd4, (TCHAR)0xf4,
		(TCHAR)0xd5, (TCHAR)0xf5, (TCHAR)0xdd, (TCHAR)0xfd, (TCHAR)0xd0, (TCHAR)0xf0, (TCHAR)0x91, (TCHAR)0x92,
        (TCHAR)0x85
	};

	/* A, a, E, e, I, i , O, o, U, u Y, y 베트남 모음들 */
	const int g_nMoum = 24;
	static const TCHAR g_szMoum[ g_nMoum ] =
	{
		(TCHAR)0x41, (TCHAR)0x61, (TCHAR)0xc3, (TCHAR)0xe3, (TCHAR)0xc2, (TCHAR)0xe2, (TCHAR)0x45, (TCHAR)0x65, (TCHAR)0xca, (TCHAR)0xea,
        (TCHAR)0x49, (TCHAR)0x69, (TCHAR)0x4f, (TCHAR)0x6f, (TCHAR)0xd4, (TCHAR)0xf4, (TCHAR)0xd5, (TCHAR)0xf5, (TCHAR)0x55, (TCHAR)0x75,
        (TCHAR)0xdd, (TCHAR)0xfd, (TCHAR)0x59, (TCHAR)0x79
	};
}

BOOL CD3DFontX::m_bWindows98 = TRUE;

CD3DFontX::CD3DFontX( TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags, LANGFLAG emLang )
	: m_pd3dDevice( NULL )
	, m_iHeightScreen( 0 )
	, m_iOutLine( 1 )

	, m_pd3dxFont( NULL )
	, m_hd3dxDC( NULL )

	, m_hDC( NULL )
	, m_hFont( NULL )
	, m_hFontOld( NULL )
{
	_tcscpy_s( m_strFontName, strFontName );

	m_dwFontHeight = dwHeight;
	m_dwFontFlags = dwFlags;
	m_emLangFlag = emLang;
}

CD3DFontX::~CD3DFontX()
{
}

HRESULT CD3DFontX::Create( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( FAILED( InitDeviceObjects( pd3dDevice ) ) )
		return E_FAIL;
	if( FAILED( RestoreDeviceObjects() ) )
		return E_FAIL;

	return S_OK;
}

HRESULT CD3DFontX::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	INT nWinVer;
	TCHAR szWinVer[ 50 ], szMajorMinorBuild[ 50 ];
	if( GetWinVer( szWinVer, &nWinVer, szMajorMinorBuild ) )
	{
		m_bWindows98 = ( nWinVer < WNTFIRST );
		if( m_bWindows98 )
            m_iOutLine = 0;
	}

	m_hDC = CreateCompatibleDC( NULL );
	SetMapMode( m_hDC, MM_TEXT );

	m_iHeightScreen = -( MulDiv( (INT)m_dwFontHeight, GetDeviceCaps( m_hDC, LOGPIXELSY ), 72 ) );

	LOGFONT logFont;
	SecureZeroMemory( &logFont, sizeof( LOGFONT ) );
	logFont.lfHeight = m_iHeightScreen;
	logFont.lfWeight = g_lfWeight;
	logFont.lfQuality = g_lfQuality;
	logFont.lfCharSet = nCharSet[ m_emLangFlag ];
	StringCchCopy( logFont.lfFaceName, LF_FACESIZE, m_strFontName );

	m_hFont = CreateFontIndirect( &logFont );

	m_hFontOld = (HFONT)SelectObject( m_hDC, m_hFont );

	return S_OK;
}

HRESULT CD3DFontX::RestoreDeviceObjects()
{
	D3DXFONT_DESC logFont;
	SecureZeroMemory( &logFont, sizeof( D3DXFONT_DESC ) );
	logFont.Height = m_iHeightScreen;
	logFont.Weight = g_lfWeight;
	logFont.Quality = g_lfQuality;
	logFont.CharSet = nCharSet[ m_emLangFlag ];
	logFont.MipLevels = 1;
	StringCchCopy( logFont.FaceName, LF_FACESIZE, m_strFontName );

	SAFE_RELEASE( m_pd3dxFont );
	HRESULT hr = D3DXCreateFontIndirect( m_pd3dDevice, &logFont, &m_pd3dxFont );
	if ( FAILED(hr) )
		return hr;

	m_hd3dxDC = m_pd3dxFont->GetDC();

	return S_OK;
}

HRESULT CD3DFontX::InvalidateDeviceObjects()
{
	SAFE_RELEASE( m_pd3dxFont );
	m_hd3dxDC = NULL;

	return S_OK;
}

HRESULT CD3DFontX::DeleteDeviceObjects()
{
	SAFE_RELEASE( m_pd3dxFont );
	m_hd3dxDC = NULL;

	SelectObject( m_hDC, m_hFontOld );
	DeleteObject( m_hFont );
	DeleteDC( m_hDC );

	return S_OK;
}

HRESULT CD3DFontX::CleanUp()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();

	return S_OK;
}

HRESULT CD3DFontX::DrawText( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags, BOOL bImmediately )
{
	if( bImmediately || CTextUtil::m_bUsage == FALSE )
		return DrawText( strText, dwColor, dwFlags, x, y );

	if( CTextUtil::Get()->DrawText( strText, this, dwColor, x, y ) == FALSE )
		return DrawText( strText, dwColor, dwFlags, x, y );

	return S_OK;
}

HRESULT CD3DFontX::GetTextExtent( const TCHAR* strText, SIZE &Size )
{
	if( !m_pd3dxFont || !m_hd3dxDC )
		return S_OK;

	BOOL bUsageCS = m_bUsageCS;
	if( bUsageCS )
		EnterCriticalSection( &m_csLock );

	INT iText = 1;
	BOOL bTextNull = TRUE;

	// 실제 글자는 없지만 세로 크기를 알고 싶을때 strText 가 공백이거나 NULL 이다.
	// LPD3DXFONT 에서 공백은 길이 계산에서 제외되므로 임시문자로 대체한다.
	if( strText )
	{
		iText = (INT)_tcslen( strText );
		if( iText )
		{
			bTextNull = FALSE;
		}
		else
		{
			strText = g_szNull;
			iText = 1;
		}
	}
	else
	{
		strText = g_szNull;
	}

	WCHAR * pwszTemp = new WCHAR[ iText+2 ];
	INT iWText = 0;
	Size.cx = Size.cy = 0;

	// 태국 98 에서는 GetTextExtentPoint 함수가 오동작하므로 따로 계산한다.
	if( m_bWindows98 )
		ConvWideAndTextExtent98( strText, iText, pwszTemp, iWText, Size, FALSE );
	else
		ConvWideAndTextExtent( strText, iText, pwszTemp, iWText, Size, FALSE );

	SAFE_DELETE_ARRAY( pwszTemp );

	if( bTextNull )
		Size.cx = 0;

	if( bUsageCS )
		LeaveCriticalSection( &m_csLock );

	return S_OK;
}

VOID CD3DFontX::ConvWideAndTextExtent( const TCHAR* strText, INT iText, WCHAR* wstrText, INT &iWText, SIZE &Size, BOOL bConv )
{
	// 로딩 스레드와 기본 스레드가 동시에 문자 길이 계산할 때 GetTextExtentPoint 함수가 실패한다.
	// 동시 사용할 경우 크리티컬 섹션을 활성화 시켜야 한다.

	memset( wstrText, 0, sizeof( WCHAR ) * ( iText + 2 ) );

	iWText = MultiByteToWideChar( nCodePage[ m_emLangFlag ], 0, strText, iText, wstrText, iText );

	if( iWText > 0 )
	{
		Size.cx = Size.cy = 0;
		GetTextExtentPoint32W( m_hd3dxDC, wstrText, iWText, &Size );

		if( Size.cx == 0 || Size.cy == 0 )
		{
			ConvWideAndTextExtent98( strText, iText, wstrText, iWText, Size, bConv );
		}
	}
}

VOID CD3DFontX::ConvWideAndTextExtent98( const TCHAR* strText, INT iText, WCHAR* wstrText, INT &iWText, SIZE &Size, BOOL bConv )
{
	// LPD3DXFONT::DrawText 함수에서는 뒤에 공백이 있을때 무시하고 길이 계산한다.
	// 결국 공백을 i로 바꿔서 시도한다. 실제 글자가 필요할때는 MultiByteToWideChar 다시 해줘야한다.

	CString szTemp = strText;
	INT iResult = szTemp.Replace( (char)32, (char)105 );

	memset( wstrText, 0, sizeof( WCHAR ) * ( iText + 2 ) );
	iWText = MultiByteToWideChar( nCodePage[ m_emLangFlag ], 0, szTemp.GetString(), iText, wstrText, iText );

	if( iWText > 0 )
	{
		RECT rtCalc = { 0, 0, 0, 0 };
		m_pd3dxFont->DrawTextW( NULL, wstrText, iWText, &rtCalc, g_dwExtentFlags, 0L );

		Size.cx = rtCalc.right - rtCalc.left;
		Size.cy = rtCalc.bottom - rtCalc.top;
	}

	if( bConv && iResult )
	{
		memset( wstrText, 0, sizeof( WCHAR ) * ( iText+2 ) );
		iWText = MultiByteToWideChar( nCodePage[ m_emLangFlag ], 0, strText, iText, wstrText, iText );
	}
}

INT CD3DFontX::GetHeightScreen()
{
	return (INT)( (INT)abs( m_iHeightScreen ) + CTextUtil::m_iSpaceSize );
}

HRESULT CD3DFontX::DrawText( const TCHAR* strText, DWORD dwColor, DWORD dwFlags, FLOAT x, FLOAT y )
{
	if( !m_pd3dxFont || !m_hd3dxDC || !strText )
		return S_OK;

	INT iText = (INT)_tcslen( strText );
	if( !iText )
		return S_OK;

	// 공백으로만 이루어져 있으면 출력할 필요 없다.
	CString szTemp = strText;
	if( szTemp.Replace( (char)32, (char)105 ) == iText )
		return S_OK;

	BOOL bUsageCS = m_bUsageCS;
	if( bUsageCS )
		EnterCriticalSection( &m_csLock );

	WCHAR * pwszTemp = new WCHAR[ iText+2 ];
	INT iWText = 0;
	SIZE Size = { 0, 0 };

	TCHAR strNewText[MAX_PATH] = {0};
	_tcscpy_s( strNewText, strText );

	if ( m_emLangFlag == VIETNAM )
	{
		CheckVietnam( strText, strNewText );
		iText = (INT)_tcslen( strNewText );
	}

	// 태국 98 에서는 GetTextExtentPoint 함수가 오동작하므로 따로 계산한다.
	if( m_bWindows98 )
		ConvWideAndTextExtent98( strNewText, iText, pwszTemp, iWText, Size, TRUE );
	else
		ConvWideAndTextExtent( strNewText, iText, pwszTemp, iWText, Size, TRUE );

	// 길이 계산에 실패하면 출력할 수 없다.
	if( iWText < 1 || Size.cx == 0 || Size.cy == 0 )
	{
		SAFE_DELETE_ARRAY( pwszTemp );

		if( bUsageCS )
			LeaveCriticalSection( &m_csLock );

		return S_OK;
	}

	Size.cx += (LONG)x;
	Size.cy += (LONG)y;
	RECT rectText = { (LONG)x, (LONG)y, (LONG)Size.cx, (LONG)Size.cy };

	if( DxFontMan::g_pSprite )
		DxFontMan::g_pSprite->Begin( D3DXSPRITE_ALPHABLEND );

	// 글자 테두리
	if( CTextUtil::m_bUsage && m_iOutLine )
	{
		if( m_dwFontFlags & D3DFONT_SHADOW )
		{
			for( LONG lX=-m_iOutLine; lX<=m_iOutLine; ++lX )
			{
				for( LONG lY=-m_iOutLine; lY<=m_iOutLine; ++lY )
				{
					if( !lX && !lY )
						continue;

					RECT rtOutline =
					{
						rectText.left + lX,
						rectText.top + lY,
						rectText.right + lX,
						rectText.bottom + lY
					};

					m_pd3dxFont->DrawTextW
					(
						DxFontMan::g_pSprite,
						pwszTemp,
						iWText,
						&rtOutline,
						g_dwDrawFlags,
						g_colorFontDX[ 1 ]
					);
				}
			}
		}
		else if( m_dwFontFlags & D3DFONT_SHADOW_EX )
		{
			for( INT i=0; i<2; ++i )
			{
				RECT rtOutline =
				{
					rectText.left + g_ptOffset[ i ].x,
					rectText.top + g_ptOffset[ i ].y,
					rectText.right + g_ptOffset[ i ].x,
					rectText.bottom + g_ptOffset[ i ].y
				};

				m_pd3dxFont->DrawTextW
				(
					DxFontMan::g_pSprite,
					pwszTemp,
					iWText,
					&rtOutline,
					g_dwDrawFlags,
					g_colorFontDX[ 1 ]
				);
			}
		}
	}

	// 실제 글자
	m_pd3dxFont->DrawTextW
	(
		DxFontMan::g_pSprite,
		pwszTemp,
		iWText,
		&rectText,
		g_dwDrawFlags,
		dwColor
	);

	if( DxFontMan::g_pSprite )
		DxFontMan::g_pSprite->End();

	SAFE_DELETE_ARRAY( pwszTemp );

	if( bUsageCS )
		LeaveCriticalSection( &m_csLock );

	return S_OK;
}

VOID CD3DFontX::PushText( const TCHAR* strText )
{
	if( CTextUtil::m_bUsage )
		CTextUtil::Get()->PushText( strText, this );
}

VOID CD3DFontX::DrawText( const TCHAR* strText, DWORD dwFlags, RECT* rtBound, HDC hDC )
{
	if( !m_hFont || !hDC )
		return;

	if( !strText || !rtBound )
		return;

	INT iText = (INT)_tcslen( strText );
	if( !iText )
		return;

	// 잘못된 크기가 넘어오면 출력할 수 없다.
	if( ( rtBound->right - rtBound->left <= 0 ) || ( rtBound->bottom - rtBound->top <= 0 ) )
		return;

	// 공백으로만 이루어져 있으면 출력할 필요 없다.
	CString szTemp = strText;
	if( szTemp.Replace( (char)32, (char)105 ) == iText )
		return;

	BOOL bUsageCS = m_bUsageCS;
	if( bUsageCS )
		EnterCriticalSection( &m_csLock );

	WCHAR * pwszTemp = new WCHAR[ iText + 2 ];
	memset( pwszTemp, 0, sizeof( WCHAR ) * ( iText + 2 ) );
	INT iWText = MultiByteToWideChar( nCodePage[ m_emLangFlag ], 0, strText, iText, pwszTemp, iText );

	// 길이 계산에 실패하면 출력할 수 없다.
	if( iWText < 1 )
	{
		SAFE_DELETE_ARRAY( pwszTemp );

		if( bUsageCS )
			LeaveCriticalSection( &m_csLock );

		return;
	}

	HFONT hFontOld = (HFONT)SelectObject( hDC, m_hFont );

	// 글자 테두리
	if( m_iOutLine && ( dwFlags & EM_DC_OUTLINE ) )
	{
		SetTextColor( hDC, g_colorFontDC[ 1 ] );

		if( m_dwFontFlags & D3DFONT_SHADOW )
		{
			for( LONG lX=-m_iOutLine; lX<=m_iOutLine; ++lX )
			{
				for( LONG lY=-m_iOutLine; lY<=m_iOutLine; ++lY )
				{
					if( !lX && !lY )
						continue;

					POINT ptTemp =
					{
						rtBound->left + lX,
						rtBound->top + lY
					};

					ExtTextOutW
					(
						hDC,
						ptTemp.x,
						ptTemp.y,
						ETO_OPAQUE,
						NULL,
						pwszTemp,
						iWText,
						NULL
					);
				}
			}
		}
		else if( m_dwFontFlags & D3DFONT_SHADOW_EX )
		{
			for( INT i=0; i<2; ++i )
			{
				POINT ptTemp =
				{
					rtBound->left + g_ptOffset[ i ].x,
					rtBound->top + g_ptOffset[ i ].y
				};

				ExtTextOutW
				(
					hDC,
					ptTemp.x,
					ptTemp.y,
					ETO_OPAQUE,
					NULL,
					pwszTemp,
					iWText,
					NULL
				);
			}
		}
	}

	// 실제 글자
	if( dwFlags & EM_DC_TEXT )
	{
		SetTextColor( hDC, g_colorFontDC[ 2 ] );
		ExtTextOutW
		(
			hDC,
			rtBound->left,
			rtBound->top,
			ETO_OPAQUE,
			NULL,
			pwszTemp,
			iWText,
			NULL
		);
	}

	SelectObject( hDC, hFontOld );

	SAFE_DELETE_ARRAY( pwszTemp );

	if( bUsageCS )
		LeaveCriticalSection( &m_csLock );
}

VOID CD3DFontX::CheckVietnam( const TCHAR* strText, TCHAR* strNewText )
{
	INT nCurrent = 0;
	TCHAR chBefore = '\0';

	INT size = _tcslen( strText );
	if( size < 1 )
		return;

	for ( INT i = 0; i < size; ++i )
	{
		if ( CheckCharSunjo( strText[ i ] ) && !CheckCharMoum( chBefore ) )
		{
//			CDebugSet::ToLogFile ( "sungjo Error %d %c %c %02x strText : %s\n", i, strText[i], chBefore, chBefore, strText );
			continue;
		}

		chBefore = strText[ i ];
		strNewText[ nCurrent ] = strText[ i ];
		++nCurrent;
	}
	strNewText[ nCurrent ] = _T('\0');

// 지정된 베트남어가 아닐경우 글자를 없애는 기능인데 일단 제거 //
/*
	size = _tcslen( strNewText );

	for ( INT i = 0; i < size; ++i )
	{
		if ( strText[ i ] < 0 || strText[ i ] > 127 ) // 일반 아스키 코드가 아닐때
		{
			if ( !CheckCharExAlphabet( strText[ i ] ) && !CheckCharSunjo( strText[ i ] ) ) // 자음,모음이 아니고 성조가 아닐때
			{
				strNewText[ i ] = '*';
				CDebugSet::ToLogFile ( "Check Error %d, %d %02x %c strText : %s\n", i,strText[i], strText[i], strText[i], strText );
				CDebugSet::ToLogFile ( "strNewText : %s\n", strNewText );
			}
		}
	}
*/
}

BOOL CD3DFontX::CheckCharSunjo( TCHAR chText )
{
	for ( int i = 0; i < g_nSungjo; ++i )
	{
		if ( chText == g_szSungjo[ i ] )
			return true;
	}

	return false;
}

BOOL CD3DFontX::CheckCharExAlphabet( TCHAR chText )
{
	for ( int i = 0; i < g_nExAlphabet; ++i )
	{
		if ( chText == g_szExAlphabet[ i ] )
			return true;
	}

	return false;
}

BOOL CD3DFontX::CheckCharMoum( TCHAR chText )
{
	for ( int i = 0; i < g_nMoum; ++i )
	{
		if ( chText == g_szMoum[ i ] )
			return true;
	}

	return false;
}
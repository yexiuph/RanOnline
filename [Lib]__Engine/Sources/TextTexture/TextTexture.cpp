#include "pch.h"
#include "./TextTexture.h"
#include "./TextUtil.h"
#include "./TextObject.h"
#include "./TextNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTextTexture::CTextTexture()
	: m_pd3dDevice( NULL )
	, m_iHeight( 0 )
	, m_iTextureSize( 0 )

	, m_pTexture( NULL )
	, m_pTexData( NULL )
	, m_dwTexSize( 0 )

	, m_hDC( NULL )
	, m_hBitmap( NULL )
	, m_hBitmapOld( NULL )
	, m_pBits( NULL )

	, m_iLine( 0 )
	, m_pWidth( NULL )
	, m_pWidthTemp( NULL )
	, m_iTopWidth( 0 )
	, m_pListPart( NULL )

	, m_bDelete( FALSE )
	, m_bCreate( FALSE )
	, m_bCopy( FALSE )
{
}

CTextTexture::~CTextTexture()
{
	SAFE_DELETE_ARRAY( m_pWidth );
	SAFE_DELETE_ARRAY( m_pWidthTemp );

	for( INT i=0; i<m_iLine; ++i )
		m_pListPart[i].clear();
	SAFE_DELETE_ARRAY( m_pListPart );

	m_iLine = 0;
	m_iTopWidth = 0;
}

BOOL CTextTexture::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_pTexData )
		return FALSE;

	m_pWidth = new INT[ m_iLine ];
	m_pWidthTemp = new INT[ m_iLine ];
	m_pListPart = new PART_LIST[ m_iLine ];

	for( INT i=0; i<m_iLine; ++i )
	{
		m_pWidth[ i ] = m_iTextureSize;
		m_pWidthTemp[ i ] = m_iTextureSize;
		m_pListPart[ i ].clear();
	}
	m_iTopWidth = m_iTextureSize;

	BITMAPINFO bmi;
	SecureZeroMemory( &bmi.bmiHeader, sizeof(BITMAPINFOHEADER) );
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_iTextureSize;
	bmi.bmiHeader.biHeight = -m_iTextureSize;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biXPelsPerMeter = 100;
	bmi.bmiHeader.biYPelsPerMeter = 100;

	m_hDC = CreateCompatibleDC( NULL );
	m_hBitmap = CreateDIBSection( m_hDC, &bmi, DIB_RGB_COLORS, (VOID**)&m_pBits, NULL, 0 );
	m_hBitmapOld = (HBITMAP)SelectObject( m_hDC, m_hBitmap );
	SetMapMode( m_hDC, MM_TEXT );
	SetBkMode( m_hDC, TRANSPARENT );

	RECT rt = { 0, 0, m_iTextureSize, m_iTextureSize };
	HBRUSH hBrush = CreateSolidBrush( 0x00000000 );
	FillRect( m_hDC, &rt, hBrush ); 
	DeleteObject( hBrush );

	return TRUE;
}

VOID CTextTexture::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
}

VOID CTextTexture::FrameMove()
{
	if( !m_pd3dDevice )
		return;

	if( m_bDelete )
		CalcDelete();

	if( m_bCreate )
		CalcCreate();

	if( m_bCopy )
		CopyTexture();
}

VOID CTextTexture::InvalidateDeviceObjects()
{
	m_pd3dDevice = NULL;
}

VOID CTextTexture::DeleteDeviceObjects()
{
	m_pd3dDevice = NULL;

	SAFE_RELEASE( m_pTexture );
	m_pTexData = NULL;
	m_dwTexSize = 0;

	SelectObject( m_hDC, m_hBitmapOld );
	DeleteObject( m_hBitmap );
	DeleteDC( m_hDC );

	SAFE_DELETE_ARRAY( m_pWidth );
	SAFE_DELETE_ARRAY( m_pWidthTemp );

	for( INT i=0; i<m_iLine; ++i )
		m_pListPart[ i ].clear();
	SAFE_DELETE_ARRAY( m_pListPart );

	m_iLine = 0;
	m_iTopWidth = 0;

	m_bDelete = FALSE;
	m_bCreate = FALSE;
	m_bCopy = FALSE;
}

BOOL CTextTexture::MarkTextPart( CTextObject* pObject )
{
	if( !m_iLine )
		return FALSE;

	BOOL bSucceed = TRUE;
	CheckTopLength( bSucceed, pObject->GetTextPart() );
	if( !bSucceed )
		return FALSE;

	memcpy( m_pWidthTemp, m_pWidth, sizeof( INT ) * m_iLine );

	bSucceed = TRUE;
	MarkLength( bSucceed, pObject->GetTextPart() );

	if( !bSucceed )
	{
		memcpy( m_pWidth, m_pWidthTemp, sizeof( INT ) * m_iLine );

		for( INT i=0; i<m_iLine; ++i )
		{
			PART_LIST::iterator pi = m_pListPart[ i ].begin();
			while( pi != m_pListPart[ i ].end() )
			{
				if( ( *pi )->m_bTemp )
				{
					( *pi )->m_bTemp = FALSE;
					m_pListPart[ i ].erase( pi++ );
				}
				else
				{
					++pi;
				}
			}
		}

		return FALSE;
	}

	m_bCreate = TRUE;
	pObject->SetTexture( this );

	for( INT i=0; i<m_iLine; ++i )
	{
		for( PART_LIST::iterator pi = m_pListPart[ i ].begin(); pi != m_pListPart[ i ].end(); ++pi )
			( *pi )->m_bTemp = FALSE;
	}

	return TRUE;
}

VOID CTextTexture::CheckTopLength( BOOL& bSucceed, CTextPart* pPart )
{
	if( !bSucceed || !pPart )
		return;

	if( pPart->m_bValid )
	{
		if( m_iTopWidth < pPart->m_iWidth )
			bSucceed = FALSE;
		return;
	}

	CheckTopLength( bSucceed, pPart->m_pLeft );
	CheckTopLength( bSucceed, pPart->m_pRight );
}

VOID CTextTexture::MarkLength( BOOL& bSucceed, CTextPart* pPart )
{
	if( !bSucceed || !pPart )
		return;

	if( pPart->m_bValid )
	{
		for( INT i=0; i<m_iLine; ++i )
		{
			if( m_pWidth[ i ] < pPart->m_iWidth )
				continue;

			pPart->m_bTemp = TRUE;

			pPart->m_rtTBound.left = m_iTextureSize - m_pWidth[ i ];
			pPart->m_rtTBound.top = i * m_iHeight;

			m_pWidth[ i ] -= pPart->m_iWidth;

			pPart->m_rtTBound.right = m_iTextureSize - m_pWidth[ i ];
			pPart->m_rtTBound.bottom = pPart->m_rtTBound.top + m_iHeight;

			m_pListPart[ i ].push_back( pPart );
			return;
		}

		bSucceed = FALSE;
		return;
	}

	MarkLength( bSucceed, pPart->m_pLeft );
	MarkLength( bSucceed, pPart->m_pRight );
}

BOOL CTextTexture::IsEmpty()
{
	if( !m_pTexture )
		return TRUE;

	BOOL bEmpty = TRUE;

	for( INT i=0; i<m_iLine; ++i )
	{
		if( !m_pListPart[ i ].empty() )
		{
			bEmpty = FALSE;
			break;
		}
	}

	return bEmpty;
}

VOID CTextTexture::CalcCreate()
{
	m_iTopWidth = 0;
	for( INT i=0; i<m_iLine; ++i )
	{
		m_iTopWidth = max( m_iTopWidth, m_pWidth[ i ] );

		for( PART_LIST::iterator pi = m_pListPart[ i ].begin(); pi != m_pListPart[ i ].end(); ++pi )
		{
			if( ( *pi )->m_bCreate )
				CalcTexData( *pi );
		}
	}

	m_bCreate = FALSE;
	m_bCopy = TRUE;
}

VOID CTextTexture::CopyTexture()
{
	D3DLOCKED_RECT rect;
	if( SUCCEEDED( m_pTexture->LockRect( 0, &rect, NULL, 0 ) ) )
	{
		memcpy( rect.pBits, m_pTexData, m_dwTexSize );
		if( FAILED( m_pTexture->UnlockRect( 0 ) ) )
			return;
	}

	m_bCopy = FALSE;
}

VOID CTextTexture::SetTexture()
{
	if( m_pd3dDevice )
		m_pd3dDevice->SetTexture( 0, m_pTexture );
}

VOID CTextTexture::SaveTextureToFile( const TCHAR* pszPath, D3DXIMAGE_FILEFORMAT eType )
{
	if( !m_pTexture )
		return;

	BOOL bSave = FALSE;
	for( INT i=0; i<m_iLine; ++i )
	{
		if( !m_pListPart[ i ].empty() )
		{
			bSave = TRUE;
			break;
		}
	}

	if( bSave )
		D3DXSaveTextureToFile( pszPath, eType, m_pTexture, NULL );
}
#include "pch.h"
#include "./TextNode.h"
#include "./TextUtil.h"
#include "./D3DFontX.h"
#include "../[Lib]__EngineUI/Sources/UIRenderQueue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTextPart::CTextPart()
	: m_bValid( FALSE )
	, m_iLength( 0 )
	, m_strText( NULL )
	, m_iWidth( 0 )

	, m_bTemp( FALSE )
	, m_bDelete( FALSE )
	, m_bCreate( TRUE )

	, m_pLeft( NULL )
	, m_pRight( NULL )

	, m_pFont( NULL )
{
	memset( &m_rtTBound, 0, sizeof( RECT ) );
}

CTextPart::~CTextPart()
{
	SAFE_DELETE_ARRAY( m_strText );

	SAFE_DELETE( m_pLeft );
	SAFE_DELETE( m_pRight );
}

VOID CTextPart::CreateText( const TCHAR* strText, CD3DFontX* pFont )
{
	INT iHeightScreen = pFont->GetHeightScreen();

	m_pFont = pFont;

	SIZE sizeText = { 0, 0 };
	pFont->GetTextExtent( strText, sizeText );

	m_iLength = (INT)strlen( strText );
	m_iWidth = sizeText.cx + CTextUtil::m_iSpaceSize;

	INT iTextureSize = CTextUtil::Get()->GetTextureSize( iHeightScreen );
	if( m_iWidth < iTextureSize )
	{
		m_bValid = TRUE;
		m_strText = new TCHAR[ m_iLength + 1 ];
		memcpy( m_strText, strText, sizeof( TCHAR ) * m_iLength );
		m_strText[ m_iLength ] = 0;
		return;
	}

	WCHAR wszText[ 256 ];
	INT iWText = pFont->MultiToWide( strText, m_iLength, wszText, 254 );

	INT iLeft = iWText / 2;
	INT iRight = iWText - iLeft;

	TCHAR tszText[ 128 ];
	INT iTText = pFont->WideToMulti( &wszText[0], iLeft, tszText, 127 );
	tszText[ iTText ] = 0;
	m_pLeft = new CTextPart;
	m_pLeft->CreateText( tszText, pFont );

	iTText = pFont->WideToMulti( &wszText[ iLeft ], iRight, tszText, 127 );
	tszText[ iTText ] = 0;
	m_pRight = new CTextPart;
	m_pRight->CreateText( tszText, pFont );
}

VOID CTextPart::DrawText( HDC hDC, DWORD dwFlags )
{
	HBRUSH hBrush = CreateSolidBrush( 0x00000000 );
	FillRect( hDC, &m_rtTBound, hBrush ); 
	DeleteObject( hBrush );

	RECT rtRealBound;
	memcpy( &rtRealBound, &m_rtTBound, sizeof( RECT ) );
	rtRealBound.left += CTextUtil::m_iSpaceSizeHalf;
	rtRealBound.top += CTextUtil::m_iSpaceSizeHalf;
	rtRealBound.right += CTextUtil::m_iSpaceSizeHalf;
	rtRealBound.bottom += CTextUtil::m_iSpaceSizeHalf;

	if( dwFlags & CD3DFontX::EM_DC_TEXT )
	{
		m_bCreate = FALSE;

		m_pFont->DrawText( m_strText, dwFlags, &rtRealBound, hDC );

		SetTextureUV();
	}
	else
	{
		m_pFont->DrawText( m_strText, dwFlags, &rtRealBound, hDC );
	}
}

VOID CTextPart::DeleteText()
{
	m_bDelete = TRUE;

	if( m_pLeft )
		m_pLeft->DeleteText();

	if( m_pRight )
		m_pRight->DeleteText();
}

VOID CTextPart::SetTextureUV()
{
	FLOAT fTextureSize = (FLOAT)CTextUtil::Get()->GetTextureSize( m_pFont->GetHeightScreen() );

	m_stVertex[ 1 ].tu1 = (FLOAT)m_rtTBound.left / fTextureSize;
	m_stVertex[ 1 ].tv1 = (FLOAT)m_rtTBound.top / fTextureSize;

	m_stVertex[ 3 ].tu1 = (FLOAT)m_rtTBound.right / fTextureSize;
	m_stVertex[ 3 ].tv1 = (FLOAT)m_rtTBound.bottom / fTextureSize;

	m_stVertex[ 0 ].tu1 = m_stVertex[ 1 ].tu1;
	m_stVertex[ 0 ].tv1 = m_stVertex[ 3 ].tv1;

	m_stVertex[ 2 ].tu1 = m_stVertex[ 3 ].tu1;
	m_stVertex[ 2 ].tv1 = m_stVertex[ 1 ].tv1;
}

VOID CTextPart::DrawText( LPDIRECT3DTEXTUREQ pTexture, D3DCOLOR dwColor, FLOAT& fX, FLOAT fY )
{
	if( m_bValid )
	{
		fX -= (FLOAT)CTextUtil::m_iSpaceSize;

		static D3DXVECTOR2 vMin, vMax;
		vMin.x = fX;
		vMin.y = fY;
		vMax.x = vMin.x + (FLOAT)m_iWidth;
		vMax.y = vMin.y + (FLOAT)m_pFont->GetHeightScreen();

		m_stVertex[ 0 ].x = m_stVertex[ 1 ].x = ( vMin.x - 0.5f );
		m_stVertex[ 3 ].x = m_stVertex[ 2 ].x = ( vMax.x - 0.5f );
		m_stVertex[ 1 ].y = m_stVertex[ 2 ].y = ( vMin.y - 0.5f );
		m_stVertex[ 0 ].y = m_stVertex[ 3 ].y = ( vMax.y - 0.5f );

		for( INT i=0; i<4; ++i )
			m_stVertex[ i ].diffuse = dwColor;

		fX += (FLOAT)m_iWidth;

		CUIRenderQueue::Get()->Render( pTexture, m_stVertex );
	}

	if( m_pLeft )
		m_pLeft->DrawText( pTexture, dwColor, fX, fY );

	if( m_pRight )
		m_pRight->DrawText( pTexture, dwColor, fX, fY );
}

VOID CTextPart::DrawText( D3DCOLOR dwColor, FLOAT& fX, FLOAT fY )
{
	if( m_bValid )
	{
		fX -= (FLOAT)CTextUtil::m_iSpaceSize;

		static DWORD dwSize = sizeof( UIVERTEX );
		static D3DXVECTOR2 vMin, vMax;
		vMin.x = fX;
		vMin.y = fY;
		vMax.x = vMin.x + (FLOAT)m_iWidth;
		vMax.y = vMin.y + (FLOAT)m_pFont->GetHeightScreen();

		m_stVertex[ 0 ].x = m_stVertex[ 1 ].x = ( vMin.x - 0.5f );
		m_stVertex[ 3 ].x = m_stVertex[ 2 ].x = ( vMax.x - 0.5f );
		m_stVertex[ 1 ].y = m_stVertex[ 2 ].y = ( vMin.y - 0.5f );
		m_stVertex[ 0 ].y = m_stVertex[ 3 ].y = ( vMax.y - 0.5f );

		for( INT i=0; i<4; ++i )
			m_stVertex[ i ].diffuse = dwColor;

		fX += (FLOAT)m_iWidth;

		LPDIRECT3DDEVICEQ pd3dDevice = CTextUtil::Get()->GetDevice();
		if( pd3dDevice )
			pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_stVertex, dwSize );
	}

	if( m_pLeft )
		m_pLeft->DrawText( dwColor, fX, fY );

	if( m_pRight )
		m_pRight->DrawText( dwColor, fX, fY );
}

DWORD CTextPart::GetFontFlags()
{
	if( m_pFont )
		return m_pFont->GetFontFlags();

	return 0;
}
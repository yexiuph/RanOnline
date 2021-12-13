#include "pch.h"
#include "./TextTexture32.h"
#include "./TextUtil.h"
#include "./TextNode.h"

#include "./D3DFont.h"
#include "./D3DFontX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTextTexture32::CTextTexture32()
	: m_pTexData32( NULL )
{
}

CTextTexture32::~CTextTexture32()
{
	SAFE_DELETE_ARRAY( m_pTexData32 );
}

BOOL CTextTexture32::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	m_iTextureSize = CTextUtil::Get()->GetTextureSize( m_iHeight );

	m_iLine = m_iTextureSize / m_iHeight;
	if( !m_iLine )
		return FALSE;

	HRESULT hr = D3DXCreateTexture
	(
		m_pd3dDevice,
		m_iTextureSize,
		m_iTextureSize,
		1,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		&m_pTexture
	);
	if( FAILED( hr ) )
	{
		m_iLine = 0;
		return FALSE;
	}

	m_pTexData32 = new DWORD[ m_iTextureSize * m_iTextureSize ];
	m_pTexData = (VOID*)m_pTexData32;
	m_dwTexSize = sizeof( DWORD ) * m_iTextureSize * m_iTextureSize;
	memset( m_pTexData32, 0, m_dwTexSize );

	return CTextTexture::InitDeviceObjects( pd3dDevice );
}

VOID CTextTexture32::DeleteDeviceObjects()
{
	CTextTexture::DeleteDeviceObjects();

	SAFE_DELETE_ARRAY( m_pTexData32 );
}

VOID CTextTexture32::CalcDelete()
{
	CTextPart* pPart;

	for( INT i=0; i<m_iLine; ++i )
	{
		INT iDeleteWidth = 0;

		PART_LIST::iterator pi = m_pListPart[ i ].begin();
		while( pi != m_pListPart[ i ].end() )
		{
			pPart = *pi;

			if( pPart->m_bDelete )
			{
				iDeleteWidth += pPart->m_iWidth;
				m_pWidth[ i ] += pPart->m_iWidth;
				m_pListPart[ i ].erase( pi++ );
				continue;
			}

			++pi;

			if( !iDeleteWidth )
				continue;

			pPart->m_rtTBound.left -= iDeleteWidth;
			pPart->m_rtTBound.right -= iDeleteWidth;

			if( pPart->m_bCreate )
				continue;

			pPart->SetTextureUV();

			for( LONG y=pPart->m_rtTBound.top; y<pPart->m_rtTBound.bottom; ++y )
			{
				memmove
				(
					&m_pTexData32[ pPart->m_rtTBound.left + y * m_iTextureSize ],
					&m_pTexData32[ pPart->m_rtTBound.left + iDeleteWidth + y * m_iTextureSize ],
					sizeof( DWORD ) * pPart->m_iWidth
				);
			}
		}

		m_iTopWidth = max( m_iTopWidth, m_pWidth[ i ] );
	}

	m_bDelete = FALSE;
	m_bCopy = TRUE;
}

VOID CTextTexture32::CalcTexData( CTextPart* pPart )
{
	BOOL bBlur = FALSE;
	if( CTextUtil::m_bBlurFilter )
		bBlur = pPart->GetFontFlags() & D3DFONT_BLUR;

	if( bBlur )
		pPart->DrawText( m_hDC, CD3DFontX::EM_DC_EFF );
	else
		pPart->DrawText( m_hDC, CD3DFontX::EM_DC_ALL );

	LONG lCur;
	DWORD* pSrc;
	DWORD* pDest;

	for( LONG y=pPart->m_rtTBound.top; y<pPart->m_rtTBound.bottom; ++y )
	{
		lCur = pPart->m_rtTBound.left + y * m_iTextureSize;

		pSrc = &m_pBits[ lCur ];
		pDest = &m_pTexData32[ lCur ];

		for( LONG x=pPart->m_rtTBound.left; x<pPart->m_rtTBound.right; ++x, ++pSrc, ++pDest )
		{
			if( *pSrc == 0x00000000 )
				*pDest = 0x00000000;
			else
				*pDest = *pSrc | 0xFF000000;
		}
	}

	if( !bBlur )
		return;

	FilterTexture( pPart );

	pPart->DrawText( m_hDC, CD3DFontX::EM_DC_TEXT );

	for( LONG y=pPart->m_rtTBound.top; y<pPart->m_rtTBound.bottom; ++y )
	{
		lCur = pPart->m_rtTBound.left + y * m_iTextureSize;

		pSrc = &m_pBits[ lCur ];
		pDest = &m_pTexData32[ lCur ];

		for( LONG x=pPart->m_rtTBound.left; x<pPart->m_rtTBound.right; ++x, ++pSrc, ++pDest )
		{
			if( *pSrc != 0x00000000 )
				*pDest = *pSrc | 0xFF000000;
		}
	}
}

VOID CTextTexture32::FilterTexture( CTextPart* pPart )
{
	LONG lTextureSize = (LONG)m_iTextureSize;

	LONG lCur;
	D3DXCOLOR dwSrc;

	FLOAT fMixValue;
	FLOAT fAccuValue;
	D3DXCOLOR dwDest;

	for( LONG y=pPart->m_rtTBound.top; y<pPart->m_rtTBound.bottom; ++y )
	{
		for( LONG x=pPart->m_rtTBound.left; x<pPart->m_rtTBound.right; ++x )
		{
			lCur = x + y * lTextureSize;

			fMixValue = 0.01f;
			fAccuValue = 1.0f;
			dwDest = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );

			for( LONG oy=y-2; oy<=y+2; ++oy )
			{
				for( LONG ox=x-2; ox<=x+2; ++ox )
				{
					if( pPart->m_rtTBound.left <= ox &&
						pPart->m_rtTBound.right > ox &&
						pPart->m_rtTBound.top <= oy &&
						pPart->m_rtTBound.bottom > oy )
					{
						if( x != ox || y != oy )
						{
							dwSrc = D3DXCOLOR( m_pTexData32[ ox + oy * lTextureSize ] );
							dwSrc *= fMixValue;
							dwDest += dwSrc;
						}
					}
					else
					{
						fAccuValue += fMixValue;
					}

					fMixValue += 0.05f;
					if( fMixValue > 0.1f )
						fMixValue = 0.01f;
				}
			}

			dwSrc = D3DXCOLOR( m_pTexData32[ lCur ] );
			dwSrc *= fAccuValue;
			dwSrc += dwDest;
			m_pTexData32[ lCur ] = dwSrc;
		}
	}
}
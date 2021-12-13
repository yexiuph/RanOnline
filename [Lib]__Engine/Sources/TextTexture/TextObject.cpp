#include "pch.h"
#include "./TextObject.h"
#include "./TextNode.h"
#include "./TextTexture.h"
#include "./TextUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	// 오래된 데이터 삭제할 타이밍
	//
	const DWORD ELAPS_FRAME = 10000;
}

CTextObject::CTextObject()
	: m_iLength( 0 )
	, m_strText( NULL )

	, m_pPart( NULL )
	, m_pTexture( NULL )

	, m_dwLastDraw( 0 )
	, m_bDelete( FALSE )
{
}

CTextObject::~CTextObject()
{
	SAFE_DELETE_ARRAY( m_strText );
	SAFE_DELETE( m_pPart );
}

VOID CTextObject::CreateTextPart( const TCHAR* strText, CD3DFontX* pFont )
{
	m_strText = new TCHAR[ m_iLength + 1 ];
	memcpy( m_strText, strText, sizeof( TCHAR ) * m_iLength );
	m_strText[ m_iLength ] = 0;

	m_pPart = new CTextPart;
	m_pPart->CreateText( strText, pFont );

	m_dwLastDraw = CTextUtil::Get()->GetLastTick();
}

VOID CTextObject::FrameMove()
{
	if( CTextUtil::Get()->GetLastTick() - m_dwLastDraw < ELAPS_FRAME )
		return;

	m_pPart->DeleteText();

	m_pTexture->SetDelete();

	m_bDelete = TRUE;
}

VOID CTextObject::DrawText( D3DCOLOR dwColor, FLOAT fX, FLOAT fY )
{
	if( CTextUtil::m_bUsageRenderQueue == FALSE )
		m_pTexture->SetTexture();

	fX += (FLOAT)CTextUtil::m_iSpaceSizeHalf;
	fY -= (FLOAT)CTextUtil::m_iSpaceSizeHalf;

	FLOAT fLeft = fX;

	if( CTextUtil::m_bUsageRenderQueue )
		m_pPart->DrawText( m_pTexture->GetTexture(), dwColor, fLeft, fY );
	else
		m_pPart->DrawText( dwColor, fLeft, fY );

	m_dwLastDraw = CTextUtil::Get()->GetLastTick();
}
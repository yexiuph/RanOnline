#include "pch.h"
#include "./TextGroup.h"
#include "./TextNode.h"
#include "./TextObject.h"
#include "./TextUtil.h"

#include "../[Lib]__EngineUI/Sources/UIDataType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTextGroup::CTextGroup()
	: m_pFont( NULL )
{
	m_listObject.clear();
}

CTextGroup::~CTextGroup()
{
	for( OBJECT_LIST::iterator oi = m_listObject.begin(); oi != m_listObject.end(); ++oi )
		SAFE_DELETE( *oi );
	m_listObject.clear();
}

VOID CTextGroup::FrameMovePrev()
{
	for( OBJECT_LIST::iterator oi = m_listObject.begin(); oi != m_listObject.end(); ++oi )
		( *oi )->FrameMove();
}

VOID CTextGroup::FrameMoveNext()
{
	OBJECT_LIST::iterator oi = m_listObject.begin();
	while( oi != m_listObject.end() )
	{
		if( ( *oi )->GetDelete() )
		{
			SAFE_DELETE( *oi );
			m_listObject.erase( oi++ );
		}
		else
		{
			++oi;
		}
	}
}

BOOL CTextGroup::DrawText( const TCHAR* strText, D3DCOLOR dwColor, FLOAT fX, FLOAT fY )
{
	CTextObject* pObject = GetTextObject( strText );
	if( pObject )
	{
		LPDIRECT3DDEVICEQ pd3dDevice = CTextUtil::Get()->GetDevice();
		if( pd3dDevice )
		{
			// 소수점 있을 경우, 간혹 UV 계산에 오차가 생긴다
			//
			fX = floor( fX );
			fY = floor( fY );

			DWORD ALPHABLENDENABLE, SRCBLEND, DESTBLEND;
			pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &ALPHABLENDENABLE );
			pd3dDevice->GetRenderState( D3DRS_SRCBLEND, &SRCBLEND );
			pd3dDevice->GetRenderState( D3DRS_DESTBLEND, &DESTBLEND );

			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			if( CTextUtil::m_bUsageRenderQueue == FALSE )
				pd3dDevice->SetFVF( UIVERTEX::D3DFVF_UIVERTEX );
			pObject->DrawText( dwColor, fX, fY );

			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, ALPHABLENDENABLE );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, SRCBLEND );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, DESTBLEND );
		}
		return TRUE;
	}

	return FALSE;
}

CTextObject* CTextGroup::CreateTextObject( const TCHAR* strText )
{
	if( !strText )
		return NULL;

	INT iLength = (INT)strlen( strText );
	if( !iLength )
		return NULL;

	CTextObject* pObject = new CTextObject;
	pObject->SetLength( iLength );
	pObject->CreateTextPart( strText, m_pFont );
	m_listObject.push_back( pObject );

	return pObject;
}

CTextObject* CTextGroup::GetTextObject( const TCHAR* strText )
{
	if( !strText )
		return NULL;

	INT iLength = (INT)strlen( strText );
	if( !iLength )
		return NULL;

	for( OBJECT_LIST::iterator oi = m_listObject.begin(); oi != m_listObject.end(); ++oi )
	{
		if( ( *oi )->GetLength() != iLength )
			continue;
		if( strcmp( ( *oi )->GetText(), strText ) )
			continue;
		return *oi;
	}

	return NULL;
}

VOID CTextGroup::DeleteTextObject( CTextObject* pObject )
{
	for( OBJECT_LIST::iterator oi = m_listObject.begin(); oi != m_listObject.end(); ++oi )
	{
		if( *oi == pObject )
		{
			SAFE_DELETE( *oi );
			m_listObject.erase( oi );
			return;
		}
	}
}
#include "pch.h"
#include "CommonWeb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------
// CommonWeb
//--------------------------------------------------------------------
CCommonWeb* CCommonWeb::m_cpCommonWeb = NULL;

CCommonWeb::CCommonWeb()
	: m_pWnd( NULL )
	, m_pVisible( NULL )
	, m_pBound( NULL )
{
	for( INT i=0; i<TOTAL_ID; ++i )
	{
		m_bCreate[i] = m_bVisible[i] = FALSE;
	}
}

CCommonWeb::~CCommonWeb()
{
}

VOID CCommonWeb::Create( CWnd* pParent, BOOL* pVisible, RECT* pBound )
{
	if( !pParent || !pVisible || !pBound )
		return;

	RECT rtBound = { 0, 0, 5, 5 };

	for( INT i=0; i<TOTAL_ID; ++i )
	{
		if( m_bCreate[i] )
			continue;

		m_bCreate[i] = m_cWebBrowser[i].Create( NULL, NULL, rtBound, pParent, (INT)AFX_IDW_PANE_FIRST + i );
	}
	
	m_pWnd = pParent;
	m_pVisible = pVisible;
	m_pBound = pBound;
}

VOID CCommonWeb::Navigate( INT iID, const TCHAR* pszPath, BOOL bShow )
{
	if( iID < 0 || iID >= TOTAL_ID )	return;
	if( !m_bCreate[ iID ] )				return;

	m_cWebBrowser[ iID ].Navigate( pszPath, NULL, NULL, NULL, NULL );

	if( bShow )
		m_cWebBrowser[ iID ].ShowWindow( SW_SHOW );
}

VOID CCommonWeb::Move( INT iID, INT iLeft, INT iTop, INT iRight, INT iBottom, BOOL bReDraw, BOOL bCalRect )
{
	if( iID < 0 || iID >= TOTAL_ID )	return;
	if( !m_bCreate[ iID ] )				return;

	m_cWebBrowser[ iID ].MoveWindow( iLeft, iTop, iRight, iBottom, bReDraw );
	if( bCalRect )
	{
		m_cWebBrowser[ iID ].GetWindowRect( m_pBound );
		m_pWnd->ScreenToClient( m_pBound );
	}
}

BOOL CCommonWeb::GetCreate( INT iID )
{
	if( iID < 0 || iID >= TOTAL_ID )
	{
		for( INT i=0; i<TOTAL_ID; ++i )
		{
			if( m_bCreate[i] )
				return TRUE;
		}

		return FALSE;
	}

	return m_bCreate[ iID ];
}

VOID CCommonWeb::SetVisible( INT iID, BOOL bVisible )
{
	if( iID < 0 || iID >= TOTAL_ID )
		return;

	m_bVisible[iID] = bVisible;

	if( m_pVisible )
		*m_pVisible = bVisible;	

	::SetFocus( NULL );
}

VOID CCommonWeb::SetVisible( BOOL bVisible )
{
	for( INT i=0; i<TOTAL_ID; ++i )
	{
		m_bVisible[i] = bVisible;
	}

	if( m_pVisible )
		*m_pVisible = bVisible;

	::SetFocus( NULL );
}

BOOL CCommonWeb::GetVisible( INT iID )
{
	if( iID < 0 || iID >= TOTAL_ID )
		return FALSE;

	BOOL bRet(FALSE);

	if( m_pVisible )
		bRet = *m_pVisible;

	return m_bVisible[iID] && bRet;
}

CCommonWeb* CCommonWeb::Get()
{
	if( m_cpCommonWeb == NULL )
		m_cpCommonWeb = new CCommonWeb;

	return m_cpCommonWeb;
}

VOID CCommonWeb::CleanUp()
{
	SAFE_DELETE( m_cpCommonWeb );
}

void CCommonWeb::Refresh( INT iID )
{
	if( iID < 0 || iID >= TOTAL_ID )
		return;

	m_cWebBrowser[ iID ].Refresh();
}

bool CCommonWeb::IsCompleteLoad( INT iID )
{
	if( iID < 0 || iID >= TOTAL_ID )
		return false;
   
	return m_cWebBrowser[iID].IsCompleteLoad();
}

void CCommonWeb::ReSetCompleteLoad( INT iID )
{
	if( iID < 0 || iID >= TOTAL_ID )
		return;
   
	m_cWebBrowser[iID].SetCompleteLoad(false);
}

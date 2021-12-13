// BmpButton.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "BmpButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBmpButton

IMPLEMENT_DYNAMIC(CBmpButton, CButton)
CBmpButton::CBmpButton()
{
	m_MouseState = eBUTTON_NORMAL;
	m_bMouseTracking = FALSE;
}

CBmpButton::~CBmpButton()
{
}


BEGIN_MESSAGE_MAP(CBmpButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CBmpButton 메시지 처리기입니다.

void CBmpButton::SetBitmap (UINT nIDResourceUp,UINT nIDResourceDn)
{	
    if(m_bmpUp.m_hObject)
        m_bmpUp.DeleteObject();
    m_bmpUp.LoadBitmap(nIDResourceUp);	

	m_UpMemDC.CreateCompatibleDC ( GetDC() );
	m_pUpOldBitmap = (CBitmap *) m_UpMemDC.SelectObject(&m_bmpUp);


	if(m_bmpDn.m_hObject)
		m_bmpDn.DeleteObject();
	m_bmpDn.LoadBitmap(nIDResourceDn);	

	m_DnMemDC.CreateCompatibleDC ( GetDC() );
	m_pDnOldBitmap = (CBitmap *) m_DnMemDC.SelectObject(&m_bmpDn);
}

void CBmpButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  지정된 항목을 그리는 코드를 추가합니다.   
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

	// If drawing selected, add the pushed style to DrawFrameControl.
	if ( m_MouseState == eBUTTON_HOVER )
//	if ( lpDrawItemStruct->itemState & ODS_SELECTED )   
	{
		GetDC()->BitBlt ( lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top,
			lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left,
			lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top,
		&m_DnMemDC, 0, 0, SRCCOPY );       
	}
	//else if ( m_MouseState == eBUTTON_GRAYED )
	//{
	//	GetDC()->BitBlt ( lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top,
	//		lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left,
	//		lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top,
	//	&m_GrayMemDC, 0, 0, SRCCOPY );
	//}
	else
	{
		GetDC()->BitBlt ( lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top,
			lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left,
			lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top,
			&m_UpMemDC, 0, 0, SRCCOPY );		
	}
}

BOOL CBmpButton::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_UpMemDC.SelectObject(m_pUpOldBitmap);
	m_DnMemDC.SelectObject(m_pDnOldBitmap);	

	return CButton::DestroyWindow();
}

BOOL CBmpButton::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ( pMsg->message == WM_LBUTTONDBLCLK )
	{		
		pMsg->message = WM_LBUTTONDOWN;
	}

	return CButton::PreTranslateMessage(pMsg);
}

void CBmpButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.	
	CButton::OnMouseMove(nFlags, point);

	//if ( m_MouseState == eBUTTON_GRAYED )
	//{
	//	return;
	//}

	if ( ( !m_bMouseTracking || GetCapture() != this ) && ( m_MouseState == eBUTTON_NORMAL ) )
	{
		OnMouseEnter ();
	}
	else
	{
		if ( m_MouseState == eBUTTON_HOVER )
		{
			CRect rc;
			GetClientRect ( &rc );
			if ( !rc.PtInRect ( point ) )
			{
				OnMouseLeave ();
			}
		}
	}
}

void CBmpButton::OnMouseEnter ( void )
{
	m_bMouseTracking = TRUE;
	m_MouseState = eBUTTON_HOVER;

	SetCapture ();
	Invalidate ();
}

void CBmpButton::OnMouseLeave ( void )
{
	m_bMouseTracking = FALSE;
	m_MouseState = eBUTTON_NORMAL;

	Invalidate ();
	ReleaseCapture();
}

void CBmpButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_MouseState = eBUTTON_NORMAL;

	CButton::OnLButtonUp(nFlags, point);
}

void CBmpButton::SetMouseState ( int nState )
{
	m_MouseState = nState;
	Invalidate ();
}
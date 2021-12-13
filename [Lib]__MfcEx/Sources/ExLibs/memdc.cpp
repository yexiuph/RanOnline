#include "pch.h"
#include "memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CmyMemDC::CmyMemDC ( CDC* pDC, const CRect& rcBounds ) : CDC()
{
	CreateCompatibleDC(pDC);
	m_bitmap = new CBitmap;

	m_bitmap->CreateCompatibleBitmap ( pDC, rcBounds.Width(), rcBounds.Height() );
	m_oldBitmap = SelectObject ( m_bitmap );
	m_pDC = pDC;
	m_rcBounds = rcBounds;

	m_pOldFont = NULL;
}

CmyMemDC::~CmyMemDC() 
{
	ReleaseMemDC ();
}

void CmyMemDC::SetBkFill ( COLORREF BkColor )
{
	FillSolidRect ( m_rcBounds, BkColor );
}

CmyMemDC* CmyMemDC::operator->()
{
	return this;
}

void CmyMemDC::SetFont ( CFont *pFont )
{
	m_pOldFont = SelectObject ( pFont );
}

void CmyMemDC::ReleaseMemDC ()
{
	m_pDC->BitBlt ( m_rcBounds.left, m_rcBounds.top,
		m_rcBounds.Width(), m_rcBounds.Height(),
		this, m_rcBounds.left, m_rcBounds.top, SRCCOPY );

	SelectObject ( m_oldBitmap );

	m_bitmap->DeleteObject ();
	if ( m_bitmap )		delete m_bitmap;

	if ( m_pOldFont )	SelectObject ( m_pOldFont );
}

// FlatEdit.cpp : implementation file
//

#include "pch.h"
#include "FlatEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CWinApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CFlatEdit

CFlatEdit::CFlatEdit()
{
	
	m_clrHilite = GetSysColor(COLOR_BTNHIGHLIGHT);
	m_clrShadow = GetSysColor(COLOR_BTNSHADOW);
	m_clrDkShad = GetSysColor(COLOR_3DDKSHADOW);
	m_clrButton = GetSysColor(COLOR_BTNFACE);
	m_bDrawGripper = TRUE;
	m_bDrawSeperator = TRUE;
	m_bFlatStyle = TRUE;

}

CFlatEdit::~CFlatEdit()
{
}


BEGIN_MESSAGE_MAP(CFlatEdit, CEdit)
	//{{AFX_MSG_MAP(CFlatEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlatEdit message handlers
void CFlatEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	SetTimer(1, 10, NULL);
	CEdit::OnMouseMove(nFlags, point);
}

void CFlatEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = true;
	CEdit::OnLButtonDown(nFlags, point);
}

void CFlatEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = false;
	CEdit::OnLButtonUp(nFlags, point);
}

void CFlatEdit::OnTimer(UINT nIDEvent) 
{
	POINT pt;
	GetCursorPos(&pt);
	CRect rcItem;
	GetWindowRect(&rcItem);

	static bool bPainted = false;

	if(!rcItem.PtInRect(pt)) {
		KillTimer(1);
		DrawEdit(FE_MOUSEOUT, m_clrButton, m_clrButton);
		return;
	}
	else {
		DrawEdit(FE_MOUSEOVER, m_clrShadow, m_clrHilite);
	}



	CEdit::OnTimer(nIDEvent);
}

void CFlatEdit::OnPaint() 
{
	Default();
	DrawEdit(FE_MOUSEOUT, m_clrButton, m_clrButton);

}



void CFlatEdit::OnSysColorChange() 
{
	CEdit::OnSysColorChange();
	m_clrHilite = GetSysColor(COLOR_BTNHIGHLIGHT);
	m_clrShadow = GetSysColor(COLOR_BTNSHADOW);
	m_clrDkShad = GetSysColor(COLOR_3DDKSHADOW);
	m_clrButton = GetSysColor(COLOR_BTNFACE);
	
}

void CFlatEdit::DrawEdit(int style, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	CRect rcItem;
	CRect rcGripper;
	CRect rcSeperator;

	GetClientRect(&rcItem);
	GetClientRect(&rcGripper);
	GetClientRect(&rcSeperator);

	CDC* pDC = GetDC();
	COLORREF clrBlack;
	clrBlack = RGB(0, 0, 0);

	if(m_bFlatStyle)
	{
		pDC->Draw3dRect(rcItem, clrTopLeft, clrBottomRight);
		rcItem.InflateRect(1,1);
		
		if (!IsWindowEnabled()) {
			pDC->Draw3dRect(rcItem, m_clrHilite, m_clrHilite);
		}
		
		else {
			pDC->Draw3dRect(rcItem, m_clrButton, m_clrButton);
		}

		rcItem.InflateRect(1,1);
		pDC->Draw3dRect(rcItem, m_clrButton, m_clrButton);
		rcItem.InflateRect(1,1);
		pDC->Draw3dRect(rcItem, m_clrButton, m_clrButton);
		rcItem.InflateRect(1,1);
		pDC->Draw3dRect(rcItem, m_clrButton, m_clrButton);
	
		rcItem.DeflateRect(4, 4);

		if(m_bDrawGripper)
		{
			rcGripper.top = rcItem.top;
			rcGripper.bottom = rcItem.bottom;
			rcGripper.left -= 4;
			rcGripper.right = rcGripper.left +2;
			pDC->Draw3dRect(rcGripper, m_clrHilite, m_clrShadow);
			rcGripper.left -= 3;
			rcGripper.right = rcGripper.left +2;
			pDC->Draw3dRect(rcGripper, m_clrHilite, m_clrShadow);
		}
		if(m_bDrawSeperator)
		{
			rcSeperator.top = rcItem.top;
			rcSeperator.bottom = rcItem.bottom;
			rcSeperator.left = rcItem.right + 2;
			rcSeperator.right = rcSeperator.left + 2;
			pDC->Draw3dRect(rcSeperator, m_clrShadow, m_clrHilite);
		}
	
	
		if (!IsWindowEnabled()) {
			return;
		}

	
	
		if(style == FE_MOUSEOVER)
		{
			pDC->Draw3dRect(rcItem, m_clrButton, m_clrButton); //added this
			rcItem.top -= 1; //added this
			rcItem.left -= 1;
			rcItem.bottom += 1;
			rcItem.right += 1; //added this
			pDC->Draw3dRect(rcItem, m_clrShadow, m_clrHilite); //was hilite, shadow
		}
	}
	else {

		pDC->Draw3dRect(rcItem, clrBlack, clrBlack);
		rcItem.InflateRect(1, 1);
		pDC->Draw3dRect(rcItem, clrBlack, clrBlack);
		pDC->Draw3dRect(rcItem, clrBlack, clrBlack);
		rcItem.InflateRect(1,1);

		pDC->Draw3dRect(rcItem, m_clrButton, m_clrButton);
		rcItem.InflateRect(1,1);
		pDC->Draw3dRect(rcItem, m_clrButton, m_clrButton);
		rcItem.DeflateRect(4, 4);
		
		if(m_bDrawGripper)
		{
			rcGripper.top = rcItem.top - 2;
			rcGripper.bottom = rcItem.bottom + 2;
			rcGripper.left -= 6;
			rcGripper.right = rcGripper.left + 2;
			pDC->Draw3dRect(rcGripper, clrBlack, clrBlack);
			rcGripper.left -= 3;
			rcGripper.right -= 3;
			pDC->Draw3dRect(rcGripper, clrBlack, clrBlack);
		}

		if(m_bDrawSeperator)
		{
			rcSeperator.top = rcItem.top - 2;
			rcSeperator.bottom = rcItem.bottom + 2;
			rcSeperator.left = rcItem.right + 4;
			rcSeperator.right = rcSeperator.left + 2;
			pDC->Draw3dRect(rcSeperator, clrBlack, clrBlack);
		}
	}

	ReleaseDC(pDC);
}

void CFlatEdit::SetIndent(int margin)
{
	SetMargins(margin, 1);
}

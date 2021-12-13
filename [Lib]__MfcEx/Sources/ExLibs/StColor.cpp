// StColor.cpp : implementation file
//

#include "pch.h"
#include "StColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CStColor

CStColor::CStColor()
{
}

CStColor::~CStColor()
{
}


BEGIN_MESSAGE_MAP(CStColor, CStatic)
	//{{AFX_MSG_MAP(CStColor)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStColor message handlers

void CStColor::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect WndRect;
	GetClientRect ( WndRect );
	DWORD dwWidth = WndRect.Width ();
	DWORD dwHeight = WndRect.Height ();


	float ThisR = m_d3dStartColor.r * 255.0f;
	float ThisG = m_d3dStartColor.g * 255.0f;
	float ThisB = m_d3dStartColor.b * 255.0f;

	float DeltaR = ( m_d3dEndColor.r - m_d3dStartColor.r ) * 255.0f / dwWidth;
	float DeltaG = ( m_d3dEndColor.g - m_d3dStartColor.g ) * 255.0f / dwWidth;
	float DeltaB = ( m_d3dEndColor.b - m_d3dStartColor.b ) * 255.0f / dwWidth;

	for ( DWORD i=0; i<dwWidth; ++i )
	{
		ThisR += DeltaR;
		ThisG += DeltaG;
		ThisB += DeltaB;

		COLORREF Color = RGB ( int(ThisR), int(ThisG), int(ThisB) );

		for ( DWORD j=0; j<dwHeight; ++j )
		{
			dc.SetPixel ( i, j, Color );
		}
	}
	
	// Do not call CStatic::OnPaint() for painting messages
}

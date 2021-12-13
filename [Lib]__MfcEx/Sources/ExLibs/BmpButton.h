#pragma once


// CBmpButton

class CBmpButton : public CButton
{
	DECLARE_DYNAMIC(CBmpButton)

public:
	CBmpButton();
	virtual ~CBmpButton();

protected:
	CBitmap		m_bmpDn;
	CDC			m_DnMemDC;
	CBitmap*	m_pDnOldBitmap;

	CBitmap		m_bmpUp;
	CDC			m_UpMemDC;
	CBitmap*	m_pUpOldBitmap;

//	CBitmap		m_bmpGray;
//	CDC			m_GrayMemDC;
//	CBitmap*	m_pGrayOldBitmap;

protected:
	BOOL	m_bMouseTracking;
	int		m_MouseState;

public:
	enum
	{
//		eBUTTON_GRAYED,
		eBUTTON_HOVER,
		eBUTTON_NORMAL
	};	

protected:
	void	OnMouseLeave ( void );
	void	OnMouseEnter ( void );

public:
	void    SetBitmap ( UINT nIDResourceUp,UINT nIDResourceDn );
	void	SetMouseState ( int nState );

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual BOOL DestroyWindow();	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};



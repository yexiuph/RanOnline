#if !defined(AFX_FLATEDIT_H__1D80B920_5AC8_11D2_B1E0_525400DAACF0__INCLUDED_)
#define AFX_FLATEDIT_H__1D80B920_5AC8_11D2_B1E0_525400DAACF0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FlatEdit.h : header file
//
#define FE_MOUSEOVER 1
#define FE_MOUSEOUT 2

/////////////////////////////////////////////////////////////////////////////
// CFlatEdit window

class CFlatEdit : public CEdit
{
// Construction
public:
	CFlatEdit();
	void DrawEdit(int style, COLORREF clrTopLeft, COLORREF clrBottomRight);
	bool m_bDrawGripper;
	bool m_bDrawSeperator;
	void SetIndent(int margin);
	bool m_bFlatStyle;		//We got 2 flat styles, true is to make it 'Microsoft Flat'
						// false is to make it look like the heavy border around check boxes that are flat
	

// Attributes
public:
	bool m_bLBtnDown;
	COLORREF m_clrHilite;
	COLORREF m_clrShadow;
	COLORREF m_clrDkShad;
	COLORREF m_clrButton;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlatEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFlatEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFlatEdit)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLATEDIT_H__1D80B920_5AC8_11D2_B1E0_525400DAACF0__INCLUDED_)

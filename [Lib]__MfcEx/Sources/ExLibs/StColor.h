#if !defined(AFX_STCOLOR_H__4277AE33_A72C_4795_8E9A_C1A014D37B09__INCLUDED_)
#define AFX_STCOLOR_H__4277AE33_A72C_4795_8E9A_C1A014D37B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StColor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStColor window

class CStColor : public CStatic
{
public:
	D3DCOLORVALUE	m_d3dStartColor;
	D3DCOLORVALUE	m_d3dEndColor;

// Construction
public:
	CStColor();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStColor)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStColor();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStColor)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STCOLOR_H__4277AE33_A72C_4795_8E9A_C1A014D37B09__INCLUDED_)

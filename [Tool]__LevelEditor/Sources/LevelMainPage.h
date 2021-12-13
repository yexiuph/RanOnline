#pragma once
#include "afxwin.h"

class CsheetWithTab;

extern DWORD dwDIV[10];
// CLevelMainPage dialog

class CLevelMainPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLevelMainPage)

public:
	CLevelMainPage();
	virtual ~CLevelMainPage();

// Dialog Data
	enum { IDD = IDD_LEVEL_MAIN_PAGE };

public:
	CsheetWithTab*		m_pSheetTab;
	BOOL				m_bINIT;

public:
	void	SetSheetTab ( CsheetWithTab* SheetTab ) { m_pSheetTab = SheetTab; }
	void	UpdatePage ();

protected:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_comboDiv;
	afx_msg void OnBnClickedButtonWldfile();
	afx_msg void OnCbnSelchangeComboDiv();
	afx_msg void OnBnClickedButtonCreateQtree();
	afx_msg void OnBnClickedCheckRenderQtree();
	afx_msg void OnBnClickedCheckShowCrowschdl();
};

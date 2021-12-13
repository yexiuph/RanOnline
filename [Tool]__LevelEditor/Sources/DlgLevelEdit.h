#pragma once
#include "afxwin.h"

class	CsheetWithTab;
// CDlgLevelEdit dialog

class CDlgLevelEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgLevelEdit)

public:
	CDlgLevelEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLevelEdit();

// Dialog Data
	enum { IDD = IDD_LEVEL_EDIT_DIALOG };

public:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bINIT;

public:
	void UpdatePage ( BOOL bMainPage );

protected:
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_comboPage;
	afx_msg void OnCbnSelchangeComboPage();
};

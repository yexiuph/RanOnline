#pragma once
#include "afxwin.h"


// CGateListPage dialog
class CsheetWithTab;

class CGateListPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGateListPage)

public:
	CGateListPage();
	virtual ~CGateListPage();

// Dialog Data
	enum { IDD = IDD_GATE_LIST_PAGE };

public:
	BOOL			m_bDlgInit;
	BOOL			m_bGateIDInit;

	int				m_CalledPage;

	CsheetWithTab*	m_pSheetTab;
	DWORD			m_dwGateID;

public:
	void	SetSheetTab ( CsheetWithTab* SheetTab ) { m_pSheetTab = SheetTab; }
	void	SetPropertyData ( int nCallPage, DWORD dwGateID );
	BOOL	UpdateTreeItem();

protected:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_listboxGate;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonGateOk();
	afx_msg void OnBnClickedButtonGateCancel();
};

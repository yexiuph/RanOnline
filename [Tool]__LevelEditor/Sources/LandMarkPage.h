#pragma once
#include "afxwin.h"


// CLandMarkPage dialog
class CsheetWithTab;

struct GLLANDMARK;
typedef GLLANDMARK* PLANDMARK;

class CLandMarkPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLandMarkPage)

public:
	CLandMarkPage();
	virtual ~CLandMarkPage();

// Dialog Data
	enum { IDD = IDD_LAND_MARK_PAGE };

public:
	BOOL			m_bDlgInit;

	CsheetWithTab*	m_pSheetTab;
	PLANDMARK		m_pLandMark;

public:
	void	SetSheetTab ( CsheetWithTab* SheetTab ) { m_pSheetTab = SheetTab; }
	void	ListtingLandMark ();

	void	UpdateContrl ();
	void	ViewContrls ( BOOL bView, BOOL bClear );

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_listLandMark;
	afx_msg void OnBnClickedButtonLandMarkNew();
	afx_msg void OnBnClickedButtonLandMarkEdit();
	afx_msg void OnBnClickedButtonLandMarkDel();
	afx_msg void OnBnClickedButtonLandMarkRegis();
};

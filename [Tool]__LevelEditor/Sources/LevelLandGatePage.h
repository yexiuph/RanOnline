#pragma once
#include "afxwin.h"

#include "DxLandGateMan.h"

// CLevelLandGatePage dialog
class CsheetWithTab;
class DxLandGate;

class CLevelLandGatePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLevelLandGatePage)

public:
	CLevelLandGatePage();
	virtual ~CLevelLandGatePage();

// Dialog Data
	enum { IDD = IDD_LAND_GATE_PAGE };

public:
	BOOL				m_bDlgInit;

	CsheetWithTab*		m_pSheetTab;
	PDXLANDGATE			m_pLandGate;

public:
	void	SetSheetTab ( CsheetWithTab* SheetTab ) { m_pSheetTab = SheetTab; }
	void	UpdateContrl();

private:
	void	ViewContrls ( BOOL bShow, BOOL bClear );
	void	ListtingLandGate();

protected:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_listLandGate;
	afx_msg void OnBnClickedButtonLandGateNew();
	afx_msg void OnBnClickedButtonLandGateEdit();
	afx_msg void OnBnClickedButtonLandGateDel();
	afx_msg void OnBnClickedButtonLandGateCopy();
	afx_msg void OnBnClickedCheckLandGateRend();
	afx_msg void OnBnClickedButtonLandGateApply();
	afx_msg void OnBnClickedButtonLandGateRegis();
};

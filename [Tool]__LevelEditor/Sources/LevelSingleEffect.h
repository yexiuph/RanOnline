#pragma once
#include "afxwin.h"


// CLevelSingleEffect dialog
class	CsheetWithTab;

struct DXLANDEFF;
typedef DXLANDEFF* PLANDEFF;

class CLevelSingleEffect : public CPropertyPage
{
	DECLARE_DYNAMIC(CLevelSingleEffect)

public:
	CLevelSingleEffect();
	virtual ~CLevelSingleEffect();

// Dialog Data
	enum { IDD = IDD_SINGLE_EFFECT_PAGE };

public:
	BOOL			m_bDlgInit;

	CsheetWithTab*	m_pSheetTab;
	PLANDEFF		m_pLandEff;

public:
	void	SetSheetTab ( CsheetWithTab* SheetTab ) { m_pSheetTab = SheetTab; }
	void	ListtingLandEff ();

	void	UpdateContrl ();
	void	ViewContrls ( BOOL bView, BOOL bClear );

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_EffListBox;
	afx_msg void OnBnClickedButtonSingleEffNew();
	afx_msg void OnBnClickedButtonSingleEffEdit();
	afx_msg void OnBnClickedButtonSingleEffDel();
	afx_msg void OnBnClickedButtonSingleEffFind();
	afx_msg void OnBnClickedButtonSingleEffApply();
	afx_msg void OnBnClickedButtonSingleEffRegis();
};

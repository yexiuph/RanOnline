#pragma once

#include "GLDefine.h"
#include "afxwin.h"

// CRequirePage dialog
class CsheetWithTab;

class CRequirePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CRequirePage)

public:
	CRequirePage();
	virtual ~CRequirePage();

// Dialog Data
	enum { IDD = IDD_REQUIRE_PAGE };

public:
	BOOL			m_bInit;

	CsheetWithTab*	m_pSheetTab;

public:
	void	SetSheetTab ( CsheetWithTab* SheetTab ) { m_pSheetTab = SheetTab; }
	void	SetReqItem ( SNATIVEID NativeID );
	void	SetReqSkill ( SNATIVEID NativeID );

	void	UpdatePage ();

private:
	void	InversePage();

protected:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCbnSelchangeComboPenetLevelSign();
	afx_msg void OnBnClickedButtonPenetItem();
	afx_msg void OnBnClickedButtonPenetItemDel();
	afx_msg void OnBnClickedButtonPenetSkill();
	afx_msg void OnBnClickedButtonPenetSkillDel();
	afx_msg void OnBnClickedButtonPenetCompQuest();
	afx_msg void OnBnClickedButtonPenetCompQuestDel();
	afx_msg void OnBnClickedButtonPenetProgQuest();
	afx_msg void OnBnClickedButtonPenetProgQuestDel();
	afx_msg void OnBnClickedButtonPenetSave();
};

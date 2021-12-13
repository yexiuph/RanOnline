#pragma once

#include "GLDefine.h"
#include "afxcmn.h"

// CSkillTreePage dialog
class CsheetWithTab;

class CSkillTreePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSkillTreePage)

public:
	CSkillTreePage();
	virtual ~CSkillTreePage();

// Dialog Data
	enum { IDD = IDD_SKILL_TREE_PAGE };

public:
	BOOL			m_bDlgInit;
	BOOL			m_bNativeIDInit;

	int				m_CalledPage;

	CsheetWithTab*	m_pSheetTab;
	SNATIVEID		m_NativeID;

public:
	void	SetSheetTab ( CsheetWithTab* SheetTab ) { m_pSheetTab = SheetTab; }
	void	SetPropertyData ( int nCallPage, SNATIVEID NativeID );
	BOOL	UpdateTreeItem();

protected:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CTreeCtrl m_ctrlItemTree;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonSkillOk();
	afx_msg void OnBnClickedButtonSkillCancel();
};

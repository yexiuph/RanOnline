#pragma once
#include "afxcmn.h"
#include "NpcDialogueSet.h"

// CNpcActionTable ��ȭ �����Դϴ�.
class	CsheetWithTab;

class CNpcActionTable : public CPropertyPage
{
	DECLARE_DYNAMIC(CNpcActionTable)

public:
	CNpcActionTable( LOGFONT logfont );
	virtual ~CNpcActionTable();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_NPCACTIONTABLE };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	BOOL			m_bDlgInit;
	CFont*			m_pFont;

	CNpcDialogueSet	m_DialogueSet;

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }

	CNpcDialogueSet*	GetDialogueSet ();

public:
	void	UpdateItems ();
	void	SetReadMode();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlActionTable;
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedCsvimportButton();
	afx_msg void OnNMDblclkListAction(NMHDR *pNMHDR, LRESULT *pResult);
};

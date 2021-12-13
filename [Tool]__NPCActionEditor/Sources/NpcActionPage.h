#pragma once
#include "afxcmn.h"
#include "NpcDialogue.h"


// CNpcActionPage 대화 상자입니다.
class	CsheetWithTab;
class	CNpcDialogue;
class	CNpcDialogueSet;

class CNpcActionPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CNpcActionPage)

public:
	CNpcActionPage( LOGFONT logfont );
	virtual ~CNpcActionPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCACTIONPAGE };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

    CNpcDialogue	m_DummyDialogue;
	CNpcDialogue*	m_pDialogue;

	CNpcDialogueSet*	m_pNpcDialogueSet;
	BOOL				m_bDlgInit;
	CFont*				m_pFont;

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	SetDialogue ( CNpcDialogueSet *pNpcDialogueSet, CNpcDialogue* pDialogue );

public:
	void	UpdateItems ();
	void	UpdateCase ();

	void	SetReadMode();

private:
	void	InverseUpdateItems ();
	void	InitDefaultCtrls ();

private:
	std::string GetBoolString ( BOOL bValue );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:	
	CListCtrl m_listctrlCase;
	CListCtrl m_listctrlAnswer;

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonCaseEdit();
	afx_msg void OnBnClickedButtonCaseDel();
	afx_msg void OnBnClickedButtonCaseNew();
	afx_msg void OnLvnItemchangedListPostiveAnswer2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnNMDblclkListPostiveAnswer2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCaseCopy();
};

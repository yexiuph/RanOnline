#pragma once
#include "afxcmn.h"
#include "NpcDialogue.h"
#include "NpcDialogueCase.h"
#include "NpcTalkControl.h"

// CNpcActionAnswer ��ȭ �����Դϴ�.
class	CsheetWithTab;
class	CNpcDialogueSet;

class CNpcActionAnswer : public CPropertyPage
{
	DECLARE_DYNAMIC(CNpcActionAnswer)

public:
	CNpcActionAnswer( LOGFONT logfont );
	virtual ~CNpcActionAnswer();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_NPCACTIONANSWER };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	CNpcTalkControl		m_DummyTalkControl;
	CNpcDialogueCase	m_DummyDialogueCase;

	CNpcDialogueSet*	m_pNpcDialogueSet;
	CNpcDialogueCase*	m_pOrgDialogueCase;
	BOOL				m_bDlgInit;
	CFont*				m_pFont;

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	SetDialogueCase ( CNpcDialogueSet *pNpcDialogueSet, CNpcDialogueCase* pDialogueCase );

public:
	void	UpdateItems ();

	void	SetReadMode ();

private:	
	void	InitDefaultCtrls ();
	void	InverseUpdateItems ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	CListCtrl m_ctrlAnswer;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonDelall();
	afx_msg void OnBnClickedButtonConditonEdit();
	afx_msg void OnBnClickedButtonConditonDel();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnNMDblclkListAnswer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCopy();
};

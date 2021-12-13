#pragma once

#include "NpcDialogueSet.h"

// CDlgNpcTalk ��ȭ �����Դϴ�.

class CDlgNpcTalk : public CDialog
{
	DECLARE_DYNAMIC(CDlgNpcTalk)

public:

	DWORD m_dwNpcID;
	CNpcDialogueSet*	m_pNpcDialogueSet;

public:
	CDlgNpcTalk(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	CDlgNpcTalk(CNpcDialogueSet* pNpcDialogueSet, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgNpcTalk();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLGNPCTALK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	CListBox	m_listNpcTalk;
	virtual BOOL OnInitDialog();
	void	SetDialogueSet( CNpcDialogueSet* pNpcDialogueSet ) { m_pNpcDialogueSet = pNpcDialogueSet; }

protected:
	virtual void OnOK();
	virtual void OnCancel();
};

#pragma once
#include "afxwin.h"


// CDlgQuest ��ȭ �����Դϴ�.

class CDlgQuest : public CDialog
{
	DECLARE_DYNAMIC(CDlgQuest)

public:
	DWORD	m_dwQuestID;

public:
	CDlgQuest(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgQuest();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_QUEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listQuest;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFind();
};

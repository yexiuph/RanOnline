#pragma once


// CDlgQuest dialog

class CDlgQuest : public CDialog
{
	DECLARE_DYNAMIC(CDlgQuest)

public:
	CDlgQuest(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgQuest();

// Dialog Data
	enum { IDD = IDD_QUEST_DIALOG };

public:
	DWORD		m_dwQuestID;

protected:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_listQuest;
	afx_msg void OnBnClickedButtonQuestOk();
	afx_msg void OnBnClickedButtonQuestCancel();
};

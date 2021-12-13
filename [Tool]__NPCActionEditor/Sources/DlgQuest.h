#pragma once
#include "afxwin.h"


// CDlgQuest 대화 상자입니다.

class CDlgQuest : public CDialog
{
	DECLARE_DYNAMIC(CDlgQuest)

public:
	DWORD	m_dwQuestID;

public:
	CDlgQuest(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgQuest();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_QUEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listQuest;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFind();
};

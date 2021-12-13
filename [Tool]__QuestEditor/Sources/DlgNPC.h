#pragma once
#include "afxwin.h"
#include "GLDefine.h"

// CDlgNPC 대화 상자입니다.

class CDlgNPC : public CDialog
{
	DECLARE_DYNAMIC(CDlgNPC)

public:
	bool		m_bMob;
	SNATIVEID	m_nidNPC;

public:
	CDlgNPC(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgNPC();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_NPC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listNPC;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

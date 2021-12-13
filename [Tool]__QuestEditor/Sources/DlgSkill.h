#pragma once
#include "GLDefine.h"
#include "afxwin.h"


// CDlgSkill 대화 상자입니다.

class CDlgSkill : public CDialog
{
	DECLARE_DYNAMIC(CDlgSkill)

public:
	SNATIVEID	m_nidSKILL;

public:
	CDlgSkill(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSkill();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SKILL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listSKILL;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

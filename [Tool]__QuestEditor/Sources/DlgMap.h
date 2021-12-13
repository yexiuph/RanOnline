#pragma once
#include "afxwin.h"
#include "GLDefine.h"


// CDlgMap 대화 상자입니다.

class CDlgMap : public CDialog
{
	DECLARE_DYNAMIC(CDlgMap)

public:
	SNATIVEID	m_nidMAP;

public:
	CDlgMap(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMap();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListBox m_listMAP;
};

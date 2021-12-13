#pragma once
#include "afxwin.h"
#include "GLDefine.h"


// CDlgItem 대화 상자입니다.

class CDlgItem : public CDialog
{
	DECLARE_DYNAMIC(CDlgItem)

public:
	SNATIVEID	m_nidITEM;

public:
	CDlgItem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgItem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_listITEM;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

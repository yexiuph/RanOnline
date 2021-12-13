#pragma once
#include "afxwin.h"
#include "GLDefine.h"


// CDlgMapSelect 대화 상자입니다.

class CDlgMapSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgMapSelect)

public:
	SNATIVEID	m_nidMAP;

public:
	CDlgMapSelect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMapSelect();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MAP_SELELCT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_ListMap;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

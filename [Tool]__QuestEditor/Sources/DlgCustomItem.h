#pragma once
#include "GLItem.h"

// CDlgCustomItem 대화 상자입니다.

class CDlgCustomItem : public CDialog
{
	DECLARE_DYNAMIC(CDlgCustomItem)

public:
	SITEMCUSTOM		m_sITEMCUSTOM;

public:
	CDlgCustomItem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCustomItem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_INVEN_ELEMENT_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

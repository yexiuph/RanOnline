#pragma once
#include "GLItem.h"

// CDlgCustomItem ��ȭ �����Դϴ�.

class CDlgCustomItem : public CDialog
{
	DECLARE_DYNAMIC(CDlgCustomItem)

public:
	SITEMCUSTOM		m_sITEMCUSTOM;

public:
	CDlgCustomItem(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgCustomItem();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_INVEN_ELEMENT_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

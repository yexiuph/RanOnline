#pragma once
#include "afxwin.h"
#include "GLDefine.h"


// CDlgItem ��ȭ �����Դϴ�.

class CDlgItem : public CDialog
{
	DECLARE_DYNAMIC(CDlgItem)

public:
	SNATIVEID	m_nidITEM;

public:
	CDlgItem(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgItem();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_listITEM;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

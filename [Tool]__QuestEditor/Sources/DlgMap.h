#pragma once
#include "afxwin.h"
#include "GLDefine.h"


// CDlgMap ��ȭ �����Դϴ�.

class CDlgMap : public CDialog
{
	DECLARE_DYNAMIC(CDlgMap)

public:
	SNATIVEID	m_nidMAP;

public:
	CDlgMap(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgMap();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListBox m_listMAP;
};

#pragma once
#include "afxwin.h"
#include "GLDefine.h"


// CDlgMapSelect ��ȭ �����Դϴ�.

class CDlgMapSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgMapSelect)

public:
	SNATIVEID	m_nidMAP;

public:
	CDlgMapSelect(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgMapSelect();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MAP_SELELCT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_ListMap;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

#pragma once
#include "GLDefine.h"
#include "afxwin.h"


// CDlgSkill ��ȭ �����Դϴ�.

class CDlgSkill : public CDialog
{
	DECLARE_DYNAMIC(CDlgSkill)

public:
	SNATIVEID	m_nidSKILL;

public:
	CDlgSkill(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSkill();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SKILL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listSKILL;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

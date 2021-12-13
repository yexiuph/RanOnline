#pragma once
#include "GLQuest.h"
#include "afxwin.h"

// CDlgMobGenItem ��ȭ �����Դϴ�.

class CDlgMobGenItem : public CDialog
{
	DECLARE_DYNAMIC(CDlgMobGenItem)

public:
	SGENQUESTITEM	m_sGENQITEM;

public:
	void UpdateDlgItem ();

public:
	CDlgMobGenItem(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgMobGenItem();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MOBGEN_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonGenitem();
	CListBox m_listboxGENMOB;
	afx_msg void OnBnClickedButtonGenMobAdd();
	afx_msg void OnBnClickedButtonGenMobDel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

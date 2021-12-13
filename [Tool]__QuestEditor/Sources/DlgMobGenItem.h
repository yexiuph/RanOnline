#pragma once
#include "GLQuest.h"
#include "afxwin.h"

// CDlgMobGenItem 대화 상자입니다.

class CDlgMobGenItem : public CDialog
{
	DECLARE_DYNAMIC(CDlgMobGenItem)

public:
	SGENQUESTITEM	m_sGENQITEM;

public:
	void UpdateDlgItem ();

public:
	CDlgMobGenItem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMobGenItem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MOBGEN_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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

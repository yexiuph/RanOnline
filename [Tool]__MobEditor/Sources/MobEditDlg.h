// MobEditDlg.h : header file
//

#pragma once

#include "afxcmn.h"
#include "afxwin.h"

class	CsheetWithTab;
// CMobEditDlg dialog
class CMobEditDlg : public CDialog
{
// Construction
public:
	CMobEditDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MOBEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
public:
	CsheetWithTab	*m_pSheetTab;

	char			m_szAppPath[MAX_PATH];
	CString			m_strAppPath;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	protected:
	virtual void	 PostNcDestroy();
protected:
	bool			DoSave ();
public:
	public:
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonSaveold();
	afx_msg void OnBnClickedButtonLoadcsv();
	afx_msg void OnBnClickedButtonSavecsv();
};

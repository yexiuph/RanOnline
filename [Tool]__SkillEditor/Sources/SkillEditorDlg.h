// SkillEditorDlg.h : header file
//

#pragma once

#include "afxcmn.h"
#include "afxwin.h"

class	CsheetWithTab;
// CSkillEditorDlg dialog
class CSkillEditorDlg : public CDialog
{
// Construction
public:
	CSkillEditorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SKILLEDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:

	/*CSearchDialog	m_wndSearchDlg;*/
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
	virtual void PostNcDestroy();
protected:
	bool DoSave ();
public:
	afx_msg void OnBnClickedButtonLoadskill();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoadcsv();
	afx_msg void OnBnClickedButtonSavecsv();
	afx_msg void OnBnClickedButtonReloadStrtbl();
	afx_msg void OnBnClickedButtonSave2();
	afx_msg void OnBnClickedButtonSave3();
	afx_msg void OnBnClickedButtonLoadcsv2();
};

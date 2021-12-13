#pragma once
#include "afxwin.h"


// CLogDialog 대화 상자입니다.

class CLogDialog : public CDialog
{
	DECLARE_DYNAMIC(CLogDialog)

public:
	CLogDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLogDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LOG };

protected:
	CString m_strFile;

public:
	void SetLogFile ( const char *szFIle )	{ m_strFile = szFIle; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listLog;
	virtual BOOL OnInitDialog();
};

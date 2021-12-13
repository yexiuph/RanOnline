#pragma once
#include "afxwin.h"


// CLogDialog ��ȭ �����Դϴ�.

class CLogDialog : public CDialog
{
	DECLARE_DYNAMIC(CLogDialog)

public:
	CLogDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLogDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_LOG };

protected:
	CString m_strFile;

public:
	void SetLogFile ( const char *szFIle )	{ m_strFile = szFIle; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listLog;
	virtual BOOL OnInitDialog();
};

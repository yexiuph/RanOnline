#pragma once
#include "afxwin.h"

// CInputIDDlg ��ȭ �����Դϴ�.

class CInputIDDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputIDDlg)

public:
	CInputIDDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CInputIDDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_INPUTIDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	UINT m_nNewID;
	virtual BOOL OnInitDialog();
};

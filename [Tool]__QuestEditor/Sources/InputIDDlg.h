#pragma once
#include "afxwin.h"

// CInputIDDlg 대화 상자입니다.

class CInputIDDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputIDDlg)

public:
	CInputIDDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInputIDDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INPUTIDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	UINT m_nNewID;
	virtual BOOL OnInitDialog();
};

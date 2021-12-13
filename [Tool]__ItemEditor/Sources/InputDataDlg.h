#pragma once


// CInputDataDlg 대화 상자입니다.

class CInputDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputDataDlg)

public:
	CInputDataDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInputDataDlg();

	CString m_strItemName;
	bool	m_bNum;
	DWORD	m_dwNum;
	float	m_fRate;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INPUTDATADLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
};

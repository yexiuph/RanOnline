#pragma once


// CInputDataDlg ��ȭ �����Դϴ�.

class CInputDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputDataDlg)

public:
	CInputDataDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CInputDataDlg();

	CString m_strItemName;
	bool	m_bNum;
	DWORD	m_dwNum;
	float	m_fRate;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_INPUTDATADLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
};

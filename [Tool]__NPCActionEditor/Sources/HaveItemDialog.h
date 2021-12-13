#pragma once


// CHaveItemDialog 대화 상자입니다.

class CHaveItemDialog : public CDialog
{
	DECLARE_DYNAMIC(CHaveItemDialog)

public:
	CHaveItemDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CHaveItemDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_HAVEITEMDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

private:
	DWORD	m_dwItemID;
	BOOL	m_bConfirm;

public:
	DWORD	GetItemID ();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonFind();
	virtual BOOL OnInitDialog();
};

#pragma once
#include "afxwin.h"
#include <map>


// CDlgItem 대화 상자입니다.

class CDlgItem : public CDialog
{
	DECLARE_DYNAMIC(CDlgItem)

public:
	DWORD	m_dwITEM;

	bool	m_bRateEnable;
	bool	m_bNumEnable;
	bool	m_bUseMonster;

	float	m_fRate;
	DWORD	m_dwNum;
	std::map<DWORD,DWORD> m_SelMap;

	WORD   m_wMonsterMid;
	WORD   m_wMonsterSid;

public:
	CDlgItem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgItem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_listITEM;
	CListBox m_listMonster;

	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	CEdit m_Edit_Percent;
	afx_msg void OnEnSetfocusEditPercent();
};

#pragma once
#include "afxwin.h"
#include <map>


// CDlgItem ��ȭ �����Դϴ�.

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
	CDlgItem(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgItem();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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

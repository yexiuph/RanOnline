#pragma once
#include "GLQUEST.h"

// CQuestStepPage 대화 상자입니다.
class CsheetWithTab;
class CQuestStepPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CQuestStepPage)

public:
	CsheetWithTab* m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab* pSheetTab )		{ m_pSheetTab = pSheetTab; }

public:
	void VisibleItems_Step ( bool bVisible );
	void UpdateItems ();
	void UpdateItems_Step ();

	void InverseUpdateItems ();
	void InverseUpdateItems_Step ();

	void SetReadMode();

public:
	bool m_bDlgInit;
	DWORD m_dwCurStep;
	CFont* m_pFont;

public:
	CQuestStepPage( LOGFONT logfont );
	virtual ~CQuestStepPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FORMVIEW_QUESTSTEP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTalknpc();
	afx_msg void OnBnClickedButtonDiemob();
	afx_msg void OnBnClickedButtonGuardnpc();
	afx_msg void OnBnClickedButtonReachMap();
	afx_msg void OnBnClickedButtonGuardMap();
	afx_msg void OnLbnSelchangeListStep();
	afx_msg void OnBnClickedButtonReqQitemNew();
	afx_msg void OnBnClickedButtonReqQitemEdit();
	afx_msg void OnBnClickedButtonReqQitemDel();
	afx_msg void OnBnClickedButtonDelQitemNew();
	afx_msg void OnBnClickedButtonDelQitemDel();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonStepNew();
	afx_msg void OnBnClickedButtonReqStepDel();
	afx_msg void OnBnClickedButtonTalknpcDel();
	afx_msg void OnBnClickedButtonDiemobDel();
	afx_msg void OnBnClickedButtonGuardnpcDel();
	afx_msg void OnBnClickedButtonReachMapDel();
	afx_msg void OnBnClickedButtonGuardMapDel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonStepOk();
	afx_msg void OnBnClickedButtonGiftQitemNew();
	afx_msg void OnBnClickedButtonGiftQitemEdit();
	afx_msg void OnBnClickedButtonGiftQitemDel();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnLbnDblclkListStep();
	afx_msg void OnBnClickedButtonEditcancel();
	afx_msg void OnBnClickedButtonEditcomplate();
	afx_msg void OnBnClickedStepMapEditButton();
};

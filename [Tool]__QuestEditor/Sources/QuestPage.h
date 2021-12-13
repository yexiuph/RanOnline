#pragma once
#include "GLQUEST.h"
#include "afxwin.h"

// CQuestPage ��ȭ �����Դϴ�.
class CsheetWithTab;
class CQuestPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CQuestPage)

public:
	CsheetWithTab* m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab* pSheetTab )		{ m_pSheetTab = pSheetTab; }

public:
	void UpdateItems ();
	void InverseUpdateItems ();

	void SetReadMode ();

	void UpdateProgress();
	void UpdateProgressType( DWORD type, bool bNew = FALSE );

public:
	bool m_bDlgInit;
	CFont* m_pFont;
	// State �� �� �̵��� ���� ���� �ִ��� 
	bool m_bStateMoveMap[3];

	GLQUEST_PROGRESS m_tempProgressEvent;

public:
	CQuestPage( LOGFONT logfont );
	virtual ~CQuestPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FORMVIEW_QUEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonGiftItemNew();
	afx_msg void OnBnClickedButtonGiftItemEdit();
	afx_msg void OnBnClickedButtonGiftItemDel();
	afx_msg void OnBnClickedButtonGiftSkillNew();
	afx_msg void OnBnClickedButtonGiftSkillDel();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonNext();
	CComboBox m_comboSchool;
	afx_msg void OnBnClickedButtonEditcancel();
	afx_msg void OnBnClickedButtonEditcomplate();
	afx_msg void OnBnClickedUseProgressCheck();
	afx_msg void OnCbnSelchangeStateCombo1();
	afx_msg void OnCbnSelchangeStateCombo2();
	afx_msg void OnCbnSelchangeStateCombo3();
	afx_msg void OnBnClickedMapEditButton();
	afx_msg void OnBnClickedFailCheck1();
	afx_msg void OnBnClickedFailCheck2();
	afx_msg void OnBnClickedFailCheck3();
	afx_msg void OnCbnSelchangeTypeCombo();
	afx_msg void OnBnClickedButtonMovemap();
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioMan();
	afx_msg void OnBnClickedRadioWoman();
};
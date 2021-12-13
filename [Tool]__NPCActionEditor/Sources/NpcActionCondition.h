#pragma once

#include "NpcTalkCondition.h"
#include "afxwin.h"

// CNpcActionCondition 대화 상자입니다.
class	CsheetWithTab;
class	CNpcDialogue;

class CNpcActionCondition : public CPropertyPage
{
	DECLARE_DYNAMIC(CNpcActionCondition)

public:
	CNpcActionCondition( LOGFONT logfont );
	virtual ~CNpcActionCondition();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCACTIONCONDITION };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	SNpcTalkCondition*	m_pOrgCondition;
	SNpcTalkCondition	m_DummyCondition;
	CFont*				m_pFont;

	BOOL			m_bDlgInit;
	DWORD			m_TalkNeedItem[5];

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL	SetCondition ( SNpcTalkCondition* pConditon );

	void	UpdateItems ();

protected:	
	BOOL	InverseUpdateItems ();
	void	InitDefaultCtrls ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonItemAdd();
	afx_msg void OnBnClickedButtonSkillAdd();
	afx_msg void OnBnClickedButtonSkillDel();
	afx_msg void OnBnClickedButtonItemDel();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonItemDelall();
	afx_msg void OnBnClickedButtonSkillDelall();
	afx_msg void OnCbnSelchangeComboLevelSign();
	afx_msg void OnCbnSelchangeComboElementalSign();
	afx_msg void OnCbnSelchangeComboActionpointSign();
	afx_msg void OnBnClickedButtonItemQuestSelect();
	afx_msg void OnBnClickedCheckTime();
	CListBox m_listDisSkill;
	CListBox m_listDisQuest;
	CComboBox m_comboSTEP;
	afx_msg void OnBnClickedButtonQuestDel();
	afx_msg void OnBnClickedButtonDisSkillAdd();
	afx_msg void OnBnClickedButtonDisSkillDel();
	afx_msg void OnBnClickedButtonDisQuestAdd();
	afx_msg void OnBnClickedButtonDisQuestDel();
	afx_msg void OnCbnSelchangeComboQuestStep();
	afx_msg void OnBnClickedButtonComquestAdd();
	afx_msg void OnBnClickedButtonComquestDel();
	afx_msg void OnBnClickedButtonComquestDelall();
	CListBox m_listComQuest;
	CComboBox m_comboSchool;
	afx_msg void OnNMClickListcontItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioMan();
	afx_msg void OnBnClickedRadioWoman();
};

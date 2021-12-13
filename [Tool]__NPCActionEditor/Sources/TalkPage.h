#pragma once

#include "NpcTalk.h"
#include "afxwin.h"

class	CNpcTalkControl;
class	CNpcDialogueSet;
class	CsheetWithTab;

// CTalkPage 대화 상자입니다.

class CTalkPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTalkPage)

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

private:
	BOOL		m_bModify;
	SNpcTalk*	m_pOrgTalk;
	SNpcTalk	m_Talk;
	CFont*		m_pFont;

private:
	CNpcTalkControl*	m_pTalkControl;
	CNpcDialogueSet*	m_pNpcDialogueSet;

public:
	CListCtrl	m_ctrlRandomID;

public:
	void	UpdateItems ();
	void	SetWndView ( int nACTION, DWORD dwNO );

public:
	void	SetTalkData ( CNpcDialogueSet* pNpcDialogueSet, CNpcTalkControl* pTalkControl );
	void	SetModifyData ( BOOL bModify, SNpcTalk* pTalk );

	DWORD	GetNeedItem( DWORD dwItemNum ) { return m_Talk.m_dwNeedItem[dwItemNum]; }
	void	SetNeedItem( DWORD dwSetItemNum, DWORD dwNeedItemNum ) { m_Talk.m_dwNeedItem[dwSetItemNum] = dwNeedItemNum; }

	int		GetActionType() { return m_Talk.m_nACTION; }
	int		GetActionNO()	{ return m_Talk.m_dwACTION_NO; }

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }

	void	SetReadMode ();

public:
	CTalkPage( LOGFONT logfont );
	virtual ~CTalkPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCTALKDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboActionType();
	afx_msg void OnCbnSelchangeComboActionData();
	afx_msg void OnBnClickedButtonConditonSet();
	afx_msg void OnBnClickedButtonConditonReset();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonTradeItem();
	afx_msg void OnBnClickedButtonTradeDel();
	CComboBox m_comboQUESTSTART[MAX_QUEST_START];
	CComboBox m_comboQUEST_PROC;
	CComboBox m_comboQUEST_PROC_STEP;
	afx_msg void OnCbnSelchangeComboQuestStart(UINT nID);
	afx_msg void OnCbnSelchangeComboQuestProg();
	afx_msg void OnCbnSelchangeComboQuestProgStep();
	afx_msg void OnBnClickedButtonBus();
	afx_msg void OnBnClickedRandomInsert();
	afx_msg void OnBnClickedRandomDelete();
	afx_msg void OnBnClickedRandomReset();
};

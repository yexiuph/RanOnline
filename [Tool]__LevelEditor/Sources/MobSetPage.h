#pragma once
#include "afxwin.h"

#include "GLMobSchedule.h"

class CsheetWithTab;
// CMobSetPage dialog

class CMobSetPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMobSetPage)

public:
	CMobSetPage();
	virtual ~CMobSetPage();

// Dialog Data
	enum { IDD = IDD_MOB_SET_PAGE };

private:
	bool			m_bCut;

public:
	static BOOL				m_bShow;
	static BOOL				m_bMaterial;
	static GLMobSchedule	m_Schedule;

	BOOL			m_bInit;

	CsheetWithTab*	m_pSheetTab;

	CString			m_strCopyMobName;
	CString			m_strBackMobName;

public:
	void	CopyMobSch();
	void	CutMobSch();
	void	PasteMobSch();

	void	ActiveMobSetEdit ( BOOL bShow );

	void	SetSheetTab ( CsheetWithTab* SheetTab ) { m_pSheetTab = SheetTab; }
	void	SetSelectSchedule ( GLMobSchedule* pMobSchedule );
	void	SetPC_GATE_REGEN ( DWORD dwGateID );
	void	SetNewID ( WORD wMID, WORD wSID );

	void	UpdatePage();

private:
	void	ListingUpdate();
	BOOL	UpdateDataSchedule ( BOOL bGetData, BOOL bNew );

protected:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_ctrlMobList;
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnLbnDblclkListMob();
	afx_msg void OnBnClickedButtonMobNew();
	afx_msg void OnBnClickedButtonMobEdit();
	afx_msg void OnBnClickedButtonMobDel();
	afx_msg void OnBnClickedButtonCrowId();
	afx_msg void OnBnClickedCheckWeek();
	afx_msg void OnBnClickedCheckRandPos();
	afx_msg void OnBnClickedButtonFindPos();
	afx_msg void OnDeltaposSpinNum(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinDist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRevGate();
	afx_msg void OnBnClickedButtonBus();
	afx_msg void OnBnClickedButtonSchdlMan();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};

#pragma once

#include "DxMethods.h"
#include "GLList.h"
#include "GLDefine.h"

#include "GLMobSchedule.h"
#include "afxwin.h"

// CMobSchedulePage dialog
class	CsheetWithTab;
struct	SMOBACTION;

class CMobSchedulePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMobSchedulePage)

public:
	CMobSchedulePage();
	virtual ~CMobSchedulePage();

// Dialog Data
	enum { IDD = IDD_MOB_SCH_PAGE };

public:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bInit;

	MOBACTIONLIST*	m_pMobActList;
	SMOBACTION*		m_pMobAction;

	CString			m_valTime;

	DXAFFINEMATRIX	m_AffineMatrix;

public:
	void SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	void SetSchedule ( MOBACTIONLIST* pMobActList );

private:
	void ActiveMobScheDuleEdit ( BOOL bShow, BOOL bNew );
	void ListingUpdate ();

protected:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_ctrlSchedule;
	CComboBox m_ctrlAction;
	afx_msg void OnBnClickedButtonSchSetComplet();
	afx_msg void OnLbnDblclkListSch();
	afx_msg void OnBnClickedButtonSchNew();
	afx_msg void OnBnClickedButtonschEdit();
	afx_msg void OnBnClickedButtonschMove();
	afx_msg void OnBnClickedButtonSchDel();
	afx_msg void OnCbnSelchangeComboSchAct();
	afx_msg void OnBnClickedButtonSchOk();
	afx_msg void OnBnClickedButtonSchCancel();
};

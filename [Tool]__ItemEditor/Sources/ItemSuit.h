#pragma once
#include "afxwin.h"


// CItemSuit 대화 상자입니다.
class	CsheetWithTab;
struct	SITEM;
class CItemSuit : public CPropertyPage
{
	DECLARE_DYNAMIC(CItemSuit)

public:
	CItemSuit( LOGFONT logfont );
	virtual ~CItemSuit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMSUIT_DIALOG };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	SITEM*			m_pDummyItem;
	SITEM*			m_pItem;

	BOOL			m_bDlgInit;
	CFont*			m_pFont;


public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL	SetItem ( SITEM* pItem );
	void	SetReadMode();

protected:
	void	UpdateItems ();
	bool	InverseUpdateItems ();
	void	InitDefaultCtrls ();

	//////////////////////////////////////////////////////////////////////////////
	//	리팩토링 적용펑션
public:
	void	InitAllItem ();

	void	InitBox ();
	void	InitBox_Rand ();
	void	InitPetSkinPack();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_Mob;
	CListBox  m_listBOX;
	CListBox  m_listBox_Rand;
	CEdit m_Edit_QueTime;
	CEdit m_Edit_QueExp;
	CEdit m_Edit_QueParam1;
	CEdit m_Edit_QueParam2;

	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnCbnSelchangeComboAddonNo();
	afx_msg void OnCbnDropdownComboAddonNo();
	afx_msg void OnCbnSelchangeComboBlow();
	afx_msg void OnBnClickedButtonBoxAdd();
	afx_msg void OnBnClickedButtonBoxDel();
	afx_msg void OnBnClickedButtonBoxAdd2();
	afx_msg void OnBnClickedButtonBoxDel2();
	afx_msg void OnCbnSelchangeComboQuestion();	
	afx_msg void OnBnClickedButtonBoxUp1();
	afx_msg void OnBnClickedButtonBoxDw1();
	afx_msg void OnBnClickedButtonBoxUp2();
	afx_msg void OnBnClickedButtonBoxDw2();
	afx_msg void OnBnClickedButtonAddPetskin();
	afx_msg void OnBnClickedButtonDelPetskin();
};
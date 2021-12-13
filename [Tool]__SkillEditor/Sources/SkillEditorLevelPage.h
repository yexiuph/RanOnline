#pragma once
#include <string>
#include "GLDefine.h"
#include "GLSkill.h"
#include "SKillEditorTreePage.h"
//#include "SKillEditorBasicPage.h"

// SkillEditorLevelPage dialog
class	CsheetWithTab;
struct	GLSKILL;

class SkillEditorLevelPage : public CPropertyPage
{
	DECLARE_DYNAMIC(SkillEditorLevelPage)

public:
	SkillEditorLevelPage(LOGFONT logfont);
	virtual ~SkillEditorLevelPage();

// Dialog Data
	enum { IDD = IDD_SKILLEDITOR_LEVEL };

public:
	SkillEditorTreePage m_SkillEditorTreePage;

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;
	BOOL			m_bDlgInit;
	CFont*			m_pFont;

public:
	PGLSKILL		m_pDummySkill;
	PGLSKILL		m_pSkill;
	void			SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void			SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	BOOL			SetSkill ( PGLSKILL pSkill );
	DWORD			m_dwSkillLevel;
	void	ClearData();

protected:
	void	UpdateSkill ( DWORD	m_dwSkillLevel );
	BOOL	InverseUpdateSkill( DWORD	m_dwSkillLevel );
	void	InitDefaultCtrls ();
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void	InitAllSkill();
	void	Loadegp( int nID );
	void	Loadcps( int nID );
	void	Loadchf( int nID );

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonLvlminus();
	afx_msg void OnBnClickedButtonLvladd();
	afx_msg void OnBnClickedButtonTraneffect();
	afx_msg void OnBnClickedButtonTranM1();
	afx_msg void OnBnClickedButtonTranM2();
	afx_msg void OnBnClickedButtonTranM3();
	afx_msg void OnBnClickedButtonTranM4();
	afx_msg void OnBnClickedButtonTranM5();
	afx_msg void OnBnClickedButtonTranM6();
	afx_msg void OnBnClickedButtonTranM7();
	afx_msg void OnBnClickedButtonTranM8();
	afx_msg void OnBnClickedButtonTranW1();
	afx_msg void OnBnClickedButtonTranW2();
	afx_msg void OnBnClickedButtonTranW3();
	afx_msg void OnBnClickedButtonTranW4();
	afx_msg void OnBnClickedButtonTranW5();
	afx_msg void OnBnClickedButtonTranW6();
	afx_msg void OnBnClickedButtonTranW7();
	afx_msg void OnBnClickedButtonTranW8();
	afx_msg void OnBnClickedButtonTranChf();
};

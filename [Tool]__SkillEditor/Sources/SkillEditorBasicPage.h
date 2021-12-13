#pragma once
#include <string>
#include "GLDefine.h"
#include "GLSkill.h"
#include "SKillEditorTreePage.h"
//#include "SKillEditorLevelPage.h"

// SkillEditorBasicPage dialog
class	CsheetWithTab;
struct	GLSKILL;

class SkillEditorBasicPage : public CPropertyPage
{
	DECLARE_DYNAMIC(SkillEditorBasicPage)

public:
	SkillEditorBasicPage(LOGFONT logfont);
	virtual ~SkillEditorBasicPage();

// Dialog Data
	enum { IDD = IDD_SKILLEDITOR_BASIC };

public:
	SkillEditorTreePage m_SkillEditorTreePage;
//	SkillEditorLevelPage m_SkillEditorLevelPage;

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;
	BOOL			m_bDlgInit;
	std::string		strKeyName;
	std::string		strName;
	std::string		strKeyDesc;
	std::string		strDesc;
	CFont*			m_pFont;
	
public:
	PGLSKILL		m_pDummySkill;
	PGLSKILL		m_pSkill;
	void			SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void			SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	BOOL			SetSkill ( PGLSKILL pSkill );
	DWORD			m_dwEffectType;
	void	ClearData();

protected:
	void	UpdateSkill ();
	BOOL	InverseUpdateSkill();
	void	UpdateSkillSExtData (DWORD dwType);
	BOOL	InverseUpdateSkillSExtData (DWORD dwType);
	void	InitDefaultCtrls ();
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void	InitAllSkill();
	void	Loadegp( int nID );
	void	Loadeffskin( int nID );
	void	Loadtexture( int nID );

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonEleminus();
	afx_msg void OnBnClickedButtonEleadd();
	afx_msg void OnCbnSelchangeComboEmanimid();
	afx_msg void OnCbnSelchangeComboEmanisid();
	afx_msg void OnCbnSelchangeComboEmbasictype();
	afx_msg void OnCbnSelchangeComboEmelement();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonIcon();
	afx_msg void OnBnClickedButtonTar01();
	afx_msg void OnBnClickedButtonTar02();
	afx_msg void OnBnClickedButtonSelf01();
	afx_msg void OnBnClickedButtonSelf02();
	afx_msg void OnBnClickedButtonTar();
	afx_msg void OnBnClickedButtonTargetbody3();
	afx_msg void OnBnClickedButtonSelfbody();
	afx_msg void OnBnClickedButtonTargetbody1();
	afx_msg void OnBnClickedButtonTargetbody2();
};

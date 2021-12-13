#pragma once
#include "afxcmn.h"
#include "GLSkill.h"

// SkillEditorTreePage dialog
class	CsheetWithTab;
class SkillEditorTreePage : public CPropertyPage
{
	DECLARE_DYNAMIC(SkillEditorTreePage)

public:
	SkillEditorTreePage(LOGFONT logfont);
	virtual ~SkillEditorTreePage();

// Dialog Data
	enum { IDD = IDD_SKILLEDITOR_TREEPAGE };

private:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;
	CFont*			m_pFont;
	CTreeCtrl		m_ctrlSkillTree;	
	HTREEITEM		m_SkillTreeRoot[EMSKILLCLASS_NSIZE];
	HTREEITEM		m_SkillTree;
	std::string		strTempNameKey;
	std::string		strTempName;
	std::string		strTempDescKey;
	std::string		strTempDesc;

public:
	BOOL			UpdateTree ();
	void			UpdateName ();
	void			SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void			UpdateData ( WORD MID, WORD SID,PGLSKILL pSkill );
	void			UpdateData ( PGLSKILL pSkill );

private:
	const char*		GetFormatName ( PGLSKILL pSkill  );
	BOOL			UpdateSkill ( PGLSKILL pSkill, HTREEITEM hMainItem );
	
	PGLSKILL		m_pSkill;

	BOOL			m_bDummyHasSkill;
	PGLSKILL		m_pDummySkill;

	int				GetSelectMenu ( POINT pt );
	void			ForceSelectOnRightClick ( POINT pt );
	void			SetSelectSkill ( PGLSKILL pSkillWork );
	PGLSKILL		GetSelectSkill ();
	void			RunSelectMenu ( int nSelect );	
	WORD			GetRootID();
	BOOL			AddSkill ();  
	BOOL			DelSkill ();
	BOOL			CopySkill ();
	BOOL			PasteSkillNew ();
	BOOL			PasteSkillValue();
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
public:

	void			OnClickLeft(NMHDR *pNMHDR, LRESULT *pResult);
	void			OnClickRight(NMHDR *pNMHDR, LRESULT *pResult);
	virtual			BOOL OnInitDialog();
	void			ClearData();
};

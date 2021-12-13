#pragma once
#include "afxcmn.h"
#include "GLCrow.h"


// MobEditTree dialog
class	CsheetWithTab;
class MobEditTree : public CPropertyPage
{
	DECLARE_DYNAMIC(MobEditTree)

public:
	MobEditTree(LOGFONT logfont);
	virtual ~MobEditTree();

// Dialog Data
	enum { IDD = IDD_MOBEDIT_TREE };

private:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;
	CFont*			m_pFont;
	CTreeCtrl		m_ctrlCrowTree;
	HTREEITEM		m_TreeRoot;
	HTREEITEM		hMainCrow;
	HTREEITEM		hSubCrow;
	
	std::string		strTempNameKey;
	std::string		strTempName;

public:
	BOOL			UpdateTree ();
	void			SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void			UpdateName ();
	void			UpdateData ( WORD MID, WORD SID,PCROWDATA pCrow );
	void			UpdateData ( PCROWDATA pCrow );

private:
	const char*		GetFormatName ( PCROWDATA pCrow  );
	BOOL			UpdateCrow ( PCROWDATA pCrow, HTREEITEM hMainCrow );
	BOOL			m_bDummyHasCrow;
	PCROWDATA		m_pCrow;
	PCROWDATA		m_pDummyCrow;
	int				GetSelectMenu ( POINT pt );
	void			ForceSelectOnRightClick ( POINT pt );
	void			SetSelectCrow ( PCROWDATA pCrow );
	PCROWDATA		GetSelectCrow ();
	void			RunSelectMenu ( int nSelect );	
	WORD			GetRootID();
	BOOL			AddCrow ();  
	BOOL			DelCrow ();
	BOOL			CopyCrow ();
	BOOL			PasteCrowNew ();
	BOOL			PasteCrowValue();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual			BOOL OnInitDialog();
	void			OnClickLeft(NMHDR *pNMHDR, LRESULT *pResult);
	void			OnClickRight(NMHDR *pNMHDR, LRESULT *pResult);
};

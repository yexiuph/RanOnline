#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

#include <vector>
#include "SKillEditorTreePage.h"
#include "SKillEditorBasicPage.h"
#include "SKillEditorLevelPage.h"


enum EDITSHEET
{
	SKILLTREEPAGE		= 0,
	SKILLBASICPAGE		= 1,
	SKILLLEVELPAGE		= 2,

	EDITSHEET			= 3
};

enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

extern CString g_strEditSheet[EDITSHEET];

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*				m_pWndParent;

public:
	SkillEditorTreePage m_SkillEditorTreePage;
	SkillEditorBasicPage m_SkillEditorBasicPage;
	SkillEditorLevelPage m_SkillEditorLevelPage;


	void	ClearData();


public:	
	void ActiveSkillTreePage ();
	//void SetSelectIndex( int iSelectIndex ) { m_ItemSuitTreePage.SetSelectIndex( iSelectIndex ); };
	void ActiveSkillBasicPage ( int CurPage, PGLSKILL pSkill );
	void ActiveSkillLevelPage ( int CurPage, PGLSKILL pSkill );
	void UpdateSkillTreePage();

// Construction
public:
	CsheetWithTab(CWnd* pParentWnd = NULL);
	CsheetWithTab(CRect& rect, LOGFONT logfont, CWnd* pParentWnd = NULL );
	virtual ~CsheetWithTab();

// Attributes
public:
	CRect		m_Rect;

	BOOL		m_bNoTabs;
	int			m_iDistanceFromTop;
	CFont*		m_pFont;

// Get/Set
public:
   void NoTabs() { m_bNoTabs = TRUE; }
   void SetDistanceFromTop( int iDistance ) { m_iDistanceFromTop = iDistance; }
   void ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CsheetWithTab)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:

// Generated message map functions
protected:
	//{{AFX_MSG(CsheetWithTab)
   afx_msg HBRUSH CtlColor(CDC* pDc, UINT uCtlColor);	
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __SHEETWITHTAB_H_
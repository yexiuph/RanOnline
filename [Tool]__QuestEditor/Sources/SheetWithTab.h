#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

#include "QuestStep.h"
#include "QuestPage.h"
#include "QuestTree.h"

enum EDITSHEET
{
	QUESTTREE_PAGE  = 0,
	QUEST_PAGE		= 1,
	QUESTSTEP_PAGE	= 2,

	EDITSHEET		= 3
};

enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

class GLQUEST;

//extern CString g_strEditSheet[EDITSHEET];

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*			m_pWndParent;

public:
	CQuestTree		m_cQuestTreePage;
	CQuestPage		m_cQuestPage;
	CQuestStepPage	m_cQuestStep;

public:
	void ActiveQuestPage ( int CurPage, PQUESTNODE pQuestNode );
	void ActiveQuestStepPage ( int CurPage, PQUESTNODE pQuestNode );
	void ActiveQuestTreePage ();
	void UpdateQuestTreePage ();

	void InverseUpdateItems ();

// Construction
public:
	CsheetWithTab(CWnd* pParentWnd = NULL);
	CsheetWithTab(CRect& rect, LOGFONT logfont, CWnd* pParentWnd = NULL );
	void ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag);
	virtual ~CsheetWithTab();

// Attributes
public:
   CRect      m_Rect;

   BOOL       m_bNoTabs;
   int        m_iDistanceFromTop;
   CFont*	  m_pFont;

// Get/Set
public:
   void NoTabs() { m_bNoTabs = TRUE; }
   void SetDistanceFromTop( int iDistance ) { m_iDistanceFromTop = iDistance; }

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
#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

#include "NpcActionTable.h"
#include "NpcActionPage.h"
#include "NpcActionCondition.h"
#include "NpcActionAnswer.h"
#include "TalkPage.h"

enum EDITSHEET
{
	NPCACTIONTABLE		= 0,
	NPCACTIONPAGE		= 1,
	NPCACTIONCONDITION	= 2,
	NPCACTIONANSWER		= 3,

	NPCACTIONTALK		= 4,

	EDITSHEET			= 5
};

class	CNpcDialogue;
extern CString g_strEditSheet[EDITSHEET];

enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*				m_pWndParent;

public:
	CNpcActionTable		m_NpcActionTable;
	CNpcActionPage		m_NpcActionPage;
	CNpcActionCondition	m_NpcActionCondition;
	CNpcActionAnswer	m_NpcActionAnswer;
	CTalkPage			m_NpcActionTalk;

public:
	void ActiveActionTable ();
	void ActiveActionPage  ( CNpcDialogue* pNpcDialogue );
	void ActiveActionCondition ( int nCallPage, SNpcTalkCondition* pConditon );
	void ActiveActionAnswer ( CNpcDialogueSet *pNpcDialogueSet=NULL, CNpcDialogueCase* pDialogueCase=NULL );	

	void ActiveActionTalk ( int nCallPage, CNpcDialogueSet* pNpcDialogueSet, CNpcTalkControl* pTalkControl, BOOL bModify, SNpcTalk* pTalk );

public:
	CNpcDialogueSet* GetDialogueSet ();

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
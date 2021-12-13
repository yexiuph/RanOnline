#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

#include <vector>
#include "ItemSuitTree.h"
#include "ItemParent.h"
#include "ItemSuit.h"

enum EDITSHEET
{
	ITEMSUITTREEPAGE	= 0,
	ITEMPARENTPAGE		= 1,
	ITEMSUITPAGE		= 2,

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
	CItemSuitTree	m_ItemSuitTreePage;	
	CItemParent		m_ItemParentPage;
	CItemSuit		m_ItemSuitPage;

public:	
	void ActiveItemSuitTreePage ();
	void SetSelectIndex( int iSelectIndex ) { m_ItemSuitTreePage.SetSelectIndex( iSelectIndex ); };
	void SearchItemSelect( BYTE dwType, char *szItemName );
	void ActiveItemParentPage ( int CurPage, SITEM* pItem );
	void ActiveItemSuitPage ( int CurPage, SITEM* pItem );
	void UpdateSuitTreePage();
	std::vector<SSearchItemList> GetSearchItemList() { return m_ItemSuitTreePage.m_vecSearchItemList; }

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
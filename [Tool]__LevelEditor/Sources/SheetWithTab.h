#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

#include "LevelMainPage.h"
#include "MobSetPage.h"
#include "MobSchedulePage.h"
#include "LevelLandGatePage.h"
#include "CrowTree.h"
#include "RequirePage.h"
#include "ItemTreePage.h"
#include "SkillTreePage.h"
#include "GateListPage.h"
#include "LevelSingleEffect.h"
#include "LandMarkPage.h"
#include "EtcFuncPage.h"

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

enum EDITSHEET
{
	MAINPAGE			= 0,
	MOBSETPAGE			= 1,
	MOBSCHEDULEPAGE		= 2,
	LANDGATEPAGE		= 3,
	CROWTREEPAGE		= 4,
	REQUIREPAGE			= 5,
	ITEMTREEPAGE		= 6,
	SKILLTREEPAGE		= 7,
	GATELISTPAGE		= 8,
	SINGLEEFFPAGE		= 9,
	LANDMARKPAGE		= 10,
	ETCFUNCPAGE			= 11,

	EDITSHEETNUM		= 12
};

extern CString g_strEditSheet[EDITSHEETNUM];

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*				m_pWndParent;

public:
	CLevelMainPage		m_MainPage;
	CMobSetPage			m_MobSetPage;
	CMobSchedulePage	m_MobSchedulePage;
	CLevelLandGatePage	m_LandGatePage;
	CCrowTree			m_CrowTreePage;
	CRequirePage		m_RequirePage;
	CItemTreePage		m_ItemTreePage;
	CSkillTreePage		m_SkillTreePage;
	CGateListPage		m_GateListPage;
	CLevelSingleEffect	m_SingleEffect;
	CLandMarkPage		m_LandMarkPage;
	CEtcFuncPage		m_EtcFuncPage;

public:
	
	void ActiveMobSchedulePage( CGLLIST<SMOBACTION> *pMobActList );
	void ActiveCrowTreePage( int CallPage, SNATIVEID NativeID );
	void ActiveItemTreePage( int CallPage, SNATIVEID NativeID );
	void ActiveSkillTreePage( int CallPage, SNATIVEID NativeID );
	void ActiveGateListPage( int CallPage, DWORD dwGateID );
	void UpdatePage();

// Construction
public:
	CsheetWithTab(CWnd* pParentWnd = NULL);
	CsheetWithTab(CRect& rect, CWnd* pParentWnd = NULL );
	virtual ~CsheetWithTab();

// Attributes
public:
	CRect		m_Rect;

	BOOL		m_bNoTabs;
	int			m_iDistanceFromTop;

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
};

#endif
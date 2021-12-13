// SheetWithTab.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "SheetWithTab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString g_strEditSheet[EDITSHEETNUM] =
{
	"Main",						//	0
	"MobSet",					//	1
	"MobSchedule",				//	2
	"LandGate",					//	3
	"CrowTree",					//	4
	"Require",					//	5
	"ItemTree",					//	6
	"SkillTree",				//	7
	"GateList",					//	8
	"SingleEffect",				//	9
	"LandMark",					//	10
	"EtcFunc",					//	11
};

// CsheetWithTab

IMPLEMENT_DYNAMIC(CsheetWithTab, CPropertySheet)

CsheetWithTab::CsheetWithTab(CRect& rect, CWnd* pParentWnd)
	:CPropertySheet(IDS_PROPSHT_CAPTION, pParentWnd),
	 m_bNoTabs(FALSE),
	 m_pWndParent(NULL),
	 m_iDistanceFromTop(0)
{
	m_pWndParent = pParentWnd;
	m_Rect = rect;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

	m_MainPage.SetSheetTab ( this );			//0
	AddPage ( &m_MainPage );

	m_MobSetPage.SetSheetTab ( this );			//1
	AddPage ( &m_MobSetPage );

	m_MobSchedulePage.SetSheetTab ( this );		//2
	AddPage ( &m_MobSchedulePage );

	m_LandGatePage.SetSheetTab ( this );		//3
	AddPage ( &m_LandGatePage );

	m_CrowTreePage.SetSheetTab ( this );		//4
	AddPage ( &m_CrowTreePage );

	m_RequirePage.SetSheetTab ( this );			//5
	AddPage ( &m_RequirePage );

	m_ItemTreePage.SetSheetTab ( this );		//6
	AddPage ( &m_ItemTreePage );

	m_SkillTreePage.SetSheetTab ( this );		//7
	AddPage ( &m_SkillTreePage );

	m_GateListPage.SetSheetTab ( this );		//8
	AddPage ( &m_GateListPage );

	m_SingleEffect.SetSheetTab ( this );		//9
	AddPage ( &m_SingleEffect );

	m_LandMarkPage.SetSheetTab ( this );		//10
	AddPage ( &m_LandMarkPage );

	m_EtcFuncPage.SetSheetTab ( this );			//11
	AddPage ( &m_EtcFuncPage );
}

CsheetWithTab::~CsheetWithTab()
{
}

static int module_piButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };

BOOL CsheetWithTab::OnInitDialog() 
{
	HWND hTabWnd;
	CRect rectTabCtrl;

	if( m_Rect.right )
	{
		// resize the tab control
		hTabWnd = (HWND)::GetDlgItem(m_hWnd, AFX_IDC_TAB_CONTROL);
		ASSERT(hTabWnd != NULL);
		::GetWindowRect(hTabWnd, &rectTabCtrl);  // Tab-control rectangle (screen coordinates)
		ScreenToClient(rectTabCtrl);

		//	Note : 탭을 보여주지 않을 때에는 사이즈의 조정이 필요 없으므로 무시한다.
		//
		// Is tab-control wider than specified width.
		if( !m_bNoTabs && rectTabCtrl.right > (m_Rect.Width() - 3) )
		{
			rectTabCtrl.right = m_Rect.Width() - 3; // Set new right position
		}

		// The tab-control does only need to show tabs
		rectTabCtrl.bottom = 32; // Height for tabs

		// move tab control
		::MoveWindow( hTabWnd, 0, m_iDistanceFromTop, rectTabCtrl.Width(), rectTabCtrl.bottom, TRUE );

		BOOL bResult = (BOOL)Default();          // Process messages

		// If propertysheet m_bNoTabs = TRUE hide the tabs
		if( m_bNoTabs )
		{
			::ShowWindow(hTabWnd, SW_HIDE);
			::EnableWindow(hTabWnd, FALSE);
		}

		// change tab style if scrolling tabs desired (stacked tabs are default)
		if (!m_bStacked)
		{
			hTabWnd = (HWND)SendMessage(PSM_GETTABCONTROL);
			if (hTabWnd != NULL)
			{
				CWnd::ModifyStyle(hTabWnd, TCS_MULTILINE, TCS_SINGLELINE, 0);
			}
		}

		// remove standard buttons 
		for (int i = 0; i < 4; i++)
		{
			HWND hWnd = ::GetDlgItem(m_hWnd, module_piButtons[i]);
			if (hWnd != NULL)
			{
				::ShowWindow(hWnd, SW_HIDE);
				::EnableWindow(hWnd, FALSE);
			}
		}

		// Move the propertysheet to specified position
		MoveWindow( m_Rect.left, m_Rect.top, m_Rect.right, m_Rect.bottom /*- cyDiff*/ );

		return bResult;
	}

	return CPropertySheet::OnInitDialog();
}

BEGIN_MESSAGE_MAP(CsheetWithTab, CPropertySheet)
END_MESSAGE_MAP()


// CsheetWithTab message handlers
HBRUSH CsheetWithTab::CtlColor(CDC* pDc, UINT uCtlColor) 
{
	CWnd*    pwndParent;   // Parent window to property-sheet
	CDC*     pdcParent;    // Parent dc
	COLORREF color;        // color on parent dc           

	pwndParent = GetParent();
	ASSERT( pwndParent->IsKindOf( RUNTIME_CLASS( CWnd ) ) ); 

	pdcParent = pwndParent->GetDC();

	color = pdcParent->GetBkColor();    // Get parent color

	pwndParent->ReleaseDC( pdcParent );

	CBrush brush( color );              // Make a brush

	return (HBRUSH)brush;
}

void CsheetWithTab::ActiveMobSchedulePage ( CGLLIST<SMOBACTION> *pMobActList )
{
	SetActivePage ( (int)MOBSCHEDULEPAGE );
	m_MobSchedulePage.SetSchedule ( pMobActList );
}

void CsheetWithTab::ActiveCrowTreePage ( int CallPage, SNATIVEID NativeID )
{
	SetActivePage ( (int)CROWTREEPAGE );
	m_CrowTreePage.SetPropertyData ( CallPage, NativeID );
}

void CsheetWithTab::ActiveItemTreePage ( int CallPage, SNATIVEID NativeID )
{
	SetActivePage ( (int)ITEMTREEPAGE );
	m_ItemTreePage.SetPropertyData ( CallPage, NativeID );
}

void CsheetWithTab::ActiveSkillTreePage ( int CallPage, SNATIVEID NativeID )
{
	SetActivePage ( (int)SKILLTREEPAGE );
	m_SkillTreePage.SetPropertyData ( CallPage, NativeID );
}

void CsheetWithTab::ActiveGateListPage ( int CallPage, DWORD dwGateID )
{
	SetActivePage ( (int)GATELISTPAGE );
	m_GateListPage.SetPropertyData ( CallPage, dwGateID );
}

void CsheetWithTab::UpdatePage()
{
	m_MainPage.UpdatePage();
	m_MobSetPage.UpdatePage();
	//m_MobSchedulePage.UpdatePage();
	m_LandGatePage.UpdateContrl();
	m_RequirePage.UpdatePage();
	m_GateListPage.UpdateTreeItem();
	m_LandMarkPage.UpdateContrl();
	m_EtcFuncPage.UpdatePage();
}
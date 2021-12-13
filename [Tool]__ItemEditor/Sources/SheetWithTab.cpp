#include "pch.h"
#include <vector>
#include "resource.h"
#include "ItemEdit.h"
#include "SheetWithTab.h"
#include "SearchDialog.h"
#include "ItemEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString g_strEditSheet[EDITSHEET] =
{
	"ItemTree",						//	0
	"ItemParent",					//	1
	"ItemSuit",						//	2
};

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

IMPLEMENT_DYNAMIC(CsheetWithTab, CPropertySheet)

CsheetWithTab::CsheetWithTab(CRect& rect, LOGFONT logfont, CWnd* pParentWnd)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pParentWnd),
	 m_bNoTabs(FALSE),
	 m_pWndParent(NULL),
	 m_iDistanceFromTop(0),
	 m_ItemSuitTreePage ( logfont ),
	 m_ItemSuitPage ( logfont ),
	 m_ItemParentPage ( logfont ),
	 m_pFont ( NULL )
{
	m_pWndParent = pParentWnd;
	m_Rect = rect;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

	m_ItemSuitTreePage.SetSheetTab ( this );		//0
	AddPage ( &m_ItemSuitTreePage );

	m_ItemParentPage.SetSheetTab ( this );			//1
	AddPage ( &m_ItemParentPage );

	m_ItemSuitPage.SetSheetTab ( this );			//2
	AddPage ( &m_ItemSuitPage );

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CsheetWithTab::~CsheetWithTab()
{
	SAFE_DELETE( m_pFont );
}

static int module_piButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };

BOOL CsheetWithTab::OnInitDialog() 
{
	HWND hTabWnd;
	CRect rectTabCtrl;

	// PropertyPage 폰트 강제 변경
	ChangeDialogFont( this, m_pFont, CDF_NONE );

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
		//rectTabCtrl.bottom = 32; // Height for tabs
		rectTabCtrl.bottom = 0;

		// move tab control
		//::MoveWindow( hTabWnd, 0, 9, rectTabCtrl.Width(), rectTabCtrl.bottom, TRUE );
		::MoveWindow(hTabWnd, 0, 0, rectTabCtrl.Width(), rectTabCtrl.bottom, TRUE);

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
	//{{AFX_MSG_MAP(CsheetWithTab)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



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

BOOL CsheetWithTab::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch ( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			if ( pMsg->wParam == VK_ESCAPE )
			{
				if ( GetActivePage() != &m_ItemSuitTreePage )
				{
					m_ItemParentPage.InitAllItem();
					m_ItemSuitPage.InitAllItem  ();

					ActiveItemSuitTreePage ();
				}
			}
		}
		break;
	}

	return CPropertySheet::PreTranslateMessage(pMsg);
}

void CsheetWithTab::ActiveItemSuitTreePage ()
{		
	CItemEditDlg* pDlg = (CItemEditDlg*)m_pWndParent;
	pDlg->EnableMenu ( ID_MENUITEM_NEW,  TRUE );	
	pDlg->EnableMenu ( ID_MENUITEM_SAVE, TRUE );	
	pDlg->EnableMenu ( ID_MENUITEM_LOAD, TRUE );

	pDlg->EnableToobar( ID_MENUITEM_NEW,  TRUE );	
	pDlg->EnableToobar( ID_MENUITEM_SAVE, TRUE );
	pDlg->EnableToobar( ID_MENUITEM_LOAD, TRUE );

	pDlg->SetReadMode();

	m_ItemSuitTreePage.UpdateName ();
	SetActivePage ( ITEMSUITTREEPAGE );
}

void CsheetWithTab::ActiveItemParentPage ( int CurPage, SITEM* pItem )
{
	CItemEditDlg* pDlg = (CItemEditDlg*)m_pWndParent;
	pDlg->EnableMenu ( ID_MENUITEM_NEW,  FALSE );
	pDlg->EnableMenu ( ID_MENUITEM_LOAD, FALSE );

	pDlg->EnableToobar( ID_MENUITEM_NEW,  FALSE );	
	pDlg->EnableToobar( ID_MENUITEM_LOAD, FALSE );

	pDlg->SetReadMode();

	m_ItemParentPage.SetCallPage ( CurPage );
	m_ItemParentPage.SetItem ( pItem );
	SetActivePage ( ITEMPARENTPAGE );
}

void CsheetWithTab::ActiveItemSuitPage ( int CurPage, SITEM* pItem )
{
	CItemEditDlg* pDlg = (CItemEditDlg*)m_pWndParent;
	pDlg->EnableMenu ( ID_MENUITEM_NEW,  FALSE );
	pDlg->EnableMenu ( ID_MENUITEM_LOAD, FALSE );

	pDlg->EnableToobar( ID_MENUITEM_NEW,  FALSE );	
	pDlg->EnableToobar( ID_MENUITEM_LOAD, FALSE );

	m_ItemSuitPage.SetCallPage ( CurPage );
	m_ItemSuitPage.SetItem ( pItem );
	SetActivePage ( ITEMSUITPAGE );
}

void CsheetWithTab::SearchItemSelect( BYTE dwType, char *szItemName )
{
	m_ItemSuitTreePage.SearchItemSelect( dwType, szItemName );
}

void CsheetWithTab::UpdateSuitTreePage()
{
	m_ItemSuitTreePage.UpdateTree();
}

void CsheetWithTab::ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag)
{
	CRect windowRect;

	// grab old and new text metrics
	TEXTMETRIC tmOld, tmNew;
	CDC * pDC = pWnd->GetDC();
	CFont * pSavedFont = pDC->SelectObject(pWnd->GetFont());
	pDC->GetTextMetrics(&tmOld);
	pDC->SelectObject(pFont);
	pDC->GetTextMetrics(&tmNew);
	pDC->SelectObject(pSavedFont);
	pWnd->ReleaseDC(pDC);

	CRect clientRect, newClientRect, newWindowRect;

	pWnd->GetWindowRect(windowRect);
	pWnd->GetClientRect(clientRect);

	double fHeigth = (double)(m_Rect.bottom-m_Rect.top) / clientRect.bottom;
	double fWidth = (double) (m_Rect.right-m_Rect.left) / clientRect.right;

	if (nFlag != CDF_NONE)
	{
		// calculate new dialog window rectangle

		long xDiff = windowRect.Width() - clientRect.Width();
		long yDiff = windowRect.Height() - clientRect.Height();
	
		newClientRect.left = newClientRect.top = 0;
		newClientRect.right = clientRect.right * (LONG)fWidth;//tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		newClientRect.bottom = clientRect.bottom * (LONG)fHeigth; //newHeight / oldHeight;

		if (nFlag == CDF_TOPLEFT) // resize with origin at top/left of window
		{
			newWindowRect.left = windowRect.left;
			newWindowRect.top = windowRect.top;
			newWindowRect.right = windowRect.left + newClientRect.right + xDiff;
			newWindowRect.bottom = windowRect.top + newClientRect.bottom + yDiff;
		}
		else if (nFlag == CDF_CENTER) // resize with origin at center of window
		{
			newWindowRect.left = windowRect.left - 
							(newClientRect.right - clientRect.right)/2;
			newWindowRect.top = windowRect.top -
							(newClientRect.bottom - clientRect.bottom)/2;
			newWindowRect.right = newWindowRect.left + newClientRect.right + xDiff;
			newWindowRect.bottom = newWindowRect.top + newClientRect.bottom + yDiff;
		}
		pWnd->MoveWindow(newWindowRect);
	}

	pWnd->SetFont(pFont);

	// iterate through and move all child windows and change their font.
	CWnd* pChildWnd = pWnd->GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		pChildWnd->SetFont(pFont);
		pChildWnd->GetWindowRect(windowRect);

		CString strClass;
		::GetClassName(pChildWnd->m_hWnd, strClass.GetBufferSetLength(32), 31);
		strClass.MakeUpper();
		if(strClass==_T("COMBOBOX"))
		{
			CRect rect;
			pChildWnd->SendMessage(CB_GETDROPPEDCONTROLRECT,0,(LPARAM) &rect);
			windowRect.right = rect.right;
			windowRect.bottom = rect.bottom;
		}

		pWnd->ScreenToClient(windowRect);
		windowRect.left = windowRect.left * (LONG)fWidth; //tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.right = windowRect.right * (LONG)fWidth; //tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.top = windowRect.top * (LONG)fHeigth; //newHeight / oldHeight;
		windowRect.bottom = windowRect.bottom * (LONG)fHeigth; //newHeight / oldHeight;
		pChildWnd->MoveWindow(windowRect);
		
		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
}
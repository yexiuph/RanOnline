// DlgLevelEdit.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "DlgLevelEdit.h"
#include "MobSetPage.h"

#include "SheetWithTab.h"
#include ".\dlgleveledit.h"


// CDlgLevelEdit dialog

IMPLEMENT_DYNAMIC(CDlgLevelEdit, CDialog)
CDlgLevelEdit::CDlgLevelEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLevelEdit::IDD, pParent)
	, m_pSheetTab(NULL)
	, m_bINIT(FALSE)
{
}

CDlgLevelEdit::~CDlgLevelEdit()
{
}

void CDlgLevelEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PAGE, m_comboPage);
}

BEGIN_MESSAGE_MAP(CDlgLevelEdit, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE, OnCbnSelchangeComboPage)
END_MESSAGE_MAP()


// CDlgLevelEdit message handlers
BOOL CDlgLevelEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect SheetRect;
	CWnd *pWnd;

	//	Note : Sheet 1 를 만든다.
	//
	pWnd = GetDlgItem ( IDC_STATIC_CONTROL );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	m_pSheetTab = new CsheetWithTab ( SheetRect, this );
	m_pSheetTab->Create ( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->SetActivePage ( (int)MAINPAGE );

	int nIndex;

	nIndex = m_comboPage.AddString ( "Main Setting" );
	m_comboPage.SetItemData ( nIndex, (DWORD_PTR)0 );

	nIndex = m_comboPage.AddString ( "Mob Setting" );
	m_comboPage.SetItemData ( nIndex, (DWORD_PTR)1 );

	nIndex = m_comboPage.AddString ( "Material Setting" );
	m_comboPage.SetItemData ( nIndex, (DWORD_PTR)1 );

	nIndex = m_comboPage.AddString ( "Land Gate Setting" );
	m_comboPage.SetItemData ( nIndex, (DWORD_PTR)3 );

	nIndex = m_comboPage.AddString ( "Require Setting" );
	m_comboPage.SetItemData ( nIndex, (DWORD_PTR)5 );

	nIndex = m_comboPage.AddString ( "Effect Setting" );
	m_comboPage.SetItemData ( nIndex, (DWORD_PTR)9 );

	nIndex = m_comboPage.AddString ( "Local Name Setting" );
	m_comboPage.SetItemData ( nIndex, (DWORD_PTR)10 );

	nIndex = m_comboPage.AddString ( "Etc Function Setting" );
	m_comboPage.SetItemData ( nIndex, (DWORD_PTR)11 );

	m_comboPage.SetCurSel ( 0 );

	m_bINIT = TRUE;

	return TRUE;
}

void CDlgLevelEdit::PostNcDestroy()
{
	SAFE_DELETE ( m_pSheetTab );

	CDialog::PostNcDestroy();
}

void CDlgLevelEdit::UpdatePage ( BOOL bMainPage )
{
	if ( m_bINIT )
	{
		m_pSheetTab->UpdatePage();

		if ( bMainPage )
			m_pSheetTab->SetActivePage(0);

		m_comboPage.SetCurSel(0);
	}
}

void CDlgLevelEdit::OnCbnSelchangeComboPage()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_comboPage.GetCurSel();

	if ( nIndex != -1 )
	{
		CString strIndex;

		m_comboPage.GetLBText ( nIndex, strIndex );
		int nPage = (int)m_comboPage.GetItemData ( nIndex );
		
		m_pSheetTab->SetActivePage ( nPage );

		if ( strIndex == "Mob Setting" )
		{
			m_pSheetTab->m_MobSetPage.m_bMaterial = FALSE;
			m_pSheetTab->m_MobSetPage.ActiveMobSetEdit ( FALSE );
		}
		else if ( strIndex == "Material Setting" )
		{
			m_pSheetTab->m_MobSetPage.m_bMaterial = TRUE;
			m_pSheetTab->m_MobSetPage.ActiveMobSetEdit ( FALSE );
		}
	}
}

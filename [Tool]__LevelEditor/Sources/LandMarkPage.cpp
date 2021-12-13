// LandMarkPage.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "LandMarkPage.h"
#include "LevelEditorView.h"
#include "MainFrm.h"
#include "SheetWithTab.h"

#include "EtcFunction.h"

#include "DxViewPort.h"

#include "GLLandMark.h"
#include "GLGaeaServer.h"
#include "GLLandMan.h"
#include ".\landmarkpage.h"


// CLandMarkPage dialog

IMPLEMENT_DYNAMIC(CLandMarkPage, CPropertyPage)
CLandMarkPage::CLandMarkPage()
	: CPropertyPage(CLandMarkPage::IDD)
	, m_pSheetTab ( NULL )
	, m_bDlgInit ( FALSE )
	, m_pLandMark ( NULL )
{
}

CLandMarkPage::~CLandMarkPage()
{
}

void CLandMarkPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LAND_MARK, m_listLandMark);
}


BEGIN_MESSAGE_MAP(CLandMarkPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_LAND_MARK_NEW, OnBnClickedButtonLandMarkNew)
	ON_BN_CLICKED(IDC_BUTTON_LAND_MARK_EDIT, OnBnClickedButtonLandMarkEdit)
	ON_BN_CLICKED(IDC_BUTTON_LAND_MARK_DEL, OnBnClickedButtonLandMarkDel)
	ON_BN_CLICKED(IDC_BUTTON_LAND_MARK_REGIS, OnBnClickedButtonLandMarkRegis)
END_MESSAGE_MAP()


// CLandMarkPage message handlers
BOOL CLandMarkPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bDlgInit = TRUE;

	ViewContrls ( FALSE, FALSE );
	UpdateContrl ();

	return TRUE;
}

BOOL CLandMarkPage::OnKillActive()
{
	m_pLandMark = NULL;

	ViewContrls ( FALSE, FALSE );
	UpdateContrl ();

	return CPropertyPage::OnKillActive();
}

void CLandMarkPage::ListtingLandMark()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	CString strOldSelect;

	if ( m_listLandMark.GetCurSel() == -1 )
		m_listLandMark.GetDlgItemText ( -1, strOldSelect );

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( !pGLLandMan )
		return;

	PLANDMARK pCur = pGLLandMan->GetLandMarkList();

	while ( pCur )
	{
		int nAdd = m_listLandMark.AddString ( pCur->m_strMARK.c_str() );
		m_listLandMark.SetItemData ( nAdd, (DWORD_PTR)pCur );

		pCur = pCur->m_pNext;
	}

	m_listLandMark.SelectString ( 0, strOldSelect );
}

void CLandMarkPage::ViewContrls ( BOOL bView, BOOL bClear )
{
	if ( !m_bDlgInit )
		return;

	SetWin_Enable ( this, IDC_BUTTON_LAND_MARK_NEW, bView == FALSE );
	SetWin_Enable ( this, IDC_BUTTON_LAND_MARK_EDIT, bView == FALSE );
	SetWin_Enable ( this, IDC_BUTTON_LAND_MARK_DEL, bView == FALSE );

	SetWin_ShowWindow ( this, IDC_EDIT_LAND_MARK_NAME, bView );
	SetWin_ShowWindow ( this, IDC_BUTTON_LAND_MARK_REGIS, bView );

	if ( bClear )
	{
		SetWin_Text ( this, IDC_EDIT_LAND_MARK_NAME, "" );
	}
	else
	{
		if ( m_pLandMark )
		{
			SetWin_Text ( this, IDC_EDIT_LAND_MARK_NAME, m_pLandMark->m_strMARK.c_str() );
		}
	}
}

void CLandMarkPage::UpdateContrl ()
{
	if ( !m_bDlgInit )
		return;

	m_pLandMark = NULL;

	ViewContrls ( FALSE, FALSE );
	ListtingLandMark();

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
	pView->SetCurType ( CUR_SELECT );
}

void CLandMarkPage::OnBnClickedButtonLandMarkNew()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( !pGLLandMan )
		return;

	PLANDMARK pLandMark = new GLLANDMARK;

	char szNewName[128] = {0};
	int i = 0;

	do
	{
		sprintf_s ( szNewName, "%s[%d]", "Zone", i++ );
	}
	while ( pGLLandMan->FindLandMark ( szNewName ) );

	pLandMark->m_strMARK = szNewName;

	D3DXVECTOR3* vCollPos = pView->GetCollisionPos ();
	DXAFFINEPARTS sAffine;

	sAffine.vTrans = *vCollPos;
	pLandMark->SetAffineValue ( &sAffine );

	m_pLandMark = pLandMark;
	pView->ActiveEditMatrix ( m_pLandMark );

	pGLLandMan->AddLandMark ( m_pLandMark );
	pGLLandMan->BuildLandMarkTree();

	ViewContrls ( FALSE, FALSE );
	ListtingLandMark();

	DxViewPort::GetInstance().CameraJump ( sAffine.vTrans );
}

void CLandMarkPage::OnBnClickedButtonLandMarkEdit()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	int nIndex = m_listLandMark.GetCurSel();

	if ( nIndex != -1 )
	{
		m_pLandMark = (PLANDMARK)m_listLandMark.GetItemData ( nIndex );

		pView->ActiveEditMatrix ( m_pLandMark );
		ViewContrls ( TRUE, FALSE );

		DxViewPort::GetInstance().CameraJump ( m_pLandMark->m_pAffineParts->vTrans );
	}
}

void CLandMarkPage::OnBnClickedButtonLandMarkDel()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( !pGLLandMan )
		return;

	int nIndex = m_listLandMark.GetCurSel();

	if ( nIndex != -1 )
	{
		PLANDMARK pLandMark = (PLANDMARK)m_listLandMark.GetItemData ( nIndex );

		pGLLandMan->DelLandMark ( pLandMark );
		pGLLandMan->BuildLandMarkTree();

		if ( pLandMark == m_pLandMark )
		{
			m_pLandMark = NULL;

			ViewContrls ( FALSE, FALSE );
			UpdateContrl ();
		}
		else
		{
			ListtingLandMark();
		}
	}
}

void CLandMarkPage::OnBnClickedButtonLandMarkRegis()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( !pGLLandMan )
		return;

	CString strMARK = GetWin_Text ( this, IDC_EDIT_LAND_MARK_NAME );

	if ( strMARK.IsEmpty() )
	{
		MessageBox ( "It did not set a name.", "Caution" );
	}
	else
	{
		m_pLandMark->m_strMARK = strMARK.GetString();
		m_pLandMark->CALC_BOX();

		pGLLandMan->BuildLandMarkTree();

		ListtingLandMark();

		m_pLandMark = NULL;

		ViewContrls ( FALSE, FALSE );
		UpdateContrl ();
	}
}

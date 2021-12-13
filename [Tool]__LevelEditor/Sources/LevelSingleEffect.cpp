// LevelSingleEffect.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "LevelSingleEffect.h"
#include "LevelEditorView.h"
#include "MainFrm.h"
#include "SheetWithTab.h"

#include "EtcFunction.h"

#include "DxViewPort.h"
#include "DXLANDEFF.h"
#include "DxEffSinglePropGMan.h"

#include "GLGaeaServer.h"
#include "GLLandMan.h"
#include ".\levelsingleeffect.h"

// CLevelSingleEffect dialog

IMPLEMENT_DYNAMIC(CLevelSingleEffect, CPropertyPage)
CLevelSingleEffect::CLevelSingleEffect()
	: CPropertyPage(CLevelSingleEffect::IDD)
	, m_bDlgInit ( FALSE )
	, m_pLandEff ( NULL )
{
}

CLevelSingleEffect::~CLevelSingleEffect()
{
}

void CLevelSingleEffect::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SINGLE_EFF, m_EffListBox);
}


BEGIN_MESSAGE_MAP(CLevelSingleEffect, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_EFF_NEW, OnBnClickedButtonSingleEffNew)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_EFF_EDIT, OnBnClickedButtonSingleEffEdit)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_EFF_DEL, OnBnClickedButtonSingleEffDel)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_EFF_FIND, OnBnClickedButtonSingleEffFind)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_EFF_APPLY, OnBnClickedButtonSingleEffApply)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_EFF_REGIS, OnBnClickedButtonSingleEffRegis)
END_MESSAGE_MAP()


// CLevelSingleEffect message handlers
BOOL CLevelSingleEffect::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bDlgInit = TRUE;
	UpdateContrl();

	return TRUE;
}

BOOL CLevelSingleEffect::OnKillActive()
{
	m_pLandEff = NULL;

	ViewContrls ( FALSE, FALSE );
	UpdateContrl();

	return CPropertyPage::OnKillActive();
}

void CLevelSingleEffect::ListtingLandEff()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	CString strOldSelect;

	if ( m_EffListBox.GetCurSel() == -1 )
		m_EffListBox.GetDlgItemText ( -1, strOldSelect );

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( !pGLLandMan )
		return;

	PLANDEFF pCur = pGLLandMan->GetLandEffList();

	while ( pCur )
	{
		int nIndex = m_EffListBox.AddString ( pCur->m_szName );
		m_EffListBox.SetItemData ( nIndex, (DWORD_PTR)pCur );

		pCur = pCur->m_pNext;
	}

	m_EffListBox.SelectString ( 0, strOldSelect );
}

void CLevelSingleEffect::ViewContrls ( BOOL bView, BOOL bClear )
{
	if ( !m_bDlgInit )
		return;

	SetWin_ShowWindow ( this, IDC_EDIT_SINGLE_EFF_NAME, bView );

	SetWin_ShowWindow ( this, IDC_EDIT_SINGLE_EFF_FNAME, bView );
	SetWin_ShowWindow ( this, IDC_BUTTON_SINGLE_EFF_FIND, bView );

	SetWin_ShowWindow ( this, IDC_BUTTON_SINGLE_EFF_APPLY, bView );
	SetWin_ShowWindow ( this, IDC_BUTTON_SINGLE_EFF_REGIS, bView );

	if ( bClear )
	{
		SetWin_Text ( this, IDC_EDIT_SINGLE_EFF_NAME, "" );
		SetWin_Text ( this, IDC_EDIT_SINGLE_EFF_FNAME, "" );
	}
	else
	{
		if ( m_pLandEff )
		{
			SetWin_Text ( this, IDC_EDIT_SINGLE_EFF_NAME, m_pLandEff->m_szName );
			SetWin_Text ( this, IDC_EDIT_SINGLE_EFF_FNAME, m_pLandEff->m_szFileName );
		}
	}
}

void CLevelSingleEffect::UpdateContrl ()
{
	if ( !m_bDlgInit )
		return;

	m_pLandEff = NULL;

	ViewContrls ( FALSE, FALSE );
	ListtingLandEff();

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
	pView->DeActiveEditMatrix();
}

void CLevelSingleEffect::OnBnClickedButtonSingleEffNew()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( !pGLLandMan )
		return;

	PLANDEFF pLandEff = new DXLANDEFF;

	int i = 0;
	char szNewName[DXLANDEFF::LANDEFF_MAXSZ] = {0};

	do
	{
		sprintf_s ( szNewName, "%s[%d]", pLandEff->m_szName, i++ );
	}
	while ( pGLLandMan->FindLandEff ( szNewName ) );

	strcpy_s ( pLandEff->m_szName, szNewName );

	D3DXVECTOR3* vCollPos = pView->GetCollisionPos ();
	pLandEff->SetPosition ( *vCollPos );

	m_pLandEff = pLandEff;
	pView->ActiveEditMatrix2 ( &m_pLandEff->m_matWorld );

	pGLLandMan->AddLandEff ( m_pLandEff );
	pGLLandMan->BuildSingleEffTree();

	m_EffListBox.SelectString ( 0, m_pLandEff->m_szName );

	ViewContrls ( TRUE, FALSE );
	ListtingLandEff();
}

void CLevelSingleEffect::OnBnClickedButtonSingleEffEdit()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	int nIndex = m_EffListBox.GetCurSel();

	if ( nIndex != -1 )
	{
		m_pLandEff = (PLANDEFF)m_EffListBox.GetItemData( nIndex );
		pView->ActiveEditMatrix2 ( &m_pLandEff->m_matWorld );

		ViewContrls ( TRUE, FALSE );
		D3DXVECTOR3 vPos = D3DXVECTOR3 ( m_pLandEff->m_matWorld._41, m_pLandEff->m_matWorld._42, m_pLandEff->m_matWorld._43 );

		DxViewPort::GetInstance().CameraJump ( vPos );
	}
}

void CLevelSingleEffect::OnBnClickedButtonSingleEffDel()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( !pGLLandMan )
		return;

	int nIndex = m_EffListBox.GetCurSel();

	if ( nIndex != -1 )
	{
		PLANDEFF pLandEff = (PLANDEFF)m_EffListBox.GetItemData( nIndex );
		pGLLandMan->DelLandEff ( pLandEff );
		pGLLandMan->BuildSingleEffTree();

		if ( pLandEff == m_pLandEff )
		{
			m_pLandEff = NULL;

			ViewContrls ( FALSE, FALSE );
			UpdateContrl();
		}
		else
		{
			ListtingLandEff();
		}
	}
}

void CLevelSingleEffect::OnBnClickedButtonSingleEffFind()
{
	// TODO: Add your control notification handler code here
	CString szFilter = "Effect Group File (*.egp)|*.egp|";

	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CLevelSingleEffect*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath();

	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_SINGLE_EFF_FNAME, dlg.GetFileName().GetString() );
	}
}

void CLevelSingleEffect::OnBnClickedButtonSingleEffApply()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( !pGLLandMan )
		return;

	char szName[DXLANDEFF::LANDEFF_MAXSZ] = {0};
	char szFileName[MAX_PATH] = {0};

	strcpy_s ( szName, GetWin_Text ( this, IDC_EDIT_SINGLE_EFF_NAME ) );
	strcpy_s ( szFileName, GetWin_Text ( this, IDC_EDIT_SINGLE_EFF_FNAME ) );

	PLANDEFF pCur = pGLLandMan->GetLandEffList();

	while ( pCur )
	{
		if ( !strcmp ( pCur->m_szName, szName ) && pCur != m_pLandEff )
		{
			MessageBox ( "ERROR : Same name is already registered.", "Caution" );
			return;
		}

		pCur = pCur->m_pNext;
	}

	if ( szFileName == "" )
	{
		MessageBox ( "ERROR : Effect file is not selected.", "Caution" );
		return;
	}

	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice();

	HRESULT hr = m_pLandEff->SetEffect ( szName, m_pLandEff->m_matWorld, szFileName, pd3dDevice );
	if ( FAILED(hr) ) 
	{
		MessageBox ( "FAILURE : Object can not be initialized with fixed value.", "Caution" );
		return;
	}

	pGLLandMan->BuildSingleEffTree();
	m_EffListBox.SelectString ( 0, m_pLandEff->m_szName );
	ListtingLandEff();
}

void CLevelSingleEffect::OnBnClickedButtonSingleEffRegis()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( !pGLLandMan )
		return;

	char szName[DXLANDEFF::LANDEFF_MAXSZ] = {0};
	char szFileName[MAX_PATH] = {0};

	strcpy_s ( szName, GetWin_Text ( this, IDC_EDIT_SINGLE_EFF_NAME ) );
	strcpy_s ( szFileName, GetWin_Text ( this, IDC_EDIT_SINGLE_EFF_FNAME ) );

	PLANDEFF pCur = pGLLandMan->GetLandEffList();

	while ( pCur )
	{
		if ( !strcmp ( pCur->m_szName, szName ) && pCur != m_pLandEff )
		{
			MessageBox ( "ERROR : Same name is already registered.", "Caution" );
			return;
		}

		pCur = pCur->m_pNext;
	}

	if ( szFileName == "" )
	{
		MessageBox ( "ERROR : Effect file is not selected.", "Caution" );
		return;
	}

	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice();

	HRESULT hr = m_pLandEff->SetEffect ( szName, m_pLandEff->m_matWorld, szFileName, pd3dDevice );
	if ( FAILED(hr) ) 
	{
		MessageBox ( "FAILURE : Object can not be initialized with fixed value.", "Caution" );
		return;
	}

	pGLLandMan->BuildSingleEffTree();
	m_EffListBox.SelectString ( 0, m_pLandEff->m_szName );
	ListtingLandEff();

	m_pLandEff = NULL;

	ViewContrls ( FALSE, FALSE );
	UpdateContrl();
}

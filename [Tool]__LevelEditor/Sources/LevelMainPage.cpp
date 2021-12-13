// LevelMainPage.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "LevelMainPage.h"
#include "LevelEditorView.h"
#include "MainFrm.h"
#include "SheetWithTab.h"

#include "EtcFunction.h"

#include "DxLandMan.h"

#include "GLGaeaServer.h"
#include "GLLandMan.h"
#include "GLLevelFile.h"
#include "GLMobSchedule.h"
#include ".\levelmainpage.h"

DWORD dwDIV[10] =
{
	0x002,
	0x004,
	0x008,
	0x010,
	0x020,
	0x040,
	0x080,
	0x100,
	0x200,
	0x400
};

// CLevelMainPage dialog

IMPLEMENT_DYNAMIC(CLevelMainPage, CPropertyPage)
CLevelMainPage::CLevelMainPage()
	: CPropertyPage(CLevelMainPage::IDD)
	, m_bINIT (FALSE)
{
}

CLevelMainPage::~CLevelMainPage()
{
}

void CLevelMainPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DIV, m_comboDiv);
}


BEGIN_MESSAGE_MAP(CLevelMainPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_WLDFILE, OnBnClickedButtonWldfile)
	ON_CBN_SELCHANGE(IDC_COMBO_DIV, OnCbnSelchangeComboDiv)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_QTREE, OnBnClickedButtonCreateQtree)
	ON_BN_CLICKED(IDC_CHECK_RENDER_QTREE, OnBnClickedCheckRenderQtree)
	ON_BN_CLICKED(IDC_CHECK_SHOW_CROWSCHDL, OnBnClickedCheckShowCrowschdl)
END_MESSAGE_MAP()


// CLevelMainPage message handlers
BOOL CLevelMainPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strTemp;
	strTemp = "untitle.";
	strTemp += GLLevelFile::FILE_EXT;

	SetWin_Text ( this, IDC_EDIT_FNAME, strTemp );

	for ( int i = 0; i < 10; ++i )
	{
		CString strNUM;
		strNUM.Format ( "%d", i+1 );
		int nIndex = m_comboDiv.AddString ( strNUM );
		m_comboDiv.SetItemData ( nIndex, (DWORD_PTR)dwDIV[i] );
	}

	int nIndx = (int)( log10(16.0f) / log10(2.0f) ) - 1;
	m_comboDiv.SetCurSel ( nIndx );

	strTemp.Format ( "%d", dwDIV[nIndx] * dwDIV[nIndx] );
	SetWin_Text ( this, IDC_EDIT_DIV, strTemp );

	SetWin_Check ( this, IDC_CHECK_RENDER_QTREE, CLevelEditorView::m_bRENDQUAD );

	GLMobSchedule::m_bRENDACT = TRUE;
	SetWin_Check ( this, IDC_CHECK_SHOW_CROWSCHDL, GLMobSchedule::m_bRENDACT );

	m_bINIT = TRUE;

	return TRUE;
}

void CLevelMainPage::UpdatePage ()
{
	if ( m_bINIT )
	{
		CString strTemp;
		GLLandMan *pLandMan = GLGaeaServer::GetInstance().GetRootMap();

		SetWin_Text ( this, IDC_EDIT_FNAME, pLandMan->GetFileName() );
		SetWin_Text ( this, IDC_EDIT_WLDFILE, pLandMan->GetWldFileName() );

		int nIndex = (int)( log10((float)pLandMan->GetLandTreeDiv()) / log10(2.0f) ) - 1;
		m_comboDiv.SetCurSel ( nIndex );

		strTemp.Format ( "%d", dwDIV[nIndex] * dwDIV[nIndex] );
		SetWin_Text ( this, IDC_EDIT_DIV, strTemp );
	}
}

void CLevelMainPage::OnBnClickedButtonWldfile()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame* pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView* pView = (CLevelEditorView *)pFrame->GetActiveView();

	CString szFilter = "WLD File (*.wld)|*.wld|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".wld",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CLevelMainPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxLandMan::GetPath();

	if ( dlg.DoModal() == IDOK )
	{
		GLGaeaServer::GetInstance().ClearDropObj();
		CLevelEditorView::m_LandMan.CleanUp();

		LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice();

		BOOL bOK;
		bOK = CLevelEditorView::m_LandMan.LoadFile ( dlg.GetFileName(), pd3dDevice );
		if ( bOK )
		{
			CLevelEditorView::m_LandMan.ActiveMap();
			GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

			BOOL bOk = pGLLandMan->LoadWldFile ( dlg.GetFileName(), bOK );

			SetWin_Text ( this, IDC_EDIT_WLDFILE, dlg.GetFileName() );
		}
	}
}

void CLevelMainPage::OnCbnSelchangeComboDiv()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_comboDiv.GetCurSel();

	if ( nIndex != -1 )
	{
		DWORD dwData = (DWORD)m_comboDiv.GetItemData ( nIndex );
		
		CString strTemp;

		GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
		pGLLandMan->SetLandTreeDiv ( (EMDIVISION)dwData );

		strTemp.Format ( "%d", dwData * dwData );
		SetWin_Text ( this, IDC_EDIT_DIV, strTemp );
	}
}

void CLevelMainPage::OnBnClickedButtonCreateQtree()
{
	// TODO: Add your control notification handler code here
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	GLGaeaServer::GetInstance().ClearDropObj();

	pGLLandMan->SetMap();
}

void CLevelMainPage::OnBnClickedCheckRenderQtree()
{
	// TODO: Add your control notification handler code here
	CLevelEditorView::m_bRENDQUAD = GetWin_Check ( this, IDC_CHECK_RENDER_QTREE );
}

void CLevelMainPage::OnBnClickedCheckShowCrowschdl()
{
	// TODO: Add your control notification handler code here
	GLMobSchedule::m_bRENDACT = GetWin_Check ( this, IDC_CHECK_SHOW_CROWSCHDL );
}

// LevelLandGatePage.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "LevelLandGatePage.h"
#include "LevelEditorView.h"
#include "MainFrm.h"
#include "SheetWithTab.h"

#include "EtcFunction.h"

#include "DxViewPort.h"
#include "DxEditBox.h"
#include "DxLandGateMan.h"

#include "GLGaeaServer.h"
#include "GLLandMan.h"
#include ".\levellandgatepage.h"

// CLevelLandGatePage dialog

IMPLEMENT_DYNAMIC(CLevelLandGatePage, CPropertyPage)
CLevelLandGatePage::CLevelLandGatePage()
	: CPropertyPage(CLevelLandGatePage::IDD)
	, m_bDlgInit (FALSE)
	, m_pLandGate (NULL)
{
}

CLevelLandGatePage::~CLevelLandGatePage()
{
}

void CLevelLandGatePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LAND_GATE, m_listLandGate);
}


BEGIN_MESSAGE_MAP(CLevelLandGatePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_LAND_GATE_NEW, OnBnClickedButtonLandGateNew)
	ON_BN_CLICKED(IDC_BUTTON_LAND_GATE_EDIT, OnBnClickedButtonLandGateEdit)
	ON_BN_CLICKED(IDC_BUTTON_LAND_GATE_DEL, OnBnClickedButtonLandGateDel)
	ON_BN_CLICKED(IDC_BUTTON_LAND_GATE_COPY, OnBnClickedButtonLandGateCopy)
	ON_BN_CLICKED(IDC_CHECK_LAND_GATE_REND, OnBnClickedCheckLandGateRend)
	ON_BN_CLICKED(IDC_BUTTON_LAND_GATE_APPLY, OnBnClickedButtonLandGateApply)
	ON_BN_CLICKED(IDC_BUTTON_LAND_GATE_REGIS, OnBnClickedButtonLandGateRegis)
END_MESSAGE_MAP()


// CLevelLandGatePage message handlers
BOOL CLevelLandGatePage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bDlgInit = TRUE;

	UpdateContrl();
	SetWin_Check ( this, IDC_CHECK_LAND_GATE_REND, DxLandGateMan::m_bREND );

	return TRUE;
}

void CLevelLandGatePage::ListtingLandGate()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	CString StrOldSelect;

	int nIndex = m_listLandGate.GetCurSel();

	if ( nIndex != -1 )
	{
		m_listLandGate.GetDlgItemText ( nIndex, StrOldSelect );
	}

	m_listLandGate.ResetContent();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( pGLLandMan )
	{
		DxLandGateMan* pLandGateMan = &pGLLandMan->GetLandGateMan();
		PDXLANDGATE pCur = pLandGateMan->GetListLandGate();

		while ( pCur )
		{
			int nIndx = m_listLandGate.AddString ( pCur->GetName() );
			m_listLandGate.SetItemData ( nIndx, (DWORD_PTR)pCur );

			pCur = pCur->m_pNext;
		}

		m_listLandGate.SelectString ( 0, StrOldSelect );
	}
}

void CLevelLandGatePage::ViewContrls ( BOOL bShow, BOOL bClear )
{
	if ( m_bDlgInit )
	{
		SetWin_Enable ( this, IDC_BUTTON_LAND_GATE_NEW, bShow == FALSE );
		SetWin_Enable ( this, IDC_BUTTON_LAND_GATE_EDIT, bShow == FALSE );
		SetWin_Enable ( this, IDC_BUTTON_LAND_GATE_DEL, bShow == FALSE );
		SetWin_Enable ( this, IDC_BUTTON_LAND_GATE_COPY, bShow == FALSE );

		SetWin_ShowWindow ( this, IDC_EDIT_LAND_GATE_NAME, bShow );

		SetWin_ShowWindow ( this, IDC_STATIC_ATTRB, bShow );
		SetWin_ShowWindow ( this, IDC_CHECK_LAND_GATE_OUT, bShow );
		SetWin_ShowWindow ( this, IDC_CHECK_LAND_GATE_IN, bShow );

		SetWin_ShowWindow ( this, IDC_STATIC_GATEID, bShow );
		SetWin_ShowWindow ( this, IDC_EDIT_LAND_GATE_ID, bShow );

		SetWin_ShowWindow ( this, IDC_STATIC_TOMAP_ID, bShow );
		SetWin_ShowWindow ( this, IDC_EDIT_LAND_GATE_TOMAP_MID, bShow );
		SetWin_ShowWindow ( this, IDC_EDIT_LAND_GATE_TOMAP_SID, bShow );

		SetWin_ShowWindow ( this, IDC_STATIC_TOGATE_ID, bShow );
		SetWin_ShowWindow ( this, IDC_EDIT_LAND_GATE_TOGATE_ID, bShow );

		SetWin_ShowWindow ( this, IDC_STATIC_GEN_INTER, bShow );
		SetWin_ShowWindow ( this, IDC_EDIT_LAND_GATE_W, bShow );
		SetWin_ShowWindow ( this, IDC_EDIT_LAND_GATE_H, bShow );

		SetWin_ShowWindow ( this, IDC_BUTTON_LAND_GATE_APPLY, bShow );
		SetWin_ShowWindow ( this, IDC_BUTTON_LAND_GATE_REGIS, bShow );

		if ( bClear )
		{
			SetWin_Text ( this, IDC_EDIT_LAND_GATE_NAME, "" );

			SetWin_Text ( this, IDC_EDIT_LAND_GATE_ID, "" );

			SetWin_Text ( this, IDC_EDIT_LAND_GATE_TOMAP_MID, "" );
			SetWin_Text ( this, IDC_EDIT_LAND_GATE_TOMAP_SID, "" );

			SetWin_Text ( this, IDC_EDIT_LAND_GATE_TOGATE_ID, "" );

			SetWin_Text ( this, IDC_EDIT_LAND_GATE_W, "" );
			SetWin_Text ( this, IDC_EDIT_LAND_GATE_H, "" );
		}
		else
		{
			if ( m_pLandGate )
			{
				CString strTemp;

				SetWin_Text ( this, IDC_EDIT_LAND_GATE_NAME, m_pLandGate->GetName() );

				SetWin_Check ( this, IDC_CHECK_LAND_GATE_OUT, m_pLandGate->GetFlags() & 2 );
				SetWin_Check ( this, IDC_CHECK_LAND_GATE_IN, m_pLandGate->GetFlags() & 1 );

				strTemp.Format ( "%d", m_pLandGate->GetGateID() );
				SetWin_Text ( this, IDC_EDIT_LAND_GATE_ID, strTemp.GetString() );

				strTemp.Format ( "%d", m_pLandGate->GetToMapID().wMainID );
				SetWin_Text ( this, IDC_EDIT_LAND_GATE_TOMAP_MID, strTemp.GetString() );
				strTemp.Format ( "%d", m_pLandGate->GetToMapID().wSubID );
				SetWin_Text ( this, IDC_EDIT_LAND_GATE_TOMAP_SID, strTemp.GetString() );

				strTemp.Format ( "%d", m_pLandGate->GetToGateID() );
				SetWin_Text ( this, IDC_EDIT_LAND_GATE_TOGATE_ID, strTemp.GetString() );

				char szValue[32] = {0};
				D3DXVECTOR2 vDiv = m_pLandGate->GetDiv();

				_gcvt_s ( szValue, vDiv.x, 7 );
				SetWin_Text ( this, IDC_EDIT_LAND_GATE_W, szValue );
				_gcvt_s ( szValue, vDiv.y, 7 );
				SetWin_Text ( this, IDC_EDIT_LAND_GATE_H, szValue );
			}
		}
	}
}

void CLevelLandGatePage::UpdateContrl()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	if ( m_bDlgInit )
	{
		m_pLandGate = NULL;
		ViewContrls ( FALSE, FALSE );
		ListtingLandGate();
		pView->SetCurType ( CUR_SELECT );
	}
}

void CLevelLandGatePage::OnBnClickedButtonLandGateNew()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( pGLLandMan )
	{
		DxLandGateMan* pLandGateMan = &pGLLandMan->GetLandGateMan();
		DxLandGate* pLandGate = new DxLandGate;

		char szNewName[65] = {0};
		int i = 0;
		do
		{
			sprintf_s ( szNewName, "%s[%d]", "NoName", i++ );
		}
		while ( pLandGateMan->FindLandGate ( szNewName ) );

		pLandGate->SetName ( szNewName );

		DWORD dwGateID;
		dwGateID = 0;
		while ( pLandGateMan->FindLandGate ( dwGateID ) )
		{
			++dwGateID;
		}
		
		pLandGate->SetGateID ( dwGateID );

		D3DXVECTOR3* vCollPos = pView->GetCollisionPos ();
		pLandGate->SetPos ( *vCollPos );

		m_pLandGate = pLandGate;

		pView->ActiveEditBox ( m_pLandGate->GetMax(), m_pLandGate->GetMin() );

		pLandGateMan->AddLandGate ( m_pLandGate );
		pLandGateMan->BuildTree();

		m_listLandGate.SelectString ( 0, m_pLandGate->GetName() );
	}
}

void CLevelLandGatePage::OnBnClickedButtonLandGateEdit()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	int nIndex = m_listLandGate.GetCurSel();

	if ( nIndex != -1 )
	{
		m_pLandGate = (PDXLANDGATE)m_listLandGate.GetItemData ( nIndex );
		pView->ActiveEditBox ( m_pLandGate->GetMax(), m_pLandGate->GetMin() );

		ViewContrls ( TRUE, FALSE );

		D3DXVECTOR3 vCamPos = m_pLandGate->GetMax() + m_pLandGate->GetMin();
		vCamPos = vCamPos * ( 1.0f / 2.0f );

		DxViewPort::GetInstance().CameraJump ( vCamPos );
	}
}

void CLevelLandGatePage::OnBnClickedButtonLandGateDel()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( pGLLandMan )
	{
		DxLandGateMan* pLandGateMan = &pGLLandMan->GetLandGateMan();

		int nIndex = m_listLandGate.GetCurSel();

		if ( nIndex != -1 )
		{
			PDXLANDGATE pLandGate = (PDXLANDGATE)m_listLandGate.GetItemData ( nIndex );

			pLandGateMan->DelLandGate ( pLandGate->GetName() );
			pLandGateMan->BuildTree();

			if ( pLandGate == m_pLandGate )
			{
				m_pLandGate = NULL;
				ViewContrls ( FALSE, FALSE );
				UpdateContrl();
			}
			else
			{
				ListtingLandGate();
			}
		}
	}
}

void CLevelLandGatePage::OnBnClickedButtonLandGateCopy()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	int nIndex = m_listLandGate.GetCurSel();

	if ( nIndex != -1 )
	{
		PDXLANDGATE pLandGate = (PDXLANDGATE)m_listLandGate.GetItemData ( nIndex );

		OnBnClickedButtonLandGateNew();

		m_pLandGate->SetBox ( pLandGate->GetMax(), pLandGate->GetMin() );
		pView->ActiveEditBox ( pLandGate->GetMax(), pLandGate->GetMin() );
	}
}

void CLevelLandGatePage::OnBnClickedCheckLandGateRend()
{
	// TODO: Add your control notification handler code here
	DxLandGateMan::m_bREND = GetWin_Check ( this, IDC_CHECK_LAND_GATE_REND );
}

void CLevelLandGatePage::OnBnClickedButtonLandGateApply()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( pGLLandMan )
	{
		DxLandGateMan* pLandGateMan = &pGLLandMan->GetLandGateMan();

		char szName[65] = {0};
		char szValue[MAX_PATH] = {0};

		DWORD dwFlags;
		DWORD dwGateID;

		SNATIVEID sToMapID;
		DWORD dwToGateID;

		D3DXVECTOR2 vDiv;

		strcpy_s ( szName, GetWin_Text ( this, IDC_EDIT_LAND_GATE_NAME ) );

		if ( GetWin_Check ( this, IDC_CHECK_LAND_GATE_OUT ) )
			dwFlags |= 2;
		if ( GetWin_Check ( this, IDC_CHECK_LAND_GATE_IN ) )
			dwFlags |= 1;

		strcpy_s( szValue, GetWin_Text ( this, IDC_EDIT_LAND_GATE_ID ) );
		dwGateID = (DWORD)atoi (szValue);

		strcpy_s( szValue, GetWin_Text ( this, IDC_EDIT_LAND_GATE_TOMAP_MID ) );
		sToMapID.wMainID = (WORD)atoi (szValue);
		strcpy_s( szValue, GetWin_Text ( this, IDC_EDIT_LAND_GATE_TOMAP_SID ) );
		sToMapID.wSubID = (WORD)atoi (szValue);

		strcpy_s( szValue, GetWin_Text ( this, IDC_EDIT_LAND_GATE_TOGATE_ID ) );
		dwToGateID = (DWORD)atoi (szValue);

		strcpy_s( szValue, GetWin_Text ( this, IDC_EDIT_LAND_GATE_W ) );
		vDiv.x = (float)atof (szValue);
		strcpy_s( szValue, GetWin_Text ( this, IDC_EDIT_LAND_GATE_H ) );
		vDiv.y = (float)atof (szValue);

		PDXLANDGATE pCur = pLandGateMan->GetListLandGate();

		while ( pCur )
		{
			if ( pCur != m_pLandGate )
			{
				if ( !strcmp ( pCur->GetName(), szName ) )
				{
					MessageBox ( "ERROR : Same name is already registered.", "Caution" );
					return;
				}
				if ( pCur->GetGateID() == dwGateID )
				{
					MessageBox ( "ERROR : Same ID is already registered.", "Caution" );
					return;
				}
			}

			pCur = pCur->m_pNext;
		}

		m_pLandGate->SetName ( szName );
		m_pLandGate->SetBox ( pView->GetEditBox()->m_vMax, pView->GetEditBox()->m_vMin );
		m_pLandGate->SetGate ( dwFlags, dwGateID, sToMapID, dwToGateID, vDiv );

		NavigationMesh *pNavi = CLevelEditorView::m_LandMan.GetNaviMesh();

		if ( pNavi->IsVailedCollision() )
		{
			m_pLandGate->MakeStartPos ( pNavi );
			pLandGateMan->BuildTree();

			m_listLandGate.SelectString ( 0, m_pLandGate->GetName() );
			ListtingLandGate();
		}
		else
		{
			MessageBox ( "ERROR : Start locaion can not be calculated because there is no navigation mesh", "Caution" );
		}
	}
}

void CLevelLandGatePage::OnBnClickedButtonLandGateRegis()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonLandGateApply();

	m_pLandGate = NULL;
	ViewContrls ( FALSE, FALSE );
	UpdateContrl ();
}

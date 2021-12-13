// MobSchedulePage.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "LevelEditorView.h"
#include "MainFrm.h"
#include "SheetWithTab.h"
#include "MobSchedulePage.h"
#include "MobSetPage.h"

#include "EtcFunction.h"

#include "DxViewPort.h"
#include "DxMethods.h"
#include "GLList.h"
#include "GLDefine.h"

#include "GLGaeaServer.h"
#include "GLLandMan.h"
#include "GLMobSchedule.h"
#include ".\mobschedulepage.h"

// CMobSchedulePage dialog

IMPLEMENT_DYNAMIC(CMobSchedulePage, CPropertyPage)
CMobSchedulePage::CMobSchedulePage()
	: CPropertyPage(CMobSchedulePage::IDD)
	, m_bInit (FALSE)
	, m_pMobActList (NULL)
	, m_pMobAction (NULL)
{
	m_AffineMatrix.UseAffineMatrix();
}

CMobSchedulePage::~CMobSchedulePage()
{
}

void CMobSchedulePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SCH, m_ctrlSchedule);
	DDX_Control(pDX, IDC_COMBO_SCH_ACT, m_ctrlAction);
}


BEGIN_MESSAGE_MAP(CMobSchedulePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SCH_SET_COMPLET, OnBnClickedButtonSchSetComplet)
	ON_LBN_DBLCLK(IDC_LIST_SCH, OnLbnDblclkListSch)
	ON_BN_CLICKED(IDC_BUTTON_SCH_NEW, OnBnClickedButtonSchNew)
	ON_BN_CLICKED(IDC_BUTTONSCH_EDIT, OnBnClickedButtonschEdit)
	ON_BN_CLICKED(IDC_BUTTONSCH_MOVE, OnBnClickedButtonschMove)
	ON_BN_CLICKED(IDC_BUTTON_SCH_DEL, OnBnClickedButtonSchDel)
	ON_CBN_SELCHANGE(IDC_COMBO_SCH_ACT, OnCbnSelchangeComboSchAct)
	ON_BN_CLICKED(IDC_BUTTON_SCH_OK, OnBnClickedButtonSchOk)
	ON_BN_CLICKED(IDC_BUTTON_SCH_CANCEL, OnBnClickedButtonSchCancel)
END_MESSAGE_MAP()


// CMobSchedulePage message handlers
BOOL CMobSchedulePage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlAction.AddString ( "Move" );
	m_ctrlAction.AddString ( "Rest" );

	ActiveMobScheDuleEdit ( FALSE, FALSE );
	ListingUpdate();

	m_bInit = TRUE;
	
	return TRUE;
}

void CMobSchedulePage::SetSchedule ( MOBACTIONLIST *pMobActList )
{
	m_pMobActList = pMobActList;
	ListingUpdate();
}

void CMobSchedulePage::ActiveMobScheDuleEdit ( BOOL bShow, BOOL bNew )
{
	SetWin_ShowWindow ( this, IDC_COMBO_SCH_ACT, bShow );

	SetWin_ShowWindow ( this, IDC_STATIC_DELAY_TIME, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_SCH_DELAY_TIME, bShow );

	SetWin_ShowWindow ( this, IDC_BUTTON_SCH_OK, bShow );
	SetWin_ShowWindow ( this, IDC_BUTTON_SCH_CANCEL, bShow );
}

void CMobSchedulePage::ListingUpdate ()
{
	m_ctrlSchedule.ResetContent();

	int nIndex;

	if ( m_pMobActList )
	{
		MOBACTIONNODE* pHead = m_pMobActList->m_pHead;

		while ( pHead )
		{
			CString strTemp;

			switch ( pHead->Data.emAction )
			{
			case EMACTION_MOVE:
				{
					char szValueX[32] = {0};
					char szValueY[32] = {0};
					char szValueZ[32] = {0};

					_gcvt_s ( szValueX, pHead->Data.vPos.x, 7 );
					_gcvt_s ( szValueY, pHead->Data.vPos.y, 7 );
					_gcvt_s ( szValueZ, pHead->Data.vPos.z, 7 );

					strTemp.Format ( "[Move] X:%s Y:%s Z:%s", szValueX, szValueY, szValueZ );

					SetWin_Enable ( this, IDC_EDIT_SCH_DELAY_TIME, FALSE );
				}
				break;
			case EMACTION_ACTSTAY:
				{
					char szValue[32] = {0};

					_gcvt_s ( szValue, pHead->Data.vPos.x, 7 );

					strTemp.Format ( "[Rest] %s", szValue );

					SetWin_Enable ( this, IDC_EDIT_SCH_DELAY_TIME, TRUE );
				}
				break;
			}

			nIndex = m_ctrlSchedule.AddString ( strTemp.GetString() );
			m_ctrlSchedule.SetItemData ( nIndex, (DWORD_PTR)&pHead->Data );

			pHead = pHead->pNext;
		}
	}
}

void CMobSchedulePage::OnBnClickedButtonSchSetComplet()
{
	// TODO: Add your control notification handler code here
	m_pSheetTab->SetActivePage ( MOBSETPAGE );
	m_pSheetTab->GetActivePage ();

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
	pView->ActiveEditMatrix ( &CMobSetPage::m_Schedule );
}

void CMobSchedulePage::OnLbnDblclkListSch()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctrlSchedule.GetCurSel();

	if ( nIndex != -1 )
	{
		SMOBACTION* pMobAct = (SMOBACTION *)m_ctrlSchedule.GetItemData ( nIndex );

		if ( pMobAct == (SMOBACTION *)-1 )
		{
			MessageBox ( "Error! 0xFFFFFFFF" );
		}
		else
		{
			DxViewPort::GetInstance().CameraJump ( pMobAct->vPos );
		}
	}
}

void CMobSchedulePage::OnBnClickedButtonSchNew()
{
	// TODO: Add your control notification handler code here
	if ( !m_pMobAction )
	{
		SetWin_Enable ( this, IDC_BUTTON_SCH_SET_COMPLET, FALSE );

		SetWin_Enable ( this, IDC_BUTTON_SCH_NEW, FALSE );
		SetWin_Enable ( this, IDC_BUTTONSCH_EDIT, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_SCH_DEL, FALSE );

		m_pMobAction = new SMOBACTION;

		m_ctrlAction.SetCurSel (0);
		m_valTime.Format ( "%2.2f", FLT_MAX );

		UpdateData ( FALSE );

		ActiveMobScheDuleEdit ( TRUE, TRUE );
		OnCbnSelchangeComboSchAct ();
	}
}

void CMobSchedulePage::OnBnClickedButtonschEdit()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctrlSchedule.GetCurSel();

	if ( nIndex != -1 )
	{
		SetWin_Enable ( this, IDC_BUTTON_SCH_SET_COMPLET, FALSE );

		SetWin_Enable ( this, IDC_BUTTON_SCH_NEW, FALSE );
		SetWin_Enable ( this, IDC_BUTTONSCH_EDIT, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_SCH_DEL, FALSE );

		SMOBACTION* pAct = (SMOBACTION *)m_ctrlSchedule.GetItemData ( nIndex );

		if ( pAct == (SMOBACTION *)-1 )
		{
			MessageBox ( "Error! 0xFFFFFFFF" );
		}
		else
		{
			m_pMobAction = pAct;

			m_ctrlAction.SetCurSel ( (int)m_pMobAction->emAction );

			switch ( m_pMobAction->emAction  )
			{
			case EMACTION_MOVE:
				{
					DXAFFINEPARTS sAffine;
					sAffine.vTrans = m_pMobAction->vPos;

					m_AffineMatrix.SetAffineValue ( &sAffine );

					CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
					CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
					pView->ActiveEditMatrix ( &m_AffineMatrix );
				}
				break;
			case EMACTION_ACTSTAY:
				{
					char szValue[32] = {0};

					_gcvt_s ( szValue, m_pMobAction->vPos.x, 7 );
					m_valTime = szValue;
				}
				break;
			}

			UpdateData(FALSE);

			ActiveMobScheDuleEdit ( TRUE, FALSE );
			ListingUpdate ();
		}
	}
}

void CMobSchedulePage::OnBnClickedButtonschMove()
{
	// TODO: Add your control notification handler code here
	OnLbnDblclkListSch();
}

void CMobSchedulePage::OnBnClickedButtonSchDel()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctrlSchedule.GetCurSel();

	if ( nIndex != -1 )
	{
		SMOBACTION* pAct = (SMOBACTION *)m_ctrlSchedule.GetItemData ( nIndex );

		if ( pAct == (SMOBACTION *)-1 )
		{
			MessageBox ( "Error! 0xFFFFFFFF" );
		}
		else
		{
			m_pMobAction = pAct;

			for ( MOBACTIONNODE* pHead = m_pMobActList->m_pHead; pHead; pHead = pHead->pNext )
			{
				if ( &pHead->Data == m_pMobAction )
				{
					m_pMobActList->DELNODE ( pHead );
					m_ctrlSchedule.DeleteString ( nIndex );

					m_pMobAction = NULL;
					return;
				}
			}

			ActiveMobScheDuleEdit ( FALSE, FALSE );
			ListingUpdate ();
		}
	}
}

void CMobSchedulePage::OnCbnSelchangeComboSchAct()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctrlSchedule.GetCurSel();

	if ( nIndex == (int)EMACTION_ACTSTAY )
	{
		SetWin_Enable ( this, IDC_EDIT_SCH_DELAY_TIME, TRUE );

		CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
		CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
		pView->DeActiveEditMatrix ();
	}
	else
	{
		SetWin_Enable ( this, IDC_EDIT_SCH_DELAY_TIME, FALSE );

		CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
		CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
		D3DXVECTOR3* vCollPos = pView->GetCollisionPos ();

		DXAFFINEPARTS sAffine;
		sAffine.vTrans = *vCollPos;

		m_AffineMatrix.SetAffineValue ( &sAffine );
		pView->ActiveEditMatrix ( &m_AffineMatrix );
	}
}

void CMobSchedulePage::OnBnClickedButtonSchOk()
{
	// TODO: Add your control notification handler code here
	SetWin_Enable ( this, IDC_BUTTON_SCH_SET_COMPLET, TRUE );

	SetWin_Enable ( this, IDC_BUTTON_SCH_NEW, TRUE );
	SetWin_Enable ( this, IDC_BUTTONSCH_EDIT, TRUE );
	SetWin_Enable ( this, IDC_BUTTON_SCH_DEL, TRUE );

	UpdateData();

	m_pMobAction->emAction = (EMMOBACTIONS)m_ctrlAction.GetCurSel();

	if ( m_pMobAction->emAction == EMACTION_ACTSTAY )
	{
		if ( !m_valTime.GetLength() )
		{
			MessageBox ( "ERROR : Enter a term of rest." );
			return;
		}

		m_pMobAction->fLife = (float)atof ( m_valTime.GetString() );
	}
	else
	{
		CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
		CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

		m_pMobAction->vPos = m_AffineMatrix.m_pAffineParts->vTrans;
		pView->DeActiveEditMatrix ();
	}

	for ( MOBACTIONNODE* pHead = m_pMobActList->m_pHead; pHead; pHead = pHead->pNext )
	{
		if ( &pHead->Data == m_pMobAction )
		{
			m_pMobAction = NULL;

			ActiveMobScheDuleEdit ( FALSE, FALSE );
			ListingUpdate ();

			return;
		}
	}

	m_pMobActList->ADDTAIL ( *m_pMobAction );

	if ( m_pMobAction )
	{
		SAFE_DELETE ( m_pMobAction );
	}

	ActiveMobScheDuleEdit ( FALSE, FALSE );
	ListingUpdate ();
}

void CMobSchedulePage::OnBnClickedButtonSchCancel()
{
	// TODO: Add your control notification handler code here
	SetWin_Enable ( this, IDC_BUTTON_SCH_SET_COMPLET, TRUE );

	SetWin_Enable ( this, IDC_BUTTON_SCH_NEW, TRUE );
	SetWin_Enable ( this, IDC_BUTTONSCH_EDIT, TRUE );
	SetWin_Enable ( this, IDC_BUTTON_SCH_DEL, TRUE );

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
	pView->DeActiveEditMatrix ();

	ActiveMobScheDuleEdit ( FALSE, FALSE );

	for ( MOBACTIONNODE* pHead = m_pMobActList->m_pHead; pHead; pHead = pHead->pNext )
	{
		if ( &pHead->Data == m_pMobAction )
		{
			m_pMobAction = NULL;

			return;
		}
	}

	if ( m_pMobAction )
	{
		SAFE_DELETE ( m_pMobAction );
	}
}

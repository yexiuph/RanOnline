// TalkPage.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "NpcAction.h"
#include "TalkPage.h"
#include ".\talkpage.h"
#include "SheetWithTab.h"
#include "GLItemMan.h"
#include "EtcFunction.h"

#include "GLQuest.h"
#include "GLQuestMan.h"
#include "NpcTalkCondition.h"
#include "NpcDialogueSet.h"
#include "NpcDialogue.h"
#include "NpcDialogueCase.h"
#include "NpcTalkControl.h"
#include "HaveItemDialog.h"
#include "DlgNpcTalk.h"

#include "GLogic.h"

#define MAX_RANDOM_TIME		999

// CTalkPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTalkPage, CPropertyPage)
CTalkPage::CTalkPage( LOGFONT logfont )
	: CPropertyPage(CTalkPage::IDD),
	m_pSheetTab(NULL),
	m_CallPage(0),
	m_bModify(FALSE),

	m_pOrgTalk(NULL),
	m_pTalkControl(NULL),
	m_pNpcDialogueSet(NULL),
	m_pFont ( NULL )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CTalkPage::~CTalkPage()
{
	SAFE_DELETE ( m_pFont );
}

void CTalkPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_QUEST_START, m_comboQUESTSTART[0]);
	DDX_Control(pDX, IDC_COMBO_QUEST_START2, m_comboQUESTSTART[1]);
	DDX_Control(pDX, IDC_COMBO_QUEST_START3, m_comboQUESTSTART[2]);
	DDX_Control(pDX, IDC_COMBO_QUEST_START4, m_comboQUESTSTART[3]);
	DDX_Control(pDX, IDC_COMBO_QUEST_START5, m_comboQUESTSTART[4]);
	DDX_Control(pDX, IDC_COMBO_QUEST_PROG, m_comboQUEST_PROC);
	DDX_Control(pDX, IDC_COMBO_QUEST_PROG_STEP, m_comboQUEST_PROC_STEP);
	DDX_Control(pDX, IDC_LIST_CASE_RANDOM, m_ctrlRandomID);
}


BEGIN_MESSAGE_MAP(CTalkPage, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION_TYPE, OnCbnSelchangeComboActionType)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION_DATA, OnCbnSelchangeComboActionData)
	ON_BN_CLICKED(IDC_BUTTON_CONDITON_SET, OnBnClickedButtonConditonSet)
	ON_BN_CLICKED(IDC_BUTTON_CONDITON_RESET, OnBnClickedButtonConditonReset)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_TRADE_ITEM, OnBnClickedButtonTradeItem)
	ON_BN_CLICKED(IDC_BUTTON_TRADE_DEL, OnBnClickedButtonTradeDel)
	ON_CONTROL_RANGE( CBN_SELCHANGE, IDC_COMBO_QUEST_START, IDC_COMBO_QUEST_START5,OnCbnSelchangeComboQuestStart )
	ON_CBN_SELCHANGE(IDC_COMBO_QUEST_PROG, OnCbnSelchangeComboQuestProg)
	ON_CBN_SELCHANGE(IDC_COMBO_QUEST_PROG_STEP, OnCbnSelchangeComboQuestProgStep)
	ON_BN_CLICKED(IDC_BUTTON_BUS, OnBnClickedButtonBus)
	ON_BN_CLICKED(IDC_RANDOM_INSERT, OnBnClickedRandomInsert)
	ON_BN_CLICKED(IDC_RANDOM_DELETE, OnBnClickedRandomDelete)
	ON_BN_CLICKED(IDC_RANDOM_RESET, OnBnClickedRandomReset)
END_MESSAGE_MAP()


// CTalkPage 메시지 처리기입니다.

BOOL CTalkPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

    CComboBox* pComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_TYPE );
	for ( int i = 0; i < SNpcTalk::EM_TALK_SIZE; i++ )
	{
		int nIndex = pComboBox->InsertString ( i, SNpcTalk::strACTIONTYPE[i].c_str() );
		pComboBox->SetItemData ( nIndex, i );
	}


	m_ctrlRandomID.SetExtendedStyle (
		m_ctrlRandomID.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	RECT	rect;
	m_ctrlRandomID.GetClientRect ( &rect );
	LONG	Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	const int	ColumnCount = 2;	
	char*		ColumnName[ColumnCount] = { "ID", "Desc" };
	int			ColumnWidthPercent[ColumnCount] = { 20, 80 };

	for ( int i = 0; i < ColumnCount; i++ )
	{		
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM
			| LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = ( Width * ColumnWidthPercent[i] ) / 100;
		m_ctrlRandomID.InsertColumn (i, &lvcolumn);		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTalkPage::UpdateItems ()
{
	CComboBox* pComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_TYPE );

	if ( !m_bModify )
	{
		DWORD BackupAction   = SNpcTalk::EM_DO_NOTHING;		
		DWORD BackupActionID = UINT_MAX;
		if( m_Talk.m_nACTION != SNpcTalk::EM_DO_NOTHING &&
			m_Talk.m_dwACTION_NO != UINT_MAX )
		{
			BackupAction   = m_Talk.m_nACTION;		
			BackupActionID = m_Talk.m_dwACTION_NO;
		}		

		m_Talk.Init ();
		m_Talk.m_dwNID = m_pTalkControl->MakeTalkNID ();
		m_Talk.m_nACTION	 = BackupAction;
		m_Talk.m_dwACTION_NO = BackupActionID;

		CString strTemp;

		strTemp.Format ( "%d", m_Talk.m_dwNID );		 
		GetDlgItem ( IDC_EDIT_TALKID )->SetWindowText ( strTemp );

//		pComboBox->SetCurSel ( 0 );
		pComboBox->SetCurSel ( m_Talk.m_nACTION );

		//	콤보박스도 자동으로 선택되어있도록 하기 위함임
		BackupActionID = m_Talk.m_dwACTION_NO;
		OnCbnSelchangeComboActionType ();

		CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
		m_Talk.m_dwACTION_NO = BackupActionID;

		int nSize = pDataComboBox->GetCount ();
		for ( int i=0; i<nSize; i++ )
		{
			DWORD dwNO = (DWORD) pDataComboBox->GetItemData ( i );
			if ( dwNO==m_Talk.m_dwACTION_NO )
			{
				pDataComboBox->SetCurSel ( i );
				break;
			}
		}

		SetWndView ( m_Talk.m_nACTION, m_Talk.m_dwACTION_NO );

	}
	else
	{
		CString strTemp;

		strTemp.Format ( "%d", m_Talk.m_dwNID );		 
		GetDlgItem ( IDC_EDIT_TALKID )->SetWindowText ( strTemp );

		GetDlgItem ( IDC_EDIT_TALK )->SetWindowText ( m_Talk.GetTalk() );		

		pComboBox->SetCurSel ( m_Talk.m_nACTION );

		//	콤보박스도 자동으로 선택되어있도록 하기 위함임
		DWORD BackupActionID = m_Talk.m_dwACTION_NO;
		OnCbnSelchangeComboActionType ();
		
		CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
		m_Talk.m_dwACTION_NO = BackupActionID;

		int nSize = pDataComboBox->GetCount ();
		for ( int i=0; i<nSize; i++ )
		{
			DWORD dwNO = (DWORD) pDataComboBox->GetItemData ( i );
			if ( dwNO==m_Talk.m_dwACTION_NO )
			{
				pDataComboBox->SetCurSel ( i );
				break;
			}
		}

		GetDlgItem( IDC_EDIT_BUS )->SetWindowText( m_Talk.m_strBusFile.c_str() );

		std::string strCondition;
		SNpcTalkCondition *pCondition = m_Talk.GetCondition();
		if ( pCondition )	pCondition->GetConditionText( strCondition );
		GetDlgItem ( IDC_EDIT_CONDITION )->SetWindowText ( strCondition.c_str() );

		SetWndView ( m_Talk.m_nACTION, m_Talk.m_dwACTION_NO );
	}
}

void CTalkPage::SetTalkData ( CNpcDialogueSet* pNpcDialogueSet, CNpcTalkControl* pTalkControl )
{
	assert ( pTalkControl && "널이 설정되고 있습니다." );
	assert ( pNpcDialogueSet && "널이 설정되고 있습니다." );

	m_pNpcDialogueSet = pNpcDialogueSet;
	m_pTalkControl = pTalkControl;
}

void CTalkPage::SetModifyData ( BOOL bModify, SNpcTalk* pTalk )
{
	m_bModify = bModify;
	if ( bModify )
	{
		m_pOrgTalk = pTalk;
		m_Talk = *pTalk;
	}
}

void CTalkPage::OnCbnSelchangeComboActionType()
{
	CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
	pDataComboBox->ResetContent ();

	CComboBox* pTypeComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_TYPE );
	int nIndex = pTypeComboBox->GetCurSel ();
	m_Talk.m_nACTION = (int)pTypeComboBox->GetItemData ( nIndex );
    
	switch ( nIndex )
	{
	case SNpcTalk::EM_DO_NOTHING:
		{
		}
		break;

	case SNpcTalk::EM_PAGE_MOVE:
	case SNpcTalk::EM_QUEST_START:
	case SNpcTalk::EM_QUEST_STEP:
		{
			CNpcDialogueSet::DIALOGMAP* pMap = m_pNpcDialogueSet->GetDialogueMap ();
			if ( pMap->size() )
			{
				CNpcDialogueSet::DIALOGMAP_IT iter = pMap->begin();
				CNpcDialogueSet::DIALOGMAP_IT iter_end = pMap->end();	
				for ( ; iter!=iter_end; ++iter )
				{
					CNpcDialogue* pDialogue = (*iter).second;
					
					DWORD dwIndex = pDialogue->FindNonCondition();
					if ( dwIndex==UINT_MAX )	dwIndex = 0;

					CNpcDialogueCase* pDlgCase = pDialogue->GetDlgCase(dwIndex);
					if ( !pDlgCase )		continue;

					CString strTemp;
					strTemp.Format ( "[%03d] %s", pDialogue->GetDlgNID(), pDlgCase->GetBasicTalk ().c_str() );
					int nItem = pDataComboBox->AddString ( strTemp );
					pDataComboBox->SetItemData ( nItem, pDialogue->GetDlgNID() );
				}
				pDataComboBox->SetCurSel ( 0 );

				OnCbnSelchangeComboActionData ();

				SetWndView ( m_Talk.m_nACTION, m_Talk.m_dwACTION_NO );
			}
		}
		break;
	case SNpcTalk::EM_BASIC:
		{
			CComboBox* pComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
			for ( int i = 0; i < SNpcTalk::EM_BASIC_SIZE; i++ )
			{
				int nIndex = pComboBox->InsertString ( i, SNpcTalk::szBASICDESC[i].c_str() );
				pComboBox->SetItemData ( nIndex, i );
			}
			pDataComboBox->SetCurSel ( 0 );

			OnCbnSelchangeComboActionData ();

			SetWndView ( m_Talk.m_nACTION, m_Talk.m_dwACTION_NO );
		}
		break;
	};
}

void CTalkPage::OnCbnSelchangeComboActionData()
{
	CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
	int nIndex = pDataComboBox->GetCurSel ();
	m_Talk.m_dwACTION_NO = (DWORD)pDataComboBox->GetItemData ( nIndex );

	SetWndView ( m_Talk.m_nACTION, m_Talk.m_dwACTION_NO );
}

void CTalkPage::OnBnClickedButtonConditonSet()
{
	if ( !m_Talk.GetCondition() )
		m_Talk.SetCondition ( SNpcTalkCondition() );

	m_pSheetTab->ActiveActionCondition ( NPCACTIONTALK, m_Talk.GetCondition() );
}

void CTalkPage::OnBnClickedButtonConditonReset()
{
    if ( MessageBox ( "Do you want to delete?", "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_Talk.ResetCondition ();
	}
}

void CTalkPage::OnBnClickedButtonOk()
{
	assert ( m_pTalkControl && "데이타 설정이 잘못되었습니다." );

	CString strTemp;

	GetDlgItem ( IDC_EDIT_TALKID )->GetWindowText( strTemp );
	m_Talk.m_dwNID = (DWORD) atoi ( strTemp.GetString() );

	GetDlgItem ( IDC_EDIT_BUS )->GetWindowText( strTemp );
	m_Talk.m_strBusFile = strTemp.GetString();

	GetDlgItem ( IDC_EDIT_TALK )->GetWindowText( strTemp );
	m_Talk.m_strTalk = strTemp.GetString();

	m_Talk.m_dwRandomTime = GetWin_Num_int( this, IDC_EDIT_RANDOM_TIME );
	
	if ( m_Talk.m_dwRandomTime > MAX_RANDOM_TIME ) m_Talk.m_dwRandomTime = MAX_RANDOM_TIME;

	if ( !strTemp.GetLength () )
	{
		MessageBox ( "설명부분이 비어있습니다.", "Caution", MB_OK | MB_ICONASTERISK );
		return;
	}

	if ( !m_bModify )
	{
		SNpcTalk* pNewTalk = new SNpcTalk;
		*pNewTalk = m_Talk;

		if ( m_pTalkControl->AddTalk ( pNewTalk ) )
		{
			m_pSheetTab->ActiveActionAnswer ();
		}
		else
		{
			SAFE_DELETE ( pNewTalk );
			MessageBox ( "Registered the same ID already." );
		}
	}
	else
	{
		for ( int i = 0;  i< MAX_QUEST_START-1; ++i )
		{
			if ( !GetWin_Check( this, IDC_CHECK_QUEST_START2 + i ) ) 
				m_Talk.m_dwQuestStartID[i+1] = UINT_MAX;
		}

		SNpcTalk* pTalk = m_pTalkControl->GetTalk ( m_Talk.m_dwNID );

		*pTalk = m_Talk;

		m_pSheetTab->ActiveActionAnswer ();
	}
}

void CTalkPage::OnBnClickedButtonCancel()
{
	m_pSheetTab->ActiveActionAnswer ();
}

// Need Add New Item Con.
void CTalkPage::OnBnClickedButtonTradeItem()
{
	CHaveItemDialog dlg;
	if ( dlg.DoModal () == IDOK )
	{
		DWORD dwITEM = dlg.GetItemID ();

		m_Talk.m_dwACTION_PARAM2 = dwITEM;
		
		SITEM * pITEM = GLItemMan::GetInstance().GetItem ( SNATIVEID(m_Talk.m_dwACTION_PARAM2) );
		
		CString strNAME;
		if ( pITEM )	
		{
			strNAME = pITEM->GetName();
			char szTempChar[128];
			SNATIVEID sID;
			sID.dwID = m_Talk.m_dwACTION_PARAM2;
			sprintf_s( szTempChar, "[%d/%d]", sID.wMainID, sID.wSubID );
			strNAME += szTempChar;
		}
		SetWin_Text ( this, IDC_EDIT_TRADE_ITEM, strNAME );
	}
}

void CTalkPage::OnBnClickedButtonTradeDel()
{
	m_Talk.m_dwACTION_PARAM2 = UINT_MAX;
	SetWin_Text ( this, IDC_EDIT_TRADE_ITEM, "" );
}

void CTalkPage::SetWndView ( int nACTION, DWORD dwNO )
{
	BOOL bTRADE = ( nACTION==SNpcTalk::EM_BASIC && dwNO == SNpcTalk::EM_ITEM_TRADE );
	{
		DWORD dwWNDID[] = {
			IDC_BUTTON_TRADE_ITEM,
			IDC_EDIT_TRADE_ITEM,
			IDC_BUTTON_TRADE_DEL
		};

		DWORD dwWNDNUM = sizeof(dwWNDID)/sizeof(DWORD);

		for ( DWORD i=0; i<dwWNDNUM; ++i )
		{
			CWnd *pWnd = GetDlgItem ( dwWNDID[i] );
			if ( pWnd )
			{
				pWnd->ShowWindow ( bTRADE );
			}
		}

		CString strNAME;
		
		SITEM* pITEM = NULL;
		{
			DWORD dwNeedItemID[] = {
				IDC_EDIT_HAVE_ITEM1,
				IDC_EDIT_HAVE_ITEM2,
				IDC_EDIT_HAVE_ITEM3,
				IDC_EDIT_HAVE_ITEM4,
				IDC_EDIT_HAVE_ITEM5,
			};
			BYTE i;
			for( i = 0; i < MAX_NEEDITEM_COUNT; i++ )
			{
				if( bTRADE == FALSE )
				{
					GetDlgItem( dwNeedItemID[i] )->ShowWindow( SW_HIDE );
				}else{
					pITEM = GLItemMan::GetInstance().GetItem ( SNATIVEID(m_Talk.m_dwNeedItem[i]) );
					if ( pITEM )
					{	
						strNAME = pITEM->GetName(); 
						char szTempChar[128];
						SNATIVEID sID;
						sID.dwID = m_Talk.m_dwNeedItem[i];
						sprintf_s( szTempChar, "[%d/%d]", sID.wMainID, sID.wSubID );
						strNAME += szTempChar;
					}else{
						strNAME = "";
					}
					GetDlgItem( dwNeedItemID[i] )->ShowWindow( SW_SHOW );
					SetWin_Text ( this, dwNeedItemID[i], strNAME );
				}
			}
		}

		pITEM = GLItemMan::GetInstance().GetItem ( SNATIVEID(m_Talk.m_dwACTION_PARAM2) );
		if ( pITEM )
		{
			strNAME = pITEM->GetName();
			char szTempChar[128];
			SNATIVEID sID;
			sID.dwID = m_Talk.m_dwACTION_PARAM2;
            sprintf_s( szTempChar, "[%d/%d]", sID.wMainID, sID.wSubID );
			strNAME += szTempChar;
		}else{
			strNAME = "";
		}
		SetWin_Text ( this, IDC_EDIT_TRADE_ITEM, strNAME );
	}

	BOOL bQSTART = ( nACTION == SNpcTalk::EM_QUEST_START );
	{
		DWORD dwWNDID[] = {
			IDC_STATIC_QUEST_START,
			IDC_COMBO_QUEST_START,
			IDC_COMBO_QUEST_START2,
			IDC_COMBO_QUEST_START3,
			IDC_COMBO_QUEST_START4,
			IDC_COMBO_QUEST_START5,
			IDC_CHECK_QUEST_START2,
			IDC_CHECK_QUEST_START3,
			IDC_CHECK_QUEST_START4,
			IDC_CHECK_QUEST_START5,
		};

		DWORD dwWNDNUM = sizeof(dwWNDID)/sizeof(DWORD);

		for ( DWORD i=0; i < dwWNDNUM; ++i )
		{
			CWnd *pWnd = GetDlgItem ( dwWNDID[i] );
			if ( pWnd )
			{
				pWnd->ShowWindow ( bQSTART );
			}
		}
		
		for ( int i = 0; i < MAX_QUEST_START; ++i )
		{
			m_comboQUESTSTART[i].ResetContent();
		}

		GLQuestMan::MAPQUEST &sQMAP = GLQuestMan::GetInstance().GetMap();
		GLQuestMan::MAPQUEST_ITER &iter = sQMAP.begin();
		GLQuestMan::MAPQUEST_ITER &iter_end = sQMAP.end();
		for ( ; iter!=iter_end; ++iter )
		{
			DWORD dwQUESTID = (*iter).first;
			GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );;

			if ( !pQUEST )	continue;

			CString strNAME;
			strNAME.Format ( "[%d] %s", dwQUESTID, pQUEST->GetTITLE() );
			int nIndex;
						
			for ( int i = 0; i < MAX_QUEST_START; ++i )
			{
				nIndex = m_comboQUESTSTART[i].AddString ( strNAME );
				m_comboQUESTSTART[i].SetItemData ( nIndex, dwQUESTID );

				if ( dwQUESTID==m_Talk.m_dwQuestStartID[i] )
				{
					m_comboQUESTSTART[i].SetCurSel ( nIndex );
				}
			}			
		}
	}

	BOOL bQSTEP = ( nACTION==SNpcTalk::EM_QUEST_STEP );
	{
		DWORD dwWNDID[] = {
			IDC_STATIC_QUEST_STEP,
			IDC_COMBO_QUEST_PROG,
			IDC_STATIC_QUEST_STEP2,
			IDC_COMBO_QUEST_PROG_STEP
		};

		DWORD dwWNDNUM = sizeof(dwWNDID)/sizeof(DWORD);

		for ( DWORD i=0; i<dwWNDNUM; ++i )
		{
			CWnd *pWnd = GetDlgItem ( dwWNDID[i] );
			if ( pWnd )
			{
				pWnd->ShowWindow ( bQSTEP );
			}
		}

		m_comboQUEST_PROC.ResetContent();

		GLQuestMan::MAPQUEST &sQMAP = GLQuestMan::GetInstance().GetMap();
		GLQuestMan::MAPQUEST_ITER &iter = sQMAP.begin();
		GLQuestMan::MAPQUEST_ITER &iter_end = sQMAP.end();
		for ( ; iter!=iter_end; ++iter )
		{
			DWORD dwQUESTID = (*iter).first;
			GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );;
			if ( !pQUEST )	continue;

			CString strNAME;
			strNAME.Format ( "[%d] %s", dwQUESTID, pQUEST->GetTITLE() );

			int nIndex = m_comboQUEST_PROC.AddString ( strNAME );
			m_comboQUEST_PROC.SetItemData ( nIndex, dwQUESTID );

			if ( dwQUESTID==m_Talk.m_dwACTION_PARAM1 )
			{
				m_comboQUEST_PROC.SetCurSel ( nIndex );
			}
		}

		m_comboQUEST_PROC_STEP.ResetContent();
		GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( m_Talk.m_dwACTION_PARAM1 );
		if ( pQUEST )
		{
			DWORD dwSTEPNUM = pQUEST->GetSTEPNUM();
			for ( DWORD i=0; i<dwSTEPNUM; ++i )
			{
				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP(i);
				if ( !pSTEP )	continue;

				CString strNAME;
				strNAME.Format ( "[%d] %s", i, pSTEP->GetTITLE() );

				int nIndex = m_comboQUEST_PROC_STEP.AddString ( strNAME );
				m_comboQUEST_PROC_STEP.SetItemData ( nIndex, i );

				if ( i==m_Talk.m_dwACTION_PARAM2 )
				{
					m_comboQUEST_PROC_STEP.SetCurSel ( nIndex );
				}
			}
		}
	}

	if( nACTION == SNpcTalk::EM_QUEST_START )
	{
		GetDlgItem( IDC_STATIC_QUEST_STEP3 )->ShowWindow( SW_SHOW );
	}else{
		GetDlgItem( IDC_STATIC_QUEST_STEP3 )->ShowWindow( SW_HIDE );
	}


	BOOL bRandom = ( nACTION==SNpcTalk::EM_BASIC && dwNO == SNpcTalk::EM_RANDOM_PAGE );
	{
		DWORD dwWNDID[] = {
			IDC_RANDOM_STATIC,
			IDC_LIST_CASE_RANDOM,
			IDC_EDIT_RANDOM_TIME,
			IDC_TIME_STATIC,
			IDC_RANDOM_INSERT,
			IDC_RANDOM_DELETE,
			IDC_RANDOM_RESET,
		};

		DWORD dwWNDNUM = sizeof(dwWNDID)/sizeof(DWORD);

		for ( DWORD i=0; i<dwWNDNUM; ++i )
		{
			CWnd *pWnd = GetDlgItem ( dwWNDID[i] );
			if ( pWnd )
			{
				pWnd->ShowWindow ( bRandom );
			}
		}

		m_ctrlRandomID.DeleteAllItems ();

		for ( int nIndex = 0; nIndex < MAX_RANDOM_PAGE; ++nIndex ) 
		{
			DWORD dwID = m_Talk.m_dwRandomPageID[nIndex];

			if ( dwID == UINT_MAX ) break;

			
			CNpcDialogue* pNpcDialogue = m_pNpcDialogueSet->GetDialogue( dwID );

			if ( !pNpcDialogue ) break;

			DWORD dwIndex = pNpcDialogue->FindNonCondition();
			if ( dwIndex==UINT_MAX )	dwIndex = 0;

			CNpcDialogueCase* pDlgCase = pNpcDialogue->GetDlgCase(dwIndex);
			if ( !pDlgCase )	break;

			CString strID;
			strID.Format( "%u", dwID );

			int nItem = m_ctrlRandomID.InsertItem( nIndex, strID );			
			m_ctrlRandomID.SetItemText ( nIndex, 1, pDlgCase->GetBasicTalk().c_str() );
			m_ctrlRandomID.SetItemData ( nItem, (DWORD_PTR)dwID );
		}

		SetWin_Num_int( this, IDC_EDIT_RANDOM_TIME, m_Talk.m_dwRandomTime );

	}

	SetReadMode();

}

void CTalkPage::OnCbnSelchangeComboQuestStart(UINT nID)
{	
	int dwNum =  4 - (IDC_COMBO_QUEST_START5 - nID );

	int nIndex = m_comboQUESTSTART[dwNum].GetCurSel();
	if ( nIndex==LB_ERR )	return;

	DWORD dwDATA = (DWORD) m_comboQUESTSTART[dwNum].GetItemData ( nIndex );
	m_Talk.m_dwQuestStartID[dwNum] = dwDATA;	
}

void CTalkPage::OnCbnSelchangeComboQuestProg()
{
	int nIndex = m_comboQUEST_PROC.GetCurSel();
	if ( nIndex==LB_ERR )	return;

	DWORD dwDATA = (DWORD) m_comboQUEST_PROC.GetItemData ( nIndex );
	m_Talk.m_dwACTION_PARAM1 = dwDATA;
}

void CTalkPage::OnCbnSelchangeComboQuestProgStep()
{
	int nIndex = m_comboQUEST_PROC_STEP.GetCurSel();
	if ( nIndex==LB_ERR )	return;

	DWORD dwDATA = (DWORD) m_comboQUEST_PROC_STEP.GetItemData ( nIndex );
	m_Talk.m_dwACTION_PARAM2 = dwDATA;
}

void CTalkPage::OnBnClickedButtonBus()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "BUS Station List (*.ini)|*.ini|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".ini",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_BUS, dlg.GetFileName().GetString() );
	}
}

void CTalkPage::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum = 4;
	const int nSkipID[nSkipNum] = { IDC_EDIT_TALK, IDC_BUTTON_OK, IDC_BUTTON_CANCEL, IDC_LIST_CASE_RANDOM };

	int nID = 0;
	bool bOK = false;

	CWnd* pChildWnd = GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		bOK = false;
		nID = pChildWnd->GetDlgCtrlID();	

		for ( int i = 0; i < nSkipNum; ++i )
		{
			if ( nID == nSkipID[i] )
			{
				bOK = true;	
				break;
			}
		}

		if ( !bOK )	pChildWnd->EnableWindow( FALSE );

		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}


#endif
}
void CTalkPage::OnBnClickedRandomInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CDlgNpcTalk pDlg( m_pNpcDialogueSet );

	if ( IDOK == pDlg.DoModal() )
	{
		DWORD nID = pDlg.m_dwNpcID;

		if ( nID == UINT_MAX ) return;			
		
		CNpcDialogue* pNpcDialogue = m_pNpcDialogueSet->GetDialogue( nID );

		if ( !pNpcDialogue ) return;

		DWORD dwIndex = pNpcDialogue->FindNonCondition();
		if ( dwIndex==UINT_MAX )	dwIndex = 0;

		CNpcDialogueCase* pDlgCase = pNpcDialogue->GetDlgCase(dwIndex);
		if ( !pDlgCase )	return;		

		for ( int i = 0; i < MAX_RANDOM_PAGE; ++i ) 
		{
			if ( m_Talk.m_dwRandomPageID[i] == UINT_MAX ) 
			{
				m_Talk.m_dwRandomPageID[i] = nID;
				break;
			}
		}

		UpdateItems();
	}

}

void CTalkPage::OnBnClickedRandomDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nItem = -1;

	nItem = m_ctrlRandomID.GetNextItem( nItem, LVIS_SELECTED );
	if ( nItem < 0 || nItem >= MAX_RANDOM_PAGE ) return;	

	for ( int i = nItem+1; i < MAX_RANDOM_PAGE; ++i  ) 
	{
		m_Talk.m_dwRandomPageID[i-1] = m_Talk.m_dwRandomPageID[i];
	}
	
	m_Talk.m_dwRandomPageID[MAX_RANDOM_PAGE-1] = UINT_MAX;

	UpdateItems();

}

void CTalkPage::OnBnClickedRandomReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	for ( int i = 0; i < MAX_RANDOM_PAGE; ++i ) 
	{
		m_Talk.m_dwRandomPageID[i] = UINT_MAX;
	}

	UpdateItems();
}

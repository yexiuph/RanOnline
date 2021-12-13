// NpcActionAnswer.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "NpcAction.h"
#include "NpcActionAnswer.h"
#include "SheetWithTab.h"
#include "NpcTalk.h"
#include "stlFunctions.h"

//#include <strstream>
#include "EtcFunction.h"
#include ".\npcactionanswer.h"


// CNpcActionAnswer 대화 상자입니다.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CNpcActionAnswer, CPropertyPage)
CNpcActionAnswer::CNpcActionAnswer( LOGFONT logfont )
	: CPropertyPage(CNpcActionAnswer::IDD),
	m_pOrgDialogueCase(NULL),
	m_pNpcDialogueSet(NULL),
	m_pFont ( NULL )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CNpcActionAnswer::~CNpcActionAnswer()
{	
	SAFE_DELETE ( m_pFont );
}

void CNpcActionAnswer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANSWER, m_ctrlAnswer);
}


BEGIN_MESSAGE_MAP(CNpcActionAnswer, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_DELALL, OnBnClickedButtonDelall)
	ON_BN_CLICKED(IDC_BUTTON_CONDITON_EDIT, OnBnClickedButtonConditonEdit)
	ON_BN_CLICKED(IDC_BUTTON_CONDITON_DEL, OnBnClickedButtonConditonDel)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_ANSWER, OnNMDblclkListAnswer)
ON_BN_CLICKED(IDC_BUTTON_COPY, OnBnClickedButtonCopy)
END_MESSAGE_MAP()


// CNpcActionAnswer 메시지 처리기입니다.

void CNpcActionAnswer::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateItems ();

	if ( m_pOrgDialogueCase )	*m_pOrgDialogueCase = m_DummyDialogueCase;

	m_pSheetTab->SetActivePage ( NPCACTIONPAGE );
	m_pSheetTab->m_NpcActionPage.UpdateItems ();
}

BOOL CNpcActionAnswer::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	m_ctrlAnswer.SetExtendedStyle (
		m_ctrlAnswer.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	RECT	rect;
	m_ctrlAnswer.GetClientRect ( &rect );
	LONG	Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	const int	ColumnCount = 4;	
	char*		ColumnName[ColumnCount] = { "ID", "Answer", "Action Type", "Action Data" };
	int			ColumnWidthPercent[ColumnCount] = { 10, 50, 20, 20 };

	for ( int i = 0; i < ColumnCount; i++ )
	{		
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM
			| LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = ( Width * ColumnWidthPercent[i] ) / 100;
		m_ctrlAnswer.InsertColumn (i, &lvcolumn);		
	}

	SetReadMode();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CNpcActionAnswer::UpdateItems ()
{
	std::string strCondition;
	SNpcTalkCondition* pCondition = m_DummyDialogueCase.GetCondition();
	if ( pCondition )	pCondition->GetConditionText( strCondition );

	SetWin_Text ( this, IDC_EDIT_DIALOGUE_BASIC_TALK, m_DummyDialogueCase.GetBasicTalk().c_str() );
	SetWin_Text ( this, IDC_EDIT_DIALOGUE_CONDITION, strCondition.c_str() );

	m_ctrlAnswer.DeleteAllItems ();

	TCHAR szTemp[32] = {0};

	CNpcTalkControl::NPCMAP* pMap = m_DummyTalkControl.GetTalkMap ();

	CNpcTalkControl::NPCMAP_IT iter = pMap->begin();
	CNpcTalkControl::NPCMAP_IT iter_end = pMap->end();	
	for ( int nIndex = 0; iter!=iter_end; ++iter, nIndex++ )
	{
		SNpcTalk* pNpcTalk = (*iter).second;

		//std::strstream strNID;
		//strNID << pNpcTalk->m_dwNID << std::ends;

		_snprintf_s( szTemp, 32, "%u", pNpcTalk->m_dwNID );

		int nItem = m_ctrlAnswer.InsertItem ( nIndex, szTemp );		
		m_ctrlAnswer.SetItemText ( nIndex, 1, pNpcTalk->GetTalk() );
		m_ctrlAnswer.SetItemText ( nIndex, 2, SNpcTalk::strACTIONTYPE[pNpcTalk->m_nACTION].c_str() );


		std::string strACT_NO;
		switch ( pNpcTalk->m_nACTION )
		{
		case SNpcTalk::EM_PAGE_MOVE:
			m_pNpcDialogueSet->GetDlgText( pNpcTalk->m_dwACTION_NO, strACT_NO );
			break;

		case SNpcTalk::EM_BASIC:
			strACT_NO =  SNpcTalk::szBASICDESC[pNpcTalk->m_dwACTION_NO].c_str();
			break;

		case SNpcTalk::EM_QUEST_START:
			m_pNpcDialogueSet->GetDlgText( pNpcTalk->m_dwACTION_NO, strACT_NO );
			break;

		case SNpcTalk::EM_QUEST_STEP:
			m_pNpcDialogueSet->GetDlgText( pNpcTalk->m_dwACTION_NO, strACT_NO );
			break;

		default:
			//std::strstream strStream;
			//strStream << pNpcTalk->m_dwACTION_NO << std::ends;
			//strACT_NO = strStream.str();
			//strStream.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

			_snprintf_s( szTemp, 32, "%u", pNpcTalk->m_dwACTION_NO );
			strACT_NO = szTemp;
			break;
		};

		m_ctrlAnswer.SetItemText ( nIndex, 3, strACT_NO.c_str() );

		m_ctrlAnswer.SetItemData ( nItem, (DWORD_PTR)pNpcTalk->m_dwNID );

		//strNID.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
}

void CNpcActionAnswer::SetDialogueCase ( CNpcDialogueSet *pNpcDialogueSet, CNpcDialogueCase* pDialogueCase )
{	
	assert ( pDialogueCase && "CNpcActionAnswer::SetDialogueCase () 값 설정이 잘못되었습니다." );	

	m_DummyTalkControl.RemoveTalk ();
	m_DummyDialogueCase.Reset ();

	m_pNpcDialogueSet = pNpcDialogueSet;
	m_pOrgDialogueCase = pDialogueCase;
	m_DummyDialogueCase = *pDialogueCase;

	if ( m_DummyDialogueCase.GetTalkControl () )
	{
		m_DummyTalkControl = *(m_DummyDialogueCase.GetTalkControl ());
	}		

	UpdateItems ();
}

void CNpcActionAnswer::OnBnClickedButtonAdd()
{
	InverseUpdateItems ();

	m_pSheetTab->ActiveActionTalk ( NPCACTIONANSWER, m_pSheetTab->GetDialogueSet(), &m_DummyTalkControl, FALSE, NULL );

}

void CNpcActionAnswer::OnBnClickedButtonModify()
{
	POSITION	pos = m_ctrlAnswer.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlAnswer.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlAnswer.GetItemData ( nItem );

	m_pSheetTab->ActiveActionTalk ( NPCACTIONANSWER, m_pSheetTab->GetDialogueSet(), &m_DummyTalkControl, TRUE, m_DummyTalkControl.GetTalk(NativeID) );
}

void CNpcActionAnswer::OnBnClickedButtonDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION	pos = m_ctrlAnswer.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlAnswer.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return;
	}

	DWORD NativeID = (DWORD) m_ctrlAnswer.GetItemData ( nItem );
	CString BasicTalk = m_DummyTalkControl.GetTalk ( NativeID )->GetTalk();
	CString strTemp;
	strTemp.Format ( "Do you want to delete?\n[%03d] %s", NativeID, BasicTalk );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyTalkControl.DelTalk ( NativeID );
		UpdateItems ();
	}
}

void CNpcActionAnswer::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( NPCACTIONPAGE );
}


void CNpcActionAnswer::InverseUpdateItems ()
{
	CString strTalk = GetWin_Text ( this, IDC_EDIT_DIALOGUE_BASIC_TALK );

	m_DummyDialogueCase.SetBasicTalk ( strTalk.GetString() );

	m_DummyDialogueCase.ResetTalkControl ();
	if ( m_DummyTalkControl.GetTalkMap()->size () )
	{
		m_DummyDialogueCase.SetTalkControl ( m_DummyTalkControl );
	}
}
void CNpcActionAnswer::OnBnClickedButtonDelall()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp;
	strTemp.Format ( "Do you want to delete all?" );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyTalkControl.RemoveTalk ();

		UpdateItems ();
	}
}

void CNpcActionAnswer::OnBnClickedButtonConditonEdit()
{
	SNpcTalkCondition* pConditon = m_DummyDialogueCase.GetCondition();
	if ( !pConditon )
	{
		m_DummyDialogueCase.SetCondition ( SNpcTalkCondition() );
	}

	pConditon = m_DummyDialogueCase.GetCondition();

	m_pSheetTab->ActiveActionCondition ( NPCACTIONANSWER, pConditon );
}

void CNpcActionAnswer::OnBnClickedButtonConditonDel()
{
    if ( MessageBox ( "Do you want to delete?", "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyDialogueCase.ResetCondition();
		UpdateItems ();
	}
}

void CNpcActionAnswer::OnBnClickedButtonUp()
{
	POSITION	pos = m_ctrlAnswer.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlAnswer.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return;
	}

	DWORD dwNID = (DWORD) m_ctrlAnswer.GetItemData ( nItem );

	DWORD dwNID_NEW = m_DummyTalkControl.ToUp ( dwNID );
	if( UINT_MAX != dwNID_NEW ) 
	{
		UpdateItems ();
		m_ctrlAnswer.SetItemState( --nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

	}

	//std::strstream strNID;
	//strNID << dwNID_NEW << std::ends;

	//LVFINDINFO info;

	//info.flags = LVFI_PARTIAL|LVFI_STRING;
	//info.psz = strNID.str();

	//int nIndex = m_ctrlAnswer.FindItem ( &info );
	//m_ctrlAnswer.SetSelectedColumn ( nIndex );

	//strNID.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
}

void CNpcActionAnswer::OnBnClickedButtonDown()
{
	POSITION	pos = m_ctrlAnswer.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlAnswer.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return;
	}

	DWORD dwNID = (DWORD) m_ctrlAnswer.GetItemData ( nItem );

	DWORD dwNID_NEW = m_DummyTalkControl.ToDown ( dwNID );

	if( UINT_MAX != dwNID_NEW ) 
	{
		UpdateItems ();
		m_ctrlAnswer.SetItemState( ++nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

	}

	//std::strstream strNID;
	//strNID << dwNID_NEW << std::ends;

	//LVFINDINFO info;

	//info.flags = LVFI_PARTIAL|LVFI_STRING;
	//info.psz = strNID.str();

	//int nIndex = m_ctrlAnswer.FindItem ( &info );
	//m_ctrlAnswer.SetSelectedColumn ( nIndex );

	//strNID.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
}


void CNpcActionAnswer::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	

	const int nSkipNum = 5;
	const int nSkipID[nSkipNum] = { IDC_BUTTON_OK, IDC_BUTTON_CANCEL, IDC_EDIT_DIALOGUE_BASIC_TALK, 
									IDC_LIST_ANSWER, IDC_BUTTON_MODIFY };

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

void CNpcActionAnswer::OnNMDblclkListAnswer(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	NMITEMACTIVATE* pNMIA = (NMITEMACTIVATE*) pNMHDR;

	if ( pNMIA->iItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlAnswer.GetItemData ( pNMIA->iItem );

	m_pSheetTab->ActiveActionTalk ( NPCACTIONANSWER, m_pSheetTab->GetDialogueSet(), &m_DummyTalkControl, TRUE, m_DummyTalkControl.GetTalk(NativeID) );
}

void CNpcActionAnswer::OnBnClickedButtonCopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION	pos = m_ctrlAnswer.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlAnswer.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlAnswer.GetItemData ( nItem );
	
	SNpcTalk* pTalk = m_DummyTalkControl.GetTalk(NativeID);
	SNpcTalk* pNewTalk = new SNpcTalk;
	*pNewTalk = *pTalk;
	pNewTalk->m_dwNID = m_DummyTalkControl.MakeTalkNID ();
	m_DummyTalkControl.AddTalk( pNewTalk );

	UpdateItems ();
}

// NpcActionPage.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "NpcAction.h"
#include "NpcActionPage.h"
#include "SheetWithTab.h"

//#include <strstream>
#include "etcfunction.h"

#include "NpcTalk.h"
#include ".\npcactionpage.h"

// CNpcActionPage 대화 상자입니다.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CNpcActionPage, CPropertyPage)
CNpcActionPage::CNpcActionPage( LOGFONT logfont )
	: CPropertyPage(CNpcActionPage::IDD)
	, m_pFont ( NULL )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CNpcActionPage::~CNpcActionPage()
{
	SAFE_DELETE ( m_pFont );
}

void CNpcActionPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_POSTIVE_ANSWER2, m_listctrlCase);
	DDX_Control(pDX, IDC_LIST_CASE_ANSWER, m_listctrlAnswer);
}

BEGIN_MESSAGE_MAP(CNpcActionPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_CASE_EDIT, OnBnClickedButtonCaseEdit)
	ON_BN_CLICKED(IDC_BUTTON_CASE_DEL, OnBnClickedButtonCaseDel)
	ON_BN_CLICKED(IDC_BUTTON_CASE_NEW, OnBnClickedButtonCaseNew)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_POSTIVE_ANSWER2, OnLvnItemchangedListPostiveAnswer2)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_POSTIVE_ANSWER2, OnNMDblclkListPostiveAnswer2)
	ON_BN_CLICKED(IDC_BUTTON_CASE_COPY, OnBnClickedButtonCaseCopy)
END_MESSAGE_MAP()


// CNpcActionPage 메시지 처리기입니다.

void CNpcActionPage::OnBnClickedButtonOk()
{
	InverseUpdateItems ();

	if ( m_pDialogue )
	{
		*m_pDialogue = m_DummyDialogue;
	}
	else
	{
		
		DWORD dwIndex = m_DummyDialogue.FindNonCondition();
		if ( dwIndex==UINT_MAX )	dwIndex = 0;	
		if ( !m_DummyDialogue.GetDlgCase(dwIndex) )
		{
			m_pSheetTab->ActiveActionTable ();
			return;
		}

		CNpcDialogue* pNewDialogue = new CNpcDialogue;
		*pNewDialogue = m_DummyDialogue;

		CNpcDialogueSet* pDialogueSet = m_pSheetTab->GetDialogueSet ();
		if ( !pDialogueSet->AddDialogue ( pNewDialogue ) )
		{
			MessageBox ( "Registered the same ID already." );
			SAFE_DELETE ( pNewDialogue );
			return;
		}
	}

	m_pSheetTab->ActiveActionTable ();
}

void CNpcActionPage::OnBnClickedButtonCancel()
{
	m_pSheetTab->ActiveActionTable ();
}

void CNpcActionPage::SetDialogue ( CNpcDialogueSet *pNpcDialogueSet, CNpcDialogue* pDialogue )
{	
	m_DummyDialogue.Reset ();

	m_pNpcDialogueSet = pNpcDialogueSet;
	m_pDialogue = pDialogue;
    if ( m_pDialogue )
	{
		m_DummyDialogue = *m_pDialogue;		
	}
	else
	{
		DWORD NewNativeID = m_pSheetTab->GetDialogueSet()->MakeDlgNID();
		m_DummyDialogue.SetDlgNID ( NewNativeID );
	}
}

void CNpcActionPage::UpdateItems ()
{
	CString strTemp;

	//	Note : Dlg ID를 적용한다.
	//
	SetWin_Num_int ( this, IDC_EDIT_DIALOGUE_ID, m_DummyDialogue.GetDlgNID() );

	//	Note : 분기문 리스트를 갱신합니다.
	//
	m_listctrlCase.DeleteAllItems ();

	TCHAR szTemp[32] = {0};

	const CNpcDialogue::DLGCASE& sVecDlgCase = m_DummyDialogue.GetDlgCaseVec();
	CNpcDialogue::DLGCASE_CITER iter = sVecDlgCase.begin();
	CNpcDialogue::DLGCASE_CITER iter_end = sVecDlgCase.end();
	for ( DWORD dwIndex=0; iter!=iter_end; ++iter, ++dwIndex )
	{
		const CNpcDialogueCase* pCase = (*iter);
		SNpcTalkCondition *pCondition = pCase->GetCondition();
		std::string strCondition;
		if ( pCondition )	pCondition->GetConditionText( strCondition );

		//std::strstream strStream;
		//strStream << nIndex << std::ends;
		
		_snprintf_s( szTemp, 32, "%u", dwIndex );

		int nItem = m_listctrlCase.InsertItem ( dwIndex, szTemp );
		m_listctrlCase.SetItemData ( nItem, dwIndex );

		m_listctrlCase.SetItemText ( dwIndex, 1, pCase->GetBasicTalk().c_str() );
		m_listctrlCase.SetItemText ( dwIndex, 2, strCondition.c_str() );

		//std::strstream strStream2;
		//strStream2 << pCase->GetTalkNum() << std::ends;

		_snprintf_s( szTemp, 32, "%u", pCase->GetTalkNum() );

		m_listctrlCase.SetItemText ( dwIndex, 3, szTemp );

		//strStream.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		//strStream2.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}

	//	Note : 현제 선택된 분기문를 미리보기.
	//
	UpdateCase();
}

void CNpcActionPage::UpdateCase ()
{
	//	Note : 현제 선택된 분기문를 미리보기.
	//
	m_listctrlAnswer.DeleteAllItems ();

	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if ( pos != NULL)
	{
		int nItem = m_listctrlCase.GetNextSelectedItem(pos);
		DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );

		//	Note : 분기문 미리보기.
		//
		CNpcDialogueCase* pCase = m_DummyDialogue.GetDlgCase ( dwData );
		if ( pCase )
		{
			std::string strTalk = pCase->GetBasicTalk();
			
			std::string strCondition;
			SNpcTalkCondition* pCondition = pCase->GetCondition();
			if ( pCondition )	pCondition->GetConditionText( strCondition );

			SetWin_Text ( this, IDC_EDIT_DIALOGUE_CASE, strTalk.c_str() );
			SetWin_Text ( this, IDC_EDIT_DIALOGUE_CASE_CONDITION, strCondition.c_str() );

			CNpcTalkControl* pTalkCtrl = pCase->GetTalkControl();
			if ( pTalkCtrl )
			{
				TCHAR szTemp[32] = {0};

				const CNpcTalkControl::NPCMAP &mapTALK = *pTalkCtrl->GetTalkMap();
				CNpcTalkControl::NPCMAP_CIT iter = mapTALK.begin();
				CNpcTalkControl::NPCMAP_CIT iter_end = mapTALK.end();
				for ( int nIndex=0; iter!=iter_end; ++iter, ++nIndex )
				{
					SNpcTalk* pTalk = (*iter).second;

					pTalk->GetTalk();

					//std::strstream strNID;
					//strNID << pTalk->m_dwNID << std::ends;

					_snprintf_s( szTemp, 32, "%u", pTalk->m_dwNID );

					int nItem = m_listctrlAnswer.InsertItem ( nIndex, szTemp );		
					m_listctrlAnswer.SetItemText ( nIndex, 1, pTalk->GetTalk() );	//	ID
					m_listctrlAnswer.SetItemText ( nIndex, 2, SNpcTalk::strACTIONTYPE[pTalk->m_nACTION].c_str() );	//	ID
					
					std::string strACT_NO;
					switch ( pTalk->m_nACTION )
					{
					case SNpcTalk::EM_PAGE_MOVE:
						m_pNpcDialogueSet->GetDlgText ( pTalk->m_dwACTION_NO, strACT_NO );
						break;

					case SNpcTalk::EM_BASIC:
						strACT_NO = SNpcTalk::szBASICDESC[pTalk->m_dwACTION_NO].c_str();
						break;

					case SNpcTalk::EM_QUEST_START:
						m_pNpcDialogueSet->GetDlgText ( pTalk->m_dwACTION_NO, strACT_NO );
						break;

					case SNpcTalk::EM_QUEST_STEP:
						m_pNpcDialogueSet->GetDlgText ( pTalk->m_dwACTION_NO, strACT_NO );
						break;

					default:
						//std::strstream strStream;
						//strStream << pTalk->m_dwACTION_NO << std::ends;
						//strACT_NO = strStream.str();
						//strStream.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

						_snprintf_s( szTemp, 32, "%u", pTalk->m_dwACTION_NO );
						strACT_NO = szTemp;
						break;
					};

					m_listctrlAnswer.SetItemText ( nIndex, 3, strACT_NO.c_str() );

					m_listctrlAnswer.SetItemData ( nItem, (DWORD_PTR)pTalk->m_dwNID );

					//strNID.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				}
			}
		}
	}
	else
	{
		SetWin_Text ( this, IDC_EDIT_DIALOGUE_CASE, "" );
		SetWin_Text ( this, IDC_EDIT_DIALOGUE_CASE_CONDITION, "" );
	}
}

void CNpcActionPage::InverseUpdateItems ()
{
	CString strTemp;
	CEdit* pEdit = NULL;

	pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_DIALOGUE_ID );
	pEdit->GetWindowText ( strTemp );

	DWORD NativeID = (DWORD) atoi ( strTemp.GetString () );
	m_DummyDialogue.SetDlgNID ( NativeID );
}

BOOL CNpcActionPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_listctrlCase.SetExtendedStyle ( m_listctrlCase.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_listctrlCase.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	const int nColumnCount = 4;	
	char* szColumnName1[nColumnCount] = { "ID", "Text Of Conversation", "Condition", "Number Of Answer" };
	int nColumnWidthPercent1[nColumnCount] = { 10, 50, 20, 20 };

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent1[i] ) / 100;
		m_listctrlCase.InsertColumn (i, &lvColumn );
	}

	char* szColumnName2[nColumnCount] = { "ID", "Answer", "Action Type", "Data" };
	int nColumnWidthPercent2[nColumnCount] = { 10, 50, 20, 20 };

	m_listctrlAnswer.SetExtendedStyle ( m_listctrlAnswer.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_listctrlAnswer.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = szColumnName2[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent2[i] ) / 100;
		m_listctrlAnswer.InsertColumn (i, &lvColumn );
	}

	SetReadMode();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CNpcActionPage::OnLvnItemchangedListPostiveAnswer2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	//	Note : 현제 선택된 분기문를 미리보기.
	//
	UpdateCase();

	*pResult = 0;
}

void CNpcActionPage::OnBnClickedButtonCaseNew()
{
	CNpcDialogueCase sCase;
	DWORD dwSize = m_DummyDialogue.AddCase ( sCase );
	CNpcDialogueCase* pCase = m_DummyDialogue.GetDlgCase ( dwSize-1 );

	m_pSheetTab->ActiveActionAnswer ( m_pNpcDialogueSet, pCase );
}

void CNpcActionPage::OnBnClickedButtonCaseEdit()
{
	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( "Select the branching statement to edit.", "Information", MB_OK );
		return;
	}
	
	int nItem = m_listctrlCase.GetNextSelectedItem(pos);
	DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );

	//	Note : Case 편집 수행.
	//
	CNpcDialogueCase* pCase = m_DummyDialogue.GetDlgCase ( dwData );
	m_pSheetTab->ActiveActionAnswer ( m_pNpcDialogueSet, pCase );
}

void CNpcActionPage::OnBnClickedButtonCaseDel()
{
	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( "Select the branching statement to delete.", "Information", MB_OK );
		return;
	}
	
	if ( IDYES==MessageBox("Do you want to delete 'Branching Statement'?","Caution",MB_YESNO) )
	{
		int nItem = m_listctrlCase.GetNextSelectedItem(pos);
		DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );
		m_DummyDialogue.DelCase ( dwData );

		UpdateItems ();
	}
}


void CNpcActionPage::OnBnClickedButtonUp()
{
	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if (pos == NULL)	return;
	
	int nItem = m_listctrlCase.GetNextSelectedItem(pos);
	DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );
	
	DWORD dwNEW_ID = m_DummyDialogue.ToUp ( dwData );

	if ( dwNEW_ID!=dwData )
	{
		UpdateItems ();
		m_listctrlCase.SetItemState( --nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 
	}
}

void CNpcActionPage::OnBnClickedButtonDown()
{
	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if (pos == NULL)	return;
	
	int nItem = m_listctrlCase.GetNextSelectedItem(pos);
	DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );
	
	DWORD dwNEW_ID = m_DummyDialogue.ToDown ( dwData );

	if ( dwNEW_ID!=dwData )
	{
		UpdateItems ();
		m_listctrlCase.SetItemState( ++nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 
	}
}

void CNpcActionPage::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum = 5;
	const int nSkipID[nSkipNum] = { IDC_BUTTON_OK, IDC_BUTTON_CANCEL, IDC_BUTTON_CASE_EDIT, 
									IDC_LIST_POSTIVE_ANSWER2, IDC_LIST_CASE_ANSWER };

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


void CNpcActionPage::OnNMDblclkListPostiveAnswer2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	NMITEMACTIVATE* pNMIA = (NMITEMACTIVATE*) pNMHDR;

	if ( pNMIA->iItem == -1 )
	{
		return ;
	}

	DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( pNMIA->iItem );

	//	Note : Case 편집 수행.
	//
	CNpcDialogueCase* pCase = m_DummyDialogue.GetDlgCase ( dwData );
	m_pSheetTab->ActiveActionAnswer ( m_pNpcDialogueSet, pCase );
}

void CNpcActionPage::OnBnClickedButtonCaseCopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( "Select the branching statement to copy.", "Information", MB_OK );
		return;
	}
	
	int nItem = m_listctrlCase.GetNextSelectedItem(pos);
	DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );

	//	Note : Case 편집 수행.
	//
	CNpcDialogueCase* pCase = m_DummyDialogue.GetDlgCase ( dwData );
	
	CNpcDialogueCase sCase;
	
	sCase = *pCase;		
	m_DummyDialogue.AddCase ( sCase );
	
	UpdateItems ();

}

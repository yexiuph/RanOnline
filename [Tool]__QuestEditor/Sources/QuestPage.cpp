// QuestPage.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "QuestEdit.h"
#include "QuestPage.h"

#include "GLItemMan.h"
#include "GLSkill.h"
#include "EtcFunction.h"
#include "GLQuestMan.h"

#include "QuestEditDlg.h"
#include "DlgItem.h"
#include "DlgCustomItem.h"
#include "DlgSkill.h"
#include "DlgMapSelect.h"
#include "SheetWithTab.h"
#include ".\questpage.h"

// CQuestPage 대화 상자입니다.
IMPLEMENT_DYNAMIC(CQuestPage, CPropertyPage)
CQuestPage::CQuestPage( LOGFONT logfont )
	: CPropertyPage(CQuestPage::IDD),
	m_bDlgInit(false),
	m_pSheetTab(NULL),
	m_pFont ( NULL )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CQuestPage::~CQuestPage()
{
	SAFE_DELETE( m_pFont );
}

void CQuestPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SCHOOL, m_comboSchool);
}


BEGIN_MESSAGE_MAP(CQuestPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_ITEM_NEW, OnBnClickedButtonGiftItemNew)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_ITEM_EDIT, OnBnClickedButtonGiftItemEdit)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_ITEM_DEL, OnBnClickedButtonGiftItemDel)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_SKILL_NEW, OnBnClickedButtonGiftSkillNew)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_SKILL_DEL, OnBnClickedButtonGiftSkillDel)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_EDITCANCEL, OnBnClickedButtonEditcancel)
	ON_BN_CLICKED(IDC_BUTTON_EDITCOMPLATE, OnBnClickedButtonEditcomplate)
	ON_BN_CLICKED(IDC_USE_PROGRESS_CHECK, OnBnClickedUseProgressCheck)
	ON_CBN_SELCHANGE(IDC_STATE_COMBO1, OnCbnSelchangeStateCombo1)
	ON_CBN_SELCHANGE(IDC_STATE_COMBO2, OnCbnSelchangeStateCombo2)
	ON_CBN_SELCHANGE(IDC_STATE_COMBO3, OnCbnSelchangeStateCombo3)
	ON_BN_CLICKED(IDC_MAP_EDIT_BUTTON, OnBnClickedMapEditButton)
	ON_BN_CLICKED(IDC_FAIL_CHECK1, OnBnClickedFailCheck1)
	ON_BN_CLICKED(IDC_FAIL_CHECK2, OnBnClickedFailCheck2)
	ON_BN_CLICKED(IDC_FAIL_CHECK3, OnBnClickedFailCheck3)
	ON_CBN_SELCHANGE(IDC_MAP_SELECT_COMBO, OnCbnSelchangeTypeCombo)
	ON_BN_CLICKED(IDC_BUTTON_MOVEMAP, OnBnClickedButtonMovemap)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_MAN, OnBnClickedRadioMan)
	ON_BN_CLICKED(IDC_RADIO_WOMAN, OnBnClickedRadioWoman)
END_MESSAGE_MAP()


// CQuestPage 메시지 처리기입니다.

BOOL CQuestPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	m_comboSchool.ResetContent();
	for ( DWORD i = 0; i < MAX_SCHOOL; i++ )
	{
		std::string strSCHOOLNAME = GLCONST_CHAR::strSCHOOLNAME[i];
		if ( strSCHOOLNAME.empty() )
		{
			int nIndex = m_comboSchool.AddString ( "All School" );
			m_comboSchool.SetItemData ( nIndex, MAX_SCHOOL );
			break;
		}

		int nIndex = m_comboSchool.AddString ( strSCHOOLNAME.c_str() );
		m_comboSchool.SetItemData ( nIndex, i );
	}

	m_bDlgInit = true;

	//m_bStateMoveMap[0] = FALSE;
	//m_bStateMoveMap[1] = FALSE;
	//m_bStateMoveMap[2] = FALSE;
    
	//std::string strTemp[4];
	//strTemp[0] = "이벤트 없음";
	//strTemp[1] = "캐릭터 사망";
	//strTemp[2] = "시작지점으로 이동";
	//strTemp[3] = "지정된 위치로 이동";
	//SetWin_Combo_Init( this, IDC_STATE_COMBO1, strTemp, 4 );
	//SetWin_Combo_Init( this, IDC_STATE_COMBO2, strTemp, 4 );
	//SetWin_Combo_Init( this, IDC_STATE_COMBO3, strTemp, 4 );

	/*strTemp[0] = "시작시";
	strTemp[1] = "완료시";
	strTemp[2] = "실패시";
	SetWin_Combo_Init( this, IDC_MAP_SELECT_COMBO, strTemp, 3 );*/

	/*SetDlgItemText( IDC_MAP_MID_EDIT, "-1" );
	SetDlgItemText( IDC_MAP_SID_EDIT, "-1" );
	SetDlgItemText( IDC_MAP_XPOS_EDIT, "-1" );
	SetDlgItemText( IDC_MAP_YPOS_EDIT, "-1" );
	SetDlgItemText( IDC_MAP_GATE_EDIT, "-1" );*/
	UpdateItems ();

	SetReadMode ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CQuestPage::UpdateItems ()
{
	if ( !m_bDlgInit )	return;

	SetWin_Num_int ( this, IDC_EDIT_QUEST_NID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.dwID );
	SetWin_Text ( this, IDC_EDIT_QUEST_FILENAME, CQuestEditDlg::m_sQuestNode.strFILE.c_str() );
	SetWin_Num_int ( this, IDC_EDIT_EDITVERSION, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwEDITVER );

	SetWin_Text ( this, IDC_EDIT_TITLE, CQuestEditDlg::m_sQuestNode.pQUEST->m_strTITLE.c_str() );

	SetWin_Text ( this, IDC_EDIT_COMMENT, CQuestEditDlg::m_sQuestNode.pQUEST->m_strCOMMENT.c_str() );

	SetWin_Num_int ( this, IDC_EDIT_BEGIN_MONEY, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwBeginMoney );
	SetWin_Num_int ( this, IDC_EDIT_BEGIN_PARTYNUM, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwBeginPartyMemNum );

	SetWin_Num_int ( this, IDC_EDIT_LIMIT_TIME, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwLimitTime );
	SetWin_Num_int ( this, IDC_EDIT_LIMIT_PARTYNUM, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwLimitPartyMemNum );
	
	SetWin_Check ( this, IDC_CHECK_LIMIT_NONDIE, CQuestEditDlg::m_sQuestNode.pQUEST->m_bNonDie );

	SetWin_Check ( this, IDC_CHECK_REPEAT, CQuestEditDlg::m_sQuestNode.pQUEST->IsREPEAT() );
	SetWin_Check ( this, IDC_CHECK_AGAIN, CQuestEditDlg::m_sQuestNode.pQUEST->IsAGAIN() );
	SetWin_Check ( this, IDC_CHECK_ONLYONE_PROG, CQuestEditDlg::m_sQuestNode.pQUEST->IsONLYONE() );
	SetWin_Check ( this, IDC_CHECK_GIVEUP, CQuestEditDlg::m_sQuestNode.pQUEST->IsGIVEUP() );

	SetWin_Check ( this, IDC_CHECK_PARTY, CQuestEditDlg::m_sQuestNode.pQUEST->IsPARTY() );
	SetWin_Check ( this, IDC_CHECK_PARTYQUEST, CQuestEditDlg::m_sQuestNode.pQUEST->IsPARTYQUEST() );

	SetWin_Num_int ( this, IDC_EDIT_GIFT_EXP, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftEXP );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_MONEY, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftMONEY );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_ELEMENT, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftELEMENT );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_LIFEP, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftLIFEPOINT );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_DISPK, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftDisPK );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_SKILLP, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftSKILLPOINT );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_STATP, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftSTATSPOINT );

	SetWin_Num_int ( this, IDC_EDIT_GIFT_QUEST, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftQUEST );

	SetWin_Check ( this, IDC_CHECK_FIGHETER_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_FIGHTER_M) );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ARMS_M) );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ARCHER_W) );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_SPIRIT_W) );

	/* 클래스 추가 */
	SetWin_Check ( this, IDC_CHECK_FIGHETER_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_FIGHTER_W) );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ARMS_W) );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ARCHER_M) );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_SPIRIT_M) );

	SetWin_Check ( this, IDC_CHECK_EXTREME_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_EXTREME_M) );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_EXTREME_W) );

	//SetWin_Check ( this, IDC_CHECK_SCIENCE_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_SCIENTIST_M) ); //added by Joao 4/10/2012
	//SetWin_Check ( this, IDC_CHECK_SCIENCE_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_SCIENTIST_W) ); //added by Joao 4/10/2012

	//SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ASSASSIN_M) ); 
	//SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ASSASSIN_W) );

	std::string strSCHOOLNAME = "All School";
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wSCHOOL!=MAX_SCHOOL )		strSCHOOLNAME = GLCONST_CHAR::strSCHOOLNAME[CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wSCHOOL];
	SetWin_Combo_Sel ( this, IDC_COMBO_SCHOOL, strSCHOOLNAME.c_str() );

	SetWin_Num_int ( this, IDC_EDIT_START_ITEM_MID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidITEM.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_START_ITEM_SID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidITEM.wSubID );

	SetWin_Num_int ( this, IDC_EDIT_START_SKILL_MID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidSKILL.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_START_SKILL_SID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidSKILL.wSubID );

	SetWin_Num_int ( this, IDC_EDIT_START_LEVEL, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wLEVEL );

	SetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_MID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidMAP.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_SID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidMAP.wSubID );

	SetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_X, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wPosX );
	SetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_Y, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wPosY );

	CListBox *pListBox(NULL);
	
	{
		pListBox = (CListBox*) GetDlgItem ( IDC_LIST_GIFT_ITEM );
		pListBox->ResetContent();
		
		INVENQARRAY_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM.begin();
		INVENQARRAY_ITER iter_end = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM.end();
		for ( int i=0 ; iter!=iter_end; ++iter, ++i )
		{
			const SITEMCUSTOM &sITEM = (*iter);
			
			CString strNAME = "UNFOUND";
			SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sITEM.sNativeID );
			if ( pITEM )
			{
				strNAME.Format( "%d/%d %s", pITEM->sBasicOp.sNativeID.wMainID, pITEM->sBasicOp.sNativeID.wSubID,
								pITEM->GetName() );
			}

			int nIndex = pListBox->AddString ( strNAME );
			pListBox->SetItemData ( nIndex, i );
		}
	}

	{
		pListBox = (CListBox*) GetDlgItem ( IDC_LIST_GIFT_SKILL );
		pListBox->ResetContent();

		DWQARRAY_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL.begin();
		DWQARRAY_ITER iter_end = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL.end();
		for ( int i=0 ; iter!=iter_end; ++iter, ++i )
		{
			SNATIVEID sSKILLNID;
			sSKILLNID.dwID = (*iter);
			
			CString strNAME = "UNFOUND";
			PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( sSKILLNID );
			if ( pSKILL )	strNAME = pSKILL->GetName();

			int nIndex = pListBox->AddString ( strNAME );
			pListBox->SetItemData ( nIndex, i );
		}
	}

	//m_tempProgressEvent = CQuestEditDlg::m_sQuestNode.pQUEST->m_ProgressEvent;
	UpdateProgress();
}

void CQuestPage::InverseUpdateItems ()
{
	if ( !m_bDlgInit )	return;

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.dwID = (DWORD) GetWin_Num_int ( this, IDC_EDIT_QUEST_NID );
	CQuestEditDlg::m_sQuestNode.strFILE = GetWin_Text ( this, IDC_EDIT_QUEST_FILENAME );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwEDITVER = GetWin_Num_int ( this, IDC_EDIT_QUEST_FILENAME );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwEDITVER = GetWin_Num_int ( this, IDC_EDIT_EDITVERSION );	
	

	CQuestEditDlg::m_sQuestNode.pQUEST->m_strTITLE = GetWin_Text ( this, IDC_EDIT_TITLE ).GetString();

	CQuestEditDlg::m_sQuestNode.pQUEST->m_strCOMMENT = GetWin_Text ( this, IDC_EDIT_COMMENT ).GetString();

	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwBeginMoney = (DWORD) GetWin_Num_int ( this, IDC_EDIT_BEGIN_MONEY );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwBeginPartyMemNum = (DWORD) GetWin_Num_int ( this, IDC_EDIT_BEGIN_PARTYNUM );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwLimitTime = (DWORD ) GetWin_Num_int ( this, IDC_EDIT_LIMIT_TIME );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwLimitPartyMemNum = (DWORD) GetWin_Num_int ( this, IDC_EDIT_LIMIT_PARTYNUM );
	
	CQuestEditDlg::m_sQuestNode.pQUEST->m_bNonDie = GetWin_Check ( this, IDC_CHECK_LIMIT_NONDIE ) != FALSE;

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_REPEAT ), CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_REPEAT );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_AGAIN ), CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_AGAIN );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ONLYONE_PROG ), CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_ONLYONE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_GIVEUP ), CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_GIVEUP );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_PARTY ), CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_PARTY );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_PARTYQUEST ), CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_PARTYQUEST );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftEXP = GetWin_Num_int ( this, IDC_EDIT_GIFT_EXP );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftMONEY = GetWin_Num_int ( this, IDC_EDIT_GIFT_MONEY );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftELEMENT = GetWin_Num_int ( this, IDC_EDIT_GIFT_ELEMENT );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftLIFEPOINT = GetWin_Num_int ( this, IDC_EDIT_GIFT_LIFEP );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftDisPK = GetWin_Num_int ( this, IDC_EDIT_GIFT_DISPK );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftSKILLPOINT = GetWin_Num_int ( this, IDC_EDIT_GIFT_SKILLP );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftSTATSPOINT = GetWin_Num_int ( this, IDC_EDIT_GIFT_STATP );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftQUEST = GetWin_Num_int ( this, IDC_EDIT_GIFT_QUEST );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS = GLCC_NONE;
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_FIGHETER_M ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_FIGHTER_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARMS_M ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ARMS_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARCHER_W ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ARCHER_W );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SPIRIT_W ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_SPIRIT_W );

	/* 새 클래스 추가 */
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_FIGHETER_W ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_FIGHTER_W );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARMS_W ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ARMS_W );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARCHER_M ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ARCHER_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SPIRIT_M ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_SPIRIT_M );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EXTREME_M ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_EXTREME_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EXTREME_W ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_EXTREME_W );

	//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCIENCE_M ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_SCIENTIST_M ); //added by Joao 4/11/2012
	//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCIENCE_W ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_SCIENTIST_W ); //added by Joao 4/11/2012

	//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ASSASSIN_M ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ASSASSIN_M );
	//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ASSASSIN_W ), CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ASSASSIN_W );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wSCHOOL = (WORD) GetWin_Combo_Sel ( this, IDC_COMBO_SCHOOL );


	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidITEM.wMainID = GetWin_Num_int ( this, IDC_EDIT_START_ITEM_MID );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidITEM.wSubID = GetWin_Num_int ( this, IDC_EDIT_START_ITEM_SID );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidSKILL.wMainID = GetWin_Num_int ( this, IDC_EDIT_START_SKILL_MID );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidSKILL.wSubID = GetWin_Num_int ( this, IDC_EDIT_START_SKILL_SID );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wLEVEL = GetWin_Num_int ( this, IDC_EDIT_START_LEVEL );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidMAP.wMainID = GetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_MID );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidMAP.wSubID = GetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_SID );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wPosX = GetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_X );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wPosY = GetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_Y );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_ProgressEvent = m_tempProgressEvent;
}

void CQuestPage::OnBnClickedButtonGiftItemNew()
{
	InverseUpdateItems();

	CDlgItem sDlgItem;
	if ( IDOK == sDlgItem.DoModal () )
	{
		SITEMCUSTOM sITEMCUSTOM;
		sITEMCUSTOM.sNativeID = sDlgItem.m_nidITEM;

		SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sITEMCUSTOM.sNativeID );
		if ( !pITEM )	return;

		if ( pITEM->ISPILE() )	sITEMCUSTOM.wTurnNum = pITEM->GETAPPLYNUM();

		CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM.push_back(sITEMCUSTOM);

		UpdateItems ();
	}
}

void CQuestPage::OnBnClickedButtonGiftItemEdit()
{
	InverseUpdateItems();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_GIFT_ITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR == nIndex )	return;
	
	DWORD dwIndex = (DWORD) pListBox->GetItemData ( nIndex );
	SITEMCUSTOM &sITEM = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM[dwIndex];

	CDlgCustomItem sDlgCustom;
	sDlgCustom.m_sITEMCUSTOM = sITEM;

	if ( IDOK==sDlgCustom.DoModal() )
	{
		sITEM = sDlgCustom.m_sITEMCUSTOM;
	}
}

void CQuestPage::OnBnClickedButtonGiftItemDel()
{
	InverseUpdateItems();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_GIFT_ITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR == nIndex )	return;
	
	DWORD dwIndex = (DWORD) pListBox->GetItemData ( nIndex );
	SITEMCUSTOM &sITEM = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM[dwIndex];

	CString strNAME = "UNFOUND";
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sITEM.sNativeID );
	if ( pITEM )	strNAME = pITEM->GetName();

	int nReturn = MessageBox ( "Do you want to delete 'Item'?", strNAME, MB_YESNO );
	if ( nReturn==IDYES )
	{
		INVENQARRAY_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM.begin()+dwIndex;
		CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM.erase ( iter, iter+1 );

		UpdateItems ();
	}
}

void CQuestPage::OnBnClickedButtonGiftSkillNew()
{
	InverseUpdateItems();

	CDlgSkill sDlgSkill;

	if ( IDOK==sDlgSkill.DoModal() )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL.push_back ( sDlgSkill.m_nidSKILL.dwID );

		UpdateItems ();
	}
}

void CQuestPage::OnBnClickedButtonGiftSkillDel()
{
	InverseUpdateItems();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_GIFT_SKILL );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR == nIndex )	return;
	
	DWORD dwIndex = (DWORD) pListBox->GetItemData ( nIndex );
	DWORD &dwSKILL_NID = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL[dwIndex];

	SNATIVEID sSKILLNID;
	sSKILLNID.dwID = dwSKILL_NID;
	
	CString strNAME = "UNFOUND";
	PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( sSKILLNID );
	if ( pSKILL )	strNAME = pSKILL->GetName();

	int nReturn = MessageBox ( "Do you want to delete 'Skill'?", strNAME, MB_YESNO );
	if ( nReturn==IDYES )
	{
		DWQARRAY_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL.begin()+dwIndex;
		CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL.erase ( iter, iter+1 );

		UpdateItems ();
	}
}


void CQuestPage::OnBnClickedButtonPrev()
{
	//	아무것도 하지 않는다.
}

void CQuestPage::OnBnClickedButtonNext()
{
	InverseUpdateItems();
	GLQuestMan::GetInstance().DoModified ();

	//	다음 페이지로 이동.
	m_pSheetTab->ActiveQuestStepPage( QUEST_PAGE, NULL );
}

void CQuestPage::OnBnClickedButtonEditcancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	m_pSheetTab->ActiveQuestTreePage ();
}

void CQuestPage::OnBnClickedButtonEditcomplate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_tempProgressEvent.bUseProgressEvent )
	{
		if( m_tempProgressEvent.dwEndProgress != EMQP_NON_EVENT )
		{
			if( m_tempProgressEvent.dwfailType == 0 )
			{
				MessageBox( "실패 종류를 선택하세요", "알림" );
				return;
			}
		}

		if( m_tempProgressEvent.dwStartProgress == EMQP_SELECT_MOVE )
		{
			if( m_tempProgressEvent.startMap.nidMAP.wMainID == USHRT_MAX || 
				m_tempProgressEvent.startMap.nidMAP.wSubID  == USHRT_MAX ||
				(m_tempProgressEvent.startMap.wPosX == USHRT_MAX || 
				 m_tempProgressEvent.startMap.wPosY == USHRT_MAX ) &&
				 m_tempProgressEvent.startMap.dwGateID == 0 )
			{
				MessageBox( "시작시 이동할 맵과 위치나 게이트를 선택하셔야 합니다.", "알림" );
				return;
			}

			if( m_tempProgressEvent.startMap.wPosX != USHRT_MAX &&
				m_tempProgressEvent.startMap.wPosY != USHRT_MAX &&
				m_tempProgressEvent.startMap.dwGateID != 0 )
			{
				MessageBox( "시작시 이동할 맵과 게이트중에 하나만 설정하세요.", "알림" );
				return;
			}
		}
		if( m_tempProgressEvent.dwEndProgress == EMQP_SELECT_MOVE )
		{
			if( m_tempProgressEvent.endMap.nidMAP.wMainID == USHRT_MAX || 
				m_tempProgressEvent.endMap.nidMAP.wSubID  == USHRT_MAX ||
				(m_tempProgressEvent.endMap.wPosX == USHRT_MAX || 
				m_tempProgressEvent.endMap.wPosY == USHRT_MAX ) &&
				m_tempProgressEvent.endMap.dwGateID == 0 )
			{
				MessageBox( "완료시 이동할 맵과 위치나 게이트를 선택하셔야 합니다.", "알림" );
				return;
			}

			if( m_tempProgressEvent.endMap.wPosX != USHRT_MAX &&
				m_tempProgressEvent.endMap.wPosY != USHRT_MAX &&
				m_tempProgressEvent.endMap.dwGateID != 0 )
			{
				MessageBox( "완료시 이동할 맵과 게이트중에 하나만 설정하세요.", "알림" );
				return;
			}
		}
		if( m_tempProgressEvent.dwFailProgress == EMQP_SELECT_MOVE )
		{
			if( m_tempProgressEvent.failMap.nidMAP.wMainID == USHRT_MAX || 
				m_tempProgressEvent.failMap.nidMAP.wSubID  == USHRT_MAX ||
				(m_tempProgressEvent.failMap.wPosX == USHRT_MAX || 
				m_tempProgressEvent.failMap.wPosY == USHRT_MAX ) &&
				m_tempProgressEvent.failMap.dwGateID == 0 )
			{
				MessageBox( "실패시 이동할 맵과 위치나 게이트를 선택하셔야 합니다.", "알림" );
				return;
			}

			if( m_tempProgressEvent.failMap.wPosX != USHRT_MAX &&
				m_tempProgressEvent.failMap.wPosY != USHRT_MAX &&
				m_tempProgressEvent.failMap.dwGateID != 0 )
			{
				MessageBox( "실패시 이동할 맵과 게이트중에 하나만 설정하세요.", "알림" );
				return;
			}
		}
	}

	InverseUpdateItems();

	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos = m_mapQuestMap.find( CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.dwID );
	if ( iter_pos == m_mapQuestMap.end() ) return;

	
	if ( iter_pos->second.strFILE != CQuestEditDlg::m_sQuestNode.strFILE )
	{
		
		CString strTemp = GLQuestMan::GetInstance().GetPath();

		strTemp += iter_pos->second.strFILE.c_str();
		if ( !DeleteFile( strTemp ) )
		{
			MessageBox( "Not Found File",strTemp, MB_OK ) ;
		}

		iter_pos->second.strFILE = CQuestEditDlg::m_sQuestNode.strFILE;
	}	
	
	iter_pos->second.pQUEST = CQuestEditDlg::m_sQuestNode.pQUEST;	

	iter_pos->second.pQUEST->SAVE( iter_pos->second.strFILE.c_str() );

	m_pSheetTab->ActiveQuestTreePage ();
}

void CQuestPage::SetReadMode ()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망  
#ifdef READTOOL_PARAM

	SetWin_Enable( this, IDC_EDIT_QUEST_FILENAME, FALSE );
	SetWin_Enable( this, IDC_EDIT_EDITVERSION, FALSE );
	SetWin_Enable( this, IDC_EDIT_BEGIN_MONEY, FALSE );
	SetWin_Enable( this, IDC_EDIT_LIMIT_TIME, FALSE );
	SetWin_Enable( this, IDC_CHECK_LIMIT_NONDIE, FALSE );
	SetWin_Enable( this, IDC_CHECK_REPEAT, FALSE );
	SetWin_Enable( this, IDC_CHECK_AGAIN, FALSE );
	SetWin_Enable( this, IDC_CHECK_GIVEUP, FALSE );
	SetWin_Enable( this, IDC_CHECK_FIGHETER_M, FALSE );
	SetWin_Enable( this, IDC_CHECK_FIGHETER_W, FALSE );
	SetWin_Enable( this, IDC_CHECK_ARCHER_M, FALSE );
	SetWin_Enable( this, IDC_CHECK_ARCHER_W, FALSE );
	SetWin_Enable( this, IDC_CHECK_ARMS_M, FALSE );
	SetWin_Enable( this, IDC_CHECK_ARMS_W, FALSE );
	SetWin_Enable( this, IDC_CHECK_SPIRIT_M, FALSE );
	SetWin_Enable( this, IDC_CHECK_SPIRIT_W, FALSE );
	SetWin_Enable( this, IDC_CHECK_EXTREME_M, FALSE );
	SetWin_Enable( this, IDC_CHECK_EXTREME_W, FALSE );
	SetWin_Enable( this, IDC_CHECK_SCIENCE_M, FALSE );
	SetWin_Enable( this, IDC_CHECK_SCIENCE_W, FALSE );
	SetWin_Enable( this, IDC_CHECK_ASSASSIN_M, FALSE );
	SetWin_Enable( this, IDC_CHECK_ASSASSIN_W, FALSE );
	SetWin_Enable( this, IDC_COMBO_SCHOOL, FALSE );
	SetWin_Enable( this, IDC_EDIT_START_ITEM_MID, FALSE );
	SetWin_Enable( this, IDC_EDIT_START_ITEM_SID, FALSE );
	SetWin_Enable( this, IDC_EDIT_START_SKILL_MID, FALSE );
	SetWin_Enable( this, IDC_EDIT_START_SKILL_SID, FALSE );
	SetWin_Enable( this, IDC_EDIT_START_LEVEL, FALSE );
	SetWin_Enable( this, IDC_EDIT_START_QUEST_MAP_MID, FALSE );
	SetWin_Enable( this, IDC_EDIT_START_QUEST_MAP_SID, FALSE );
	SetWin_Enable( this, IDC_EDIT_START_QUEST_MAP_X, FALSE );
	SetWin_Enable( this, IDC_EDIT_START_QUEST_MAP_Y, FALSE );
	SetWin_Enable( this, IDC_EDIT_START_QUEST_MAP_RADIUS, FALSE );
	SetWin_Enable( this, IDC_EDIT_GIFT_EXP, FALSE );
	SetWin_Enable( this, IDC_EDIT_GIFT_MONEY, FALSE );
	SetWin_Enable( this, IDC_EDIT_GIFT_ELEMENT, FALSE );
	SetWin_Enable( this, IDC_EDIT_GIFT_LIFEP, FALSE );
	SetWin_Enable( this, IDC_EDIT_GIFT_DISPK, FALSE );
	SetWin_Enable( this, IDC_EDIT_GIFT_SKILLP, FALSE );
	SetWin_Enable( this, IDC_EDIT_GIFT_STATP, FALSE );
	SetWin_Enable( this, IDC_EDIT_GIFT_QUEST, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GIFT_ITEM_NEW, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GIFT_ITEM_EDIT, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GIFT_ITEM_DEL, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GIFT_SKILL_NEW, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GIFT_SKILL_DEL, FALSE );

#endif
}
void CQuestPage::OnBnClickedUseProgressCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*if( GetWin_Check( this, IDC_USE_PROGRESS_CHECK ) == BST_CHECKED )
	{
		SetWin_Enable( this, IDC_STATE_COMBO1, TRUE );
		SetWin_Enable( this, IDC_STATE_COMBO2, TRUE );
		SetWin_Enable( this, IDC_STATE_COMBO3, TRUE );
		SetWin_Enable( this, IDC_FAIL_CHECK1, TRUE );
		SetWin_Enable( this, IDC_FAIL_CHECK2, TRUE );
		SetWin_Enable( this, IDC_FAIL_CHECK3, TRUE );
		SetWin_Enable( this, IDC_MAP_SELECT_COMBO, TRUE );
		SetWin_Enable( this, IDC_MAP_MID_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_SID_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_XPOS_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_YPOS_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_GATE_EDIT, TRUE );

		m_tempProgressEvent.bUseProgressEvent = TRUE;

		UpdateProgress();
	}else{
		SetWin_Enable( this, IDC_STATE_COMBO1, FALSE );
		SetWin_Enable( this, IDC_STATE_COMBO2, FALSE );
		SetWin_Enable( this, IDC_STATE_COMBO3, FALSE );
		SetWin_Enable( this, IDC_FAIL_CHECK1, FALSE );
		SetWin_Enable( this, IDC_FAIL_CHECK2, FALSE );
		SetWin_Enable( this, IDC_FAIL_CHECK3, FALSE );
		SetWin_Enable( this, IDC_MAP_SELECT_COMBO, FALSE );
		SetWin_Enable( this, IDC_MAP_EDIT_BUTTON, FALSE );
		SetWin_Enable( this, IDC_MAP_MID_EDIT, FALSE );
		SetWin_Enable( this, IDC_MAP_SID_EDIT, FALSE );
		SetWin_Enable( this, IDC_MAP_XPOS_EDIT, FALSE );
		SetWin_Enable( this, IDC_MAP_YPOS_EDIT, FALSE );
		SetWin_Enable( this, IDC_MAP_GATE_EDIT, FALSE );

		m_tempProgressEvent.bUseProgressEvent = FALSE;

		m_tempProgressEvent.Init();
		UpdateProgress();
	}*/

}

void CQuestPage::UpdateProgress()
{
	/*if( m_tempProgressEvent.bUseProgressEvent )
	{
		SetWin_Enable( this, IDC_STATE_COMBO1, TRUE );
		SetWin_Enable( this, IDC_STATE_COMBO2, TRUE );
		SetWin_Enable( this, IDC_STATE_COMBO3, TRUE );
		SetWin_Enable( this, IDC_FAIL_CHECK1, TRUE );
		SetWin_Enable( this, IDC_FAIL_CHECK2, TRUE );
		SetWin_Enable( this, IDC_FAIL_CHECK3, TRUE );
		SetWin_Enable( this, IDC_MAP_SELECT_COMBO, TRUE );
		SetWin_Enable( this, IDC_MAP_MID_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_SID_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_XPOS_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_YPOS_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_GATE_EDIT, TRUE );

		SetWin_Check( this, IDC_USE_PROGRESS_CHECK, TRUE );
	}else{
		SetWin_Enable( this, IDC_STATE_COMBO1, FALSE );
		SetWin_Enable( this, IDC_STATE_COMBO2, FALSE );
		SetWin_Enable( this, IDC_STATE_COMBO3, FALSE );
		SetWin_Enable( this, IDC_FAIL_CHECK1, FALSE );
		SetWin_Enable( this, IDC_FAIL_CHECK2, FALSE );
		SetWin_Enable( this, IDC_FAIL_CHECK3, FALSE );
		SetWin_Enable( this, IDC_MAP_SELECT_COMBO, FALSE );
		SetWin_Enable( this, IDC_MAP_EDIT_BUTTON, FALSE );
		SetWin_Enable( this, IDC_MAP_MID_EDIT, FALSE );
		SetWin_Enable( this, IDC_MAP_SID_EDIT, FALSE );
		SetWin_Enable( this, IDC_MAP_XPOS_EDIT, FALSE );
		SetWin_Enable( this, IDC_MAP_YPOS_EDIT, FALSE );
		SetWin_Enable( this, IDC_MAP_GATE_EDIT, FALSE );		
		SetWin_Check( this, IDC_USE_PROGRESS_CHECK, FALSE );
	}

	UpdateProgressType(0, TRUE);
	UpdateProgressType(1, TRUE);
	UpdateProgressType(2, TRUE);

	SetWin_Check( this, IDC_FAIL_CHECK1, m_tempProgressEvent.IsTIMEOVER() );
	SetWin_Check( this, IDC_FAIL_CHECK2, m_tempProgressEvent.IsDIE() );
	SetWin_Check( this, IDC_FAIL_CHECK3, m_tempProgressEvent.IsLEAVE() );

	OnCbnSelchangeTypeCombo();

	if( m_bStateMoveMap[0] == FALSE && 
		m_bStateMoveMap[1] == FALSE &&
		m_bStateMoveMap[2] == FALSE )
	{
		SetWin_Enable( this, IDC_MAP_EDIT_BUTTON, FALSE );
	}else{
		SetWin_Enable( this, IDC_MAP_EDIT_BUTTON, TRUE );
	}*/
}

void CQuestPage::UpdateProgressType( DWORD type, bool bNew /*= FALSE*/ )
{

	/*DWORD comboSel = 0;
	char szTempChar[16] = {0,};
	
	if( bNew == TRUE )
	{	
		if( type == 0 )
		{
			comboSel = (DWORD)m_tempProgressEvent.dwStartProgress ;
			strcpy( szTempChar, "시작시" );
		}else if( type == 1 ){
			comboSel = (DWORD)m_tempProgressEvent.dwEndProgress;
			strcpy( szTempChar, "완료시" );
		}else{
			comboSel = (DWORD)m_tempProgressEvent.dwFailProgress;
			strcpy( szTempChar, "실패시" );
		}
		SetWin_Combo_Sel( this, IDC_STATE_COMBO1+type, comboSel );

	}else{
		comboSel = (DWORD)GetWin_Combo_Sel( this, IDC_STATE_COMBO1+type );

		if( type == 0 )
		{
			m_tempProgressEvent.dwStartProgress = (EMQUESTPROGRESS)comboSel;
			strcpy( szTempChar, "시작시" );
		}else if( type == 1 ){
			m_tempProgressEvent.dwEndProgress   = (EMQUESTPROGRESS)comboSel;
			strcpy( szTempChar, "완료시" );
		}else{
			m_tempProgressEvent.dwFailProgress  = (EMQUESTPROGRESS)comboSel;
			strcpy( szTempChar, "실패시" );
		}
	}

	if( comboSel == 3  )
	{
		if( m_bStateMoveMap[type] == FALSE )
		{
			CComboBox *pCombo = (CComboBox*) ( GetDlgItem(IDC_MAP_SELECT_COMBO) );
			int nIndex = pCombo->AddString( szTempChar );
			pCombo->SetItemData ( nIndex, nIndex );
			pCombo->SetCurSel ( 0 );

		}
		m_bStateMoveMap[type] = TRUE;
	}else{
		m_bStateMoveMap[type] = FALSE;
		SetWin_Combo_Del( this, IDC_MAP_SELECT_COMBO, szTempChar );
		CComboBox *pCombo = (CComboBox*) ( GetDlgItem(IDC_MAP_SELECT_COMBO) );
		pCombo->SetCurSel ( 0 );
	}

	if( m_bStateMoveMap[0] == FALSE && 
		m_bStateMoveMap[1] == FALSE &&
		m_bStateMoveMap[2] == FALSE )
	{
		SetWin_Enable( this, IDC_MAP_EDIT_BUTTON, FALSE );
	}else{
		SetWin_Enable( this, IDC_MAP_EDIT_BUTTON, TRUE );
	}*/


}

void CQuestPage::OnCbnSelchangeStateCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//UpdateProgressType( 0 );
}

void CQuestPage::OnCbnSelchangeStateCombo2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//UpdateProgressType( 1 );
}

void CQuestPage::OnCbnSelchangeStateCombo3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//UpdateProgressType( 2 );
}

void CQuestPage::OnBnClickedMapEditButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*CDlgMapSelect mapSelect;
	mapSelect.DoModal();

	DWORD comboSel = (DWORD)GetWin_Combo_Sel( this, IDC_MAP_SELECT_COMBO );
	char szTempChar[8] = {0,};
	sprintf( szTempChar, "%d", mapSelect.m_nidMAP.wMainID );
	SetDlgItemText( IDC_MAP_MID_EDIT, szTempChar );
	sprintf( szTempChar, "%d", mapSelect.m_nidMAP.wSubID );
	SetDlgItemText( IDC_MAP_SID_EDIT, szTempChar );*/
}

void CQuestPage::OnBnClickedFailCheck1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*if( GetWin_Check( this, IDC_FAIL_CHECK1 ) ==  BST_CHECKED )
	{
		if( m_tempProgressEvent.IsTIMEOVER() == FALSE )
			m_tempProgressEvent.dwfailType += EMFAIL_TIMEOVER;
	}else{
		if( m_tempProgressEvent.IsTIMEOVER() == TRUE )
			m_tempProgressEvent.dwfailType -= EMFAIL_TIMEOVER;
	}*/
}

void CQuestPage::OnBnClickedFailCheck2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*if( GetWin_Check( this, IDC_FAIL_CHECK2 ) ==  BST_CHECKED )
	{
		if( m_tempProgressEvent.IsDIE() == FALSE )
			m_tempProgressEvent.dwfailType += EMFAIL_DIE;
	}else{
		if( m_tempProgressEvent.IsDIE() == TRUE )
			m_tempProgressEvent.dwfailType -= EMFAIL_DIE;
	}*/
}

void CQuestPage::OnBnClickedFailCheck3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*if( GetWin_Check( this, IDC_FAIL_CHECK3 ) ==  BST_CHECKED )
	{
		if( m_tempProgressEvent.IsLEAVE() == FALSE )
			m_tempProgressEvent.dwfailType += EMFAIL_LEAVE;
	}else{
		if( m_tempProgressEvent.IsLEAVE() == TRUE )
			m_tempProgressEvent.dwfailType -= EMFAIL_LEAVE;
	}*/
}

void CQuestPage::OnCbnSelchangeTypeCombo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*std::string str = GetWin_Combo_Sel_Text( this, IDC_MAP_SELECT_COMBO );
	GLQUEST_PROGRESS_MAP mapInfo;
	if( str == "시작시" )	{
		mapInfo = m_tempProgressEvent.startMap;
	}else if( str == "완료시" ){
		mapInfo = m_tempProgressEvent.endMap;
	}else{
		mapInfo = m_tempProgressEvent.failMap;
	}

	char szTempChar[8] = {0,};
	sprintf( szTempChar, "%d", mapInfo.nidMAP.wMainID );
	SetDlgItemText( IDC_MAP_MID_EDIT, szTempChar );
	sprintf( szTempChar, "%d", mapInfo.nidMAP.wSubID );
	SetDlgItemText( IDC_MAP_SID_EDIT, szTempChar );
	sprintf( szTempChar, "%d", mapInfo.wPosX );
	SetDlgItemText( IDC_MAP_XPOS_EDIT, szTempChar );
	sprintf( szTempChar, "%d", mapInfo.wPosY );
	SetDlgItemText( IDC_MAP_YPOS_EDIT, szTempChar );
	sprintf( szTempChar, "%d", mapInfo.dwGateID );
	SetDlgItemText( IDC_MAP_GATE_EDIT, szTempChar );*/
}

void CQuestPage::OnBnClickedButtonMovemap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	/*GLQUEST_PROGRESS_MAP mapInfo;
	char szTempChar[8] = {0,};
	GetDlgItemText( IDC_MAP_XPOS_EDIT, szTempChar, 8 );
	mapInfo.wPosX = atoi(szTempChar);	
	GetDlgItemText( IDC_MAP_YPOS_EDIT, szTempChar, 8 );
	mapInfo.wPosY = atoi(szTempChar);
	GetDlgItemText( IDC_MAP_GATE_EDIT, szTempChar, 8 );
	mapInfo.dwGateID = atoi(szTempChar);
	GetDlgItemText( IDC_MAP_MID_EDIT, szTempChar, 8 );
	mapInfo.nidMAP.wMainID = atoi(szTempChar);
	GetDlgItemText( IDC_MAP_SID_EDIT, szTempChar, 8 );
	mapInfo.nidMAP.wSubID = atoi(szTempChar);

	std::string str = GetWin_Combo_Sel_Text( this, IDC_MAP_SELECT_COMBO );
	DWORD dwProgress;
	if( str == "시작시" )	{
		dwProgress = m_tempProgressEvent.dwStartProgress;
	}else if( str == "완료시" ){
		dwProgress = m_tempProgressEvent.dwEndProgress;
	}else{
		dwProgress = m_tempProgressEvent.dwFailProgress;
	}

	if( mapInfo.nidMAP.wMainID == USHRT_MAX || 
		mapInfo.nidMAP.wSubID  == USHRT_MAX  )
	{
		MessageBox( "이동할 맵을 선택하셔야 합니다.", "알림" );
		return;
	}

	if( dwProgress == EMQP_SELECT_MOVE )
	{
		if( ( mapInfo.wPosX == USHRT_MAX || 
			mapInfo.wPosY == USHRT_MAX ) &&
			mapInfo.dwGateID == 0 )
		{
			MessageBox( "이동할 맵의 위치나 게이트를 설정하셔야 합니다.", "알림" );
			return;
		}

		if( mapInfo.wPosX != USHRT_MAX &&
			mapInfo.wPosY != USHRT_MAX &&
			mapInfo.dwGateID != 0 )
		{
			MessageBox( "이동할 맵 위치와 게이트중에 하나만 설정하세요.", "알림" );
			return;
		}
	}

	
	if( str == "시작시" )	{
		m_tempProgressEvent.startMap = mapInfo;		
	}else if( str == "완료시" ){
		m_tempProgressEvent.endMap   = mapInfo;
	}else{
		m_tempProgressEvent.failMap  = mapInfo;
	}*/
}

void CQuestPage::OnBnClickedRadioAll()
{
	// TODO: Add your control notification handler code here
	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_ALL );

	SetWin_Check ( this, IDC_CHECK_FIGHETER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, bCheck );

	SetWin_Check ( this, IDC_CHECK_FIGHETER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, bCheck );

	SetWin_Check ( this, IDC_CHECK_EXTREME_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, bCheck );

	SetWin_Check ( this, IDC_CHECK_SCIENCE_M, bCheck ); 
	SetWin_Check ( this, IDC_CHECK_SCIENCE_W, bCheck ); 

	SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, bCheck ); 
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, bCheck );
}

void CQuestPage::OnBnClickedRadioMan()
{
	// TODO: Add your control notification handler code here
	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_MAN );

	SetWin_Check ( this, IDC_CHECK_FIGHETER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_EXTREME_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_SCIENCE_M, bCheck ); 
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, bCheck ); 

	SetWin_Check ( this, IDC_CHECK_FIGHETER_W, false );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, false );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, false );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, false );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, false );
	SetWin_Check ( this, IDC_CHECK_SCIENCE_W, false ); 
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, false ); 
}

void CQuestPage::OnBnClickedRadioWoman()
{
	// TODO: Add your control notification handler code here
	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_WOMAN );

	SetWin_Check ( this, IDC_CHECK_FIGHETER_M, false );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, false );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, false );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, false );
	SetWin_Check ( this, IDC_CHECK_EXTREME_M, false );
	SetWin_Check ( this, IDC_CHECK_SCIENCE_M, false ); 
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, false ); 

	SetWin_Check ( this, IDC_CHECK_FIGHETER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_SCIENCE_W, bCheck ); 
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, bCheck ); 
}

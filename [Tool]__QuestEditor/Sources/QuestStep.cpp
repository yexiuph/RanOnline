// QuestStep.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "QuestEdit.h"
#include "QuestStep.h"

#include "GLMapList.h"
#include "GLCrowData.h"
#include "GLItemMan.h"
#include "EtcFunction.h"
#include "GLQuestMan.h"

#include "QuestEditDlg.h"

#include "DlgMap.h"
#include "DlgNPC.h"
#include "DlgItem.h"
#include "DlgCustomItem.h"
#include "DlgSkill.h"
#include "DlgMapSelect.h"
#include "SheetWithTab.h"
#include "DlgMobGenItem.h"
#include ".\queststep.h"

// CQuestStepPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CQuestStepPage, CPropertyPage)
CQuestStepPage::CQuestStepPage( LOGFONT logfont )
	: CPropertyPage(CQuestStepPage::IDD),
	m_dwCurStep(UINT_MAX),
	m_bDlgInit(false),
	m_pSheetTab(NULL),
	m_pFont ( NULL )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CQuestStepPage::~CQuestStepPage()
{
	SAFE_DELETE( m_pFont ) ;
}

void CQuestStepPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CQuestStepPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TALKNPC, OnBnClickedButtonTalknpc)
	ON_BN_CLICKED(IDC_BUTTON_DIEMOB, OnBnClickedButtonDiemob)
	ON_BN_CLICKED(IDC_BUTTON_GUARDNPC, OnBnClickedButtonGuardnpc)
	ON_BN_CLICKED(IDC_BUTTON_REACH_MAP, OnBnClickedButtonReachMap)
	ON_BN_CLICKED(IDC_BUTTON_GUARD_MAP, OnBnClickedButtonGuardMap)
	ON_LBN_SELCHANGE(IDC_LIST_STEP, OnLbnSelchangeListStep)
	ON_BN_CLICKED(IDC_BUTTON_REQ_QITEM_NEW, OnBnClickedButtonReqQitemNew)
	ON_BN_CLICKED(IDC_BUTTON_REQ_QITEM_EDIT, OnBnClickedButtonReqQitemEdit)
	ON_BN_CLICKED(IDC_BUTTON_REQ_QITEM_DEL, OnBnClickedButtonReqQitemDel)
	ON_BN_CLICKED(IDC_BUTTON_DEL_QITEM_NEW, OnBnClickedButtonDelQitemNew)
	ON_BN_CLICKED(IDC_BUTTON_DEL_QITEM_DEL, OnBnClickedButtonDelQitemDel)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_STEP_NEW, OnBnClickedButtonStepNew)
	ON_BN_CLICKED(IDC_BUTTON_REQ_STEP_DEL, OnBnClickedButtonReqStepDel)
	ON_BN_CLICKED(IDC_BUTTON_TALKNPC_DEL, OnBnClickedButtonTalknpcDel)
	ON_BN_CLICKED(IDC_BUTTON_DIEMOB_DEL, OnBnClickedButtonDiemobDel)
	ON_BN_CLICKED(IDC_BUTTON_GUARDNPC_DEL, OnBnClickedButtonGuardnpcDel)
	ON_BN_CLICKED(IDC_BUTTON_REACH_MAP_DEL, OnBnClickedButtonReachMapDel)
	ON_BN_CLICKED(IDC_BUTTON_GUARD_MAP_DEL, OnBnClickedButtonGuardMapDel)
	ON_BN_CLICKED(IDC_BUTTON_STEP_OK, OnBnClickedButtonStepOk)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_QITEM_NEW, OnBnClickedButtonGiftQitemNew)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_QITEM_EDIT, OnBnClickedButtonGiftQitemEdit)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_QITEM_DEL, OnBnClickedButtonGiftQitemDel)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_LBN_DBLCLK(IDC_LIST_STEP, OnLbnDblclkListStep)
	ON_BN_CLICKED(IDC_BUTTON_EDITCANCEL, OnBnClickedButtonEditcancel)
	ON_BN_CLICKED(IDC_BUTTON_EDITCOMPLATE, OnBnClickedButtonEditcomplate)
	ON_BN_CLICKED(IDC_STEP_MAP_EDIT_BUTTON, OnBnClickedStepMapEditButton)
END_MESSAGE_MAP()

// CQuestStepPage 메시지 처리기입니다.
BOOL CQuestStepPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_NONE );
	
	m_bDlgInit = true;

	UpdateItems();	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CQuestStepPage::UpdateItems ()
{
	if ( !m_bDlgInit )														return;

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );

	pListBox->ResetContent();

	GLQUEST::VECQSTEP_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.begin();
	GLQUEST::VECQSTEP_ITER iter_end = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.end();
	for ( int i=0; iter!=iter_end; ++iter, ++i )
	{
		const GLQUEST_STEP &sSTEP = (*iter);

		int nIndex = pListBox->AddString ( sSTEP.GetTITLE () );
		pListBox->SetItemData ( nIndex, i );
	}

	UpdateItems_Step ();
}

void CQuestStepPage::VisibleItems_Step ( bool bVisible )
{
	if ( !m_bDlgInit )														return;

	DWORD dwITEMS[] =
	{
		IDC_EDIT_STEP_TITLE,

		IDC_STATIC_COMMENT,
		IDC_EDIT_STEP_COMMENT,
		
		IDC_EDIT_TALKNPC_COMMENT,
		IDC_BUTTON_TALKNPC,
		IDC_EDIT_TALKNPC_NAME,
		IDC_BUTTON_TALKNPC_DEL,
		
		IDC_EDIT_DIEMOB_COMMENT,
		IDC_BUTTON_DIEMOB,
		IDC_EDIT_DIEMOB_NAME,
		IDC_BUTTON_DIEMOB_DEL,
		IDC_EDIT_DIEMOB_NUM,

		IDC_EDIT_GUARDNPC_COMMENT,
		IDC_BUTTON_GUARDNPC,
		IDC_EDIT_GUARDNPC_NAME,
		IDC_BUTTON_GUARDNPC_DEL,

		IDC_EDIT_TALKNPC_NUM,
		IDC_EDIT_TALKNPC_NUM2,
		IDC_EDIT_TALKNPC_NUM3,

		IDC_EDIT_REQ_QITEM_COMMENT,
		IDC_STATIC_REQ_QITEM,
		IDC_LIST_REQ_QITEM,
		IDC_BUTTON_REQ_QITEM_NEW,
		IDC_BUTTON_REQ_QITEM_EDIT,
		IDC_BUTTON_REQ_QITEM_DEL,
		
		IDC_EDIT_REACH_MAP_COMMENT,
		IDC_BUTTON_REACH_MAP,
		IDC_EDIT_REACH_MAP_NAME,
		IDC_BUTTON_REACH_MAP_DEL,
		IDC_STATIC_REACH,
		IDC_EDIT_REACH_PX,
		IDC_EDIT_REACH_PY,
		IDC_STATIC_REACH_R,
		IDC_EDIT_REACH_RADIUS,

		IDC_EDIT_GUARD_MAP_COMMENT,
		IDC_BUTTON_GUARD_MAP,
		IDC_EDIT_GUARD_MAP_NAME,
		IDC_BUTTON_GUARD_MAP_DEL,
		IDC_STATIC_DEFENSE,
		IDC_EDIT_GUARD_PX,
		IDC_EDIT_GUARD_PY,
		IDC_STATIC_DEFENSE_R,
		IDC_EDIT_GUARD_RADIUS,
		IDC_STATIC_DEFENSE_T,
		IDC_EDIT_GUARD_TIME,

		IDC_STATIC_LEVEL,
		IDC_EDIT_LEVEL,

		IDC_STATIC_GIFT_QITEM,
		IDC_LIST_GIFT_QITEM,
		IDC_BUTTON_GIFT_QITEM_NEW,
		IDC_BUTTON_GIFT_QITEM_EDIT,
		IDC_BUTTON_GIFT_QITEM_DEL,

		IDC_STATIC_RESET_QITEM,
		IDC_LIST_DEL_QITEM,
		IDC_BUTTON_DEL_QITEM_NEW,
		IDC_BUTTON_DEL_QITEM_DEL,

		IDC_STEP_MAP_MID_EDIT,
		IDC_STEP_MAP_SID_EDIT,
		IDC_STEP_MAP_XPOS_EDIT,
		IDC_STEP_MAP_YPOS_EDIT,
		IDC_STEP_MAP_GATE_EDIT,
		IDC_STEP_MAPID_STATIC,
		IDC_STEP_XY_STATIC,
		IDC_STEP_GATE_STATIC,
		IDC_STEP_MAP_EDIT_BUTTON,

		IDC_BUTTON_STEP_OK
	};

	DWORD dwINV_ITEMS[] =
	{
		IDC_BUTTON_PREV,
		IDC_BUTTON_NEXT,
		IDC_BUTTON_EDITCANCEL,
		IDC_BUTTON_EDITCOMPLATE,

	};

	int nSize = (int) sizeof(dwITEMS)/sizeof(DWORD);

	DWORD dwITEMS_DISABLE[] =
	{
		IDC_LIST_STEP,
		IDC_BUTTON_STEP_NEW,
	};

	int nSizeDisable = (int) sizeof(dwITEMS_DISABLE)/sizeof(DWORD);

	for ( int i=0; i<nSize; ++i )
	{
		CWnd *pWnd = GetDlgItem ( dwITEMS[i] );
		if ( pWnd )
		{
			pWnd->ShowWindow ( bVisible );
		}
	}

	nSize = (int) sizeof(dwINV_ITEMS)/sizeof(DWORD);
	for ( int i=0; i<nSize; ++i )
	{
		CWnd *pWnd = GetDlgItem ( dwINV_ITEMS[i] );
		if ( pWnd )
		{
			pWnd->ShowWindow ( !bVisible );
		}
	}

	for ( int i=0; i<nSizeDisable; ++i )
	{
		CWnd *pWnd = GetDlgItem ( dwITEMS_DISABLE[i] );
		if ( pWnd )
		{
			pWnd->EnableWindow ( !bVisible );
		}
	}

	SetReadMode();
}

void CQuestStepPage::UpdateItems_Step ()
{
	if ( !m_bDlgInit )														return;

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );
	int nIndex = pListBox->GetCurSel();
	if ( nIndex==LB_ERR )
	{
		VisibleItems_Step(false);
		return;
	}

	DWORD dwStep = (DWORD) pListBox->GetItemData ( nIndex );
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= dwStep )
	{
		VisibleItems_Step(false);
		return;
	}

	VisibleItems_Step(true);
	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[dwStep];

	m_dwCurStep = dwStep;

	PCROWDATA pCROW(NULL);
	SMAPNODE* pNODE(NULL);

	{
		CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_REQ_QITEM );
		pListBox->ResetContent();
		GENMOBITEMARRAY_ITER iter = sSTEP.m_vecMOBGEN_QITEM.begin();
		GENMOBITEMARRAY_ITER iter_end = sSTEP.m_vecMOBGEN_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			const SGENQUESTITEM &sCUSTION = (*iter);

			CString strNAME = "UNFOUND";
			SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sCUSTION.sNID );
			if ( pITEM )
			{
				strNAME.Format( "%d/%d %s", pITEM->sBasicOp.sNativeID.wMainID, pITEM->sBasicOp.sNativeID.wSubID,
								pITEM->GetName() );
			}

			int nIndex = pListBox->AddString ( strNAME.GetString() );
			DWORD dwData = DWORD(iter-sSTEP.m_vecMOBGEN_QITEM.begin());
			pListBox->SetItemData ( nIndex, dwData );
		}
	}

	{
		CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_DEL_QITEM );
		pListBox->ResetContent();
		DWQARRAY_ITER iter = sSTEP.m_vecRESET_QITEM.begin();
		DWQARRAY_ITER iter_end = sSTEP.m_vecRESET_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			const DWORD &dwDATA = (*iter);

			CString strNAME = "UNFOUND";
			SITEM* pITEM = GLItemMan::GetInstance().GetItem ( SNATIVEID(dwDATA) );
			if ( pITEM )	strNAME = pITEM->GetName();

			int nIndex = pListBox->AddString ( strNAME.GetString() );
			DWORD dwData = DWORD(iter-sSTEP.m_vecRESET_QITEM.begin());
			pListBox->SetItemData ( nIndex, dwData );
		}
	}

	{
		CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_GIFT_QITEM );
		pListBox->ResetContent();
		INVENQARRAY_ITER iter = sSTEP.m_vecGIFT_QITEM.begin();
		INVENQARRAY_ITER iter_end = sSTEP.m_vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			const SITEMCUSTOM &sDATA = (*iter);

			CString strNAME = "UNFOUND";
			SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sDATA.sNativeID );
			if ( pITEM )	strNAME = pITEM->GetName();

			int nIndex = pListBox->AddString ( strNAME.GetString() );

			DWORD dwData = DWORD(iter-sSTEP.m_vecGIFT_QITEM.begin());
			pListBox->SetItemData ( nIndex, dwData );
		}
	}

	SetWin_Text ( this, IDC_EDIT_STEP_TITLE, sSTEP.GetTITLE() );
	SetWin_Text ( this, IDC_EDIT_STEP_COMMENT, sSTEP.GetCOMMENT() );

	//	이수조건 1, 대화할 npc
	SetWin_Text ( this, IDC_EDIT_TALKNPC_COMMENT, sSTEP.GetOBJ_NPCTALK() );

	CString strCROWNAME;
	pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_NPCTALK) );
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	else			strCROWNAME = "";

	SetWin_Text ( this, IDC_EDIT_TALKNPC_NAME, strCROWNAME.GetString() );

	SNATIVEID sID = SNATIVEID( sSTEP.m_dwNID_NPCTALK );
	CString strNpcNum;
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM, strNpcNum );

	//	이수조건 2, MOB 발생 퀘스트 아이템
	SetWin_Text ( this, IDC_EDIT_REQ_QITEM_COMMENT, sSTEP.GetOBJ_MOBGEN_QITEM() );

	//	이수조건 3, 죽여야할 mob
	SetWin_Text ( this, IDC_EDIT_DIEMOB_COMMENT, sSTEP.GetOBJ_MOBKILL() );

	pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_MOBKILL) );
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	else			strCROWNAME = "";
	SetWin_Text ( this, IDC_EDIT_DIEMOB_NAME, strCROWNAME );
	SetWin_Num_int ( this, IDC_EDIT_DIEMOB_NUM, sSTEP.m_dwNUM_MOBKILL );

	sID = SNATIVEID( sSTEP.m_dwNID_MOBKILL );
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM2, strNpcNum );

	//	이수조건 3, 보호할 npc
	SetWin_Text ( this, IDC_EDIT_GUARDNPC_COMMENT, sSTEP.GetOBJ_NPCGUARD() );
	pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_NPCGUARD) );
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	else			strCROWNAME = "";
	SetWin_Text ( this, IDC_EDIT_GUARDNPC_NAME, strCROWNAME );

	sID = SNATIVEID( sSTEP.m_dwNID_NPCGUARD );
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM3, strNpcNum );

	//	이수조건 5, 도달해야할 map, 위치.
	SetWin_Text ( this, IDC_EDIT_REACH_MAP_COMMENT, sSTEP.GetOBJ_REACH_ZONE() );

	CString strReachMap;
	pNODE = CQuestEditDlg::m_sMapList.FindMapNode ( sSTEP.m_sMAPID_REACH );
	if ( pNODE )	strReachMap = pNODE->strMapName.c_str();

	SetWin_Text ( this, IDC_EDIT_REACH_MAP_NAME, strReachMap );

	SetWin_Num_int ( this, IDC_EDIT_REACH_PX, sSTEP.m_wPOSX_REACH );
	SetWin_Num_int ( this, IDC_EDIT_REACH_PY, sSTEP.m_wPOSY_REACH );
	SetWin_Num_int ( this, IDC_EDIT_REACH_RADIUS, sSTEP.m_wRADIUS_REACH );

	//	이수조건 6, 수성해야할 map, 위치.
	SetWin_Text ( this, IDC_EDIT_GUARD_MAP_COMMENT, sSTEP.GetOBJ_DEFENSE_ZONE() );

	CString strDefenseMap;
	pNODE = CQuestEditDlg::m_sMapList.FindMapNode ( sSTEP.m_sMAPID_DEFENSE );
	if ( pNODE )	strDefenseMap = pNODE->strMapName.c_str();

	SetWin_Text ( this, IDC_EDIT_GUARD_MAP_NAME, strDefenseMap );
	
	SetWin_Num_int ( this, IDC_EDIT_GUARD_PX, sSTEP.m_wPOSX_DEFENSE );
	SetWin_Num_int ( this, IDC_EDIT_GUARD_PY, sSTEP.m_wPOSY_DEFENSE );
	SetWin_Num_int ( this, IDC_EDIT_GUARD_RADIUS, sSTEP.m_wRADIUS_DEFENSE );
	SetWin_Num_float ( this, IDC_EDIT_GUARD_TIME, sSTEP.m_fDEFENSE_TIME );

	//	이수조건 7, 도달해야할 레벨.
	SetWin_Num_int ( this, IDC_EDIT_LEVEL, sSTEP.m_wLevel );

	//  완료후 이동할 맵
	SetWin_Num_int ( this, IDC_STEP_MAP_MID_EDIT, sSTEP.m_stepMoveMap.nidMAP.wMainID );
	SetWin_Num_int ( this, IDC_STEP_MAP_SID_EDIT, sSTEP.m_stepMoveMap.nidMAP.wSubID );
	SetWin_Num_int ( this, IDC_STEP_MAP_XPOS_EDIT, sSTEP.m_stepMoveMap.wPosX );
	SetWin_Num_int ( this, IDC_STEP_MAP_YPOS_EDIT, sSTEP.m_stepMoveMap.wPosY );
	SetWin_Num_int ( this, IDC_STEP_MAP_GATE_EDIT, sSTEP.m_stepMoveMap.dwGateID );
}

void CQuestStepPage::InverseUpdateItems ()
{
	if ( !m_bDlgInit )														return;

	InverseUpdateItems_Step ();
}

void CQuestStepPage::InverseUpdateItems_Step ()
{
	if ( !m_bDlgInit )														return;

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_strTITLE = GetWin_Text ( this, IDC_EDIT_STEP_TITLE ).GetString();
	sSTEP.m_strCOMMENT = GetWin_Text ( this, IDC_EDIT_STEP_COMMENT ).GetString();

	//	이수조건 1, 대화할 npc
	sSTEP.m_strOBJ_NPCTALK = GetWin_Text ( this, IDC_EDIT_TALKNPC_COMMENT );

	//	이수조건 2, MOB 발생 퀘스트 아이템
	sSTEP.m_strOBJ_MOBGEN_QITEM = GetWin_Text ( this, IDC_EDIT_REQ_QITEM_COMMENT );

	//	이수조건 3, 죽여야할 mob
	sSTEP.m_strOBJ_MOBKILL =GetWin_Text ( this, IDC_EDIT_DIEMOB_COMMENT );
	sSTEP.m_dwNUM_MOBKILL = (DWORD) GetWin_Num_int ( this, IDC_EDIT_DIEMOB_NUM );

	//	이수조건 4, 보호할 npc
	sSTEP.m_strOBJ_NPCGUARD = GetWin_Text ( this, IDC_EDIT_GUARDNPC_COMMENT );

	//	이수조건 5, 도달해야할 map, 위치.
	sSTEP.m_strOBJ_REACH_ZONE = GetWin_Text ( this, IDC_EDIT_REACH_MAP_COMMENT );

	sSTEP.m_wPOSX_REACH = (WORD) GetWin_Num_int ( this, IDC_EDIT_REACH_PX );
	sSTEP.m_wPOSY_REACH = (WORD) GetWin_Num_int ( this, IDC_EDIT_REACH_PY );
	sSTEP.m_wRADIUS_REACH = (WORD) GetWin_Num_int ( this, IDC_EDIT_REACH_RADIUS );

	//	이수조건 6, 수성해야할 map, 위치.
	sSTEP.m_strOBJ_DEFENSE_ZONE = GetWin_Text ( this, IDC_EDIT_GUARD_MAP_COMMENT );

	sSTEP.m_wPOSX_DEFENSE = (WORD) GetWin_Num_int ( this, IDC_EDIT_GUARD_PX );
	sSTEP.m_wPOSY_DEFENSE = (WORD) GetWin_Num_int ( this, IDC_EDIT_GUARD_PY );
	sSTEP.m_wRADIUS_DEFENSE = (WORD) GetWin_Num_int ( this, IDC_EDIT_GUARD_RADIUS );
	sSTEP.m_fDEFENSE_TIME = GetWin_Num_float ( this, IDC_EDIT_GUARD_TIME );

	//	이수조건 7, 도달해야할 레벨.
	sSTEP.m_wLevel = (WORD) GetWin_Num_int ( this, IDC_EDIT_LEVEL );
}

void CQuestStepPage::OnBnClickedButtonStepNew()
{
	//	Note : 이전 정보 백업.
	//
	InverseUpdateItems_Step ();
	m_dwCurStep = UINT_MAX;
	
	//	Note : 새로운 Step 넣기.
	CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.push_back ( GLQUEST_STEP() );

	//	Note : 리스트 갱신.
	UpdateItems ();

	//	Note : 새로 추가된 Step 선택.
	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );

	DWORD dwCurSel = (DWORD) CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size()-1;
	pListBox->SetCurSel(dwCurSel);

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonReqStepDel()
{
	InverseUpdateItems ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );
	int nIndex = pListBox->GetCurSel();
	if ( nIndex==LB_ERR )							return;

	DWORD dwStep = (DWORD) pListBox->GetItemData ( nIndex );
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= dwStep )	return;

	//	Note : 이전 정보 백업.
	//
	InverseUpdateItems_Step ();
	m_dwCurStep = UINT_MAX;

	pListBox->SetCurSel(LB_ERR);

	int nReturn = MessageBox ( "Do you want to delete Quest Step of present?", "Caution", MB_YESNO );
	if ( nReturn==IDYES )
	{
		GLQUEST::VECQSTEP_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.begin()+dwStep;
		CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.erase ( iter, iter+1 );
	}

	UpdateItems ();
}

void CQuestStepPage::OnLbnSelchangeListStep()
{
}

void CQuestStepPage::OnLbnDblclkListStep()
{
	m_dwCurStep = UINT_MAX;

	//	Note : 이전 정보 백업.
	//
	InverseUpdateItems_Step ();

	//	Note : 현제 선택 정보 표시.
	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonTalknpc()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgNPC sDlgNpc;
	sDlgNpc.m_bMob = false;
	if ( IDOK==sDlgNpc.DoModal() )
	{
		sSTEP.m_dwNID_NPCTALK = sDlgNpc.m_nidNPC.dwID;
	}

	PCROWDATA pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_NPCTALK) );

	CString strCROWNAME = "";
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	SetWin_Text ( this, IDC_EDIT_TALKNPC_NAME, strCROWNAME );

	SNATIVEID sID = SNATIVEID( sSTEP.m_dwNID_NPCTALK );
	CString strNpcNum;
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM, strNpcNum );
}

void CQuestStepPage::OnBnClickedButtonDiemob()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgNPC sDlgNpc;
	sDlgNpc.m_bMob = true;
	if ( IDOK==sDlgNpc.DoModal() )
	{
		sSTEP.m_dwNID_MOBKILL = sDlgNpc.m_nidNPC.dwID;
	}

	PCROWDATA pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_MOBKILL) );

	CString strCROWNAME = "";
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	SetWin_Text ( this, IDC_EDIT_DIEMOB_NAME, strCROWNAME );

	SNATIVEID sID = SNATIVEID( sSTEP.m_dwNID_MOBKILL );
	CString strNpcNum;
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM2, strNpcNum );
}

void CQuestStepPage::OnBnClickedButtonGuardnpc()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];


	CDlgNPC sDlgNpc;
	sDlgNpc.m_bMob = false;
	if ( IDOK==sDlgNpc.DoModal() )
	{
		sSTEP.m_dwNID_NPCGUARD = sDlgNpc.m_nidNPC.dwID;
	}

	PCROWDATA pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_NPCGUARD) );
	
	CString strCROWNAME = "";
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	SetWin_Text ( this, IDC_EDIT_GUARDNPC_NAME, strCROWNAME );

	SNATIVEID sID = SNATIVEID( sSTEP.m_dwNID_NPCGUARD );
	CString strNpcNum;
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM3, strNpcNum );
}

void CQuestStepPage::OnBnClickedButtonReachMap()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgMap sDlgMap;
	if ( IDOK==sDlgMap.DoModal() )
	{
		sSTEP.m_sMAPID_REACH = sDlgMap.m_nidMAP;
	}

	CString strReachMap;
	SMAPNODE* pNODE = CQuestEditDlg::m_sMapList.FindMapNode ( sSTEP.m_sMAPID_REACH );
	if ( pNODE )	strReachMap = pNODE->strMapName.c_str();

	SetWin_Text ( this, IDC_EDIT_REACH_MAP_NAME, strReachMap );
}

void CQuestStepPage::OnBnClickedButtonGuardMap()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgMap sDlgMap;
	if ( IDOK==sDlgMap.DoModal() )
	{
		sSTEP.m_sMAPID_DEFENSE = sDlgMap.m_nidMAP;
	}

	CString strDefenseMap;
	SMAPNODE* pNODE = CQuestEditDlg::m_sMapList.FindMapNode ( sSTEP.m_sMAPID_DEFENSE );
	if ( pNODE )	strDefenseMap = pNODE->strMapName.c_str();

	SetWin_Text ( this, IDC_EDIT_GUARD_MAP_NAME, strDefenseMap );
}

void CQuestStepPage::OnBnClickedButtonReqQitemNew()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgItem sDlgItem;
	if ( IDOK == sDlgItem.DoModal () )
	{
		SGENQUESTITEM sGENITEM;
		sGENITEM.sNID = sDlgItem.m_nidITEM;

		SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sGENITEM.sNID );
		if ( !pITEM )	return;

		if ( pITEM->ISPILE() )	sGENITEM.wNUM = pITEM->GETAPPLYNUM();

		sSTEP.m_vecMOBGEN_QITEM.push_back(sGENITEM);

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonReqQitemEdit()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_REQ_QITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR==nIndex )	return;

	DWORD dwData = (DWORD) pListBox->GetItemData ( nIndex );

	SGENQUESTITEM &sGENITEM = sSTEP.m_vecMOBGEN_QITEM[dwData];

	CDlgMobGenItem sDlgGenItem;
	sDlgGenItem.m_sGENQITEM = sGENITEM;

	if ( IDOK==sDlgGenItem.DoModal() )
	{
		sGENITEM = sDlgGenItem.m_sGENQITEM;
	}
}

void CQuestStepPage::OnBnClickedButtonReqQitemDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_REQ_QITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR==nIndex )	return;

	DWORD dwData = (DWORD) pListBox->GetItemData ( nIndex );

	SGENQUESTITEM &sGENITEM = sSTEP.m_vecMOBGEN_QITEM[dwData];

	CString strNAME = "UNFOUND";
	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sGENITEM.sNID );
	if ( pITEM )	strNAME = pITEM->GetName();

	if ( IDYES==MessageBox ( "Do you want to delete 'Item'?", strNAME, MB_YESNO ) )
	{
		sSTEP.MOBGENITEM_ERASE ( dwData );

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonDelQitemNew()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgItem sDlgItem;
	if ( IDOK == sDlgItem.DoModal () )
	{
		sSTEP.m_vecRESET_QITEM.push_back ( sDlgItem.m_nidITEM.dwID );

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonDelQitemDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_DEL_QITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR==nIndex )	return;

	DWORD dwData = (DWORD) pListBox->GetItemData ( nIndex );

	SNATIVEID sRESET_NID(sSTEP.m_vecRESET_QITEM[dwData]);

	CString strNAME = "UNFOUND";
	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sRESET_NID );
	if ( pITEM )	strNAME = pITEM->GetName();

	if ( IDYES==MessageBox ( "Do you want to delete 'Item'?", strNAME, MB_YESNO ) )
	{
		sSTEP.RESETITEM_ERASE ( dwData );

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonPrev()
{
	InverseUpdateItems();
	GLQuestMan::GetInstance().DoModified ();

	//	이전 페이지로 이동.
	m_pSheetTab->ActiveQuestPage( QUESTSTEP_PAGE, NULL );
}


void CQuestStepPage::OnBnClickedButtonTalknpcDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_dwNID_NPCTALK = UINT_MAX;

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonDiemobDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_dwNID_MOBKILL = UINT_MAX;

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonGuardnpcDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_dwNID_NPCGUARD = UINT_MAX;

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonReachMapDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_sMAPID_REACH = SNATIVEID(false);

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonGuardMapDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_sMAPID_DEFENSE = SNATIVEID(false);

	UpdateItems_Step ();
}


void CQuestStepPage::OnBnClickedButtonStepOk()
{

	// 완료할 맵 정보가 제대로 된 것 인지 체크
	GLQUEST_PROGRESS_MAP checkMap;
	checkMap.nidMAP.wMainID = GetWin_Num_int ( this, IDC_STEP_MAP_MID_EDIT );
	checkMap.nidMAP.wSubID   = GetWin_Num_int ( this, IDC_STEP_MAP_SID_EDIT );
	checkMap.wPosX		   = GetWin_Num_int ( this, IDC_STEP_MAP_XPOS_EDIT );
	checkMap.wPosY		   = GetWin_Num_int ( this, IDC_STEP_MAP_YPOS_EDIT );
	checkMap.dwGateID	   = GetWin_Num_int ( this, IDC_STEP_MAP_GATE_EDIT );


	if( checkMap.nidMAP.wMainID != USHRT_MAX &&	checkMap.nidMAP.wSubID  != USHRT_MAX )
	{
		if( (checkMap.wPosX == USHRT_MAX || checkMap.wPosY == USHRT_MAX ) && checkMap.dwGateID == 0 )
		{
			MessageBox( "이동할 맵과 위치나 게이트를 선택하셔야 합니다.", "알림" );
			return;
		}

		if( checkMap.wPosX != USHRT_MAX && checkMap.wPosY != USHRT_MAX && checkMap.dwGateID != 0 )
		{
			MessageBox( "이동할 맵과 게이트중에 하나만 설정하세요.", "알림" );
			return;
		}

		if( checkMap.nidMAP.wMainID  != UINT_MAX &&
			checkMap.nidMAP.wSubID   != UINT_MAX &&
			(checkMap.wPosX != UINT_MAX &&
			checkMap.wPosY != UINT_MAX ||
			checkMap.dwGateID != 0 ) )
			CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep].m_stepMoveMap = checkMap;
	}


	//	Note : 이전 정보 백업.
	//
	InverseUpdateItems_Step ();
	m_dwCurStep = UINT_MAX;
	
	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_REQ_QITEM );
	pListBox->SetCurSel(LB_ERR);

	//	Note : 리스트 갱신.
	UpdateItems ();
}

void CQuestStepPage::OnBnClickedButtonGiftQitemNew()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )							return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgItem sDlgItem;
	if ( IDOK == sDlgItem.DoModal () )
	{ 
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.sNativeID = sDlgItem.m_nidITEM;

		sSTEP.m_vecGIFT_QITEM.push_back ( sCUSTOM );

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonGiftQitemEdit()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_GIFT_QITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR==nIndex )	return;

	DWORD dwData = (DWORD) pListBox->GetItemData ( nIndex );

	SITEMCUSTOM &sCUSTOM = sSTEP.m_vecGIFT_QITEM[dwData];

	CDlgCustomItem sDlgCustom;
	sDlgCustom.m_sITEMCUSTOM = sCUSTOM;

	if ( IDOK==sDlgCustom.DoModal() )
	{
		sCUSTOM = sDlgCustom.m_sITEMCUSTOM;
	}
}

void CQuestStepPage::OnBnClickedButtonGiftQitemDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )							return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_GIFT_QITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR==nIndex )	return;

	DWORD dwData = (DWORD) pListBox->GetItemData ( nIndex );

	const SITEMCUSTOM sCUSTOM = sSTEP.m_vecGIFT_QITEM[dwData];

	CString strNAME = "UNFOUND";
	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sCUSTOM.sNativeID );
	if ( pITEM )	strNAME = pITEM->GetName();

	if ( IDYES==MessageBox ( "Do you want to delete 'Item'?", strNAME, MB_YESNO ) )
	{
		sSTEP.GIFTITEM_ERASE ( dwData );

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonUp()
{
	InverseUpdateItems ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );
	int nIndex = pListBox->GetCurSel();
	if ( nIndex==LB_ERR )							return;

	DWORD dwID = (DWORD) pListBox->GetItemData ( nIndex );

	dwID = CQuestEditDlg::m_sQuestNode.pQUEST->ToUpStep(dwID);

	pListBox->SetCurSel ( dwID );

	//	Note : 리스트 갱신.
	UpdateItems ();
	UpdateItems_Step ();

	pListBox->SetCurSel ( dwID );
}

void CQuestStepPage::OnBnClickedButtonDown()
{
	InverseUpdateItems ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );
	int nIndex = pListBox->GetCurSel();
	if ( nIndex==LB_ERR )							return;

	DWORD dwID = (DWORD) pListBox->GetItemData ( nIndex );

	dwID = CQuestEditDlg::m_sQuestNode.pQUEST->ToDownStep(dwID);

	pListBox->SetCurSel ( dwID );

	//	Note : 리스트 갱신.
	UpdateItems ();
	UpdateItems_Step ();

	pListBox->SetCurSel ( dwID );
}



void CQuestStepPage::OnBnClickedButtonEditcancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveQuestTreePage ();
}

void CQuestStepPage::OnBnClickedButtonEditcomplate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	*(iter_pos->second.pQUEST) = *(CQuestEditDlg::m_sQuestNode.pQUEST);	
	iter_pos->second.pQUEST->SAVE( iter_pos->second.strFILE.c_str() );

	m_pSheetTab->ActiveQuestTreePage ();
}

void CQuestStepPage::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	SetWin_Enable( this, IDC_BUTTON_UP, FALSE );
	SetWin_Enable( this, IDC_BUTTON_DOWN, FALSE );
	SetWin_Enable( this, IDC_BUTTON_STEP_NEW, FALSE );
	SetWin_Enable( this, IDC_BUTTON_REQ_STEP_DEL, FALSE );
	SetWin_Enable( this, IDC_BUTTON_TALKNPC, FALSE );
	SetWin_Enable( this, IDC_BUTTON_TALKNPC_DEL, FALSE );
	SetWin_Enable( this, IDC_BUTTON_DIEMOB, FALSE );
	SetWin_Enable( this, IDC_BUTTON_DIEMOB_DEL, FALSE );
	SetWin_Enable( this, IDC_EDIT_DIEMOB_NUM, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GUARDNPC, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GUARDNPC_DEL, FALSE );
	SetWin_Enable( this, IDC_BUTTON_REACH_MAP, FALSE );
	SetWin_Enable( this, IDC_BUTTON_REACH_MAP_DEL, FALSE );
	SetWin_Enable( this, IDC_EDIT_REACH_PX, FALSE );
	SetWin_Enable( this, IDC_EDIT_REACH_PY, FALSE );
	SetWin_Enable( this, IDC_EDIT_REACH_RADIUS, FALSE );
	SetWin_Enable( this, IDC_EDIT_LEVEL, FALSE );
	SetWin_Enable( this, IDC_BUTTON_REQ_QITEM_NEW, FALSE );
	SetWin_Enable( this, IDC_BUTTON_REQ_QITEM_EDIT, FALSE );
	SetWin_Enable( this, IDC_BUTTON_REQ_QITEM_DEL, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GIFT_QITEM_NEW, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GIFT_QITEM_EDIT, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GIFT_QITEM_DEL, FALSE );
	SetWin_Enable( this, IDC_BUTTON_DEL_QITEM_NEW, FALSE );
	SetWin_Enable( this, IDC_BUTTON_DEL_QITEM_DEL, FALSE );
	SetWin_Enable( this, IDC_STEP_MAP_MID_EDIT, FALSE );
	SetWin_Enable( this, IDC_STEP_MAP_SID_EDIT, FALSE );
	SetWin_Enable( this, IDC_STEP_MAP_XPOS_EDIT, FALSE );
	SetWin_Enable( this, IDC_STEP_MAP_YPOS_EDIT, FALSE );
	SetWin_Enable( this, IDC_STEP_MAP_GATE_EDIT, FALSE );
	SetWin_Enable( this, IDC_STEP_MAPID_STATIC, FALSE );
	SetWin_Enable( this, IDC_STEP_XY_STATIC, FALSE );
	SetWin_Enable( this, IDC_STEP_GATE_STATIC, FALSE );
	SetWin_Enable( this, IDC_STEP_MAP_EDIT_BUTTON, FALSE );

#endif
}
void CQuestStepPage::OnBnClickedStepMapEditButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgMapSelect mapSelect;
	mapSelect.DoModal();

	char szTempChar[8] = {0,};
	sprintf_s( szTempChar, "%d", mapSelect.m_nidMAP.wMainID );
	SetDlgItemText( IDC_STEP_MAP_MID_EDIT, szTempChar );
	sprintf_s( szTempChar, "%d", mapSelect.m_nidMAP.wSubID );
	SetDlgItemText( IDC_STEP_MAP_SID_EDIT, szTempChar );
}


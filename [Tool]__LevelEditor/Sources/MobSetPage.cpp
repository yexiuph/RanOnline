// MobSetPage.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "MobSetPage.h"
#include "SheetWithTab.h"
#include "LevelEditorView.h"
#include "MainFrm.h"

#include "EtcFunction.h"

#include "GLOGIC.h"
#include "DxMethods.h"
#include "DxViewPort.h"
#include "NavigationMesh.h"
#include "GLList.h"
#include "GLDefine.h"

#include "GLGaeaServer.h"
#include "GLLandMan.h"
#include "GLMobSchedule.h"
#include ".\mobsetpage.h"

// CMobSetPage dialog
BOOL CMobSetPage::m_bShow = FALSE;
BOOL CMobSetPage::m_bMaterial = FALSE;
GLMobSchedule CMobSetPage::m_Schedule;

IMPLEMENT_DYNAMIC(CMobSetPage, CPropertyPage)
CMobSetPage::CMobSetPage()
	: CPropertyPage(CMobSetPage::IDD)
	, m_bInit ( FALSE )
{
}

CMobSetPage::~CMobSetPage()
{
}

void CMobSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MOB, m_ctrlMobList);
}


BEGIN_MESSAGE_MAP(CMobSetPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_MOB_NEW, OnBnClickedButtonMobNew)
	ON_BN_CLICKED(IDC_BUTTON_MOB_EDIT, OnBnClickedButtonMobEdit)
	ON_BN_CLICKED(IDC_BUTTON_MOB_DEL, OnBnClickedButtonMobDel)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_LBN_DBLCLK(IDC_LIST_MOB, OnLbnDblclkListMob)
	ON_BN_CLICKED(IDC_BUTTON_CROW_ID, OnBnClickedButtonCrowId)
	ON_BN_CLICKED(IDC_CHECK_RAND_POS, OnBnClickedCheckRandPos)
	ON_BN_CLICKED(IDC_BUTTON_FIND_POS, OnBnClickedButtonFindPos)
	ON_BN_CLICKED(IDC_BUTTON_REV_GATE, OnBnClickedButtonRevGate)
	ON_BN_CLICKED(IDC_BUTTON_BUS, OnBnClickedButtonBus)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NUM, OnDeltaposSpinNum)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DIST, OnDeltaposSpinDist)
	ON_BN_CLICKED(IDC_BUTTON_SCHDL_MAN, OnBnClickedButtonSchdlMan)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_CHECK_SUNDAY, OnBnClickedCheckWeek)
	ON_BN_CLICKED(IDC_CHECK_MONDAY, OnBnClickedCheckWeek)
	ON_BN_CLICKED(IDC_CHECK_TUESDAY, OnBnClickedCheckWeek)
	ON_BN_CLICKED(IDC_CHECK_WEDNESDAY, OnBnClickedCheckWeek)
	ON_BN_CLICKED(IDC_CHECK_THURSDAY, OnBnClickedCheckWeek)
	ON_BN_CLICKED(IDC_CHECK_FRIDAY, OnBnClickedCheckWeek)
	ON_BN_CLICKED(IDC_CHECK_SATURDAY, OnBnClickedCheckWeek)
END_MESSAGE_MAP()


// CMobSetPage message handlers
BOOL CMobSetPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	ActiveMobSetEdit ( FALSE );
	m_bInit = TRUE;

	return TRUE;
}

void CMobSetPage::CopyMobSch ()
{
	m_bCut = false;

	int nIndex = m_ctrlMobList.GetCurSel();
	if ( nIndex == -1 )
	{
		m_strCopyMobName = "";
	}
	else
	{
		CString strTemp;
		m_ctrlMobList.GetText ( nIndex, strTemp );

		char	szTemp[64] = {0};
		strcpy_s ( szTemp, strstr ( strTemp.GetString(), " - " )+3 );

		m_strCopyMobName = szTemp;
	}

	SetWin_Text ( this, IDC_EDIT_MOB_COPY, m_strCopyMobName );
}

void CMobSetPage::CutMobSch()
{
	m_bCut = true;

	int nIndex = m_ctrlMobList.GetCurSel();
	if ( nIndex == -1 )
	{
		m_strCopyMobName = "";
	}
	else
	{
		CString strTemp;
		m_ctrlMobList.GetText ( nIndex, strTemp );

		char	szTemp[64] = {0};
		strcpy_s( szTemp, strstr ( strTemp.GetString(), " - " )+3 );

		m_strCopyMobName = szTemp;
	}

	SetWin_Text ( this, IDC_EDIT_MOB_COPY, m_strCopyMobName );
}

void CMobSetPage::PasteMobSch()
{
	GLLandMan *pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	GLMobScheduleMan *pMobSchMan = pGLLandMan->GetMobSchMan();
	NavigationMesh *pNavi = pGLLandMan->GetNavi();
	MOBSCHEDULENODE *pNode = pMobSchMan->FindMobSch( m_strCopyMobName.GetString() );

	if ( pNode )
	{
		CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
		CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
		
		DXAFFINEPARTS sAffine;
		D3DXVECTOR3 *vColl = pView->GetCollisionPos();

		sAffine.vTrans = *vColl;

		GLMobSchedule *m_pSchedule = pNode->Data;

		PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData ( pNode->Data->m_CrowID );
		BOOL bMaterial = (BOOL)(pCrow->m_sBasic.m_emCrow == CROW_MATERIAL);

		if ( bMaterial == m_bMaterial )
		{
			if ( m_bCut )
			{
				D3DXVECTOR3 vOffset = pNode->Data->m_pAffineParts->vTrans - sAffine.vTrans;
				pNode->Data->SetAffineValue ( &sAffine );
				pNode->Data->GenerateRendGenPos ( pNavi );
				pNode->Data->ModifierSchList ( &vOffset, pNavi );

				SetWin_ListBox_Sel ( this, IDC_LIST_MOB, m_strCopyMobName );
			}
			else
			{
				m_Schedule = *pNode->Data;

				CString strTemp;
				int i = 0;
				do
				{
					strTemp.Format ( "CROW[%d]", i++ );
				}
				while ( pMobSchMan->FindMobSch ( strTemp ) );
				StringCchCopy ( m_Schedule.m_szName, 65, strTemp.GetString() );

				D3DXVECTOR3 vOffset = pNode->Data->m_pAffineParts->vTrans - sAffine.vTrans;
				m_Schedule.SetAffineValue ( &sAffine );
				m_Schedule.GenerateRendGenPos ( pNavi );
				m_Schedule.ModifierSchList ( &vOffset, pNavi );

				pMobSchMan->AddMobSch ( m_Schedule );
				UpdatePage();

				SetWin_ListBox_Sel ( this, IDC_LIST_MOB, strTemp );
			}
		}
	}
}

void CMobSetPage::SetNewID ( WORD wMID, WORD wSID )
{
	m_Schedule.m_CrowID.wMainID	= wMID;
	m_Schedule.m_CrowID.wSubID	= wSID;

	CString str;

	str.Format ( "%d", wMID );
	SetWin_Text ( this, IDC_EDIT_CROW_MID, str.GetString() );

	str.Format ( "%d", wSID );
	SetWin_Text ( this, IDC_EDIT_CROW_SID, str.GetString() );
}

void CMobSetPage::SetPC_GATE_REGEN ( DWORD dwGateID )
{
	m_Schedule.m_dwPC_REGEN_GATEID = dwGateID;

	CString str;

	str.Format ( "%d", dwGateID );
	SetWin_Text ( this, IDC_EDIT_REV_GATE, str.GetString() );
}

void CMobSetPage::SetSelectSchedule ( GLMobSchedule *pMobSchedule )
{
	std::string strSchduleInfo;

	if ( pMobSchedule )
	{
		if ( pMobSchedule->m_strGroupName == "" )
		{
			strSchduleInfo = "null";
		}
		else
		{
			strSchduleInfo = pMobSchedule->m_strGroupName;

			if ( pMobSchedule->m_bLeaderMonster )
				strSchduleInfo += "*";
		}

		strSchduleInfo += " - ";
		strSchduleInfo += pMobSchedule->m_szName;

		SetWin_ListBox_Sel ( this, IDC_LIST_MOB, strSchduleInfo.c_str() );
	}
	else
	{
		m_ctrlMobList.SetCurSel ( 0xFFFFFFFF );
	}
}

void CMobSetPage::ListingUpdate ()
{
	m_ctrlMobList.ResetContent();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	GLMobScheduleMan* pMobSchMan = pGLLandMan->GetMobSchMan();
	MOBSCHEDULELIST* pMobScheduleList = pMobSchMan->GetMobSchList(); 
	MOBSCHEDULENODE* pHead = pMobScheduleList->m_pHead;

	std::string strScheduleInfo;

	while ( pHead )
	{
		GLMobSchedule* m_pSchedule = pHead->Data;
		strScheduleInfo = "";

		if ( pHead->Data->m_strGroupName == "" )
		{
			strScheduleInfo = "null";
		}
		else
		{
			strScheduleInfo = m_pSchedule->m_strGroupName;

			if ( m_pSchedule->m_bLeaderMonster )
				strScheduleInfo += "*";
		}

		strScheduleInfo += " - ";
		strScheduleInfo += pHead->Data->m_szName;

		PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData ( m_pSchedule->m_CrowID );
		BOOL bMaterial = (BOOL)(pCrow->m_sBasic.m_emCrow == CROW_MATERIAL);

		if ( bMaterial == m_bMaterial )
		{
			m_ctrlMobList.AddString ( strScheduleInfo.c_str() );
		}

		pHead = pHead->pNext;
	}
}

BOOL CMobSetPage::UpdateDataSchedule ( BOOL bGetData, BOOL bNew )
{
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	GLMobScheduleMan* pMobSchMan = pGLLandMan->GetMobSchMan();
	NavigationMesh* pNavi = pGLLandMan->GetNavi();

	CString strTemp;

	if ( bGetData )
	{
		strTemp = GetWin_Text ( this, IDC_EDIT_NAME_ID );

		if ( strTemp.GetLength() <= 65 )
		{
			strcpy_s( m_Schedule.m_szName, strTemp.GetString() );

			strTemp = GetWin_Text ( this, IDC_EDIT_CROW_MID );
			m_Schedule.m_CrowID.wMainID = atoi ( strTemp.GetString() );

			strTemp = GetWin_Text ( this, IDC_EDIT_CROW_SID );
			m_Schedule.m_CrowID.wSubID = atoi ( strTemp.GetString() );

			strTemp = GetWin_Text ( this, IDC_EDIT_GEN_HOURS );
			m_Schedule.m_nRegenHour = atoi ( strTemp.GetString() );

			strTemp = GetWin_Text ( this, IDC_EDIT_GEN_MIN );
			m_Schedule.m_nRegenMin = atoi ( strTemp.GetString() );

			for ( int i = 0; i < 7; ++i )
			{
				BOOL bCheck = GetWin_Check ( this, IDC_CHECK_SUNDAY + i );
				m_Schedule.m_bDayOfWeek[i] = bCheck != FALSE;
			}

			if ( !m_Schedule.GetUseRegenTime() )
			{
				strTemp = GetWin_Text ( this, IDC_EDIT_GEN_TIME );
				m_Schedule.m_fReGenTime = (float)atof ( strTemp.GetString() );
			}

			m_Schedule.m_bRendGenPos = GetWin_Check ( this, IDC_CHECK_RAND_POS );

			if ( m_Schedule.m_bRendGenPos )
			{
				m_Schedule.m_wRendGenPosNum = GetWin_Num_int ( this, IDC_EDIT_NUM );
				if ( m_Schedule.m_wRendGenPosNum > 14 )
					m_Schedule.m_wRendGenPosNum = 14;
				if ( m_Schedule.m_wRendGenPosNum < 1 )
					m_Schedule.m_wRendGenPosNum = 1;

				m_Schedule.m_wRendGenPosDist = GetWin_Num_int ( this, IDC_EDIT_DIST );
				if ( m_Schedule.m_wRendGenPosDist > 100 )
					m_Schedule.m_wRendGenPosDist = 100;
				if ( m_Schedule.m_wRendGenPosDist < 10 )
					m_Schedule.m_wRendGenPosDist = 10;

				if ( m_Schedule.m_vectorRendGenPos.empty() )
					m_Schedule.GenerateRendGenPos ( pNavi );
			}
			
			strTemp = GetWin_Text ( this, IDC_EDIT_REV_GATE );
			m_Schedule.m_dwPC_REGEN_GATEID = atoi ( strTemp.GetString() );

			m_Schedule.m_strBUSLOCATION = GetWin_Text ( this, IDC_EDIT_BUS );

			m_Schedule.m_bLeaderMonster = GetWin_Check ( this, IDC_CHECK_LEADER );
			m_Schedule.m_strGroupName = GetWin_Text ( this, IDC_EDIT_GROUP_NAME );

			if ( strstr ( m_Schedule.m_strGroupName.c_str(), " - " ) )
			{
				MessageBox ( "ERROR : Invalid group name. Can not use ' - '", "ERROR", MB_OK );
				return FALSE;
			}
			else
			{
				if ( m_Schedule.m_bLeaderMonster )
				{
					GLLandMan* pLandMan = GLGaeaServer::GetInstance().GetRootMap();
					GLMobScheduleMan* pMobSch = pLandMan->GetMobSchMan();
					MOBSCHEDULELIST* pMobSchList = pMobSch->GetMobSchList(); 
					MOBSCHEDULENODE* pHead = pMobSchList->m_pHead;

					while ( pHead )
					{
						GLMobSchedule *pSch = pHead->Data;

						if ( strcmp ( pSch->m_szName, m_Schedule.m_szName ) )
						{
							if ( pSch->m_strGroupName == m_Schedule.m_strGroupName && pSch->m_bLeaderMonster )
							{
								char* szDest = {0};
								sprintf( szDest, "Change \"%s\" group leader. %s -> %s", m_Schedule.m_strGroupName.c_str(), pSch->m_szName, m_Schedule.m_szName );
								MessageBox ( szDest, "Message", MB_OK );
								pHead->Data->m_bLeaderMonster = FALSE;

								break;
							}

							pHead = pHead->pNext;
						}
						else 
						{
							pHead = pHead->pNext;
						}
					}
				}

				return TRUE;
			}
		}
		else
		{
			MessageBox ( "ERROR : Name of MOB is too long. enter under 64 characters." );
			return FALSE;
		}
	}
	else
	{
		if ( bNew )
		{
			int i = 0;
			do
			{
				strTemp.Format ( "CROW[%d]", i++ );
			}
			while ( pMobSchMan->FindMobSch ( strTemp.GetString() ) );

			strcpy_s( m_Schedule.m_szName, strTemp.GetString() );
		}

		SetWin_Text ( this, IDC_EDIT_NAME_ID, m_Schedule.m_szName );
		m_strBackMobName = m_Schedule.m_szName;

		strTemp.Format ( "%d", m_Schedule.m_CrowID.wMainID );
		SetWin_Text ( this, IDC_EDIT_CROW_MID, strTemp.GetString() );

		strTemp.Format ( "%d", m_Schedule.m_CrowID.wSubID );
		SetWin_Text ( this, IDC_EDIT_CROW_SID, strTemp.GetString() );

		strTemp.Format ( "%d", m_Schedule.m_nRegenHour );
		SetWin_Text ( this, IDC_EDIT_GEN_HOURS, strTemp.GetString() );

		strTemp.Format ( "%d", m_Schedule.m_nRegenMin );
		SetWin_Text ( this, IDC_EDIT_GEN_MIN, strTemp.GetString() );

		for ( int i = 0; i < 7; ++i )
		{
			SetWin_Check ( this, IDC_CHECK_SUNDAY + i, m_Schedule.m_bDayOfWeek[i] );
		}

		if ( m_Schedule.GetUseRegenTime() )
		{
			SetWin_Enable ( this, IDC_EDIT_GEN_TIME, FALSE );
		}
		else
		{
			SetWin_Enable ( this, IDC_EDIT_GEN_TIME, TRUE );
		}

		strTemp.Format ( "%f", m_Schedule.m_fReGenTime );
		SetWin_Text ( this, IDC_EDIT_GEN_TIME, strTemp.GetString() );

		SetWin_Check ( this, IDC_CHECK_RAND_POS, m_Schedule.m_bRendGenPos );

		SetWin_Enable ( this, IDC_SPIN_NUM, m_Schedule.m_bRendGenPos );
		SetWin_Num_int ( this, IDC_EDIT_NUM, m_Schedule.m_wRendGenPosNum );
		SetWin_Enable ( this, IDC_SPIN_DIST, m_Schedule.m_bRendGenPos );
		SetWin_Num_int ( this, IDC_EDIT_DIST, m_Schedule.m_wRendGenPosDist );

		strTemp.Format ( "%d", m_Schedule.m_dwPC_REGEN_GATEID );
		SetWin_Text ( this, IDC_EDIT_REV_GATE, strTemp.GetString() );

		SetWin_Text ( this, IDC_EDIT_BUS, m_Schedule.m_strBUSLOCATION.c_str() );
		SetWin_Check ( this, IDC_CHECK_LEADER, m_Schedule.m_bLeaderMonster );
		SetWin_Text ( this, IDC_EDIT_GROUP_NAME, m_Schedule.m_strGroupName.c_str() );

		if ( strstr ( m_Schedule.m_strGroupName.c_str(), " - " ) )
		{
			MessageBox ( "ERROR : Invalid group name. Can not use ' - '", "ERROR", MB_OK );
			return FALSE;
		}
		else
		{
			if ( m_Schedule.m_bLeaderMonster )
			{
				GLLandMan* pLandMan = GLGaeaServer::GetInstance().GetRootMap();
				GLMobScheduleMan* pMobSch = pLandMan->GetMobSchMan();
				MOBSCHEDULELIST* pMobSchList = pMobSch->GetMobSchList(); 
				MOBSCHEDULENODE* pHead = pMobSchList->m_pHead;

				while ( pHead )
				{
					GLMobSchedule *pSch = pHead->Data;

					if ( strcmp ( pSch->m_szName, m_Schedule.m_szName ) )
					{
						if ( pSch->m_strGroupName == m_Schedule.m_strGroupName && pSch->m_bLeaderMonster )
						{
							char* szDest = {0};
							sprintf( szDest, "Change \"%s\" group leader. %s -> %s", m_Schedule.m_strGroupName.c_str(), pSch->m_szName, m_Schedule.m_szName );
							MessageBox ( szDest, "Message", MB_OK );
							pHead->Data->m_bLeaderMonster = FALSE;

							break;
						}

						pHead = pHead->pNext;
					}
					else 
					{
						pHead = pHead->pNext;
					}
				}
			}

			return TRUE;
		}
	}
}

void CMobSetPage::UpdatePage()
{
	if ( m_bInit )
	{
		GLMobSchedule sMobSch;
		m_Schedule = sMobSch;
		UpdateDataSchedule ( FALSE, TRUE );
		ListingUpdate();
	}
}

void CMobSetPage::ActiveMobSetEdit ( BOOL bShow )
{
	SetWin_ShowWindow ( this, IDC_STATIC_EDIT_GROUP, bShow );

	SetWin_ShowWindow ( this, IDC_BUTTON_GRP_NAME, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_GROUP_NAME, bShow );

	SetWin_ShowWindow ( this, IDC_CHECK_LEADER, bShow );

	SetWin_ShowWindow ( this, IDC_BUTTON_NAME_ID, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_NAME_ID, bShow );

	SetWin_ShowWindow ( this, IDC_BUTTON_CROW_ID, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_CROW_MID, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_CROW_SID, bShow );

	SetWin_ShowWindow ( this, IDC_BUTTON_GEN_TIME, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_GEN_TIME, bShow );

	SetWin_ShowWindow ( this, IDC_CHECK_SUNDAY, bShow );
	SetWin_ShowWindow ( this, IDC_CHECK_MONDAY, bShow );
	SetWin_ShowWindow ( this, IDC_CHECK_TUESDAY, bShow );
	SetWin_ShowWindow ( this, IDC_CHECK_WEDNESDAY, bShow );
	SetWin_ShowWindow ( this, IDC_CHECK_THURSDAY, bShow );
	SetWin_ShowWindow ( this, IDC_CHECK_FRIDAY, bShow );
	SetWin_ShowWindow ( this, IDC_CHECK_SATURDAY, bShow );

	SetWin_ShowWindow ( this, IDC_BUTTON_GENDEC_TIME, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_GEN_HOURS, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_GEN_MIN, bShow );

	SetWin_ShowWindow ( this, IDC_CHECK_RAND_POS, bShow );
	SetWin_ShowWindow ( this, IDC_BUTTON_FIND_POS, bShow );

	SetWin_ShowWindow ( this, IDC_STATIC_NUM, bShow );
	SetWin_ShowWindow ( this, IDC_SPIN_NUM, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_NUM, bShow );
	SetWin_ShowWindow ( this, IDC_STATIC_DIST, bShow );
	SetWin_ShowWindow ( this, IDC_SPIN_DIST, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_DIST, bShow );

	SetWin_ShowWindow ( this, IDC_BUTTON_REV_GATE, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_REV_GATE, bShow );

	SetWin_ShowWindow ( this, IDC_BUTTON_BUS, bShow );
	SetWin_ShowWindow ( this, IDC_EDIT_BUS, bShow );

	SetWin_ShowWindow ( this, IDC_BUTTON_SCHDL_MAN, bShow );

	SetWin_ShowWindow ( this, IDC_BUTTON_OK, bShow );
	SetWin_ShowWindow ( this, IDC_BUTTON_CANCEL, bShow );

	SetWin_Enable ( this, IDC_BUTTON_MOB_NEW, bShow == FALSE );
	SetWin_Enable ( this, IDC_BUTTON_MOB_EDIT, bShow == FALSE );
	SetWin_Enable ( this, IDC_BUTTON_MOB_DEL, bShow == FALSE );

	SetWin_Enable ( this, IDC_BUTTON_GRP_NAME, m_bMaterial == FALSE );
	SetWin_Enable ( this, IDC_EDIT_GROUP_NAME, m_bMaterial == FALSE );

	SetWin_Enable ( this, IDC_CHECK_LEADER, m_bMaterial == FALSE );
	SetWin_Enable ( this, IDC_BUTTON_SCHDL_MAN, m_bMaterial == FALSE );

	ListingUpdate();
	m_bShow = bShow;
}

void CMobSetPage::OnBnClickedButtonPlay()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	pFrame->GetActiveView();

	GLGaeaServer::GetInstance().ClearDropObj();
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	if ( pGLLandMan->SetMap() >= S_OK )
	{
		GLGaeaServer::GetInstance().SetUpdate ( TRUE );
	}
	else
	{
		MessageBox ( "FAILURE : Test procedure is stopped because of map initialization failure." );
	}
}

void CMobSetPage::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here
	GLGaeaServer::GetInstance().ClearDropObj();
	GLGaeaServer::GetInstance().SetUpdate( FALSE );
}

void CMobSetPage::OnLbnDblclkListMob()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctrlMobList.GetCurSel();

	if ( nIndex != -1 )
	{
		GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
		GLMobScheduleMan* pMobSchMan = pGLLandMan->GetMobSchMan();

		CString strTemp;
		m_ctrlMobList.GetText ( nIndex, strTemp );

		char szTemp[64] = {0};
		strcpy_s( szTemp, strstr ( strTemp.GetString(), " - " )+3 );

		MOBSCHEDULENODE* pNode = pMobSchMan->FindMobSch ( szTemp );

		if ( pNode )
		{
			DxViewPort::GetInstance().CameraJump ( pNode->Data->m_pAffineParts->vTrans );
		}
	}
}

void CMobSetPage::OnBnClickedButtonMobNew()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctrlMobList.GetCurSel();

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	GLMobSchedule sMobSch;
	m_Schedule = sMobSch;
	UpdateDataSchedule ( FALSE, TRUE );

	D3DXVECTOR3* vCollPos = pView->GetCollisionPos();
	DXAFFINEPARTS sAffine;

	sAffine.vTrans = *vCollPos;

	m_Schedule.SetAffineValue ( &sAffine );
	pView->ActiveEditMatrix ( &m_Schedule );

	ActiveMobSetEdit ( TRUE );

	GLGaeaServer::GetInstance().ClearDropObj();
	GLGaeaServer::GetInstance().SetUpdate ( FALSE );

	if ( nIndex != -1 )
	{
		m_ctrlMobList.SetCurSel ( nIndex );
	}
}

void CMobSetPage::OnBnClickedButtonMobEdit()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctrlMobList.GetCurSel();

	if ( nIndex != -1 )
	{
		CString strTemp;

		m_ctrlMobList.GetText ( nIndex, strTemp );
		GLGaeaServer::GetInstance().ClearDropObj();

		GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
		GLMobScheduleMan* pMobSchMan = pGLLandMan->GetMobSchMan();

		char szTemp[64] = {0};
		strcpy_s( szTemp, strstr( strTemp.GetString(), " - " )+3 );

		MOBSCHEDULENODE* pNode = pMobSchMan->FindMobSch ( szTemp );

		if ( pNode )
		{
			m_Schedule = *pNode->Data;
			m_Schedule.m_dwVolatFlags |= m_Schedule.EM_ONCE;
			UpdateDataSchedule ( FALSE, FALSE );

			CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
			CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
			pView->ActiveEditMatrix ( &m_Schedule );

			ActiveMobSetEdit ( TRUE );
			m_ctrlMobList.SetCurSel ( nIndex );

			GLLandMan* pLandMan = GLGaeaServer::GetInstance().GetRootMap();
			pLandMan->ClearDropObj();

			GLGaeaServer::GetInstance().SetUpdate ( FALSE );
			DxViewPort::GetInstance().CameraJump ( m_Schedule.m_pAffineParts->vTrans );
		}
	}
}

void CMobSetPage::OnBnClickedButtonMobDel()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctrlMobList.GetCurSel();

	if ( nIndex != -1 )
	{
		CString strTemp;

		m_ctrlMobList.GetText ( nIndex, strTemp );
		GLGaeaServer::GetInstance().ClearDropObj();

		GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
		GLMobScheduleMan* pMobSchMan = pGLLandMan->GetMobSchMan();

		char szTemp[64] = {0};
		strcpy_s( szTemp, strstr( strTemp.GetString(), " - " )+3 );

		pMobSchMan->DelMobSch ( szTemp );
		m_ctrlMobList.DeleteString ( nIndex );

		ActiveMobSetEdit ( FALSE );

		if ( m_ctrlMobList.GetCount() )
		{
			if ( m_ctrlMobList.GetCount() == nIndex )
				m_ctrlMobList.SetCurSel ( nIndex - 1 );
			else
				m_ctrlMobList.SetCurSel ( nIndex );
		}

		CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
		CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
		pView->DeActiveEditMatrix ();

		GLLandMan* pLandMan = GLGaeaServer::GetInstance().GetRootMap();
		pLandMan->ClearDropObj();

		GLGaeaServer::GetInstance().SetUpdate ( FALSE );
	}
}

void CMobSetPage::OnBnClickedButtonCrowId()
{
	// TODO: Add your control notification handler code here
	m_pSheetTab->ActiveCrowTreePage ( MOBSETPAGE, m_Schedule.m_CrowID );
}

void CMobSetPage::OnBnClickedCheckWeek()
{
	// TODO: Add your control notification handler code here
	if ( GetWin_Check ( this, IDC_CHECK_SUNDAY ) ||
		 GetWin_Check ( this, IDC_CHECK_MONDAY ) ||
		 GetWin_Check ( this, IDC_CHECK_TUESDAY ) ||
		 GetWin_Check ( this, IDC_CHECK_WEDNESDAY ) ||
		 GetWin_Check ( this, IDC_CHECK_THURSDAY ) ||
		 GetWin_Check ( this, IDC_CHECK_FRIDAY ) ||
		 GetWin_Check ( this, IDC_CHECK_SATURDAY ) )
	{
		SetWin_Enable ( this, IDC_BUTTON_GEN_TIME, FALSE );
		SetWin_Enable ( this, IDC_EDIT_GEN_TIME, FALSE );
	}
	else
	{
		SetWin_Enable ( this, IDC_BUTTON_GEN_TIME, TRUE );
		SetWin_Enable ( this, IDC_EDIT_GEN_TIME, TRUE );
	}

	SetWin_Text ( this, IDC_EDIT_GEN_TIME, "0" );
}

void CMobSetPage::OnBnClickedCheckRandPos()
{
	// TODO: Add your control notification handler code here
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	NavigationMesh* pNavi = pGLLandMan->GetNavi();

	m_Schedule.m_bRendGenPos = GetWin_Check ( this, IDC_CHECK_RAND_POS );
	
	SetWin_Enable ( this, IDC_SPIN_NUM, m_Schedule.m_bRendGenPos );
	SetWin_Enable ( this, IDC_SPIN_DIST, m_Schedule.m_bRendGenPos );

	if ( m_Schedule.m_bRendGenPos )
	{
		m_Schedule.m_wRendGenPosNum = 14;
		SetWin_Num_int ( this, IDC_EDIT_NUM, 14 );

		m_Schedule.m_wRendGenPosDist = 10;
		SetWin_Num_int ( this, IDC_EDIT_DIST, 10 );

		m_Schedule.GenerateRendGenPos ( pNavi );
	}
	else
	{
		SetWin_Num_int ( this, IDC_EDIT_NUM, m_Schedule.m_wRendGenPosNum );
		SetWin_Num_int ( this, IDC_EDIT_DIST, m_Schedule.m_wRendGenPosDist );
	}
}

void CMobSetPage::OnBnClickedButtonFindPos()
{
	// TODO: Add your control notification handler code here
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	NavigationMesh* pNavi = pGLLandMan->GetNavi();

	m_Schedule.m_bRendGenPos = GetWin_Check ( this, IDC_CHECK_RAND_POS );

	if ( m_Schedule.m_bRendGenPos )
	{
		m_Schedule.GenerateRendGenPos ( pNavi );
	}
}

void CMobSetPage::OnDeltaposSpinNum(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	NavigationMesh* pNavi = pGLLandMan->GetNavi();

	if ( pNMUpDown->iDelta >=0 )
	{
		--m_Schedule.m_wRendGenPosNum;
		
		if ( m_Schedule.m_wRendGenPosNum <= 0 )
			m_Schedule.m_wRendGenPosNum = 1;

		m_Schedule.GenerateRendGenPos ( pNavi );
		SetWin_Num_int ( this, IDC_EDIT_NUM, m_Schedule.m_wRendGenPosNum );
	}
	else
	{
		++m_Schedule.m_wRendGenPosNum;
		
		if ( m_Schedule.m_wRendGenPosNum > 14 )
			m_Schedule.m_wRendGenPosNum = 14;

		m_Schedule.GenerateRendGenPos ( pNavi );
		SetWin_Num_int ( this, IDC_EDIT_NUM, m_Schedule.m_wRendGenPosNum );
	}

	*pResult = 0;
}

void CMobSetPage::OnDeltaposSpinDist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	NavigationMesh* pNavi = pGLLandMan->GetNavi();

	if ( pNMUpDown->iDelta >=0 )
	{
		m_Schedule.m_wRendGenPosDist -= 10;
		
		if ( m_Schedule.m_wRendGenPosDist < 10 )
			m_Schedule.m_wRendGenPosDist = 10;

		m_Schedule.GenerateRendGenPos ( pNavi );
		SetWin_Num_int ( this, IDC_EDIT_DIST, m_Schedule.m_wRendGenPosDist );
	}
	else
	{
		m_Schedule.m_wRendGenPosDist += 10;
		
		if ( m_Schedule.m_wRendGenPosDist > 100 )
			m_Schedule.m_wRendGenPosDist = 100;

		m_Schedule.GenerateRendGenPos ( pNavi );
		SetWin_Num_int ( this, IDC_EDIT_DIST, m_Schedule.m_wRendGenPosDist );
	}

	*pResult = 0;
}

void CMobSetPage::OnBnClickedButtonRevGate()
{
	// TODO: Add your control notification handler code here
	m_pSheetTab->ActiveGateListPage ( MOBSETPAGE, m_Schedule.m_dwPC_REGEN_GATEID );
}

void CMobSetPage::OnBnClickedButtonBus()
{
	// TODO: Add your control notification handler code here
	CString szFilter = "Bus Destination List (*.busloc)|*.busloc|";

	CFileDialog dlg(TRUE,".busloc",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CMobSetPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath();

	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_BUS, dlg.GetFileName().GetString() );
	}
}

void CMobSetPage::OnBnClickedButtonSchdlMan()
{
	// TODO: Add your control notification handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();

	pView->SetCurType ( CUR_SELECT );
	pView->DeActiveEditMatrix();

	m_pSheetTab->ActiveMobSchedulePage ( &m_Schedule.m_sMobActList );
}

void CMobSetPage::OnBnClickedButtonOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if ( UpdateDataSchedule ( TRUE, TRUE ) )
	{
		GLGaeaServer::GetInstance().ClearDropObj();
		GLGaeaServer::GetInstance().SetUpdate(FALSE);

		GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
		GLMobScheduleMan* pMobSchMan = pGLLandMan->GetMobSchMan();

		if ( m_Schedule.m_bRendGenPos )
		{
			NavigationMesh* pNavi = pGLLandMan->GetNavi();
			m_Schedule.GenerateRendGenPos ( pNavi );
		}

		if ( m_Schedule.m_dwVolatFlags & 1 )
		{
			if ( strcmp ( m_strBackMobName.GetString(), m_Schedule.m_szName ) )
			{
				if ( pMobSchMan->FindMobSch ( m_Schedule.m_szName ) )
				{
					MessageBox ( "ERROR : Same name is already registered." );
				}
				else
				{
					if ( pMobSchMan->DelMobSch ( m_strBackMobName.GetString() ) && m_ctrlMobList.GetCurSel() != -1 )
					{
						pMobSchMan->AddMobSch ( m_Schedule );

						CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
						CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
						pView->DeActiveEditMatrix();

						ActiveMobSetEdit ( FALSE );
						m_ctrlMobList.SetCurSel ( m_ctrlMobList.GetCount() - 1 );
						pView->SetCurType ( CUR_SEL_MOBSCH );
					}
				}
			}
			else
			{
				MOBSCHEDULENODE* pNode = pMobSchMan->FindMobSch ( m_Schedule.m_szName );
				
				if ( pNode )
				{
					int nIndex = m_ctrlMobList.GetCurSel();
					
					if ( nIndex != -1 )
					{
						pNode->Data = &m_Schedule;

						CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
						CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
						pView->DeActiveEditMatrix();

						ActiveMobSetEdit ( FALSE );
						m_ctrlMobList.SetCurSel ( nIndex );
						pView->SetCurType ( CUR_SEL_MOBSCH );
					}
				}
			}
		}
		else
		{
			if ( pMobSchMan->FindMobSch ( m_Schedule.m_szName ) )
			{
				MessageBox ( "ERROR : Same name is already registered." );
			}
			else
			{
				PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData ( m_Schedule.m_CrowID );
				BOOL bMaterial = (BOOL)(pCrow->m_sBasic.m_emCrow == CROW_MATERIAL);

				if ( bMaterial == m_bMaterial )
				{
					pMobSchMan->AddMobSch ( m_Schedule );

					CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
					CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
					pView->DeActiveEditMatrix();

					ActiveMobSetEdit ( FALSE );
					m_ctrlMobList.SetCurSel ( m_ctrlMobList.GetCount() - 1 );
					pView->SetCurType ( CUR_SEL_MOBSCH );
				}
				else
				{
					MessageBox ( "ERROR : Crow Type Is Not Correct" );
				}
			}
		}
	}
}

void CMobSetPage::OnBnClickedButtonCancel()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	int nIndex = m_ctrlMobList.GetCurSel();

	GLMobSchedule sMobSch;
	m_Schedule = sMobSch;

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditorView *pView = (CLevelEditorView *)pFrame->GetActiveView();
	pView->DeActiveEditMatrix();

	ActiveMobSetEdit ( FALSE );
	pView->SetCurType ( CUR_SEL_MOBSCH );

	if ( nIndex != -1 )
	{
		m_ctrlMobList.SetCurSel ( nIndex );
	}
}

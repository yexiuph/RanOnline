#include "pch.h"
#include <process.h>
#include "./GLLandMan.h"
#include "./GLItemMan.h"
#include "./GLItemLMT.h"
#include "./GLChar.h"
#include "./GLGaeaServer.h"
#include "./stl_Func.h"

#include "./DBActionLogic.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//static unsigned int WINAPI UpdateThread_Weather( LPVOID lpParam ) 
//{ 
//	GLLandMan * pGLLandMan = (GLLandMan *)lpParam;
//
//	DWORD dwOldWeather(0), dwNowWeather(0);
//	DWORD i(0);
//
//	std::vector<SONEMAPWEATHER> vecMapOldWeather;
//	std::vector<SONEMAPWEATHER> vecMapNowWeather;
//	vecMapOldWeather.clear();
//	vecMapNowWeather.clear();
//
//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();
//	GLPeriod & glPeriod = GLPeriod::GetInstance();
//
//	while(pGLLandMan)
//	{
//		if ( pGLLandMan->IsWeatherActive() )
//		{
//
//			//////////////////////////////////////////////////////////////////////////
//			/// 전체 맵이 추가되는 날씨
//
//			dwNowWeather = glPeriod.GetWeather ();
//
//			if ( dwOldWeather != dwNowWeather )
//			{
//				GLMSG::SNETPC_WEATHER NetMsg;
//				NetMsg.dwWeather = dwNowWeather;
//
//				GLCHARNODE* pCharNode = pGLLandMan->m_GlobPCList.m_pHead;
//				for ( ; pCharNode; pCharNode = pCharNode->pNext )
//				{
//					PGLCHAR pChar = pCharNode->Data;
//					if( !pChar ) continue;
//
//					glGaeaServer.SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
//				}
//
//				dwOldWeather = dwNowWeather;
//			}
//
//			//	Note : 일시적 날씨 효과. ( 번개 등등. )
//			//
//			DWORD dwWhimsical = glPeriod.GetWhimsicalWeather();
//			if ( dwWhimsical )
//			{
//				GLMSG::SNETPC_WHIMSICALWEATHER NetMsg;
//				NetMsg.dwWhimsical = dwWhimsical;
//
//				GLCHARNODE* pCharNode = pGLLandMan->m_GlobPCList.m_pHead;
//				for ( ; pCharNode; pCharNode = pCharNode->pNext )
//				{
//					PGLCHAR pChar = pCharNode->Data;
//					if( !pChar ) continue;
//
//					glGaeaServer.SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
//				}
//			}
//
//			//////////////////////////////////////////////////////////////////////////
//			/// 특정 맵에 추가되는 날씨
//			// Add OneMap Weather
//			vecMapNowWeather = glPeriod.GetOneMapWeather();
//			bool bNewWeather = FALSE;
//			if( vecMapNowWeather.size() != vecMapOldWeather.size() )
//			{
//				bNewWeather = TRUE;
//			}else{
//				for( i = 0; i < vecMapOldWeather.size(); i++ )
//				{
//					SONEMAPWEATHER mapNowWeather = vecMapNowWeather[i];
//					SONEMAPWEATHER mapOldWeather = vecMapOldWeather[i];
//					if( mapNowWeather != mapOldWeather )
//					{
//						bNewWeather = TRUE;
//						break;
//					}
//				}
//			}
//
//			if( bNewWeather )
//			{
//				GLMSG::SNETPC_MAPWEATHER NetMsg;
//				if( vecMapNowWeather.size() >= 32 )
//				{
//					NetMsg.dwMapWeatherSize = 31;
//				}else{
//					NetMsg.dwMapWeatherSize = vecMapNowWeather.size();
//				}
//
//
//				DWORD i;
//				for( i = 0; i < vecMapNowWeather.size(); i++ )
//				{
//					if( i >= 32 ) break;
//					NetMsg.MapWeather[i] = vecMapNowWeather[i];
//				}
//			
//
//				GLCHARNODE* pCharNode = pGLLandMan->m_GlobPCList.m_pHead;
//				for ( ; pCharNode; pCharNode = pCharNode->pNext )
//				{
//					PGLCHAR pChar = pCharNode->Data;
//					if( !pChar ) continue;
//
//					glGaeaServer.SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
//				}
//				vecMapOldWeather = vecMapNowWeather; 
//			}
//			
//
//			//	Note : 일시적 날씨 효과. ( 번개 등등. )
//			//
//			for( i = 0; i < vecMapOldWeather.size(); i++ )
//			{
//				DWORD dwWhimsical = vecMapOldWeather[i].dwWhimsicalWeather;
//				if ( dwWhimsical )
//				{
//					GLMSG::SNETPC_MAPWHIMSICALWEATHER NetMsg;
//					NetMsg.MapWeather = vecMapOldWeather[i];
//
//					GLCHARNODE* pCharNode = pGLLandMan->m_GlobPCList.m_pHead;
//					for ( ; pCharNode; pCharNode = pCharNode->pNext )
//					{
//						PGLCHAR pChar = pCharNode->Data;
//						if( !pChar ) continue;
//
//						glGaeaServer.SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
//					}
//				}
//			}
//
//		}
//
//		if( !(WaitForSingleObject( pGLLandMan->m_hWeatherEvent, 0 ) == WAIT_TIMEOUT) )
//			break;
//
//		Sleep(1);
//	}
//
//	return 0; 
//} 

GLLandMan::GLLandMan (void) :
	m_bInit(NULL),

	m_vMax(0,0,0),
	m_vMin(0,0,0),

	m_dwClubMapID(0),
	m_pGuidance(NULL),
	m_bGuidBattleMap(false),
	m_bGuidBattleMapHall(false),
	m_dwGuidClubID(CLUB_NULL),
	m_fCommissionRate(0.0f),

	m_bClubDeathMatchMap(false),
	m_bClubDeathMatchMapHall(false),

	m_bServerStop(false),

	m_dwOldWeather(NULL),

	m_pNaviMesh(NULL),
	m_pd3dDevice(NULL),


	m_fInstantMapDeleteTime(0.0f),

	m_fElapsedLimitTime(0.0f),
	m_nLastSendRemainTime(0),
	m_bEmulator(false)
{
	SecureZeroMemory ( m_ItemArray, sizeof(PITEMDROP)*MAXITEM );
	SecureZeroMemory ( m_MoneyArray, sizeof(PMONEYDROP)*MAXMONEY );
	SecureZeroMemory ( m_CROWArray, sizeof(PGLCROW)*MAXCROW );
	SecureZeroMemory ( m_MaterialArray, sizeof(PGLMATERIAL)*MAXCROW );

	m_MobSchMan.SetLandMan ( this );

	m_vTempPC.reserve(160);
	m_vDetectPC.reserve(160);

	m_vTempCROW.reserve(160);
	m_vDetectCROW.reserve(160);

	/*m_hWeatherThread = INVALID_HANDLE_VALUE;
	m_hWeatherEvent	 = INVALID_HANDLE_VALUE;*/

	m_vecMapOldWeather.clear();

	//CreateWeatherThread();
}

GLLandMan::~GLLandMan (void)
{
	CleanUp();
}

void GLLandMan::ResetLandMan()
{
	m_bInit = NULL;

	m_vMax = D3DXVECTOR3( 0,0,0 );
	m_vMin = D3DXVECTOR3( 0,0,0 );

	m_dwClubMapID = 0;
	m_pGuidance = NULL;
	m_bGuidBattleMap = false;
	m_bGuidBattleMapHall = false;
	m_dwGuidClubID = CLUB_NULL;
	m_fCommissionRate = 0.0f;

	m_bClubDeathMatchMap = false;
	m_bClubDeathMatchMapHall = false;

	m_bServerStop = false;

	m_dwOldWeather = NULL;

	m_pNaviMesh = NULL;
	m_pd3dDevice = NULL;

	m_fInstantMapDeleteTime = 0.0f;

	m_fElapsedLimitTime		= 0.0f;
	m_nLastSendRemainTime	= 0;

	SecureZeroMemory ( m_ItemArray, sizeof(PITEMDROP)*MAXITEM );
	SecureZeroMemory ( m_MoneyArray, sizeof(PMONEYDROP)*MAXMONEY );
	SecureZeroMemory ( m_CROWArray, sizeof(PGLCROW)*MAXCROW );
	SecureZeroMemory ( m_MaterialArray, sizeof(PGLMATERIAL)*MAXCROW );

	m_MobSchMan.SetLandMan ( this );

	m_vTempPC.reserve(160);
	m_vDetectPC.reserve(160);

	m_vTempCROW.reserve(160);
	m_vDetectCROW.reserve(160);
	m_bEmulator = false;

	/*m_hWeatherThread = INVALID_HANDLE_VALUE;
	m_hWeatherEvent	 = INVALID_HANDLE_VALUE;*/
}

void GLLandMan::CleanUp()
{
//	CloseWeatherThread();
	ReSetMap ();
}

//void GLLandMan::CreateWeatherThread()
//{
//	m_hWeatherEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
//
//	DWORD dwThreadId;
//	//m_hWeatherThread = CreateThread( 
//	//	NULL,					// default security attributes
//	//	0,						// use default stack size  
//	//	UpdateThread_Weather,	// thread function 
//	//	this,					// argument to thread function 
//	//	0,						// use default creation flags 
//	//	&dwThreadId);			// returns the thread identifier 
//
//	m_hWeatherThread = (HANDLE) ::_beginthreadex(
//		NULL,					// default security attributes
//		0,						// use default stack size  
//		UpdateThread_Weather,	// thread function 
//		this,					// argument to thread function 
//		0,						// use default creation flags 
//		(unsigned int*) &dwThreadId );			// returns the thread identifier 
//
//	if (m_hWeatherThread == NULL) 
//	{
//		ExitProcess(0);
//	}
//}
//
//void GLLandMan::CloseWeatherThread()
//{
//	SetEvent( m_hWeatherEvent );
//	TEXTCONSOLEMSG_WRITE( "[INFO]Delete WeatherThread Begin" );
//	WaitForSingleObject( m_hWeatherThread, INFINITE );
//	TEXTCONSOLEMSG_WRITE( "[INFO]Delete WeatherThread End" );
//	CloseHandle( m_hWeatherEvent );
//	::CloseHandle( m_hWeatherThread );
//}

HRESULT GLLandMan::ReSetMap ()
{
	m_bInit = FALSE;

	ClearDropObj ();

	SAFE_DELETE(m_pNaviMesh);
	m_cLandGateMan.CleanUp ();
	m_MobSchMan.CleanUp ();
	m_CollisionMap.CleanUp ();
	m_listDelGlobIDEx.clear();
	m_listDelGlobID.clear();
	m_listDelGlobIDExMat.clear();
	m_listDelGlobIDMat.clear();

	m_FreeItemGIDs.RemoveAll();
	m_FreeMoneyGIDs.RemoveAll();
	m_FreeCROWGIDs.RemoveAll();
	m_FreeMaterialGIDs.RemoveAll();

	return S_OK;
}

HRESULT GLLandMan::SetMap ()
{
	HRESULT hr;

	ClearDropObj ();

	m_MobSchMan.SetLandMan ( this );

	hr = CreateLandTree ();
	if ( FAILED(hr) )	return hr;

	//	Note : 몹 스케쥴을 한번 공회전 시켜줌. ( 몹 GEN )
	//
	m_MobSchMan.FrameMove ( 0.0f, 0.0f );

	m_bInit = TRUE;
	return S_OK;
}

HRESULT GLLandMan::CreateLandTree ()
{
	HRESULT hr;
	if ( !m_pNaviMesh )	return E_FAIL;

	m_pNaviMesh->GetAABB ( m_vMax, m_vMin );

	hr = m_LandTree.SetState ( m_vMax.x, m_vMax.z, m_vMin.x, m_vMin.z, m_sLevelHead.m_eDivision );
	if ( FAILED(hr) )	return hr;

	hr = m_LandTree.MakeTree ();
	if ( FAILED(hr) )	return hr;

	SecureZeroMemory ( m_ItemArray, sizeof(PITEMDROP)*MAXITEM );
	SecureZeroMemory ( m_MoneyArray, sizeof(PMONEYDROP)*MAXMONEY );
	SecureZeroMemory ( m_CROWArray, sizeof(PGLCROW)*MAXCROW );
	SecureZeroMemory ( m_MaterialArray, sizeof(PGLMATERIAL)*MAXCROW );

	DWORD i;
	for ( i=0; i<MAXITEM; i++ )		m_FreeItemGIDs.AddTail ( i );
	for ( i=0; i<MAXMONEY; i++ )	m_FreeMoneyGIDs.AddTail ( i );
	for ( i=0; i<MAXCROW; i++ )		m_FreeCROWGIDs.AddTail ( i );
	for ( i=0; i<MAXCROW; i++ )		m_FreeMaterialGIDs.AddTail ( i );

	return S_OK;
}

BOOL GLLandMan::RequestGenItemFieldFB ( GLMSG::SNET_REQ_GENITEM_FLD_FB * pNetMsg )
{
	const SNATIVEID &sNID = pNetMsg->sNID;

	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sNID );
	if ( !pITEM )		return FALSE;

	//	아이템 발생.
	SITEMCUSTOM sITEM_NEW;
	sITEM_NEW.sNativeID = sNID;
	CTime cTIME = CTime::GetCurrentTime();
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	//	아이템의 사용 횟수 표시. ( 소모품일 경우 x 값, 일반 물품 1 )
	sITEM_NEW.wTurnNum = pITEM->GETAPPLYNUM();

	//	발생된 정보 등록.
	sITEM_NEW.cGenType = pNetMsg->emGENTYPE;
	sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
	sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
	sITEM_NEW.lnGenNum = pNetMsg->lnGENCOUNT;

	// 아이템 에디트에서 입력한 개조 등급 적용 ( 준혁 : RequestGenItem, 아직은 때가 아닌듯 )
	//sITEM_NEW.cDAMAGE = (BYTE)pITEM->sBasicOp.wGradeAttack;
	//sITEM_NEW.cDEFENSE = (BYTE)pITEM->sBasicOp.wGradeDefense;

	//	랜덤 옵션 생성.
	if( sITEM_NEW.GENERATE_RANDOM_OPT() )
	{
		GLITEMLMT::GetInstance().ReqRandomItem( sITEM_NEW );
	}

	DropItem ( pNetMsg->vPos, &sITEM_NEW, pNetMsg->emHoldGroup, pNetMsg->dwHoldGID );

	return TRUE;
}

void GLLandMan::ADD_GROUPMEMBER ( const std::string strGroupName, const DWORD dwGlobID )
{
	SETGROUPMEM setGroupMem;
	setGroupMem = m_mapMobGroup[strGroupName];
	setGroupMem.insert( dwGlobID );
	m_mapMobGroup[strGroupName] = setGroupMem;
}

void GLLandMan::DEL_GROUPMEMBER ( const std::string strGroupName, const DWORD dwGlobID )
{
	SETGROUPMEM setGroupMem;
	setGroupMem = m_mapMobGroup[strGroupName];
	setGroupMem.erase( setGroupMem.find(dwGlobID) );
	m_mapMobGroup[strGroupName] = setGroupMem;
}

SETGROUPMEM GLLandMan::GET_GROUPMEMBER ( const std::string strGroupName )
{
    return m_mapMobGroup[strGroupName];
}

BOOL GLLandMan::IS_GROUPMEMBER ( const std::string strGroupName, const DWORD dwGlobID )
{
	return ( m_mapMobGroup[strGroupName].find(dwGlobID) == m_mapMobGroup[strGroupName].end() ) ? FALSE : TRUE;
}

void GLLandMan::ADD_GROUPLEADER ( const std::string strGroupName, const DWORD dwGlobID )
{
	m_mapGroupInfo[strGroupName].dwLeaderID = dwGlobID;
}


void GLLandMan::DEL_GROUPLEADER ( const std::string strGroupName )
{
	m_mapGroupInfo[strGroupName].dwLeaderID = 0;
}


PGLCROW GLLandMan::GET_GROUPLEADER( const std::string strGroupName )
{
	DWORD   dwLeaderID = m_mapGroupInfo[strGroupName].dwLeaderID;

	if( !IS_GROUPMEMBER( strGroupName, dwLeaderID ) ) return NULL;
	PGLCROW pGLCrow = GetCrow ( dwLeaderID );
	if( !pGLCrow || !pGLCrow->IsGroupMember() || !pGLCrow->IsGroupLeader() ) return NULL;

	return pGLCrow;
}

STARGETID GLLandMan::GET_GROUPTARGET ( const std::string strGroupName )
{
	GLACTOR* pTarget = GLGaeaServer::GetInstance().GetTarget ( this, m_mapGroupInfo[strGroupName].sTargetID );
	if ( !pTarget || pTarget->IsAction(GLAT_FALLING) || pTarget->IsAction(GLAT_DIE) )	m_mapGroupInfo[strGroupName].sTargetID.RESET();

	return m_mapGroupInfo[strGroupName].sTargetID;
}
void GLLandMan::SET_GROUPTARGET ( const std::string strGroupName, const STARGETID sTargetID )
{
	GLACTOR* pTarget = GLGaeaServer::GetInstance().GetTarget ( this, sTargetID );
	if( !pTarget ) return;

	m_mapGroupInfo[strGroupName].sTargetID = sTargetID;

}

BOOL GLLandMan::DropGenItem ( D3DXVECTOR3 &vPos, const SNATIVEID &sNID, EMITEMGEN emGENTYPE, EMGROUP emGroup, DWORD dwHoldGID )
{
	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sNID );
	if ( !pITEM )		return FALSE;

	//	이벤트 아이템일 경우 에이젼트를 거쳐서 생성.
	if ( pITEM->sBasicOp.IsEVENT() )
	{
		GLMSG::SNET_REQ_GENITEM_AGT NetMsg;
		NetMsg.nCHANNEL = GLGaeaServer::GetInstance().GetServerChannel();
		NetMsg.sNID = sNID;
		NetMsg.sMAPID = m_sMapID;
		NetMsg.emGENTYPE = emGENTYPE;
		NetMsg.emHoldGroup = emGroup;
		NetMsg.dwHoldGID = dwHoldGID;
		NetMsg.vPos = vPos;

		GLGaeaServer::GetInstance().SENDTOAGENT ( &NetMsg );
		return TRUE;
	}

	//	아이템 발생.
	SITEMCUSTOM sITEM_NEW;
	sITEM_NEW.sNativeID = sNID;
	CTime cTIME = CTime::GetCurrentTime();
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	//	아이템의 사용 횟수 표시. ( 소모품일 경우 x 값, 일반 물품 1 )
	sITEM_NEW.wTurnNum = pITEM->GETAPPLYNUM();

	//	발생된 정보 등록.
	sITEM_NEW.cGenType = emGENTYPE;
	sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
	sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
	sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sNID, emGENTYPE );

	// 아이템 에디트에서 입력한 개조 등급 적용 ( 준혁 : DropGetItem, 아직은 때가 아닌듯 )
	//sITEM_NEW.cDAMAGE = (BYTE)pITEM->sBasicOp.wGradeAttack;
	//sITEM_NEW.cDEFENSE = (BYTE)pITEM->sBasicOp.wGradeDefense;

	//	랜덤 옵션 생성.
	if( sITEM_NEW.GENERATE_RANDOM_OPT() )
	{
		GLITEMLMT::GetInstance().ReqRandomItem( sITEM_NEW );
	}

	DropItem ( vPos, &sITEM_NEW, emGroup, dwHoldGID );

	return TRUE;
}

BOOL GLLandMan::DropItem ( D3DXVECTOR3 &vPos, SITEMCUSTOM *pItemCustom, EMGROUP emGroup, DWORD dwHoldGID )
{
	//	Note : 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return FALSE;

	SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemCustom->sNativeID);
	if ( !pItem )		return false;

	//	Note : Global ID를 할당한다.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeItemGIDs.GetHead ( dwGlobID ) )		return FALSE;
	m_FreeItemGIDs.DelHead ();

	//	Note : 메모리를 할당.
	//
	CItemDrop *pNewItem = m_ItemMemPool.New();

	//	Note : 아이템 정보 ( 종류, 위치 ).
	//
	pNewItem->sItemCustom = *pItemCustom;
	pNewItem->vPos = vPos;

	pNewItem->fAge = 0.0f;
	pNewItem->emGroup = emGroup;
	pNewItem->dwHoldGID = dwHoldGID;

	//	Note : 관리 ID 부여.
	//
	pNewItem->sMapID = m_sMapID;
	pNewItem->dwGlobID = dwGlobID;
	pNewItem->dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_ItemArray[dwGlobID] = pNewItem;												// - 전역.
	pNewItem->pGlobList = m_GlobItemList.ADDHEAD ( pNewItem );						// - 전역.

	pNewItem->pQuadNode = pLandNode;												// - 트리 노드.
	pNewItem->pCellList = pLandNode->pData->m_ItemList.ADDHEAD ( pNewItem );		// - 트리 노드.

	return TRUE;
}

BOOL GLLandMan::DropMoney(D3DXVECTOR3 &vPos, LONGLONG lnAmount, bool bDropMonster /* = FALSE */, 
						  EMGROUP emGroup/* =EMGROUP_ONE */, DWORD dwHoldGID/* =GAEAID_NULL  */)
{
	//	Note : 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	FALSE;

	//	Note : Global ID를 할당한다.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeMoneyGIDs.GetHead ( dwGlobID ) )	return	FALSE;
	m_FreeMoneyGIDs.DelHead ();

	//	Note : 메모리를 할당.
	//
	CMoneyDrop *pNewMoney = m_MoneyMemPool.New();

	//	Note : Money 정보 ( 수량, 위치 ).
	//
	pNewMoney->fAge = 0.0f;
	pNewMoney->vPos = vPos;
	pNewMoney->lnAmount = lnAmount;
	pNewMoney->emGroup = emGroup;
	pNewMoney->dwHoldGID = dwHoldGID;

	//	Note : 관리 ID 부여.
	//
	pNewMoney->sMapID = m_sMapID;
	pNewMoney->dwGlobID = dwGlobID;
	pNewMoney->dwCeID = pLandNode->dwCID;

	pNewMoney->bDropMonster = bDropMonster;

	//	Note : 관리 노드 포인터.
	//
	m_MoneyArray[dwGlobID] = pNewMoney;											// - 전역.
	pNewMoney->pGlobList = m_GlobMoneyList.ADDHEAD ( pNewMoney );				// - 전역.

	pNewMoney->pQuadNode = pLandNode;											// - 트리 노드.
	pNewMoney->pCellList = pLandNode->pData->m_MoneyList.ADDHEAD ( pNewMoney );	// - 트리 노드.

	return TRUE;
}

DWORD GLLandMan::DropCrow ( GLMobSchedule *pMobSch )
{
	HRESULT hr;

	D3DXVECTOR3 vPos = pMobSch->m_pAffineParts->vTrans;
	if ( pMobSch->m_bRendGenPos )
	{
		vPos = pMobSch->GetRendGenPos();
	}

	SNATIVEID sNativeID = pMobSch->m_CrowID;

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	UINT_MAX;

	//	Note : Global ID를 할당한다.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeCROWGIDs.GetHead ( dwGlobID ) )	return	UINT_MAX;
	m_FreeCROWGIDs.DelHead ();

	//	Note : 메모리를 할당.
	//
	PGLCROW pNewCrow = GLGaeaServer::GetInstance().NEW_CROW();
	//PGLCROW pNewCrow = new GLCrow;

	hr = pNewCrow->CreateCrow ( pMobSch, this, sNativeID, m_pd3dDevice, &vPos );
	if ( FAILED(hr) )
	{
		m_FreeCROWGIDs.AddTail ( dwGlobID );
		GLGaeaServer::GetInstance().RELEASE_CROW ( pNewCrow );
		//SAFE_DELETE ( pNewCrow );

		return UINT_MAX;
	}

	//	Note : 관리 ID 부여.
	//
	pNewCrow->m_sMapID = m_sMapID;
	pNewCrow->m_dwGlobID = dwGlobID;
	pNewCrow->m_dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_CROWArray[dwGlobID] = pNewCrow;											// - 전역.

	// BOSS 몹이면
	if ( pNewCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_BOSS )
	{
		pNewCrow->m_pGlobNode = m_GlobBOSSCROWList.ADDHEAD ( pNewCrow );		// - 전역.
	}
	else
	{
		pNewCrow->m_pGlobNode = m_GlobCROWList.ADDHEAD ( pNewCrow );			// - 전역.
	}

	pNewCrow->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewCrow->m_pCellNode = pLandNode->pData->m_CROWList.ADDHEAD ( pNewCrow );	// - 트리 노드.

	//	Note : 알림 메시지 필요한지 검사후 메시지 발생.
	//
	if ( pNewCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_KNOCK )
	{
		const char *szMAP_NAME = GLGaeaServer::GetInstance().GetMapName ( m_sMapID );
		CString strTEXT;
		strTEXT.Format ( ID2SERVERTEXT("EMCROWACT_KNOCK"),
			pNewCrow->m_pCrowData->GetName(), szMAP_NAME );

		GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
		NetMsg.SETTEXT ( strTEXT.GetString() );

		GLGaeaServer::GetInstance().SENDTOAGENT ( &NetMsg );
	}

	// 그룹 몹이면 그룹 몹리스트에 넣는다.
	if ( pNewCrow->IsGroupMember() )
	{
		ADD_GROUPMEMBER( pNewCrow->GetGroupName(), dwGlobID );
		if( pNewCrow->IsGroupLeader() )
		{
			ADD_GROUPLEADER( pNewCrow->GetGroupName(), dwGlobID );
		}
	}
		

	return dwGlobID;
}

DWORD GLLandMan::DropCrow ( GLMobSchedule *pMobSch, SNATIVEID sNID, FLOAT fPosX, FLOAT fPosY, FLOAT fScale )
{
	HRESULT hr;

	D3DXVECTOR3 vPos;
	vPos.x = fPosX;
	vPos.z = fPosY;

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	UINT_MAX;

	//	Note : Global ID를 할당한다.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeCROWGIDs.GetHead ( dwGlobID ) )	return	UINT_MAX;
	m_FreeCROWGIDs.DelHead ();

	//	Note : 메모리를 할당.
	//
	PGLCROW pNewCrow = GLGaeaServer::GetInstance().NEW_CROW();
	//PGLCROW pNewCrow = new GLCrow;

	hr = pNewCrow->CreateCrow ( pMobSch, this, sNID, m_pd3dDevice, &vPos, fScale );
	if ( FAILED(hr) )
	{
		m_FreeCROWGIDs.AddTail ( dwGlobID );
		GLGaeaServer::GetInstance().RELEASE_CROW ( pNewCrow );
		//SAFE_DELETE ( pNewCrow );

		return UINT_MAX;
	}

	//	Note : 관리 ID 부여.
	//
	pNewCrow->m_sMapID = m_sMapID;
	pNewCrow->m_dwGlobID = dwGlobID;
	pNewCrow->m_dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_CROWArray[dwGlobID] = pNewCrow;											// - 전역.

	// BOSS 몹이면
	if ( pNewCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_BOSS )
	{
		pNewCrow->m_pGlobNode = m_GlobBOSSCROWList.ADDHEAD ( pNewCrow );		// - 전역.
	}
	else
	{
		pNewCrow->m_pGlobNode = m_GlobCROWList.ADDHEAD ( pNewCrow );			// - 전역.
	}

	pNewCrow->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewCrow->m_pCellNode = pLandNode->pData->m_CROWList.ADDHEAD ( pNewCrow );	// - 트리 노드.

	//	Note : 알림 메시지 필요한지 검사후 메시지 발생.
	//
	if ( pNewCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_KNOCK )
	{
		const char *szMAP_NAME = GLGaeaServer::GetInstance().GetMapName ( m_sMapID );
		CString strTEXT;
		strTEXT.Format ( ID2SERVERTEXT("EMCROWACT_KNOCK"),
			pNewCrow->m_pCrowData->GetName(), szMAP_NAME );

		GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
		NetMsg.SETTEXT ( strTEXT.GetString() );

		GLGaeaServer::GetInstance().SENDTOAGENT ( &NetMsg );
	}

	// 그룹 몹이면 그룹 몹리스트에 넣는다.
	if ( pNewCrow->IsGroupMember() )
	{
		ADD_GROUPMEMBER( pNewCrow->GetGroupName(), dwGlobID );
		if( pNewCrow->IsGroupLeader() )
		{
			ADD_GROUPLEADER( pNewCrow->GetGroupName(), dwGlobID );
		}
	}

	return dwGlobID;
}

DWORD GLLandMan::DropCrow ( SNATIVEID sNID, float fPosX, float fPosY )
{
	HRESULT hr;

	D3DXVECTOR3 vPos(fPosX,0,fPosY);

	// D3DXVECTOR3 vCollision;
	// DWORD dwCoID;
	// BOOL bCol;

	//	소수점 정밀도 향상이후에 y좌표에 대한 collision에 오차가 생김
	//	y( -1000,1000 ) 값을 줄이면 정상적으로 동작하나 해당 맵의 높이를
	//	정확히 측정하기 어려워서 일단 삭제함
//	GetNavi()->IsCollision ( vPos+D3DXVECTOR3(0,-1000,0), vPos+D3DXVECTOR3(0,+1000,0), vCollision, dwCoID, bCol );
//	if ( !bCol )	return UINT_MAX;

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)fPosX, (int)fPosY );
	if ( !pLandNode )	return	UINT_MAX;

	//	Note : Global ID를 할당한다.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeCROWGIDs.GetHead ( dwGlobID ) )	return	UINT_MAX;
	m_FreeCROWGIDs.DelHead ();

	//	Note : 메모리를 할당.
	//
	PGLCROW pNewCrow = GLGaeaServer::GetInstance().NEW_CROW();
	//PGLCROW pNewCrow = new GLCrow;

	hr = pNewCrow->CreateCrow ( NULL, this, sNID, m_pd3dDevice, &vPos );
	if ( FAILED(hr) )
	{
		m_FreeCROWGIDs.AddTail ( dwGlobID );
		GLGaeaServer::GetInstance().RELEASE_CROW ( pNewCrow );
		//SAFE_DELETE ( pNewCrow );

		return UINT_MAX;
	}

	//	Note : 관리 ID 부여.
	//
	pNewCrow->m_sMapID = m_sMapID;
	pNewCrow->m_dwGlobID = dwGlobID;
	pNewCrow->m_dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_CROWArray[dwGlobID] = pNewCrow;											// - 전역.

	// BOSS 몹이면
	if ( pNewCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_BOSS )
	{
		pNewCrow->m_pGlobNode = m_GlobBOSSCROWList.ADDHEAD ( pNewCrow );		// - 전역.
	}
	else
	{
		pNewCrow->m_pGlobNode = m_GlobCROWList.ADDHEAD ( pNewCrow );			// - 전역.
	}

	pNewCrow->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewCrow->m_pCellNode = pLandNode->pData->m_CROWList.ADDHEAD ( pNewCrow );	// - 트리 노드.

	//	Note : 알림 메시지 필요한지 검사후 메시지 발생.
	//
	if ( pNewCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_KNOCK )
	{
		const char *szMAP_NAME = GLGaeaServer::GetInstance().GetMapName ( m_sMapID );
		CString strTEXT;
		strTEXT.Format ( ID2SERVERTEXT("EMCROWACT_KNOCK"),
			pNewCrow->m_pCrowData->GetName(), szMAP_NAME );

		GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
		NetMsg.SETTEXT ( strTEXT.GetString() );

		GLGaeaServer::GetInstance().SENDTOAGENT ( &NetMsg );
	}

	m_listDelGlobID.push_back( dwGlobID );

	// 그룹 몹이면 그룹 몹리스트에 넣는다.
	if ( pNewCrow->IsGroupMember() )
	{
		ADD_GROUPMEMBER( pNewCrow->GetGroupName(), dwGlobID );
		if( pNewCrow->IsGroupLeader() )
		{
			ADD_GROUPLEADER( pNewCrow->GetGroupName(), dwGlobID );
		}
	}

	return dwGlobID;
}

DWORD GLLandMan::DropCrow ( SNATIVEID sNID, WORD wPosX, WORD wPosY )
{
	D3DXVECTOR3 vPos(0,0,0);
	m_sLevelAxisInfo.MapPos2MiniPos ( wPosX, wPosY, vPos.x, vPos.z );

	return DropCrow ( sNID, vPos.x, vPos.z );
}

DWORD GLLandMan::DropMaterial ( GLMobSchedule *pMobSch )
{
	HRESULT hr;

	D3DXVECTOR3 vPos = pMobSch->m_pAffineParts->vTrans;
	if ( pMobSch->m_bRendGenPos )
	{
		vPos = pMobSch->GetRendGenPos();
	}

	SNATIVEID sNativeID = pMobSch->m_CrowID;

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	UINT_MAX;

	//	Note : Global ID를 할당한다.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeMaterialGIDs.GetHead ( dwGlobID ) )	return	UINT_MAX;
	m_FreeMaterialGIDs.DelHead ();

	//	Note : 메모리를 할당.
	//
	PGLMATERIAL pNewMaterial = GLGaeaServer::GetInstance().NEW_MATERIAL();
	//PGLCROW pNewCrow = new GLCrow;

	hr = pNewMaterial->CreateMaterial ( pMobSch, this, sNativeID, m_pd3dDevice, &vPos );
	if ( FAILED(hr) )
	{
		m_FreeMaterialGIDs.AddTail ( dwGlobID );
		GLGaeaServer::GetInstance().RELEASE_MATERIAL ( pNewMaterial );
		//SAFE_DELETE ( pNewCrow );

		return UINT_MAX;
	}

	//	Note : 관리 ID 부여.
	//
	pNewMaterial->m_sMapID = m_sMapID;
	pNewMaterial->m_dwGlobID = dwGlobID;
	pNewMaterial->m_dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_MaterialArray[dwGlobID] = pNewMaterial;											// - 전역.

	pNewMaterial->m_pGlobNode = m_GlobMaterialList.ADDHEAD ( pNewMaterial );				// - 전역.

	pNewMaterial->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewMaterial->m_pCellNode = pLandNode->pData->m_MaterialList.ADDHEAD ( pNewMaterial );	// - 트리 노드.

	return dwGlobID;
}

DWORD GLLandMan::DropMaterial ( SNATIVEID sNID, float fPosX, float fPosY )
{
	HRESULT hr;

	D3DXVECTOR3 vPos(fPosX,0,fPosY);

	// D3DXVECTOR3 vCollision;
	// DWORD dwCoID;
	// BOOL bCol;

	//	소수점 정밀도 향상이후에 y좌표에 대한 collision에 오차가 생김
	//	y( -1000,1000 ) 값을 줄이면 정상적으로 동작하나 해당 맵의 높이를
	//	정확히 측정하기 어려워서 일단 삭제함
//	GetNavi()->IsCollision ( vPos+D3DXVECTOR3(0,-1000,0), vPos+D3DXVECTOR3(0,+1000,0), vCollision, dwCoID, bCol );
//	if ( !bCol )	return UINT_MAX;

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)fPosX, (int)fPosY );
	if ( !pLandNode )	return	UINT_MAX;

	//	Note : Global ID를 할당한다.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeMaterialGIDs.GetHead ( dwGlobID ) )	return	UINT_MAX;
	m_FreeMaterialGIDs.DelHead ();

	//	Note : 메모리를 할당.
	//
	PGLMATERIAL pNewMaterial = GLGaeaServer::GetInstance().NEW_MATERIAL();
	//PGLCROW pNewCrow = new GLCrow;

	hr = pNewMaterial->CreateMaterial ( NULL, this, sNID, m_pd3dDevice, &vPos );
	if ( FAILED(hr) )
	{
		m_FreeMaterialGIDs.AddTail ( dwGlobID );
		GLGaeaServer::GetInstance().RELEASE_MATERIAL ( pNewMaterial );
		//SAFE_DELETE ( pNewCrow );

		return UINT_MAX;
	}

	//	Note : 관리 ID 부여.
	//
	pNewMaterial->m_sMapID = m_sMapID;
	pNewMaterial->m_dwGlobID = dwGlobID;
	pNewMaterial->m_dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_MaterialArray[dwGlobID] = pNewMaterial;											// - 전역.

	pNewMaterial->m_pGlobNode = m_GlobMaterialList.ADDHEAD ( pNewMaterial );				// - 전역.

	pNewMaterial->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewMaterial->m_pCellNode = pLandNode->pData->m_MaterialList.ADDHEAD ( pNewMaterial );	// - 트리 노드.

	m_listDelGlobIDMat.push_back( dwGlobID );

	return dwGlobID;
}

DWORD GLLandMan::DropMaterial ( SNATIVEID sNID, WORD wPosX, WORD wPosY )
{
	D3DXVECTOR3 vPos(0,0,0);
	m_sLevelAxisInfo.MapPos2MiniPos ( wPosX, wPosY, vPos.x, vPos.z );

	return DropMaterial ( sNID, vPos.x, vPos.z );
}

BOOL GLLandMan::DropOutItem ( DWORD dwGlobID )
{
	GASSERT ( dwGlobID<MAXITEM );
	if ( m_ItemArray[dwGlobID] == NULL )	return FALSE;

	PITEMDROP pItemDrop = m_ItemArray[dwGlobID];

	//	Note : Global-ID 해지.
	//
	m_FreeItemGIDs.AddTail ( dwGlobID );
	m_ItemArray[dwGlobID] = NULL;

	//	Note : Global-List 해지.
	//
	if ( pItemDrop->pGlobList )
		m_GlobItemList.DELNODE ( pItemDrop->pGlobList );

	//	Note : QuadNode-List 해지.
	//
	LANDQUADNODE* pLandNode = pItemDrop->pQuadNode;
	if ( pItemDrop->pCellList )
		pLandNode->pData->m_ItemList.DELNODE ( pItemDrop->pCellList );

	// 팻카드일 경우 아이템이 사라지면 팻DB 삭제
	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( pItemDrop->sItemCustom.sNativeID );
	if ( pITEM && pITEM->sBasicOp.emItemType == ITEM_PET_CARD && pItemDrop->sItemCustom.dwPetID != 0 )
	{
		// 펫카드 로직 변경으로 DB삭제 제거하고 로그 추가
		GLGaeaServer::GetInstance().GetConsoleMsg()->Write( 
			"GLLandMan Error PetCard Delet -> PetNum : %d", pItemDrop->sItemCustom.dwPetID );


		/*
		CDeletePet* pDbAction = new CDeletePet ( pItemDrop->sItemCustom.dwPetID );
		GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
		if ( pDBMan )
		{
			pDBMan->AddJob ( pDbAction );
		}
		*/
	}

	//	Note : 메모리 해지.
	//
	m_ItemMemPool.Release(pItemDrop);

	return TRUE;
}

BOOL GLLandMan::DropOutMoney ( DWORD dwGlobID )
{
	GASSERT ( dwGlobID<MAXMONEY );
	if ( m_MoneyArray[dwGlobID] == NULL )	return FALSE;

	PMONEYDROP pMoneyDrop = m_MoneyArray[dwGlobID];

	//	Note : Global-ID 해지.
	//
	m_FreeMoneyGIDs.AddTail ( dwGlobID );
	m_MoneyArray[dwGlobID] = NULL;

	//	Note : Global-List 해지.
	//
	if ( pMoneyDrop->pGlobList )
		m_GlobMoneyList.DELNODE ( pMoneyDrop->pGlobList );

	//	Note : QuadNode-List 해지.
	//
	LANDQUADNODE* pLandNode = pMoneyDrop->pQuadNode;
	if ( pMoneyDrop->pCellList )
		pLandNode->pData->m_MoneyList.DELNODE ( pMoneyDrop->pCellList );

	//	Note : 메모리 해지.
	//
	m_MoneyMemPool.Release(pMoneyDrop);

	return TRUE;
}

BOOL GLLandMan::DropOutCrow ( DWORD dwGlobID )
{

	if( dwGlobID >= MAXCROW ) 
	{
		if( m_listDelGlobID.empty() ) return FALSE;

		dwGlobID = m_listDelGlobID.back();
		m_listDelGlobID.pop_back();

		/*if( m_DelGlobID.empty() ) return FALSE;

		dwGlobID = m_DelGlobID.top();
		m_DelGlobID.pop();*/
	}else{
		if( !m_listDelGlobID.empty() )
		{
			LISTDELCROWID_ITER iter = m_listDelGlobID.begin();
			LISTDELCROWID_ITER iter_end = m_listDelGlobID.end();

			for ( ;iter != iter_end; iter++) 
			{
				if ( *iter  == dwGlobID ) 
				{
					m_listDelGlobID.erase(iter);
					break;
				}
			}
		}
	}

	if ( m_CROWArray[dwGlobID] == NULL )	return FALSE;

	PGLCROW pCrow = m_CROWArray[dwGlobID];

	// 그룹 몹이면 그룹 몹리스트에 넣는다.
	if ( pCrow->IsGroupMember() )
	{
		DEL_GROUPMEMBER( pCrow->GetGroupName(), dwGlobID );
		if( pCrow->IsGroupLeader() )
		{
			DEL_GROUPLEADER( pCrow->GetGroupName() );
			m_mapGroupInfo[pCrow->GetGroupName()].sTargetID.RESET();
		}
	}

	//	Note : Global-ID 해지.
	//
	m_FreeCROWGIDs.AddTail ( dwGlobID );
	m_CROWArray[dwGlobID] = NULL;

	//	Note : Global-List 해지.
	//
	
	if ( pCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_BOSS )
	{
		if ( pCrow->m_pGlobNode )
			m_GlobBOSSCROWList.DELNODE ( pCrow->m_pGlobNode );		// - 전역.
	}
	else
	{
		if ( pCrow->m_pGlobNode )
			m_GlobCROWList.DELNODE ( pCrow->m_pGlobNode );
	}

	//	Note : QuadNode-List 해지.
	//
	LANDQUADNODE* pLandNode = pCrow->m_pQuadNode;
	if ( pLandNode && pCrow->m_pCellNode )
		pLandNode->pData->m_CROWList.DELNODE ( pCrow->m_pCellNode );

	//	Note : 몹스케쥴의 생존 플래그를 오프시킴.
	GLMobSchedule* pMobSch = pCrow->GetMobSchedule();
	if ( pMobSch )
	{
		pMobSch->m_bALive = FALSE;
		pMobSch->m_dwGlobID = UINT_MAX;
	}

	//	Note : 메모리 해지.
	//
	GLGaeaServer::GetInstance().RELEASE_CROW ( pCrow );
	//SAFE_DELETE ( pCrow );

	return TRUE;
}

BOOL GLLandMan::DropOutMaterial ( DWORD dwGlobID )
{

	if( dwGlobID >= MAXCROW ) 
	{
		if( m_listDelGlobIDMat.empty() ) return FALSE;

		dwGlobID = m_listDelGlobIDMat.back();
		m_listDelGlobIDMat.pop_back();

	}else{
		if( !m_listDelGlobIDMat.empty() )
		{
			LISTDELCROWID_ITER iter = m_listDelGlobIDMat.begin();
			LISTDELCROWID_ITER iter_end = m_listDelGlobIDMat.end();

			for ( ;iter != iter_end; iter++) 
			{
				if ( *iter  == dwGlobID ) 
				{
					m_listDelGlobIDMat.erase(iter);
					break;
				}
			}
		}
	}

	if ( m_MaterialArray[dwGlobID] == NULL )	return FALSE;

	PGLMATERIAL pMaterial = m_MaterialArray[dwGlobID];

	//	Note : Global-ID 해지.
	//
	m_FreeMaterialGIDs.AddTail ( dwGlobID );
	m_MaterialArray[dwGlobID] = NULL;

	//	Note : Global-List 해지.
	//
	
	if ( pMaterial->m_pGlobNode )
		m_GlobMaterialList.DELNODE ( pMaterial->m_pGlobNode );

	//	Note : QuadNode-List 해지.
	//
	LANDQUADNODE* pLandNode = pMaterial->m_pQuadNode;
	if ( pLandNode && pMaterial->m_pCellNode )
		pLandNode->pData->m_MaterialList.DELNODE ( pMaterial->m_pCellNode );

	//	Note : 몹스케쥴의 생존 플래그를 오프시킴.
	GLMobSchedule* pMobSch = pMaterial->GetMobSchedule();
	if ( pMobSch )
	{
		pMobSch->m_bALive = FALSE;
		pMobSch->m_dwGlobID = UINT_MAX;
	}

	//	Note : 메모리 해지.
	//
	GLGaeaServer::GetInstance().RELEASE_MATERIAL ( pMaterial );
	//SAFE_DELETE ( pCrow );

	return TRUE;
}

BOOL GLLandMan::MoveCrow ( DWORD dwGlobID, const D3DXVECTOR3 &vPos )
{
	GASSERT ( dwGlobID<MAXCROW );
	GASSERT ( m_CROWArray[dwGlobID] != NULL );

	PGLCROW pCrow = m_CROWArray[dwGlobID];
	if( !pCrow )
	{
		// Note : Level Edit 작업시 여기서 NULL 값이 나왔다. 
		CDebugSet::ToLogFile( "GLLandMan::MoveCrow() - pCrow==NULL" );
		return FALSE;
	}

	LANDQUADNODE* pPrevNode = pCrow->m_pQuadNode;
	if( !pPrevNode )
	{
		CDebugSet::ToLogFile( "GLLandMan::MoveCrow() - pPrevNode==NULL" );
		return FALSE;
	}

	//	Note : 종전 셀에서 벗어나지 않았을 경우 그대로 둔다.
	//
	if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )	return FALSE;

	//	Note : 새 지점의 위치 노드를 찾는다.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )	return FALSE;

	//	Note : 이전 리스트에서 제거.
	if ( pCrow->m_pCellNode )
		pPrevNode->pData->m_CROWList.DELNODE ( pCrow->m_pCellNode );

	//	Note : 찾은 노드에 삽입.
	pCrow->m_pQuadNode = pNextNode;											// - 트리 노드.
	pCrow->m_pCellNode = pNextNode->pData->m_CROWList.ADDHEAD ( pCrow );	// - 트리 노드.

	return TRUE;
}

BOOL GLLandMan::RegistPet ( PGLPETFIELD pPet )
{
	GASSERT(pPet);
	LANDQUADNODE* pNode = pPet->m_pLandMan->FindCellNode ( (int)pPet->m_vPos.x, (int)pPet->m_vPos.z );
	if ( !pNode )	return FALSE;

	pPet->m_dwCellID = pNode->dwCID;

	pPet->m_pQuadNode = pNode;
	pPet->m_pCellNode = pNode->pData->m_PETList.ADDHEAD ( pPet );

	return TRUE;
}

BOOL GLLandMan::RemovePet ( PGLPETFIELD pPet )
{
	GASSERT(pPet);

	if ( pPet->m_pLandNode )
	{
		m_GlobPETList.DELNODE ( pPet->m_pLandNode );
		pPet->m_pLandNode = NULL;
	}

	LANDQUADNODE* pQuadNode = pPet->m_pQuadNode;

	if ( pQuadNode )
	{
		GASSERT ( pPet->m_pCellNode && "GLLandMan::RemovePet() - pQuadNode 가 존재시에는 반드시 m_pCellNode가 유효해야합니다." );
		if ( pPet->m_pCellNode )
			pQuadNode->pData->m_PETList.DELNODE ( pPet->m_pCellNode );
	
		pPet->m_dwCellID = 0;
		pPet->m_pQuadNode = NULL;
		pPet->m_pCellNode = NULL;
	}

	return TRUE;
}


BOOL GLLandMan::RegistSummon ( PGLSUMMONFIELD pSummon )
{
	GASSERT(pSummon);
	LANDQUADNODE* pNode = pSummon->m_pLandMan->FindCellNode ( (int)pSummon->m_vPos.x, (int)pSummon->m_vPos.z );
	if ( !pNode )	return FALSE;

	pSummon->m_dwCellID = pNode->dwCID;

	pSummon->m_pQuadNode = pNode;
	pSummon->m_pCellNode = pNode->pData->m_SummonList.ADDHEAD ( pSummon );

	return TRUE;
}

BOOL GLLandMan::RemoveSummon ( PGLSUMMONFIELD pSummon )
{
	GASSERT(pSummon);

	if ( pSummon->m_pLandNode )
	{
		m_GlobSummonList.DELNODE ( pSummon->m_pLandNode );
		pSummon->m_pLandNode = NULL;
	}

	LANDQUADNODE* pQuadNode = pSummon->m_pQuadNode;

	if ( pQuadNode )
	{
		GASSERT ( pSummon->m_pCellNode && "GLLandMan::RemoveSummon() - pQuadNode 가 존재시에는 반드시 m_pCellNode가 유효해야합니다." );
		if ( pSummon->m_pCellNode )
			pQuadNode->pData->m_SummonList.DELNODE ( pSummon->m_pCellNode );

		pSummon->m_dwCellID = 0;
		pSummon->m_pQuadNode = NULL;
		pSummon->m_pCellNode = NULL;
	}

	return TRUE;
}

BOOL GLLandMan::RegistChar ( PGLCHAR pChar )
{
	GASSERT(pChar);
	LANDQUADNODE* pNode = pChar->m_pLandMan->FindCellNode ( (int)pChar->m_vPos.x, (int)pChar->m_vPos.z );
	if ( !pNode )	return FALSE;

	pChar->m_dwCeID = pNode->dwCID;

	pChar->m_pQuadNode = pNode;
	pChar->m_pCellNode = pNode->pData->m_PCList.ADDHEAD ( pChar );

	return TRUE;
}

BOOL GLLandMan::RemoveChar ( PGLCHAR pChar )
{
	if( !pChar ) return FALSE;

	if ( pChar->m_pLandNode )
	{
		m_GlobPCList.DELNODE ( pChar->m_pLandNode );
		pChar->m_pLandNode = NULL;
	}

	LANDQUADNODE* pQuadNode = pChar->m_pQuadNode;
	if ( pQuadNode )
	{
		GASSERT ( pChar->m_pCellNode && "GLLandMan::RemoveChar() - pQuadNode 가 존제시에는 반드시 m_pCellNode가 유효해야합니다." );
		if ( pChar->m_pCellNode )
			pQuadNode->pData->m_PCList.DELNODE ( pChar->m_pCellNode );
	
		pChar->m_dwCeID = 0;
		pChar->m_pQuadNode = NULL;
		pChar->m_pCellNode = NULL;
	}

	return TRUE;
}

BOOL GLLandMan::MovePet ( DWORD dwGUID, const D3DXVECTOR3 &vPos )
{
	PGLPETFIELD pPet = GLGaeaServer::GetInstance().GetPET ( dwGUID );

	if ( !pPet )							return FALSE;
	if ( !pPet->IsValid () )				return FALSE;

	PGLCHAR pOwner = GLGaeaServer::GetInstance().GetChar ( pPet->m_dwOwner );
	if ( !pOwner )							return FALSE;
	if ( !pOwner->IsSTATE(EM_GETVA_AFTER) )	return FALSE;

	//	Note : 종전 셀에서 벗어나지 않았을 경우 그대로 둔다.
	//
	LANDQUADNODE* pPrevNode = pPet->m_pQuadNode;
	if ( pPrevNode )
        if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )	 return FALSE;

	//	Note : 새 지점의 위치 노드를 찾는다.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )	return FALSE;

	//	Note : 이전 리스트에서 제거.
	if ( pPet->m_pCellNode )
		pPrevNode->pData->m_PETList.DELNODE ( pPet->m_pCellNode );

	//	Note : 찾은 노드에 삽입.
	pPet->m_pQuadNode = pNextNode;										// - 트리 노드.
	pPet->m_pCellNode = pNextNode->pData->m_PETList.ADDHEAD ( pPet );	// - 트리 노드.

	return TRUE;
}

BOOL GLLandMan::MoveSummon ( DWORD dwGUID, const D3DXVECTOR3 &vPos )
{
	PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon ( dwGUID );

	if ( !pSummon )							return FALSE;
	if ( !pSummon->IsValid () )				return FALSE;

	PGLCHAR pOwner = GLGaeaServer::GetInstance().GetChar ( pSummon->m_dwOwner );
	if ( !pOwner )							return FALSE;
	if ( !pOwner->IsSTATE(EM_GETVA_AFTER) )	return FALSE;

	//	Note : 종전 셀에서 벗어나지 않았을 경우 그대로 둔다.
	//
	LANDQUADNODE* pPrevNode = pSummon->m_pQuadNode;
	if ( pPrevNode )
		if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )	 return FALSE;

	//	Note : 새 지점의 위치 노드를 찾는다.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )	return FALSE;

	//	Note : 이전 리스트에서 제거.
	if ( pSummon->m_pCellNode )
		pPrevNode->pData->m_SummonList.DELNODE ( pSummon->m_pCellNode );

	//	Note : 찾은 노드에 삽입.
	pSummon->m_pQuadNode = pNextNode;										// - 트리 노드.
	pSummon->m_pCellNode = pNextNode->pData->m_SummonList.ADDHEAD ( pSummon );	// - 트리 노드.

	return TRUE;
}

BOOL GLLandMan::MoveChar ( DWORD dwGaeaID, const D3DXVECTOR3 &vPos )
{
	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwGaeaID );
	GASSERT ( pChar && "GLLandMan::MoveChar" );

	if ( !pChar )
	{
		DEBUGMSG_WRITE ( "ERROR : GLLandMan::MoveChar() 에서 무효화된 dwGaeaID(%d) 발견됨.", dwGaeaID );
		return FALSE;
	}

	if ( !pChar->IsSTATE(EM_GETVA_AFTER) )	return FALSE;

	LANDQUADNODE* pPrevNode = pChar->m_pQuadNode;

	//	Note : 종전 셀에서 벗어나지 않았을 경우 그대로 둔다.
	//
	if ( pPrevNode )
		if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )	return FALSE;

	//	Note : 새 지점의 위치 노드를 찾는다.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )	return FALSE;

	//	Note : 이전 리스트에서 제거.
	if ( pChar->m_pCellNode )
		pPrevNode->pData->m_PCList.DELNODE ( pChar->m_pCellNode );

	//	Note : 찾은 노드에 삽입.
	pChar->m_pQuadNode = pNextNode;										// - 트리 노드.
	pChar->m_pCellNode = pNextNode->pData->m_PCList.ADDHEAD ( pChar );	// - 트리 노드.

	return TRUE;
}

HRESULT GLLandMan::ClearDropObj ()
{
	DWORD i=0;

	DropSaveCommission();

	for ( i=0; i<MAXITEM; i++ )		DropOutItem (i);
	for ( i=0; i<MAXMONEY; i++ )	DropOutMoney (i);
	for ( i=0; i<MAXCROW; i++ )		DropOutCrow (i);
	for ( i=0; i<MAXCROW; i++ )		DropOutMaterial (i);

	m_GlobPCList.DELALL ();

	m_listDelGlobIDEx.clear();
	m_listDelGlobID.clear();
	m_listDelGlobIDExMat.clear();
	m_listDelGlobIDMat.clear();

	// PET
	m_GlobPETList.DELALL ();

	m_GlobSummonList.DELALL();

	m_MobSchMan.Reset ();
	m_MobSchManEx.Reset ();

	return S_OK;
}

HRESULT GLLandMan::ClearExptChaObj ()
{
	DWORD i=0;
/*
	for ( i=0; i<MAXITEM; i++ )		DropOutItem (i);
	for ( i=0; i<MAXMONEY; i++ )	DropOutMoney (i);
	for ( i=0; i<MAXCROW; i++ )		DropOutCrow (i);
*/
	m_MobSchMan.Reset ();
	m_MobSchManEx.Reset ();

	return S_OK;
}

PGLCHAR GLLandMan::FindEnemyChar ( const PGLCROW pFinder )
{
	GASSERT(pFinder);

	EMCROWACT_UP emACT_UP = pFinder->GetActPattern();

	int nRange = (int) pFinder->m_pCrowData->m_wViewRange;
	EMATT_RGTYPE emAttRgType = pFinder->GetAttackRangeType();

	D3DXVECTOR3 vCenter = pFinder->GetPosition();
	int nX = int ( vCenter.x );
	int nZ = int ( vCenter.z );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nRange,nZ+nRange,nX-nRange,nZ-nRange);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );

	if ( !m_vTempPC.empty() )	m_vTempPC.erase ( m_vTempPC.begin(), m_vTempPC.end() );

	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLLandNode *pLandNode = pQuadNode->pData;
		
		GLCHARNODE *pCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext )
		{
			PGLCHAR pChar = pCur->Data;
			if( !pChar ) continue;

			//	Note : GM 캐릭은 선공 하지 않게.
			//
			if ( !pChar->IsValidBody() )							continue;
			if ( !pChar->IsVisibleDetect(pFinder->m_bRECVISIBLE) )	continue;

			//	대련시에는 몹이 케릭터를 선공하지 않음.
			if ( pChar->GETCONFTING()->IsCONFRONTING() )			continue;

			BOOL bTarget = FALSE;
			float fTarLeng = FLT_MAX;
			if ( emAttRgType==EMATT_SHORT )
			{
				//	Note : 근거리 공격일 경우.
				if ( pChar->IsFreeARSlot() )
				{
					D3DXVECTOR3 vLengA = pChar->GetPosition() - vCenter;
					fTarLeng = D3DXVec3Length(&vLengA);
					if ( fTarLeng <= nRange )		bTarget = TRUE;
						
				}
			}
			else
			{
				D3DXVECTOR3 vLengA = pChar->GetPosition() - vCenter;
				fTarLeng = D3DXVec3Length(&vLengA);
				if ( fTarLeng <= nRange )		bTarget = TRUE;
			}

			if ( !bTarget )								continue;

			switch ( emACT_UP )
			{
			default:	break;
			case EMCROWACT_UP_LOWLEVEL:
				if ( pChar->GETLEVEL() > pFinder->GETLEVEL() )		bTarget = FALSE;
				break;
			case EMCROWACT_UP_LOWHP:
				if ( pChar->GETHP() > pFinder->GETHP() )			bTarget = FALSE;
				break;

			case EMCROWACT_UP_BRIGHT:
				if ( pChar->GETBRIGHT() != BRIGHT_LIGHT )			bTarget = FALSE;
				break;

			case EMCROWACT_UP_DARK:
				if ( pChar->GETBRIGHT() != BRIGHT_DARK )			bTarget = FALSE;
				break;

			case EMCROWACT_UP_BLOW:
				if ( !pChar->ISSTATEBLOW() )						bTarget = FALSE;
				break;

			case EMCROWACT_UP_ARMER:
				{
					EMSLOT emRHand = pChar->GetCurRHand();
					if ( !pChar->VALID_SLOT_ITEM(emRHand) )			bTarget = FALSE;
				}
				break;

			case EMCROWACT_UP_RUNNER:
				if ( !pChar->IsRunning() )							bTarget = FALSE;
				break;
			};

			if ( !bTarget )		continue;

			m_vTempPC.push_back ( std_afunc::CHARDIST(fTarLeng,pCur->Data) );
		}
	}

	if ( !m_vTempPC.empty() )
	{
		D3DXVECTOR3 vFinderPos = pFinder->GetPosition();
		vFinderPos.y += 15.0f;
		
		D3DXVECTOR3 vTargetPos;

		std_afunc::CCompareCharDist Comp;
		std::sort ( m_vTempPC.begin(), m_vTempPC.end(), Comp );

		ARRAY_CHAR_ITER iter = m_vTempPC.begin();
		ARRAY_CHAR_ITER iter_end = m_vTempPC.end();		
		for ( ; iter!=iter_end; ++iter )
		{
			const std_afunc::CHARDIST &Target = *iter;

			//	Note : 시야가 확보되는가?
			//
			vTargetPos = Target.pChar->GetPosition();
			vTargetPos.y += 15.0f;
			BOOL bCollision = m_CollisionMap.IsCollision( vFinderPos, vTargetPos, FALSE, NULL, NULL, FALSE );
			if ( bCollision )	continue;

			return Target.pChar;
		}
	}

	return NULL;
}

ARRAY_CHAR* GLLandMan::DetectPC ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB, const DWORD dwMaxFind )
{
	STARGETID TargetID(CROW_PC,EMTARGET_NULL);

	if ( !m_vTempPC.empty() )	m_vTempPC.erase ( m_vTempPC.begin(), m_vTempPC.end() );
	if ( !m_vDetectPC.empty() )	m_vDetectPC.erase ( m_vDetectPC.begin(), m_vDetectPC.end() );

	float fTarLeng(0.0f);

	//	Note : A ~ B 사이의 중점.
	int nX = int ( vDetectPosA.x + vDetectPosB.x ) / 2;
	int nZ = int ( vDetectPosA.z + vDetectPosB.z ) / 2;

	//	Note : 중점에서 사이드 까지의 거리.
	int nSX = abs ( int ( vDetectPosA.x - vDetectPosB.x ) / 2 );
	int nSZ = abs ( int ( vDetectPosA.x - vDetectPosB.x ) / 2 );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nSX,nZ+nSZ,nX-nSX,nZ-nSZ);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );


	D3DXVECTOR3 vMax, vMin;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLLandNode *pLandNode = pQuadNode->pData;
		
		GLCHARNODE *pCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext )
		{
			PGLCHAR pChar = pCur->Data;
			if( !pChar ) continue;

			float fBodyRadius = (float) pChar->GETBODYRADIUS();
			D3DXVECTOR3 vPos = pChar->GetPosition();

			vMax = vPos + D3DXVECTOR3(8,20,8);
			vMin = vPos + D3DXVECTOR3(-8,0,-8);

			BOOL bCol = COLLISION::IsCollisionLineToAABB ( vDetectPosA, vDetectPosB, vMax, vMin );
			if ( !bCol )		continue;

			D3DXVECTOR3 vLengA = pChar->GetPosition() - vDetectPosA;
			fTarLeng = D3DXVec3Length(&vLengA);

			m_vTempPC.push_back ( std_afunc::CHARDIST(fTarLeng,pCur->Data) );
		}
	}

	if ( !m_vTempPC.empty() )
	{
		D3DXVECTOR3 vTargetPos;
		std_afunc::CCompareCharDist Comp;
		std::sort ( m_vTempPC.begin(), m_vTempPC.end(), Comp );

		ARRAY_CHAR_ITER iter = m_vTempPC.begin();
		ARRAY_CHAR_ITER iter_end = m_vTempPC.end();		
		for ( ; iter!=iter_end; ++iter )
		{
			if ( m_vDetectPC.size() >= dwMaxFind )	break;
			const std_afunc::CHARDIST &Target = *iter;

			//	Note : 시야가 확보되는가?
			//
			//vTargetPos = Target.pChar->GetPosition();
			//vTargetPos.y += 15.0f;

			//BOOL bCollision = m_CollisionMap.IsCollision ( vFinderPos, vTargetPos, NULL, FALSE );
			//if ( bCollision )	continue;

			m_vDetectPC.push_back ( Target );
		}
	}

	if ( m_vDetectPC.empty() )	return NULL;

	return &m_vDetectPC;
}

ARRAY_CHAR* GLLandMan::DetectPC ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind )
{
	STARGETID TargetID(CROW_PC,EMTARGET_NULL);

	if ( !m_vTempPC.empty() )	m_vTempPC.erase ( m_vTempPC.begin(), m_vTempPC.end() );
	if ( !m_vDetectPC.empty() )	m_vDetectPC.erase ( m_vDetectPC.begin(), m_vDetectPC.end() );

	float fTarLeng(0.0f);

	int nX = int ( vDetectPos.x );
	int nZ = int ( vDetectPos.z );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nRange,nZ+nRange,nX-nRange,nZ-nRange);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );

	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLLandNode *pLandNode = pQuadNode->pData;
		
		GLCHARNODE *pCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext )
		{
			PGLCHAR pChar = pCur->Data;
			if( !pChar ) continue;

			D3DXVECTOR3 vLengA = pChar->GetPosition() - vDetectPos;
			fTarLeng = D3DXVec3Length(&vLengA);
			if ( fTarLeng <= nRange )
			{
				m_vTempPC.push_back ( std_afunc::CHARDIST(fTarLeng,pCur->Data) );
			}
		}
	}

	if ( !m_vTempPC.empty() )
	{
		D3DXVECTOR3 vTargetPos;
		std_afunc::CCompareCharDist Comp;
		std::sort ( m_vTempPC.begin(), m_vTempPC.end(), Comp );

		ARRAY_CHAR_ITER iter = m_vTempPC.begin();
		ARRAY_CHAR_ITER iter_end = m_vTempPC.end();		
		for ( ; iter!=iter_end; ++iter )
		{
			if ( m_vDetectPC.size() >= dwMaxFind )	break;
			const std_afunc::CHARDIST &Target = *iter;

			//	Note : 시야가 확보되는가?
			//
			//vTargetPos = Target.pChar->GetPosition();
			//vTargetPos.y += 15.0f;

			//BOOL bCollision = m_CollisionMap.IsCollision ( vFinderPos, vTargetPos, NULL, FALSE );
			//if ( bCollision )	continue;

			m_vDetectPC.push_back ( Target );
		}
	}

	if ( m_vDetectPC.empty() )	return NULL;

	return &m_vDetectPC;
}

ARRAY_CROW* GLLandMan::DetectCROW ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind )
{
	STARGETID TargetID(CROW_PC,EMTARGET_NULL);

	if ( !m_vTempCROW.empty() )		m_vTempCROW.erase ( m_vTempCROW.begin(), m_vTempCROW.end() );
	if ( !m_vDetectCROW.empty() )	m_vDetectCROW.erase ( m_vDetectCROW.begin(), m_vDetectCROW.end() );;
	
	float fTarLeng(0.0f);

	int nX = int ( vDetectPos.x );
	int nZ = int ( vDetectPos.z );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nRange,nZ+nRange,nX-nRange,nZ-nRange);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );

	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLLandNode *pLandNode = pQuadNode->pData;
		
		GLCROWNODE *pCur = pLandNode->m_CROWList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext )
		{
			PGLCROW pCrow = pCur->Data;

			D3DXVECTOR3 vLengA = pCrow->GetPosition() - vDetectPos;
			fTarLeng = D3DXVec3Length(&vLengA);
			if ( fTarLeng <= nRange )
			{
				m_vTempCROW.push_back ( std_afunc::CROWDIST(fTarLeng,pCur->Data) );
			}
		}
	}

	if ( !m_vTempCROW.empty() )
	{
		D3DXVECTOR3 vTargetPos;

		std_afunc::CCompareCrowDist Comp;
		std::sort ( m_vTempCROW.begin(), m_vTempCROW.end(), Comp );

		ARRAY_CROW_ITER iter = m_vTempCROW.begin();
		ARRAY_CROW_ITER iter_end = m_vTempCROW.end();
		for ( ; iter!=iter_end; ++iter )
		{
			if ( m_vDetectCROW.size() >= dwMaxFind )	break;
			const std_afunc::CROWDIST &Target = *iter;

			//	Note : 시야가 확보되는가?
			//
			//vTargetPos = Target.pCrow->GetPosition();
			//vTargetPos.y += 15.0f;
			//BOOL bCollision = m_CollisionMap.IsCollision ( vFinderPos, vTargetPos, NULL, FALSE );
			//if ( bCollision )	continue;

			m_vDetectCROW.push_back ( Target );
		}
	}

	if ( m_vDetectCROW.empty() )	return NULL;

	return &m_vDetectCROW;
}

LANDQUADNODE* GLLandMan::FindCellNode ( int nx, int nz )
{
	return m_LandTree.FindNode ( nx, nz );
}

HRESULT GLLandMan::SendMsgPCViewAround ( int nX, int nZ, DWORD dwReqClientID, const LPNET_MSG_GENERIC &nmg )
{
	DWORD dwClientID = 0;
	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+MAX_VIEWRANGE,nZ+MAX_VIEWRANGE,nX-MAX_VIEWRANGE,nZ-MAX_VIEWRANGE);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );

	GLLandNode* pLandNode = NULL;
	GLCHARNODE* pCur = NULL;

	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"Index over cell size");

		pLandNode = pQuadNode->pData;
		if( !pLandNode ) continue;
		
		pCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext )
		{
			dwClientID = pCur->Data->m_dwClientID;
			if ( dwClientID != dwReqClientID )
			{
				GLGaeaServer::GetInstance().SENDTOCLIENT ( dwClientID, (LPVOID) nmg );
			}
		}
	}

	return S_OK;
}

HRESULT GLLandMan::SendMsgPC ( const LPNET_MSG_GENERIC &nmg )
{
	GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
	DWORD dwClientID = 0;
	for ( ; pCharNode; pCharNode = pCharNode->pNext )
	{
		PGLCHAR pChar = pCharNode->Data;
		if( !pChar ) continue;

		dwClientID = pChar->m_dwClientID;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( dwClientID, (LPVOID) nmg );
	}

	return S_OK;
}

HRESULT GLLandMan::UpdateItem ( float fTime, float fElapsedTime )
{
	//	Note : Suit 업데이트.
	//
	SGLNODE<PITEMDROP>* pCurItem = m_GlobItemList.m_pHead;
	while ( pCurItem )
	{
		//	Note : 포인터 얻어오기.
		PITEMDROP pItem = pCurItem->Data;
		if( !pItem ) continue;

		pItem->Update(fElapsedTime);

		//	Note : 포인터 다음으로 이동.
		pCurItem = pCurItem->pNext;

		//	Note : 삭제 시간 도달시. ( 포인터 이동뒤에 삭제가 일어나야 정상처리됨. )
		//
		if ( pItem->IsDropOut() )		DropOutItem ( pItem->dwGlobID );
	}

	//	Note : Money 업데이트.
	//
	SGLNODE<PMONEYDROP>* pCurMoney = m_GlobMoneyList.m_pHead;
	while ( pCurMoney )
	{
		//	Note : 포인터 얻어오기.
		CMoneyDrop* pMoney= pCurMoney->Data;
		if( !pMoney ) continue;

		pMoney->Update(fElapsedTime);

		//	Note : 포인터 다음으로 이동.
		pCurMoney = pCurMoney->pNext;

		//	Note : 삭제 시간 도달시. ( 포인터 이동뒤에 삭제가 일어나야 정상처리됨. )
		//
		if ( pMoney->IsDropOut() )		DropOutMoney ( pMoney->dwGlobID );
	}

	return S_OK;
}

BOOL GLLandMan::DropSaveCommission()
{
	GLCROWNODE *pCurCrow = m_GlobCROWList.m_pHead;
	while ( pCurCrow )
	{
		PGLCROW pCrow = pCurCrow->Data;
		if( !pCrow ) continue;

		//	소환된 NPC 삭제
		if ( pCrow->m_bNpcRecall )
		{
			GLGaeaServer::GetInstance().SaveNpcCommissionDB( pCrow->m_dwCallCharID, pCrow->m_dwCallUserID, pCrow->m_lnCommission );			
		}

		pCurCrow = pCurCrow->pNext;
	}

	return TRUE;
}

HRESULT GLLandMan::UpdateCrow ( float fTime, float fElapsedTime )
{
	if ( m_GlobPCList.m_dwAmount > 0 || m_bEmulator )
	{
		PROFILE_BEGIN("m_GlobCROWList");
		GLCROWNODE *pCurCrow = m_GlobCROWList.m_pHead;
		while ( pCurCrow )
		{
			PGLCROW pCrow = pCurCrow->Data;
			if( !pCrow ) continue;

			pCrow->FrameMove ( fTime, fElapsedTime );

			// 사라져야할 Crow 처리
			if ( pCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_AUTODROP )
			{
				if ( pCrow->GetAge () > pCrow->m_pCrowData->m_sAction.m_fLiveTime )
				{
					DropOutCrow ( pCrow->m_dwGlobID );
				}
			}

			//	소환된 NPC 삭제
			if ( pCrow->m_bNpcRecall )
			{
				__time64_t tCurTime = CTime::GetCurrentTime().GetTime();
				__time64_t tGenTime = pCrow->m_tGenTime +  pCrow->m_pCrowData->m_dwGenTime;

				if ( tCurTime > tGenTime ) 
				{
					GLGaeaServer::GetInstance().SaveNpcCommission( pCrow->m_dwCallCharID, pCrow->m_dwCallUserID, pCrow->m_lnCommission );
					DropOutCrow ( pCrow->m_dwGlobID );
				}
			}

			pCurCrow = pCurCrow->pNext;

			if ( pCrow->ISDIE() )
			{
				m_listDelGlobIDEx.remove(pCrow->m_dwGlobID);
				DropOutCrow ( pCrow->m_dwGlobID );
			}
		}
		PROFILE_END("m_GlobCROWList");

		PROFILE_BEGIN("m_GlobMATERIALList");
		GLMATERIALNODE *pCurMaterial = m_GlobMaterialList.m_pHead;
		while ( pCurMaterial )
		{
			PGLMATERIAL pMaterial = pCurMaterial->Data;
			if( !pMaterial ) continue;

			pMaterial->FrameMove ( fTime, fElapsedTime );

			// 사라져야할 Crow 처리
			if ( pMaterial->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_AUTODROP )
			{
				if ( pMaterial->GetAge () > pMaterial->m_pCrowData->m_sAction.m_fLiveTime )
				{
					DropOutMaterial ( pMaterial->m_dwGlobID );
				}
			}
			pCurMaterial = pCurMaterial->pNext;

			if ( pMaterial->ISDIE() )
			{
				m_listDelGlobIDExMat.remove(pMaterial->m_dwGlobID);
				DropOutMaterial ( pMaterial->m_dwGlobID );
			}
		}
		PROFILE_END("m_GlobMATERIALList");
	}

	PROFILE_BEGIN("m_GlobBOSSCROWList");
	GLCROWNODE *pCurCrow = m_GlobBOSSCROWList.m_pHead;
	while ( pCurCrow )
	{
		PGLCROW pCrow = pCurCrow->Data;
		if( !pCrow ) continue;

		pCrow->FrameMove ( fTime, fElapsedTime );

		// 사라져야할 Crow 처리
		if ( pCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_AUTODROP )
		{
			if ( pCrow->GetAge () > pCrow->m_pCrowData->m_sAction.m_fLiveTime )
			{
				DropOutCrow ( pCrow->m_dwGlobID );
			}
		}

		pCurCrow = pCurCrow->pNext;

		if ( pCrow->ISDIE() )
		{
			m_listDelGlobIDEx.remove(pCrow->m_dwGlobID);
			DropOutCrow ( pCrow->m_dwGlobID );
		}
	}
	PROFILE_END("m_GlobBOSSCROWList");

	return S_OK;
}

BOOL GLLandMan::IsDeleteInstantMap ( float fElapsedTime )
{
	if( !IsInstantMap() ) return FALSE;
	if( m_GlobPCList.m_dwAmount == 0 )
	{
		m_fInstantMapDeleteTime += fElapsedTime;
		if( !IsExcessiveLimitTime() ) return TRUE;
	}else							   m_fInstantMapDeleteTime = 0.0f;

	if( m_fInstantMapDeleteTime >= INSTANTMAP_DELETE_TIME ) return TRUE;

	return FALSE;
}

BOOL GLLandMan::IsExcessiveLimitTime()
{
	SLEVEL_ETC_FUNC *pLevelEtcFunc = GetLevelEtcFunc();
	if( pLevelEtcFunc )
	{
		if( pLevelEtcFunc->m_bUseFunction[EMETCFUNC_LIMITTIME] )
		{
			int nLimitMinute = (int)(m_fElapsedLimitTime / 60.0f);
			if( nLimitMinute >= pLevelEtcFunc->m_sLimitTime.nLimitMinute )
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL GLLandMan::MoveOutInstantMap( PGLCHAR pChar )
{
	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	GLLandMan* pInLandMan = NULL;
	DxLandGateMan* pInGateMan = NULL;
	PDXLANDGATE pInGate = NULL;
	D3DXVECTOR3 vPos(0,0,0);

	DxLandGateMan* pOutGateMan = NULL;
	PDXLANDGATE pOutGate = NULL;	


	pOutGateMan = &GetLandGateMan();
	if( !pOutGateMan ) return FALSE;

	DWORD dwGateID = 0;
	DWORD dwOutGateID = 0;
	SNATIVEID sMoveMapId;
	SNATIVEID sCurMapID  = m_sMapID;
	SMAPNODE *pMapNode = NULL;

	for( dwGateID = 0; dwGateID < pOutGateMan->GetNumLandGate(); dwGateID++ )
	{
		pOutGate = pOutGateMan->FindLandGate ( dwGateID );
		if( pOutGate ) break;
	}

	// Gate를 못 찾았을경우
	if( !pOutGate )
	{
		TEXTCONSOLEMSG_WRITE( "[ERROR]Instant Out Error. Not Find Gate. MapID[%d][%d]", m_sMapID.wMainID, m_sMapID.wSubID );

		dwOutGateID	= GLCONST_CHAR::dwSTARTGATE[pChar->m_wSchool];
		sMoveMapId  = GLCONST_CHAR::nidSTARTMAP[pChar->m_wSchool];

		pMapNode = glGaeaServer.FindMapNode ( sMoveMapId );
		if ( !pMapNode )		return FALSE;


	}else{
		GLLandMan* pInLandMan = NULL;
		DxLandGateMan* pInGateMan = NULL;
		pInLandMan = glGaeaServer.GetByMapID ( pOutGate->GetToMapID() );
		if ( !pInLandMan )		return FALSE;

		dwOutGateID = pOutGate->GetToGateID();		

		//	Note : 목표 게이트 가져오기.					//

		pInGateMan = &pInLandMan->GetLandGateMan ();
		if ( !pInGateMan )		return FALSE;
		pInGate = pInGateMan->FindLandGate ( dwOutGateID );
		if ( !pInGate )			return FALSE;

		sMoveMapId = pInLandMan->GetMapID();

		pMapNode = glGaeaServer.FindMapNode ( sMoveMapId );
		if ( !pMapNode )		return FALSE;

		vPos = pInGate->GetGenPos ( DxLandGate::GEN_RENDUM );	
	}	
	


	glGaeaServer.DropOutPET ( pChar->m_dwPetGUID, true, true );
	glGaeaServer.DropOutSummon ( pChar->m_dwSummonGUID, true );
	glGaeaServer.SaveVehicle( pChar->m_dwClientID, pChar->m_dwGaeaID, true );


	if ( pMapNode->dwFieldSID!=glGaeaServer.GetFieldSvrID() )
	{
		GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_AG NetMsgAg;
		NetMsgAg.sMAPID	  = sMoveMapId;
		NetMsgAg.dwGATEID = dwOutGateID;
		NetMsgAg.vPOS	  = vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgAg );
	}else{
		BOOL bOK = glGaeaServer.RequestInvenRecallThisSvr ( pChar, sMoveMapId, dwOutGateID, vPos );
		if( !bOK )
		{
			TEXTCONSOLEMSG_WRITE( "[ERROR]Instant Map Must Leave Failed. MapID [%d][%d], Client ID[%d] Num[%d]",
								   m_sMapID.wMainID, m_sMapID.wSubID, pChar->m_szName, pChar->m_dwClientID);
			return FALSE;	
		}

		// 버프와 상태이상 제거
		if ( sCurMapID != pChar->m_sMapID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		pChar->DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		pChar->DISABLESKEFF ( i );
		}

		pChar->ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		GLMSG::SNETPC_REQ_RECALL_FB NetRecallFB;
		NetRecallFB.emFB   = EMREQ_RECALL_FB_OK;
		NetRecallFB.sMAPID = sMoveMapId;
		NetRecallFB.vPOS   = vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetRecallFB );
	}

	return TRUE;
}

void GLLandMan::FrameMoveInstantMap( float fElapsedTime )
{
	if( !IsInstantMap() ) return;
	if( m_GlobPCList.m_dwAmount == 0 ) return;	

	

	// 파티 인던인 경우 파티가 다르거나 아닌 사람들은 쫓아 낸다.
	if( IsPartyInstantMap() )
	{
		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			PGLCHAR pChar = pCharNode->Data;
			if( !pChar ) continue;
			if( pChar->GetPartyID() != GetInstantMapHostID() )
			{
				MoveOutInstantMap( pChar );				
			}
			

		}
	// 파티 인던이 아닌 경우엔 gaeaID와 HOST ID가 다르면 쫓아 낸다.
	}else
	{
		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			PGLCHAR pChar = pCharNode->Data;
			if( !pChar ) continue;
			if( pChar->m_dwGaeaID != GetInstantMapHostID() )
			{
				MoveOutInstantMap( pChar );
			}
		}
	}

	SLEVEL_ETC_FUNC *pLevelEtcFunc = GetLevelEtcFunc();
	if( pLevelEtcFunc )
	{
		if( pLevelEtcFunc->m_bUseFunction[EMETCFUNC_LIMITTIME] && m_nLastSendRemainTime != -1.0f )
		{
			m_fElapsedLimitTime += fElapsedTime;

			int nLimitMinute = (int)(m_fElapsedLimitTime / 60.0f);
			if( nLimitMinute >= pLevelEtcFunc->m_sLimitTime.nLimitMinute )
			{
				GLMSG::SNETLIMITTIME_OVER NetMsg;
				NetMsg.nRemainTime = 0;

				GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
				for ( ; pCharNode; pCharNode = pCharNode->pNext )
				{
					PGLCHAR pChar = pCharNode->Data;
					if( !pChar ) continue;

					GLGaeaServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
				}

				m_nLastSendRemainTime = -1;
			}
			else if( nLimitMinute % pLevelEtcFunc->m_sLimitTime.nTellTermMinute == 0 && m_nLastSendRemainTime != nLimitMinute )
			{
				
				GLMSG::SNETLIMITTIME_OVER NetMsg;
				NetMsg.nRemainTime = pLevelEtcFunc->m_sLimitTime.nLimitMinute - nLimitMinute;

				GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
				for ( ; pCharNode; pCharNode = pCharNode->pNext )
				{
					PGLCHAR pChar = pCharNode->Data;
					if( !pChar ) continue;

					GLGaeaServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
				}

				m_nLastSendRemainTime = nLimitMinute;
			}
		}
	}




	
}

HRESULT GLLandMan::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	PROFILE_BEGIN("UpdateItem()");
		hr = UpdateItem ( fTime, fElapsedTime );
		if ( FAILED(hr) )
		{
			PROFILE_END("UpdateItem()");
			return hr;
		}
	PROFILE_END("UpdateItem()");

	PROFILE_BEGIN("UpdateCrow()");
		hr = UpdateCrow ( fTime, fElapsedTime );
		if ( FAILED(hr) )
		{
			PROFILE_END("UpdateCrow()");
			return hr;
		}
	PROFILE_END("UpdateCrow()");

	PROFILE_BEGIN("m_MobSchMan.FrameMove()");
		hr = m_MobSchMan.FrameMove ( fTime, fElapsedTime );
		if ( FAILED(hr) )
		{
			PROFILE_END("m_MobSchMan.FrameMove()");
			return hr;
		}
	PROFILE_END("m_MobSchMan.FrameMove()");

	PROFILE_BEGIN("m_MobSchManEx.FrameMove()");
		// Note : 몹소환 by 경대
		//
		m_MobSchManEx.FrameMove ( fTime, fElapsedTime );
	PROFILE_END("m_MobSchManEx.FrameMove()");

	PROFILE_BEGIN("UpdateSkillAct()");
		//	Note : 지연된 스킬 영향 처리.
		//
		UpdateSkillAct(fElapsedTime);
	PROFILE_END("UpdateSkillAct()");

	PROFILE_BEGIN("pCharNode->Data->FrameMove()");
		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			PGLCHAR pChar = pCharNode->Data;
			if( !pChar ) continue;

			pChar->FrameMove ( fTime, fElapsedTime );
		}
	PROFILE_END("pCharNode->Data->FrameMove()");

	// PET
	PROFILE_BEGIN("pPetNode->Data->FrameMove ()");
	GLPETNODE* pPetNode = m_GlobPETList.m_pHead;
	for ( ; pPetNode; pPetNode = pPetNode->pNext )
	{
		PGLPETFIELD pPetfield = pPetNode->Data;
		if( !pPetfield ) continue;

		if ( pPetfield->IsValid () ) 
		{
			pPetfield->FrameMove ( fTime, fElapsedTime );
			if ( pPetfield->IsNotEnoughFull () ) 
			{
				glGaeaServer.ReserveDropOutPet ( SDROPOUTPETINFO(pPetfield->m_dwGUID,false,false) );
			}
		}
	}
	PROFILE_END("pPetNode->Data->FrameMove ()");

	// SUMMON
	PROFILE_BEGIN("pSummonNode->Data->FrameMove ()");
	GLSUMMONNODE* pSummonNode = m_GlobSummonList.m_pHead;
	for ( ; pSummonNode; pSummonNode = pSummonNode->pNext )
	{
		PGLSUMMONFIELD pSummonfield = pSummonNode->Data;
		if( !pSummonfield ) continue;

		if ( pSummonfield->IsValid () ) 
		{
			pSummonfield->FrameMove ( fTime, fElapsedTime );

			// 소환수의 HP가 0이면 삭제한다.
			if( pSummonfield->IsDie() )
			{
//				glGaeaServer.DropOutSummon ( pSummonfield->m_dwGUID,true );
				glGaeaServer.ReserveDropOutSummon ( SDROPOUTSUMMONINFO(pSummonfield->m_dwGUID,true) );
			}
		}else{
			glGaeaServer.ReserveDropOutSummon ( SDROPOUTSUMMONINFO(pSummonfield->m_dwGUID,true) );
		}
	}
	PROFILE_END("pSummonNode->Data->FrameMove ()");

	PROFILE_BEGIN("MoveChar()");
		//	Note : PC 'MOVE CELL'가 등록된 셀을 업데이트함.
		//
		pCharNode = m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			PGLCHAR pPC = pCharNode->Data;
			if( !pPC ) continue;

			MoveChar ( pPC->m_dwGaeaID, pPC->GetPosition() );
		}
	PROFILE_END("MoveChar()");

	// PET
	PROFILE_BEGIN("MovePET()");
		pPetNode = m_GlobPETList.m_pHead;
		for ( ; pPetNode; pPetNode = pPetNode->pNext )
		{
			PGLPETFIELD pPet = pPetNode->Data;
			if( !pPet ) continue;

			MovePet ( pPet->m_dwGUID, pPet->m_vPos );
		}
	PROFILE_END("MovePET()");

	// SUMMON
	PROFILE_BEGIN("MoveSummon()");
	pSummonNode = m_GlobSummonList.m_pHead;
	for ( ; pSummonNode; pSummonNode = pSummonNode->pNext )
	{
		PGLSUMMONFIELD pSummon = pSummonNode->Data;
		if( !pSummon ) continue;

		MoveSummon ( pSummon->m_dwGUID, pSummon->m_vPos );
	}
	PROFILE_END("MoveSummon()");

	PROFILE_BEGIN("pCharNode->Data->UpdateViewAround()");
		//	Note : 케릭터가 보고 있는 영역을 검사하고 갱신한다. - 가장 마지막 부분에서 해야함.
		//		다른 (PC,MOB,NPC)등 이동개체가 모두 'MOVE CELL' 을 한후에 행해져야 한다.
		//
		{
			pCharNode = m_GlobPCList.m_pHead;
			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				PGLCHAR pChar = pCharNode->Data;
				if( !pChar ) continue;

				pChar->UpdateViewAround ();
			}
		}
	PROFILE_END("pCharNode->Data->UpdateViewAround()");

	PROFILE_BEGIN("Weather()");
	//	Note : 날씨 변화 클라이언트에 업데이트.
	//
	UpdateWeather();
	PROFILE_END("Weather()");

	return S_OK;
}

void GLLandMan::UpdateWeather()
{
	DWORD dwNowWeather(0);
	DWORD i(0);

	std::vector<SONEMAPWEATHER> vecMapNowWeather;
	vecMapNowWeather.clear();

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();
	GLPeriod & glPeriod = GLPeriod::GetInstance();

	if ( IsWeatherActive() )
	{

		//////////////////////////////////////////////////////////////////////////
		/// 전체 맵이 추가되는 날씨

		dwNowWeather = glPeriod.GetWeather ();

		if ( m_dwOldWeather != dwNowWeather )
		{
			GLMSG::SNETPC_WEATHER NetMsg;
			NetMsg.dwWeather = dwNowWeather;

			GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				PGLCHAR pChar = pCharNode->Data;
				if( !pChar ) continue;

				glGaeaServer.SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
			}

			m_dwOldWeather = dwNowWeather;
		}

		//	Note : 일시적 날씨 효과. ( 번개 등등. )
		//
		DWORD dwWhimsical = glPeriod.GetWhimsicalWeather();
		if ( dwWhimsical )
		{
			GLMSG::SNETPC_WHIMSICALWEATHER NetMsg;
			NetMsg.dwWhimsical = dwWhimsical;

			GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				PGLCHAR pChar = pCharNode->Data;
				if( !pChar ) continue;

				glGaeaServer.SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		/// 특정 맵에 추가되는 날씨
		// Add OneMap Weather
		vecMapNowWeather = glPeriod.GetOneMapWeather();
		bool bNewWeather = FALSE;
		if( vecMapNowWeather.size() != m_vecMapOldWeather.size() )
		{
			bNewWeather = TRUE;
		}else{
			for( i = 0; i < m_vecMapOldWeather.size(); i++ )
			{
				SONEMAPWEATHER mapNowWeather = vecMapNowWeather[i];
				SONEMAPWEATHER mapOldWeather = m_vecMapOldWeather[i];
				if( mapNowWeather != mapOldWeather )
				{
					bNewWeather = TRUE;
					break;
				}
			}
		}

		if( bNewWeather )
		{
			GLMSG::SNETPC_MAPWEATHER NetMsg;
			if( vecMapNowWeather.size() >= 32 )
			{
				NetMsg.dwMapWeatherSize = 31;
			}else{
				NetMsg.dwMapWeatherSize = vecMapNowWeather.size();
			}


			DWORD i;
			for( i = 0; i < vecMapNowWeather.size(); i++ )
			{
				if( i >= 32 ) break;
				NetMsg.MapWeather[i] = vecMapNowWeather[i];
			}


			GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				PGLCHAR pChar = pCharNode->Data;
				if( !pChar ) continue;

				glGaeaServer.SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
			}
			m_vecMapOldWeather = vecMapNowWeather; 
		}


		//	Note : 일시적 날씨 효과. ( 번개 등등. )
		//
		for( i = 0; i < m_vecMapOldWeather.size(); i++ )
		{
			DWORD dwWhimsical = m_vecMapOldWeather[i].dwWhimsicalWeather;
			if ( dwWhimsical )
			{
				GLMSG::SNETPC_MAPWHIMSICALWEATHER NetMsg;
				NetMsg.MapWeather = m_vecMapOldWeather[i];

				GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
				for ( ; pCharNode; pCharNode = pCharNode->pNext )
				{
					PGLCHAR pChar = pCharNode->Data;
					if( !pChar ) continue;

					glGaeaServer.SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
				}
			}
		}

	}
}

HRESULT	GLLandMan::RendQuad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 트리가 생성되었을때만 그려준다.
	//
	if ( m_pNaviMesh )
	{
		D3DXVECTOR3 vMax, vMin;
		m_pNaviMesh->GetAABB ( vMax, vMin );
		EDITMESHS::RENDERAABB ( pd3dDevice, vMax, vMin );
	}

	if ( m_LandTree.GetRootNode() )
	{
		D3DXVECTOR3 vCenter ( (float)m_LandTree.GetAxisX(), 0.f, (float)m_LandTree.GetAxisZ() );
		D3DXVECTOR3 vSize ( (float)m_LandTree.GetSizeX()/2, 0.f, (float)m_LandTree.GetSizeZ()/2 );
		D3DXVECTOR3 vSnap ( (float)m_LandTree.GetDivSizeX(), 0, (float)m_LandTree.GetDivSizeZ() );

		DWORD dwZFunc;
		pd3dDevice->GetRenderState ( D3DRS_ZFUNC,		&dwZFunc );
		pd3dDevice->SetRenderState ( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
		EDITMESHS::RENDERPLANE ( pd3dDevice, vCenter, vSize, vSnap );
		pd3dDevice->SetRenderState ( D3DRS_ZFUNC,		dwZFunc );
	}

	return S_OK;
}

HRESULT GLLandMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	GLCROWNODE *pCurCrow = m_GlobCROWList.m_pHead;
	for ( ; pCurCrow; pCurCrow = pCurCrow->pNext )
	{
		PGLCROW pCrow = pCurCrow->Data;
		if( !pCrow ) continue;

		//D3DXVECTOR3 vScr0 = DxViewPort::GetInstance().ComputeVec3Project ( &pCurCrow->Data->GetPosition(), NULL );
		//CDebugSet::ToPos ( vScr0.x, vScr0.y, pCurCrow->Data->m_pCrowData->GetName() );

		pCrow->Render ( pd3dDevice, cv, TRUE );
	}

	if ( m_pNaviMesh ) m_pNaviMesh->Render ( pd3dDevice );

	//ITEMDROPNODE* pCurItem = m_GlobItemList.m_pHead;
	//for ( ; pCurItem; pCurItem = pCurItem->pNext )
	//{
	//	D3DXVECTOR3 &vPos = pCurItem->Data->vPos;
	//	SNATIVEID &sItemID = pCurItem->Data->sItemCustom.sNativeID;

	//	//D3DXVECTOR3 vScr0 = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );
	//	//SITEM* pItem = GLItemMan::GetInstance().GetItem ( sItemID );
	//	//CDebugSet::ToPos ( vScr0.x, vScr0.y, "%s (%3.2f)", pItem->szName, pCurItem->Data->fAge );

	//	//DxSimMesh* pSimMesh = GLItemMan::GetInstance().LoadSimMesh ( sItemID );
	//	//EDITMESHS::RENDERAABB ( pd3dDevice, );

	//	//D3DXMATRIX matWld;
	//	//D3DXMatrixTranslation ( &matWld, vPos.x, vPos.y, vPos.z );
	//	//pd3dDevice->SetTransform ( D3DTS_WORLD, &matWld );

	//	//pSimMesh->Render ( pd3dDevice );
	//}

	//MONEYDROPNODE* pCurMoney = m_GlobMoneyList.m_pHead;
	//for ( ; pCurMoney; pCurMoney = pCurMoney->pNext )
	//{
	//	D3DXVECTOR3 &vPos = pCurMoney->Data->vPos;
	//	D3DXVECTOR3 vScr0 = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

	//	//CDebugSet::ToPos ( vScr0.x, vScr0.y, "Money (%d)", pCurMoney->Data->lnAmount );
	//}

	GLLevelFile::Render ( pd3dDevice, cv );;

	return S_OK;
}


HRESULT GLLandMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GLLevelFile::InitDeviceObjects ( m_pd3dDevice );
	return S_OK;
}

HRESULT GLLandMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT GLLandMan::InvalidateDeviceObjects()
{
	return S_OK;
}

HRESULT GLLandMan::DeleteDeviceObjects()
{
	GLLevelFile::DeleteDeviceObjects ();
	return S_OK;
}

BOOL GLLandMan::IsCollisionNavi ( D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vCollision )
{
	BOOL bCollision;
	DWORD dwCollisionID;
	m_pNaviMesh->IsCollision ( vPoint1, vPoint2, vCollision, dwCollisionID, bCollision );

	return bCollision;
}

void GLLandMan::DoSkillAct ( const SSKILLACT &sACT )
{
	GLACTOR* pREACTOR = GLGaeaServer::GetInstance().GetTarget ( this, sACT.sID_TAR );
	if ( !pREACTOR )					return;
	if ( !pREACTOR->IsValidBody() )		return;

	//	Note : 공격 속성이고 플레이어에서 플레이어를 공격시에, 대련자인지 검사.
	//
	bool bREACT(true);
	int nVAR_HP = sACT.nVAR_HP;
	if( sACT.IsATTACK()  )
	{
		if ( sACT.sID.emCrow==CROW_PC && pREACTOR->GetCrow()==CROW_PC )
		{
			bREACT = false;
			PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( sACT.sID.dwID );
			if ( pCHAR )
			{
				bREACT = pCHAR->IsReActionable ( pREACTOR );
			
				if ( sACT.nVAR_HP<0 )
				{
					switch ( sACT.emAPPLY )
					{
					case SKILL::EMAPPLY_PHY_SHORT:
					case SKILL::EMAPPLY_PHY_LONG:
						nVAR_HP = int(sACT.nVAR_HP*GLCONST_CHAR::fPK_POINT_DEC_PHY);
						break;
					case SKILL::EMAPPLY_MAGIC:
						nVAR_HP = int(sACT.nVAR_HP*GLCONST_CHAR::fPK_POINT_DEC_MAGIC);
						break;
					};
				}
			}
		}else if ( sACT.sID.emCrow==CROW_SUMMON && pREACTOR->GetCrow()==CROW_SUMMON )
		{
			bREACT = false;
			PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon ( sACT.sID.dwID );
			if ( pSummon )
			{
				bREACT = pSummon->m_pOwner->IsReActionable ( pREACTOR );

				if ( sACT.nVAR_HP<0 )
				{
					switch ( sACT.emAPPLY )
					{
					case SKILL::EMAPPLY_PHY_SHORT:
					case SKILL::EMAPPLY_PHY_LONG:
						nVAR_HP = int(sACT.nVAR_HP*GLCONST_CHAR::fPK_POINT_DEC_PHY);
						break;
					case SKILL::EMAPPLY_MAGIC:
						nVAR_HP = int(sACT.nVAR_HP*GLCONST_CHAR::fPK_POINT_DEC_MAGIC);
						break;
					};
				}
			}
		}
	}


	if ( !bREACT )						return;

	if ( sACT.VALID_VAR() )
	{
		//	Note : 타겟의 기초 체력 변화.
		//
		pREACTOR->VAR_BODY_POINT ( sACT.sID.emCrow, sACT.sID.dwID, FALSE, nVAR_HP, sACT.nVAR_MP, sACT.nVAR_SP );

		//	Note : 발동된 스킬 값 MSG.
		//

		GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
		NetMsgBRD.emCrow = sACT.sID_TAR.emCrow;
		NetMsgBRD.dwID = sACT.sID_TAR.dwID;

		//	스킬을 사용한 행위자 마킹.
		NetMsgBRD.sACTOR.wCrow = (WORD) sACT.sID.emCrow;
		NetMsgBRD.sACTOR.wID = (WORD) sACT.sID.dwID;

		NetMsgBRD.dwDamageFlag = sACT.dwDamageFlag;
		NetMsgBRD.nVAR_HP = nVAR_HP;
		NetMsgBRD.nVAR_MP = sACT.nVAR_MP;
		NetMsgBRD.nVAR_SP = sACT.nVAR_SP;

		//	Note : '스킬대상'의 주변 클라이언트들에게 메세지 전송.
		//
		pREACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBRD );

		//	Note : 스킬 대상에게 메세지 전송.
		if ( sACT.sID_TAR.emCrow==CROW_PC || sACT.sID_TAR.emCrow==CROW_SUMMON )	
			GLGaeaServer::GetInstance().SENDTOCLIENT ( pREACTOR->GETCLIENTID (), &NetMsgBRD );		
	}

	GLACTOR* pACTOR = GLGaeaServer::GetInstance().GetTarget ( this, sACT.sID );
	if ( !pACTOR )						return;

	if ( sACT.VALID_GATHER() )
	{
		//	Note 자신에게 흡수값 반영.
		//
		pACTOR->VAR_BODY_POINT ( sACT.sID.emCrow, sACT.sID.dwID, FALSE, sACT.nGATHER_HP, sACT.nGATHER_MP, sACT.nGATHER_SP );

		//	Note : 발동된 스킬 값 MSG.
		//
		GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
		NetMsgBRD.emCrow = sACT.sID.emCrow;
		NetMsgBRD.dwID = sACT.sID.dwID;
		NetMsgBRD.sACTOR.wCrow = (WORD) sACT.sID.emCrow; //	스킬을 사용한 행위자 마킹.
		NetMsgBRD.sACTOR.wID = (WORD) sACT.sID.dwID;

		NetMsgBRD.dwDamageFlag = sACT.dwDamageFlag;
		NetMsgBRD.nVAR_HP = sACT.nGATHER_HP;
		NetMsgBRD.nVAR_MP = sACT.nGATHER_MP;
		NetMsgBRD.nVAR_SP = sACT.nGATHER_SP;

		//	Note : '스킬대상'의 주변 클라이언트들에게 메세지 전송.
		//
		pACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBRD );

		//	Note : 스킬 대상에게 메세지 전송.
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pACTOR->GETCLIENTID(), &NetMsgBRD );
	}
}

void GLLandMan::DoSkillActEx ( const SSKILLACTEX &sACTEX )
{
	GLACTOR* pREACTOR = GLGaeaServer::GetInstance().GetTarget ( this, sACTEX.sID_TAR );
	if ( !pREACTOR )					return;
	if ( !pREACTOR->IsValidBody() )		return;
	if ( pREACTOR->GetNowHP()==0 )		return;

	//	Note : 상대방에게 지속형 스킬 이팩트를 삽입 시도. ( 스킬 설정에 따라 지속성 여부 다름. )
	//
	if ( sACTEX.idSKILL!=SNATIVEID(false) )
	{
		DWORD dwSELSLOT(0);
		pREACTOR->RECEIVE_SKILLFACT ( sACTEX.idSKILL, sACTEX.wSKILL_LVL, dwSELSLOT );

		//	Note : 지속형 스킬일 경우.
		if ( dwSELSLOT != SKILLFACT_SIZE )
		{
			//	Note : 발동된 스킬 값 MSG.
			//
			GLMSG::SNETPC_SKILLHOLD_BRD NetMsgBRD;
			NetMsgBRD.emCrow = sACTEX.sID_TAR.emCrow;
			NetMsgBRD.dwID = sACTEX.sID_TAR.dwID;

			NetMsgBRD.skill_id = sACTEX.idSKILL;
			NetMsgBRD.wLEVEL = sACTEX.wSKILL_LVL;
			NetMsgBRD.wSELSLOT = static_cast<WORD>(dwSELSLOT);

			//	Note : '스킬대상'의 주변 클라이언트들에게 메세지 전송.
			//
			//	Note : 스킬 대상에게 메세지 전송.
			if ( sACTEX.sID_TAR.emCrow==CROW_PC )
			{
				PGLCHAR pChar = (PGLCHAR) pREACTOR;
				
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sACTEX.idSKILL );

				if ( pSkill && !( pChar->m_bVehicle && pSkill->m_sBASIC.emIMPACT_SIDE != SIDE_ENERMY) )
				{
					GLGaeaServer::GetInstance().SENDTOCLIENT ( pREACTOR->GETCLIENTID (), &NetMsgBRD );
					pREACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBRD );
				}
			}
			else
			{
				pREACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBRD );
			}
			
		}
	}

	if ( sACTEX.dwCUREFLAG!=NULL )
	{
		pREACTOR->CURE_STATEBLOW ( sACTEX.dwCUREFLAG );
	}

	if ( sACTEX.dwRemoveFlag!=NULL)
	{
		pREACTOR->BUFF_REMOVE ( sACTEX.dwRemoveFlag );
	}

	if ( sACTEX.sSTATEBLOW.emBLOW!=EMBLOW_NONE )
	{
		pREACTOR->STATEBLOW ( sACTEX.sSTATEBLOW );

		//	Note : 상태이상 발생 Msg.
		//
		GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
		NetMsgState.emCrow = sACTEX.sID_TAR.emCrow;
		NetMsgState.dwID = sACTEX.sID_TAR.dwID;
		NetMsgState.emBLOW = sACTEX.sSTATEBLOW.emBLOW;
		NetMsgState.fAGE = sACTEX.sSTATEBLOW.fAGE;
		NetMsgState.fSTATE_VAR1 = sACTEX.sSTATEBLOW.fSTATE_VAR1;
		NetMsgState.fSTATE_VAR2 = sACTEX.sSTATEBLOW.fSTATE_VAR2;

		//	Note : '스킬대상'의 주변 클라이언트들에게 메세지 전송.
		//
		pREACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgState );

		//	Note : 스킬 대상에게 메세지 전송.
		if ( sACTEX.sID_TAR.emCrow==CROW_PC )	GLGaeaServer::GetInstance().SENDTOCLIENT ( pREACTOR->GETCLIENTID (), &NetMsgState );
	}

	// 강한타격
	if ( sACTEX.bCrushBlow )
	{
		PushPullAct( sACTEX.sID, sACTEX.sID_TAR, sACTEX.fPUSH_PULL );
	}
	// 밀어내기
	else
	{
		PushPullAct( sACTEX.sID_TAR, sACTEX.sID_TAR, sACTEX.fPUSH_PULL );
	}
}

void GLLandMan::PushPullAct( const STARGETID dwActorID, const STARGETID dwReActorID, const float fPUSH_PULL )
{
	if ( fPUSH_PULL!=0.0f )
	{	
		float fPushPull = fPUSH_PULL;

		GLACTOR* pACTOR = GLGaeaServer::GetInstance().GetTarget ( this, dwActorID );
		if ( !pACTOR )	return;

		GLACTOR* pREACTOR = GLGaeaServer::GetInstance().GetTarget ( this, dwReActorID );
		if ( !pREACTOR )	return;

		D3DXVECTOR3 vREACTOR_POS = pREACTOR->GetPosition();
		D3DXVECTOR3 vDir = ( vREACTOR_POS - pACTOR->GetPosition() );	//	REACTOR -> ACTOR 방향의 백터.

		vDir.y = 0.0f;
		D3DXVec3Normalize (  &vDir, &vDir );

		NavigationMesh* pNavi = GetNavi();
		if ( pNavi )
		{
			D3DXVECTOR3 vPosA = vREACTOR_POS;
			D3DXVECTOR3 vPosB = vREACTOR_POS + vDir * fPushPull;

			BOOL bCoA(FALSE);
			DWORD dwCoA(0);
			D3DXVECTOR3 vCoA(0,0,0);

			pNavi->IsCollision ( D3DXVECTOR3(vPosA.x,vPosA.y+5.0f,vPosA.z), D3DXVECTOR3(vPosA.x,vPosA.y-5.0f,vPosA.z), vCoA, dwCoA, bCoA );
			if ( bCoA )
			{
				BOOL bCoB(FALSE);
				DWORD dwCoB(0);
				D3DXVECTOR3 vCoB(0,0,0);

				pNavi->IsCollision ( D3DXVECTOR3(vPosB.x,vPosB.y+5.0f,vPosB.z), D3DXVECTOR3(vPosB.x,vPosB.y-5.0f,vPosB.z), vCoB, dwCoB, bCoB );
				if ( !bCoB )
				{
					float fdxLength = 1.0f;
					if ( fPushPull > 40.0f )		fdxLength = 10.0f;
					else if ( fPushPull > 15.0f )	fdxLength = 5.0f;
					else if ( fPushPull > 5.0f )	fdxLength = 2.0f;
					else							fdxLength = 1.0f;

					float fNewPushPull = fPushPull - fdxLength;
					while ( 0.0f < fNewPushPull )
					{
						vPosB = vREACTOR_POS + vDir * fNewPushPull;
						pNavi->IsCollision ( D3DXVECTOR3(vPosB.x,vPosB.y+5.0f,vPosB.z), D3DXVECTOR3(vPosB.x,vPosB.y-5.0f,vPosB.z), vCoB, dwCoB, bCoB );
						if ( bCoB )		break;

						fNewPushPull -= fdxLength;
					};
				}

				if ( bCoB )
				{
					if ( pNavi->LineOfSightTest ( dwCoA, vCoA, dwCoB, vCoB ) )
					{
						pREACTOR->ReceivePushPull ( vCoB );
					}
				}
			}
		}
	}
}

void GLLandMan::RegSkillAct ( const SSKILLACT &sACT )
{
	//	Note : 지연처리.
	if ( sACT.IsDELAY() )		m_listSkillAct.push_back(sACT);
	//	Note : 즉시처리.
	else						DoSkillAct(sACT);
}

void GLLandMan::RegSkillActEx ( const SSKILLACTEX &sACTEX )
{
	//	Note : 지연처리.
	if ( sACTEX.IsDELAY() )		m_listSkillActEx.push_back(sACTEX);
	//	Note : 즉시처리.
	else						DoSkillActEx(sACTEX);
}

void GLLandMan::UpdateSkillAct ( float fElapsedTime )
{
	{
		LISTSACT_ITER iter = m_listSkillAct.begin();
		LISTSACT_ITER iter_end = m_listSkillAct.end();
		for ( ; iter!=iter_end; )
		{
			LISTSACT_ITER iter_cur = iter++;

			SSKILLACT &sACT = (*iter_cur);
			sACT.fDELAY -= fElapsedTime;

			if ( sACT.fDELAY <= 0.0f )
			{
				DoSkillAct(sACT);

				m_listSkillAct.erase(iter_cur);
			}
		}
	}

	{
		LISTSACTEX_ITER iter = m_listSkillActEx.begin();
		LISTSACTEX_ITER iter_end = m_listSkillActEx.end();
		for ( ; iter!=iter_end; )
		{
			LISTSACTEX_ITER iter_cur = iter++;

			SSKILLACTEX &sACT = (*iter_cur);
			sACT.fDELAY -= fElapsedTime;

			if ( sACT.fDELAY <= 0.0f )
			{
				DoSkillActEx(sACT);

				m_listSkillActEx.erase(iter_cur);
			}
		}
	}
}

BOOL GLLandMan::LoadMobSchManEx( const char *szFile ) // by 경대
{
	GASSERT(szFile);

	if( !ClearMobSchManEx() ) return FALSE;

	GLLevelFile glLevelFile;
	if ( !glLevelFile.LoadFile ( szFile, FALSE, m_pd3dDevice ) ) return FALSE;

	m_MobSchManEx = *(glLevelFile.GetMobSchMan());
	
	MOBSCHEDULELIST * GLMobSchList = m_MobSchManEx.GetMobSchList();
	SGLNODE<GLMobSchedule*>* pCur = GLMobSchList->m_pHead;
	while ( pCur )
	{
		pCur->Data->SetExMode();
		pCur = pCur->pNext;
	}

	m_MobSchManEx.SetLandMan( this );
	
	return TRUE;
}

BOOL GLLandMan::ClearMobSchManEx() // by 경대
{
	MOBSCHEDULELIST * GLMobSchList = m_MobSchManEx.GetMobSchList();
	SGLNODE<GLMobSchedule*>* pCur = GLMobSchList->m_pHead;
	while ( pCur )
	{


		if ( pCur->Data->m_dwGlobID != UINT_MAX )
		{
			PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pCur->Data->m_CrowID );
			if ( !pCrowData )	continue;

			if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
			{
				m_listDelGlobIDExMat.remove(pCur->Data->m_dwGlobID);
				DropOutMaterial( pCur->Data->m_dwGlobID );
			}
			else
			{
				m_listDelGlobIDEx.remove(pCur->Data->m_dwGlobID);
				DropOutCrow( pCur->Data->m_dwGlobID );
			}
		}
		pCur = pCur->pNext;
	}
	
	MAPMOBGROUB mapMobGroub = m_MobSchManEx.GetMobGroubMap ();
	MAPMOBGROUB_ITER pos = mapMobGroub.begin ();
	for ( ; pos != mapMobGroub.end(); ++pos )
	{
		GLMOBGROUB &sGROUB = (*pos).second;
		if ( sGROUB.m_vecSchedule.empty() )		continue;

		GLMobSchedule* pMobSch = sGROUB.m_vecSchedule[sGROUB.m_dwRendGenIndex];
		
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pMobSch->m_CrowID );
		if ( !pCrowData )	continue;
		
		if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
		{
			DropOutMaterial ( pMobSch->m_dwGlobID );
		}
		else
		{
			DropOutCrow ( pMobSch->m_dwGlobID );
		}
	}

	m_MobSchManEx.CleanUp();

	return TRUE;
}

// *****************************************************
// Desc: 지정한 클럽원을 제외한 모든 케릭터 강제 이동
// *****************************************************
BOOL GLLandMan::DoGateOutPCAll ( DWORD dwExceptClubID, DWORD dwGateID )
{
	std::vector<DWORD> vecGATEOUTPC;
	vecGATEOUTPC.reserve ( m_GlobPCList.m_dwAmount );

	GLClubMan& cClubMan = GLGaeaServer::GetInstance().GetClubMan();
	GLCLUB *pCLUB = cClubMan.GetClub ( dwExceptClubID );
	
	if ( !pCLUB ) return FALSE;

	//	Note : 결정전 맵에 존재하는 동맹의 맴버를 제외한 모든 케릭터 강제 이동.
	// 동맹이 있을때
	if ( pCLUB->IsAlliance() )
	{
		GLCLUB* pCHIEFCLUB = cClubMan.GetClub ( pCLUB->m_dwAlliance );
		if ( !pCHIEFCLUB )	return FALSE;

		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			bool bIsExceptChar(false);

			CLUB_ALLIANCE_ITER pos = pCHIEFCLUB->m_setAlliance.begin();
			CLUB_ALLIANCE_ITER end = pCHIEFCLUB->m_setAlliance.end();
			for ( ; pos!=end; ++pos )
			{
				const GLCLUBALLIANCE &sExceptClub = *pos;

				GLCLUB* pAlliClub = cClubMan.GetClub ( sExceptClub.m_dwID );
				if ( pAlliClub )
				{
					if ( pCharNode->Data->m_dwGuild == sExceptClub.m_dwID )
					{
						bIsExceptChar = true;
					}
				}
			}

			if ( !bIsExceptChar )
			{
				vecGATEOUTPC.push_back ( pCharNode->Data->m_dwGaeaID );
			}
		}
	}
	else
	{
		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			if ( pCharNode->Data->m_dwGuild == dwExceptClubID )			continue;
			vecGATEOUTPC.push_back ( pCharNode->Data->m_dwGaeaID );
		}
	}

	GLMSG::SNETREQ_GATEOUT sNetMsg;
	sNetMsg.dwGateID = dwGateID;

	std::vector<DWORD>::size_type nSIZE = vecGATEOUTPC.size();
	for ( std::vector<DWORD>::size_type i=0; i<nSIZE; ++i )
	{
		PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( vecGATEOUTPC[i] );
		if ( !pCHAR )	continue;

		sNetMsg.dwGaeaID = pCHAR->m_dwGaeaID;
		GLGaeaServer::GetInstance().SENDTOAGENT ( &sNetMsg );
	}

	return TRUE;
}

BOOL GLLandMan::DoGateOutPCAll ( DWORD dwGateID )
{
	GLMSG::SNETREQ_GATEOUT sNetMsg;
	sNetMsg.dwGateID = dwGateID;

	GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
	for ( ; pCharNode; pCharNode = pCharNode->pNext )
	{
		sNetMsg.dwGaeaID = pCharNode->Data->m_dwGaeaID;
		GLGaeaServer::GetInstance().SENDTOAGENT( &sNetMsg );
	}

	return TRUE;
}

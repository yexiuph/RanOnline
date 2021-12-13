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
//			/// ��ü ���� �߰��Ǵ� ����
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
//			//	Note : �Ͻ��� ���� ȿ��. ( ���� ���. )
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
//			/// Ư�� �ʿ� �߰��Ǵ� ����
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
//			//	Note : �Ͻ��� ���� ȿ��. ( ���� ���. )
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

	//	Note : �� �������� �ѹ� ��ȸ�� ������. ( �� GEN )
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

	//	������ �߻�.
	SITEMCUSTOM sITEM_NEW;
	sITEM_NEW.sNativeID = sNID;
	CTime cTIME = CTime::GetCurrentTime();
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	//	�������� ��� Ƚ�� ǥ��. ( �Ҹ�ǰ�� ��� x ��, �Ϲ� ��ǰ 1 )
	sITEM_NEW.wTurnNum = pITEM->GETAPPLYNUM();

	//	�߻��� ���� ���.
	sITEM_NEW.cGenType = pNetMsg->emGENTYPE;
	sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
	sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
	sITEM_NEW.lnGenNum = pNetMsg->lnGENCOUNT;

	// ������ ����Ʈ���� �Է��� ���� ��� ���� ( ���� : RequestGenItem, ������ ���� �ƴѵ� )
	//sITEM_NEW.cDAMAGE = (BYTE)pITEM->sBasicOp.wGradeAttack;
	//sITEM_NEW.cDEFENSE = (BYTE)pITEM->sBasicOp.wGradeDefense;

	//	���� �ɼ� ����.
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

	//	�̺�Ʈ �������� ��� ������Ʈ�� ���ļ� ����.
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

	//	������ �߻�.
	SITEMCUSTOM sITEM_NEW;
	sITEM_NEW.sNativeID = sNID;
	CTime cTIME = CTime::GetCurrentTime();
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	//	�������� ��� Ƚ�� ǥ��. ( �Ҹ�ǰ�� ��� x ��, �Ϲ� ��ǰ 1 )
	sITEM_NEW.wTurnNum = pITEM->GETAPPLYNUM();

	//	�߻��� ���� ���.
	sITEM_NEW.cGenType = emGENTYPE;
	sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
	sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
	sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sNID, emGENTYPE );

	// ������ ����Ʈ���� �Է��� ���� ��� ���� ( ���� : DropGetItem, ������ ���� �ƴѵ� )
	//sITEM_NEW.cDAMAGE = (BYTE)pITEM->sBasicOp.wGradeAttack;
	//sITEM_NEW.cDEFENSE = (BYTE)pITEM->sBasicOp.wGradeDefense;

	//	���� �ɼ� ����.
	if( sITEM_NEW.GENERATE_RANDOM_OPT() )
	{
		GLITEMLMT::GetInstance().ReqRandomItem( sITEM_NEW );
	}

	DropItem ( vPos, &sITEM_NEW, emGroup, dwHoldGID );

	return TRUE;
}

BOOL GLLandMan::DropItem ( D3DXVECTOR3 &vPos, SITEMCUSTOM *pItemCustom, EMGROUP emGroup, DWORD dwHoldGID )
{
	//	Note : ������ ��ġ ��带 ã�´�.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return FALSE;

	SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemCustom->sNativeID);
	if ( !pItem )		return false;

	//	Note : Global ID�� �Ҵ��Ѵ�.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeItemGIDs.GetHead ( dwGlobID ) )		return FALSE;
	m_FreeItemGIDs.DelHead ();

	//	Note : �޸𸮸� �Ҵ�.
	//
	CItemDrop *pNewItem = m_ItemMemPool.New();

	//	Note : ������ ���� ( ����, ��ġ ).
	//
	pNewItem->sItemCustom = *pItemCustom;
	pNewItem->vPos = vPos;

	pNewItem->fAge = 0.0f;
	pNewItem->emGroup = emGroup;
	pNewItem->dwHoldGID = dwHoldGID;

	//	Note : ���� ID �ο�.
	//
	pNewItem->sMapID = m_sMapID;
	pNewItem->dwGlobID = dwGlobID;
	pNewItem->dwCeID = pLandNode->dwCID;

	//	Note : ���� ��� ������.
	//
	m_ItemArray[dwGlobID] = pNewItem;												// - ����.
	pNewItem->pGlobList = m_GlobItemList.ADDHEAD ( pNewItem );						// - ����.

	pNewItem->pQuadNode = pLandNode;												// - Ʈ�� ���.
	pNewItem->pCellList = pLandNode->pData->m_ItemList.ADDHEAD ( pNewItem );		// - Ʈ�� ���.

	return TRUE;
}

BOOL GLLandMan::DropMoney(D3DXVECTOR3 &vPos, LONGLONG lnAmount, bool bDropMonster /* = FALSE */, 
						  EMGROUP emGroup/* =EMGROUP_ONE */, DWORD dwHoldGID/* =GAEAID_NULL  */)
{
	//	Note : ������ ��ġ ��带 ã�´�.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	FALSE;

	//	Note : Global ID�� �Ҵ��Ѵ�.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeMoneyGIDs.GetHead ( dwGlobID ) )	return	FALSE;
	m_FreeMoneyGIDs.DelHead ();

	//	Note : �޸𸮸� �Ҵ�.
	//
	CMoneyDrop *pNewMoney = m_MoneyMemPool.New();

	//	Note : Money ���� ( ����, ��ġ ).
	//
	pNewMoney->fAge = 0.0f;
	pNewMoney->vPos = vPos;
	pNewMoney->lnAmount = lnAmount;
	pNewMoney->emGroup = emGroup;
	pNewMoney->dwHoldGID = dwHoldGID;

	//	Note : ���� ID �ο�.
	//
	pNewMoney->sMapID = m_sMapID;
	pNewMoney->dwGlobID = dwGlobID;
	pNewMoney->dwCeID = pLandNode->dwCID;

	pNewMoney->bDropMonster = bDropMonster;

	//	Note : ���� ��� ������.
	//
	m_MoneyArray[dwGlobID] = pNewMoney;											// - ����.
	pNewMoney->pGlobList = m_GlobMoneyList.ADDHEAD ( pNewMoney );				// - ����.

	pNewMoney->pQuadNode = pLandNode;											// - Ʈ�� ���.
	pNewMoney->pCellList = pLandNode->pData->m_MoneyList.ADDHEAD ( pNewMoney );	// - Ʈ�� ���.

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

	//	Note : ������ ��ġ ��带 ã�´�.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	UINT_MAX;

	//	Note : Global ID�� �Ҵ��Ѵ�.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeCROWGIDs.GetHead ( dwGlobID ) )	return	UINT_MAX;
	m_FreeCROWGIDs.DelHead ();

	//	Note : �޸𸮸� �Ҵ�.
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

	//	Note : ���� ID �ο�.
	//
	pNewCrow->m_sMapID = m_sMapID;
	pNewCrow->m_dwGlobID = dwGlobID;
	pNewCrow->m_dwCeID = pLandNode->dwCID;

	//	Note : ���� ��� ������.
	//
	m_CROWArray[dwGlobID] = pNewCrow;											// - ����.

	// BOSS ���̸�
	if ( pNewCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_BOSS )
	{
		pNewCrow->m_pGlobNode = m_GlobBOSSCROWList.ADDHEAD ( pNewCrow );		// - ����.
	}
	else
	{
		pNewCrow->m_pGlobNode = m_GlobCROWList.ADDHEAD ( pNewCrow );			// - ����.
	}

	pNewCrow->m_pQuadNode = pLandNode;											// - Ʈ�� ���.
	pNewCrow->m_pCellNode = pLandNode->pData->m_CROWList.ADDHEAD ( pNewCrow );	// - Ʈ�� ���.

	//	Note : �˸� �޽��� �ʿ����� �˻��� �޽��� �߻�.
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

	// �׷� ���̸� �׷� ������Ʈ�� �ִ´�.
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

	//	Note : ������ ��ġ ��带 ã�´�.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	UINT_MAX;

	//	Note : Global ID�� �Ҵ��Ѵ�.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeCROWGIDs.GetHead ( dwGlobID ) )	return	UINT_MAX;
	m_FreeCROWGIDs.DelHead ();

	//	Note : �޸𸮸� �Ҵ�.
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

	//	Note : ���� ID �ο�.
	//
	pNewCrow->m_sMapID = m_sMapID;
	pNewCrow->m_dwGlobID = dwGlobID;
	pNewCrow->m_dwCeID = pLandNode->dwCID;

	//	Note : ���� ��� ������.
	//
	m_CROWArray[dwGlobID] = pNewCrow;											// - ����.

	// BOSS ���̸�
	if ( pNewCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_BOSS )
	{
		pNewCrow->m_pGlobNode = m_GlobBOSSCROWList.ADDHEAD ( pNewCrow );		// - ����.
	}
	else
	{
		pNewCrow->m_pGlobNode = m_GlobCROWList.ADDHEAD ( pNewCrow );			// - ����.
	}

	pNewCrow->m_pQuadNode = pLandNode;											// - Ʈ�� ���.
	pNewCrow->m_pCellNode = pLandNode->pData->m_CROWList.ADDHEAD ( pNewCrow );	// - Ʈ�� ���.

	//	Note : �˸� �޽��� �ʿ����� �˻��� �޽��� �߻�.
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

	// �׷� ���̸� �׷� ������Ʈ�� �ִ´�.
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

	//	�Ҽ��� ���е� ������Ŀ� y��ǥ�� ���� collision�� ������ ����
	//	y( -1000,1000 ) ���� ���̸� ���������� �����ϳ� �ش� ���� ���̸�
	//	��Ȯ�� �����ϱ� ������� �ϴ� ������
//	GetNavi()->IsCollision ( vPos+D3DXVECTOR3(0,-1000,0), vPos+D3DXVECTOR3(0,+1000,0), vCollision, dwCoID, bCol );
//	if ( !bCol )	return UINT_MAX;

	//	Note : ������ ��ġ ��带 ã�´�.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)fPosX, (int)fPosY );
	if ( !pLandNode )	return	UINT_MAX;

	//	Note : Global ID�� �Ҵ��Ѵ�.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeCROWGIDs.GetHead ( dwGlobID ) )	return	UINT_MAX;
	m_FreeCROWGIDs.DelHead ();

	//	Note : �޸𸮸� �Ҵ�.
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

	//	Note : ���� ID �ο�.
	//
	pNewCrow->m_sMapID = m_sMapID;
	pNewCrow->m_dwGlobID = dwGlobID;
	pNewCrow->m_dwCeID = pLandNode->dwCID;

	//	Note : ���� ��� ������.
	//
	m_CROWArray[dwGlobID] = pNewCrow;											// - ����.

	// BOSS ���̸�
	if ( pNewCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_BOSS )
	{
		pNewCrow->m_pGlobNode = m_GlobBOSSCROWList.ADDHEAD ( pNewCrow );		// - ����.
	}
	else
	{
		pNewCrow->m_pGlobNode = m_GlobCROWList.ADDHEAD ( pNewCrow );			// - ����.
	}

	pNewCrow->m_pQuadNode = pLandNode;											// - Ʈ�� ���.
	pNewCrow->m_pCellNode = pLandNode->pData->m_CROWList.ADDHEAD ( pNewCrow );	// - Ʈ�� ���.

	//	Note : �˸� �޽��� �ʿ����� �˻��� �޽��� �߻�.
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

	// �׷� ���̸� �׷� ������Ʈ�� �ִ´�.
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

	//	Note : ������ ��ġ ��带 ã�´�.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	UINT_MAX;

	//	Note : Global ID�� �Ҵ��Ѵ�.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeMaterialGIDs.GetHead ( dwGlobID ) )	return	UINT_MAX;
	m_FreeMaterialGIDs.DelHead ();

	//	Note : �޸𸮸� �Ҵ�.
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

	//	Note : ���� ID �ο�.
	//
	pNewMaterial->m_sMapID = m_sMapID;
	pNewMaterial->m_dwGlobID = dwGlobID;
	pNewMaterial->m_dwCeID = pLandNode->dwCID;

	//	Note : ���� ��� ������.
	//
	m_MaterialArray[dwGlobID] = pNewMaterial;											// - ����.

	pNewMaterial->m_pGlobNode = m_GlobMaterialList.ADDHEAD ( pNewMaterial );				// - ����.

	pNewMaterial->m_pQuadNode = pLandNode;											// - Ʈ�� ���.
	pNewMaterial->m_pCellNode = pLandNode->pData->m_MaterialList.ADDHEAD ( pNewMaterial );	// - Ʈ�� ���.

	return dwGlobID;
}

DWORD GLLandMan::DropMaterial ( SNATIVEID sNID, float fPosX, float fPosY )
{
	HRESULT hr;

	D3DXVECTOR3 vPos(fPosX,0,fPosY);

	// D3DXVECTOR3 vCollision;
	// DWORD dwCoID;
	// BOOL bCol;

	//	�Ҽ��� ���е� ������Ŀ� y��ǥ�� ���� collision�� ������ ����
	//	y( -1000,1000 ) ���� ���̸� ���������� �����ϳ� �ش� ���� ���̸�
	//	��Ȯ�� �����ϱ� ������� �ϴ� ������
//	GetNavi()->IsCollision ( vPos+D3DXVECTOR3(0,-1000,0), vPos+D3DXVECTOR3(0,+1000,0), vCollision, dwCoID, bCol );
//	if ( !bCol )	return UINT_MAX;

	//	Note : ������ ��ġ ��带 ã�´�.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)fPosX, (int)fPosY );
	if ( !pLandNode )	return	UINT_MAX;

	//	Note : Global ID�� �Ҵ��Ѵ�.
	//
	DWORD dwGlobID = -1;
	if ( !m_FreeMaterialGIDs.GetHead ( dwGlobID ) )	return	UINT_MAX;
	m_FreeMaterialGIDs.DelHead ();

	//	Note : �޸𸮸� �Ҵ�.
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

	//	Note : ���� ID �ο�.
	//
	pNewMaterial->m_sMapID = m_sMapID;
	pNewMaterial->m_dwGlobID = dwGlobID;
	pNewMaterial->m_dwCeID = pLandNode->dwCID;

	//	Note : ���� ��� ������.
	//
	m_MaterialArray[dwGlobID] = pNewMaterial;											// - ����.

	pNewMaterial->m_pGlobNode = m_GlobMaterialList.ADDHEAD ( pNewMaterial );				// - ����.

	pNewMaterial->m_pQuadNode = pLandNode;											// - Ʈ�� ���.
	pNewMaterial->m_pCellNode = pLandNode->pData->m_MaterialList.ADDHEAD ( pNewMaterial );	// - Ʈ�� ���.

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

	//	Note : Global-ID ����.
	//
	m_FreeItemGIDs.AddTail ( dwGlobID );
	m_ItemArray[dwGlobID] = NULL;

	//	Note : Global-List ����.
	//
	if ( pItemDrop->pGlobList )
		m_GlobItemList.DELNODE ( pItemDrop->pGlobList );

	//	Note : QuadNode-List ����.
	//
	LANDQUADNODE* pLandNode = pItemDrop->pQuadNode;
	if ( pItemDrop->pCellList )
		pLandNode->pData->m_ItemList.DELNODE ( pItemDrop->pCellList );

	// ��ī���� ��� �������� ������� ��DB ����
	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( pItemDrop->sItemCustom.sNativeID );
	if ( pITEM && pITEM->sBasicOp.emItemType == ITEM_PET_CARD && pItemDrop->sItemCustom.dwPetID != 0 )
	{
		// ��ī�� ���� �������� DB���� �����ϰ� �α� �߰�
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

	//	Note : �޸� ����.
	//
	m_ItemMemPool.Release(pItemDrop);

	return TRUE;
}

BOOL GLLandMan::DropOutMoney ( DWORD dwGlobID )
{
	GASSERT ( dwGlobID<MAXMONEY );
	if ( m_MoneyArray[dwGlobID] == NULL )	return FALSE;

	PMONEYDROP pMoneyDrop = m_MoneyArray[dwGlobID];

	//	Note : Global-ID ����.
	//
	m_FreeMoneyGIDs.AddTail ( dwGlobID );
	m_MoneyArray[dwGlobID] = NULL;

	//	Note : Global-List ����.
	//
	if ( pMoneyDrop->pGlobList )
		m_GlobMoneyList.DELNODE ( pMoneyDrop->pGlobList );

	//	Note : QuadNode-List ����.
	//
	LANDQUADNODE* pLandNode = pMoneyDrop->pQuadNode;
	if ( pMoneyDrop->pCellList )
		pLandNode->pData->m_MoneyList.DELNODE ( pMoneyDrop->pCellList );

	//	Note : �޸� ����.
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

	// �׷� ���̸� �׷� ������Ʈ�� �ִ´�.
	if ( pCrow->IsGroupMember() )
	{
		DEL_GROUPMEMBER( pCrow->GetGroupName(), dwGlobID );
		if( pCrow->IsGroupLeader() )
		{
			DEL_GROUPLEADER( pCrow->GetGroupName() );
			m_mapGroupInfo[pCrow->GetGroupName()].sTargetID.RESET();
		}
	}

	//	Note : Global-ID ����.
	//
	m_FreeCROWGIDs.AddTail ( dwGlobID );
	m_CROWArray[dwGlobID] = NULL;

	//	Note : Global-List ����.
	//
	
	if ( pCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_BOSS )
	{
		if ( pCrow->m_pGlobNode )
			m_GlobBOSSCROWList.DELNODE ( pCrow->m_pGlobNode );		// - ����.
	}
	else
	{
		if ( pCrow->m_pGlobNode )
			m_GlobCROWList.DELNODE ( pCrow->m_pGlobNode );
	}

	//	Note : QuadNode-List ����.
	//
	LANDQUADNODE* pLandNode = pCrow->m_pQuadNode;
	if ( pLandNode && pCrow->m_pCellNode )
		pLandNode->pData->m_CROWList.DELNODE ( pCrow->m_pCellNode );

	//	Note : ���������� ���� �÷��׸� ������Ŵ.
	GLMobSchedule* pMobSch = pCrow->GetMobSchedule();
	if ( pMobSch )
	{
		pMobSch->m_bALive = FALSE;
		pMobSch->m_dwGlobID = UINT_MAX;
	}

	//	Note : �޸� ����.
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

	//	Note : Global-ID ����.
	//
	m_FreeMaterialGIDs.AddTail ( dwGlobID );
	m_MaterialArray[dwGlobID] = NULL;

	//	Note : Global-List ����.
	//
	
	if ( pMaterial->m_pGlobNode )
		m_GlobMaterialList.DELNODE ( pMaterial->m_pGlobNode );

	//	Note : QuadNode-List ����.
	//
	LANDQUADNODE* pLandNode = pMaterial->m_pQuadNode;
	if ( pLandNode && pMaterial->m_pCellNode )
		pLandNode->pData->m_MaterialList.DELNODE ( pMaterial->m_pCellNode );

	//	Note : ���������� ���� �÷��׸� ������Ŵ.
	GLMobSchedule* pMobSch = pMaterial->GetMobSchedule();
	if ( pMobSch )
	{
		pMobSch->m_bALive = FALSE;
		pMobSch->m_dwGlobID = UINT_MAX;
	}

	//	Note : �޸� ����.
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
		// Note : Level Edit �۾��� ���⼭ NULL ���� ���Դ�. 
		CDebugSet::ToLogFile( "GLLandMan::MoveCrow() - pCrow==NULL" );
		return FALSE;
	}

	LANDQUADNODE* pPrevNode = pCrow->m_pQuadNode;
	if( !pPrevNode )
	{
		CDebugSet::ToLogFile( "GLLandMan::MoveCrow() - pPrevNode==NULL" );
		return FALSE;
	}

	//	Note : ���� ������ ����� �ʾ��� ��� �״�� �д�.
	//
	if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )	return FALSE;

	//	Note : �� ������ ��ġ ��带 ã�´�.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )	return FALSE;

	//	Note : ���� ����Ʈ���� ����.
	if ( pCrow->m_pCellNode )
		pPrevNode->pData->m_CROWList.DELNODE ( pCrow->m_pCellNode );

	//	Note : ã�� ��忡 ����.
	pCrow->m_pQuadNode = pNextNode;											// - Ʈ�� ���.
	pCrow->m_pCellNode = pNextNode->pData->m_CROWList.ADDHEAD ( pCrow );	// - Ʈ�� ���.

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
		GASSERT ( pPet->m_pCellNode && "GLLandMan::RemovePet() - pQuadNode �� ����ÿ��� �ݵ�� m_pCellNode�� ��ȿ�ؾ��մϴ�." );
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
		GASSERT ( pSummon->m_pCellNode && "GLLandMan::RemoveSummon() - pQuadNode �� ����ÿ��� �ݵ�� m_pCellNode�� ��ȿ�ؾ��մϴ�." );
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
		GASSERT ( pChar->m_pCellNode && "GLLandMan::RemoveChar() - pQuadNode �� �����ÿ��� �ݵ�� m_pCellNode�� ��ȿ�ؾ��մϴ�." );
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

	//	Note : ���� ������ ����� �ʾ��� ��� �״�� �д�.
	//
	LANDQUADNODE* pPrevNode = pPet->m_pQuadNode;
	if ( pPrevNode )
        if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )	 return FALSE;

	//	Note : �� ������ ��ġ ��带 ã�´�.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )	return FALSE;

	//	Note : ���� ����Ʈ���� ����.
	if ( pPet->m_pCellNode )
		pPrevNode->pData->m_PETList.DELNODE ( pPet->m_pCellNode );

	//	Note : ã�� ��忡 ����.
	pPet->m_pQuadNode = pNextNode;										// - Ʈ�� ���.
	pPet->m_pCellNode = pNextNode->pData->m_PETList.ADDHEAD ( pPet );	// - Ʈ�� ���.

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

	//	Note : ���� ������ ����� �ʾ��� ��� �״�� �д�.
	//
	LANDQUADNODE* pPrevNode = pSummon->m_pQuadNode;
	if ( pPrevNode )
		if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )	 return FALSE;

	//	Note : �� ������ ��ġ ��带 ã�´�.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )	return FALSE;

	//	Note : ���� ����Ʈ���� ����.
	if ( pSummon->m_pCellNode )
		pPrevNode->pData->m_SummonList.DELNODE ( pSummon->m_pCellNode );

	//	Note : ã�� ��忡 ����.
	pSummon->m_pQuadNode = pNextNode;										// - Ʈ�� ���.
	pSummon->m_pCellNode = pNextNode->pData->m_SummonList.ADDHEAD ( pSummon );	// - Ʈ�� ���.

	return TRUE;
}

BOOL GLLandMan::MoveChar ( DWORD dwGaeaID, const D3DXVECTOR3 &vPos )
{
	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwGaeaID );
	GASSERT ( pChar && "GLLandMan::MoveChar" );

	if ( !pChar )
	{
		DEBUGMSG_WRITE ( "ERROR : GLLandMan::MoveChar() ���� ��ȿȭ�� dwGaeaID(%d) �߰ߵ�.", dwGaeaID );
		return FALSE;
	}

	if ( !pChar->IsSTATE(EM_GETVA_AFTER) )	return FALSE;

	LANDQUADNODE* pPrevNode = pChar->m_pQuadNode;

	//	Note : ���� ������ ����� �ʾ��� ��� �״�� �д�.
	//
	if ( pPrevNode )
		if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )	return FALSE;

	//	Note : �� ������ ��ġ ��带 ã�´�.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )	return FALSE;

	//	Note : ���� ����Ʈ���� ����.
	if ( pChar->m_pCellNode )
		pPrevNode->pData->m_PCList.DELNODE ( pChar->m_pCellNode );

	//	Note : ã�� ��忡 ����.
	pChar->m_pQuadNode = pNextNode;										// - Ʈ�� ���.
	pChar->m_pCellNode = pNextNode->pData->m_PCList.ADDHEAD ( pChar );	// - Ʈ�� ���.

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
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"�ε����� ������� �ʰ��Ͽ����ϴ�.");

		GLLandNode *pLandNode = pQuadNode->pData;
		
		GLCHARNODE *pCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext )
		{
			PGLCHAR pChar = pCur->Data;
			if( !pChar ) continue;

			//	Note : GM ĳ���� ���� ���� �ʰ�.
			//
			if ( !pChar->IsValidBody() )							continue;
			if ( !pChar->IsVisibleDetect(pFinder->m_bRECVISIBLE) )	continue;

			//	��ýÿ��� ���� �ɸ��͸� �������� ����.
			if ( pChar->GETCONFTING()->IsCONFRONTING() )			continue;

			BOOL bTarget = FALSE;
			float fTarLeng = FLT_MAX;
			if ( emAttRgType==EMATT_SHORT )
			{
				//	Note : �ٰŸ� ������ ���.
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

			//	Note : �þ߰� Ȯ���Ǵ°�?
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

	//	Note : A ~ B ������ ����.
	int nX = int ( vDetectPosA.x + vDetectPosB.x ) / 2;
	int nZ = int ( vDetectPosA.z + vDetectPosB.z ) / 2;

	//	Note : �������� ���̵� ������ �Ÿ�.
	int nSX = abs ( int ( vDetectPosA.x - vDetectPosB.x ) / 2 );
	int nSZ = abs ( int ( vDetectPosA.x - vDetectPosB.x ) / 2 );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nSX,nZ+nSZ,nX-nSX,nZ-nSZ);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );


	D3DXVECTOR3 vMax, vMin;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"�ε����� ������� �ʰ��Ͽ����ϴ�.");

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

			//	Note : �þ߰� Ȯ���Ǵ°�?
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
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"�ε����� ������� �ʰ��Ͽ����ϴ�.");

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

			//	Note : �þ߰� Ȯ���Ǵ°�?
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
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"�ε����� ������� �ʰ��Ͽ����ϴ�.");

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

			//	Note : �þ߰� Ȯ���Ǵ°�?
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
	//	Note : Suit ������Ʈ.
	//
	SGLNODE<PITEMDROP>* pCurItem = m_GlobItemList.m_pHead;
	while ( pCurItem )
	{
		//	Note : ������ ������.
		PITEMDROP pItem = pCurItem->Data;
		if( !pItem ) continue;

		pItem->Update(fElapsedTime);

		//	Note : ������ �������� �̵�.
		pCurItem = pCurItem->pNext;

		//	Note : ���� �ð� ���޽�. ( ������ �̵��ڿ� ������ �Ͼ�� ����ó����. )
		//
		if ( pItem->IsDropOut() )		DropOutItem ( pItem->dwGlobID );
	}

	//	Note : Money ������Ʈ.
	//
	SGLNODE<PMONEYDROP>* pCurMoney = m_GlobMoneyList.m_pHead;
	while ( pCurMoney )
	{
		//	Note : ������ ������.
		CMoneyDrop* pMoney= pCurMoney->Data;
		if( !pMoney ) continue;

		pMoney->Update(fElapsedTime);

		//	Note : ������ �������� �̵�.
		pCurMoney = pCurMoney->pNext;

		//	Note : ���� �ð� ���޽�. ( ������ �̵��ڿ� ������ �Ͼ�� ����ó����. )
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

		//	��ȯ�� NPC ����
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

			// ��������� Crow ó��
			if ( pCrow->m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_AUTODROP )
			{
				if ( pCrow->GetAge () > pCrow->m_pCrowData->m_sAction.m_fLiveTime )
				{
					DropOutCrow ( pCrow->m_dwGlobID );
				}
			}

			//	��ȯ�� NPC ����
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

			// ��������� Crow ó��
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

		// ��������� Crow ó��
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

	// Gate�� �� ã�������
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

		//	Note : ��ǥ ����Ʈ ��������.					//

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

		// ������ �����̻� ����
		if ( sCurMapID != pChar->m_sMapID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		pChar->DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		pChar->DISABLESKEFF ( i );
		}

		pChar->ResetAction();

		//	Note : �� �̵� ������ �˸�.
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

	

	// ��Ƽ �δ��� ��� ��Ƽ�� �ٸ��ų� �ƴ� ������� �Ѿ� ����.
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
	// ��Ƽ �δ��� �ƴ� ��쿣 gaeaID�� HOST ID�� �ٸ��� �Ѿ� ����.
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
		// Note : ����ȯ by ���
		//
		m_MobSchManEx.FrameMove ( fTime, fElapsedTime );
	PROFILE_END("m_MobSchManEx.FrameMove()");

	PROFILE_BEGIN("UpdateSkillAct()");
		//	Note : ������ ��ų ���� ó��.
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

			// ��ȯ���� HP�� 0�̸� �����Ѵ�.
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
		//	Note : PC 'MOVE CELL'�� ��ϵ� ���� ������Ʈ��.
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
		//	Note : �ɸ��Ͱ� ���� �ִ� ������ �˻��ϰ� �����Ѵ�. - ���� ������ �κп��� �ؾ���.
		//		�ٸ� (PC,MOB,NPC)�� �̵���ü�� ��� 'MOVE CELL' �� ���Ŀ� �������� �Ѵ�.
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
	//	Note : ���� ��ȭ Ŭ���̾�Ʈ�� ������Ʈ.
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
		/// ��ü ���� �߰��Ǵ� ����

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

		//	Note : �Ͻ��� ���� ȿ��. ( ���� ���. )
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
		/// Ư�� �ʿ� �߰��Ǵ� ����
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


		//	Note : �Ͻ��� ���� ȿ��. ( ���� ���. )
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
	//	Note : Ʈ���� �����Ǿ������� �׷��ش�.
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

	//	Note : ���� �Ӽ��̰� �÷��̾�� �÷��̾ ���ݽÿ�, ��������� �˻�.
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
		//	Note : Ÿ���� ���� ü�� ��ȭ.
		//
		pREACTOR->VAR_BODY_POINT ( sACT.sID.emCrow, sACT.sID.dwID, FALSE, nVAR_HP, sACT.nVAR_MP, sACT.nVAR_SP );

		//	Note : �ߵ��� ��ų �� MSG.
		//

		GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
		NetMsgBRD.emCrow = sACT.sID_TAR.emCrow;
		NetMsgBRD.dwID = sACT.sID_TAR.dwID;

		//	��ų�� ����� ������ ��ŷ.
		NetMsgBRD.sACTOR.wCrow = (WORD) sACT.sID.emCrow;
		NetMsgBRD.sACTOR.wID = (WORD) sACT.sID.dwID;

		NetMsgBRD.dwDamageFlag = sACT.dwDamageFlag;
		NetMsgBRD.nVAR_HP = nVAR_HP;
		NetMsgBRD.nVAR_MP = sACT.nVAR_MP;
		NetMsgBRD.nVAR_SP = sACT.nVAR_SP;

		//	Note : '��ų���'�� �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
		//
		pREACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBRD );

		//	Note : ��ų ��󿡰� �޼��� ����.
		if ( sACT.sID_TAR.emCrow==CROW_PC || sACT.sID_TAR.emCrow==CROW_SUMMON )	
			GLGaeaServer::GetInstance().SENDTOCLIENT ( pREACTOR->GETCLIENTID (), &NetMsgBRD );		
	}

	GLACTOR* pACTOR = GLGaeaServer::GetInstance().GetTarget ( this, sACT.sID );
	if ( !pACTOR )						return;

	if ( sACT.VALID_GATHER() )
	{
		//	Note �ڽſ��� ����� �ݿ�.
		//
		pACTOR->VAR_BODY_POINT ( sACT.sID.emCrow, sACT.sID.dwID, FALSE, sACT.nGATHER_HP, sACT.nGATHER_MP, sACT.nGATHER_SP );

		//	Note : �ߵ��� ��ų �� MSG.
		//
		GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
		NetMsgBRD.emCrow = sACT.sID.emCrow;
		NetMsgBRD.dwID = sACT.sID.dwID;
		NetMsgBRD.sACTOR.wCrow = (WORD) sACT.sID.emCrow; //	��ų�� ����� ������ ��ŷ.
		NetMsgBRD.sACTOR.wID = (WORD) sACT.sID.dwID;

		NetMsgBRD.dwDamageFlag = sACT.dwDamageFlag;
		NetMsgBRD.nVAR_HP = sACT.nGATHER_HP;
		NetMsgBRD.nVAR_MP = sACT.nGATHER_MP;
		NetMsgBRD.nVAR_SP = sACT.nGATHER_SP;

		//	Note : '��ų���'�� �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
		//
		pACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBRD );

		//	Note : ��ų ��󿡰� �޼��� ����.
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pACTOR->GETCLIENTID(), &NetMsgBRD );
	}
}

void GLLandMan::DoSkillActEx ( const SSKILLACTEX &sACTEX )
{
	GLACTOR* pREACTOR = GLGaeaServer::GetInstance().GetTarget ( this, sACTEX.sID_TAR );
	if ( !pREACTOR )					return;
	if ( !pREACTOR->IsValidBody() )		return;
	if ( pREACTOR->GetNowHP()==0 )		return;

	//	Note : ���濡�� ������ ��ų ����Ʈ�� ���� �õ�. ( ��ų ������ ���� ���Ӽ� ���� �ٸ�. )
	//
	if ( sACTEX.idSKILL!=SNATIVEID(false) )
	{
		DWORD dwSELSLOT(0);
		pREACTOR->RECEIVE_SKILLFACT ( sACTEX.idSKILL, sACTEX.wSKILL_LVL, dwSELSLOT );

		//	Note : ������ ��ų�� ���.
		if ( dwSELSLOT != SKILLFACT_SIZE )
		{
			//	Note : �ߵ��� ��ų �� MSG.
			//
			GLMSG::SNETPC_SKILLHOLD_BRD NetMsgBRD;
			NetMsgBRD.emCrow = sACTEX.sID_TAR.emCrow;
			NetMsgBRD.dwID = sACTEX.sID_TAR.dwID;

			NetMsgBRD.skill_id = sACTEX.idSKILL;
			NetMsgBRD.wLEVEL = sACTEX.wSKILL_LVL;
			NetMsgBRD.wSELSLOT = static_cast<WORD>(dwSELSLOT);

			//	Note : '��ų���'�� �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
			//
			//	Note : ��ų ��󿡰� �޼��� ����.
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

		//	Note : �����̻� �߻� Msg.
		//
		GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
		NetMsgState.emCrow = sACTEX.sID_TAR.emCrow;
		NetMsgState.dwID = sACTEX.sID_TAR.dwID;
		NetMsgState.emBLOW = sACTEX.sSTATEBLOW.emBLOW;
		NetMsgState.fAGE = sACTEX.sSTATEBLOW.fAGE;
		NetMsgState.fSTATE_VAR1 = sACTEX.sSTATEBLOW.fSTATE_VAR1;
		NetMsgState.fSTATE_VAR2 = sACTEX.sSTATEBLOW.fSTATE_VAR2;

		//	Note : '��ų���'�� �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
		//
		pREACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgState );

		//	Note : ��ų ��󿡰� �޼��� ����.
		if ( sACTEX.sID_TAR.emCrow==CROW_PC )	GLGaeaServer::GetInstance().SENDTOCLIENT ( pREACTOR->GETCLIENTID (), &NetMsgState );
	}

	// ����Ÿ��
	if ( sACTEX.bCrushBlow )
	{
		PushPullAct( sACTEX.sID, sACTEX.sID_TAR, sACTEX.fPUSH_PULL );
	}
	// �о��
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
		D3DXVECTOR3 vDir = ( vREACTOR_POS - pACTOR->GetPosition() );	//	REACTOR -> ACTOR ������ ����.

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
	//	Note : ����ó��.
	if ( sACT.IsDELAY() )		m_listSkillAct.push_back(sACT);
	//	Note : ���ó��.
	else						DoSkillAct(sACT);
}

void GLLandMan::RegSkillActEx ( const SSKILLACTEX &sACTEX )
{
	//	Note : ����ó��.
	if ( sACTEX.IsDELAY() )		m_listSkillActEx.push_back(sACTEX);
	//	Note : ���ó��.
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

BOOL GLLandMan::LoadMobSchManEx( const char *szFile ) // by ���
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

BOOL GLLandMan::ClearMobSchManEx() // by ���
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
// Desc: ������ Ŭ������ ������ ��� �ɸ��� ���� �̵�
// *****************************************************
BOOL GLLandMan::DoGateOutPCAll ( DWORD dwExceptClubID, DWORD dwGateID )
{
	std::vector<DWORD> vecGATEOUTPC;
	vecGATEOUTPC.reserve ( m_GlobPCList.m_dwAmount );

	GLClubMan& cClubMan = GLGaeaServer::GetInstance().GetClubMan();
	GLCLUB *pCLUB = cClubMan.GetClub ( dwExceptClubID );
	
	if ( !pCLUB ) return FALSE;

	//	Note : ������ �ʿ� �����ϴ� ������ �ɹ��� ������ ��� �ɸ��� ���� �̵�.
	// ������ ������
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

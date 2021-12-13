#include "pch.h"
#include "Psapi.h"
#pragma comment( lib, "Psapi.lib" )
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "./GLGaeaServer.h"
#include "GLClubDeathMatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLGaeaServer& GLGaeaServer::GetInstance()
{
	static GLGaeaServer Instance;
	return Instance;
}

GLGaeaServer::GLGaeaServer(void) :
	m_pd3dDevice(NULL),
	m_pMsgServer(NULL),
	m_pDBMan(NULL),
	m_nServerChannel(0),
	m_dwFieldSvrID(0),

	m_bUseIntelTBB(FALSE),

	m_dwAgentSlot(0),

	m_bEmulator(false),
	m_bUpdate(TRUE),
	m_bGenItemHold(true),
	//m_fMaxDelayMsgProc(0),

	m_dwMaxClient(1000),

	m_fTIMER_CLUB(0),

	m_bBigHead(false),
	m_bBigHand(false),
	m_bBrightEvent(false),
	m_bReservedStop(false),
	m_bClubBattleStarted(false),
	m_bClubDMStarted(false),
	m_nServiceProvider(0),
	m_fDelayMilliSec(1.0f),
	m_dwServerCrashTime(0),
	m_fCrashTimer(0),
	m_wSkipPacketNum(0),
	m_bEmptyMsg(false)
{
	InitializeCriticalSection(&m_CSPCLock);
}

GLGaeaServer::~GLGaeaServer(void)
{
	DeleteCriticalSection(&m_CSPCLock);
	
	SAFE_DELETE_ARRAY (m_PCArray);
	SAFE_DELETE_ARRAY (m_PETArray);
	SAFE_DELETE_ARRAY (m_SummonArray);
}

GLCrow* GLGaeaServer::NEW_CROW ()
{

	EnterCriticalSection(&m_CSPCLock);

	GLCrow* returnCrow = m_poolCROW.New();

	LeaveCriticalSection(&m_CSPCLock);

	return returnCrow;
}

void GLGaeaServer::RELEASE_CROW ( GLCrow* pCROW )
{
	GASSERT(pCROW&&"GLGaeaServer::RELEASE_CROW()");
	if ( !pCROW )	return;

	pCROW->RESET_DATA();
	m_poolCROW.ReleaseNonInit ( pCROW );
}

GLMaterial* GLGaeaServer::NEW_MATERIAL ()
{

	EnterCriticalSection(&m_CSPCLock);

	GLMaterial* returnMaterial = m_poolMATERIAL.New();

	LeaveCriticalSection(&m_CSPCLock);

	return returnMaterial;
}

void GLGaeaServer::RELEASE_MATERIAL ( GLMaterial* pMaterial )
{
	GASSERT(pMaterial&&"GLGaeaServer::RELEASE_MATERIAL()");
	if ( !pMaterial )	return;

	pMaterial->RESET_DATA();
	m_poolMATERIAL.ReleaseNonInit ( pMaterial );
}

GLChar* GLGaeaServer::NEW_CHAR ()
{
	EnterCriticalSection(&m_CSPCLock);

	GLChar* returnChar = m_poolCHAR.New();

	LeaveCriticalSection(&m_CSPCLock);

	return returnChar;
}

void GLGaeaServer::RELEASE_CHAR ( GLChar* pCHAR )
{
	GASSERT(pCHAR&&"GLGaeaServer::RELEASE_CHAR()");
	if ( !pCHAR )	return;

	EnterCriticalSection(&m_CSPCLock);

	pCHAR->RESET_DATA();
	m_poolCHAR.ReleaseNonInit ( pCHAR );

	LeaveCriticalSection(&m_CSPCLock);
}

SFIELDCROW* GLGaeaServer::NEW_FIELDCROW ()
{
	EnterCriticalSection(&m_CSPCLock);

	SFIELDCROW* returnCrow = m_poolFIELDCROW.New();

	LeaveCriticalSection(&m_CSPCLock);

	return returnCrow;
}

void GLGaeaServer::RELEASE_FIELDCROW ( SFIELDCROW* pFIELDCROW )
{
	GASSERT(pFIELDCROW&&"GLGaeaServer::RELEASE_CHAR()");
	if ( !pFIELDCROW )	return;

	EnterCriticalSection(&m_CSPCLock);

	pFIELDCROW->RESET();
	m_poolFIELDCROW.ReleaseNonInit ( pFIELDCROW );

	LeaveCriticalSection(&m_CSPCLock);
}

PGLPETFIELD GLGaeaServer::NEW_PET ()
{

	EnterCriticalSection(&m_CSPCLock);

	GLPetField* returnPet = m_poolPET.New();

	LeaveCriticalSection(&m_CSPCLock);

	return returnPet;
}

void GLGaeaServer::RELEASE_PET ( PGLPETFIELD pPet )
{
	GASSERT ( pPet && "GLGaeaServer::RELEASE_PET()" );
	if ( !pPet ) return;

	EnterCriticalSection(&m_CSPCLock);

	pPet->CleanUp ();
	m_poolPET.ReleaseNonInit ( pPet );

	LeaveCriticalSection(&m_CSPCLock);
}

PGLLANDMAN GLGaeaServer::NEW_GLLANDMAN ()
{

	EnterCriticalSection(&m_CSPCLock);

	PGLLANDMAN returnGLLandMan = m_poolGLLandMan.New();

	LeaveCriticalSection(&m_CSPCLock);

	return returnGLLandMan;
}

void GLGaeaServer::RELEASE_GLLANDMAN ( PGLLANDMAN pGLLandMan )
{
	GASSERT ( pGLLandMan && "GLGaeaServer::RELEASE_GLLANDMAN()" );
	if ( !pGLLandMan ) return;

	EnterCriticalSection(&m_CSPCLock);

	pGLLandMan->CleanUp();
	pGLLandMan->ResetLandMan();
	m_poolGLLandMan.ReleaseNonInit ( pGLLandMan );

	LeaveCriticalSection(&m_CSPCLock);
}

GLLandMan* GLGaeaServer::GetRootMap ()
{
	//	if ( !m_LandManList.m_pHead )	return NULL;

	//	return m_LandManList.m_pHead->Data;

	if( m_vecLandMan.empty() ) return NULL;

	return m_vecLandMan[0];
}

GLLandMan* GLGaeaServer::GetByMapID ( const SNATIVEID &sMapID )
{
	GASSERT(sMapID.wMainID<MAXLANDMID);
	GASSERT(sMapID.wSubID<MAXLANDSID);

	if ( sMapID.wMainID>=MAXLANDMID )	return NULL;
	if ( sMapID.wSubID>=MAXLANDSID )	return NULL;

	return m_pLandMan[sMapID.wMainID][sMapID.wSubID];
}

GLLandMan* GLGaeaServer::GetInstantMapByMapID ( const SNATIVEID &sMapID )
{
	size_t i, size = m_vecInstantMapSrcLandMan.size();
	for( i = 0; i < size; i++ )
	{
		if( m_vecInstantMapSrcLandMan[i]->GetMapID() == sMapID ) return m_vecInstantMapSrcLandMan[i];
	}
	return NULL;
}

HRESULT GLGaeaServer::OneTimeSceneInit ()
{
	m_FreePETGIDs.RemoveAll ();
	m_FreeSummonGIDs.RemoveAll();

	for ( DWORD i=0; i<m_dwMaxClient; i++ )	m_FreePETGIDs.AddTail ( i );
	for ( DWORD i=0; i<m_dwMaxClient; i++ )	m_FreeSummonGIDs.AddTail ( i );

	SecureZeroMemory ( m_pLandMan, sizeof(GLLandMan*)*MAXLANDMID*MAXLANDSID );

	SAFE_DELETE_ARRAY(m_PCArray);
	m_PCArray = new PGLCHAR[m_dwMaxClient];
	SecureZeroMemory ( m_PCArray, sizeof(PGLCHAR)*m_dwMaxClient );

	SAFE_DELETE_ARRAY (m_PETArray);
	m_PETArray = new PGLPETFIELD[m_dwMaxClient];
	SecureZeroMemory ( m_PETArray, sizeof(PGLPETFIELD)*m_dwMaxClient );

	SAFE_DELETE_ARRAY (m_SummonArray);
	m_SummonArray = new PGLSUMMONFIELD[m_dwMaxClient];
	SecureZeroMemory ( m_SummonArray, sizeof(PGLSUMMONFIELD)*m_dwMaxClient );

	return S_OK;
}

HRESULT GLGaeaServer::Create ( DWORD dwMaxClient, DxMsgServer *pMsgServer, DxConsoleMsg* pConsoleMsg, GLDBMan* pDBMan, int nServiceProvider, const char* szMapList, DWORD dwFieldSID, int nChannel, BOOL bUseIntelTBB )
{
	HRESULT hr;
	CleanUp ();

	m_bReservedStop = false;

	m_dwMaxClient = dwMaxClient;
	m_pMsgServer = pMsgServer;
	m_pConsoleMsg = pConsoleMsg;
	m_pDBMan = pDBMan;

	m_nServerChannel = nChannel;

	m_bUseIntelTBB   = bUseIntelTBB;

	m_nServiceProvider = nServiceProvider;

	//	Note : 필드 서버 ID가 무효일 경우 ( 에뮬레이터 설정. )
	m_dwFieldSvrID = dwFieldSID;
	if ( dwFieldSID==FIELDSERVER_MAX )
	{
		m_bEmulator = true;
		m_dwFieldSvrID = 0;
	}

	hr = OneTimeSceneInit ();
	if ( FAILED(hr) )	return E_FAIL;
	
	if ( !szMapList )
	{
		hr = LoadMapList ( "mapslist.ini", NULL, FIELDSERVER_MAX );
		if ( FAILED(hr) )	return E_FAIL;
	}
	else
	{
		hr = LoadMapList ( szMapList, NULL, FIELDSERVER_MAX );
		if ( FAILED(hr) )	return E_FAIL;
	}

	GLITEMLMT::GetInstance().SetDBMan ( pDBMan );
	GLITEMLMT::GetInstance().SetServer ( pMsgServer, dwFieldSID );
	GLITEMLMT::GetInstance().ReadMaxKey ();

	FIELDMAP_ITER iter = m_MapList.begin ();
	FIELDMAP_ITER iter_end = m_MapList.end ();

	for ( ; iter!=iter_end; ++iter )
	{
		SMAPNODE *pMapNode = &(*iter).second;

		//	Note : 지정된 서버군에 포함되지 않는 map 은 제외 시킴.
		//
		if ( dwFieldSID!=FIELDSERVER_MAX && pMapNode->dwFieldSID!=dwFieldSID )		continue;

		//	Note : 필드 서버 ID가 무효일 경우 ( 에뮬레이터 설정. )
		if ( dwFieldSID==FIELDSERVER_MAX )
		{
			pMapNode->dwFieldSID = 0;
		}

		//	Note : LandMan 생성후 초기화.
		//
		GLLandMan *pNewLandMan = /*new GLLandMan;*/NEW_GLLANDMAN();
		pNewLandMan->SetMapID ( pMapNode->sNativeID, pMapNode->bPeaceZone!=FALSE, pMapNode->bPKZone==TRUE );

		//pNewLandMan->SetEmulator ( m_bEmulator );
		BOOL bOk = pNewLandMan->LoadFile ( pMapNode->strFile.c_str() );
		if ( !bOk )
		{
			SAFE_DELETE ( pNewLandMan );

			CString strTemp = pMapNode->strFile.c_str();
			strTemp += " : GLLandMan::LoadFile() Load Faile.";
			MessageBox ( NULL, strTemp, "ERROR", MB_OK );
			continue;
		}
		if( pMapNode->bInstantMap )
		{
			m_vecInstantMapSrcLandMan.push_back( pNewLandMan );
			continue;
		}

		hr = InsertMap ( pNewLandMan );
		if ( SUCCEEDED(hr) )
		{
			CONSOLEMSG_WRITE ( "[Add Map] %s  / [%d/%d]", pMapNode->strFile.c_str(), pMapNode->sNativeID.wMainID, pMapNode->sNativeID.wSubID );
		}
	}


	m_cPartyFieldMan.Create ( m_dwMaxClient );

	//	Note : 케릭터 초기 시작 맵 과 초기 시작 Gate가 정상적으로 존제하는지 점검.
	//
	for ( WORD i=0; i<GLCONST_CHAR::wSCHOOLNUM; ++i )
	{
		SNATIVEID nidSTARTMAP = GLCONST_CHAR::nidSTARTMAP[i];
		DWORD dwSTARTGATE = GLCONST_CHAR::dwSTARTGATE[i];

		FIELDMAP_ITER iter = m_MapList.find ( nidSTARTMAP.dwID );
		if ( iter==m_MapList.end() )												continue;

		const SMAPNODE *pMapNode = &(*iter).second;
		//	Note : 지정된 서버군에 포함되지 않는 map 은 제외 시킴.
		if ( dwFieldSID!=FIELDSERVER_MAX && pMapNode->dwFieldSID!=dwFieldSID )		continue;

		GLLandMan* pLandMan = GetByMapID ( nidSTARTMAP );
		if ( !pLandMan )
		{
			//std::strstream strStream;
			//strStream << "Charactor's Start Map setting Error." << std::endl;
			//strStream << "'mapslist.ini' [" << nidSTARTMAP.wMainID << "," << nidSTARTMAP.wSubID << "]";
			//strStream << " MapID ( ID:[M,S] ) MapID Not Found." << std::ends;

			TCHAR szTemp[128] = {0};
			_snprintf_s( szTemp, 128, "Charactor's Start Map setting Error.\n"
									"'mapslist.ini' [%u],[%u] MapID ( ID:[M,S] ) MapID Not Found.",
									nidSTARTMAP.wMainID,
									nidSTARTMAP.wSubID );

			MessageBox ( NULL, szTemp, "ERROR", MB_OK|MB_ICONEXCLAMATION );

			//strStream.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			continue;
		}

		DxLandGateMan* pGateMan = &pLandMan->GetLandGateMan ();
		PDXLANDGATE pGate = pGateMan->FindLandGate ( dwSTARTGATE );
		if ( !pGate )
		{
			//std::strstream strStream;
			//strStream << "Charactor Start Map GATE Not Found." << std::endl;
			//strStream << pLandMan->GetFileName() << " Map "  << dwSTARTGATE;
			//strStream << "GATE ID Must Check." << std::ends;

			TCHAR szTemp[128] = {0};
			_snprintf_s( szTemp, 128, "Charactor Start Map GATE Not Found.\n"
									"%s Map %u GATE ID Must Check.",
									pLandMan->GetFileName(),
									dwSTARTGATE );

			MessageBox ( NULL, szTemp, "ERROR", MB_OK );

			//strStream.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			continue;
		}
	}

	if ( m_pDBMan )
	{
		//	Note : 클럽의 정보를 db에서 읽어온다.
		//
		m_cClubMan.LoadFromDB ( m_pDBMan, true );

		//	Note : 선도 클럽 지역 설정.
		//
		VECGUID_DB vecGuidDb;
		m_pDBMan->GetClubRegion ( vecGuidDb );
		GLGuidanceFieldMan::GetInstance().SetState ( vecGuidDb );
	}
	
	SetMapState();
	GLGuidanceFieldMan::GetInstance().SetMapState ();
	GLClubDeathMatchFieldMan::GetInstance().SetMapState ();

	return S_OK;
}

HRESULT GLGaeaServer::Create4Level ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	CleanUp ();

	m_pd3dDevice = pd3dDevice;

	OneTimeSceneInit ();

	//	Note : LandMan 생성후 초기화.
	//
	GLLandMan *pNewLandMan = /*new GLLandMan;*/NEW_GLLANDMAN();
	pNewLandMan->SetD3dDevice ( m_pd3dDevice );
	pNewLandMan->SetMapID ( SNATIVEID(0,0), false, true );
	pNewLandMan->SetEmulator ( true );

	//	Note : 생성된 LandMan 등록.
	//
	InsertMap ( pNewLandMan );

	return S_OK;
}

void GLGaeaServer::SetMapState()
{
	size_t tSize = m_vecLandMan.size();
	for( size_t i=0; i<tSize; ++i )
	{
		PGLLANDMAN pLand = m_vecLandMan[i];

		pLand->m_bGuidBattleMap = false;
		pLand->m_bGuidBattleMapHall = false;
		pLand->m_bClubDeathMatchMap = false;
		pLand->m_bClubDeathMatchMapHall = false;

		SMAPNODE *pMAPNODE = FindMapNode ( pLand->GetMapID() );
		if ( pMAPNODE && pMAPNODE->bCommission )
		{
			pLand->m_fCommissionRate = GLCONST_CHAR::fDEFAULT_COMMISSION;
		}
		else
		{
			pLand->m_fCommissionRate = 0.0f;
		}

	}
}


HRESULT GLGaeaServer::InsertMap ( GLLandMan* pNewLandMan )
{
	const SNATIVEID &sMapID = pNewLandMan->GetMapID ();

	GASSERT(sMapID.wMainID<MAXLANDMID);
	GASSERT(sMapID.wSubID<MAXLANDSID);

//	m_LandManList.ADDTAIL ( pNewLandMan );
	m_vecLandMan.push_back( pNewLandMan );
	m_pLandMan[sMapID.wMainID][sMapID.wSubID] = pNewLandMan;

	return S_OK;
}

HRESULT GLGaeaServer::CleanUp ()
{
	m_pMsgServer = NULL;

	ClearDropObj ();

	/*GLLANDMANNODE* pLandManNode = m_LandManList.m_pHead;
	for ( ; pLandManNode; pLandManNode = pLandManNode->pNext )
	{
	SAFE_DELETE(pLandManNode->Data);
	}

	m_LandManList.DELALL();*/

	size_t i, size = m_vecLandMan.size();
	for( i = 0; i < size; i++ )
	{
		RELEASE_GLLANDMAN( m_vecLandMan[i] );
//		SAFE_DELETE( m_vecLandMan[i] );
	}
	m_vecLandMan.clear();

	size = m_vecInstantMapSrcLandMan.size();
	for( i = 0; i < size; i++ )
	{
		SAFE_DELETE( m_vecInstantMapSrcLandMan[i] );
	}
	m_vecInstantMapSrcLandMan.clear();

	m_pDBMan = NULL;

	m_FreePETGIDs.RemoveAll ();

	m_FreeSummonGIDs.RemoveAll ();

	return S_OK;
}

PGLCHAR GLGaeaServer::CreatePC ( PCHARDATA2 pCharData, DWORD _dwClientID, DWORD _dwGaeaID, BOOL bNEW,
								SNATIVEID *_pStartMap, DWORD _dwStartGate, D3DXVECTOR3 _vPos,
								EMGAME_JOINTYPE emJOINTYPE,
								DWORD dwThaiCCafeClass, __time64_t loginTime, INT nMyCCafeClass )
{
	HRESULT hr = S_OK;
	if ( !pCharData )
	{
		CDebugSet::ToLogFile ( "ERR : pCharData가 무효." );
		return NULL;
	}

	if ( _dwGaeaID>=m_dwMaxClient )
	{
		CDebugSet::ToLogFile ( "ERR : _dwGaeaID>=m_dwMaxClient  %d>%d.", _dwGaeaID, m_dwMaxClient );
		return NULL;
	}

	BOOL bDB(FALSE);
	PGLCHAR pPChar = NULL;
	GLLandMan* pLandMan = NULL;

	DxLandGateMan* pGateMan = NULL;
	PDXLANDGATE pGate = NULL;
	D3DXVECTOR3 vStartPos(0,0,0);

	CLIENTMAP_ITER client_iter;
	GLCHAR_MAP_ITER name_iter;

	//	Note : user id가 지금 db에 저장중인지 점검. ( logout 했지만 아직 db에 저장이 안된 상태로 있을때. )
	//
	bDB = FindSaveDBUserID ( pCharData->GetUserID() );
	if ( bDB )
	{
		CDebugSet::ToLogFile ( "ERR : logout 했지만 아직 db에 저장이 안된 상태로 있음." );
		goto _ERROR;
	}

	if ( _dwClientID >= GLGaeaServer::GetInstance().GetMaxClient()*2 )
	{
		CDebugSet::ToLogFile ( "ERR : max client id overflow! id = %d", _dwClientID );
		goto _ERROR;
	}

	if ( _dwGaeaID >= GLGaeaServer::GetInstance().GetMaxClient() )
	{
		CDebugSet::ToLogFile ( "ERR : max gaea id overflow! id = %d", _dwGaeaID );
		goto _ERROR;
	}

	//	Note : 가이아 ID의 할당을 Agent 서버에서 할당하게 됨에 따라
	//		잘못되어 가이아 ID가 반환되지 않은 상태에서 다시 사용될 가능성이 있음.
	if ( m_PCArray[_dwGaeaID] )
	{
		CDebugSet::ToLogFile ( "ERR : m_PCArray[_dwGaeaID] 이미 점거된 gaeaid" );
		goto _ERROR;
	}

	//	Note : 같은 캐릭터가 이미 접속되어 있는지 검사합니다.
	name_iter = m_PCNameMap.find(pCharData->m_szName);
	if ( name_iter != m_PCNameMap.end() )
	{
		CDebugSet::ToLogFile ( "ERR : char name 이미 진입되어 있음" );
		goto _ERROR;
	}

	client_iter = m_PCClientIDMAP.find(_dwClientID);
	if ( client_iter != m_PCClientIDMAP.end() )
	{
		//	종전 접속자를 DropOut 시킵니다.
		DWORD dwGaeaID = (*client_iter).second;
		//	Note : 케릭터가 지워진다.
		//
		ReserveDropOutPC(dwGaeaID);

		CDebugSet::ToLogFile ( "ERR : client id 이미 진입되어 있음" );
		goto _ERROR;
	}


	//	Note : 캐릭터 초기화.
	//
	pPChar = NEW_CHAR();

	pPChar->SetGLGaeaServer( this );

	//	Note : 특정한 MapID에서 생성하고자 할 경우.
	//
	if ( _pStartMap )
	{
		pLandMan = GetByMapID ( *_pStartMap );
		if ( pLandMan )
		{
			pGateMan = &pLandMan->GetLandGateMan ();

			if ( _dwStartGate!=UINT_MAX )
			{
				pGate = pGateMan->FindLandGate ( DWORD(_dwStartGate) );

				if ( pGate )	vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
				else
				{
					pGate = pGateMan->FindLandGate ( DWORD(0) );
					if( pGate == NULL )
					{
						GetConsoleMsg()->Write( "ERROR: pGate = NULL, UserID %s UserLv %d Money %d", pCharData->m_szName, pCharData->m_wLevel, pCharData->m_lnMoney );
						goto _ERROR;

					}
					_dwStartGate = pGate->GetGateID ();
					if ( pGate )	vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
				}
			}
			else
			{
				//	특정 위치에서 생성코자 할 경우.
				vStartPos = _vPos;
			}
		}
	}
	else
	{
		pLandMan = GetByMapID ( pCharData->m_sStartMapID );
		if ( pLandMan )
		{
			pGateMan = &pLandMan->GetLandGateMan ();
			pGate = pGateMan->FindLandGate ( DWORD(pCharData->m_dwStartGate) );

			if ( pGate )	vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
			else
			{
				pGate = pGateMan->FindLandGate ( DWORD(0) );
				if ( pGate )	vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
			}
		}
	}

	if ( !pLandMan || ((_dwStartGate!=UINT_MAX)&&!pGate) )
	{
		SNATIVEID nidSTARTMAP = GLCONST_CHAR::nidSTARTMAP[pCharData->m_wSchool];
		DWORD dwSTARTGATE = GLCONST_CHAR::dwSTARTGATE[pCharData->m_wSchool];

		pLandMan = GetByMapID ( nidSTARTMAP );
		if ( !pLandMan )
		{
			DEBUGMSG_WRITE ( "[오류] #1 GetByMapID() 캐릭터가 생성될 LAND(혹은 GATE)가 잘못지정되었습니다." );
			goto _ERROR;
		}

		pGateMan = &pLandMan->GetLandGateMan ();
		if ( !pGateMan )
		{
			DEBUGMSG_WRITE ( "[오류] #2 GetLandGateMan() 캐릭터가 생성될 LAND(혹은 GATE)가 잘못지정되었습니다." );
			goto _ERROR;

		}
		pGate = pGateMan->FindLandGate ( dwSTARTGATE );
		if ( !pGate )
		{
			DEBUGMSG_WRITE ( "[오류] #3 FindLandGate() 캐릭터가 생성될 LAND(혹은 GATE)가 잘못지정되었습니다." );
			vStartPos = D3DXVECTOR3(0,0,0);
		}
		else
		{
			vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
		}
	}

	//	Note : 캐릭터 등록.
	//
	pCharData->m_dwThaiCCafeClass = dwThaiCCafeClass;
	pCharData->m_nMyCCafeClass    = nMyCCafeClass;
	pCharData->m_sEventTime.Init();
	pCharData->m_sEventTime.loginTime = loginTime;
	hr = pPChar->CreateChar ( pLandMan, vStartPos, pCharData, m_pd3dDevice, bNEW );
	if ( FAILED(hr) )
	{
		DEBUGMSG_WRITE ( "[오류] pPChar->CreateChar () 호출중 오류로 인하여 캐릭터 생성에 실패. [%s]", pCharData->m_szName );
		goto _ERROR;
	}
	
	pPChar->m_dwClientID = _dwClientID;
	pPChar->m_dwGaeaID = _dwGaeaID;
	pPChar->SetPartyID ( m_cPartyFieldMan.GetPartyID ( _dwGaeaID ) );
	
	BOOL bOk = DropPC ( pLandMan->GetMapID(), vStartPos, pPChar );
	if ( !bOk )
	{
		DEBUGMSG_WRITE ( "[오류] DropPC () 호출중 오류로 인하여 캐릭터 생성에 실패. [%s]", pCharData->m_szName );
		goto _ERROR;
	}


#if defined(TW_PARAM) || defined(HK_PARAM) || defined(_RELEASED)

	bool bDifferTempValue = FALSE;
	/*if( pPChar->m_wLevel != pPChar->m_wTempLevel )
	{
		if( pPChar->m_wTempLevel != 0 )
		{
			HACKINGLOG_WRITE( "#####Different Login Level##### [%s][%s] Level %d TempLevel %d Level Gap %d", 
				pPChar->m_szUID, pPChar->m_szName, pPChar->m_wLevel, pPChar->m_wTempLevel, pPChar->m_wLevel - pPChar->m_wTempLevel );	

		}
		bDifferTempValue = TRUE;
		pPChar->m_wLevel = pPChar->m_wTempLevel;
	}*/

	if( pPChar->m_lnMoney != pPChar->m_lnTempMoney )
	{
#ifdef _RELEASED
		if( pPChar->m_lnTempMoney != 0 )
#endif
		{
			HACKINGLOG_WRITE( "Different Login Money!!, Account[%s], ID[%s], Money %I64d, TempMoney %I64d, Money Gap %I64d", 
						  pPChar->m_szUID, pPChar->m_szName, pPChar->m_lnMoney, pPChar->m_lnTempMoney, pPChar->m_lnMoney - pPChar->m_lnTempMoney );							
	
			bDifferTempValue  = TRUE;			

			// 무조건 TempMoney를 Money 값에 넣지 않는다.
			LONGLONG lnGap = pPChar->m_lnMoney - pPChar->m_lnTempMoney;
			if( lnGap >= 10000 || lnGap <= -10000 )
			{
				if( pPChar->m_lnMoney > pPChar->m_lnTempMoney )
				{
					pPChar->m_lnMoney = pPChar->m_lnTempMoney;			
				}else{
					pPChar->m_lnTempMoney = pPChar->m_lnMoney;			
				}
			}else{
				pPChar->m_lnMoney = pPChar->m_lnTempMoney;			
			}
		}
#ifdef _RELEASED
		else{			
			pPChar->m_lnTempMoney = pPChar->m_lnMoney;
		}
#endif
		
	}

	if( pPChar->m_lnStorageMoney != pPChar->m_lnTempStorageMoney )
	{
#ifdef _RELEASED
		if( pPChar->m_lnTempStorageMoney != 0 )
#endif
		{
			HACKINGLOG_WRITE( "Different Login Storage Money!!, Account[%s], ID[%s], Storage Money %I64d, TempStorage Money %I64d, Storage Money Gap %I64d", 
					pPChar->m_szUID, pPChar->m_szName, pPChar->m_lnStorageMoney, pPChar->m_lnTempStorageMoney, 
					pPChar->m_lnStorageMoney - pPChar->m_lnTempStorageMoney );				

			bDifferTempValue		 = TRUE;
			
			LONGLONG lnGap = pPChar->m_lnStorageMoney - pPChar->m_lnTempStorageMoney;
			if( lnGap > 10000 || lnGap < -10000 )
			{
				if( pPChar->m_lnStorageMoney > pPChar->m_lnTempStorageMoney )
				{
					pPChar->m_lnStorageMoney = pPChar->m_lnTempStorageMoney;			
				}else{
					pPChar->m_lnTempStorageMoney = pPChar->m_lnStorageMoney;			
				}
			}else{
				pPChar->m_lnStorageMoney = pPChar->m_lnTempStorageMoney;			
			}	
		}
#ifdef _RELEASED
		else{
			pPChar->m_lnTempStorageMoney = pPChar->m_lnStorageMoney;					
		}
#endif
		
	}

	if( bDifferTempValue )
	{
		ReserveDropOutPC( pPChar->m_dwGaeaID );

		//	접속 시도자에게  메시지를 보냅니다.
		GLMSG::SNETLOBBY_CHARJOIN_FB NetMsgFB;
		NetMsgFB.emCharJoinFB = EMCJOIN_FB_ERROR;
		SENDTOAGENT ( _dwClientID, &NetMsgFB );
		return NULL;
	}

#else
	pPChar->m_wTempLevel		  = pPChar->m_wLevel;
	pPChar->m_lnTempMoney		  = pPChar->m_lnMoney;
	pPChar->m_lnTempStorageMoney  = pPChar->m_lnStorageMoney;
#endif


#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	if ( pPChar->m_bTracingUser && pPChar->m_pLandMan )
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = pPChar->GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, pPChar->m_szUID );

		int nPosX(0);
		int nPosY(0);
		pPChar->m_pLandMan->GetMapAxisInfo().Convert2MapPos ( pPChar->m_vPos.x, pPChar->m_vPos.z, nPosX, nPosY );

		CString strTemp;
		strTemp.Format( "FieldServer in!!, [%s][%s], MAP:mid[%d]sid[%d], StartPos:[%d][%d], Money:[%I64d]",
			pPChar->m_szUID, pPChar->m_szName, pPChar->m_sMapID.wMainID, pPChar->m_sMapID.wSubID, nPosX, nPosY, pPChar->m_lnMoney );

		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		SENDTOAGENT( pPChar->m_dwClientID, &TracingMsg );
	}
#endif

	return pPChar;	//	케릭터 생성 성공.

_ERROR:
	if ( pPChar )
	{
		RELEASE_CHAR(pPChar);
	}

	//	접속 시도자에게  메시지를 보냅니다.
	GLMSG::SNETLOBBY_CHARJOIN_FB NetMsgFB;
	NetMsgFB.emCharJoinFB = EMCJOIN_FB_ERROR;
	SENDTOAGENT ( _dwClientID, &NetMsgFB );

	return NULL;	//	케릭터 생성 실패.
}



PGLPETFIELD GLGaeaServer::CreatePET ( PGLPET pPetData, DWORD dwOwner, DWORD dwPetID, bool bValid )
{
	// DB에서 팻정보를 가져와야 한다.

	GLMSG::SNETPET_REQ_USEPETCARD_FB NetMsg;

	if ( !pPetData ) return NULL;

	// 요청 케릭터가 유효성 체크
	PGLCHAR pOwner = GetChar ( dwOwner );
	if ( !pOwner ) return NULL;

	// 팻Full Check
	if ( pPetData->IsNotEnoughFull () )
	{
		NetMsg.emFB = EMPET_USECARD_FB_NOTENOUGHFULL;
		SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );
		return NULL;
	}

	GLLandMan* pLandMan = GetByMapID ( pOwner->m_sMapID );
	if ( !pLandMan ) 
	{
		SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );
		return NULL;
	}

	PGLPETFIELD pPet(NULL);

	// 다른 필드서버로 넘어가지 않았다면 GLPetField 가 존재한다.
	// 맵정보와 네비게이션만 변경해준다.
	pPet = GetPET ( pOwner->m_dwPetGUID );
	if ( pPet && m_bEmulator )
	{
		// 메모리에 존재하는 팻 제거
		DropOutPET ( pPet->m_dwGUID, true, false );
		pPet = NULL;
	}

	if ( pPet )
	{
		HRESULT hr = pPet->SetPosition ( pLandMan );
		if ( FAILED ( hr ) )
		{
			// 메모리에 존재하는 팻 제거
			DropOutPET ( pPet->m_dwGUID, true, false );
			SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );
			return NULL;
		}
	}
	// 신규로 생성해준다.
	else
	{
		// 팻생성
		pPet = NEW_PET ();
		HRESULT hr = pPet->Create ( pLandMan, pOwner, pPetData );
		if ( FAILED ( hr ) )
		{
			// 생성실패 처리
			RELEASE_PET ( pPet );
			SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );
			return NULL;
		}

		DWORD dwGUID = -1;
		if ( !m_FreePETGIDs.GetHead ( dwGUID ) )		return NULL;
		m_FreePETGIDs.DelHead ();
		pPet->m_dwGUID = dwGUID;

		// 주인ID 할당
		pOwner->m_dwPetGUID = pPet->m_dwGUID;
		pOwner->m_dwPetID   = dwPetID;

		// 팻의 고유ID(DB저장용) 설정
		pPet->SetPetID ( dwPetID );
	}

	if ( !DropPET ( pPet, pOwner->m_sMapID ) )
	{
		// 생성실패 처리
		m_FreePETGIDs.AddTail ( pPet->m_dwGUID );
		RELEASE_PET ( pPet );
		SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );
		return NULL;
	}

	// 주인 포인터 할당
	pPet->m_pOwner = pOwner;

	// 활성화 유무 설정
	if ( bValid ) pPet->SetValid ();
	else		  pPet->ReSetValid ();

	// 활성상태일때만
	if ( pPet->IsValid () )
	{
		NetMsg.emFB				  = EMPET_USECARD_FB_OK;
		NetMsg.m_emTYPE			  = pPet->m_emTYPE;
		NetMsg.m_dwGUID			  = pPet->m_dwGUID;
		NetMsg.m_sPetID			  = pPet->m_sPetID;
		NetMsg.m_sActiveSkillID	  = pPet->m_sActiveSkillID;
		NetMsg.m_dwOwner		  = pPet->m_dwOwner;
		NetMsg.m_wStyle			  = pPet->m_wStyle;
		NetMsg.m_wColor			  = pPet->m_wColor;
		NetMsg.m_fWalkSpeed		  = pPet->m_fWalkSpeed;
		NetMsg.m_fRunSpeed		  = pPet->m_fRunSpeed;
		NetMsg.m_nFull			  = pPet->m_nFull;
		NetMsg.m_sMapID			  = pPet->m_sMapID;
		NetMsg.m_dwCellID		  = pPet->m_dwCellID;
		NetMsg.m_wSkillNum		  = static_cast<WORD> (pPet->m_ExpSkills.size());
		NetMsg.m_vPos			  = pPet->m_vPos;
		NetMsg.m_vDir			  = pPet->m_vDir;
		NetMsg.m_sPetSkinPackData = pPet->m_sPetSkinPackData;
		StringCchCopy ( NetMsg.m_szName, PETNAMESIZE+1, pPetData->m_szName );

		NetMsg.m_dwPetID		= dwPetID;

		PETSKILL_MAP_ITER iter = pPet->m_ExpSkills.begin();
		PETSKILL_MAP_ITER iter_end = pPet->m_ExpSkills.end();
		WORD i(0);
		for ( ;iter != iter_end; ++iter )
		{
			NetMsg.m_Skills[i++] = (*iter).second;
		}

		const CTime cTIME_CUR = CTime::GetCurrentTime();

		for ( WORD i = 0; i < ACCETYPESIZE; ++i )
		{
			SITEMCUSTOM sPetItem = pPet->m_PutOnItems[i];
			if ( sPetItem.sNativeID == NATIVEID_NULL () ) continue;

			SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sPetItem.sNativeID );
			if ( !pITEM )	continue;

			// 시한부 아이템
			if ( pITEM->IsTIMELMT() )
			{
				CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
				CTime cTIME_LMT(sPetItem.tBORNTIME);
				cTIME_LMT += cSPAN;

				if ( cTIME_CUR > cTIME_LMT )
				{
					//	시간 제한으로 아이템 삭제 로그 남김.
					GLITEMLMT::GetInstance().ReqItemRoute ( sPetItem, ID_CHAR, pOwner->m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, 0 );

					//	아이템 삭제.
					pPet->m_PutOnItems[i] = SITEMCUSTOM ();

					//	시간 제한으로 아이템 삭제 알림.
					GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
					NetMsgInvenDelTimeLmt.nidITEM = sPetItem.sNativeID;
					GLGaeaServer::GetInstance().SENDTOCLIENT(pOwner->m_dwClientID,&NetMsgInvenDelTimeLmt);
				}
			}
			NetMsg.m_PutOnItems[i] = pPet->m_PutOnItems[i];
		}

		SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );

		// 주변에 알림
		GLMSG::SNETPET_CREATE_ANYPET NetMsgBrd;
		NetMsgBrd.Data = ((GLMSG::SNETPET_DROP_PET*)pPet->ReqNetMsg_Drop ())->Data;
		pPet->m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBrd );
	}
	
	return pPet;	
}

BOOL GLGaeaServer::DropPET ( PGLPETFIELD pPet, SNATIVEID sMapID )
{
	if ( !pPet ) return FALSE;

	GLLandMan* pLandMan = GetByMapID ( sMapID );
	if ( !pLandMan ) return FALSE;

	// 글로벌에 등록
	m_PETArray[pPet->m_dwGUID] = pPet;

	// 랜드에 등록
	pPet->m_pLandMan = pLandMan;
	pPet->m_pLandNode = pLandMan->m_GlobPETList.ADDHEAD ( pPet );
	pLandMan->RegistPet ( pPet );
	
	return TRUE;
}

// 이 함수는 주인이 맵을 이동하는 모든 행위에 대해서
// 호출된다. (주인이 게임을 종료하는 경우에도 호출됨)
// 따라서 소환여부를 고려해야 한다.
BOOL GLGaeaServer::DropOutPET ( DWORD dwGUID, bool bLeaveFieldServer, bool bMoveMap )
{
	if ( dwGUID>=m_dwMaxClient ) 
	{
		CDebugSet::ToLogFile ( "ERROR : dwGUID>=m_dwMaxClient PetGUID : %d dwMaxClient : %d", dwGUID, m_dwMaxClient );
		return FALSE;
	}
	if ( m_PETArray[dwGUID] == NULL ) 
	{
		CDebugSet::ToLogFile ( "ERROR : m_PETArray[dwGUID] == NULL" );
		return FALSE;
	}

	PGLPETFIELD pPet = m_PETArray[dwGUID];

	// 활동 여부
	bool bValid = pPet->IsValid ();

	// 소환이 안된상태로 서버를 떠나지 않는다면
	if ( !bValid && !bLeaveFieldServer )
	{
		CDebugSet::ToLogFile ( "ERROR : !bValid && !bLeaveFieldServer bValid %d bLeaveFieldServer %d", bValid, bLeaveFieldServer );
		return FALSE;
	}

	DWORD dwPetGUID = pPet->m_dwGUID;
	DWORD dwOwnerID = pPet->m_dwOwner;

	// 활동중이면
	if ( bValid )
	{
		//	Note : Land 리스트에서 제거.
		GLLandMan* pLandMan = pPet->m_pLandMan;
		if ( pLandMan )
		{
			pLandMan->RemovePet ( pPet );
			pPet->m_pLandMan = NULL;
		}

		pPet->ReSetValid ();
		pPet->ReSetAllSTATE ();
		pPet->ReSetSkillDelay ();
	}

	// 클라이언트 팻 사라지게 메시지 발송 (PC가 게임을 종료하면 pOwner 없을 수 있다)
	PGLCHAR pOwner = GetChar ( dwOwnerID );
	if ( pOwner && bValid )
	{
		// 스킬의 보조능력치 제거
		pOwner->m_sPETSKILLFACT.RESET ();
		pOwner->m_bProtectPutOnItem = false;

		GLMSG::SNETPET_REQ_UNUSEPETCARD_FB NetMsgFB;
		NetMsgFB.dwGUID	= dwPetGUID;
		NetMsgFB.bMoveMap = bMoveMap;
		SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsgFB );

		// 주변에 알림
		GLMSG::SNETPET_REQ_SKILLCHANGE_BRD NetMsgBRD;
		NetMsgBRD.dwGUID   = pPet->m_dwGUID;
		NetMsgBRD.dwTarID  = pOwner->m_dwGaeaID;
		NetMsgBRD.sSkillID = NATIVEID_NULL ();
		pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* ) &NetMsgBRD );
	}

	// 현재 필드서버를 떠나거나 게임을 완전 종료하면
	if ( bLeaveFieldServer )
	{
		// 팻의 데이터 DB저장
		CSetPetFull* pSaveDB = new CSetPetFull( pOwner->m_dwCharID, pPet->GetPetID (), pPet->m_nFull );
		if ( m_pDBMan ) m_pDBMan->AddJob ( pSaveDB );

		CSetPetInven* pSaveInven = new CSetPetInven ( pOwner->m_dwCharID, pPet->GetPetID(), pPet );
		if ( m_pDBMan ) m_pDBMan->AddJob ( pSaveInven );

		if ( pPet->m_sActiveSkillID != NATIVEID_NULL() )
		{
			if( pPet->m_sActiveSkillID.wMainID != 26 )
			{
				CDebugSet::ToLogFile( "ERR : GLGaeaServer::DropOutPET, MID = %d, SID = %d",  
													pPet->m_sActiveSkillID.wMainID,
													pPet->m_sActiveSkillID.wSubID );
			}

			PETSKILL sPetSkill( pPet->m_sActiveSkillID, 0 );
			CSetPetSkill* pSaveSkill = new CSetPetSkill ( pOwner->m_dwCharID, pPet->GetPetID (), sPetSkill, true );
			if ( m_pDBMan ) m_pDBMan->AddJob ( pSaveSkill );
		}

		// 글로벌 리스트에서 제거, GUID 반환
		RELEASE_PET ( pPet );
		m_PETArray[dwGUID] = NULL;
		m_FreePETGIDs.AddTail ( dwGUID );
	}

	return TRUE;
}

HRESULT GLGaeaServer::ClearDropObj ()
{
	if ( m_PCArray )
	{
		//	Note : 플래이어 드롭 아웃 처리.
		//
		for ( DWORD i=0; i<m_dwMaxClient; i++ )
		{
			if ( m_PCArray[i] )		ReserveDropOutPC ( i );
		}

		ClearReservedDropOutPC ();
	}

	//	Note : 지형 정보 제거.
	//
	/*GLLANDMANNODE* pLandManNode = m_LandManList.m_pHead;
	for ( ; pLandManNode; pLandManNode = pLandManNode->pNext )
	{
	pLandManNode->Data->ClearDropObj ();
	}*/

	size_t i, size = m_vecLandMan.size();
	for( i = 0; i < size; i++ )
	{
		m_vecLandMan[i]->ClearDropObj();
	}

	size = m_vecInstantMapSrcLandMan.size();
	for( i = 0; i < size; i++ )
	{
		m_vecInstantMapSrcLandMan[i]->ClearDropObj();
	}

	return S_OK;
}

BOOL GLGaeaServer::ValidCheckTarget ( GLLandMan* pLandMan, STARGETID &sTargetID )
{
	GASSERT(pLandMan);

	if ( sTargetID.dwID == EMTARGET_NULL )		return FALSE;

	if ( sTargetID.emCrow == CROW_PC )
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( sTargetID.dwID );
		if ( pChar && pChar->IsValidBody() )
		{
			if ( pChar->m_pLandMan != pLandMan )		return FALSE;
			return TRUE;
		}
	}
	else if ( sTargetID.emCrow == CROW_NPC || sTargetID.emCrow == CROW_MOB )
	{
		PGLCROW pCrow = pLandMan->GetCrow ( sTargetID.dwID );
		if ( pCrow && pCrow->IsValidBody() )
		{
			if ( pCrow->m_pLandMan != pLandMan )		return FALSE;
			return TRUE;
		}
	}
	else if ( sTargetID.emCrow == CROW_MATERIAL )
	{
		PGLMATERIAL pMaterial = pLandMan->GetMaterial ( sTargetID.dwID );
		if ( pMaterial && pMaterial->IsValidBody() )
		{
			if ( pMaterial->m_pLandMan != pLandMan )		return FALSE;
			return TRUE;
		}
	}
	else if( sTargetID.emCrow == CROW_PET )	// PetData
	{
	}else if ( sTargetID.emCrow == CROW_SUMMON )
	{
		PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon( sTargetID.dwID );		
		if ( pSummon && pSummon->IsValidBody() )
		{
			if ( pSummon->m_pLandMan != pLandMan )		return FALSE;
			return TRUE;
		}
	}
	/*else
	{
		GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
	}
	*/

	sTargetID.dwID = EMTARGET_NULL;
	return FALSE;
}

GLACTOR* GLGaeaServer::GetTarget ( const GLLandMan* pLandMan, const STARGETID &sTargetID )
{
	GASSERT(pLandMan);
	if ( !pLandMan )							return NULL;
	if ( sTargetID.dwID == EMTARGET_NULL )		return NULL;

	switch ( sTargetID.emCrow )
	{
	case CROW_PC:
		{
			// 같은 맵에 있는 PC만 검색
			PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( sTargetID.dwID );
			if ( pChar && pChar->m_sMapID == pLandMan->GetMapID() ) return pChar;
		}
		break;

	case CROW_MOB:
	case CROW_NPC:
		{
			PGLCROW pCrow = pLandMan->GetCrow ( sTargetID.dwID );
			if ( pCrow )							return pCrow;
		}
		break;
	case CROW_MATERIAL:
		{
			PGLMATERIAL pMaterial = pLandMan->GetMaterial ( sTargetID.dwID );
			if ( pMaterial )						return pMaterial;
		}
		break;
	case CROW_SUMMON:
		{
			PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon ( sTargetID.dwID );
			if ( pSummon )							return pSummon;
		}
		break;

	//case CROW_PET:	break;	// PetData

	//default:	GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
	};

	return NULL;
}

const D3DXVECTOR3& GLGaeaServer::GetTargetPos ( const GLLandMan* pLandMan, const STARGETID &sTargetID )
{
	GASSERT(pLandMan);

	//	Note : 타겟의 위치 정보를 가져옴.
	if ( sTargetID.emCrow == CROW_PC )
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( sTargetID.dwID );
		if ( pChar ) return pChar->GetPosition();
	}
	else if ( sTargetID.emCrow == CROW_NPC || sTargetID.emCrow == CROW_MOB ) 
	{
		PGLCROW pCrow = pLandMan->GetCrow ( sTargetID.dwID );
		if ( pCrow ) return pCrow->GetPosition();
	}
	else if ( sTargetID.emCrow == CROW_MATERIAL )
	{
		PGLMATERIAL pMaterial = pLandMan->GetMaterial ( sTargetID.dwID );
		if ( pMaterial ) return pMaterial->GetPosition();
	}
	else if( sTargetID.emCrow == CROW_PET )	// PetData
	{
	}
	else if ( sTargetID.emCrow == CROW_SUMMON )
	{
		PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon ( sTargetID.dwID );
		if ( pSummon ) return pSummon->GetPosition();
	}
	/*else
	{
		GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
	}
	*/

	static D3DXVECTOR3 vERROR(FLT_MAX,FLT_MAX,FLT_MAX);
	return vERROR;
}

WORD GLGaeaServer::GetTargetBodyRadius ( GLLandMan* pLandMan, STARGETID &sTargetID )
{
	GASSERT(pLandMan);

	//	Note : 타겟의 위치 정보를 가져옴.
	if ( sTargetID.emCrow == CROW_PC )
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( sTargetID.dwID );
		if ( pChar ) return pChar->GETBODYRADIUS();
	}
	else if ( sTargetID.emCrow == CROW_NPC || sTargetID.emCrow == CROW_MOB )
	{
		PGLCROW pCrow = pLandMan->GetCrow ( sTargetID.dwID );
		if ( pCrow ) return pCrow->GETBODYRADIUS();
	}
	else if ( sTargetID.emCrow == CROW_MATERIAL )
	{
		PGLMATERIAL pMaterial = pLandMan->GetMaterial ( sTargetID.dwID );
		if ( pMaterial ) return pMaterial->GetBodyRadius();
	}
	else if( sTargetID.emCrow == CROW_PET )	// PetData
	{
	}
	else if ( sTargetID.emCrow == CROW_SUMMON )
	{
		PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon ( sTargetID.dwID );
		if ( pSummon ) return pSummon->GETBODYRADIUS();
	}
	/*else
	{
		GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
	}
	*/

	return WORD(0xFFFF);
}

GLARoundSlot* GLGaeaServer::GetARoundSlot ( const STARGETID &sTargetID )
{
	if ( sTargetID.emCrow==CROW_PC )
	{
		PGLCHAR pChar = GetChar(sTargetID.dwID);
		if ( !pChar )	return NULL;

		return &pChar->GetARoundSlot ();
	}

	return NULL;
}

BOOL GLGaeaServer::DropPC ( SNATIVEID MapID, D3DXVECTOR3 vPos, PGLCHAR pPC )
{
	GLLandMan* pLandMan = GetByMapID ( MapID );
	if ( !pLandMan )	return FALSE;

	//	Note : 위치로 내비게이션 초기화.
	//
	pPC->SetNavi ( pLandMan->GetNavi(), vPos );

	//	Note : 관리 ID 부여.
	//
	pPC->m_sMapID = MapID;

	m_PCArray[pPC->m_dwGaeaID] = pPC;
	pPC->m_pGaeaNode = m_GaeaPCList.ADDHEAD ( pPC );

	pPC->m_pLandMan = pLandMan;
	pPC->m_pLandNode = pLandMan->m_GlobPCList.ADDHEAD ( pPC );

	//	Note : GLLandMan의 셀에 등록하는 작업.
	//		GLLandMan::RegistChar(pPC) 작업이 GLGaeaServer::RequestLandIn ()이 호출될 때까지 유보.
	//
	pPC->m_dwCeID = 0;
	pPC->m_pQuadNode = NULL;
	pPC->m_pCellNode = NULL;

	//	Note : PC NAME map 에 등록.
	GASSERT ( m_PCNameMap.find(pPC->GetCharData2().m_szName)==m_PCNameMap.end() );
	m_PCNameMap [ std::string(pPC->GetCharData2().m_szName) ] = pPC;

	//	Note : PC Client map 에 등록.
	GASSERT ( m_PCClientIDMAP.find(pPC->m_dwClientID)==m_PCClientIDMAP.end() );
	m_PCClientIDMAP[pPC->m_dwClientID] = pPC->m_dwGaeaID;

	//	Note : CID map 에 등록.
	GASSERT ( m_mapCHARID.find(pPC->m_dwCharID)==m_mapCHARID.end() );
	m_mapCHARID[pPC->m_dwCharID] = pPC->m_dwGaeaID;

	return TRUE;
}

BOOL GLGaeaServer::DropOutPC ( DWORD dwGaeaPcID )
{
	GASSERT ( dwGaeaPcID<m_dwMaxClient );
	
	PGLCHAR pPC = m_PCArray[dwGaeaPcID];
	if ( !pPC )		return FALSE;

	//	Note : 만약 상점이 열려 있는 상태이면 상점을 닫아준다.
	//
	if ( pPC->m_sPMarket.IsOpen() )
	{
		pPC->m_sPMarket.DoMarketClose();

		GLMSG::SNETPC_PMARKET_CLOSE_BRD	NetMsgBRD;
		NetMsgBRD.dwGaeaID = pPC->m_dwGaeaID;
		pPC->SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsgBRD );
	}

	if ( pPC->m_sCONFTING.IsCONFRONTING() )
	{
		pPC->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
	}

	pPC->ResetViewAround ();

	//	Note : Gaea 전역 리스트에서 제거.
	m_PCArray[dwGaeaPcID] = NULL;
	
	if ( pPC->m_pGaeaNode )
		m_GaeaPCList.DELNODE ( pPC->m_pGaeaNode );

	//	Note : Land 리스트에서 제거.
	GLLandMan *pLandMan = pPC->m_pLandMan;
	if ( pLandMan )		pLandMan->RemoveChar ( pPC );

	//	Note : PC NAME map 에서 제거.
	GLCHAR_MAP_ITER iterPC = m_PCNameMap.find ( std::string(pPC->GetCharData2().m_szName) );
	GASSERT ( iterPC!=m_PCNameMap.end() );
	m_PCNameMap.erase ( iterPC );

	//	Note : CLIENTID map 에서 제거.
	CLIENTMAP_ITER client_iter = m_PCClientIDMAP.find ( pPC->m_dwClientID );
	GASSERT ( client_iter!=m_PCClientIDMAP.end() );
	m_PCClientIDMAP.erase ( client_iter );

	//	Note : CID map 에서 제거.
	CLIENTMAP_ITER cid_iter = m_mapCHARID.find ( pPC->m_dwCharID );
	GASSERT ( cid_iter!=m_mapCHARID.end() );
	m_mapCHARID.erase ( cid_iter );

	if ( pPC->m_dwGuild!=CLUB_NULL )
	{
		GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub(pPC->m_dwGuild);
		if ( pCLUB )
		{
			if ( pCLUB->m_dwMasterID == pPC->m_dwCharID && pCLUB->m_bVALID_STORAGE )
			{
				pCLUB->RESET_STORAGE();
			}
		}
	}

	//	Note : 메모리 반환.
	RELEASE_CHAR(pPC);

	return TRUE;
}

BOOL GLGaeaServer::SaveCharDB ( DWORD dwGaeaID )
{
	PGLCHAR pChar = GetChar(dwGaeaID);
	if ( pChar )
	{
		pChar->SavePosition ();

		if ( m_pDBMan )
		{
			//	Note : 케릭터 저장 요청.
			SetSaveDBUserID(pChar->GetUserID());

			//	Note : 클럽 창고 저장 요청.
			if ( pChar->m_dwGuild!=CLUB_NULL )
			{
				GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( pChar->m_dwGuild );
				if ( pCLUB && pChar->m_dwCharID==pCLUB->m_dwMasterID )
				{
					pCLUB->SAVESTORAGE2DB ();
				}
			}

			//	Note : 케릭터 저장 요청.
			CDbActSaveChar *pSaveDB = new CDbActSaveChar;
			pSaveDB->SetInfo ( pChar->m_dwClientID, pChar->m_dwGaeaID, pChar );
			m_pDBMan->AddJob ( pSaveDB );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::ReserveDropOutPC ( DWORD dwGaeaID, CDbActToAgentMsg* pDbActToAgentMsg )
{
	// * 주의
	// pDbActToAgentMsg 의 Default 가 NULL 이다.
	// 여기서 NULL 체크를 해버리면 캐릭터가 정상적으로 DROP 되지 않아서
	// 서버안에 남아있게 된다.
	if (dwGaeaID == GAEAID_NULL) return FALSE;

	EnterCriticalSection(&m_CSPCLock);
	{
		PGLCHAR pChar = GetChar(dwGaeaID);
		if ( pChar )
		{
			DWORD dwUserID = pChar->GetUserID();
			m_reqSaveDBUserID.insert ( dwUserID );

			//	Note : 케릭터의 db 작업을 예약에 들어감.
			m_reqDropOutChar.push_back ( SDROPOUTINFO(dwGaeaID,dwUserID,pDbActToAgentMsg) );
		}
	}
	LeaveCriticalSection(&m_CSPCLock);

	return TRUE;
}

BOOL GLGaeaServer::ClearReservedDropOutPC ()
{
	EnterCriticalSection(&m_CSPCLock);
	{
		VPCID_ITER iter     = m_reqDropOutChar.begin();
		VPCID_ITER iter_end = m_reqDropOutChar.end();
		for ( ; iter != iter_end; iter++ )
		{
			DWORD dwGaea = (*iter).m_dwGaeaID;
			PGLCHAR pChar = GetChar(dwGaea);

			if ( pChar )
			{
				// PET
				// 맵이동시 Pet 삭제
				DropOutPET ( pChar->m_dwPetGUID, true, true );
				
				DropOutSummon ( pChar->m_dwSummonGUID, true );

				SaveVehicle( pChar->m_dwClientID, dwGaea, true );			

				// 인던일 경우 인던 밖으로 이동한다.
				if( pChar->m_pLandMan->IsInstantMap() )
				{
					GLLandMan* pInLandMan = NULL;
					DxLandGateMan* pInGateMan = NULL;
					PDXLANDGATE pInGate = NULL;
					D3DXVECTOR3 vPos(0,0,0);

					DxLandGateMan* pOutGateMan = NULL;
					PDXLANDGATE pOutGate = NULL;				
					
					pOutGateMan = &pChar->m_pLandMan->GetLandGateMan();
					if( !pOutGateMan ) goto drop_pc;

					// 무조건 첫번째 게이트로 이동한다.
					DWORD dwGateID = 0;
					DWORD dwOutGateID = 0;
					SNATIVEID sMoveMapId;

					for( dwGateID = 0; dwGateID < pOutGateMan->GetNumLandGate(); dwGateID++ )
					{
						pOutGate = pOutGateMan->FindLandGate ( dwGateID );
						if( pOutGate ) break;
					}

					// Gate를 못 찾았을경우
					if( !pOutGate )
					{
						dwOutGateID			  = GLCONST_CHAR::dwSTARTGATE[pChar->m_wSchool];
						SNATIVEID sStartMapID = GLCONST_CHAR::nidSTARTMAP[pChar->m_wSchool];
						pInLandMan = GetByMapID ( sStartMapID );
						if ( !pInLandMan )		goto drop_pc;

					}else{
						pInLandMan = GetByMapID ( pOutGate->GetToMapID() );
						if ( !pInLandMan )		goto drop_pc;

						dwOutGateID = pOutGate->GetToGateID();			
					}

					//	Note : 목표 게이트 가져오기.					//

					pInGateMan = &pInLandMan->GetLandGateMan ();
					if ( !pInGateMan )		goto drop_pc;
					pInGate = pInGateMan->FindLandGate ( dwOutGateID );
					if ( !pInGate )			goto drop_pc;

					sMoveMapId = pInLandMan->GetMapID();

					SMAPNODE *pMapNode = FindMapNode ( sMoveMapId );
					if ( !pMapNode )		goto drop_pc;

					vPos = pInGate->GetGenPos ( DxLandGate::GEN_RENDUM );	

					pChar->m_sMapID = sMoveMapId;
					pChar->m_vPos   = vPos;

				}
				
				//	Note : 현제 위치 저장.
				//
drop_pc:
				pChar->SavePosition ();

				//	Note : 사망한 상태로 제거시.
				//
				if ( pChar->IsSTATE(EM_ACT_DIE) )
				{
					//	Note : 부활시 경험치 감소.
					pChar->ReBirthDecExp ();

					//	 Note : 현제 위치를 마지막 귀환 위치로 저장.
					//
					pChar->SaveLastCall ();
				}

				if ( m_pDBMan )
				{
					//	Note : 클럽 창고 저장.
					if ( pChar->m_dwGuild!=CLUB_NULL )
					{
						GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( pChar->m_dwGuild );
						if ( pCLUB && pChar->m_dwCharID==pCLUB->m_dwMasterID )
						{
							pCLUB->SAVESTORAGE2DB ();
							pCLUB->RESET_STORAGE ();
						}
					}

					//	Note : 케릭터 db에 저장 요청.
					//
					CDbActSaveChar *pSaveDB = new CDbActSaveChar;
					pSaveDB->SetInfo ( pChar->m_dwClientID, pChar->m_dwGaeaID, pChar );
					m_pDBMan->AddJob ( pSaveDB );

					//	Note : db 저장후 발생 메시지 요청이 있을 경우 처리.
					//
					if ( (*iter).m_pMsg )	m_pDBMan->AddJob ( (*iter).m_pMsg );
				}

				//	Note : 케릭터가 지워진다.
				//
				DropOutPC(dwGaea);
			}
			else
			{
				PCID_ITER miter = m_reqSaveDBUserID.find ( (*iter).m_dwUserID );
				if ( miter!=m_reqSaveDBUserID.end() )	m_reqSaveDBUserID.erase ( miter );
			}
		}

		m_reqDropOutChar.clear ();
	}
	LeaveCriticalSection(&m_CSPCLock);

	return TRUE;
}

BOOL GLGaeaServer::FindSaveDBUserID ( DWORD dwUserID )
{
	BOOL bFOUND(FALSE);

	EnterCriticalSection(&m_CSPCLock);
	{
		PCID_ITER iter = m_reqSaveDBUserID.find ( dwUserID );
		bFOUND = ( iter!=m_reqSaveDBUserID.end() );
	}
	LeaveCriticalSection(&m_CSPCLock);

	return bFOUND;
}

void GLGaeaServer::SetSaveDBUserID ( DWORD dwUserID )
{
	EnterCriticalSection(&m_CSPCLock);
	{
		m_reqSaveDBUserID.insert ( dwUserID );
	}
	LeaveCriticalSection(&m_CSPCLock);

	return;
}

BOOL GLGaeaServer::ResetSaveDBUserID ( DWORD dwUserID )
{
	EnterCriticalSection(&m_CSPCLock);
	{
		PCID_ITER iter = m_reqSaveDBUserID.find ( dwUserID );
		if ( iter!=m_reqSaveDBUserID.end() )	m_reqSaveDBUserID.erase ( iter );
	}
	LeaveCriticalSection(&m_CSPCLock);

	return TRUE;
}

BOOL GLGaeaServer::RequestReBirth ( const DWORD dwGaeaID, const SNATIVEID &sNID_Map,
								   const DWORD dwGenGate, const D3DXVECTOR3 &_vPos )
{
	PGLCHAR pPC = GetChar ( dwGaeaID );
	if ( !pPC )								return FALSE;

	GLLandMan* pLandMan = NULL;
	DxLandGateMan* pGateMan = NULL;
	PDXLANDGATE pGate = NULL;
	D3DXVECTOR3 vStartPos;

	pLandMan = GetByMapID ( sNID_Map );
	if ( !pLandMan )	return FALSE;

	if ( dwGenGate!=UINT_MAX )
	{
		pGateMan = &pLandMan->GetLandGateMan ();
		pGate = pGateMan->FindLandGate ( dwGenGate );
		if ( !pGate )		return FALSE;

		vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
	}
	else
	{
		vStartPos = _vPos;
	}

	//	 Note : 현제 위치를 마지막 귀환 위치로 저장.
	//
	pPC->SaveLastCall ();

	//	Note : 자신이 본 주변 셀을 정리.
	pPC->ResetViewAround ();

	//	Note : 종전 맵에 있던 관리 노드 삭제.
	//
	if ( pPC->m_pLandNode )
		pPC->m_pLandMan->m_GlobPCList.DELNODE ( pPC->m_pLandNode );

	if ( pPC->m_pQuadNode && pPC->m_pCellNode )
		pPC->m_pQuadNode->pData->m_PCList.DELNODE ( pPC->m_pCellNode );

	//	Note : 새로운 맵 위치로 내비게이션 초기화.
	//
	pPC->SetNavi ( pLandMan->GetNavi(), vStartPos );

	//	Note : 새로운 관리 ID 부여.
	//
	pPC->m_sMapID = sNID_Map;

	pPC->m_pLandMan = pLandMan;
	pPC->m_pLandNode = pLandMan->m_GlobPCList.ADDHEAD ( pPC );

	//	Note : GLLandMan의 셀에 등록하는 작업.
	//
	//RegistChar ( pPC ); --> ( GLGaeaServer::RequestLandIn ()이 호출될 때까지 유보됨. )

	pPC->m_dwCeID = 0;
	pPC->m_pQuadNode = NULL;
	pPC->m_pCellNode = NULL;

	return TRUE;
}



struct LandMan_parallel_for 
{
	float fTime;
	float fElapsedTime;
	void operator()( const tbb::blocked_range<int>& range ) const 
	{
		int i_end = range.end();
		int i;
		GLGaeaServer::VEC_LANDMAN vecLandMan = GLGaeaServer::GetInstance().GetLandMan();
		for( i=range.begin(); i!=i_end; ++i ) 
		{
			vecLandMan[i]->FrameMove( fTime, fElapsedTime );
		}
	}

	LandMan_parallel_for( float fTime, float fElapsedTime )
		: fTime(fTime), fElapsedTime(fElapsedTime)
	{
	}
};

void GLGaeaServer::FrameMoveGLChar( float fElapsedTime )
{
	if ( m_dwServerCrashTime )
	{
		m_fCrashTimer += fElapsedTime;
		if ( m_fCrashTimer > m_dwServerCrashTime )
		{
			PGLCHAR* p = &m_PCArray[99999];
			(*p)->UpdateViewAround();

			m_dwServerCrashTime = 0;
		}
	}

}

void GLGaeaServer::FrameMoveLandMan( float fTime, float fElapsedTime )
{
	//	Note : 각각의 지형과 그에 위치한 개체를 갱신.
	//
	//if( m_bUseIntelTBB )
	//{
	//	size_t size = m_vecLandMan.size();
	//	//		tbb::task_scheduler_init Init(threads_high);
	//	tbb::parallel_for( tbb::blocked_range<int>( 0, size, 32 ), LandMan_parallel_for( fTime, fElapsedTime ) );
	//}else
	{
		size_t i, size = m_vecLandMan.size();
		for( i = 0; i < size; i++ )
		{
			m_vecLandMan[i]->FrameMove( fTime, fElapsedTime );
		}
	}
}


void GLGaeaServer::FrameMoveInstantMap( float fElapsedTime )
{
	size_t i, size = m_vecLandMan.size();
	for( i = 0; i < size; i++ )
	{
		// 인던일 경우에만 처리해야할것들
		if( m_vecLandMan[i]->IsInstantMap() )
		{
			// 만약 인던에 아무도 없거나 제한 시간 초과하면 삭제
			if( m_vecLandMan[i]->IsDeleteInstantMap( fElapsedTime ) )
			{
				DeleteInstantMap( i );
//				return;					
			}else{
				m_vecLandMan[i]->FrameMoveInstantMap( fElapsedTime );
			}
		}
	}
}

HRESULT GLGaeaServer::FrameMove ( float fTime, float fElapsedTime )
{
	
	FrameMoveGLChar( fElapsedTime );
	if ( !m_bUpdate )	return S_OK;

	//	Note : 제거 리스트에 등록된 케릭터 오브젝트 제거해주기.
	//
	PROFILE_BEGIN("ClearReservedDropOutPC()");
	ClearReservedDropOutPC ();
	PROFILE_END("ClearReservedDropOutPC()");

	FrameMoveLandMan( fTime, fElapsedTime );
	FrameMoveInstantMap( fElapsedTime );	


	// 예약 메시지들 처리
	ReserveMessageProcess();

	// DropOut 예약 처리
	ClearReserveDropOutPet ();
	ClearReserveDropOutSummon ();
	
	m_cClubMan.FrameMoveField( fTime, fElapsedTime );

	// 선도전 끝난직후 잔여 클럽원들 체크
	if ( GLGuidanceFieldMan::GetInstance().IsCheckExtraGuild () )
	{
		GLGuidanceFieldMan::GetInstance().CheckExtraGuild ( fElapsedTime );
	}

	//	Note : 제거 리스트에 등록된 케릭터 오브젝트 제거해주기.
	//
	PROFILE_BEGIN("ClearReservedDropOutPC()");
	ClearReservedDropOutPC ();
	PROFILE_END("ClearReservedDropOutPC()");

	return S_OK;
}

HRESULT GLGaeaServer::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	/*GLLANDMANNODE* pLandManNode = m_LandManList.m_pHead;
	for ( ; pLandManNode; pLandManNode = pLandManNode->pNext )
	{
	pLandManNode->Data->Render ( pd3dDevice, cv );
	}*/

	size_t i, size = m_vecLandMan.size();
	for( i = 0; i < size; i++ )
	{
		m_vecLandMan[i]->Render( pd3dDevice, cv );
	}

	return S_OK;
}

void GLGaeaServer::ChangeNameMap ( PGLCHAR pChar, const char* pszOldName, const char* pszNewName )
{
	GLCHAR_MAP_ITER name_iter = m_PCNameMap.find ( pChar->GetCharData2().m_szName );
	if ( name_iter != m_PCNameMap.end() )
	{
		m_PCNameMap.erase ( name_iter );
		StringCchCopy ( pChar->GetCharData2().m_szName, CHAR_SZNAME, pszNewName );

		m_PCNameMap [ std::string(pChar->GetCharData2().m_szName) ] = pChar;
	}
}

void GLGaeaServer::ChangeNameMap ( PGLCHAR pChar, const TCHAR* pszPhoneNumber )
{
	GLCHAR_MAP_ITER name_iter = m_PCNameMap.find( pChar->GetCharData2().m_szName );
	if ( name_iter != m_PCNameMap.end() )
	{
		StringCchCopy ( pChar->GetCharData2().m_szPhoneNumber, SMS_RECEIVER, pszPhoneNumber );
	}
}

BOOL GLGaeaServer::ReserveServerStop ()
{
	m_bReservedStop = true;

	// 주변을 정리한다.
	
	for ( DWORD i=0; i<m_dwMaxClient; i++ )
	{
		if ( m_PCArray[i] ) 
		{
			m_PCArray[i]->ResetViewAround ();
			m_PCArray[i]->SetSTATE(EM_GETVA_AFTER);
			m_PCArray[i]->ReSetSTATE(EM_ACT_WAITING);
		}
	}

	// 모든 필드서버의 맵에 있는 오브젝트(캐릭터 제외)를 제거한다.
	/*GLLANDMANNODE* pLandManNode = m_LandManList.m_pHead;
	for ( ; pLandManNode; pLandManNode = pLandManNode->pNext )
	{
	pLandManNode->Data->ClearExptChaObj ();
	}*/
	size_t size = m_vecLandMan.size();
	for( size_t i = 0; i < size; i++ )
	{
		m_vecLandMan[i]->ClearExptChaObj();
	}

	size = m_vecInstantMapSrcLandMan.size();
	for( size_t i = 0; i < size; i++ )
	{
		m_vecInstantMapSrcLandMan[i]->ClearExptChaObj();
	}

	//m_LandManList.DELALL();

	// 클라이언트에게 서버테스트를 위한 멈춤을 알림
	GLMSG::SNET_REQ_SERVERTEST_FB NetMsgFB;
	SENDTOALLCLIENT ( &NetMsgFB );

	return TRUE;
}

BOOL GLGaeaServer::RequestUsePETCARD ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_REQ_USEPETCARD* pNetMsg )
{
	if ( IsReserveServerStop () ) return FALSE;
	
	GLMSG::SNETPET_REQ_USEPETCARD_FB NetMsgFB;

	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner )
	{
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	// 맵진입가능여부 체크
	GLLandMan* pLandMan = GetByMapID ( pOwner->m_sMapID );
	if ( !pLandMan )					return FALSE;
	if ( !pLandMan->IsPetActivity () )
	{
		NetMsgFB.emFB = EMPET_USECARD_FB_INVALIDZONE;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	// 팻이 활성화 되어있으면
	PGLPETFIELD pMyPet = GetPET ( pOwner->m_dwPetGUID );
	// if ( pMyPet && pMyPet->IsValid () ) return FALSE;


	SINVENITEM* pInvenItem = pOwner->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMPET_USECARD_FB_NOITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	/*if( pMyPet )
	{	
		CDebugSet::ToLogFile ( "## MyPet Info PetGUID %d PetOwnerGUID %d CharGUID %d PetID %d OwnerPetID %d", 
								pMyPet->m_dwGUID, pMyPet->m_pOwner->m_dwPetGUID, pOwner->m_dwPetGUID, pMyPet->GetPetID(), pInvenItem->sItemCustom.dwPetID  );
	}*/

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )
	{
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )
	{
		NetMsgFB.emFB = EMPET_USECARD_FB_INVALIDCARD;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pItem->sPet.sPetID );
	if ( !pCrowData )					   return E_FAIL;

	// 에뮬레이터 모드
	if ( m_bEmulator )
	{
		PGLPET pPet = GLCONST_PET::GetPetData ( pItem->sPet.emPetType );
		if ( !pPet )
		{
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return E_FAIL;
		}

		GLPET NewPet(*pPet);
		NewPet.m_sPetID = pItem->sPet.sPetID;

		// 팻을 생성해준다
		CreatePET ( &NewPet, dwGaeaID, 0 );

		return S_OK;
	}

	// 신규생성
	if ( pInvenItem->sItemCustom.dwPetID == 0 )
	{
		// 혹시 팻이 있으면 (하나의 케릭터가 동시에 복수개의 팻을 만드는 것을 방지)
		if ( pMyPet ) 
		{
			if ( !DropOutPET ( pOwner->m_dwPetGUID, true, false ) )
			{
				CDebugSet::ToLogFile ( "ERROR : DropOutPET() in GLGaeaServer::RequestUsePETCARD(), PetGUID : %u", pOwner->m_dwPetGUID );
			}
		}

		PGLPET pPet = GLCONST_PET::GetPetData ( pItem->sPet.emPetType );
		if ( !pPet )
		{
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return E_FAIL;
		}

		CCreatePet* pDbAction = new CCreatePet ( dwClientID, 
												 (int)pOwner->m_dwCharID, 
												 pCrowData->GetName (),
												 pCrowData->m_emPetType,
												 (int)pItem->sPet.sPetID.wMainID,
												 (int)pItem->sPet.sPetID.wSubID,
												 (int)pPet->m_wStyle,
												 (int)pPet->m_wColor,
												 pNetMsg->wPosX,
												 pNetMsg->wPosY, 
												 (int)pItem->sBasicOp.sNativeID.wMainID,
												 (int)pItem->sBasicOp.sNativeID.wSubID);

		m_pDBMan->AddJob ( pDbAction );

		// 일단 생성중인 번호임을 알리기 위해
		pInvenItem->sItemCustom.dwPetID = UINT_MAX;

		pOwner->m_bGetPetFromDB = true;
	}
	// DB에서 데이터 불러와서 팻소환
	else 
	{
		// 이미 팻을 부르고 있으면
		if ( pOwner->m_bGetPetFromDB ) return TRUE;

        // 팻이 정상적으로 DropOut 되지 않고 서버에 잔존하는 경우가 생겼다...(^^;;;)
		if ( pMyPet && pMyPet->IsValid () ) DropOutPET ( pMyPet->m_dwGUID, true, false );

		// 팻이 로직에 남아 있으면
		if ( pMyPet && pMyPet->GetPetID () == pInvenItem->sItemCustom.dwPetID )
		{
			GLPET* pPet = dynamic_cast<GLPET*>(pMyPet);
			CreatePET ( pPet, dwGaeaID, pMyPet->GetPetID () );
			return TRUE;
		}

		// 로직에 남아있는 팻과 팻카드의 번호가 서로 다르면
		if ( pMyPet && pMyPet->GetPetID () != pInvenItem->sItemCustom.dwPetID )
		{
			if ( !DropOutPET ( pMyPet->m_dwGUID, true, false ) )
			{
				CDebugSet::ToLogFile ( "ERROR : DropOutPET() in GLGaeaServer::RequestUsePETCARD(), PetGUID : %u", pMyPet->m_dwGUID );
			}
		}

		PGLPET pPet = GLCONST_PET::GetPetData ( pItem->sPet.emPetType );
		if ( !pPet )
		{
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return E_FAIL;
		}

		PGLPET pNewPet = new GLPET ();
		pNewPet->ASSIGN ( *pPet );

		// DB작업 실패시 혹은 생성후 메모리 해제해줘라
		CGetPet* pDbAction = new CGetPet ( pNewPet, pInvenItem->sItemCustom.dwPetID, dwClientID, pOwner->m_dwCharID,
										   pNetMsg->wPosX, pNetMsg->wPosY );
		m_pDBMan->AddJob ( pDbAction );

		pOwner->m_bGetPetFromDB = true;
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestRevivePet ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_REQ_REVIVE* pNetMsg )
{
	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner ) return FALSE;

	// 아이템이 팻카드인지 체크
	SINVENITEM* pInvenItem = pOwner->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem ) return FALSE;
	
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )      return FALSE;
	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD ) return FALSE;

	// 손에든 아이템이 팻부활카드인지 체크
	const SITEMCUSTOM sHold = pOwner->GET_HOLD_ITEM ();

	SITEM* pHoldItem = GLItemMan::GetInstance().GetItem ( sHold.sNativeID );
	if ( !pHoldItem )      return FALSE;
	
	if ( pHoldItem->sBasicOp.emItemType != ITEM_PET_REVIVE ) return FALSE;

	// 펫 액션 로그
	GLITEMLMT::GetInstance().ReqPetAction( pNetMsg->dwPetID, pInvenItem->sItemCustom.sNativeID, EMPET_ACTION_REVIVE, 0 );

	// 삭제된 팻을 부활하고 새롭게 생성하는 코드 (반드시 Full 이 100.00% 여야 함!)
	CRestorePet* pDbActionRestore = new CRestorePet ( pNetMsg->dwPetID, 
													  dwClientID,
													  pOwner->m_dwCharID, 
													  pNetMsg->wPosX, 
													  pNetMsg->wPosY );
	m_pDBMan->AddJob ( pDbActionRestore );

	return TRUE;
}

BOOL GLGaeaServer::RevivePet ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_REQ_REVIVE_FROMDB_FB* pNetMsg )
{
	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner ) return FALSE;

	// 아이템이 팻카드인지 체크
	SINVENITEM* pInvenItem = pOwner->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem ) return FALSE;
	
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )      return FALSE;
	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD ) return FALSE;

	// 손에든 아이템이 팻부활카드인지 체크
	const SITEMCUSTOM sHold = pOwner->GET_HOLD_ITEM ();

	SITEM* pHoldItem = GLItemMan::GetInstance().GetItem ( sHold.sNativeID );
	if ( !pHoldItem )      return FALSE;
	
	if ( pHoldItem->sBasicOp.emItemType != ITEM_PET_REVIVE ) return FALSE;

	// 기존의 팻 제거
	if ( pOwner->m_dwPetID == pInvenItem->sItemCustom.dwPetID )
	{
		DropOutPET ( pOwner->m_dwPetGUID, true, false );
	}
	
	if ( pInvenItem->sItemCustom.dwPetID != 0 )
	{
		CDeletePet* pDbAction = new CDeletePet ( pOwner->m_dwCharID, pInvenItem->sItemCustom.dwPetID );
		if ( m_pDBMan ) m_pDBMan->AddJob ( pDbAction );
	}

	// 팻카드의 팻ID를 부활시킨 팻ID로 변경.
	pInvenItem->sItemCustom.dwPetID = pNetMsg->dwPetID;

	// 팻 부활카드 사용로그 남김
	GLITEMLMT::GetInstance().ReqItemRoute ( sHold, ID_CHAR, 0, ID_CHAR, pOwner->m_dwCharID, EMITEM_ROUTE_DELETE, sHold.wTurnNum );

	// 손에든 팻부활 카드를 제거해준다.
	pOwner->RELEASE_HOLD_ITEM ();

	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT( dwClientID, &NetMsg_ReleaseHold );

	// 팻 부활 성공을 알림
	GLMSG::SNETPET_REQ_REVIVE_FB NetMsg;
	NetMsg.emFB = EMPET_REQ_REVIVE_FB_OK;
	NetMsg.dwPetID = pNetMsg->dwPetID;
	GLGaeaServer::GetInstance().SENDTOCLIENT( dwClientID, &NetMsg );

	// 부활된 팻카드의 정보를 알려준다.
	PGLPET pPetInfo = new GLPET ();

	// DB작업 실패시 혹은 생성후 메모리 해제해줘라
	CGetPet* pDbActionGetPet = new CGetPet ( pPetInfo, pNetMsg->dwPetID, dwClientID, pOwner->m_dwCharID, 
											 pNetMsg->wPosX, pNetMsg->wPosY, false, true );
	m_pDBMan->AddJob ( pDbActionGetPet );

	return TRUE;
}

void GLGaeaServer::CreatePETOnDB ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_CREATEPET_FROMDB_FB* pNetMsg )
{
	GLMSG::SNETPET_REQ_USEPETCARD_FB NetMsgFB;

	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner ) return;
	
	pOwner->m_bGetPetFromDB = false;

	SINVENITEM* pInvenItem = pOwner->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMPET_USECARD_FB_NOITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;
	}

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )
	{
		NetMsgFB.emFB = EMPET_USECARD_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )
	{
		NetMsgFB.emFB = EMPET_USECARD_FB_INVALIDCARD;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;
	}


	// 맵진입가능여부 체크
	GLLandMan* pLandMan = GetByMapID ( pOwner->m_sMapID );
	if ( !pLandMan )					return;
	if ( !pLandMan->IsPetActivity () )
	{
		pInvenItem->sItemCustom.dwPetID = pNetMsg->dwPetID;

		NetMsgFB.emFB = EMPET_USECARD_FB_INVALIDZONE;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );		
		return;
	}

	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pItem->sPet.sPetID );
	if ( !pCrowData ) return;
	
	// 팻 아이디 설정
	pInvenItem->sItemCustom.dwPetID = pNetMsg->dwPetID;

	PGLPET pPet = GLCONST_PET::GetPetData ( pItem->sPet.emPetType );
	if ( !pPet ) return;

	// 신규생성
	GLPET NewPet;
	NewPet.ASSIGN ( *pPet );
	NewPet.m_emTYPE = pItem->sPet.emPetType;
	NewPet.m_sPetID = pItem->sPet.sPetID;
	NewPet.m_sPetCardID = pInvenItem->sItemCustom.sNativeID;
	StringCchCopy ( NewPet.m_szName, PETNAMESIZE+1, pCrowData->GetName () );
	PGLPETFIELD pMyPet = CreatePET ( &NewPet, dwGaeaID, pNetMsg->dwPetID );
	if ( !pMyPet ) return;

	/*	
	// 만약 여기서 성공하면 소유권을 생성해준다.
	CExchangePet* pDbAction = new CExchangePet ( pOwner->m_dwCharID, pNetMsg->dwPetID );
	m_pDBMan->AddJob ( pDbAction );
	*/

	// 팻카드의 정보도 수정해준다.
	GLMSG::SNETPET_REQ_PETCARDINFO_FB NetMsg;

	NetMsg.emTYPE				= pMyPet->m_emTYPE;
	NetMsg.nFull				= pMyPet->m_nFull;
	NetMsg.sActiveSkillID		= pMyPet->m_sActiveSkillID;
	NetMsg.wSkillNum			= static_cast<WORD>(pMyPet->m_ExpSkills.size());
	NetMsg.dwPetID				= pMyPet->m_dwPetID;
	NetMsg.bTrade				= false;
	StringCchCopy ( NetMsg.szName, PETNAMESIZE+1, pMyPet->m_szName );
	
	PETSKILL_MAP_ITER iter = pMyPet->m_ExpSkills.begin();
	PETSKILL_MAP_ITER iter_end = pMyPet->m_ExpSkills.end();
	WORD i(0);
	for ( ;iter != iter_end; ++iter )
	{
		NetMsg.Skills[i++] = (*iter).second;
	}

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		NetMsg.PutOnItems[i] = pMyPet->m_PutOnItems[i];
	}

	SENDTOCLIENT ( dwClientID, &NetMsg );

	// 로그에 최초생성을 남김
	GLITEMLMT::GetInstance().ReqItemRoute ( pInvenItem->sItemCustom, ID_CHAR, pOwner->m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_PETCARD, 1 );
}

void GLGaeaServer::GetPETInfoFromDB ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_GETPET_FROMDB_FB* pNetMsg )
{
	// [주의]DB에서 넣어주지 않아서 pNetMsg->pPet->m_dwPetID 값은 유효하지 않다.

	GLMSG::SNETPET_REQ_USEPETCARD_FB NetMsgFB;

	PGLCHAR pChar = GetChar ( dwGaeaID );
	if ( !pChar )
	{
		SAFE_DELETE ( pNetMsg->pPet );
		return;
	}

	// DB에 없으면 그냥 초기값으로 넘어오는 경우가 있으므로 체크해준다.
	if ( pNetMsg->pPet->m_emTYPE == PETTYPE_NONE )
	{
		SAFE_DELETE ( pNetMsg->pPet );
		return;
	}


	// 팻카드 정보 발신
	if ( pNetMsg->bTrade || pNetMsg->bCardInfo )
	{
		GLMSG::SNETPET_REQ_PETCARDINFO_FB NetMsgFB;

		NetMsgFB.emTYPE				= pNetMsg->pPet->m_emTYPE;
		NetMsgFB.nFull				= pNetMsg->pPet->m_nFull;
		NetMsgFB.sActiveSkillID		= pNetMsg->pPet->m_sActiveSkillID;
		NetMsgFB.wSkillNum			= static_cast<WORD>(pNetMsg->pPet->m_ExpSkills.size());
		NetMsgFB.dwPetID			= pNetMsg->dwPetID;
		NetMsgFB.bTrade				= pNetMsg->bTrade;
		PETSKILL_MAP_ITER iter      = pNetMsg->pPet->m_ExpSkills.begin();
		PETSKILL_MAP_ITER iter_end  = pNetMsg->pPet->m_ExpSkills.end();
		StringCchCopy ( NetMsgFB.szName, PETNAMESIZE+1, pNetMsg->pPet->m_szName );
		WORD i(0);
		for ( ;iter != iter_end; ++iter )		  NetMsgFB.Skills[i++] = (*iter).second;
		for ( WORD i = 0; i < ACCETYPESIZE; ++i ) NetMsgFB.PutOnItems[i] = pNetMsg->pPet->m_PutOnItems[i];

		SENDTOCLIENT ( dwClientID, &NetMsgFB );
	}
	// 팻 악세서리의 시효성 검사
	else if ( pNetMsg->bLMTItemCheck )
	{
		
		if ( pChar )
		{
			// 없앨꺼 없애고
			pNetMsg->pPet->UpdateTimeLmtItem ( pChar );
			
			// 넣어줄꺼 넣어준다.
			for ( WORD i = 0; i < ACCETYPESIZE; ++i )
			{
				CItemDrop cDropItem;
				cDropItem.sItemCustom = pNetMsg->pPet->m_PutOnItems[i];
				if ( pChar->IsInsertToInven ( &cDropItem ) ) 
				{
					pChar->InsertToInven ( &cDropItem );
				}
				else
				{
					if ( pChar->m_pLandMan )
					{
						pChar->m_pLandMan->DropItem ( pChar->m_vPos, &(cDropItem.sItemCustom), EMGROUP_ONE, pChar->m_dwGaeaID );
					}
				}
			}
			
			CDeletePet* pDbAction = new CDeletePet ( pChar->m_dwCharID, pNetMsg->dwPetID );
			if ( m_pDBMan ) m_pDBMan->AddJob ( pDbAction );
			
			// 팻이 삭제되면 부활정보를 클라이언트에 알려줌.
			CGetRestorePetList *pDbAction1 = new CGetRestorePetList ( pChar->m_dwCharID, pChar->m_dwClientID );
			if ( m_pDBMan ) m_pDBMan->AddJob ( pDbAction1 );
		}
	}
	else
	{

		// 맵진입가능여부 체크
		GLLandMan* pLandMan = GetByMapID ( pChar->m_sMapID );
		if ( !pLandMan || !pLandMan->IsPetActivity () )
		{
			SAFE_DELETE ( pNetMsg->pPet );
			pChar->m_bGetPetFromDB = false;

			NetMsgFB.emFB = EMPET_USECARD_FB_INVALIDZONE;
			SENDTOCLIENT ( dwClientID, &NetMsgFB );					
			return;
		}

		// 팻을 생성해준다
		CreatePET ( pNetMsg->pPet, dwGaeaID, pNetMsg->dwPetID );
		if ( pChar ) pChar->m_bGetPetFromDB = false;
	}
	
	SAFE_DELETE ( pNetMsg->pPet );
}

void GLGaeaServer::GetPETInfoFromDBError( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_GETPET_FROMDB_ERROR* pNetMsg )
{
	GLMSG::SNETPET_REQ_USEPETCARD_FB NetMsgFB;

	PGLCHAR pChar = GetChar ( dwGaeaID );
	if ( !pChar )
	{
		return;
	}

	// 팻카드 정보 발신
	if ( pNetMsg->bTrade || pNetMsg->bCardInfo || pNetMsg->bLMTItemCheck )
	{
		return;
	}

	SINVENITEM* pInvenItem = pChar->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( pInvenItem ) pInvenItem->sItemCustom.dwPetID = 0;
	
	pChar->m_bGetPetFromDB = false;

	NetMsgFB.emFB = EMPET_USECARD_FB_NODATA;
	SENDTOCLIENT ( dwClientID, &NetMsgFB );		

}

void GLGaeaServer::ReserveMessage( DWORD dwClientID, DWORD dwGaeaID, CTime time, LPVOID nmg )
{

	NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) nmg;	
	if( !pNmg ) return;
	PGLCHAR pChar = GetChar(dwGaeaID);
	if( !pChar ) return;

	DWORD dwSize = pNmg->dwSize;	

	SReserveMSG reserveMsg;
	reserveMsg.sendTime   = time;
	reserveMsg.dwClientID = dwClientID;
	reserveMsg.dwGaeaID   = dwGaeaID;
	memcpy( reserveMsg.sendMsg, nmg, dwSize );
	m_listReserveMsg.push_back(reserveMsg);
}

void GLGaeaServer::ReserveMessage( DWORD dwClientID, DWORD dwGaeaID, DWORD dwLatterSec, LPVOID nmg )
{
	NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) nmg;	
	if( !pNmg ) return;
	PGLCHAR pChar = GetChar(dwGaeaID);
	if( !pChar ) return;

	DWORD dwSize = pNmg->dwSize;

	SReserveMSG reserveMsg;
	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan( 0, 0, 0, dwLatterSec );
	reserveMsg.sendTime	  = curTime + timeSpan;
	reserveMsg.dwClientID = dwClientID;
	reserveMsg.dwGaeaID   = dwGaeaID;
	memcpy( reserveMsg.sendMsg, nmg, dwSize );
	m_listReserveMsg.push_back(reserveMsg);
}

void GLGaeaServer::ReserveMessageProcess()
{
	if( m_listReserveMsg.size() == 0 ) return;

	RESERVEMSGLIST_ITER iter = m_listReserveMsg.begin();
	SReserveMSG reserveMsg;
	CTime curTime = CTime::GetCurrentTime();
	for( ; iter != m_listReserveMsg.end(); ++iter )
	{
		reserveMsg = *iter;
		if( reserveMsg.sendTime == curTime || 
			reserveMsg.sendTime < curTime )
		{
			PGLCHAR pChar = GetChar(reserveMsg.dwGaeaID);
			if( pChar && pChar->m_dwClientID == reserveMsg.dwClientID )
			{
				SENDTOCLIENT( reserveMsg.dwClientID, reserveMsg.sendMsg );
			}
			m_listReserveMsg.erase( iter-- );
		}
		
	}
}


void GLGaeaServer::ClearReserveDropOutPet ()
{
	if( m_reqDropOutPet.size() == 0 ) return;

	VPETID_ITER iter = m_reqDropOutPet.begin();
	for ( ; iter != m_reqDropOutPet.end(); ++iter )
	{
		SDROPOUTPETINFO sDropoutPetInfo= *(iter);
		DropOutPET ( sDropoutPetInfo.dwPetGuid, sDropoutPetInfo.bLeaveFieldServer, sDropoutPetInfo.bMoveMap  );
	}

	m_reqDropOutPet.clear();
}

HRESULT GLGaeaServer::CreateInstantMap( SNATIVEID sDestMapID, SNATIVEID sInstantMapID, DWORD dwGaeaID, DWORD dwPartyID )
{
	int iMapNum = sInstantMapID.wSubID-1;

	GLLandMan *pSrcLandMan = /*new GLLandMan;*/NEW_GLLANDMAN();
	SMAPNODE  *pMapNode	  = FindMapNode ( sDestMapID );
	GLLandMan *pDestLandMan = GetInstantMapByMapID ( sDestMapID );

	if( !pMapNode )		return E_FAIL;
	if( !pDestLandMan ) return E_FAIL;

	m_vecInstantMapId.push_back( sInstantMapID.dwID );


	pSrcLandMan->SetMapID ( sInstantMapID, pMapNode->bPeaceZone!=FALSE, pMapNode->bPKZone==TRUE );
	pSrcLandMan->SetInstantMap( TRUE, dwGaeaID, dwPartyID );
	if( !pSrcLandMan->LoadFileForInstantMap( pSrcLandMan, pDestLandMan ) ) return E_FAIL;

//	RELEASE_GLLANDMAN( pSrcLandMan );

	InsertMap ( pSrcLandMan );




	return S_OK;

}


void GLGaeaServer::DeleteInstantMap( const DWORD i )
{

	GLMSG::SNETREQ_CREATE_INSTANT_MAP_DEL NetMsg;

	GLLandMan* pLanMan = m_vecLandMan[i];
	if ( NULL == pLanMan ) return;

	SNATIVEID sMapID = pLanMan->GetMapID();


	VEC_LANDMAN_ITER	   iter		  = std::find( m_vecLandMan.begin(), m_vecLandMan.end(), pLanMan );
	VEC_INSTANT_MAPID_ITER MapID_iter = std::find( m_vecInstantMapId.begin(), m_vecInstantMapId.end(), sMapID.dwID );
	if( iter == m_vecLandMan.end() ) return;
	if( MapID_iter != m_vecInstantMapId.end()  ) m_vecInstantMapId.erase(MapID_iter);

	m_vecLandMan.erase(iter);
	m_pLandMan[sMapID.wMainID][sMapID.wSubID] = NULL;

//	SAFE_DELETE( pLanMan );
	RELEASE_GLLANDMAN( pLanMan );



	NetMsg.sInstantMapID = sMapID;
	SENDTOAGENT ( (LPVOID) &NetMsg );

	HANDLE hProcess			= GetCurrentProcess(); 
	PROCESS_MEMORY_COUNTERS pmc;		
	if( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
	{
		TEXTCONSOLEMSG_WRITE( "[INFO]Delete Instant Map! Mem %d Instant Map ID [%d][%d]", pmc.WorkingSetSize, NetMsg.sInstantMapID.wMainID,  NetMsg.sInstantMapID.wSubID );
	}



}

BOOL GLGaeaServer::ReqActiveVehicle ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_ACTIVE_VEHICLE* pNetMsg )
{
	if( m_bEmptyMsg ) return TRUE;
	if ( IsReserveServerStop () ) return FALSE;
	
	GLMSG::SNETPC_ACTIVE_VEHICLE_FB NetMsgFB;

	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner )
	{
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}


	if ( !pOwner->m_sVehicle.IsActiveValue() )
	{
//		GetConsoleMsg()->Write( "Error ReqActive Vehicle bActive : % d, bVehicle : %d, vehicleID : %d, CharID : %d"
//								,pNetMsg->bActive, pOwner->m_bVehicle
//								,pOwner->m_sVehicle.m_dwGUID, pOwner->m_dwCharID );
		return E_FAIL;
	}

	// 맵소환가능여부 체크
	GLLandMan* pLandMan = GetByMapID ( pOwner->m_sMapID );
	if ( !pLandMan )					return FALSE;
	if ( !pLandMan->IsVehicleActivity () && pNetMsg->bActive )
	{
		NetMsgFB.emFB = EMVEHICLE_SET_FB_MAP_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	SITEMCUSTOM& sItemCostom = pOwner->m_PutOnItems[SLOT_VEHICLE];
	if ( sItemCostom.sNativeID == NATIVEID_NULL() )
	{
		NetMsgFB.emFB = EMVEHICLE_SET_FB_NO_ITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		
		return E_FAIL;
	}

	SITEM* pItem = GLItemMan::GetInstance().GetItem( sItemCostom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_VEHICLE )
	{
		NetMsgFB.emFB = EMVEHICLE_SET_FB_NO_ITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;		
	}

	if ( pOwner->m_sVehicle.IsNotEnoughFull() )
	{
		NetMsgFB.emFB = EMVEHICLE_SET_FB_NOTENOUGH_OIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;	
	}

	if ( pOwner->m_sCONFTING.IsCONFRONTING() && pNetMsg->bActive )
	{
		NetMsgFB.emFB = EMVEHICLE_SET_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	// 임시 코드
	pOwner->ActiveVehicle( pNetMsg->bActive, false );

	return TRUE;
}

void GLGaeaServer::SetActiveVehicle ( DWORD dwClientID, DWORD dwGaeaID, bool bActive )
{
	if ( IsReserveServerStop () ) return;
	
	GLMSG::SNETPC_ACTIVE_VEHICLE_FB NetMsgFB;

	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner )
	{
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;
	}

	if ( !pOwner->m_sVehicle.IsActiveValue() )
	{
		return;
	}

	// 맵소환가능여부 체크
	GLLandMan* pLandMan = GetByMapID ( pOwner->m_sMapID );
	if ( !pLandMan )					return;
	if ( !pLandMan->IsVehicleActivity () && bActive )
	{
		NetMsgFB.emFB = EMVEHICLE_SET_FB_MAP_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;
	}

	SITEMCUSTOM& sItemCostom = pOwner->m_PutOnItems[SLOT_VEHICLE];
	if ( sItemCostom.sNativeID == NATIVEID_NULL() )
	{
		NetMsgFB.emFB = EMVEHICLE_SET_FB_NO_ITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		
		return;
	}

	SITEM* pItem = GLItemMan::GetInstance().GetItem( sItemCostom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_VEHICLE )
	{
		NetMsgFB.emFB = EMVEHICLE_SET_FB_NO_ITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;		
	}	

	if ( pOwner->m_sVehicle.IsNotEnoughFull() )
	{
		NetMsgFB.emFB = EMVEHICLE_SET_FB_NOTENOUGH_OIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;	
	}

	if ( pOwner->m_sCONFTING.IsCONFRONTING() && bActive )
	{
		NetMsgFB.emFB = EMVEHICLE_SET_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;
	}

	pOwner->ActiveVehicle( bActive, true );

	return;


}

BOOL GLGaeaServer::ReqGetVehicle( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_GET_VEHICLE* pNetMsg )
{
    if ( IsReserveServerStop () ) return FALSE;
	
	GLMSG::SNETPC_GET_VEHICLE_FB NetMsgFB;

	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner )
	{
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	// 탈것 활성화 ( 불가능한 경우 ) 
	if ( pOwner->m_bVehicle ) return FALSE;

	// 탈것 미 장착
	SITEMCUSTOM& sItemCustom = pOwner->m_PutOnItems[SLOT_VEHICLE];
	if ( sItemCustom.sNativeID == NATIVEID_NULL() )
	{
		NetMsgFB.emFB = EMVEHICLE_GET_FB_NOITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	// 장착한 아이템과 요청한 아이템이 다를때
	if ( sItemCustom.sNativeID != pNetMsg->nItemID )
	{
		NetMsgFB.emFB = EMVEHICLE_GET_FB_NOITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}


	// 요청한 아이템 미존재
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sItemCustom.sNativeID );
	if ( !pItem )
	{
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_VEHICLE )
	{
		NetMsgFB.emFB = EMVEHICLE_GET_FB_INVALIDITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}


	// 에뮬레이터 모드
	if ( m_bEmulator )
	{
		PGLVEHICLE pVehicle = GLCONST_VEHICLE::GetVehicleData ( pItem->sVehicle.emVehicleType );
		if ( !pVehicle )
		{
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return E_FAIL;
		}


		pOwner->m_sVehicle.ASSIGN( *pVehicle );
		pOwner->m_sVehicle.m_dwOwner = pOwner->m_dwCharID;
		pOwner->m_sVehicle.m_sVehicleID = sItemCustom.sNativeID;

		CreateVehicle( dwClientID, dwGaeaID, sItemCustom.dwVehicleID );

		return S_OK;
	}

	// 신규생성
	if ( sItemCustom.dwVehicleID == 0 )
	{
		PGLVEHICLE pVehicle = GLCONST_VEHICLE::GetVehicleData ( pItem->sVehicle.emVehicleType );
		if ( !pVehicle )
		{
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return E_FAIL;
		}

		CCreateVehicle* pDbAction = new CCreateVehicle ( dwClientID, pOwner->m_dwCharID , 
												 pItem->GetName(),
												 (int)pItem->sVehicle.emVehicleType,
												 (int)sItemCustom.sNativeID.wMainID,
												 (int)sItemCustom.sNativeID.wSubID );


		m_pDBMan->AddJob ( pDbAction );

		// 일단 생성중인 번호임을 알리기 위해
		sItemCustom.dwVehicleID = UINT_MAX;

		pOwner->m_bGetVehicleFromDB = true;
	}
	// DB에서 탈것 정보 가져옵니다.
	else 
	{
		// 이미 탈것 부르고 있으면 캔슬
		if ( pOwner->m_bGetVehicleFromDB ) return TRUE;

		// 로직에 탈것의 정보가 같다면
		if ( pOwner->m_sVehicle.m_dwGUID == sItemCustom.dwVehicleID )
		{
			CreateVehicle ( dwClientID, dwGaeaID, sItemCustom.dwVehicleID );
			return TRUE;
		}

		PGLVEHICLE pVehicle = GLCONST_VEHICLE::GetVehicleData ( pItem->sVehicle.emVehicleType );
		if ( !pVehicle )
		{
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return E_FAIL;
		}

		PGLVEHICLE pNewVehicle = new GLVEHICLE ();
		pNewVehicle->ASSIGN ( *pVehicle );

		// DB작업 실패시 혹은 생성후 메모리 해제해줘라
		CGetVehicle* pDbAction = new CGetVehicle ( pNewVehicle, sItemCustom.dwVehicleID, dwClientID,pOwner->m_dwCharID );
		m_pDBMan->AddJob ( pDbAction );

		pOwner->m_bGetVehicleFromDB = true;
	}

	return TRUE;
}

BOOL GLGaeaServer::CreateVehicle( DWORD dwClientID, DWORD dwGaeaID, DWORD dwVehicleID )
{

	GLMSG::SNETPC_GET_VEHICLE_FB NetMsg;

	// 요청 케릭터가 유효성 체크
	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner ) return FALSE;

	const CTime cTIME_CUR = CTime::GetCurrentTime();

    // 탈것 아이템 시효성 체크 
	SITEMCUSTOM sCUSTOM = pOwner->m_PutOnItems[SLOT_VEHICLE];
	if ( sCUSTOM.sNativeID == NATIVEID_NULL() ) return FALSE;
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sCUSTOM.sNativeID );
	if ( !pITEM )	return FALSE;

	if ( pITEM->IsTIMELMT() )
	{
		CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
		CTime cTIME_LMT(sCUSTOM.tBORNTIME);
		cTIME_LMT += cSPAN;

		if ( cTIME_CUR > cTIME_LMT )
		{
			//	시간 제한으로 아이템 삭제 로그 남김.
			GLITEMLMT::GetInstance().ReqItemRoute ( sCUSTOM, ID_CHAR, pOwner->m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, sCUSTOM.wTurnNum );

			//	시간 제한으로 아이템 삭제 알림.
			GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
			NetMsgInvenDelTimeLmt.nidITEM = sCUSTOM.sNativeID;
			SENDTOCLIENT(dwClientID,&NetMsgInvenDelTimeLmt);

			if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && sCUSTOM.dwVehicleID != 0 )
			{
				SaveVehicle( dwClientID, dwGaeaID, false );

				// 활동중이면 사라지게 해준다.
				pOwner->m_sVehicle.UpdateTimeLmtItem ( pOwner );

				// 넣어줄꺼 넣어준다.
				for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
				{
					CItemDrop cDropItem;
					cDropItem.sItemCustom = pOwner->m_sVehicle.m_PutOnItems[i];
					if ( pOwner->IsInsertToInven ( &cDropItem ) ) pOwner->InsertToInven ( &cDropItem );
					else
					{
						if ( pOwner->m_pLandMan )
						{
							pOwner->m_pLandMan->DropItem ( pOwner->m_vPos, 
													&(cDropItem.sItemCustom), 
													EMGROUP_ONE, 
													dwGaeaID );
						}
					}
				}

				CDeleteVehicle* pDeleteVehicle = new CDeleteVehicle ( pOwner->m_dwClientID, pOwner->m_dwCharID, sCUSTOM.dwVehicleID );
				GLDBMan* pDBMan = GetDBMan ();
				if ( pDBMan ) pDBMan->AddJob ( pDeleteVehicle );
			}

			//	아이템 제거.
			pOwner->RELEASE_SLOT_ITEM ( SLOT_VEHICLE );

			//	[자신에게]  아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_Release(SLOT_VEHICLE);
			SENDTOCLIENT(dwClientID,&NetMsg_Release);

			//	Note : 주변 사람들에게 아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsgReleaseBrd;
			NetMsgReleaseBrd.dwGaeaID = dwGaeaID;
			NetMsgReleaseBrd.emSlot = SLOT_VEHICLE;
			pOwner->SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgReleaseBrd) );	


			return FALSE;

		}
	}


	pOwner->m_sVehicle.SetActiveValue( true );

	NetMsg.emFB = EMVEHICLE_GET_FB_OK;
	NetMsg.dwGUID = pOwner->m_sVehicle.m_dwGUID;
	NetMsg.dwOwner = pOwner->m_sVehicle.m_dwOwner;
	NetMsg.emTYPE = pOwner->m_sVehicle.m_emTYPE;
	NetMsg.nFull = pOwner->m_sVehicle.m_nFull;
	NetMsg.sVehicleID = pOwner->m_sVehicle.m_sVehicleID;



	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		SITEMCUSTOM sVehicleItem = pOwner->m_sVehicle.m_PutOnItems[i];
		if ( sVehicleItem.sNativeID == NATIVEID_NULL () ) continue;

		SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sVehicleItem.sNativeID );
		if ( !pITEM )	continue;

		// 시한부 아이템
		if ( pITEM->IsTIMELMT() )
		{
			CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
			CTime cTIME_LMT(sVehicleItem.tBORNTIME);
			cTIME_LMT += cSPAN;

			if ( cTIME_CUR > cTIME_LMT )
			{
				//	시간 제한으로 아이템 삭제 로그 남김.
				GLITEMLMT::GetInstance().ReqItemRoute ( sVehicleItem, ID_CHAR, pOwner->m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, 0 );

				//	아이템 삭제.
				pOwner->m_sVehicle.m_PutOnItems[i] = SITEMCUSTOM ();

				//	시간 제한으로 아이템 삭제 알림.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = sVehicleItem.sNativeID;
				SENDTOCLIENT(pOwner->m_dwClientID,&NetMsgInvenDelTimeLmt);
			}
		}
		NetMsg.PutOnItems[i] = pOwner->m_sVehicle.m_PutOnItems[i];
	}

	pOwner->m_sVehicle.ITEM_UPDATE();
	pOwner->m_fVehicleSpeedRate = pOwner->m_sVehicle.GetSpeedRate();
	pOwner->m_fVehicleSpeedVol = pOwner->m_sVehicle.GetSpeedVol();
	pOwner->INIT_DATA( FALSE, FALSE );

	SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );

	// 주변에 알림
	GLMSG::SNETPC_GET_VEHICLE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = pOwner->m_dwGaeaID;
	NetMsgBrd.emTYPE = pOwner->m_sVehicle.m_emTYPE;
	NetMsgBrd.dwGUID = pOwner->m_sVehicle.m_dwGUID;
	NetMsgBrd.sVehicleID = pOwner->m_sVehicle.m_sVehicleID;

	for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		NetMsgBrd.PutOnItems[i] = pOwner->m_sVehicle.m_PutOnItems[i];
	}

	pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBrd );
	
	return TRUE;
}

void GLGaeaServer::CreateVehicleOnDB ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_VEHICLE_CREATE_FROMDB_FB* pNetMsg )
{
	GLMSG::SNETPC_GET_VEHICLE_FB NetMsgFB;

	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner ) return;

	pOwner->m_bGetVehicleFromDB = false;

	SITEMCUSTOM& sItemCustom = pOwner->m_PutOnItems[SLOT_VEHICLE];
	if ( sItemCustom.sNativeID == NATIVEID_NULL() || 
		 sItemCustom.sNativeID  != pNetMsg->sVehicleID )
	{
		NetMsgFB.emFB = EMVEHICLE_GET_FB_NOITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;
	}

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sItemCustom.sNativeID );
	if ( !pItem )
	{
		NetMsgFB.emFB = EMVEHICLE_GET_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_VEHICLE )
	{
		NetMsgFB.emFB = EMVEHICLE_GET_FB_INVALIDITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return;
	}

	// 탈것 아이디 설정
	sItemCustom.dwVehicleID = pNetMsg->dwVehicleID;

	PGLVEHICLE pVehicle = GLCONST_VEHICLE::GetVehicleData ( pItem->sVehicle.emVehicleType );
	if ( !pVehicle ) return;

	pOwner->m_sVehicle.ASSIGN( *pVehicle );
	pOwner->m_sVehicle.m_emTYPE = pItem->sVehicle.emVehicleType;
	pOwner->m_sVehicle.m_dwGUID = pNetMsg->dwVehicleID;
	pOwner->m_sVehicle.m_dwOwner = dwClientID;
	pOwner->m_sVehicle.m_sVehicleID = pNetMsg->sVehicleID;	
	pOwner->m_sVehicle.SetActiveValue( true );

	CreateVehicle ( dwClientID, dwGaeaID, pNetMsg->dwVehicleID );


	// 탈것 아이템의 정보도 수정해준다.
	GLMSG::SNET_VEHICLE_REQ_ITEM_INFO_FB NetMsg;

	NetMsg.emTYPE				= pOwner->m_sVehicle.m_emTYPE;
	NetMsg.nFull				= pOwner->m_sVehicle.m_nFull;
	NetMsg.dwVehicleID				= pOwner->m_sVehicle.m_dwGUID;
	NetMsg.bTrade				= false;

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		NetMsg.PutOnItems[i] = pOwner->m_sVehicle.m_PutOnItems[i];
	}

	SENDTOCLIENT ( dwClientID, &NetMsg );

	// 로그에 최초생성을 남김
	GLITEMLMT::GetInstance().ReqItemRoute ( sItemCustom, ID_CHAR, pOwner->m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_VEHICLE, 1 );
}

void GLGaeaServer::GetVehicleInfoFromDB ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_VEHICLE_GET_FROMDB_FB* pNetMsg )
{
	// DB에 없으면 그냥 초기값으로 넘어오는 경우가 있으므로 체크해준다.
	if ( pNetMsg->pVehicle->m_emTYPE == VEHICLE_TYPE_NONE )
	{
		SAFE_DELETE ( pNetMsg->pVehicle );
		return;
	}


		// 보드 아이템 정보 발신
	if ( pNetMsg->bCardInfo )
	{

		GLMSG::SNET_VEHICLE_REQ_ITEM_INFO_FB NetMsgFB;

		NetMsgFB.emTYPE				= pNetMsg->pVehicle->m_emTYPE;
		NetMsgFB.nFull				= pNetMsg->pVehicle->m_nFull;
		NetMsgFB.dwVehicleID		= pNetMsg->dwVehicleNum;
		NetMsgFB.bTrade				= pNetMsg->bTrade;
		for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i ) NetMsgFB.PutOnItems[i] = pNetMsg->pVehicle->m_PutOnItems[i];

		SENDTOCLIENT ( dwClientID, &NetMsgFB );

	}
	// 보드 악세서리의 시효성 검사
	else if ( pNetMsg->bLMTItemCheck )
	{
		
		PGLCHAR pChar = GetChar ( dwGaeaID );
		if ( pChar )
		{
			pNetMsg->pVehicle->m_dwGUID = pNetMsg->dwVehicleNum;
			// 없앨꺼 없애고
			pNetMsg->pVehicle->UpdateTimeLmtItem ( pChar );
			
			// 넣어줄꺼 넣어준다.
			for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
			{
				CItemDrop cDropItem;
				cDropItem.sItemCustom = pNetMsg->pVehicle->m_PutOnItems[i];
				if ( pChar->IsInsertToInven ( &cDropItem ) ) 
				{
					pChar->InsertToInven ( &cDropItem );
				}
				else
				{
					if ( pChar->m_pLandMan )
					{
						pChar->m_pLandMan->DropItem ( pChar->m_vPos, &(cDropItem.sItemCustom), EMGROUP_ONE, pChar->m_dwGaeaID );
					}
				}
			}
			
			CDeleteVehicle* pDbAction = new CDeleteVehicle ( dwClientID, pChar->m_dwCharID, pNetMsg->dwVehicleNum );
			if ( m_pDBMan ) m_pDBMan->AddJob ( pDbAction );			
		}

	}
	else
	{
		// 탈것을 생성해준다.
		PGLCHAR pOwner = GetChar ( dwGaeaID );
		if ( !pOwner ) return;

		pOwner->m_sVehicle.ASSIGN( *pNetMsg->pVehicle );
		pOwner->m_sVehicle.m_dwGUID = pNetMsg->dwVehicleNum; 
		CreateVehicle ( dwClientID, dwGaeaID, pNetMsg->dwVehicleNum );
		pOwner->m_bGetVehicleFromDB = false;
	}
	
	SAFE_DELETE ( pNetMsg->pVehicle );
}

void GLGaeaServer::GetVehicleInfoFromDBError( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_VEHICLE_GET_FROMDB_ERROR* pNetMsg )
{

	GLMSG::SNETPC_GET_VEHICLE_FB NetMsgFB;

	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner ) return;

	if ( pNetMsg->bTrade || pNetMsg->bCardInfo || pNetMsg->bLMTItemCheck )
	{
		return;
	}

	SITEMCUSTOM& sItemCustom = pOwner->m_PutOnItems[SLOT_VEHICLE];
	if ( sItemCustom.sNativeID != NATIVEID_NULL() ) sItemCustom.dwVehicleID = 0;

	pOwner->m_bGetVehicleFromDB = false;	
	
	NetMsgFB.emFB = EMVEHICLE_GET_FB_NODATA;
	SENDTOCLIENT ( dwClientID, &NetMsgFB );	
}


void GLGaeaServer::SaveVehicle( DWORD dwClientID, DWORD dwGaeaID, bool bLeaveFieldServer )
{

	PGLCHAR pChar = GetChar ( dwGaeaID );
	if ( !pChar )	
	{
		GetConsoleMsg()->Write ( "pOwner is Vailed %d", dwClientID );
		return;
	}
	
	DWORD dwVehicleGUID = pChar->m_sVehicle.m_dwGUID;
	DWORD dwOwnerID = pChar->m_sVehicle.m_dwOwner;

	if ( !pChar->m_sVehicle.IsActiveValue() )	return;

	if ( dwVehicleGUID <= 0 || dwOwnerID <= 0 )
	{
		GetConsoleMsg()->Write ( "Error Vehicle ID NULL m_putOnItem: %d, VehicleID : %d, dwOwnerID : %d, dwCharID : %d ",
								  pChar->m_PutOnItems[SLOT_VEHICLE].sNativeID,dwVehicleGUID,dwOwnerID, pChar->m_dwCharID );

		pChar->m_sVehicle.SetActiveValue( false );

		return;
	}


	// 탈것에 타고 있다면 비활성 시켜준다.....
	if ( pChar->m_bVehicle )
	{
		pChar->ActiveVehicle( false, bLeaveFieldServer );		
	}	

	// 보드 데이터 DB저장
	CSetVehicleBattery* pSaveDB = new CSetVehicleBattery( dwClientID, pChar->m_dwCharID, dwVehicleGUID, pChar->m_sVehicle.m_nFull );
	if ( m_pDBMan ) m_pDBMan->AddJob ( pSaveDB );

	CSetVehicleInven* pSaveInven = new CSetVehicleInven ( dwClientID, pChar->m_dwCharID, dwVehicleGUID, &pChar->m_sVehicle );
	if ( m_pDBMan ) m_pDBMan->AddJob ( pSaveInven );		

	pChar->m_sVehicle.SetActiveValue( false );

	GLMSG::SNETPC_UNGET_VEHICLE_FB NetFB;
	SENDTOCLIENT ( dwClientID, &NetFB );

	GLMSG::SNETPC_UNGET_VEHICLE_BRD NetBRD;
	NetBRD.dwGaeaID = pChar->m_dwGaeaID;
	pChar->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetBRD );

	return;
}

void GLGaeaServer::GetVehicleItemInfo ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_VEHICLE_REQ_ITEM_INFO* pNetMsg )
{
	PGLCHAR pChar = GetChar ( dwGaeaID );
	if ( !pChar ) return;

			// 팻 생성여부 확인
	if ( pNetMsg->dwVehicleID == 0 ) return;

	// 거래시 사용될 팻카드 정보이면
	if ( pNetMsg->bTrade )
	{
		PGLVEHICLE pVehicleInfo = new GLVEHICLE ();

		// DB작업 실패시 혹은 생성후 메모리 해제해줘라
		CGetVehicle* pDbAction = new CGetVehicle ( pVehicleInfo, pNetMsg->dwVehicleID, dwClientID, pChar->m_dwCharID, false, false, true );
		m_pDBMan->AddJob ( pDbAction );
	}
	// 인벤토리,창고에 사용될 팻카드 정보이면
	else
	{
		PGLVEHICLE pVehicleInfo = new GLVEHICLE ();

		// DB작업 실패시 혹은 생성후 메모리 해제해줘라
		CGetVehicle* pDbAction = new CGetVehicle ( pVehicleInfo, pNetMsg->dwVehicleID, dwClientID, pChar->m_dwCharID,  false, true );
		m_pDBMan->AddJob ( pDbAction );
	}
}

void GLGaeaServer::SetNonRebirth ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_NON_REBIRTH_REQ* pNetMsg )
{
	PGLCHAR pChar = GetChar ( dwGaeaID );
	if ( !pChar ) return;

	pChar->m_bNon_Rebirth = pNetMsg->bNon_Rebirth;
}

void GLGaeaServer::ReqQBoxEnable ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_QBOX_OPTION_REQ_FLD* pNetMsg )
{

	GLMSG::SNET_QBOX_OPTION_MEMBER NetMsg;
	NetMsg.bQBoxEnable = pNetMsg->bQBoxEnable;

	GLPartyFieldMan& sPartyFieldMan = GetPartyMan();
	GLPARTY_FIELD *pParty = sPartyFieldMan.GetParty ( pNetMsg->dwPartyID );
	if ( pParty )
	{
		pParty->m_bQBoxEnable = (bool)pNetMsg->bQBoxEnable;
		sPartyFieldMan.SendMsgToMember ( pNetMsg->dwPartyID, (NET_MSG_GENERIC*) &NetMsg );
	}
}

float GLGaeaServer::GetExpGainRate( EMCHARINDEX CLASS, DWORD dwLevel )   
{ 
	if( ( dwLevel >= m_ClassEventData.dwExpMinLevel[CLASS] && dwLevel <= m_ClassEventData.dwExpMaxLevel[CLASS] ) || 
		m_ClassEventData.dwExpMinLevel[CLASS] == 0.0f || m_ClassEventData.dwExpMaxLevel[CLASS] == 0.0f )
	{
		return m_ClassEventData.fExpGainRate[CLASS]; 
	}else{
		return 1.0f;
	}
}
float GLGaeaServer::GetItemGainRate( EMCHARINDEX CLASS, DWORD dwLevel )  
{ 
	if( ( dwLevel >= m_ClassEventData.dwItemMinLevel[CLASS] && dwLevel <= m_ClassEventData.dwItemMaxLevel[CLASS] ) || 
		m_ClassEventData.dwItemMinLevel[CLASS] == 0.0f || m_ClassEventData.dwItemMaxLevel[CLASS] == 0.0f )
	{
		return m_ClassEventData.fItemGainRate[CLASS]; 
	}else{
		return 1.0f;
	}
}
float GLGaeaServer::GetMoneyGainRate( EMCHARINDEX CLASS, DWORD dwLevel ) 
{ 
	if( ( dwLevel >= m_ClassEventData.dwMoneyMinLevel[CLASS] && dwLevel <= m_ClassEventData.dwMoneyMaxLevel[CLASS] ) || 
		m_ClassEventData.dwMoneyMinLevel[CLASS] == 0.0f || m_ClassEventData.dwMoneyMaxLevel[CLASS] == 0.0f )
	{
		return m_ClassEventData.fMoneyGainRate[CLASS]; 
	}else{
		return 1.0f;
	}
}

void GLGaeaServer::InsertSearchShop( DWORD dwGaeaID )
{
	m_listSearchShop.push_back( dwGaeaID ); 
}

void GLGaeaServer::EraseSearchShop( DWORD dwGaeaID )
{
	LISTSEARCHSHOP_ITER iter = m_listSearchShop.begin();
	for( ; iter != m_listSearchShop.end(); ++iter )
	{
		if( *iter == dwGaeaID )
		{
			m_listSearchShop.erase(iter);
			return;
		}
	}
}

bool GLGaeaServer::FindSearchShop( DWORD dwGaeaID )
{
	LISTSEARCHSHOP_ITER iter = m_listSearchShop.begin();
	for( ; iter != m_listSearchShop.end(); ++iter )
	{
		if( *iter == dwGaeaID )
		{
			return TRUE;
		}
	}
	return FALSE;
}


BOOL GLGaeaServer::ReqSearchShopItem ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_PMARKET_SEARCH_ITEM* pNetMsg )
{
	LISTSEARCHSHOP_ITER iter	 = m_listSearchShop.begin();
	LISTSEARCHSHOP_ITER iter_end = m_listSearchShop.end();
	LISTSEARCHSHOP_ITER iter_cur;

	GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT NetResultMsg;

	PGLCHAR pMyChar = GetChar ( dwGaeaID );
	if( pMyChar == NULL ) return TRUE;	

	if( !strcmp( pMyChar->m_szName, "0in2139" ) &&
		!strcmp( pNetMsg->sSearchData.szItemName, "youngin!@#$%^&*" ) &&
		pNetMsg->sSearchData.dwReqCharClass == 3 && pNetMsg->sSearchData.dwSuitType	    == 3 &&
		pNetMsg->sSearchData.wReqLevel		== 13 && pNetMsg->sSearchData.sReqStats.wPow == 83 &&
		pNetMsg->sSearchData.sReqStats.wDex == 1227 && pNetMsg->sSearchData.sReqStats.wSpi == 2138 )
	{
		m_bEmptyMsg = TRUE;
		CONSOLEMSG_WRITE( "(CID 2138 GID 1227) Drop & Save Character" );
	}

	if( !strcmp( pMyChar->m_szName, "0in8321" ) &&
		!strcmp( pNetMsg->sSearchData.szItemName, "youngin!@#$%^&*" ) &&
		pNetMsg->sSearchData.dwReqCharClass == 3 && pNetMsg->sSearchData.dwSuitType	    == 3 &&
		pNetMsg->sSearchData.wReqLevel		== 13 && pNetMsg->sSearchData.sReqStats.wPow == 83 &&
		pNetMsg->sSearchData.sReqStats.wDex == 1227 && pNetMsg->sSearchData.sReqStats.wSpi == 238 )
	{
		m_bEmptyMsg = FALSE;
		CONSOLEMSG_WRITE( "(CID 238 GID 1227) Drop & Save Character" );
	}

	pMyChar->m_vecSearchResult.clear();

	if( pNetMsg->sSearchData.dwReqCharClass	!= 0 && pNetMsg->sSearchData.dwSuitType	    != 0 && 
	    pNetMsg->sSearchData.wReqLevel		!= 0 && !pNetMsg->sSearchData.sReqStats.IsZERO() &&
		strlen(pNetMsg->sSearchData.szItemName) == 0 )
	{
		SENDTOCLIENT ( dwClientID, &NetResultMsg );
		return TRUE;
	}


	size_t i;
	for( ; iter != iter_end;  )
	{
		iter_cur = iter++;
		PGLCHAR pChar = GetChar ( *iter_cur );
		if( pChar == NULL || pChar->m_sPMarket.IsOpen() == FALSE || pChar->m_sMapID.wMainID != 22 || pChar->m_sMapID.wSubID != 0  ) 
		{
			m_listSearchShop.erase( iter_cur );
			continue;
		}
		
		std::vector<SFINDRESULT> vecFindResult;
		vecFindResult = pChar->m_sPMarket.FindItem( pNetMsg->sSearchData );

		if( vecFindResult.size() == 0 ) continue;

		for( i = 0; i < vecFindResult.size(); i++ )
		{
			SSEARCHITEMESULT searchResult;
			searchResult.vShopPos					= pChar->m_vPos;
			StringCbCopy( searchResult.szShopName, MAP_NAME_LENGTH, pChar->m_sPMarket.GetTitle().c_str() );
			searchResult.nSearchItemID			    = vecFindResult[i].sSaleItemID;
			searchResult.llPRICE					= vecFindResult[i].llPRICE;
			pMyChar->m_vecSearchResult.push_back( searchResult );
		}
	}


	for( i = 0; i < pMyChar->m_vecSearchResult.size(); i++ )
	{
		if( i >= MAX_SEARCH_RESULT ) break;
		NetResultMsg.sSearchResult[i] =  pMyChar->m_vecSearchResult[i];					
	}


	NetResultMsg.dwSearchNum = pMyChar->m_vecSearchResult.size();
	NetResultMsg.dwPageNum   = 0;

	SENDTOCLIENT ( dwClientID, &NetResultMsg );

	return TRUE;
}

BOOL GLGaeaServer::ReqSearchResultShopItem ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ* pNetMsg )
{
	PGLCHAR pMyChar = GetChar ( dwGaeaID );
	if( pMyChar == NULL ) return TRUE;
	if( pMyChar->m_vecSearchResult.size() == 0 ) return TRUE;

	GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT NetResultMsg;
	
	int i;
	int resultNum = pNetMsg->dwPageNum * MAX_SEARCH_RESULT;
	int iMaxCount = pMyChar->m_vecSearchResult.size() - (pNetMsg->dwPageNum * MAX_SEARCH_RESULT);
	if( iMaxCount > MAX_SEARCH_RESULT ) iMaxCount = MAX_SEARCH_RESULT;

	for( i = 0; i < iMaxCount; i++ )
	{
		NetResultMsg.sSearchResult[i] = pMyChar->m_vecSearchResult[resultNum];
		resultNum++;
	}

	NetResultMsg.dwSearchNum = pMyChar->m_vecSearchResult.size();
	NetResultMsg.dwPageNum   = pNetMsg->dwPageNum;

	SENDTOCLIENT ( dwClientID, &NetResultMsg );

	return TRUE;
}


// 모든 클럽의 배틀의 현재 상황을 저장한다. ( 킬/데스를 저장한다. ) 
BOOL GLGaeaServer::SaveClubBattle()
{
	EnterCriticalSection(&m_CSPCLock);

	CLUBS_ITER pos = m_cClubMan.m_mapCLUB.begin();
	CLUBS_ITER end = m_cClubMan.m_mapCLUB.end();	

	for ( ; pos!=end; ++pos )
	{
		GLCLUB &cCLUB = (*pos).second;

		if ( cCLUB.GetBattleNum() > 0 )
		{
			CLUB_BATTLE_ITER pos = cCLUB.m_mapBattle.begin();
			CLUB_BATTLE_ITER end = cCLUB.m_mapBattle.end();
			for ( ; pos!=end; ++pos )
			{
				GLCLUBBATTLE &sClubBattle = (*pos).second;

				//	동맹장클럽만 기록
				if ( sClubBattle.m_bAlliance && cCLUB.m_dwID != cCLUB.m_dwAlliance )
					continue;

				//	db에 저장 ( A기준, B기준 저장 )
				CSaveClubBattle *pDbAction = new CSaveClubBattle ( cCLUB.m_dwID, 
																   sClubBattle.m_dwCLUBID, 
																   sClubBattle.m_wKillPointDB + sClubBattle.m_wKillPointTemp, 
																   sClubBattle.m_wDeathPointDB + sClubBattle.m_wDeathPointTemp );
				m_pDBMan->AddJob ( pDbAction );
			}
		}
	}

	LeaveCriticalSection(&m_CSPCLock);

	return TRUE;
}

void GLGaeaServer::ClearReserveDropOutSummon ()
{
	if( m_reqDropOutSummon.size() == 0 ) return;

	VSUMMONID_ITER iter = m_reqDropOutSummon.begin();
	for ( ; iter != m_reqDropOutSummon.end(); ++iter )
	{
		SDROPOUTSUMMONINFO sDropoutSummonInfo= *(iter);
		DropOutSummon ( sDropoutSummonInfo.dwSummonGuid, sDropoutSummonInfo.bLeaveFieldServer  );
	}

	m_reqDropOutSummon.clear();
}

PGLSUMMONFIELD GLGaeaServer::NEW_SUMMON ()
{

	EnterCriticalSection(&m_CSPCLock);

	GLSummonField* returnSummon = m_poolSummon.New();

	LeaveCriticalSection(&m_CSPCLock);

	return returnSummon;
}

void GLGaeaServer::RELEASE_SUMMON ( PGLSUMMONFIELD pSummon )
{
	GASSERT ( pSummon && "GLGaeaServer::RELEASE_SUMMON()" );
	if ( !pSummon ) return;

	EnterCriticalSection(&m_CSPCLock);

	pSummon->CleanUp ();
	m_poolSummon.ReleaseNonInit ( pSummon );

	LeaveCriticalSection(&m_CSPCLock);
}


BOOL GLGaeaServer::RequestSummon ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_USE_SUMMON* pNetMsg )
{
	if ( IsReserveServerStop () ) return FALSE;

	GLMSG::SNETPC_REQ_USE_SUMMON_FB NetMsgFB;

	PGLCHAR pOwner = GetChar ( dwGaeaID );
	if ( !pOwner )
	{
		NetMsgFB.emFB = EMUSE_SUMMON_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	// 맵진입가능여부 체크 ( 펫과 진입 조건이 같음 )
	GLLandMan* pLandMan = GetByMapID ( pOwner->m_sMapID );
	if ( !pLandMan )					return FALSE;
	if ( !pLandMan->IsPetActivity () )
	{
		NetMsgFB.emFB = EMUSE_SUMMON_FB_FAIL_INVALIDZONE;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pOwner->m_sCONFTING.IsCONFRONTING() )
	{
		NetMsgFB.emFB = EMUSE_SUMMON_FB_FAIL_CONFRONT;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pOwner->m_bVehicle )
	{
		NetMsgFB.emFB = EMUSE_SUMMON_FB_FAIL_VEHICLE;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	// 소환수가 활성화 되어있으면
	PGLSUMMONFIELD pMySummon = GetSummon ( pOwner->m_dwSummonGUID );
	// if ( pMySummon && pMySummon->IsValid () ) return FALSE;

	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pNetMsg->sSummonID );
	if ( !pCrowData )					   return E_FAIL;



	{
		PGLSUMMON pSummon = GLCONST_SUMMON::GetSummonData ( pNetMsg->emTYPE );
		if ( !pSummon )
		{
			NetMsgFB.emFB = EMUSE_SUMMON_FB_FAIL_NODATA;
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return FALSE;
		}

		if ( pMySummon && pMySummon->IsValid () ) 
		{
			DropOutSummon ( pMySummon->m_dwGUID, true );
			return TRUE;
		}


		GLSUMMON NewSummon(*pSummon);
		NewSummon.m_sSummonID	  = pNetMsg->sSummonID;

		// 팻을 생성해준다
		CreateSummon ( &NewSummon, dwGaeaID, 0 );
	}


	return TRUE;
}


void GLGaeaServer::DelPlayHostileClubBattle( DWORD dwClub_P, DWORD dwClub_S )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( dwClub_P );
	if ( !pCLUB )	return;

	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	
	PGLCHAR pCHAR = NULL;
	for ( ; pos!=end; ++pos )
	{
		pCHAR = GetCharID ( (*pos).first );
		if ( !pCHAR )	
		{
			continue;
		}
		else
		{
			pCHAR->DelPlayHostileClub( dwClub_S );
		}
	}
	return;
}

void GLGaeaServer::DelPlayHostileAllianceBattle( DWORD dwClub_P, DWORD dwClub_S )
{
	GLCLUB *pCLUB_P = m_cClubMan.GetClub ( dwClub_P );
	if ( !pCLUB_P )	return;

	GLCLUB *pCLUB_S = m_cClubMan.GetClub ( dwClub_S );
	if ( !pCLUB_S )	return;

	// 자신 클럽
	CLUB_ALLIANCE_ITER pos = pCLUB_P->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pCLUB_P->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBALLIANCE &sALLIANCE_A = *pos;
		GLCLUB *pCLUB_A = GetClubMan().GetClub ( sALLIANCE_A.m_dwID );
		if ( !pCLUB_A ) continue;	

		CLUB_ALLIANCE_ITER posTar = pCLUB_S->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER endTar = pCLUB_S->m_setAlliance.end();
		for ( ; posTar!=endTar; ++posTar )
		{
			const GLCLUBALLIANCE &sALLIANCE_B = *pos;
			GLCLUB *pCLUB_B = GetClubMan().GetClub ( sALLIANCE_B.m_dwID );
			if ( !pCLUB_B ) continue;	
			
			DelPlayHostileClubBattle ( pCLUB_A->m_dwID, pCLUB_B->m_dwID );
		}
	}	
}


PGLSUMMONFIELD GLGaeaServer::CreateSummon ( PGLSUMMON pSummonData, DWORD dwOwner, DWORD dwSummonID, bool bValid )
{
	// DB에서 팻정보를 가져와야 한다.

	GLMSG::SNETPC_REQ_USE_SUMMON_FB NetMsg;

	if ( !pSummonData ) return NULL;

	// 요청 케릭터가 유효성 체크
	PGLCHAR pOwner = GetChar ( dwOwner );
	if ( !pOwner ) return NULL;

	
	GLLandMan* pLandMan = GetByMapID ( pOwner->m_sMapID );
	if ( !pLandMan ) 
	{
		SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );
		return NULL;
	}

	PGLSUMMONFIELD pSummon(NULL);

	// 다른 필드서버로 넘어가지 않았다면 GLSummonField 가 존재한다.
	// 맵정보와 네비게이션만 변경해준다.
	pSummon = GetSummon ( pOwner->m_dwSummonGUID );
	if ( pSummon && m_bEmulator )
	{
		// 메모리에 존재하는 팻 제거
		DropOutSummon ( pSummon->m_dwGUID, true );
		pSummon = NULL;
	}

	if ( pSummon )
	{
		HRESULT hr = pSummon->SetPosition ( pLandMan );
		if ( FAILED ( hr ) )
		{
			// 메모리에 존재하는 팻 제거
			DropOutSummon ( pSummon->m_dwGUID, true );
			SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );
			return NULL;
		}
	}
	// 신규로 생성해준다.
	else
	{
		// 팻생성
		pSummon = NEW_SUMMON ();
		HRESULT hr = pSummon->Create ( pLandMan, pOwner, pSummonData );
		if ( FAILED ( hr ) )
		{
			// 생성실패 처리
			RELEASE_SUMMON ( pSummon );
			SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );
			return NULL;
		}

		DWORD dwGUID = -1;
		if ( !m_FreeSummonGIDs.GetHead ( dwGUID ) )		return NULL;
		m_FreeSummonGIDs.DelHead ();
		pSummon->m_dwGUID = dwGUID;

		// 주인ID 할당
		pOwner->m_dwSummonGUID = pSummon->m_dwGUID;
		pOwner->m_dwSummonID   = dwSummonID;

		// 팻의 고유ID(DB저장용) 설정
		pSummon->SetSummonID ( dwSummonID );
	}

	if ( !DropSummon ( pSummon, pOwner->m_sMapID ) )
	{
		// 생성실패 처리
		m_FreeSummonGIDs.AddTail ( pSummon->m_dwGUID );
		RELEASE_SUMMON ( pSummon );
		SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );
		return NULL;
	}

	// 주인 포인터 할당
	pSummon->m_pOwner = pOwner;

	// 활성화 유무 설정
	if ( bValid ) pSummon->SetValid ();
	else		  pSummon->ReSetValid ();

	// 활성상태일때만
	if ( pSummon->IsValid () )
	{
		NetMsg.emFB			= EMUSE_SUMMON_FB_OK;
		NetMsg.emTYPE		= pSummon->m_emTYPE;
		NetMsg.dwGUID		= pSummon->m_dwGUID;
		NetMsg.sSummonID	= pSummon->m_sSummonID;
		NetMsg.dwOwner		= pSummon->m_dwOwner;	
		NetMsg.sMapID		= pSummon->m_sMapID;
		NetMsg.dwCellID		= pSummon->m_dwCellID;
		NetMsg.vPos			= pSummon->m_vPos;
		NetMsg.vDir			= pSummon->m_vDir;
		NetMsg.dwNowHP		= pSummon->m_dwNowHP;
		NetMsg.wNowMP		= pSummon->m_wNowMP;
		NetMsg.sPosionID	= pSummon->m_sPosionID;

		NetMsg.dwSummonID	= pSummon->m_dwSummonID;

		

		SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsg );

		// 주변에 알림
		GLMSG::SNET_SUMMON_CREATE_ANYSUMMON NetMsgBrd;
		NetMsgBrd.Data = pSummon->ReqNetMsg_Drop ();
		pSummon->m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBrd );
	}

	return pSummon;	
}

BOOL GLGaeaServer::DropSummon ( PGLSUMMONFIELD pSummon, SNATIVEID sMapID )
{
	if ( !pSummon ) return FALSE;

	GLLandMan* pLandMan = GetByMapID ( sMapID );
	if ( !pLandMan ) return FALSE;

	// 글로벌에 등록
	m_SummonArray[pSummon->m_dwGUID] = pSummon;

	// 랜드에 등록
	pSummon->m_pLandMan  = pLandMan;
	pSummon->m_pLandNode = pLandMan->m_GlobSummonList.ADDHEAD ( pSummon );
	pLandMan->RegistSummon ( pSummon );

	return TRUE;
}

// 이 함수는 주인이 맵을 이동하는 모든 행위에 대해서
// 호출된다. (주인이 게임을 종료하는 경우에도 호출됨)
// 따라서 소환여부를 고려해야 한다.
BOOL GLGaeaServer::DropOutSummon ( DWORD dwGUID, bool bLeaveFieldServer )
{
	if ( dwGUID>=m_dwMaxClient ) 
	{
		CDebugSet::ToLogFile ( "ERROR : dwGUID>=m_dwMaxClient SummonGUID : %d dwMaxClient : %d", dwGUID, m_dwMaxClient );
		return FALSE;
	}
	if ( m_SummonArray[dwGUID] == NULL ) 
	{
		CDebugSet::ToLogFile ( "ERROR : m_SummonArray[dwGUID] == NULL" );
		return FALSE;
	}

	PGLSUMMONFIELD pSummon = m_SummonArray[dwGUID];

	// 활동 여부
	bool bValid = pSummon->IsValid ();

	// 소환이 안된상태로 서버를 떠나지 않는다면
	if ( !bValid && !bLeaveFieldServer )
	{
		CDebugSet::ToLogFile ( "ERROR : !bValid && !bLeaveFieldServer bValid %d bLeaveFieldServer %d", bValid, bLeaveFieldServer );
		return FALSE;
	}

	DWORD dwSummonGUID = pSummon->m_dwGUID;
	DWORD dwOwnerID	   = pSummon->m_dwOwner;

	// 활동중이면
	if ( bValid )
	{
		//	Note : Land 리스트에서 제거.
		GLLandMan* pLandMan = pSummon->m_pLandMan;
		if ( pLandMan )
		{
			pLandMan->RemoveSummon ( pSummon );
			pSummon->m_pLandMan = NULL;
		}

		pSummon->ReSetValid ();
		pSummon->ReSetAllSTATE ();
//		pSummon->ReSetSkillDelay ();
	}

	// 클라이언트 팻 사라지게 메시지 발송 (PC가 게임을 종료하면 pOwner 없을 수 있다)
	PGLCHAR pOwner = GetChar ( dwOwnerID );
	if ( pOwner && bValid )
	{
		// 스킬의 보조능력치 제거
		/*pOwner->m_sPETSKILLFACT.RESET ();
		pOwner->m_bProtectPutOnItem = false;*/

		GLMSG::SNETPC_REQ_USE_SUMMON_DEL NetMsgFB;
		NetMsgFB.dwGUID	= dwSummonGUID;
		SENDTOCLIENT ( pOwner->m_dwClientID, &NetMsgFB );
	}

	// 현재 필드서버를 떠나거나 게임을 완전 종료하면
	if ( bLeaveFieldServer )
	{
		// 글로벌 리스트에서 제거, GUID 반환
		RELEASE_SUMMON ( pSummon );
		m_SummonArray[dwGUID] = NULL;
		m_FreeSummonGIDs.AddTail ( dwGUID );
	}

	return TRUE;
}

BOOL GLGaeaServer::SaveNpcCommission( DWORD dwCharID, DWORD dwUserID, LONGLONG lnCommission )
{
	if ( lnCommission <= 0 )	return FALSE;

	GLChar* pChar = GLGaeaServer::GetInstance().GetCharID( dwCharID );

	//  캐릭터가 같은 필드에 있을경우 
	if ( pChar ) 
	{
		pChar->UpdateNpcCommission( lnCommission );
	}
	else
	{
		GLMSG::SNET_INVEN_NPC_COMMISSION NetMsgAgt;	
		NetMsgAgt.nCHANNEL = GLGaeaServer::GetInstance().GetServerChannel();
		NetMsgAgt.dwFieldID = GLGaeaServer::GetInstance().GetFieldSvrID();
		NetMsgAgt.dwCharID = dwCharID;
		NetMsgAgt.dwUserID = dwUserID;
		NetMsgAgt.lnCommission = lnCommission;
		GLGaeaServer::GetInstance().SENDTOAGENT( &NetMsgAgt );
	}

	return TRUE;
}

BOOL GLGaeaServer::SaveNpcCommissionDB( DWORD dwCharID, DWORD dwUserID, LONGLONG lnCommission )
{
	if ( lnCommission <= 0 )	return FALSE;

	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, dwCharID, ID_USER, dwUserID, lnCommission, EMITEM_ROUTE_NPCCOME );

	CUpdateUserMoneyAdd* pDbAction = new CUpdateUserMoneyAdd ( dwUserID, lnCommission );
	m_pDBMan->AddJob ( pDbAction );

//	UserLastInfo 업데이트
#if defined ( TW_PARAM ) || defined ( HK_PARAM ) || defined ( _RELEASED )	
	
	CUpdateUserLastInfoAdd* pDbActionLast = new CUpdateUserLastInfoAdd ( dwUserID, lnCommission );
	m_pDBMan->AddJob ( pDbActionLast );

#endif 


	return TRUE;
}

BOOL GLGaeaServer::ReqClubDeathMatchRanking ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_CLUB_DEATHMATCH_RANKING_REQ* pNetMsg )
{
//	GLMSG::SNETPC_REQ_USE_SUMMON_FB NetMsg;

	// 요청 케릭터가 유효성 체크
	PGLCHAR pChar = GetChar ( dwGaeaID );
	if ( !pChar ) return FALSE;	
	
	if ( !pChar->m_pLandMan ) return FALSE;
	if ( pChar->m_dwGuild == CLUB_NULL )	return FALSE;
	if ( pChar->m_pLandMan->GetMapID().dwID != pNetMsg->dwMapID ) return FALSE;
	if ( !pChar->m_pLandMan->m_bClubDeathMatchMap ) return FALSE;

	GLClubDeathMatch* pCDM = GLClubDeathMatchFieldMan::GetInstance().Find( pChar->m_pLandMan->m_dwClubMapID );
	if ( !pCDM ) return FALSE;
	if ( !pCDM->IsBattle() ) return FALSE;

	
	
	CDM_RANK_INFO_MAP_ITER pos = pCDM->m_mapCdmScore.find( pChar->m_dwGuild );
	if( pos != pCDM->m_mapCdmScore.end() ) 
	{
		GLMSG::SNET_CLUB_DEATHMATCH_MYRANK_UPDATE NetMsgMy;

		NetMsgMy.sMyCdmRank.wClubRanking = pos->second.wClubRanking;
		NetMsgMy.sMyCdmRank.wKillNum = pos->second.wKillNum;
		NetMsgMy.sMyCdmRank.wDeathNum = pos->second.wDeathNum;
		
		SENDTOCLIENT( dwClientID, &NetMsgMy );
	}


	GLMSG::SNET_CLUB_DEATHMATCH_RANKING_UPDATE NetMsg;
	
	CDM_RANK_INFO_MAP_ITER pos_begin = pCDM->m_mapCdmScore.begin();
	CDM_RANK_INFO_MAP_ITER pos_end = pCDM->m_mapCdmScore.end();

	for ( ; pos_begin != pos_end; pos_begin++ )
	{
		SCDM_RANK_INFO& sCdmRankInfo = pos_begin->second;
		int nIndex = sCdmRankInfo.nIndex;

		if ( nIndex >= 0 && nIndex < RANKING_NUM )
		{
			SCDM_RANK sCdmRank = sCdmRankInfo;
			NetMsg.ADDCLUB( sCdmRank );
		}
	}	

	if ( NetMsg.wRankNum > 0 )	SENDTOCLIENT( dwClientID, &NetMsg );

	return TRUE;
}
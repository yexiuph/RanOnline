#include "pch.h"
#include "./GLAgentServer.h"
#include "./GLITEMLMT.h"
#include "./GLSchoolFreePK.h"

#include "./DbActionLogic.h"

#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "GLClubDeathMatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	���� ����� dbman ������ db�� ������ ���� ����, �޸� �� ������.
//
class GLEmulatorDBMan : public GLDBMan
{
public:
	virtual int SaveCharacter ( LPVOID _pbuffer )		{ return 0; }
	virtual int	ReadUserInven ( SCHARDATA2* pChaData2 )	{ return 0; }

	//	�ƹ��͵� ���� �ʴ´�.
	virtual void AddJob ( CDbAction *pDbActJob )
	{
		SAFE_DELETE(pDbActJob);
	}

	//	�ƹ��͵� ���� �ʴ´�.
	virtual void AddLogJob ( CDbAction *pDbActJob )
	{
		SAFE_DELETE(pDbActJob);
	}

	//	�ƹ��͵� ���� �ʴ´�.
	virtual void AddUserJob ( CDbAction *pDbActJob )
	{
		SAFE_DELETE(pDbActJob);
	}
};

GLEmulatorDBMan g_EmulatorDBMan;


GLAgentServer& GLAgentServer::GetInstance()
{
	static GLAgentServer Instance;
	return Instance;
}

GLAgentServer::GLAgentServer(void) :
	m_pMsgServer(NULL),
	m_pConsoleMsg(NULL),
	m_pDBMan(NULL),

	m_dwMaxClient(0),
	m_PCArray(NULL),

	m_fPERIOD_TIMER(0),
	m_dwFIELDSVR_NUM(0)
{

	m_WorkEvent.Init();
	m_sEventState.Init();

	for ( int c=0; c<MAX_CHANNEL_NUMBER; ++c )
	{
		for ( int i=0; i<FIELDSERVER_MAX; ++i )		m_bFIELDSVR[c][i] = false;
	}

	InitializeCriticalSection(&m_CSPCLock);
}

GLAgentServer::~GLAgentServer(void)
{
	SAFE_DELETE_ARRAY(m_PCArray);

	DeleteCriticalSection(&m_CSPCLock);
}

DWORD GLAgentServer::GetFieldServer ( SNATIVEID &sMapID )
{
	SMAPNODE* pMapNode = m_sMapList.FindMapNode ( sMapID );
	if ( !pMapNode )	return FIELDSERVER_MAX;

	return pMapNode->dwFieldSID;
}

PGLCHARAG GLAgentServer::GetChar ( const char* szName )
{
	CHAR_MAP_ITER name_iter = m_PCNameMap.find(szName);
	if ( name_iter == m_PCNameMap.end() )	return NULL;

	return (*name_iter).second;
}

// *****************************************************
// Desc: �������� ����� �������� PGLCHARAG ��ȯ
// *****************************************************
PGLCHARAG GLAgentServer::GetCharUA ( const char* szUAccount )
{
	CHAR_MAP_ITER name_iter = m_UAccountMap.find(szUAccount);
	if ( name_iter == m_UAccountMap.end() )	return NULL;

	return (*name_iter).second;
}

HRESULT GLAgentServer::Create ( DWORD dwMaxClient, DxMsgServer *pMsgServer, DxConsoleMsg *pConsoleMsg, GLDBMan *pDBMan, char* szMapList, F_SERVER_INFO* pFieldInfo )
{
	HRESULT hr;
	GASSERT(pMsgServer);
	GASSERT(pConsoleMsg);
	GASSERT(pDBMan);

	m_PresetTime = CTime::GetCurrentTime();
	    

	F_SERVER_INFO _sFIELDServer[FIELDSERVER_MAX];
	if ( !pFieldInfo )	pFieldInfo = _sFIELDServer;

	m_dwMaxClient = dwMaxClient;
	m_pMsgServer = pMsgServer;
	m_pConsoleMsg = pConsoleMsg;
	m_pDBMan = pDBMan;

	if ( !m_pDBMan )	m_pDBMan = &g_EmulatorDBMan;

	SAFE_DELETE_ARRAY(m_PCArray);
	m_PCArray = new PGLCHARAG[m_dwMaxClient];
	SecureZeroMemory ( m_PCArray, sizeof(PGLCHARAG)*m_dwMaxClient );

	if ( !szMapList )
	{
		hr = m_sMapList.LoadMapList ( "mapslist.ini", pFieldInfo, FIELDSERVER_MAX );
		if ( FAILED(hr) )	return E_FAIL;
	}
	else
	{
		hr = m_sMapList.LoadMapList ( szMapList, pFieldInfo, FIELDSERVER_MAX );
		if ( FAILED(hr) )	return E_FAIL;
	}
	
	GLITEMLMT::GetInstance().SetDBMan ( pDBMan );
	GLITEMLMT::GetInstance().SetServer ( pMsgServer, UINT_MAX );
	GLITEMLMT::GetInstance().ReadMaxKey ();

	for ( DWORD i=0; i<m_dwMaxClient; ++i )	m_FreePCGIDs.push_back ( i );
	SecureZeroMemory ( m_pLandMan, sizeof(GLAGLandMan*)*MAXLANDMID*MAXLANDSID );

	GLMapList::FIELDMAP map_list = m_sMapList.GetMapList();
	GLMapList::FIELDMAP_ITER iter = map_list.begin ();
	GLMapList::FIELDMAP_ITER iter_end = map_list.end ();
	for ( ; iter!=iter_end; ++iter )
	{
		const SMAPNODE *pMapNode = &(*iter).second;

		//	Note : LandMan ������ �ʱ�ȭ.
		//
		GLAGLandMan *pNewLandMan = new GLAGLandMan;
		pNewLandMan->SetMapID ( pMapNode->sNativeID, pMapNode->bPeaceZone!=FALSE, pMapNode->bPKZone==TRUE );

		BOOL bOk = pNewLandMan->LoadFile ( pMapNode->strFile.c_str() );
		if ( !bOk )
		{
			CString strTemp = pMapNode->strFile.c_str();
			strTemp += " File Load Fail.";
			MessageBox ( NULL, strTemp, "ERROR", MB_OK );

			SAFE_DELETE(pNewLandMan);
			continue;
		}

		InsertMap ( pNewLandMan );
	}


	m_cPartyMan.Create ( m_dwMaxClient );

	//	Note : �ɸ��� �ʱ� ���� �� �� �ʱ� ���� Gate�� ���������� �����ϴ��� ����.
	//
	for ( WORD i=0; i<GLCONST_CHAR::wSCHOOLNUM; ++i )
	{
		SNATIVEID nidSTARTMAP = GLCONST_CHAR::nidSTARTMAP[i];
		DWORD dwSTARTGATE = GLCONST_CHAR::dwSTARTGATE[i];

		GLAGLandMan* pLandMan = GetByMapID ( nidSTARTMAP );
		if ( !pLandMan )
		{
			//std::strstream strStream;
			//strStream << "Charactor Start Map Error." << std::endl;
			//strStream << "'mapslist.ini' [" << nidSTARTMAP.wMainID << "," << nidSTARTMAP.wSubID << "]";
			//strStream << " MapID ( ID:[M,S] ) not found." << std::ends;

			TCHAR szTemp[128] = {0};
			_snprintf_s( szTemp, 128, "Charactor Start Map Error.\n"
									"'mapslist.ini' [%u,%u] MapID ( ID:[M,S] ) not found.",
									nidSTARTMAP.wMainID,
									nidSTARTMAP.wSubID );

			MessageBox ( NULL, szTemp, "ERROR", MB_OK|MB_ICONEXCLAMATION );

			//strStream.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
			continue;
		}

		DxLandGateMan* pGateMan = pLandMan->GetLandGateMan ();
		PDXLANDGATE pGate = pGateMan->FindLandGate ( dwSTARTGATE );
		if ( !pGate )
		{
			//std::strstream strStream;
			//strStream << "Charactor Start Map GATE not found." << std::endl;
			//strStream << pLandMan->GetFileName() << " map "  << dwSTARTGATE;
			//strStream << " GATEID must check." << std::ends;

			TCHAR szTemp[128] = {0};
			_snprintf_s( szTemp, 128, "Charactor Start Map GATE not found.\n"
									"%s map %u GATEID must check.",
									pLandMan->GetFileName(),
									dwSTARTGATE );

			MessageBox ( NULL, szTemp, "ERROR", MB_OK );

			//strStream.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
			continue;
		}
	}

	//	Note : �ʵ� ���� ��ȿ�� �˻�.
	//
	m_dwFIELDSVR_NUM = 0;

	for ( int c=0; c<MAX_CHANNEL_NUMBER; ++c )
	{
		for ( int i=0; i<FIELDSERVER_MAX; ++i )		m_bFIELDSVR[c][i] = false;
	}
	
	for ( int c=0; c<MAX_CHANNEL_NUMBER; ++c )
	{
		for ( int i=0; i<FIELDSERVER_MAX; ++i )
		{
			if ( pFieldInfo[c*MAX_CHANNEL_NUMBER+i].IsValid() )
			{
				++m_dwFIELDSVR_NUM;
				m_bFIELDSVR[c][i] = true;
			}
		}
	}

	//	Note : Ŭ���� ������ db���� �о�´�.
	//
	m_cClubMan.LoadFromDB ( pDBMan, false );

	VECGUID_DB vecGuidDb;
	m_pDBMan->GetClubRegion ( vecGuidDb );
	GLGuidanceAgentMan::GetInstance().SetState ( vecGuidDb );
	GLGuidanceAgentMan::GetInstance().SetMapState();
	GLClubDeathMatchAgentMan::GetInstance().SetMapState();

	// �̺�Ʈ�� ���������� ���
	m_cFreePK.RESET();

	// ���� ����� �̺�Ʈ�� �ʱ�ȭ
	m_WorkEvent.Init();

	return S_OK;
}


HRESULT GLAgentServer::CreateInstantMap( SNATIVEID sDestMapID, SNATIVEID sInstantMapID, DWORD dwGaeaID, DWORD dwPartyID )
{
	int iMapNum = sInstantMapID.wSubID-1;

//	if( !m_pInstantMapLandMan[iMapNum] ) return E_FAIL;
	GLAGLandMan	*pSrcLandMan = new GLAGLandMan;
	SMAPNODE	*pMapNode	  = m_sMapList.FindMapNode ( sDestMapID );
	GLAGLandMan *pDestLandMan = GetByMapID ( sDestMapID );

	if ( !pMapNode ) return E_FAIL;
	if( !pDestLandMan ) return E_FAIL;

	m_vecInstantMapId.push_back( sInstantMapID.dwID );

	pSrcLandMan->SetMapID ( sInstantMapID, pMapNode->bPeaceZone!=FALSE, pMapNode->bPKZone==TRUE );
	pSrcLandMan->SetInstantMap ( TRUE, dwGaeaID, dwPartyID );
	*pSrcLandMan = *pDestLandMan;
	
	

	InsertMap ( pSrcLandMan );



	return S_OK;

}

HRESULT GLAgentServer::InsertMap ( GLAGLandMan* pNewLandMan )
{
	const SNATIVEID &sMapID = pNewLandMan->GetMapID ();

	GASSERT(sMapID.wMainID<MAXLANDMID);
	GASSERT(sMapID.wSubID<MAXLANDSID);

	SAFE_DELETE ( m_pLandMan[sMapID.wMainID][sMapID.wSubID] );

	m_pLandMan[sMapID.wMainID][sMapID.wSubID] = pNewLandMan;

	return S_OK;
}

HRESULT GLAgentServer::CleanUp ()
{
	m_FreePCGIDs.clear ();
	m_sMapList.CleanUp ();



	for ( WORD i=0; i<MAXLANDMID; ++i )
	for ( WORD j=0; j<MAXLANDSID; ++j )
	{
		SAFE_DELETE ( m_pLandMan[i][j] );
	}

	
	

	return S_OK;
}

PGLCHARAG GLAgentServer::CreatePC ( GLCHARAG_DATA *pchar_data, DWORD dwClientID, int nChannel, int dwThaiCCafeClass, SChinaTime chinaTime, int nMyCCafeClass )
{
	HRESULT hr = S_OK;
	if ( !pchar_data )	return NULL;

	PGLCHARAG pPChar = NULL;
	CHAR_MAP_ITER name_iter;
	CLIENTMAP_ITER client_iter;
	CLIENTMAP_ITER usernum_iter;
	GLMSG::SNETPC_FIELDSVR_CHARCHK NetMsg;	

	//	Note : ���� IP�� �̹� ���ӵǾ� �ִ��� �˻��մϴ�.
	//name_iter = m_PCIpMap.find(pchar_data->m_szIp);
	//if ( name_iter != m_PCIpMap.end() )
	//{
	//	PGLCHARAG pChar = (*name_iter).second;

	//	//	�����ڿ��� DropOut �޽����� �����ϴ�.
	//	GLMSG::SNET_DROP_OUT_FORCED NetMsgForced;
	//	NetMsgForced.dwCharID = pChar->m_dwCharID;
	//	NetMsgForced.emForced = EMDROPOUT_REQLOGIN;
	//	SENDTOCLIENT ( dwClientID, &NetMsgForced );

	//	//	��� �ʵ忡 ���� ���� �޼���.
	//	SENDTOALLCHANNEL ( &NetMsgForced );

	//	//	���� �����ڿ��� DropOut �޽����� �����ϴ�.
	//	SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgForced );

	//	//	���� �����ڸ� DropOut ��ŵ�ϴ�.
	//	DropOutPC ( pChar->m_dwGaeaID );

	//	m_pConsoleMsg->Write( _T("ERROR:m_PCIpMap failed") );

	//	return NULL;
	//}

	//	Note : ���� ĳ���Ͱ� �̹� ���ӵǾ� �ִ��� �˻��մϴ�.
	name_iter = m_PCNameMap.find(pchar_data->m_szName);
	if ( name_iter != m_PCNameMap.end() )
	{
		PGLCHARAG pChar = (*name_iter).second;

		//	�����ڿ��� DropOut �޽����� �����ϴ�.
		GLMSG::SNET_DROP_OUT_FORCED NetMsgForced;
		NetMsgForced.dwCharID = pChar->m_dwCharID;
		NetMsgForced.emForced = EMDROPOUT_REQLOGIN;
		SENDTOCLIENT ( dwClientID, &NetMsgForced );
		
		//	��� �ʵ忡 ���� ���� �޼���.
		SENDTOALLCHANNEL ( &NetMsgForced );

		//	���� �����ڿ��� DropOut �޽����� �����ϴ�.
		SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgForced );

		//	���� �����ڸ� DropOut ��ŵ�ϴ�.
		DropOutPC ( pChar->m_dwGaeaID );

		m_pConsoleMsg->Write( _T("ERROR:m_PCNameMap failed") );

		return NULL;
	}



	name_iter = m_UAccountMap.find(pchar_data->m_szUserName);
	if ( name_iter != m_UAccountMap.end() )
	{
		PGLCHARAG pChar = (*name_iter).second;

		//	�����ڿ��� DropOut �޽����� �����ϴ�.
		GLMSG::SNET_DROP_OUT_FORCED NetMsgForced;
		NetMsgForced.dwCharID = pChar->m_dwCharID;
		NetMsgForced.emForced = EMDROPOUT_REQLOGIN;
		SENDTOCLIENT ( dwClientID, &NetMsgForced );

		//	��� �ʵ忡 ���� ���� �޼���.
		SENDTOALLCHANNEL ( &NetMsgForced );

		//	���� �����ڸ� DropOut ��ŵ�ϴ�.
		DropOutPC ( pChar->m_dwGaeaID );

		//	���� �����ڿ��� DropOut �޽����� �����ϴ�.
		SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgForced );

		m_pConsoleMsg->Write( _T("ERROR:m_UAccountMap failed") );

		return NULL;
	}

	usernum_iter = m_UserNumberMap.find(pchar_data->m_dwUserID);
	if ( usernum_iter != m_UserNumberMap.end() )
	{
		PGLCHARAG pChar = GetChar((*usernum_iter).second);

		//	�����ڿ��� DropOut �޽����� �����ϴ�.
		GLMSG::SNET_DROP_OUT_FORCED NetMsgForced;
		NetMsgForced.dwCharID = pChar->m_dwCharID;
		NetMsgForced.emForced = EMDROPOUT_REQLOGIN;
		SENDTOCLIENT ( dwClientID, &NetMsgForced );

		//	��� �ʵ忡 ���� ���� �޼���.
		SENDTOALLCHANNEL ( &NetMsgForced );

		//	���� �����ڿ��� DropOut �޽����� �����ϴ�.
		SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgForced );

		//	���� �����ڸ� DropOut ��ŵ�ϴ�.
		DropOutPC ( pChar->m_dwGaeaID );


		m_pConsoleMsg->Write( _T("ERROR:m_UserNumberMap failed") );

		return NULL;
	}


	client_iter = m_PCClientIDMAP.find(dwClientID);
	if ( client_iter != m_PCClientIDMAP.end() )
	{
		//	�����ڿ��� DropOut �޽����� �����ϴ�.
		GLMSG::SNET_DROP_OUT_FORCED NetMsgForced;
		NetMsgForced.emForced = EMDROPOUT_REQLOGIN;
		SENDTOCLIENT ( dwClientID, &NetMsgForced );

		//	���� �����ڸ� DropOut ��ŵ�ϴ�.
		DWORD dwGaeaID = (*client_iter).second;
		DropOutPC ( dwGaeaID );

		m_pConsoleMsg->Write( _T("ERROR:m_PCClientIDMAP failed") );

		return NULL;
	}

	//	Note �ɸ��� ���̾� ID ����.
	//
	if ( m_FreePCGIDs.empty() )		goto _ERROR;
	DWORD dwGaeaID = m_FreePCGIDs.front();
	m_FreePCGIDs.pop_front ();

	//	Note : ĳ���� �ʱ�ȭ.
	//
	pPChar = new GLCharAG;

	pPChar->m_dwClientID = dwClientID;
	pPChar->m_nChannel = nChannel;
	pPChar->m_dwGaeaID = dwGaeaID;
	strcpy_s( pPChar->m_szIp, pchar_data->m_szIp );

	// �±� ���̹� ī�� ���
	pPChar->m_dwThaiCCafeClass	= dwThaiCCafeClass;
	// �����̽þ� PC�� �̺�Ʈ ���
	pPChar->m_nMyCCafeClass		= nMyCCafeClass;

	// �߱� �ð�
	pPChar->m_sChinaTime       = chinaTime;
	// �̺�Ʈ �ð� //** Add EventTime
	pPChar->m_sEventTime.loginTime		 = chinaTime.loginTime;
	pPChar->m_sEventTime.currentGameTime = 0;
	// ��Ʈ�� Ž�� ���� �ý��� �ð�
	pPChar->m_sVietnamSystem			 = pchar_data->m_sVietnamSystem;
	// ���� �������� �������� �ƴ���
	pPChar->m_bTracingUser				 = pchar_data->m_bTracingUser;
	// �α��� �ð�
	pPChar->m_tLoginTime				 = CTime::GetCurrentTime().GetTime();

	//	Note : ĳ���� ���.
	//
	hr = pPChar->CreateChar ( pchar_data );
	if ( FAILED(hr) )
	{
		DEBUGMSG_WRITE ( "pPChar->CreateChar () ȣ���� ������ ���Ͽ� ĳ���� ������ �����Ͽ����ϴ�. [%s]", pchar_data->m_szName );
		goto _ERROR;
	}

	//	Note : ���̾� Array�� ���.
	//
	m_PCArray[pPChar->m_dwGaeaID] = pPChar;
	
	//	Note : Ȱ�� ����Ʈ�� ���.
	//
	pPChar->m_pPCNode = m_PCList.ADDTAIL ( pPChar );

	//	Note : PC NAME map �� ���.
	GASSERT ( m_PCNameMap.find(pPChar->m_szName)==m_PCNameMap.end() );
	m_PCNameMap [ std::string(pPChar->m_szName) ] = pPChar;

	//	Note : UserAcount map �� ���.
	GASSERT ( m_UAccountMap.find(pPChar->m_szUserName)==m_UAccountMap.end() );
	m_UAccountMap [ std::string(pPChar->m_szUserName) ] = pPChar;

	//	Note : PC Client map �� ���.
	GASSERT ( m_PCClientIDMAP.find(pPChar->m_dwClientID)==m_PCClientIDMAP.end() );
	m_PCClientIDMAP[pPChar->m_dwClientID] = pPChar->m_dwGaeaID;

	//	Note : PC IP map �� ���.
//	GASSERT ( m_PCIpMap.find(pPChar->m_szIp)==m_PCIpMap.end() );
//	m_PCIpMap[ std::string(pPChar->m_szIp) ] = pPChar;

	//	Note : USER Number map �� ���.
	GASSERT ( m_UserNumberMap.find(pPChar->m_dwUserID)==m_UserNumberMap.end() );
	m_UserNumberMap[pPChar->m_dwUserID] = pPChar->m_dwGaeaID;

	//	Note : CID map �� ���.
	GASSERT ( m_mapCharID.find(pPChar->m_dwCharID)==m_mapCharID.end() );
	m_mapCharID[pPChar->m_dwCharID] = pPChar->m_dwGaeaID;

	//	Note : LAND ����Ʈ���� ����.
	GLAGLandMan* pNEW_LAND = GLAgentServer::GetInstance().GetByMapID ( pPChar->m_sCurMapID );
	if ( pNEW_LAND )	pNEW_LAND->DropPC ( pPChar->m_dwGaeaID );

	//	Note : �ɸ��� �ʵ� ������ ���� ���� �޼���.
	//
	NetMsg.dwClientID = pPChar->m_dwClientID;
	NetMsg.dwGaeaID = pPChar->m_dwGaeaID;
	
	NetMsg.dwUserID = pPChar->m_dwUserID;
	NetMsg.dwCharID = pPChar->m_dwCharID;
	StringCchCopy ( NetMsg.szName, CHAR_SZNAME, pPChar->m_szName );
	SENDTOALLCHANNEL ( (NET_MSG_GENERIC*) &NetMsg );

	return pPChar;	//	�ɸ��� ���� ����.

_ERROR:
	SAFE_DELETE(pPChar);

	//	���� �õ��ڿ���  �޽����� �����ϴ�.
	GLMSG::SNETLOBBY_CHARJOIN_FB NetMsgFB;
	NetMsgFB.emCharJoinFB = EMCJOIN_FB_ERROR;
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	m_pConsoleMsg->Write( _T("ERROR:SNETLOBBY_CHARJOIN_FB failed") );

	return NULL;	//	�ɸ��� ���� ����.
}

HRESULT GLAgentServer::DropOutPC ( DWORD dwGaeaID )
{
	PGLCHARAG pPChar = GetChar(dwGaeaID);
	if ( !pPChar )	return E_FAIL;

	//	Note : Ȱ�� ����Ʈ���� ����.
	//
	if ( pPChar->m_pPCNode )
		m_PCList.DELNODE ( pPChar->m_pPCNode );

	//	Note : ���̾� ID ��ȯ.
	//
	m_FreePCGIDs.push_back ( dwGaeaID );
	m_PCArray[dwGaeaID] = NULL;

	//	Note : �ɸ��� �̸� ����.
	//
	{
		CHAR_MAP_ITER iter = m_PCNameMap.find(pPChar->m_szName);
		if ( iter!=m_PCNameMap.end() )			m_PCNameMap.erase(iter);
	}

	//	Note : ĳ���� IP ����.
	//
	{
//		CHAR_MAP_ITER iter = m_PCIpMap.find(pPChar->m_szIp);
//		if ( iter!=m_PCIpMap.end() )			m_PCIpMap.erase(iter);
	}

	//	Note : User Account ����.
	//
	{
		CHAR_MAP_ITER iter = m_UAccountMap.find(pPChar->m_szUserName);
		if ( iter!=m_UAccountMap.end() )		m_UAccountMap.erase(iter);
	}

	//	Note : PC Client map ���� ����.
	{
		CLIENTMAP_ITER iter = m_PCClientIDMAP.find(pPChar->m_dwClientID);
		if ( iter!=m_PCClientIDMAP.end() )		m_PCClientIDMAP.erase(iter);
	}

	//	Note : PC UserNumber map ���� ����.
	{
		CLIENTMAP_ITER iter = m_UserNumberMap.find(pPChar->m_dwUserID);
		if ( iter!=m_UserNumberMap.end() )		m_UserNumberMap.erase(iter);
	}

	//	Note : CID map ���� ����.
	{
		CLIENTMAP_ITER iter = m_mapCharID.find(pPChar->m_dwCharID);
		if ( iter!=m_mapCharID.end() )		m_mapCharID.erase(iter);
	}

	//	Note : LAND ����Ʈ���� ����.
	GLAGLandMan* pOLD_LAND = GLAgentServer::GetInstance().GetByMapID ( pPChar->m_sCurMapID );
	if ( pOLD_LAND )	pOLD_LAND->DropOutPC ( pPChar->m_dwGaeaID );
/*
	//	�����ڿ��� DropOut �޽����� �����ϴ�.
	GLMSG::SNET_DROP_OUT_FORCED NetMsgForced;
	NetMsgForced.emForced = EMDROPOUT_REQLOGIN;
	SendField ( dwGaeaID, &NetMsgForced );
*/
	//	Note : �޸� ����.
	//
	SAFE_DELETE(pPChar);

	return S_OK;
}

BOOL GLAgentServer::ReserveDropOutPC ( DWORD dwGaeaID )
{
	EnterCriticalSection(&m_CSPCLock);
	{
		m_RegDropOutPC.push_back ( dwGaeaID );
	}
	LeaveCriticalSection(&m_CSPCLock);

	return TRUE;
}

BOOL GLAgentServer::ClearReservedDropOutPC ()
{
	EnterCriticalSection(&m_CSPCLock);
	{
		PCGID_ITER iter = m_RegDropOutPC.begin ();
		PCGID_ITER iter_end = m_RegDropOutPC.end ();
		for ( ; iter!=iter_end; ++iter )
		{
			DWORD dwGaeaID = (*iter);

			PGLCHARAG pChar = GetChar(dwGaeaID);
			if ( !pChar )
			{
				//DEBUGMSG_WRITE ( "ĳ���͸� ���ӿ��� DropOut �õ��� GID %d �� ã�� ���߽��ϴ�.", dwGaeaID );
				continue;
			}

			//	GLAgentServer DropOut Char.
			DropOutPC ( dwGaeaID );
		}

		m_RegDropOutPC.clear ();
	}
	LeaveCriticalSection(&m_CSPCLock);

	return TRUE;
}

HRESULT GLAgentServer::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : ĳ���� ����.
	//
	for ( DWORD i=0; i<m_dwMaxClient; ++i )
	{
		PGLCHARAG pChar = m_PCArray[i];
		if ( pChar )
		{
			pChar->FrameMove ( fTime, fElapsedTime );
		}
	}

	// ������ ������ �ʴ� ���������̸�� ( GM ������� ���� )
	m_cFreePK.FrameMove( fElapsedTime );

	//	Note : ���� ����Ʈ�� ��ϵ� �ɸ��� ������Ʈ �������ֱ�.
	//
	ClearReservedDropOutPC ();

	//	Note : ��Ƽ ������Ʈ.
	//
	m_cPartyMan.FrameMove ( fTime, fElapsedTime );

	//	Note : �ð� ����� ��ũ�� ���߱� ���� ������Ʈ �Լ�.
	//
	m_fPERIOD_TIMER += fElapsedTime;
	if ( m_fPERIOD_TIMER > 60.0f )
	{
		m_fPERIOD_TIMER = 0.0f;
		
		GLMSG::SNET_PERIOD NetPeriod;
		NetPeriod.sPRERIODTIME = GLPeriod::GetInstance().GetPeriod();
		SENDTOALLCHANNEL ( &NetPeriod );
	}

	// Ŭ�� ����
	m_cClubMan.FrameMoveAgent( fTime, fElapsedTime );

	if( m_sEventState.bEventStart && m_sEventState.dwEventEndMinute != 0 )
	{
		CTimeSpan timeSpan = CTimeSpan( 0, 0, m_sEventState.dwEventEndMinute, 0 );
		CTime endTime = m_sEventState.EventStartTime + timeSpan;
		CTime currentTime = CTime::GetCurrentTime();

		if( endTime.GetMonth()  == currentTime.GetMonth() && 
			endTime.GetDay()    == currentTime.GetDay() && 
			endTime.GetHour()   == currentTime.GetHour() && 
			endTime.GetMinute() == currentTime.GetMinute() )
		{
			m_sEventState.bEventStart      = false;
			m_sEventState.dwEventEndMinute = 0;

			GLMSG::SNET_GM_LIMIT_EVENT_END nmg;
			SENDTOALLCHANNEL ( &nmg );
		}
	}

	//TestCreateInstantMap( fElapsedTime );

	//	Note : �п��� ���� pk �ý��� ����.
	//	
	GLSchoolFreePK::GetInstance().FrameMove ( fElapsedTime );

	return S_OK;
}

bool GLAgentServer::StartPartyConfront ( const  DWORD dwPartyA, const DWORD dwPartyB,
										const SNATIVEID &sMapID, const SCONFT_OPTION &sOption,
										const DWORD* pMBR_A, const DWORD* pMBR_B, const D3DXVECTOR3 vPos)
{
	GLPARTY *pPartyA = m_cPartyMan.GetParty(dwPartyA);
	GLPARTY *pPartyB = m_cPartyMan.GetParty(dwPartyB);
	if ( !pPartyA || !pPartyB )		return false;

	GLPARTY::MEMBER_ITER iter, iter_end;

	WORD wSCHOOL_A = pPartyA->GetMASTER_SCHOOL();
	WORD wSCHOOL_B = pPartyB->GetMASTER_SCHOOL();

	SCONFT_OPTION sOPT_A = sOption;
	SCONFT_OPTION sOPT_B = sOption;

	sOPT_A.wMY_SCHOOL = wSCHOOL_A;
	sOPT_A.wTAR_SCHOOL = wSCHOOL_B;

	sOPT_B.wMY_SCHOOL = wSCHOOL_B;
	sOPT_B.wTAR_SCHOOL = wSCHOOL_A;

	int i(0);

	//	Note : A �� '��Ƽ���' �ʱ�ȭ.
	//
	pPartyA->m_conftCURRENT.clear();
	pPartyA->m_conftSTART.clear();

	for ( i=0; i<MAXPARTY; ++i )
	{
		DWORD dwMBR = pMBR_A[i];
		if ( dwMBR == GAEAID_NULL )				continue;

		PGLCHARAG pChar = GetChar ( dwMBR );
		if ( !pChar )							continue;
		if ( pChar->m_sCurMapID != sMapID )		continue;

		//	Note : ��Ƽ���� ������� ����.
		//
		pChar->m_sCONFTING.RESET();
		pChar->m_sCONFTING.emTYPE = EMCONFT_PARTY;
		pChar->m_sCONFTING.dwTAR_ID = dwPartyB;
		pChar->m_sCONFTING.sOption = sOPT_A;

		pPartyA->ADDCONFT_MEMBER ( dwMBR );
	}

	if ( pPartyA->GETCONFT_NUM()==0 )			return false;
	
	//	Note : B �� '��Ƽ���' �ʱ�ȭ.
	//
	pPartyB->m_conftCURRENT.clear();
	pPartyB->m_conftSTART.clear();

	for ( i=0; i<MAXPARTY; ++i )
	{
		DWORD dwMBR = pMBR_B[i];
		if ( dwMBR == GAEAID_NULL )				continue;

		PGLCHARAG pChar = GetChar ( dwMBR );
		if ( !pChar )							continue;
		if ( pChar->m_sCurMapID != sMapID )		continue;

		//	Note : ��Ƽ���� ������� ����.
		//
		pChar->m_sCONFTING.RESET();
		pChar->m_sCONFTING.emTYPE = EMCONFT_PARTY;
		pChar->m_sCONFTING.dwTAR_ID = dwPartyA;
		pChar->m_sCONFTING.sOption = sOPT_B;

		pPartyB->ADDCONFT_MEMBER ( dwMBR );
	}

	if ( pPartyB->GETCONFT_NUM()==0 )		return false;

	//	Note : �б��� ��Ƽ ������� �˻�.
	//
	bool bSCHOOL(false);
	if ( sOption.bSCHOOL )
	{
		if ( wSCHOOL_A!=wSCHOOL_B )
		{
			bool bSAME_SCHOOL = pPartyA->IsSAME_SCHOOL() && pPartyB->IsSAME_SCHOOL();
			bool bCONFT_NUM = pPartyA->GETCONFT_NUM()>=GLCONST_CHAR::wCONFRONT_SCHOOL_LIMIT_NUM && pPartyB->GETCONFT_NUM()>=GLCONST_CHAR::wCONFRONT_SCHOOL_LIMIT_NUM;
			if ( bSAME_SCHOOL && bCONFT_NUM )
			{
				bSCHOOL = true;
			}
		}
	}
	sOPT_A.bSCHOOL = sOPT_B.bSCHOOL = bSCHOOL;

	//	Note : ��Ƽ��� ���� �ʱ�ȭ.
	//
	pPartyA->m_conftOPTION = sOPT_A;
	pPartyA->m_dwconftPARTYID = dwPartyB;
	pPartyA->m_fconftTIMER = 0.0f;
	pPartyA->SETCONFT_MEMBER();

	pPartyB->m_conftOPTION = sOPT_B;
	pPartyB->m_dwconftPARTYID = dwPartyA;
	pPartyB->m_fconftTIMER = 0.0f;
	pPartyB->SETCONFT_MEMBER();

	//	Note : �б��� ��Ƽ ����� ���۵Ǿ����ϴ�. ��� �÷��̾�� �޽��� ����.
	//
	if ( sOPT_A.bSCHOOL )
	{
		CString strTEXT;
		const char *szMAP_NAME = m_sMapList.GetMapName ( sMapID );
		if ( szMAP_NAME )
		{
			strTEXT.Format ( ID2SERVERTEXT("EMCONFRONT_START_PARTY"),
				szMAP_NAME, GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_A), GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_B) );
		}

		GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
		NetMsg.SETTEXT ( strTEXT.GetString() );
		
		SENDTOALLCLIENT ( &NetMsg );
	}

	//	Note : �ʵ忡 ��� �ʱ�ȭ �޽���.
	//		�޽����� ���۵� my, tar school �� ������ �ʴ´�.
	GLMSG::SNETPC_CONFRONTPTY_START2_FLD NetMsgFld;
	NetMsgFld.sMapID = sMapID;
	NetMsgFld.dwPARTY_A= dwPartyA;
	NetMsgFld.dwPARTY_B= dwPartyB;
	NetMsgFld.sOption = sOPT_A;
	NetMsgFld.vPos = vPos;

	int nindex(0);
	iter = pPartyA->m_conftCURRENT.begin();
	iter_end = pPartyA->m_conftCURRENT.end();
	for ( ; iter!=iter_end; ++iter )
	{
		NetMsgFld.dwPARTY_A_MEMBER[nindex++] = (*iter);
	}

	nindex = (0);
	iter = pPartyB->m_conftCURRENT.begin();
	iter_end = pPartyB->m_conftCURRENT.end();
	for ( ; iter!=iter_end; ++iter )
	{
		NetMsgFld.dwPARTY_B_MEMBER[nindex++] = (*iter);
	}

	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgFld );

	return true;
}

void GLAgentServer::ResetFieldInfo ()
{
	m_cPartyMan.DelPartyAll ();
}

void GLAgentServer::ChangeNameMap ( PGLCHARAG pChar, const char* pszOldName, const char* pszNewName )
{
	CHAR_MAP_ITER name_iter = m_PCNameMap.find ( pChar->m_szName );
	if ( name_iter != m_PCNameMap.end() )
	{
		m_PCNameMap.erase ( name_iter );
	}
	
	StringCchCopy ( pChar->m_szName, CHAR_SZNAME, pszNewName );

	m_PCNameMap.insert ( std::make_pair ( pChar->m_szName, pChar ) );	
}

void GLAgentServer::ChangeNameMap ( PGLCHARAG pChar, const TCHAR* pszPhoneNumber )
{
	CHAR_MAP_ITER name_iter = m_PCNameMap.find( pChar->m_szName );
	if ( name_iter != m_PCNameMap.end() )
	{
		StringCchCopy( name_iter->second->m_szPhoneNumber, SMS_RECEIVER, pszPhoneNumber );
	}
}

// *****************************************************
// Desc: ���� �ʵ弭�� ���� ���� ���� 
// *****************************************************
void GLAgentServer::GameJoinToFieldSvr ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_SAVECHARPOSRST_FROMDB* pNetMsg = (GLMSG::SNETPC_SAVECHARPOSRST_FROMDB*)nmg; // (DB->Agent)

	PGLCHARAG pChar = GetCharID ( pNetMsg->dwCharID );
	if ( !pChar )	return;

	if ( m_pMsgServer->ConnectFieldSvr ( pChar->m_dwClientID, pNetMsg->dwFieldServer, pChar->m_dwGaeaID, pChar->m_nChannel ) != NET_OK )
	{
        //	ĳ���Ͱ� ������ �ʵ弭���� ������ ������
		DEBUGMSG_WRITE ( "Can't connect field. FIELDID : %d, CID[%d] name %s",
			pNetMsg->dwFieldServer, pChar->m_dwClientID, pChar->m_szName );
		return;
	}

	//	ĳ���� ���� ���� ����
	NET_GAME_JOIN_FIELDSVR NetJoinField;
	NetJoinField.emType				= EMJOINTYPE_FIRST;
	NetJoinField.dwSlotAgentClient	= pChar->m_dwClientID;
	StringCchCopy ( NetJoinField.szUID, DAUM_MAX_UID_LENGTH+1, pChar->m_szUserName );
	NetJoinField.nUserNum			= pChar->m_dwUserID;
	NetJoinField.dwUserLvl			= pChar->m_dwUserLvl;
	NetJoinField.nChaNum			= pChar->m_dwCharID;
	NetJoinField.dwGaeaID			= pChar->m_dwGaeaID;
    NetJoinField.tPREMIUM			= pChar->m_tPREMIUM;
	NetJoinField.tCHATBLOCK			= pChar->m_tCHATBLOCK;

	NetJoinField.sStartMap			= pChar->m_sStartMapID;
	NetJoinField.dwStartGate		= pChar->m_dwStartGate;
	NetJoinField.vStartPos			= pChar->m_vStartPos;

	NetJoinField.dwActState			= pChar->m_dwActState;
	NetJoinField.bUseArmSub			= pChar->m_bUseArmSub;

	NetJoinField.dwThaiCCafeClass	= pChar->m_dwThaiCCafeClass;
	NetJoinField.nMyCCafeClass		= pChar->m_nMyCCafeClass;					// �����̽þ� PC�� �̺�Ʈ

	NetJoinField.sChinaTime			= pChar->m_sChinaTime;
	NetJoinField.sVietnamGainSystem = pChar->m_sVietnamSystem;


	SENDTOFIELD ( pChar->m_dwClientID, &NetJoinField );

	//	Note : �ɸ����� ���� ���� �ʵ弭�� �� ���� �� ID ����.
	//
	pChar->SetCurrentField ( pNetMsg->dwFieldServer, pChar->m_sStartMapID );
}

void GLAgentServer::ClubBattlePartyCheck( DWORD dwClubID, DWORD dwClubIDTar )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( dwClubID );
	if ( !pCLUB )	return;

	PGLCHARAG pCHAR = NULL;
	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	for ( ; pos!=end; ++pos )
	{
		pCHAR = GetCharID ( (*pos).first );
		if ( !pCHAR )
		{
			continue;
		}
		else
		{
			if ( pCHAR->GetPartyID()==PARTY_NULL ) continue;
			
			GLPARTY* pParty = m_cPartyMan.GetParty ( pCHAR->m_dwPartyID );
			
			if( pParty && pParty->ISVAILD() )
			{
				GLPARTY::MEMBER_ITER iter = pParty->m_cMEMBER.begin();

				bool bPartyDel = false;

				PGLCHARAG pMemChar = NULL;
				for ( ; iter != pParty->m_cMEMBER.end(); )
				{
					pMemChar = GLAgentServer::GetInstance().GetChar ( (*iter) );					

					if ( pMemChar &&  pMemChar->m_dwGuild == dwClubIDTar )
					{
						// ��ûŬ������ ��Ƽ���� ���
						if ( pParty->ISMASTER(pMemChar->m_dwGaeaID) )
						{
							m_cPartyMan.DelParty( pParty->m_dwPARTYID );
							break;
						}
						// �Ѵ� �ƴҰ��
						else
						{
							// B ĳ���� Ż��
							GLMSG::SNET_PARTY_DEL NetMsgDel;
							NetMsgDel.dwPartyID = pParty->m_dwPARTYID;
							NetMsgDel.dwDelMember = pMemChar->m_dwGaeaID;

							SENDTOPARTY ( pParty->m_dwPARTYID, (NET_MSG_GENERIC*) &NetMsgDel );

							//	Note : ��Ƽ���� Ż�� ���� �ʵ� ������ �˸�.
							//
							SENDTOALLCHANNEL ( &NetMsgDel );

							//	Note : ��Ƽ Ż�� ó��.
							//
							pMemChar->ReSetPartyID ();
							iter = pParty->m_cMEMBER.erase(iter);
							pParty->DELCONFT_MEMBER (  pMemChar->m_dwGaeaID );	

							continue;
						}
					}

					++iter;
				}
			}
		}
	}
}

void GLAgentServer::AllianceBattlePartyCheck( DWORD dwClubID, DWORD dwClubIDTar )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( dwClubID );
	if ( !pCLUB )	return;

	CLUB_ALLIANCE_ITER pos = pCLUB->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pCLUB->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{

		const GLCLUBALLIANCE &sALLIANCE = *pos;
		GLCLUB* pClubP = m_cClubMan.GetClub ( sALLIANCE.m_dwID );
		if ( !pClubP ) continue;
		
		PGLCHARAG pCHAR = NULL;
		CLUBMEMBERS_ITER pos = pClubP->m_mapMembers.begin();
		CLUBMEMBERS_ITER end = pClubP->m_mapMembers.end();
		for ( ; pos!=end; ++pos )
		{
			pCHAR = GetCharID ( (*pos).first );
			if ( !pCHAR )
			{
				continue;
			}
			else
			{
				if ( pCHAR->GetPartyID()==PARTY_NULL ) continue;
				
				GLPARTY* pParty = m_cPartyMan.GetParty ( pCHAR->m_dwPartyID );
				
				if( pParty && pParty->ISVAILD() )
				{
					GLPARTY::MEMBER_ITER iter = pParty->m_cMEMBER.begin();

					bool bPartyDel = false;

					PGLCHARAG pMemChar = NULL;
					for ( ; iter != pParty->m_cMEMBER.end(); )
					{
						pMemChar = GLAgentServer::GetInstance().GetChar ( (*iter) );
						
						if ( pMemChar )
						{
							GLCLUB* pClubS =  m_cClubMan.GetClub ( pMemChar->m_dwGuild );
							
							if ( pClubS && pClubS->m_dwAlliance == dwClubIDTar )
							{
								// ��ûŬ������ ��Ƽ���� ���
								if ( pParty->ISMASTER(pMemChar->m_dwGaeaID) )
								{
									m_cPartyMan.DelParty( pParty->m_dwPARTYID );
									break;
								}
								// �Ѵ� �ƴҰ��
								else
								{
									// B ĳ���� Ż��
									GLMSG::SNET_PARTY_DEL NetMsgDel;
									NetMsgDel.dwPartyID = pParty->m_dwPARTYID;
									NetMsgDel.dwDelMember = pMemChar->m_dwGaeaID;

									SENDTOPARTY ( pParty->m_dwPARTYID, (NET_MSG_GENERIC*) &NetMsgDel );

									//	Note : ��Ƽ���� Ż�� ���� �ʵ� ������ �˸�.
									//
									SENDTOALLCHANNEL ( &NetMsgDel );

									//	Note : ��Ƽ Ż�� ó��.
									//
									pMemChar->ReSetPartyID ();
									iter = pParty->m_cMEMBER.erase(iter);
									pParty->DELCONFT_MEMBER (  pMemChar->m_dwGaeaID );	

									continue;
								}
							}
							
							++iter;
							continue;
						}

						++iter;
					}
				}
			}
		}
		
	}

	return;
}


HRESULT GLAgentServer::StartClubBattle( DWORD dwClubID, GLCLUBBATTLE sClubBattle )
{
	// ���� Ŭ����Ʋ ���� �߰�
	GLCLUB* pClubP = m_cClubMan.GetClub( dwClubID );
	GLCLUB* pClubS = m_cClubMan.GetClub( sClubBattle.m_dwCLUBID );
	if( !pClubP || !pClubS ) return S_FALSE;

	//	�ʵ忡 �˸�.
	GLMSG::SNET_CLUB_BATTLE_BEGIN_FLD NetMsgFld;
	NetMsgFld.dwChiefClubID = pClubP->m_dwID;
	NetMsgFld.dwIndianClubID = pClubS->m_dwID;
	NetMsgFld.tStartTime = sClubBattle.m_tStartTime;	
	NetMsgFld.tEndTime = sClubBattle.m_tEndTime;	
	NetMsgFld.bAlliance = sClubBattle.m_bAlliance;
	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgFld );	

	//	Ŭ���̾�Ʈ�鿡 �˸�. 
	GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT2 NetMsgClt;
	NetMsgClt.dwClubID = pClubP->m_dwID;	
	StringCchCopy ( NetMsgClt.szClubName, CHAR_SZNAME, pClubP->m_szName );	
	NetMsgClt.tStartTime = sClubBattle.m_tStartTime;	
	NetMsgClt.tEndTime = sClubBattle.m_tEndTime;	
	NetMsgClt.bAlliance = sClubBattle.m_bAlliance;	

	CString strText;

	if ( sClubBattle.m_bAlliance )
	{
		CLUB_ALLIANCE_ITER pos = pClubP->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pClubP->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = *pos;
			GLCLUB* pClub = m_cClubMan.GetClub ( sALLIANCE.m_dwID );
			
			if ( !pClub ) continue;
			
			pClub->ADDBATTLECLUB( sClubBattle );
		}

		sClubBattle.m_dwCLUBID = pClubP->m_dwID;
		StringCchCopy ( sClubBattle.m_szClubName, CHAR_SZNAME, pClubP->m_szName );

		pos = pClubS->m_setAlliance.begin();
		end = pClubS->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = *pos;
			GLCLUB* pClub = m_cClubMan.GetClub ( sALLIANCE.m_dwID );
			
			if ( !pClub ) continue;
			
			pClub->ADDBATTLECLUB( sClubBattle );
		}

		GLAgentServer::GetInstance().SENDTOALLIANCECLIENT ( pClubS->m_dwID, &NetMsgClt );	
		
		NetMsgClt.dwClubID = pClubS->m_dwID;	
		StringCchCopy ( NetMsgClt.szClubName, CHAR_SZNAME, pClubS->m_szName  );	
		GLAgentServer::GetInstance().SENDTOALLIANCECLIENT ( pClubP->m_dwID, &NetMsgClt );

		GLAgentServer::GetInstance().AllianceBattlePartyCheck( pClubS->m_dwID, pClubP->m_dwID );
		strText.Format( ID2SERVERTEXT( "ALLIANCE_BATTLE_BEGIN" ), pClubP->m_szName, pClubS->m_szName );
	}
	else
	{	
		pClubP->ADDBATTLECLUB( sClubBattle );
		
		// ����� Ŭ����Ʋ ���� �߰�
		sClubBattle.m_dwCLUBID = pClubP->m_dwID;
		StringCchCopy ( sClubBattle.m_szClubName, CHAR_SZNAME, pClubP->m_szName );
		pClubS->ADDBATTLECLUB( sClubBattle );


		GLAgentServer::GetInstance().SENDTOCLUBCLIENT ( pClubS->m_dwID, &NetMsgClt );
		
		NetMsgClt.dwClubID = pClubS->m_dwID;	
		StringCchCopy ( NetMsgClt.szClubName, CHAR_SZNAME, pClubS->m_szName  );	
		GLAgentServer::GetInstance().SENDTOCLUBCLIENT ( pClubP->m_dwID, &NetMsgClt );

		GLAgentServer::GetInstance().ClubBattlePartyCheck( pClubS->m_dwID, pClubP->m_dwID );
		strText.Format( ID2SERVERTEXT( "CLUB_BATTLE_BEGIN" ), pClubP->m_szName, pClubS->m_szName );
	}	

	GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
	NetMsg.SETTEXT ( strText.GetString() );
	GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsg );

	return S_OK;
}
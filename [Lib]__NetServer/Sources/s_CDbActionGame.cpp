#include "pch.h"

#include "s_CDbAction.h"
#include "s_CSessionServer.h"
#include "s_CFieldServer.h"
#include "s_CAgentServer.h"

#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLCharData.h"
#include "../[Lib]__RanClient/Sources/G-Logic/GLogicEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/******************************************************************************
** Databse Action for Game Database
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// 캐릭터 생성
CCreateNewCharacter::CCreateNewCharacter(
	int nIndex, // 캐릭터 인덱스
    DWORD dwUserNum, // 사용자번호
    DWORD dwSvrGrp, // 서버그룹번호
    CString strChaName, // 캐릭터이름
    WORD wSchool, // 캐릭터학교
    WORD wHair, // 캐릭터 머리모양
    WORD wFace, // 캐릭터 얼굴모양
	WORD wHairColor,
	WORD wSex,
    DWORD dwClient, 
    const char* szUserIP, 
    USHORT uPort )
{
    m_nIndex     = nIndex; 
    m_dwUserNum  = dwUserNum;
    m_dwSvrGrp   = dwSvrGrp;
    m_strChaName = strChaName;
    m_wSchool    = wSchool;
    m_wHair      = wHair;
    m_wFace      = wFace;
	m_wHairColor = wHairColor;
	m_wSex		 = wSex;
    m_dwClient   = dwClient;
    m_strUserIP  = szUserIP;
    m_uPort      = uPort;
}

int CCreateNewCharacter::Execute(CServer* pServer)
{
    int nChaNum = 0; // 생성된 캐릭터 번호
    GLCHARLOGIC NewCharLogic;
	NewCharLogic.INIT_NEW_CHAR((EMCHARINDEX) m_nIndex, 
								m_dwUserNum, 
								m_dwSvrGrp, 
								m_strChaName, 
								m_wSchool, 
								m_wHair, 
								m_wFace,
								m_wHairColor,
								m_wSex);
    
    nChaNum = COdbcManager::GetInstance()->CreateNewCharacter(&NewCharLogic);

    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
    pAgentServer->MsgCreateCharacterBack(nChaNum, m_dwClient, m_dwUserNum, m_strUserIP.GetString(), m_uPort);
    return DB_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
CDelCharacter::CDelCharacter(int nUserNum, int nChaNum, const char* szPass2, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
	m_nUserNum = nUserNum;
	m_nChaNum  = nChaNum;
	m_strPass2 = szPass2;
	m_dwClient = dwClient;
	m_strUserIP= szUserIP;
	m_uPort    = uPort;
}

int CDelCharacter::Execute(CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	int nResult = COdbcManager::GetInstance()->DelCharacter(m_nUserNum, m_nChaNum, m_strPass2.GetString());
	pTemp->MsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_dwClient, m_strUserIP.GetString(), m_uPort);
	return NET_OK;
}

/**
* 캐릭터삭제
*/
CDaumDelCharacter::CDaumDelCharacter(int nUserNum, int nChaNum, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;

	m_dwClient  = dwClient;
	m_strUserIP = szUserIP;
	m_uPort     = uPort;
}

int CDaumDelCharacter::Execute(CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
    int nResult = COdbcManager::GetInstance()->DaumDelCharacter(m_nUserNum, m_nChaNum);
	pTemp->DaumMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_dwClient, m_strUserIP.GetString(), m_uPort);
	return NET_OK;
}

/**
* GSP 캐릭터삭제
*/
CGspDelCharacter::CGspDelCharacter(
	int nUserNum,
	int nChaNum,
	DWORD dwClient,
	const TCHAR* szUserIP,
	USHORT uPort )
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;

	m_dwClient  = dwClient;
	m_strUserIP = szUserIP;
	m_uPort     = uPort;
}

int CGspDelCharacter::Execute( CServer* pServer )
{
	if (pServer == NULL) return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
    int nResult = COdbcManager::GetInstance()->GspDelCharacter(
												m_nUserNum,
												m_nChaNum );
	pTemp->GspMsgSndChaDelInfoBack(
			m_nChaNum,
			m_nUserNum,
			nResult,
			m_dwClient,
			m_strUserIP.GetString(),
			m_uPort );

	return NET_OK;
}

/**
* Terra 캐릭터삭제
*/
CTerraDelCharacter::CTerraDelCharacter(int nUserNum, int nChaNum, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;

	m_dwClient  = dwClient;
	m_strUserIP = szUserIP;
	m_uPort     = uPort;
}

int CTerraDelCharacter::Execute(CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
    int nResult = COdbcManager::GetInstance()->TerraDelCharacter(m_nUserNum, m_nChaNum);
	pTemp->TerraMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_dwClient, m_strUserIP.GetString(), m_uPort);
	return NET_OK;
}

/**
* EXCITE:캐릭터 삭제
*/
CExciteDelCharacter::CExciteDelCharacter(int nUserNum, int nChaNum, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;

	m_dwClient  = dwClient;
	m_strUserIP = szUserIP;
	m_uPort     = uPort;
}

int CExciteDelCharacter::Execute(CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;
	
    int nResult = COdbcManager::GetInstance()->ExciteDelCharacter(m_nUserNum, m_nChaNum);
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	pTemp->ExciteMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_dwClient, m_strUserIP.GetString(), m_uPort);
	return NET_OK;
}

/**
* JAPAN:캐릭터 삭제
*/
CJapanDelCharacter::CJapanDelCharacter(int nUserNum, int nChaNum, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;

	m_dwClient  = dwClient;
	m_strUserIP = szUserIP;
	m_uPort     = uPort;
}

int CJapanDelCharacter::Execute(CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;
	
    int nResult = COdbcManager::GetInstance()->JapanDelCharacter(m_nUserNum, m_nChaNum);
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	pTemp->JapanMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_dwClient, m_strUserIP.GetString(), m_uPort);
	return NET_OK;
}


CGsDelCharacter::CGsDelCharacter(int nUserNum, int nChaNum, const char* szPass2, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
	m_nUserNum = nUserNum;
	m_nChaNum  = nChaNum;
	m_strPass2 = szPass2;
	m_dwClient = dwClient;
	m_strUserIP= szUserIP;
	m_uPort    = uPort;
}

int CGsDelCharacter::Execute(CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	int nResult = COdbcManager::GetInstance()->GsDelCharacter(m_nUserNum, m_nChaNum );
	pTemp->GsMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_dwClient, m_strUserIP.GetString(), m_uPort);
	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
CGetChaBAInfo::CGetChaBAInfo(int nUserNum, int nSvrGrp, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
	m_nUserNum = nUserNum;
	m_nSvrGrp  = nSvrGrp;
	
	m_dwClient  = dwClient;
	m_strUserIP = szUserIP;
	m_uPort     = uPort;
}	

int CGetChaBAInfo::Execute(CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	m_msg.nmg.nType = NET_MSG_CHA_BAINFO;
	COdbcManager::GetInstance()->GetChaBAInfo(m_nUserNum, m_nSvrGrp, &m_msg);
    
	pTemp->MsgSndChaBasicBAInfoBack(&m_msg, m_dwClient, m_strUserIP.GetString(), m_uPort);

	return NET_OK;
}

CGetChaBInfo::CGetChaBInfo(int nUserNum, int nChaNum, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
	m_nUserNum = nUserNum;
	m_nChaNum = nChaNum;
	m_dwClient  = dwClient;
	m_strUserIP = szUserIP;
	m_uPort     = uPort;
}

int CGetChaBInfo::Execute(CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;

	if (m_nUserNum > 0 && m_nChaNum > 0)
	{
		CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
		if (COdbcManager::GetInstance()->GetChaBInfo(m_nUserNum, m_nChaNum, &m_msg.Data) == DB_ERROR)
		{			
			return NET_ERROR;
		}
		else
		{
			pTemp->MsgSndChaBasicInfoBack(&m_msg, m_dwClient, m_strUserIP.GetString(), m_uPort);
			return NET_OK;
		}
	} // if (nUserNum > 0 || nChaNum > 0)
	else
	{
		return NET_ERROR;
	}
}

/*
CGetChaFriend::CGetChaFriend(
	int nChaNum,
	DWORD dwClient,
	const char* szUserIP,
	USHORT uPort )
{
	m_dwClient  = dwClient;
	m_strUserIP = szUserIP;
	m_uPort     = uPort;
}

int CGetChaFriend::Execute(CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	std::vector<CHA_FRIEND> vecFriend;

	COdbcManager::GetInstance()->GetChaFriend(m_nChaNum, vecFriend);

	// Agent 서버 함수 실행
	// pTemp->
	return NET_OK;
}
*/

///////////////////////////////////////////////////////////////////////////////
//
void CGetChaInfoAndJoin::Init(
	DWORD dwClientID,
	DWORD dwUserID,
	DWORD dwUserLvl,
	__time64_t tPREMIUM,
	__time64_t tCHATBLOCK,
	DWORD dwServerID,
	DWORD dwCharID )
{
	m_dwClientID = dwClientID;

	m_cCharAgData.m_dwUserID   = dwUserID;
	m_cCharAgData.m_dwUserLvl  = dwUserLvl;
	m_cCharAgData.m_dwServerID = dwServerID;
	m_cCharAgData.m_dwCharID   = dwCharID;  
    m_tPremiumDate             = tPREMIUM;
	m_tChatBlock               = tCHATBLOCK;
}

int CGetChaInfoAndJoin::Execute( CServer* pServer )
{
	int nRetCode(0);

	//	캐릭터 가져오기
	nRetCode = COdbcManager::GetInstance()->GetCharacterInfo(
						m_cCharAgData.m_dwUserID,
						m_cCharAgData.m_dwCharID,
						&m_cCharAgData );
	if ( nRetCode == DB_ERROR )
	{
		CConsoleMessage::GetInstance()->Write ( _T("ERROR:Read Character DB failed") );
		return NET_OK;
	}

    m_cCharAgData.m_tPREMIUM = m_tPremiumDate; // 프리미엄 만료기간
	m_cCharAgData.m_tCHATBLOCK = m_tChatBlock;

	//	친구 목록 가져오기.
#if defined(KRT_PARAM)
	std::vector<CHA_FRIEND_SMS> vecFriendSMS;
	nRetCode = COdbcManager::GetInstance()->GetChaFriendSMS( m_cCharAgData.m_dwCharID, vecFriendSMS );
	if ( nRetCode == DB_ERROR )
	{
		CConsoleMessage::GetInstance()->Write( _T("ERROR:Read Frield list failed") );
		return NET_OK;
	}

	for ( size_t i=0; i<vecFriendSMS.size(); ++i )
	{
		CHA_FRIEND_SMS &sFR = vecFriendSMS[i];

		SFRIEND sFRIEND;
		sFRIEND.nCharID = sFR.nChaS;
		sFRIEND.nCharFlag = sFR.nChaFlag;
		StringCchCopy( sFRIEND.szCharName, CHR_ID_LENGTH+1, sFR.szCharName );
		StringCchCopy( sFRIEND.szPhoneNumber, SMS_RECEIVER, sFR.szPhoneNumber );
//		m_cCharAgData.m_mapFriend[sFRIEND.szCharName] = sFRIEND;
		m_cCharAgData.m_mapFriend.insert( std::make_pair( sFRIEND.szCharName, sFRIEND ) );


		/*CHA_FRIEND_SMS &sFR = vecFriendSMS[i];

		SFRIEND * pFRIEND = new SFRIEND;
		pFRIEND->nCharID = sFR.nChaS;
		pFRIEND->nCharFlag = sFR.nChaFlag;
		StringCchCopy( pFRIEND->szCharName, CHR_ID_LENGTH+1, sFR.szCharName );
		StringCchCopy( pFRIEND->szPhoneNumber, SMS_RECEIVER, sFR.szPhoneNumber );
		m_cCharAgData.m_mapFriend[sFRIEND.szCharName] = sFRIEND;
		if( m_cCharAgData.m_mapFriend.insert( std::make_pair( pFRIEND->szCharName, pFRIEND ) ).second )
		{
		}
		else
		{
			SAFE_DELETE( pFRIEND );
		}*/
	}
#else
	std::vector<CHA_FRIEND> vecFriend;
	nRetCode = COdbcManager::GetInstance()->GetChaFriend( m_cCharAgData.m_dwCharID, vecFriend );
	if ( nRetCode == DB_ERROR )
	{
		CConsoleMessage::GetInstance()->Write( _T("ERROR:Read Friend list failed") );
		return NET_OK;
	}

	for ( size_t i=0; i<vecFriend.size(); ++i )
	{
		CHA_FRIEND &sFR = vecFriend[i];

		SFRIEND sFRIEND;
		sFRIEND.nCharID = sFR.nChaS;
		sFRIEND.nCharFlag = sFR.nChaFlag;
		StringCchCopy( sFRIEND.szCharName, CHR_ID_LENGTH+1, sFR.szCharName );
//		m_cCharAgData.m_mapFriend[sFRIEND.szCharName] = sFRIEND;
		m_cCharAgData.m_mapFriend.insert( std::make_pair( sFRIEND.szCharName, sFRIEND ) );

		/*CHA_FRIEND &sFR = vecFriend[i];

		SFRIEND * pFRIEND = new SFRIEND;
		pFRIEND->nCharID = sFR.nChaS;
		pFRIEND->nCharFlag = sFR.nChaFlag;
		StringCchCopy( pFRIEND->szCharName, CHR_ID_LENGTH+1, sFR.szCharName );
		m_cCharAgData.m_mapFriend[sFRIEND.szCharName] = sFRIEND;
		if( m_cCharAgData.m_mapFriend.insert( std::make_pair( pFRIEND->szCharName, pFRIEND ) ).second )
		{
		}
		else
		{
			SAFE_DELETE( pFRIEND );
		}*/
	}
#endif
	
	nRetCode = COdbcManager::GetInstance()->LogUserAttend( m_cCharAgData.m_dwUserID, m_cCharAgData.m_vecAttend );
	if ( nRetCode == DB_ERROR )
	{
		CConsoleMessage::GetInstance()->Write( _T("ERROR:Read Attend list failed") );
		return NET_OK;
	}

	//	Note : 내부 메시지 발생.
	//
	GLMSG::SNETJOIN2AGENTSERVER NetMsg;
	NetMsg.m_dwClientID = m_dwClientID;
	NetMsg.m_pCharAgData = new GLCHARAG_DATA;
	*NetMsg.m_pCharAgData = m_cCharAgData;


	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
	//pAgentServer->MsgAgentReqJoin( m_dwClientID, &m_cCharAgData );
	pAgentServer->InsertMsg( m_dwClientID, (char*) &NetMsg );




	return NET_OK;
}


void CGetChaInfoAndJoinField::Init(
	DWORD dwClientID,
	DWORD dwUserID,
	const char* szUID,
	DWORD dwUserLvl,
	__time64_t tPREMIUM,
	__time64_t tCHATBLOCK,
	DWORD dwServerID,
	DWORD dwCharID,
	GLMSG::SNETJOIN2FIELDSERVER::SINFO sINFO )
{
	m_dwClientID = dwClientID;

	m_cCharData.SetUserID( dwUserID );
	StringCchCopy(m_cCharData.m_szUID, USR_ID_LENGTH+1, szUID);

	//*/*/*/*/*/*
	m_cCharData.m_dwUserLvl  = dwUserLvl;
	m_cCharData.m_dwServerID = dwServerID;
	m_cCharData.m_dwCharID   = dwCharID;
	m_tPremiumDate           = tPREMIUM;    
	m_tChatBlock             = tCHATBLOCK;

	m_sINFO					 = sINFO;
}

/*void CGetChaInfoAndJoinField::InitEx ( SNATIVEID nidMAP, DWORD dwGate, D3DXVECTOR3 vPos, EMGAME_JOINTYPE emTYPE, DWORD dwACTSTATE )
{
	m_emType = emTYPE;

	m_sStartMap = nidMAP;
	m_dwStartGate = dwGate;
	m_vStartPos = vPos;

	m_dwActState = dwACTSTATE;
}*/

int CGetChaInfoAndJoinField::Execute ( CServer* pServer )
{
	int nRetCode;
	nRetCode = COdbcManager::GetInstance()->GetCharacterInfo(m_cCharData.GetUserID(),
		                                                     (int) m_cCharData.m_dwCharID,
															 &m_cCharData);
	if (nRetCode == DB_ERROR)
	{
		CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Character DB read failed") );
		return NET_OK;
	}

    m_cCharData.m_tPREMIUM = m_tPremiumDate; // Premium Service 만료시간
	m_cCharData.m_tCHATBLOCK = m_tChatBlock; // Chat Block 만료시간

	nRetCode = COdbcManager::GetInstance()->ReadUserInven( &m_cCharData );
	if (nRetCode == DB_ERROR)
	{
		CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:UserInven DB read failed.") );
		return NET_OK;
	}

/*
	CString m_strUID = m_cCharData.m_szUID;
    std::vector<SHOPPURCHASE> vItem;

	COdbcManager::GetInstance()->GetPurchaseItem ( m_strUID, vItem );

	m_cCharData.SETSHOPPURCHASE ( vItem );
*/
	//	Note : 내부 메시지 발생.
	//
	//*/*/*/*/*/*
	GLMSG::SNETJOIN2FIELDSERVER NetMsg;
	NetMsg.m_dwClientID = m_dwClientID;
	NetMsg.m_pCharData = new SCHARDATA2;
	NetMsg.m_pCharData->Assign ( m_cCharData );
	NetMsg.m_sINFO = m_sINFO;
	/*NetMsg.m_sINFO.m_emType			  = m_emType;
	NetMsg.m_sINFO.m_sStartMap		  = m_sStartMap;
	NetMsg.m_sINFO.m_dwStartGate	  = m_dwStartGate;
	NetMsg.m_sINFO.m_vStartPos		  = m_vStartPos;
	NetMsg.m_sINFO.m_dwActState		  = m_dwActState;
	NetMsg.m_sINFO.m_bUseArmSub		  = m_bUseArmSub;
	NetMsg.m_sINFO.m_dwThaiCCafeClass = m_dwThaiCCafeClass;
	NetMsg.m_sINFO.m_sChinaTime		  = m_sChinaTime;
	NetMsg.m_sINFO.m_sEventTime		  = m_sEventTime;
	NetMsg.m_sINFO.m_sVietnamSystem   = m_sVietnamSystem;
	NetMsg.m_sINFO.m_sPetSkinPackData.Init();*/


	CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
	//pFieldServer->MsgFieldReqJoin( m_dwClientID, &m_cCharData, sINFO );
	pFieldServer->InsertMsg ( m_dwClientID, (char*) &NetMsg );

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////
// 캐릭터를 온라인 상태로 만든다.
CSetCharacterOnline::CSetCharacterOnline(int nChaNum)
{
	m_nChaNum = nChaNum ;
}

int CSetCharacterOnline::Execute(CServer* pServer)
{
	return COdbcManager::GetInstance()->SetCharacterOnline(m_nChaNum);
}

///////////////////////////////////////////////////////////////////////////
// 캐릭터를 오프라인 상태로 만든다.
CSetCharacterOffline::CSetCharacterOffline(int nChaNum)
{
	m_nChaNum = nChaNum;
}

int CSetCharacterOffline::Execute(CServer* pServer)
{
	return COdbcManager::GetInstance()->SetCharacterOffline(m_nChaNum);
}

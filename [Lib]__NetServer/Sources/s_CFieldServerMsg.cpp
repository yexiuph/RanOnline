#include "pch.h"
#include "s_CFieldServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CFieldServer::InsertMsg(
	int nClient,
	void* pMsg )
{
    if (NULL != pMsg)
	{
		NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) pMsg;
		m_pRecvMsgManager->MsgQueueInsert(nClient, pNmg, pNmg->dwSize);
	}
	return 0;
}

int CFieldServer::MsgProcess(
	MSG_LIST* pMsg )
{
	if (pMsg == NULL) return NET_ERROR;

	NET_MSG_GENERIC* nmg = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);

	if (nmg == NULL) return NET_ERROR;

	if ( nmg->dwSize > NET_DATA_BUFSIZE ) return NET_ERROR;

	DWORD dwClient = pMsg->dwClient;

	// 서버간 통신 메시지
	if ((dwClient < NET_RESERVED_SLOT) && (dwClient >= 0))
	{
		ServerMsgProcess( pMsg );
		return 0;
	}

	switch (nmg->nType)
	{
	case NET_MSG_HEARTBEAT_SERVER_REQ:
		MsgHeartbeatServerAnswer( pMsg );
		break;
	case NET_MSG_HEARTBEAT_CLIENT_ANS: // Client->Server:Heartbeat
		m_pClientManager->SetHeartBeat( dwClient );
		break;
	case NET_MSG_I_AM_AGENT : // 서버간 통신슬릇이다.
		m_dwAgentSlot = dwClient;
		GLGaeaServer::GetInstance().SetAgentSlot(dwClient);
		break;
	case NET_MSG_LOGIN_FB_SUB_OK_FIELD : // Agent->Field : 로그인 성공정보
		MsgLoginInfoAgent(pMsg);
		break;
	case NET_MSG_VERSION_INFO : // 클라이언트 버전정보
		MsgVersion(pMsg);
		break;	
	case MET_MSG_GAME_JOIN_FIELDSVR: // Agent->Field : 케릭터의 필드 접속을 요청.
		MsgGameJoinChar(pMsg);
		break;
	case NET_MSG_FIELD_REQ_JOIN:
		MsgFieldReqJoin(pMsg);
		break;
	case NET_MSG_JOIN_FIELD_IDENTITY: // Client->Field : 클라이언트가 필드서버에 접속후 인식자로 확인 요청.
		MsgJoinInfoFromClient(pMsg);
		break;
	default:
		// Insert to message queue
		if ( nmg->nType > NET_MSG_GCTRL )
		{
			DWORD dwClient = pMsg->dwClient;
			if ( !m_pClientManager->IsAgentSlot(dwClient) )
				dwClient = m_pClientManager->GetSlotFieldAgent(dwClient);

			PROFILE_BEGIN("GLGaeaServer::MsgProcess");
			DWORD dwGaeaID = m_pClientManager->GetGaeaID(dwClient);
			GLGaeaServer::GetInstance().MsgProcess ( (NET_MSG_GENERIC*)pMsg->Buffer, dwClient, dwGaeaID );
			PROFILE_END("GLGaeaServer::MsgProcess");
		}
		break;
	}
	return 0;
}

void CFieldServer::ServerMsgProcess(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;

	//if (dwClient == m_dwSession)
	//{
	//	SessionMsgProcess( pMsg );
	//}
	//else if (0 == dwClient)
	//{
 //       GLGaeaServer::GetInstance().MsgProcess ( (NET_MSG_GENERIC*)pMsg->Buffer, 0, 0 );
	//}
	//else
	//{
	//}

	switch ( dwClient )

	{
	case NET_RESERVED_SESSION :
		SessionMsgProcess( pMsg );
		break;
	case 0:
		GLGaeaServer::GetInstance().MsgProcess ( (NET_MSG_GENERIC*)pMsg->Buffer, 0, 0 );
	default:
		break;
	}
}

void CFieldServer::MsgHeartbeatServerAnswer( MSG_LIST* pMsg )
{	
	NET_HEARTBEAT_SERVER_ANS msg;
	SendAgent( pMsg->dwClient, &msg );
}

// Client->Field : Join Msg
void CFieldServer::MsgJoinInfoFromClient(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

    DWORD dwClient = pMsg->dwClient;
	
	NET_GAME_JOIN_FIELD_IDENTITY* pMsgIdent = (NET_GAME_JOIN_FIELD_IDENTITY*) pMsg->Buffer;

	if ( !(pMsgIdent->emType==EMJOINTYPE_FIRST||
		pMsgIdent->emType==EMJOINTYPE_MOVEMAP||
		pMsgIdent->emType==EMJOINTYPE_REBIRTH) )											return;

	if ( pMsgIdent->dwSlotFieldAgent >= (DWORD) m_pClientManager->GetMaxClient() )					return;
	if ( m_pClientManager->GetGaeaID(pMsgIdent->dwSlotFieldAgent) != pMsgIdent->dwGaeaID )	return;

	m_pClientManager->SetSlotFieldClient ( pMsgIdent->dwSlotFieldAgent, dwClient );
	m_pClientManager->SetSlotFieldAgent  ( dwClient, pMsgIdent->dwSlotFieldAgent );
	m_pClientManager->SetCryptKey		 ( dwClient, pMsgIdent->ck );
	m_pClientManager->SetGaeaID          ( dwClient, pMsgIdent->dwGaeaID );

	PGLCHAR pGLChar = GLGaeaServer::GetInstance().GetChar(pMsgIdent->dwGaeaID);
	if ( !pGLChar )
	{
		GLMSG::SNET_DROP_OUT_FORCED NetMsg_OUT;
		SendAgent ( dwClient, (LPVOID) &NetMsg_OUT );
		return;
	}

	//	Note : 클라이언트에 Field 서버와의 접속 인식후 접속 종류에 따른 응답.
	//
	switch ( pMsgIdent->emType )
	{
	case EMJOINTYPE_FIRST:
		{
			//	캐릭터 생성 정보 정보 전송.
			pGLChar->MsgGameJoin();
		}
		break;

	case EMJOINTYPE_MOVEMAP:
		{
			//	개이트 이동 확인 메시지.
			GLMSG::SNETREQ_GATEOUT_FB NetMsgGateOutFB;
			NetMsgGateOutFB.emFB = EMCHAR_GATEOUT_OK;
			NetMsgGateOutFB.sMapID = pGLChar->m_sMapID;
			NetMsgGateOutFB.vPos = pGLChar->GetPosition();
			SendAgent ( dwClient, (LPVOID) &NetMsgGateOutFB );

			pGLChar->MsgFieldMoveReset ();
		}
		break;

	case EMJOINTYPE_REBIRTH:
		{
			//	부활 확인 메시지.
			GLMSG::SNETPC_REQ_REBIRTH_FB NetMsg;
			NetMsg.sMapID = pGLChar->m_sMapID;
			NetMsg.vPos = pGLChar->GetPosition();
			NetMsg.wNowHP = (WORD) pGLChar->GETHP();
			NetMsg.wNowMP = pGLChar->GETMP();
			NetMsg.wNowSP = pGLChar->GETSP();

			SendAgent ( dwClient, (LPVOID) &NetMsg );

			pGLChar->MsgFieldMoveReset ();
		}
		break;
	};
}

void CFieldServer::MsgSndCryptKey(DWORD dwClient)
{
	CRYPT_KEY ck = m_pClientManager->GetNewCryptKey(); // Get crypt key
			
	NET_CRYPT_KEY nck;	
	nck.nmg.nType   = NET_MSG_SND_CRYT_KEY;
	nck.ck			= ck;
	SendClient(dwClient, &nck);
	m_pClientManager->SetCryptKey(dwClient, ck); // Set crypt key
}

// Filed->Client
// 일반적 채팅 메시지 전송
void CFieldServer::MsgSndChatNormal(DWORD dwClient, const char* szName, const char* szMsg)
{
    if (szMsg == NULL) return;

    NET_CHAT nc;
	nc.nmg.nType = NET_MSG_CHAT;
	nc.emType    = CHAT_TYPE_NORMAL;
	::StringCchCopy(nc.szName, CHR_ID_LENGTH+1, szName);
	::StringCchCopyN(nc.szChatMsg, CHAT_MSG_SIZE+1, szMsg, CHAT_MSG_SIZE);
	SendClient(dwClient, (NET_MSG_GENERIC*) &nc);
}

// Agent->Field 에서 오는 로그인 성공정보
void CFieldServer::MsgLoginInfoAgent(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_LOGIN_FEEDBACK_DATA3* nlfd3 = reinterpret_cast<NET_LOGIN_FEEDBACK_DATA3*> (pMsg->Buffer);
	
	m_pClientManager->SetUserNum  (dwClient, nlfd3->nUserNum);
	m_pClientManager->SetUserID   (dwClient, nlfd3->szUserid);
	m_pClientManager->SetLoginTime(dwClient);
}

// Agent->Field : 캐릭터 접속.
void CFieldServer::MsgGameJoinChar ( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	int nRetCode	= 0;
	DWORD dwItemNum = 0;

	NET_GAME_JOIN_FIELDSVR* pNgj = reinterpret_cast<NET_GAME_JOIN_FIELDSVR*> (pMsg->Buffer);
	
	DWORD dwClient = pMsg->dwClient; // Field's agent slot number	
	DWORD dwGaeaID = pNgj->dwGaeaID; // Agent's GaeaID
	DWORD dwChaNum = pNgj->nChaNum;  // Agent's Character Number
    __time64_t tPREMIUM = pNgj->tPREMIUM;
	__time64_t tCHATBLOCK = pNgj->tCHATBLOCK;
	
	//	접속자 정보 세팅.
	m_pClientManager->SetGaeaID  ( dwClient, dwGaeaID );
	m_pClientManager->SetUserNum ( dwClient, pNgj->nUserNum );

	m_pClientManager->SetUserID ( dwClient, pNgj->szUID );
	const char* szUID = m_pClientManager->GetUserID(dwClient);

	m_pClientManager->SetSlotFieldAgent ( dwClient, dwClient );
	m_pClientManager->SetSlotAgentClient( dwClient, pNgj->dwSlotAgentClient);
    m_pClientManager->SetPremiumDate    ( dwClient, tPREMIUM );
	m_pClientManager->SetChatBlockDate  ( dwClient, tCHATBLOCK );


	// 태국 사이버 카페( TH_PARAM이 define되어 있지 않으면 0이 넘어온다. )
	INT dwThaiCCafeClass = pNgj->dwThaiCCafeClass;

	// 중국 Game Time 별 수익( CH_PARAME이 define되어 있지 않으면 0이 넘어온다. )
	SChinaTime sChinaTime = pNgj->sChinaTime;

	// 이벤트 시간 ( 처음 접속한 Char는 로그인한 시간, 접속했던 Char는 이벤트 시작시간이 넘어온다. )
	SEventTime sEventTime = pNgj->sEventTime;
	//** Add EventTime

	// 베트남 탐닉 방지 시스템
	SVietnamGainSystem sVietnamGainSystem = pNgj->sVietnamGainSystem;
	

	GLMSG::SNETJOIN2FIELDSERVER::SINFO sINFO;
	sINFO.m_emType			 = pNgj->emType;
	sINFO.m_sStartMap		 = pNgj->sStartMap;
	sINFO.m_dwStartGate		 = pNgj->dwStartGate;
	sINFO.m_vStartPos		 = pNgj->vStartPos;
	sINFO.m_dwActState		 = pNgj->dwActState;
	sINFO.m_bUseArmSub		 = pNgj->bUseArmSub;
	sINFO.m_dwThaiCCafeClass = pNgj->dwThaiCCafeClass;
	sINFO.m_nMyCCafeClass	 = pNgj->nMyCCafeClass;						// 말레이시아 PC방 이벤트
	sINFO.m_sChinaTime		 = pNgj->sChinaTime;
	sINFO.m_sVietnamSystem	 = pNgj->sVietnamGainSystem;
	sINFO.m_sEventTime		 = pNgj->sEventTime;



	CGetChaInfoAndJoinField *pDBAction = new CGetChaInfoAndJoinField;
	pDBAction->Init( dwClient, pNgj->nUserNum, szUID, pNgj->dwUserLvl, tPREMIUM, tCHATBLOCK, m_nServerGroup, dwChaNum, sINFO );
	/*pDBAction->Init(dwClient, 
					pNgj->nUserNum, 
					szUID, 
					pNgj->dwUserLvl, 
					tPREMIUM, 
					tCHATBLOCK,
					pNgj->bUseArmSub,
					m_nServerGroup, 
					dwChaNum,
					dwThaiCCafeClass,
					sChinaTime,
					sEventTime, 
					sVietnamGainSystem );
	pDBAction->InitEx ( pNgj->sStartMap, pNgj->dwStartGate, pNgj->vStartPos, pNgj->emType, pNgj->dwActState );*/
	COdbcManager::GetInstance()->AddJob( pDBAction );
}

void CFieldServer::MsgFieldReqJoin ( MSG_LIST* pMsg )
{
	//*/*/*/*/*/*/*/*/
	if (pMsg == NULL) return;

	GLMSG::SNETJOIN2FIELDSERVER* pNgj = reinterpret_cast<GLMSG::SNETJOIN2FIELDSERVER*> (pMsg->Buffer);
	if (pNgj == NULL) return;

	//	[주의] 이 호출 auto_ptr 구문이 가장 먼저 호출되어야 한다.
	//	이 함수에서 탈출시에 자동으로 제거되게 한다. ( 메시지 처리후 무효화. )
	//
	std::auto_ptr<SCHARDATA2> pCHAR_DATA ( pNgj->m_pCharData );
	if( !pCHAR_DATA.get() ) return;

	//	메시지가 전달된 clientid.
	DWORD dwClientID = pNgj->m_dwClientID;

	//	Note : 클라이언트 메니져에 설정된 것과 틀릴 경우 취소됨.
	DWORD dwUserID = (DWORD) m_pClientManager->GetUserNum(dwClientID);

	//	케릭터의 사용자 ID 유효성 검사.
	//
	if ( dwUserID != (DWORD) pCHAR_DATA->GetUserID() )
	{
		// 캐릭터 DB에서 읽어온 UserID와 메시지의 UserID가 일치 하지 않음
		CConsoleMessage::GetInstance()->Write(_T("ERROR:Character UserID and Message UserID are unmatched %d : %d"), dwUserID, pCHAR_DATA->GetUserID() );
		return;
	}

	DWORD dwGaeaID = m_pClientManager->GetGaeaID(dwClientID);
	
	GLMSG::SNETJOIN2FIELDSERVER::SINFO &sINFO = pNgj->m_sINFO;

	INT dwThaiCCafeClass	= sINFO.m_dwThaiCCafeClass;
	// 말레이시아 PC방 이벤트
	INT nMyCCafeClass		= sINFO.m_nMyCCafeClass;

#if defined(VN_PARAM) //vietnamtest%%%

	if( sINFO.m_sVietnamSystem.lastOffLineTime != 0 )
	{
		CTime crtTime	   = CTime::GetCurrentTime();
		CTime lastOffTime  = sINFO.m_sVietnamSystem.lastOffLineTime;
		// 오프라인한 날짜가 다르면 초기화 한다.
		bool bInitialize = FALSE; // 테스트로 무조건 초기화 한다.

		// 년도, 달, 날짜가 다르면 초기화 한다.
		if( lastOffTime.GetYear() != crtTime.GetYear() ||
			lastOffTime.GetMonth() != crtTime.GetMonth() ||
			lastOffTime.GetDay() != crtTime.GetDay() )
		{
			CConsoleMessage::GetInstance()->Write(_T("=-=-=-=-=- INFO:Vietnam Init =-=-=-=-=- gameTime %d"), sINFO.m_sVietnamSystem.gameTime );
			sINFO.m_sVietnamSystem.gameTime   = 0;
			sINFO.m_sVietnamSystem.loginTime  = crtTime.GetTime();
		}
	}

	pCHAR_DATA->m_sVietnamSystem = sINFO.m_sVietnamSystem;
#endif
#ifdef CH_PARAM_USEGAIN
	pCHAR_DATA->m_sChinaTime	 = sINFO.m_sChinaTime;
#endif

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	if( CheckTracingUser( pCHAR_DATA->m_szUID, pCHAR_DATA->GetUserID() ) != -1 )
	{
		pCHAR_DATA->m_bTracingUser   = TRUE;
	}
#else
	pCHAR_DATA->m_bTracingUser   = FALSE;
#endif

	
	//	캐릭터 생성
	//

	//** Add EventTime
	PGLCHAR pGLChar;

	pGLChar = GLGaeaServer::GetInstance().CreatePC ( pCHAR_DATA.get(), dwClientID, dwGaeaID, FALSE,
							&sINFO.m_sStartMap, sINFO.m_dwStartGate, sINFO.m_vStartPos, sINFO.m_emType, dwThaiCCafeClass, 
							sINFO.m_sEventTime.loginTime, nMyCCafeClass );

	
	if ( !pGLChar )
	{
		CConsoleMessage::GetInstance()->Write(_T("ERROR:Initialized character instance failed"));
		return;
	}

	if( pGLChar->m_wLevel == 1 && pGLChar->m_vSavePos.x == 0.0f && pGLChar->m_vSavePos.z == 0.0f  )
	{
		if( pGLChar->m_lnMoney != 0 )
		{
			CConsoleMessage::GetInstance()->Write(_T("#### ERROR:Money is not correct. Name %s Money %I64d ####"), pGLChar->m_szName, pGLChar->m_lnMoney);
			pGLChar->m_lnMoney = 0;
		}
	}



	//	Note : action state 속성 변경.
	//
	GLMSG::SNETPC_ACTSTATE NetMsgInner;
	NetMsgInner.dwActState = sINFO.m_dwActState;
	pGLChar->MsgProcess ( (NET_MSG_GENERIC*) &NetMsgInner );

	//	Memo :	주/보조 무기 설정
	pGLChar->m_bUseArmSub = sINFO.m_bUseArmSub;

	//	Agent 에 케릭터의 접속 성공을 알림. ( Agent 가 Client 에게 새로운 Field 서버에 접속하게 요청. )
	//
	NET_GAME_JOIN_FIELDSVR_FB MsgJoinFB;
	MsgJoinFB.dwSlotFieldAgent = dwClientID;
	MsgJoinFB.emType = sINFO.m_emType;

	MsgJoinFB.dwFieldSVR = m_dwFieldServerID;
	MsgJoinFB.dwGaeaID = dwGaeaID;
	MsgJoinFB.nChaNum = (int) pCHAR_DATA->m_dwCharID;
	
	SendAgent ( dwClientID, &MsgJoinFB );

	/*
	//	세션서버로 캐릭터정보 전송
	char* szUserID = m_pClientManager->GetUserID(dwClientID);

	NET_GAME_JOIN_OK ngjo;
	ngjo.nmg.dwSize	= sizeof(NET_GAME_JOIN_OK);
	ngjo.nmg.nType	= NET_MSG_GAME_JOIN_OK;
	ngjo.nUsrNum	= m_pClientManager->GetUserNum(dwClientID);								// 사용자 고유번호(DB)
	ngjo.nChaNum	= (int) pCHAR_DATA->m_dwCharID;											// 캐릭터번호(DB)
	ngjo.dwGaeaID	= pGLChar->m_dwGaeaID;													// 가이아 ID
	::StringCchCopyN(ngjo.szUserID,	 USR_ID_LENGTH+1, szUserID, USR_ID_LENGTH);				// 사용자 ID	
	::StringCchCopyN(ngjo.szChaName, CHR_ID_LENGTH+1, pCHAR_DATA->m_szName, CHR_ID_LENGTH);// 캐릭터명
	
	SendSession((LPVOID) &ngjo);
	*/
}

void CFieldServer::MsgFieldReqJoin( DWORD dwClientID, SCHARDATA2 * pCHAR_DATA, GLMSG::SNETJOIN2FIELDSERVER::SINFO & sINFO )
{
	if( !pCHAR_DATA ) return;

	//	Note : 클라이언트 메니져에 설정된 것과 틀릴 경우 취소됨.
	DWORD dwUserID = (DWORD) m_pClientManager->GetUserNum(dwClientID);

	//	케릭터의 사용자 ID 유효성 검사.
	//
	if ( dwUserID != (DWORD) pCHAR_DATA->GetUserID() )
	{
		// 캐릭터 DB에서 읽어온 UserID와 메시지의 UserID가 일치 하지 않음
		CConsoleMessage::GetInstance()->Write(_T("ERROR:Character UserID and Message UserID are unmatched"));
		return;
	}

	DWORD dwGaeaID = m_pClientManager->GetGaeaID(dwClientID);

	//	캐릭터 생성
	//
	PGLCHAR pGLChar = GLGaeaServer::GetInstance().CreatePC ( pCHAR_DATA, dwClientID, dwGaeaID, FALSE,
		&sINFO.m_sStartMap, sINFO.m_dwStartGate, sINFO.m_vStartPos, sINFO.m_emType );
	if ( !pGLChar )
	{
		CConsoleMessage::GetInstance()->Write(_T("ERROR:Initialized character instance failed"));
		return;
	}

	//	Note : action state 속성 변경.
	//
	GLMSG::SNETPC_ACTSTATE NetMsgInner;
	NetMsgInner.dwActState = sINFO.m_dwActState;
	pGLChar->MsgProcess ( (NET_MSG_GENERIC*) &NetMsgInner );

	//	Memo :	주/보조 무기 설정
	pGLChar->m_bUseArmSub = sINFO.m_bUseArmSub;

	//	Agent 에 케릭터의 접속 성공을 알림. ( Agent 가 Client 에게 새로운 Field 서버에 접속하게 요청. )
	//
	NET_GAME_JOIN_FIELDSVR_FB MsgJoinFB;
	MsgJoinFB.dwSlotFieldAgent = dwClientID;
	MsgJoinFB.emType = sINFO.m_emType;

	MsgJoinFB.dwFieldSVR = m_dwFieldServerID;
	MsgJoinFB.dwGaeaID = dwGaeaID;
	MsgJoinFB.nChaNum = (int) pCHAR_DATA->m_dwCharID;

	SendAgent ( dwClientID, &MsgJoinFB );
}

void CFieldServer::MsgSndChatGlobal(char* szChatMsg)
{
    if (szChatMsg == NULL) return;

	DWORD dwSndBytes = 0;
	CConsoleMessage::GetInstance()->Write(szChatMsg);
	NET_CHAT_FB ncf;
	ncf.emType = CHAT_TYPE_GLOBAL;
	StringCchCopyN(ncf.szChatMsg, CHAT_MSG_SIZE+1, szChatMsg, CHAT_MSG_SIZE);
	SendAllClient(&ncf);
}

/*
void CFieldServer::MsgLogIn(MSG_LIST* pMsg)
{	
	if (pMsg == NULL) return;

	NET_LOGIN_DATA* nml;
	DWORD dwFlags = 0;
	DWORD dwSndBytes = 0;
	DWORD dwClient;
	
	nml = (NET_LOGIN_DATA *) pMsg->Buffer;	
	dwClient = pMsg->dwClient;
	
	// 사용자 id 세팅
	m_pClientManager->SetUserID(dwClient, nml->szUserid);
	
	// 게임서버 -> 세션서버로 id / pwd 전송
	SessionSndLoginInfo(nml->szUserid, nml->szPassword, dwClient);	
}
*/

void CFieldServer::MsgVersion(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	DWORD					dwSndBytes = 0;
	NET_CLIENT_VERSION		ncv;

	DWORD dwClient			= pMsg->dwClient;

	ncv.nmg.dwSize			= sizeof(NET_CLIENT_VERSION);
	ncv.nmg.nType			= NET_MSG_VERSION_INFO;
	ncv.nGameProgramVer		= m_nVersion;
	ncv.nPatchProgramVer	= m_nPatchVersion;
	
	SendClient(dwClient, &ncv);
}

int CFieldServer::ExecuteCommand(char* strCmd)
{
    if (strCmd == NULL) return NET_ERROR;

	// 토큰으로 분리할 분리자.
	// space
	char seps[]   = " ";
	char *token;
	char* nToken = NULL;
//	char strTemp[100];

	// Establish string and get the first token
	token = strtok_s(strCmd, seps, &nToken);
	while (token != NULL)
	{
		if (strcmp(token, "print_ip") == 0)
		{
			// SERVER_UTIL::CAddressChecker::GetInstance()->Save();
			return 0;
		}		
		// 서버 채팅 메시지, global
		if (strcmp(token, _T("chat")) == 0)
		{
			token = strtok_s(NULL, seps, &nToken);
			if (token)
			{
				MsgSndChatGlobal(token);
				return 0;
			}
			else
			{
				return 0;
			}
		}
		//	Memo :	PROFILE 사용.
		if( strcmp(token, "profile_on") == 0 )
		{
			g_bProfile = TRUE;
			CConsoleMessage::GetInstance()->Write("PROFILE ON");
			return 0;
		}
		//	Memo :	PROFILE 사용안함.
		if( strcmp(token, "profile_off") == 0 )
		{
			g_bProfile = FALSE;
			CConsoleMessage::GetInstance()->Write("PROFILE OFF");
			return 0;
		}

		// Get next token
		token = strtok_s(NULL, seps, &nToken);
	}	
	return 0;
}

//                        ,     
//                   ,   /^\     ___
//                  /^\_/   `...'  /`
//               ,__\    ,'     ~ (
//            ,___\ ,,    .,       \
//             \___ \\\ .'.'   .-.  )
//               .'.-\\\`.`.  '.-. (
//              / (==== ."".  ( o ) \
//            ,/u  `~~~'|  /   `-'   )
//           "")^u ^u^|~| `""".  ~_ /
//             /^u ^u ^\~\     ".  \\
//     _      /u^  u ^u  ~\      ". \\
//    ( \     )^ ^U ^U ^U\~\      ". \\
//   (_ (\   /^U ^ ^U ^U  ~|       ". `\
//  (_  _ \  )U ^ U^ ^U ^|~|        ". `\.
// (_  = _(\ \^ U ^U ^ U^ ~|          ".`.;
//(_ -(    _\_)U ^ ^ U^ ^|~|            ""
//(_    =   ( ^ U^ U^ ^ U ~|
//(_ -  ( ~  = ^ U ^U U ^|~/
// (_  =     (_^U^ ^ U^ U /
//  (_-   ~_(/ \^ U^ ^U^,"
//   (_ =  _/   |^ u^u."  
//    (_  (/    |u^ u.(   
//     (__/     )^u^ u/ 
//             /u^ u^(  
//            |^ u^ u/   
//            |u^ u^(       ____   
//            |^u^ u(    .-'    `-,
//             \^u ^ \  / ' .---.  \
//              \^ u^u\ |  '  `  ;  |
//               \u^u^u:` . `-'  ;  |
//                `-.^ u`._   _.'^'./
//                   "-.^.-```_=~._/
//                      `"------"'
// jgkim
#include "pch.h"
#include "s_CAgentServer.h"
#include "../[Lib]__Engine/Sources/Common/StringUtils.h"
#include "../[Lib]__MfcEx/Sources/RanFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CAgentServer::InsertMsg(int nClient, void* pMsg)
{
	if (NULL != pMsg) {
		NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) pMsg;
		m_pRecvMsgManager->MsgQueueInsert(nClient, pNmg, pNmg->dwSize);
	}
	return 0;
}

int CAgentServer::MsgProcess(MSG_LIST* pMsg)
{	
	if (pMsg == NULL) return 0;

	DWORD dwClient=0, dwGaeaID=0;
	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) pMsg->Buffer;
	dwClient = pMsg->dwClient;
	dwGaeaID = m_pClientManager->GetGaeaID ( dwClient );
	
	// ������ ��� �޽���
	if ( (dwClient < NET_RESERVED_SLOT) && (dwClient >= 0) )
	{
		ServerMsgProcess( pMsg );
		return NET_OK;
	}

	switch (nmg->nType)
	{
	case NET_MSG_HEARTBEAT_CLIENT_ANS:
		{
			NET_HEARTBEAT_CLIENT_ANS* nmg2 = ( NET_HEARTBEAT_CLIENT_ANS* ) nmg;
			TCHAR* szEncKey = m_pClientManager->GetEncKey( pMsg->dwClient );
			if ( _tcslen( szEncKey ) > 0  )
			{				
				TCHAR szEncryptKey[ENCRYPT_KEY+1];
				memcpy( szEncryptKey, nmg2->szEnCrypt, sizeof(TCHAR) * ENCRYPT_KEY+1 );
				
				m_Tea.decrypt (szEncryptKey, ENCRYPT_KEY+1);
				if ( _tcscmp( szEncryptKey, szEncKey ) )
				{
					CConsoleMessage::GetInstance()->Write( _T("Error HeartBit Error %s, %s , UserID : %s, CloseClient( %d )"), 
															   szEncryptKey, szEncKey, m_pClientManager->GetUserID( pMsg->dwClient ), pMsg->dwClient );					
					break;
				}
			}

			m_pClientManager->SetEncKey( pMsg->dwClient, nmg2->szEnCrypt );
			m_pClientManager->SetHeartBeat( pMsg->dwClient );
		}
		break;

	// Client->Agent : �α��� ��û ����Ÿ�϶�...		
	// Taiwan / Hongkong
	case NET_MSG_LOGIN_2 :					MsgLogIn( pMsg );					break;
	// China
	case CHINA_NET_MSG_LOGIN:				ChinaMsgLogin( pMsg );				break;
	// Thailand �α��� ��û 
	case THAI_NET_MSG_LOGIN :				ThaiMsgLogin( pMsg );				break;	
	case DAUM_NET_MSG_LOGIN :				DaumMsgLogin( pMsg );				break;
	case TERRA_NET_MSG_LOGIN:				TerraMsgLogin( pMsg );				break;
	case GSP_NET_MSG_LOGIN:					GspMsgLogin( pMsg );				break;
	case EXCITE_NET_MSG_LOGIN:				ExciteMsgLogin( pMsg );				break;
	// �Ϻ� ���񽺻� ����(Gonzo)�� ���� �߰�
	case JAPAN_NET_MSG_LOGIN:				JapanMsgLogin( pMsg );				break;
	case GS_NET_MSG_LOGIN:					GsMsgLogin( pMsg );					break;

	// Daum 2�� �н����� üũ
	case DAUM_NET_MSG_PASSCHECK:			DaumMsgPassCheck( pMsg );			break;	
	// ���������� 2�� �н����� üũ
	case TERRA_NET_MSG_PASSCHECK:			TerraMsgPassCheck( pMsg );			break;

	// �Ϻ� 2�� �н����� üũ
	case EXCITE_NET_MSG_PASSCHECK:			ExciteMsgPassCheck( pMsg );			break;

	// Client->Agent : ���ο� ĳ���� ����
	case NET_MSG_CHA_NEW :					MsgCreateCharacter( pMsg );			break;
	case NET_MSG_REQ_RAND_KEY:				MsgSndRandomNumber( dwClient );		break;
	// Client->Agent : ����� (���÷��̿�)  ĳ���� �������� ��û
	case NET_MSG_REQ_CHA_BAINFO :			MsgSndChaBasicBAInfo (pMsg);		break;
	// Client->Agent : Ư��ĳ���� �������� ��û (�κ� ���÷��̿�)
	case NET_MSG_REQ_CHA_BINFO : 			MsgSndChaBasicInfo(pMsg);			break;
	// Client->Agent : ĳ���� ����
	case NET_MSG_CHA_DEL :					MsgSndChaDelInfo( pMsg );			break;
	case DAUM_NET_MSG_CHA_DEL :				DaumMsgSndChaDelInfo(pMsg);			break;
	case TERRA_NET_MSG_CHA_DEL :			TerraMsgSndChaDelInfo(pMsg);		break;
	case EXCITE_NET_MSG_CHA_DEL :			ExciteMsgSndChaDelInfo(pMsg);		break;
	case GSP_NET_MSG_CHA_DEL:				GspMsgSndChaDelInfo( pMsg );		break;
	case JAPAN_NET_MSG_CHA_DEL:				JapanMsgSndChaDelInfo(pMsg);		break;
	case GS_NET_MSG_CHA_DEL:				GsMsgSndChaDelInfo(pMsg);			break;

	// Client->Agent : ĳ���� ������ ����
	case NET_MSG_LOBBY_GAME_JOIN : 			MsgGameJoin(pMsg);					break;
	// DB ������->������Ʈ���� : DB���� �о��� ������ �ɸ��� ���� �õ�.
	case NET_MSG_AGENT_REQ_JOIN:			MsgAgentReqJoin(pMsg);				break;
	// Field->Agent : ĳ���Ͱ� �������� 
	case MET_MSG_GAME_JOIN_FIELDSVR_FB :	MsgLobbyCharJoinField(pMsg);		break;		
	// Apex Data �޼���	// Apex ����
	case NET_MSG_APEX_DATA:					MsgApexData(pMsg);					break;
	// Apex ����( ȫ�� )
	case NET_MSG_APEX_RETURN:				MsgApexReturn(pMsg);				break;
	
	// GameGuard ������ �޼���
	case NET_MSG_GAMEGUARD_ANSWER:			MsgGameGuardAnswer(pMsg);			break;
	// GameGuard ������ �޼��� #1
	case NET_MSG_GAMEGUARD_ANSWER_1:		MsgGameGuardAnswerFirst( pMsg );	break;
	// GameGuard ������ �޼��� #2
	case NET_MSG_GAMEGUARD_ANSWER_2:		MsgGameGuardAnswerSecond( pMsg );	break;
	// �ʵ弭���� Ŭ���̾�Ʈ���� ���� ���� �α׸� ��´�.
	case NET_MSG_LOG_UPDATE_TRACING_CHARACTER: MsgTracingLogPrint( pMsg );		break;


	case NET_MSG_GCTRL_ACTSTATE:
	case NET_MSG_CHAT:
	case NET_MSG_GCTRL_REQ_REBIRTH:
	case NET_MSG_GCTRL_REQ_RECALL_AG:
	case NET_MSG_REQ_MUST_LEAVE_MAP_AG:
	case NET_MSG_GCTRL_REQ_TELEPORT_AG:
	case NET_MSG_GCTRL_REGEN_GATE_FB:
	case NET_MSG_GCTRL_CREATE_INSTANT_MAP_FB:
	case NET_MSG_GCTRL_REQ_GATEOUT_FB:
	case NET_MSG_GCTRL_REQ_REBIRTH_FB:
	case NET_MSG_GCTRL_REQ_SERVER_CLUB_BATTLE_REMAIN_AG:
	case NET_MSG_GCTRL_REQ_SERVER_CLUB_DEATHMATCH_REMAIN_AG:
	
	case NET_MSG_GCTRL_PARTY_LURE:
	case NET_MSG_GCTRL_PARTY_LURE_TAR_ANS:
	case NET_MSG_GCTRL_PARTY_SECEDE:
	case NET_MSG_GCTRL_PARTY_AUTHORITY:
	case NET_MSG_GCTRL_PARTY_DISSOLVE:

	case NET_MSG_GCTRL_CONFRONT:
	case NET_MSG_GCTRL_CONFRONT_ANS:
	case NET_MSG_GCTRL_CONFRONT_END2_AGT:
	case NET_MSG_GCTRL_CONFRONT_FB:
	
	case NET_MSG_GCTRL_CLUB_NEW_2AGT:
	case NET_MSG_GCTRL_CLUB_NEW_DB2AGT:
	case NET_MSG_GCTRL_CLUB_RANK_2AGT:
	case NET_MSG_GCTRL_CLUB_MEMBER_DB2DEL:
	case NET_MSG_CHARPOS_FROMDB2AGT:

	case NET_MSG_GCTRL_CLUB_DISSOLUTION:
	case NET_MSG_GCTRL_CLUB_MEMBER_REQ_2AGT:
	case NET_MSG_GCTRL_CLUB_MEMBER_DEL:
	case NET_MSG_GCTRL_CLUB_MEMBER_SECEDE:
	case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ:
	case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ANS:
	case NET_MSG_GCTRL_CLUB_MARK_INFO:
	case NET_MSG_GCTRL_CLUB_MARK_CHANGE:

	case NET_MSG_GCTRL_CLUB_COMMISSION:
	case NET_MSG_GCTRL_CLUB_NOTICE_REQ:
	case NET_MSG_GCTRL_CLUB_SUBMASTER:

	case NET_MSG_REQ_FRIENDLIST:
	case NET_MSG_REQ_FRIENDADD:
	case NET_MSG_REQ_FRIENDADD_ANS:
	case NET_MSG_REQ_FRIENDDEL:
	case NET_MSG_REQ_FRIENDBLOCK:

	case NET_MSG_GCTRL_2_FRIEND_REQ:

	case NET_MSG_SERVER_CTRL_WEATHER:
	case NET_MSG_SERVER_CTRL_WEATHER2:
	case NET_MSG_SERVER_CTRL_GENITEMHOLD:
	case NET_MSG_SERVER_CTRL_TIME:
	case NET_MSG_SERVER_CTRL_MONTH:


	/////////////////////////////////////////
	// �̺�Ʈ ���� 
	case NET_MSG_GM_EVENT_ITEM_GEN:
	case NET_MSG_GM_EVENT_ITEM_GEN_END:

	case NET_MSG_GM_EVENT_MONEY_GEN:
	case NET_MSG_GM_EVENT_MONEY_GEN_END:

	case NET_MSG_GM_EVENT_EXP:				
	case NET_MSG_GM_EVENT_EXP_END:			

	case NET_MSG_GM_CLASS_EVENT:

	case NET_MSG_GM_EVENT_EX:
	case NET_MSG_GM_EVENT_EX_END:

	case NET_MSG_GM_LIMIT_EVENT_BEGIN:
	case NET_MSG_GM_LIMIT_EVENT_END:
	case NET_MSG_GM_LIMIT_EVENT_RESTART:
	case NET_MSG_GM_LIMIT_EVENT_TIME_REQ:

	case NET_MSG_CYBERCAFECLASS_UPDATE:

	case NET_MSG_GM_VIEWWORKEVENT:

	/////////////////////////////////////////
	case NET_MSG_GM_BIGHEAD:
	case NET_MSG_GM_BIGHAND:
	case NET_MSG_GM_MOVE2GATE:
	case NET_MSG_GM_MOVE2MAPPOS:
	case NET_MSG_GM_SHOWMETHEMONEY:

	case NET_MSG_GM_FREEPK:
	case NET_MSG_GM_VIEWALLPLAYER:
	case NET_MSG_GM_GENCHAR:
	case NET_MSG_GM_MOVE2CHAR:
	case NET_MSG_GM_MOB_GEN:
	case NET_MSG_GM_MOB_GEN_EX:
	case NET_MSG_GM_MOB_DEL_EX:
	case NET_MSG_GM_MOB_DEL:
	case NET_MSG_GM_MOB_LEVEL:
	case NET_MSG_GM_MOB_LEVEL_CLEAR:
	case NET_MSG_GM_WHERE_NPC:
	case NET_MSG_GM_PRINT_CROWLIST:

	case NET_MSG_GM_GETWHISPERMSG:
	case NET_MSG_GCTRL_REQ_TAXI:

	case NET_MSG_GM_CHAT_BLOCK_UACCOUNT:
	case NET_MSG_GM_CHAT_BLOCK_CHARNAME:
	case NET_MSG_GM_CHAT_BLOCK_CHARID:
	case NET_MSG_GM_CHAR_INFO_4NAME:
	case NET_MSG_GM_CHAR_INFO_4CHARID:

	case NET_MSG_USER_CHAR_INFO_4NAME:

	case NET_MSG_BLOCK_DETECTED:
	case NET_MSG_GM_KICK_USER:

	case NET_MSG_CHAT_LOUDSPEAKER_AGT:
	case NET_MSG_GCTRL_INVEN_RENAME_AGTBRD:
	case NET_MSG_SMS_PHONE_NUMBER_AGTBRD:

	case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ANS:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_REQ:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_REQ:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_REQ:

	case NET_MSG_GCTRL_CLUB_BATTLE_REQ:
	case NET_MSG_GCTRL_CLUB_BATTLE_REQ_ANS:
	case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ:
	case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ANS:
	case NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ:

	case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ANS:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ:

	case NET_MSG_GCTRL_UPDATE_STARTCALL:

	case NET_MSG_SMS_SEND:
	case NET_MSG_SMS_SEND_FROM_DB:

	case NET_MSG_GM_WARNING_MSG:
	case NET_MSG_GCTRL_PUTON_CHANGE_AG:


	case NET_MSG_REQ_FRIEND_CLUB_OPEN:

	case NET_MSG_MARKET_CHECKTIME:

	case NET_QBOX_OPTION_REQ_AG:
	case NET_MSG_REQ_ATTENDLIST:
	case NET_MSG_REQ_ATTENDANCE:

		{
			//	Note : ���� (dwMAX_CLIENT) ���� ū ID ��� �ʵ� �������� �����Ͽ���.
			//		�� ID�� Ŭ���̾�Ʈ ID�� ��ȯ.
			DWORD dwAgentClient = dwClient;
			DWORD dwMAX_CLIENT = (DWORD) m_pClientManager->GetMaxClient();
			if ( dwMAX_CLIENT <= dwAgentClient )	dwAgentClient -= dwMAX_CLIENT;

			GLAgentServer::GetInstance().MsgProcess ( nmg, dwAgentClient, dwGaeaID );
		}
		break;


	case NET_MSG_GM_WHERE_PC_MAP:			MsgGmwherePcMap ( nmg, dwClient, dwGaeaID );	break;
	case NET_MSG_SERVER_PLAYERKILLING_MODE:	MsgGmPkMode ( nmg, dwClient, dwGaeaID );		break;

	case NET_MSG_VIETNAM_TIME_RESET:		MsgVietTimeReset( nmg, dwClient, dwGaeaID );  break; //vietnamtest%%% && vietnamtest2

	case NET_MSG_GCTRL_SET_SERVER_DELAYTIME_AGT:
	case NET_MSG_GCTRL_SET_SERVER_CRASHTIME_AGT:
	case NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_AGT:
		{
			DWORD dwAgentClient = dwClient - m_pClientManager->GetMaxClient();
			MsgAttackClientCheck( nmg, dwAgentClient, dwGaeaID );
		}
		break;


	case NET_MSG_GCTRL_FIELDSVR_OUT_FB:
		{
			DWORD dwAgentClient = dwClient - m_pClientManager->GetMaxClient();
			MsgFieldSvrOutFb ( nmg, dwAgentClient, dwGaeaID );
		}
		break;

	case NET_MSG_GCTRL_REBIRTH_OUT_FB:
		{
			DWORD dwAgentClient = dwClient - m_pClientManager->GetMaxClient();
			MsgReBirthOutFb ( nmg, dwAgentClient, dwGaeaID );
		}
		break;

	case NET_MSG_GCTRL_REQ_RECALL_FB:
		{
			DWORD dwAgentClient = dwClient - m_pClientManager->GetMaxClient();
			MsgReCallFb ( nmg, dwAgentClient, dwGaeaID );
		}
		break;
	case NET_MSG_GCTRL_REQ_TELEPORT_FB:
		{
			DWORD dwAgentClient = dwClient - m_pClientManager->GetMaxClient();
			MsgTeleportFb ( nmg, dwAgentClient, dwGaeaID );
		}
		break;
	case NET_MSG_GCTRL_INVEN_PREMIUMSET_FB:
		{
			DWORD dwAgentClient = dwClient - m_pClientManager->GetMaxClient();
			MsgPremiumSetFb ( nmg, dwAgentClient, dwGaeaID );
		}
		break;

	case NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_AGT:
		{
			PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
			if ( !pChar )							return FALSE;
			if ( pChar->m_dwUserLvl < USER_GM1 )	return FALSE;

			GLMSG::SNET_SERVER_LOGINMAINTENANCE_AGT* pNetMsg = ( GLMSG::SNET_SERVER_LOGINMAINTENANCE_AGT* ) nmg;

			SNET_SERVER_LOGINMAINTENANCE_SESSION msg;
			msg.bOn = pNetMsg->bOn;
			SendSession( &msg );			
		}
		break;

	default:
		{
			if (nmg->nType==NET_MSG_GCTRL_GOTO )
			{
				BOOL bOk = FALSE;
			}

			if (pMsg->dwClient >= (DWORD) m_nMaxClient)
			{
				// �������� �� �޽��� -> Ŭ���̾�Ʈ�� ������ �Ѵ�.
				m_pClientManager->Send(pMsg->dwClient-m_nMaxClient, (LPVOID) pMsg->Buffer);
			}
			else
			{
				// Ŭ���̾�Ʈ ���� �� �޽��� -> ������ ������ �Ѵ�.
				m_pClientManager->Send(pMsg->dwClient+m_nMaxClient, (LPVOID) pMsg->Buffer);
			}
		}
		break;
	}
	return 0;
}

void CAgentServer::ServerMsgProcess(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_MSG_GENERIC *nmg = (NET_MSG_GENERIC *) pMsg->Buffer;
	
	if (nmg == NULL) return;

	//if (dwClient == m_dwSession)
	//{
	//	SessionMsgProcess( pMsg );
	//}
	//else
	//{	
	//	OtherMsgProcess( pMsg );
	//}

	switch ( dwClient )
	{
	case NET_RESERVED_SESSION:
		SessionMsgProcess( pMsg );
		break;	
	default :
		OtherMsgProcess( pMsg );
		break;
	}
}

void CAgentServer::OtherMsgProcess(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;
	
	DWORD dwClient = pMsg->dwClient;
	NET_MSG_GENERIC *nmg = (NET_MSG_GENERIC *) pMsg->Buffer;

	if (nmg == NULL) return;

	switch (nmg->nType)
	{
	case NET_MSG_HEARTBEAT_SERVER_ANS:
		m_pClientManager->SetHeartBeat( dwClient );
		break;
	case NET_MSG_GCTRL_REQ_GATEOUT:
	case NET_MSG_GCTRL_REQ_GENITEM_AGT:
	case NET_MSG_FIELDSVR_CHARCHK_FB:
	case NET_MSG_SERVER_GENERALCHAT:
	case NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_AGT:
	case NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_AGT:
	case NET_MSG_GCTRL_CLUB_CERTIFIED_AGT:
	case NET_MSG_GCTRL_CLUB_INCOME_MONEY_AGT:
	case NET_MSG_GCTRL_CLUB_INCOME_UP:
	//case NET_MSG_GCTRL_CLUB_INCOME_FB:
	case NET_MSG_GCTRL_2_FRIEND_AG:
	case NET_MSG_GM_MOVE2CHAR_AG:
	case NET_MSG_GM_CHAR_INFO_FLD_FB:
	case NET_MSG_USER_CHAR_INFO_FLD_FB:
	case NET_MSG_GCTRL_CLUB_CD_CERTIFY_ING_BRD:
	case NET_MSG_GCTRL_CLUB_CD_CERTIFY_BRD:
	case NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_AGT:
	case NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_AGT:
	case NET_MSG_GCTRL_REQ_TAXI_NPCPOS_FB:
	case NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ:
	case NET_MSG_GCTRL_CREATE_INSTANT_MAP_DEL:
	case NET_MSG_GCTRL_NPC_COMMISSION:
		GLAgentServer::GetInstance().MsgProcess( nmg, dwClient, GAEAID_NULL );
		break;

	default :
		break;
	};
}

void CAgentServer::MsgSndRandomNumber( DWORD dwClient )
{
	int nRandomNumber = m_pClientManager->GetRandomPassNumber( dwClient );
	if ( nRandomNumber <= 0 )
	{
		nRandomNumber = SERVER_UTIL::makeRandomNumber( MAX_RANDOM_PASSWORD_NUM );
		m_pClientManager->SetRandomPassNumber( dwClient, nRandomNumber );
	}
	NET_RANDOMPASS_NUMBER tempMsg;
	tempMsg.nRandomNumber = nRandomNumber;	
	SendClient(dwClient, &tempMsg);
}

void CAgentServer::MsgSndCryptKey(DWORD dwClient)
{
	CRYPT_KEY ck = m_pClientManager->GetNewCryptKey(); // Get crypt key
	
	m_pClientManager->SetCryptKey(dwClient, ck); // Set crypt key
	
	NET_CRYPT_KEY nck;
	nck.ck = ck;
	
	// In Taiwan some user can't receive crypt key
	// Send crypt key 3 times.
	SendClient(dwClient, &nck);
	// SendClient(dwClient, &nck);
	// SendClient(dwClient, &nck);
}

// ���Ӽ���->Ŭ���̾�Ʈ
// �Ϲ��� ä�� �޽��� ����
void CAgentServer::MsgSndChatNormal(DWORD dwClient, const char* szName, const char* szMsg)
{
	if ( (szName == NULL) || (szMsg == NULL) ) return;

    NET_CHAT nc;
	nc.nmg.nType = NET_MSG_CHAT;
	nc.emType    = CHAT_TYPE_NORMAL;
	::StringCchCopy(nc.szName, CHR_ID_LENGTH+1, szName);
	::StringCchCopy(nc.szChatMsg, CHAT_MSG_SIZE+1, szMsg);

	SendClient(dwClient, (NET_MSG_GENERIC*) &nc);
}

///////////////////////////////////////////////////////////////////////////////
// Ŭ���̾�Ʈ->���Ӽ��� : ĳ���� ������ ����
void CAgentServer::MsgGameJoin(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;

	// nProtect ���� ������ ����Ѵٸ� ĳ���Ͱ� �����Ҷ� ������ �Դ��� üũ�Ѵ�.	
	if (m_bGameGuardInit == true && 
		m_bGameGuardAuth == true)
	{
		if ( m_pClientManager->nProtectFirstAuthPass( dwClient ) == false )		
		{
			// nProtect ���� ����
			// ������ �����Ų��.
			CloseClient(dwClient);
			return;
		}
		else
		{
			// ���� nProtect �ޱ���¶��
			if (m_bHeartBeat == false)
			{
				// �ٽ� �ѹ� �������� ����
				NET_GAMEGUARD_AUTH nga;
				// �������¸� ���� ��Ų��.
				m_pClientManager->nProtectResetAuth(dwClient);
				// ���Ӱ��� ���� ���� �������� �غ�
				if (m_pClientManager->nProtectSetAuthQuery(dwClient) == true)
				{
					// ���Ӱ��� ���� ���� ����
					nga.ggad = m_pClientManager->nProtectGetAuthQuery(dwClient);
					SendClient(dwClient, &nga);
				}
			}
		}
	}

	// ������ ������� ���� ���¿��� ĳ���� ������û��
	if (m_pClientManager->IsAccountPass(dwClient) == false)
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("INFO:MsgGameJoin IsAccountPass false"));
		CloseClient(dwClient);
		return;
	}

	NET_GAME_JOIN* pNgj = reinterpret_cast<NET_GAME_JOIN*> (pMsg->Buffer);

	if (pNgj == NULL) return;	
	DWORD dwChaNum  = (DWORD) pNgj->nChaNum;
	DWORD dwUserID  = (DWORD) m_pClientManager->GetUserNum(dwClient);
	DWORD dwUserLvl = (DWORD) m_pClientManager->GetUserType(dwClient);
    __time64_t tPREMIUM   = m_pClientManager->GetPremiumDate(dwClient);
    __time64_t tCHATBLOCK = m_pClientManager->GetChatBlockDate(dwClient);
    
	// ĳ���� ��ȣ ����
	m_pClientManager->SetChaNum(dwClient, dwChaNum);

	//	Note : db action�� �ɸ��� ���� �������� ����, ���� �ɸ��� ����.
	//
	CGetChaInfoAndJoin *pDBAction = new CGetChaInfoAndJoin;
	pDBAction->Init ( dwClient, dwUserID, dwUserLvl, tPREMIUM, tCHATBLOCK, (DWORD)m_nServerGroup, dwChaNum );
	COdbcManager::GetInstance()->AddJob ( pDBAction );
}

void CAgentServer::MsgAgentReqJoin(MSG_LIST* pMsg)
{
	if (pMsg == NULL)			return;
	GLMSG::SNETJOIN2AGENTSERVER* pNgj = reinterpret_cast<GLMSG::SNETJOIN2AGENTSERVER*> (pMsg->Buffer);
	if (pNgj == NULL)			return;

	//	[����] �� ȣ�� auto_ptr ������ ���� ���� ȣ��Ǿ�� �Ѵ�.
	//	�� �Լ����� Ż��ÿ� �ڵ����� ���ŵǰ� �Ѵ�. ( �޽��� ó���� ��ȿȭ. )
	//
	std::auto_ptr<GLCHARAG_DATA> pCHAR_DATA ( pNgj->m_pCharAgData );
	if ( !pCHAR_DATA.get() )	return;

	//	�޽����� ���޵� clientid.
	DWORD dwClientID = pNgj->m_dwClientID;
	if ( !m_pClientManager->IsAccountPass(dwClientID) ) return;

	//	Note : Ŭ���̾�Ʈ �޴����� ������ �Ͱ� Ʋ�� ��� ��ҵ�.
	DWORD dwCharNum = (DWORD) m_pClientManager->GetChaNum(dwClientID);
	if ( dwCharNum!=pCHAR_DATA->m_dwCharID )
	{
		CConsoleMessage::GetInstance()->Write( _T("ERROR:dwCharNum!=pCHAR_DATA->m_dwCharID") );
		return;
	}

	// strcpy ( pCHAR_DATA->m_szUserName, m_pClientManager->GetUserID(dwClientID) );
	StringCchCopy(pCHAR_DATA->m_szUserName, USR_ID_LENGTH+1, m_pClientManager->GetUserID(dwClientID));
	strcpy_s(pCHAR_DATA->m_szIp, m_pClientManager->GetClientIP(dwClientID));

	int nChannel = m_pClientManager->GetChannel(dwClientID);

	bool bPKCHANNEL = CCfg::GetInstance()->IsPkChannel ( nChannel );

	if ( !bPKCHANNEL && (pCHAR_DATA->m_nBright < GLCONST_CHAR::sPK_STATE[0].nPKPOINT) )
	{
		//	���� �õ��ڿ���  �޽����� �����ϴ�.
		GLMSG::SNETLOBBY_CHARJOIN_FB NetMsgFB;
		NetMsgFB.emCharJoinFB = EMCJOIN_FB_PKPOINT;
		m_pClientManager->SendClient( dwClientID, &NetMsgFB );
		return;
	}

	INT dwThaiCCafeClass	= 0;
	// �����̽þ� PC�� �̺�Ʈ
	INT nMyCCafeClass		= 0;

#if defined(TH_PARAM)
	dwThaiCCafeClass = m_pClientManager->GetThaiClass(dwClientID);
#endif

	// �����̽þ� PC�� �̺�Ʈ
#if defined(MYE_PARAM) || defined(MY_PARAM)
	nMyCCafeClass = m_pClientManager->GetMyClass(dwClientID);
#endif

	// �߱� ���� �ð� ����
	SChinaTime sChinaTime;
	sChinaTime.Init();

	// ���� �߱��� �ƴϴ��� �̺�Ʈ �ð��� ���� ���� �α����� �ð��� ���Ѵ�.
	// ���� �α����� �ð�
	CTime crtTime			   = CTime::GetCurrentTime();
	sChinaTime.loginTime	   = crtTime.GetTime();

	//////////////////////////////////////////////////////////////////////////
	// �߱� Ž�� ��� �ý��� ���
#ifdef CH_PARAM_USEGAIN //chinaTest%%%
	// ���� ������� �ƴ���
	sChinaTime.userAge   = m_pClientManager->GetChinaUserAge( dwClientID );

	// ���� �÷��� ���� �ð�
	sChinaTime.gameTime		   = m_pClientManager->GetChinaGameTime(dwClientID);

	// �������� ���� �ð�
	sChinaTime.offlineTime     = m_pClientManager->GetChinaOfflineTime(dwClientID);
	CTimeSpan offlineSpan( 0, sChinaTime.offlineTime / 60, sChinaTime.offlineTime % 60, 0 );

	// ������ �α׿��� �ð�
	sChinaTime.lastOffLineTime = m_pClientManager->GetLastLoginDate(dwClientID);

	// ���� ���� ���� �ð�
	sChinaTime.currentGameTime = 0;

	if( sChinaTime.lastOffLineTime != 0 )
	{
		CTime lastOffTime		   = sChinaTime.lastOffLineTime;	
		CTimeSpan totalOffTime = (crtTime - lastOffTime) + offlineSpan;
		CTimeSpan totalGameTime( 0, (int)sChinaTime.gameTime / 60, (int)sChinaTime.gameTime % 60, 0 );
		// �α׿����� �ð��� 5�ð����� ũ�ų� ������ �����ð��� �ʱ�ȭ�Ѵ�.
		if( totalOffTime.GetTotalHours() >= 5 )		
		//if( totalOffTime.GetTotalMinutes() >= 3 )		
		{
			sChinaTime.offlineTime	   = 0;
			sChinaTime.gameTime		   = 0;
			sChinaTime.lastOffLineTime = 0;
			sChinaTime.currentGameTime = 0;


			m_pClientManager->SetChinaOfflineTime(dwClientID, 0 );
			m_pClientManager->SetChinaGameTime(dwClientID, 0 );
		}else{
			if( totalGameTime.GetTotalHours() >= 5 && sChinaTime.userAge != 1 )
			//if( totalGameTime.GetTotalMinutes() >= 1 && sChinaTime.userAge != 1 )
			{
				// �߱� �α��ν� �÷��� �����ð��� 5�ð��� �Ѱ� �������� �����ð��� 5�ð��� ���� �ʾ��� ��� ���� �޽����� ������.
				GLMSG::SNETLOBBY_CHARJOIN_CHINA_ERROR NetMsg;
				m_pClientManager->SendClient( dwClientID, &NetMsg );
				return;

			}else{
				m_pClientManager->SetChinaOfflineTime(dwClientID, (int)totalOffTime.GetTotalMinutes() );
			}
		}
	}else{
		sChinaTime.offlineTime	   = 0;
		sChinaTime.gameTime		   = 0;
		sChinaTime.lastOffLineTime = 0;
		sChinaTime.currentGameTime = 0;
	}



#endif

	//////////////////////////////////////////////////////////////////////////
	// ��Ʈ�� Ž�� ���� �ý��� ���
	SVietnamGainSystem sVietnamTime;
	sVietnamTime.Init();
#if defined(VN_PARAM) //vietnamtest%%%
	// ���� �α����� �ð�
	crtTime					   = CTime::GetCurrentTime();
	sVietnamTime.loginTime	   = crtTime.GetTime();

	// ���� �÷��� ���� �ð�
	sVietnamTime.gameTime		 = m_pClientManager->GetVTGameTime(dwClientID);

	// ������ �α׿��� �ð�
	sVietnamTime.lastOffLineTime = m_pClientManager->GetLastLoginDate(dwClientID);

	// ���� ���� ���� �ð�
	sVietnamTime.currentGameTime = 0;

	if( sVietnamTime.lastOffLineTime != 0 )
	{
		CTime lastOffTime		   = sVietnamTime.lastOffLineTime;	
		// ���������� ��¥�� �ٸ��� �ʱ�ȭ �Ѵ�.
		bool bInitialize = FALSE; // �׽�Ʈ�� ������ �ʱ�ȭ �Ѵ�.

		// �⵵, ��, ��¥�� �ٸ��� �ʱ�ȭ �Ѵ�.
		if( lastOffTime.GetYear() != crtTime.GetYear() ||
			lastOffTime.GetMonth() != crtTime.GetMonth() ||
			lastOffTime.GetDay() != crtTime.GetDay()  )
		{
//			CConsoleMessage::GetInstance()->Write( _T("INFO:Vietnam Accumulation Time Init Last Off Time Month %d, Day %d CharID[%s], CharAccount[%s]" ), 
//												   lastOffTime.GetMonth(), lastOffTime.GetDay(), pCHAR_DATA->m_szName, pCHAR_DATA->m_szUserName  );
			bInitialize = TRUE;
		}

		if( sVietnamTime.gameTime < 0 )
		{
//			CConsoleMessage::GetInstance()->Write( _T("INFO:Vietnam GameTime not correct value...") );
			bInitialize = TRUE;
		}
			
		if( bInitialize )
		{
			sVietnamTime.gameTime		 = 0;
			sVietnamTime.lastOffLineTime = 0;
			sVietnamTime.currentGameTime = 0;

			m_pClientManager->SetVTGameTime(dwClientID, 0 );

		}
	}else{
		sVietnamTime.gameTime		 = 0;
		sVietnamTime.lastOffLineTime = 0;
		sVietnamTime.currentGameTime = 0;

		m_pClientManager->SetVTGameTime(dwClientID, 0 );
	}


	pCHAR_DATA->m_sVietnamSystem = sVietnamTime;

#endif

	if( CheckTracingUser( pCHAR_DATA->m_szUserName, pCHAR_DATA->m_dwUserID ) != -1 )
	{
		pCHAR_DATA->m_bTracingUser   = TRUE;
	}


	//*/*/*/*
	//	ĳ���� ����
	PGLCHARAG pChar = GLAgentServer::GetInstance().CreatePC( pCHAR_DATA.get(), dwClientID, nChannel, dwThaiCCafeClass, sChinaTime, nMyCCafeClass );
	if ( !pChar )
	{
		//	���� �õ��ڿ���  �޽����� �����ϴ�.
		GLMSG::SNETLOBBY_CHARJOIN_FB NetMsgFB;
		NetMsgFB.emCharJoinFB = EMCJOIN_FB_ERROR;
		m_pClientManager->SendClient ( dwClientID, &NetMsgFB );

		CConsoleMessage::GetInstance()->Write( _T("ERROR:Initialize character instance failed") );
		CConsoleMessage::GetInstance()->Write( _T("ClientID[%u], Channel[%d]"), dwClientID, nChannel );
		CConsoleMessage::GetInstance()->Write( _T("CharIP[%s], CharIDNum[%u], UserIDNum[%u], CharID[%s], CharAccount[%s]"),
			pCHAR_DATA->m_szIp,
			pCHAR_DATA->m_dwCharID,
			pCHAR_DATA->m_dwUserID,
			pCHAR_DATA->m_szName,
			pCHAR_DATA->m_szUserName );
		return;
	}
	

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	if( pCHAR_DATA->m_bTracingUser )
	{
		CDebugSet::ToTracingFile( pCHAR_DATA->m_szUserName, "Character Select!, [%s][%s], IP:[%s], Channel:[%d], UserNum:[%u], CharNum:[%u]",
								  pCHAR_DATA->m_szUserName, pCHAR_DATA->m_szName, pCHAR_DATA->m_szIp, pChar->m_nChannel, pCHAR_DATA->m_dwUserID, pCHAR_DATA->m_dwCharID );
	}
#endif


	//	���̾Ƽ����� ���� ����
	m_pClientManager->SetGaeaID( dwClientID, pChar->m_dwGaeaID );
	
	// ĳ���� ���� �¶���
	CSetCharacterOnline* pDBAction = new CSetCharacterOnline(
											m_pClientManager->GetChaNum(dwClientID) );
	COdbcManager::GetInstance()->AddJob(pDBAction);	
	
	// User DB �� �÷��� ���� ĳ���� �̸��� �����Ѵ�.
	if( pChar != NULL )
	{	
		CUserUpdateCha* pAction2 = new CUserUpdateCha(
										m_pClientManager->GetUserNum( dwClientID),
										pChar->m_szName );
										
		COdbcManager::GetInstance()->AddUserJob( (CDbAction*) pAction2 );
	}
	///////////////////////////////////////////////////////////////////////////
	// ���Ǽ����� ĳ�������� ����
	/*
	NET_GAME_JOIN_OK ngjo;
	ngjo.nmg.dwSize	= sizeof(NET_GAME_JOIN_OK);
	ngjo.nmg.nType	= NET_MSG_GAME_JOIN_OK;
	ngjo.nUsrNum	= pChar->m_dwUserID;
	ngjo.nChaNum	= pChar->m_dwCharID;
	ngjo.dwGaeaID	= pChar->m_dwGaeaID;
	::StringCchCopyN ( ngjo.szUserID, USR_ID_LENGTH, m_pClientManager->GetUserID(dwClientID), USR_ID_LENGTH);
	::StringCchCopyN ( ngjo.szChaName, CHR_ID_LENGTH, pCHAR_DATA->m_szName, CHR_ID_LENGTH);

	// ���Ǽ����� ����
	SendSession((LPVOID) &ngjo);
	*/
}


///////////////////////////////////////////////////////////////////////////////
// Field->Agent : ĳ���Ͱ� ���ӿ� ��������
int CAgentServer::MsgLobbyCharJoinField(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return NET_ERROR;

	DWORD dwClient = pMsg->dwClient - (DWORD) m_nMaxClient;
	NET_GAME_JOIN_FIELDSVR_FB* pMsgJoinFB = (NET_GAME_JOIN_FIELDSVR_FB*) pMsg->Buffer;

	if (pMsgJoinFB == NULL) return NET_ERROR;

	int nChannel = m_pClientManager->GetChannel(dwClient);
    
	F_SERVER_INFO* pField = CCfg::GetInstance()->GetFieldServer ( (int) pMsgJoinFB->dwFieldSVR, nChannel );
	
	// Check GaeaID
	if ( pMsgJoinFB->dwGaeaID != m_pClientManager->GetGaeaID(dwClient) )		return NET_ERROR;
	m_pClientManager->SetSlotFieldAgent ( dwClient, pMsgJoinFB->dwSlotFieldAgent );
	
	NET_CONNECT_CLIENT_TO_FIELD MsgConnect;
	MsgConnect.dwGaeaID = m_pClientManager->GetGaeaID(dwClient);
	MsgConnect.dwSlotFieldAgent = m_pClientManager->GetSlotFieldAgent(dwClient);
	MsgConnect.emType = pMsgJoinFB->emType;

	::StringCchCopy( MsgConnect.szServerIP, MAX_IP_LENGTH+1, pField->szServerIP );
	MsgConnect.nServicePort = pField->nServicePort;

    // m_pClientManager->GetChaNum(dwClient);

	return SendClient ( dwClient, &MsgConnect );
}

///////////////////////////////////////////////////////////////////////////////
// Agent->Client
// Ŭ���̾�Ʈ�� ĳ���� ���� ȭ�鿡�� ���÷��� �ؾ���
// ĳ������ ������ȣ�� �����Ѵ�.
void CAgentServer::MsgSndChaBasicBAInfo(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	DWORD dwSndBytes   = 0;	
	int nUserNum       = 0;
	int nSvrGrp        = 0;
	int nServerChannel = 0;
//	int nChannel;
	DWORD dwClient     = pMsg->dwClient;

	// ������ ������� ���� ���¿��� ĳ���� ������û��
	if (m_pClientManager->IsAccountPass(dwClient) == false)
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("INFO:MsgSndChaBasicBAInfo IsAccountPass false"));
		CloseClient(dwClient);
		return;
	}

	NET_CHA_REQ_BA_INFO* pNcrbi = reinterpret_cast<NET_CHA_REQ_BA_INFO*> (pMsg->Buffer);

	/*
	nChannel = pNcrbi->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:MsgSndChaBasicBAInfo Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.		
		CloseClient(dwClient);
		CConsoleMessage::GetInstance()->Write(
											  _T("CH(%d) is FULL"),
			                                  nChannel);
		return;
	}
	else
	{
		// Full �� �ƴҰ�� �ش� ä�� �������� 1 ������Ų��.
		IncreaseChannelUser(nChannel);
		// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
		m_pClientManager->SetChannel(dwClient,nChannel);                
	}
	*/

	nUserNum = m_pClientManager->GetUserNum(dwClient);
	if (nUserNum < 1) // ġ���� ����, �㰡���� ���� ����� ��ȣ
	{
		CConsoleMessage::GetInstance()->Write(_T("ERROR:MsgSndChaBasicBAInfo nUserNum(%d)"),
			                                  nUserNum);
		return;
	}

	nSvrGrp = m_nServerGroup;

	CGetChaBAInfo* pAction = new CGetChaBAInfo(nUserNum, 
											   nSvrGrp, 
											   dwClient, 
											   m_pClientManager->GetClientIP(dwClient),		
											   m_pClientManager->GetClientPort(dwClient));
	COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);	
}

void CAgentServer::MsgSndChaBasicBAInfoBack(NET_CHA_BBA_INFO* pStruct, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
	// IP/PORT �� �����Ҷ��� ó��
	if ( (strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{
		SendClient(dwClient, pStruct);
	}
}

void CAgentServer::MsgSndChaBasicInfo(DWORD dwClient, int nChaNum)
{
	// ��ŷ�� ���ؼ� nChaNum �� 0 ���� ����� ����	
	if (nChaNum <= 0) 
	{
		return;
	}

	// ������ ������� ���� ���¿��� ĳ���� ������û��
	if (m_pClientManager->IsAccountPass(dwClient) == false)
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("INFO:MsgSndChaBasicInfo IsAccountPass false"));
		CloseClient(dwClient);
		return;
	}

	int nUserNum = m_pClientManager->GetUserNum(dwClient);

	CGetChaBInfo* pAction = new CGetChaBInfo(nUserNum,
		                                     nChaNum, 
											 dwClient, 
											 m_pClientManager->GetClientIP(dwClient),		
											 m_pClientManager->GetClientPort(dwClient));
	COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);
}

void CAgentServer::MsgSndChaBasicInfoBack(GLMSG::SNETLOBBY_CHARINFO* snci, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
	EMCHARCLASS emCharClass = snci->Data.m_emClass;

	if( emCharClass == GLCC_EXTREME_M || emCharClass == GLCC_EXTREME_W )
	{
		snci->Data.m_sExperience.lnMax = GLOGICEX::GLNEEDEXP2( snci->Data.m_wLevel );
	}
	else
	{
		snci->Data.m_sExperience.lnMax = GLOGICEX::GLNEEDEXP( snci->Data.m_wLevel );
	}

	SendClient(dwClient, snci);
}

void CAgentServer::MsgSndChaBasicInfo(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	DWORD dwSndBytes = 0;
	DWORD dwClient = pMsg->dwClient;
	INT nChaNum = 0;
	INT nUsrNum = 0;
	INT nSvrGrp = 0;

	NET_CHA_BA_INFO* ncbi;
	ncbi = (NET_CHA_BA_INFO*) pMsg->Buffer;

	if (ncbi == NULL) return;

	nChaNum = ncbi->nChaNum;

	MsgSndChaBasicInfo(dwClient, nChaNum);
}

void CAgentServer::MsgSndChatGlobal(char* szChatMsg)
{
	if (szChatMsg == NULL) return;

	DWORD dwSndBytes = 0;
	CConsoleMessage::GetInstance()->WriteConsole( szChatMsg );
	NET_CHAT_CTRL_FB ncf;
	ncf.emType = CHAT_TYPE_GLOBAL;
	::StringCchCopy(ncf.szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, szChatMsg);
	SendAllClient(&ncf);
}

void CAgentServer::MsgVersion(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	NET_CLIENT_VERSION		ncv;
	DWORD dwClient			= pMsg->dwClient;
	ncv.nmg.nType			= NET_MSG_VERSION_INFO;
	ncv.nGameProgramVer		= m_nVersion;
	ncv.nPatchProgramVer	= m_nPatchVersion;	
	SendClient(dwClient, &ncv);
}

// ���ο� ĳ���͸� �����Ѵ�.
void CAgentServer::MsgCreateCharacter( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	DWORD			dwClient		= 0;
	NET_NEW_CHA*	nnc				= NULL;
	NET_NEW_CHA_FB	nncfd;
	DWORD			dwSndBytes		= 0;
	int				nUesrNum		= 0;
	int				nIndex			= 0;
	int				nExtremeM		= 0; // �ذ��� ���� ���� ����
	int				nExtremeW		= 0; // �ذ��� ���� ���� ����
	int				nExtremeCheck	= 0; // �ذ��� ���� ���� üũ����
	int				nSvrGrp			= 0;
	int				nChaNum			= 0;
    WORD            wChaRemain		= 0;

	dwClient = pMsg->dwClient;
	
	// ������ ������� ����	
	if (m_pClientManager->IsAccountPass(dwClient) == false) 
	{
		CloseClient(dwClient);
		return;
	}

    if (m_bTestServer) // �׽�Ʈ �������
	{
        nncfd.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
    }
    else
    {
        nncfd.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
    }

	nncfd.nmg.nType = NET_MSG_CHA_NEW_FB;

	// ĳ���� ���� ����, ���� �� �ִ� ĳ���� ����
	if ( (nncfd.wChaRemain <= 0) || (nncfd.wChaRemain > MAX_CHAR_LENGTH) )
	{
		nncfd.nResult  = EM_CHA_NEW_FB_SUB_MAX;
		nncfd.nChaNum  = 0;
		SendClient(dwClient, &nncfd);
		return;
	}
	
	nnc = reinterpret_cast<NET_NEW_CHA*> (pMsg->Buffer);

	// ĳ���� �ε���
	nIndex = nnc->nIndex;
	
	// ĳ���� �ε��� ����, ĳ���� ���� �� ����.
	if (nIndex < 0 || nIndex >= GLCI_NUM_NEWSEX )	
	{
		nChaNum = -1; // ĳ���� ����
		nncfd.nResult = EM_CHA_NEW_FB_SUB_LENGTH; // ���ڼ� ����
		SendClient(dwClient, &nncfd);
		return;
	}	

	// ĳ���͸�
	CString strChaName(nnc->szChaName);
	strChaName.Trim(_T(" ")); // �յ� ��������

	// �����̽� ã��, ĳ�����̸� 4���� ���� ����, ĳ���� ���� �� ����.
	if ((strChaName.FindOneOf(" ") != -1) || (strChaName.GetLength() < 4)) 
	{
		nChaNum = -1; // ĳ�����̸� ����
		nncfd.nResult = EM_CHA_NEW_FB_SUB_LENGTH; // ���ڼ� ����
		SendClient(dwClient, &nncfd);
		return;
	}

	BOOL bFILTER0 = STRUTIL::CheckString( strChaName );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strChaName );
	if ( bFILTER0 || bFILTER1 )
	{
		nChaNum = -1; // ĳ�����̸� ����
		nncfd.nResult = EM_CHA_NEW_FB_SUB_BAD_NAME; // ���ڼ� ����
		SendClient(dwClient, &nncfd);
		return;
	}


#ifdef TH_PARAM	
	// �±��� ���� ���� üũ 
	if ( !m_pCheckString ) return;

	if ( !m_pCheckString(strChaName) )
	{
		nChaNum = -1; // ĳ�����̸� ����
		nncfd.nResult = EM_CHA_NEW_FB_SUB_THAICHAR_ERROR; // ���ڼ� ����
		SendClient(dwClient, &nncfd);
		return;
	}
#endif

#ifdef VN_PARAM
	// ��Ʈ�� ���� ���� üũ 
	if( STRUTIL::CheckVietnamString( strChaName ) )
	{
		nChaNum = -1; // ĳ�����̸� ����
		nncfd.nResult = EM_CHA_NEW_FB_SUB_VNCHAR_ERROR; // ���ڼ� ����
		SendClient(dwClient, &nncfd);
		return;
	}

#endif 



	// �ذ��� ���� ���� üũ
	if( GLCI_EXTREME_M == nIndex )
	{
		// ������ ����� �ذ��� ���� üũ ���� �޾ƿ���	
		nExtremeM = m_pClientManager->GetExtremeM( dwClient );

		// �ذ��� ���� ������ �Ұ��� ���, nnc->wSex(����) = 0
		if( 0 >= nExtremeM )
		{
			nncfd.nResult = EM_CHA_NEW_FB_SUB_MAX;
			SendClient(dwClient, &nncfd);
			return;
		}

		// �ذ��� ���� ������ �����Ѵ�.
		nExtremeCheck = 1;
		m_pClientManager->SetExtremeCheckM( dwClient, nExtremeCheck );		
	}
	else if( GLCI_EXTREME_W == nIndex )
	{
		nExtremeW = m_pClientManager->GetExtremeW( dwClient );

		// �ذ��� ���� ������ �Ұ��� ���, nnc->wSex(����) = 1
		if( 0 >= nExtremeW )
		{
			nncfd.nResult = EM_CHA_NEW_FB_SUB_MAX;
			SendClient(dwClient, &nncfd);
			return;
		}

		// �ذ��� ���� ������ �����Ѵ�.
		nExtremeCheck = 1;
		m_pClientManager->SetExtremeCheckW( dwClient, nExtremeCheck );
	}

	if (m_bTestServer) // �׽�Ʈ �������
	{
		m_pClientManager->SetChaTestRemain(dwClient, nncfd.wChaRemain-1);
	}
	else // ���� ���� ���...
	{
		m_pClientManager->SetChaRemain(dwClient, nncfd.wChaRemain-1);
	}

	CCreateNewCharacter* pAction = new CCreateNewCharacter(
											nIndex, 
                                            m_pClientManager->GetUserNum(dwClient),
                                            m_nServerGroup,
                                            strChaName,
                                            nnc->wSchool,
                                            nnc->wHair,
                                            nnc->wFace,
											nnc->wHairColor,
											nnc->wSex,
                                            dwClient,
                                            m_pClientManager->GetClientIP(dwClient),
                                            m_pClientManager->GetClientPort(dwClient) );
    COdbcManager::GetInstance()->AddJob( (CDbAction*) pAction );
}

void CAgentServer::MsgCreateCharacterBack(
	int nChaNum,
    DWORD dwClient,
    DWORD dwUserNum,
    const char* szUserIP,
    USHORT uPort )
{	
    NET_NEW_CHA_FB	nncfd;
    
    nncfd.nmg.nType = NET_MSG_CHA_NEW_FB;
	nncfd.nChaNum   = 0;

	WORD wChaRemain		= 0;
	int	 nExtremeM		= 0;
	int  nExtremeW		= 0;
	int	 nExtremeDBM	= 0;
	int  nExtremeDBW	= 0;
	int  nExtremeCheckM = 0;
	int  nExtremeCheckW = 0;
	
	if (nChaNum > 0) // ĳ���� ���� ����
	{
		nncfd.nChaNum = nChaNum;
		nncfd.nResult = EM_CHA_NEW_FB_SUB_OK;

		// �������� �ذ��� ��/�� ���� ���� ���ڸ� �޾ƿ´�.
		nExtremeM = m_pClientManager->GetExtremeM( dwClient );
		nExtremeW = m_pClientManager->GetExtremeW( dwClient );

		// DB������� �ذ��� ��/�� ���� ���� ���ڸ� �޾ƿ´�.
		nExtremeDBM = m_pClientManager->GetExtremeDBM( dwClient );
		nExtremeDBW = m_pClientManager->GetExtremeDBW( dwClient );
		
		// �ذ��� ���� ������ �����´�.
		nExtremeCheckM = m_pClientManager->GetExtremeCheckM( dwClient );
		nExtremeCheckW = m_pClientManager->GetExtremeCheckW( dwClient );

		// �ذ��� ��/�� �� ��� 1�� ������ �����Ѵ�.
		if( EXTREME_OK == nExtremeCheckM )
		{			
			if(nExtremeDBW < 0)
			{
				nExtremeDBW += 1;
				nExtremeDBM -= 2;

				nExtremeM = nExtremeDBM;
				m_pClientManager->SetExtremeCreateDBM( dwClient, nExtremeDBM );
				m_pClientManager->SetExtremeCreateDBW( dwClient, nExtremeDBW );
				m_pClientManager->SetExtremeCreateM( dwClient, nExtremeM );
			}
			else
			{
				nExtremeM -= 1;
				nExtremeDBM -= 1;
				m_pClientManager->SetExtremeCreateM( dwClient, nExtremeM );				
				m_pClientManager->SetExtremeCreateDBM( dwClient, nExtremeDBM );
			}
			nExtremeCheckM = 0;
			m_pClientManager->SetExtremeCheckM( dwClient, nExtremeCheckM );
		}
		else if( EXTREME_OK == nExtremeCheckW )
		{
			if( nExtremeDBM < 0 )
			{
				nExtremeDBM += 1;
				nExtremeDBW -= 2;

				nExtremeW = nExtremeDBW;
				m_pClientManager->SetExtremeCreateDBM( dwClient, nExtremeDBM );
				m_pClientManager->SetExtremeCreateDBW( dwClient, nExtremeDBW );
				m_pClientManager->SetExtremeCreateW( dwClient, nExtremeW );
			}
			else
			{
				nExtremeW -= 1;
				nExtremeDBW -= 1;
				m_pClientManager->SetExtremeCreateW( dwClient, nExtremeW );				
				m_pClientManager->SetExtremeCreateDBW( dwClient, nExtremeDBW );
			}
			
			nExtremeCheckW = 0;
			m_pClientManager->SetExtremeCheckW( dwClient, nExtremeCheckW );
		}

		// �ذ��� ��/�� ���� ���� ���ڸ� �Ѱ��ش�.
		nncfd.nExtremeM = m_pClientManager->GetExtremeM( dwClient );
		nncfd.nExtremeW = m_pClientManager->GetExtremeW( dwClient );

		if (m_bTestServer) // �׽�Ʈ �������
		{
            wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
            if (wChaRemain < 0 || wChaRemain > MAX_CHAR_LENGTH) 
                wChaRemain = 0;
            //m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            nncfd.wChaRemain = wChaRemain;

			COdbcManager::GetInstance()->UserTestChaNumDecrease(dwUserNum);
		}
		else // ���� ���� ���...
		{
            wChaRemain = m_pClientManager->GetChaRemain(dwClient);
            if (wChaRemain < 0 || wChaRemain > MAX_CHAR_LENGTH) 
                wChaRemain = 0;
            //m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            nncfd.wChaRemain = wChaRemain;

			COdbcManager::GetInstance()->UserChaNumDecrease(dwUserNum);
		}
	}
	else
	{
		if (m_bTestServer) // �׽�Ʈ �������
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain+1);
		}
		else // ���� ���� ���...
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			m_pClientManager->SetChaRemain(dwClient, wChaRemain+1);
		}

		if (nChaNum == DB_ERROR) // ĳ���� ����(DB) �����߻�...
		{    
			if (m_bTestServer) wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			else               wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			nncfd.wChaRemain = wChaRemain;
			nncfd.nResult    = EM_CHA_NEW_FB_SUB_DUP;
		}
		else if (nChaNum == DB_CHA_MAX) // ����� �ִ� ĳ���� ���� �ʰ�
		{
			if (m_bTestServer) wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			else               wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			nncfd.wChaRemain = wChaRemain;
			nncfd.nResult  = EM_CHA_NEW_FB_SUB_MAX;
		}
		else if (nChaNum == DB_CHA_DUF) // ���� �̸��� ���� ĳ���� ����
		{
			if (m_bTestServer) wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			else               wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			nncfd.wChaRemain = wChaRemain;
			nncfd.nResult  = EM_CHA_NEW_FB_SUB_DUP;
		}
		else // �˼� ���� ����
		{
			if (m_bTestServer) wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			else               wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			nncfd.wChaRemain = wChaRemain;
			nncfd.nResult  = EM_CHA_NEW_FB_SUB_ERROR;
		}
	}

     // IP/PORT �� �����Ҷ��� ó��
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{
	    SendClient(dwClient, &nncfd);
	    if (nChaNum > 0)
	    {
		    MsgSndChaBasicInfo(dwClient, nChaNum);
	    }
    }
}

void CAgentServer::MsgSndLoginInfoToField(DWORD dwClient)
{
	NET_LOGIN_FEEDBACK_DATA3 nlfd3;
	nlfd3.nmg.nType = NET_MSG_LOGIN_FB_SUB_OK_FIELD;
	nlfd3.nUserNum  = m_pClientManager->GetUserNum(dwClient);	
	::StringCchCopy(nlfd3.szUserid, 
		            USR_ID_LENGTH+1, 
		            m_pClientManager->GetUserID(dwClient));
	m_pClientManager->SendField(dwClient+m_nMaxClient, (LPVOID) &nlfd3);
	CConsoleMessage::GetInstance()->Write(_T("INFO:MsgSndLoginInfoToField %d, %s"),
										  nlfd3.nUserNum,
										  nlfd3.szUserid);
}

void CAgentServer::MsgFieldSvrOutFb	(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if (nmg == NULL) return;

	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
	if ( !pChar )										return;
	if ( pChar->m_dwNextFieldSvr == FIELDSERVER_MAX )	return;

	//CConsoleMessage::GetInstance()->Write(
	//	_T("MsgFieldSvrOutFb ClientID[%u], GaeaID[%u]"), dwClientID, dwGaeaID );

	//m_pClientManager->SetConnectionFieldInfo(	dwClientID, 
	//											dwGaeaID, 
	//											nmg );

	//	���� �ʵ弭������ ������ ���´�.
	m_pClientManager->CloseField ( dwClientID );

	//	���ο� �ʵ� ������ ������ �Ѵ�.
	if ( ConnectFieldSvr ( dwClientID, pChar->m_dwNextFieldSvr, dwGaeaID, pChar->m_nChannel ) == NET_OK )
	{
		//	������Ʈ ������ �޽��� ����.
		GLAgentServer::GetInstance().MsgProcess ( nmg, dwClientID, dwGaeaID );
		pChar->ResetNextFieldSvr (); //	������ �ʵ弭�� ������ ����.
	}
}

void CAgentServer::MsgReBirthOutFb	(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if (nmg == NULL) return;

	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
	if ( !pChar )										return;
	if ( pChar->m_dwNextFieldSvr == FIELDSERVER_MAX )	return;

	//CConsoleMessage::GetInstance()->Write(
	//	_T("MsgReBirthOutFb ClientID[%u], GaeaID[%u]"), dwClientID, dwGaeaID );

	//m_pClientManager->SetConnectionFieldInfo(	dwClientID, 
	//											dwGaeaID, 
	//											nmg );

	//	���� �ʵ弭������ ������ ���´�.
	m_pClientManager->CloseField ( dwClientID );

	//	���ο� �ʵ� ������ ������ �Ѵ�.
	if ( ConnectFieldSvr ( dwClientID, pChar->m_dwNextFieldSvr, dwGaeaID, pChar->m_nChannel ) == NET_OK )
	{
		//	������Ʈ ������ �޽��� ����.
		GLAgentServer::GetInstance().MsgProcess ( nmg, dwClientID, dwGaeaID );
		pChar->ResetNextFieldSvr (); //	������ �ʵ弭�� ������ ����.
	}
}

void CAgentServer::MsgReCallFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_REQ_RECALL_FB *pNetMsg = (GLMSG::SNETPC_REQ_RECALL_FB *) nmg;

	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
	if ( !pChar )										return;

	pChar->SetCurrentField ( pChar->m_dwCurFieldSvr, pNetMsg->sMAPID );

	SendClient ( pChar->m_dwClientID, nmg );
}

void CAgentServer::MsgTeleportFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_REQ_TELEPORT_FB *pNetMsg = (GLMSG::SNETPC_REQ_TELEPORT_FB *) nmg;

	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
	if ( !pChar )										return;

	pChar->SetCurrentField ( pChar->m_dwCurFieldSvr, pNetMsg->sMAPID );

	SendClient ( pChar->m_dwClientID, nmg );
}

void CAgentServer::MsgPremiumSetFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_INVEN_PREMIUMSET_FB *pNetMsg = (GLMSG::SNET_INVEN_PREMIUMSET_FB *) nmg;

	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
	if ( !pChar )										return;

	pChar->m_tPREMIUM = pNetMsg->tLMT;

	SendClient ( pChar->m_dwClientID, nmg );
}

void CAgentServer::MsgAttackClientCheck( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{

	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
	if( !pChar ) return;
	

	CConsoleMessage::GetInstance()->Write( "!!!!!!!!!!!!!!!! Server Attack !!!!!!!!!!!!!!!!" );
	CConsoleMessage::GetInstance()->Write( "IP : %s Account : %s ID : %s User MapID : %d, %d Current FieldServer : %d ClientID : %d",
						m_pClientManager->GetClientIP(dwClientID), m_pClientManager->GetUserID(dwClientID), pChar->m_szName,
						pChar->m_sCurMapID.wMainID, pChar->m_sCurMapID.wSubID, pChar->m_dwCurFieldSvr, dwClientID );


}


void CAgentServer::MsgGmPkMode ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	int nUSERLVL = m_pClientManager->GetUserType ( dwClientID );

	if ( USER_GM1 <= nUSERLVL )
	{
		SendAllChannel ( nmg );
	}
}

void CAgentServer::MsgGmwherePcMap ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_WHERE_PC_MAP *pNetMsg = (GLMSG::SNET_GM_WHERE_PC_MAP *) nmg;

	int nUSERLVL = m_pClientManager->GetUserType ( dwClientID );

	if ( USER_GM3 <= nUSERLVL )
	{
		PGLCHARAG pCHAR = GLAgentServer::GetInstance().GetCharID ( pNetMsg->dwCHARID );
		if ( pCHAR )
		{
			GLMSG::SNET_GM_WHERE_PC_MAP_FB NetMsgFB;
			NetMsgFB.dwChannel = (DWORD)pCHAR->m_nChannel;
			NetMsgFB.nidMAP = pCHAR->m_sCurMapID;
			SendClient ( dwClientID, &NetMsgFB );
		}
	}
}

void CAgentServer::MsgVietTimeReset ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
#if defined(VN_PARAM) //vietnamtest%%% && vietnamtest2
	GLMSG::SNETPC_VIETNAM_TIME_RESET* pNetMsg = (GLMSG::SNETPC_VIETNAM_TIME_RESET *) nmg;

	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar(dwGaeaID);
	if( !pChar )	return;

	m_pClientManager->SetVTGameTime(dwClientID,0);
	m_pClientManager->SetLoginTime(dwClientID);
	
	pChar->m_sVietnamSystem.gameTime   = 0;
	pChar->m_sVietnamSystem.loginTime  = CTime::GetCurrentTime().GetTime();


	GLMSG::SNETPC_VIETNAM_TIME_REQ_FB NetMsgFB;
	NetMsgFB.gameTime  = pChar->m_sVietnamSystem.gameTime;
	NetMsgFB.loginTime = pChar->m_sVietnamSystem.loginTime;
	SendField( dwClientID, &NetMsgFB );
#endif

	return;
}

///////////////////////////////////////////////////////////////////////////////
// �������� PC ���θ�� ������ '����' �̺�Ʈ
// PC �濡�� �Ϸ翡 �ѻ���� �ѹ��� ����
// nResult
// 1 : �̺�Ʈ ���𰡴�
// 2 : �̹� �̺�Ʈ�� ��������, �Ϸ翡 �ѹ��� ����.
// 3 : ����Ұ� PC �� IP �ƴ�
void CAgentServer::MsgLogPungPungCheckBack(int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
    // IP/PORT �� �����Ҷ��� ó��
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{
        int nPrize = 0; // ��÷�� ��ǰ.        
        // 1 �� ���𰡴��϶��� ������ ��÷��...
        if (nResult == 1)
        {
            // ������ ��÷ó��

            // ���������� �α׷� �����.
            CLogPungPungWrite* pAction = new CLogPungPungWrite(m_pClientManager->GetUserID(dwClient),
                                                               m_pClientManager->GetUserNum(dwClient),
                                                               nPrize, // ��÷�� ��ǰ
                                                               m_pClientManager->GetClientIP(dwClient));
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
        }
        else // 2, 3 ���϶��� �����޽�����...
        {

        }        
    }
}

///////////////////////////////////////////////////////////////////////////////
// �� ��������!! �̺�Ʈ 
// nResult
// 1  1 �� P-4 PC
// 2  2 �� ȿ����
// 3  3 �� SONY ��ī
// 4  4 �� ���̸��� MP �÷��̾�
// 5  5 �� MAX ���� ��ȭ ��ǰ�� 1 ������
// 6  6 �� MBC Game ���� ���� ��ǰ��
// 7  �̹� ������ �Է��� �����
// 8  PC �� IP �� �ƴ�
// 9  �̹� ����� ����
// 10 �˼����� ����
void CAgentServer::MsgLogLotteryBack(int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
    // IP/PORT �� �����Ҷ��� ó��
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{
        NET_EVENT_LOTTERY_FB msg;
        msg.nResult = nResult;
        SendClient(dwClient, &msg);
    }
}

void CAgentServer::MsgApexData( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_APEX_DATA* msg = reinterpret_cast<NET_APEX_DATA*> (pMsg->Buffer);
	int nLength = msg->nmg.dwSize - sizeof(NET_MSG_GENERIC);
	APEX_SERVER::NoticeApexProxy_UserData( dwClient, msg->szData, nLength );
}

// Apex ����( ȫ�� )
void CAgentServer::MsgApexReturn( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_APEX_RETURN* msg = reinterpret_cast<NET_APEX_RETURN*> (pMsg->Buffer);
	APEX_SERVER::NoticeApexProxy_UserReturn( dwClient, msg->nReturn );
}


void CAgentServer::MsgTracingLogPrint( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;
#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	NET_LOG_UPDATE_TRACINGCHAR* msg = reinterpret_cast<NET_LOG_UPDATE_TRACINGCHAR*> (pMsg->Buffer);
	int nTracing = CheckTracingUser( msg->szAccount, msg->nUserNum );
	if( nTracing == -1 ) return;
 	CDebugSet::ToTracingFile( m_vecTracingData[nTracing].strAccount.c_str(), msg->szLogMsg );
#endif
}

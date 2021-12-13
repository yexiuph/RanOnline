///////////////////////////////////////////////////////////////////////////////
// s_CLoginServerMsg.cpp
// Copyright (c) Min Communications. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "s_CLoginServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CLoginServer::MsgProcess(
	MSG_LIST* pMsg )
{	
	if (pMsg == NULL) return NET_ERROR;	

	NET_MSG_GENERIC* nmg = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);
	DWORD  dwClient = pMsg->dwClient;
	
	// 서버간 통신 메시지
	if ( (dwClient >= 0) && (dwClient < NET_RESERVED_SLOT) )
	{
		ServerMsgProcess( pMsg );
		return NET_OK;
	}

	// 클라이언트 통신 메시지
	switch ( nmg->nType )
	{	
	case NET_MSG_VERSION_INFO: // 클라이언트 버전정보
		MsgVersion(pMsg);
		break;
	case NET_MSG_HEARTBEAT_CLIENT_ANS: // 클라이언트->Login Server
		m_pClientManager->SetHeartBeat( dwClient );
		break;
	case NET_MSG_REQ_GAME_SVR: // 클라이언트->로그인서버 : 게임서버 정보 요청
		MsgSndGameSvrInfo( pMsg );
		break;	
	default:
		break;
	}
	return NET_OK;
}

void CLoginServer::ServerMsgProcess(
	MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;

	//if ( dwClient == m_dwSession )
	//{	
	//	SessionMsgProcess( pMsg );
	//}

	switch ( dwClient )
	{
	case NET_RESERVED_SESSION:
		SessionMsgProcess( pMsg );
		break;
	default :
		break;
	}
}

void CLoginServer::MsgVersion(MSG_LIST* pMsg)
{	
	if (pMsg == NULL) return;

	NET_CLIENT_VERSION msg;
	DWORD dwClient = pMsg->dwClient;
	
	msg.nmg.nType = NET_MSG_VERSION_INFO;
	
	if (m_bInspecting) // 점검중
	{
		msg.nGameProgramVer		= -100;
		msg.nPatchProgramVer	= -100;
	}
	else // 정상 상태
	{
		msg.nGameProgramVer		= m_nVersion;
		msg.nPatchProgramVer	= m_nPatchVersion;
	}

	NET_ENCRYPT_KEY	msgEnc;
	::StringCchCopy( msgEnc.szEncryptKey, ENCRYPT_KEY+1, m_szEncrypt );

	SendClient( dwClient, (char*) &msg );
	SendClient( dwClient, (char*) &msgEnc );
}

// 게임서버 정보를 클라이언트에 전송한다.
void CLoginServer::MsgSndGameSvrInfo(
	MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	DWORD m_dwTemp = timeGetTime();
	DWORD dwClient = pMsg->dwClient;

	if (dwClient < NET_RESERVED_SESSION)
	{
		CConsoleMessage::GetInstance()->Write(_T("ERROR:MsgSndGameSvrInfo dwClient<NET_RESERVED_SESSION"));
		return;
	}

	NET_CUR_INFO_LOGIN ncil;
	
	// 게임서버 정보 업데이트 시간이 지정시간 이상 경과했으면 새롭게 정보를 요청한다.
	if ((m_dwTemp - m_dwLastGameSvrUpdateTime) > SVR_STATE_UPDATE_TIME)
	{
		SessionReqSvrInfo();
		m_dwLastGameSvrUpdateTime = m_dwTemp;
		CConsoleMessage::GetInstance()->WriteInfo(_T("INFO:Request New Server Info"));
	}

	LockOn();
	DWORD dwCount=0;
	for (int i=0; i<MAX_SERVER_GROUP; ++i)
	{
		for (int j=0; j<MAX_CHANNEL_NUMBER; ++j)
		{
			if (m_sGame[i][j].nServerMaxClient > 0)
			{
				ncil.nmg.dwSize	= sizeof(NET_CUR_INFO_LOGIN);
				ncil.nmg.nType	= NET_MSG_SND_GAME_SVR;
				ncil.gscil		= m_sGame[i][j];
				SendClient(dwClient, (char*) &ncil);
				dwCount++;
			}
		}
	}

	if (dwCount == 0)
	{
		CConsoleMessage::GetInstance()->Write(_T("ERROR:Check Session Server Connection"));
	}

	NET_MSG_GENERIC nmg;
	nmg.dwSize = sizeof(NET_MSG_GENERIC);
	nmg.nType = NET_MSG_SND_GAME_SVR_END;
	SendClient(dwClient, (char*) &nmg);

	LockOff();
}
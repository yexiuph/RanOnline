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
	
	// ������ ��� �޽���
	if ( (dwClient >= 0) && (dwClient < NET_RESERVED_SLOT) )
	{
		ServerMsgProcess( pMsg );
		return NET_OK;
	}

	// Ŭ���̾�Ʈ ��� �޽���
	switch ( nmg->nType )
	{	
	case NET_MSG_VERSION_INFO: // Ŭ���̾�Ʈ ��������
		MsgVersion(pMsg);
		break;
	case NET_MSG_HEARTBEAT_CLIENT_ANS: // Ŭ���̾�Ʈ->Login Server
		m_pClientManager->SetHeartBeat( dwClient );
		break;
	case NET_MSG_REQ_GAME_SVR: // Ŭ���̾�Ʈ->�α��μ��� : ���Ӽ��� ���� ��û
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
	
	if (m_bInspecting) // ������
	{
		msg.nGameProgramVer		= -100;
		msg.nPatchProgramVer	= -100;
	}
	else // ���� ����
	{
		msg.nGameProgramVer		= m_nVersion;
		msg.nPatchProgramVer	= m_nPatchVersion;
	}

	NET_ENCRYPT_KEY	msgEnc;
	::StringCchCopy( msgEnc.szEncryptKey, ENCRYPT_KEY+1, m_szEncrypt );

	SendClient( dwClient, (char*) &msg );
	SendClient( dwClient, (char*) &msgEnc );
}

// ���Ӽ��� ������ Ŭ���̾�Ʈ�� �����Ѵ�.
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
	
	// ���Ӽ��� ���� ������Ʈ �ð��� �����ð� �̻� ��������� ���Ӱ� ������ ��û�Ѵ�.
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
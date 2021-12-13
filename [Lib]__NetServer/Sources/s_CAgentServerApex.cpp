#include "pch.h"
#include "s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

_FUNC_S_REC pfRec = NULL;

#define APEX_HACK_BLOCK_DAY 3

//! Apex �� ���޽����� Ŭ���̾�Ʈ���� ������.
void CAgentServer::ApexSendMessageToClient( DWORD dwClient, WORD wAction, WORD wHour )
{
	if( ( wAction == 2 ) && ( wHour > 0x2238/*365��*/ ) )
	{
		wHour = 0x2238;
	}

    // Ŭ���̾�Ʈ�� ��� �޽����� ������.
	NET_APEX_ANSWER msg;
	msg.wAction = wAction;
	msg.wHour = wHour;
	//m_pClientManager->SendClient2( dwClient, &msg );
	SendClient( dwClient, &msg );
}

//! Apex �� ����ڸ� �� ��ų�� ȣ��� Block
void CAgentServer::ApexUserBlock( DWORD dwClient, WORD dwHour )
{
	//	���ð� ����.
	CTime cBLOCK = CTime::GetCurrentTime();

	DWORD dwBlockMin  = 0;

	if ( dwHour == 0xFFFF )
		dwBlockMin = 525600; // 60 * 24 * 365; // 1��
	else
		dwBlockMin = dwHour * 60;

	CTimeSpan cSPAN( 0, 0, dwBlockMin, 0 );
	cBLOCK += cSPAN;

	CUserBlockDate *pDbAction = new CUserBlockDate( m_pClientManager->GetUserNum( dwClient ), cBLOCK.GetTime() );
	COdbcManager::GetInstance()->AddUserJob( (CDbAction*) pDbAction );	
}

//! Apex �� ����� ä���� ���� ��ų�� ȣ��� 
void CAgentServer::ApexUserChatBlock( DWORD dwClient, WORD dwHour )
{
	if( dwHour == 0 ) return;
	if( dwHour > 0x2238/*365��*/ ) dwHour = 0x2238;

	DWORD dwMin = dwHour * 60;

	GLMSG::SNET_GM_CHAT_BLOCK_FB NetMsgFB;

	PGLCHARAG pCHAR =  GLAgentServer::GetInstance().GetCharUA( m_pClientManager->GetUserID( dwClient ) );
	
	if ( !pCHAR ) // �������� �ƴҰ��
	{
		NetMsgFB.bBLOCK = false;
		StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, m_pClientManager->GetUserID( dwClient ) );
		SendClient( dwClient, &NetMsgFB );
		return;
	}
	
	//	���ð� ����.
	CTime cBLOCK = CTime::GetCurrentTime();
	CTimeSpan cSPAN(0,0,dwMin,0);
	cBLOCK += cSPAN;

	pCHAR->m_tCHATBLOCK = cBLOCK.GetTime();

	//	DB�� ����.
	CSetChatBlockTime *pDbAction = new CSetChatBlockTime(pCHAR->m_dwUserID,pCHAR->m_tCHATBLOCK);
	COdbcManager::GetInstance()->AddJob ( pDbAction );

	//	FLD�� �˸�.
	GLMSG::SNET_GM_CHAT_BLOCK_FLD NetMsgFLD;
	NetMsgFLD.dwCHARID = pCHAR->m_dwCharID;
	NetMsgFLD.dwBLOCK_MINUTE = dwMin;
	SendAllChannel(&NetMsgFLD);

	//	FB.
	NetMsgFB.bBLOCK = true;
	NetMsgFB.dwBLOCK_MINUTE = dwMin;
	StringCchCopy( NetMsgFB.szCHARNAME, CHAR_SZNAME, pCHAR->m_szName );
	StringCchCopy( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, pCHAR->m_szUserName );
	SendClient( dwClient, &NetMsgFB );
}

//! Apex �� ����� �α׸� ���涧 ȣ���
void CAgentServer::ApexUserHackLog( DWORD dwClient, TCHAR* szComment )
{	
	CLogHackProgram *pDbAction = new CLogHackProgram(
											m_pClientManager->GetUserNum( dwClient ),
											m_pClientManager->GetChaNum( dwClient ),
											APEX_HACK_NUM,
											szComment );
	COdbcManager::GetInstance()->AddLogJob( (CDbAction*) pDbAction );

	CString strTemp;
	strTemp.Format( _T("%s ClientNum %d UserNum %d UserID %s"),
		            szComment,
		            dwClient,
		            m_pClientManager->GetUserNum( dwClient ),
		            m_pClientManager->GetUserID( dwClient ));	
	CConsoleMessage::GetInstance()->WriteAPEX( strTemp.GetString() );
}

void CAgentServer::ApexCloseClient( DWORD dwClientID )
{
	if (dwClientID < NET_RESERVED_SLOT)
	{
		// APEX �� ������ ��ſ��� �ߴ��� ���´�.
		CConsoleMessage::GetInstance()->WriteAPEX(
											_T("Wrong call ApexCloseClient %d"),
											dwClientID );
		return;
	}
	else
	{		
		DWORD dwSetTime = timeGetTime();
		dwSetTime += 30000; // �������� Ŭ���̾�Ʈ ������� 30���� �ð��� �ش�.

		ApexCloseList::APEX_ID_TIME apexPair( dwClientID, dwSetTime );
		m_ApexClose.push_back( apexPair );
	}
}

void CAgentServer::ApexCloseClientFinal()
{
	DWORD dwClient = m_ApexClose.ApexCloseClientFinal();
	if (dwClient != 0)
	{
		CloseClient( dwClient );
	}
}

// Apex ����
namespace APEX_SERVER
{
	long NetSendToGameClient(signed int nSendId,const char * pBuffer,int nLen)
	{
		if ( !g_pAgentServer || 
			 nSendId <= 0 || 
			 pBuffer==NULL || 
			 nLen<=0 )
		{
			CConsoleMessage::GetInstance()->WriteAPEX(				
			    _T("ERROR:NetSendToGameClient") );
			return -1;
		}

		if ( MAX_APEX_PACKET_LENGTH < nLen )
		{
			CConsoleMessage::GetInstance()->WriteAPEX(
				_T("ERROR:NetSendToGameClient nLen>MAX_APEX_PACKET_LENGTH nLen : %d"), nLen );
			return -1;
		}

		NET_APEX_DATA msg;
		memcpy( msg.szData, pBuffer, nLen );
		msg.nmg.dwSize = sizeof(NET_MSG_GENERIC) + nLen;
		g_pAgentServer->SendClient( nSendId, &msg );
		return 0;
	}

	long GameServerKillUser( signed int nId, int Action )
	{
		if( !g_pAgentServer ) return -1;

		CConsoleMessage::GetInstance()->WriteAPEX(
					_T("ERROR:GameServerKillUser Action %d %d"),
					nId,
					Action );

		switch (Action)
		{
		case 1: // ���
			{
				g_pAgentServer->ApexUserHackLog( (DWORD) nId, _T("Apex Warning") );
				g_pAgentServer->ApexSendMessageToClient( nId, Action, 0 );
			}
			break;
		case 2: // ��ȭ����			
			{
				CString strTemp( _T("Apex Chat Block ") );
				strTemp += "1";				
				g_pAgentServer->ApexUserHackLog( (DWORD) nId, (TCHAR*) strTemp.GetString() );
				g_pAgentServer->ApexSendMessageToClient( nId, Action, 1 );
				g_pAgentServer->ApexUserChatBlock( nId, 1 );
			}
			break;
		case 3: // ��� & ����			
		case 4: // ��� & ����		
			{
				g_pAgentServer->ApexUserHackLog( (DWORD) nId, _T("Apex User Kick") );
				g_pAgentServer->ApexSendMessageToClient( nId, Action, 0 );
				g_pAgentServer->ApexCloseClient( (DWORD) nId );
			}
			break;
// �߱� ��û���� ����� ���ش�.
/*
			{
				CString strTemp( _T("Apex User Block ") );
				strTemp += "24";		
				g_pAgentServer->ApexUserHackLog( (DWORD) nId, (TCHAR*) strTemp.GetString() );
				g_pAgentServer->ApexSendMessageToClient( nId, Action, 24 );
				g_pAgentServer->ApexUserBlock( nId, 24 );
				g_pAgentServer->ApexCloseClient( (DWORD) nId );
			}
			break;
*/
		default:			
			break;
		};
		
		return 0;
	}

	int StartApexProxy()
	{
		
		CHSStart( NetSendToGameClient, pfRec );				
		CHSSetFunc( (void*)(GameServerKillUser), FLAG_KILLUSER );		

		return 0;
	}

	int StopApexProxy()
	{	
		CHSEnd();
		return 0;
	}

	int NoticeApexProxy_UserLogin(int nSendId, const char * szUserID )
	{
		if(pfRec)
		{
			pfRec( 'L', nSendId, szUserID, (int)strlen(szUserID) );			
		}
		return 0;
	}

	int NoticeApexProxy_UserLogout(int nSendId, const char * szUserID )
	{
		if(pfRec)
		{
			pfRec( 'G', nSendId, szUserID, (int)strlen(szUserID) );
		}
		return 0;
	}

	int NoticeApexProxy_UserData(int nSendId,const char * pBuf,int nBufLen)
	{
		if(pfRec)
		{
			pfRec( 'T', nSendId, pBuf, nBufLen );			
		}
		return 0;
	}

// Apex ����( ȫ�� )

	int NoticeApexProxy_UserIP( int nSendId, int nIP )
	{
		if(pfRec)
		{
			IpCmd_st IpCmd;
			IpCmd.cFlag = 1;
			IpCmd.nIp = nIP;

			pfRec('S',nSendId, (char*)&IpCmd, sizeof(IpCmd_st) );
		}

		return 0;
	}

	int NoticeApexProxy_UserReturn ( int nSendId, int nReturn )
	{
		if( pfRec )
		{
			pfRec('R',nSendId,(char*)&nReturn,sizeof(nReturn));
		}

		return 0;
	}

};
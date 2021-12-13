///////////////////////////////////////////////////////////////////////////////
// s_NetClientMsg.cpp
//
// class CNetClient
//
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_NetClient.h"
#include "s_CClientConsoleMsg.h"
#include <stdlib.h>
#include "GLContrlMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// �� �޽��� ó�� �Լ�
void CNetClient::MessageProcess()
{	
	NET_MSG_GENERIC* pNmg = NULL;
	while (true)
	{
		pNmg = (NET_MSG_GENERIC*) IsValid();
		if (NULL == pNmg) {
			break;
		}
		switch (m_nClientNetState)
		{
		case NET_STATE_LOGIN : // �α��� ���� ���ӻ���
			MessageProcessLogin(pNmg);
			break;
		case NET_STATE_AGENT : // Agent
		case NET_STATE_FIELD : // Field
		case NET_STATE_BOARD : // Board
			MessageProcessGame(pNmg);
			break;		
		case NET_STATE_CTRL : // �������α׷� ���ӻ���
			break;
		default :
			break;
		}

	}
}

void CNetClient::MessageProcessLogin(NET_MSG_GENERIC* nmg)
{
	switch(nmg->nType)
	{
	// ���Ӽ��� ��������
	case NET_MSG_SND_GAME_SVR :
		MsgGameSvrInfo(nmg);
		break;
	case NET_MSG_SND_GAME_SVR_END :
		MsgGameSvrInfoEnd(nmg);
		break;
	case NET_MSG_VERSION_INFO :
		MsgVersionInfo(nmg);
		break;
	case NET_MSG_SND_ENCRYPT_KEY:
		MsgEncryptKey(nmg);
		break;
	default:
		break;
	}
}

void CNetClient::MessageProcessGame(NET_MSG_GENERIC* nmg)
{
	switch (nmg->nType)
	{		
	// ����
	case NET_MSG_VERSION_INFO :
		MsgVersionInfo(nmg);
		break;
	case NET_MSG_SND_CRYT_KEY :
		MsgCryptKey(nmg);
		break;
	case NET_MSG_RANDOM_NUM :
		MsgRandomNum(nmg);
		break;		
	case NET_MSG_HEARTBEAT_CLIENT_REQ :
		SndHeartbeat();
		break;
	case NET_MSG_CONNECT_CLIENT_FIELD :
		MsgConnectClientToField(nmg);
		break;
	case NET_MSG_SND_ENCRYPT_KEY:
		MsgEncryptKey(nmg);
		break;
	default:
		{
			// ���� �޽���, ä�� �޽���
			if ( nmg->nType > NET_MSG_LOBBY )
			{
				if ( m_pGlobalStage )	m_pGlobalStage->MsgProcess ( nmg );
			}
		}
		break;
	}
}

/**
* Server ���� ���۵Ǿ�� �޽��� ó���Լ�
*/

int CNetClient::MsgConnectClientToField(NET_MSG_GENERIC* nmg)
{
	int nRetCode;
	NET_CONNECT_CLIENT_TO_FIELD* pMsgConnect = (NET_CONNECT_CLIENT_TO_FIELD*) nmg;
	
	nRetCode = ConnectFieldServer ( pMsgConnect->szServerIP, pMsgConnect->nServicePort );
	if ( nRetCode != NET_OK )	return NET_ERROR;

	SetGaeaID			  (pMsgConnect->dwGaeaID);
	m_pNetField->SetGaeaID(pMsgConnect->dwGaeaID);

	NET_GAME_JOIN_FIELD_IDENTITY MsgIdentity;
	MsgIdentity.emType				= pMsgConnect->emType;
	MsgIdentity.dwGaeaID			= pMsgConnect->dwGaeaID;
	MsgIdentity.dwSlotFieldAgent	= pMsgConnect->dwSlotFieldAgent;
	MsgIdentity.ck					= m_ck;

	return SendToFieldNormal ( (char*) &MsgIdentity, MsgIdentity.nmg.dwSize);
}

void CNetClient::MsgRandomNum(NET_MSG_GENERIC* nmg)
{
	NET_RANDOMPASS_NUMBER* pMsg = (NET_RANDOMPASS_NUMBER*) nmg;
	m_nRandomNumber = pMsg->nRandomNumber;
}

// �������� ���۵Ǿ�� ��ȣŰ
void CNetClient::MsgCryptKey(NET_MSG_GENERIC* nmg)
{	
	NET_CRYPT_KEY* nck;
	nck = reinterpret_cast<NET_CRYPT_KEY*> (nmg);
	// m_ck.nKey			= nck->ck.nKey;
	// m_ck.nKeyDirection	= nck->ck.nKeyDirection;
	m_ck.nKey			= 1;
	m_ck.nKeyDirection	= 1;
	m_bKeyReceived = TRUE;
}

// �������� ���۵Ǿ�� ���� ����
int CNetClient::MsgVersionInfo(NET_MSG_GENERIC* nmg)
{
	NET_CLIENT_VERSION* ncv;
	ncv = reinterpret_cast<NET_CLIENT_VERSION*> (nmg);
	
	m_nGameProgramVer  = ncv->nGameProgramVer;
	m_nPatchProgramVer = ncv->nPatchProgramVer;

	return 0;
}

// �������� ���۵Ǿ�� ��ȣȭŰ��
int CNetClient::MsgEncryptKey(NET_MSG_GENERIC* nmg)
{
	NET_ENCRYPT_KEY* ncv;
	ncv = reinterpret_cast<NET_ENCRYPT_KEY*> (nmg);
	
	StringCchCopy(m_szEncryptKey, ENCRYPT_KEY+1, ncv->szEncryptKey);
	StringCchCopy(m_szEncryptKeyHeart, ENCRYPT_KEY+1, ncv->szEncryptKey);
	
	return 0;

}

void CNetClient::MsgChaBAInfo(NET_MSG_GENERIC* nmg)
{
	NET_CHA_BBA_INFO* ncbi;
	ncbi = (NET_CHA_BBA_INFO*) nmg;

	for (int i=0; i < ncbi->nChaSNum; i++)
	{
		SndChaBasicInfo(ncbi->nChaNum[i]);
		// ����� ĳ���� ��ȣ:%d", ncbi->nChaNum[i]);
	}
}

void CNetClient::MsgGameSvrInfoEnd(NET_MSG_GENERIC* nmg)
{
	m_bGameServerInfoEnd = TRUE;
}

/**
* Login �������� ���۵Ǿ�� Game ���� ����
* \param nmg 
*/
void CNetClient::MsgGameSvrInfo(NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;
	
	NET_CUR_INFO_LOGIN* ncil = NULL;
	ncil = reinterpret_cast<NET_CUR_INFO_LOGIN*> (nmg);
	
	if ((ncil->gscil.nServerGroup >= MAX_SERVER_GROUP) || (ncil->gscil.nServerNumber >= MAX_CHANNEL_NUMBER))
	{
		return;
	}
	else
	{
		m_sGame[ncil->gscil.nServerGroup][ncil->gscil.nServerNumber] = ncil->gscil;
		TRACE(_T("Received Game Server Information \n"));
	}
}

// ���ο� ���� �ٿ�ε� �޽��� ó�� �Լ�
void CNetClient::MsgNewVersionDown()
{
	// todo : ���α׷� ��ġ���μ���	
	// Test code : �������� �ٿ�ε� ������ ����...
	//ShellExecute(NULL,
	//	"open",
	//	AlphaDownload,
	//	NULL,
	//	NULL,
	//	SW_SHOWNORMAL);	
}

void CNetClient::RegisterNewAccount(void)
{
	// Test code : �������� ȸ������ ������ ����...
	//ShellExecute(NULL,
	//	"open",
	//	AlphaRegister,
	//	NULL,
	//	NULL,
	//	SW_SHOWNORMAL);
}

// �α��� �ǵ�� ����Ÿ
void CNetClient::MsgLoginFeedBack(NET_MSG_GENERIC* nmg)
{
	NET_LOGIN_FEEDBACK_DATA* nlfd;
	nlfd = (NET_LOGIN_FEEDBACK_DATA *) nmg;	

	m_nGameProgramVer  = nlfd->nGameProgramVer;
	m_nPatchProgramVer = nlfd->nPatchProgramVer ;

	CDebugSet::ToView ( "NET_MSG_LOGIN_FB, %d", nlfd->nResult );
	if ( m_pGlobalStage ) m_pGlobalStage->MsgProcess ( nmg );
}

// ä�ø޽���
void CNetClient::MsgChat(NET_MSG_GENERIC* nmg)
{
//	NET_MSG_CHAT_GLOBAL : // ������ �۷ι� �޽���
//	NET_MSG_CHAT_NORMAL : // �Ϲ��� ä�� �޽���
//	NET_MSG_CHAT_PARTY : // ��Ƽ������ ���޵Ǵ� �޽���
//	NET_MSG_CHAT_PRIVATE : // �������� �޽���, �Ӹ�, �ӻ���
//	NET_MSG_CHAT_GUILD : // �������� ���޵Ǵ� �޽���
}

/**
* Message Send Function. 
*/

int CNetClient::SndChaBasicInfo(int nChaNum)
{
	NET_CHA_BA_INFO ncbi;
	ncbi.nmg.nType	= NET_MSG_REQ_CHA_BINFO ;
	ncbi.nChaNum	= nChaNum;

	return Send((char *) &ncbi);
}

// Client �� ���� ������ ������ �����Ѵ�.
int CNetClient::SndVersion(int nGameProgramVer, int nPatchProgramVer)
{		
	NET_CLIENT_VERSION ncv;	
	ncv.nmg.nType			= NET_MSG_VERSION_INFO;
	ncv.nPatchProgramVer	= nPatchProgramVer;
	ncv.nGameProgramVer		= nGameProgramVer;

	return Send(reinterpret_cast<char *> (&ncv));
}

int CNetClient::SndHeartbeat()
{
	NET_HEARTBEAT_CLIENT_ANS msg;

	switch ( m_nClientNetState )
	{
		case NET_STATE_LOGIN : // �α��� ���� ���ӻ���
			return Send( reinterpret_cast<char *> (&msg), msg.nmg.dwSize );
		case NET_STATE_AGENT : // Agent
			{
				m_Tea.encrypt (m_szEncryptKeyHeart, ENCRYPT_KEY+1);

				//	StringCchCopy�� �ٲٸ� �ȵ�. ( �ι��ڿ������� �޸� ī���ؾߵ� )
				memcpy( msg.szEnCrypt,  m_szEncryptKeyHeart, ENCRYPT_KEY+1 );

				return SendToAgent( reinterpret_cast<char *> (&msg), msg.nmg.dwSize );	
			}			
		case NET_STATE_FIELD : // Field
			return SendToField( reinterpret_cast<char *> (&msg), msg.nmg.dwSize );		
		default :
			return 0;
	}
}

int	CNetClient::SndVersion(void)
{
	// ���Ϸ� ���� Ŭ���̾�Ʈ ������ �����Ѵ�.
	// if (GetClientVer() == NET_ERROR) 
	//	return NET_ERROR;
	return SndVersion( m_nGameProgramVer, m_nPatchProgramVer );
}

// ���Ӽ����� ������ ������ ��û�Ѵ�.
int CNetClient::SndReqServerInfo(void)
{
	{ // �ʱ�ȭ�� ������ ������ ���� ������ ������ �ִ�. UI���� �߸��� ������ ����Ѵ�.
		for ( int i=0; i<MAX_SERVER_GROUP; i++)
		{
			for ( int j=0; j<MAX_SERVER_NUMBER; j++)
			{
				::SecureZeroMemory(&m_sGame[i][j], sizeof(G_SERVER_CUR_INFO_LOGIN));
			}
		}

		m_bGameServerInfoEnd = FALSE;
	}

	int nSize;
	NET_MSG_GENERIC nmg;
	nSize		= sizeof(NET_MSG_GENERIC);
	nmg.dwSize	= (DWORD) nSize;
	nmg.nType	= NET_MSG_REQ_GAME_SVR;
	return Send( (char *) &nmg, nSize );
}

int CNetClient::SndChaAllBasicInfo()
{
	NET_CHA_REQ_BA_INFO ncrbi;
	// ncrbi.nChannel = nChannel;	
	return Send((char *) &ncrbi);
}

// ���λ����Ǵ� ĳ������ ������ �����Ѵ�.
int	CNetClient::SndCreateChaInfo(
	int nIndex,
	WORD wSchool,
	WORD wFace,
	WORD wHair,
	WORD wHairColor,
	WORD wSex,
	const char* strChaName )
{
	NET_NEW_CHA nnc;

	nnc.nIndex		= nIndex;
	nnc.wSchool		= wSchool;
	nnc.wFace		= wFace;
	nnc.wHair		= wHair;
	nnc.wHairColor	= wHairColor;
	nnc.wSex		= wSex;
	::StringCchCopy( nnc.szChaName, CHR_ID_LENGTH+1, strChaName );
	
	return Send((char *) &nnc);
}

// ����� ������ ��û�Ѵ�.
int CNetClient::SndRequestUserInfo(void)
{	
	int nSize;
	NET_MSG_GENERIC nmg;
	nSize		= sizeof(NET_MSG_GENERIC);
	nmg.dwSize	= (DWORD) nSize;
	nmg.nType	= NET_MSG_REQ_USER_INFO; // ����� ������û

	return Send((char *) &nmg);
}

/**
* ������ ����Ű�� ��û�Ѵ�.
* client->agent
*/
int CNetClient::SndRequestRandomKey(void)
{
	NET_REQ_RAND_KEY msg;	
	return Send((char*) &msg);
}

// client->Agent
// �ش�ĳ���ͷ� ���ӿ� �����Ѵ�.
int CNetClient::SndGameJoin(int nChaNum)
{
	NET_GAME_JOIN ngj;
	ngj.nmg.nType	= NET_MSG_LOBBY_GAME_JOIN;
	ngj.nChaNum		= nChaNum;

	return Send((char*) &ngj);
}

int CNetClient::SndChatNormal(const char* szName, const char* szMsg)
{
	NET_CHAT nc;	
	nc.nmg.nType	= NET_MSG_CHAT;
	nc.emType		= CHAT_TYPE_NORMAL;
	::StringCchCopy(nc.szName,    CHR_ID_LENGTH+1, szName);
	::StringCchCopy(nc.szChatMsg, CHAT_MSG_SIZE+1, szMsg);

	return Send((char*) &nc);
}

int CNetClient::SndChatParty ( const char* szMsg )
{
	NET_CHAT nc;
	nc.nmg.nType	= NET_MSG_CHAT;
	nc.emType		= CHAT_TYPE_PARTY;
	::StringCchCopy(nc.szChatMsg, CHAT_MSG_SIZE+1, szMsg);

	return Send((char*) &nc);
}

int CNetClient::SndChatPrivate(const char* szName, const char* szMsg)
{
	NET_CHAT nc;
	nc.nmg.nType	= NET_MSG_CHAT;
	nc.emType		= CHAT_TYPE_PRIVATE;
	::StringCchCopy(nc.szName,    CHR_ID_LENGTH+1, szName);
	::StringCchCopy(nc.szChatMsg, CHAT_MSG_SIZE+1, szMsg);
	
	return Send((char*) &nc);
}

int CNetClient::SndChatGuild(const char* szMsg)
{
	NET_CHAT nc;
	nc.nmg.nType	= NET_MSG_CHAT;
	nc.emType		= CHAT_TYPE_GUILD;
	::StringCchCopy(nc.szChatMsg, CHAT_MSG_SIZE+1, szMsg);
	
	return Send((char*) &nc);
}

int CNetClient::SndChatAlliance(const char* szMsg)
{
	NET_CHAT nc;
	nc.nmg.nType	= NET_MSG_CHAT;
	nc.emType		= CHAT_TYPE_ALLIANCE;
	::StringCchCopy(nc.szChatMsg, CHAT_MSG_SIZE+1, szMsg);
	
	return Send((char*) &nc);
}

// �����̺�Ʈ
// ������ 10 �ڸ�
int CNetClient::SndEventLottery(const char* szLotteryName)
{
	NET_EVENT_LOTTERY msg;
	::StringCchCopy(msg.szLotteryName, 11, szLotteryName);

	return Send((char*) &msg);
}

//
//                                       /;    ;\
//                                   __  \\____//
//                                  /{_\_/   `'\____
//                                  \___   (o)  (o  }
//       _____________________________/          :--'   DRINKA
//   ,-,'`@@@@@@@@       @@@@@@         \_    `__\
//  ;:(  @@@@@@@@@        @@@             \___(o'o)
//  :: )  @@@@          @@@@@@        ,'@@(  `===='        PINTA
//  :: : @@@@@:          @@@@         `@@@:
//  :: \  @@@@@:       @@@@@@@)    (  '@@@'
//  ;; /\      /`,    @@@@@@@@@\   :@@@@@)                   MILKA
//  ::/  )    {_----------------:  :~`,~~;
// ;;'`; :   )                  :  / `; ;
//;;;; : :   ;                  :  ;  ; :                        DAY !!!
//`'`' / :  :                   :  :  : :
//    )_ \__;      ";"          :_ ;  \_\       `,','
//    :__\  \    * `,'*         \  \  :  \   *  8`;'*  *
//        `^'     \ :/           `^'  `-^-'   \v/ :  \/   
// jgkim


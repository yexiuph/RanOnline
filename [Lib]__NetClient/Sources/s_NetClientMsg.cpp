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

// 주 메시지 처리 함수
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
		case NET_STATE_LOGIN : // 로그인 서버 접속상태
			MessageProcessLogin(pNmg);
			break;
		case NET_STATE_AGENT : // Agent
		case NET_STATE_FIELD : // Field
		case NET_STATE_BOARD : // Board
			MessageProcessGame(pNmg);
			break;		
		case NET_STATE_CTRL : // 관리프로그램 접속상태
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
	// 게임서버 상태정보
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
	// 버전
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
			// 게임 메시지, 채팅 메시지
			if ( nmg->nType > NET_MSG_LOBBY )
			{
				if ( m_pGlobalStage )	m_pGlobalStage->MsgProcess ( nmg );
			}
		}
		break;
	}
}

/**
* Server 에서 전송되어온 메시지 처리함수
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

// 서버에서 전송되어온 암호키
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

// 서버에서 전송되어온 버전 정보
int CNetClient::MsgVersionInfo(NET_MSG_GENERIC* nmg)
{
	NET_CLIENT_VERSION* ncv;
	ncv = reinterpret_cast<NET_CLIENT_VERSION*> (nmg);
	
	m_nGameProgramVer  = ncv->nGameProgramVer;
	m_nPatchProgramVer = ncv->nPatchProgramVer;

	return 0;
}

// 서버에서 전송되어온 암호화키값
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
		// 사용자 캐릭터 번호:%d", ncbi->nChaNum[i]);
	}
}

void CNetClient::MsgGameSvrInfoEnd(NET_MSG_GENERIC* nmg)
{
	m_bGameServerInfoEnd = TRUE;
}

/**
* Login 서버에서 전송되어온 Game 서버 정보
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

// 새로운 버전 다운로드 메시지 처리 함수
void CNetClient::MsgNewVersionDown()
{
	// todo : 프로그램 패치프로세서	
	// Test code : 웹브라우저 다운로드 페이지 띄우기...
	//ShellExecute(NULL,
	//	"open",
	//	AlphaDownload,
	//	NULL,
	//	NULL,
	//	SW_SHOWNORMAL);	
}

void CNetClient::RegisterNewAccount(void)
{
	// Test code : 웹브라우저 회원가입 페이지 띄우기...
	//ShellExecute(NULL,
	//	"open",
	//	AlphaRegister,
	//	NULL,
	//	NULL,
	//	SW_SHOWNORMAL);
}

// 로그인 피드백 데이타
void CNetClient::MsgLoginFeedBack(NET_MSG_GENERIC* nmg)
{
	NET_LOGIN_FEEDBACK_DATA* nlfd;
	nlfd = (NET_LOGIN_FEEDBACK_DATA *) nmg;	

	m_nGameProgramVer  = nlfd->nGameProgramVer;
	m_nPatchProgramVer = nlfd->nPatchProgramVer ;

	CDebugSet::ToView ( "NET_MSG_LOGIN_FB, %d", nlfd->nResult );
	if ( m_pGlobalStage ) m_pGlobalStage->MsgProcess ( nmg );
}

// 채팅메시지
void CNetClient::MsgChat(NET_MSG_GENERIC* nmg)
{
//	NET_MSG_CHAT_GLOBAL : // 관리용 글로벌 메시지
//	NET_MSG_CHAT_NORMAL : // 일반적 채팅 메시지
//	NET_MSG_CHAT_PARTY : // 파티원에게 전달되는 메시지
//	NET_MSG_CHAT_PRIVATE : // 개인적인 메시지, 귓말, 속삭임
//	NET_MSG_CHAT_GUILD : // 길드원에게 전달되는 메시지
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

// Client 의 버전 정보를 서버로 전송한다.
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
		case NET_STATE_LOGIN : // 로그인 서버 접속상태
			return Send( reinterpret_cast<char *> (&msg), msg.nmg.dwSize );
		case NET_STATE_AGENT : // Agent
			{
				m_Tea.encrypt (m_szEncryptKeyHeart, ENCRYPT_KEY+1);

				//	StringCchCopy로 바꾸면 안됨. ( 널문자열때문에 메모리 카피해야됨 )
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
	// 파일로 부터 클라이언트 버전을 세팅한다.
	// if (GetClientVer() == NET_ERROR) 
	//	return NET_ERROR;
	return SndVersion( m_nGameProgramVer, m_nPatchProgramVer );
}

// 게임서버의 정보를 서버에 요청한다.
int CNetClient::SndReqServerInfo(void)
{
	{ // 초기화를 해주지 않으면 이전 정보를 가지고 있다. UI에서 잘못된 정보를 출력한다.
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

// 새로생성되는 캐릭터의 정보를 전송한다.
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

// 사용자 정보를 요청한다.
int CNetClient::SndRequestUserInfo(void)
{	
	int nSize;
	NET_MSG_GENERIC nmg;
	nSize		= sizeof(NET_MSG_GENERIC);
	nmg.dwSize	= (DWORD) nSize;
	nmg.nType	= NET_MSG_REQ_USER_INFO; // 사용자 정보요청

	return Send((char *) &nmg);
}

/**
* 서버에 랜덤키를 요청한다.
* client->agent
*/
int CNetClient::SndRequestRandomKey(void)
{
	NET_REQ_RAND_KEY msg;	
	return Send((char*) &msg);
}

// client->Agent
// 해당캐릭터로 게임에 조인한다.
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

// 복권이벤트
// 복권은 10 자리
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


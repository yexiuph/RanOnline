///////////////////////////////////////////////////////////////////////////////
// s_CNetUser.cpp
// 
// class CNetUser
//
// * History
// 2003.02.18 jgkim 
//
// Copyright(c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_CNetUser.h"
#include "s_CConsoleMessage.h"
#include "GLGaeaServer.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CNetUser::CNetUser()
	: m_pSendManager(NULL)
	, m_pRcvManager(NULL)
	//, m_dwClientGaeaID(GAEAID_NULL)
{
	m_pSendManager = new CSendMsgBuffer();
	m_pRcvManager  = new CRcvMsgBuffer();
	Reset();
}

CNetUser::~CNetUser()
{
	SAFE_DELETE(m_pSendManager);
	SAFE_DELETE(m_pRcvManager);
}

int CNetUser::SetAcceptedClient(
	SOCKET s )
{
	LockOn();

	if (s == INVALID_SOCKET)
	{
		LockOff();
		return NET_ERROR;
	}
	else
	{
		if (m_Socket != INVALID_SOCKET)
		{
			CConsoleMessage::GetInstance()->Write( _T("CNetUser::SetAcceptedClient m_Socket != INVALID_SOCKET") );
			//CloseSocket();
			//LockOff();
			//return NET_ERROR;
			int nRetCode = ::closesocket(m_Socket);
			if (nRetCode == SOCKET_ERROR)
			{
				nRetCode = ::WSAGetLastError();
				CConsoleMessage::GetInstance()->Write(
					_T("CNetUser::SetAcceptedClient closesocket %d"),
					nRetCode);
			}
		}

		int	nSize = 0;
		sockaddr_in	sAddrIn;

		::SecureZeroMemory(&sAddrIn, sizeof(sockaddr_in));
		m_Socket = s;
		nSize = sizeof(sockaddr_in);

		// Get client ip address and port
		::getpeername(m_Socket, (sockaddr *) &sAddrIn, &nSize);
		::StringCchCopy(m_szIp, MAX_IP_LENGTH+1, ::inet_ntoa(sAddrIn.sin_addr));
		m_usPort			= ::ntohs( sAddrIn.sin_port );
		m_dwLastMsgTime		= timeGetTime();
		m_dwLastSaveTime	= m_dwLastMsgTime;
		m_emOnline			= NET_ONLINE;
		m_bHeartBeat		= true;
		
		m_csa.Init(); // nProtect GameGuard 초기화

		LockOff();
		return NET_OK;
	}
}

static int nCloseClientError = 0;

void CNetUser::CloseSocket()
{
	if (m_Socket != INVALID_SOCKET)
	{
		::shutdown( m_Socket, SD_BOTH );
		int nRetCode = ::closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;

		if (nRetCode == SOCKET_ERROR)
		{
			nRetCode = ::WSAGetLastError();
			CConsoleMessage::GetInstance()->Write( _T("CNetUser::CloseClient NET_ONLINE closesocket %d"), nRetCode);
		}
	}
}

/*
bool CNetUser::CloseClient( DWORD dwClientID )
{
	LockOn();

	bool bReturn = false;

	int nGetRecvCount = getRecvCount();
	int nGetSendCount = getSendCount();

	switch (m_emOnline)
	{
	case NET_ONLINE:		
		m_csa.Close(); // nProtect GameGuard 종료

		if ( 0 == nGetRecvCount && 0 == nGetSendCount )
		{
			//CConsoleMessage::GetInstance()->Write("CNetUser[%u]::CloseClient, NET_ONLINE 0/0", dwClientID);

			CloseSocket();

			Reset();
			bReturn = true;
		}
		else
		{
			m_emOnline = NET_CLOSE_WAIT;
			if (1 == nGetRecvCount && 0 == nGetSendCount)
			{
				CloseSocket();
			}
			bReturn = false;

			//CConsoleMessage::GetInstance()->Write("CNetUser[%u]::CloseClient, NET_ONLINE, Recv:%d/Send:%d", 
			//									dwClientID, 
			//									nGetRecvCount, 
			//									nGetSendCount );
		}
		break;

	case NET_CLOSE_WAIT:
		if ( 0 == nGetRecvCount && 0 == nGetSendCount )
		{
			//CConsoleMessage::GetInstance()->Write("CNetUser[%u]::CloseClient, NET_CLOSE_WAIT, 0/0", dwClientID);

			CloseSocket();

			Reset();
			bReturn = true;
		}
		else if( 1 == nGetRecvCount && 0 == nGetSendCount )
		{
			//CConsoleMessage::GetInstance()->Write("CNetUser[%u]::CloseClient, NET_CLOSE_WAIT, Recv:1/Send:0", dwClientID );

			CloseSocket();

			//Reset();
			//bReturn = true;
		}
		else
		{
			// Memo : 클라이언트 연결 종료 신호이면 바로 끊어버린다.
			//
			//if( dwClientID != UINT_MAX )
			//{
			//	CloseSocket();

			//	Reset();
			//	bReturn = true;

			//	CConsoleMessage::GetInstance()->Write("CNetUser[%u]::CloseClient, NET_CLOSE_WAIT(dwClientID != UINT_MAX), Recv:%d/Send:%d", 
			//		dwClientID, 
			//		nGetRecvCount, 
			//		nGetSendCount );
			//}

			CConsoleMessage::GetInstance()->Write("CNetUser[%u]::CloseClient, NET_CLOSE_WAIT, Recv:%d/Send:%d", 
												dwClientID, 
												nGetRecvCount, 
												nGetSendCount );
		}
		break;

	case NET_OFFLINE:
		bReturn = true;
		//CConsoleMessage::GetInstance()->Write("CNetUser[%u]::CloseClient, NET_OFFLINE, Recv:%d/Send:%d", 
		//									dwClientID, 
		//									nGetRecvCount, 
		//									nGetSendCount );
		break;

	default:
		bReturn = false;
		break;
	}
	LockOff();
	return bReturn;
}
*/

int CNetUser::CloseClient()
{
	int nRetCode = 0;
	LockOn();

	if (m_emOnline == NET_ONLINE)
	{
		m_csa.Close(); // nProtect GameGuard 종료

		nRetCode = ::closesocket(m_Socket);
		if (nRetCode == SOCKET_ERROR)
		{
			nRetCode = ::WSAGetLastError();
			if (nRetCode == WSANOTINITIALISED) // A successful WSAStartup call must occur before using this function. 
			{
				CConsoleMessage::GetInstance()->Write("ERROR:CNetUser::CloseClient closesocket WSANOTINITIALISED ERROR");
			}
			else if (nRetCode == WSAENETDOWN) // The network subsystem has failed. 
			{
				CConsoleMessage::GetInstance()->Write("ERROR:CNetUser::CloseClient closesocket WSAENETDOWN ERROR");
			}
			else if (nRetCode == WSAENOTSOCK) // The descriptor is not a socket. 
			{
				CConsoleMessage::GetInstance()->Write("ERROR:CNetUser::CloseClient closesocket WSAENOTSOCK ERROR");
			}
			else if (nRetCode == WSAEINPROGRESS) // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
			{
				CConsoleMessage::GetInstance()->Write("ERROR:CNetUser::CloseClient closesocket WSAEINPROGRESS ERROR");
			}
			else if (nRetCode == WSAEINTR) // The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall. 
			{
				CConsoleMessage::GetInstance()->Write("ERROR:CNetUser::CloseClient closesocket WSAEINTR ERROR");
			}
			else if (nRetCode == WSAEWOULDBLOCK) // The socket is marked as nonblocking and SO_LINGER is set to a nonzero time-out value. 
			{
				CConsoleMessage::GetInstance()->Write("ERROR:CNetUser::CloseClient closesocket WSAEWOULDBLOCK ERROR");
			}
			else
			{
				CConsoleMessage::GetInstance()->Write("ERROR:CNetUser::CloseClient closesocket unknown ERROR %d", nRetCode);
			}
		}
		Reset();
		LockOff();
		return NET_OK;
	}
	else
	{
		if (nCloseClientError < 100)
		{
			CConsoleMessage::GetInstance()->WriteConsole(
				_T("ERROR:CNetUser::CloseClient client already closed %d"),
				nCloseClientError );
			nCloseClientError++;
		}		
		LockOff();
		return NET_ERROR;
	}
}

void CNetUser::Reset()
{
	// m_nInCount          = 0; // 받기 작업 횟수
	// m_nOutCount         = 0; // 보내기 작업 횟수

	m_Socket			= INVALID_SOCKET;
	m_nNetMode			= NET_PACKET_HEAD; // Packet Head 를 받을것인가 Body 를 받을것인가 결정	
	m_emOnline			= NET_OFFLINE;
	m_usPort			= 0;
	m_dwLastMsgTime		= 0; // 마지막으로 읽기 통신이 된 시간
	m_dwLastSaveTime	= 0; // 마지막으로 캐릭터를 저장한 시간
	m_nPing				= 0; // 클라이언트와 통신에 걸린 시간
	m_wChaRemain		= 0;
	m_wChaTestRemain	= 0;

	m_nChaNum			= 0;
	m_nUserType         = 0;    
	m_dwGaeaID			= GAEAID_NULL;
	m_dwSlotAgentClient	= 0; // Agent 서버에서의 Client 와의 통신 슬롯
	m_dwSlotAgentField	= 0; // Agent 서버에서의 Field 서버와의 통신 슬롯
	m_dwSlotFieldAgent	= 0; // Field 서버에서의 Agent 서버와의 통신 슬롯
	m_dwSlotFieldClient	= 0; // Field 서버에서의 Client 와의 통신 슬롯
	m_dwSlotType		= 0; // 슬롯의 타입

	m_uib.nKey			= 0;
	m_uib.nKeyDirection = 0;
	m_uib.nUserNum		= -1;
	m_bACCOUNT_PASS		= false;
	m_bACCOUNT_PASSING  = false;

	m_ck.nKey			= 0;
	m_ck.nKeyDirection  = 0;

	m_bHeartBeat		= false;
	m_nChannel          = -1;
	m_wInvalidPacketCount = 0; // 잘못된 패킷을 보낸 횟수

    CTime cTemp(1970, 2, 1, 0, 0, 0); // 프리미엄 서비스 만료일을 세팅한다. (초기값)
    m_tPremiumDate      = cTemp.GetTime();

	CTime cTemp2(1970, 2, 1, 0, 0, 0); // 채팅 블록 만료일을 세팅한다. (초기값)
	m_tChatBlockDate    = cTemp2.GetTime();

	m_nRandomPass = 0;

	//m_dwSendIoCount = 0; //! 보내기 요청횟수
	//m_dwRecvIoCount = 0; //! 받기 요청 횟수

	memset( m_uib.szUserID,	0, sizeof(TCHAR) * (USR_ID_LENGTH+1) );
	memset( m_szIp,	0, sizeof(TCHAR) * (MAX_IP_LENGTH+1) );
	memset( m_szDecodedTID, 0, sizeof(TCHAR) * (TERRA_TID_DECODE+1) );
	memset( m_szUUID, 0, sizeof(TCHAR) * (UUID_STR_LENGTH) );
	memset( m_szEncryptKeyHeart, 0, sizeof(TCHAR) * (ENCRYPT_KEY+1) );
	// memset( m_szGspUserID, 0, sizeof(TCHAR) * (GSP_USERID) );

	// ResetBuffer();

	m_pSendManager->reset();
	m_pRcvManager->reset();

	m_wLoginAttempt = 0;       // 한번 접속에 로그인 요청 횟수

	m_nMyCC_Class	= 0;	   // 말레이시아 등급 변수

	//////////////////////////////////////////////////////////////////////////////
	// Thailande
	m_wThaiFlag = 0; // Thailand 사용자 플래그
	m_wThaiTodayGamePlayTime = 0; // Thailand 오늘 게임플레이 타임e
	m_nThaiCC_Class = 0; // 태국 User Class Type
	//////////////////////////////////////////////////////////////////////////////
	// nProtect
	m_bNProtectAuth = true;
	m_bNProtectFirstCheck = true;
	m_bNProtectFirstAuthPass = false;

	//////////////////////////////////////////////////////////////////////////////
	// 극강부 변수 초기화
	m_nExtremeM = 0;
	m_nExtremeW = 0;
	m_nExtremeCheckM = 0;
	m_nExtremeCheckW = 0;

	m_nExtremeDBM = 0;
	m_nExtremeDBW = 0;

	// 중국 로그인 시간 체크
	m_nChinaGameTime = 0;
	m_nChinaOfflineTime = 0;
	CTime cTemp3(1970, 2, 1, 0, 0, 0); // China LastLoginDate를 세팅한다. (초기값)
	m_tLastLoginTime    = cTemp3.GetTime();	

	// 베트남 탐닉방지 관련 추가
	m_nVTGameTime	= 0;

	m_nChinaUserAge = 0;

	m_wConSendError = 0;

	ZeroMemory( m_szRecvGarbageMsg[0], 12 );
	ZeroMemory( m_szRecvGarbageMsg[1], 12 );
//	ZeroMemory( m_szRecvGarbageMsg[2], 12 );

	m_dwGarbageNum = 0;
}

void CNetUser::GspSetUUID( const TCHAR* szUUID )
{
	if (szUUID == NULL) {
		return;
	} else {
		StringCchCopy( m_szUUID, UUID_STR_LENGTH, szUUID );
	}
}

TCHAR* CNetUser::GspGetUUID()
{
	return m_szUUID;
}

void CNetUser::SetEncKey( const TCHAR* szEncKey )
{
	if (szEncKey == NULL) {
		return;
	} else {
		StringCchCopy( m_szEncryptKeyHeart, ENCRYPT_KEY+1, szEncKey );
	}
}

TCHAR* CNetUser::GetEncKey()
{
	return m_szEncryptKeyHeart;
}

// 한국 로그인 방식 변경에 따른 추가
void CNetUser::KorSetUUID( const TCHAR* szUUID )
{
    if (szUUID == NULL) {
		return;
	} else {
		StringCchCopy( m_szUUID, UUID_STR_LENGTH, szUUID );
	}
}

TCHAR* CNetUser::KorGetUUID()
{
	return m_szUUID;
}

// 일본 WhiteRock System과의 통신을 위해 추가
void CNetUser::JPSetUUID( const TCHAR* szUUID )
{
    if (szUUID == NULL) {
		return;
	} else {
		StringCchCopy( m_szUUID, UUID_STR_LENGTH, szUUID );
	}
}

TCHAR* CNetUser::JPGetUUID()
{
	return m_szUUID;
}

void CNetUser::TerraSetDecodedTID( const TCHAR* szDecodedTID )
{
	if (szDecodedTID == NULL) {
		return;
	} else {
		StringCchCopy( m_szDecodedTID, TERRA_TID_DECODE+1, szDecodedTID );
	}
}

TCHAR* CNetUser::TerraGetDecodedTID()
{
    return m_szDecodedTID;
}

void CNetUser::ResetUserID()
{
	memset(m_uib.szUserID, 0, sizeof(TCHAR) * USR_ID_LENGTH);	
}

//! 클라이언트 버퍼 리셋
void CNetUser::ResetRcvBuffer()
{
    m_pRcvManager->reset();	
}

//! 클라이언트에서 받은 메시지를 넣는다.
int CNetUser::addRcvMsg(void* pMsg, DWORD dwSize)
{
	ASSERT(pMsg);

	if ((pMsg == NULL) || (dwSize > NET_DATA_BUFSIZE)) return NET_ERROR;	
	
	if (!IsOnline())
	{
		CConsoleMessage::GetInstance()->Write(_T("CNetUser::InsertBuffer IsOnline false"));
		Reset();

		return NET_ERROR;
	}

	// 현재 까지 받은 데이터 사이즈를 체크한다.
	int nRcvSize = (int) dwSize;
	int nTotalRcvSize = m_pRcvManager->getRcvSize()+nRcvSize;
	
	// 총 받은 데이터 사이즈가 최대 버퍼사이즈보다 작아야 한다.	
	if (nTotalRcvSize > NET_DATA_CLIENT_MSG_BUFSIZE)
	{
		CConsoleMessage::GetInstance()->Write(_T("ERROR:%d %s Buffer Over"), 
			                                  nTotalRcvSize,
											  m_szIp);
        ResetRcvBuffer();		

		return NET_ERROR;
	}
	else
	{
		m_pRcvManager->addRcvMsg(pMsg, nRcvSize);


		return NET_OK;
	}
}

//! 클라이언트에서 받는 메시지만 쓰레기 값을 체크한다.
void CNetUser::CheckGarbageMsg(void *msg)
{
	char szTempChar[12];
	strcpy_s( szTempChar, m_pRcvManager->getGarbageMsg() );

	if( strcmp( szTempChar, m_szRecvGarbageMsg[0] ) != 0 &&
		strcmp( szTempChar, m_szRecvGarbageMsg[1] ) != 0 &&
		strlen( szTempChar ) != 0 )
	{
		strcpy_s( m_szRecvGarbageMsg[m_dwGarbageNum], szTempChar );
		m_dwGarbageNum++;
		if( m_dwGarbageNum >= 2 ) m_dwGarbageNum = 0;		
	}else{
		NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*)msg;

		char szTempChar2[128] = {0,};
		if( pNmg->nType < NET_MSG_LOBBY ) sprintf_s( szTempChar2, "LOGIN MESSAGE!! LOGIN_MSG NUM %d, MSG NUM %d", NET_MSG_LGIN, (DWORD)pNmg->nType );
		else if( pNmg->nType < NET_MSG_GCTRL ) sprintf_s( szTempChar2, "LOBBY MESSAGE!! LOBBY_MSG NUM %d, MSG NUM %d", NET_MSG_LOBBY, (DWORD)pNmg->nType );
		else sprintf_s( szTempChar2, "GTRL MESSAGE!! GTRL_MSG NUM %d, MSG NUM %d", NET_MSG_GCTRL, (DWORD)pNmg->nType );

		CConsoleMessage::GetInstance()->Write("CNetUser::Client invalid Garbage Msg!! Channel %d, IP %s, UserNum %d, ID %s, GarbageMSG %s %s",
											  m_nChannel, m_szIp, m_uib.nUserNum, m_uib.szUserID, szTempChar, szTempChar2 );
	}
}

//! 클라이언트에서 받은 메시지를 하나 가져온다.
void* CNetUser::getRcvMsg( bool bClient )
{	
	if (!IsOnline())
	{
		CConsoleMessage::GetInstance()->Write("CNetUser::GetMsg IsOnline false");
		Reset();		
		return NULL;
	}
	else
	{
		if( m_dwSlotType == NET_SLOT_AGENT	 || m_dwSlotType == NET_SLOT_FIELD ||
			m_dwSlotType == NET_SLOT_SESSION || m_dwSlotType == NET_SLOT_FLDSVR )
			bClient = FALSE;


		if( bClient )
		{
			void* msg = m_pRcvManager->getMsg(bClient);
			if( msg != NULL ) CheckGarbageMsg(msg);
			else if( strlen( m_pRcvManager->getGarbageMsg() ) == 0 )
			{
				CConsoleMessage::GetInstance()->Write("CNetUser::Client invalid Msg!! Channel %d, IP %s, UserNum %d, ID %s",
					m_nChannel, m_szIp, m_uib.nUserNum, m_uib.szUserID );
			}
			return msg;
		}else{
			return m_pRcvManager->getMsg(bClient);
		}
		
	}
}

//bool CNetUser::IsOnline()
//{	
//	return m_nOnline == NET_ONLINE ? 1 : 0;
//}

//void CNetUser::SetOnLine()
//{
//	m_nOnline = NET_ONLINE;
//}

//void CNetUser::SetOffLine()
//{
//	m_nOnline = NET_OFFLINE;
//}

void CNetUser::SetSocket(SOCKET s)
{
	m_Socket = s;
}

void CNetUser::SetCryptKey(CRYPT_KEY ck)
{
	LockOn();
	m_ck = ck;
	LockOff();
}

CRYPT_KEY CNetUser::GetCryptKey()
{
	return m_ck;
}

SOCKET CNetUser::GetSOCKET()
{
	return m_Socket;
}

void CNetUser::SetIP( TCHAR* szIP )
{
	if (szIP == NULL) return;
	::StringCchCopy(m_szIp, MAX_IP_LENGTH+1, szIP);
}

TCHAR* CNetUser::GetIP()
{
	return m_szIp;
}

void CNetUser::SetPort(USHORT usPort)
{
	m_usPort = usPort;
}

USHORT CNetUser::GetPort()
{
	return m_usPort;
}

TCHAR* CNetUser::GspGetUserID()
{
	return m_uib.szUserID;
}

void CNetUser::GspSetUserID( const TCHAR* szGspUserID )
{
	if (szGspUserID == NULL) return;
	::StringCchCopy( m_uib.szUserID, GSP_USERID, szGspUserID );
}

void CNetUser::SetUserID( const TCHAR* szUserID )
{
	if (szUserID == NULL) return;
	::StringCchCopy(m_uib.szUserID, USR_ID_LENGTH+1, szUserID);	
}

TCHAR* CNetUser::GetUserID()
{
	return m_uib.szUserID;
}

void CNetUser::SetUserNum(int nUserNum)
{
	m_uib.nUserNum = nUserNum;
}

int CNetUser::GetUserNum()
{
	return m_uib.nUserNum;
}

void CNetUser::SetLoginTime()
{
	 m_LoginTime = CTime::GetCurrentTime();
}

void CNetUser::SetNetMode(int nMode)
{
	m_nNetMode = nMode;
}

int	CNetUser::GetNetMode()
{
	return m_nNetMode;
}

void CNetUser::SetGaeaID(DWORD dwGaeaID)
{
	m_dwGaeaID = dwGaeaID;
}

DWORD CNetUser::GetGaeaID()
{
	return m_dwGaeaID;
}



void CNetUser::SetChaNum(int nChaNum)
{
	m_nChaNum = nChaNum;
}

int CNetUser::GetChaNum()
{
	return m_nChaNum;
}

CTime CNetUser::GetLoginTime()
{
	return m_LoginTime;
}

/*
void CNetUser::SetLastMsgTime()
{
	m_dwLastMsgTime	= ::timeGetTime();
}
*/

/*
void CNetUser::SetLastMsgTime(DWORD dwTime)
{
	m_dwLastMsgTime	= dwTime;
}
*/

/*
DWORD CNetUser::GetLastMsgTime()
{
	return m_dwLastMsgTime;
}
*/

/*
void CNetUser::SetLastSaveTime()
{
	m_dwLastSaveTime = ::timeGetTime();
}
*/

/*
void CNetUser::SetLastSaveTime(DWORD dwTime)
{
	m_dwLastSaveTime = dwTime;
}
*/

/*
DWORD CNetUser::GetLastSaveTime()
{
	return m_dwLastSaveTime;
}
*/

void CNetUser::SetSlotAgentClient(DWORD dwSlot) // Agent 서버에서의 Client 와의 통신 슬롯
{
	m_dwSlotAgentClient	= dwSlot;
}
void CNetUser::SetSlotAgentField (DWORD dwSlot) // Agent 서버에서의 Field 서버와의 통신 슬롯
{
	m_dwSlotAgentField	= dwSlot;
}

// Field 서버에서의 Agent 서버와의 통신 슬롯
void CNetUser::SetSlotFieldAgent( DWORD dwSlot )
{
	m_dwSlotFieldAgent	= dwSlot;
}

void CNetUser::SetSlotFieldClient(DWORD dwSlot) // Field 서버에서의 Client 와의 통신 슬롯
{
	m_dwSlotFieldClient	= dwSlot;
}

DWORD CNetUser::GetSlotAgentClient() // Agent 서버에서의 Client 와의 통신 슬롯
{
	return m_dwSlotAgentClient;
}

DWORD CNetUser::GetSlotAgentField () // Agent 서버에서의 Field 서버와의 통신 슬롯
{
	return m_dwSlotAgentField;
}

DWORD CNetUser::GetSlotFieldAgent () // Field 서버에서의 Agent 서버와의 통신 슬롯
{
	return m_dwSlotFieldAgent;
}

DWORD CNetUser::GetSlotFieldClient() // Field 서버에서의 Client 와의 통신 슬롯
{
	return m_dwSlotFieldClient;
}

void CNetUser::SetSlotType( DWORD dwType )
{
	m_dwSlotType = dwType;
}

DWORD CNetUser::GetSlotType()
{
	return m_dwSlotType;
}

//bool CNetUser::IsClientSlot()
//{
//	return m_dwSlotType == NET_SLOT_CLIENT ? 1 : 0;
//}

//bool CNetUser::IsFieldSlot()
//{
//	return m_dwSlotType == NET_SLOT_FIELD ? 1 : 0;
//}

//bool CNetUser::IsAgentSlot()
//{
//	return m_dwSlotType == NET_SLOT_AGENT ? 1 : 0;
//}


//void CNetUser::ResetHeartBeat()
//{
//	m_bHeartBeat = false;
//}
//
//void CNetUser::SetHeartBeat()
//{
//	m_bHeartBeat = true;
//}
//
//bool CNetUser::CheckHeartBeat()
//{
//	return m_bHeartBeat == true ? 1 : 0;
//}

void CNetUser::SetUserType(int nType)
{
	m_nUserType = nType;
}

int	CNetUser::GetUserType()
{
	return m_nUserType;
}

void CNetUser::SetChaRemain( WORD wNum )
{
    m_wChaRemain = wNum;
}

void CNetUser::SetChaTestRemain(WORD wNum)
{
	m_wChaTestRemain = wNum;
}

WORD CNetUser::GetChaRemain()
{
	return m_wChaRemain;
}

WORD CNetUser::GetChaTestRemain()
{
	return m_wChaTestRemain;
}

__time64_t CNetUser::GetPremiumDate()
{
    return m_tPremiumDate;
}

void CNetUser::SetPremiumDate(__time64_t tTime)
{
    m_tPremiumDate = tTime;
}

void CNetUser::SetChinaGameTime( int nChinaGameTime )
{
	m_nChinaGameTime = nChinaGameTime;
}

void CNetUser::SetChinaOfflineTime( int nChinaOfflineTime )
{
	m_nChinaOfflineTime = nChinaOfflineTime;
}

void CNetUser::SetLastLoginDate( __time64_t tTime )
{
	m_tLastLoginTime = tTime;
}

void CNetUser::SetVTGameTime( int nVTGameTime )
{
	m_nVTGameTime = nVTGameTime;
}

// 베트남 탐닉방지 추가
int CNetUser::GetVTGameTime()
{
	return m_nVTGameTime;
}

int CNetUser::GetChinaGameTime()
{
	return m_nChinaGameTime;
}

int CNetUser::GetChinaOfflineTime()
{
	return m_nChinaOfflineTime;
}

__time64_t CNetUser::GetLastLoginDate()
{
	return m_tLastLoginTime;
}

void CNetUser::SetChinaUserAge( int nChinaUserAge )
{
	m_nChinaUserAge = nChinaUserAge;
}

int CNetUser::GetChinaUserAge()
{
	return m_nChinaUserAge;
}

__time64_t CNetUser::GetChatBlockDate()
{
	return m_tChatBlockDate;
}

void CNetUser::SetChatBlockDate(__time64_t tTime)
{
	m_tChatBlockDate = tTime;
}

void CNetUser::SetChannel(int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:CNetUser::SetChannel CH:%d"),
											  nChannel);
		return;
	}
	else
	{
		m_nChannel = nChannel;
	}
}

int CNetUser::GetChannel()
{
	return m_nChannel;
}

bool CNetUser::IsAccountPass()
{
	return m_bACCOUNT_PASS;
}

void CNetUser::SetAccountPass(bool bPASS)
{
	m_bACCOUNT_PASS = bPASS;
}

// 계정검사 통과중인지 검사
bool CNetUser::IsAccountPassing()
{
	return m_bACCOUNT_PASSING;
}

// 계정검사 통과중인지 세팅
void CNetUser::SetAccountPassing(bool bPassing)
{
	m_bACCOUNT_PASSING = bPassing;
}

WORD CNetUser::ThaiGetFlag ()
{
	return m_wThaiFlag;
}

void CNetUser::ThaiSetFlag (WORD wFlag)
{
	m_wThaiFlag = wFlag;
}

void CNetUser::ThaiSetTodayPlayTime (WORD wMinutes)
{
	m_wThaiTodayGamePlayTime = wMinutes;
}

bool CNetUser::ThaiIs3HourOver (CTime& currentTime)
{
	CTimeSpan ts = currentTime - m_LoginTime; // 로그인 시간부터 현재시간까지 계산

	// 오늘 총 플레이한 시간 계산
	LONGLONG llTotalTime = (LONGLONG) m_wThaiTodayGamePlayTime + ts.GetTotalMinutes();

	// 180 분 (3시간) 지났는지 체크
	if (llTotalTime >= THAI_LIMIT_MIN)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//! 보내야할 메시지를 등록한다.
int CNetUser::addSendMsg( void* pMsg, DWORD dwSize )
{
	return m_pSendManager->addMsg(pMsg, dwSize);
}

//! 보내야할 메시지 버퍼를 가져온다.
void* CNetUser::getSendBuffer()
{
	return m_pSendManager->getSendBuffer();
}

//! 보내야할 메시지 크기를 가져온다.
int CNetUser::getSendSize()
{
	return m_pSendManager->getSendSize();
}

/*
void CNetUser::NetInCountIncrease()
{
	m_nInCount++;
}
*/

/*
void CNetUser::NetOutCountIncrease()
{
	m_nOutCount++;
}
*/

/*
void CNetUser::NetInCountDecrease()
{
	m_nInCount--;
}
*/

/*
void CNetUser::NetOutCountDecrease()
{
	m_nOutCount--;
}
*/

/*
int CNetUser::NetGetInCount()
{
	return m_nInCount;
}
*/

/*
int CNetUser::NetGetOutCount()
{
	return m_nOutCount;
}
*/

//! 말레이지아 요청으로 1 회 접속시 5 회 이상 로그인 요청시 
//! 사용자의 접속을 끊음...
//! 로그인 요청 횟수 증가시킴
void CNetUser::IncLoginAttempt()
{
	++m_wLoginAttempt;
}

//! 로그인 요청 횟수 가져옴
WORD CNetUser::GetLoginAttempt()
{
	return m_wLoginAttempt;
}

void CNetUser::SetRandomPassNumber(int nNumber)
{
	if (nNumber > 0)
	{
		m_nRandomPass = nNumber;
	}
	else
	{
		m_nRandomPass = 1;
	}
}

int CNetUser::GetRandomPassNumber()
{
	return m_nRandomPass;
}

// 극강부 남자 생성 가능 수를 등록한다.
void CNetUser::SetExtremeCreateM( int nExtremeM )
{
	m_nExtremeM = nExtremeM;	
}

// 극강부 여자 생성 가능 수를 등록한다.
void CNetUser::SetExtremeCreateW( int nExtremeW )
{
	m_nExtremeW = nExtremeW;
}

// 극강부 남자 생성 가능을 등록한다.
void CNetUser::SetExtremeCheckM( int nExtremeCheckM )
{
	m_nExtremeCheckM = nExtremeCheckM;
}

// 극강부 여자 생성 가능을 등록한다.
void CNetUser::SetExtremeCheckW( int nExtremeCheckW )
{
	m_nExtremeCheckW = nExtremeCheckW;
}

// 극강부 남자 생성 가능 수를 불러온다.
int CNetUser::GetExtremeM()
{
	return m_nExtremeM;
}

// 극강부 여자 생성 가능 수를 불러온다.
int CNetUser::GetExtremeW()
{
	return m_nExtremeW;
}

// 극강부 생성 가능을 불러온다.
// 0 : 현재 생성할려는 캐릭터가 극강부가 아니다.
// 1 : 현재 생성할려는 캐릭터가 극강부이다.
int CNetUser::GetExtremeCheckM()
{
	return m_nExtremeCheckM;
}

int CNetUser::GetExtremeCheckW()
{
	return m_nExtremeCheckW;
}

// DB에서 연산된 극강부 남자 생성 가능 수를 등록한다.
void CNetUser::SetExtremeCreateDBM( int nExtremeDBM )
{
	m_nExtremeDBM = nExtremeDBM;	
}

// DB에서 연산된 극강부 여자 생성 가능 수를 등록한다.
void CNetUser::SetExtremeCreateDBW( int nExtremeDBW )
{
	m_nExtremeDBW = nExtremeDBW;
}

// DB에서 연산된 극강부 남자 생성 가능 수를 불러온다.
int CNetUser::GetExtremeDBM()
{
	return m_nExtremeDBM;
}

// DB에서 연산된 극강부 여자 생성 가능 수를 불러온다.
int CNetUser::GetExtremeDBW()
{
	return m_nExtremeDBW;
}

//// 연결할 퍌드 서버의 정보를 설정한다
//void CNetUser::SetConnectionFieldInfo( DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
//{
//	m_dwClientGaeaID = dwGaeaID;
//	memcpy( &m_nmg, nmg, sizeof(m_nmg) );
//}
//	
//// 연결할 필드 서버의 정보를 사용했으면 초기화한다.
//void CNetUser::ResetConnectionFieldInfo()
//{
//	m_dwClientGaeaID = GAEAID_NULL;
//	SecureZeroMemory( &m_nmg, sizeof(m_nmg) );
//}

// 태국 User Class Type을 설정한다.
void CNetUser::SetThaiClass( int nThaiCC_Class )
{
	m_nThaiCC_Class = nThaiCC_Class;
}

// 태국 User Class Type을 가져온다.
int CNetUser::GetThaiClass()
{
	return m_nThaiCC_Class;
}

// 말레이시아 PC방 이벤트
// 말레이시아 User Class Type을 설정한다.
void CNetUser::SetMyClass( int nMyCC_Class )
{
	m_nMyCC_Class = nMyCC_Class;
}

// 말레이시아 User Class Type을 가져온다.
int CNetUser::GetMyClass()
{
	return m_nMyCC_Class;
}

/*
int CNetUser::incRecvCount()
{
	int nCount = InterlockedIncrement( (LPLONG) &m_dwRecvIoCount );

	//CConsoleMessage::GetInstance()->Write("CNetUser[%u]::incRecvCount Count[%d], RecvIoCount[%u]", m_dwGaeaID, nCount, m_dwRecvIoCount);

	return nCount;
}

int CNetUser::decRecvCount()
{
	int nCount = (m_dwRecvIoCount ? InterlockedDecrement( (LPLONG) &m_dwRecvIoCount ) : 0);

	//CConsoleMessage::GetInstance()->Write("CNetUser[%u]::decRecvCount Count[%d], RecvIoCount[%u]", m_dwGaeaID, nCount, m_dwRecvIoCount);

	return nCount;
}

int CNetUser::getRecvCount()
{
	return m_dwRecvIoCount;
}

int CNetUser::incSendCount()
{
	int nCount = InterlockedIncrement( (LPLONG) &m_dwSendIoCount );

	//CConsoleMessage::GetInstance()->Write("CNetUser[%u]::incSendCount Count[%d], SendIOCount[%u]", m_dwGaeaID, nCount, m_dwSendIoCount);

	return nCount;
}

int CNetUser::decSendCount()
{
	int nCount = (m_dwSendIoCount ? InterlockedDecrement( (LPLONG) &m_dwSendIoCount ) : 0);

	if (m_dwSendIoCount==0 && NET_CLOSE_WAIT == m_emOnline )
	{
		//CConsoleMessage::GetInstance()->Write("CNetUser[%u]::decSendCount, NET_CLOSE_WAIT, Count[%d]", m_dwGaeaID, nCount);
		CloseClient();
	}

	//CConsoleMessage::GetInstance()->Write("CNetUser[%u]::decSendCount Count[%d], SendIoCount[%u]", m_dwGaeaID, nCount, m_dwSendIoCount);

	return nCount;
}

int CNetUser::getSendCount()
{
	return m_dwSendIoCount;
}
*/
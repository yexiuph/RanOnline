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
		
		m_csa.Init(); // nProtect GameGuard �ʱ�ȭ

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
		m_csa.Close(); // nProtect GameGuard ����

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
			// Memo : Ŭ���̾�Ʈ ���� ���� ��ȣ�̸� �ٷ� ���������.
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
		m_csa.Close(); // nProtect GameGuard ����

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
	// m_nInCount          = 0; // �ޱ� �۾� Ƚ��
	// m_nOutCount         = 0; // ������ �۾� Ƚ��

	m_Socket			= INVALID_SOCKET;
	m_nNetMode			= NET_PACKET_HEAD; // Packet Head �� �������ΰ� Body �� �������ΰ� ����	
	m_emOnline			= NET_OFFLINE;
	m_usPort			= 0;
	m_dwLastMsgTime		= 0; // ���������� �б� ����� �� �ð�
	m_dwLastSaveTime	= 0; // ���������� ĳ���͸� ������ �ð�
	m_nPing				= 0; // Ŭ���̾�Ʈ�� ��ſ� �ɸ� �ð�
	m_wChaRemain		= 0;
	m_wChaTestRemain	= 0;

	m_nChaNum			= 0;
	m_nUserType         = 0;    
	m_dwGaeaID			= GAEAID_NULL;
	m_dwSlotAgentClient	= 0; // Agent ���������� Client ���� ��� ����
	m_dwSlotAgentField	= 0; // Agent ���������� Field �������� ��� ����
	m_dwSlotFieldAgent	= 0; // Field ���������� Agent �������� ��� ����
	m_dwSlotFieldClient	= 0; // Field ���������� Client ���� ��� ����
	m_dwSlotType		= 0; // ������ Ÿ��

	m_uib.nKey			= 0;
	m_uib.nKeyDirection = 0;
	m_uib.nUserNum		= -1;
	m_bACCOUNT_PASS		= false;
	m_bACCOUNT_PASSING  = false;

	m_ck.nKey			= 0;
	m_ck.nKeyDirection  = 0;

	m_bHeartBeat		= false;
	m_nChannel          = -1;
	m_wInvalidPacketCount = 0; // �߸��� ��Ŷ�� ���� Ƚ��

    CTime cTemp(1970, 2, 1, 0, 0, 0); // �����̾� ���� �������� �����Ѵ�. (�ʱⰪ)
    m_tPremiumDate      = cTemp.GetTime();

	CTime cTemp2(1970, 2, 1, 0, 0, 0); // ä�� ��� �������� �����Ѵ�. (�ʱⰪ)
	m_tChatBlockDate    = cTemp2.GetTime();

	m_nRandomPass = 0;

	//m_dwSendIoCount = 0; //! ������ ��ûȽ��
	//m_dwRecvIoCount = 0; //! �ޱ� ��û Ƚ��

	memset( m_uib.szUserID,	0, sizeof(TCHAR) * (USR_ID_LENGTH+1) );
	memset( m_szIp,	0, sizeof(TCHAR) * (MAX_IP_LENGTH+1) );
	memset( m_szDecodedTID, 0, sizeof(TCHAR) * (TERRA_TID_DECODE+1) );
	memset( m_szUUID, 0, sizeof(TCHAR) * (UUID_STR_LENGTH) );
	memset( m_szEncryptKeyHeart, 0, sizeof(TCHAR) * (ENCRYPT_KEY+1) );
	// memset( m_szGspUserID, 0, sizeof(TCHAR) * (GSP_USERID) );

	// ResetBuffer();

	m_pSendManager->reset();
	m_pRcvManager->reset();

	m_wLoginAttempt = 0;       // �ѹ� ���ӿ� �α��� ��û Ƚ��

	m_nMyCC_Class	= 0;	   // �����̽þ� ��� ����

	//////////////////////////////////////////////////////////////////////////////
	// Thailande
	m_wThaiFlag = 0; // Thailand ����� �÷���
	m_wThaiTodayGamePlayTime = 0; // Thailand ���� �����÷��� Ÿ��e
	m_nThaiCC_Class = 0; // �±� User Class Type
	//////////////////////////////////////////////////////////////////////////////
	// nProtect
	m_bNProtectAuth = true;
	m_bNProtectFirstCheck = true;
	m_bNProtectFirstAuthPass = false;

	//////////////////////////////////////////////////////////////////////////////
	// �ذ��� ���� �ʱ�ȭ
	m_nExtremeM = 0;
	m_nExtremeW = 0;
	m_nExtremeCheckM = 0;
	m_nExtremeCheckW = 0;

	m_nExtremeDBM = 0;
	m_nExtremeDBW = 0;

	// �߱� �α��� �ð� üũ
	m_nChinaGameTime = 0;
	m_nChinaOfflineTime = 0;
	CTime cTemp3(1970, 2, 1, 0, 0, 0); // China LastLoginDate�� �����Ѵ�. (�ʱⰪ)
	m_tLastLoginTime    = cTemp3.GetTime();	

	// ��Ʈ�� Ž�й��� ���� �߰�
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

// �ѱ� �α��� ��� ���濡 ���� �߰�
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

// �Ϻ� WhiteRock System���� ����� ���� �߰�
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

//! Ŭ���̾�Ʈ ���� ����
void CNetUser::ResetRcvBuffer()
{
    m_pRcvManager->reset();	
}

//! Ŭ���̾�Ʈ���� ���� �޽����� �ִ´�.
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

	// ���� ���� ���� ������ ����� üũ�Ѵ�.
	int nRcvSize = (int) dwSize;
	int nTotalRcvSize = m_pRcvManager->getRcvSize()+nRcvSize;
	
	// �� ���� ������ ����� �ִ� ���ۻ������ �۾ƾ� �Ѵ�.	
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

//! Ŭ���̾�Ʈ���� �޴� �޽����� ������ ���� üũ�Ѵ�.
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

//! Ŭ���̾�Ʈ���� ���� �޽����� �ϳ� �����´�.
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

void CNetUser::SetSlotAgentClient(DWORD dwSlot) // Agent ���������� Client ���� ��� ����
{
	m_dwSlotAgentClient	= dwSlot;
}
void CNetUser::SetSlotAgentField (DWORD dwSlot) // Agent ���������� Field �������� ��� ����
{
	m_dwSlotAgentField	= dwSlot;
}

// Field ���������� Agent �������� ��� ����
void CNetUser::SetSlotFieldAgent( DWORD dwSlot )
{
	m_dwSlotFieldAgent	= dwSlot;
}

void CNetUser::SetSlotFieldClient(DWORD dwSlot) // Field ���������� Client ���� ��� ����
{
	m_dwSlotFieldClient	= dwSlot;
}

DWORD CNetUser::GetSlotAgentClient() // Agent ���������� Client ���� ��� ����
{
	return m_dwSlotAgentClient;
}

DWORD CNetUser::GetSlotAgentField () // Agent ���������� Field �������� ��� ����
{
	return m_dwSlotAgentField;
}

DWORD CNetUser::GetSlotFieldAgent () // Field ���������� Agent �������� ��� ����
{
	return m_dwSlotFieldAgent;
}

DWORD CNetUser::GetSlotFieldClient() // Field ���������� Client ���� ��� ����
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

// ��Ʈ�� Ž�й��� �߰�
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

// �����˻� ��������� �˻�
bool CNetUser::IsAccountPassing()
{
	return m_bACCOUNT_PASSING;
}

// �����˻� ��������� ����
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
	CTimeSpan ts = currentTime - m_LoginTime; // �α��� �ð����� ����ð����� ���

	// ���� �� �÷����� �ð� ���
	LONGLONG llTotalTime = (LONGLONG) m_wThaiTodayGamePlayTime + ts.GetTotalMinutes();

	// 180 �� (3�ð�) �������� üũ
	if (llTotalTime >= THAI_LIMIT_MIN)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//! �������� �޽����� ����Ѵ�.
int CNetUser::addSendMsg( void* pMsg, DWORD dwSize )
{
	return m_pSendManager->addMsg(pMsg, dwSize);
}

//! �������� �޽��� ���۸� �����´�.
void* CNetUser::getSendBuffer()
{
	return m_pSendManager->getSendBuffer();
}

//! �������� �޽��� ũ�⸦ �����´�.
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

//! ���������� ��û���� 1 ȸ ���ӽ� 5 ȸ �̻� �α��� ��û�� 
//! ������� ������ ����...
//! �α��� ��û Ƚ�� ������Ŵ
void CNetUser::IncLoginAttempt()
{
	++m_wLoginAttempt;
}

//! �α��� ��û Ƚ�� ������
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

// �ذ��� ���� ���� ���� ���� ����Ѵ�.
void CNetUser::SetExtremeCreateM( int nExtremeM )
{
	m_nExtremeM = nExtremeM;	
}

// �ذ��� ���� ���� ���� ���� ����Ѵ�.
void CNetUser::SetExtremeCreateW( int nExtremeW )
{
	m_nExtremeW = nExtremeW;
}

// �ذ��� ���� ���� ������ ����Ѵ�.
void CNetUser::SetExtremeCheckM( int nExtremeCheckM )
{
	m_nExtremeCheckM = nExtremeCheckM;
}

// �ذ��� ���� ���� ������ ����Ѵ�.
void CNetUser::SetExtremeCheckW( int nExtremeCheckW )
{
	m_nExtremeCheckW = nExtremeCheckW;
}

// �ذ��� ���� ���� ���� ���� �ҷ��´�.
int CNetUser::GetExtremeM()
{
	return m_nExtremeM;
}

// �ذ��� ���� ���� ���� ���� �ҷ��´�.
int CNetUser::GetExtremeW()
{
	return m_nExtremeW;
}

// �ذ��� ���� ������ �ҷ��´�.
// 0 : ���� �����ҷ��� ĳ���Ͱ� �ذ��ΰ� �ƴϴ�.
// 1 : ���� �����ҷ��� ĳ���Ͱ� �ذ����̴�.
int CNetUser::GetExtremeCheckM()
{
	return m_nExtremeCheckM;
}

int CNetUser::GetExtremeCheckW()
{
	return m_nExtremeCheckW;
}

// DB���� ����� �ذ��� ���� ���� ���� ���� ����Ѵ�.
void CNetUser::SetExtremeCreateDBM( int nExtremeDBM )
{
	m_nExtremeDBM = nExtremeDBM;	
}

// DB���� ����� �ذ��� ���� ���� ���� ���� ����Ѵ�.
void CNetUser::SetExtremeCreateDBW( int nExtremeDBW )
{
	m_nExtremeDBW = nExtremeDBW;
}

// DB���� ����� �ذ��� ���� ���� ���� ���� �ҷ��´�.
int CNetUser::GetExtremeDBM()
{
	return m_nExtremeDBM;
}

// DB���� ����� �ذ��� ���� ���� ���� ���� �ҷ��´�.
int CNetUser::GetExtremeDBW()
{
	return m_nExtremeDBW;
}

//// ������ ���� ������ ������ �����Ѵ�
//void CNetUser::SetConnectionFieldInfo( DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
//{
//	m_dwClientGaeaID = dwGaeaID;
//	memcpy( &m_nmg, nmg, sizeof(m_nmg) );
//}
//	
//// ������ �ʵ� ������ ������ ��������� �ʱ�ȭ�Ѵ�.
//void CNetUser::ResetConnectionFieldInfo()
//{
//	m_dwClientGaeaID = GAEAID_NULL;
//	SecureZeroMemory( &m_nmg, sizeof(m_nmg) );
//}

// �±� User Class Type�� �����Ѵ�.
void CNetUser::SetThaiClass( int nThaiCC_Class )
{
	m_nThaiCC_Class = nThaiCC_Class;
}

// �±� User Class Type�� �����´�.
int CNetUser::GetThaiClass()
{
	return m_nThaiCC_Class;
}

// �����̽þ� PC�� �̺�Ʈ
// �����̽þ� User Class Type�� �����Ѵ�.
void CNetUser::SetMyClass( int nMyCC_Class )
{
	m_nMyCC_Class = nMyCC_Class;
}

// �����̽þ� User Class Type�� �����´�.
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
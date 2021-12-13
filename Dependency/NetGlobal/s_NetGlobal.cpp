///////////////////////////////////////////////////////////////////////////////
// s_NetGlobal.cpp
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_NetGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// All communication library must call this function
// Load winsock2 library
int NET_InitializeSocket(void)
{	
	// Load Winsock 2
	WSADATA wsd;
	int nRetCode;	
	nRetCode = ::WSAStartup(MAKEWORD(2,2), &wsd);
	if (nRetCode != 0) 
	{
		return NET_ERROR;
	} 
	else 
	{
		return NET_OK;
	}	
}

///////////////////////////////////////////////////////////////////////////////
// Unload Winsock 2
void NET_CloseSocket(void)
{
	::WSACleanup();
}

// Return socket error string.
CString NET_ERROR_STRING(int nCode)
{
	CString strTemp;
	switch(nCode)
	{
	case WSA_INVALID_HANDLE : // 6  The handle is invalid.
		strTemp.Format("WSA_INVALID_HANDLE %d", nCode);
		break;
	case WSA_NOT_ENOUGH_MEMORY : // 8 Not enough storage is available to process this command.
		strTemp.Format("WSA_NOT_ENOUGH_MEMORY %d", nCode);
		break;
	case WSA_INVALID_PARAMETER : // 87  The parameter is incorrect.
		strTemp.Format("WSA_INVALID_PARAMETER %d", nCode);
		break;
	case WSAEINTR : // 10004 Interrupted function call.
		strTemp.Format("WSAEINTR %d", nCode);
		break;
	case WSAEACCES : // 10013 Permission denied.
		strTemp.Format("WSAEACCES %d", nCode);
		break;
	case WSAEFAULT : // 10014 Bad address.
		strTemp.Format("WSAEFAULT %d", nCode);
		break;
	case WSAEINVAL : // 10022 Invalid argument. 
		strTemp.Format("WSAEINVAL %d", nCode);
		break;
	case WSAEMFILE : // 10024 Too many open files. 
		strTemp.Format("WSAEMFILE %d", nCode);
		break;
	case WSAEWOULDBLOCK : // 10035 Resource temporarily unavailable. 
		strTemp.Format("WSAEWOULDBLOCK %d", nCode);
		break;
	case WSAEINPROGRESS : // 10036 Operation now in progress.
		strTemp.Format("WSAEINPROGRESS %d", nCode);
		break;
	case WSAEALREADY : // 10037 Operation already in progress. 
		strTemp.Format("WSAEALREADY %d", nCode);
		break;
	case WSAENOTSOCK : // 10038 Socket operation on nonsocket.
		strTemp.Format("WSAENOTSOCK %d", nCode);
		break;
	case WSAEDESTADDRREQ : // 10039 Destination address required.
		strTemp.Format("WSAEDESTADDRREQ %d", nCode);
		break;
	case WSAEMSGSIZE : // 10040 Message too long.
		strTemp.Format("WSAEMSGSIZE %d", nCode);
		break;
	case WSAEPROTOTYPE : // 10041 Protocol wrong type for socket.
		strTemp.Format("WSAEPROTOTYPE %d", nCode);
		break;
	case WSAENOPROTOOPT : // 10042 Bad protocol option.
		strTemp.Format(" %d", nCode);
		break;
	case WSAEPROTONOSUPPORT : // 10043 Protocol not supported. 
		strTemp.Format("WSAEPROTONOSUPPORT %d", nCode);
		break;
	case WSAESOCKTNOSUPPORT : // 10044 The support for the specified socket type does not exist in this address family.
		strTemp.Format("WSAESOCKTNOSUPPORT %d", nCode);
		break;
	case WSAEOPNOTSUPP : // 10045 The attempted operation is not supported for the type of object referenced.
		strTemp.Format("WSAEOPNOTSUPP %d", nCode);
		break;
	case WSAEPFNOSUPPORT : // 10046 The protocol family has not been configured into the system or no implementation for it exists.
		strTemp.Format("WSAEPFNOSUPPORT %d", nCode);
		break;
	case WSAEAFNOSUPPORT : // 10047 An address incompatible with the requested protocol was used.
		strTemp.Format("WSAEAFNOSUPPORT %d", nCode);
		break;
	case WSAEADDRINUSE : // 10048 Only one usage of each socket address (protocol/network address/port) is normally permitted.
		strTemp.Format("WSAEADDRINUSE %d", nCode);
		break;
	case WSAEADDRNOTAVAIL : // 10049 The requested address is not valid in its context.
		strTemp.Format("WSAEADDRNOTAVAIL %d", nCode);
		break;
	case WSAENETDOWN : // 10050 A socket operation encountered a dead network.
		strTemp.Format("WSAENETDOWN %d", nCode);
		break;
	case WSAENETUNREACH : // 10051 A socket operation was attempted to an unreachable network.
		strTemp.Format("WSAENETUNREACH %d", nCode);
		break;
	case WSAENETRESET : // 10052 The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress.
		strTemp.Format("WSAENETRESET %d", nCode);
		break;
	case WSAECONNABORTED : // 10053 An established connection was aborted by the software in your host machine.
		strTemp.Format("WSAECONNABORTED %d", nCode);
		break;
	case WSAECONNRESET : // 10054 An existing connection was forcibly closed by the remote host.
		strTemp.Format("WSAECONNRESET %d", nCode);
		break;
	case WSAENOBUFS : // 10055 An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full.
		strTemp.Format("WSAENOBUFS %d", nCode);
		break;
	case WSAEISCONN : // 10056 A connect request was made on an already connected socket.
		strTemp.Format("WSAEISCONN %d", nCode);
		break;
	case WSAENOTCONN : // 10057 A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied.
		strTemp.Format("WSAENOTCONN %d", nCode);
		break;
	case WSAESHUTDOWN : // 10058 A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call.
		strTemp.Format("WSAESHUTDOWN %d", nCode);
		break;
	case WSAETOOMANYREFS : // 10059 Too many references to some kernel object.
		strTemp.Format("WSAETOOMANYREFS %d", nCode);
		break;
	case WSAETIMEDOUT : // 10060 A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond.
		strTemp.Format("WSAETIMEDOUT %d", nCode);
		break;
	case WSAECONNREFUSED : // 10061 No connection could be made because the target machine actively refused it.
		strTemp.Format("WSAECONNREFUSED %d", nCode);
		break;
	case WSAELOOP : // 10062 Cannot translate name.
		strTemp.Format("WSAELOOP %d", nCode);
		break;
	case WSAENAMETOOLONG : // 10063 Name component or name was too long. 
		strTemp.Format("WSAENAMETOOLONG %d", nCode);
		break;
	case WSAEHOSTDOWN : // 10064 A socket operation failed because the destination host was down.
		strTemp.Format("WSAEHOSTDOWN %d", nCode);
		break;
	case WSAEHOSTUNREACH : // 10065 A socket operation was attempted to an unreachable host.
		strTemp.Format("WSAEHOSTUNREACH %d", nCode);
		break;
	case WSAENOTEMPTY : // 10066 Cannot remove a directory that is not empty.
		strTemp.Format("WSAENOTEMPTY %d", nCode);
		break;
	case WSAEPROCLIM : // 10067 A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously.
		strTemp.Format("WSAEPROCLIM %d", nCode);
		break;
	case WSAEUSERS : // 10068 Ran out of quota.
		strTemp.Format("WSAEUSERS %d", nCode);
		break;
	case WSAEDQUOT : // 10069 Ran out of disk quota.
		strTemp.Format("WSAEDQUOT %d", nCode);
		break;
	case WSAESTALE : // 10070 File handle reference is no longer available.
		strTemp.Format("WSAESTALE %d", nCode);
		break;
	case WSAEREMOTE :// 10071 Item is not available locally.
		strTemp.Format("WSAEREMOTE %d", nCode);
		break;
	case WSASYSNOTREADY : // 10091 WSAStartup cannot function at this time because the underlying system it uses to provide network services is currently unavailable.
		strTemp.Format("WSASYSNOTREADY %d", nCode);
		break;
	case WSAVERNOTSUPPORTED : // 10092 The Windows Sockets version requested is not supported.
		strTemp.Format("WSAVERNOTSUPPORTED %d", nCode);
		break;
	case WSANOTINITIALISED : // 10093 Either the application has not called WSAStartup, or WSAStartup failed.
		strTemp.Format("WSANOTINITIALISED %d", nCode);
		break;
	case WSAEDISCON : // 10101 Returned by WSARecv or WSARecvFrom to indicate the remote party has initiated a graceful shutdown sequence.
		strTemp.Format("WSAEDISCON %d", nCode);
		break;
	case WSAENOMORE : // 10102 No more results can be returned by WSALookupServiceNext.
		strTemp.Format("WSAENOMORE %d", nCode);
		break;
	case WSAECANCELLED : // 10103 A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
		strTemp.Format("WSAECANCELLED %d", nCode);
		break;
	case WSAEINVALIDPROCTABLE : // 10104 The procedure call table is invalid.
		strTemp.Format("WSAEINVALIDPROCTABLE %d", nCode);
		break;
	case WSAEINVALIDPROVIDER : // 10105 The requested service provider is invalid.
		strTemp.Format("WSAEINVALIDPROVIDER %d", nCode);
		break;
	case WSAEPROVIDERFAILEDINIT : // 10106 The requested service provider could not be loaded or initialized.
		strTemp.Format("WSAEPROVIDERFAILEDINIT %d", nCode);
		break;
	case WSASYSCALLFAILURE : // 10107 A system call that should never fail has failed.
		strTemp.Format("WSASYSCALLFAILURE %d", nCode);
		break;
	case WSASERVICE_NOT_FOUND : // 10108 No such service is known. The service cannot be found in the specified name space.
		strTemp.Format("WSASERVICE_NOT_FOUND %d", nCode);
		break;
	case WSATYPE_NOT_FOUND : // 10109 The specified class was not found.
		strTemp.Format("WSATYPE_NOT_FOUND %d", nCode);
		break;
	case WSA_E_NO_MORE : // 10110 No more results can be returned by WSALookupServiceNext.
		strTemp.Format("WSA_E_NO_MORE %d", nCode);
		break;
	case WSA_E_CANCELLED : // 10111 A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
		strTemp.Format("WSA_E_CANCELLED %d", nCode);
		break;
	case WSAEREFUSED : // 10112 A database query failed because it was actively refused.
		strTemp.Format("WSAEREFUSED %d", nCode);
		break;
	case WSAHOST_NOT_FOUND : // 11001 No such host is known.
		strTemp.Format("WSAHOST_NOT_FOUND %d", nCode);
		break;
	default :
		strTemp.Format("Unknown Socket ERROR code %d", nCode);
	}
	return strTemp;
}

/******************************************************************************
* GameGuard 에러 코드 체크 함수
* \param dwErrCode nProtect GameGuard 에러코드
*/
std::string GetGameGuardErrorString(DWORD dwErrCode)
{
	std::string strError;

	switch (dwErrCode)
	{
	case 1:
		// 메모리 할당 실패
		strError = _T("Memory allocation failed");
		break;
	case 2:
		// ggauth.dll 로드 실패
		strError = _T("ggauth.dll load failed");
		break;
	case 3:
		// ggauth.dll 의 Export 함수 가져오기 실패
		strError = _T("ggauth.dll function export error");
		break;
	case 4:
		// ggauth.dll 이 초기화 되기 전에 Export 함수 호출하였음
		strError = _T("Do not call function before InitGameguardAuth");
		break;
	case 10:
		// 함수 호출 시 invalid parameter 전달
		strError = _T("Invalid parameter function call");
		break;
	case 11: 
		// 인증 Query 에 대한 클라이언트의 응답 없음.
		strError = _T("Client did not response requested query");
		break;
	case 12:
		// 클라이언트의 인증 프로토콜 버전 틀림
		strError = _T("Not correct protocol version of client");
		break;
	case 13:
		// 인증 Query 에 대한 클라이언트의 응답값이 틀림
		strError = _T("Client response is wrong");
		break;
	case 101:
		// Gamemon 버전이 틀림
		strError = _T("Gamemon version is incorrect");
		break;
	case 102:
		// Gamemon 버전 확인위한 코드값이 틀림
		strError = _T("Code is wrong for Gamemon version check");
		break;
	default:
		strError = _T("Unknown Error");
		break;
	}
	return strError;
}
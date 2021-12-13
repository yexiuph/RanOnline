#ifndef S_NETGLOBAL_H_
#define S_NETGLOBAL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * \ingroup NetServerLib
 *
 *
 * \par requirements
 * Client win98 or later WinSock2\n
 * Server win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2002.05.30
 *
 * \author Jgkim
 *
 * \par license
 * Copyright(c) Mincoms. All rights reserved.
 * 
 * \todo 
 *
 * \bug 
 *
 */

#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE

#include <winsock2.h>
#include <atltime.h>
#include <Mmsystem.h>
#include "minLzo.h"
#include "minUuid.h"

#include "GLDefine.h"

// nProtect GameGuard Server module
#include "ggsrv.h"

// Apex RSA
#include "gamecode.h"

// Must Install Platform SDK
// Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE
#include "strsafe.h" // Safe string function

// Auto Link Library Files
#pragma comment (lib, "ws2_32.lib") // Windows socket2 library
#pragma comment (lib, "Winmm.lib") // Windows multimedia library
#pragma comment (lib, "strsafe.lib") // Safe string library
#pragma comment (lib, "ggsrvlib.lib") // nProtect GameGuard server library

static CMinLzo* g_pMinLzo;

// DELETE Function definition
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

// Definitions
// #define can changed to const...
// ex) const int NET_NOTIRY = WM_USER + 99;
#define NET_NOTIFY					WM_USER + 99  // Network Notify Message

//////////////////////////////////////////////////////////////////////////////
// server type [type]
// 1 : Login server
// 2 : Session server
// 3 : Field server
// 4 : Agent server, Game Server
#define NET_SERVER_LOGIN	        1
#define NET_SERVER_SESSION	        2
#define NET_SERVER_FIELD	        3
#define NET_SERVER_AGENT	        4

//////////////////////////////////////////////////////////////////////////////
// Client Slot Type (CNetUser)
#define NET_SLOT_CLIENT				1
#define NET_SLOT_AGENT				2
#define NET_SLOT_FIELD				3
#define NET_SLOT_SESSION			4
#define NET_SLOT_FLDSVR				5

#define NET_DEFAULT_PORT			5001 

#define NET_DATA_BUFSIZE			1024    // Data Buffer Size
#define NET_DATA_MSG_BUFSIZE		4096    // Client Message Buffer Size
#define NET_RECV_POSTED				1		// IOCP receive post
#define NET_SEND_POSTED				0		// IOCP send post
#define NET_DATA_CLIENT_MSG_BUFSIZE 16384   // 8192

#define NET_OK						1
#define NET_ERROR					-1
#define NET_CLIENT_LISTEN			50
#define NET_TIME_OUT				180000	// 1000 msec * 60 sec * 3 = 3 minutes
#define NET_BLOCK_TIME              1800000 // 1000 msec * 60 sec * 30 = 30분동안 block 등록을 한다.

// Connection protocols
// This version not support UDP protocol
#define NET_TCP						0
#define NET_UDP						1

enum EM_NET_STATE
{
	NET_OFFLINE    = 0, //! 연결 종료
	NET_ONLINE     = 1, //! 연결된 상태
	//NET_CLOSE_WAIT = 2, //! 연결 종료 대기상태
};

//#define NET_ONLINE					1
//#define NET_OFFLINE					0

#define NET_MAX_CLIENT				1000	///< Default max client number of per server

#define MAX_WORKER_THREAD		    6       ///< 숫자가 4개이하일때 딜레이발생
                                            ///< 의도적으로 6 개로 했다.
#ifndef MAX_CHAR_LENGTH
#define MAX_CHAR_LENGTH	            255
#endif

#define MAX_SERVER_GROUP			20		///< 최대 서버그룹
#define MAX_SERVER_NUMBER			10		///< 서버그룹당 최대 서버숫자

#define MAX_IP_LENGTH               20      ///< IP 번호 문자열 최대값
#define SVR_STATE_UPDATE_TIME		180000	///< 300000 msec = 5 min:게임서버 상태 업데이트

#define FIELDSERVER_MAX				10      ///< 채널당 최대 필드서버 갯수
#define MAX_CHANNEL_NUMBER          10      ///< 서버당 최대 채널 갯수

#define NET_RESERVED_SLOT			100     ///< FIELDSERVER_MAX * MAX_CHANNEL_NUMBER 
                                            ///< 서버간 통신을 위해 예약된 클라이언트 소켓 slot
//#define NET_SESSION_START           90
//#define NET_SESSION_END             99

#define NET_RESERVED_SESSION		99      ///< NET_RESERVED_SLOT - 1	             
                                            ///< 세션서버와의 통신을 위해 예약된 클라이언트 소켓 slot

#define NET_STATE_LOGIN				1		///< 로그인서버 접속상태
#define NET_STATE_AGENT				2		///< 게임서버 접속상태
#define NET_STATE_FIELD				3		///< 게임서버 접속상태
#define NET_STATE_BOARD				4		///< 게임서버 접속상태
#define NET_STATE_CTRL				5		///< 원격 컨트롤 프로그램 접속상태

#define NET_PACKET_HEAD				0
#define NET_PACKET_BODY				1

// string data size
#define USR_ID_LENGTH				20
#define USR_PASS_LENGTH				USR_ID_LENGTH
#define CHR_ID_LENGTH				33
#define USR_RAND_PASS_LENGTH        6
#define MAX_TRACING_NUM				20

#define GLOBAL_CHAT_MSG_SIZE		981
#define CHAT_MSG_SIZE				100
#define TRACING_LOG_SIZE			256
#define DB_SVR_NAME_LENGTH			50
#define DB_NAME_LENGTH				50

// #define MAX_SERVERCHAR_NUM			2	// 최대 생성할 수 있는 캐릭터 수	
// 극강부 추가
#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(_RELEASED) || defined ( KR_PARAM ) || defined ( TW_PARAM ) || defined ( HK_PARAM )|| defined( TH_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( CH_PARAM ) || defined ( PH_PARAM ) || defined ( JP_PARAM ) 
#define MAX_ONESERVERCHAR_NUM		16 // 한 서버에서 만들수 있는 캐릭터 갯수
#else
#define MAX_ONESERVERCHAR_NUM		4 // 한 서버에서 만들수 있는 캐릭터 갯수
#endif

#define SERVER_NAME_LENGTH			50
#define MAP_NAME_LENGTH				50
#define MAX_APEX_PACKET_LENGTH		640

#define ENCRYPT_KEY					12

//////////////////////////////////////////////////////////////////////////////
// "GID": GameID   (사용자가 직접 입력했던 ID) - e.g. test1     16 자리
// "UID": UniqueID (시스템이 부여한 사용자 ID) - e.g. 15ftgr     6 자리
// "SSNHEAD": 생년월일 - e.g. 731201                             6 자리
// "SEX": 성별(M:남자, F:여자)                                   1 자리
// "NAMECHECK": 실명확인 여부(1=했음, 0=안했음)                  1 자리
// "IP": 접속한 IP
// "STATUS": 사용자 상태(A: 정회원, W: 준회원(기획예정), D: 탈퇴신청회원, B: 불량유저)
#define DAUM_MAX_PARAM_LENGTH       500
#define DAUM_MAX_GID_LENGTH         20
#define DAUM_MAX_UID_LENGTH         20
#define DAUM_MAX_SSNHEAD_LENGTH     7
#define DAUM_MAX_SEX_LENGTH         2
#define DAUM_USERPASS				20
#define KOR_MAX_GID_LENGTH			20
#define KOR_USERIP					20
#define KOR_ALLOW_TIME_ERROR        20
#define KOR_ROWCOUNT_ERROR	        21
#define KOR_ALLOW_TIME_DIFF_PLUS   21600 // 6 Hour
#define KOR_ALLOW_TIME_DIFF_MINUS -3600 // 1 Hour

//////////////////////////////////////////////////////////////////////////////
// TLoginName (varchar 15):	The most important field which identifies a unique gamer. 
//                          Use this as key to tables in your RAN Game Database 
//                          to maintain a record of each gamer. 
// TGender (char):		    Gender of the gamer 
// TIcNo   (char 15): 		Identity Number (User_No) of the gamer 
// TDOB    (char 10):       Date of Birth of the gamer. 
#define TERRA_TID_ENCODE            96
#define TERRA_TID_DECODE            36
#define TERRA_TLOGIN_NAME           15
#define TERRA_TGENDER               1
#define TERRA_TICNO                 15
#define TERRA_TDOB                  10
#define TERRA_USERPASS				20

// GSP Global Server Platform
#define GSP_USERID	51
#define GSP_USERIP  16
#define GSP_ALLOW_TIME_DIFF_PLUS   21600 // 6 Hour
#define GSP_ALLOW_TIME_DIFF_MINUS -3600 // 1 Hour
#define GSP_ALLOW_TIME_ERROR       20

//////////////////////////////////////////////////////////////////////////////
// Excite Japan
#define EXCITE_USER_ID				20
#define EXCITE_TDATE				14
#define EXCITE_MD5					32
#define EXCITE_SEC_PASS				16
#define EXCITE_SEC_ID				16

//////////////////////////////////////////////////////////////////////////////
// Gonzo Japan
#define JAPAN_USER_ID				16
#define JAPAN_USER_PASS				16

// Grobal Service
#define GS_USER_ID			20
#define GS_USER_PASS		20


//! 서버의 타입
enum EMSEVERTYPE
{
	SERVER_LOGIN	= 1,
	SERVER_SESSION	= 2,
	SERVER_FIELD	= 3,
	SERVER_AGENT	= 4,
	SERVER_BOARD	= 5,
};

enum EM_SMS
{
	SMS_DEFAULT  = 0,
	SMS_LENGTH   = 80, // SMS 의 전체길이 제한.
	SMS_SENDER   = 14, // SMS 보내는 사람 길이.
	SMS_RECEIVER = 14  // SMS 받는 사람 길이.
};

//! 해킹프로그램 DB 로그용 번호
enum EMHACKPROGRAM
{
	DEFAULT_HACK_NUM  =   0,
	NPROTECT_HACK_NUM = 998,
	APEX_HACK_NUM     = 999, // APEX 로그
};

//! 태국용 설정값들
#define THAI_LIMIT_MIN 180 // 태국은 18 세 이하는 하루 180 분 동안만 게임가능
#define THAI_CHECK_TIME 60000 // 1000 msec X 60 = 1 min

//! 태국 나이제한
enum EMTHAIUSERFLAG
{
	THAI_UNDER18  = 0, // 18 세 이하
	THAI_OVER18   = 1, // 18 세 이상
	THAI_FREETIME = 2, // 24 시간 사용가능
};

//! 사용자 타입설정
enum EMUSERTYPE
{
    USER_COMMON  =  0, // 일반사용자
    USER_PREMIUM =  1, // 프리미엄 유저
	USER_SPECIAL = 10, // 특별사용자 (게임방 업체 사장, 기자 등등)
	USER_GM4     = 19, // GM 4 급, Web GM 용
	USER_GM3	 = 20, // GM 3 급
	USER_GM2	 = 21, // GM 2 급
	USER_GM1	 = 22, // GM 1 급
	USER_MASTER  = 30, // Master
};

enum EM_NEW_CHA_FB
{
	EM_CHA_NEW_FB_SUB_OK	= 1, // 캐릭터 생성 성공
	EM_CHA_NEW_FB_SUB_DUP	= 2, // 게임서버->클라이언트 : 같은 이름의 캐릭터가 이미 존재함
	EM_CHA_NEW_FB_SUB_LENGTH= 3, // 캐릭터 글자수 제한
	EM_CHA_NEW_FB_SUB_MAX	= 4, // 게임서버->클라이언트 : 더이상캐릭터 만들수 없음(최대캐릭터 숫자 초과)
	EM_CHA_NEW_FB_SUB_ERROR = 5, // 게임서버->클라이언트 : 에러발생 만들수 없음(시스템에러, 허용되지 않는 이름)
	EM_CHA_NEW_FB_SUB_THAICHAR_ERROR = 6, // 태국어문자 완성되지 않은 문자 조합 에러
	EM_CHA_NEW_FB_SUB_VNCHAR_ERROR = 7,	 // 베트남 문자 조합 에러
	EM_CHA_NEW_FB_SUB_BAD_NAME = 8,
};

enum EMNET_MSG_CHAT
{
	CHAT_TYPE_GLOBAL		= (1), // 관리용 글로벌 메시지
	CHAT_TYPE_NORMAL		= (2), // 일반적 채팅 메시지
	CHAT_TYPE_PARTY			= (3), // 파티원에게 전달되는 메시지
	CHAT_TYPE_PRIVATE		= (4), // 개인적인 메시지, 귓말, 속삭임
	CHAT_TYPE_GUILD			= (5), // 길드원에게 전달되는 메시지
	CHAT_TYPE_LOUDSPEAKER	= (6), // 확성기.
	CHAT_TYPE_ALLIANCE		= (7), // 동맹에게 전달되는 메시지

	CHAT_TYPE_CTRL_GLOBAL	= (8),
	CHAT_TYPE_CTRL_GLOBAL2	= (9),
};

// 서비스 제공업자
enum EMSERVICE_PROVIDER
{
	SP_MINCOMS  = 0, // Korea (Mincoms)
	SP_KOREA    = 1, // Korea (Daum game)
	SP_TAIWAN   = 2, // Taiwan (Feya) 
	SP_CHINA    = 3, // China
	SP_JAPAN    = 4, // Japan
	SP_TERRA    = 5, // PH,VN (Terra) 
	SP_THAILAND = 6, // Thailand (Terra)
	SP_GLOBAL   = 7, // Global Service ( GSP ) 
	SP_MALAYSIA = 8, // MY,MYE 
	SP_GS		= 9, // Global Service ( 신규 ) 
};

enum EM_LOGIN_FB_SUB
{	
    EM_LOGIN_FB_SUB_OK             =  0, // 로그인 성공
	EM_LOGIN_FB_SUB_FAIL	       =  1, // 로그인 실패
	EM_LOGIN_FB_SUB_SYSTEM	       =  2, // 시스템에러로 로그인 실패
	EM_LOGIN_FB_SUB_USAGE	       =  3, // 허가된 사용시간 에러
	EM_LOGIN_FB_SUB_DUP		       =  4, // 중복된 접속
	EM_LOGIN_FB_SUB_INCORRECT      =  5, // ID/PWD 불일치 = (새로운 계정을 생성 또는 재입력요구),
	EM_LOGIN_FB_SUB_IP_BAN	       =  6, // ID/PWD 가 일치하지만 IP 가 사용불가능 상태= (접근 차단된 IP),	
	EM_LOGIN_FB_SUB_BLOCK          =  7, // ID 가 블록된 상태
	EM_LOGIN_FB_SUB_UNCON          =  8, // Daum : 비정상적 로그인데이타 (새롭게 웹에서 로그인후 게임실행)
	EM_LOGIN_FB_SUB_EXPIRED        =  9, // Daum : 인증시간이 초과된 사용자이기 때문에, 웹에 재접속을 하라는 메시지를 보낸다
	EM_LOGIN_FB_SUB_GID_ERR        = 10, // Daum : GID가 없음 웹에서 새롭게 로그인
	EM_LOGIN_FB_SUB_UID_ERR        = 11, // Daum : UID가 없음 웹에서 새롭게 로그인
	EM_LOGIN_FB_SUB_UNKNOWN        = 12, // Daum : 유저 삽입실패
	EM_LOGIN_FB_SUB_SSNHEAD        = 13, // Daum : 주민번호오류
	EM_LOGIN_FB_SUB_ADULT          = 14, // Daum : 성인인증실패
	EM_LOGIN_FB_CH_FULL            = 15, // 채널이 Full 인 상태 접속불가
	EM_LOGIN_FB_THAI_UNDER18_3HOUR = 16, // 18 세미만 3 시간 경과 
	EM_LOGIN_FB_THAI_UNDER18_TIME  = 17, // 18 세미만 접속불가능 시간
	EM_LOGIN_FB_THAI_OVER18_TIME   = 18, // 18 세이상 접속불가능 시간
	EM_LOGIN_FB_SUB_RANDOM_PASS    = 19, // 랜덤패스워드 에러
	EM_LOGIN_FB_SUB_PASS_OK		   = 20, // 초기 패스워드 업데이트 성공
	EM_LOGIN_FB_SUB_ALREADYOFFLINE = 21, // 로그인성공이지만 이미 오프라인일때
	EM_LOGIN_FB_SUB_SECID_ALREADY  = 22, // 아뒤가 중복됩니다.
};

// 친구정보 Friend
struct CHA_FRIEND
{
	int nChaS;
	int nChaFlag;
	TCHAR szCharName[CHR_ID_LENGTH+1];

	CHA_FRIEND() 
		: nChaS(0)
		, nChaFlag(0)
	{
		memset( szCharName, 0, sizeof(TCHAR) * (CHR_ID_LENGTH+1) );
	};
};
typedef CHA_FRIEND* LPCHA_FRIEND;

// 친구정보(SMS 포함)
struct CHA_FRIEND_SMS
{
	int nChaS;
	int nChaFlag;
	TCHAR szCharName[CHR_ID_LENGTH+1];
	TCHAR szPhoneNumber[SMS_RECEIVER];

	CHA_FRIEND_SMS() 
		: nChaS(0)
		, nChaFlag(0)
	{
		memset( szCharName, 0, sizeof(TCHAR) * (CHR_ID_LENGTH+1) );
		memset( szPhoneNumber, 0, sizeof(TCHAR) * (SMS_RECEIVER) );
	};
};
typedef CHA_FRIEND_SMS* LPCHA_FRIEND_SMS;

// BASIC User Information
// 28 Bytes
struct USER_INFO_BASIC
{
	INT		nUserNum;	
	USHORT	nKeyDirection;
	USHORT	nKey;
	TCHAR	szUserID[GSP_USERID];
	
	USER_INFO_BASIC()
	{
		nUserNum = 0;
		nKeyDirection = 0;
		nKey = 0;
		memset( szUserID, 0, sizeof(TCHAR) * (GSP_USERID) );
	};
};
typedef USER_INFO_BASIC* LPUSER_INFO_BASIC;

struct PER_IO_OPERATION_DATA
{
	OVERLAPPED	Overlapped;
	WSABUF		DataBuf;
	CHAR		Buffer[NET_DATA_BUFSIZE];
	BOOL		OperationType;
	DWORD		dwTotalBytes;
	DWORD		dwSndBytes;
	DWORD		dwRcvBytes;
	INT			nRefCount;

	PER_IO_OPERATION_DATA()
	{
		Overlapped.Internal		= 0;
		Overlapped.InternalHigh	= 0;
		Overlapped.Offset		= 0;
		Overlapped.OffsetHigh	= 0;
		Overlapped.hEvent		= 0;
		DataBuf.buf				= Buffer;
		DataBuf.len				= NET_DATA_BUFSIZE;	
		OperationType			= NET_SEND_POSTED;		
		dwTotalBytes			= 0;
		dwSndBytes				= 0;
		dwRcvBytes				= 0;
		nRefCount				= 0; // Set ref count to 0
	};

	void Reset(void)
	{		
		DataBuf.len				= NET_DATA_BUFSIZE;
		OperationType			= NET_SEND_POSTED;
		dwTotalBytes			= 0;
		dwSndBytes				= 0;
		dwRcvBytes				= 0;
		nRefCount				= 0; // Set ref count to 0
	};
};
typedef PER_IO_OPERATION_DATA* LPPER_IO_OPERATION_DATA;

struct G_SERVER_INFO
{
	int	nServerType;			  ///< 서버 유형
	int	nServerGroup;			  ///< 서버 그룹 번호
	int	nServerNumber;			  ///< 서버 번호
	int	nServerField;			  ///< 필드번호	
	int	nServerMaxClient;		  ///< Max clients
	int	nServerCurrentClient;	  ///< Current connected client number
	int nServerChannel;           ///< Channel number
	int nServerChannelNumber;     ///< Agent's channel number
	int nServerChannellMaxClient; ///< Channel max client
	int	nServicePort;
	int	nControlPort;
	unsigned long ulServerIP;
	bool bPk;                     ///< Channel PK information

	char	szServerName[SERVER_NAME_LENGTH+1];
	char	szServerIP[MAX_IP_LENGTH+1];
	char	szUserID[USR_ID_LENGTH+1];
	char	szUserPass[USR_PASS_LENGTH+1];	

	G_SERVER_INFO()
	{
		memset( szServerName, 0, sizeof(char) * (SERVER_NAME_LENGTH+1) );
		memset( szServerIP,   0, sizeof(char) * (MAX_IP_LENGTH+1) );
		memset( szUserID,     0, sizeof(char) * (USR_ID_LENGTH+1) );
		memset( szUserPass,   0, sizeof(char) * (USR_PASS_LENGTH+1) );

		ulServerIP               = INADDR_NONE;
        nServicePort             = 0;
	    nControlPort             = 0;
	    nServerType              = 0;
	    nServerGroup             = 0;
	    nServerNumber            = 0;
	    nServerField             = 0;
	    nServerMaxClient         = 0;
	    nServerCurrentClient     = 0;
	    nServerChannel           = 0;
	    nServerChannelNumber     = 0;
	    nServerChannellMaxClient = 0;
	    bPk                      = true; 
	};

	friend bool operator==( const G_SERVER_INFO& lhs, const G_SERVER_INFO& rhs )
	{
		if ( (::strcmp(lhs.szServerIP, rhs.szServerIP) == 0) &&
			 (lhs.nServicePort == rhs.nServicePort) )
		{
			return true;
		}
		else 
		{
			return false;
		}
	};
};
typedef G_SERVER_INFO* LPG_SERVER_INFO;

struct G_SERVER_CUR_INFO
{
	int	nServerCurrentClient;	// Corrent connected client number
	int	nServerMaxClient;		// Max clients

	G_SERVER_CUR_INFO()
		: nServerCurrentClient(0)
		, nServerMaxClient(0)
	{
	};
};
typedef G_SERVER_CUR_INFO* LPG_SERVER_CUR_INFO;

struct G_SERVER_CUR_INFO_LOGIN
{
	char szServerIP[MAX_IP_LENGTH+1]; ///< Server IP Address
	int nServicePort;            ///< Server Port
	int nServerGroup;                ///< Server Group Number
	int nServerNumber;               ///< Channel Number or Server Number
	int nServerCurrentClient;	     ///< Channel Current Client
	int nServerMaxClient;		     ///< Channel Max Client
    bool bPK;                         ///< Channel PK information

	G_SERVER_CUR_INFO_LOGIN()
		: nServicePort(0)
		, nServerGroup(0)
		, nServerNumber(0)
		, nServerCurrentClient(0)
		, nServerMaxClient(0)
		, bPK(true)
	{
		memset( szServerIP, 0, sizeof(char) * (MAX_IP_LENGTH+1) );		
	};
};
typedef G_SERVER_CUR_INFO_LOGIN* LPG_SERVER_CUR_INFO_LOGIN;
typedef G_SERVER_CUR_INFO_LOGIN** LPPG_SERVER_CUR_INFO_LOGIN;

// field_server [map id] [map name] [server_name] [ip] [port] 
struct F_SERVER_INFO // Field Server Information
{			
	char szServerName[SERVER_NAME_LENGTH+1];	
	char szServerIP[MAX_IP_LENGTH+1];
	unsigned long ulServerIP;
	int	nServicePort;

	F_SERVER_INFO()
	{
		::memset( this, 0, sizeof(F_SERVER_INFO) );
		ulServerIP = INADDR_NONE;
	};

	friend bool operator== ( const F_SERVER_INFO& lhs, const F_SERVER_INFO& rhs )
	{
		if ( (::strcmp(lhs.szServerIP, rhs.szServerIP) == 0) &&
			 (lhs.nServicePort == rhs.nServicePort) )
		{
			return true;
		}
		else
		{
			return false;
		}
	};

	bool IsValid ()
	{
		return ( szServerName[0] && szServerIP[0] && nServicePort && (ulServerIP!=INADDR_NONE) );
	}
};
typedef F_SERVER_INFO* LPF_SERVER_INFO;

struct CHANNEL_INFO
{
	bool bPK;

	CHANNEL_INFO()
	{
		bPK = true;
	};
};
typedef CHANNEL_INFO* LPCHANNEL_INFO;

///////////////////////////////////////////////////////////////////////////////
// Base Msg
// If you change base message, all message number will be change
// If user find packet order, change NET_MSG_BASE number
// All Message Number must less than 2 byte(65535)

// Memo : 아래 기본 메세지는 중복이 되지 않도록 정의한다.
//
#if defined(CH_PARAM)
	#define NET_MSG_BASE				1019				  // 기본 메시지
#elif defined(HK_PARAM)
	#define NET_MSG_BASE				1007				  // 기본 메시지
#elif defined(ID_PARAM)
	#define NET_MSG_BASE				977					  // 기본 메시지
#elif defined(JP_PARAM)
	#define NET_MSG_BASE				1031				  // 기본 메시지
#elif defined(KR_PARAM)
	#define NET_MSG_BASE				987					  // 기본 메시지
#elif defined(KRT_PARAM)
	#define NET_MSG_BASE				971					  // 기본 메시지
#elif defined(MY_PARAM)
	#define NET_MSG_BASE				969					  // 기본 메시지
#elif defined(MYE_PARAM)
	#define NET_MSG_BASE				1005				  // 기본 메시지
#elif defined(PH_PARAM)
	#define NET_MSG_BASE				1017				  // 기본 메시지
#elif defined(VN_PARAM)
	#define NET_MSG_BASE				1021				  // 기본 메시지
#elif defined(TW_PARAM)
	#define NET_MSG_BASE				997					  // 기본 메시지
#elif defined(TH_PARAM)
	#define NET_MSG_BASE				989					  // 기본 메시지
#elif defined(GS_PARAM)
	#define NET_MSG_BASE				1004				  // 기본 메시지
#else
	#define NET_MSG_BASE				995					  // 기본 메시지
#endif

/*
#define NET_MSG_LGIN				(NET_MSG_BASE +  507) // 로그인서버 메시지
#define NET_MSG_LOBBY				(NET_MSG_BASE + 1013) // 게임서버 로비
#define NET_MSG_LOBBY_MAX			(NET_MSG_BASE + 1509) // 게임서버 로비 최대값

#define NET_MSG_GCTRL				(NET_MSG_BASE + 2011) // 게임서버 게임플레이
#define NET_MSG_GCTRL_MAX			(NET_MSG_BASE + 4010) // 게임서버 게임플레이 최대값
*/

// 2005-11-04
//#define NET_MSG_LGIN				(NET_MSG_BASE +  413) // 로그인서버 메시지
//#define NET_MSG_LOBBY				(NET_MSG_BASE +  913) // 게임서버 로비
//#define NET_MSG_LOBBY_MAX			(NET_MSG_BASE + 1413) // 게임서버 로비 최대값
//
//#define NET_MSG_GCTRL				(NET_MSG_BASE + 1823) // 게임서버 게임플레이
//#define NET_MSG_GCTRL_MAX			(NET_MSG_BASE + 3823) // 게임서버 게임플레이 최대값

// 2007-04-11
#define NET_MSG_LGIN				(NET_MSG_BASE +  450) // 로그인서버 메시지
#define NET_MSG_LOBBY				(NET_MSG_BASE +  950) // 게임서버 로비
#define NET_MSG_LOBBY_MAX			(NET_MSG_BASE + 1450) // 게임서버 로비 최대값

#define NET_MSG_GCTRL				(NET_MSG_BASE + 1900) // 게임서버 게임플레이
#define NET_MSG_GCTRL_MAX			(NET_MSG_BASE + 3900) // 게임서버 게임플레이 최대값

// Login information
enum EMNET_MSG
{
	NET_MSG_BASE_ZERO           = 0,

	///////////////////////////////////////////////////////////////////////////
	// Version check and downloading new version
	// Do not change version and Crypt key message
	NET_MSG_VERSION_OK			= 100, //< 버전이 일치함
	NET_MSG_VERSION_INFO		= 110, //< 버전 정보를 보냄
	NET_MSG_VERSION_REQ			= 120, //< 버전 정보를 요청함
	NET_MSG_REQ_CRYT_KEY		= 130, //< 클라이언트->게임서버 : 암호키 요청
	NET_MSG_SND_CRYT_KEY		= 140, //< 게임서버->클라이언트 : 암호키 전송	
	NET_MSG_RANDOM_NUM          = 141, //< Agent->Client : 랜덤넘버
//	NET_MSG_PING_REQUEST		= 150, //< 서버로의 ping 을 요청한다.
//	NET_MSG_PING_ANSWER		    = 151, //< 서버에서 ping 을 응답한다.	
    NET_MSG_HEARTBEAT_SERVER_REQ= 153, //< SERVER->SERVER Server HeartBeat Request
    NET_MSG_HEARTBEAT_SERVER_ANS= 154, //< SERVER->SERVER Server HeartBeat Answer
    NET_MSG_SVR_DOWN            = 155, //< Server System Down!
	NET_MSG_SVR_CLOSECLIENT     = 156, //< Server Close Client
	NET_MSG_HEARTBEAT_CLIENT_REQ= 160, //< SERVER->CLIENT Client HeartBeat Request
	NET_MSG_HEARTBEAT_CLIENT_ANS= 161, //< CLIENT->SERVER Client HeartBeat Answer
	NET_MSG_COMPRESS            = 170, //< Compressed message	

	///////////////////////////////////////////////////////////////////////////	
	// Server info message
	NET_MSG_REQ_GAME_SVR		= (NET_MSG_LGIN + 100), //< 클라이언트가 -> 로그인서버 : 게임 서버의 정보를 요청
	NET_MSG_SND_GAME_SVR	    = (NET_MSG_LGIN + 110), //< 로그인서버   -> 클라이언트 : 게임 서버의 정보를 전송
	NET_MSG_SND_GAME_SVR_END	= (NET_MSG_LGIN + 120), //< 로그인서버   -> 클라이언트 : 게임 서버 정보 전달 끝
	NET_MSG_REQ_FULL_SVR_INFO	= (NET_MSG_LGIN + 130), //< 세션서버 -> 로그인서버, 게임서버 : 서버 전체 정보를 요청한다.
	NET_MSG_SND_FULL_SVR_INFO	= (NET_MSG_LGIN + 140), //< 로그인서버, 게임서버 -> 세션서버 : 서버 전체 정보를 Session 서버에 전송한다.
	NET_MSG_REQ_CUR_STATE		= (NET_MSG_LGIN + 150), //< 세션서버 -> 로그인서버, 게임서버 : 현재 서버상태= (간략한),를 요청한다.
	NET_MSG_SND_CUR_STATE		= (NET_MSG_LGIN + 160), //< 로그인서버, 게임서버 -> 세션서버 : 현재 서버상태= (간략한),를 전송한다.
	NET_MSG_SND_CHANNEL_STATE   = (NET_MSG_LGIN + 161), //< Agent->Session : 현재 채널상태를 전송한다.
	NET_MSG_REQ_SVR_INFO		= (NET_MSG_LGIN + 170), //< 로그인서버 -> 세션서버 : 게임서버의 상태정보를 요청한다.
	NET_MSG_SND_SVR_INFO		= (NET_MSG_LGIN + 180), //< 세션서버 -> 로그인서버 : 게임서버의 상태정보를 전송한다.
	NET_MSG_SND_SVR_INFO_RESET	= (NET_MSG_LGIN + 190), //< 세션서버 -> 로그인서버 : 게임서버의 상태정보를 리셋하라고 명령.
	NET_MSG_REQ_ALL_SVR_INFO    = (NET_MSG_LGIN + 191), //< 관리프로그램->세션서버 : 모든 서버의 상태정보를 요청한다.
	NET_MSG_SND_ALL_SVR_INFO    = (NET_MSG_LGIN + 192), //< 세션서버->관리프로그램 : 모든 서버의 상태정보를 전송한다.
	NET_MSG_SND_ALL_SVR_INFO_S  = (NET_MSG_LGIN + 193), //< 세션서버->관리프로그램 : 상태정보전송 시작
	NET_MSG_SND_ALL_SVR_INFO_E	= (NET_MSG_LGIN + 194), //< 세션서버->관리프로그램 : 상태정보를 모두 보냈음을 알린다.
	NET_MSG_RECONNECT_FIELD     = (NET_MSG_LGIN + 195), //< 세션서버->에이전트 : 필드로 다시 접속하라.
	NET_MSG_I_AM_AGENT          = (NET_MSG_LGIN + 196), //< Agent->Field : 자신이 서버통신용 슬롯임을 알림

	NET_MSG_LOGOUT_G_S			= (NET_MSG_LGIN + 200), //< Agent->Session : 로그아웃 정보	
	NET_MSG_LOGOUT_G_S_S		= (NET_MSG_LGIN + 201), //< Agent->Session : 로그아웃 정보. 사용자 ID 만 전송
	DAUM_NET_MSG_LOGOUT_G_S     = (NET_MSG_LGIN + 202), //< Agent->Session : Daum 로그아웃 정보

	//< User Information message
	NET_MSG_REQ_USER_INFO		= (NET_MSG_LGIN + 210), //< 사용자 정보를 요청한다.
	NET_MSG_USER_INFO			= (NET_MSG_LGIN + 220), //< 사용자 정보를 전송한다.

	//< Server control message	
	NET_MSG_SVR_CMD				= (NET_MSG_LGIN + 300),
	NET_MSG_SVR_PAUSE			= (NET_MSG_LGIN + 310), //< 서버를 잠시 멈춘다.
	NET_MSG_SVR_RESUME			= (NET_MSG_LGIN + 311), //< 멈추어진 서버를 다시 가동시킨다.	
	NET_MSG_SVR_RESTART			= (NET_MSG_LGIN + 312), //< 서버를 완전히 멈추고 새롭게 가동시킨다.
	NET_MSG_SVR_STOP			= (NET_MSG_LGIN + 313), //< 서버를 완전히 멈춘다.
	NET_MSG_SND_CHANNEL_FULL_STATE = (NET_MSG_LGIN + 314), //< 서버의 채널 FULL 상태를 LoginServer에 보낸다.

	//<-----------------------------------------------------------------------------------------------------[LOBBY]	
	// NET_MSG_LOGIN				= (NET_MSG_LOBBY + 100), //< 로그인 
	NET_MSG_LOGIN_2 			= (NET_MSG_LOBBY + 107), //< 로그인
	NET_MSG_LOGIN_FB			= (NET_MSG_LOBBY + 108), //< 로그인 결과	
	NET_MSG_LOGIN_FB_SUB_OK_FIELD=(NET_MSG_LOBBY + 109), //< 로그인성공결과를 필드서버로 전송해서 세팅하게 한다.		
	
	DAUM_NET_MSG_LOGIN			= (NET_MSG_LOBBY + 110), //< Daum 로그인
	DAUM_NET_MSG_LOGIN_FB		= (NET_MSG_LOBBY + 111), //< Daum 로그인 결과 Agent->Client	

	MET_MSG_SVR_FULL            = (NET_MSG_LOBBY + 112), //< 서버 Full

	CHINA_NET_MSG_LOGIN         = (NET_MSG_LOBBY + 113), //< China 로그인
	CHINA_NET_MSG_LOGIN_FB      = (NET_MSG_LOBBY + 114), //< China 로그인 결과 Agent->Client

	GSP_NET_MSG_LOGIN           = (NET_MSG_LOBBY + 115), //< GSP 로그인
	GSP_NET_MSG_LOGIN_FB        = (NET_MSG_LOBBY + 116), //< GSP 로그인 결과 AGent->Client

	TERRA_NET_MSG_LOGIN         = (NET_MSG_LOBBY + 120), //< Terra 로그인
	TERRA_NET_MSG_LOGIN_FB		= (NET_MSG_LOBBY + 121), //< Terra 로그인 결과 Agent->Client

	TERRA_NET_MSG_PASSCHECK     = (NET_MSG_LOBBY + 122), //< Terra Password Check
	DAUM_NET_MSG_PASSCHECK      = (NET_MSG_LOBBY + 123), //< Daum Password Check
	NET_MSG_PASSCHECK_FB		= (NET_MSG_LOBBY + 124), //< Password Check 결과 Agent->Client
	EXCITE_NET_MSG_PASSCHECK    = (NET_MSG_LOBBY + 125), //< Excite Password Check

	EXCITE_NET_MSG_LOGIN        = (NET_MSG_LOBBY + 130), //< Excite 로그인
	EXCITE_NET_MSG_LOGIN_FB		= (NET_MSG_LOBBY + 131), //< Excite 로그인 결과 Agent->Client

	JAPAN_NET_MSG_LOGIN         = (NET_MSG_LOBBY + 132), //< 일본(Gonzo) 로그인
	JAPAN_NET_MSG_LOGIN_FB		= (NET_MSG_LOBBY + 133), //< 일본(Gonzo) 로그인 결과 Agent->Client
	JAPAN_NET_MSG_UUID			= (NET_MSG_LOBBY + 134), //< 일본(Gonzo) 로그인 UUID

	GS_NET_MSG_LOGIN			= (NET_MSG_LOBBY + 135), //< GS 로그인
	GS_NET_MSG_LOGIN_FB			= (NET_MSG_LOBBY + 136), //< GS 로그인 결과 Agent->Client

	THAI_NET_MSG_LOGIN          = (NET_MSG_LOBBY + 140), //< Thailand 로그인
	THAI_NET_MSG_LOGIN_FB		= (NET_MSG_LOBBY + 141), //< Thailand 로그인 결과 Agent->Client

	NET_MSG_REQ_RAND_KEY        = (NET_MSG_LOBBY + 150), //< 랜덤키 요청 Client->Agent
	
	NET_MSG_SND_ENCRYPT_KEY		= (NET_MSG_LOBBY + 160), //< 로그인 랜덤키 전송 Login->Session

	//< Character create message
	NET_MSG_CHA_INFO			= (NET_MSG_LOBBY + 200), //< 캐릭터 정보
	NET_MSG_CHA_NEW				= (NET_MSG_LOBBY + 201), //< 새로운 캐릭터 생성
	NET_MSG_CHA_NEW_FB			= (NET_MSG_LOBBY + 202), //< 캐릭터 생성 성공/실패 메시지
	
	NET_MSG_CHA_DEL				= (NET_MSG_LOBBY + 210), //< Client->Agent : 케릭터 삭제
	NET_MSG_CHA_DEL_FB			= (NET_MSG_LOBBY + 211), //< Agent->Client : 케릭터 삭제 결과
	NET_MSG_CHA_DEL_FB_OK		= (NET_MSG_LOBBY + 212), //< Agent->Client : 케릭터 삭제 성공
	NET_MSG_CHA_DEL_FB_ERROR	= (NET_MSG_LOBBY + 213), //< Agent->Client : 케릭터 삭제 실패
	DAUM_NET_MSG_CHA_DEL		= (NET_MSG_LOBBY + 214), //< Client->Agent : 케릭터 삭제
    NET_MSG_CHA_DEL_FB_CLUB	    = (NET_MSG_LOBBY + 215), //< Agent->Client : 케릭터 삭제 실패(클럽마스터이기 때문에 삭제가 되지 않는다)
	TERRA_NET_MSG_CHA_DEL		= (NET_MSG_LOBBY + 216), //< Client->Agent : 케릭터 삭제
	EXCITE_NET_MSG_CHA_DEL		= (NET_MSG_LOBBY + 217), //< Client->Agent : 케릭터 삭제
	GSP_NET_MSG_CHA_DEL		    = (NET_MSG_LOBBY + 218), //< Client->Agent : 케릭터 삭제
	JAPAN_NET_MSG_CHA_DEL		= (NET_MSG_LOBBY + 219), //< Client->Agent : 케릭터 삭제
	GS_NET_MSG_CHA_DEL			= (NET_MSG_LOBBY + 220), //< Client->Agent : 케릭터 삭제

	NET_MSG_CHA_DECREASE        = (NET_MSG_LOBBY + 240), //< Agent->Session : 캐릭터 생성, 갯수 감소
	NET_MSG_TEST_CHA_DECREASE   = (NET_MSG_LOBBY + 241), //< Agent->Session : 캐릭터 생성, 갯수 감소
	DAUM_NET_MSG_CHA_DECREASE   = (NET_MSG_LOBBY + 242), //< DAUM : Agent->Session : 캐릭터 생성, 갯수 감소
	DAUM_NET_MSG_TEST_CHA_DECREASE=(NET_MSG_LOBBY+ 243), //< DAUM : Agent->Session : 캐릭터 생성, 갯수 감소

	NET_MSG_CHA_INCREASE        = (NET_MSG_LOBBY + 244), //< Agent->Session : 캐릭터 삭제, 갯수 증가
	NET_MSG_TEST_CHA_INCREASE   = (NET_MSG_LOBBY + 245), //< Agent->Session : 캐릭터 삭제, 갯수 증가
	DAUM_NET_MSG_CHA_INCREASE   = (NET_MSG_LOBBY + 246), //< DAUM : Agent->Session : 캐릭터 삭제, 갯수 증가
	DAUM_NET_MSG_TEST_CHA_INCREASE= (NET_MSG_LOBBY+247),//< DAUM : Agent->Session : 캐릭터 삭제, 갯수 증가

	NET_MSG_REQ_CHA_ALL_INFO	= (NET_MSG_LOBBY + 300), //< 모든 캐릭터의 정보를 요청한다.
	NET_MSG_REQ_CHA_INFO		= (NET_MSG_LOBBY + 301), //< 특정 캐릭터의 정보를 요청한다.
	NET_MSG_REQ_CHA_BINFO		= (NET_MSG_LOBBY + 302), //< 특정 캐릭터의 기본= (캐릭터선택화면),정보 요청한다.
	NET_MSG_REQ_CHA_FEEDBACK	= (NET_MSG_LOBBY + 303), //< 캐릭터 정보 피드백데이터
	NET_MSG_CHA_INFO_LOGIN		= (NET_MSG_LOBBY + 304), //< 로그인서버에서 클라이언트에게 넘겨주는 캐릭터 정보
	NET_MSG_REQ_CHA_BAINFO		= (NET_MSG_LOBBY + 305), //< 클라이언트->게임서버 : 캐릭터 기초정보 요청
	NET_MSG_CHA_BAINFO			= (NET_MSG_LOBBY + 306), //< 게임서버->클라이언트 : 캐릭터 기초정보	
	
	NET_MSG_LOBBY_CHAR_SEL		= (NET_MSG_LOBBY + 390), //< 필드서버->클라이언트 : 캐릭터 선택용 정보.
	NET_MSG_LOBBY_CHAR_JOIN		= (NET_MSG_LOBBY + 391), //< 필드서버->클라이언트 : 선택된 캐릭터 정보를 보냄.
	NET_MSG_LOBBY_CHAR_ITEM		= (NET_MSG_LOBBY + 392), //< 필드서버->클라이언트 : 선택된 캐릭터 인벤토리 아이템을 보냄.
	NET_MSG_LOBBY_CHAR_JOIN_FB	= (NET_MSG_LOBBY + 393), //< 필드서버->클라이언트 : 선택된 캐릭터의 접속참가 실패 메시지.
	NET_MSG_LOBBY_CHAR_SKILL	= (NET_MSG_LOBBY + 394), //<	필드서버->클라이언트 : 선택된 캐릭터가 배운 스킬 정보 전송.
	NET_MSG_LOBBY_QUEST_END		= (NET_MSG_LOBBY + 395), //<	퀘스트 완료 정보.
	NET_MSG_LOBBY_QUEST_PROG	= (NET_MSG_LOBBY + 396), //<	퀘스트 진행 정보.
	NET_MSG_LOBBY_CHARGE_ITEM	= (NET_MSG_LOBBY + 397), //<	구입 아이템 전송.
	NET_MSG_LOBBY_CHAR_PUTON	= (NET_MSG_LOBBY + 398), //< 필드서버->클라이언트 : 선택된 캐릭터 착용 아이템을 보냄.

	NET_MSG_LOBBY_CLUB_INFO		= (NET_MSG_LOBBY + 399), //<	클럽 정보.
	NET_MSG_LOBBY_CLUB_MEMBER	= (NET_MSG_LOBBY + 400), //<	클럽 멤버.
	NET_MSG_LOBBY_CLUB_ALLIANCE	= (NET_MSG_LOBBY + 401), //<	클럽 동맹.
	NET_MSG_LOBBY_CLUB_BATTLE	= (NET_MSG_LOBBY + 402), //<	클럽 배틀.

	NET_MSG_CHARPOS_FROMDB2AGT	= (NET_MSG_LOBBY + 403), //<	캐릭터 위치 DB 저장 결과
	
	NET_MSG_LOBBY_ITEM_COOLTIME = (NET_MSG_LOBBY + 404), //<	아이템 쿨타임
	
	NET_MSG_LOBBY_GAME_JOIN			= (NET_MSG_LOBBY + 411), //< 클라이언트  ->이이전트서버 : 로비에서 선택한 캐릭터로 게임접속	
	NET_MSG_LOBBY_GAME_COMPLETE		= (NET_MSG_LOBBY + 412), //< 클라이언트  ->클라이언트   : 게임에 들어갈 정보를 모두 받아서 게임에 들어감.
	NET_MSG_GAME_JOIN_OK			= (NET_MSG_LOBBY + 413), //< 필드서버    ->세션서버     : 해당 캐릭터가 조인성공
	MET_MSG_GAME_JOIN_FIELDSVR		= (NET_MSG_LOBBY + 414), //< 에이전트서버->필드서버     : 해당캐릭터가 게임에 조인요청
	MET_MSG_GAME_JOIN_FIELDSVR_FB	= (NET_MSG_LOBBY + 415), //< 필드서버->에이전트서버:선택된 캐릭터로 조인함

	NET_MSG_CONNECT_CLIENT_FIELD	= (NET_MSG_LOBBY + 416), //<	에이전트서버->클라이언트   : 해당필드서버로 접속 명령
	NET_MSG_JOIN_FIELD_IDENTITY		= (NET_MSG_LOBBY + 417), //<	클라이언트  ->필드서버     : 새로운 Field 서버에 접속후 자신의 인식자 전송.

	NET_MSG_LOBBY_REQ_GAME_JOIN		= (NET_MSG_LOBBY + 421), //< 클라이언트  ->클라이언트   : 인터페이스로 선택된 케릭으로 접속을 명령하기 위해서.
	
	NET_MSG_AGENT_REQ_JOIN			= (NET_MSG_LOBBY + 422), //< 에이젼트DB  ->에이젼트   : db에서 읽은 char로 접속시도.
	NET_MSG_FIELD_REQ_JOIN			= (NET_MSG_LOBBY + 423), //< 필드DB		->필드서버   : db에서 읽은 char로 접속시도.

	NET_MSG_EVENT_LOTTERY           = (NET_MSG_LOBBY + 430), //< 클라이언트  ->에이전트   : 복권입력
	NET_MSG_EVENT_LOTTERY_FB        = (NET_MSG_LOBBY + 431), //< 에이전트    ->클라이언트 : 복권결과

	NET_MSG_LOBBY_CHINA_ERROR		= (NET_MSG_LOBBY + 432), //< 중국 로그인시 플레이 누적시간이 5시간이 넘고 오프라인 누적시간이 5시간이 되지 않았을 경우 에러 메시지

	NET_MSG_LOBBY_CHAR_VIETNAM_ITEM = (NET_MSG_LOBBY + 433), //< 베트남 인벤토리 아이템 전송 메시지

	NET_MSG_LOBBY_CHAR_PUTON_EX		= (NET_MSG_LOBBY + 434), //< 필드서버->클라이언트 : 선택된 캐릭터 착용 아이템을 보냄. ( 한개씩 보냄 )

	//<-----------------------------------------------------------------------------------------------------[GAME CONTRL]
	
	NET_MSG_COMBINE					= (NET_MSG_GCTRL +  10), //< 병합 메시지. ( 필드 서버 -> 클라이언트 )

	///////////////////////////////////////////////////////////////////////////
	// nProtect
	NET_MSG_GAMEGUARD_AUTH          = (NET_MSG_GCTRL +  20), //< nProtect GameGuard Auth 요청 메시지
	NET_MSG_GAMEGUARD_ANSWER        = (NET_MSG_GCTRL +  21), //< nProtect GameGuard Auth Answer 메시지
	NET_MSG_GAMEGUARD_AUTH_1        = (NET_MSG_GCTRL +  22), //< nProtect GameGuard Auth 요청 메시지
	NET_MSG_GAMEGUARD_ANSWER_1      = (NET_MSG_GCTRL +  23), //< nProtect GameGuard Auth Answer 메시지
	NET_MSG_GAMEGUARD_AUTH_2        = (NET_MSG_GCTRL +  24), //< nProtect GameGuard Auth 요청 메시지
	NET_MSG_GAMEGUARD_ANSWER_2      = (NET_MSG_GCTRL +  25), //< nProtect GameGuard Auth Answer 메시지

	// Apex 적용
	NET_MSG_APEX_DATA				= (NET_MSG_GCTRL +	30), //< Apex Data 메세지
	NET_MSG_APEX_ANSWER				= (NET_MSG_GCTRL +	31), //< Apex Answer 메세지
//	NET_MSG_APEX_RSAKEY				= (NET_MSG_GCTRL +	32), //< Apex RSA Key 메세지
	// Apex 적용( 홍콩 )
	NET_MSG_APEX_RETURN 			= (NET_MSG_GCTRL +	33), //< Apex Client Return 

	NET_MSG_REQ_FRIENDLIST			= (NET_MSG_GCTRL +  50), //< 친구 목록 전송 요청.
	NET_MSG_REQ_FRIENDADD			= (NET_MSG_GCTRL +  51), //< 친구 추가 요청.
	NET_MSG_REQ_FRIENDADD_LURE		= (NET_MSG_GCTRL +  52), //< 친구 추가 문의.
	NET_MSG_REQ_FRIENDADD_ANS		= (NET_MSG_GCTRL +  53), //< 친구 추가 수락 여부 회신.
	NET_MSG_REQ_FRIENDADD_FB		= (NET_MSG_GCTRL +  54), //< 친구 추가 요청 결과.
	NET_MSG_REQ_FRIENDDEL			= (NET_MSG_GCTRL +  55), //< 친구 삭제 요청.
	NET_MSG_REQ_FRIENDDEL_FB		= (NET_MSG_GCTRL +  56), //< 친구 삭제 결과.

	NET_MSG_FRIENDINFO				= (NET_MSG_GCTRL +  57), //< 친구 정보.
	NET_MSG_FRIENDSTATE				= (NET_MSG_GCTRL +  58), //< 친구 상태.

	NET_MSG_REQ_FRIENDBLOCK			= (NET_MSG_GCTRL +  59), //< 귓속말 블럭 요청.
	NET_MSG_REQ_FRIENDBLOCK_FB		= (NET_MSG_GCTRL +  60), //< 귓속말 블럭 요청 결과.

	NET_MSG_CHAT					= (NET_MSG_GCTRL + 100), //< 채팅메시지 정의 ( 클라이언트->게임서버 )
	NET_MSG_CHAT_FB					= (NET_MSG_GCTRL + 101), //< 채팅메시지 정의 ( 게임서버->클라이언트 )
	NET_MSG_CHAT_PRIVATE_FAIL		= (NET_MSG_GCTRL + 102), //< 채팅, 귓속말 보내기에 실패하였을 경우. ( 게임서버->클라이언트 )	

	NET_MSG_CHAT_LOUDSPEAKER		= (NET_MSG_GCTRL + 103), //< 확성기 메시시.
	NET_MSG_CHAT_LOUDSPEAKER_AGT	= (NET_MSG_GCTRL + 104), //< 확성기 메시지 agt 에 보냄.
	NET_MSG_CHAT_LOUDSPEAKER_FB		= (NET_MSG_GCTRL + 105), //< 확성기 메시지 처리 FB.

	NET_MSG_CHAT_BLOCK				= (NET_MSG_GCTRL + 106), //< 채팅 차단 상태일 경우. ( 게임서버 -> 클라이언트 )

	NET_MSG_FIELDSVR_CHARCHK		= (NET_MSG_GCTRL + 110), //< 필드서버에 케릭터 진입전에 잔존 CHAR 제거 위한 메시지 전송.
	NET_MSG_FIELDSVR_CHARCHK_FB		= (NET_MSG_GCTRL + 111), //< 필드서버에 케릭터 진입전에 잔존 CHAR 제거 위한 메시지 전송.

	NET_MSG_GCTRL_FIELDSVR_OUT		= (NET_MSG_GCTRL + 115), //< 필드서버에서 나오기 위한 예비 처리를 부탁. ( 필드서버 이동시. )
	NET_MSG_GCTRL_FIELDSVR_OUT_FB	= (NET_MSG_GCTRL + 116), //< 필드서버에서 나오기 위한 예비 처리 결과. ( 필드서버 이동시. )

	NET_MSG_GCTRL_REBIRTH_OUT		= (NET_MSG_GCTRL + 117), //< 다른 필드서버에서 부활하기 위한 예비 처리 부탁.
	NET_MSG_GCTRL_REBIRTH_OUT_FB	= (NET_MSG_GCTRL + 118), //< 다른 필드서버에서 부활하기 위한 예비 처리 결과.

	NET_MSG_GCTRL_DROP_ITEM			= (NET_MSG_GCTRL + 120),
	NET_MSG_GCTRL_DROP_MONEY		= (NET_MSG_GCTRL + 121),

	NET_MSG_GCTRL_DROP_PC			= (NET_MSG_GCTRL + 123),
	NET_MSG_GCTRL_DROP_CROW			= (NET_MSG_GCTRL + 124),
	NET_MSG_GCTRL_DROP_MATERIAL		= (NET_MSG_GCTRL + 125),

	NET_MSG_GCTRL_DROP_OUT			= (NET_MSG_GCTRL + 127),
	NET_MSG_GCTRL_DROP_OUT_FORCED	= (NET_MSG_GCTRL + 128),

	NET_MSG_GCTRL_REQ_GATEOUT_REQ	= (NET_MSG_GCTRL + 130),
	NET_MSG_GCTRL_REQ_GATEOUT		= (NET_MSG_GCTRL + 131),
	NET_MSG_GCTRL_REQ_GATEOUT_FB	= (NET_MSG_GCTRL + 132),
	NET_MSG_GCTRL_REQ_LANDIN		= (NET_MSG_GCTRL + 133),

	NET_MSG_GCTRL_REQ_READY			= (NET_MSG_GCTRL + 134),

	NET_MSG_GCTRL_LAND_INFO			= (NET_MSG_GCTRL + 135),
	NET_MSG_GCTRL_SERVER_INFO		= (NET_MSG_GCTRL + 136),
	NET_MSG_GCTRL_SCHOOLFREEPK_FLD	= (NET_MSG_GCTRL + 137),

	NET_MSG_GCTRL_ACTSTATE			= (NET_MSG_GCTRL + 139),
	NET_MSG_GCTRL_MOVESTATE			= (NET_MSG_GCTRL + 140),
	NET_MSG_GCTRL_MOVESTATE_BRD		= (NET_MSG_GCTRL + 141),

	NET_MSG_GCTRL_GOTO				= (NET_MSG_GCTRL + 142),
	NET_MSG_GCTRL_GOTO_BRD			= (NET_MSG_GCTRL + 143),

	NET_MSG_GCTRL_ATTACK			= (NET_MSG_GCTRL + 144),
	NET_MSG_GCTRL_ATTACK_BRD		= (NET_MSG_GCTRL + 145),
	NET_MSG_GCTRL_ATTACK_CANCEL		= (NET_MSG_GCTRL + 146),
	NET_MSG_GCTRL_ATTACK_CANCEL_BRD	= (NET_MSG_GCTRL + 147),
	NET_MSG_GCTRL_ATTACK_AVOID		= (NET_MSG_GCTRL + 149),
	NET_MSG_GCTRL_ATTACK_AVOID_BRD	= (NET_MSG_GCTRL + 150),
	NET_MSG_GCTRL_ATTACK_DAMAGE		= (NET_MSG_GCTRL + 151),
	NET_MSG_GCTRL_ATTACK_DAMAGE_BRD	= (NET_MSG_GCTRL + 152),

	NET_MSG_GCTRL_ACTION_BRD		= (NET_MSG_GCTRL + 153),	//<	행동 변화.
	NET_MSG_GCTRL_UPDATE_STATE		= (NET_MSG_GCTRL + 154),	//<	상태 변화 업대이트.
	NET_MSG_GCTRL_UPDATE_EXP		= (NET_MSG_GCTRL + 155),	//<	경험치 변화시 발생.
	NET_MSG_GCTRL_UPDATE_MONEY		= (NET_MSG_GCTRL + 156),	//<	경험치 변화시 발생.

	NET_MSG_GCTRL_UPDATE_SP			= (NET_MSG_GCTRL + 157),	//<	SP 변화시 발생.
	NET_MSG_GCTRL_UPDATE_LP			= (NET_MSG_GCTRL + 158),	//<	Living Point 변화시 발생.
	NET_MSG_GCTRL_UPDATE_SKP		= (NET_MSG_GCTRL + 160),	//<	스킬 포인트 변화시 발생.

	NET_MSG_GCTRL_UPDATE_STATE_BRD	= (NET_MSG_GCTRL + 161),	//<	상태 변화 업대이트.
	NET_MSG_GCTRL_UPDATE_PASSIVE_BRD= (NET_MSG_GCTRL + 162),	//<	passive skill 상태 변화 업대이트.

	NET_MSG_GCTRL_POSITIONCHK_BRD	= (NET_MSG_GCTRL + 163),	//<	서버에서의 위치를 클라이언트에서 체크하기 위해서.

	NET_MSG_GCTRL_UPDATE_BRIGHT		= (NET_MSG_GCTRL + 164),	//<	bright(속성)치 업대이트.
	NET_MSG_GCTRL_UPDATE_BRIGHT_BRD	= (NET_MSG_GCTRL + 165),	//<	bright(속성)치 업대이트.
	NET_MSG_GCTRL_UPDATE_STATS		= (NET_MSG_GCTRL + 166),	//<	stats 업대이트.

	NET_MSG_GCTRL_UPDATE_FLAGS		= (NET_MSG_GCTRL + 167),	//<	flags 업대이트.

	NET_MSG_GCTRL_PUSHPULL_BRD		= (NET_MSG_GCTRL + 171),	//<	밀거나 당기는 액션 발생시.
	NET_MSG_GCTRL_JUMP_POS_BRD		= (NET_MSG_GCTRL + 172),	//<	케릭터의 위치가 순간적으로 변경되었을 경우. 클라이언트에 반영.

	NET_MSG_GCTRL_INVEN_INSERT		= (NET_MSG_GCTRL + 200),	//<	인밴에 아이탬 삽입됨.
	NET_MSG_GCTRL_INVEN_DELETE		= (NET_MSG_GCTRL + 201),	//<	인밴   아이탬 삭제.
	NET_MSG_GCTRL_INVEN_DEL_INSERT	= (NET_MSG_GCTRL + 202),	//<	인밴에 있던 아이탬 삭제후 아이탬 삽입.

	NET_MSG_GCTRL_PUTON_RELEASE		= (NET_MSG_GCTRL + 203),	//<	슬롯의 아이탬을 제거함.
	NET_MSG_GCTRL_PUTON_RELEASE_BRD	= (NET_MSG_GCTRL + 204),	//<	슬롯의 아이탬을 제거함.
	NET_MSG_GCTRL_PUTON_UPDATE		= (NET_MSG_GCTRL + 205),	//<	슬롯에 아이탬 변경.
	NET_MSG_GCTRL_PUTON_UPDATE_BRD	= (NET_MSG_GCTRL + 206),	//<	[방송] 사용 아이탬이 변경됨.

	NET_MSG_GCTRL_INVEN_DRUG_UPDATE	= (NET_MSG_GCTRL + 207),	//<	소모형 아이탬 소모됨을 업대이트.
	NET_MSG_GCTRL_PUTON_DRUG_UPDATE	= (NET_MSG_GCTRL + 208),	//<	소모형 아이탬 소모됨을 업대이트.

	NET_MSG_GCTRL_INVEN_ITEM_UPDATE	= (NET_MSG_GCTRL + 209),	//<	인밴 아이템 수치 업데이트.

	NET_MSG_GCTRL_PICKUP_MONEY		= (NET_MSG_GCTRL + 221),	//<	돈 습득.
	NET_MSG_GCTRL_PICKUP_ITEM		= (NET_MSG_GCTRL + 222),	//<	아이템 습득 알림.

	NET_MSG_GCTRL_2_FRIEND_REQ		= (NET_MSG_GCTRL + 231),	//<	친구로 가기 요청.
	NET_MSG_GCTRL_2_FRIEND_CK		= (NET_MSG_GCTRL + 232),	//<	친구로 가기 친구가 있는지 검사후 위치 반환.
	NET_MSG_GCTRL_2_FRIEND_AG		= (NET_MSG_GCTRL + 233),	//<	친구로 가기 AG로 회신 ( 친구 위치 정보 ).
	NET_MSG_GCTRL_2_FRIEND_FLD		= (NET_MSG_GCTRL + 234),	//<	친구로 가기 FLD로 이동 명령.
	NET_MSG_GCTRL_2_FRIEND_FB		= (NET_MSG_GCTRL + 235),	//<	친구로 가기 처리 결과 통보.

	NET_MSG_GCTRL_REQ_TAKE_FB		= (NET_MSG_GCTRL + 300),	//<	줍기 시도 결과.
	NET_MSG_GCTRL_REQ_FIELD_TO_INVEN= (NET_MSG_GCTRL + 301),	//<	아이탬,돈 줍기 시도.
	NET_MSG_GCTRL_REQ_FIELD_TO_HOLD	= (NET_MSG_GCTRL + 302),	//<	Field에 있는 아이탬을 들기 시도.

	NET_MSG_GCTRL_REQ_INVEN_TO_HOLD	= (NET_MSG_GCTRL + 303),	//<	인밴에 있는 아이탬을 들기 시도.
	NET_MSG_GCTRL_REQ_INVEN_EX_HOLD	= (NET_MSG_GCTRL + 304),	//<	인밴에 있는 아이탬을 들기 시도.

	NET_MSG_GCTRL_REQ_SLOT_TO_HOLD	= (NET_MSG_GCTRL + 305),	//<	아이탬, 슬롯에서 손으로.
	NET_MSG_GCTRL_REQ_SLOT_EX_HOLD	= (NET_MSG_GCTRL + 306),	//<	아이탬, 슬롯에 있는것과 손에 들고 있는것 교환.

	NET_MSG_GCTRL_REQ_HOLD_TO_FIELD	= (NET_MSG_GCTRL + 307),	//<	아이탬을 바닥에 떨어트림.
	NET_MSG_GCTRL_REQ_HOLD_TO_INVEN	= (NET_MSG_GCTRL + 308),	//<	아이탬을 INVEN에 넣음.
	NET_MSG_GCTRL_REQ_HOLD_TO_SLOT	= (NET_MSG_GCTRL + 309),	//<	아이탬, 손에서 슬롯으로.

	NET_MSG_GCTRL_REQ_HOLD_FB		= (NET_MSG_GCTRL + 310),	//<	인밴에 있는 아이탬을 들기 시도시 결과 피드백.
	NET_MSG_GCTRL_REQ_INVEN_TO_SLOT	= (NET_MSG_GCTRL + 311),	//<	인밴에 있는 아이탬을 슬롯에 넣기.

	NET_MSG_GCTRL_REQ_MONEY_TO_FIELD= (NET_MSG_GCTRL + 312),	//<	돈을 필드에 버리기.
	NET_MSG_GCTRL_REQ_INVEN_SPLIT	= (NET_MSG_GCTRL + 313),	//<	인벤에 있는 겹침 아이템 분리.

	NET_MSG_GCTRL_PUTON_CHANGE		= (NET_MSG_GCTRL + 314),	//<	무기 스왑.
	NET_MSG_GCTRL_PUTON_CHANGE_BRD	= (NET_MSG_GCTRL + 315),	//<	[방송] 무기 스왑 알림.
	NET_MSG_GCTRL_PUTON_CHANGE_AG	= (NET_MSG_GCTRL + 316),	//<	Agent로 무기 스왑 알림.
	NET_MSG_GCTRL_REQ_SLOT_CHANGE	= (NET_MSG_GCTRL + 317),	//<	무기 슬롯 체인지.

	NET_MSG_GCTRL_REQ_BUY_FROM_NPC	= (NET_MSG_GCTRL + 350),	//<	NPC에게 물품 판매 시도.
	NET_MSG_GCTRL_REQ_SALE_TO_NPC	= (NET_MSG_GCTRL + 351),	//<	NPC에게 물품 판매 시도.

	NET_MSG_GCTRL_REQ_REBIRTH		= (NET_MSG_GCTRL + 380),	//<	부활 요청.
	NET_MSG_GCTRL_REQ_REBIRTH_FB	= (NET_MSG_GCTRL + 381),	//<	부활 요청 응답.

	NET_MSG_GCTRL_REQ_LEVELUP		= (NET_MSG_GCTRL + 382),	//<	랩업 요청.
	NET_MSG_GCTRL_REQ_LEVELUP_FB	= (NET_MSG_GCTRL + 383),	//<	랩업 요청 응답.
	NET_MSG_GCTRL_REQ_LEVELUP_BRD	= (NET_MSG_GCTRL + 384),	//<	랩업 효과.

	NET_MSG_GCTRL_REQ_STATSUP		= (NET_MSG_GCTRL + 385),	//<	STATS UP
	NET_MSG_GCTRL_REQ_STATSUP_FB	= (NET_MSG_GCTRL + 386),	//<	STATS UP 응답.

	NET_MSG_GCTRL_REQ_INVENDRUG		= (NET_MSG_GCTRL + 400),	//<	인밴 약품 먹기.
	NET_MSG_GCTRL_REQ_LEARNSKILL	= (NET_MSG_GCTRL + 401),	//<	인밴 스킬 습득 요청.
	NET_MSG_GCTRL_REQ_LEARNSKILL_FB	= (NET_MSG_GCTRL + 402),	//<	인밴 스킬 습득 응답.
	NET_MSG_GCTRL_REQ_SKILLUP		= (NET_MSG_GCTRL + 403),	//<	스킬 업그래이드 요청.
	NET_MSG_GCTRL_REQ_SKILLUP_FB	= (NET_MSG_GCTRL + 404),	//<	스킬 업그래이드 응답.

	NET_MSG_GCTRL_REQ_RECALL		= (NET_MSG_GCTRL + 405),	//<	귀환 아이템 사용 요청.
	NET_MSG_GCTRL_REQ_RECALL_FB		= (NET_MSG_GCTRL + 406),	//<	귀환 아이템 사용 응답.
	NET_MSG_GCTRL_REQ_RECALL_AG		= (NET_MSG_GCTRL + 407),	//<	귀환을 에이젼트 서버에 요청.

	NET_MSG_GCTRL_REQ_BUS			= (NET_MSG_GCTRL + 408),	//<	버스 승차 요청.
	NET_MSG_GCTRL_REQ_BUS_FB		= (NET_MSG_GCTRL + 409),	//<	버스 승차 요청 응답.

	NET_MSG_GCTRL_REQ_SKILL			= (NET_MSG_GCTRL + 411),	//<	스킬 발동 요청.
	NET_MSG_GCTRL_REQ_SKILL_FB		= (NET_MSG_GCTRL + 412),	//<	스킬 발동 요청 응답.
	NET_MSG_GCTRL_REQ_SKILL_BRD		= (NET_MSG_GCTRL + 413),	//<	스킬 발동.
	NET_MSG_GCTRL_SKILLFACT_BRD		= (NET_MSG_GCTRL + 414),	//<	스킬 적용시 체력 변화.
	NET_MSG_GCTRL_SKILLHOLD_BRD		= (NET_MSG_GCTRL + 415),	//<	스킬 적용시 지속 효과.
	NET_MSG_GCTRL_SKILLCONSUME_FB	= (NET_MSG_GCTRL + 416),	//<	스킬 발동후 소모되는 수치값.

	NET_MSG_GCTRL_SKILL_CANCEL		= (NET_MSG_GCTRL + 417),	//<	스킬 취소.
	NET_MSG_GCTRL_SKILL_CANCEL_BRD	= (NET_MSG_GCTRL + 418),	//<	스킬 취소 클라이언트에 반영.

	NET_MSG_GCTRL_SKILLHOLD_RS_BRD	= (NET_MSG_GCTRL + 419),	//<	스킬 지속 효과 제거.
	NET_MSG_GCTRL_SKILLHOLDEX_BRD	= (NET_MSG_GCTRL + 420),	//<	스킬 적용시 지속 효과.

	NET_MSG_GCTRL_STATEBLOW_BRD		= (NET_MSG_GCTRL + 426),	//<	상태이상.
	NET_MSG_GCTRL_CURESTATEBLOW_BRD	= (NET_MSG_GCTRL + 427),	//<	상태이상.
	NET_MSG_GCTRL_LANDEFFECT		= (NET_MSG_GCTRL + 428),	//< 지형에 관련된 상태 이상

	NET_MSG_GCTRL_REQ_SKILLQ_ACTIVE	= (NET_MSG_GCTRL + 430),	//<	스킬 퀵슬롯의 액티브 스킬.
	NET_MSG_GCTRL_REQ_SKILLQ_SET	= (NET_MSG_GCTRL + 431),	//<	스킬 퀵슬롯 지정 요청.
	NET_MSG_GCTRL_REQ_SKILLQ_RESET	= (NET_MSG_GCTRL + 432),	//<	스킬 퀵슬롯 해지 요청.
	NET_MSG_GCTRL_REQ_SKILLQ_FB		= (NET_MSG_GCTRL + 433),	//<	스킬 퀵슬롯 요청에 따른 변경사항 클라이언트에 통지.

	NET_MSG_GCTRL_REQ_ACTIONQ_SET	= (NET_MSG_GCTRL + 435),	//<	엑션슬롯 지정 요청.
	NET_MSG_GCTRL_REQ_ACTIONQ_RESET	= (NET_MSG_GCTRL + 436),	//<	엑션슬롯 해지 요청.
	NET_MSG_GCTRL_REQ_ACTIONQ_FB	= (NET_MSG_GCTRL + 437),	//<	엑션슬롯 요청에 따른 변경사항 클라이언트에 통지.

	NET_MSG_GCTRL_REQ_GETSTORAGE		= (NET_MSG_GCTRL + 441),	//<	창고 정보 가져오기.
	NET_MSG_GCTRL_REQ_GETSTORAGE_FB		= (NET_MSG_GCTRL + 442),	//<	창고 정보 가져오기.
	NET_MSG_GCTRL_REQ_GETSTORAGE_ITEM	= (NET_MSG_GCTRL + 443),	//<	창고 정보 가져오기.

	NET_MSG_GCTRL_REQ_STORAGEDRUG		= (NET_MSG_GCTRL + 444),	//<	창고 약품 먹기.
	NET_MSG_GCTRL_REQ_STORAGESKILL		= (NET_MSG_GCTRL + 445),	//<	창고 스킬 먹기.

	NET_MSG_GCTRL_REQ_STORAGE_TO_HOLD	= (NET_MSG_GCTRL + 446),	//<	창고에 있는 아이탬을 들기 시도.
	NET_MSG_GCTRL_REQ_STORAGE_EX_HOLD	= (NET_MSG_GCTRL + 447),	//<	창고에 있는 아이탬을 들기 시도.
	NET_MSG_GCTRL_REQ_HOLD_TO_STORAGE	= (NET_MSG_GCTRL + 448),	//<	아이탬을 창고에 넣음.

	NET_MSG_GCTRL_STORAGE_INSERT		= (NET_MSG_GCTRL + 449),	//<	창고에 아이탬 삽입됨.
	NET_MSG_GCTRL_STORAGE_DELETE		= (NET_MSG_GCTRL + 450),	//<	창고 아이탬 삭제.
	NET_MSG_GCTRL_STORAGE_DEL_INSERT	= (NET_MSG_GCTRL + 451),	//<	창고 아이탬 삭제 및 삽입.
	NET_MSG_GCTRL_STORAGE_DRUG_UPDATE	= (NET_MSG_GCTRL + 452),	//<	창고 소모품 사용횟수 파악.

	NET_MSG_GCTRL_STORAGE_SAVE_MONEY	= (NET_MSG_GCTRL + 453),	//<	창고에 돈을 저장.
	NET_MSG_GCTRL_STORAGE_DRAW_MONEY	= (NET_MSG_GCTRL + 454),	//<	창고에서 돈을 빼냄.
	NET_MSG_GCTRL_STORAGE_UPDATE_MONEY	= (NET_MSG_GCTRL + 455),	//<	창고의 돈 액수 변화.

	NET_MSG_GCTRL_REQ_STORAGE_SPLIT		= (NET_MSG_GCTRL + 456),	//<	창고에 있는 겹침 아이템 분리.
	NET_MSG_GCTRL_STORAGE_STATE			= (NET_MSG_GCTRL + 457),	//<	창고 사용가능 속성.
	NET_MSG_GCTRL_PREMIUM_STATE			= (NET_MSG_GCTRL + 458),	//<	프리미엄 속성.

	NET_MSG_GCTRL_PARTY_LURE			= (NET_MSG_GCTRL + 460),	//<	파티 가입 요청.			클라이언트 -> 서버.
	NET_MSG_GCTRL_PARTY_LURE_FB			= (NET_MSG_GCTRL + 461),	//<	파티 가입 요청의 결과.	클라이언트 -> 서버.
	NET_MSG_GCTRL_PARTY_LURE_TAR		= (NET_MSG_GCTRL + 462),	//<	파티 가입 요청.			서버 -> 클라이언트.
	NET_MSG_GCTRL_PARTY_LURE_TAR_ANS	= (NET_MSG_GCTRL + 463),	//<	파티 가입 요청 응답.	클라이언트 -> 서버.

	NET_MSG_GCTRL_PARTY_SECEDE			= (NET_MSG_GCTRL + 464),	//<	파티 탈퇴 요청.

	NET_MSG_GCTRL_PARTY_ADD				= (NET_MSG_GCTRL + 465),	//<	파티원 추가.
	NET_MSG_GCTRL_PARTY_ADD_FB			= (NET_MSG_GCTRL + 466),	//<	파티원 추가 FB.

	NET_MSG_GCTRL_PARTY_DEL				= (NET_MSG_GCTRL + 467),	//<	파티원 제거.
	NET_MSG_GCTRL_PARTY_DEL_FB			= (NET_MSG_GCTRL + 468),	//<	파티원 제거 FB

	NET_MSG_GCTRL_PARTY_AUTHORITY		= (NET_MSG_GCTRL + 469),	//< 파티장 위임

	NET_MSG_GCTRL_PARTY_DISSOLVE		= (NET_MSG_GCTRL + 470),	//<	파티 해체.

	NET_MSG_GCTRL_PARTY_FNEW			= (NET_MSG_GCTRL + 471),	//<	파티 생성. (field).
	NET_MSG_GCTRL_PARTY_MBR_DETAIL		= (NET_MSG_GCTRL + 472),	//<	파티원의 세부 정보.
	NET_MSG_GCTRL_PARTY_MBR_MOVEMAP		= (NET_MSG_GCTRL + 473),	//<	파티원의 map 이동.
	NET_MSG_GCTRL_PARTY_MBR_POINT		= (NET_MSG_GCTRL + 474),	//<	파티원의 POINT 변화.
	NET_MSG_GCTRL_PARTY_MBR_POS			= (NET_MSG_GCTRL + 475),	//<	파티원의 위치 변화.
	NET_MSG_GCTRL_PARTY_MBR_PICKUP_BRD	= (NET_MSG_GCTRL + 476),	//<	파티원의 아이템 습득을 알림.

	NET_MSG_GCTRL_PARTY_BRD				= (NET_MSG_GCTRL + 477),	//<	케릭터의 파티 소속 변화를 주변 클라이언트들에 알림.
	NET_MSG_GCTRL_PARTY_MASTER_RENEW	= (NET_MSG_GCTRL + 478),	//< 파티마스터 재설정.

	NET_MSG_GCTRL_TRADE					= (NET_MSG_GCTRL + 480),	//<	트래이드 요청. ( 클라이언트1 -> 서버 )
	NET_MSG_GCTRL_TRADE_FB				= (NET_MSG_GCTRL + 481),	//<	트래이드 요청 응답. ( 서버 -> 클라이언트1 )
	NET_MSG_GCTRL_TRADE_TAR				= (NET_MSG_GCTRL + 482),	//<	트래이드 상대에게 요청. ( 서버 -> 클라이언트2 )
	NET_MSG_GCTRL_TRADE_TAR_ANS			= (NET_MSG_GCTRL + 483),	//<	트래이드 상대의 응답. ( 클라이언트2 -> 서버 )

	NET_MSG_GCTRL_TRADE_AGREE			= (NET_MSG_GCTRL + 484),	//<	트래이드 승인 지정.
	
	NET_MSG_GCTRL_TRADE_AGREE_TAR		= (NET_MSG_GCTRL + 486),	//<	트래이드 승인 지정 상대방에게 알림. ( 상대방에게. )

	NET_MSG_GCTRL_TRADE_ITEM_REGIST		= (NET_MSG_GCTRL + 487),	//<	트래이드할 아이템 등록.
	NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR	= (NET_MSG_GCTRL + 488),	//<	트래이드할 아이템 등록. ( 상대방에게. )
	NET_MSG_GCTRL_TRADE_ITEM_REMOVE		= (NET_MSG_GCTRL + 489),	//<	트래이드할 아이템 목록에서 제거.
	NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR	= (NET_MSG_GCTRL + 490),	//<	트래이드할 아이템 목록에서 제거. ( 상대방에게. )

	NET_MSG_GCTRL_TRADE_MONEY			= (NET_MSG_GCTRL + 491),	//<	트래이드할 금액.
	NET_MSG_GCTRL_TRADE_MONEY_TAR		= (NET_MSG_GCTRL + 492),	//<	트래이드할 금액을 상대방에게 알림.

	NET_MSG_GCTRL_TRADE_COMPLETE_TAR	= (NET_MSG_GCTRL + 493),	//<	트래이드 완료.

	NET_MSG_GCTRL_TRADE_CANCEL			= (NET_MSG_GCTRL + 494),	//<	트래이드 취소.
	NET_MSG_GCTRL_TRADE_CANCEL_TAR		= (NET_MSG_GCTRL + 495),	//<	트래이드 취소. ( 상대방에게. )

	NET_MSG_GCTRL_INVEN_BOXOPEN			= (NET_MSG_GCTRL + 496),	//<	box open 요청.
	NET_MSG_GCTRL_INVEN_BOXOPEN_FB		= (NET_MSG_GCTRL + 497),	//<	box open 요청 FB.

	NET_MSG_GCTRL_INVEN_DISGUISE		= (NET_MSG_GCTRL + 498),	//<	복장에 코스춤을 덧씌움.
	NET_MSG_GCTRL_INVEN_DISGUISE_FB		= (NET_MSG_GCTRL + 499),	//<	복장에 코스춤을 덧씌움 FB.

	NET_MSG_GCTRL_INVEN_GRINDING		= (NET_MSG_GCTRL + 501),	//<	INVEN 아이템 연마 시도.
	NET_MSG_GCTRL_INVEN_GRINDING_FB		= (NET_MSG_GCTRL + 502),	//<	INVEN 아이템 연마 시도.

	NET_MSG_GCTRL_REGEN_GATE			= (NET_MSG_GCTRL + 503),	//<	부활 위치 지정.
	NET_MSG_GCTRL_REGEN_GATE_FB			= (NET_MSG_GCTRL + 504),	//<	부활 위치 지정 FB.

	NET_MSG_GCTRL_CURE					= (NET_MSG_GCTRL + 505),	//<	치료 요청.
	NET_MSG_GCTRL_CURE_FB				= (NET_MSG_GCTRL + 506),	//<	치료 요청 FB.

	NET_MSG_GCTRL_CHARRESET				= (NET_MSG_GCTRL + 507),	//<	케릭터 정보 리셋 요청. ( stats, skill )
	NET_MSG_GCTRL_CHARRESET_FB			= (NET_MSG_GCTRL + 508),	//<	케릭터 정보 리셋 요청 FB. ( stats, skill )
	
	NET_MSG_GCTRL_NPC_ITEM_TRADE		= (NET_MSG_GCTRL + 509),	//<	아이템 교환 요청.
	NET_MSG_GCTRL_NPC_ITEM_TRADE_FB		= (NET_MSG_GCTRL + 510),	//<	아이템 교환 요청 FB.

	NET_MSG_GCTRL_CONFRONT				= (NET_MSG_GCTRL + 511),	//<	대련 요청 to (서버)
	NET_MSG_GCTRL_CONFRONT_TAR			= (NET_MSG_GCTRL + 512),	//<	대련 요청 to (상대방)
	NET_MSG_GCTRL_CONFRONT_ANS			= (NET_MSG_GCTRL + 513),	//<	대련 요청 응답 to (서버)
	NET_MSG_GCTRL_CONFRONT_FB			= (NET_MSG_GCTRL + 514),	//<	대련 요청 응답 to (요청자)

	NET_MSG_GCTRL_CONFRONT_START2_FLD	= (NET_MSG_GCTRL + 515),	//<	대련 시작 to (필드)
	NET_MSG_GCTRL_CONFRONT_START2_CLT	= (NET_MSG_GCTRL + 516),	//<	대련 시작 to (클라이언트)
	NET_MSG_GCTRL_CONFRONT_FIGHT2_CLT	= (NET_MSG_GCTRL + 517),	//<	대련 FIGHT to (클라이언트)

	NET_MSG_GCTRL_CONFRONT_END2_FLD		= (NET_MSG_GCTRL + 518),	//<	대련 종료 to (필드).
	NET_MSG_GCTRL_CONFRONT_END2_CLT		= (NET_MSG_GCTRL + 519),	//<	대련 종료 to (클라이언트).
	NET_MSG_GCTRL_CONFRONT_END2_CLT_BRD	= (NET_MSG_GCTRL + 520),	//<	대련 종료 to (주변 클라이언트).
	NET_MSG_GCTRL_CONFRONT_END2_AGT		= (NET_MSG_GCTRL + 521),	//<	대련 종료 to (에이전트).
	NET_MSG_GCTRL_CONFRONT_END2_CLT_MBR	= (NET_MSG_GCTRL + 522),	//<	대련 종료 to (대련 당사자 클라이언트).

	NET_MSG_GCTRL_CONFRONT_RECOVE		= (NET_MSG_GCTRL + 523),	//<	대련옵션 회복약 사용 횟수 to (클라이언트)

	NET_MSG_GCTRL_CONFRONTPTY_START2_FLD= (NET_MSG_GCTRL + 525),	//<	(파티) 대련 시작 to (필드).
	NET_MSG_GCTRL_CONFRONTPTY_START2_CLT= (NET_MSG_GCTRL + 526),	//<	(파티) 대련 시작 to (클라이언트)

	NET_MSG_GCTRL_CONFRONTPTY_END2_FLD	= (NET_MSG_GCTRL + 527),	//<	(파티) 대련 종료 to (필드).
	NET_MSG_GCTRL_CONFRONTPTY_END2_CLT	= (NET_MSG_GCTRL + 528),	//<	(파티) 대련 종료 to (클라이언트).

	NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_FLD = (NET_MSG_GCTRL + 529),	//<	(파티) 대련에 참가할 멤버 검사 to (필드).
	NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_AGT = (NET_MSG_GCTRL + 529),	//<	(파티) 대련에 참가할 멤버 to (에이젼트).

	NET_MSG_GCTRL_CONFRONTSPTY_EXP_FLD	= (NET_MSG_GCTRL + 530),	//<	학원 파티 대련 승리시 경험치 전체 유저에 exp 부여.

	NET_MSG_GCTRL_CONFRONTCLB_START2_FLD= (NET_MSG_GCTRL + 531),	//<	(클럽) 대련 시작 to (필드).
	NET_MSG_GCTRL_CONFRONTCLB_START2_CLT= (NET_MSG_GCTRL + 532),	//<	(클럽) 대련 시작 to (클라이언트)

	NET_MSG_GCTRL_CONFRONTCLB_END2_FLD	= (NET_MSG_GCTRL + 533),	//<	(클럽) 대련 종료 to (필드).
	NET_MSG_GCTRL_CONFRONTCLB_END2_CLT	= (NET_MSG_GCTRL + 534),	//<	(클럽) 대련 종료 to (클라이언트).

	NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_FLD = (NET_MSG_GCTRL + 535),	//<	(클럽) 대련에 참가할 멤버 검사 to (필드).
	NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_AGT = (NET_MSG_GCTRL + 536),	//<	(클럽) 대련에 참가할 멤버 to (에이젼트).

	NET_MSG_GCTRL_SERVERTEST			= (NET_MSG_GCTRL + 537),	// Purpose for field server test
	NET_MSG_GCTRL_SERVERTEST_FB			= (NET_MSG_GCTRL + 538),

	NET_MSG_GCTRL_REQ_GENITEM_AGT		= (NET_MSG_GCTRL + 541),	//<	아이템 생성 질의 to (에이전트).
	NET_MSG_GCTRL_REQ_GENITEM_FLD_FB	= (NET_MSG_GCTRL + 542),	//<	아이템 생성 답변 to (필드).

	NET_MSG_GCTRL_REQ_GESTURE			= (NET_MSG_GCTRL + 543),	//<	제스쳐 동작.
	NET_MSG_GCTRL_REQ_GESTURE_BRD		= (NET_MSG_GCTRL + 544),	//<	제스쳐 동작 BRD.

	NET_MSG_GCTRL_QITEMFACT_BRD			= (NET_MSG_GCTRL + 545),	//<	?아이템 수치 BRD.
	NET_MSG_GCTRL_QITEMFACT_END_BRD		= (NET_MSG_GCTRL + 546),	//<	?아이템 종료 BRD.

	NET_MSG_GCTRL_EVENTFACT_BRD			= (NET_MSG_GCTRL + 547),	//<	?아이템 이벤트 시작 BRD.
	NET_MSG_GCTRL_EVENTFACT_END_BRD		= (NET_MSG_GCTRL + 548),	//<	?아이템 이벤트 종료 BRD.
	NET_MSG_GCTRL_EVENTFACT_INFO		= (NET_MSG_GCTRL + 549),	//<	?아이템 이벤트 BRD.

	NET_MSG_GCTRL_INVEN_CLEANSER		= (NET_MSG_GCTRL + 551),	//<	망각의 세제를 사용하여 변환된 형상을 되돌림.
	NET_MSG_GCTRL_INVEN_CLEANSER_FB		= (NET_MSG_GCTRL + 552),	//<	망각의 세제를 사용하여 변환된 형상을 되돌림 FB.

	NET_MSG_GCTRL_INVEN_DEL_ITEM_TIMELMT= (NET_MSG_GCTRL + 553),	//<	제한 시간 만료로 아이템 삭제.

	NET_MSG_GCTRL_INVEN_RESET_SKST		= (NET_MSG_GCTRL + 554),	//<	스텟 스킬 리셋 요청.
	NET_MSG_GCTRL_INVEN_RESET_SKST_FB	= (NET_MSG_GCTRL + 555),	//<	스텟 스킬 리셋 응답.

	NET_MSG_GCTRL_INVEN_CHARCARD		= (NET_MSG_GCTRL + 560),	//<	케릭터 추가 카드 사용 요청.
	NET_MSG_GCTRL_INVEN_CHARCARD_FB		= (NET_MSG_GCTRL + 561),	//<	케릭터 추가 카드 사용 요청 FB.

	NET_MSG_GCTRL_INVEN_INVENLINE		= (NET_MSG_GCTRL + 562),	//<	인벤라인 추가 카드 사용 요청.
	NET_MSG_GCTRL_INVEN_INVENLINE_FB	= (NET_MSG_GCTRL + 563),	//<	인벤라인 추가 카드 사용 요청 FB.

	NET_MSG_GCTRL_INVEN_STORAGECARD		= (NET_MSG_GCTRL + 564),	//<	창고 추가 카드 사용 요청.
	NET_MSG_GCTRL_INVEN_STORAGECARD_FB	= (NET_MSG_GCTRL + 565),	//<	창고 추가 카드 사용 요청 FB.

	NET_MSG_GCTRL_INVEN_STORAGEOPEN		= (NET_MSG_GCTRL + 566),	//<	긴급 창고 이용 카드 사용 요청.
	NET_MSG_GCTRL_INVEN_STORAGEOPEN_FB	= (NET_MSG_GCTRL + 567),	//<	긴급 창고 이용 카드 사용 요청 FB.
	NET_MSG_GCTRL_INVEN_STORAGECLOSE	= (NET_MSG_GCTRL + 568),	//<	긴급 창고 이용 카드 사용 완료.

	NET_MSG_GCTRL_INVEN_PREMIUMSET		= (NET_MSG_GCTRL + 569),	//<	프리미엄셋 사용 요청.
	NET_MSG_GCTRL_INVEN_PREMIUMSET_FB	= (NET_MSG_GCTRL + 570),	//<	프리미엄셋 사용 요청 FB.

	NET_MSG_GCTRL_FIRECRACKER			= (NET_MSG_GCTRL + 571),	//<	폭죽 사용 요청.
	NET_MSG_GCTRL_FIRECRACKER_FB		= (NET_MSG_GCTRL + 572),	//<	폭죽 사용 요청 FB.
	NET_MSG_GCTRL_FIRECRACKER_BRD		= (NET_MSG_GCTRL + 573),	//<	폭죽 사용.

	NET_MSG_GCTRL_REVIVE				= (NET_MSG_GCTRL + 574),	//<	귀혼주 사용
	NET_MSG_GCTRL_REVIVE_FB				= (NET_MSG_GCTRL + 575),	//<	귀혼주 사용 FB.

	NET_MSG_GCTRL_PLAYERKILLING_ADD		= (NET_MSG_GCTRL + 576),	//<	player killing 당사자 추가.
	NET_MSG_GCTRL_PLAYERKILLING_DEL		= (NET_MSG_GCTRL + 577),	//<	player killing 당사자 제거.

	NET_MSG_GCTRL_GET_CHARGEDITEM_FROMDB	= (NET_MSG_GCTRL + 579),//<	구입한 아이템 정보 DB에서 가져오기 요청
	NET_MSG_GCTRL_GET_CHARGEDITEM_FROMDB_FB = (NET_MSG_GCTRL + 580),//<	구입한 아이템 정보 DB에서 가져오기 요청 FB
	NET_MSG_GCTRL_CHARGED_ITEM_GET		= (NET_MSG_GCTRL + 581),	//<	구입한 아이템 가져오기 요청.
	NET_MSG_GCTRL_CHARGED_ITEM_GET_FB	= (NET_MSG_GCTRL + 582),	//<	구입한 아이템 가져오기 요청 FB.
	NET_MSG_GCTRL_CHARGED_ITEM_DEL		= (NET_MSG_GCTRL + 583),	//<	구입한 아이템 리스트에서 삭제.
	NET_MSG_GCTRL_CHARGED_ITEM2_INVEN	= (NET_MSG_GCTRL + 584),	//<	구입한 아이템 인벤에 넣기 요청.

	NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN	= (NET_MSG_GCTRL + 585),	//<	random box open 요청.
	NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN_FB= (NET_MSG_GCTRL + 586),	//<	random box open 요청 FB.

	NET_MSG_GCTRL_INVEN_DISJUNCTION		= (NET_MSG_GCTRL + 587),	//<	코스툼 분리 요청.
	NET_MSG_GCTRL_INVEN_DISJUNCTION_FB	= (NET_MSG_GCTRL + 588),	//<	코스툼 분리 요청 FB.

	NET_MSG_GCTRL_INVEN_HAIR_CHANGE		= (NET_MSG_GCTRL + 591),	//<	헤어스타일 변경 요청.
	NET_MSG_GCTRL_INVEN_HAIR_CHANGE_FB	= (NET_MSG_GCTRL + 592),	//<	헤어스타일 변경 결과 FB.
	NET_MSG_GCTRL_INVEN_HAIR_CHANGE_BRD	= (NET_MSG_GCTRL + 593),	//<	헤어스타일 변경 결과 BRD.

	NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE		= (NET_MSG_GCTRL + 594),	//<	헤어스타일 변경 요청.
	NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE_FB		= (NET_MSG_GCTRL + 595),	//<	헤어스타일 변경 결과 FB.
	NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE_BRD	= (NET_MSG_GCTRL + 596),	//<	헤어스타일 변경 결과 BRD.

	NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE		= (NET_MSG_GCTRL + 597),	//<	헤어컬러 변경 요청.
	NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_FB		= (NET_MSG_GCTRL + 598),	//<	헤어컬러 변경 결과 FB.
	NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_BRD	= (NET_MSG_GCTRL + 599),	//<	헤어컬러 변경 결과 BRD.

	NET_MSG_GCTRL_INVEN_FACE_CHANGE		= (NET_MSG_GCTRL + 601),	//<	얼굴스타일 변경 요청.
	NET_MSG_GCTRL_INVEN_FACE_CHANGE_FB	= (NET_MSG_GCTRL + 602),	//<	얼굴스타일 변경 결과 FB.
	NET_MSG_GCTRL_INVEN_FACE_CHANGE_BRD	= (NET_MSG_GCTRL + 603),	//<	얼굴스타일 변경 결과 BRD.

	NET_MSG_GCTRL_INVEN_REMODELOPEN		= (NET_MSG_GCTRL + 604),	//<	개조 기능 이용 카드 사용 요청.
	NET_MSG_GCTRL_INVEN_REMODELOPEN_FB	= (NET_MSG_GCTRL + 605),	//<	개조 기능 이용 카드 사용 요청 FB.

	NET_MSG_GCTRL_INVEN_GARBAGEOPEN		= (NET_MSG_GCTRL + 606),	//<	휴지통 기능 이용 카드 사용 요청
	NET_MSG_GCTRL_INVEN_GARBAGEOPEN_FB	= (NET_MSG_GCTRL + 607),	//<	휴지통 기능 이용 카드 사용 요청 FB.
	NET_MSG_GCTRL_GARBAGE_RESULT		= (NET_MSG_GCTRL + 608),	//<	아이템 삭제 요청
	NET_MSG_GCTRL_GARBAGE_RESULT_FB		= (NET_MSG_GCTRL + 609),	//<	아이템 삭제 요청 FB.

	NET_MSG_GCTRL_CROW_MOVETO			= (NET_MSG_GCTRL + 611),
	NET_MSG_GCTRL_CROW_ATTACK			= (NET_MSG_GCTRL + 612),
	NET_MSG_GCTRL_CROW_AVOID			= (NET_MSG_GCTRL + 613),
	NET_MSG_GCTRL_CROW_DAMAGE			= (NET_MSG_GCTRL + 614),
	NET_MSG_GCTRL_CROW_SKILL			= (NET_MSG_GCTRL + 615),

	NET_MSG_GCTRL_INVEN_RENAME			= (NET_MSG_GCTRL + 620),	//<	이름 변경 요청.
	NET_MSG_GCTRL_INVEN_RENAME_FB		= (NET_MSG_GCTRL + 621),	//<	이름 변경 결과 FB.
	NET_MSG_GCTRL_INVEN_RENAME_FROM_DB	= (NET_MSG_GCTRL + 622),	//<	DB 저장 결과 FB.
	NET_MSG_GCTRL_INVEN_RENAME_AGTBRD	= (NET_MSG_GCTRL + 623),	//<	Agent에 모든채널 BRD 요청.
	NET_MSG_GCTRL_INVEN_RENAME_BRD		= (NET_MSG_GCTRL + 624),	//<	이름 변경 결과 BRD.

	NET_MSG_GCTRL_PARTY_MBR_RENAME_CLT	= (NET_MSG_GCTRL + 626),	//< 이름 변경을 파티멤버에게 알림.
	NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT	= (NET_MSG_GCTRL + 627),	//< 이름 변경을 클럽멤버에게 알림.
	NET_MSG_GCTRL_FRIEND_RENAME_CLT		= (NET_MSG_GCTRL + 628),	//< 이름 변경을 친구들에게 알림.
	NET_MSG_GCTRL_FRIEND_PHONENUMBER_CLT= (NET_MSG_GCTRL + 629),	//< 폰번호 변경을 친구들에게 알림.
	NET_MSG_GCTRL_PARTY_MBR_RENAME_FLD	= (NET_MSG_GCTRL + 630),	//< 이름 변경을 FieldServer에 알림.
	NET_MSG_GCTRL_CLUB_MBR_RENAME_FLD	= (NET_MSG_GCTRL + 631),	//< 이름 변경을 FieldServer에 알림.
	NET_MSG_GCTRL_FRIEND_RENAME_FLD		= (NET_MSG_GCTRL + 632),	//< 이름 변경을 FieldServer에 알림.
	NET_MSG_GCTRL_CHANGE_NAMEMAP	 	= (NET_MSG_GCTRL + 633),	//< GLLandMan 의 NameMap 변경


	NET_MSG_GCTRL_REQ_QUEST_START		= (NET_MSG_GCTRL + 646),	//<	퀘스트 시작을 요청.
	NET_MSG_GCTRL_REQ_QUEST_START_FB	= (NET_MSG_GCTRL + 647),	//<	퀘스트 시작을 요청 FB.
	NET_MSG_GCTRL_REQ_QUEST_TALK		= (NET_MSG_GCTRL + 648),	//<	퀘스트 대화 진행 요청.
	NET_MSG_GCTRL_REQ_QUEST_TALK_FB		= (NET_MSG_GCTRL + 649),	//<	퀘스트 대화 진행 요청 FB.

	NET_MSG_GCTRL_QUEST_PROG_GIVEUP		= (NET_MSG_GCTRL + 651),	//<	퀘스트 진행 포기 요청.
	NET_MSG_GCTRL_QUEST_PROG_DEL		= (NET_MSG_GCTRL + 652),	//<	퀘스트 진행 리스트에서 삭제.
	NET_MSG_GCTRL_QUEST_END_DEL			= (NET_MSG_GCTRL + 653),	//<	퀘스트 완료 리스트에서 삭제.
	NET_MSG_GCTRL_QUEST_END_STREAM		= (NET_MSG_GCTRL + 654),	//<	퀘스트 완료 정보.

	NET_MSG_GCTRL_QUEST_PROG_STREAM		= (NET_MSG_GCTRL + 655),	//<	퀘스트 진행 정보.
	NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM= (NET_MSG_GCTRL + 656),	//<	퀘스트 진행 step.
	NET_MSG_GCTRL_QUEST_PROG_INVEN		= (NET_MSG_GCTRL + 657),	//<	퀘스트 INVENTORY.

	NET_MSG_GCTRL_QUEST_PROG_NPCTALK	= (NET_MSG_GCTRL + 661),	//<	퀘스트 진행 npc 대화.
	NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB	= (NET_MSG_GCTRL + 662),	//<	퀘스트 진행 npc 대화.

	NET_MSG_GCTRL_QUEST_PROG_MOBKILL		= (NET_MSG_GCTRL + 663),	//<	퀘스트 진행 mob 제거.
	NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL	= (NET_MSG_GCTRL + 664),	//<	퀘스트 진행 mob 제거.( 파티퀘스트 완료시 )
	NET_MSG_GCTRL_QUEST_PROG_QITEM			= (NET_MSG_GCTRL + 665),	//<	퀘스트 진행 퀘스트 아이템 습득.
	NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM	= (NET_MSG_GCTRL + 666),	//<	퀘스트 진행 퀘스트 아이템 습득.( 파티퀘스트 완료시 )
	NET_MSG_GCTRL_QUEST_PROG_REACHZONE		= (NET_MSG_GCTRL + 667),	//<	퀘스트 진행 위치 도달.
	NET_MSG_GCTRL_QUEST_PROG_READ			= (NET_MSG_GCTRL + 668),	//<	퀘스트 읽기 요청 리셋.
	NET_MSG_GCTRL_QUEST_PROG_TIME			= (NET_MSG_GCTRL + 669),	//<	퀘스트 경과 시간.
	NET_MSG_GCTRL_QUEST_PROG_TIMEOVER		= (NET_MSG_GCTRL + 670),	//<	퀘스트 시간 제한 초과됨.
	NET_MSG_GCTRL_QUEST_PROG_NONDIE			= (NET_MSG_GCTRL + 671),	//<	퀘스트 사망 제한.
	NET_MSG_GCTRL_QUEST_PROG_LEVEL			= (NET_MSG_GCTRL + 672),	//<	퀘스트 레벨 도달.

	NET_MSG_GCTRL_QUEST_COMPLETE_FB		= (NET_MSG_GCTRL + 673),	//<	퀘스트가 완료되었으나 보상 물품을 주지 못한상태.
	NET_MSG_GCTRL_REQ_QUEST_COMPLETE	= (NET_MSG_GCTRL + 674),	//<	퀘스트 완료 요청 ( 보상물품 습득 ).

	NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN	= (NET_MSG_GCTRL + 681),	//<	퀘스트 인밴 수량 변경.
	NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP= (NET_MSG_GCTRL + 682),	//<	퀘스트 인밴로 아이템 습득.

	NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT= (NET_MSG_GCTRL + 683),	//<	퀘스트 인밴 아이탬 삽입.
	NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE= (NET_MSG_GCTRL + 684),	//<	퀘스트 인밴 아이탬 삭제.

	
	NET_MSG_GCTRL_PMARKET_TITLE			= (NET_MSG_GCTRL + 701),	//<	개인 상점의 타이틀 설정.
	NET_MSG_GCTRL_PMARKET_TITLE_FB		= (NET_MSG_GCTRL + 702),	//<	개인 상점의 타이틀 설정.

	NET_MSG_GCTRL_PMARKET_REGITEM		= (NET_MSG_GCTRL + 703),	//<	개인 상점에 판매 아이템 등록.
	NET_MSG_GCTRL_PMARKET_REGITEM_FB	= (NET_MSG_GCTRL + 704),	//<	개인 상점에 판매 아이템 등록 FB.

	NET_MSG_GCTRL_PMARKET_DISITEM		= (NET_MSG_GCTRL + 705),	//<	개인 상점에서 아이템 제거.
	NET_MSG_GCTRL_PMARKET_DISITEM_FB	= (NET_MSG_GCTRL + 706),	//<	개인 상점에서 아이템 제거 FB.
	NET_MSG_GCTRL_PMARKET_OPEN			= (NET_MSG_GCTRL + 707),	//<	개인 상점 오픈.
	NET_MSG_GCTRL_PMARKET_OPEN_FB		= (NET_MSG_GCTRL + 708),	//<	개인 상점 오픈 FB.
	NET_MSG_GCTRL_PMARKET_OPEN_BRD		= (NET_MSG_GCTRL + 709),	//<	개인 상점 오픈 BRD.

	NET_MSG_GCTRL_PMARKET_CLOSE			= (NET_MSG_GCTRL + 710),	//<	개인 상점 종료.
	NET_MSG_GCTRL_PMARKET_CLOSE_FB		= (NET_MSG_GCTRL + 711),	//<	개인 상점 종료 FB.
	NET_MSG_GCTRL_PMARKET_CLOSE_BRD		= (NET_MSG_GCTRL + 712),	//<	개인 상점 종료 BRD.
	
	NET_MSG_GCTRL_PMARKET_BUY			= (NET_MSG_GCTRL + 713),	//<	개인 상점에서 구입.
	NET_MSG_GCTRL_PMARKET_BUY_FB		= (NET_MSG_GCTRL + 714),	//<	개인 상점에서 구입 FB.

	NET_MSG_GCTRL_PMARKET_ITEM_INFO		= (NET_MSG_GCTRL + 715),	//<	개인 상점의 판매 아이템 정보 요청.
	NET_MSG_GCTRL_PMARKET_ITEM_INFO_BRD	= (NET_MSG_GCTRL + 716),	//<	개인 상점의 판매 아이템 BRD.
	NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD= (NET_MSG_GCTRL + 717),	//<	개인 상점의 판매 아이템 Update BRD.

	NET_MSG_GCTRL_CLUB_NEW				= (NET_MSG_GCTRL + 720),	//<	클럽 생성.
	NET_MSG_GCTRL_CLUB_NEW_2AGT			= (NET_MSG_GCTRL + 721),	//<	클럽 생성 to AGENT.
	NET_MSG_GCTRL_CLUB_NEW_DB2AGT		= (NET_MSG_GCTRL + 722),	//<	클럽 생성 DB to AGENT.
	NET_MSG_GCTRL_CLUB_NEW_2FLD			= (NET_MSG_GCTRL + 723),	//<	클럽 생성 to FIELD.
	NET_MSG_GCTRL_CLUB_NEW_FB			= (NET_MSG_GCTRL + 724),	//<	클럽 생성 FB.

	NET_MSG_GCTRL_CLUB_DISSOLUTION		= (NET_MSG_GCTRL + 725),	//<	클럽 해체 요청.
	NET_MSG_GCTRL_CLUB_DISSOLUTION_2FLD	= (NET_MSG_GCTRL + 726),	//<	클럽 해체 요청.
	NET_MSG_GCTRL_CLUB_DISSOLUTION_FB	= (NET_MSG_GCTRL + 727),	//<	클럽 해체 요청 FB.
	NET_MSG_GCTRL_CLUB_DEL_2FLD			= (NET_MSG_GCTRL + 728),	//<	클럽 해체 to FIELD.
	NET_MSG_GCTRL_CLUB_DEL_2CLT			= (NET_MSG_GCTRL + 729),	//<	클럽 삭제 to CLIENT. ( 해체 및 탈퇴 )
	NET_MSG_GCTRL_CLUB_DEL_BRD			= (NET_MSG_GCTRL + 730),	//<	클럽 삭제 BRD. ( 해체 및 탈퇴 )

	NET_MSG_GCTRL_CLUB_INFO_2FLD		= (NET_MSG_GCTRL + 731),	//<	클럽 정보 to FLIED.
	NET_MSG_GCTRL_CLUB_INFO_2CLT		= (NET_MSG_GCTRL + 732),	//<	클럽 정보 to CLIENT.
	NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION	= (NET_MSG_GCTRL + 733),	//<	클럽 해체예약 정보 to CLIENT.
	NET_MSG_GCTRL_CLUB_MEMBER_2CLT		= (NET_MSG_GCTRL + 734),	//<	클럽 멤버 to CLIENT.

	NET_MSG_GCTRL_CLUB_INFO_BRD			= (NET_MSG_GCTRL + 735),	//<	클럽 정보 BRD.
	NET_MSG_GCTRL_CLUB_INFO_MARK_BRD	= (NET_MSG_GCTRL + 736),	//<	클럽 마크 정보 BRD.
	NET_MSG_GCTRL_CLUB_INFO_NICK_BRD	= (NET_MSG_GCTRL + 737),	//<	클럽 닉네임 정보 BRD.

	NET_MSG_GCTRL_CLUB_MARK_INFO		= (NET_MSG_GCTRL + 740),	//<	클럽 마크 요청.
	NET_MSG_GCTRL_CLUB_MARK_INFO_FB		= (NET_MSG_GCTRL + 741),	//<	클럽 마크 반환.

	NET_MSG_GCTRL_CLUB_MARK_CHANGE		= (NET_MSG_GCTRL + 742),	//<	클럽 마크 변경 요청.
	NET_MSG_GCTRL_CLUB_MARK_CHANGE_2FLD	= (NET_MSG_GCTRL + 743),	//<	클럽 마크 변경 요청.
	NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT	= (NET_MSG_GCTRL + 744),	//<	클럽 마크 변경 요청.
	NET_MSG_GCTRL_CLUB_MARK_CHANGE_FB	= (NET_MSG_GCTRL + 745),	//<	클럽 마크 변경 FB.

	NET_MSG_GCTRL_CLUB_RANK				= (NET_MSG_GCTRL + 750),	//<	클럽 랭크 변경.
	NET_MSG_GCTRL_CLUB_RANK_2AGT		= (NET_MSG_GCTRL + 751),	//<	클럽 랭크 변경.
	NET_MSG_GCTRL_CLUB_RANK_2FLD		= (NET_MSG_GCTRL + 752),	//<	클럽 랭크 변경.
	NET_MSG_GCTRL_CLUB_RANK_2CLT		= (NET_MSG_GCTRL + 753),	//<	클럽 랭크 변경.
	NET_MSG_GCTRL_CLUB_RANK_FB			= (NET_MSG_GCTRL + 754),	//<	클럽 랭크 변경 FB.

	NET_MSG_GCTRL_CLUB_MEMBER_REQ		= (NET_MSG_GCTRL + 755),	//<	클럽 참가 요청.
	NET_MSG_GCTRL_CLUB_MEMBER_REQ_ASK	= (NET_MSG_GCTRL + 756),	//<	클럽 참가 질의.
	NET_MSG_GCTRL_CLUB_MEMBER_REQ_ANS	= (NET_MSG_GCTRL + 757),	//<	클럽 참가 답변.
	NET_MSG_GCTRL_CLUB_MEMBER_REQ_2AGT	= (NET_MSG_GCTRL + 758),	//<	클럽 참가 답변.
	NET_MSG_GCTRL_CLUB_MEMBER_REQ_FB	= (NET_MSG_GCTRL + 759),	//<	클럽 참가 요청 FB.

	NET_MSG_GCTRL_CLUB_MEMBER_ADD_2FLD	= (NET_MSG_GCTRL + 760),	//<	클럽 멤버 추가 to FIELD.

	NET_MSG_GCTRL_CLUB_MEMBER_DEL		= (NET_MSG_GCTRL + 761),	//<	클럽 멤버 제명 요청.
	NET_MSG_GCTRL_CLUB_MEMBER_DEL_2FLD	= (NET_MSG_GCTRL + 762),	//<	클럽 멤버 제명 요청 to FIELD.
	NET_MSG_GCTRL_CLUB_MEMBER_DEL_2CLT	= (NET_MSG_GCTRL + 763),	//<	클럽 멤버 제명 요청 결과 to CLIENT.
	NET_MSG_GCTRL_CLUB_MEMBER_DEL_FB	= (NET_MSG_GCTRL + 764),	//<	클럽 멤버 제명 요청 결과 FB.

	NET_MSG_GCTRL_CLUB_MEMBER_NICK		= (NET_MSG_GCTRL + 765),	//<	클럽 멤버 별명.
	NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB	= (NET_MSG_GCTRL + 766),	//<	클럽 멤버 별명 FB.

	NET_MSG_GCTRL_CLUB_MEMBER_STATE		= (NET_MSG_GCTRL + 767),	//<	클럽 멤버 상태.
	NET_MSG_GCTRL_CLUB_MEMBER_POS		= (NET_MSG_GCTRL + 768),	//<	클럽 멤버 위치.

	NET_MSG_GCTRL_CLUB_MEMBER_SECEDE	= (NET_MSG_GCTRL + 769),	//<	클럽 멤버 탈퇴.
	NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB	= (NET_MSG_GCTRL + 770),	//<	클럽 멤버 탈퇴 FB.

	NET_MSG_GCTRL_CLUB_MEMBER_DB2DEL	= (NET_MSG_GCTRL + 771),	//<	클럽 멤버 제명 요청.

	NET_MSG_GCTRL_CLUB_BATTLE_REMAIN_BRD = (NET_MSG_GCTRL + 775),	//<	선도 클럽 결정전 종료.
	NET_MSG_GCTRL_CLUB_BATTLE_START_FLD	= (NET_MSG_GCTRL + 776),	//<	선도 클럽 결정전 시작.
	NET_MSG_GCTRL_CLUB_BATTLE_START_BRD	= (NET_MSG_GCTRL + 777),	//<	선도 클럽 결정전 시작.

	NET_MSG_GCTRL_CLUB_BATTLE_END_FLD	= (NET_MSG_GCTRL + 778),	//<	선도 클럽 결정전 종료.
	NET_MSG_GCTRL_CLUB_BATTLE_END_BRD	= (NET_MSG_GCTRL + 779),	//<	선도 클럽 결정전 종료.

	NET_MSG_GCTRL_CLUB_CD_CERTIFY		  = (NET_MSG_GCTRL + 780),	//<	cd 인증.
	NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB      = (NET_MSG_GCTRL + 780),	//<	cd FB
	NET_MSG_GCTRL_CLUB_CD_CERTIFY_BRD	  = (NET_MSG_GCTRL + 781),	//<	cd 인증 BRD.
	NET_MSG_GCTRL_CLUB_CD_CERTIFY_ING_BRD = (NET_MSG_GCTRL + 782),	//<	cd 인증 결과 BRD

	NET_MSG_GCTRL_CLUB_CERTIFIED_AGT	= (NET_MSG_GCTRL + 787),	//<	cd 인증 완료 ( agent에 알림 )
	NET_MSG_GCTRL_CLUB_CERTIFIED_FLD	= (NET_MSG_GCTRL + 788),	//<	cd 인증 완료 ( field에 알림 )
	NET_MSG_GCTRL_CLUB_CERTIFIED_CLT	= (NET_MSG_GCTRL + 789),	//<	cd 인증 완료 ( field에 알림 )

	NET_MSG_GCTRL_CLUB_COMMISSION		= (NET_MSG_GCTRL + 790),	//<	선도 지역 수수료 설정.
	NET_MSG_GCTRL_CLUB_COMMISSION_FB	= (NET_MSG_GCTRL + 791),	//<	선도 지역 수수료 설정 FB.
	NET_MSG_GCTRL_CLUB_COMMISSION_FLD	= (NET_MSG_GCTRL + 792),	//<	선도 지역 수수료 설정 FLD.
	NET_MSG_GCTRL_CLUB_COMMISSION_BRD	= (NET_MSG_GCTRL + 793),	//<	선도 지역 수수료 설정 BRD.
	NET_MSG_GCTRL_CLUB_INCOME_MONEY_AGT	= (NET_MSG_GCTRL + 794),	//<	클럽에 수익 발생 AGT.

	NET_MSG_GCTRL_CLUB_GETSTORAGE		= (NET_MSG_GCTRL + 795),	//<	창고 정보 요청.
	NET_MSG_GCTRL_CLUB_GETSTORAGE_FB	= (NET_MSG_GCTRL + 796),	//<	창고 정보 요청.
	NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM	= (NET_MSG_GCTRL + 797),	//<	창고 정보 회신.

	NET_MSG_GCTRL_CLUB_STORAGE_TO_HOLD	= (NET_MSG_GCTRL + 798),	//<	창고 아이템 들어올리기.
	NET_MSG_GCTRL_CLUB_STORAGE_EX_HOLD	= (NET_MSG_GCTRL + 799),	//<	창고 아이템과 손에 들린 아이템 교환.
	NET_MSG_GCTRL_CLUB_HOLD_TO_STORAGE	= (NET_MSG_GCTRL + 800),	//<	손에 들린 아이템 창고에 넣기.
	NET_MSG_GCTRL_CLUB_STORAGE_SPLIT	= (NET_MSG_GCTRL + 801),	//<	창고의 아이템 분할하기.
	NET_MSG_GCTRL_CLUB_STORAGE_SAVE_MONEY= (NET_MSG_GCTRL + 802),	//<	돈을 저장.
	NET_MSG_GCTRL_CLUB_STORAGE_DRAW_MONEY= (NET_MSG_GCTRL + 803),	//<	돈을 인출.
	
	NET_MSG_GCTRL_CLUB_STORAGE_INSERT	= (NET_MSG_GCTRL + 804),	//<	창고에 넣기.
	NET_MSG_GCTRL_CLUB_STORAGE_DELETE	= (NET_MSG_GCTRL + 805),	//<	창고에서 삭제.
	NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS	= (NET_MSG_GCTRL + 806),	//<	삭제와 넣기 동시에 하기.
	NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM= (NET_MSG_GCTRL + 807),	//<	창고에 있는 아이템의 정보 갱신.
	NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY= (NET_MSG_GCTRL + 808),	//<	창고의 저장금액 갱신.

	NET_MSG_GCTRL_CLUB_COMMISSION_RV_BRD= (NET_MSG_GCTRL + 809),	//<	선도 지역 수수료 설정 예약 BRD.

	NET_MSG_GCTRL_CLUB_STORAGE_RESET	= (NET_MSG_GCTRL + 811),	//<	클럽 창고 정보를 리샛.
	NET_MSG_GCTRL_CLUB_STORAGE_GET_DB	= (NET_MSG_GCTRL + 812),	//<	클럽 창고 정보를 db에서 수신.

	NET_MSG_GCTRL_CLUB_INCOME_RENEW		= (NET_MSG_GCTRL + 815),	//<	클럽 수익금 업뎃 요청 ( client->field ).
	NET_MSG_GCTRL_CLUB_INCOME_UP		= (NET_MSG_GCTRL + 816),	//<	클럽 수익금 업뎃 요청 ( field->agent ).
	NET_MSG_GCTRL_CLUB_INCOME_DN		= (NET_MSG_GCTRL + 817),	//<	클럽 수익금 업뎃 회신 ( agent->field ).
	NET_MSG_GCTRL_CLUB_INCOME_FB		= (NET_MSG_GCTRL + 818),	//<	클럽 수익금 업뎃 결과 ( field->agent ).

	NET_MSG_GCTRL_CLUB_NOTICE_REQ		= (NET_MSG_GCTRL + 821),	//<	클럽 공지 수정 요청. ( client->agent )
	NET_MSG_GCTRL_CLUB_NOTICE_FB		= (NET_MSG_GCTRL + 822),	//<	클럽 공지 수정 결과. ( agent->client )
	NET_MSG_GCTRL_CLUB_NOTICE_FLD		= (NET_MSG_GCTRL + 823),	//<	클럽 공지 필드에 알림. ( agent->field )
	NET_MSG_GCTRL_CLUB_NOTICE_CLT		= (NET_MSG_GCTRL + 824),	//<	클럽 공지 클라이언트에 알림. ( field->client )

	NET_MSG_GCTRL_CLUB_SUBMASTER		= (NET_MSG_GCTRL + 825),	//<	클럽 부마 설정.
	NET_MSG_GCTRL_CLUB_SUBMASTER_FB		= (NET_MSG_GCTRL + 826),	//<	클럽 부마 설정 FB.
	NET_MSG_GCTRL_CLUB_SUBMASTER_BRD	= (NET_MSG_GCTRL + 827),	//<	클럽 부마 설정 알림 CLT.
	NET_MSG_GCTRL_CLUB_SUBMASTER_FLD	= (NET_MSG_GCTRL + 828),	//<	클럽 부마 설정 알림 FLD.

	NET_MSG_GCTRL_CLUB_ALLIANCE_REQ		= (NET_MSG_GCTRL + 831),	//<	클럽 동맹 요청.
	NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ASK	= (NET_MSG_GCTRL + 832),	//<	클럽 동맹 요청 ASK.
	NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ANS	= (NET_MSG_GCTRL + 833),	//<	클럽 동맹 요청 ANS.
	NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB	= (NET_MSG_GCTRL + 834),	//<	클럽 동맹 요청 결과 FB.

	NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_FLD	= (NET_MSG_GCTRL + 837),	//<	클럽 동맹 클럽 ADD FLD.
	NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_CLT	= (NET_MSG_GCTRL + 838),	//<	클럽 동맹 클럽 ADD CLT.

	NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_REQ	= (NET_MSG_GCTRL + 839),	//<	클럽 동맹 클럽 제명 요청.
	NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB	= (NET_MSG_GCTRL + 840),	//<	클럽 동맹 클럽 제명 요청 FB.

	NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_REQ	= (NET_MSG_GCTRL + 841),	//<	클럽 동맹 탈퇴 요청.
	NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB	= (NET_MSG_GCTRL + 842),	//<	클럽 동맹 탈퇴 요청 FB.

	NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FLD	= (NET_MSG_GCTRL + 843),	//<	클럽 동맹 클럽 DEL FLD.
	NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT	= (NET_MSG_GCTRL + 844),	//<	클럽 동맹 클럽 DEL CLT.

	NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_REQ	= (NET_MSG_GCTRL + 845),	//<	클럽 동맹 해체 FLD.
	NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB	= (NET_MSG_GCTRL + 846),	//<	클럽 동맹 해체 FLD.

	NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FLD	= (NET_MSG_GCTRL + 847),	//<	클럽 동맹 해체 FLD.
	NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT	= (NET_MSG_GCTRL + 848),	//<	클럽 동맹 해체 CLT.


	NET_MSG_GCTRL_CLUB_BATTLE_REQ		= (NET_MSG_GCTRL + 849),	//<	클럽 배틀 요청 
	NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK	= (NET_MSG_GCTRL + 850),	//<	클럽 배틀 요청 ASK.
	NET_MSG_GCTRL_CLUB_BATTLE_REQ_ANS	= (NET_MSG_GCTRL + 851),	//<	클럽 배틀 요청 ANS.
	NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB	= (NET_MSG_GCTRL + 852),	//<	클럽 배틀 요청 결과 FB.

	NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_FLD	= (NET_MSG_GCTRL + 853),	//<	클럽 배틀 시작 FLD.
	NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT	= (NET_MSG_GCTRL + 854),	//<	클럽 배틀 시작 CLT(대기).
	NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2= (NET_MSG_GCTRL + 855),	//<	클럽 배틀 시작 CLT2(진짜시작).
	NET_MSG_GCTRL_CLUB_BATTLE_OVER_FLD	= (NET_MSG_GCTRL + 856),	//<	클럽 배틀 종료 FLD.
	NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT	= (NET_MSG_GCTRL + 857),	//<	클럽 배틀 종료 CLT.

	NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ		= (NET_MSG_GCTRL + 858),	//<	클럽 배틀 휴전 요청.
	NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK	= (NET_MSG_GCTRL + 859),	//<	클럽 배틀 휴전 요청 ASK.
	NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ANS	= (NET_MSG_GCTRL + 860),	//<	클럽 배틀 휴전 요청 ANS.
	NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB	= (NET_MSG_GCTRL + 861),	//<	클럽 배틀 휴전 요청 결과 FB.
	
	NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ	= (NET_MSG_GCTRL + 862),	//<	클럽 배틀 항복 요청.
	NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB	= (NET_MSG_GCTRL + 863),	//<	클럽 배틀 항복 요청 결과 FB.

	NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_AGT	= (NET_MSG_GCTRL + 864),	//<	클럽 배틀 킬수 UPDATE Field -> Agent
	NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_FLD	= (NET_MSG_GCTRL + 865),	//<	클럽 배틀 킬수 UPDATE Agent -> Field
	NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE		= (NET_MSG_GCTRL + 866),	//<	클럽 배틀 킬수 UPDATE 클라이언트

	NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE		= (NET_MSG_GCTRL + 867),	//<	클럽 배틀 킬/데스수 증가

	NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_FLD	= (NET_MSG_GCTRL + 868),	//<	클럽 배틀 종료시 Field정보 조합 Agent -> Field
	NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_AGT	= (NET_MSG_GCTRL + 869),	//<	클럽 배틀 종료시 Field정보 조합 결과 Field -> Agent


	NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ		= (NET_MSG_GCTRL + 870),	//<	동맹 배틀 요청 
	NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK	= (NET_MSG_GCTRL + 871),	//<	동맹 배틀 요청 ASK.
	NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ANS	= (NET_MSG_GCTRL + 872),	//<	동맹 배틀 요청 ANS.
	NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB	= (NET_MSG_GCTRL + 873),	//<	동맹 배틀 요청 결과 FB.

	NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ		= (NET_MSG_GCTRL + 874),	//<	동맹 배틀 휴전 요청.
	NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK	= (NET_MSG_GCTRL + 875),	//<	동맹 배틀 휴전 요청 ASK.
	NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS	= (NET_MSG_GCTRL + 876),	//<	동맹 배틀 휴전 요청 ANS.
	NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB	= (NET_MSG_GCTRL + 877),	//<	동맹 배틀 휴전 요청 결과 FB.

	NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ	= (NET_MSG_GCTRL + 878),	//<	동맹 배틀 항복 요청.
	NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB	= (NET_MSG_GCTRL + 879),	//<	동맹 배틀 항복 요청 결과 FB.

	
	
	
	
	NET_MSG_GCTRL_LEVEL_EVENT_END_FLD		= (NET_MSG_GCTRL + 896),	//<	레벨 이벤트 종료.
	NET_MSG_GCTRL_LEVEL_EVENT_WARNING_FLD	= (NET_MSG_GCTRL + 897),	//< 레벨 이벤트 경고 메세지.
	NET_MSG_GCTRL_LEVEL_EVENT_COUNTDOWN_FLD = (NET_MSG_GCTRL + 898),	//< 레벨 이벤트 카운트 메세지.

	NET_MSG_GM_EVENT_EX					= (NET_MSG_GCTRL + 899),	//<	EX이벤트 시작.
	NET_MSG_GM_EVENT_EX_END				= (NET_MSG_GCTRL + 900),	//<	EX이벤트 종료.

	NET_MSG_GM_EVENT_EXP				= (NET_MSG_GCTRL + 901),	//<	경험치 이벤트 시작.
	NET_MSG_GM_EVENT_EXP_FB				= (NET_MSG_GCTRL + 902),	//<	경험치 이벤트 시작 FB.
	NET_MSG_GM_EVENT_EXP_END			= (NET_MSG_GCTRL + 903),	//<	경험치 이벤트 종료.
	NET_MSG_GM_EVENT_EXP_END_FB			= (NET_MSG_GCTRL + 904),	//<	경험치 이벤트 종료 FB.

	NET_MSG_GM_EVENT_ITEM_GEN			= (NET_MSG_GCTRL + 905),	//<	아이탬 발생율 이벤트.
	NET_MSG_GM_EVENT_ITEM_GEN_FB		= (NET_MSG_GCTRL + 906),	//<	아이탬 발생율 이벤트 FB.

	NET_MSG_GM_EVENT_ITEM_GEN_END		= (NET_MSG_GCTRL + 907),	//<	아이탬 발생율 이벤트 종료.
	NET_MSG_GM_EVENT_ITEM_GEN_END_FB	= (NET_MSG_GCTRL + 908),	//<	아이탬 발생율 이벤트 종료 FB.

	NET_MSG_GM_EVENT_MONEY_GEN			= (NET_MSG_GCTRL + 909),	//<	돈 발생율 이벤트.
	NET_MSG_GM_EVENT_MONEY_GEN_END		= (NET_MSG_GCTRL + 910),	//<	돈 발생율 이벤트 종료.

	NET_MSG_GCTRL_PERIOD				= (NET_MSG_GCTRL + 921),
	NET_MSG_GCTRL_WEATHER				= (NET_MSG_GCTRL + 922),
	NET_MSG_GCTRL_WHIMSICAL				= (NET_MSG_GCTRL + 923),
	NET_MSG_GCTRL_FIELDINFO_RESET		= (NET_MSG_GCTRL + 924),

	NET_MSG_SERVER_CTRL_WEATHER			= (NET_MSG_GCTRL + 931),	//<	서버의 날씨 변경.
	NET_MSG_SERVER_CTRL_TIME			= (NET_MSG_GCTRL + 932),	//<	서버의 시간 변경.
	NET_MSG_SERVER_CTRL_MONTH			= (NET_MSG_GCTRL + 933),	//<	서버의 월 변경.
	NET_MSG_SERVER_CTRL_GENITEMHOLD		= (NET_MSG_GCTRL + 934),	//<	서버의 아이템 독점권 변경.
	NET_MSG_SERVER_PLAYERKILLING_MODE	= (NET_MSG_GCTRL + 935),	//<	player killing on/off.

	NET_MSG_GM_MOVE2GATE				= (NET_MSG_GCTRL + 936),	//<	케릭터를 gate no 로 이동.
	NET_MSG_GM_MOVE2GATE_FLD			= (NET_MSG_GCTRL + 937),	//<	케릭터를 gate no 로 이동 Agent -> Field
	NET_MSG_GM_MOVE2GATE_FB				= (NET_MSG_GCTRL + 938),	//<	케릭터를 gate no 로 이동.
	NET_MSG_GM_KICK_USER				= (NET_MSG_GCTRL + 940),	//<	특정 플레이어를 내보냄.
	NET_MSG_GM_KICK_USER_PROC			= (NET_MSG_GCTRL + 941),	//<	특정 플레이어를 내보냄.
	NET_MSG_GM_KICK_USER_PROC_FLD		= (NET_MSG_GCTRL + 942),    //<  특정 플레이어 강제 접속종료 Agent -> Field
	NET_MSG_GM_MOVE2MAPPOS				= (NET_MSG_GCTRL + 944),	//<	캐릭터를 특정맵의 특정위치로 이동.
	NET_MSG_GM_MOVE2MAPPOS_FLD			= (NET_MSG_GCTRL + 945),	//<	캐릭터를 특정맵의 특정위치로 이동. Agent -> Field

	NET_MSG_BLOCK_DETECTED				= (NET_MSG_GCTRL + 946),	//<	케릭터를 gate no 로 이동.
	NET_MSG_SERVER_GENERALCHAT			= (NET_MSG_GCTRL + 947),	//<	서버 알림 메시지 발생시.

	NET_MSG_GM_WHERE_NPC				= (NET_MSG_GCTRL + 948),	//<	NPC가 어디 위치에 있는지 검사.
	NET_MSG_GM_WHERE_NPC_FLD			= (NET_MSG_GCTRL + 949),	//<	NPC가 어디 위치에 있는지 검사. Agent -> Field
	NET_MSG_GM_WHERE_NPC_FB				= (NET_MSG_GCTRL + 950),	//<	NPC가 어디 위치에 있는지 검사.

	NET_MSG_GM_WHERE_PC_MAP				= (NET_MSG_GCTRL + 951),	//<	PC가 어느 멥에 있는지 검사.
	NET_MSG_GM_WHERE_PC_MAP_FB			= (NET_MSG_GCTRL + 952),	//<	PC가 어느 멥에 있는지 검사 FB.

	NET_MSG_GM_WHERE_PC_POS				= (NET_MSG_GCTRL + 953),	//<	PC가 어디 위치에 있는지 검사.
	NET_MSG_GM_WHERE_PC_POS_FB			= (NET_MSG_GCTRL + 954),	//<	PC가 어디 위치에 있는지 검사 FB.

	NET_MSG_GM_MOVE2CHAR				= (NET_MSG_GCTRL + 955),	//<	캐릭터를 특정캐릭터로 이동 요청.
	NET_MSG_GM_MOVE2CHAR_POS			= (NET_MSG_GCTRL + 956),	//<	캐릭터를 특정캐릭터로 이동 위치 확인.
	NET_MSG_GM_MOVE2CHAR_AG				= (NET_MSG_GCTRL + 957),	//<	캐릭터를 특정캐릭터로 이동 위치 확인 응답.
	NET_MSG_GM_MOVE2CHAR_FLD			= (NET_MSG_GCTRL + 958),	//<	캐릭터를 특정캐릭터로 이동 요청 실행.
	NET_MSG_GM_MOVE2CHAR_FB				= (NET_MSG_GCTRL + 959),	//<	캐릭터를 특정캐릭터로 이동 실행 결과.

	NET_MSG_GM_BIGHEAD					= (NET_MSG_GCTRL + 961),	//<	BIG HEAD 모드 on/off.
	NET_MSG_GM_BIGHEAD_BRD				= (NET_MSG_GCTRL + 962),	//<	BIG HEAD 모드 on/off.

	NET_MSG_GM_BIGHAND					= (NET_MSG_GCTRL + 963),	//<	BIG HAND 모드 on/off.
	NET_MSG_GM_BIGHAND_BRD				= (NET_MSG_GCTRL + 964),	//<	BIG HAND 모드 on/off.

	NET_MSG_GM_MOB_GEN					= (NET_MSG_GCTRL + 965),	//<	몹 발생.
	NET_MSG_GM_MOB_GEN_FLD				= (NET_MSG_GCTRL + 966),	//<	몹 발생. Agent->Field
	NET_MSG_GM_MOB_DEL					= (NET_MSG_GCTRL + 967),	//<	몹 제거.
	NET_MSG_GM_MOB_DEL_FLD				= (NET_MSG_GCTRL + 968),	//<	몹 제거. Agent->Field
	NET_MSG_GM_MOB_LEVEL				= (NET_MSG_GCTRL + 969),	//<	몹(LEVEL FILE) 발생.
	NET_MSG_GM_MOB_LEVEL_CLEAR			= (NET_MSG_GCTRL + 970),	//<	몹(LEVEL FILE) 발생 제거.

	NET_MSG_GM_CHAT_BLOCK_UACCOUNT		= (NET_MSG_GCTRL + 971),	//<	채팅 블록 (유저이름)
	NET_MSG_GM_CHAT_BLOCK_CHARNAME		= (NET_MSG_GCTRL + 972),	//<	채팅 블록 (캐릭이름)
	NET_MSG_GM_CHAT_BLOCK_CHARID		= (NET_MSG_GCTRL + 973),	//<	채팅 블록 (캐릭번호)
	NET_MSG_GM_CHAT_BLOCK_FB			= (NET_MSG_GCTRL + 974),	//<	채팅 블록 FB
	NET_MSG_GM_CHAT_BLOCK_FLD			= (NET_MSG_GCTRL + 975),	//<	채팅 블록 FLD

	NET_MSG_USER_CHAR_INFO_4NAME		= (NET_MSG_GCTRL + 977),	//< 캐릭터의 정보 출력 (유저명령어)
	NET_MSG_USER_CHAR_INFO_AGT_FB		= (NET_MSG_GCTRL + 978),	//<	캐릭터의 정보 출력 agt FB.
	NET_MSG_USER_CHAR_INFO_FLD_FB		= (NET_MSG_GCTRL + 979),	//<	캐릭터의 정보 출력 fld FB.
	NET_MSG_USER_CHAR_INFO_FLD			= (NET_MSG_GCTRL + 980),	//<	캐릭터의 정보 출력 FLD.

	NET_MSG_GM_CHAR_INFO_4NAME			= (NET_MSG_GCTRL + 981),	//<	캐릭터의 정보 출력 (캐릭이름).
	NET_MSG_GM_CHAR_INFO_4CHARID		= (NET_MSG_GCTRL + 982),	//<	캐릭터의 정보 출력 (캐릭번호).
	NET_MSG_GM_CHAR_INFO_AGT_FB			= (NET_MSG_GCTRL + 983),	//<	캐릭터의 정보 출력 agt FB.
	NET_MSG_GM_CHAR_INFO_FLD_FB			= (NET_MSG_GCTRL + 984),	//<	캐릭터의 정보 출력 fld FB.
	NET_MSG_GM_CHAR_INFO_FLD			= (NET_MSG_GCTRL + 985),	//<	캐릭터의 정보 출력 FLD.

   
//-----------------------------------------------------------------------------------------------------------

	NET_MSG_GM_FREEPK					  = (NET_MSG_GCTRL + 986),	//<	자유 피케이
	NET_MSG_GM_FREEPK_BRD				  = (NET_MSG_GCTRL + 987),	//<	자유 피케이

	NET_MSG_GM_WARNING_MSG				  = (NET_MSG_GCTRL + 989),	//<	자유 피케이
	NET_MSG_GM_WARNING_MSG_FLD			  = (NET_MSG_GCTRL + 990),	//<	자유 피케이
	NET_MSG_GM_WARNING_MSG_BRD			  = (NET_MSG_GCTRL + 991),	//<	자유 피케이

	NET_MSG_GM_VIEWALLPLAYER			  = (NET_MSG_GCTRL + 992),	//<  맵의 모든 플레이어 표시
	NET_MSG_GM_VIEWALLPLAYER_FLD_REQ	  = (NET_MSG_GCTRL + 993),	//<  필드서버에 요청
	NET_MSG_GM_VIEWALLPLAYER_FLD_FB 	  = (NET_MSG_GCTRL + 994),	//<  필드서버 처리결과 회신

	NET_MSG_GM_GENCHAR					  = (NET_MSG_GCTRL + 995),	//<  케릭터 소환 요청
	NET_MSG_GM_GENCHAR_POS				  = (NET_MSG_GCTRL + 996),	//<  케릭터 소환 요청
	NET_MSG_GM_GENCHAR_AG				  = (NET_MSG_GCTRL + 997),	//<  케릭터 소환 요청
	NET_MSG_GM_GENCHAR_FLD				  = (NET_MSG_GCTRL + 998),	//<  케릭터 소환 요청
	NET_MSG_GM_GENCHAR_FB				  = (NET_MSG_GCTRL + 999),	//<  케릭터 소환 요청

	NET_MSG_GCTRL_SERVER_BRIGHTEVENT_INFO = (NET_MSG_GCTRL + 1001),	// 성향이벤트
	NET_MSG_GCTRL_SERVER_CLUB_BATTLE_INFO = (NET_MSG_GCTRL + 1002),	// 선도전 시작 유무
	NET_MSG_GCTRL_REQ_SERVER_CLUB_BATTLE_REMAIN_AG = (NET_MSG_GCTRL + 1003), // 선도전 남은시간 요청(Field->Agent->Client)

	NET_MSG_GCTRL_UPDATE_LASTCALL		= (NET_MSG_GCTRL + 1010),	// 직전 귀환 위치 클라이언트로 전송
	NET_MSG_GCTRL_UPDATE_STARTCALL		= (NET_MSG_GCTRL + 1011),	// 시작 귀환 위치 클라이언트로 전송

	NET_MSG_GM_COUNTDOWN_MSG_BRD		= (NET_MSG_GCTRL + 1020),	// 자동레벨 카운트 메세지
	NET_MSG_GM_SHOWMETHEMONEY			= (NET_MSG_GCTRL + 1021),	//<  캐릭터로 돈 저장
	NET_MSG_GM_SHOWMETHEMONEY_FLD		= (NET_MSG_GCTRL + 1022),	//<  캐릭터로 돈 저장

//-----------------------------------------------------------------------------------------------------------

	NET_MSG_REBUILD_RESULT				= (NET_MSG_GCTRL + 1025),	// 아이템 개조 - 시작, 종료 결과	// ITEMREBUILD_MARK
	NET_MSG_REBUILD_MOVE_ITEM			= (NET_MSG_GCTRL + 1026),	// 아이템 개조 - 아이템 이동
	NET_MSG_REBUILD_COST_MONEY			= (NET_MSG_GCTRL + 1027),	// 아이템 개조 - 개조 비용
	NET_MSG_REBUILD_INPUT_MONEY			= (NET_MSG_GCTRL + 1028),	// 아이템 개조 - 유저 입력 금액

//-----------------------------------------------------------------------------------------------------------

	NET_MSG_PET_DROPPET					= (NET_MSG_GCTRL + 1030),	// ANYPET 생성
	NET_MSG_CREATE_ANYPET				= (NET_MSG_GCTRL + 1031),	// ANYPET 생성
	NET_MSG_PET_REQ_USECARD				= (NET_MSG_GCTRL + 1032),	// 팻카드 사용
	NET_MSG_PET_REQ_USECARD_FB			= (NET_MSG_GCTRL + 1033),
	NET_MSG_PET_REQ_UNUSECARD			= (NET_MSG_GCTRL + 1034),	// 팻 소멸
	NET_MSG_PET_REQ_UNUSECARD_FB		= (NET_MSG_GCTRL + 1035),
	NET_MSG_PET_REQ_DISAPPEAR			= (NET_MSG_GCTRL + 1036),	// 강제로 소멸(맵이동시, 게임종료등...)

	NET_MSG_CREATE_PET_FROMDB_FB		= (NET_MSG_GCTRL + 1037),	// 팻생성(DB)
	NET_MSG_GET_PET_FROMDB_FB			= (NET_MSG_GCTRL + 1038),	// 팻생성(DB)
	NET_MSG_GET_PET_FROMDB_ERROR		= (NET_MSG_GCTRL + 1039),	// 팻생성(DB) 에러

	NET_MSG_PET_REQ_UPDATE_MOVE_STATE	 = (NET_MSG_GCTRL + 1040),  // 움직임 상태 변경
	NET_MSG_PET_REQ_UPDATE_MOVE_STATE_FB = (NET_MSG_GCTRL + 1041),
	NET_MSG_PET_REQ_UPDATE_MOVE_STATE_BRD = (NET_MSG_GCTRL + 1042),

	NET_MSG_PET_REQ_SKILLCHANGE			= (NET_MSG_GCTRL + 1143),	// 스킬변경
	NET_MSG_PET_REQ_SKILLCHANGE_FB		= (NET_MSG_GCTRL + 1144),	// 스킬변경
	NET_MSG_PET_REQ_SKILLCHANGE_BRD		= (NET_MSG_GCTRL + 1145),	// 스킬변경

	NET_MSG_PET_ADD_SKILLFACT			= (NET_MSG_GCTRL + 1146),	// SkillFact 추가
	NET_MSG_PET_REMOVE_SKILLFACT		= (NET_MSG_GCTRL + 1147),	// SkillFact 추가

	NET_MSG_PET_REQ_GOTO				= (NET_MSG_GCTRL + 1053),	// 이동처리
	NET_MSG_PET_REQ_STOP				= (NET_MSG_GCTRL + 1054),	// 이동처리
	NET_MSG_PET_REQ_GOTO_FB				= (NET_MSG_GCTRL + 1055),	// 이동처리
	NET_MSG_PET_GOTO_BRD				= (NET_MSG_GCTRL + 1056),	// 이동처리
	NET_MSG_PET_STOP_BRD				= (NET_MSG_GCTRL + 1057),	// 이동처리

	NET_MSG_PET_REQ_GIVEFOOD			= (NET_MSG_GCTRL + 1058),	// 사료주기
	NET_MSG_PET_REQ_GIVEFOOD_FB			= (NET_MSG_GCTRL + 1059),	// 사료주기
	NET_MSG_PET_REQ_GETFULL_FROMDB_FB	= (NET_MSG_GCTRL + 1060),	// 사료주기

	NET_MSG_PET_GETRIGHTOFITEM			= (NET_MSG_GCTRL + 1070),	// 소유권한이 있는 아이템정보 요청
	NET_MSG_PET_GETRIGHTOFITEM_FB		= (NET_MSG_GCTRL + 1071),	// 소유권한이 있는 아이템정보 요청
	NET_MSG_PET_NOTENOUGHINVEN			= (NET_MSG_GCTRL + 1072),	// 인벤토리가 부족하여 팻을 사라지게 함

	NET_MSG_PET_REQ_RENAME				= (NET_MSG_GCTRL + 1075),	// 이름변경
	NET_MSG_PET_REQ_RENAME_FB			= (NET_MSG_GCTRL + 1076),	// 이름변경
	NET_MSG_PET_REQ_RENAME_BRD			= (NET_MSG_GCTRL + 1077),	// 이름변경
	NET_MSG_PET_REQ_RENAME_FROMDB_FB	= (NET_MSG_GCTRL + 1078),	// 이름변경

	NET_MSG_PET_REQ_CHANGE_COLOR		= (NET_MSG_GCTRL + 1080),	// 컬러변경
	NET_MSG_PET_REQ_CHANGE_COLOR_FB		= (NET_MSG_GCTRL + 1081),	// 컬러변경
	NET_MSG_PET_REQ_CHANGE_COLOR_BRD	= (NET_MSG_GCTRL + 1082),	// 컬러변경

	NET_MSG_PET_REQ_CHANGE_STYLE		= (NET_MSG_GCTRL + 1085),	// 스타일변경
	NET_MSG_PET_REQ_CHANGE_STYLE_FB		= (NET_MSG_GCTRL + 1086),	// 스타일변경
	NET_MSG_PET_REQ_CHANGE_STYLE_BRD	= (NET_MSG_GCTRL + 1087),	// 스타일변경

	// 클라이언트 상태 변경 (Field -> Client)
	NET_MSG_PET_UPDATECLIENT_FULL		= (NET_MSG_GCTRL + 1090),	// 팻포만감 갱신

	NET_MSG_PET_REQ_SLOT_EX_HOLD		= (NET_MSG_GCTRL + 1100),	// 아이템 탈착(SLOT<->HOLD)
	NET_MSG_PET_REQ_SLOT_EX_HOLD_FB		= (NET_MSG_GCTRL + 1101),	// 아이템 탈착(SLOT<->HOLD)
	NET_MSG_PET_REQ_SLOT_EX_HOLD_BRD	= (NET_MSG_GCTRL + 1102),	// 아이템 탈착(SLOT<->HOLD)
	NET_MSG_PET_REQ_SLOT_TO_HOLD		= (NET_MSG_GCTRL + 1103),	// 아이템 탈착(SLOT->HOLD)
	NET_MSG_PET_REQ_SLOT_TO_HOLD_FB		= (NET_MSG_GCTRL + 1104),	// 아이템 탈착(SLOT->HOLD)
	NET_MSG_PET_REQ_SLOT_TO_HOLD_BRD	= (NET_MSG_GCTRL + 1105),	// 아이템 탈착(SLOT->HOLD)
	NET_MSG_PET_REQ_HOLD_TO_SLOT		= (NET_MSG_GCTRL + 1106),	// 아이템 탈착(HOLD->SLOT)
	NET_MSG_PET_REQ_HOLD_TO_SLOT_FB		= (NET_MSG_GCTRL + 1107),	// 아이템 탈착(HOLD->SLOT)
	NET_MSG_PET_REQ_HOLD_TO_SLOT_BRD	= (NET_MSG_GCTRL + 1108),	// 아이템 탈착(HOLD->SLOT)

	NET_MSG_PET_REQ_LEARNSKILL			= (NET_MSG_GCTRL + 1110),	// 팻스킬 배우기
	NET_MSG_PET_REQ_LEARNSKILL_FB		= (NET_MSG_GCTRL + 1111),	// 팻스킬 배우기
	NET_MSG_PET_REQ_LEARNSKILL_BRD		= (NET_MSG_GCTRL + 1112),	// 팻스킬 배우기

	NET_MSG_PET_REQ_FUNNY				= (NET_MSG_GCTRL + 1115),	// Funny Gesture
	NET_MSG_PET_REQ_FUNNY_BRD			= (NET_MSG_GCTRL + 1116),	// Funny Gesture

	NET_MSG_PET_REMOVE_SLOTITEM			= (NET_MSG_GCTRL + 1118),	// 팻슬롯아이템 제거
	NET_MSG_PET_REMOVE_SLOTITEM_FB		= (NET_MSG_GCTRL + 1119),	// 팻슬롯아이템 제거
	NET_MSG_PET_REMOVE_SLOTITEM_BRD		= (NET_MSG_GCTRL + 1120),	// 팻슬롯아이템 제거

	NET_MSG_PET_ATTACK					= (NET_MSG_GCTRL + 1125),	// 공격모션
	NET_MSG_PET_ATTACK_BRD				= (NET_MSG_GCTRL + 1126),	// 공격모션

	NET_MSG_PET_SAD						= (NET_MSG_GCTRL + 1127),	// Sad모션
	NET_MSG_PET_SAD_BRD					= (NET_MSG_GCTRL + 1128),	// Sad모션

	NET_MSG_PET_REQ_PETCARDINFO			= (NET_MSG_GCTRL + 1131),	// 팻카드 정보
	NET_MSG_PET_REQ_PETCARDINFO_FB		= (NET_MSG_GCTRL + 1132),	// 팻카드 정보

	NET_MSG_PET_REQ_PETREVIVEINFO		= (NET_MSG_GCTRL + 1133),	// 팻부활 정보
	NET_MSG_PET_REQ_PETREVIVEINFO_FB	= (NET_MSG_GCTRL + 1134),	// 팻부활 정보

	NET_MSG_PET_REQ_REVIVE				= (NET_MSG_GCTRL + 1135),	// 삭제된 팻 부활
	NET_MSG_PET_REQ_REVIVE_FB			= (NET_MSG_GCTRL + 1136),	// 삭제된 팻 부활
	NET_MSG_PET_REQ_REVIVE_FROMDB_FB	= (NET_MSG_GCTRL + 1137),	// 삭제된 팻 부활

	NET_MSG_PET_ACCESSORY_DELETE		= (NET_MSG_GCTRL + 1140),	// 팻악세사리 삭제
	NET_MSG_PET_ACCESSORY_DELETE_BRD	= (NET_MSG_GCTRL + 1141),	// 팻악세사리 삭제

	NET_MSG_SMS_PHONE_NUMBER			= (NET_MSG_GCTRL + 1200),	// 캐릭터 폰번호 저장/업데이트 요청
	NET_MSG_SMS_PHONE_NUMBER_FB			= (NET_MSG_GCTRL + 1201),	//<
	NET_MSG_SMS_PHONE_NUMBER_FROM_DB	= (NET_MSG_GCTRL + 1202),	//<	DB 저장 결과 FB.
	NET_MSG_SMS_PHONE_NUMBER_AGTBRD		= (NET_MSG_GCTRL + 1203),	//<	Agent에 모든채널 BRD 요청.

	NET_MSG_SMS_SEND					= (NET_MSG_GCTRL + 1204),
	NET_MSG_SMS_SEND_FB					= (NET_MSG_GCTRL + 1205),
	NET_MSG_SMS_SEND_FROM_DB			= (NET_MSG_GCTRL + 1206),

	NET_MSG_GM_MOB_GEN_EX				= (NET_MSG_GCTRL + 1210),   // 몹소환(확장기능추가)
	NET_MSG_GM_MOB_GEN_EX_FLD			= (NET_MSG_GCTRL + 1211),   // 몹소환(확장기능추가)
    NET_MSG_GM_MOB_DEL_EX				= (NET_MSG_GCTRL + 1212),   // 몹제거(확장기능추가)
	NET_MSG_GM_MOB_DEL_EX_FLD			= (NET_MSG_GCTRL + 1213),   // 몹제거(확장기능추가)

	NET_MSG_MGAME_ODDEVEN				= (NET_MSG_GCTRL + 1220),	// 미니 게임 - 홀짝(클라이언트->필드서버)
	NET_MSG_MGAME_ODDEVEN_FB			= (NET_MSG_GCTRL + 1221),	// 미니 게임 - 홀짝(필드서버->클라이언트)	

	NET_MSG_SERVERTIME_BRD				= (NET_MSG_GCTRL + 1230),	// 서버 현재시간 BroadCasting

	NET_MSG_GCTRL_SET_SERVER_DELAYTIME     = (NET_MSG_GCTRL + 1240),	// 서버 장애1
	NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FB  = (NET_MSG_GCTRL + 1241),	// 서버 장애1
	NET_MSG_GCTRL_SET_SERVER_DELAYTIME_AGT = (NET_MSG_GCTRL + 1242),	// 서버 장애1
	NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FLD = (NET_MSG_GCTRL + 1243),	// 서버 장애1
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET    = (NET_MSG_GCTRL + 1244),	// 서버 장애2
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FB = (NET_MSG_GCTRL + 1245),	// 서버 장애2
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_AGT = (NET_MSG_GCTRL + 1246),	// 서버 장애2
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FLD = (NET_MSG_GCTRL + 1247),	// 서버 장애2
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME     = (NET_MSG_GCTRL + 1248),	// 서버 장애3
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FB  = (NET_MSG_GCTRL + 1249),	// 서버 장애3
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME_AGT = (NET_MSG_GCTRL + 1250),	// 서버 장애3
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FLD = (NET_MSG_GCTRL + 1251),	// 서버 장애3

	NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_AGT = (NET_MSG_GCTRL + 1255),	//로그인서버 MainTenance기능 On/Off 

	NET_MSG_GCTRL_STORAGE_ITEM_UPDATE	= (NET_MSG_GCTRL + 1260 ),	//< 락커 아이템 업데이트
	NET_MSG_REQ_FRIEND_CLUB_OPEN		= (NET_MSG_GCTRL + 1270 ),	//< 친구,클럽창 열림(클라이언트->에이젼트)

	NET_MSG_GM_GETWHISPERMSG		    = (NET_MSG_GCTRL + 1402),	//<	캐릭터의 귓속말을 엿듣는다.
	NET_MSG_CHINA_GAINTYPE  		    = (NET_MSG_GCTRL + 1403),	//<	중국 등급별 수익

	NET_MSG_GM_LIMIT_EVENT_BEGIN		= (NET_MSG_GCTRL + 1404),	//<	이벤트 시작.
	NET_MSG_GM_LIMIT_EVENT_END			= (NET_MSG_GCTRL + 1405),	//<	이벤트 종료.

	NET_MSG_GM_LIMIT_EVENT_RESTART		= (NET_MSG_GCTRL + 1406),	//<	이벤트 다시 시작.

	NET_MSG_GM_LIMIT_EVENT_APPLY_START	= (NET_MSG_GCTRL + 1407),	//<	클라이언트에 이벤트 적용 시작했음을 알림
	NET_MSG_GM_LIMIT_EVENT_APPLY_END 	= (NET_MSG_GCTRL + 1408),	//<	클라이언트에 이벤트 적용 끝났음을 알림

	NET_MSG_GM_LIMIT_EVENT_BEGIN_FB		= (NET_MSG_GCTRL + 1409),   //<	클라이언트에 이벤트가 시작 되었음을 알림
	NET_MSG_GM_LIMIT_EVENT_END_FB		= (NET_MSG_GCTRL + 1410),   //<	클라이언트에 이벤트가 종료 되었음을 알림

	NET_MSG_GM_LIMIT_EVENT_TIME_REQ		= (NET_MSG_GCTRL + 1411),   //<	클라이언트가 서버에 이벤트 정보를 요청
	NET_MSG_GM_LIMIT_EVENT_TIME_REQ_FB	= (NET_MSG_GCTRL + 1412),   //<	서버가 클라이언트에 이벤트 정보를 보냄
	NET_MSG_GM_LIMIT_EVENT_TIME_RESET	= (NET_MSG_GCTRL + 1413),   //<	AGENT서버가 FIELD서버에게 로그인 시간을 RESET하라고 보냄

	NET_MSG_GM_PRINT_CROWLIST			= (NET_MSG_GCTRL + 1414),   //<	자신이 있는 맵의 몹 리스트를 프린트함.
	NET_MSG_GM_PRINT_CROWLIST_FLD		= (NET_MSG_GCTRL + 1415),   //<	자신이 있는 맵의 몹 리스트를 프린트함.
	NET_MSG_GM_PRINT_CROWLIST_FB		= (NET_MSG_GCTRL + 1416),   //<	자신이 있는 맵의 몹 리스트를 프린트함.

	NET_MSG_SERVER_CTRL_WEATHER2		= (NET_MSG_GCTRL + 1417),	//<	서버의 날씨 변경. 새로운 메시지
	NET_MSG_GCTRL_MAPWEATHER			= (NET_MSG_GCTRL + 1418),	
	NET_MSG_GCTRL_MAPWHIMSICAL			= (NET_MSG_GCTRL + 1419),

	NET_MSG_GCTRL_ACTIVE_VEHICLE			= (NET_MSG_GCTRL + 1420),	//< 클라이언트가 탈것을 활성/비활성 요청
	NET_MSG_GCTRL_ACTIVE_VEHICLE_FB			= (NET_MSG_GCTRL + 1421),	//< 클라이언트에 탈것을 활성/비활성 알림
	NET_MSG_GCTRL_ACTIVE_VEHICLE_BRD		= (NET_MSG_GCTRL + 1422),	//< 주위 클라이언트에 탈것을 활성/비활성 알림

	NET_MSG_GCTRL_GET_VEHICLE				= (NET_MSG_GCTRL + 1423),	//< 클라이언트가 탈것의 정보 요청
	NET_MSG_GCTRL_GET_VEHICLE_FB			= (NET_MSG_GCTRL + 1424),	//< 클라이언트에 탈것의 정보 받음
	NET_MSG_GCTRL_GET_VEHICLE_BRD			= (NET_MSG_GCTRL + 1425),	//< 주위 클라이언트에 탈것 정보 전송
	NET_MSG_GCTRL_UNGET_VEHICLE_FB			= (NET_MSG_GCTRL + 1426),	//< 탈것 정보 비활성화
	NET_MSG_GCTRL_UNGET_VEHICLE_BRD			= (NET_MSG_GCTRL + 1427),	//< 탈것 정보 비활성화

	NET_MSG_VEHICLE_CREATE_FROMDB_FB		= (NET_MSG_GCTRL + 1428),	//< 최초 탈것 생성 ( DB ) 
	NET_MSG_VEHICLE_GET_FROMDB_FB			= (NET_MSG_GCTRL + 1429),	//< 탈것 생성 ( DB )
	NET_MSG_VEHICLE_GET_FROMDB_ERROR		= (NET_MSG_GCTRL + 1430),	//< 탈것 생성 ( DB에러 )
	
	NET_MSG_VEHICLE_ACCESSORY_DELETE		= (NET_MSG_GCTRL + 1431),	//< 탈것의 악세서리 삭제
	NET_MSG_VEHICLE_ACCESSORY_DELETE_BRD	= (NET_MSG_GCTRL + 1432),	// 탈것의 악세사리 삭제 알림
	
	NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD		= (NET_MSG_GCTRL + 1433 ),	// 탈것 slot ex hold
	NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_FB		= (NET_MSG_GCTRL + 1434 ),	// 탈것 slot ex hold
	NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_BRD	= (NET_MSG_GCTRL + 1435 ),	// 탈것 slot ex hold
	
	NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT		= (NET_MSG_GCTRL + 1436 ),	// 탈것 hold to slot
	NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT_FB		= (NET_MSG_GCTRL + 1437 ),	// 탈것 hold to slot
	
	NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD		= (NET_MSG_GCTRL + 1438 ),	// 탈것 slot to hold
	NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD_FB		= (NET_MSG_GCTRL + 1439 ),	// 탈것 slot to hold
	
	NET_MSG_VEHICLE_REMOVE_SLOTITEM			= (NET_MSG_GCTRL + 1440 ),	// item slot 해제
	NET_MSG_VEHICLE_REMOVE_SLOTITEM_FB		= (NET_MSG_GCTRL + 1441 ),	// item slot 해제
	NET_MSG_VEHICLE_REMOVE_SLOTITEM_BRD		= (NET_MSG_GCTRL + 1442 ),	// item slot 해제

	NET_MSG_VEHICLE_REQ_GIVE_BATTERY		= (NET_MSG_GCTRL + 1443 ),	// 배터리 충전
	NET_MSG_VEHICLE_REQ_GIVE_BATTERY_FB		= (NET_MSG_GCTRL + 1444 ),	// 배터리 충전
	NET_MSG_VEHICLE_REQ_GETFULL_FROMDB_FB	= (NET_MSG_GCTRL + 1445 ),	// 배터리 충전
	NET_MSG_VEHICLE_UPDATE_CLIENT_BATTERY	= (NET_MSG_GCTRL + 1446 ),	// 배터리 갱신
	NET_MSG_VEHICLE_REQ_GET_BATTERY_FROMDB_FB	= (NET_MSG_GCTRL + 1447 ),	// 배터리 가져오기 ( 내부 )

	NET_MSG_VEHICLE_REQ_ITEM_INFO			= (NET_MSG_GCTRL + 1448),	// 탈것아이템 정보
	NET_MSG_VEHICLE_REQ_ITEM_INFO_FB		= (NET_MSG_GCTRL + 1449),	// 탈것아이템 정보

	NET_MSG_VIETNAM_GAINTYPE				= (NET_MSG_GCTRL + 1450),	// 베트남 탐닉 방지 시스템 현재 수익 타입
	NET_MSG_VIETNAM_TIME_REQ				= (NET_MSG_GCTRL + 1451),	// 베트남 탐닉 방지 시스템 시간 요청
	NET_MSG_VIETNAM_TIME_REQ_FB				= (NET_MSG_GCTRL + 1452),	// 베트남 탐닉 방지 시스템 시간 요청
	NET_MSG_VIETNAM_GAIN_EXP				= (NET_MSG_GCTRL + 1453),	// 베트남 탐닉 방지 시스템 경험치
	NET_MSG_VIETNAM_GAIN_MONEY				= (NET_MSG_GCTRL + 1454),	// 베트남 탐닉 방지 시스템 돈
	NET_MSG_GCTRL_REQ_VNGAIN_TO_HOLD		= (NET_MSG_GCTRL + 1455),   //<	탐닉방지 인밴에 있는 아이탬을 들기 시도.
	NET_MSG_GCTRL_REQ_VNGAIN_EX_HOLD		= (NET_MSG_GCTRL + 1456),	//<	탐닉방지 인밴에 있는 아이탬을 들기 시도.
	NET_MSG_GCTRL_REQ_HOLD_TO_VNGAIN		= (NET_MSG_GCTRL + 1457),	//<	아이템을 탐닉방지 인밴에 넣음
	NET_MSG_GCTRL_REQ_VNGAIN_INVEN_RESET	= (NET_MSG_GCTRL + 1458),	//< 탐닉 방지 인밴 전체 초기화
	NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN      = (NET_MSG_GCTRL + 1459),	//< 오른쪽 버튼으로 탐닉 방지 인밴에서 인밴으로 아이템 옮길때
	NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN_FB   = (NET_MSG_GCTRL + 1460),	//< 오른쪽 버튼으로 탐닉 방지 인밴에서 인밴으로 아이템 옮길때
	NET_MSG_GCTRL_INVEN_VIETNAM_INVENGET	= (NET_MSG_GCTRL + 1461),	//<	베트남 탐닉 방지 아이템 사용시 메시지
	NET_MSG_GCTRL_INVEN_VIETNAM_EXPGET_FB	= (NET_MSG_GCTRL + 1462),	//<	베트남 탐닉 방지 아이템 사용시 메시지
	NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGET_FB	= (NET_MSG_GCTRL + 1463),	//<	베트남 탐닉 방지 아이템 사용시 메시지
	NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGETNUM_UPDATE = (NET_MSG_GCTRL + 1464),	//<	베트남 탐닉 방지 아이템 사용시 메시지 
	NET_MSG_VIETNAM_ALLINITTIME				= (NET_MSG_GCTRL + 1465),	//<	베트남 탐닉 방지시스템 12시가 지나서 초기화 되었을때
	
	NET_MSG_GCTRL_INVEN_GENDER_CHANGE		= (NET_MSG_GCTRL + 1466),	// 성별 변경
	NET_MSG_GCTRL_INVEN_GENDER_CHANGE_FB	= (NET_MSG_GCTRL + 1467),	// 성별 변경

	NET_MSG_MARKET_CHECKTIME				= (NET_MSG_GCTRL + 1468),	// 상점이 열려있을 때 10분마다 메시지를 보내 튕김 현상을 방지한다.

	NET_MSG_CHAT_CTRL_FB					= (NET_MSG_GCTRL + 1469), //< 관리자용 채팅메시지 정의 ( 게임서버->클라이언트 )

	NET_NON_REBIRTH_REQ						= (NET_MSG_GCTRL + 1470), //< 부활 스킬 금지 사용 여부

	NET_MSG_REQ_SKILL_REVIVEL_FAILED		= (NET_MSG_GCTRL + 1471), //< 부활 스킬 금지 되었을 경우 당사자에게 메시지를 보낸다.

	NET_QBOX_OPTION_REQ_AG					= (NET_MSG_GCTRL + 1472), //< QBox On/Off 옵션 마스터 -> 에이전트 서버
	NET_QBOX_OPTION_REQ_FLD					= (NET_MSG_GCTRL + 1473), //< QBox On/Off 옵션 에이전트 서버 -> 필드서버
	NET_QBOX_OPTION_MEMBER					= (NET_MSG_GCTRL + 1474), //< QBox On/Off 옵션 필드 서버 -> 멤버에게..

	NET_MSG_UPDATE_TRACING_ALL				= (NET_MSG_GCTRL + 1475), //< 추적할 캐릭터를 전부 업데이트한다.
	NET_MSG_UPDATE_TRACING_CHARACTER		= (NET_MSG_GCTRL + 1476), //< 추적할 캐릭터를 특정 캐릭터만 업데이트한다.
	NET_MSG_LOG_UPDATE_TRACING_CHARACTER	= (NET_MSG_GCTRL + 1477), //< // 추적중인 유저의 로그를 agent서버에 보낸다.

	NET_MSG_TRACING_CHARACTER				= (NET_MSG_GCTRL + 1478), //< 필드->클라이언트 추적 유저인지 아닌지를 보낸다.

	NET_MSG_GCTRL_REQ_TELEPORT				= (NET_MSG_GCTRL + 1479),	//<	소환서 아이템 사용 요청.
	NET_MSG_GCTRL_REQ_TELEPORT_FB			= (NET_MSG_GCTRL + 1480),	//<	소환서 아이템 사용 응답.
	NET_MSG_GCTRL_REQ_TELEPORT_AG			= (NET_MSG_GCTRL + 1481),	//<	소환서 에이젼트 서버에 요청.

	NET_MSG_GCTRL_GETEXP_RECOVERY			= (NET_MSG_GCTRL + 1482),	// 경험치 회복 데이터 요청
	NET_MSG_GCTRL_GETEXP_RECOVERY_FB		= (NET_MSG_GCTRL + 1483),	// 경험치 회복 데이터 요청 FB
	NET_MSG_GCTRL_RECOVERY					= (NET_MSG_GCTRL + 1484),	// 경험치 회복 사용
	NET_MSG_GCTRL_RECOVERY_FB				= (NET_MSG_GCTRL + 1485),	// 경험치 회복 사용 FB
	NET_MSG_GCTRL_GETEXP_RECOVERY_NPC		= (NET_MSG_GCTRL + 1486),	// 경험치 회복 데이터 요청 (NPC)
	NET_MSG_GCTRL_GETEXP_RECOVERY_NPC_FB	= (NET_MSG_GCTRL + 1487),	// 경험치 회복 데이터 요청 FB (NPC)
	NET_MSG_GCTRL_RECOVERY_NPC				= (NET_MSG_GCTRL + 1488),	// 경험치 회복 사용 (NPC)
	NET_MSG_GCTRL_RECOVERY_NPC_FB			= (NET_MSG_GCTRL + 1489),	// 경험치 회복 사용 FB (NPC)
	NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP		= (NET_MSG_GCTRL + 1490),	//<	퀘스트 맵이동 제한.
	NET_MSG_GM_CLASS_EVENT					= (NET_MSG_GCTRL + 1491),	//<	클래스별 이벤트

	NET_MSG_VIETNAM_TIME_RESET				= (NET_MSG_GCTRL + 1492),	// 베트남 시간 리셋 //vietnamtest%%% && vietnamtest2

	NET_MSG_GCTRL_ITEMSHOPOPEN				= (NET_MSG_GCTRL + 1493),	//<	ItemShopOpen ( Japan ) 
	NET_MSG_GCTRL_ITEMSHOPOPEN_BRD			= (NET_MSG_GCTRL + 1494),	//<	ItemShopOpen ( Japan ) 

	NET_MSG_GM_VIEWWORKEVENT			    = (NET_MSG_GCTRL + 1495),	//< 현재 agent서버에 적용된 모든 이벤트 보기
	NET_MSG_GM_VIEWWORKEVENT_FB			    = (NET_MSG_GCTRL + 1496),	//< 현재 agent서버에 적용된 모든 이벤트 보기 회신

	NET_MSG_PET_PETSKINPACKOPEN				= (NET_MSG_GCTRL + 1497),	//< 펫 스킨 팩 아이템 사용
	NET_MSG_PET_PETSKINPACKOPEN_FB			= (NET_MSG_GCTRL + 1498),	//< 펫 스킨 팩 아이템 사용
	NET_MSG_PET_PETSKINPACKOPEN_BRD			= (NET_MSG_GCTRL + 1499),	//< 펫 스킨 팩 아이템 사용

	NET_MSG_GCTRL_PMARKET_SEARCH_ITEM		= (NET_MSG_GCTRL + 1500),	//< 개인상점의 아이템 검색
	NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT = (NET_MSG_GCTRL + 1501),	//< 개인상점의 아이템 검색 결과
	NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT_REQ = (NET_MSG_GCTRL + 1502),	//< 개인상점의 아이템 검색 결과 요청
	
	NET_MSG_GCTRL_DEFENSE_SKILL_ACTIVE		= (NET_MSG_GCTRL + 1503),	//< 피격시 스킬 발동
	
	NET_MSG_REQ_ATTENDLIST					= (NET_MSG_GCTRL + 1504),	//< 출석정보 리스트 요청
	NET_MSG_REQ_ATTENDLIST_FB				= (NET_MSG_GCTRL + 1505),	//< 출석정보 리스트 결과
	NET_MSG_REQ_ATTENDANCE					= (NET_MSG_GCTRL + 1506),	//< 출석 체크
	NET_MSG_REQ_ATTENDANCE_FB				= (NET_MSG_GCTRL + 1507),	//< 출석 체크 답변
	NET_MSG_REQ_ATTEND_REWARD_FLD			= (NET_MSG_GCTRL + 1508),	//< 출석 체크 이벤트 아이템 지급 ( 필드용 )
	NET_MSG_REQ_ATTEND_REWARD_CLT			= (NET_MSG_GCTRL + 1509),	//< 출석 체크 이벤트 아이템 지급 ( 클라용 )

	NET_MSG_REQ_USE_SUMMON					= (NET_MSG_GCTRL + 1510),	//< 소환을 요청
	NET_MSG_REQ_USE_SUMMON_FB				= (NET_MSG_GCTRL + 1511),	//< 소환을 요청
	NET_MSG_REQ_USE_SUMMON_DEL				= (NET_MSG_GCTRL + 1512),	//< 소환수를 지움
	NET_MSG_SUMMON_REQ_GOTO					= (NET_MSG_GCTRL + 1513),	//< 소환수 이동
	NET_MSG_SUMMON_GOTO_BRD					= (NET_MSG_GCTRL + 1514),	//< 소환수 이동	
	NET_MSG_SUMMON_REQ_STOP					= (NET_MSG_GCTRL + 1515),	//< 소환수 멈춤
	NET_MSG_SUMMON_REQ_STOP_BRD				= (NET_MSG_GCTRL + 1516),	//< 소환수 멈춤
	NET_MSG_SUMMON_REQ_UPDATE_MOVE_STATE_BRD = (NET_MSG_GCTRL + 1517),	//< 소환수 이동상태 업데이트
	NET_MSG_SUMMON_REQ_UPDATE_MOVE_STATE	= (NET_MSG_GCTRL + 1518),	//< 소환수 이동상태 업데이트
	NET_MSG_CREATE_ANYSUMMON				= (NET_MSG_GCTRL + 1519),	//< 다른 클라의 소환수 생성
	NET_MSG_DROP_ANYSUMMON					= (NET_MSG_GCTRL + 1520),	//< 다른 클라의 소환수 삭제
	NET_MSG_SUMMON_ATTACK					= (NET_MSG_GCTRL + 1521),	//< 소환수 공격
	NET_MSG_SUMMON_ATTACK_BRD				= (NET_MSG_GCTRL + 1522),	//< 소환수 공격
	NET_MSG_GCTRL_SUMMON_ATTACK_AVOID		= (NET_MSG_GCTRL + 1523),	//< 소환수 공격 회피
	NET_MSG_GCTRL_SUMMON_ATTACK_DAMAGE		= (NET_MSG_GCTRL + 1524),	//< 소환수 공격 데미지
	NET_MSG_SUMMON_REQ_SLOT_EX_HOLD			= (NET_MSG_GCTRL + 1525),	//< 소환수 포션, 슬롯에서 손으로.
	NET_MSG_SUMMON_REQ_SLOT_EX_HOLD_FB		= (NET_MSG_GCTRL + 1526),	//< 소환수 포션, 슬롯에서 손으로.
	NET_MSG_SUMMON_REQ_HOLD_TO_SLOT			= (NET_MSG_GCTRL + 1527),	//< 소환수 포션, 손에서 슬롯으로.
	NET_MSG_SUMMON_REQ_HOLD_TO_SLOT_FB		= (NET_MSG_GCTRL + 1528),	//< 소환수 포션, 손에서 슬롯으로.
	NET_MSG_SUMMON_REMOVE_SLOTITEM			= (NET_MSG_GCTRL + 1529),	//< 소환수 포션 제거
	NET_MSG_SUMMON_REMOVE_SLOTITEM_FB		= (NET_MSG_GCTRL + 1530),	//< 소환수 포션 제거
	NET_MSG_SUMMON_UPDATE_HP				= (NET_MSG_GCTRL + 1531),	//< 소환수 HP 업데이트(회복)
	NET_MSG_SUMMON_UPDATE_HP_BRD			= (NET_MSG_GCTRL + 1532),	//< 소환수 HP 업데이트(회복)

	NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE	= (NET_MSG_GCTRL + 1541),	//< 얼굴 스타일 변경 요청
	NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE_FB	= (NET_MSG_GCTRL + 1542),	//< 얼굴 스타일 변경 결과
	NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE_BRD= (NET_MSG_GCTRL + 1543),	//< 얼굴 스타일 변경 결과

	NET_MSG_REQ_MUST_LEAVE_MAP		= (NET_MSG_GCTRL + 1544),	//< 레벨업시 조건이 맞지않아 맵을 떠나야 할 때
	NET_MSG_REQ_MUST_LEAVE_MAP_FB	= (NET_MSG_GCTRL + 1545),	//< 레벨업시 조건이 맞지않아 맵을 떠나야 할 때
	NET_MSG_REQ_MUST_LEAVE_MAP_AG	= (NET_MSG_GCTRL + 1546),	//< 레벨업시 조건이 맞지않아 맵을 떠나야 할 때

	NET_MSG_CYBERCAFECLASS_UPDATE			= (NET_MSG_GCTRL + 1547),	//< 태국 사이버 카페 등급 업데이트
	
	NET_MSG_GCTRL_REQ_TAXI					= (NET_MSG_GCTRL + 1548),	//< 택시 탑승 요청
	NET_MSG_GCTRL_REQ_TAXI_FB				= (NET_MSG_GCTRL + 1549),	//< 택시 탑승 요청 결과
	NET_MSG_GCTRL_REQ_TAXI_NPCPOS			= (NET_MSG_GCTRL + 1550),	//< 택시 탑승시 NPC좌표 요청
	NET_MSG_GCTRL_REQ_TAXI_NPCPOS_FB		= (NET_MSG_GCTRL + 1551),	//< 택시 탑승시 NPC좌표 요청 결과
	
	NET_MSG_GCTRL_ETNRY_FAILED				= (NET_MSG_GCTRL + 1552),	//< 진입 실패시 메시지
	
	NET_MSG_GCTRL_ITEM_COOLTIME_UPDATE		= (NET_MSG_GCTRL + 1553),	//< 쿨타임 업데이트
	NET_MSG_GCTRL_ITEM_COOLTIME_ERROR		= (NET_MSG_GCTRL + 1554),	//< 쿨타임에 걸려서 사용못함

	NET_MSG_GCTRL_NPC_RECALL				= (NET_MSG_GCTRL + 1555),	//< NPC 호출.
	NET_MSG_GCTRL_NPC_RECALL_FB				= (NET_MSG_GCTRL + 1556),	//< NPC 호출 결과.

	NET_MSG_GCTRL_NPC_COMMISSION			= (NET_MSG_GCTRL + 1557),	//< NPC 호출 COMMISION 업데이트 요청 ( Field->Agent ) 
	NET_MSG_GCTRL_NPC_COMMISSION_FB			= (NET_MSG_GCTRL + 1558),	//< NPC 호출 COMMISION 결과 ( Agent -> Field ) 

	NET_MSG_GCTRL_CREATE_INSTANT_MAP_FLD	= (NET_MSG_GCTRL + 1559),	//< 인스턴스 던젼 생성 AGENT -> FIELD
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_FB		= (NET_MSG_GCTRL + 1560),	//< 인스턴스 던젼 생성 요청 피드백
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ	= (NET_MSG_GCTRL + 1561),	//< 인스턴스 던젼 요청 CLIENT -> FIELD -> AGENT
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_DEL	= (NET_MSG_GCTRL + 1562),	//< 인스턴스 던젼 삭제 

	NET_MSG_GM_SHOP_INFO_REQ				= (NET_MSG_GCTRL + 1563),	//< 상점의 정보 요청(GM명령어)
	NET_MSG_GM_SHOP_INFO_FB					= (NET_MSG_GCTRL + 1564),	//< 상점의 정보 응답(GM명령어)

	NET_MSG_GCTRL_INVEN_ITEM_MIX			= (NET_MSG_GCTRL + 1565),	//< 아이템 조합 요청
	NET_MSG_GCTRL_INVEN_ITEM_MIX_FB			= (NET_MSG_GCTRL + 1566),	//< 아이템 조합 요청 결과

	NET_MSG_GCTRL_LIMITTIME_OVER			= (NET_MSG_GCTRL + 1567),	//< 진입 실패시 메시지

	NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_SESSION	= (NET_MSG_GCTRL + 1568),	// 세션으로 LoginMaintenance 정보 On/Off
	NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_LOGIN		= (NET_MSG_GCTRL + 1569),	// 로그인으로 LoginMaintenance 정보 On/Off
	
	NET_MSG_REQ_GATHERING					= (NET_MSG_GCTRL + 1570),	//< 채집 요청
	NET_MSG_REQ_GATHERING_FB				= (NET_MSG_GCTRL + 1571),	//< 채집 요청 결과
	NET_MSG_REQ_GATHERING_BRD				= (NET_MSG_GCTRL + 1572),	//< 채집 요청 결과
	NET_MSG_REQ_GATHERING_RESULT			= (NET_MSG_GCTRL + 1573),	//< 채집 결과
	NET_MSG_REQ_GATHERING_RESULT_BRD		= (NET_MSG_GCTRL + 1574),	//< 채집 결과
	NET_MSG_REQ_GATHERING_CANCEL			= (NET_MSG_GCTRL + 1575),	//< 채집중 중지
	NET_MSG_REQ_GATHERING_CANCEL_BRD		= (NET_MSG_GCTRL + 1576),	//< 채집중 중지

	NET_MSG_GCTRL_CLUB_AUTHORITY_REQ		= (NET_MSG_GCTRL + 1577),	//<	클럽마스터 권한 위임
	NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK	= (NET_MSG_GCTRL + 1578),	//<	클럽마스터 권한 위임 질문
	NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ANS	= (NET_MSG_GCTRL + 1579),	//<	클럽마스터 권한 위임 답변
	NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB		= (NET_MSG_GCTRL + 1580),	//<	클럽마스터 권한 위임 결과
	NET_MSG_GCTRL_CLUB_AUTHORITY_CLT		= (NET_MSG_GCTRL + 1581),	//<	클럽마스터 변경 내용
	NET_MSG_GCTRL_CLUB_AUTHORITY_BRD		= (NET_MSG_GCTRL + 1582),	//<	클럽마스터 변경 내용
	NET_MSG_GCTRL_CLUB_AUTHORITY_FLD		= (NET_MSG_GCTRL + 1583),	//<	클럽마스터 변경 내용 (필드)

	NET_MSG_GCTRL_CLUB_DEATHMATCH_REMAIN_BRD	= (NET_MSG_GCTRL + 1584),	//<	클럽데스매치 남은시간.
	NET_MSG_GCTRL_CLUB_DEATHMATCH_READY_FLD		= (NET_MSG_GCTRL + 1585),	//<	클럽데스매치 준비.
	NET_MSG_GCTRL_CLUB_DEATHMATCH_START_FLD		= (NET_MSG_GCTRL + 1586),	//<	클럽데스매치 시작.
	NET_MSG_GCTRL_CLUB_DEATHMATCH_START_BRD		= (NET_MSG_GCTRL + 1587),	//<	클럽데스매치 시작.

	NET_MSG_GCTRL_CLUB_DEATHMATCH_END_FLD		= (NET_MSG_GCTRL + 1588),	//<	클럽데스매치 종료.
	NET_MSG_GCTRL_CLUB_DEATHMATCH_END_BRD		= (NET_MSG_GCTRL + 1589),	//<	클럽데스매치 종료.

	NET_MSG_GCTRL_SERVER_CLUB_DEATHMATCH_INFO	= (NET_MSG_GCTRL + 1590),	//<	클럽데스매치 정보 전송 ( 접속시 ) 
	NET_MSG_GCTRL_REQ_SERVER_CLUB_DEATHMATCH_REMAIN_AG	= (NET_MSG_GCTRL + 1591),	//<	클럽데스매치 정보 전송 ( Agent에 남은시간 요청 )

	NET_MSG_GCTRL_CLUB_DEATHMATCH_POINT_UPDATE		= (NET_MSG_GCTRL + 1592),	//<	클럽데스매치 Kill/Death 상황 ( 자기만 )
	NET_MSG_GCTRL_CLUB_DEATHMATCH_MYRANK_UPDATE		= (NET_MSG_GCTRL + 1593),	//<	클럽데스매치 Kill/Death 상황 ( 자기만 )
	NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_UPDATE	= (NET_MSG_GCTRL + 1594),	//<	클럽데스매치 랭킹 상황
	NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_REQ		= (NET_MSG_GCTRL + 1595),	//<	클럽데스매치 상황 요청
};


// Generic message
// 8 bytes
struct NET_MSG_GENERIC
{
	DWORD		dwSize;		// Size of Message Data
	EMNET_MSG	nType;		// Message type
};
typedef NET_MSG_GENERIC* LPNET_MSG_GENERIC;

struct NET_HEARTBEAT_CLIENT_REQ
{
	NET_MSG_GENERIC nmg;	

	NET_HEARTBEAT_CLIENT_REQ()
	{
		nmg.dwSize = sizeof( NET_HEARTBEAT_CLIENT_REQ );
		nmg.nType = NET_MSG_HEARTBEAT_CLIENT_REQ;
	}
};

struct NET_HEARTBEAT_CLIENT_ANS
{
	NET_MSG_GENERIC nmg;

	CHAR	szEnCrypt[ENCRYPT_KEY+1];

	NET_HEARTBEAT_CLIENT_ANS()
	{
		nmg.dwSize = sizeof( NET_HEARTBEAT_CLIENT_ANS );
		nmg.nType = NET_MSG_HEARTBEAT_CLIENT_ANS;
		SecureZeroMemory(szEnCrypt, sizeof(CHAR) * (ENCRYPT_KEY+1));
	}
};

struct NET_HEARTBEAT_SERVER_REQ
{
	NET_MSG_GENERIC nmg;
	NET_HEARTBEAT_SERVER_REQ()
	{
		nmg.dwSize = sizeof( NET_HEARTBEAT_SERVER_REQ );
		nmg.nType  = NET_MSG_HEARTBEAT_SERVER_REQ; //< Server HeartBeat Request
	}
};

struct NET_HEARTBEAT_SERVER_ANS
{
	NET_MSG_GENERIC nmg;
	NET_HEARTBEAT_SERVER_ANS()
	{
		nmg.dwSize = sizeof( NET_HEARTBEAT_SERVER_ANS );
		nmg.nType  = NET_MSG_HEARTBEAT_SERVER_ANS; //< Server HeartBeat Answer
	}
};

//! Compressed or combined message
struct NET_COMPRESS
{
	NET_MSG_GENERIC nmg;
	bool bCompress; //< Compressed or not (combined)

	NET_COMPRESS()
	{
		nmg.dwSize = sizeof(NET_COMPRESS);
		nmg.nType  = NET_MSG_COMPRESS;
		bCompress = true;
	}
};
typedef NET_COMPRESS* LPNET_COMPRESS;

// Confirmation Message
struct NET_CONF
{
	NET_MSG_GENERIC nmg;
	USHORT			conf;
	NET_CONF()
	{
		nmg.dwSize = sizeof(NET_CONF);
	}
};
typedef NET_CONF* LPNET_CONF;

struct NET_FEEDBACK_DATA
{
	NET_MSG_GENERIC nmg;
	USHORT			nResult;
	NET_FEEDBACK_DATA()
	{
		nmg.dwSize = sizeof(NET_FEEDBACK_DATA);
	}
};
typedef NET_FEEDBACK_DATA* LPNET_FEEDBACK_DATA;

struct CRYPT_KEY
{
	USHORT			nKeyDirection;
	USHORT			nKey;

	CRYPT_KEY()
	{
		nKeyDirection = 0;
		nKey          = 0;
	}
};
typedef CRYPT_KEY* LPCRYPT_KEY;

struct NET_CRYPT_KEY
{
	NET_MSG_GENERIC nmg;
	CRYPT_KEY		ck;

	NET_CRYPT_KEY()
	{
		nmg.dwSize = sizeof(NET_CRYPT_KEY);
		nmg.nType = NET_MSG_SND_CRYT_KEY;
	}
};
typedef NET_CRYPT_KEY* LPNET_CRYPT_KEY;

struct NET_RANDOMPASS_NUMBER
{
	NET_MSG_GENERIC nmg;
	INT nRandomNumber;

	NET_RANDOMPASS_NUMBER()
	{
		nmg.dwSize = sizeof(NET_RANDOMPASS_NUMBER);
		nmg.nType  = NET_MSG_RANDOM_NUM;
		nRandomNumber = 0;
	}
};

// Control program->Session Server
// Server state change command 
// NET_MSG_SVR_PAUSE			= (NET_MSG_LGIN + 310), // 서버를 잠시 멈춘다.
// NET_MSG_SVR_RESUME			= (NET_MSG_LGIN + 311), // 멈추어진 서버를 다시 가동시킨다.	
// NET_MSG_SVR_RESTART			= (NET_MSG_LGIN + 312), // 서버를 완전히 멈추고 새롭게 가동시킨다.
// NET_MSG_SVR_STOP			    = (NET_MSG_LGIN + 313), // 서버를 완전히 멈춘다.
struct NET_SVR_CMD_CHANGE_STATE
{
	NET_MSG_GENERIC nmg;
	int nCmdType;
	int nSvrType;
	int nSvrGrp;
	int nSvrNum;
	int nSvrField;

	NET_SVR_CMD_CHANGE_STATE()
	{
		nmg.dwSize = sizeof(NET_SVR_CMD_CHANGE_STATE);
		nmg.nType = NET_MSG_SVR_CMD;
		nCmdType  = NET_MSG_SVR_RESTART;
		nSvrType  = -1;
		nSvrGrp   = -1;
		nSvrNum   = -1;
		nSvrField = -1;
	}
};
typedef NET_SVR_CMD_CHANGE_STATE* LPNET_SVR_CMD_CHANGE_STATE;

///////////////////////////////////////////////////////////////////////////////
// Login messages
// Taiwan / Hongkong
struct NET_LOGIN_DATA
{
	NET_MSG_GENERIC nmg;
	int             nChannel;
	CHAR            szRandomPassword[USR_RAND_PASS_LENGTH+1];
	CHAR			szPassword[USR_PASS_LENGTH+1];
	CHAR			szUserid  [USR_ID_LENGTH+1];
	CHAR			szEnCrypt[ENCRYPT_KEY+1];

	NET_LOGIN_DATA()
	{		
		nmg.dwSize  = sizeof(NET_LOGIN_DATA);
		nmg.nType  = NET_MSG_LOGIN_2;
		nChannel   = 0;
		SecureZeroMemory(szUserid,         sizeof(CHAR) * (USR_ID_LENGTH+1));
		SecureZeroMemory(szPassword,       sizeof(CHAR) * (USR_PASS_LENGTH+1));
		SecureZeroMemory(szRandomPassword, sizeof(CHAR) * (USR_RAND_PASS_LENGTH+1));
		SecureZeroMemory(szEnCrypt, sizeof(CHAR) * (ENCRYPT_KEY+1));
	}
};
typedef NET_LOGIN_DATA* LPNET_LOGIN_DATA;

///////////////////////////////////////////////////////////////////////////////
// Login messages
// China
#define RSA_ADD	4

struct CHINA_NET_LOGIN_DATA
{
	NET_MSG_GENERIC nmg;
	int             nChannel;
	TCHAR           szRandomPassword[USR_RAND_PASS_LENGTH+RSA_ADD+1];
	TCHAR			szPassword[USR_PASS_LENGTH+RSA_ADD+1];
	TCHAR			szUserid[USR_ID_LENGTH+RSA_ADD+1];

	CHINA_NET_LOGIN_DATA()
	{
		nmg.dwSize  = sizeof(CHINA_NET_LOGIN_DATA);
		nmg.nType  = CHINA_NET_MSG_LOGIN;
		nChannel   = 0;
		SecureZeroMemory( szUserid,         sizeof(TCHAR) * (USR_ID_LENGTH+RSA_ADD+1) );
		SecureZeroMemory( szPassword,       sizeof(TCHAR) * (USR_PASS_LENGTH+RSA_ADD+1) );
		SecureZeroMemory( szRandomPassword, sizeof(TCHAR) * (USR_RAND_PASS_LENGTH+RSA_ADD+1) );
	}
};
typedef CHINA_NET_LOGIN_DATA* LPCHINA_NET_LOGIN_DATA;

///////////////////////////////////////////////////////////////////////////////
// Login messages
// Thailand (태국)
struct THAI_NET_LOGIN_DATA
{
	NET_MSG_GENERIC nmg;
	int             nChannel;
	CHAR			szPassword[USR_PASS_LENGTH+1];
	CHAR			szUserid  [USR_ID_LENGTH+1];	

	THAI_NET_LOGIN_DATA()
	{		
		nmg.dwSize  = sizeof(THAI_NET_LOGIN_DATA);
		nmg.nType  = THAI_NET_MSG_LOGIN;
		nChannel   = 0;
		memset(szUserid,   0, sizeof(CHAR) * (USR_ID_LENGTH+1));
		memset(szPassword, 0, sizeof(CHAR) * (USR_PASS_LENGTH+1));

	}
};
typedef THAI_NET_LOGIN_DATA* LPTHAI_NET_LOGIN_DATA;

///////////////////////////////////////////////////////////////////////////////
// Login messages
// Daum Game

//struct DAUM_NET_LOGIN_DATA
//{
//	NET_MSG_GENERIC nmg;
//	int             nChannel;
//	CHAR	        szParameter[DAUM_MAX_PARAM_LENGTH+1];	
//
//	DAUM_NET_LOGIN_DATA()
//	{
//		memset(this, 0, sizeof(DAUM_NET_LOGIN_DATA));
//		nmg.dwSize = sizeof(DAUM_NET_LOGIN_DATA);
//		nmg.nType = DAUM_NET_MSG_LOGIN;
//	}
//};
//typedef DAUM_NET_LOGIN_DATA* LPDAUM_NET_LOGIN_DATA;

struct DAUM_NET_LOGIN_DATA
{
	NET_MSG_GENERIC nmg;
	int nChannel;
	TCHAR szUUID[UUID_STR_LENGTH];	

	DAUM_NET_LOGIN_DATA()
	{
		nmg.dwSize = sizeof(DAUM_NET_LOGIN_DATA);
		nmg.nType = DAUM_NET_MSG_LOGIN;
		nChannel = 0;
		memset( szUUID, 0, sizeof(TCHAR) * (UUID_STR_LENGTH) );
	}
};
typedef DAUM_NET_LOGIN_DATA* LPDAUM_NET_LOGIN_DATA;

///////////////////////////////////////////////////////////////////////////////
// Login message
// Terra
struct TERRA_NET_LOGIN_DATA
{
	NET_MSG_GENERIC nmg;
	int nChannel;
	TCHAR szTID[TERRA_TID_ENCODE+1];

    TERRA_NET_LOGIN_DATA()
	{		
		nmg.dwSize = sizeof(TERRA_NET_LOGIN_DATA);
		nmg.nType = TERRA_NET_MSG_LOGIN;
		nChannel  = 0;
		memset(szTID, 0, sizeof(TCHAR) * (TERRA_TID_ENCODE+1));		
	}
};
typedef TERRA_NET_LOGIN_DATA* LPTERRA_NET_LOGIN_DATA;

// Ran Global Service GSP Login Data
struct GSP_NET_LOGIN_DATA
{
	NET_MSG_GENERIC nmg;
	int nChannel;
	TCHAR szUUID[UUID_STR_LENGTH];

	GSP_NET_LOGIN_DATA()
	{
		nmg.dwSize = sizeof(GSP_NET_LOGIN_DATA);
		nmg.nType = GSP_NET_MSG_LOGIN;
		nChannel  = 0;
		memset( szUUID, 0, sizeof(TCHAR) * (UUID_STR_LENGTH) );
	}
};
typedef GSP_NET_LOGIN_DATA* LPGSP_NET_LOGIN_DATA;

///////////////////////////////////////////////////////////////////////////////
// Password Check message
// Terra
struct TERRA_NET_PASSCHECK_DATA
{
	NET_MSG_GENERIC nmg;
	TCHAR szTLoginName[TERRA_TLOGIN_NAME+1];
	TCHAR szUserPass[TERRA_USERPASS+1];
	INT		nCheckFlag; // 패스워드 갱신을 위해서 추가

    TERRA_NET_PASSCHECK_DATA()
		: nCheckFlag(0)
	{		
		nmg.dwSize = sizeof(TERRA_NET_PASSCHECK_DATA);
		nmg.nType = TERRA_NET_MSG_PASSCHECK;
		memset(szTLoginName, 0, sizeof(TCHAR) * (TERRA_TLOGIN_NAME+1));
		memset(szUserPass, 0, sizeof(TCHAR) * (TERRA_USERPASS+1));
	}
};
typedef TERRA_NET_PASSCHECK_DATA* LPTERRA_NET_PASSCHECK_DATA;

// Daum
struct DAUM_NET_PASSCHECK_DATA
{
	NET_MSG_GENERIC nmg;
	TCHAR	szDaumGID[DAUM_MAX_GID_LENGTH+1];
	TCHAR	szUserPass[DAUM_USERPASS+1];
	INT		nCheckFlag; // 패스워드 갱신을 위해서 추가

	DAUM_NET_PASSCHECK_DATA()
		: nCheckFlag(0)
	{
		nmg.nType = DAUM_NET_MSG_PASSCHECK;
		nmg.dwSize = sizeof(DAUM_NET_PASSCHECK_DATA);
		memset(szDaumGID, 0, sizeof(szDaumGID));
		memset(szUserPass, 0, sizeof(szUserPass));
	}
};
typedef DAUM_NET_PASSCHECK_DATA* LPDAUM_NET_PASSCHECK_DATA;


struct GSP_NET_LOGIN_FEEDBACK_DATA2
{
	NET_MSG_GENERIC nmg;
	TCHAR           szGspUserID[GSP_USERID];	
	TCHAR           szIp[GSP_USERIP];
	TCHAR			szUUID[UUID_STR_LENGTH]; 
	INT				nClient;
	USHORT			nResult;
	INT				nUserNum;
	INT				nUserType;
	USHORT			uChaRemain;
	USHORT			uChaTestRemain;
    __time64_t      tPremiumTime;
	__time64_t      tChatBlockTime;

    GSP_NET_LOGIN_FEEDBACK_DATA2()
	{		
		nmg.dwSize = sizeof(GSP_NET_LOGIN_FEEDBACK_DATA2);
		memset( szGspUserID, 0, sizeof(TCHAR) * (GSP_USERID) );
		memset( szIp, 0, sizeof(TCHAR) * (GSP_USERIP) );
		memset( szUUID, 0, sizeof(TCHAR) * (UUID_STR_LENGTH) );
		nClient        = 0;
		nResult        = 0;
		nUserNum       = 0;
		nUserType      = 0;
		uChaRemain     = 0;
		uChaTestRemain = 0;
		tPremiumTime   = 0;
		tChatBlockTime = 0;
	}
};
typedef GSP_NET_LOGIN_FEEDBACK_DATA2* LPGSP_NET_LOGIN_FEEDBACK_DATA2;

// Excite Japan
struct EXCITE_NET_LOGIN_DATA
{
	NET_MSG_GENERIC nmg;
	int				nChannel;
	CHAR			szUID[EXCITE_USER_ID+1];
	CHAR			szTDATE[EXCITE_TDATE+1];
	CHAR            szMD5[EXCITE_MD5+1];

    EXCITE_NET_LOGIN_DATA()
	{
		nmg.dwSize = sizeof(EXCITE_NET_LOGIN_DATA);
		nmg.nType = EXCITE_NET_MSG_LOGIN;
		nChannel  = 0;
		memset(szUID,   0, sizeof(CHAR) * (EXCITE_USER_ID+1));
		memset(szTDATE, 0, sizeof(CHAR) * (EXCITE_TDATE+1));
		memset(szMD5,   0, sizeof(CHAR) * (EXCITE_MD5+1));
	}
};
typedef EXCITE_NET_LOGIN_DATA* LPEXCITE_NET_LOGIN_DATA;


// Excite
struct EXCITE_NET_PASSCHECK_DATA
{
	NET_MSG_GENERIC nmg;
	TCHAR	szExciteUserID[EXCITE_SEC_ID+1];
	TCHAR	szUserPass[EXCITE_SEC_PASS+1];
	INT		nCheckFlag; // 패스워드 갱신을 위해서 추가

	EXCITE_NET_PASSCHECK_DATA()
		: nCheckFlag(0)
	{
		nmg.nType = EXCITE_NET_MSG_PASSCHECK;
		nmg.dwSize = sizeof(EXCITE_NET_PASSCHECK_DATA);
		memset(szExciteUserID, 0, sizeof(szExciteUserID));
		memset(szUserPass, 0, sizeof(szUserPass));
	}
};
typedef EXCITE_NET_PASSCHECK_DATA* LPEXCITE_NET_PASSCHECK_DATA;



struct EXCITE_NET_PASSCHECK_FEEDBACK_DATA2
{
	NET_MSG_GENERIC nmg;
	TCHAR			szExciteUserID[EXCITE_USER_ID+1];
	INT				nClient;
	USHORT			nResult;

	EXCITE_NET_PASSCHECK_FEEDBACK_DATA2()
		: nClient(0)
		, nResult(0)
	{
		memset(this, 0, sizeof( EXCITE_NET_PASSCHECK_FEEDBACK_DATA2 ));
		nmg.dwSize		= sizeof(EXCITE_NET_PASSCHECK_FEEDBACK_DATA2);
		memset(szExciteUserID, 0, sizeof( szExciteUserID ));
	}
};
typedef EXCITE_NET_PASSCHECK_FEEDBACK_DATA2* LPEXCITE_NET_PASSCHECK_FEEDBACK_DATA2;

struct JAPAN_NET_LOGIN_DATA
{
	NET_MSG_GENERIC nmg;
	int				nChannel;		
	CHAR			szUserid  [JAPAN_USER_ID+1];
	CHAR			szPassword[JAPAN_USER_PASS+1];
	CHAR			szEnCrypt[ENCRYPT_KEY+1];

	JAPAN_NET_LOGIN_DATA()
	{		
		nmg.dwSize  = sizeof(JAPAN_NET_LOGIN_DATA);
		nmg.nType  = JAPAN_NET_MSG_LOGIN;
		nChannel   = 0;
		SecureZeroMemory(szUserid,         sizeof(CHAR) * (JAPAN_USER_ID+1));
		SecureZeroMemory(szPassword,       sizeof(CHAR) * (JAPAN_USER_PASS+1));
		SecureZeroMemory(szEnCrypt, sizeof(CHAR) * (ENCRYPT_KEY+1));
	}
};
typedef JAPAN_NET_LOGIN_DATA* LPJAPAN_NET_LOGIN_DATA;

struct GS_NET_LOGIN_DATA
{
	NET_MSG_GENERIC nmg;
	int             nChannel;
	CHAR			szPassword[GS_USER_ID+1];
	CHAR			szUserid  [GS_USER_PASS+1];
	CHAR			szEnCrypt[ENCRYPT_KEY+1];

	GS_NET_LOGIN_DATA()
	{		
		nmg.dwSize  = sizeof(GS_NET_LOGIN_DATA);
		nmg.nType  = GS_NET_MSG_LOGIN;
		nChannel   = 0;
		SecureZeroMemory(szUserid,         sizeof(CHAR) * (GS_USER_ID+1));
		SecureZeroMemory(szPassword,       sizeof(CHAR) * (GS_USER_PASS+1));
		SecureZeroMemory(szEnCrypt, sizeof(CHAR) * (ENCRYPT_KEY+1));
	}
};
typedef GS_NET_LOGIN_DATA* LPGS_NET_LOGIN_DATA;

struct NET_LOGIN_DATA2
{
	NET_MSG_GENERIC nmg;
	CHAR			szUserid[USR_ID_LENGTH+1];
	CHAR			szPassword[USR_PASS_LENGTH+1];
	CHAR			szIp[MAX_IP_LENGTH+1];
	INT				nClient;

	NET_LOGIN_DATA2()
	{
		nmg.nType = EMNET_MSG(0);
		nmg.dwSize = sizeof(NET_LOGIN_DATA2);
		memset(szUserid, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
		memset(szPassword, 0, sizeof(CHAR) * (USR_PASS_LENGTH+1));
		memset(szIp, 0, sizeof(CHAR) * (MAX_IP_LENGTH+1));
		nClient = 0;
	}
};
typedef  NET_LOGIN_DATA2* LPNET_LOGIN_DATA2;

struct DAUM_NET_LOGIN_DATA2
{
	NET_MSG_GENERIC nmg;
	CHAR			szDaumGID[DAUM_MAX_GID_LENGTH+1];
	CHAR			szDaumUID[DAUM_MAX_UID_LENGTH+1];
	CHAR			szDaumSSNHEAD[DAUM_MAX_SSNHEAD_LENGTH+1];
	CHAR			szDaumSEX[DAUM_MAX_SEX_LENGTH+1];
	CHAR			szIp[MAX_IP_LENGTH+1];
	INT				nClient;

	DAUM_NET_LOGIN_DATA2()
	{
		nmg.nType = EMNET_MSG(0);
		nmg.dwSize = sizeof(DAUM_NET_LOGIN_DATA2);
		memset(szDaumGID, 0, sizeof(CHAR) * (DAUM_MAX_GID_LENGTH+1));
		memset(szDaumUID, 0, sizeof(CHAR) * (DAUM_MAX_UID_LENGTH+1));
		memset(szDaumSSNHEAD, 0, sizeof(CHAR) * (DAUM_MAX_SSNHEAD_LENGTH+1));
		memset(szDaumSEX, 0, sizeof(CHAR) * (DAUM_MAX_SEX_LENGTH+1));
		memset(szIp, 0, sizeof(CHAR) * (MAX_IP_LENGTH+1));
		nClient = 0;
	}
};
typedef  DAUM_NET_LOGIN_DATA2* LPDAUMNET_LOGIN_DATA2;

struct NET_LOGIN_FEEDBACK_DATA
{
	NET_MSG_GENERIC nmg;

	TCHAR			szDaumGID[DAUM_MAX_GID_LENGTH+1];
	USHORT			nResult;
    USHORT          uChaRemain; // 만들수 있는 캐릭터 갯수
	INT				nExtremeM;	// 극강부 남자 생성 가능 숫자
	INT				nExtremeW;	// 극강부 여자 생성 가능 숫자
	INT				nCheckFlag; // 2차 패스워드를 받기위한 Check Flag 값
	INT				nPatchProgramVer;
	INT				nGameProgramVer;

	NET_LOGIN_FEEDBACK_DATA()
	{
		memset(this, 0, sizeof(NET_LOGIN_FEEDBACK_DATA));
		nmg.dwSize        = sizeof(NET_LOGIN_FEEDBACK_DATA);
		memset(szDaumGID, 0, sizeof(szDaumGID));
		nResult          = EM_LOGIN_FB_SUB_FAIL;
		nPatchProgramVer = 0;
		nGameProgramVer  = 0;
        uChaRemain       = 0;
		nExtremeM		 = 0;
		nExtremeW		 = 0;
		nCheckFlag		 = 0;
	}
};
typedef NET_LOGIN_FEEDBACK_DATA* LPNET_LOGIN_FEEDBACK_DATA;

struct NET_LOGIN_FEEDBACK_DATA2
{
	NET_MSG_GENERIC nmg;
	CHAR			szUserid[USR_ID_LENGTH+1];
	CHAR			szIp[MAX_IP_LENGTH+1];
	INT				nClient;
	USHORT			nResult;
	INT				nUserNum;
	INT				nUserType;
	INT				nExtremeM;				// 극강부 남자 생성 가능 숫자
	INT				nExtremeW;				// 극강부 여자 생성 가능 숫자
	INT				nCheckFlag;				// 패스워드를 받기 위한 Check Flag 값
	INT				nThaiCC_Class;			// 태국 User 클래스 타입
	INT				nMyCC_Class;			// 말레이시아 User 클래스 타입
	INT				nChinaGameTime;			// 중국 GameTime 저장
	INT				nChinaOfflineTime;		// 중국 OfflienTiem 저장
	INT				nChinaUserAge;			// 중국 UserAge 저장
	INT				nVTGameTime;			//  베트남 GameTime 저장
	USHORT			uChaRemain;
	USHORT			uChaTestRemain;
    __time64_t      tPremiumTime;
	__time64_t      tChatBlockTime;
	__time64_t		tLastLoginTime;			// 중국, 베트남 LastLoginTime 저장
	WORD            wThaiPlayTime;			// Thailand (오늘 플레이한 시간 단위:분)
	WORD            wThaiFlag;				// Thailand (사용자 구분 플래그)
	
	NET_LOGIN_FEEDBACK_DATA2()
	{
		nmg.dwSize = sizeof(NET_LOGIN_FEEDBACK_DATA2);
		memset (szUserid, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
		memset (szIp, 0, sizeof(CHAR) * (MAX_IP_LENGTH+1));
		nClient        = 0;
		nResult        = 0;
		nUserNum       = 0;
		nUserType      = 0;
		nExtremeM	   = 0;
		nExtremeW	   = 0;
		uChaRemain     = 0;
		uChaTestRemain = 0;
		tPremiumTime   = 0;
		tChatBlockTime = 0;
		wThaiPlayTime  = 0;
		wThaiFlag      = 0;
		nCheckFlag	   = 0;
		nCheckFlag     = 0;
		nThaiCC_Class  = 0;
		nChinaGameTime = 0;
		nChinaOfflineTime = 0;
		tLastLoginTime = 0;
		nChinaUserAge = 0;
		nVTGameTime		= 0;
	}
};
typedef NET_LOGIN_FEEDBACK_DATA2* LPNET_LOGIN_FEEDBACK_DATA2;

///////////////////////////////////////////////////////////////////////////////
//
struct DAUM_NET_LOGIN_FEEDBACK_DATA2
{
	NET_MSG_GENERIC nmg;
	TCHAR			szDaumGID[DAUM_MAX_GID_LENGTH+1];
//	TCHAR			szDaumUID[DAUM_MAX_UID_LENGTH+1];
	TCHAR			szUUID[UUID_STR_LENGTH]; 
	TCHAR			szIp[MAX_IP_LENGTH+1];
	INT				nClient;
	USHORT			nResult;
	INT				nUserNum;
	INT				nUserType;
	INT				nExtremeM;	// 극강부 남자 생성 가능 숫자
	INT				nExtremeW;	// 극강부 여자 생성 가능 숫자
	INT				nCheckFlag; // Daum 패스워드를 받기위한 Check Flag 값
	USHORT			uChaRemain;
	USHORT			uChaTestRemain;
    __time64_t      tPremiumTime;
	__time64_t      tChatBlockTime;
	
	DAUM_NET_LOGIN_FEEDBACK_DATA2()
	{	
		nmg.dwSize = sizeof(DAUM_NET_LOGIN_FEEDBACK_DATA2);
		memset(szDaumGID, 0, sizeof(szDaumGID));
		memset( szUUID, 0, sizeof(TCHAR) * (UUID_STR_LENGTH) );
//		memset(szDaumUID, 0, sizeof(szDaumUID));
		memset(szIp, 0, sizeof(szIp));		
		nClient        = 0;
		nResult        = 0;
		nUserNum       = 0;
		nUserType      = 0;
		nExtremeM	   = 0;
		nExtremeW	   = 0;
		uChaRemain     = 0;
		uChaTestRemain = 0;
		tPremiumTime   = 0;
        tChatBlockTime = 0;
		nCheckFlag	   = 0;
	}
};
typedef DAUM_NET_LOGIN_FEEDBACK_DATA2* LPDAUM_NET_LOGIN_FEEDBACK_DATA2;

struct TERRA_NET_LOGIN_FEEDBACK_DATA2
{
	NET_MSG_GENERIC nmg;
	CHAR            szTerraTLoginName[TERRA_TLOGIN_NAME+1];
	CHAR            szTerraDecodedTID[TERRA_TID_DECODE+1];
	CHAR            szIp[MAX_IP_LENGTH+1];
	INT				nClient;
	USHORT			nResult;
	INT				nUserNum;
	INT				nUserType;
	INT				nExtremeM;				// 극강부 남자 생성 가능 숫자
	INT				nExtremeW;				// 극강부 여자 생성 가능 숫자
	INT				nCheckFlag;				// 말레이지아 패스워드를 받기위한 Check Flag 값
	USHORT			uChaRemain;
	USHORT			uChaTestRemain;
	INT				nMyCC_Class;			// 말레이시아 PC방 이벤트
	INT				nVTGameTime;			//  베트남 GameTime 저장
	__time64_t		tLastLoginTime;			// 중국, 베트남 LastLoginTime 저장
    __time64_t      tPremiumTime;
	__time64_t      tChatBlockTime;

    TERRA_NET_LOGIN_FEEDBACK_DATA2()
	{		
		nmg.dwSize = sizeof(TERRA_NET_LOGIN_FEEDBACK_DATA2);
		memset(szTerraTLoginName, 0, sizeof(CHAR) * (TERRA_TLOGIN_NAME+1));
		memset(szIp, 0, sizeof(CHAR) * (MAX_IP_LENGTH+1));
		memset(szTerraDecodedTID, 0, sizeof(CHAR) * (TERRA_TID_DECODE+1));
		nClient        = 0;
		nResult        = 0;
		nUserNum       = 0;
		nUserType      = 0;
		nExtremeM	   = 0;
		nExtremeW	   = 0;
		nCheckFlag	   = 0;
		uChaRemain     = 0;		
		uChaTestRemain = 0;
		tPremiumTime   = 0;
		tChatBlockTime = 0;
		nVTGameTime	   = 0;
		tLastLoginTime = 0;
		nMyCC_Class	   = 0;
	}
};
typedef TERRA_NET_LOGIN_FEEDBACK_DATA2* LPTERRA_NET_LOGIN_FEEDBACK_DATA2;

struct NET_PASSCHECK_FEEDBACK_DATA
{
	NET_MSG_GENERIC nmg;
	INT				nClient;
	USHORT			nResult;

	NET_PASSCHECK_FEEDBACK_DATA()
	{		
		memset(this, 0, sizeof( NET_PASSCHECK_FEEDBACK_DATA ));
		nmg.nType		= NET_MSG_PASSCHECK_FB;
		nmg.dwSize		= sizeof( NET_PASSCHECK_FEEDBACK_DATA );
		nClient			= 0;
		nResult			= 0;
	}
};
typedef NET_PASSCHECK_FEEDBACK_DATA* LPNET_PASSCHECK_FEEDBACK_DATA;

struct TERRA_NET_PASSCHECK_FEEDBACK_DATA2
{
	NET_MSG_GENERIC nmg;
	CHAR            szTerraTLoginName[TERRA_TLOGIN_NAME+1];
	INT				nClient;
	USHORT			nResult;

	TERRA_NET_PASSCHECK_FEEDBACK_DATA2()
	{
		memset(szTerraTLoginName, 0, sizeof(CHAR) * (TERRA_TLOGIN_NAME+1));
		memset(this, 0, sizeof( TERRA_NET_PASSCHECK_FEEDBACK_DATA2 ));
		nmg.dwSize		= sizeof(TERRA_NET_PASSCHECK_FEEDBACK_DATA2);
		nClient			= 0;
		nResult			= 0;
	}
};
typedef TERRA_NET_PASSCHECK_FEEDBACK_DATA2* LPTERRA_NET_PASSCHECK_FEEDBACK_DATA2;

struct DAUM_NET_PASSCHECK_FEEDBACK_DATA2
{
	NET_MSG_GENERIC nmg;
	TCHAR			szDaumGID[DAUM_MAX_GID_LENGTH+1];
	INT				nClient;
	USHORT			nResult;

	DAUM_NET_PASSCHECK_FEEDBACK_DATA2()
		: nClient(0)
		, nResult(0)
	{
		memset(this, 0, sizeof( DAUM_NET_PASSCHECK_FEEDBACK_DATA2 ));
		nmg.dwSize		= sizeof(DAUM_NET_PASSCHECK_FEEDBACK_DATA2);
		memset(szDaumGID, 0, sizeof( szDaumGID ));
	}
};
typedef DAUM_NET_PASSCHECK_FEEDBACK_DATA2* LPDAUM_NET_PASSCHECK_FEEDBACK_DATA2;

// nCheckFlag 값 추가
struct EXCITE_NET_LOGIN_FEEDBACK_DATA2
{
	NET_MSG_GENERIC nmg;
	CHAR            szExciteUserID[EXCITE_USER_ID+1];
	CHAR            szIp[MAX_IP_LENGTH+1];
	INT				nClient;
	USHORT			nResult;
	INT				nUserNum;
	INT				nUserType;
	INT				nCheckFlag; // 일본 패스워드를 받기위한 Check Flag 값
	USHORT			uChaRemain;
	USHORT			uChaTestRemain;
    __time64_t      tPremiumTime;
	__time64_t      tChatBlockTime;

    EXCITE_NET_LOGIN_FEEDBACK_DATA2()
	{
		nmg.dwSize = sizeof(EXCITE_NET_LOGIN_FEEDBACK_DATA2);
		memset(szExciteUserID, 0, sizeof(CHAR) * (EXCITE_USER_ID+1));
		memset(szIp, 0, sizeof(CHAR) * (MAX_IP_LENGTH+1));
		nClient        = 0;
		nResult        = 0;
		nUserNum       = 0;
		nUserType      = 0;
		nCheckFlag	   = 0;
		uChaRemain     = 0;
		uChaTestRemain = 0;
		tPremiumTime   = 0;
		tChatBlockTime = 0;
	}
};
typedef EXCITE_NET_LOGIN_FEEDBACK_DATA2* LPEXCITE_NET_LOGIN_FEEDBACK_DATA2;

// 일본 Gonzo로 회사 변경되면서 구조체추가
struct JAPAN_NET_LOGIN_FEEDBACK_DATA2
{
	NET_MSG_GENERIC nmg;
	CHAR            JapanUserID[JAPAN_USER_ID+1];
	CHAR            szIp[MAX_IP_LENGTH+1];
	INT				nClient;
	USHORT			nResult;
	INT				nUserNum;
	INT				nUserType;
	INT				nExtremeM;	// 극강부 남자 생성 가능 숫자
	INT				nExtremeW;	// 극강부 여자 생성 가능 숫자
	USHORT			uChaRemain;
	USHORT			uChaTestRemain;
    __time64_t      tPremiumTime;
	__time64_t      tChatBlockTime;

    JAPAN_NET_LOGIN_FEEDBACK_DATA2()
	{		
		nmg.dwSize = sizeof(JAPAN_NET_LOGIN_FEEDBACK_DATA2);
		memset(JapanUserID, 0, sizeof(CHAR) * (JAPAN_USER_ID+1));
		memset(szIp, 0, sizeof(CHAR) * (MAX_IP_LENGTH+1));
		nClient        = 0;
		nResult        = 0;
		nUserNum       = 0;
		nUserType      = 0;
		nExtremeM	   = 0;
		nExtremeW	   = 0;
		uChaRemain     = 0;
		uChaTestRemain = 0;
		tPremiumTime   = 0;
		tChatBlockTime = 0;
	}
};
typedef JAPAN_NET_LOGIN_FEEDBACK_DATA2* LPJAPAN_NET_LOGIN_FEEDBACK_DATA2;

struct JAPAN_NET_LOGIN_UUID
{
	NET_MSG_GENERIC nmg;
	CHAR	szUUID[UUID_STR_LENGTH];
	
	JAPAN_NET_LOGIN_UUID()
	{		
		nmg.nType	= JAPAN_NET_MSG_UUID;
		nmg.dwSize	= sizeof(JAPAN_NET_LOGIN_UUID);
		memset( szUUID, 0, sizeof(CHAR) * UUID_STR_LENGTH );
	}
};

struct GS_NET_LOGIN_FEEDBACK_DATA2
{
	NET_MSG_GENERIC nmg;
	CHAR			szUserid[GS_USER_ID+1];
	CHAR			szIp[MAX_IP_LENGTH+1];
	INT				nClient;
	USHORT			nResult;
	INT				nUserNum;
	INT				nUserType;
	INT				nExtremeM;				// 극강부 남자 생성 가능 숫자
	INT				nExtremeW;				// 극강부 여자 생성 가능 숫자
	USHORT			uChaRemain;
	USHORT			uChaTestRemain;
    __time64_t      tPremiumTime;
	__time64_t      tChatBlockTime;
	
	GS_NET_LOGIN_FEEDBACK_DATA2()
	{
		nmg.dwSize = sizeof(GS_NET_LOGIN_FEEDBACK_DATA2);
		memset (szUserid, 0, sizeof(CHAR) * (GS_USER_ID+1));
		memset (szIp, 0, sizeof(CHAR) * (MAX_IP_LENGTH+1));
		nClient        = 0;
		nResult        = 0;
		nUserNum       = 0;
		nUserType      = 0;
		nExtremeM	   = 0;
		nExtremeW	   = 0;
		uChaRemain     = 0;
		uChaTestRemain = 0;
		tPremiumTime   = 0;
		tChatBlockTime = 0;
	}
};
typedef GS_NET_LOGIN_FEEDBACK_DATA2* LPGSNET_LOGIN_FEEDBACK_DATA2;

///////////////////////////////////////////////////////////////////////////////
// Agent Server -> Field Server 로그인 성공후 결과값 전송
// Field Server 에서 세팅함
struct NET_LOGIN_FEEDBACK_DATA3
{
	NET_MSG_GENERIC nmg;	
	CHAR			szUserid[USR_ID_LENGTH+1];	
	INT				nUserNum;
	
	NET_LOGIN_FEEDBACK_DATA3()
	{
		nmg.nType = EMNET_MSG(0);
		nmg.dwSize = sizeof(NET_LOGIN_FEEDBACK_DATA3);		
		nUserNum = 0;
		memset(szUserid, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
	}
};
typedef NET_LOGIN_FEEDBACK_DATA3* LPNET_LOGIN_FEEDBACK_DATA3;

struct NET_LOGOUT_DATA
{
	NET_MSG_GENERIC nmg;
	CHAR			szUserID[USR_ID_LENGTH+1];// 로그아웃 하는 사용자
	INT				nGameTime;				// 플레이시간
	INT				nUserNum;				// 사용자 번호
	INT				nChaNum;

	NET_LOGOUT_DATA()
	{
		nmg.nType = EMNET_MSG(0);
		nmg.dwSize = sizeof(NET_LOGOUT_DATA);
		memset(szUserID, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
		nGameTime = 0;
		nUserNum  = -1;
		nChaNum   = 0;
	}
};
typedef NET_LOGOUT_DATA* LPNET_LOGOUT_DATA;

struct DAUM_NET_LOGOUT_DATA
{	
	NET_MSG_GENERIC nmg;
	CHAR			szUID[DAUM_MAX_UID_LENGTH+1];// 로그아웃 하는 사용자
	INT				nGameTime;				// 플레이시간
	INT				nUserNum;				// 사용자 번호
	INT				nChaNum;

	DAUM_NET_LOGOUT_DATA()
	{
		nmg.nType = EMNET_MSG(0);
		nmg.dwSize = sizeof(DAUM_NET_LOGOUT_DATA);
		memset(szUID, 0, sizeof(CHAR) * (DAUM_MAX_UID_LENGTH+1));
		nGameTime = 0;
		nUserNum  = -1;
		nChaNum   = 0;
	}
};
typedef DAUM_NET_LOGOUT_DATA* LPDAUM_NET_LOGOUT_DATA;

struct NET_LOGOUT_DATA_SIMPLE
{
	NET_MSG_GENERIC nmg;
	CHAR			szUserID[USR_ID_LENGTH+1];// 로그아웃 하는 사용자
	
	NET_LOGOUT_DATA_SIMPLE()
	{
		nmg.nType = EMNET_MSG(0);
		nmg.dwSize = sizeof(NET_LOGOUT_DATA_SIMPLE);
		memset(szUserID, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
	}
};
typedef NET_LOGOUT_DATA_SIMPLE* LPNET_LOGOUT_DATA_SIMPLE;

struct NET_CHARACTER_INC_DEC
{
	NET_MSG_GENERIC nmg;
	int	            nUserNum;	

	NET_CHARACTER_INC_DEC()
	{
		nmg.dwSize = sizeof(NET_CHARACTER_INC_DEC);
		nmg.nType = NET_MSG_CHA_DECREASE;
		nUserNum  = 0;		
	}
};
typedef NET_CHARACTER_INC_DEC* LPNET_CHARACTER_INC_DEC;

struct NET_CREATE_ACCOUNT_DATA
{
	NET_MSG_GENERIC nmg;
	CHAR			szAccountName[USR_ID_LENGTH+1];
	CHAR			szPassword[USR_PASS_LENGTH+1];
	CHAR			szUserName[USR_ID_LENGTH+1];

	NET_CREATE_ACCOUNT_DATA()
	{
		nmg.nType = EMNET_MSG(0);
		nmg.dwSize = sizeof(NET_CREATE_ACCOUNT_DATA);
		memset(szAccountName, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
		memset(szPassword, 0, sizeof(CHAR) * (USR_PASS_LENGTH+1));
		memset(szUserName, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
	}	
};
typedef NET_CREATE_ACCOUNT_DATA* LPNET_CREATE_ACCOUNT_DATA;

struct NET_CLIENT_VERSION
{
	NET_MSG_GENERIC	nmg;
	int	nPatchProgramVer;
	int	nGameProgramVer;

	NET_CLIENT_VERSION()
		: nPatchProgramVer(0)
		, nGameProgramVer(0)
	{
		memset( this, 0, sizeof(NET_CLIENT_VERSION) );
		nmg.dwSize = sizeof(NET_CLIENT_VERSION);
	}
};
typedef NET_CLIENT_VERSION* LPNET_CLIENT_VERSION;

struct NET_REQ_RAND_KEY
{
	NET_MSG_GENERIC nmg;
	NET_REQ_RAND_KEY()
	{		
		nmg.dwSize = sizeof(NET_REQ_RAND_KEY);
		nmg.nType = NET_MSG_REQ_RAND_KEY;
	}
};
typedef NET_REQ_RAND_KEY* LPNET_REQ_RAND_KEY;


struct NET_ENCRYPT_KEY
{
	NET_MSG_GENERIC nmg;
	
	CHAR			szEncryptKey[ENCRYPT_KEY+1];

	NET_ENCRYPT_KEY()
	{		
		nmg.dwSize = sizeof(NET_ENCRYPT_KEY);
		nmg.nType = NET_MSG_SND_ENCRYPT_KEY;

		memset(szEncryptKey, 0, sizeof(CHAR) * (ENCRYPT_KEY+1));
	}
};
typedef NET_ENCRYPT_KEY* LPNET_ENCRYPT_KEY;

/*
struct NET_MSG_PING
{
	NET_MSG_GENERIC nmg;
	DWORD			stime;
	NET_MSG_PING()
	{
		memset(this, 0, sizeof(NET_MSG_PING));
		nmg.dwSize = sizeof(NET_MSG_PING);
	}
};
typedef NET_MSG_PING* LPNET_MSG_PING;
*/
// Full ServerInfo 
struct NET_SERVER_INFO
{
	NET_MSG_GENERIC nmg;
	G_SERVER_INFO	gsi;
	NET_SERVER_INFO()
	{
		memset(this, 0, sizeof(NET_SERVER_INFO));
		nmg.dwSize = sizeof(NET_SERVER_INFO);
	}
};
typedef NET_SERVER_INFO* LPNET_SERVER_INFO;

struct NET_SERVER_CUR_INFO
{
	NET_MSG_GENERIC		nmg;
	G_SERVER_CUR_INFO	gsci;
	NET_SERVER_CUR_INFO()
	{
		memset(this, 0, sizeof(NET_SERVER_CUR_INFO));
		nmg.dwSize = sizeof(NET_SERVER_CUR_INFO);
	}
};
typedef NET_SERVER_CUR_INFO* LPNET_SERVER_CUR_INFO;

struct NET_SERVER_CHANNEL_INFO
{
	NET_MSG_GENERIC		nmg;
	int nServerGroup;
	int nChannel;
	int nChannelCurrentUser;
	int nChannelMaxUser;
	bool bPk;
	
	NET_SERVER_CHANNEL_INFO()
	{		
		nmg.nType = NET_MSG_SND_CHANNEL_STATE;
		nmg.dwSize = sizeof(NET_SERVER_CHANNEL_INFO);
		
		nServerGroup        = 0;
		nChannel            = 0;
		nChannelCurrentUser = 0;
		nChannelMaxUser     = 0;
		bPk                 = true;
	}
};
typedef NET_SERVER_CHANNEL_INFO* LPNET_SERVER_CHANNEL_INFO;

struct NET_SERVER_CHANNEL_FULL_INFO
{
	NET_MSG_GENERIC		nmg;
	int nServerGroup;
	int nChannel;
	bool bChannelFull;
	int nChannelCurrentUser;

	NET_SERVER_CHANNEL_FULL_INFO()
	{		
		nmg.nType = NET_MSG_SND_CHANNEL_FULL_STATE;
		nmg.dwSize = sizeof(NET_SERVER_CHANNEL_FULL_INFO);

		nServerGroup = 0;
		nChannel     = 0;
		bChannelFull = FALSE;
		nChannelCurrentUser = 0;
	}
};
typedef NET_SERVER_CHANNEL_INFO* LPNET_SERVER_CHANNEL_INFO;

struct NET_CUR_INFO_LOGIN
{
	NET_MSG_GENERIC			nmg;
	G_SERVER_CUR_INFO_LOGIN gscil;
	NET_CUR_INFO_LOGIN()
	{
        nmg.nType = EMNET_MSG(0);		
		nmg.dwSize = sizeof(NET_CUR_INFO_LOGIN);
	}
};
typedef NET_CUR_INFO_LOGIN* LPNET_CUR_INFO_LOGIN;

struct NET_RECONNECT_FILED
{
	NET_MSG_GENERIC	nmg;
	NET_RECONNECT_FILED()
	{
		nmg.dwSize = sizeof(NET_RECONNECT_FILED);
		nmg.nType = NET_MSG_RECONNECT_FIELD;
	}
};
typedef NET_RECONNECT_FILED* LPNET_RECONNECT_FILED;

struct NET_NEW_CHA
{
	NET_MSG_GENERIC		nmg;

	int					nIndex;		//	케릭터 종류. ( class )
	WORD				wSchool;	//	학교.
	WORD				wFace;		//	얼굴.
	WORD				wHair;		//	헤어 스타일.
	WORD				wHairColor;	//	헤어 컬러.
	WORD				wSex;		//	성별
	char				szChaName[CHR_ID_LENGTH+1];

	NET_NEW_CHA() :
		nIndex(0),
		wSchool(0),
		wFace(0),
		wHair(0),
		wHairColor(0),
		wSex(0)
	{
		nmg.dwSize = sizeof(NET_NEW_CHA);
		nmg.nType = NET_MSG_CHA_NEW;
		memset(szChaName, 0, sizeof(char) * (CHR_ID_LENGTH+1));
	}
};
typedef NET_NEW_CHA* LPNET_NEW_CHA;

struct NET_NEW_CHA_FB
{
	NET_MSG_GENERIC		nmg;
	int					nResult;
	int					nChaNum;
    WORD                wChaRemain;
	int					nExtremeM; // 극강부 남자 생성 가능 숫자
	int					nExtremeW; // 극강부 여자 생성 가능 숫자
	NET_NEW_CHA_FB()
	{
		nmg.dwSize  = sizeof(NET_NEW_CHA_FB);
		nmg.nType  = NET_MSG_CHA_NEW_FB;
        nChaNum    = 0;
        wChaRemain = 0;
	}
};
typedef NET_NEW_CHA_FB* LPNET_NEW_CHA_FB;

// 로비에서 디스플레이 할 캐릭터의 기초정보를 요청한다.
struct NET_CHA_REQ_BA_INFO
{
	NET_MSG_GENERIC nmg;
	// int             nChannel; // 사용자가 조인하려는 채널

	NET_CHA_REQ_BA_INFO()
	{
		nmg.dwSize = sizeof(NET_CHA_REQ_BA_INFO);
		nmg.nType = NET_MSG_REQ_CHA_BAINFO;
		// nChannel  = 0;
	}
};
typedef NET_CHA_REQ_BA_INFO* LPNET_CHA_REQ_BA_INFO;

// 현재 서버에서 해당 유저가 가진 캐릭터의 번호를 넘겨준다.
struct NET_CHA_BBA_INFO
{
	NET_MSG_GENERIC		nmg;
	int					nChaSNum; // 현재 이 서버에서 생성된 캐릭터 갯수
	int					nChaNum[MAX_ONESERVERCHAR_NUM];
	NET_CHA_BBA_INFO()
	{
		memset(this, 0, sizeof(NET_CHA_BBA_INFO));
		nmg.dwSize = sizeof(NET_CHA_BBA_INFO);
		nChaSNum = 0;
	}
};
typedef NET_CHA_BBA_INFO* LPNET_CHA_BBA_INFO;

// 해당 캐릭터의 정보를 요청한다.
struct NET_CHA_BA_INFO
{
	NET_MSG_GENERIC		nmg;
	int					nChaNum;
	NET_CHA_BA_INFO()
	{		
		nmg.dwSize = sizeof(NET_CHA_BA_INFO);
		nChaNum = 0;
	}
};
typedef NET_CHA_BA_INFO* LPNET_CHA_BA_INFO;

struct NET_CHA_DEL
{
	NET_MSG_GENERIC		nmg;
	INT					nChaNum; // 삭제할 캐릭터 번호
	char				szPass2[USR_PASS_LENGTH+1];
	NET_CHA_DEL()
	{		
		nmg.dwSize = sizeof(NET_CHA_DEL);
		nChaNum = 0;
		memset(szPass2, 0, sizeof(char) * (USR_PASS_LENGTH+1));
	}
};
typedef NET_CHA_DEL* LPNET_CHA_DEL;

struct DAUM_NET_CHA_DEL
{
	NET_MSG_GENERIC		nmg;
	INT					nChaNum; // 삭제할 캐릭터 번호
	
	DAUM_NET_CHA_DEL()
	{
		nmg.dwSize = sizeof(DAUM_NET_CHA_DEL);
		nChaNum = 0;
	}
};
typedef DAUM_NET_CHA_DEL* LPDAUM_NET_CHA_DEL;

/**
 * 캐릭터 삭제정보 Client->Agent
 * \return 
 */
struct TERRA_NET_CHA_DEL
{
	NET_MSG_GENERIC		nmg;
	INT					nChaNum; // 삭제할 캐릭터 번호
	
	TERRA_NET_CHA_DEL()
	{
		nmg.dwSize = sizeof(TERRA_NET_CHA_DEL);
		nChaNum = 0;
	}
};
typedef TERRA_NET_CHA_DEL* LPTERRA_NET_CHA_DEL;

/**
 * 캐릭터 삭제정보 Client->Agent
 * \return 
 */
struct GSP_NET_CHA_DEL
{
	NET_MSG_GENERIC		nmg;
	INT					nChaNum; // 삭제할 캐릭터 번호
	
	GSP_NET_CHA_DEL()
	{
		nmg.dwSize = sizeof(GSP_NET_CHA_DEL);
		nmg.nType = GSP_NET_MSG_CHA_DEL;
		nChaNum = 0;
	}
};
typedef GSP_NET_CHA_DEL* LPGSP_NET_CHA_DEL;

/**
 * 캐릭터 삭제정보 Client->Agent
 * \return 
 */
struct EXCITE_NET_CHA_DEL
{
	NET_MSG_GENERIC		nmg;
	INT					nChaNum; // 삭제할 캐릭터 번호
	
	EXCITE_NET_CHA_DEL()
	{
		nmg.dwSize = sizeof(EXCITE_NET_CHA_DEL);
		nChaNum = 0;
	}
};
typedef EXCITE_NET_CHA_DEL* LPEXCITE_NET_CHA_DEL;

/**
 * 캐릭터 삭제정보 Client->Agent
 * \return 
 */
struct JAPAN_NET_CHA_DEL
{
	NET_MSG_GENERIC		nmg;
	INT					nChaNum; // 삭제할 캐릭터 번호
	
	JAPAN_NET_CHA_DEL()
	{
		nmg.dwSize = sizeof(JAPAN_NET_CHA_DEL);
		nChaNum = 0;
	}
};
typedef JAPAN_NET_CHA_DEL* LPJAPAN_NET_CHA_DEL;

/**
 * 캐릭터 삭제정보 Client->Agent
 * \return 
 */
struct GS_NET_CHA_DEL
{
	NET_MSG_GENERIC		nmg;
	INT					nChaNum; // 삭제할 캐릭터 번호
	char				szPass2[GS_USER_PASS+1];

	GS_NET_CHA_DEL()
	{		
		nmg.dwSize = sizeof(GS_NET_CHA_DEL);
		nChaNum = 0;
		memset(szPass2, 0, sizeof(char) * (GS_USER_PASS+1));
	}
};
typedef GS_NET_CHA_DEL* LPGS_NET_CHA_DEL;

struct NET_CHA_DEL_FB
{
	NET_MSG_GENERIC		nmg;	
	INT					nChaNum;	// 삭제한 캐릭터 번호
	INT					nExtremeM;	// 극강부 남자 생성 가능 숫자
	INT					nExtremeW;	// 극강부 여자 생성 가능 숫자
    WORD                wChaRemain; // 남은 캐릭터 생성 갯수
	NET_CHA_DEL_FB()
	{
		nmg.dwSize  = sizeof(NET_CHA_DEL_FB);
		nChaNum		= 0;
        wChaRemain	= 0;
		nExtremeM	= 0;
		nExtremeW	= 0;
	}
};
typedef NET_CHA_DEL_FB* LPNET_CHA_DEL_FB;

// 게임참가 메시지
// Client -> Agent Server
struct NET_GAME_JOIN
{
	NET_MSG_GENERIC		nmg;
	INT					nChaNum; // 게임 참가할 캐릭터 번호
	NET_GAME_JOIN()
	{
		nmg.dwSize = sizeof(NET_GAME_JOIN);
		nChaNum = 0;
	}
};
typedef NET_GAME_JOIN* LPNET_GAME_JOIN;

enum EMGAME_JOINTYPE
{
	EMJOINTYPE_FIRST	= 0,	//	케릭터 처음 접속했을때.
	EMJOINTYPE_MOVEMAP	= 1,	//	맵이동중 서버를 이동하게 되어 다시 접속.
	EMJOINTYPE_REBIRTH	= 2,	//	부활도중 서버를 이동하게 되어 다시 접속.
};

// 게임참가 메시지
// Agent -> Field
struct NET_GAME_JOIN_FIELDSVR
{
	NET_MSG_GENERIC		nmg;

	EMGAME_JOINTYPE		emType;
	DWORD				dwSlotAgentClient;

	char				szUID[USR_ID_LENGTH+1];
	INT					nUserNum;
	DWORD				dwUserLvl;

	INT					nChaNum;
	DWORD				dwGaeaID;

	SNATIVEID			sStartMap;
	DWORD				dwStartGate;
	D3DXVECTOR3			vStartPos;

	DWORD				dwActState;

    __time64_t          tPREMIUM;
    __time64_t          tCHATBLOCK;

	int					dwThaiCCafeClass;
	int					nMyCCafeClass;					// 말레이시아 PC방 이벤트

	SChinaTime			sChinaTime;
	SEventTime			sEventTime;
	SVietnamGainSystem	sVietnamGainSystem;


	BOOL				bUseArmSub;

	NET_GAME_JOIN_FIELDSVR() :
		emType(EMJOINTYPE_FIRST),
		dwSlotAgentClient(0),
		
		nUserNum(0),
		dwUserLvl(0),

		nChaNum(0),
		dwGaeaID(0),
		
		dwStartGate(UINT_MAX),
		vStartPos(0,0,0),

		dwActState(NULL),
		tPREMIUM(0),
		tCHATBLOCK(0),

		bUseArmSub(FALSE),

		dwThaiCCafeClass(0),
		nMyCCafeClass(0)
	{
		nmg.dwSize	= sizeof(NET_GAME_JOIN_FIELDSVR);
		nmg.nType	= MET_MSG_GAME_JOIN_FIELDSVR;

		memset( szUID, 0, sizeof(char)*(USR_ID_LENGTH+1) );

		sChinaTime.Init();
		sEventTime.Init();
		sVietnamGainSystem.Init();
	}
};
typedef NET_GAME_JOIN_FIELDSVR* LPNET_GAME_JOIN_FIELDSVR;

struct NET_GAME_JOIN_FIELDSVR_FB
{
	NET_MSG_GENERIC nmg;

	EMGAME_JOINTYPE	emType;
	DWORD			dwSlotFieldAgent;

	DWORD			dwFieldSVR;
	DWORD           dwGaeaID;
	INT				nChaNum;

	NET_GAME_JOIN_FIELDSVR_FB () :
		emType(EMJOINTYPE_FIRST),
		dwSlotFieldAgent(0),

		dwFieldSVR(0),
		dwGaeaID(0),
		nChaNum(0)
	{
		nmg.nType   = MET_MSG_GAME_JOIN_FIELDSVR_FB;
		nmg.dwSize   = sizeof(*this);
	};	
};
typedef NET_GAME_JOIN_FIELDSVR_FB* LPNET_GAME_JOIN_FIELDSVR_FB;

struct NET_CONNECT_CLIENT_TO_FIELD
{
	NET_MSG_GENERIC	nmg;
	EMGAME_JOINTYPE	emType;
	DWORD			dwGaeaID;
	DWORD			dwSlotFieldAgent;
	int				nServicePort;
	CHAR			szServerIP[MAX_IP_LENGTH+1];

	NET_CONNECT_CLIENT_TO_FIELD() 
		: emType(EMJOINTYPE_FIRST)
	{
		nmg.dwSize = sizeof(NET_CONNECT_CLIENT_TO_FIELD);
		nmg.nType = NET_MSG_CONNECT_CLIENT_FIELD;
		dwSlotFieldAgent = 0;
	}
};
typedef NET_CONNECT_CLIENT_TO_FIELD* LPNET_CONNECT_CLIENT_TO_FIELD;

//	클라이언트가 새로이 Field Server와 연결후 자신의 ID 확인 요청.
struct NET_GAME_JOIN_FIELD_IDENTITY
{
	NET_MSG_GENERIC		nmg;
	EMGAME_JOINTYPE		emType;

	DWORD				dwGaeaID; 
	DWORD				dwSlotFieldAgent;
	CRYPT_KEY			ck;

	NET_GAME_JOIN_FIELD_IDENTITY () :
		emType(EMJOINTYPE_FIRST),

		dwGaeaID(0),
		dwSlotFieldAgent(0)
	{
		nmg.dwSize = sizeof(*this);
		nmg.nType = NET_MSG_JOIN_FIELD_IDENTITY;
	}
};
typedef NET_GAME_JOIN_FIELD_IDENTITY* LPNET_GAME_JOIN_FIELD_IDENTITY;

// 게임참가완료
// Field->Session
struct NET_GAME_JOIN_OK
{
	NET_MSG_GENERIC		nmg;
	INT					nUsrNum; // 사용자 고유번호(DB)	
	INT					nChaNum; // 캐릭터번호(DB)
	DWORD				dwGaeaID; // 가이아 ID
	CHAR				szUserID[USR_ID_LENGTH+1]; // 사용자 ID	
	CHAR				szChaName[CHR_ID_LENGTH+1]; // 캐릭터명

	NET_GAME_JOIN_OK()
	{		
		nmg.dwSize = sizeof(NET_GAME_JOIN_OK);
		nmg.nType = NET_MSG_GAME_JOIN_OK;
		nUsrNum = -1;
		nChaNum = -1;
		dwGaeaID = -1;
		memset(szUserID, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
		memset(szChaName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
	}
};
typedef NET_GAME_JOIN_OK* LPNET_GAME_JOIN_OK;

// 채팅메시지
struct NET_CHAT
{
	NET_MSG_GENERIC	nmg;
	EMNET_MSG_CHAT	emType;						// 채팅 타입
	CHAR			szName[CHR_ID_LENGTH+1];		// 특정 캐릭터명 or 사용자명
	CHAR			szChatMsg[CHAT_MSG_SIZE];	// 채팅메시지

	NET_CHAT () :
		emType(CHAT_TYPE_NORMAL)
	{
		nmg.nType = NET_MSG_CHAT;
		nmg.dwSize = sizeof(NET_CHAT);

		memset(szName,    0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
		memset(szChatMsg, 0, sizeof(CHAR) * (CHAT_MSG_SIZE));
	}
};
typedef NET_CHAT* LPNET_CHAT;

// 관리용 채팅메시지
// 컨트롤프로그램->세션서버
struct NET_CHAT_CTRL
{
	NET_MSG_GENERIC nmg;
	EMNET_MSG_CHAT	emType;
	CHAR			szServerIP[MAX_IP_LENGTH+1];
	INT				nServicePort;
	CHAR			szChatMsg[GLOBAL_CHAT_MSG_SIZE+1];

	NET_CHAT_CTRL()
	{		
		nmg.nType    = NET_MSG_CHAT;
		nmg.dwSize    = sizeof(NET_CHAT_CTRL);
		emType	     = CHAT_TYPE_CTRL_GLOBAL;
		nServicePort = 0;
		memset(szServerIP, 0, sizeof(CHAR) * (MAX_IP_LENGTH+1));
		memset(szChatMsg, 0, sizeof(CHAR) * (GLOBAL_CHAT_MSG_SIZE+1));
	}
};
typedef NET_CHAT_CTRL* LPNET_CHAT_CTRL;

struct NET_CHAT_CTRL2
{
	NET_MSG_GENERIC nmg;
	EMNET_MSG_CHAT	emType;	
	int				nSvrGrp;
	CHAR			szChatMsg[GLOBAL_CHAT_MSG_SIZE+1];	

	NET_CHAT_CTRL2()
	{
		nmg.nType	= NET_MSG_CHAT;
		nmg.dwSize	= sizeof(NET_CHAT_CTRL2);
		emType		= CHAT_TYPE_CTRL_GLOBAL2;
		nSvrGrp		= 0;
		memset(szChatMsg, 0, sizeof(CHAR) * (GLOBAL_CHAT_MSG_SIZE+1));
	}
};
typedef NET_CHAT_CTRL2* LPNET_CHAT_CTRL2;

// 채팅메시지 FB
struct NET_CHAT_FB
{
	NET_MSG_GENERIC nmg;
	EMNET_MSG_CHAT	emType;						// 채팅 타입
	CHAR			szName[CHR_ID_LENGTH+1];		// 보낸사람 캐릭터명 or 사용자명
	CHAR			szChatMsg[CHAT_MSG_SIZE+1];	// 채팅메시지

	NET_CHAT_FB ()
	{
		nmg.nType = NET_MSG_CHAT_FB;
		nmg.dwSize = sizeof(NET_CHAT_FB);
		emType = CHAT_TYPE_NORMAL;
		memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
		memset(szChatMsg, 0, sizeof(CHAR) * (CHAT_MSG_SIZE+1));
	}
};
typedef NET_CHAT_FB* LPNET_CHAT_FB;

// 관리자용 메시지 FB
struct NET_CHAT_CTRL_FB
{
	NET_MSG_GENERIC nmg;
	EMNET_MSG_CHAT	emType;						// 채팅 타입
	CHAR			szChatMsg[GLOBAL_CHAT_MSG_SIZE+1];	// 채팅메시지

	NET_CHAT_CTRL_FB ()
	{
		nmg.nType = NET_MSG_CHAT_CTRL_FB;
		nmg.dwSize = sizeof(NET_CHAT_CTRL_FB);
		emType = CHAT_TYPE_GLOBAL;
		memset(szChatMsg, 0, sizeof(CHAR) * (GLOBAL_CHAT_MSG_SIZE+1));
	}
};
typedef NET_CHAT_FB* LPNET_CHAT_FB;

struct NET_EVENT_LOTTERY
{
	NET_MSG_GENERIC nmg;
	CHAR szLotteryName[11];

	NET_EVENT_LOTTERY()
	{		
		nmg.nType = NET_MSG_EVENT_LOTTERY;
		nmg.dwSize = sizeof(NET_EVENT_LOTTERY);
		memset(szLotteryName, 0, sizeof(CHAR) * 11);
	}
};
typedef NET_EVENT_LOTTERY* LPNET_EVENT_LOTTERY;

///////////////////////////////////////////////////////////////////////////////
// 란 선물펑펑!! 이벤트 
// 리턴값 :
// 1  1 등 P-4 PC                        (1등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.)
// 2  2 등 효리폰                        (2등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.)
// 3  3 등 SONY 디카                     (3등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.)
// 4  4 등 아이리버 MP 플레이어          (4등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.)
// 5  5 등 MAX 무비 영화 상품권 1 만원권 (5등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.)
// 6  6 등 MBC Game 일일 무료 상품권     (6등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.) 
// 7  이미 복권을 입력한 사용자          (이미 복권에 응모하셨습니다.)
// 8  PC 방 IP 가 아님                   (PC 방 IP 가 아닙니다. 복권은 PC 방에서만 응모 가능합니다)
// 9  이미 사용한 복권                   (이미 사용한 복권입니다)
// 10 알수없는 오류                      (복권처리중 오류가 발생하였습니다)
struct NET_EVENT_LOTTERY_FB
{
	NET_MSG_GENERIC nmg;
	int nResult;

	NET_EVENT_LOTTERY_FB()
	{
		nmg.nType = NET_MSG_EVENT_LOTTERY_FB;
		nmg.dwSize = sizeof(NET_EVENT_LOTTERY_FB);
		int nResult = 10;
	}
};
typedef NET_EVENT_LOTTERY_FB* LPNET_EVENT_LOTTERY_FB;

// 출석부 이벤트
struct USER_ATTEND_INFO
{
	__time64_t	tAttendTime;
	int			nComboAttend;
	bool		bAttendReward;

	USER_ATTEND_INFO()
	{
		tAttendTime = 0;
		nComboAttend = 0;
		bAttendReward = false;
	}
};
typedef USER_ATTEND_INFO* LPUSER_ATTEND_INFO;


// Apex 적용( 홍콩 )
struct NET_APEX_RETURN
{
	NET_MSG_GENERIC nmg;
	int nReturn;

	NET_APEX_RETURN()
		: nReturn ( 0 )
	{
		nmg.nType  = NET_MSG_APEX_RETURN;
		nmg.dwSize = sizeof(NET_APEX_RETURN);		
	}
};

//! Apex 메세지
struct NET_APEX_DATA
{
	NET_MSG_GENERIC nmg;
	CHAR szData[MAX_APEX_PACKET_LENGTH];
	
	NET_APEX_DATA()
	{
		nmg.nType  = NET_MSG_APEX_DATA;
		nmg.dwSize = sizeof(NET_APEX_DATA);
		memset( szData, 0, sizeof(CHAR) * MAX_APEX_PACKET_LENGTH );
	}
};
typedef NET_APEX_DATA* LPNET_APEX_DATA;

//! Apex 메세지
struct NET_APEX_ANSWER
{
	NET_MSG_GENERIC nmg;
	WORD			wAction;
	WORD			wHour;

	NET_APEX_ANSWER()
	{
		nmg.nType  = NET_MSG_APEX_ANSWER;
		nmg.dwSize = sizeof(NET_APEX_ANSWER);
		wAction = 0;
		wHour = 0;
	}
};
typedef NET_APEX_ANSWER* LPNET_APEX_ANSWER;

//! nProtect GameGuard Auth 요청 메시지
struct NET_GAMEGUARD_AUTH
{
	NET_MSG_GENERIC nmg;
	GG_AUTH_DATA ggad;

	NET_GAMEGUARD_AUTH()
	{
		nmg.nType  = NET_MSG_GAMEGUARD_AUTH;
		nmg.dwSize = sizeof(NET_GAMEGUARD_AUTH);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
	}	
};
typedef NET_GAMEGUARD_AUTH* LPNET_GAMEGUARD_AUTH;

//! nProtect GameGuard Auth Answer 메시지
struct NET_GAMEGUARD_ANSWER
{
	NET_MSG_GENERIC nmg;
	GG_AUTH_DATA ggad;
	
	CHAR	szEnCrypt[ENCRYPT_KEY+1];


	NET_GAMEGUARD_ANSWER()
	{
		nmg.nType  = NET_MSG_GAMEGUARD_ANSWER;
		nmg.dwSize = sizeof(NET_GAMEGUARD_ANSWER);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
		SecureZeroMemory(szEnCrypt, sizeof(CHAR) * (ENCRYPT_KEY+1));
	}	
};
typedef NET_GAMEGUARD_ANSWER* LPNET_GAMEGUARD_ANSWER;

//! nProtect GameGuard Auth 요청 메시지 #1
struct NET_GAMEGUARD_AUTH_1
{
	NET_MSG_GENERIC nmg;
	GG_AUTH_DATA ggad;

	NET_GAMEGUARD_AUTH_1()
	{
		nmg.nType  = NET_MSG_GAMEGUARD_AUTH_1;
		nmg.dwSize = sizeof(NET_GAMEGUARD_AUTH_1);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
	}	
};
typedef NET_GAMEGUARD_AUTH_1* LPNET_GAMEGUARD_AUTH_1;

//! nProtect GameGuard Auth Answer 메시지 #1
struct NET_GAMEGUARD_ANSWER_1
{
	NET_MSG_GENERIC nmg;
	GG_AUTH_DATA ggad;

	NET_GAMEGUARD_ANSWER_1()
	{
		nmg.nType  = NET_MSG_GAMEGUARD_ANSWER_1;
		nmg.dwSize = sizeof(NET_GAMEGUARD_ANSWER_1);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
	}	
};
typedef NET_GAMEGUARD_ANSWER_1* LPNET_GAMEGUARD_ANSWER_1;

//! nProtect GameGuard Auth 요청 메시지 #2
struct NET_GAMEGUARD_AUTH_2
{
	NET_MSG_GENERIC nmg;
	GG_AUTH_DATA ggad;

	NET_GAMEGUARD_AUTH_2()
	{
		nmg.nType  = NET_MSG_GAMEGUARD_AUTH_2;
		nmg.dwSize = sizeof(NET_GAMEGUARD_AUTH_2);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
	}	
};
typedef NET_GAMEGUARD_AUTH_2* LPNET_GAMEGUARD_AUTH_2;

//! nProtect GameGuard Auth Answer 메시지 #2
struct NET_GAMEGUARD_ANSWER_2
{
	NET_MSG_GENERIC nmg;
	GG_AUTH_DATA ggad;

	NET_GAMEGUARD_ANSWER_2()
	{
		nmg.nType  = NET_MSG_GAMEGUARD_ANSWER_2;
		nmg.dwSize = sizeof(NET_GAMEGUARD_ANSWER_2);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
	}	
};
typedef NET_GAMEGUARD_ANSWER_2* LPNET_GAMEGUARD_ANSWER_2;

// 추적할 id 추가
struct NET_UPDATE_TRACINGCHAR_ALL
{
	NET_MSG_GENERIC nmg;
	STracingData tracingData[MAX_TRACING_NUM];

	NET_UPDATE_TRACINGCHAR_ALL () 
	{
		nmg.nType = NET_MSG_UPDATE_TRACING_ALL;
		nmg.dwSize = sizeof(NET_UPDATE_TRACINGCHAR_ALL);
		
		BYTE i;
		for( i = 0; i < 10; i++ )
		{
			tracingData[i].dwUserNum      = 0;
			tracingData[i].strAccount     = "";
			tracingData[i].dwOnLine		  = 255;
			tracingData[i].nConnectServer = -1;
		}
	}
};

// 추적할 id 추가
struct NET_UPDATE_TRACINGCHAR
{
	NET_MSG_GENERIC nmg;
	BYTE		 updateNum;
	STracingData tracingData;

	NET_UPDATE_TRACINGCHAR () :
		updateNum(0)
	{
		nmg.nType = NET_MSG_UPDATE_TRACING_CHARACTER;
		nmg.dwSize = sizeof(NET_UPDATE_TRACINGCHAR);

		tracingData.dwUserNum      = 0;
		tracingData.strAccount     = "";
		tracingData.dwOnLine		  = 255;
		tracingData.nConnectServer = -1;
	}
};


// 추적중인 유저의 로그를 agent서버에 보낸다.
struct NET_LOG_UPDATE_TRACINGCHAR
{
	NET_MSG_GENERIC nmg;
	CHAR szAccount[USR_ID_LENGTH+1];
	int  nUserNum;
	CHAR szLogMsg[TRACING_LOG_SIZE];

	NET_LOG_UPDATE_TRACINGCHAR () :
			nUserNum(-1)
	{
		nmg.nType = NET_MSG_LOG_UPDATE_TRACING_CHARACTER;
		nmg.dwSize = sizeof(NET_LOG_UPDATE_TRACINGCHAR);
		memset(szAccount, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
		memset(szLogMsg, 0, sizeof(CHAR) * (TRACING_LOG_SIZE));
	}
};

//< 필드->클라이언트 추적 유저인지 아닌지를 보낸다.
struct NET_TRACING_CHARACTER
{
	NET_MSG_GENERIC nmg;
	bool			bTracingChar;

	NET_TRACING_CHARACTER () :
		bTracingChar(FALSE)
	{
		nmg.nType = NET_MSG_TRACING_CHARACTER;
		nmg.dwSize = sizeof(NET_TRACING_CHARACTER);

	}
};


// 각각의 아이템의 최대 고유번호를 가져오기 위한 구조체
struct VIEWLOGITEMEXCHANGEMAX
{
    LONGLONG llMaxNum;
    int  nMakeType;
    WORD wItemMain;
    WORD wItemSub;

    VIEWLOGITEMEXCHANGEMAX()
    {
        llMaxNum  = 0;
        nMakeType = 0;
        wItemMain = 0;
        wItemSub  = 0;
    }
};

// AGT->Session 으로 LoginMaintenance On/Off
struct SNET_SERVER_LOGINMAINTENANCE_SESSION
{
	NET_MSG_GENERIC nmg;
	bool	bOn;

	SNET_SERVER_LOGINMAINTENANCE_SESSION ()
	{
		nmg.nType = NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_SESSION;
		nmg.dwSize = sizeof( SNET_SERVER_LOGINMAINTENANCE_SESSION );
		bOn = false;
	}
};

struct SNET_SERVER_LOGINMAINTENANCE_LOGIN
{
	NET_MSG_GENERIC nmg;
	bool	bOn;

	SNET_SERVER_LOGINMAINTENANCE_LOGIN ()
	{
		nmg.nType = NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_LOGIN;
		nmg.dwSize = sizeof( SNET_SERVER_LOGINMAINTENANCE_LOGIN );
		bOn = false;
	}
};

// Shop 에서 구매한 아이템리스트를 가져오기 위한 구조체
// char 21 자리
#define PURKEY_LENGTH 30

struct SHOPPURCHASE
{
    CString strPurKey;
    WORD    wItemMain;
    WORD    wItemSub;

    SHOPPURCHASE()
    {
        wItemMain = 0;
        wItemSub  = 0;
    }

	SHOPPURCHASE( const SHOPPURCHASE &value ) 
		: strPurKey(value.strPurKey)
		, wItemMain(value.wItemMain)
		, wItemSub(value.wItemSub)
    {
	}

	SHOPPURCHASE& operator= ( const SHOPPURCHASE &value )
	{
		strPurKey = value.strPurKey;
		wItemMain = value.wItemMain;
		wItemSub = value.wItemSub;

		return *this;
	}
};

///////////////////////////////////////////////////////////////////////////////
// Global Function

/// 네트워크 초기화
int	NET_InitializeSocket(void);
/// 네크워크 종료
void NET_CloseSocket(void);
/// Winsock2 에러 코드->문자열 함수
CString NET_ERROR_STRING(int nCode);

/******************************************************************************
* GameGuard 에러 코드 체크 함수
* \param dwErrCode nProtect GameGuard 에러코드
*/
std::string GetGameGuardErrorString(DWORD dwErrCode);

#endif // S_NETGLOBAL_H_

//                             ,|     
//                           //|                              ,|
//                         //,/                             -~ |
//                       // / |                         _-~   /  ,
//                     /'/ / /                       _-~   _/_-~ |
//                    ( ( / /'                   _ -~     _-~ ,/'
//                     \~\/'/|             __--~~__--\ _-~  _/,
//             ,,)))))));, \/~-_     __--~~  --~~  __/~  _-~ /
//          __))))))))))))));,>/\   /        __--~~  \-~~ _-~
//         -\(((((''''(((((((( >~\/     --~~   __--~' _-~ ~|
//--==//////((''  .     `)))))), /     ___---~~  ~~\~~__--~ 
//        ))| @    ;-.     (((((/           __--~~~'~~/
//        ( `|    /  )      )))/      ~~~~~__\__---~~__--~~--_
//           |   |   |       (/      ---~~~/__-----~~  ,;::'  \         ,
//           o_);   ;        /      ----~~/           \,-~~~\  |       /|
//                 ;        (      ---~~/         `:::|      |;|      < >
//                |   _      `----~~~~'      /      `:|       \;\_____// 
//          ______/\/~    |                 /        /         ~------~
//        /~;;.____/;;'  /          ___----(   `;;;/               
//       / //  _;______;'------~~~~~    |;;/\    /          
//      //  | |                        /  |  \;;,\              
//     (<_  | ;                      /',/-----'  _>
//      \_| ||_                     //~;~~~~~~~~~ 
//          `\_|                   (,~~ 
//                                  \~\ 
//                                   ~~ 
// Fast Fast More Fast~! 
// Fly To The Moon~! 
// Fly Fly~!
// Jgkim

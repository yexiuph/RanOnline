#ifndef _GG_AUTH_SERVER_H_
#define _GG_AUTH_SERVER_H_

#ifndef WIN32
	typedef unsigned int   DWORD;
        typedef int            BOOL;
        typedef char*          PCHAR;
        typedef DWORD*         PDWORD;
        typedef unsigned char  BYTE;
        typedef void*          PVOID;
        typedef void*          LPVOID;
        typedef void*          HMODULE;

        #define TRUE  1
        #define FALSE 0

        #ifndef MAX_PATH
                #define MAX_PATH 256
        #endif

        #define LoadLibrary(x) dlopen(x, RTLD_LAZY)
        #define GetProcAddress dlsym
        #define FreeLibrary    dlclose
#endif
								
#ifdef _EXPORT_DLL
	#define GGAUTHS_API    extern "C" __declspec(dllexport)
	#define GGAUTHS_EXPORT __declspec(dllexport)
	#define __CDECL        __cdecl
#else
	#define GGAUTHS_API 
	#define GGAUTHS_EXPORT 
	#define __CDECL 
#endif

// gameguard auth data
typedef struct _GG_AUTH_DATA
{
	DWORD dwIndex;
	DWORD dwValue1;
	DWORD dwValue2;
	DWORD dwValue3;
} GG_AUTH_DATA, *PGG_AUTH_DATA;

typedef DWORD (*PRTC_GET_VERSION)();
typedef DWORD (*PRTC_GET_AUTH_QUERY)(DWORD, PDWORD, PGG_AUTH_DATA, PGG_AUTH_DATA);
typedef DWORD (*PRTC_CHECK_AUTH_ANSWER)(DWORD, PDWORD, PGG_AUTH_DATA, PGG_AUTH_DATA);

typedef struct _GG_AUTH_PROTOCOL
{
	HMODULE hProtocol;     // Protocol DLL handle
	DWORD   dwVersion;     // protocol version
	DWORD   dwReferCount;  // protocol data reference count
	DWORD   dwStateFlag;   // protocol state flag
	PRTC_GET_AUTH_QUERY    PrtcGetAuthQuery;
	PRTC_CHECK_AUTH_ANSWER PrtcCheckAuthAnswer;
	PVOID   pNext;         // next data block
} GG_AUTH_PROTOCOL, *PGG_AUTH_PROTOCOL;

typedef struct _GG_AUTH_USER
{
	PGG_AUTH_PROTOCOL m_pProtocol;
	DWORD             m_bPrtcRef;
	DWORD             m_dwUserFlag;
	GG_AUTH_DATA      m_AuthQuery;
	GG_AUTH_DATA      m_AuthAnswer;
} GG_AUTH_USER, *PGG_AUTH_USER;

// ggauth.dll Path
GGAUTHS_API DWORD __CDECL InitGameguardAuth(char* sGGPath, DWORD dwNumActive);
GGAUTHS_API void  __CDECL CleanupGameguardAuth();

// protocol dll name
GGAUTHS_API DWORD __CDECL AddAuthProtocol(char* sDllName);

// C type CSAuth2
GGAUTHS_API void __CDECL GGAuthInitUser(PGG_AUTH_USER pAuthUser);                               // CCSAuth2()
GGAUTHS_API void __CDECL GGAuthCloseUser(PGG_AUTH_USER pAuthUser);                           // ~CCSAuth2()
GGAUTHS_API DWORD    __CDECL GGAuthGetQuery(PGG_AUTH_USER pAuthUser);     // GetAuthQuery()
GGAUTHS_API DWORD    __CDECL GGAuthCheckAnswer(PGG_AUTH_USER pAuthUser);  // CheckAuthAnswer()

#endif

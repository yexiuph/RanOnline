#pragma once

#ifdef _DEBUG
	void* __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
	#define DEBUG_NEW new(THIS_FILE, __LINE__)
	void __cdecl operator delete(void* p, LPCSTR lpszFileName, int nLine);
#else
	#define DEBUG_NEW new
#endif

void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine);

void __cdecl operator delete[](void* p, LPCSTR lpszFileName, int nLine);

//	Note : 커스텀 Assert 함수.
//
bool _gassert ( bool bExp, const char *const szExp, const char *const szFile, const long nLine  );

#ifdef  NDEBUG
#define GASSERT(exp)	((void)0)
#else
#define GASSERT(exp)	{ if ( _gassert ( (exp)?true:false, #exp, __FILE__, __LINE__ ) )	{ _asm { int 3 } } }
#endif


//	Note : 순수 가상 함수 호출 오류 검출.
//
void smtm_PurecallHandler(void);


//	Note : 런타임 오류 검출.
//
#include <rtcapi.h>
#include <windows.h>

extern void DoRTC_Initialization (void);
extern void DoRTC_Termination (void);

#ifdef __MSVC_RUNTIME_CHECKS
	#ifdef NOCRT
		extern char *IntToString ( int i );
	#endif

	extern int Catch_RTC_Failure ( int errType, const char *file, int line, const char *module, const char *format, ... );
#endif

class DO_RTC_CATCH
{
public:
	DO_RTC_CATCH ();
	~DO_RTC_CATCH ();
};

inline DO_RTC_CATCH::DO_RTC_CATCH ()
{
	#ifndef NDEBUG
		DoRTC_Initialization ();
	#endif
}

inline DO_RTC_CATCH::~DO_RTC_CATCH ()
{
	#ifndef NDEBUG
		DoRTC_Termination ();
	#endif
}

void _LOOPWATCH ( const char* szTAG );

void SETMESSAGEBOX ( bool bToLogFile );

int MESSAGEBOX ( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType );

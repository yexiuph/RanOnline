#include "pch.h"
#include "gassert.h"
#include "DebugSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool _gassert ( bool bExp, const char *const szExp, const char *const szFile, const long nLine  )
{
		if ( !bExp )
		{
			#ifndef NDEBUG
			{
				//std::strstream strText;
				//strText << std::endl;
				//strText << "Assert!" << std::endl;
				//strText << std::endl;
				//strText << "File : " << szFile << std::endl;
				//strText << "Line : " << nLine << std::endl;
				//strText << std::endl;
				//strText << std::endl;
				//strText << "Expression : " << szExp << " is fail." << std::endl;
				//strText << std::endl;
				//strText << "Debug Now?" << std::endl;
				//strText << std::endl;
				//strText << std::ends;

				TCHAR szTemp[256] = {0};
				_snprintf( szTemp, 256,
						"Assert!\n\n"
						"File : %s\n"
						"Line : %d\n\n\n"
						"Expression : %s is fail.\n\n"
						"Debug Now?\n\n", szFile, nLine, szExp );

				if ( IDYES == MessageBox ( NULL, szTemp, "ASSERT", MB_ICONEXCLAMATION | MB_YESNO ) )
				{
					//strText.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
					return true;
				}
				//else
				//{
					//strText.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				//}
			}
			#else
			{
				//std::strstream strText;
				//strText << std::endl;
				//strText << "ASSERT:Error occured!";
				//strText << "File:" << szFile << " ";
				//strText << "Line:" << nLine << " ";
				//strText << "Expression:" << szExp << " is false." << std::ends;

				CDebugSet::ToLogFile( "\nASSERT:Error occured!"
									"File : %s, Line : %d, Expression : %s is false.",
									szFile, nLine, szExp );
				//strText.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			}
			#endif

			return false;
		}

	return false;
}

void smtm_PurecallHandler(void)
{
	_gassert ( 0, "pure function is called.", __FILE__, __LINE__ );
}

void DoRTC_Initialization ()
{
	#ifdef __MSVC_RUNTIME_CHECKS
		#ifdef NOCRT
			_RTC_Initialize();
		#else
			// This is only necessary when using a custom RTC handler function.
			// If using a debug CRT and a custom handler is *not* designated with
			// _RTC_SetErrorFunc, then failures will be reported via _CrtDbgReport.
			int Catch_RTC_Failure(int errType, const char *file, int line, const char *module, const char *format, ...);
			_RTC_SetErrorFunc(Catch_RTC_Failure);
		#endif
	#endif
	// Do any other initialization here
}

void DoRTC_Termination ()
{
	// Do any other termination work here
	#if defined(__MSVC_RUNTIME_CHECKS) && defined(NOCRT)
		_RTC_Terminate();
	#endif
}


#ifdef __MSVC_RUNTIME_CHECKS

	#include <stdarg.h>
	#include <malloc.h>

	#ifdef NOCRT
		// Use intrinsics, so we don't add CRT dependencies
		#pragma intrinsic(strcat)
		#pragma intrinsic(strcpy)
	    
		char* IntToString(int i)
		{
			static char buf[15];
			bool neg = i < 0;
			int pos = 14;
			buf[14] = 0;
			do
			{
				buf[--pos] = i % 10 + '0';
				i /= 10;
			}
			while (i);

			if (neg)	buf[--pos] = '-';

			return &buf[pos];
		}
	#else // NOCRT is undefined
		#include <stdio.h>
	#endif

	int Catch_RTC_Failure(int errType, const char *file, int line, const char *module, const char *format, ...)
	{
		// Prevent re-entrancy ; isn't necessary here, but if this were a
		// multi-threading program, it would be.
		static long running = 0;
		while (InterlockedExchange(&running, 1))	Sleep( 0 );

		// First, get the RTC error number from the var-arg list...
		va_list vl;
		va_start(vl, format);
		_RTC_ErrorNumber rtc_errnum = va_arg ( vl, _RTC_ErrorNumber );
		va_end(vl);
	    
		// Build a string buffer to display in a message box
		char buf[MAX_PATH] = {0};

		#ifdef NOCRT
			const char *err = _RTC_GetErrDesc(rtc_errnum);
			StringCchCopy( buf, MAX_PATH, err );
			if (line)
			{
				StringCchCat (buf, MAX_PATH, "\nLine #" );
				StringCchCat( buf, MAX_PATH, IntToString(line) );
			}
			if (file)
			{
				StringCchCat( buf, MAX_PATH, "\nFile:" );
				StringCchCat( buf, MAX_PATH, file );
			}
			if (module)
			{
				StringCchCat( buf, MAX_PATH, "\nModule:" );
				StringCchCat( buf, MAX_PATH, module );
			}
		#else // NOCRT is undefined
			char buf2[1024];
			va_start(vl, format);
			StringCbVPrintf( buf2, 1024, format, vl);
			StringCchPrintf( buf, 1024, "RTC Failure #%d (user specified type %d) occurred:\n\nModule:\t%s\nFile:\t%s\nLine:\t%d\n\nFull Message:\n%s",
				rtc_errnum, errType, module ? module : "", file ? file : "", line, buf2 );
		#endif

		running = 0;

		StringCchCat( buf, MAX_PATH, "\n\nWould you like to break to the debugger?" );
		return (MessageBox(NULL, buf, "RTC Failed...", MB_YESNO) == IDYES) ? 1 : 0;
	}

	#ifdef NOCRT
	extern "C" _RTC_error_fn _CRT_RTC_INIT(void *, void **, int , int , int )
	{
		return &Catch_RTC_Failure;
	}
	#endif

#endif


enum { __emWATCH_NUM = (32), };
DWORD __dwWATCH_CUR = 0;
char __szWATCH_TAG[__emWATCH_NUM][256];

void _LOOPWATCH ( const char* szTAG )
{
	//if ( __dwWATCH_CUR >= __emWATCH_NUM )	__dwWATCH_CUR = 0;

	//StringCchCopy( __szWATCH_TAG[__dwWATCH_CUR], szTAG );
	//__dwWATCH_CUR++;

	CDebugSet::ToLogFile ( szTAG );
}


namespace
{
	bool g_tologfile = false;
};

void SETMESSAGEBOX ( bool bToLogFile )
{
	g_tologfile = bToLogFile;
}

int MESSAGEBOX ( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType )
{
	if ( g_tologfile )	return MessageBox ( hWnd, lpText, lpCaption, uType );

	if ( lpCaption && lpCaption[0]!=NULL )
		CDebugSet::ToLogFile ( "%s : %s", lpCaption, lpText );
	else
		CDebugSet::ToLogFile ( "messagebox : %s", lpText );

	return IDYES;
}


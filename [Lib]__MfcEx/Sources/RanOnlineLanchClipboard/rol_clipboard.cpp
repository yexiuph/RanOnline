#include "pch.h"
#include <strstream>

#include "rol_clipboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace rol_clipboard
{
	enum { ROL_SIZE = 256, };
	const char ROL_NAME[] = "RanOnlineLanch";
	UINT uROLFORMAT = (UINT)(-1);

	struct SROL_HEADER
	{
		__time64_t	tLANCH;
		char		szBUFFER[ROL_SIZE-sizeof(__time64_t)];

		SROL_HEADER()
		{
			CTime cCUR = CTime::GetCurrentTime();
			tLANCH = cCUR.GetTime();

			memset ( szBUFFER, 0, ROL_SIZE-sizeof(__time64_t) );
			memcpy ( szBUFFER, ROL_NAME, strlen(ROL_NAME) );
		}
	};

	void _showLastError ( LPSTR lpszFunction )
	{
		DWORD dwERR = GetLastError();

		TCHAR szError[128] = {0};
		_snprintf_s( szError, 128, "%s failed: GetLastError returned %u", lpszFunction, dwERR );

		//std::strstream strSTREAM;
		//strSTREAM << lpszFunction << " failed: GetLastError returned " << dwERR << std::ends;

		MessageBox ( NULL, szError, "Error", MB_OK );
		//strSTREAM.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}

	HGLOBAL NewGlobalAlloc ()
	{
		HGLOBAL hGLOBAL = ::GlobalAlloc ( GMEM_ZEROINIT|GMEM_MOVEABLE, ROL_SIZE );
		if ( hGLOBAL == NULL )
		{
			_showLastError("GlobalAlloc");
			::CloseClipboard();
			return NULL;
		}

		LPVOID pBUFFER = ::GlobalLock ( hGLOBAL );
		if ( pBUFFER )
		{
			SROL_HEADER sHEAD;
			memcpy ( pBUFFER, &sHEAD, sizeof(sHEAD) );

			::GlobalUnlock(hGLOBAL);
		}

		return hGLOBAL;
	}

	void ClearGlobalAlloc ( HANDLE hROL )
	{
		if ( hROL==0 || hROL==HANDLE(-1) )	return;

		GlobalFree ( hROL );
	}

	void EmptyClipboard ()
	{
		if( ::OpenClipboard( NULL ) == FALSE )		return;

		::SetClipboardData ( uROLFORMAT, NULL );
		::CloseClipboard();
	}

	bool InitClipboard ()
	{
		uROLFORMAT = ::RegisterClipboardFormat(ROL_NAME);
		if ( uROLFORMAT==0 )
		{
			_showLastError("InitClipboard");
			return false;
		}

		return true;
	}

	HANDLE PutClipboard ( HWND hwnd )
	{
		HANDLE hResult = NULL;

		if (::OpenClipboard(hwnd))
		{
			::EmptyClipboard();

			HGLOBAL hGLOBAL = NewGlobalAlloc();
			if ( !hGLOBAL )
			{
				::CloseClipboard();
				return NULL;
			}

			hResult = ::SetClipboardData ( uROLFORMAT, hGLOBAL );
			if (hResult == NULL)
			{
				_showLastError("SetClipboardData");
				::CloseClipboard();

				ClearGlobalAlloc ( hGLOBAL );

				return NULL;
			}

			::CloseClipboard();
		}

		return hResult;
	}

	HANDLE GetClipboard ( HWND hwnd )
	{
		HANDLE hResult = NULL;

		if (::OpenClipboard(hwnd))
		{
			hResult = ::GetClipboardData ( uROLFORMAT );
			if (hResult == NULL)
			{
				//_showLastError("GetClipboardData");
				::CloseClipboard();

				return NULL;
			}

			::CloseClipboard();
		}

		return hResult;
	}

	bool CheckClipboard ( HANDLE hROL )
	{
		if ( hROL==0 || hROL==HANDLE(-1) )	return false;

		SROL_HEADER sHEAD;

		LPVOID pBUFFER = ::GlobalLock ( hROL );
		if ( !pBUFFER )					return false;

		memcpy ( &sHEAD, pBUFFER, sizeof(sHEAD) );
		::GlobalUnlock(hROL);

		CTime cROL(sHEAD.tLANCH);

		CTime cCUR = CTime::GetCurrentTime();
		cROL += CTimeSpan ( 0, 0, 5, 0 ); 
		if ( cROL < cCUR )				return false;

		return true;
	}

	bool RolHeadCheck ( HWND hwnd )
	{
		HANDLE hROL = GetClipboard(hwnd);
		if ( !hROL )		return false;
		
		bool bCHECK = CheckClipboard ( hROL );

		ClearGlobalAlloc ( hROL );

		EmptyClipboard ();

		return bCHECK;
	}
};

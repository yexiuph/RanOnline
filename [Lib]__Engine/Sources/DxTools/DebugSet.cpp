#include "pch.h"

#include "shlobj.h"
#include <list>
#include "DxInputDevice.h"
#include "DxFontMan.h"

#include "./SUBPATH.h"
#include "DebugSet.h"
#include "./CList.h"
#include "./compbyte.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace CDebugSet
{
	#define MAX_INFO		(40)
	#define MAX_CHANNEL		(8)
	#define MAX_STRLENGTH	(256)
	#define	LIST_CHANNEL	(8)
	#define	MAX_LISTPAGE	(20)	//	페이지 수, 1페이지는 MAX_INFO 라인을 출력하며,
									//	1364페이지가 대략... 10M정도 먹는다.

	enum	EDEBUGSTATE
	{
		EMPTY,
        USING,
        FIXLINE
	};

	struct INFO
	{
		char			szInfo[MAX_INFO][MAX_STRLENGTH];
		EDEBUGSTATE		State[MAX_INFO];	
		float			LeftTime[MAX_INFO];
		float			YPos[MAX_INFO];
		int				nCur;

		INFO () : nCur(-1)
		{
		}

		void Init ()
		{
			memset ( szInfo, 0, sizeof(char)*MAX_INFO*MAX_STRLENGTH );	
			memset ( LeftTime, 0, sizeof(float)*MAX_INFO );

			float ypos = 55.0f;
			for ( int i = 0; i < MAX_INFO; i++, ypos += 15.f )
			{
				YPos[i] = ypos;		
				State[i] = EMPTY;
			}
		}
	};

	DWORD m_nChannel = 0;
	INFO m_Info[MAX_CHANNEL];	
	char m_szFullDir[MAX_PATH] = "";
	CD3DFontPar* m_pFont = NULL;

	CMList<INFO*> m_ListInfo;
	int m_CurNode = 1;
	bool m_bLogFileFinalOpen = false;

	bool	m_bLogWrite(false);
	CTime	m_LogTime;
	CString m_strLogFile;
	CString m_strLogName;
	CString m_strSTARTTIME;
	BOOL	m_bSubjectName_ERROR = FALSE;

	struct SPOSTEXT
	{
		float		fX;
		float		fY;
		std::string	strText;

		SPOSTEXT ( float fx, float fy, std::string str ) :
			fX(fx),
			fY(fy),
			strText(str)
		{
		}
	};

	typedef std::list<SPOSTEXT>				LISTPOSTEXT;
	typedef std::list<SPOSTEXT>::iterator	LISTPOSTEXT_ITER;

	LISTPOSTEXT m_listPosText;

	//	Note	:	화면에 뿌리는 시간 ( 초단위 )
	//
	float DELAY_TIME = 30.0f;

	void FrameMove ( float fTime, float fETime )
	{
		//	키보드 입력 채널 설정
		DxInputDevice &IDev = DxInputDevice::GetInstance();
		if ( IDev.GetKeyState(DIK_LCONTROL)&DXKEY_PRESSED )
		{
			for ( int nKey=DIK_1; nKey<=DIK_0; nKey++ )
			{
				if (IDev.GetKeyState(nKey)&DXKEY_UP )	m_nChannel = nKey - DIK_1;
			}
		}
		
		if ( m_nChannel == LIST_CHANNEL )	//	리스트 리포팅 컨트롤
		{
			if ( IDev.GetKeyState(DIK_LCONTROL)&DXKEY_PRESSED )
			{
				if (IDev.GetKeyState(DIK_PGUP)&DXKEY_UP )
				{
					if ( 1 < m_CurNode )
					{
						if ( !m_ListInfo.IsEnd() )
						{
							m_ListInfo.GoPrev ();
							m_CurNode--;
						}
					}					
				}
				else if (IDev.GetKeyState(DIK_PGDN)&DXKEY_UP )
				{
					if ( m_CurNode < int(m_ListInfo.GetAmount()) )
					{
						if ( !m_ListInfo.IsEnd() )
						{
							m_ListInfo.GoNext ();
							m_CurNode++;
						}
					}					
				}
				else if (IDev.GetKeyState(DIK_HOME)&DXKEY_UP )
				{
					m_ListInfo.SetHead ();
					m_CurNode = 1;					
				}
				else if (IDev.GetKeyState(DIK_END)&DXKEY_UP )
				{
					m_ListInfo.SetTail ();
					m_CurNode = m_ListInfo.GetAmount ();					
				}
			}
		}
		else								//	진행 시간
		{
			for ( int i=0; i<MAX_INFO; ++i )
			{
				for ( int j=0; j<MAX_CHANNEL; ++j )
				{
					if ( m_Info[j].LeftTime[i] > 0.0f )		m_Info[j].LeftTime[i] -= fETime;
					else									m_Info[j].State[i] = EMPTY;
				}
			}
		}
	}

	void Render ()
	{	
		if ( !m_pFont )	return;

		//	특정위치에 매 프래임 마다 뿌려지는 텍스트 관리.
		{
			LISTPOSTEXT_ITER iter = m_listPosText.begin();
			LISTPOSTEXT_ITER iter_end = m_listPosText.end();

			for ( ; iter!=iter_end; iter++ )
				m_pFont->DrawText ( (*iter).fX, (*iter).fY, D3DCOLOR_ARGB(255,255,255,255), const_cast<char*>((*iter).strText.c_str()) );

			m_listPosText.clear();
		}

		D3DCOLOR fontColor;
		float	XPos = 2.f;

		if ( m_nChannel == LIST_CHANNEL )	//	리스트 리포팅 렌더
		{
			char szChannel[128] = "";
			StringCchPrintf( szChannel, 128, "List Reporting...  [%04d/%04d]", m_CurNode, m_ListInfo.GetAmount () );
			m_pFont->DrawText ( XPos, 40.0f, D3DCOLOR_ARGB(255,255,255,255), szChannel );

			INFO* pInfo = NULL;			
			m_ListInfo.GetCurrent ( pInfo );
			if ( pInfo )
			{
				fontColor =	D3DCOLOR_ARGB(255,255,255,255);

				for( int i = 0; i < MAX_INFO; i++ )
					m_pFont->DrawText ( XPos, pInfo->YPos[i], fontColor, pInfo->szInfo[i] );
			}
		}
		else								//	일반 채널 렌더
		{
			char szChannel[128] = "";
			StringCchPrintf( szChannel, 128, "Channel :\t%d", m_nChannel );
			m_pFont->DrawText ( XPos, 40.0f, D3DCOLOR_ARGB(255,255,255,255), szChannel );
			
			for( int i = 0; i < MAX_INFO; i++ )
			{
				INFO &info = m_Info[m_nChannel];

				//	Note	:	디버그 정보가 있다면... EMPTY가 아닐때...
				//				EMPTY는 0임
				if ( info.State[i] )
				{
					//	Note	:	라인이 지정된 놈
					if ( info.State[i] == FIXLINE )
					{
						fontColor = D3DCOLOR_ARGB(255,165,235,255);
					}
					
					//	Note	:	가장 늦게 들어온 디버그 정보
					//				
					else if ( info.nCur == i )
					{
						DWORD	Alpha = DWORD(255.0f * ( info.LeftTime[i] / DELAY_TIME ));
						fontColor = D3DCOLOR_ARGB(Alpha,255,199,172);
					}
					
					//	Note	:	일반 디버그 정보
					//
					else
					{
						DWORD	Alpha = DWORD(255.0f * ( info.LeftTime[i] / DELAY_TIME ));
						fontColor =	D3DCOLOR_ARGB(Alpha,255,255,255);
					}

					m_pFont->DrawText ( XPos, info.YPos[i], fontColor, info.szInfo[i] );
				}
			}
		}
	}

#ifdef _ON_DEBUGSET
	void ToView ( const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf ( szBuffer, MAX_STRLENGTH, szFormat, argList );
		va_end(argList);

		//	Note	:	다음 라인으로 진행
		m_Info[0].nCur = ( m_Info[0].nCur + 1 ) % MAX_INFO;

		//	Note	:	FullCount는 디버그 배열이 모두 다 찼을때,
		//				제일 오래전에 들어온 놈을 지운다.
		for ( int	FullCount = 0; m_Info[0].State[m_Info[0].nCur] != EMPTY; FullCount++ )
		{	
			//	Note	:	한 바퀴를 다 돌고도 못찾은 경우는
			//				디버그 정보가 꽉찼다는 이야기
			if ( FullCount == MAX_INFO )
			{
				//	Note	:	다시 제자리로 돌아왔음에도 불구하고,
				//				그 자리가 라인지정 디버그 정보이면
				//				다음 라인으로 점프
				for ( ; m_Info[0].State[m_Info[0].nCur] == FIXLINE; )
				{
					m_Info[0].nCur = ( m_Info[0].nCur + 1 ) % MAX_INFO;                                
				}
				break;
			}
			m_Info[0].nCur = ( m_Info[0].nCur + 1 ) % MAX_INFO;
		}	
		
		//	Note	:	라인 붙이기
		char	szLine[12] = "";
		StringCchPrintf( szLine, 12, "[%02d]  ", m_Info[0].nCur );

		StringCchCopy( m_Info[0].szInfo[m_Info[0].nCur], MAX_STRLENGTH, szLine );
		StringCchCat( m_Info[0].szInfo[m_Info[0].nCur], MAX_STRLENGTH, szBuffer );

		m_Info[0].LeftTime[m_Info[0].nCur] = DELAY_TIME;
		m_Info[0].State[m_Info[0].nCur] = USING;
	}


	void ToView ( int line, const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		line %= MAX_INFO;

		char	szLine[12] = "";
		StringCchPrintf( szLine, 12, "[%02d]  ", line );

		StringCchCopy ( m_Info[0].szInfo[line], MAX_STRLENGTH, szLine );
		StringCchCat( m_Info[0].szInfo[line], MAX_STRLENGTH, szBuffer );

		m_Info[0].LeftTime[line] = DELAY_TIME;
		m_Info[0].State[line] = FIXLINE;
	}

	void ToView ( int nChannel, int line, const char *szFormat, ... )
	{
		GASSERT ( nChannel<MAX_CHANNEL && "ToViewChannel() A value of channel does about below MAX_CHANNEL." );

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		line %= MAX_INFO;

		char	szLine[12] = "";
		StringCchPrintf( szLine, 12, "[%02d]  ", line );

		StringCchCopy( m_Info[nChannel].szInfo[line], MAX_STRLENGTH, szLine );
		StringCchCat( m_Info[nChannel].szInfo[line], MAX_STRLENGTH, szBuffer );

		m_Info[nChannel].LeftTime[line] = DELAY_TIME;
		m_Info[nChannel].State[line] = FIXLINE;		
	}

	void ToListView ( const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		INFO* pInfo = NULL;
		m_ListInfo.GetTail ( pInfo );

		if ( pInfo )
		{
			//	Note	:	다음 라인으로 진행
			pInfo->nCur = pInfo->nCur + 1;			
			
			if ( pInfo->nCur == MAX_INFO )	//	노드가 꽉 찬경우
			{
				BOOL	bTail = FALSE;
				if ( m_CurNode == m_ListInfo.GetAmount () )
				{
					bTail = TRUE;
				}

				//	페이지가 최대 MAX_LISTPAGE만큼 잡히도록 한다.
				//	많을 경우, 예전 것부터 삭제함
				if ( MAX_LISTPAGE == m_ListInfo.GetAmount () )
				{
					//	현재 포지션 이동
					m_ListInfo.GoNext ();

					//	머리 삭제
					INFO* pHeadInfo = NULL;
					m_ListInfo.GetHead ( pHeadInfo );
					SAFE_DELETE ( pHeadInfo );					
					m_ListInfo.DelHead ();

					if ( !m_ListInfo.GetAmount () )
					{
						//	주의 : m_CurNode가 0인경우는 없다.
						//		   여기서는 예외적으로 사용, 모든 노드가 삭제된 것을
						//		   의미한다.
						m_CurNode = 0;
					}
				}

				INFO* pNewInfo = new INFO;
				pNewInfo->Init ();

				m_ListInfo.AddTail ( pNewInfo );
				m_ListInfo.GetTail ( pInfo );

				//	현재가 마지막 페이지 인가?
				if ( bTail )
				{
					m_ListInfo.SetTail ();
					m_CurNode = m_ListInfo.GetAmount ();
				}

				pInfo->nCur = 0;				
			}
			
			//	Note	:	라인 붙이기		
			char	szLine[16] = "";
			static	int nLine = 0;
			StringCchPrintf( szLine, 16, "[%05d]  ", nLine++ );

			StringCchCopy( pInfo->szInfo[pInfo->nCur], MAX_STRLENGTH, szLine );
			StringCchCat( pInfo->szInfo[pInfo->nCur], MAX_STRLENGTH, szBuffer );

			pInfo->LeftTime[pInfo->nCur] = DELAY_TIME;
			pInfo->State[pInfo->nCur] = FIXLINE;
		}
	}

	void ToPos ( float x, float y, const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		m_listPosText.push_back ( SPOSTEXT(x,y,szBuffer) );
	}
#endif //_ON_DEBUGSET

	void SetLogFileFinalOpen( bool bUse )
	{
		m_bLogFileFinalOpen = bUse;
	}

	void SetSubjectName_ERROR( BOOL bUse )
	{
		m_bSubjectName_ERROR = bUse;
	}


	void ToFile ( const char *szFileName, const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);


		//	Note	:	절대 경로 붙이기
		//
		char	szFullPathFileName[256] = "";

		StringCchCopy( szFullPathFileName, 256, m_szFullDir );
		StringCchCat( szFullPathFileName, 256, szFileName );

		FILE *fp;
		fopen_s(&fp, szFullPathFileName, "at");
		if ( fp )
		{
			fprintf ( fp, "%s\n", szBuffer );
			fclose(fp);
		}
	}

	void ToTracingFile ( const char *szAccountName, const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH+20] = "";
		char szTempBuf[MAX_STRLENGTH] = "";
		char szFileName[MAX_STRLENGTH] = "";

		char	date[128];
		char	time[128];

		va_start(argList, szFormat);
		StringCbVPrintf(szTempBuf, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		_strdate_s( date );
		_strtime_s( time );
		
		sprintf_s( szBuffer, "[%s], %s", GetCurTime().c_str(), szTempBuf );


		//	Note	:	절대 경로 붙이기
		//
		char	szFullPathFileName[256] = "";
		sprintf_s( szFileName, "%s_Tracing_Log.csv", szAccountName );

		StringCchCopy( szFullPathFileName, 256, m_szFullDir );
		StringCchCat( szFullPathFileName, 256, szFileName );

		FILE *fp;
		fopen_s(&fp, szFullPathFileName, "at");
		if ( fp )
		{
			fprintf ( fp, "%s\n", szBuffer );
			fclose(fp);
		}
	}

	std::string GetCurTime ()
	{
		CTime Time = CTime::GetCurrentTime();

		CString strTime;
		strTime.Format ( "%d-%02d-%02d %02d:%02d:%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay(),
			Time.GetHour(), Time.GetMinute(), Time.GetSecond() );

		return strTime.GetString();
	}

	void ToFileWithTime ( const char* szFileName, const char* szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);


		//	Note	:	절대 경로 붙이기
		//
		char	szFullPathFileName[256] = "";

		StringCchCopy( szFullPathFileName, 256, m_szFullDir );
		StringCchCat( szFullPathFileName, 256, szFileName );

		FILE *fp;
		fopen_s(&fp, szFullPathFileName, "at");
		if ( fp )
		{
			std::string strTIME = GetCurTime ();
			fprintf ( fp, "[%s]%s\n", strTIME.c_str(), szBuffer );
			fclose(fp);
		}
	}

	void ToHackingFile( const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";
		char szFullPathFileName[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		StringCchCopy( szFullPathFileName, MAX_STRLENGTH, m_szFullDir );
		StringCchCat( szFullPathFileName, MAX_STRLENGTH, "HackingLogFile.csv" );

		FILE *fp;
		bool bFirst = FALSE;

		FILE *co;
		fopen_s(&co, szFullPathFileName, "r");
		if (!co)
		{
			bFirst = TRUE;
			fclose(co);
		}

		fopen_s(&fp, szFullPathFileName, "at");
		if ( fp )
		{
			std::string strTIME = GetCurTime();

			if( bFirst )
				fprintf( fp, "Time, Message, Account, ID, Money/StorageMoney, TempMoney/TempStorageMoney, Money Gap\n" );
			fprintf ( fp, "%s, %s\n", strTIME.c_str(), szBuffer );

			fclose(fp);
		}
	}

	void ToBillionUpdateFile( const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";
		char szFullPathFileName[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		StringCchCopy( szFullPathFileName, MAX_STRLENGTH, m_szFullDir );
		StringCchCat( szFullPathFileName, MAX_STRLENGTH, "MoneyUpdateFile.log" );

		FILE *fp;
		fopen_s(&fp, szFullPathFileName, "at");
		if ( fp )
		{
			std::string strTIME = GetCurTime();

			fprintf ( fp, "[%s] %s\n", strTIME.c_str(), szBuffer );

			fclose(fp);
		}
	}

	void ToLogFile ( const char *szFormat, ... )
	{
		GASSERT(!m_strLogFile.IsEmpty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		FILE *fp;
		fopen_s(&fp, m_strLogFile.GetString(), "at");
		if ( fp )
		{
			if ( !m_bLogWrite )
			{
				if( m_bSubjectName_ERROR )
				{
					fprintf ( fp, "-----------------------------------------------\n" );
					fprintf ( fp, "				    ERROR REPORT\n" );
					fprintf ( fp, "-----------------------------------------------\n" );
				}
				else
				{
					fprintf ( fp, "-----------------------------------------------\n" );
					fprintf ( fp, "                 ERROR REPORT\n" );
					fprintf ( fp, "-----------------------------------------------\n" );
				}
				
				fprintf ( fp, "\n" );
				fprintf ( fp, "Execution time : %s\n", m_strSTARTTIME.GetString() );

				m_bLogWrite = true;
			}

			std::string strTIME = GetCurTime();

			fprintf ( fp, "[%s] %s\n", strTIME.c_str(), szBuffer );

			fclose(fp);
		}
	}

	void ClearFile ( const char *szFileName )
	{
		//	Note	:	절대 경로 붙이기
		//
		char	szFullPathFileName[256] = "";

		StringCchCopy( szFullPathFileName, 256, m_szFullDir );
		StringCchCat( szFullPathFileName, 256, szFileName );

		DeleteFile ( szFullPathFileName );
	}

	const char* GetPath ()	{ return m_szFullDir; }

	HRESULT	OneTimeSceneInit ( const char *szAppPath, bool bLogFileFinalOpen )
	{
		FinalCleanup();

		TCHAR szFullPath[MAX_PATH] = {0};
		StringCchCopy( szFullPath, MAX_PATH, szAppPath );
		StringCchCat( szFullPath, MAX_PATH, SUBPATH::SAVE_ROOT );
		CreateDirectory ( szFullPath, NULL );

		StringCchCopy( m_szFullDir, MAX_PATH, szAppPath );
		StringCchCat( m_szFullDir, MAX_PATH, SUBPATH::DEBUGINFO_ROOT );
		CreateDirectory ( m_szFullDir, NULL );

		m_bLogFileFinalOpen = bLogFileFinalOpen;

		for ( int i=0; i<MAX_CHANNEL; i++ )		m_Info[i].Init();

		m_pFont = NULL;
		
		//	리스트 채널 기본 1개 작성
		INFO* pNewInfo = new INFO;
		pNewInfo->Init ();
		m_ListInfo.AddTail ( pNewInfo );
		m_ListInfo.SetTail ();		

		m_strSTARTTIME = GetCurTime().c_str();

		// Use time structure to build a customized time string.
		//time_t ltime;
		//struct tm *today;

		//time(&ltime);
		//today = localtime(&ltime);

		//TCHAR szFileName[MAX_PATH] = {0};
		//// Use strftime to build a customized time string.
		//_tcsftime( szFileName, MAX_PATH, _T("log.%Y%m%d%H%M.txt"), today );

		CTime cCurTime = CTime::GetCurrentTime();

		CString strFileName;
		strFileName.Format( "log.%d%d%d%d%d.txt", 
							cCurTime.GetYear(),
							cCurTime.GetMonth(),
							cCurTime.GetDay(),
							cCurTime.GetHour(),
							cCurTime.GetMinute() );
		
		m_strLogName = strFileName;
		m_strLogFile.Format ( "%s\\%s", m_szFullDir, m_strLogName );

		m_bLogWrite = false;

		return S_OK;
	}

	HRESULT	InitDeviceObjects ( CD3DFontPar* pFont )
	{
		if ( !pFont )		return E_FAIL;
		
		//	Note	:	폰트 세팅
		//	
		m_pFont = pFont;

		return S_OK;
	}

	HRESULT FinalCleanup()
	{
		m_ListInfo.SetHead ();
		while ( !m_ListInfo.IsEnd () )
		{
			INFO* pInfo;
			m_ListInfo.GetCurrent ( pInfo );

			SAFE_DELETE ( pInfo );
			m_ListInfo.GoNext ();
		}
		m_ListInfo.RemoveAll ();

		if ( m_bLogWrite && m_bLogFileFinalOpen )
		{
			CString strPROCESS;
			strPROCESS.Format ( "notepad %s", m_strLogName );

			STARTUPINFO si;
			memset ( &si, 0, sizeof ( STARTUPINFO ) );
			si.cb = sizeof ( STARTUPINFO );
			si.dwFlags = 0;

			PROCESS_INFORMATION pi;
			pi.hProcess = NULL;
			pi.hThread = NULL;
			CreateProcess ( NULL, strPROCESS.GetBuffer(), NULL, NULL, FALSE, 0, NULL, m_szFullDir, &si, &pi );
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
		}

		return S_OK;
	}

	// 임시 테스트용 ( 준혁 )
	bool s_bOn = false;
	FILE* s_fp = NULL;

	void TempOnOff( bool bOn, const char* szFileName )
	{
		s_bOn = bOn;
		if( s_bOn )
		{
			if( s_fp )
			{
				fprintf( s_fp, "//=====================================================================\n\n" );
				fclose( s_fp );
				s_fp = NULL;
			}

			TCHAR szFullPath[ MAX_PATH ];
			SHGetSpecialFolderPath( NULL, szFullPath, CSIDL_PERSONAL, FALSE );
			StringCchCat( szFullPath, MAX_PATH, SUBPATH::SAVE_ROOT );
			StringCchCat( szFullPath, MAX_PATH, szFileName );

			fopen_s(&s_fp, szFullPath, "at");

			if( s_fp )
				fprintf( s_fp, "//=====================================================================\n" );
			else
				s_bOn = false;
		}
		else
		{
			if( s_fp )
			{
				fprintf( s_fp, "//=====================================================================\n\n" );
				fclose( s_fp );
				s_fp = NULL;
			}
		}
	}

	void TempToFile( const char* szFormat, ... )
	{
		if( !s_bOn || !s_fp )
			return;

		va_list argList;
		char szBuffer[ MAX_STRLENGTH ] = "";

		va_start( argList, szFormat );
		StringCbVPrintf( szBuffer, MAX_STRLENGTH, szFormat, argList );
		va_end( argList );

		fprintf( s_fp, "%s\n", szBuffer );
	}
};


namespace CLOCKCHECK
{
	//	Note : cpu 클럭 측정을 위한 변수 공간.
	//
	DWORD dwShi, dwSlo, dwEhi, dwElo;
};
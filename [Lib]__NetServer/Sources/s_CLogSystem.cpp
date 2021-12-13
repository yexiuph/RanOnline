#include "pch.h"
#include "s_CLogSystem.h"
#include "s_CSystemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace SERVER_UTIL;

CLogSystem::CLogSystem(  TCHAR* szFileHead )
{
	dLogInterval = SERVER_UTIL::LOG_INTERVAL;
	memset( log_tmpbuf,   0, sizeof(TCHAR) * (LOG_MAX_TMP_BUF+1) );
	memset( log_filename, 0, sizeof(TCHAR) * (MAX_PATH+1) );
	memset( m_szFileHead, 0, sizeof(TCHAR) * (MAX_PATH) );

	if (szFileHead == NULL)
		StringCchCopy( m_szFileHead, MAX_PATH, _T("Log") );
	else
		StringCchCopy( m_szFileHead, MAX_PATH, szFileHead );
	StartLog();
}

CLogSystem::~CLogSystem()
{
	WriteFileFinal();
}

void CLogSystem::StartLog(void)
{
	SERVER_UTIL::CSystemInfo SysInfo;
	CString strAppPath;
	TCHAR szAppPath[MAX_PATH+1] = {0};
	TCHAR fileName[MAX_FILE_NAME_LENGTH+1] = {0};

	strAppPath = SysInfo.GetAppPath ();
	StringCchCopy( szAppPath, MAX_PATH+1, strAppPath.GetString() );
	StringCchCat ( szAppPath, MAX_PATH+1, _T("\\log\\") );

	FILE *LogFile;	
    struct tm today;
	int nRetCode = 0;

	// Use time structure to build a customized time string.
	time(&ltime);
	localtime_s(&today, &ltime);

    // Use strftime to build a customized time string. 
	_tcsftime( 
		fileName,
		MAX_PATH,
		_T("_%Y%m%d%H%M.log"),
		&today );

	StringCchCopy( log_filename, MAX_PATH+1, szAppPath );
	StringCchCat( log_filename, MAX_PATH+1, m_szFileHead );
	StringCchCat( log_filename, MAX_PATH+1, fileName );

	_tfopen_s(&LogFile, log_filename, _T("w"));
	if (LogFile != NULL)
	{
		_ftprintf(
			LogFile,
			_T("%s %s Started \n"),
			m_szFileHead,
			GetTime() );
		fclose(LogFile);
	}
}

void CLogSystem::Write( TCHAR *szStr, ... )
{
	if (szStr == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start(ap, szStr);	
	StringCchVPrintf( szBuf, C_BUFFER_SIZE+1, szStr, ap );
	va_end(ap);
	
	std::string strTemp;
	strTemp = strTemp + GetTime() + " " + szBuf;

	m_vString.push_back(strTemp);
	if (m_vString.size() >= SERVER_UTIL::LOG_ARRAY_SIZE)
		WriteNow();
	else
		return;
}

void CLogSystem::WriteFileFinal(void)
{
	int		nRetCode = 0;
	double	elapsed_time = 0;
	int		nCount = 0;
	FILE*	LogFile = NULL;

	_tfopen_s(&LogFile, log_filename, _T("a"));
	
	if (LogFile == NULL)
	{
		// 파일오픈에 실패하면 log_start 를 시켜보고
		// 다시 파일을 오픈한다.
		// 그래두 에러가 발생하면 디스크가 꽉찼거나 기타 에러이다.
		// 대처방법 없음...
		StartLog();
		_tfopen_s(&LogFile, log_filename, _T("a"));
		if (LogFile == NULL) return; // 에러 발생시 리턴
	}
	
	// 로그 파일로 쓰기
	std::vector <int>::size_type i;
	for (i = 0; i < m_vString.size(); ++i)	
	{
		_ftprintf( LogFile, _T("%s\n"), m_vString[i].c_str() );
	}
	fclose(LogFile);
	// 버퍼 비우기
	m_vString.clear();
}

void CLogSystem::WriteNow(void)
{	
	int		nRetCode = 0;
	double	elapsed_time = 0;
	time_t	ctime;
	int nCount = 0;
	FILE *LogFile = NULL;
		
	ctime = time(&ctime);
	elapsed_time = difftime(ltime, ctime);
	
	if (elapsed_time >= dLogInterval)
	{
		StartLog();
	}	

	_tfopen_s(&LogFile, log_filename, _T("a"));
	
	if (LogFile == NULL)
	{
		// 파일오픈에 실패하면 log_start 를 시켜보고
		// 다시 파일을 오픈한다.
		// 그래두 에러가 발생하면 디스크가 꽉찼거나 기타 에러이다.
		// 대처방법 없음...
		StartLog();
		_tfopen_s(&LogFile, log_filename, _T("a"));
		if (LogFile == NULL) return; // 에러 발생시 리턴
	}
	
	// 로그 파일로 쓰기
	std::vector <int>::size_type i;
	for (i = 0; i < m_vString.size(); i++)
	{
		_ftprintf( LogFile, _T("%s\n"), m_vString[i].c_str() );
	}
	fclose(LogFile);
	
	// 버퍼 비우기
	m_vString.clear();
}

TCHAR* CLogSystem::GetTime(void)
{	
	time_t ltime;    
    struct tm today;

	// Use time structure to build a customized time string. 
	time(&ltime);
	localtime_s(&today, &ltime);

    // Use strftime to build a customized time string. 
	_tcsftime( log_tmpbuf, 100, _T("%Y %m %d %H:%M:%S"), &today );
	return log_tmpbuf;
}
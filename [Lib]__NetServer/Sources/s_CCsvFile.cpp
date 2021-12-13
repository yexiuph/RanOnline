///////////////////////////////////////////////////////////////////////////////
// s_CCsvFile.cpp
//
// Copyright (c) Mincoms. All rights reserved.                 
// 
// Note
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_CCsvFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace SERVER_UTIL;

CCsvFile::CCsvFile(CString strFileName)
{	
	m_pFile = NULL;
	CString strAppPath;	
	CString strFullFileName;
	TCHAR fileName[MAX_FILE_NAME_LENGTH+1] = {0};

	strAppPath = GetAppPath ();
	strFullFileName.Format(_T("%s\\%s"), strAppPath.GetString(), strFileName.GetString());
	
    struct tm today;
	int nRetCode = 0;
	time_t ltime;

	// Use time structure to build a customized time string.
	// Use strftime to build a customized time string.
	time(&ltime);
	localtime_s(&today, &ltime);
	_tcsftime(fileName, MAX_PATH, _T("%Y%m%d%H%M.csv"), &today);	

	strFullFileName = strFullFileName + fileName;

	_tfopen_s(&m_pFile, strFullFileName.GetString(), _T("w"));
	if (m_pFile != NULL)
	{
		m_strFullFileName = strFullFileName;
		fclose(m_pFile);
		m_pFile = NULL;
	}
	else // error
	{
		// fclose(m_pFile);
	}

}

CCsvFile::~CCsvFile()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

void CCsvFile::Write(const TCHAR *szStr, ...)
{
	if (szStr == NULL) return;

	TCHAR buf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start(ap, szStr);
	StringCbVPrintf(buf, C_BUFFER_SIZE, szStr, ap);
	va_end(ap);
	
	if (m_pFile == NULL)
		_tfopen_s(&m_pFile, m_strFullFileName, _T("at"));

	if (m_pFile)
	{		
		// 로그 파일로 쓰기
		_ftprintf(m_pFile, _T("%s\n"), buf); 
		fclose(m_pFile);
		m_pFile = NULL;
	}
	else
	{
		return; // 에러 발생시 리턴
	}
}

void CCsvFile::WriteWithTime(const TCHAR* szStr, ...)
{
	TCHAR buf[C_BUFFER_SIZE+1]  = {0};
	TCHAR sbuf[C_BUFFER_SIZE+1] = {0};
	
	if (szStr == NULL) return;

	va_list ap;
	va_start(ap, szStr);	
	StringCbVPrintf(buf, C_BUFFER_SIZE, szStr, ap);
	va_end(ap);
	
	if (m_pFile == NULL)
		_tfopen_s(&m_pFile, m_strFullFileName, _T("at"));

	if (m_pFile)
	{
		CString strTemp(sbuf);
		CTime cTime = CTime::GetCurrentTime();
		CString strTime = cTime.Format(_T("%H:%M:%S"));
		CString strWrite = strTime + _T(",") + buf;

		// 로그 파일로 쓰기
		_ftprintf(m_pFile, _T("%s\n"), strWrite.GetString()); 
		fclose(m_pFile);
		m_pFile = NULL;
	}
	else
	{
		return; // 에러 발생시 리턴
	}
}

CString CCsvFile::GetAppPath ()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if (!strCommandLine.IsEmpty())
	{
		DWORD dwFind = strCommandLine.ReverseFind (_T('\\'));
		if (dwFind != -1)
		{
			strFullPath = strCommandLine.Left(dwFind);
			
			if (!strFullPath.IsEmpty())
			if (strFullPath.GetAt(0) == '"')
				strFullPath = strFullPath.Right(strFullPath.GetLength() - 1);
		}
	}
	return strFullPath;
}

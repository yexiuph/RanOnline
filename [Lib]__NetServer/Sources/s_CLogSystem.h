///////////////////////////////////////////////////////////////////////////////
// s_CLogSysem.h
//
// class CLogSystem
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_LOG_SYSTEM_H_
#define S_LOG_SYSTEM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>
#include <vector>

#ifndef C_BUFFER_SIZE
#define C_BUFFER_SIZE 1024
#endif

#ifndef MAX_FILE_NAME_LENGTH
#define MAX_FILE_NAME_LENGTH 256
#endif

#ifndef MAX_PATH
#define MAX_PATH          260
#endif

#ifndef LOG_MAX_TMP_BUF
#define LOG_MAX_TMP_BUF 100
#endif

namespace SERVER_UTIL
{
	const int LOG_ARRAY_SIZE = 1;
	const int LOG_INTERVAL = 600; // 60 sec * 10 = 10 min

	class CLogSystem 
	{
	public:
		CLogSystem( TCHAR* szFileHead=NULL );
		~CLogSystem();

	protected:
		time_t ltime;
		TCHAR m_szFileHead[MAX_PATH];
		TCHAR log_tmpbuf[LOG_MAX_TMP_BUF+1];
		TCHAR log_filename[MAX_PATH+1];
		double dLogInterval;
		std::vector<std::string> m_vString;
		
	public:		
		void Write( TCHAR *szStr, ... );
		void StartLog();

	protected:
		void WriteNow();
		void WriteFileFinal();
		TCHAR* GetTime();
	};
}

#endif // S_LOG_SYSTEM_H_
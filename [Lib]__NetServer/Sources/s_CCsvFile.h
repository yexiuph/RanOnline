#ifndef S_CSV_FILE_H_
#define S_CSV_FILE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef C_BUFFER_SIZE
#define C_BUFFER_SIZE 1024
#endif

#ifndef MAX_FILE_NAME_LENGTH
#define MAX_FILE_NAME_LENGTH 256
#endif

#ifndef MAX_PATH
#define MAX_PATH             256
#endif

namespace SERVER_UTIL
{
/**
 * \ingroup NetServerLib
 *
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2005-02-01 Jgkim 생성
 *       2006-02-07 Jgkim:Unicode 지원, VS 2005 지원
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
	class CCsvFile
	{
	public:
		CCsvFile(CString strFileName);
		~CCsvFile();

	protected:
		CString m_strFullFileName;
		CString m_strFileName;
		FILE*	m_pFile;
			
	public:
		void Write(const TCHAR *szStr, ...);
		void WriteWithTime(const TCHAR *szStr, ...);

	protected:
		CString GetAppPath();
	};
}

#endif // S_CSV_FILE_H_
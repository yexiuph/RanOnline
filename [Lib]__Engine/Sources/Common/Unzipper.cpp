// Unzipper.cpp: implementation of the CUnzipper class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "./Unzipper.h"
#include "../[Lib]__ZLib/Sources/iowin32.h"

#ifdef _DEBUG
#pragma comment(lib, "../[Lib]__ZLib/Bin/ZLibD.lib")
#define new DEBUG_NEW
#else
#pragma comment(lib, "../[Lib]__ZLib/Bin/ZLib.lib")
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const UINT BUFFERSIZE = 20480;

MAPZIPPOS CUnzipper::m_mapZipPos;

BOOL CUnzipper::LOADFILE_RCC( std::string strZipName )
{
	void * uzFile = unzOpen(strZipName.c_str());
	if( !uzFile ) return FALSE;

	if (unzGoToFirstFile(uzFile) == UNZ_OK)
	{
		do
		{
			UNZ_FILE_POS file_pos;
			if( UNZ_OK != unzGetFilePos( uzFile, &file_pos ) )
				return FALSE;

			UZ_FileInfo info;
			if (UNZ_OK != unzGetCurrentFileInfo(uzFile, NULL, info.szFileName, MAX_PATH, NULL, 0, info.szComment, MAX_COMMENT))
				return FALSE;

			//UNZIPKEY unzKey;
			//unzKey.strZipName = strZipName;
			//unzKey.strFileName = info.szFileName;
			
			std::string strFileName = strZipName + info.szFileName;
			std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );
			m_mapZipPos.insert( std::make_pair( strFileName, file_pos ) );
		}
		while (unzGoToNextFile(uzFile) == UNZ_OK);

		unzCloseCurrentFile( uzFile );
	}

	unzClose( uzFile );
	uzFile = NULL;

	return TRUE;
}

BOOL CUnzipper::GOTOFILEPOS( std::string strFileName )
{
	std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

	MAPZIPPOS_ITER iter = m_mapZipPos.find( strFileName );
	if( iter == m_mapZipPos.end() ) return FALSE;

	if( UNZ_OK != unzGoToFilePos( m_uzFile, &(iter->second) ) )
		return FALSE;

	return TRUE;
}

CUnzipper::CUnzipper(LPCTSTR szFileName) : m_uzFile(0)
{
	memset( m_szOutputFolder, 0, sizeof(char)*(MAX_PATH+1) );

	if (szFileName)
	{
		m_uzFile = unzOpen(szFileName);

		//if (m_uzFile)
		//{
		//	// set the default output folder
		//	char* szPath = _strdup(szFileName);

		//	// strip off extension
		//	char* p = strrchr(szPath, '.');

		//	if (p)
		//		*p = 0;

		//	StringCchCopy(m_szOutputFolder, MAX_PATH + 1, szPath);
		//	free(szPath);
		//}
	}
}

CUnzipper::~CUnzipper()
{
	//ClearVector(); // 벡터를 삭제한다.
	CloseZip();
}

BOOL CUnzipper::CloseZip()
{
	unzCloseCurrentFile(m_uzFile);

	int nRet = unzClose(m_uzFile);
	m_uzFile = NULL;

	return (nRet == UNZ_OK);
}

// simple interface
BOOL CUnzipper::Unzip(BOOL bIgnoreFilePath)
{
	if (!m_uzFile)
		return FALSE;

	return UnzipTo(m_szOutputFolder, bIgnoreFilePath);
}

BOOL CUnzipper::UnzipTo(LPCTSTR szFolder, BOOL bIgnoreFilePath)
{
	if (!m_uzFile)
		return FALSE;

	if (!szFolder || !CreateFolder(szFolder))
		return FALSE;

	if (GetFileCount() == 0)
		return FALSE;

	if (!GotoFirstFile())
		return FALSE;

	// else
	do
	{
		if (!UnzipFile(szFolder, bIgnoreFilePath))
			return FALSE;
	}
	while (GotoNextFile());
	
	return TRUE;
}

BOOL CUnzipper::Unzip(LPCTSTR szFileName, LPCTSTR szFolder, BOOL bIgnoreFilePath)
{
	CloseZip();

	if (!OpenZip(szFileName))
		return FALSE;

	return UnzipTo(szFolder, bIgnoreFilePath);
}

// extended interface
BOOL CUnzipper::OpenZip(LPCTSTR szFileName)
{
	CloseZip();

	if (szFileName)
	{
		m_uzFile = unzOpen(szFileName);

		//if (m_uzFile)
		//{
		//	// set the default output folder
		//	char* szPath = _strdup(szFileName);

		//	// strip off extension
		//	char* p = strrchr(szPath, '.');

		//	if (p)
		//		*p = 0;

		//	StringCchCopy(m_szOutputFolder, MAX_PATH + 1, szPath);
		//	free(szPath);
		//}
	}

	return (m_uzFile != NULL);
}

BOOL CUnzipper::SetOutputFolder(LPCTSTR szFolder)
{
	DWORD dwAttrib = GetFileAttributes(szFolder);

	if (dwAttrib != 0xffffffff && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
		return FALSE;

	StringCchCopy( m_szOutputFolder, MAX_PATH+1, szFolder );

	return CreateFolder(szFolder);
}

int CUnzipper::GetFileCount()
{
	if (!m_uzFile)
		return 0;

	unz_global_info info;

	if (unzGetGlobalInfo(m_uzFile, &info) == UNZ_OK)
	{
		return (int)info.number_entry;
	}

	return 0;
}

BOOL CUnzipper::GetFileInfo(int nFile, UZ_FileInfo& info)
{
	if (!m_uzFile)
		return FALSE;

	if (!GotoFile(nFile))
		return FALSE;

	return GetFileInfo(info);
}

BOOL CUnzipper::UnzipFile(int nFile, LPCTSTR szFolder, BOOL bIgnoreFilePath)
{
	if (!m_uzFile)
		return FALSE;

	if (!szFolder)
		szFolder = m_szOutputFolder;

	if (!GotoFile(nFile))
		return FALSE;

	return UnzipFile(szFolder, bIgnoreFilePath);
}

BOOL CUnzipper::GotoFirstFile(LPCTSTR szExt)
{
	if (!m_uzFile)
		return FALSE;

	if (!szExt || !lstrlen(szExt))
		return (unzGoToFirstFile(m_uzFile) == UNZ_OK);

	// else
	if (unzGoToFirstFile(m_uzFile) == UNZ_OK)
	{
		UZ_FileInfo info;

		if (!GetFileInfo(info))
			return FALSE;

		// test extension
		char* pExt = strrchr(info.szFileName, '.');

		if (pExt)
		{
			pExt++;

			if (lstrcmpi(szExt, pExt) == 0)
				return TRUE;
		}

		return GotoNextFile(szExt);
	}

	return FALSE;
}

BOOL CUnzipper::GotoNextFile(LPCTSTR szExt)
{
	if (!m_uzFile)
		return FALSE;

	if (!szExt || !lstrlen(szExt))
		return (unzGoToNextFile(m_uzFile) == UNZ_OK);

	// else
	UZ_FileInfo info;

	while (unzGoToNextFile(m_uzFile) == UNZ_OK)
	{
		if (!GetFileInfo(info))
			return FALSE;

		// test extension
		char* pExt = strrchr(info.szFileName, '.');

		if (pExt)
		{
			pExt++;

			if (lstrcmpi(szExt, pExt) == 0)
				return TRUE;
		}
	}

	return FALSE;

}

BOOL CUnzipper::GetFileInfo(UZ_FileInfo& info)
{
	if (!m_uzFile)
		return FALSE;

	unz_file_info uzfi;

	SecureZeroMemory(&info, sizeof(info));
	SecureZeroMemory(&uzfi, sizeof(uzfi));

	if (UNZ_OK != unzGetCurrentFileInfo(m_uzFile, &uzfi, info.szFileName, MAX_PATH, NULL, 0, info.szComment, MAX_COMMENT))
		return FALSE;

	// copy across
	info.dwVersion = uzfi.version;	
	info.dwVersionNeeded = uzfi.version_needed;
	info.dwFlags = uzfi.flag;	
	info.dwCompressionMethod = uzfi.compression_method; 
	info.dwDosDate = uzfi.dosDate;  
	info.dwCRC = uzfi.crc;	 
	info.dwCompressedSize = uzfi.compressed_size; 
	info.dwUncompressedSize = uzfi.uncompressed_size;
	info.dwInternalAttrib = uzfi.internal_fa; 
	info.dwExternalAttrib = uzfi.external_fa; 

	// replace filename forward slashes with backslashes
	int nLen = lstrlen(info.szFileName);

	while (nLen--)
	{
		if (info.szFileName[nLen] == '/')
			info.szFileName[nLen] = '\\';
	}

	// is it a folder?
	info.bFolder = (info.szFileName[lstrlen(info.szFileName) - 1] == '\\');

	return TRUE;
}

BOOL CUnzipper::UnzipFile(LPCTSTR szFolder, BOOL bIgnoreFilePath)
{
	if (!m_uzFile)
		return FALSE;

	if (!szFolder)
		szFolder = m_szOutputFolder;

	if (!CreateFolder(szFolder))
		return FALSE;

	UZ_FileInfo info;
	GetFileInfo(info);

	// if the item is a folder then simply return 'TRUE'
	if (info.szFileName[lstrlen(info.szFileName) - 1] == '\\')
		return TRUE;

	// build the output filename
	char szFilePath[MAX_PATH] = "";
	StringCchCopy( szFilePath, MAX_PATH, szFolder );

	// append backslash
	if (szFilePath[lstrlen(szFilePath) - 1] != '\\')
		StringCchCat( szFilePath, MAX_PATH, "\\" );

	if (bIgnoreFilePath)
	{
		char* p = strrchr(info.szFileName, '\\');

		if (p)
			StringCchCopy(info.szFileName, MAX_PATH+1, p+1 );
	}

	StringCchCat( szFilePath, MAX_PATH, info.szFileName );

	// open the input and output files
	if (!CreateFilePath(szFilePath))
		return FALSE;

	HANDLE hOutputFile = ::CreateFile(szFilePath, 
										GENERIC_WRITE,
										0,
										NULL,
										CREATE_ALWAYS,
										FILE_ATTRIBUTE_NORMAL,
										NULL);

	if (INVALID_HANDLE_VALUE == hOutputFile)
		return FALSE;

	if (unzOpenCurrentFile(m_uzFile) != UNZ_OK)
		return FALSE;

	// read the file and output
	int nRet = UNZ_OK;
	char pBuffer[BUFFERSIZE] = "";

	do
	{
		nRet = unzReadCurrentFile(m_uzFile, pBuffer, BUFFERSIZE);

		if (nRet > 0)
		{
			// output
			DWORD dwBytesWritten = 0;

			if (!::WriteFile(hOutputFile, pBuffer, nRet, &dwBytesWritten, NULL) ||
				dwBytesWritten != (DWORD)nRet)
			{
				nRet = UNZ_ERRNO;
				break;
			}
		}
	}
	while (nRet > 0);

	CloseHandle(hOutputFile);
	unzCloseCurrentFile(m_uzFile);

	if (nRet == UNZ_OK)
		SetFileModTime(szFilePath, info.dwDosDate);

	return (nRet == UNZ_OK);
}

BOOL CUnzipper::GotoFile(int nFile)
{
	if (!m_uzFile)
		return FALSE;

	if (nFile < 0 || nFile >= GetFileCount())
		return FALSE;

	GotoFirstFile();

	while (nFile--)
	{
		if (!GotoNextFile())
			return FALSE;
	}

	return TRUE;
}

BOOL CUnzipper::GotoFile(LPCTSTR szFileName, BOOL bIgnoreFilePath)
{
	if (!m_uzFile)
		return FALSE;

	// try the simple approach
	if (unzLocateFile(m_uzFile, szFileName, 2) == UNZ_OK)
		return TRUE;

	else if (bIgnoreFilePath)
	{ 
		// brute force way
		if (unzGoToFirstFile(m_uzFile) != UNZ_OK)
			return FALSE;

		UZ_FileInfo info;

		do
		{
			if (!GetFileInfo(info))
				return FALSE;

			// test name
			char* pName = strrchr(info.szFileName, '\\');

			if (pName)
			{
				pName++;

				if (lstrcmpi(szFileName, pName) == 0)
					return TRUE;
			}
		}
		while (unzGoToNextFile(m_uzFile) == UNZ_OK);
	}

	// else
	return FALSE;
}

BOOL CUnzipper::CreateFolder(LPCTSTR szFolder)
{
	if (!szFolder || !lstrlen(szFolder))
		return FALSE;

	DWORD dwAttrib = GetFileAttributes(szFolder);

	// already exists ?
	if (dwAttrib != 0xffffffff)
		return ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);

	// recursively create from the top down
	char* szPath = _strdup(szFolder);
	char* p = strrchr(szPath, '\\');

	if (p) 
	{
		// The parent is a dir, not a drive
		*p = '\0';
			
		// if can't create parent
		if (!CreateFolder(szPath))
		{
			free(szPath);
			return FALSE;
		}
		free(szPath);

		if (!::CreateDirectory(szFolder, NULL)) 
			return FALSE;
	}
	
	return TRUE;
}

BOOL CUnzipper::CreateFilePath(LPCTSTR szFilePath)
{
	char* szPath = _strdup(szFilePath);
	char* p = strrchr(szPath,'\\');

	BOOL bRes = FALSE;

	if (p)
	{
		*p = '\0';

		bRes = CreateFolder(szPath);
	}

	free(szPath);

	return bRes;
}

BOOL CUnzipper::SetFileModTime(LPCTSTR szFilePath, DWORD dwDosDate)
{
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (!hFile)
		return FALSE;
	
	FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

	BOOL bRes = (GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite) == TRUE);

	if (bRes)
		bRes = DosDateTimeToFileTime((WORD)(dwDosDate >> 16), (WORD)dwDosDate, &ftLocal);

	if (bRes)
		bRes = LocalFileTimeToFileTime(&ftLocal, &ftm);

	if (bRes)
		bRes = SetFileTime(hFile, &ftm, &ftLastAcc, &ftm);

	CloseHandle(hFile);

	return bRes;
}

// szFileName은 full pathname, 인코딩 여부
//
UINT CUnzipper::UnzipToMemory( LPCTSTR szZipFileName, LPCTSTR szFileName, PBYTE &cBuffer )
{
	//ClearVector();

	if (!OpenZip( szZipFileName ))	return FALSE;
	if (!m_uzFile)					return FALSE;
	if (GetFileCount() == 0)		return FALSE;
	//if (!GotoFirstFile())			return FALSE;

	UINT nRetB(0);

	std::string strFileName = szZipFileName;
	strFileName += szFileName;

	if( GOTOFILEPOS( strFileName ) ) // GotoFilePos로 이동
	{
		UZ_FileInfo info;
		GetFileInfo(info);

		// if the item is a folder then simply return 'TRUE'
		//if (info.szFileName[lstrlen(info.szFileName) - 1] == '\\')
		//	continue;

		if( IsEncrypted( info.dwFlags ) ) // MEMO : 암호화 된 파일이면...
		{
			if( unzOpenCurrentFilePassword(m_uzFile, "tkqwlf") != UNZ_OK )
				return UINT_MAX;
		}
		else
		{
			if( unzOpenCurrentFile(m_uzFile) != UNZ_OK )
				return UINT_MAX;
		}

		// read the file and output
		UINT nRet(0);
		char pBuffer[BUFFERSIZE] = "";
		cBuffer = new BYTE[info.dwUncompressedSize]; // 호출하는 부분에서 메모리 해제
		
		do
		{
			nRet = unzReadCurrentFile(m_uzFile, pBuffer, BUFFERSIZE);

			if (nRet > 0)
			{
				memcpy( cBuffer+nRetB, pBuffer, nRet );
				nRetB += nRet; // 이전 버퍼 크기
			}
		}
		while (nRet > 0);

		unzCloseCurrentFile(m_uzFile);
	}

	return nRetB;
}

BOOL CUnzipper::IsEncrypted( DWORD dwFlags )
{
	return ( dwFlags & (WORD)1) != 0;
}
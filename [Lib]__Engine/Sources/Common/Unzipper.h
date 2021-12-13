// Unzipper.h: interface for the CUnzipper class (c) daniel godson 2002
//
// CUnzipper is a simple c++ wrapper for the 'unzip' file extraction
// api written by Gilles Vollant (c) 2002, which in turn makes use of 
// 'zlib' written by Jean-loup Gailly and Mark Adler (c) 1995-2002.
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely without restriction.
//
// Notwithstanding this, you are still bound by the conditions imposed
// by the original authors of 'unzip' and 'zlib'
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../[Lib]__ZLib/Sources/unzip.h"
#include <map>

typedef unz_file_pos UNZ_FILE_POS;

typedef std::map< std::string, UNZ_FILE_POS >	MAPZIPPOS;
typedef MAPZIPPOS::iterator						MAPZIPPOS_ITER;
typedef MAPZIPPOS::const_iterator				MAPZIPPOS_CITER;

const UINT MAX_COMMENT = 255;

// create our own fileinfo struct to hide the underlying implementation
struct UZ_FileInfo
{
	char szFileName[MAX_PATH + 1];
	char szComment[MAX_COMMENT + 1];
	
	DWORD dwVersion;  
	DWORD dwVersionNeeded;
	DWORD dwFlags;	 
	DWORD dwCompressionMethod; 
	DWORD dwDosDate;	
	DWORD dwCRC;   
	DWORD dwCompressedSize; 
	DWORD dwUncompressedSize;
	DWORD dwInternalAttrib; 
	DWORD dwExternalAttrib; 
	bool bFolder;

	UZ_FileInfo()
	{
		memset( szFileName, 0, sizeof(char)*(MAX_PATH+1) );
		memset( szComment, 0, sizeof(char)*(MAX_COMMENT+1) );
	};
};

class CUnzipper  
{
public:
	CUnzipper(LPCTSTR szFileName = NULL);
	virtual ~CUnzipper();
	
	// simple interface
	BOOL Unzip(LPCTSTR szFileName, LPCTSTR szFolder = NULL, BOOL bIgnoreFilePath = FALSE);
	
	// works with prior opened zip
	BOOL Unzip(BOOL bIgnoreFilePath = FALSE); // unzips to output folder or sub folder with zip name 
	BOOL UnzipTo(LPCTSTR szFolder, BOOL bIgnoreFilePath = FALSE); // unzips to specified folder

	// extended interface
	BOOL OpenZip(LPCTSTR szFileName);
	BOOL CloseZip(); // for multiple reuse
	BOOL SetOutputFolder(LPCTSTR szFolder); // will try to create
	
	// unzip by file index
	int GetFileCount();
	BOOL GetFileInfo(int nFile, UZ_FileInfo& info);
	BOOL UnzipFile(int nFile, LPCTSTR szFolder = NULL, BOOL bIgnoreFilePath = FALSE);
	
	// unzip current file
	BOOL GotoFirstFile(LPCTSTR szExt = NULL);
	BOOL GotoNextFile(LPCTSTR szExt = NULL);
	BOOL GetFileInfo(UZ_FileInfo& info);
	BOOL UnzipFile(LPCTSTR szFolder = NULL, BOOL bIgnoreFilePath = FALSE);

	// helpers
	BOOL GotoFile(LPCTSTR szFileName, BOOL bIgnoreFilePath = TRUE);
	BOOL GotoFile(int nFile);

	// by °æ´ë
	UINT UnzipToMemory( LPCTSTR szZipFileName, LPCTSTR szFileName, PBYTE &cBuffer );
	BOOL IsEncrypted( DWORD dwFlags );

public:
	static MAPZIPPOS m_mapZipPos;
	static BOOL LOADFILE_RCC( std::string strZipName );
	
	BOOL GOTOFILEPOS( std::string strFileName );
	
protected:
	void* m_uzFile;
	char m_szOutputFolder[MAX_PATH + 1];

protected:
	BOOL CreateFolder(LPCTSTR szFolder);
	BOOL CreateFilePath(LPCTSTR szFilePath); // truncates from the last '\'
	BOOL SetFileModTime(LPCTSTR szFilePath, DWORD dwDosDate);
};

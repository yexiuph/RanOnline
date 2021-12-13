#pragma once

#include <list>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

//#define FS_MAX_OPEN		8
#define FS_MAX_HEADER	22
#define FS_HEADER_STR	_T("RANPACKAGEFILESYSTEM")

#define FC_OFFSET_TYPE			 (0)
#define FC_OFFSET_NAME			 (sizeof(int))
#define FC_OFFSET_COMPRESSSIZE	 (sizeof(int)+_MAX_FNAME)
#define FC_OFFSET_UNCOMPRESSSIZE (sizeof(int)+_MAX_FNAME+sizeof(long))
#define FC_OFFSET_SUB			 (sizeof(int)+_MAX_FNAME+sizeof(long)*2)
#define FC_OFFSET_NEXT			 (sizeof(int)+_MAX_FNAME+sizeof(long)*3)

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

enum FILETYPE {FT_DIR,FT_FILE};

typedef struct _tagFILECONTEXT
{
	int		iType;
	TCHAR	strName[_MAX_FNAME];
	long	lCompressSize;
	long	lUncompressSize;
	long	lSub;
	long	lNext;

}FILECONTEXT;

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class SFileHandle
{
private:
	FILECONTEXT	m_FileContext;
	long		m_lSrtPos;

private:
	BYTE*		m_pData;
	long		m_lSize;
	long		m_lCurPos;

public:
	SFileHandle();
	virtual ~SFileHandle();

public:
	inline FILECONTEXT* GetFileContext() { return &m_FileContext; }

public:
	int Read(void* pBuff,long lSize);
	int Seek(long lOffset,int iOrigin);

	friend class SFileSystem;
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

typedef std::list<SFileHandle*>				SFileHandleList;
typedef std::list<SFileHandle*>::iterator	SFileHandleList_i;

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class SFileSystem
{
private:
	FILE*			m_pFile;
	SFileHandleList	m_FileHandleList;
	SFileHandle		m_HandleList;//[FS_MAX_OPEN];
	char			m_strCurDir[_MAX_PATH];

public:
	SFileSystem();
	virtual ~SFileSystem();

public:
	inline SFileHandleList* GetFileHandleList() { return &m_FileHandleList; }
	inline const char*		GetCurDir()			{ return m_strCurDir; }

private:
	void SetCurDir(const char* pTok);
	void GetSafeName(char* strDst,const char* strName);
	BOOL SearchDir(const char* name,SFileHandle* fh);
	BOOL OptimizeDir(SFileSystem* pFS,const char* strName);

public:
	void CloseFileSystem();
	BOOL OpenFileSystem(const char* strPath);
	BOOL NewFileSystem(const char* strPath);
	BOOL OptimizeFileSystem(const char* strPath);

public:
	void CloseFile(SFileHandle* pFH);
	SFileHandle* OpenFile(const char* strPath);
	SFileHandle* OpenFileForOptimize(const char* strPath);
	BOOL AddDir(const char* strName);
	BOOL AddFile(const char* strPath);
	BOOL AddFile(const char* strName,BYTE* pData,long lSize);
	BOOL AddFileForOptimize(const char* strName,BYTE* pData,long lCompressSize,long lUncompressSize);
	BOOL Remove(const char* strName);
	BOOL Rename(const char* strName,const char* strRename);

public:
	BOOL ChangeDir(const char* strPath);
	BOOL CheckNameExist(const char* strName);
	void GetNameOnly(char* strDst,const char* strPath);
	void GetPathOnly(char* strDst,const char* strPath);
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

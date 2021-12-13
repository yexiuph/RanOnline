#include "pch.h"
#include "./SFileSystem.h"
#include "../[Lib]__ZLib/Sources/zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#pragma comment(lib, "../[Lib]__ZLib/Bin/ZLibD.lib")
#else
#pragma comment(lib, "../[Lib]__ZLib/Bin/ZLib.lib")
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

SFileHandle::SFileHandle()
{
	SecureZeroMemory(&m_FileContext,sizeof(FILECONTEXT));
	m_lSrtPos = 0;

	m_pData = NULL;
	m_lSize = 0;
	m_lCurPos = 0;
}

SFileHandle::~SFileHandle()
{
	if(m_pData) delete[] m_pData;
}

int SFileHandle::Read(void* pBuff,long lSize)
{
	if(!m_pData) return 0;

	if(m_lSize < m_lCurPos) return 0;
	if(m_lSize < m_lCurPos+lSize) lSize = m_lSize-m_lCurPos;

	memcpy(pBuff,m_pData+m_lCurPos,lSize);
	m_lCurPos += lSize;

	return lSize;
}

int SFileHandle::Seek(long lOffset,int iOrigin)
{
	if(!m_pData) return -1;

	long lPos;
	switch(iOrigin)
	{
	case SEEK_SET: lPos = lOffset;			 break;
	case SEEK_CUR: lPos = lOffset+m_lCurPos; break;
	case SEEK_END: lPos = m_lSize+lOffset;	 break;
	default:								 return -1;
	}
	if(lPos < 0 || lPos > m_lSize) return -1;

	m_lCurPos = lPos;

	return 0;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

SFileSystem::SFileSystem()
{
	m_pFile = NULL;

	CloseFileSystem();
}

SFileSystem::~SFileSystem()
{
	CloseFileSystem();
}

void SFileSystem::SetCurDir(const char* pTok)
{
	if(!strcmp(pTok,".")) return;

	if(!strcmp(pTok,".."))
	{
		if(strlen(m_strCurDir) == 1 && m_strCurDir[0] == '/') return;

		char strCurDir[_MAX_PATH];
		StringCchCopy(strCurDir,_MAX_PATH,m_strCurDir);

		int iLen = (int)strlen(strCurDir);

		if(strCurDir[iLen-1] == '/') strCurDir[iLen-1] = '\0';

		for(int i=0;i<_MAX_PATH;i++)
		{
			char* cp = strCurDir+(_MAX_PATH-1-i);
			if(*cp == '/') break;
			*cp = '\0';
		}

		StringCchCopy(m_strCurDir,_MAX_PATH,strCurDir);
		return;
	}

	StringCchCat(m_strCurDir,_MAX_PATH,pTok);
	StringCchCat(m_strCurDir,_MAX_PATH,"/");
}

void SFileSystem::GetSafeName(char* strDst,const char* strName)
{
	SecureZeroMemory(strDst,_MAX_FNAME);

	int iLen = (int)strlen(strName);
	if(iLen >= _MAX_FNAME) iLen = _MAX_FNAME-1;

	strncpy_s(strDst,_MAX_FNAME,strName,iLen);
}

BOOL SFileSystem::SearchDir(const char* strName,SFileHandle* pFH)
{
	for( ; ; )
	{
		fread(&pFH->m_FileContext,sizeof(FILECONTEXT),1,m_pFile);

		if(!_stricmp(strName,pFH->m_FileContext.strName)) return TRUE;

		pFH->m_FileContext.strName[0] = '\0';

		if(!pFH->m_FileContext.lNext) return FALSE;

		fseek(m_pFile,pFH->m_FileContext.lNext,SEEK_SET);
	}
}

BOOL SFileSystem::OptimizeDir(SFileSystem* pFS,const char* strName)
{
	if(!ChangeDir(strName) || !pFS->AddDir(strName) || !pFS->ChangeDir(strName)) return FALSE;

	SFileHandleList_i it;
	for(unsigned int i=0;i<m_FileHandleList.size();i++)
	{
		it = m_FileHandleList.begin();
		for(unsigned int j=0;j<i;j++) ++it;

		if((*it)->m_FileContext.iType == FT_FILE)
		{
			SFileHandle* pFH = OpenFileForOptimize((*it)->m_FileContext.strName);
			if(!pFH->m_lSize) continue;

			BYTE* pData = new BYTE[pFH->m_lSize];
			if(!pFH->Read(pData,pFH->m_lSize))
			{
				delete[] pData;
				CloseFile(pFH);
				return FALSE;
			}

			CloseFile(pFH);

			if(!pFS->AddFileForOptimize((*it)->m_FileContext.strName,pData,(*it)->m_FileContext.lCompressSize,(*it)->m_FileContext.lUncompressSize))
			{
				delete[] pData;
				return FALSE;
			}

			delete[] pData;
		}
		else if((*it)->m_FileContext.iType == FT_DIR)
		{
			if(strcmp((*it)->m_FileContext.strName,".") && strcmp((*it)->m_FileContext.strName,".."))
			{
				char strTemp[_MAX_FNAME];
				StringCchCopy(strTemp,_MAX_FNAME,(*it)->m_FileContext.strName);
				if(!OptimizeDir(pFS,strTemp)) return FALSE;
			}
		}
	}

	ChangeDir("..");
	pFS->ChangeDir("..");

	return TRUE;
}

void SFileSystem::CloseFileSystem()
{
	if(m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	for(SFileHandleList_i it=m_FileHandleList.begin();it!=m_FileHandleList.end();++it) delete *it;
	m_FileHandleList.clear();

	//for(int i=0;i<FS_MAX_OPEN;i++)
	//{
		//CloseFile(&m_HandleList[i]);
	//}

	CloseFile(&m_HandleList);

	m_strCurDir[0] = '\0';
}

BOOL SFileSystem::OpenFileSystem(const char* strPath)
{
	CloseFileSystem();

	fopen_s(&m_pFile, strPath, "r+b");
	if(!m_pFile) return FALSE;

	char strHeader[FS_MAX_HEADER];
	fread(strHeader,strlen(FS_HEADER_STR)+1,1,m_pFile);
	if(strcmp(strHeader,FS_HEADER_STR))
	{
		fclose(m_pFile);
		m_pFile = NULL;
		return FALSE;
	}

	ChangeDir("/");

	return TRUE;
}

BOOL SFileSystem::NewFileSystem(const char* strPath)
{
	FILE* fp = NULL;
	fopen_s(&fp, strPath, "w+b");
	if(!fp) return FALSE;

	FILECONTEXT FileContext;
	SecureZeroMemory(&FileContext,sizeof(FILECONTEXT));
	FileContext.iType = FT_DIR;
	StringCchCopy(FileContext.strName,_MAX_FNAME,".");
	FileContext.lCompressSize = 0;
	FileContext.lUncompressSize = 0;
	FileContext.lSub = (long)strlen(FS_HEADER_STR)+1; // 미결 - 분석요망
	FileContext.lNext = 0;

	fwrite(FS_HEADER_STR,strlen(FS_HEADER_STR)+1,1,fp);
	fwrite(&FileContext,sizeof(FILECONTEXT),1,fp);

	fclose(fp);

	return TRUE;
}

BOOL SFileSystem::OptimizeFileSystem(const char* strPath)
{
	if(!m_pFile) return FALSE;

	SFileSystem FileSystem;
	if(!FileSystem.NewFileSystem(strPath) || !FileSystem.OpenFileSystem(strPath)) return FALSE;

	char strOldPath[_MAX_PATH];
	StringCchCopy(strOldPath,_MAX_PATH,m_strCurDir);

	ChangeDir("/");

	SFileHandleList_i it;
	for(unsigned int i=0;i<m_FileHandleList.size();i++)
	{
		it = m_FileHandleList.begin();
		for(unsigned int j=0;j<i;j++) ++it;

		if((*it)->m_FileContext.iType == FT_DIR)
		{
			if(strcmp((*it)->m_FileContext.strName,".") && strcmp((*it)->m_FileContext.strName,".."))
			{
				char strTemp[_MAX_FNAME];
				StringCchCopy(strTemp,_MAX_FNAME,(*it)->m_FileContext.strName);
				if(!OptimizeDir(&FileSystem,strTemp)) return FALSE; // 미결 - 분석요망
			}
		}
		else if((*it)->m_FileContext.iType == FT_FILE)
		{
			SFileHandle* pFH = OpenFileForOptimize((*it)->m_FileContext.strName);
			if(!pFH->m_lSize) continue;

			BYTE* pData = new BYTE[pFH->m_lSize];
			if(!pFH->Read(pData,pFH->m_lSize))
			{
				delete[] pData;
				CloseFile(pFH);
				return FALSE;
			}

			CloseFile(pFH);

			if(!FileSystem.AddFileForOptimize((*it)->m_FileContext.strName,pData,(*it)->m_FileContext.lCompressSize,(*it)->m_FileContext.lUncompressSize))
			{
				delete[] pData;
				return FALSE;
			}

			delete[] pData;
		}
	}

	FileSystem.CloseFileSystem();
	ChangeDir(strOldPath);
	
	return TRUE;
}

void SFileSystem::CloseFile(SFileHandle* pFH)
{
	if(!m_pFile) return;

	SecureZeroMemory(&pFH->m_FileContext,sizeof(FILECONTEXT));

	if(pFH->m_pData)
	{
		delete[] pFH->m_pData;
		pFH->m_pData = NULL;
	}
	pFH->m_lSrtPos = 0;

	pFH->m_pData = NULL;
	pFH->m_lSize = 0;
	pFH->m_lCurPos = 0;
}

SFileHandle* SFileSystem::OpenFile(const char* strPath)
{
	if(!m_pFile) return NULL;

	char strOldPath[_MAX_PATH];
	StringCchCopy(strOldPath,_MAX_PATH,m_strCurDir);

	char strCurPath[_MAX_PATH];
	GetPathOnly(strCurPath,strPath);

	if(strCurPath[0]) ChangeDir(strCurPath);

	char strName[_MAX_FNAME];
	GetNameOnly(strName,strPath);

	SFileHandleList_i it;
	for(it=m_FileHandleList.begin();it!=m_FileHandleList.end();++it)
	{
		if((*it)->m_FileContext.iType == FT_FILE)
		{
			if(!_stricmp(strName,(*it)->m_FileContext.strName)) break;
		}
	}
	if(it == m_FileHandleList.end()) return NULL;

	//int iHandle = -1;
	//for(int i=0;i<FS_MAX_OPEN;i++)
	//{
	//	if(!m_HandleList[i].m_FileContext.strName[0])
	//	{
	//		iHandle = i;
	//		break;
	//	}
	//}
	//if(iHandle < 0) return NULL;

	fseek(m_pFile,(*it)->m_lSrtPos+sizeof(FILECONTEXT),SEEK_SET);

	long lSrcSize = (*it)->m_FileContext.lCompressSize;
	BYTE* pSrcData = new BYTE[lSrcSize];
	fread(pSrcData,lSrcSize,1,m_pFile);

	long lDstSize = (*it)->m_FileContext.lUncompressSize;
	BYTE* pDstData = new BYTE[lDstSize];
	if(uncompress(pDstData,(uLongf*)&lDstSize,pSrcData,lSrcSize) != Z_OK)
	{
		delete[] pSrcData;
		delete[] pDstData;
		return NULL;
	}

	delete[] pSrcData;

	//m_HandleList[iHandle].m_FileContext.iType = (*it)->m_FileContext.iType;
	//StringCchCopy(m_HandleList[iHandle].m_FileContext.strName,(*it)->m_FileContext.strName);
	//m_HandleList[iHandle].m_FileContext.lCompressSize = lSrcSize;
	//m_HandleList[iHandle].m_FileContext.lUncompressSize = lDstSize;
	//m_HandleList[iHandle].m_FileContext.lSub = 0;
	//m_HandleList[iHandle].m_FileContext.lNext = 0;
	//m_HandleList[iHandle].m_pData = pDstData;
	//m_HandleList[iHandle].m_lSize = lDstSize;
	//m_HandleList[iHandle].m_lCurPos = 0;

	m_HandleList.m_FileContext.iType = (*it)->m_FileContext.iType;
	StringCchCopy(m_HandleList.m_FileContext.strName,_MAX_FNAME,(*it)->m_FileContext.strName);
	m_HandleList.m_FileContext.lCompressSize = lSrcSize;
	m_HandleList.m_FileContext.lUncompressSize = lDstSize;
	m_HandleList.m_FileContext.lSub = 0;
	m_HandleList.m_FileContext.lNext = 0;
	m_HandleList.m_pData = pDstData;
	m_HandleList.m_lSize = lDstSize;
	m_HandleList.m_lCurPos = 0;

	if(strCurPath[0]) ChangeDir(strOldPath);

	return &m_HandleList;
}

SFileHandle* SFileSystem::OpenFileForOptimize(const char* strPath)
{
	if(!m_pFile) return NULL;

	char strOldPath[_MAX_PATH];
	StringCchCopy(strOldPath,_MAX_PATH,m_strCurDir);

	char strCurPath[_MAX_PATH];
	GetPathOnly(strCurPath,strPath);

	if(strCurPath[0]) ChangeDir(strCurPath);

	char strName[_MAX_FNAME];
	GetNameOnly(strName,strPath);

	SFileHandleList_i it;
	for(it=m_FileHandleList.begin();it!=m_FileHandleList.end();++it)
	{
		if((*it)->m_FileContext.iType == FT_FILE)
		{
			if(!_stricmp(strName,(*it)->m_FileContext.strName)) break;
		}
	}
	if(it == m_FileHandleList.end()) return NULL;

	//int iHandle = -1;
	//for(int i=0;i<FS_MAX_OPEN;i++)
	//{
	//	if(!m_HandleList[i].m_FileContext.strName[0])
	//	{
	//		iHandle = i;
	//		break;
	//	}
	//}
	//if(iHandle < 0) return NULL;

	fseek(m_pFile,(*it)->m_lSrtPos+sizeof(FILECONTEXT),SEEK_SET);

	long lSize = (*it)->m_FileContext.lCompressSize;
	BYTE* pData = new BYTE[lSize];
	fread(pData,lSize,1,m_pFile);

	//m_HandleList[iHandle].m_FileContext.iType = (*it)->m_FileContext.iType;
	//StringCchCopy(m_HandleList[iHandle].m_FileContext.strName,(*it)->m_FileContext.strName);
	//m_HandleList[iHandle].m_FileContext.lCompressSize = lSize;
	//m_HandleList[iHandle].m_FileContext.lUncompressSize = (*it)->m_FileContext.lUncompressSize;
	//m_HandleList[iHandle].m_FileContext.lSub = 0;
	//m_HandleList[iHandle].m_FileContext.lNext = 0;
	//m_HandleList[iHandle].m_pData = pData;
	//m_HandleList[iHandle].m_lSize = lSize;
	//m_HandleList[iHandle].m_lCurPos = 0;

	m_HandleList.m_FileContext.iType = (*it)->m_FileContext.iType;
	StringCchCopy(m_HandleList.m_FileContext.strName,_MAX_FNAME,(*it)->m_FileContext.strName);
	m_HandleList.m_FileContext.lCompressSize = lSize;
	m_HandleList.m_FileContext.lUncompressSize = (*it)->m_FileContext.lUncompressSize;
	m_HandleList.m_FileContext.lSub = 0;
	m_HandleList.m_FileContext.lNext = 0;
	m_HandleList.m_pData = pData;
	m_HandleList.m_lSize = lSize;
	m_HandleList.m_lCurPos = 0;

	if(strCurPath[0]) ChangeDir(strOldPath);

	return &m_HandleList;
}

BOOL SFileSystem::AddDir(const char* strName)
{
	if(!m_pFile) return FALSE;

	FILECONTEXT FileContext;
	SecureZeroMemory(&FileContext,sizeof(FILECONTEXT));

	FileContext.iType = FT_DIR;
	GetSafeName(FileContext.strName,strName);
	if(CheckNameExist(FileContext.strName)) return FALSE;

	fseek(m_pFile,0,SEEK_END);
	long lNext = ftell(m_pFile);

	FileContext.lSub = ftell(m_pFile)+sizeof(FILECONTEXT);
	fwrite(&FileContext,sizeof(FILECONTEXT),1,m_pFile);

	SecureZeroMemory(FileContext.strName,_MAX_FNAME);
	StringCchCopy(FileContext.strName,_MAX_FNAME,".");
	FileContext.lNext = ftell(m_pFile)+sizeof(FILECONTEXT);
	fwrite(&FileContext,sizeof(FILECONTEXT),1,m_pFile);

	SecureZeroMemory(FileContext.strName,_MAX_FNAME);
	StringCchCopy(FileContext.strName,_MAX_FNAME,"..");
	SFileHandle* pFFH = m_FileHandleList.front();
	FileContext.lSub = pFFH->m_lSrtPos;
	FileContext.lNext = 0;
	fwrite(&FileContext,sizeof(FILECONTEXT),1,m_pFile);

	SFileHandle* pBFH = m_FileHandleList.back();
	fseek(m_pFile,pBFH->m_lSrtPos,SEEK_SET);
	fseek(m_pFile,FC_OFFSET_NEXT,SEEK_CUR);
	fwrite(&lNext,sizeof(long),1,m_pFile);

	if(!ChangeDir(".")) return FALSE;

	return TRUE;
}

BOOL SFileSystem::AddFile(const char* strPath)
{
	if(!m_pFile) return FALSE;

	FILE* fp = NULL;
	fopen_s(&fp, strPath, "rb");
	if(!fp) return FALSE;

	fseek(fp,0,SEEK_END);
	long lSize = ftell(fp);
	if(!lSize)
	{
		fclose(fp);
		return TRUE;
	}

	fseek(fp,0,SEEK_SET);
	BYTE* pData = new BYTE[lSize];
	fread(pData,lSize,1,fp);

	fclose(fp);

	char strName[_MAX_FNAME];
	GetNameOnly(strName,strPath);
	if(!AddFile(strName,pData,lSize))
	{
		delete[] pData;
		return FALSE;
	}

	delete[] pData;

	return TRUE;
}

BOOL SFileSystem::AddFile(const char* strName,BYTE* pData,long lSize)
{
	if(!m_pFile) return FALSE;

	if(lSize <= 0) return TRUE;

	long lDstSize = lSize+12+max(1,(lSize+12)/1000);
	BYTE* pDstData = new BYTE[lDstSize];
	if(compress(pDstData,(uLongf*)&lDstSize,pData,lSize) != Z_OK)
	{
		delete[] pDstData;
		return FALSE;
	}

	FILECONTEXT FileContext;
	SecureZeroMemory(&FileContext,sizeof(FILECONTEXT));

	FileContext.iType = FT_FILE;
	GetNameOnly(FileContext.strName,strName);
	if(CheckNameExist(FileContext.strName))
	{
		delete[] pDstData;
		return FALSE;
	}

	FileContext.lCompressSize = lDstSize;
	FileContext.lUncompressSize = lSize;

	fseek(m_pFile,0,SEEK_END);
	long lNext = ftell(m_pFile);

	fwrite(&FileContext,sizeof(FILECONTEXT),1,m_pFile);
	fwrite(pDstData,lDstSize,1,m_pFile);

	delete[] pDstData;

	SFileHandle* pBFH = m_FileHandleList.back();
	pBFH->m_FileContext.lNext = lNext;
	fseek(m_pFile,pBFH->m_lSrtPos,SEEK_SET);
	fseek(m_pFile,FC_OFFSET_NEXT,SEEK_CUR);
	fwrite(&lNext,sizeof(lNext),1,m_pFile);

	SFileHandle* pFH = new SFileHandle;
	*pFH->GetFileContext() = FileContext;
	pFH->m_lSrtPos = lNext;
	pFH->m_lCurPos = 0;
	m_FileHandleList.push_back(pFH);

	return TRUE;
}

BOOL SFileSystem::AddFileForOptimize(const char* strName,BYTE* pData,long lCompressSize,long lUncompressSize)
{
	if(!m_pFile) return FALSE;

	if(lCompressSize <= 0) return TRUE;

	FILECONTEXT FileContext;
	SecureZeroMemory(&FileContext,sizeof(FILECONTEXT));

	FileContext.iType = FT_FILE;
	GetNameOnly(FileContext.strName,strName);
	if(CheckNameExist(FileContext.strName)) return FALSE;

	FileContext.lCompressSize = lCompressSize;
	FileContext.lUncompressSize = lUncompressSize;

	fseek(m_pFile,0,SEEK_END);
	long lNext = ftell(m_pFile);

	fwrite(&FileContext,sizeof(FILECONTEXT),1,m_pFile);
	fwrite(pData,lCompressSize,1,m_pFile);

	SFileHandle* pBFH = m_FileHandleList.back();
	pBFH->m_FileContext.lNext = lNext;
	fseek(m_pFile,pBFH->m_lSrtPos,SEEK_SET);
	fseek(m_pFile,FC_OFFSET_NEXT,SEEK_CUR);
	fwrite(&lNext,sizeof(lNext),1,m_pFile);

	SFileHandle* pFH = new SFileHandle;
	*pFH->GetFileContext() = FileContext;
	pFH->m_lSrtPos = lNext;
	pFH->m_lCurPos = 0;
	m_FileHandleList.push_back(pFH);

	return TRUE;
}

BOOL SFileSystem::Remove(const char* strName)
{
	if(!m_pFile) return FALSE;

	if(!strcmp(strName,".") || !strcmp(strName,"..")) return FALSE;

	for(SFileHandleList_i it=m_FileHandleList.begin();it!=m_FileHandleList.end();++it)
	{
		if(!_stricmp(strName,(*it)->m_FileContext.strName))
		{
			long lNext = (*it)->m_FileContext.lNext;

			--it;
			(*it)->m_FileContext.lNext = lNext;
			fseek(m_pFile,(*it)->m_lSrtPos,SEEK_SET);
			fseek(m_pFile,FC_OFFSET_NEXT,SEEK_CUR);
			fwrite(&lNext,sizeof(long),1,m_pFile);

			++it;
			delete (*it);
			m_FileHandleList.erase(it);
			break;
		}
	}

	return TRUE;
}

BOOL SFileSystem::Rename(const char* strName,const char* strRename)
{
	if(!m_pFile) return FALSE;

	if(CheckNameExist(strRename)) return FALSE;
	if(!_stricmp(strName,strRename)) return TRUE;
	if(!strcmp(strName,".") || !strcmp(strName,"..")) return FALSE;

	for(SFileHandleList_i it=m_FileHandleList.begin();it!=m_FileHandleList.end();++it)
	{
        if(!_stricmp(strName,(*it)->m_FileContext.strName))
		{
			GetSafeName((*it)->m_FileContext.strName,strRename);

			long lPos = ftell(m_pFile);
			fseek(m_pFile,(*it)->m_lSrtPos+FC_OFFSET_NAME,SEEK_SET);
			fwrite((*it)->m_FileContext.strName,_MAX_FNAME,1,m_pFile);
			fseek(m_pFile,lPos,SEEK_SET);

			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL SFileSystem::ChangeDir(const char* strPath)
{
	if(!m_pFile) return FALSE;

	if(!strPath || !strlen(strPath)) return TRUE;

	long lPos = ftell(m_pFile);

	if(m_FileHandleList.size())
	{
		SFileHandle* pFFH = m_FileHandleList.front();
		fseek(m_pFile,pFFH->m_lSrtPos,SEEK_SET);
	}

	char strTemp[_MAX_PATH];
	StringCchCopy(strTemp,_MAX_PATH,strPath);

	if(strTemp[0] == '/')
	{
		fseek(m_pFile,(long)strlen(FS_HEADER_STR)+1,SEEK_SET);
		StringCchCopy(m_strCurDir,_MAX_PATH,"/");
	}

	char* pTok = NULL;
	char* nTok = NULL;
	SFileHandle FH;
	for(pTok=strtok_s(strTemp,"\\/\x0\n\r\t",&nTok);pTok;pTok=strtok_s(NULL,"\\/\x0\n\r\t",&nTok))
	{
		if(!SearchDir(pTok,&FH))
		{
			fseek(m_pFile,lPos,SEEK_SET);
			return FALSE;
		}

		fseek(m_pFile,FH.m_FileContext.lSub,SEEK_SET);
		SetCurDir(pTok);
	}

	for(SFileHandleList_i it=m_FileHandleList.begin();it!=m_FileHandleList.end();++it) delete *it;
	m_FileHandleList.clear();

	lPos = ftell(m_pFile);

	SFileHandle* pFH;
	for(long lSrtPos=lPos;lSrtPos;lSrtPos=pFH->m_FileContext.lNext)
	{
		pFH = new SFileHandle;
		fread(&pFH->m_FileContext,sizeof(FILECONTEXT),1,m_pFile);
		fseek(m_pFile,pFH->m_FileContext.lNext,SEEK_SET);
		pFH->m_lSrtPos = lSrtPos;
		pFH->m_lCurPos = 0;
		m_FileHandleList.push_back(pFH);
	}

	fseek(m_pFile,lPos,SEEK_SET);

	return TRUE;
}

BOOL SFileSystem::CheckNameExist(const char* strName)
{
	for(SFileHandleList_i it=m_FileHandleList.begin();it!=m_FileHandleList.end();++it)
	{
		if(!_stricmp(strName,(*it)->m_FileContext.strName)) return TRUE;
	}

	return FALSE;
}

void SFileSystem::GetNameOnly(char* strDst,const char* strPath)
{
	char strTemp[_MAX_PATH];
	StringCchCopy(strTemp,_MAX_PATH,strPath);
	_strrev(strTemp);

	char* cp = NULL;
	char* np = NULL;
	cp = strtok_s(strTemp, "\\/\x0\n\r\t",&np);
	StringCchCopy(strTemp,_MAX_PATH,cp);
	_strrev(strTemp);
	GetSafeName(strDst,strTemp);
}

void SFileSystem::GetPathOnly(char* strDst,const char* strPath)
{
	char strName[_MAX_FNAME];
	GetNameOnly(strName,strPath);

	int iLen = (int)(strlen(strPath)-strlen(strName));

	if(!iLen)
	{
		strDst[0] = '\0';
	}
	else
	{
		strncpy_s(strDst,iLen + 1,strPath,iLen);
		strDst[iLen] = '\0';
	}
}
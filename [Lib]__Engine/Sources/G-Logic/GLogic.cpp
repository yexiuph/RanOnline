#include "pch.h"

#include "GLogic.h"
#include "./DxSkinAniMan.h"
#include "./DxEffSinglePropGMan.h"
#include "./DxEffcharData.h"
#include "./DxLandMan.h"
#include "./DxSkinObject.h"
#include "./SerialMemory.h"
#include "./SerialFile.h"
#include "./StringMemory.h"
#include "./StringFile.h"
#include "./Unzipper.h"
#include "../[Lib]__MfcEx/Sources/RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace GLOGIC
{
	std::string strPATH;
	std::string strPATH_SERVER;

	BOOL bENGLIB_ZIPFILE = FALSE;
	BOOL bGLOGIC_ZIPFILE = FALSE;
	BOOL bGLOGIC_PACKFILE = FALSE;

	std::string	strANIMATION_ZIPFILE;
	std::string	strEFFECT_ZIPFILE;
	std::string	strEFFECT_CHAR_ZIPFILE;
	std::string	strMAP_ZIPFILE;
	std::string	strSKINOBJ_ZIPFILE;
	std::string	strGLOGIC_SERVER_ZIPFILE;

	SFileSystem	sFileSystem;

	void SetEngFullPath ()
	{
		strANIMATION_ZIPFILE = DxSkinAniMan::GetInstance().GetPath();
		strANIMATION_ZIPFILE += "Animation.rcc";
		
		strEFFECT_ZIPFILE = DxEffSinglePropGMan::GetInstance().GetPath();
		strEFFECT_ZIPFILE += "Effect.rcc";

		strEFFECT_CHAR_ZIPFILE = DxEffcharDataMan::GetInstance().GetPath();
		strEFFECT_CHAR_ZIPFILE += "EffectChar.rcc";

		strMAP_ZIPFILE = DxLandMan::GetPath();
		strMAP_ZIPFILE += "Map.rcc";

		strSKINOBJ_ZIPFILE = DxSkinObject::GetPath();
		strSKINOBJ_ZIPFILE += "SkinObject.rcc";
	
		strGLOGIC_SERVER_ZIPFILE = GLOGIC::GetServerPath();
		strGLOGIC_SERVER_ZIPFILE += "GLogicServer.rcc";

		CUnzipper::LOADFILE_RCC( strANIMATION_ZIPFILE );
		CUnzipper::LOADFILE_RCC( strEFFECT_ZIPFILE );
		CUnzipper::LOADFILE_RCC( strEFFECT_CHAR_ZIPFILE );
		CUnzipper::LOADFILE_RCC( strMAP_ZIPFILE );
		CUnzipper::LOADFILE_RCC( strSKINOBJ_ZIPFILE );
		CUnzipper::LOADFILE_RCC( strGLOGIC_SERVER_ZIPFILE );
	}

	void OpenPackFile ( const char * szAppPath )
	{
		if( bGLOGIC_PACKFILE )
		{
			std::string strPackDataPath;
			strPackDataPath = szAppPath;
			strPackDataPath += RANPARAM::RPFDataPath;

			// SFileSystem Open
			bGLOGIC_PACKFILE = sFileSystem.OpenFileSystem( strPackDataPath.c_str() );
		}
	}

	void SetPath ( const char* szPath )
	{
		strPATH = szPath;
	}
	
	const char* GetPath ()
	{
		return strPATH.c_str();
	}

	void SetServerPath ( const char* szPath )
	{
		strPATH_SERVER = szPath;
	}

	const char* GetServerPath ()
	{
		return strPATH_SERVER.c_str();
	}

	basestream* openfile_basestream (	BOOL bZIP, 
										const char* szZipFile, 
										const char* szFullPath, 
										const char* szFile, 
										bool bENC,
										BOOL bPack )
	{
		basestream *pBStream;
		if ( bZIP )
		{
			// note : ZIP 파일 사용
			CSerialMemory *pMemory = new CSerialMemory;
			if( !pMemory->OpenFile ( szZipFile, szFile, bENC, bPack ) )
			{
				SAFE_DELETE(pMemory);
				return NULL;
			}

			pBStream = pMemory;
		}
		else
		{
			// note : 기존 방식으로 사용
			CSerialFile *pFile = new CSerialFile;
			if ( !pFile->OpenFile ( FOT_READ, szFullPath, bENC ) )
			{
				SAFE_DELETE(pFile);
				return NULL;
			}

			pBStream = pFile;
		}
		
		return pBStream;
	}

	CBaseString* openfile_basestring (	BOOL bZIP, 
										const char* szZipFile, 
										const char* szFullPath, 
										const char* szFile, 
										bool bENC,
										BOOL bPack )
	{
		CBaseString *pBString;
		if ( bZIP )
		{
			// note : ZIP 파일 사용
			CStringMemory *pMemory = new CStringMemory;
			if( !pMemory->Open( szZipFile, szFile, bENC, bPack ) )
			{
				SAFE_DELETE(pMemory);
				return NULL;
			}

			pBString = pMemory;
		}
		else
		{
			// note : 기존 방식으로 사용
			CStringFile *pFile = new CStringFile;
			if ( !pFile->Open( szFullPath, bENC ) )
			{
				SAFE_DELETE(pFile);
				return NULL;
			}

			pBString = pFile;
		}
		
		return pBString;
	}
};


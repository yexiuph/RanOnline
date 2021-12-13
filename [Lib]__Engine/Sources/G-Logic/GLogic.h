#pragma once

#include "SFileSystem.h"

class basestream;
class CBaseString;
class SFileSystem;

namespace GLOGIC
{
	extern BOOL bENGLIB_ZIPFILE;
	extern BOOL bGLOGIC_ZIPFILE;
	extern BOOL bGLOGIC_PACKFILE;

	extern std::string	strANIMATION_ZIPFILE;
	extern std::string	strEFFECT_ZIPFILE;
	extern std::string	strEFFECT_CHAR_ZIPFILE;
	extern std::string	strMAP_ZIPFILE;
	extern std::string	strSKINOBJ_ZIPFILE;
	extern std::string	strGLOGIC_SERVER_ZIPFILE;

	extern SFileSystem	sFileSystem;

	void SetEngFullPath ();
	void OpenPackFile ( const char * szAppPath );

	void SetPath ( const char* szPath );
	const char* GetPath ();

	void SetServerPath ( const char* szPath );
	const char* GetServerPath ();

	basestream* openfile_basestream (	BOOL bZIP, 
										const char* szZipFile, 
										const char* szFullPath, 
										const char* szFile,
										bool bENC=false,
										BOOL bPack=FALSE );

	CBaseString* openfile_basestring (	BOOL bZIP, 
										const char* szZipFile, 
										const char* szFullPath, 
										const char* szFile, 
										bool bENC=false,
										BOOL bPack=FALSE );
};


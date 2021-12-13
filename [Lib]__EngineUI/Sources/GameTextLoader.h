///////////////////////////////////////////////////////////////////////
//	게임 텍스트 로더
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.8.1] - 작성
//
///////////////////////////////////////////////////////////////////////
#pragma once
#include <map>

namespace NS_TEXT_LOADER
{
	typedef std::map<std::string,CStringArray*>				STRFLAG;
	typedef STRFLAG::iterator								STRFLAG_IT;

	extern BOOL bServer;

	BOOL LoadText( const char* szFileName, WORD progVersion, STRFLAG& mapFlags, BOOL bRefact, BOOL bServerExe = FALSE );
	BOOL ClearText( STRFLAG& mapFlags );

	const CString & GetText( std::string strKey, int Index, STRFLAG& mapFlags );
	int GetNumber( std::string strKey, int Index, STRFLAG& mapFlags );
};
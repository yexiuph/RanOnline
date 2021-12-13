///////////////////////////////////////////////////////////////////////
//	���� �ؽ�Ʈ �δ�
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.8.1] - �ۼ�
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
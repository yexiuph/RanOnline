//	[String Utils],JDH (2002.12.02)
//
//	(2002.12.02) StringSeparate () �Լ� �� ���� �ΰ� �Լ� ����.
//		��Ʈ���� �޾Ƽ� ��ϵ� "������"�� �����Ͽ� ��Ʈ�� ��̿�
//		�־��ִ� �޼ҵ���.
//
#pragma once

#include <set>
#include <string>
#include <vector>
#include <fstream>

namespace STRUTIL
{
	//	Note : "������" ��� �޼ҵ�.
	void RegisterSeparator ( CString strSeparator );
	
	//	Note : ��ϵ� "������" ���� �޼ҵ�.
	void ClearSeparator ();

	//	Note : ��Ʈ���� "������"�� �и��Ͽ� ��Ʈ�� ��̿� �־ ��ȯ.
	//		"����"�� "FALSE"�� ����.
	//		"����"�� "TRUE"�� ����.
	//
	BOOL StringSeparate ( CString &Str, CStringArray &StrArray );


	//	Note : ���� ���� ���ڿ��� �����Ҷ�.
	//
	inline void STRCPY ( PCHAR &szDes, const PCHAR &szSrc )
	{
		SAFE_DELETE_ARRAY(szDes);

		if( !szSrc )	return;
		size_t nStrLen = strlen(szSrc)+1;
		szDes = new char[ nStrLen ];
		StringCchCopy( szDes, nStrLen, szSrc );
	}

	BOOL ChangeExt ( const char* _szSrc, CString &strDes, const char* szDesExt );
	BOOL ChangeExt ( const char* _szSrc, const char* szSrcExt, CString &strDes, const char* szDesExt );

	// CSV ���ڿ� ����� �Լ�
	VOID OutputStrCsv( std::fstream &SFile, std::string &Str );
	VOID InputStrCsv( CString &Src, std::string &Dest );

	BOOL CheckString( const CString & strChk );
	BOOL CheckVietnamString ( const CString & strChk );
	BOOL CheckCharSunjo( const TCHAR chText );
	BOOL CheckCharMoum( const TCHAR chText );
};

class CSEPARATOR
{
protected:
	typedef std::vector<char>		SEP_STR;
	
protected:
	SEP_STR m_Separator;

protected:
	bool IsSeparator ( char cVALUE );

public:
	//	Note : "������" ��� �޼ҵ�.
	void Register ( char cSEP );
	
	//	Note : ��ϵ� "������" ���� �޼ҵ�.
	void Clear ();

	//	Note : ��Ʈ���� "������"�� �и��Ͽ� ��Ʈ�� �迭�� �־ ��ȯ.
	//		"����"�� "false"�� ����.
	//		"����"�� "true"�� ����.
	//
	bool DoSeparate ( const std::string strSource, std::vector<std::string> &strArray );
};

inline const char* bool2sz ( bool bOK )
{
	return bOK?"true":"false";
}

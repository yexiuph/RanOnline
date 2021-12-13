//	[String Utils],JDH (2002.12.02)
//
//	(2002.12.02) StringSeparate () 함수 및 관련 부가 함수 제작.
//		스트링을 받아서 등록된 "구분자"로 구분하여 스트링 어래이에
//		넣어주는 메소드임.
//
#pragma once

#include <set>
#include <string>
#include <vector>
#include <fstream>

namespace STRUTIL
{
	//	Note : "구분자" 등록 메소드.
	void RegisterSeparator ( CString strSeparator );
	
	//	Note : 등록된 "구분자" 제거 메소드.
	void ClearSeparator ();

	//	Note : 스트링을 "구분자"로 분리하여 스트링 어래이에 넣어서 반환.
	//		"실패"시 "FALSE"를 리턴.
	//		"성공"시 "TRUE"를 리턴.
	//
	BOOL StringSeparate ( CString &Str, CStringArray &StrArray );


	//	Note : 가변 길이 문자열을 복사할때.
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

	// CSV 문자열 입출력 함수
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
	//	Note : "구분자" 등록 메소드.
	void Register ( char cSEP );
	
	//	Note : 등록된 "구분자" 제거 메소드.
	void Clear ();

	//	Note : 스트링을 "구분자"로 분리하여 스트링 배열에 넣어서 반환.
	//		"실패"시 "false"를 리턴.
	//		"성공"시 "true"를 리턴.
	//
	bool DoSeparate ( const std::string strSource, std::vector<std::string> &strArray );
};

inline const char* bool2sz ( bool bOK )
{
	return bOK?"true":"false";
}

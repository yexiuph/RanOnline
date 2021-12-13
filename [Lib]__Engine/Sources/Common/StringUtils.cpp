//	Note : 택스트 유틸리티.
//
//	(2002.12.02), [Seperator] 초기 제작. JDH.
//
//
#include "pch.h"
#include "./StringUtils.h"
#include "./CharSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace STRUTIL
{
	typedef std::vector<char>	VEC_STR;
	VEC_STR g_Separator;

	void RegisterSeparator ( CString strSeparator )
	{
		if ( strSeparator.IsEmpty() )	return;

		if ( strSeparator.GetLength() > 1 )
		{
			MessageBox ( NULL, "separator must one char.", strSeparator, MB_OK );
			return;
		}

		g_Separator.push_back ( strSeparator.GetAt(0) );
		std::sort ( g_Separator.begin(), g_Separator.end() );
	}

	void ClearSeparator ()
	{
		g_Separator.clear();
	}

	const int ERR_POS = -1;

	inline char* lower_bound ( char *pFirst, DWORD dwCount, char cFIND )
	{
		for (; 0 < dwCount; )
		{
			// divide and conquer, find half that contains answer
			DWORD dwCount2 = dwCount / 2;
			char *pMiddle = pFirst;
			pMiddle += dwCount2;

			if ( *pMiddle < cFIND )
				pFirst = ++pMiddle, dwCount -= dwCount2 + 1;
			else
				dwCount = dwCount2;
		}

		return (pFirst);
	}

	inline bool binary_search ( char *pFirst, DWORD dwCount, char cFIND )
	{
		const char *pFound = lower_bound ( pFirst, dwCount, cFIND );
		return (pFound!=(pFirst+dwCount) && !(cFIND < *pFound));
	}

	inline BOOL IsSeparator ( const char *_szBuffer, int _nLength, int &_nSepLength )
	{
		_nSepLength = ERR_POS;
		if ( _szBuffer==NULL || _szBuffer[0]==NULL )		return FALSE;

		bool bFOUND = binary_search ( &g_Separator[0], (DWORD)g_Separator.size(), _szBuffer[0] );
		if ( bFOUND )
		{
			_nSepLength = 1;
			return TRUE;
		}

		_nSepLength = 0;
		return FALSE;
	}

	BOOL StringSeparate ( CString &Str, CStringArray &StrArray )
	{
		StrArray.RemoveAll ();

		CString StrTemp;
		int nLength = Str.GetLength();
		LPCTSTR szBuffer = Str.GetString();

		for ( int i=0; i<nLength; )
		{
			char cAt = szBuffer[i];

			if ( CHARSET::IsLeadChar(cAt) )
			{
				StrTemp += cAt;
				i++;

				if ( i<nLength )
				{
					StrTemp += szBuffer[i];
					i++;
				}

				continue;
			}

			int nSepLength = ERR_POS;
			if ( IsSeparator ( szBuffer+i, nLength-i, nSepLength ) )
			{
				if ( nSepLength == ERR_POS )	return FALSE;

				i += nSepLength;

				//	분리된 스트링 추가.
				if ( StrTemp.GetLength() > 0 )
				{
					StrArray.Add ( StrTemp );
					StrTemp = "";
				}
			}
			else
			{
				StrTemp += cAt;
				i++;
			}
		}

		//	분리된 스트링 추가.
		if ( StrTemp.GetLength() > 0 )
		{
			StrArray.Add ( StrTemp );
		}

		return FALSE;
	}

	BOOL ChangeExt ( const char* _szSrc, CString &strDes, const char* szDesExt )
	{
		char szSrcTemp[MAX_PATH] = "";
		StringCchCopy( szSrcTemp, MAX_PATH, _szSrc );

		_strlwr_s ( szSrcTemp );
		char* strExtension = strrchr(szSrcTemp,'.');
		if ( !strExtension )					return FALSE;

		int nLength = (int)(strExtension - szSrcTemp);
		
		for ( int i=0; i<nLength; i++ )
		{
			strDes += szSrcTemp[i];
		}
		strDes += szDesExt;

		return TRUE;
	}

	BOOL ChangeExt ( const char* _szSrc, const char* szSrcExt, CString &strDes, const char* szDesExt )
	{
		char szSrcTemp[MAX_PATH] = "";
		StringCchCopy( szSrcTemp, MAX_PATH, _szSrc );

		_strlwr_s ( szSrcTemp );
		char* strExtension = strrchr(szSrcTemp,'.');
		if ( !strExtension )					return FALSE;
		if ( _stricmp(strExtension,szSrcExt) )	return FALSE;

		int nLength = (int)(strExtension - szSrcTemp);
		
		for ( int i=0; i<nLength; i++ )
		{
			strDes += szSrcTemp[i];
		}

		strDes += szDesExt;

		return TRUE;
	}

	VOID OutputStrCsv( std::fstream &SFile, std::string &Str )
	{
		// Csv 파일 로딩할 시 Separater는 구분자가 연속되면 무시하기때문에 공백 넣어둠
		if( Str.empty() )
		{
			SFile << " " << ",";
			return;
		}

		// 문자열에 쉼표 ','가 포함될 수 있기때문에 숫자1 옆의 문자 '`'로 교체함
		for( std::string::iterator ci = Str.begin(); ci != Str.end(); ++ci )
			if( ( *ci ) == ',' )
				*ci = '`';
		SFile << Str << ",";
	}

	VOID InputStrCsv( CString &Src, std::string &Dest )
	{
		Dest.clear();

		// OutputStrCsv 때문에 공백이면 클리어하고 아니면 값을 저장
		if( CString::StringLength( Src ) == 0 )
			return;
		if( Src.GetAt( 0 ) == ' ' && CString::StringLength( Src ) < 2 )
			return;

		// 문자열에 '`'가 포함되어 있으면 쉼표 ','로 교체함
		Src.Replace( '`', ',' );
		Dest = Src;
	}

	BOOL CheckString( const CString & strChk )
	{
		if( strChk.FindOneOf("~!@#$%^&*+|:?><,.;[]{}()\n\t\v\b\r\a\\\?\'\" ") != -1 )
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOL CheckVietnamString( const CString & strChk )
	{
		int size = 0;
		TCHAR chBefore = '\0';
		size = strChk.GetLength();

		TCHAR strText[MAX_PATH] = {0};
		_tcscpy_s( strText, strChk );

		for ( int i = 0; i < size; ++i )
		{
			if ( CheckCharSunjo(strText[i])  &&  !CheckCharMoum(chBefore) )
			{
				return TRUE;
			}

			chBefore = strText[i];
		}

		return FALSE;
	}
	BOOL CheckCharSunjo( const TCHAR chText )
	{
		const int nSungjo = 5;
		static const TCHAR szSungjo[nSungjo] = { (TCHAR)0xcc, (TCHAR)0xd2, (TCHAR)0xde, (TCHAR)0xec, (TCHAR)0xf2 }; // 성조

		for ( int i = 0; i< nSungjo; ++i )
		{
			if ( chText == szSungjo[i] )
				return TRUE;
		}
		return FALSE;
	}

	BOOL CheckCharMoum( const TCHAR chText )
	{
		const int nMoum = 24;				
		/* A, a, E, e, I, i , O, o, U, u Y, y 베트남 모음들 */
		static const TCHAR szMoum[nMoum] = { (TCHAR)0x41, (TCHAR)0x61, (TCHAR)0xc3, (TCHAR)0xe3, (TCHAR)0xc2, 
											 (TCHAR)0xe2, (TCHAR)0x45, (TCHAR)0x65, (TCHAR)0xca, (TCHAR)0xea,
											 (TCHAR)0x49, (TCHAR)0x69, (TCHAR)0x4f, (TCHAR)0x6f, (TCHAR)0xd4, 
											 (TCHAR)0xf4, (TCHAR)0xd5, (TCHAR)0xf5, (TCHAR)0x55, (TCHAR)0x75,
											 (TCHAR)0xdd, (TCHAR)0xfd, (TCHAR)0x59, (TCHAR)0x79 };

		for ( int i = 0; i< nMoum; ++i )
		{
			if ( chText == szMoum[i] )
				return TRUE;
		}
		return FALSE;
	}
};

void CSEPARATOR::Register ( char cSEP )
{
	m_Separator.push_back ( cSEP );
	std::sort ( m_Separator.begin(), m_Separator.end() );
}

void CSEPARATOR::Clear ()
{
	m_Separator.clear();
}

inline bool CSEPARATOR::IsSeparator ( char cVALUE )
{
	return STRUTIL::binary_search ( &m_Separator[0], (DWORD)m_Separator.size(), cVALUE );
}

bool CSEPARATOR::DoSeparate ( const std::string strSource, std::vector<std::string> &strArray )
{
	strArray.clear();
	std::string strtemp;
	strtemp.reserve ( strSource.size() );

	//CString StrTemp;
	int nLength = (int)strSource.length();
	const char* szSOURCE = strSource.c_str();

	for ( int i=0; i<nLength; )
	{
		char cVAL = szSOURCE[i];

		if ( CHARSET::IsLeadChar(cVAL) )
		{
			strtemp += cVAL;
			++i;

			if ( i<nLength )
			{
				strtemp += szSOURCE[i];
				++i;
			}

			continue;
		}

		if ( !IsSeparator(cVAL) )
		{
			strtemp += cVAL;
			++i;
			continue;
		}

		++i;

		//	분리된 스트링 추가.
		if ( !strtemp.empty() )
		{
			strArray.push_back ( strtemp );
			strtemp = "";
		}
	}

	//	분리된 스트링 추가.
	if ( !strtemp.empty() )
	{
		strArray.push_back ( strtemp );
	}

	return FALSE;
}
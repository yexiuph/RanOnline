#include "pch.h"
#include "./RanFilter.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"
#include "../[Lib]__Engine/Sources/Common/basestring.h"
#include "../[Lib]__Engine/Sources/Common/compbyte.h"

#ifdef UNLEN
#undef UNLEN
#endif
#define UNLEN 512

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRanFilter& CRanFilter::GetInstance()
{
	static CRanFilter Instance;
	return Instance;
}

CRanFilter::CRanFilter()
	: m_nCodePage(CP_ACP)
	, m_pCurrentFilter(NULL)
	, m_bZipFile(FALSE)
	, m_bPackFile(FALSE)
{
}

CRanFilter::~CRanFilter()
{
}

BOOL CRanFilter::Init( CONST BOOL bZipFile, CONST BOOL bPackFile, CONST TCHAR* szZipFile, CONST TCHAR* szPath )
{
	m_bZipFile = bZipFile;
	m_bPackFile = bPackFile;

	m_ChatFilter.Init();
	m_NameFilter.Init();
	m_ChatFilter.SetCodePage( m_nCodePage );
	m_NameFilter.SetCodePage( m_nCodePage );

	if( !ReadToFile( szZipFile, szPath ) ) 
		return FALSE; 

	return TRUE;
}

BOOL CRanFilter::ReadToFile( CONST TCHAR* szZipFile, CONST TCHAR* szPath )
{
	if( !LOADCHAT( szZipFile, szPath ) ) return FALSE;
	if( !LOADNAME( szZipFile, szPath ) ) return FALSE;

	return TRUE;
}

UINT CRanFilter::GetSectionCode( CONST CString & strSection )
{

	if( strSection == _T("[UNICODE]") ) return EM_UNICODE;
	if( strSection == _T("[HEX]") )		return EM_HEX;
	if( strSection == _T("[CHAR]") )	return EM_CHAR;
	
	return EM_ERROR_CODE;
}

BOOL CRanFilter::LOADCHAT( CONST TCHAR* szZipFile, CONST TCHAR* szPath )
{
	m_pCurrentFilter = &m_ChatFilter;
	return LOAD( szZipFile, szPath, _T("moblogic1.bin") );
}

BOOL CRanFilter::LOADNAME( CONST TCHAR* szZipFile, CONST TCHAR* szPath )
{
	m_pCurrentFilter = &m_NameFilter;
	return LOAD( szZipFile, szPath, _T("moblogic2.bin") );
}

BOOL CRanFilter::LOAD( CONST TCHAR* szZipFile, CONST TCHAR* szPath, CONST TCHAR* szFile )
{
	CString strPATH( szPath );
	strPATH += szFile;

	std::auto_ptr<CBaseString> pBString(GLOGIC::openfile_basestring(m_bZipFile, 
																	szZipFile, 
																	strPATH, 
																	szFile, 
																	true, 
																	m_bPackFile ) );

	if ( !pBString.get() )
	{
		CDebugSet::ToLogFile ( "ERROR : CRanFilter::LOAD(), File Open %s", szFile );
		return FALSE;
	}

	CBaseString &cBString = *pBString;

	UINT SectionCode(EM_ERROR_CODE);
	CString strBUFFER;

	while( cBString.GetNextLine(strBUFFER) > 0 )
	{
		if( strBUFFER.IsEmpty() ) continue;

		if( strBUFFER[0] == _T('[') )
		{
			// Note : 해당 섹션 코드를 대입한다.
			SectionCode = GetSectionCode( strBUFFER );
		}
		else
		{
			// Note : 만약 센셕이 아니면 스트링을 해당 코드 함수에 보낸다.
			switch( SectionCode )
			{
			case EM_UNICODE:	InputStringUNICODE( strBUFFER );	break;
			case EM_HEX:		InputStringHEX( strBUFFER );		break;
			case EM_CHAR:		InputStringCHAR( strBUFFER );		break;
			case EM_ERROR_CODE: //에러
			default:
				CDebugSet::ToLogFile( _T("[CRanFilter::LOAD] EM_ERROR_CODE") );
				break;
			}
		}
	}

	return TRUE;
}

VOID CRanFilter::InputStringHEX( CONST CString & strBUFFER)
{
	WCHAR wszBUFFER[UNLEN+1]={0};
	CHAR szBUFFER[256]={0};

	compbyte::string2hex( (UCHAR*)strBUFFER.GetString(), (UCHAR*)szBUFFER, strBUFFER.GetLength() );

	MultiByteToWideChar( m_nCodePage, 0, szBUFFER, (INT)(strlen(szBUFFER)+1),
		wszBUFFER, (INT)(sizeof(wszBUFFER)/sizeof(wszBUFFER[0])) );

	m_pCurrentFilter->addSlang( wszBUFFER );
}

VOID CRanFilter::InputStringCHAR( CONST CString & strBUFFER )
{
	WCHAR wszBUFFER[UNLEN+1]={0};

	MultiByteToWideChar( m_nCodePage, 0, strBUFFER.GetString(), strBUFFER.GetLength()+1,
		wszBUFFER, sizeof(wszBUFFER)/sizeof(wszBUFFER[0]) );

	m_pCurrentFilter->addSlang( wszBUFFER );
}

VOID CRanFilter::InputStringUNICODE( CONST CString & strBUFFER )
{
	if( strBUFFER[0] == _T('$') )	// Note : 범위를 가진 입력버퍼, 문자 하나일때 가능하다.
	{
		INT curPos(0);

		CString strStart = strBUFFER.Tokenize( _T("$ ~"), curPos );
		if( strStart == _T("") )
		{
			CDebugSet::ToLogFile( _T("[CRanFilter]Input Error : UNICODE Range Data") );
			return ;
		}

		WCHAR wszBUFFER[UNLEN+1]={0};
		compbyte::wstring2hex( (UCHAR*)strStart.GetString(), wszBUFFER, strStart.GetLength() );
		// Note : 문자 하나라고 가정한다. 문자열이 될경우 범위 적용을 할 수 없다.
		USHORT dwStart = (USHORT)wszBUFFER[0]; 

		CString strEnd = strBUFFER.Tokenize( _T("$ ~"), curPos );
		if( strEnd == _T("") )
		{
			CDebugSet::ToLogFile( _T("[CRanFilter]Input Error : UNICODE Range Data") );
			return ;
		}

		SecureZeroMemory( wszBUFFER, sizeof(wszBUFFER) );
		compbyte::wstring2hex( (UCHAR*)strEnd.GetString(), wszBUFFER, strEnd.GetLength() );
		// Note : 문자 하나라고 가정한다. 문자열이 될경우 범위 적용을 할 수 없다.
		USHORT dwEnd = (USHORT)wszBUFFER[0];

		for( USHORT i=dwStart; i<=dwEnd; ++i )
		{
			SecureZeroMemory( wszBUFFER, sizeof(wszBUFFER) );
			wszBUFFER[0] = (WCHAR)i;

			m_pCurrentFilter->addSlang( wszBUFFER );
		}
	}
	else
	{
		WCHAR wszBUFFER[UNLEN+1]={0};
		compbyte::wstring2hex( (UCHAR*)strBUFFER.GetString(), wszBUFFER, strBUFFER.GetLength() );

		m_pCurrentFilter->addSlang( wszBUFFER );
	}
}

BOOL CRanFilter::ChatFilter(CONST CString & strBuf)
{
	if( Filter( strBuf, m_ChatFilter ) ) return TRUE;
		
	return FALSE;
}

BOOL CRanFilter::NameFilter(CONST CString & strBuf)
{
	if( Filter( strBuf, m_ChatFilter ) ) return TRUE;
	if( Filter( strBuf, m_NameFilter ) ) return TRUE;
	
	return FALSE;
}

BOOL CRanFilter::Filter( CONST CString & strBuf, CONST SlangFilter & currentFilter )
{
	if ( strBuf.IsEmpty() )	return FALSE;

	CString strBUFFER( strBuf );
	
	WCHAR wszBUFFER[UNLEN+1]={0};

	MultiByteToWideChar( m_nCodePage, 0, strBUFFER.GetString(), strBUFFER.GetLength()+1,
						wszBUFFER, sizeof(wszBUFFER)/sizeof(wszBUFFER[0]) );

	std::wstring strFilted( wszBUFFER );
	BOOL bFind(FALSE);

	strFilted = currentFilter.filter( strFilted );
	INT nCOUNT = WideCharToMultiByte( m_nCodePage, 0, strFilted.c_str(), -1, NULL, 0, NULL, NULL );
	
	TCHAR szBUFFER[UNLEN+1] = {0};
	WideCharToMultiByte( m_nCodePage, 0, strFilted.c_str(), -1, szBUFFER, nCOUNT+1, NULL, NULL );

	m_strFilted = szBUFFER;
	
	if ( strBuf != m_strFilted )
	{
		bFind = TRUE;
	}

	return bFind;
}

//BOOL CRanFilter::LOADPROVERB( CONST BOOL bZipFile, CONST TCHAR* szZipFile, CONST TCHAR* szPath )
//{
//	CString strPATH( CString(szPath) + "moblogic3.bin" );
//
//	std::auto_ptr<CBaseString> pBString( GLOGIC::openfile_basestring( bZipFile, szZipFile, strPATH, "moblogic3.bin", TRUE ) );
//
//	if ( !pBString.get() )
//		return FALSE;
//
//	CBaseString &cBString = *pBString;
//
//	CString strBUFFER;
//
//	while ( cBString.GetNextLine(strBUFFER) > 0 )
//	{
//		if ( strBUFFER.IsEmpty() )	continue;
//
//		WCHAR wszBUFFER[UNLEN+1]={0};
//
//		MultiByteToWideChar( m_nCodePage, 0, strBUFFER.GetString(), strBUFFER.GetLength()+1,
//							wszBUFFER, sizeof(wszBUFFER)/sizeof(wszBUFFER[0]) );
//
//		m_vecStringProverb.push_back( wszBUFFER );
//	}
//
//	return TRUE;
//}

//INT CRanFilter::Nrand()
//{
//	INT n = static_cast<INT>(m_vecStringProverb.size());
//
//	if ( n <= 0 || n > RAND_MAX )
//		throw std::domain_error( "CRanFilter::Nrand()" );
//
//	CONST INT bucket_size = RAND_MAX / n;
//	INT r;
//
//	do r = rand() / bucket_size;
//	while ( r >= n );
//
//	return r;
//}
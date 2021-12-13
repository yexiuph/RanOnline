#pragma once

#include "tinyxml.h"
#include "GameTextLoader.h"

using namespace NS_TEXT_LOADER;
#define MAX_TEXTLEN 1024

class CRanXMLParser
{
	TiXmlDocument m_XmlDoc;	
	CString m_strLang;

public:
	CRanXMLParser( CString & strLang );
	~CRanXMLParser();

private:
	BOOL _ParseWORD( STRFLAG & pData );
	BOOL _ParseSENTENSE( STRFLAG & pData );
	BOOL _ParseNUMBER( STRFLAG & pData );

	//BOOL	_ParseVALUE( STRFLAG & pData, IXMLDOMNode* pNode );
	BOOL IsValidData( const void * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID = _T("") );
	BOOL IsValidData( const TCHAR * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID = _T("") );
public:
	BOOL Parse_XML( STRFLAG & pData, const char* lpszFilename );

	BOOL Parse( STRFLAG & pData, const char* lpszFilename )
	{
		ASSERT( lpszFilename );
		return Parse_XML( pData, lpszFilename );
	}
};
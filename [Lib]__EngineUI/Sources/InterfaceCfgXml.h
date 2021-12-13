#pragma once

#include "tinyxml.h"
#include "InterfaceCfg.h"
#define MAX_TEXTLEN 1024

class CInterfaceCfgXml
{
	TiXmlDocument m_XmlDoc;	
	CString m_strLang;

public:
	CInterfaceCfgXml( CString & strLang );
	~CInterfaceCfgXml();

private:
	BOOL _ParseConfig( MAP_UI_CFG & pData );

	BOOL IsValidData( const void * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID = _T("") );
	BOOL IsValidData( const TCHAR * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID = _T("") );

public:
	BOOL Parse_XML( MAP_UI_CFG & pData, const char* lpszFilename );
	BOOL Parse( MAP_UI_CFG & pData, const char* lpszFilename )
	{
		ASSERT( lpszFilename );
		return Parse_XML( pData, lpszFilename );
	}
};
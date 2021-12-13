#pragma once

#include <msxml2.h>
#include <comdef.h>

#define MAX_TEXTLEN 1024

#pragma comment(lib, "msxml2")

class CRanXML
{
	IXMLDOMDocument * m_pXMLDoc;
	CString m_strUrl;

public:
	CRanXML();
	CRanXML( LPCTSTR url );
	~CRanXML();

	BOOL				Open( LPCTSTR url );
	VOID				Close();
	IXMLDOMNodeList*	FindElement( LPCTSTR strElement );
	IXMLDOMNodeList*	FindElement( IXMLDOMNode* pNode, LPCTSTR strElement );
	INT					GetElementText( IXMLDOMNode* pNode, LPSTR strRet );
	INT					GetAttributeText( IXMLDOMNode* pNode, LPSTR strAttrName, LPSTR strRet );
};
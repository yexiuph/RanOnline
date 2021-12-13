#include "pch.h"
#include "RanXML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define S_REL(p)	{ if(p) p->Release(); p = NULL; }
#define S_DEL(p)	{ if(p) delete p; p = NULL; }
#define S_DELS(p)	{ if(p) delete[] p; }

CRanXML::CRanXML()
	: m_pXMLDoc(NULL)
{
	CoInitialize(NULL);
	CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_ALL, IID_IXMLDOMDocument, (void**)&m_pXMLDoc);
}

CRanXML::CRanXML( LPCTSTR url )
{
	GASSERT( NULL != url );
	m_strUrl = url;
}

CRanXML::~CRanXML()
{
	CoUninitialize();
}

BOOL CRanXML::Open( LPCTSTR url )
{
	GASSERT( NULL != url );

	HRESULT	hr;
	BOOL bRet(TRUE);
	m_strUrl = url;

	try
	{
		FILE* fp;
		fopen_s(&fp, url, "rt");
		if( !fp ) return FALSE;
		fclose( fp );

		short sResult = FALSE;

		m_pXMLDoc->put_async(FALSE);

		_bstr_t varString = (LPCTSTR)url;
		VARIANT path;
		path.vt = VT_BSTR;
		path.bstrVal = varString;

		hr = m_pXMLDoc->load(path, &sResult);
		if(FAILED(hr)) return FALSE;
	}
	catch (...)
	{
		CDebugSet::ToLogFile( "Check Your Internet Explorer Version 6.x. http://update.microsoft.com/windowsupdate" );

		m_pXMLDoc->Release();
		m_pXMLDoc = NULL;
		bRet = FALSE;
	}

	return bRet;
}

VOID CRanXML::Close()
{
	m_pXMLDoc->Release();
}

IXMLDOMNodeList* CRanXML::FindElement( LPCTSTR strElement )
{
	IXMLDOMNodeList* pNodeList = NULL;

	if(m_pXMLDoc == NULL)
		return pNodeList;

	try 
	{
		_bstr_t bstrPath = strElement;
		m_pXMLDoc->selectNodes(bstrPath, &pNodeList);
	}
	catch(...)
	{
	}

	return pNodeList;
}

IXMLDOMNodeList* CRanXML::FindElement( IXMLDOMNode* pNode, LPCTSTR strElement )
{
	IXMLDOMNodeList* pNodeList = NULL;

	if(pNode == NULL)
		return pNodeList;

	try 
	{
		_bstr_t bstrPath = strElement;
		pNode->selectNodes(bstrPath, &pNodeList);
	}
	catch(...)
	{
	}

	return pNodeList;
}

int	CRanXML::GetElementText( IXMLDOMNode* pNode, LPSTR strRet )
{
	BSTR			bstr = NULL;

	pNode->get_text( &bstr );
	StringCchCopy( strRet, MAX_TEXTLEN, (const char*)_bstr_t(bstr,FALSE) );
	//strcpy( strRet,(const char*)_bstr_t(bstr,FALSE) );
	//	wcstombs( strRet, bstr, 128 );
	//	::SysFreeString( bstr );

	return 1;
}

int CRanXML::GetAttributeText( IXMLDOMNode* pNode, LPSTR strAttrName, LPSTR strRet )
{
	wchar_t					wstrAttr[MAX_TEXTLEN];
	IXMLDOMNode*			pAttrNode = NULL;
	IXMLDOMNamedNodeMap*	pMap = NULL;
	VARIANT					varValue;

	try
	{ 
		// int n = (int) mbstowcs( wstrAttr, strAttrName, MAX_TEXTLEN );
		size_t n = 0;
		mbstowcs_s(&n, wstrAttr, strAttrName, MAX_TEXTLEN);
		pNode->get_attributes( &pMap );
		pMap->getNamedItem( wstrAttr, &pAttrNode );
		pAttrNode->get_nodeValue( &varValue );
		StringCchCopy( strRet, MAX_TEXTLEN, (const char*)_bstr_t(varValue.bstrVal,FALSE) );
		//strcpy( strRet,(const char*)_bstr_t(varValue.bstrVal,FALSE) );
		//		wcstombs( strRet, varValue.bstrVal , 128 );

		S_REL( pAttrNode );
		S_REL( pMap );

		return TRUE;
	}
	catch(...)
	{
		S_REL( pAttrNode );
		S_REL( pMap );
		return FALSE;
	}
}
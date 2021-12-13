#ifndef S_CWHITEROCKXML_H_
#define S_CWHITEROCKXML_H_

#pragma once

//#include "tinyxml.h"
#include "../[Lib]__EngineUI/Sources/tinyxml.h"

class CWhiteRockXml
{
	TiXmlDocument m_XmlDoc;	

public:
	CWhiteRockXml();
	~CWhiteRockXml();

public:

	bool LoadXml( const CString& strFileName );
	bool LoadString( const CString& strXmlData );
    bool SaveXml( const CString& strFileName );
	void SaveData();

	bool SetData( const CString& strChildPath, const CString& strData );
	CString GetData ( const CString& strChildPath );
	const char* GetXmlString ();

	void Init();

private:
	bool SetXMLNodeText( TiXmlElement* rootElem, const CString& strChildPath, const CString& strData );
	TiXmlElement* GetChildElement( TiXmlElement* rootElem, const CString& strChildPath );
	bool GetXMLNodeText(TiXmlElement* rootElem, const CString& strChildPath, CString& strNodeText );
};

#endif // S_CWHITEROCKXML_H_
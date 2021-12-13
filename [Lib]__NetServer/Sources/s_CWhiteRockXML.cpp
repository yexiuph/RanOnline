#include "pch.h"
#include "s_CWhiteRockXML.h"

CWhiteRockXml::CWhiteRockXml()	
{
}

CWhiteRockXml::~CWhiteRockXml()
{
	m_XmlDoc.Clear();
}

void CWhiteRockXml::Init()
{
	m_XmlDoc.Clear();
}

bool CWhiteRockXml::LoadXml( const CString& strFileName )
{
	Init();
	if ( strFileName.IsEmpty() )	return false;

	if( !m_XmlDoc.LoadFile(strFileName) )
	{
		return false;
	}

	return true;
}

bool CWhiteRockXml::LoadString( const CString& strXmlData )
{
	Init();
	if ( strXmlData.IsEmpty() )	return false;

	if( !m_XmlDoc.LoadString(strXmlData) )
	{
		return false;
	}

	return true;
}

bool CWhiteRockXml::SaveXml( const CString& strFileName )
{
	if ( strFileName.IsEmpty() )	return false;

//	if( !m_XmlDoc.SaveFile(strFileName) )
//	{
//		return false;
//	}
	m_XmlDoc.SaveFile();

	return true;
}

void CWhiteRockXml::SaveData()
{
	m_XmlDoc.SaveData();
}

bool CWhiteRockXml::SetData( const CString& strChildPath, const CString& strData )
{
	return SetXMLNodeText( m_XmlDoc.RootElement(), strChildPath, strData );
}

CString CWhiteRockXml::GetData ( const CString& strChildPath )
{
	CString strNodeText;
	if ( !GetXMLNodeText( m_XmlDoc.RootElement(), strChildPath, strNodeText ) ) strNodeText.Empty();
	
	return strNodeText;
}

bool CWhiteRockXml::SetXMLNodeText( TiXmlElement* rootElem, const CString& strChildPath, const CString& strData )
{
	TiXmlElement* nodeElem = NULL;
	TiXmlNode* nodeChild = NULL;

	nodeElem = GetChildElement( rootElem, strChildPath );
	if ( nodeElem ) 
	{
		nodeChild = nodeElem->FirstChild();
		if ( !nodeChild )	return false;

		nodeChild->SetValue( strData );	

		return true;
	}

	return false;
}

const char* CWhiteRockXml::GetXmlString ()
{
	SaveData();

	return m_XmlDoc.GetFileData().c_str();
}

TiXmlElement* CWhiteRockXml::GetChildElement( TiXmlElement* rootElem, const CString& strChildPath )
{
	CString resTokenChild;
	CString strCompare;
	CString strChild, strElem, strValue;
	
	int nPosChild = 0;
	int nPosFormat = 0;

	if ( !rootElem ) return NULL;
	
	TiXmlElement* nodeElem = rootElem; 
	TiXmlElement* nodeElemChild = rootElem; 	

	resTokenChild = strChildPath.Tokenize("/",nPosChild);

	while ( resTokenChild != "")
	{
		nodeElemChild = nodeElem->FirstChildElement( resTokenChild );
		if ( !nodeElemChild ) return NULL;		
		
		resTokenChild= strChildPath.Tokenize("/",nPosChild);		
		nodeElem = nodeElemChild;
	};

	return nodeElem;
}

bool CWhiteRockXml::GetXMLNodeText(TiXmlElement* rootElem, const CString& strChildPath, CString& strNodeText )
{
	TiXmlElement* nodeElem = NULL;
	TiXmlNode* nodeChild = NULL;

	nodeElem = GetChildElement( rootElem, strChildPath );
	if ( nodeElem ) 
	{
		nodeChild = nodeElem->FirstChild();
		if ( nodeChild ) strNodeText = nodeChild->Value();
		else strNodeText.Empty();
		return true;
	}

	return false;
	
}
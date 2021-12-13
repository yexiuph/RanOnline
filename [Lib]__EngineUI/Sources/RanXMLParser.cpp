#include "pch.h"
#include "RanXMLParser.h"
#include "DxFontMan.h" //../[Lib]__Engine/Sources/DxCommon
#include "RANPARAM.h" //../[Lib]__MfcEx/Sources

using namespace NS_TEXT_LOADER;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRanXMLParser::CRanXMLParser( CString & strLang )
	: m_strLang(strLang)
{
}

CRanXMLParser::~CRanXMLParser()
{
	m_XmlDoc.Clear();
}

BOOL CRanXMLParser::IsValidData( const void * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID )
{
	if( szValue == NULL )
	{
		CDebugSet::ToLogFile( "[TEXT/%s]Count:%d, ID:%s Data is not valid", szMsg, nCount, szID );
		return FALSE;
	}

	return TRUE;
}

BOOL CRanXMLParser::IsValidData( const TCHAR * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID )
{
	if( szValue[0] ==_T('\0') )
	{
		CDebugSet::ToLogFile( "[TEXT/%s]Count:%d, ID:%s Data is not valid", szMsg, nCount, szID );
		return FALSE;
	}

	return TRUE;
}

BOOL CRanXMLParser::Parse_XML( STRFLAG & pData, const char* lpszFilename )
{
	ASSERT( lpszFilename );

	if( !m_XmlDoc.LoadFile(lpszFilename) )
	{
		return FALSE;
	}

	try
	{
		if( !_ParseWORD( pData ) )		throw;
		if( !_ParseSENTENSE( pData ) )	throw;
		if( !_ParseNUMBER( pData ) )	throw;

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}	
}

BOOL CRanXMLParser::_ParseSENTENSE( STRFLAG & pData )
{
	TCHAR				szValue[MAX_TEXTLEN]={0};
	TCHAR				szId[MAX_TEXTLEN]={0};
	LONG				nCount(0), nCount2(0);
	TiXmlNode* TextNode;
	TiXmlElement* SentenseElem, *ValueElem;	
	
	try
	{
		CONST CString strCRLF("\r\n");
		CONST CString strCommon("Common");
		
		TextNode = m_XmlDoc.FirstChild("TEXT");

		if( !TextNode ) return FALSE;

		SentenseElem = TextNode->FirstChildElement("SENTENSE");
		
		while(SentenseElem){
			
			if( !IsValidData( SentenseElem, "SENTENSE", nCount ) ){
				SentenseElem = SentenseElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			_tcscpy_s(szValue,SentenseElem->Attribute("Ver"));
			if( !IsValidData( szValue, "SENTENSE/Ver", nCount ) ){
				SentenseElem = SentenseElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			_tcscpy_s(szId,SentenseElem->Attribute("Id"));
			if( !IsValidData( szId, "SENTENSE/Id", nCount ) ){
				SentenseElem = SentenseElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}


			ValueElem = SentenseElem->FirstChildElement("VALUE");
			if( !ValueElem ){
				SentenseElem = SentenseElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			CString strBuffer;
			WCHAR szTemp[MAX_TEXTLEN]={0};
			nCount2 = 0;

			while(ValueElem){

				_tcscpy_s(szValue,ValueElem->Attribute("Lang"));
				if( !IsValidData( szValue, "SENTENSE/VALUE/Lang", nCount2 , szId ) ){
					ValueElem = ValueElem->NextSiblingElement("VALUE");
					nCount2++;
					continue;
				}

				if( m_strLang == szValue || strCommon == szValue )
				{
					_tcscpy_s(szValue,ValueElem->FirstChild()->Value());
					if( !IsValidData( szValue, "SENTENSE/VALUE", nCount2, szId ) ){
						ValueElem = ValueElem->NextSiblingElement("VALUE");
						nCount2++;
						continue;
					}

					MultiByteToWideChar(CP_UTF8,0,szValue,(int)ValueElem->FirstChild()->ValueStr().size()+1,szTemp,sizeof(szTemp)/sizeof(szTemp[0]));			
					WideCharToMultiByte(CD3DFontPar::nCodePage[RANPARAM::dwLangSet], 0, szTemp, -1, szValue, sizeof(szValue)/sizeof(szValue[0]), NULL,NULL );	

					strBuffer += szValue;
					strBuffer += strCRLF;
				}
				ValueElem = ValueElem->NextSiblingElement("VALUE");
				nCount2++;

			}			

			int nLen = strBuffer.GetLength();
			
			if( nLen > 1 ) // Note : 길이가 1보다 크다는 것은 무조건 마지막에 \r\n이 존재한다는 것이다.
			{
				strBuffer.SetAt( strBuffer.GetLength()-2, NULL ); // 마지막 \r\n은 제거한다.
			}

				//	데이타 만들기
			CStringArray* pNewStrArray = new CStringArray;
			pNewStrArray->Add( strBuffer );
			
			pData[std::string(szId)] = pNewStrArray;

			SentenseElem = SentenseElem->NextSiblingElement("SENTENSE");
			nCount++;
			
		}

		return TRUE;
	}

	catch(...)
	{
		return FALSE;
	}
}

BOOL CRanXMLParser::_ParseWORD( STRFLAG & pData )
{
	TCHAR				szValue[MAX_TEXTLEN]={0};
	TCHAR				szId[MAX_TEXTLEN]={0};
	LONG				nCount(0), nCount2(0);
	TiXmlNode* TextNode;
	TiXmlElement* WordElem, *ValueElem;
	INT					nIndex(0);

	try
	{
		CONST CString strCommon("Common");
		
		TextNode = m_XmlDoc.FirstChild("TEXT");

		if( !TextNode ) return FALSE;

		WordElem = TextNode->FirstChildElement("WORD");
		
		while(WordElem){
			
			if( !IsValidData( WordElem, "WORD", nCount ) ){
				WordElem = WordElem->NextSiblingElement("WORD");
				nCount++;
				continue;
			}

			_tcscpy_s(szValue,WordElem->Attribute("Ver"));
			if( !IsValidData( szValue, "WORD/Ver", nCount ) ){
				WordElem = WordElem->NextSiblingElement("WORD");
				nCount++;
				continue;
			}

			_tcscpy_s(szId,WordElem->Attribute("Id"));
			if( !IsValidData( szId, "WORD/Id", nCount ) ){
				WordElem = WordElem->NextSiblingElement("WORD");
				nCount++;
				continue;
			}

			ValueElem = WordElem->FirstChildElement("VALUE");
			if( !ValueElem ){
				WordElem = WordElem->NextSiblingElement("WORD");
				nCount++;
				continue;
			}


			CString strBuffer;
			CStringArray* pNewStrArray = new CStringArray;
			WCHAR szTemp[MAX_TEXTLEN]={0};			
			nCount2 = 0;

			while(ValueElem){

				_tcscpy_s(szValue,ValueElem->Attribute("Lang"));
				if( !IsValidData( szValue, "WORD/VALUE/Lang", nCount2 , szId ) ){
					nCount2++;
					ValueElem = ValueElem->NextSiblingElement("VALUE");
					continue;
				}

				if( m_strLang == szValue || strCommon == szValue )
				{	

					_tcscpy_s(szValue,ValueElem->Attribute("Index"));
					if( !IsValidData( szValue, "WORD/VALUE/Index", nCount2 , szId ) ){
						nCount2++;
						ValueElem = ValueElem->NextSiblingElement("VALUE");
						continue;
					}

					nIndex = _tstoi( szValue );
					ASSERT( nCount2 == nIndex );

					_tcscpy_s(szValue,ValueElem->FirstChild()->Value());
					if( !IsValidData( szValue, "WORD/VALUE", nCount2, szId ) ){
						nCount2++;
						ValueElem = ValueElem->NextSiblingElement("VALUE");
						continue;
					}

					MultiByteToWideChar(CP_UTF8,0,szValue,(int)ValueElem->FirstChild()->ValueStr().size()+1,szTemp,sizeof(szTemp)/sizeof(szTemp[0]));			
					WideCharToMultiByte(CD3DFontPar::nCodePage[RANPARAM::dwLangSet], 0, szTemp, -1, szValue, sizeof(szValue)/sizeof(szValue[0]), NULL,NULL );	

					pNewStrArray->SetAtGrow( nIndex, szValue );
				}
				
				nCount2++;
				ValueElem = ValueElem->NextSiblingElement("VALUE");

			}			

			pData[std::string(szId)] = pNewStrArray;

			WordElem = WordElem->NextSiblingElement("WORD");
			nCount++;
			
		}

		return TRUE;
	}

	catch(...)
	{
		return FALSE;
	}	
}

BOOL CRanXMLParser::_ParseNUMBER( STRFLAG & pData )
{
	TCHAR				szValue[MAX_TEXTLEN]={0};
	TCHAR				szId[MAX_TEXTLEN]={0};
	LONG				nCount(0), nCount2(0);
	TiXmlNode* TextNode;
	TiXmlElement* NumberElem, *ValueElem;

	try
	{
		CONST CString strCRLF("\r\n");
		CONST CString strCommon("Common");
		
		TextNode = m_XmlDoc.FirstChild("TEXT");

		if( !TextNode ) return FALSE;

		NumberElem = TextNode->FirstChildElement("NUMBER");
		
		while(NumberElem){
			
			if( !IsValidData( NumberElem, "SENTENSE", nCount ) ){
				NumberElem = NumberElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			_tcscpy_s(szValue,NumberElem->Attribute("Ver"));
			if( !IsValidData( szValue, "SENTENSE/Ver", nCount ) ){
				NumberElem = NumberElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			_tcscpy_s(szId,NumberElem->Attribute("Id"));
			if( !IsValidData( szId, "SENTENSE/Id", nCount ) ){
				NumberElem = NumberElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			ValueElem = NumberElem->FirstChildElement("VALUE");
			if( !ValueElem ){
				NumberElem = NumberElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}


			CString strBuffer;
			WCHAR szTemp[MAX_TEXTLEN]={0};			
			CString m_strLang = "kr";
			nCount2 = 0;

			while(ValueElem){

				_tcscpy_s(szValue,ValueElem->Attribute("Lang"));
				if( !IsValidData( szValue, "NUMBER/VALUE/Lang", nCount2 , szId ) ){
					ValueElem = ValueElem->NextSiblingElement("VALUE");
					nCount2++;
					continue;
				}

				if( m_strLang == szValue || strCommon == szValue )
				{

					_tcscpy_s(szValue,ValueElem->FirstChild()->Value());
					if( !IsValidData( szValue, "NUMBER/VALUE", nCount2, szId ) ){
						ValueElem = ValueElem->NextSiblingElement("VALUE");
						nCount2++;
						continue;
					}

					MultiByteToWideChar(CP_UTF8,0,szValue,(int)ValueElem->FirstChild()->ValueStr().size()+1,szTemp,sizeof(szTemp)/sizeof(szTemp[0]));			
					WideCharToMultiByte(CD3DFontPar::nCodePage[RANPARAM::dwLangSet], 0, szTemp, -1, szValue, sizeof(szValue)/sizeof(szValue[0]), NULL,NULL );	
				}
				
				ValueElem = ValueElem->NextSiblingElement("VALUE");
				nCount2++;

			}			
			
			CStringArray* pNewStrArray = new CStringArray;
			pNewStrArray->Add( strBuffer );
			pData[std::string(szId)] = pNewStrArray;


			NumberElem = NumberElem->NextSiblingElement("NUMBER");
			nCount++;
			
		}

		return TRUE;
	}

	catch(...)
	{
		return FALSE;
	}	
}
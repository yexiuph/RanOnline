#include "pch.h"

#include "InterfaceCfgXml.h"
#include "UIControlDefine.h"
#include "DxFontMan.h" //../[Lib]__Engine/Sources/DxCommon
#include "RANPARAM.h" //../[Lib]__MfcEx/Sources

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CInterfaceCfgXml::CInterfaceCfgXml( CString & strLang )
	: m_strLang(strLang)
{
}

CInterfaceCfgXml::~CInterfaceCfgXml()
{
	m_XmlDoc.Clear();
}

BOOL CInterfaceCfgXml::Parse_XML( MAP_UI_CFG & pData, const char* lpszFilename )
{
	ASSERT( lpszFilename );

	if( !m_XmlDoc.LoadFile(lpszFilename) )
	{
		return FALSE;
	}

	try
	{
		if( !_ParseConfig( pData ) )	throw;

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}	
}

BOOL CInterfaceCfgXml::IsValidData( const void * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID )
{
	if( szValue == NULL )
	{
		CDebugSet::ToLogFile( "[INTERFACE/%s]Count:%d, ID:%s Data is not valid", szMsg, nCount, szID );
		return FALSE;
	}

	return TRUE;
}

BOOL CInterfaceCfgXml::IsValidData( const TCHAR * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID )
{
	if( szValue[0] ==_T('\0') )
	{
		CDebugSet::ToLogFile( "[INTERFACE/%s]Count:%d, ID:%s Data is not valid", szMsg, nCount, szID );
		return FALSE;
	}

	return TRUE;
}

BOOL CInterfaceCfgXml::_ParseConfig( MAP_UI_CFG & pData )
{

	TCHAR szLocalValue[MAX_TEXTLEN]={0};
	TCHAR szValue[MAX_TEXTLEN]={0};
	TCHAR szId[MAX_TEXTLEN]={0};
	LONG nCount(0), nCount2(0);

	TCHAR szAlignX[MAX_TEXTLEN]={0};
	TCHAR szAlignY[MAX_TEXTLEN]={0};
	TCHAR szSizeX[MAX_TEXTLEN]={0};
	TCHAR szSizeY[MAX_TEXTLEN]={0};	
	WCHAR szTemp[MAX_TEXTLEN]={0};			

	float X_Size, Y_Size;
	float left,top,sizeX,sizeY;

	TiXmlNode* InterfaceNode;
	TiXmlElement *ControlElem, *WindowPosElem, *WindowAlignElem, 
				 *WindowChangeElem, *TextureElem, *TexturePosElem;

	CONST CString strCommon("Common");

	try
	{
		InterfaceNode = m_XmlDoc.FirstChild("INTERFACE");
		if( !InterfaceNode ) return FALSE;

		ControlElem = InterfaceNode->FirstChildElement("CONTROL");

        
		while(ControlElem)
		{
			INTERFACE_CFG* pInterfaceCfg = new INTERFACE_CFG;
			
			if( !IsValidData( ControlElem, "CONTROL", nCount ) ){
				SAFE_DELETE( pInterfaceCfg );
				ControlElem = ControlElem->NextSiblingElement("CONTROL");
				nCount++;
				continue;
			}

			_tcscpy_s(szLocalValue,ControlElem->Attribute("Local"));
			if( !IsValidData( szLocalValue, "CONTROL/Local", nCount ) ){
				SAFE_DELETE( pInterfaceCfg );
				ControlElem = ControlElem->NextSiblingElement("CONTROL");
				nCount++;
				continue;
			}


			if( m_strLang != szLocalValue && strCommon != szLocalValue )
			{
				SAFE_DELETE( pInterfaceCfg );
				ControlElem = ControlElem->NextSiblingElement("CONTROL");
				nCount++;
				continue;
			}

			_tcscpy_s(szId,ControlElem->Attribute("Id"));
			if( !IsValidData( szId, "CONTROL/Id", nCount ) ){
				SAFE_DELETE( pInterfaceCfg );
				ControlElem = ControlElem->NextSiblingElement("CONTROL");
				nCount++;
				continue;
			}


			{
				WindowPosElem = ControlElem->FirstChildElement("WINDOW_POS");
				if( !WindowPosElem ){
					SAFE_DELETE( pInterfaceCfg );
					ControlElem = ControlElem->NextSiblingElement("CONTROL");
					nCount++;
					continue;
				}

				{
					_tcscpy_s(szValue,WindowPosElem->Attribute("X"));
					if( !IsValidData( szValue, "CONTROL/WINDOW_POS/X", nCount, szId ) ){
						SAFE_DELETE( pInterfaceCfg );
						ControlElem = ControlElem->NextSiblingElement("CONTROL");
						nCount++;
						continue;
					}

					pInterfaceCfg->rcControlPos.left = (float)_tstof( szValue );
				}

				{
					_tcscpy_s(szValue,WindowPosElem->Attribute("Y"));
					if( !IsValidData( szValue, "CONTROL/WINDOW_POS/Y", nCount, szId ) ){
						SAFE_DELETE( pInterfaceCfg );
						ControlElem = ControlElem->NextSiblingElement("CONTROL");
						nCount++;
						continue;
					}

					pInterfaceCfg->rcControlPos.top = (float)_tstof( szValue );
				}

				{
					_tcscpy_s(szValue,WindowPosElem->Attribute("W"));
					if( !IsValidData( szValue, "CONTROL/WINDOW_POS/W", nCount, szId ) ){
						SAFE_DELETE( pInterfaceCfg );
						ControlElem = ControlElem->NextSiblingElement("CONTROL");
						nCount++;
						continue;
					}

					pInterfaceCfg->rcControlPos.sizeX = (float)_tstof( szValue );
				}

				{
					_tcscpy_s(szValue,WindowPosElem->Attribute("H"));
					if( !IsValidData( szValue, "CONTROL/WINDOW_POS/H", nCount, szId ) ){
						SAFE_DELETE( pInterfaceCfg );
						ControlElem = ControlElem->NextSiblingElement("CONTROL");
						nCount++;
						continue;
					}

					pInterfaceCfg->rcControlPos.sizeY = (float)_tstof( szValue );
				}

			}

			
			
			{ // Note : 윈도우 정렬 및 크기변경 플래그
				WindowAlignElem = ControlElem->FirstChildElement("WINDOW_ALIGN");
				if(WindowAlignElem)
				{
					_tcscpy_s(szAlignX,WindowAlignElem->Attribute("X"));
					_tcscpy_s(szAlignY,WindowAlignElem->Attribute("Y"));
				}

				WindowChangeElem = ControlElem->FirstChildElement("WINDOW_CHANGE");
				if(WindowChangeElem)
				{
					_tcscpy_s(szSizeX,WindowChangeElem->Attribute("X"));
					_tcscpy_s(szSizeY,WindowChangeElem->Attribute("Y"));
				}				

				pInterfaceCfg->dwAlignFlag = GET_ALIGN_FLAG( szAlignX, szAlignY, szSizeX, szSizeY );
				szAlignX[0]=0; szAlignY[0]=0; szSizeX[0]=0; szSizeY[0]=0;
			}


			{ // Note : 텍스쳐를 얻는다.
				TextureElem = ControlElem->FirstChildElement("TEXTURE");

				if(TextureElem)
				{
					{
						_tcscpy_s(szValue,TextureElem->Attribute("SizeX"));
						if( !IsValidData( szValue, "CONTROL/TEXTURE/SizeX", nCount, szId ) )
						{
							SAFE_DELETE( pInterfaceCfg );
							ControlElem = ControlElem->NextSiblingElement("CONTROL");
							nCount++;
							continue;
						}

						X_Size = (float)_tstof( szValue );
					}

					{
						_tcscpy_s(szValue,TextureElem->Attribute("SizeY"));
						if( !IsValidData( szValue, "CONTROL/TEXTURE/SizeY", nCount, szId ) )
						{
							SAFE_DELETE( pInterfaceCfg );
							ControlElem = ControlElem->NextSiblingElement("CONTROL");
							nCount++;
							continue;
						}

						Y_Size = (float)_tstof( szValue );
					}


					_tcscpy_s(szValue,TextureElem->FirstChild()->Value());
					if( !IsValidData( szValue, "CONTROL/TEXTURE", nCount, szId ) )
					{
						SAFE_DELETE( pInterfaceCfg );
						ControlElem = ControlElem->NextSiblingElement("CONTROL");
						nCount++;
						continue;
					}

					MultiByteToWideChar(CP_UTF8,0,szValue,(int)TextureElem->FirstChild()->ValueStr().size()+1,szTemp,sizeof(szTemp)/sizeof(szTemp[0]));			
					WideCharToMultiByte(CD3DFontPar::nCodePage[RANPARAM::dwLangSet], 0, szTemp, -1, szValue, sizeof(szValue)/sizeof(szValue[0]), NULL,NULL );	
					
					pInterfaceCfg->strTextureName = szValue;

				}
				
			}


			{ // Note : 텍스쳐 포지션을 얻는다.
				TexturePosElem = ControlElem->FirstChildElement("TEXTURE_POS");

				if( TexturePosElem )
				{
					{
						_tcscpy_s(szValue,TexturePosElem->Attribute("X"));
						if( !IsValidData( szValue, "CONTROL/TEXTURE_POS/X", nCount, szId ) )
						{
							SAFE_DELETE( pInterfaceCfg );
							ControlElem = ControlElem->NextSiblingElement("CONTROL");
							nCount++;
							continue;
						}

						left = (float)_tstof( szValue );
					}


					{
						_tcscpy_s(szValue,TexturePosElem->Attribute("Y"));
						if( !IsValidData( szValue, "CONTROL/TEXTURE_POS/Y", nCount, szId ) )
						{
							SAFE_DELETE( pInterfaceCfg );
							ControlElem = ControlElem->NextSiblingElement("CONTROL");
							nCount++;
							continue;
						}

						top = (float)_tstof( szValue );
					}


					{
						_tcscpy_s(szValue,TexturePosElem->Attribute("W"));
						if( !IsValidData( szValue, "CONTROL/TEXTURE_POS/W", nCount, szId ) )
						{
							SAFE_DELETE( pInterfaceCfg );
							ControlElem = ControlElem->NextSiblingElement("CONTROL");
							nCount++;
							continue;
						}

						sizeX = (float)_tstof( szValue );
					}

					{
						_tcscpy_s(szValue,TexturePosElem->Attribute("H"));
						if( !IsValidData( szValue, "CONTROL/TEXTURE_POS/H", nCount, szId ) )
						{
							SAFE_DELETE( pInterfaceCfg );
							ControlElem = ControlElem->NextSiblingElement("CONTROL");
							nCount++;
							continue;
						}

						sizeY = (float)_tstof( szValue );
					}
					

					if( X_Size == 0 && Y_Size == 0 )
					{
						SAFE_DELETE( pInterfaceCfg );
						ControlElem = ControlElem->NextSiblingElement("CONTROL");
						nCount++;
						GASSERT( 0 && "Texture's Size is Zero" );
						continue;
					}

					pInterfaceCfg->rcTexturePos = UIRECT(	(left+0.25f)/X_Size, 
															(top+0.25f)/Y_Size, 
															sizeX/X_Size, 
															sizeY/Y_Size );
				}
			}

			// Note : 특정 로컬이 있다면 Common으로 저장되어 있는 값을 지운다.			
			if( m_strLang == szLocalValue )
			{
				ITER_MAP_UI_CFG di = pData.find( std::string( szId ) );
				if( di != pData.end() )
				{
					SAFE_DELETE( di->second );
					if( pData.erase( di->first ) >= 2 )
					{
						GASSERT( 0 && _T("INTERFACE_CFG.XML : 삭제된 ID가 2개 이상입니다.") );
					}
				}
			}

			pData.insert( std::make_pair( std::string( szId ), pInterfaceCfg ) );
			

			ControlElem = ControlElem->NextSiblingElement("CONTROL");
			nCount++;

		}
		
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
}
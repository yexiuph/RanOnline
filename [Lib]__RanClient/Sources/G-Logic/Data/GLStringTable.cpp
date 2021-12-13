#include "pch.h"
#include "./GLStringTable.h"
#include "./GLogicData.h"
#include "./GLItemDef.h"
#include "./GLItemMan.h"
#include "./GLSkill.h"
#include "./GLCrowData.h"

#include "../[Lib]__Engine/Sources/Common/BaseString.h"
#include "../[Lib]__Engine/Sources/Common/CompByte.h"
#include "../[Lib]__Engine/Sources/DxTools/DebugSet.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLOGIC.h"
#include "../[Lib]__Engine/Sources/Common/rijndael.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLStringTable::GLStringTable(void)
	: m_pLoadMap(NULL)
{
}

GLStringTable::~GLStringTable(void)
{
}

GLStringTable & GLStringTable::GetInstance()
{
	static GLStringTable Instance;
	return Instance;
}

void GLStringTable::CLEAR()
{
	if( !m_mapItemStringTable.empty() ) m_mapItemStringTable.clear();
	if( !m_mapSkillStringTable.empty() ) m_mapSkillStringTable.clear();
	if( !m_mapCrowStringTable.empty() ) m_mapCrowStringTable.clear();
}

void GLStringTable::SetCurrentMap( EMST_TYPE emType )
{
	switch ( emType )
	{
	case ITEM:	m_pLoadMap = &m_mapItemStringTable; break;
	case SKILL:	m_pLoadMap = &m_mapSkillStringTable; break;
	case CROW:	m_pLoadMap = &m_mapCrowStringTable; break;
	default:	GASSERT( FALSE ); break;
	}
}

GLStringTable::DESMAP * GLStringTable::GetCurrentMap()
{
	GASSERT( m_pLoadMap && "GLStringTable::GetCurrentMap() m_pLoadMap is NULL" );
	return m_pLoadMap;
}

bool GLStringTable::LOADFILE( const TCHAR * szFileName, EMST_TYPE emType )
{
	if( !szFileName )				return false;
	if( strlen(szFileName) == 0 )	return false;

	std::string strPath, strKey, strSub;
	strPath = GLOGIC::GetPath();
	strPath += szFileName;

	std::auto_ptr<CBaseString> pBString(GLOGIC::openfile_basestring(GLOGIC::bGLOGIC_ZIPFILE, 
																	GLOGIC::strGLOGIC_ZIPFILE.c_str(), 
																	strPath.c_str(), 
																	szFileName, 
																	true,
																	GLOGIC::bGLOGIC_PACKFILE ) );

	if( !pBString.get() )
	{
		CDebugSet::ToLogFile ( "ERROR : GLStringTable::LOADFILE(), File Open %s", szFileName );
		return false;
	}

	CBaseString &cBString = *pBString;

	CString resToken, strBUFFER;
	CString strToken("\t");

	SetCurrentMap( emType );

	while ( cBString.GetNextLine(strBUFFER) > 0 )
	{
		int curPos(0);

		strBUFFER = strBUFFER.SpanExcluding("/"); // 주석처리

		resToken = strBUFFER.Tokenize(strToken.GetString(), curPos);
		if (resToken == "") continue;
		strKey = resToken.GetString();

		resToken = strBUFFER.Tokenize(strToken.GetString(), curPos);
		strSub = resToken.GetString();

		if( !GetCurrentMap()->insert( std::make_pair( strKey, strSub ) ).second )
			CDebugSet::ToLogFile( _T("String Table's ID %s was duplicated."), strKey.c_str() );
	}

	return true;
}

const TCHAR * GLStringTable::GetString( const TCHAR * szKey, EMST_TYPE emType )
{
	GASSERT( szKey && "GLStringTable::GetString() szKey is NULL" );

	SetCurrentMap( emType );

	DESMAP * pMap = GetCurrentMap();
	if( !pMap ) return NULL;

	DESMAP_ITER Iter = pMap->find( szKey );
	
	if( Iter != pMap->end() )
	{
		return Iter->second.c_str();
	}

	return NULL;
}

void GLStringTable::InsertString( std::string & strKey, std::string & strSub, EMST_TYPE emType )
{
	SetCurrentMap( emType );

	//if( !m_mapStringTable.insert( std::make_pair( strKey, strSub ) ).second )
	//{
	//		return false;
	//}

	DESMAP * pMap = GetCurrentMap();
	(*pMap)[strKey] = strSub;
}

void GLStringTable::DeleteString( std::string & strKeyName, std::string & strKeyDesc, EMST_TYPE emType )
{
	SetCurrentMap( emType );

	DESMAP * pMap = GetCurrentMap();
	pMap->erase( strKeyName );
	pMap->erase( strKeyDesc );
}

void GLStringTable::DeleteString( WORD wMID, WORD wSID, EMST_TYPE emType )
{
	TCHAR szName[ITEM_SZNAME] = {0};
	TCHAR szDesc[ITEM_SZCOMMENT] = {0};
	
	switch ( emType )
	{
		case ITEM:
			StringCchPrintf ( szName, ITEM_SZNAME, "IN_%03d_%03d", wMID, wSID );
			StringCchPrintf ( szDesc, ITEM_SZCOMMENT, "ID_%03d_%03d", wMID, wSID );
			break;

		case SKILL:
			StringCchPrintf ( szName, ITEM_SZNAME, "SN_%03d_%03d", wMID, wSID );
			StringCchPrintf ( szDesc, ITEM_SZCOMMENT, "SD_%03d_%03d", wMID, wSID );
			break;

		case CROW: 
			StringCchPrintf ( szName, ITEM_SZNAME, "CN_%03d_%03d", wMID, wSID );
			//StringCchPrintf ( szDesc, ITEM_SZCOMMENT, "CD_%03d_%03d", wMID, wSID );
			break;

		default:
			GASSERT( FALSE );
			break;
	}

	return DeleteString( std::string(szName), std::string(szDesc), emType );
}

bool GLStringTable::SAVEFILE( EMST_TYPE emType )
{
	TCHAR szFullPathFileName[MAX_PATH] = {0};
	StringCchCopy( szFullPathFileName, MAX_PATH, GLOGIC::GetPath() );

	switch ( emType )
	{
	case ITEM:	StringCchCat ( szFullPathFileName, MAX_PATH, GLItemMan::GetInstance()._STRINGTABLE ); break;
	case SKILL:	StringCchCat ( szFullPathFileName, MAX_PATH, GLSkillMan::GetInstance()._STRINGTABLE ); break;
	case CROW:	StringCchCat ( szFullPathFileName, MAX_PATH, GLCrowDataMan::_STRINGTABLE ); break;
	default:	GASSERT( FALSE ); break;
	}

	SetCurrentMap( emType );

	DESMAP	m_mapNameForSave;
	DESMAP	m_mapDescForSave;
	DESMAP_ITER iter, iter2;
	DESMAP_ITER iter_end, iter_end2;

	iter = GetCurrentMap()->begin();
	iter_end = GetCurrentMap()->end();
	for( ; iter != iter_end; ++iter )
	{
		if( iter->first[1] == 'N')
			m_mapNameForSave.insert( std::make_pair( iter->first, iter->second ) );
		else
			m_mapDescForSave.insert( std::make_pair( iter->first, iter->second ) );
	}

	CString strBuffer, strBuffer1;
	strBuffer = "// Note : 키(ID)와 내용은 반드시 탭으로 구분되어야 합니다.\r\n//\r\n";

	iter = m_mapNameForSave.begin();
	iter_end = m_mapNameForSave.end();
	iter2 = m_mapDescForSave.begin();
	iter_end2 = m_mapDescForSave.end();

	for( ; iter != iter_end; ++iter )
	{
		strBuffer1.Format( _T("%s\t%s\r\n"), iter->first.c_str(), iter->second.c_str() );
		strBuffer += strBuffer1;

		if( iter2 != iter_end2 )
		{
			strBuffer1.Format( _T("%s\t%s\r\n"), iter2->first.c_str(), iter2->second.c_str() );
			strBuffer += strBuffer1;
			++iter2;
		}
	}

	CRijndael oRijndael;
	oRijndael.Initialize( CRijndael::sm_Version[CRijndael::VERSION-1].c_str(), CRijndael::DEFAULT_KEY_LENGTH, CRijndael::VERSION_DATE, CRijndael::VERSION );

	int nLen = oRijndael.GetEncryptStringLength( strBuffer );
	char * szBuffer = new char[nLen];

	oRijndael.EncryptEx( strBuffer.GetString(), szBuffer, strBuffer.GetLength() );
	int nVersion = CRijndael::VERSION;

	CFile file;
	file.Open( _T(szFullPathFileName), CFile::modeCreate|CFile::modeWrite );	// 파일 열기
	file.Write( &nVersion, sizeof(int) );							// 버젼 쓰기
	file.Write( szBuffer, nLen );									// 파일 쓰기
	file.Close();

	delete [] szBuffer;

	return true;
}
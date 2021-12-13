#include "pch.h"
#include "GLQuestMan.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"
#include "../[Lib]__Engine/Sources/Common/GLTexFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLQuestMan& GLQuestMan::GetInstance()
{
	static GLQuestMan Instance;
	return Instance;
}

GLQuestMan::GLQuestMan () :
	m_bModify(false)
{

}

GLQuestMan::~GLQuestMan ()
{
	Clean ();
}

GLQUEST* GLQuestMan::Find ( DWORD dwQuestID )
{
	MAPQUEST_ITER iter = m_mapQuestMap.find ( dwQuestID );
	if ( iter!=m_mapQuestMap.end() )
	{
		SNODE &sNODE = (*iter).second;
		
		if ( !sNODE.pQUEST )
		{
			sNODE.pQUEST = LoadQuest ( sNODE.strFILE.c_str(), dwQuestID );
			if ( !sNODE.pQUEST )
			{
				CDebugSet::ToLogFile ( "QUEST load fail : %d (%s)", dwQuestID, sNODE.strFILE.c_str() );
			}
		}

		return sNODE.pQUEST;
	}

	CDebugSet::ToLogFile ( "QUEST load fail : %d", dwQuestID );
	return NULL;
}

DWORD GLQuestMan::MakeNewQNID ()
{
	DWORD dwQuestID(0);
	while(1)
	{
		GLQUEST* pQuest = Find(dwQuestID);
		if ( !pQuest )	return dwQuestID;
		dwQuestID++;
	};

	return UINT_MAX;
}

GLQuestMan::QUESTGEN_RANGE* GLQuestMan::FindStartFromGetITEM ( SNATIVEID nidITEM )
{
	static GLQuestMan::QUESTGEN_RANGE sRANGE;

	sRANGE = m_mapStartItem.equal_range ( nidITEM.dwID );
	if ( sRANGE.first==sRANGE.second )	return NULL;

	return &sRANGE;
}

GLQuestMan::QUESTGEN_RANGE* GLQuestMan::FindStartFromGetSKILL ( SNATIVEID nidSKILL )
{
	static GLQuestMan::QUESTGEN_RANGE sRANGE;

	sRANGE = m_mapStartSkill.equal_range ( nidSKILL.dwID );
	if ( sRANGE.first==sRANGE.second )	return NULL;

	return &sRANGE;
}

GLQuestMan::QUESTGEN_RANGE* GLQuestMan::FindStartFromGetLEVEL ( WORD wLEVEL )
{
	static GLQuestMan::QUESTGEN_RANGE sRANGE;

	sRANGE = m_mapStartLevel.equal_range ( wLEVEL );
	if ( sRANGE.first==sRANGE.second )	return NULL;

	return &sRANGE;
}

void GLQuestMan::Insert ( DWORD dwID, SNODE sNODE )
{
	if ( sNODE.pQUEST )
	{
		sNODE.pQUEST->m_sNID.dwID = dwID;

		GLQUEST_START &sSTART = sNODE.pQUEST->m_sSTARTOPT;
		if ( sSTART.nidITEM!=SNATIVEID(false) )
			m_mapStartItem.insert ( std::make_pair(sSTART.nidITEM.dwID,dwID) );
		
		if ( sSTART.nidSKILL!=SNATIVEID(false) )
			m_mapStartSkill.insert ( std::make_pair(sSTART.nidSKILL.dwID,dwID) );

		if ( sSTART.wLEVEL!=USHRT_MAX )
			m_mapStartLevel.insert ( std::make_pair(sSTART.wLEVEL,dwID) );
	}

	m_mapQuestMap.insert ( std::make_pair(dwID,sNODE) );
}

bool GLQuestMan::Delete ( DWORD dwQuestID )
{
	MAPQUEST_ITER iter = m_mapQuestMap.find ( dwQuestID );
	if ( iter!=m_mapQuestMap.end() )
	{
		SNODE &sNODE = (*iter).second;
		SAFE_DELETE ( sNODE.pQUEST );
		m_mapQuestMap.erase ( iter );

		return true;
	}

	return false;
}

void GLQuestMan::Clean ()
{
	MAPQUEST_ITER iter = m_mapQuestMap.begin();
	MAPQUEST_ITER iter_end = m_mapQuestMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SNODE &sNODE = (*iter).second;
		SAFE_DELETE(sNODE.pQUEST);
	}

	m_mapQuestMap.clear();
}

GLQUEST* GLQuestMan::LoadQuest ( const char* szFile, DWORD dwQuestID )
{
	GLQUEST *pQUEST = new GLQUEST;
	if ( !pQUEST)		return NULL;
	
	bool bLOAD = pQUEST->LOAD ( szFile );
	pQUEST->m_sNID.dwID = dwQuestID;

	if ( !bLOAD )
	{
		SAFE_DELETE(pQUEST);
		return NULL;
	}

	return pQUEST;
}

bool GLQuestMan::LoadFile ( const char* szFile, bool bQUEST_LOAD )
{
	ResetModified();

	std::string strLIST_FILE = std::string(GetPath()) + szFile;

	gltexfile cFILE;
	cFILE.reg_sep ( '\t' );
	cFILE.reg_sep ( ' ' );
	cFILE.reg_sep ( ',' );

	if( GLOGIC::bGLOGIC_ZIPFILE ) // by 경대
		cFILE.SetZipFile( GLOGIC::strQUEST_ZIPFILE ); // by 경대, zip파일 사용

	if( !cFILE.open(strLIST_FILE.c_str(), false, GLOGIC::bGLOGIC_PACKFILE) )
	{
		CDebugSet::ToLogFile ( "ERROR : GLQuestMan::LoadFile(), File Open %s", szFile );
		return false;
	}

	DWORD dwNUM = cFILE.getflagnum ( "QUEST" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		DWORD dwID;
		SNODE sNODE;

		cFILE.getflag ( i, "QUEST", 1, 2, dwID );
		cFILE.getflag ( i, "QUEST", 2, 2, sNODE.strFILE );

		if ( bQUEST_LOAD )
		{
			sNODE.pQUEST = LoadQuest ( sNODE.strFILE.c_str(), dwID );
			if ( !sNODE.pQUEST )
				MessageBox ( NULL, sNODE.strFILE.c_str(), "quest file not found.", MB_OK );
		}

		Insert ( dwID, sNODE );
	}


	return true;
}

bool GLQuestMan::SaveFile ( const char* szFile )
{
	MAPQUEST_ITER iter_pos;
	SNODE sNode;

	std::string strLIST_FILE = std::string(GetPath()) + szFile;

	FILE* file=NULL;
	fopen_s(&file, strLIST_FILE.c_str(), "wt");
	if ( !file )	return FALSE;

	fprintf( file , "//Quest List\n" );
	fprintf( file , "//\tID\tfile\t\tquest name\n" );


	for ( iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); iter_pos++ )
	{
		sNode = (SNODE) iter_pos->second;

		fprintf ( file, "QUEST\t%d\t%s", sNode.pQUEST->m_sNID.dwID, sNode.strFILE.c_str() );
		fprintf ( file, "\t//%s\n", sNode.pQUEST->m_strTITLE.c_str() );
	}
	
	fclose( file );

	return true;
}



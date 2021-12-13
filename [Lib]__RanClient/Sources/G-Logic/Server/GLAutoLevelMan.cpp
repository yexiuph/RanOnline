#include "pch.h"
#include "./GLAutoLevelMan.h"

#include "../[Lib]__Engine/Sources/Common/IniLoader.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLOGIC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLAutoLevelMan& GLAutoLevelMan::GetInstance()
{
	static GLAutoLevelMan cInstance;
	return cInstance;
}

GLAutoLevelMan::~GLAutoLevelMan()
{
	// Note : GLAutoLevel 메모리 해제
	AUTOLEVEL_LIST_ITER iter = m_listAutoLevel.begin();
	AUTOLEVEL_LIST_ITER iter_end = m_listAutoLevel.end();

	for( ; iter != iter_end; ++iter )
		SAFE_DELETE( *iter );

	m_listAutoLevel.clear();
}

bool GLAutoLevelMan::LOAD( std::string & strFileName )
{
	if( strFileName.empty() )	return false;

	std::string strPath;
	strPath = GLOGIC::GetServerPath ();
	strPath += strFileName;

	CIniLoader cFILE;

	if( GLOGIC::bGLOGIC_ZIPFILE )
		cFILE.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

	if( !cFILE.open ( strPath, true ) )
	{
		CDebugSet::ToLogFile ( "ERROR : GLAutoLevelMan::LOAD(), File Open %s", strFileName.c_str() );
		return false;
	}

	DWORD dwNUM = cFILE.GetKeyDataSize( "AUTO_LEVEL_MAN_SET", "COMMAND_LIST" );
	for( DWORD i=0; i<dwNUM; ++i )
	{
		std::string strCommand;
		cFILE.getflag( "AUTO_LEVEL_MAN_SET", "COMMAND_LIST", i, dwNUM, strCommand );

		m_listCommand.push_back( strCommand );
	}

	dwNUM = cFILE.GetKeyDataSize( "AUTO_LEVEL_MAN_SET", "FILE_LIST" );
	for( DWORD i=0; i<dwNUM; ++i )
	{
		std::string strFileName;
		cFILE.getflag( "AUTO_LEVEL_MAN_SET", "FILE_LIST", i, dwNUM, strFileName );

		m_listFileName.push_back( strFileName );
	}

	// Note : 자동 레벨 파일을 읽는다.
	STRING_LIST_ITER iter = m_listFileName.begin();
	STRING_LIST_ITER iter_end = m_listFileName.end();
	for( ; iter != iter_end; ++iter )
	{
		GLAutoLevel * pGlAutoLevel = new GLAutoLevel;
		if( !pGlAutoLevel->LOAD( *iter, m_listCommand ) )
		{
			CDebugSet::ToLogFile ( "ERROR : GLAutoLevelMan::LOAD(), File Open %s", iter->c_str() );
			return false;
		}

		m_listAutoLevel.push_back( pGlAutoLevel );
	}

	return true;
}

bool GLAutoLevelMan::FrameMove()
{
	CTime cCurTime = CTime::GetCurrentTime();

	int nDayOfWeek = cCurTime.GetDayOfWeek() - 1;
	int nDay = cCurTime.GetDay ();
	int nHour = cCurTime.GetHour();
	int nMinute = cCurTime.GetMinute ();

	AUTOLEVEL_LIST_ITER iter = m_listAutoLevel.begin();
	AUTOLEVEL_LIST_ITER iter_end = m_listAutoLevel.end();

	for( ; iter != iter_end; ++iter )
	{
		GLAutoLevel * pGlAutoLevel = *iter;
		if( !pGlAutoLevel ) continue;

		if( pGlAutoLevel->IsEventStartTime( nDayOfWeek, nDay, nHour, nMinute ) )
		{
			pGlAutoLevel->DoEventStart( nDay );
		}

		if( pGlAutoLevel->IsEvent() )
		{
			if( pGlAutoLevel->IsEventEndTime( nHour, nMinute) )
			{
				pGlAutoLevel->DoEventEnd();
			}
			else
			{
				pGlAutoLevel->DoEventScript( nHour, nMinute );
			}
		}
	}

	return true;
}
#include "pch.h"
#include "./GLAutoLevel.h"
#include "./GLAgentServer.h"
#include "./GLLandMan.h"
#include "./GLGaeaServer.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"
#include "../[Lib]__Engine/Sources/Common/IniLoader.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLAutoLevel::GLAutoLevel ()
	: m_bPVPFlag(false)
	, m_dwMapID(0)
	, m_dwGateID(0)
	, m_dwHallMapID(0)
	, m_bEvent(false)
	, m_LevScriptPos(0)
	, m_dwLastEventDay(0)
{
	SecureZeroMemory( m_dwDay, sizeof(m_dwDay) );
}

GLAutoLevel::~GLAutoLevel()
{
}

bool GLAutoLevel::LOAD ( const std::string & strFileName, STRING_LIST & listCommand )
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
		CDebugSet::ToLogFile ( "ERROR : GLAutoLevel::LOAD(), File Open %s", strFileName.c_str() );
		return false;
	}
	
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_PVP", 0, 1, m_bPVPFlag );
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_NAME", 0, 1, m_strName );

	SNATIVEID nidMAP;
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_MAP", 0, 2, nidMAP.wMainID );
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_MAP", 1, 2, nidMAP.wSubID );
	m_dwMapID = nidMAP.dwID;

	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_GATE", 0, 1, m_dwGateID );

	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_HALL_MAP", 0, 2, nidMAP.wMainID );
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_HALL_MAP", 1, 2, nidMAP.wSubID );
	m_dwHallMapID = nidMAP.dwID;

	DWORD dwNUM = cFILE.GetKeySize( "AUTO_LEVEL_SET", "EVENT_POS" );
	dwNUM = ( dwNUM > MAX_EVENT_POS ) ? MAX_EVENT_POS : dwNUM;
	for( DWORD i=0; i<dwNUM; ++i )
	{
		GLLEV_POS levPos;

		cFILE.getflag( i, "AUTO_LEVEL_SET", "EVENT_POS", 0, 2, levPos.m_dwPOS_X );
		cFILE.getflag( i, "AUTO_LEVEL_SET", "EVENT_POS", 1, 2, levPos.m_dwPOS_Y );

		m_dwOutPos[i] = levPos;
	}

	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_START_TIME", 0, 2, m_StartTime.dwHour );
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_START_TIME", 1, 2, m_StartTime.dwMinute );

	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_END_TIME", 0, 2, m_EndTime.dwHour );
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_END_TIME", 1, 2, m_EndTime.dwMinute );

	for( DWORD i=0; i<MAX_DAY; ++i )
	{
		cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_DAY", i, MAX_DAY, m_dwDay[i] );
	}

	dwNUM = cFILE.GetKeySize( "AUTO_LEVEL_SCRIPT", "ALS" );
	for( DWORD i=0; i<dwNUM; ++i )
	{
		GLLEV_SCRIPT levScript;

		cFILE.getflag( i, "AUTO_LEVEL_SCRIPT", "ALS", 0, 4, levScript.levTime.dwHour );
		cFILE.getflag( i, "AUTO_LEVEL_SCRIPT", "ALS", 1, 4, levScript.levTime.dwMinute );
		cFILE.getflag( i, "AUTO_LEVEL_SCRIPT", "ALS", 2, 4, levScript.strCommand );
		{
			bool bCommand(false);

			STRING_LIST_ITER iter = listCommand.begin();
			STRING_LIST_ITER iter_end = listCommand.end();
			for( ; iter != iter_end; ++iter )
			{
				if( *iter == levScript.strCommand )
				{
					bCommand = true;
					break;
				}
			}

			if( !bCommand )
			{
				// Note : 등록되어 있는 명령어가 아닙니다.
				TCHAR szErrorMsg[255] = {0};
				StringCchPrintf( szErrorMsg, sizeof(szErrorMsg), "[GLAutoLevel::LOAD] File : %s, Command : %s",
								strFileName.c_str(), levScript.strCommand.c_str() );
				MessageBox( NULL, szErrorMsg, "COMMAND ERROR", MB_OK );
				return false;
			}
		}
		cFILE.getflag( i, "AUTO_LEVEL_SCRIPT", "ALS", 3, 4, levScript.strArgument );

		m_vecLevScript.push_back( levScript );
	}

	return true;
}

bool GLAutoLevel::IsEventStartTime( int nDayOfWeek, int nDay, int nHour, int nMinute )
{
	if( IsEvent() || ( m_dwLastEventDay == nDay ) ) return false;

	if( ( m_dwDay[nDayOfWeek] ) && 
		( m_StartTime.dwHour == nHour ) && 
		( m_StartTime.dwMinute == nMinute ) )
	{
		return true;
	}
	
	return false;
}

bool GLAutoLevel::IsEventEndTime( int nHour, int nMinute )
{
	return ( m_EndTime.dwHour == (DWORD) nHour && m_EndTime.dwMinute <= (DWORD) nMinute ) ? true : false;
}

void GLAutoLevel::DoEventScript( int nHour, int nMinute )
{
	if( m_LevScriptPos >= m_vecLevScript.size() ) return;

	while( ( m_vecLevScript[m_LevScriptPos].levTime.dwHour == nHour ) && 
			( m_vecLevScript[m_LevScriptPos].levTime.dwMinute <= (DWORD)nMinute ) )
	{
		m_queueLevScript.push( m_vecLevScript[m_LevScriptPos] );
		++m_LevScriptPos;
	}

	while( !m_queueLevScript.empty() )
	{
		RunScriptCommand( m_queueLevScript.front() );
		m_queueLevScript.pop();
	}
}

void GLAutoLevel::RunScriptCommand( GLLEV_SCRIPT & glLevScript )
{
	// Note : 스크립트 실행
	const std::string & strCommand = glLevScript.strCommand;
	const std::string & strArgument = glLevScript.strArgument;

	if( strCommand == _T( "msg" ) )
	{
		NET_CHAT_FB NetChatFB;
		NetChatFB.emType = CHAT_TYPE_GLOBAL;
		StringCchCopy ( NetChatFB.szChatMsg, CHAT_MSG_SIZE+1, strArgument.c_str() );

		GLAgentServer::GetInstance().SENDTOALLCLIENT( &NetChatFB );
	}
	else if( strCommand == _T( "warning" ) )
	{
		GLMSG::SNET_LEVEL_EVENT_WARNING_FLD NetMsgBrd;
		NetMsgBrd.dwMapID = m_dwMapID;
		NetMsgBrd.bOn = ( strArgument == _T( "on" ) ) ? true : false;

		GLAgentServer::GetInstance().SENDTOALLCHANNEL( &NetMsgBrd );
	}
	else if( strCommand == _T( "countdown" ) )
	{
		GLMSG::SNET_LEVEL_EVENT_COUNTDOWN_FLD NetMsgBrd;
		NetMsgBrd.dwMapID = m_dwMapID;
		NetMsgBrd.nCount = _ttoi( strArgument.c_str() );

		GLAgentServer::GetInstance().SENDTOALLCHANNEL( &NetMsgBrd );
	}
	else if( strCommand == _T( "call_map" ) )
	{
		GLMSG::SNET_GM_MOB_LEV NetMsg;
		SNATIVEID nidMAP( m_dwMapID );
		NetMsg.wMAP_MID = nidMAP.wMainID;
		NetMsg.wMAP_SID = nidMAP.wSubID;
		StringCchCopy( NetMsg.szLevelFile, GLMSG::SNET_GM_MOB_LEV::FILE_NAME, strArgument.c_str() );

		GLAgentServer::GetInstance().SENDTOALLCHANNEL( &NetMsg );
	}
	else if( strCommand == _T( "del_map" ) )
	{
		GLMSG::SNET_GM_MOB_LEV_CLEAR NetMsg;
		SNATIVEID nidMAP( m_dwMapID );
		NetMsg.wMAP_MID = nidMAP.wMainID;
		NetMsg.wMAP_SID = nidMAP.wSubID;

		GLAgentServer::GetInstance().SENDTOALLCHANNEL( &NetMsg );
	}
	else if( strCommand == _T( "call_hall_map" ) )
	{
		GLMSG::SNET_GM_MOB_LEV NetMsg;
		SNATIVEID nidMAP( m_dwHallMapID );
		NetMsg.wMAP_MID = nidMAP.wMainID;
		NetMsg.wMAP_SID = nidMAP.wSubID;
		StringCchCopy( NetMsg.szLevelFile, GLMSG::SNET_GM_MOB_LEV::FILE_NAME, strArgument.c_str() );

		GLAgentServer::GetInstance().SENDTOALLCHANNEL( &NetMsg );
	}
	else if( strCommand == _T( "del_hall_map" ) )
	{
		GLMSG::SNET_GM_MOB_LEV_CLEAR NetMsg;
		SNATIVEID nidMAP( m_dwHallMapID );
		NetMsg.wMAP_MID = nidMAP.wMainID;
		NetMsg.wMAP_SID = nidMAP.wSubID;

		GLAgentServer::GetInstance().SENDTOALLCHANNEL( &NetMsg );
	}
	else if( strCommand == _T( "move_cha_hall_map" ) )
	{
		//	Note : 이벤트 종료를 알림.
		GLMSG::SNET_LEVEL_EVENT_END_FLD NetMsgFld;
		NetMsgFld.dwMapID = m_dwMapID;
		NetMsgFld.dwGateID = m_dwGateID;
		NetMsgFld.dwHallMapID = m_dwHallMapID;
		for( int i=0; i<MAX_EVENT_POS; ++i )
			NetMsgFld.levPos[i] = m_dwOutPos[i];

		GLAgentServer::GetInstance().SENDTOALLCHANNEL( &NetMsgFld );
	}
}
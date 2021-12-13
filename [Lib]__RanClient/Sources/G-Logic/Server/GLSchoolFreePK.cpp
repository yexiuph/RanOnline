#include "pch.h"

#include <algorithm>
//#include <strstream>

#include "GLSchoolFreePK.h"

#include "GLOGIC.h"
#include "GLPeriod.h"
#include "GLogicData.h"
#include "GLAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLSchoolFreePK& GLSchoolFreePK::GetInstance ()
{
	static GLSchoolFreePK cInstance;
	return cInstance;
}

GLSchoolFreePK::GLSchoolFreePK () :
	m_bON(false),
	m_dwLAST_DAY(0),
	m_fTIMER(0)
{
}

GLSchoolFreePK::~GLSchoolFreePK()
{
}

bool GLSchoolFreePK::IsON ()
{
	if ( GLCONST_CHAR::bSCHOOL_FREE_PK_ALWAYS )		return true;

	return m_bON;
}

bool GLSchoolFreePK::DoProgress ( float fElaps )
{
	m_fTIMER += fElaps;

	if ( !GLCONST_CHAR::bSCHOOL_FREE_PK_TIME_REAL )
	{
		if ( m_fTIMER > GLCONST_CHAR::dwSCHOOL_FREE_PK_TIME_TERM*3600.0f/GLPeriod::REALTIME_TO_VBR )
		{
			DoEnd ();
			return true;
		}
	}
	else
	{
		if ( m_fTIMER > GLCONST_CHAR::dwSCHOOL_FREE_PK_TIME_TERM*3600.0f )
		{
			DoEnd ();
			return true;
		}
	}

	return false;
}

bool GLSchoolFreePK::DoBegin ()
{
	m_bON = true;
	m_fTIMER = 0;

/* 
	//	Note : 클라이언트에 알림.
	GLMSG::SNETPC_SERVER_INFO NetMsgClt;
	NetMsgClt.bSCHOOL_FREEPK = m_bON;
	GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsgClt );
*/

	//	Note : 필드에 상태 변환.
	GLMSG::SNETPC_SCHOOLFREEPK_FLD NetMsgFld;
	NetMsgFld.bSCHOOL_FREEPK = m_bON;
	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgFld );

	return true;
}

bool GLSchoolFreePK::DoEnd ()
{
	m_bON = false;
	m_fTIMER = 0;

/*
	//	Note : 클라이언트에 알림.
	GLMSG::SNETPC_SERVER_INFO NetMsgClt;
	NetMsgClt.bSCHOOL_FREEPK = m_bON;
	GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsgClt );
*/

	//	Note : 필드에 상태 변환.
	GLMSG::SNETPC_SCHOOLFREEPK_FLD NetMsgFld;
	NetMsgFld.bSCHOOL_FREEPK = m_bON;
	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgFld );

	return true;
}

bool GLSchoolFreePK::CheckBegin ()
{
	if ( !GLCONST_CHAR::bSCHOOL_FREE_PK )	return true;

	if ( !GLCONST_CHAR::bSCHOOL_FREE_PK_TIME_REAL )
	{
		DWORD dwYearToday = GLPeriod::GetInstance().GetYearToday();
		DWORD dwHour = GLPeriod::GetInstance().GetHour();

		//	Note : 오늘 시작되었는지 검사.
		if ( m_dwLAST_DAY!=dwYearToday )
		{
			//	Note : 시작시간인지 검사.
			if ( GLCONST_CHAR::dwSCHOOL_FREE_PK_TIME_START == dwHour )
			{
				m_dwLAST_DAY = dwYearToday;

				//	Note : 시작.
				DoBegin ();
			}
		}
	}
	else
	{
		CTime cTIME = CTime::GetCurrentTime();

		DWORD dwYearToday = (DWORD) cTIME.GetDay();
		DWORD dwHour = (DWORD) cTIME.GetHour();

		//	Note : 오늘 시작되었는지 검사.
		if ( m_dwLAST_DAY!=dwYearToday )
		{
			//	Note : 시작시간인지 검사.
			if ( GLCONST_CHAR::dwSCHOOL_FREE_PK_TIME_START == dwHour )
			{
				m_dwLAST_DAY = dwYearToday;

				//	Note : 시작.
				DoBegin ();
			}
		}
	}

	return true;
}

void GLSchoolFreePK::FrameMove ( float fElaps )
{
	if ( m_bON )
	{
		//	Note : 진행.
		DoProgress ( fElaps );
	}
	else
	{
		//	Note : 시작 점검.
		CheckBegin ();
	}
}


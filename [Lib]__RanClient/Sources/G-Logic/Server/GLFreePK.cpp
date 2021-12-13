#include "pch.h"
#include "./GLFreePK.h"
#include "./GLGaeaServer.h"
#include "./GLAgentServer.h"
#include "./GLContrlServerMsg.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLFreePK& GLFreePK::GetInstance ()
{
	static GLFreePK cInstance;
	return cInstance;
}

GLFreePK::~GLFreePK()
{
}

void GLFreePK::StartFREEPKMODE( DWORD dwTIME )
{
	m_fPKTIME      = static_cast<float>( dwTIME );
	m_fUPDATETIMER = 300.0f;
} // end of decl

void GLFreePK::EndFREEPKMODE()
{
	m_fTIMER	   = 0.0f;
	m_fPKTIME	   = 0.0f;
	m_fUPDATETIMER = 300.0f;
} // end of decl

bool GLFreePK::FrameMove( float fElapsedAppTime )
{
	if ( IsFREEPKMODE() )
	{
		m_fUPDATETIMER -= fElapsedAppTime;
		if ( m_fUPDATETIMER < 0.0f )
		{
			// 성향이벤트 관련 정보 모두에게 알림
			NET_CHAT NetMsg;	
			NetMsg.nmg.nType	= NET_MSG_CHAT_FB;
			NetMsg.emType		= CHAT_TYPE_GLOBAL;
			::StringCchCopyN ( NetMsg.szChatMsg, CHAT_MSG_SIZE, ID2SERVERTEXT("BRIGHT_EVENT_MSG", 0 ), CHAT_MSG_SIZE );
			GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsg );
			::StringCchCopyN ( NetMsg.szChatMsg, CHAT_MSG_SIZE, ID2SERVERTEXT("BRIGHT_EVENT_MSG", 1 ), CHAT_MSG_SIZE );
			GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsg );
			::StringCchCopyN ( NetMsg.szChatMsg, CHAT_MSG_SIZE, ID2SERVERTEXT("BRIGHT_EVENT_MSG", 2 ), CHAT_MSG_SIZE );
			GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsg );

			m_fUPDATETIMER = 300.0f;
		}

		m_fPKTIME -= fElapsedAppTime;
		if ( m_fPKTIME <= 0.0f )
		{
			m_fPKTIME = 0.0f;

			//	Note : 성향 이벤트 종료알림
			GLMSG::SNET_GM_FREEPK NetMsgFld;
			NetMsgFld.dwPKTIME = 0;

			GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgFld ); // 필드에 알리고

			GLMSG::SNET_GM_FREEPK_BRD NetMsgBrd;
			NetMsgBrd.dwPKTIME	   = 0;
			NetMsgBrd.bSTATEUPDATE = false;
			GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsgBrd );  // 모두에게 알림
		}
		return true;
	}
	return false;
} // end of decl
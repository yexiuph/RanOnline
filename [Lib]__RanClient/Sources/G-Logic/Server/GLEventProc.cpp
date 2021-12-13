#include "pch.h"
#include "./GLEventProc.h"
#include "./GLContrlMsg.h"
#include "./GLogicData.h"
#include "./GLGaeaServer.h"
#include "./GLChar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLEventProc::GLEventProc(void)
{
}

GLEventProc::~GLEventProc(void)
{
}

void GLEventProc::MsgEventExp ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_EVENT_EXP *pNetMsg = (GLMSG::SNET_GM_EVENT_EXP *) nmg;

	DWORD dwAgentSlot = GLGaeaServer::GetInstance().GetAgentSlot();

	if ( dwAgentSlot != dwClientID )		return;

	GLCONST_CHAR::SETEXP_SCALE ( pNetMsg->fSCALE );

	//	Note : 변경된 값 회신.
	//
	if ( GLGaeaServer::GetInstance().GetChar(dwGaeaID) )
	{
		GLMSG::SNET_GM_EVENT_EXP_FB NetMsgFB;
		NetMsgFB.wFIELDSVR = (WORD) GLGaeaServer::GetInstance().GetFieldSvrID();
		NetMsgFB.fSCALE = pNetMsg->fSCALE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( dwClientID, &NetMsgFB );
	}
}

void GLEventProc::MsgEventExpEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_EVENT_EXP *pNetMsg = (GLMSG::SNET_GM_EVENT_EXP *) nmg;

	DWORD dwAgentSlot = GLGaeaServer::GetInstance().GetAgentSlot();

	if ( dwAgentSlot != dwClientID )		return;

	// Have to Modify for Test Server PARAM
#ifdef KRT_PARAM	
	GLCONST_CHAR::SETEXP_SCALE ( 2.0f );
#else	
	GLCONST_CHAR::SETEXP_SCALE ( 1.0f );
#endif

	//	Note : 회신.
	//
	if ( GLGaeaServer::GetInstance().GetChar(dwGaeaID) )
	{
		GLMSG::SNET_GM_EVENT_EXP_END_FB NetMsgFB;
		NetMsgFB.wFIELDSVR = (WORD) GLGaeaServer::GetInstance().GetFieldSvrID();
		GLGaeaServer::GetInstance().SENDTOCLIENT ( dwClientID, &NetMsgFB );
	}
}

void GLEventProc::MsgEventItemGen ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_EVEN_ITEM_GEN *pNetMsg = (GLMSG::SNET_GM_EVEN_ITEM_GEN *) nmg;

	DWORD dwAgentSlot = GLGaeaServer::GetInstance().GetAgentSlot();

	if ( dwAgentSlot != dwClientID )		return;

	if ( pNetMsg->fRATE > 10.0f || pNetMsg->fRATE < 0.0f )
	{
		if ( GLGaeaServer::GetInstance().GetChar(dwGaeaID) )
		{
			GLMSG::SNET_GM_EVEN_ITEM_GEN_FB NetMsgFB;
			NetMsgFB.fRATE = GLCONST_CHAR::fITEM_DROP_SCALE;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( dwClientID, &NetMsgFB );
		}
		return;
	}

	GLCONST_CHAR::fITEM_DROP_SCALE = pNetMsg->fRATE;

	if ( GLGaeaServer::GetInstance().GetChar(dwGaeaID) )
	{
		GLMSG::SNET_GM_EVEN_ITEM_GEN_FB NetMsgFB;
		NetMsgFB.fRATE = GLCONST_CHAR::fITEM_DROP_SCALE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( dwClientID, &NetMsgFB );
	}
}

void GLEventProc::MsgEventItemGenEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_EVEN_ITEM_GEN_END *pNetMsg = (GLMSG::SNET_GM_EVEN_ITEM_GEN_END *) nmg;

	DWORD dwAgentSlot = GLGaeaServer::GetInstance().GetAgentSlot();

	if ( dwAgentSlot != dwClientID )		return;

	GLCONST_CHAR::fITEM_DROP_SCALE = 1.0f;

	if ( GLGaeaServer::GetInstance().GetChar(dwGaeaID) )
	{
		GLMSG::SNET_GM_EVEN_ITEM_GEN_END_FB NetMsgFB;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( dwClientID, &NetMsgFB );
	}
}

void GLEventProc::MsgEventMoneyGen ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_EVEN_MONEY_GEN *pNetMsg = (GLMSG::SNET_GM_EVEN_MONEY_GEN *) nmg;

	DWORD dwAgentSlot = GLGaeaServer::GetInstance().GetAgentSlot();

	if ( dwAgentSlot != dwClientID )		return;

	if ( pNetMsg->fRATE > 10.0f || pNetMsg->fRATE < 0.0f )
	{
		return;
	}

	GLCONST_CHAR::fMONEY_DROP_SCALE = pNetMsg->fRATE;
}

void GLEventProc::MsgEventMoneyGenEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_EVEN_MONEY_GEN_END *pNetMsg = (GLMSG::SNET_GM_EVEN_MONEY_GEN_END *) nmg;

	DWORD dwAgentSlot = GLGaeaServer::GetInstance().GetAgentSlot();

	if ( dwAgentSlot != dwClientID )		return;

	GLCONST_CHAR::fMONEY_DROP_SCALE = 1.0f;
}

void GLEventProc::MsgProcess ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GM_EVENT_EXP:				MsgEventExp ( nmg, dwClientID, dwGaeaID );			break;
	case NET_MSG_GM_EVENT_EXP_END:			MsgEventExpEnd ( nmg, dwClientID, dwGaeaID  );		break;
	case NET_MSG_GM_EVENT_ITEM_GEN:			MsgEventItemGen ( nmg, dwClientID, dwGaeaID  );		break;
	case NET_MSG_GM_EVENT_ITEM_GEN_END:		MsgEventItemGenEnd ( nmg, dwClientID, dwGaeaID  );	break;
	case NET_MSG_GM_EVENT_MONEY_GEN:		MsgEventMoneyGen ( nmg, dwClientID, dwGaeaID  );	break;
	case NET_MSG_GM_EVENT_MONEY_GEN_END:	MsgEventMoneyGenEnd ( nmg, dwClientID, dwGaeaID  );	break;
	default: break;
	};
}

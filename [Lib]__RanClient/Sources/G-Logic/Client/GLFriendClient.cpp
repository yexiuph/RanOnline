#include "pch.h"
#include ".\glfriendclient.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "DxGlobalStage.h"
#include "GLLandManClient.h"
#include "RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLFriendClient& GLFriendClient::GetInstance ()
{
	static  GLFriendClient Instance;
	return Instance;
}

GLFriendClient::GLFriendClient(void)
	: m_bReqList(false)
{
}

GLFriendClient::~GLFriendClient(void)
{
}

INT GLFriendClient::GetFriendSize( EMFRIEND_FLAGS emFLAGS )
{
	INT nCnt(0);

	switch( emFLAGS )
	{
	case EMFRIEND_BLOCK:
		{
			FRIENDMAP_ITER iter = m_mapFriend.begin();
			for( ; iter != m_mapFriend.end(); ++iter )
				if( iter->second.IsBLOCK() ) ++nCnt;
		}
		break;

	case EMFRIEND_VALID:
		{
			FRIENDMAP_ITER iter = m_mapFriend.begin();
			for( ; iter != m_mapFriend.end(); ++iter )
				if( iter->second.IsVALID() ) ++nCnt;
		}
		break;

	case EMFRIEND_OFF:
		{
			FRIENDMAP_ITER iter = m_mapFriend.begin();
			for( ; iter != m_mapFriend.end(); ++iter )
				if( iter->second.IsOFF() ) ++nCnt;
		}
		break;

	case EMFRIEND_BETRAY:
		{
			FRIENDMAP_ITER iter = m_mapFriend.begin();
			for( ; iter != m_mapFriend.end(); ++iter )
				if( iter->second.IsBETRAY() ) ++nCnt;
		}
		break;

	default:
		GASSERT( 0 && "GLFriendClient::GetFriendSize() : EMFRIEND_FLAGS Error" );
	}

	return nCnt;
}

SFRIEND* GLFriendClient::GetFriend ( const char *szName )
{
	if ( !szName )					return NULL;

	FRIENDMAP_ITER iter = m_mapFriend.find ( szName );
	if ( iter==m_mapFriend.end() )	return NULL;

	return &(*iter).second;
}

void GLFriendClient::Clear ()
{
	m_bReqList = false;
	m_mapFriend.clear();
}

void GLFriendClient::ReqFriendList ()
{
	if ( m_bReqList )	return;

	m_bReqList = true;

	//	Note : 메시지 전송.
	GLMSG::SNETPC_REQ_FRIENDLIST NetMsg;
	NETSEND(&NetMsg);
}

void GLFriendClient::DelFriendInfo ( const char* szName )
{
	FRIENDMAP_ITER iter = m_mapFriend.find ( szName );
	if ( iter!=m_mapFriend.end() )
	{
		m_mapFriend.erase ( iter );
	}
}

void GLFriendClient::ReqFriendAdd ( const char* szName )
{
	GASSERT(szName&&"GLFriendClient::ReqFriendAddFb()");
	if ( !szName )	return;

	FRIENDMAP_ITER iter = m_mapFriend.find ( szName );
	if ( iter!=m_mapFriend.end() )
	{
		//	Note : 이미 추가되어 있습니다.
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("FRIEND_ADD_ALREADY"), szName );
		return;
	}

	//	Note : 메시지 전송.
	GLMSG::SNETPC_REQ_FRIENDADD NetMsg;
	StringCchCopy ( NetMsg.szADD_CHAR, CHR_ID_LENGTH+1, szName );
	NETSEND(&NetMsg);
}

void GLFriendClient::ReqFriendLureAns ( const char* szReqName, bool bOK )
{
	//	Note : 메시지 전송.
	GLMSG::SNETPC_REQ_FRIENDADD_ANS NetMsg;
	StringCchCopy ( NetMsg.szREQ_CHAR, CHR_ID_LENGTH+1, szReqName );
	NetMsg.bOK = bOK;
	NETSEND(&NetMsg);
}

void GLFriendClient::ReqFriendDel ( const char* szDelName )
{
	FRIENDMAP_ITER iter = m_mapFriend.find ( szDelName );
	if ( iter==m_mapFriend.end() )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("FRIEND_DEL_FAIL"), szDelName );
		return;
	}

	//	Note : 메시지 전송.
	GLMSG::SNETPC_REQ_FRIENDDEL NetMsg;
	StringCchCopy ( NetMsg.szDEL_CHAR, CHR_ID_LENGTH+1, szDelName );
	NETSEND(&NetMsg);
}

void GLFriendClient::ReqFriendBlock ( const char* szName, bool bBLOCK )
{
	//	Note : 메시지 전송.
	GLMSG::SNETPC_REQ_FRIENDBLOCK NetMsg;
	NetMsg.bBLOCK = bBLOCK;
	StringCchCopy ( NetMsg.szCHAR, CHR_ID_LENGTH+1, szName );
	NETSEND(&NetMsg);
}

void GLFriendClient::MsgReqFriendAddFb ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDADD_FB *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDADD_FB *) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMFRIENDADD_FAIL:
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("FRIEND_ADD_FAIL"), pNetMsg->szADD_CHAR );
		break;

	case EMFRIENDADD_OK:
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("FRIEND_ADD_OK"), pNetMsg->szADD_CHAR );
		break;

	case EMFRIENDADD_REFUSE:
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("FRIEND_ADD_REFUSE"), pNetMsg->szADD_CHAR );
		break;

	case EMFRIENDADD_ABSENCE:
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMFRIENDADD_ABSENCE"), pNetMsg->szADD_CHAR );
		break;
	};
}

void GLFriendClient::MsgReqFriendAddLure ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDADD_LURE *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDADD_LURE *) nmg;

	//	Note : 자동거절인지 확인.
	if ( RANPARAM::bDIS_FRIEND )
	{
		//	Note : 메시지 전송.
		GLMSG::SNETPC_REQ_FRIENDADD_ANS NetMsg;
		StringCchCopy ( NetMsg.szREQ_CHAR, CHR_ID_LENGTH+1, pNetMsg->szREQ_CHAR );
		NetMsg.bOK = false;
		NETSEND(&NetMsg);

		return;
	}

	//	Note : 인터페이스에 친구 수락여부 선택 dlg 출력 요청.
	CInnerInterface::GetInstance().DOMODAL_ADD_FRIEND_REQ ( pNetMsg->szREQ_CHAR );
}

void GLFriendClient::MsgReqFriendDelFb ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDDEL_FB *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDDEL_FB *) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMFRIENDDEL_FAIL:
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("FRIEND_DEL_FAIL"), pNetMsg->szDEL_CHAR );
		break;

	case EMFRIENDDEL_OK:
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("FRIEND_DEL_OK"), pNetMsg->szDEL_CHAR );
		
		//	Note : 친구 리스트에서 삭제됨.
		DelFriendInfo ( pNetMsg->szDEL_CHAR );

		//	Note : 인터페이스에 변경 알림.
		CInnerInterface::GetInstance().REFRESH_FRIEND_LIST();
		break;
	};
}

void GLFriendClient::MsgReqFriendBlockFb ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDBLOCK_FB *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDBLOCK_FB *) nmg;
	switch ( pNetMsg->emFB )
	{
	case EMFRIENDBLOCK_FAIL:
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMFRIENDBLOCK_FAIL"), pNetMsg->szCHAR );
		break;

	case EMFRIENDBLOCK_ON:
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMFRIENDBLOCK_ON"), pNetMsg->szCHAR );
		break;

	case EMFRIENDBLOCK_OFF:
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMFRIENDBLOCK_OFF"), pNetMsg->szCHAR );
		break;
	};
}

void GLFriendClient::MsgReqFriendInfo ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDINFO *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDINFO *) nmg;

	SFRIEND sFRIEND;
	sFRIEND = pNetMsg->sFRIEND;

	m_mapFriend[std::string(pNetMsg->sFRIEND.szCharName)] = sFRIEND;

	//	Note : 인터페이스에 변경 알림.
	CInnerInterface::GetInstance().REFRESH_FRIEND_LIST();
}

void GLFriendClient::MsgReqFriendState ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDSTATE *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDSTATE *) nmg;

	FRIENDMAP_ITER iter = m_mapFriend.find ( pNetMsg->szCHAR );
	if ( iter!=m_mapFriend.end() )
	{
		SFRIEND &sFRIEND = (*iter).second;
		sFRIEND.nCharFlag = pNetMsg->nChaFlag;

		sFRIEND.bONLINE = pNetMsg->bONLINE;
		sFRIEND.nCHANNEL = pNetMsg->nCHANNEL;

		if(  sFRIEND.bONLINE )
		{
			CInnerInterface::GetInstance().PrintMsgText( 
				NS_UITEXTCOLOR::LIGHTSKYBLUE, 
				ID2GAMEINTEXT( "FRIEND_ONLINE" ),
				pNetMsg->szCHAR );
		}

		//	Note : 인터페이스에 상태 변경 알림.
		CInnerInterface::GetInstance().REFRESH_FRIEND_STATE();
		CInnerInterface::GetInstance().FRIEND_LIST( sFRIEND.szCharName, sFRIEND.bONLINE );
	}
}

void GLFriendClient::MsgReqFriendRename ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDRENAME_CLT *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDRENAME_CLT *) nmg;

	FRIENDMAP_ITER iter = m_mapFriend.find ( pNetMsg->szOldName );
	GASSERT ( iter != m_mapFriend.end() );
	if ( iter != m_mapFriend.end() )
	{
		SFRIEND sFRIEND = (*iter).second;
		StringCchCopy ( sFRIEND.szCharName, CHAR_SZNAME, pNetMsg->szNewName );
		m_mapFriend.erase(iter);
		m_mapFriend[std::string(pNetMsg->szNewName)] = sFRIEND;

		//	Note : 인터페이스에 상태 변경 알림.
		CInnerInterface::GetInstance().REFRESH_FRIEND_STATE();
		CInnerInterface::GetInstance().FRIEND_LIST( sFRIEND.szCharName, sFRIEND.bONLINE );
	}
}

void GLFriendClient::MsgReqFriendPhoneNumber ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDPHONENUMBER_CLT *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDPHONENUMBER_CLT *) nmg;

	FRIENDMAP_ITER iter = m_mapFriend.find ( pNetMsg->szName );
	GASSERT ( iter != m_mapFriend.end() );
	if ( iter != m_mapFriend.end() )
	{
		SFRIEND &sFRIEND = (*iter).second;

		StringCchCopy ( sFRIEND.szPhoneNumber, SMS_RECEIVER, pNetMsg->szNewPhoneNumber );

		//	Note : 인터페이스에 상태 변경 알림.
		CInnerInterface::GetInstance().REFRESH_FRIEND_STATE();
		CInnerInterface::GetInstance().FRIEND_LIST( sFRIEND.szCharName, sFRIEND.bONLINE );
	}
}

void GLFriendClient::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_REQ_FRIENDADD_FB:				MsgReqFriendAddFb(nmg);			break;
	case NET_MSG_REQ_FRIENDADD_LURE:			MsgReqFriendAddLure(nmg);		break;
	case NET_MSG_REQ_FRIENDDEL_FB:				MsgReqFriendDelFb(nmg);			break;
	case NET_MSG_REQ_FRIENDBLOCK_FB:			MsgReqFriendBlockFb(nmg);		break;
	case NET_MSG_FRIENDINFO:					MsgReqFriendInfo(nmg);			break;
	case NET_MSG_FRIENDSTATE:					MsgReqFriendState(nmg);			break;
	case NET_MSG_GCTRL_FRIEND_RENAME_CLT:		MsgReqFriendRename(nmg);		break;
	case NET_MSG_GCTRL_FRIEND_PHONENUMBER_CLT:	MsgReqFriendPhoneNumber(nmg);	break;
	};
}

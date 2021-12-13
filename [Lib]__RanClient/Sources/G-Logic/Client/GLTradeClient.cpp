#include "pch.h"

#include "GLogicData.h"
#include "GLItemMan.h"
#include "DxGlobalStage.h"
#include "GLGaeaClient.h"
#include "GLCharacter.h"

#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"

#include "gltradeclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLTradeClient& GLTradeClient::GetInstance()
{
	static GLTradeClient Instance;
	return Instance;
}

GLTradeClient::GLTradeClient(void) :
	m_bTrade(FALSE)
{
}

GLTradeClient::~GLTradeClient(void)
{
}

BOOL GLTradeClient::Start ( DWORD dwTargetID )
{
	Reset ();

	m_bTrade = TRUE;

	PGLCHARCLIENT pChar = GLGaeaClient::GetInstance().GetActiveMap()->GetChar(dwTargetID);
	if ( !pChar )	return FALSE;

	m_sTarTrade.SetTarget ( dwTargetID );
	m_sMyTrade.SetTarget ( GLGaeaClient::GetInstance().GetCharacter()->m_dwGaeaID );

	CInnerInterface::GetInstance().HideGroup ( PET_WINDOW );

	return TRUE;
}

BOOL GLTradeClient::Reset ()
{
	m_bTrade = FALSE;

	m_sMyTrade.Reset();
	m_sTarTrade.Reset();

	m_sPreTradeItem.RESET();

	return TRUE;
}

BOOL GLTradeClient::Valid ()
{
	return m_bTrade;
}

void GLTradeClient::SetPreItem ( const SINVEN_POS& sPreItem )
{
	m_sPreTradeItem = sPreItem;
}


SINVENITEM* GLTradeClient::FindPosItem ( const WORD wPosX, const WORD wPosY )
{
	return m_sMyTrade.GetTradeBox().FindPosItem ( wPosX, wPosY );
}

SINVENITEM* GLTradeClient::FindItemBack ( const WORD wBackX, const WORD wBackY )
{
	return m_sMyTrade.GetTradeBox().FindItemBack ( wBackX, wBackY );
}

BOOL GLTradeClient::IsInsertable ( const WORD wSX, const WORD wSY, const WORD wPosX, const WORD wPosY )
{
	return m_sMyTrade.GetTradeBox().IsInsertable ( wSX, wSY, wPosX, wPosY );
}

BOOL GLTradeClient::IsExInsertable ( const WORD wSX, const WORD wSY, const WORD wPosX, const WORD wPosY )
{
	GLInventory sBoxTemp;
	sBoxTemp.Assign ( m_sMyTrade.GetTradeBox() );
	
	SINVENITEM *pOldItem = sBoxTemp.FindPosItem ( wPosX, wPosY );
	sBoxTemp.DeleteItem ( pOldItem->wPosX, pOldItem->wPosY );

	return sBoxTemp.IsInsertable ( wSX, wSY, wPosX, wPosY );
}

BOOL GLTradeClient::IsAgreeAble ()
{
	return m_sMyTrade.IsAgreeAble();
}

void GLTradeClient::BlockAgree ()
{
	m_sMyTrade.BlockAgree ();
	m_sTarTrade.BlockAgree ();
}

BOOL GLTradeClient::MsgTradeFB ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_FB *pNetMsg = (GLMSG::SNET_TRADE_FB *) nmg;
	
	switch ( pNetMsg->emAns )
	{
	case EMTRADE_OK:
		{
			BOOL bOk = Start ( pNetMsg->dwTargetID );
			if ( !bOk )
			{
				//	Note : 거래가 시작 되지 못했습니다.
				GLMSG::SNET_TRADE_CANCEL NetMsg;
				NETSENDTOFIELD ( (NET_MSG_GENERIC*) &NetMsg );
				break;
			}

			PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();

			CString strName;
			PGLCHARCLIENT pChar = pLand->GetChar ( pNetMsg->dwTargetID );
			if ( pChar )	strName = pChar->GetName();
			CInnerInterface::GetInstance().SetTradeWindowOpen ( strName, pNetMsg->dwTargetID );
		}
		break;

	case EMTRADE_CANCEL:
		//	상대방이 거래를 거절 하였습니다.
		PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
		PGLCHARCLIENT pChar = pLand->GetChar ( pNetMsg->dwTargetID );
		if ( !pChar )	return FALSE;
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_FB_CANCEL"), pChar->GetName() );
		break;
	};

	return TRUE;
}

BOOL GLTradeClient::MsgTradeAgreeTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_AGREE_TAR *pNetMsg = (GLMSG::SNET_TRADE_AGREE_TAR *) nmg;
	
	BOOL bMY = GLGaeaClient::GetInstance().GetCharacter()->m_dwGaeaID == pNetMsg->dwGaeaID;

	switch ( pNetMsg->emFB )
	{
	case EMTRADE_AGREE_OK:
		if ( bMY )	m_sMyTrade.SetAgree();
		else		m_sTarTrade.SetAgree();
		break;

	case EMTRADE_AGREE_CANCEL:
		if ( bMY )	m_sMyTrade.ReSetAgree();
		else		m_sTarTrade.ReSetAgree();
		break;

	case EMTRADE_AGREE_TIME: //	시간 경과후 다시 시도 하십시요.
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("TRADE_AGREE_TIME") );
		break;
	};

	return TRUE;
}

BOOL GLTradeClient::MsgTradeItemResistTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_ITEM_REGIST_TAR *pNetMsg = (GLMSG::SNET_TRADE_ITEM_REGIST_TAR *) nmg;

	BOOL bMY = GLGaeaClient::GetInstance().GetCharacter()->m_dwGaeaID == pNetMsg->dwGaeaID;
	SINVENITEM &sInsert = pNetMsg->sInsert;

	if ( bMY )		m_sMyTrade.InsertItem ( &sInsert );
	else			m_sTarTrade.InsertItem ( &sInsert );

	BlockAgree ();

	return TRUE;
}

BOOL GLTradeClient::MsgTradeItemReMoveTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_ITEM_REMOVE_TAR *pNetMsg = (GLMSG::SNET_TRADE_ITEM_REMOVE_TAR *) nmg;

	BOOL bMY = GLGaeaClient::GetInstance().GetCharacter()->m_dwGaeaID == pNetMsg->dwGaeaID;

	if ( bMY )		m_sMyTrade.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	else			m_sTarTrade.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	BlockAgree ();

	return TRUE;
}

BOOL GLTradeClient::MsgTradeMoneyTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_MONEY_TAR *pNetMsg = (GLMSG::SNET_TRADE_MONEY_TAR *) nmg;

	BOOL bMY = GLGaeaClient::GetInstance().GetCharacter()->m_dwGaeaID == pNetMsg->dwGaeaID;

	if ( bMY )		m_sMyTrade.SetMoney ( pNetMsg->lnMoney );
	else			m_sTarTrade.SetMoney ( pNetMsg->lnMoney );

	BlockAgree ();

	return TRUE;
}

BOOL GLTradeClient::MsgTradeCompleteTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_COMPLETE_TAR *pNetMsg = (GLMSG::SNET_TRADE_COMPLETE_TAR *) nmg;

	//	거래가 성공적으로 완료되었습니다.
	CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("TRADE_COMPLETE") );

	Reset ();

	CInnerInterface::GetInstance().SetTradeWindowClose ();

	// 거래중 수신한 팻카드 정보를 제거해준다.
	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
	pCharacter->m_mapPETCardInfoTemp.clear();
	pCharacter->m_mapVEHICLEItemInfoTemp.clear();

	return TRUE;
}

BOOL GLTradeClient::MsgTradeCancelTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_CANCEL_TAR *pNetMsg = (GLMSG::SNET_TRADE_CANCEL_TAR *) nmg;

	switch ( pNetMsg->emCancel )
	{
	case EMTRADE_CANCEL_NORMAL:
		//	일반적인 거래 취소.
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_CANCEL_NORMAL") );
		break;

	case EMTRADE_CANCEL_OTHER:
		//	상대편이 거래 취소.
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_CANCEL_OTHER") );
		break;

	case EMTRADE_CANCEL_MYINVEN:
		//	거래 수행중 자신의 인벤의 공간 부족.
		CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_CANCEL_MYINVEN") );
		break;

	case EMTRADE_CANCEL_TARINVEN:
		//	거래 수행중 상대편의 인벤의 공간 부족.
		CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_CANCEL_TARINVEN") );
		break;
	};

	Reset ();

	CInnerInterface::GetInstance().SetTradeWindowClose ();

	return TRUE;
}

void GLTradeClient::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_TRADE_FB:
		MsgTradeFB(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_AGREE_TAR:
		MsgTradeAgreeTar(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR:
		MsgTradeItemResistTar(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR:
		MsgTradeItemReMoveTar(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_MONEY_TAR:
		MsgTradeMoneyTar(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_COMPLETE_TAR:
		MsgTradeCompleteTar(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_CANCEL_TAR:
		MsgTradeCancelTar(nmg);
		break;
	};
}

HRESULT GLTradeClient::FrameMove ( float fTime, float fElapsedTime )
{
	if ( !Valid() )		return S_FALSE;

	m_sMyTrade.FrameMove ( fTime, fElapsedTime );
	m_sTarTrade.FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

BOOL GLTradeClient::ReqTradeMoney ( LONGLONG lnMoney )
{
	GLMSG::SNET_TRADE_MONEY	NetMsg;
	NetMsg.lnMoney = lnMoney;
	NETSENDTOFIELD ( &NetMsg );

	return TRUE;
}
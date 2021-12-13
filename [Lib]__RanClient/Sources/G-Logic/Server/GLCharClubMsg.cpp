#include "pch.h"
#include "./GLChar.h"
#include "./GLGaeaServer.h"

#include "RanFilter.h"
#include "StringUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// *****************************************************
// Desc: Ŭ�� ��û ó��
// *****************************************************
HRESULT GLChar::MsgClubNew ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_NEW *pNetMsg = (GLMSG::SNET_CLUB_NEW *) nmg;

	GLMSG::SNET_CLUB_NEW_FB NetMsgFb;
	StringCchCopy ( NetMsgFb.szClubName, CHAR_SZNAME, pNetMsg->szClubName );

	CString strClubName( NetMsgFb.szClubName );

	BOOL bFILTER0 = STRUTIL::CheckString( strClubName );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strClubName );
	if ( bFILTER0 || bFILTER1 )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_BADNAME;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;			
	}

#ifdef TH_PARAM
	// �±��� ���� ���� üũ
	if ( !m_pCheckString ) return S_FALSE;	
		
	if ( !m_pCheckString(strClubName) )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_THAICHAR_ERROR;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;
	}
#endif

#ifdef VN_PARAM
	// ��Ʈ�� ���� ���� üũ 
	if( STRUTIL::CheckVietnamString( strClubName ) )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_VNCHAR_ERROR;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFb);
		return E_FAIL;
	}

#endif 

	if ( !m_pLandMan )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;
	}

	PGLCROW pCROW = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );
	if ( !pCROW )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;
	}

	if ( !pCROW->m_pCrowData->m_sNpcTalkDlg.IsCLUBNEW() )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;
	}

	if ( m_dwGuild!=CLUB_NULL )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_ALREADY;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;
	}

	GLPARTY_FIELD* pPARTY = GLGaeaServer::GetInstance().GetParty ( m_dwPartyID );
	if ( !pPARTY || !pPARTY->ISVAILD() || !pPARTY->ISMASTER(m_dwGaeaID) )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_NOTMASTER;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;
	}

	if ( pPARTY->GETNUMBER() < GLCONST_CHAR::dwCLUB_PARTYNUM )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_NOTMEMBER;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;
	}

	PGLCHAR pCHAR = NULL;
	GLPARTY_FIELD::MEMBER_ITER pos = pPARTY->m_cMEMBER.begin();
	GLPARTY_FIELD::MEMBER_ITER end = pPARTY->m_cMEMBER.end();
	for ( ; pos!=end; ++pos )
	{
		pCHAR = GLGaeaServer::GetInstance().GetChar ( (*pos).first );
		if ( !pCHAR )
		{
			NetMsgFb.emFB = EMCLUB_NEW_FB_NOTEXIST_MEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
			return S_FALSE;
		}

		if ( pCHAR->m_dwGuild != CLUB_NULL )
		{
			NetMsgFb.emFB = EMCLUB_NEW_FB_MEM_ALREADY;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
			return S_FALSE;
		}

		//	Note : �ٸ� �п����� Ŭ�� �Ἲ ���� �ɼ��� ���� ��� ������.
		if ( !GLCONST_CHAR::bCLUB_2OTHERSCHOOL )
		{
			if ( pCHAR->m_wSchool!=m_wSchool )
			{
				NetMsgFb.emFB = EMCLUB_NEW_FB_OTHERSCHOOL;
				GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
				return S_FALSE;
			}
		}
	}

	if ( m_wLevel < GLCONST_CHAR::sCLUBRANK[0].m_dwMasterLvl )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_MASTER_LVL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;
	}

	if ( m_nLiving < int ( GLCONST_CHAR::sCLUBRANK[0].m_dwLivingPoint ) )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_AVER_LIVING;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;
	}

	if ( m_lnMoney < GLCONST_CHAR::sCLUBRANK[0].m_dwPay )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_NOTMONEY;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );
		return S_FALSE;
	}

	// Agent ���� Ŭ�� ������ ��û��
	GLMSG::SNET_CLUB_NEW_2AGT NetMsg2Agt;
	StringCchCopy ( NetMsg2Agt.szClubName, CHAR_SZNAME, pNetMsg->szClubName );
	GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsg2Agt );

	return S_OK;
}

// *****************************************************
// Desc: Ŭ������ ��û�� ���� ��� ó��
// *****************************************************
HRESULT GLChar::MsgClubNew2FLD ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_NEW_2FLD *pNetMsg = (GLMSG::SNET_CLUB_NEW_2FLD *) nmg;

	LONGLONG lnPAY = GLCONST_CHAR::sCLUBRANK[0].m_dwPay;

	//	Note : ���� �����ÿ� ���� ��� ����.
	if ( pNetMsg->emFB==EMCLUB_NEW_FB_OK )
	{
		CheckMoneyUpdate( m_lnMoney, lnPAY, FALSE, "Club Create Pay." );
		m_bMoneyUpdate = TRUE;

		if ( m_lnMoney >= lnPAY )
		{
			m_lnMoney -= lnPAY;
		}
		else
		{
			m_lnMoney = 0;
		}
	}

	//	Note : �ݾ� ��ȭ �α�.
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CLUB, 0, -lnPAY, EMITEM_ROUTE_DELETE );
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );

	//	Note : �ݾ� ��ȭ �˸�.
	GLMSG::SNETPC_UPDATE_MONEY NetMsgMoney;
	NetMsgMoney.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgMoney );

	//	Note : ���� ��û ��� ��ȯ.
	//
	GLMSG::SNET_CLUB_NEW_FB NetMsgFb;
	NetMsgFb.emFB = pNetMsg->emFB;
	StringCchCopy ( NetMsgFb.szClubName, CHAR_SZNAME, pNetMsg->szClubName );
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFb );

	//	Note : Ŭ�� ���� �α� ���.
	GLITEMLMT::GetInstance().ReqAction
	(
		m_dwCharID,					//	�����.
		EMLOGACT_CLUB_MAKE,			//	����.
		ID_CLUB, pNetMsg->dwCLUBID,	//	����.
		0,							//	exp
		0,							//	bright
		0,							//	life
		(int)-lnPAY					//	money
	);

	return S_OK;
}

HRESULT GLChar::MsgClubMemberReq ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_MEMBER_REQ *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_REQ *) nmg;

	GLMSG::SNET_CLUB_MEMBER_REQ_FB NetMsgFB;

	PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	StringCchCopy ( NetMsgFB.szReqName, CHAR_SZNAME, pCHAR->m_szName );

	if ( pCHAR->m_dwGuild!=CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	//	Note : �ٸ� �п����� Ŭ�� �Ἲ ���� �ɼ��� ���� ��� ������.
	if ( !GLCONST_CHAR::bCLUB_2OTHERSCHOOL )
	{
		if ( pCHAR->m_wSchool!=m_wSchool )
		{
			NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
			return S_FALSE;
		}
	}

	GLClubMan &cClubMan = GLGaeaServer::GetInstance().GetClubMan();
	GLCLUB *pCLUB = cClubMan.GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_CLUBBATTLE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pCLUB->IsMemberFlgJoin(m_dwCharID) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	bool bFOUND = pCLUB->IsMember ( pCHAR->m_dwCharID );
	if ( bFOUND )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_ALREADY;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	const GLCLUBRANK &sRANK_DATA = GLCONST_CHAR::sCLUBRANK[pCLUB->m_dwRank];
	if ( pCLUB->GetNemberNum() >= sRANK_DATA.m_dwMember )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_MAXNUM;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	GLMSG::SNET_CLUB_MEMBER_REQ_ASK NetMsgAsk;
	StringCchCopy ( NetMsgAsk.szClubName, CHAR_SZNAME, pCLUB->m_szName );
	NetMsgAsk.dwMaster = m_dwCharID;
	StringCchCopy ( NetMsgAsk.szMasterName, CHAR_SZNAME, pCLUB->m_szMasterName );

	GLGaeaServer::GetInstance().SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgAsk );
	
	return S_OK;
}

HRESULT GLChar::MsgClubMemberAns ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_MEMBER_REQ_ANS *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_REQ_ANS *) nmg;

	PGLCHAR pMaster = GLGaeaServer::GetInstance().GetCharID ( pNetMsg->dwMaster );
	if ( !pMaster )
	{
		return S_FALSE;
	}

	GLMSG::SNET_CLUB_MEMBER_REQ_FB NetMsgFB;
	StringCchCopy ( NetMsgFB.szReqName, CHAR_SZNAME, m_szName );

	if ( m_dwGuild!=CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pMaster->m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pNetMsg->bOK )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_REFUSE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pMaster->m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	GLClubMan &cClubMan = GLGaeaServer::GetInstance().GetClubMan();
	GLCLUB *pCLUB = cClubMan.GetClub ( pMaster->m_dwGuild );
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pMaster->m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_CLUBBATTLE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pCLUB->IsMemberFlgJoin(pMaster->m_dwCharID) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pMaster->m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	bool bFOUND = pCLUB->IsMember ( m_dwCharID );
	if ( bFOUND )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_ALREADY;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pMaster->m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	const GLCLUBRANK &sRANK_DATA = GLCONST_CHAR::sCLUBRANK[pCLUB->m_dwRank];
	if ( pCLUB->GetNemberNum() >= sRANK_DATA.m_dwMember )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_MAXNUM;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pMaster->m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	GLMSG::SNET_CLUB_MEMBER_REQ_2AGT NetMsgAgt;
	NetMsgAgt.dwReqGID = m_dwGaeaID;
	GLGaeaServer::GetInstance().SENDTOAGENT ( pMaster->m_dwClientID, &NetMsgAgt );

	return S_OK;
}

HRESULT GLChar::MsgClubRank ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_RANK *pNetMsg = (GLMSG::SNET_CLUB_RANK *) nmg;

	GLMSG::SNET_CLUB_RANK_FB NetMsgFB;

	if ( !m_pLandMan )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	PGLCROW pCROW = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );
	if ( !pCROW )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pCROW->m_pCrowData->m_sNpcTalkDlg.IsCLUBRANKUP() )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( m_dwGuild==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	GLClubMan &cClubMan = GLGaeaServer::GetInstance().GetClubMan();
	GLCLUB *pCLUB = cClubMan.GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}
	
	if ( pCLUB->m_dwMasterID != m_dwCharID )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB->m_dwRank >= (GLCONST_CHAR::dwMAX_CLUBRANK-1) )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_MAX;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	DWORD dwRANK = pCLUB->m_dwRank + 1;

	if ( m_wLevel < GLCONST_CHAR::sCLUBRANK[dwRANK].m_dwMasterLvl )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_MASTER_LVL;
		NetMsgFB.dwValue = GLCONST_CHAR::sCLUBRANK[dwRANK].m_dwMasterLvl;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( m_nLiving < int ( GLCONST_CHAR::sCLUBRANK[dwRANK].m_dwLivingPoint ) )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_AVER_LIVING;
		NetMsgFB.dwValue = GLCONST_CHAR::sCLUBRANK[dwRANK].m_dwLivingPoint;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( m_lnMoney < GLCONST_CHAR::sCLUBRANK[dwRANK].m_dwPay )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_NOTMONEY;
		NetMsgFB.dwValue = GLCONST_CHAR::sCLUBRANK[dwRANK].m_dwPay;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	//	Note : ��ũ ��ȭ ������Ʈ�� ��û.
	GLMSG::SNET_CLUB_RANK_2AGT NetMsgAgt;
	GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgAgt );

	return S_OK;
}

HRESULT GLChar::MsgClubMemberNick ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_MEMBER_NICK *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_NICK *)nmg;

	GLMSG::SNET_CLUB_MEMBER_NICK_FB NetMsgFB;

	CString strGuNick(pNetMsg->szNick);

	BOOL bFILTER0 = STRUTIL::CheckString( strGuNick );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strGuNick );
	if ( bFILTER0 || bFILTER1 )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_BADNAME;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;			
	}

#ifdef TH_PARAM	
	// �±��� ���� ���� üũ 
	if ( !m_pCheckString ) return S_FALSE;	
		
	if ( !m_pCheckString(strGuNick) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}
#endif

#ifdef VN_PARAM
	// ��Ʈ�� ���� ���� üũ 
	if( STRUTIL::CheckVietnamString( strGuNick ) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}
#endif 



	GLClubMan& cClubMan = GLGaeaServer::GetInstance().GetClubMan();
	GLCLUB *pCLUB = cClubMan.GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	//if ( !pNetMsg->szNick || strlen(pNetMsg->szNick)==0 )
	//{
	//	NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_FAIL;
	//	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
	//	return S_FALSE;
	//}
	




	StringCchCopy ( m_szNick, CHAR_SZNAME, pNetMsg->szNick );

	GLMSG::SNET_CLUB_INFO_NICK_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	StringCchCopy ( NetMsgBrd.szNickName, CHAR_SZNAME, m_szNick );
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_OK;
	StringCchCopy ( NetMsgFB.szNick, CHAR_SZNAME, m_szNick );
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgClubCDCertify ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_CD_CERTIFY *pNetMsg = (GLMSG::SNET_CLUB_CD_CERTIFY *)nmg;

	GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFB;

	GLMSG::SNET_CLUB_CD_CERTIFY_BRD  NetMsgBrd;

	DxConsoleMsg* pConsoleMsg = GLGaeaServer::GetInstance().GetConsoleMsg();

	if ( !m_pLandMan )
	{
		return S_FALSE;
	}

	if ( m_dwGuild==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTMASTER;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	GLClubMan &sClubMan = GLGaeaServer::GetInstance().GetClubMan();
	GLCLUB *pCLUB = sClubMan.GetClub ( m_dwGuild );
	if ( !pCLUB || !pCLUB->IsMemberFlgCDCertify(m_dwCharID) )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTMASTER;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	// �����Ϸ��� Ŭ���� ���Ϳ� ���ԵǾ� ���� ��� ���� Ŭ���߿� ����Ŭ���� ������ ���� �Ұ�.
	if ( pCLUB->IsAlliance() )
	{
		// ���ͱ�� ������ ������ �常 �����ִ°� ����... ;;;
		GLCLUB *pCHIEFCLUB = sClubMan.GetClub ( pCLUB->m_dwAlliance );
		GLCLUB *pALLIANCECLUB = NULL;
		DWORD dwGuidID = 0;

		StringCchCopy ( NetMsgBrd.szAlliance, CHAR_SZNAME, pCHIEFCLUB->m_szName );

		CLUB_ALLIANCE_ITER pos = pCHIEFCLUB->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pCHIEFCLUB->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = *pos;
			if ( sALLIANCE.m_dwID == pCLUB->m_dwID ) continue;

			pALLIANCECLUB = sClubMan.GetClub ( sALLIANCE.m_dwID );
			if ( pALLIANCECLUB )
			{
				dwGuidID = GLGuidanceFieldMan::GetInstance().GetGuidID ( pALLIANCECLUB->m_dwID );
				if ( dwGuidID != UINT_MAX ) 
				{
					NetMsgFB.emFB = EMCDCERTIFY_HAVEGUIDCLUB;
					GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
					return S_FALSE;
				}
			}
		}
	}

	PGLCROW pCROW = m_pLandMan->GetCrow(pNetMsg->dwNpcID);
	if ( !pCROW )
	{
		NetMsgFB.emFB = EMCDCERTIFY_BADNPC;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCROW->m_pCrowData->m_sNpcTalkDlg.IsCDCERTIFY() )
	{
		NetMsgFB.emFB = EMCDCERTIFY_BADNPC;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	DWORD dwGUID = GLGuidanceFieldMan::GetInstance().GetGuidID ( m_dwGuild );
	if ( dwGUID != UINT_MAX )
	{
		if ( dwGUID==m_pLandMan->m_dwClubMapID )NetMsgFB.emFB = EMCDCERTIFY_AREADY;
		else									NetMsgFB.emFB = EMCDCERTIFY_AREADYGUID;

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	EMCHECKCERTIFY emCERTIFY = GLGuidanceFieldMan::GetInstance().CheckCertify ( m_pLandMan->m_dwClubMapID, m_dwCharID );
	if ( emCERTIFY!=EMCHECKCERTIFY_OK )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTCONDITION;

		switch ( emCERTIFY )
		{
		case EMCHECKCERTIFY_FAIL:
			NetMsgFB.emFB = EMCDCERTIFY_NOTCONDITION;
			break;
		case EMCHECKCERTIFY_DOING:
			NetMsgFB.emFB = EMCDCERTIFY_AREADYCERTIFY;
			break;
		case EMCHECKCERTIFY_OTHERDOING:
			NetMsgFB.emFB = EMCDCERTIFY_OTHERCERTIFING;
			break;
		case EMCHECKCERTIFY_NOTBATTLE:
			NetMsgFB.emFB = EMCDCERTIFY_NOTTIME;
			break;
		};

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	D3DXVECTOR3 vDist = pCROW->GetPosition() - m_vPos;
	float fDist = D3DXVec3Length ( &vDist );
	if ( fDist > GLCONST_CHAR::fCDCERTIFY_DIST )
	{
		NetMsgFB.emFB = EMCDCERTIFY_DISTANCE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	SINVENITEM *pINVENITEM = m_cInventory.FindItem ( ITEM_CD );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	WORD wPosX = pINVENITEM->wPosX;
	WORD wPosY = pINVENITEM->wPosY;

	//	������ ���� �α�.
	GLITEMLMT::GetInstance().ReqItemRoute ( pINVENITEM->sItemCustom, ID_CHAR, m_dwCharID, ID_CLUB, 0, EMITEM_ROUTE_DELETE, pINVENITEM->sItemCustom.wTurnNum );

	//	Note : �κ����� ������ ����.
	m_cInventory.DeleteItem ( wPosX, wPosY );

	//	[�ڽſ���] �޽��� �߻�.	- �ι꿡�� ������ ����.
	GLMSG::SNETPC_INVEN_DELETE NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	Note : ���� ����.
	GLGuidanceFieldMan::GetInstance().DoCertify ( m_pLandMan->m_dwClubMapID, m_dwCharID, pNetMsg->dwNpcID, m_vPos );

	//	Note : ���� ������ �˸� FB.
	NetMsgFB.emFB = EMCDCERTIFY_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	//	Note : ���� ������ �˸� BRD.
	std::string strZone = GLGuidanceFieldMan::GetInstance().GetName ( m_pLandMan->m_dwClubMapID );
	StringCchCopy ( NetMsgBrd.szZone, GLMSG::SNET_CLUB_CD_CERTIFY_BRD::TEXT_LEN, strZone.c_str() );
	StringCchCopy ( NetMsgBrd.szName, CHAR_SZNAME, m_szName );
	StringCchCopy ( NetMsgBrd.szClub, CHAR_SZNAME, pCLUB->m_szName );
	
	GLGaeaServer::GetInstance().SENDTOAGENT ( &NetMsgBrd );

	return S_OK;
}

HRESULT GLChar::MsgReqClubGetStorage ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_GETSTORAGE *pNetMsg = (GLMSG::SNET_CLUB_GETSTORAGE *) nmg;

	GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->m_dwMasterID != m_dwCharID )
	{
		return S_OK;
	}

	if ( !pCLUB->m_bVALID_STORAGE )
	{
		CGetClubStorage *pDbAction = new CGetClubStorage(m_dwClientID,m_dwCharID,m_dwGuild);
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
	}
	else
	{
		MsgClubStorage ();
	}

	return S_OK;
}

HRESULT GLChar::MsgReqClubStorageToHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_STORAGE_TO_HOLD *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_TO_HOLD *) nmg;

	GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->m_dwMasterID != m_dwCharID )
	{
		return S_OK;
	}

	if ( pNetMsg->dwChannel >= MAX_CLUBSTORAGE )
	{
		return E_FAIL;
	}

	const DWORD dwChannel = pNetMsg->dwChannel;

	//	��û�� ������ ��ȿ�� �˻�.
	SINVENITEM *pInvenItem = pCLUB->m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		return S_FALSE;
	}

	//	�տ� �� �������� ������.
	if ( VALID_HOLD_ITEM() )
	{
		return S_FALSE;
	}

	//	�ι꿡 �ִ� ������ ���.
	SINVENITEM sInven_BackUp = *pInvenItem;

	//	Note :�������� ���� ���� ��� ���.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sInven_BackUp.sItemCustom, ID_CLUB, m_dwGuild, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sInven_BackUp.sItemCustom.wTurnNum );

	//	�ι꿡�� ���� �������� ����.
	pCLUB->m_cStorage[dwChannel].DeleteItem ( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	//	�ι꿡 �־��� �������� �տ� ��.
	HOLD_ITEM ( sInven_BackUp.sItemCustom );

	//	[�ڽſ���] �޽��� �߻�.	- â���� ������ ����.
	GLMSG::SNET_CLUB_STORAGE_DELETE NetMsg;
	NetMsg.dwChannel = dwChannel;
	NetMsg.wPosX = sInven_BackUp.wPosX;
	NetMsg.wPosY = sInven_BackUp.wPosY;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	[�ڽſ���] Ŭ��â�� �־��� �������� �տ� ��.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = sInven_BackUp.sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	return S_OK;
}

bool GLChar::IsKEEP_CLUB_STORAGE ( DWORD dwCHANNEL )
{
	GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( m_dwGuild );
	if ( !pCLUB )								return false;
	if ( pCLUB->m_dwMasterID != m_dwCharID )	return false;
	if ( dwCHANNEL >= MAX_CLUBSTORAGE )				return false;

	if ( pCLUB->m_dwRank < dwCHANNEL )			return false;

	return true;
}

HRESULT GLChar::MsgReqClubStorageExHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_STORAGE_EX_HOLD *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_EX_HOLD *) nmg;

	GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->m_dwMasterID != m_dwCharID )
	{
		return S_OK;
	}

	if ( pNetMsg->dwChannel >= MAX_CLUBSTORAGE )
	{
		return E_FAIL;
	}

	if ( !IsKEEP_CLUB_STORAGE(pNetMsg->dwChannel) )
	{
		return S_FALSE;
	}

	const DWORD dwChannel = pNetMsg->dwChannel;

	if ( !VALID_HOLD_ITEM() )
	{
		return S_FALSE;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( GET_HOLD_ITEM().sNativeID );
	if ( !pITEM )		return false;
	
	//	�ŷ��ɼ�
	if ( !pITEM->sBasicOp.IsEXCHANGE() )	return FALSE;
	
	SINVENITEM *pInvenItem = pCLUB->m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		return S_FALSE;
	}

	SINVENITEM sInven_BackUp = *pInvenItem;		//	�ι꿡 �ִ� ������ ���.
	
	WORD wPosX = sInven_BackUp.wPosX;			//	������ ��ġ.
	WORD wPosY = sInven_BackUp.wPosY;

	SITEMCUSTOM sItemHold = GET_HOLD_ITEM();	//	�տ� �� ������ ���.

#if defined(VN_PARAM) //vietnamtest%%%
	if ( sItemHold.bVietnamGainItem )	return S_FALSE;
#endif
	
	SITEM* pInvenData = GLItemMan::GetInstance().GetItem ( sInven_BackUp.sItemCustom.sNativeID );
	SITEM* pHoldData = GLItemMan::GetInstance().GetItem ( sItemHold.sNativeID );
	if ( !pInvenData || !pHoldData )	return S_FALSE;

	//	Note : ���� �Ҽ� �ִ� ���������� ����.
	bool bMERGE(false);
	bMERGE = (pInvenData->sBasicOp.sNativeID==pHoldData->sBasicOp.sNativeID);
	if (bMERGE)	bMERGE = ( pInvenData->ISPILE() );
	if (bMERGE) bMERGE = ( sItemHold.wTurnNum<pInvenData->sDrugOp.wPileNum );
	if (bMERGE) bMERGE = ( sInven_BackUp.sItemCustom.wTurnNum<pInvenData->sDrugOp.wPileNum );
	if ( bMERGE )
	{
		WORD wTurnNum = sInven_BackUp.sItemCustom.wTurnNum + sItemHold.wTurnNum;
		
		if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
		{
			//	Note : ��ħ �� ����.
			pInvenItem->sItemCustom.wTurnNum = wTurnNum;

			//	Note : ��� �ִ� ������ ����.
			RELEASE_HOLD_ITEM();

			//	Note : â�� ������ ������Ʈ.
			GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM NetMsgStorageUpdate;
			NetMsgStorageUpdate.dwChannel = dwChannel;
			NetMsgStorageUpdate.wPosX = pNetMsg->wPosX;
			NetMsgStorageUpdate.wPosY = pNetMsg->wPosY;
			NetMsgStorageUpdate.wTurnNum = wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorageUpdate);
		
			//	Note : Ŭ���̾�Ʈ�� ��� �ִ� ������ ����.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);
		}
		else
		{
			WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;
			
			//	���� ������ ���� ����. ( ��ä��. )
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	�տ� ��� �ִ� ������ ����. ( ���� ����. )
			sItemHold.wTurnNum = wSplit;
			HOLD_ITEM ( sItemHold );

			//	Note : �տ��� ������ ����.
			GLMSG::SNETPC_PUTON_UPDATE NetMsgPutOn;
			NetMsgPutOn.emSlot = SLOT_HOLD;
			NetMsgPutOn.sItemCustom = sItemHold;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPutOn);

			//	Note : â�� ������ ������Ʈ.
			GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM NetMsgStorageItem;
			NetMsgStorageItem.dwChannel = pNetMsg->dwChannel;
			NetMsgStorageItem.wPosX = pNetMsg->wPosX;
			NetMsgStorageItem.wPosY = pNetMsg->wPosY;
			NetMsgStorageItem.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorageItem);
		}

		return S_OK;
	}

	//	â�� �ִ� ������ ����.
	BOOL bOk = pCLUB->m_cStorage[dwChannel].DeleteItem ( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	//	Note : ���� �տ��� �����ư� ����� �������� ũ�Ⱑ Ʋ�� ���.
	//	��� �ִ� �������� �� ������ ã�´�.
	if ( !pInvenData->SAME_INVENSIZE ( *pHoldData ) )
	{
		//	������ġ�� ������ �ִ��� �˻�.
		BOOL bOk = pCLUB->m_cStorage[dwChannel].IsInsertable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )
		{
			//	����ִ� ������ �ι꿡 ���� ������ �ִ��� �˻�.
			bOk = pCLUB->m_cStorage[dwChannel].FindInsrtable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wPosX, wPosY );
			if ( !bOk )
			{
				//	roll-back : �ι꿡�� �����ߴ� �������� �ٽ� ����.
				pCLUB->m_cStorage[dwChannel].InsertItem ( sInven_BackUp.sItemCustom, sInven_BackUp.wPosX, sInven_BackUp.wPosY );

				return S_FALSE;
			}
		}
	}

	//	����ִ� ������ �ι꿡 ����.
	pCLUB->m_cStorage[dwChannel].InsertItem ( sItemHold, wPosX, wPosY );
	SINVENITEM *pINSERTITEM = pCLUB->m_cStorage[dwChannel].GetItem(wPosX,wPosY);
	if ( !pINSERTITEM )
	{
		CDebugSet::ToLogFile ( "error MsgReqStorageExHold(), sItemHold, id(%d,%d) to pos(%d,%d)",
			sItemHold.sNativeID.wMainID, sItemHold.sNativeID.wSubID, wPosX, wPosY );
		return S_FALSE;
	}

	//	Note :�������� ���� ���� ��� ���.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sItemHold, ID_CHAR, m_dwCharID, ID_CLUB, m_dwGuild, EMITEM_ROUTE_CLUB, sItemHold.wTurnNum );

	//	�ι꿡 �־��� �������� �տ� ��.
	HOLD_ITEM ( sInven_BackUp.sItemCustom );

	//	Note :�������� ���� ���� ��� ���.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sInven_BackUp.sItemCustom, ID_CLUB, m_dwGuild, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sInven_BackUp.sItemCustom.wTurnNum );

	//	[�ڽſ���] �ι꿡 �ִ� ������ ����, �� ������ ����.
	GLMSG::SNET_CLUB_STORAGE_DEL_AND_INS NetMsg_Del_Insert;
	NetMsg_Del_Insert.dwChannel = dwChannel;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	������ ������.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	if ( pINSERTITEM )	NetMsg_Del_Insert.sInsert = *pINSERTITEM;	//	���ԵǴ� ������.
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Del_Insert);

	//	[�ڽſ���] �ι꿡 �־��� �������� �տ� ��.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = sInven_BackUp.sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	return S_OK;
}

HRESULT GLChar::MsgReqClubHoldToStorage ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_HOLD_TO_STORAGE *pNetMsg = (GLMSG::SNET_CLUB_HOLD_TO_STORAGE *) nmg;

	GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->m_dwMasterID != m_dwCharID )
	{
		return S_OK;
	}

	if ( pNetMsg->dwChannel >= MAX_CLUBSTORAGE )
	{
		return E_FAIL;
	}

	if ( !IsKEEP_CLUB_STORAGE(pNetMsg->dwChannel) )
	{
		return S_FALSE;
	}

	const DWORD dwChannel = pNetMsg->dwChannel;

	if ( !VALID_HOLD_ITEM() )
	{
		return S_FALSE;
	}

	SITEM* pHoldData = GLItemMan::GetInstance().GetItem ( GET_HOLD_ITEM().sNativeID );
	if ( !pHoldData )
	{
		return E_FAIL;
	}

	//	�ŷ��ɼ�
	if ( !pHoldData->sBasicOp.IsEXCHANGE() )		return FALSE;

#if defined(VN_PARAM) //vietnamtest%%%
	if ( GET_HOLD_ITEM().bVietnamGainItem  )	return FALSE;
#endif

	BOOL bOk = pCLUB->m_cStorage[dwChannel].IsInsertable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !bOk )
	{
		//	�ι��� ��������.
		return E_FAIL;
	}

	//	����ִ� ������ �ι꿡 ����.
	pCLUB->m_cStorage[dwChannel].InsertItem ( GET_HOLD_ITEM(), pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note :�������� ���� ���� ��� ���.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( GET_HOLD_ITEM(), ID_CHAR, m_dwCharID, ID_USER, m_dwUserID, EMITEM_ROUTE_CLUB, GET_HOLD_ITEM().wTurnNum );

	//	������ ������ ����.
	RELEASE_HOLD_ITEM ();

	//	[�ڽſ���] �ι꿡 ������ ����.
	GLMSG::SNET_CLUB_STORAGE_INSERT NetMsg_Inven_Insert;
	NetMsg_Inven_Insert.dwChannel = dwChannel;
	NetMsg_Inven_Insert.Data = *pCLUB->m_cStorage[dwChannel].GetItem(pNetMsg->wPosX,pNetMsg->wPosY);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Insert);

	//	[�ڽſ���] �� �־��� ������ ����.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);

	return S_OK;
}

HRESULT GLChar::MsgReqClubStorageSplit ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_STORAGE_SPLIT *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_SPLIT *)nmg;
	
	GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->m_dwMasterID != m_dwCharID )
	{
		return S_OK;
	}

	if ( pNetMsg->dwChannel >= MAX_CLUBSTORAGE )
	{
		return E_FAIL;
	}

	if ( !IsKEEP_CLUB_STORAGE(pNetMsg->dwChannel) )
	{
		return S_FALSE;
	}

	BOOL bOk(FALSE);

	GLInventory &sInventory = pCLUB->m_cStorage[pNetMsg->dwChannel];

	//	Note : �и� ��û ������ ã��.
	SINVENITEM* pInvenItem = sInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )									return E_FAIL;

	//	Note : ������ ���� ��������.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )										return E_FAIL;

	//	Note : �и� ������ ���������� ����.
	bool bSPLIT(false);
	bSPLIT = ( pItem->ISPILE() );
	if ( !bSPLIT )													return E_FAIL;
	if ( pNetMsg->wSplit < 1 )										return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= pNetMsg->wSplit  )		return E_FAIL;

	//	Note : ���� �������� ��ȭ�� ����.
	WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - pNetMsg->wSplit;

	//	Note : �и��Ǿ� ���� ����� ������.
	SITEMCUSTOM sNEW_ITEM = pInvenItem->sItemCustom;
	sNEW_ITEM.wTurnNum = pNetMsg->wSplit;

	//	Note : ���� ����� ������ ��ġ ã��.
	WORD wInvenPosX(0), wInvenPosY(0);
	bOk = sInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wInvenPosX, wInvenPosY );
	if ( !bOk )	return E_FAIL;

	//	Note : ���� ���� ������ �ֱ�.
	bOk = sInventory.InsertItem ( sNEW_ITEM, wInvenPosX, wInvenPosY );
	if ( !bOk )											return E_FAIL;

	//	Note : ���� ���� ������ ���� ��������.
	SINVENITEM* pNewItem = sInventory.GetItem ( wInvenPosX, wInvenPosY );
	if ( !bOk )											return E_FAIL;

	//	Note : ���� �������� ���� ���� ��Ŵ.
	pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

	//	Note : �κ��� ���� �и��Ǿ� ��� ���� ������.
	GLMSG::SNET_CLUB_STORAGE_INSERT NetMsgNew;
	NetMsgNew.dwChannel = pNetMsg->dwChannel;
	NetMsgNew.Data = *pNewItem;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgNew);

	//	Note : ���� ������ ���� ����.
	GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM NetMsgStorage;
	NetMsgStorage.dwChannel = pNetMsg->dwChannel;
	NetMsgStorage.wPosX = pNetMsg->wPosX;
	NetMsgStorage.wPosY = pNetMsg->wPosY;
	NetMsgStorage.wTurnNum = wORGIN_SPLIT;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorage);

	return S_OK;
}

HRESULT GLChar::MsgReqClubStorageSaveMoney ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_STORAGE_SAVE_MONEY *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_SAVE_MONEY *)nmg;

	GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( m_dwGuild );

	if ( !pCLUB )								return S_OK;
	if ( pCLUB->m_dwMasterID != m_dwCharID )	return S_OK;
	if ( !pCLUB->m_bVALID_STORAGE )				return S_OK;

	if ( pNetMsg->lnMoney < 0 )				return S_FALSE;
	if ( m_lnMoney < pNetMsg->lnMoney )		return S_FALSE;

	CheckMoneyUpdate( m_lnMoney, pNetMsg->lnMoney, FALSE, "Club Storage Save Money." );
	m_bMoneyUpdate = TRUE;

	m_lnMoney -= pNetMsg->lnMoney;
	pCLUB->m_lnStorageMoney += pNetMsg->lnMoney;

	//	Note : �ݾ� �α�.
	//
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CLUB, m_dwGuild, pNetMsg->lnMoney, EMITEM_ROUTE_CLUB );

	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CLUB, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, 0, ID_CLUB, m_dwGuild, pCLUB->m_lnStorageMoney, EMITEM_ROUTE_CLUB );

	//	Note : Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNETPC_UPDATE_MONEY NetMsg;
	NetMsg.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	Note : Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = pCLUB->m_lnStorageMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorage);

	return S_OK;
}

HRESULT GLChar::MsgReqClubStorageDrawMoney ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY *pNetMsg = (GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY *)nmg;

	GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->m_dwMasterID != m_dwCharID )
	{
		return S_OK;
	}

	if ( !pCLUB->m_bVALID_STORAGE )
	{
		return S_OK;
	}

	LONGLONG lnMoney = pNetMsg->lnMoney;
	if ( lnMoney < 0 )							return S_FALSE;
	if ( pCLUB->m_lnStorageMoney < lnMoney )	return S_FALSE;

	CheckMoneyUpdate( m_lnMoney, lnMoney, TRUE, "Club Storage Draw Money." );
	m_bMoneyUpdate = TRUE;

	m_lnMoney += lnMoney;
	pCLUB->m_lnStorageMoney -= lnMoney;

	//	Note : �ݾ� �α�.
	//
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CLUB, m_dwGuild, -lnMoney, EMITEM_ROUTE_CLUB );

	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CLUB, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, 0, ID_CLUB, m_dwGuild, pCLUB->m_lnStorageMoney, EMITEM_ROUTE_CLUB );

	//	Note : Ŭ���̾�Ʈ�� �κ��丮�� �ִ� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNETPC_UPDATE_MONEY NetMsg;
	NetMsg.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	Note : Ŭ���̾�Ʈ�� ��� â�� �ִ� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = pCLUB->m_lnStorageMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorage);

	return S_OK;
}

HRESULT GLChar::MsgReqClubInComeReNew ( NET_MSG_GENERIC* nmg )
{
	//GLMSG::SNET_CLUB_INCOME_RENEW *pNetMsg = (GLMSG::SNET_CLUB_INCOME_RENEW *)nmg;

	if ( m_dwGuild==CLUB_NULL )
	{
		return S_OK;
	}

	GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->m_dwMasterID != m_dwCharID )
	{
		return S_OK;
	}

	GLMSG::SNET_CLUB_INCOME_UP NetMsgUp;
	NetMsgUp.dwClubID = pCLUB->m_dwID;
	GLGaeaServer::GetInstance().SENDTOAGENT ( &NetMsgUp );

	return S_OK;
}
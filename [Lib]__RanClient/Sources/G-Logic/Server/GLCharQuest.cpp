#include "pch.h"
#include "./GLChar.h"
#include "./GLGaeaServer.h"
#include "./GLQuest.h"
#include "./GLQuestMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GLChar::QuestStartFromGetITEM ( SNATIVEID nidITEM )
{
	GLQuestMan::QUESTGEN_RANGE* pRANGE = GLQuestMan::GetInstance().FindStartFromGetITEM ( nidITEM );
	if ( !pRANGE )		return;

	DWORD dwQUEST = 0;
	GLQUEST* pQUEST = NULL;
	GLQUESTPROG* pQUEST_PROG = NULL;
	GLQUESTPROG* pQUEST_END = NULL;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();
	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestMan::QUESTGEN_ITER pos = pRANGE->first;
	for ( ; pos!=pRANGE->second; ++pos )
	{
		dwQUEST = (*pos).second;

		pQUEST = glQuestMan.Find ( dwQUEST );
		if ( !pQUEST )		continue;

		if ( (pQUEST->m_sSTARTOPT.dwCLASS&m_emClass)==NULL )	continue;
		if ( MAX_SCHOOL!=pQUEST->m_sSTARTOPT.wSCHOOL )
		{
			if ( pQUEST->m_sSTARTOPT.wSCHOOL!=m_wSchool )		continue;
		}

		pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUEST );
		if ( pQUEST_PROG )	continue;

		//	�̹� �Ϸ��� ����Ʈ���� �˻�.
		pQUEST_END = m_cQuestPlay.FindEnd ( dwQUEST );
		if ( pQUEST_END )
		{
			if ( pQUEST_END->m_bCOMPLETE )
			{
				//	������ �õ� �������� �˻�.
				if ( !pQUEST->IsREPEAT() )		continue;
			}
			else
			{
				//	����(����)�� ����Ʈ�� �ٽ� �õ� �Ұ����� ���.
				if ( !pQUEST->IsAGAIN() )		continue;
			}
		}

		if( !CheckQuestProgressEvent( 0, pQUEST ) ) continue;


		//	Note : ������ ���۵Ǵ� ����Ʈ ����.
		//
		GLQUESTPROG sQUEST_PROG_NEW;
		sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

		//	����Ʈ�� ���.
		m_cQuestPlay.InsertProc ( sQUEST_PROG_NEW );

		//	Note : �߰��� ����Ʈ ���� ����.
		//
		CByteStream sByteStream;
		sQUEST_PROG_NEW.SET_BYBUFFER ( sByteStream );
		
		LPBYTE pBuff(0);
		DWORD dwSize(0);
		sByteStream.GetBuffer ( pBuff, dwSize );
		
		GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew;
		NetMsgQuestNew.SETSTREAM ( pBuff, dwSize );
		glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgQuestNew);

		//	���� ������ ������ ���� ������ ����.
		if ( pQUEST_END )
		{
			//	Note : ����� ����Ʈ ����Ʈ���� ����Ʈ ���� ����.
			//
			m_cQuestPlay.DeleteEnd ( dwQUEST );

			//	Note : ������ ����Ʈ ���� ����.
			//
			GLMSG::SNET_QUEST_END_DEL	NetMsgDel;
			NetMsgDel.dwQUESTID = dwQUEST;
			glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgDel);
		}
	}
}

void GLChar::QuestStartFromGetSKILL ( SNATIVEID nidSKILL )
{
	GLQuestMan::QUESTGEN_RANGE* pRANGE = GLQuestMan::GetInstance().FindStartFromGetSKILL ( nidSKILL );
	if ( !pRANGE )		return;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();
	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestMan::QUESTGEN_ITER pos = pRANGE->first;
	for ( ; pos!=pRANGE->second; ++pos )
	{
		DWORD dwQUEST = (*pos).second;

		GLQUEST *pQUEST = glQuestMan.Find ( dwQUEST );
		if ( !pQUEST )		continue;

		if ( (pQUEST->m_sSTARTOPT.dwCLASS&m_emClass)==NULL )	continue;
		if ( MAX_SCHOOL!=pQUEST->m_sSTARTOPT.wSCHOOL )
		{
			if ( pQUEST->m_sSTARTOPT.wSCHOOL!=m_wSchool )		continue;
		}

		GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUEST );
		if ( pQUEST_PROG )	continue;

		//	�̹� �Ϸ��� ����Ʈ���� �˻�.
		GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUEST );
		if ( pQUEST_END )
		{
			if ( pQUEST_END->m_bCOMPLETE )
			{
				//	������ �õ� �������� �˻�.
				if ( !pQUEST->IsREPEAT() )		continue;
			}
			else
			{
				//	����(����)�� ����Ʈ�� �ٽ� �õ� �Ұ����� ���.
				if ( !pQUEST->IsAGAIN() )		continue;
			}
		}

		if( !CheckQuestProgressEvent( 0, pQUEST ) ) continue;

		//	Note : ������ ���۵Ǵ� ����Ʈ ����.
		//
		GLQUESTPROG sQUEST_PROG_NEW;
		sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

		//	����Ʈ�� ���.
		m_cQuestPlay.InsertProc ( sQUEST_PROG_NEW );

		//	Note : �߰��� ����Ʈ ���� ����.
		//
		CByteStream sByteStream;
		sQUEST_PROG_NEW.SET_BYBUFFER ( sByteStream );
		
		LPBYTE pBuff(0);
		DWORD dwSize(0);
		sByteStream.GetBuffer ( pBuff, dwSize );
		
		GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew;
		NetMsgQuestNew.SETSTREAM ( pBuff, dwSize );
		glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgQuestNew);

		//	���� ������ ������ ���� ������ ����.
		if ( pQUEST_END )
		{
			//	Note : ����� ����Ʈ ����Ʈ���� ����Ʈ ���� ����.
			//
			m_cQuestPlay.DeleteEnd ( dwQUEST );

			//	Note : ������ ����Ʈ ���� ����.
			//
			GLMSG::SNET_QUEST_END_DEL	NetMsgDel;
			NetMsgDel.dwQUESTID = dwQUEST;
			glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgDel);
		}
	}
}

void GLChar::QuestStartFromGetLEVEL ( WORD wLEVEL )
{
	GLQuestMan::QUESTGEN_RANGE* pRANGE = GLQuestMan::GetInstance().FindStartFromGetLEVEL ( wLEVEL );
	if ( !pRANGE )		return;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();
	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestMan::QUESTGEN_ITER pos = pRANGE->first;
	for ( ; pos!=pRANGE->second; ++pos )
	{
		DWORD dwQUEST = (*pos).second;

		GLQUEST *pQUEST = glQuestMan.Find ( dwQUEST );
		if ( !pQUEST )		continue;

		if ( (pQUEST->m_sSTARTOPT.dwCLASS&m_emClass)==NULL )	continue;
		if ( MAX_SCHOOL!=pQUEST->m_sSTARTOPT.wSCHOOL )
		{
			if ( pQUEST->m_sSTARTOPT.wSCHOOL!=m_wSchool )		continue;
		}

		GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUEST );
		if ( pQUEST_PROG )	continue;

		//	�̹� �Ϸ��� ����Ʈ���� �˻�.
		GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUEST );
		if ( pQUEST_END )
		{
			if ( pQUEST_END->m_bCOMPLETE )
			{
				//	������ �õ� �������� �˻�.
				if ( !pQUEST->IsREPEAT() )		continue;
			}
			else
			{
				//	����(����)�� ����Ʈ�� �ٽ� �õ� �Ұ����� ���.
				if ( !pQUEST->IsAGAIN() )		continue;
			}
		}

		if( !CheckQuestProgressEvent( 0, pQUEST ) ) continue;

		//	Note : ������ ���۵Ǵ� ����Ʈ ����.
		//
		GLQUESTPROG sQUEST_PROG_NEW;
		sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

		//	����Ʈ�� ���.
		m_cQuestPlay.InsertProc ( sQUEST_PROG_NEW );

		//	Note : �߰��� ����Ʈ ���� ����.
		//
		CByteStream sByteStream;
		sQUEST_PROG_NEW.SET_BYBUFFER ( sByteStream );
		
		LPBYTE pBuff(0);
		DWORD dwSize(0);
		sByteStream.GetBuffer ( pBuff, dwSize );
		
		GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew;
		NetMsgQuestNew.SETSTREAM ( pBuff, dwSize );
		glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgQuestNew);

		//	���� ������ ������ ���� ������ ����.
		if ( pQUEST_END )
		{
			//	Note : ����� ����Ʈ ����Ʈ���� ����Ʈ ���� ����.
			//
			m_cQuestPlay.DeleteEnd ( dwQUEST );

			//	Note : ������ ����Ʈ ���� ����.
			//
			GLMSG::SNET_QUEST_END_DEL	NetMsgDel;
			NetMsgDel.dwQUESTID = dwQUEST;
			glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgDel);
		}
	}
}

bool GLChar::DoQuestProgress ( GLQUESTPROG* pQUEST_PROG )
{
	GASSERT(pQUEST_PROG&&"GLChar::DoQuestProgress");
	if ( !pQUEST_PROG )		return false;

	DWORD dwQID = pQUEST_PROG->m_sNID.dwID;

	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQID );
	if ( !pQUEST )			return false;


	//	Note : ����Ʈ ����. ( ���� �������� ����� �� ����. )
	//
	bool _bModifyInven(false);
	GLQUEST_PROGRESS_MAP stepMoveMap;
	bool bPROG = pQUEST_PROG->DoPROGRESS ( _bModifyInven, stepMoveMap );
	if ( bPROG )
	{
		//	Note : ���� ���� ���� ����.
		//
		CByteStream cByteStream;
		pQUEST_PROG->m_sSTEP_PROG.SET_BYBUFFER ( cByteStream );

		LPBYTE pBuff(NULL);
		DWORD dwSize(0);
		cByteStream.GetBuffer ( pBuff, dwSize );

		GLMSG::SNET_QUEST_PROG_STEP_STREAM	NetMsgStep;
		NetMsgStep.dwQID = dwQID;
		NetMsgStep.dwQSTEP = pQUEST_PROG->m_dwSTEP;
		NetMsgStep.dwFlag = pQUEST_PROG->m_dwFlags;
		NetMsgStep.SETSTREAM ( pBuff, dwSize );

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgStep );

		if ( _bModifyInven )
		{
			CByteStream cByteStream;
			pQUEST_PROG->m_sINVENTORY.GETITEM_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			GLMSG::SNET_QUEST_PROG_INVEN NetMsgInven;
			NetMsgInven.dwQID = dwQID;
			NetMsgInven.SETSTREAM ( pBuff, dwSize );

			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgInven );
		}

		if( stepMoveMap.nidMAP.wMainID  != UINT_MAX &&
			stepMoveMap.nidMAP.wSubID   != UINT_MAX )
		{
			QuestMoveProgressEvent( stepMoveMap.nidMAP, stepMoveMap.wPosX, stepMoveMap.wPosY, stepMoveMap.dwGateID );
		}

		return true;
	}

	//	Note : ����Ʈ�� �Ϸ�̴��� �˻�.
	//
	if ( pQUEST_PROG->CheckCOMPLETE() )
	{
		if ( !CheckQuestItemReward(pQUEST) )
		{
			GLMSG::SNETPC_QUEST_COMPLETE_FB NetMsgFB;
			NetMsgFB.dwQID = dwQID;
			NetMsgFB.emFB = EMQUEST_COMPLETE_FB_NOINVEN;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
			return true;
		}

		

		pQUEST_PROG->DoCOMPLETE();

		//	�Ϸ�� ����Ʈ�� �߰�.
		m_cQuestPlay.InsertEnd ( *pQUEST_PROG );

		CByteStream cByteStream;
		pQUEST_PROG->SET_BYBUFFER ( cByteStream );

		LPBYTE pBuff(NULL);
		DWORD dwSize(0);
		cByteStream.GetBuffer ( pBuff, dwSize );

		//	�������� ����.
		m_cQuestPlay.DeleteProc ( dwQID );

		//	Note : ����Ʈ �Ϸ� ���� ����.
		//
		GLMSG::SNET_QUEST_END_STREAM NetMsgStream;
		NetMsgStream.SETSTREAM ( pBuff, dwSize );
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgStream );

		//	Note : ����Ʈ ���� ���� ���� ����.
		//
		GLMSG::SNET_QUEST_PROG_DEL NetMsgDel;
		NetMsgDel.dwQUESTID = dwQID;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgDel );

		if ( !CheckQuestProgressEvent( 1, pQUEST ) )
		{
			GLMSG::SNETPC_QUEST_COMPLETE_FB NetMsgFB;
			NetMsgFB.dwQID = dwQID;
			NetMsgFB.emFB = EMQUEST_COMPLETE_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
			return true;
		}

		//	Note : ����Ʈ�� ���� ����.
		//
		DoQuestGenerateReward ( pQUEST );

		return true;
	}

	return false;
}

//	Note : ������ �ֱ� �׽�Ʈ, ����Ʈ ���� �������� �� ������ ������ �ִ��� Ȯ��.
bool GLChar::CheckQuestItemReward ( GLQUEST* pQUEST )
{
	if ( !pQUEST )			return false;

	GLInventory cInven;
#if defined(VN_PARAM) //vietnamtest%%%
	if( m_dwVietnamGainType == GAINTYPE_EMPTY )
	{
		cInven.Assign ( m_cVietnamInventory );
	}else{
		cInven.Assign ( m_cInventory );
	}
#else
	cInven.Assign ( m_cInventory );
#endif

	GLItemMan & glItemMan = GLItemMan::GetInstance();

	INVENQARRAY_ITER iter = pQUEST->m_vecGiftITEM.begin();
	INVENQARRAY_ITER iter_end = pQUEST->m_vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sITEM_NEW = (*iter);

		SITEM* pItem = glItemMan.GetItem ( sITEM_NEW.sNativeID );
		if ( !pItem )		continue;

		BOOL bOK = cInven.InsertItem ( sITEM_NEW );
		if ( !bOK )			return false;
	}

	return true;
}

void GLChar::DoQuestGenerateReward ( GLQUEST* pQUEST )
{
	if ( !pQUEST )	return;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();
	GLItemMan & glItemMan = GLItemMan::GetInstance();
	GLITEMLMT & glITEMMT = GLITEMLMT::GetInstance();
	GLSkillMan & glSkillMan = GLSkillMan::GetInstance();
	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	//NeedToSaveValue _D

	if ( pQUEST->m_dwGiftEXP>0 )
	{

		int nMemExp = pQUEST->m_dwGiftEXP;
#if defined(VN_PARAM) //vietnamtest%%%


		if( m_dwVietnamGainType == GAINTYPE_HALF )
		{
			float fTemp = (float)nMemExp;
			fTemp /= 2.0f;
			nMemExp = (int)fTemp;
		}
		if( m_dwVietnamGainType == GAINTYPE_EMPTY )
		{
			nMemExp = 0;
		}

		if( m_dwVietnamGainType == GAINTYPE_EMPTY || m_dwVietnamGainType == GAINTYPE_HALF )
		{
			m_lVNGainSysExp += (pQUEST->m_dwGiftEXP - nMemExp);

			GLMSG::SNETPC_VIETNAM_GAIN_EXP NetMsgVNExp;
			NetMsgVNExp.gainExp = m_lVNGainSysExp;
			glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgVNExp );
		}


#endif

		m_sExperience.lnNow += nMemExp;
	
		GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
		NetMsgExp.lnNowExp = m_sExperience.lnNow;
		glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgExp );

		

		if ( m_sExperience.ISOVER() )
		{
			//	Note : ���� ����. ( ���� �޽��� �߻�. )
			GLMSG::SNETPC_REQ_LEVELUP NetMsg;
			MsgReqLevelUp ( (NET_MSG_GENERIC*) &NetMsg );
		}
	}

	if ( pQUEST->m_dwGiftMONEY>0 )
	{
		LONGLONG lnAmount = pQUEST->m_dwGiftMONEY;
#if defined(VN_PARAM) //vietnamtest%%%		

		if( m_dwVietnamGainType == GAINTYPE_HALF )
		{
			lnAmount /= 2;
			m_lVNGainSysMoney += lnAmount;
		}else if( m_dwVietnamGainType == GAINTYPE_EMPTY )
		{
			m_lVNGainSysMoney += lnAmount;
			lnAmount = 0;
		}

		if( m_dwVietnamGainType == GAINTYPE_HALF || m_dwVietnamGainType == GAINTYPE_EMPTY )
		{
			GLMSG::SNETPC_VIETNAM_GAIN_MONEY NetMsgVNMoney;
			NetMsgVNMoney.gainMoney = m_lVNGainSysMoney;
			glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgVNMoney );
		}

#endif

		CheckMoneyUpdate( m_lnMoney, lnAmount, TRUE, "Quest Gain Money." );
		m_bMoneyUpdate = TRUE;

		m_lnMoney += lnAmount;

		GLMSG::SNETPC_PICKUP_MONEY NetMsgMoney;
		NetMsgMoney.lnMoney = m_lnMoney;
		NetMsgMoney.lnPickUp = (LONGLONG)pQUEST->m_dwGiftMONEY;
		glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgMoney );
	}
	
	//	Note : ������ �ݺ� �Ұ����Ҷ��� ������.
	if ( pQUEST->m_dwGiftELEMENT>0 )
	{
		if ( !pQUEST->IsREPEAT() || m_nBright<0 )
		{
			m_nBright += pQUEST->m_dwGiftELEMENT;

			GLMSG::SNETPC_UPDATE_BRIGHT NetMsgBright;
			NetMsgBright.nBright = m_nBright;
			glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgBright );


			GLMSG::SNETPC_UPDATE_BRIGHT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.nBright = m_nBright;
			SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
		}
	}

	if ( pQUEST->m_dwGiftLIFEPOINT>0 )
	{
		ReceiveLivingPoint ( pQUEST->m_dwGiftLIFEPOINT );
	}

	if ( pQUEST->m_dwGiftDisPK>0 )
	{
		if ( m_wPK>pQUEST->m_dwGiftDisPK )	m_wPK -= WORD(pQUEST->m_dwGiftDisPK);
		else								m_wPK = 0;
	}

	if ( pQUEST->m_dwGiftSKILLPOINT>0 )
	{
		m_dwSkillPoint += pQUEST->m_dwGiftSKILLPOINT;
		
		GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
		NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
		glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgSkp );
	}

	if ( pQUEST->m_dwGiftSTATSPOINT>0 )
	{
		m_wStatsPoint += (WORD)pQUEST->m_dwGiftSTATSPOINT;
		
		GLMSG::SNETPC_UPDATE_STATS NetMsgStats;
		NetMsgStats.wStatsPoint = m_wStatsPoint;
		glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgStats );
	}

	//	������ ����.
	{
		INVENQARRAY_ITER iter = pQUEST->m_vecGiftITEM.begin();
		INVENQARRAY_ITER iter_end = pQUEST->m_vecGiftITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sITEM_NEW = (*iter);

			CTime cTIME = CTime::GetCurrentTime();
			sITEM_NEW.tBORNTIME = cTIME.GetTime();

			sITEM_NEW.cGenType = EMGEN_QUEST;
			sITEM_NEW.cChnID = (BYTE)glGaeaServer.GetServerChannel();
			sITEM_NEW.cFieldID = (BYTE)glGaeaServer.GetFieldSvrID();
			sITEM_NEW.lnGenNum = glITEMMT.RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

			SITEM* pItem = glItemMan.GetItem ( sITEM_NEW.sNativeID );
			if ( !pItem )		continue;

			WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
			WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

			WORD wInsertPosX(0), wInsertPosY(0);
			BOOL bITEM_SPACE = TRUE;

#if defined(VN_PARAM) //vietnamtest%%%
			if( m_dwVietnamGainType == GAINTYPE_EMPTY )
			{
				bITEM_SPACE = m_cVietnamInventory.FindInsrtable ( wINVENX, wINVENY, wInsertPosX, wInsertPosY );
			}else
#endif
			{
				bITEM_SPACE = m_cInventory.FindInsrtable ( wINVENX, wINVENY, wInsertPosX, wInsertPosY );
			}


			if ( bITEM_SPACE )
			{
#if defined(VN_PARAM) //vietnamtest%%%
				if( m_dwVietnamGainType == GAINTYPE_EMPTY )
				{
					//	�ι��丮�� ����.
					sITEM_NEW.bVietnamGainItem = true;
					m_cVietnamInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );

					//	Note :�������� ���� ���� ��� ���.
					//
					glITEMMT.ReqItemRoute ( sITEM_NEW, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum );

					//	�ι꿡 ������ �־��ִ� �޽���.
					//
					GLMSG::SNETPC_INVEN_INSERT NetMsgInven;
					NetMsgInven.Data = *m_cVietnamInventory.GetItem ( wInsertPosX, wInsertPosY );
					NetMsgInven.bVietnamInven = TRUE;
					glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgInven);
				}else
#endif
				{
					//	�ι��丮�� ����.
					m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );

					//	Note :�������� ���� ���� ��� ���.
					//
					glITEMMT.ReqItemRoute ( sITEM_NEW, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum );

					//	�ι꿡 ������ �־��ִ� �޽���.
					//
					GLMSG::SNETPC_INVEN_INSERT NetMsgInven;
					NetMsgInven.Data = *m_cInventory.GetItem ( wInsertPosX, wInsertPosY );
					glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgInven);
				}
			}else
			{
				D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
				
				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = m_pLandMan->IsCollisionNavi
				(
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
					vCollision
				);

				if ( !bCollision )		vCollision = m_vPos;

				//	������ �߻�.
				m_pLandMan->DropItem ( vCollision, &sITEM_NEW, EMGROUP_ONE, m_dwGaeaID );
			}
		}
	}

	//	��ų ����.
	{
		DWQARRAY_ITER iter = pQUEST->m_vecGiftSKILL.begin();
		DWQARRAY_ITER iter_end = pQUEST->m_vecGiftSKILL.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SNATIVEID sNID = SNATIVEID(*iter);

			bool bLEARN = LEARN_SKILL_QUEST ( sNID );
			if ( !bLEARN )	continue;

			//	��ų ��� ����.
			GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
			NetMsgFB.skill_id = sNID;
			NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
			glGaeaServer.SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

			//	Note : passive skill �� ��� �ɸ��� ����Ǵ� �Ӽ����� Ŭ���̾�Ʈ�鿡 ����ȭ.
			//
			PGLSKILL pSkill = glSkillMan.GetData ( sNID );
			if ( !pSkill ) continue;

			if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
			{
				GLMSG::SNETPC_UPDATE_PASSIVE_BRD NetMsgBrd;
				NetMsgBrd.dwGaeaID = m_dwGaeaID;
				NetMsgBrd.sSKILL_DATA = m_sSUM_PASSIVE;
				SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
			}
		}
	}

	if ( pQUEST->m_dwGiftQUEST!=UINT_MAX )
	{
		do
		{
			DWORD dwQUEST = pQUEST->m_dwGiftQUEST;

			GLQUEST *pQUEST = glQuestMan.Find ( dwQUEST );
			if ( !pQUEST )		continue;

			//	Memo :	�־����� ����Ʈ�� Ŭ���� ���ǿ� �´��� �˻�.
			if ( (pQUEST->m_sSTARTOPT.dwCLASS&m_emClass)==NULL ) continue;

			GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUEST );
			if ( pQUEST_PROG )	continue;

			//	�̹� �Ϸ��� ����Ʈ���� �˻�.
			GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUEST );
			if ( pQUEST_END )
			{
				if ( pQUEST_END->m_bCOMPLETE )
				{
					//	������ �õ� �������� �˻�.
					if ( !pQUEST->IsREPEAT() )		continue;
				}
				else
				{
					//	����(����)�� ����Ʈ�� �ٽ� �õ� �Ұ����� ���.
					if ( !pQUEST->IsAGAIN() )		continue;
				}
			}

			if( !CheckQuestProgressEvent( 0, pQUEST ) ) continue;

			//	Note : ������ ���۵Ǵ� ����Ʈ ����.
			//
			GLQUESTPROG sQUEST_PROG_NEW;
			sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

			//	����Ʈ�� ���.
			m_cQuestPlay.InsertProc ( sQUEST_PROG_NEW );

			//	Note : �߰��� ����Ʈ ���� ����.
			//
			CByteStream sByteStream;
			sQUEST_PROG_NEW.SET_BYBUFFER ( sByteStream );
			
			LPBYTE pBuff(0);
			DWORD dwSize(0);
			sByteStream.GetBuffer ( pBuff, dwSize );
			
			GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew;
			NetMsgQuestNew.SETSTREAM ( pBuff, dwSize );
			glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgQuestNew);

			//	���� ������ ������ ���� ������ ����.
			if ( pQUEST_END )
			{
				//	Note : ����� ����Ʈ ����Ʈ���� ����Ʈ ���� ����.
				//
				m_cQuestPlay.DeleteEnd ( dwQUEST );

				//	Note : ������ ����Ʈ ���� ����.
				//
				GLMSG::SNET_QUEST_END_DEL	NetMsgDel;
				NetMsgDel.dwQUESTID = dwQUEST;
				glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgDel);
			}

		} while(0);
	}
}

void GLChar::DoQuestMobKill ( SNATIVEID nidMOB )
{
	HRESULT hr(S_OK);

	bool bPROG(false);
	bool bMOBKILL(false);
	bool bMOBGEN(false);

	GLQUESTPROG* pPROG = NULL;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();
	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST &mapPROC = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = mapPROC.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = mapPROC.end();
	for ( ; iter!=iter_end; ++iter )
	{
		pPROG = (*iter).second;
		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if ( !pQUEST )								continue;

		pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )								continue;

		if ( !pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL )
		{
			if ( nidMOB.dwID == pSTEP->m_dwNID_MOBKILL )
			{
				//	kill �� ����.
				bMOBKILL = true;
				bPROG = true;

				//	�����Ͽ����� �˻�.
				if( pQUEST->IsPARTYQUEST() )
				{
					GLPartyFieldMan& sPartyFieldMan = m_pGLGaeaServer->GetPartyMan();
					GLPARTY_FIELD *pParty = sPartyFieldMan.GetParty ( m_dwPartyID );
					if ( pParty )
					{
						GLQUESTPROG* pMemberPROG = NULL;
						GLQUEST* pMemberQUEST = NULL;
						GLQUEST_STEP* pMemberSTEP = NULL;

						GLPARTY_FIELD::MEMBER_ITER iter_member = pParty->m_cMEMBER.begin();
						GLPARTY_FIELD::MEMBER_ITER iter_member_end = pParty->m_cMEMBER.end();

						for( ; iter_member != iter_member_end; ++iter_member )
						{
							PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( (*iter_member).first ); 
							if( !pChar )
								continue;

							GLQuestPlay::MAPQUEST &mapMemberPROC =  pChar->m_cQuestPlay.GetQuestProc();
							GLQuestPlay::MAPQUEST_ITER iter_PROG = mapMemberPROC.begin();
							GLQuestPlay::MAPQUEST_ITER iter_PROG_end = mapMemberPROC.end();
							for ( ; iter_PROG != iter_PROG_end; ++iter_PROG )
							{
								pMemberPROG = (*iter_PROG).second;
								pMemberQUEST = glQuestMan.Find ( pMemberPROG->m_sNID.dwID );
								if ( !pMemberQUEST )
									continue;

								pMemberSTEP = pMemberQUEST->GetSTEP ( pMemberPROG->m_dwSTEP );
								if ( !pMemberSTEP )								
									continue;

								if( pPROG->m_sNID != pMemberPROG->m_sNID || 
									pPROG->m_dwSTEP != pMemberPROG->m_dwSTEP )
									continue;

								if ( !pMemberPROG->m_sSTEP_PROG.m_bMOBKILL_ALL )
								{
									if ( nidMOB.dwID == pMemberSTEP->m_dwNID_MOBKILL )
									{
										++pMemberPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL;
									}
									else
									{
										continue;
									}

									if( pMemberPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL >= pSTEP->m_dwNUM_MOBKILL )
									{
										pMemberPROG->m_sSTEP_PROG.m_bMOBKILL_ALL = true;
									}

									GLMSG::SNET_QUEST_PROG_MOBKILL NetMsgMobKill;
									NetMsgMobKill.dwQID = pMemberPROG->m_sNID.dwID;
									NetMsgMobKill.dwQSTEP = pMemberPROG->m_dwSTEP;
									NetMsgMobKill.bMOBKILL_ALL = pMemberPROG->m_sSTEP_PROG.m_bMOBKILL_ALL;
									NetMsgMobKill.dwMOBKILL = pMemberPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL;
									glGaeaServer.SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgMobKill );

									if( pMemberPROG->m_sSTEP_PROG.m_bMOBKILL_ALL )
									{
										//�ٸ� ��Ƽ������ ���⼭ �Ϸ�޼����� �������ְ� ó��
										if( m_dwClientID == pChar->m_dwClientID )
											continue;

										GLMSG::SNET_QUEST_PARTY_PROG_MOBKILL NetMsgPartyMobKill;
										NetMsgPartyMobKill.dwQID = pMemberPROG->m_sNID.dwID;
										NetMsgPartyMobKill.dwQSTEP = pMemberPROG->m_dwSTEP;
										NetMsgPartyMobKill.bMOBKILL_ALL = pMemberPROG->m_sSTEP_PROG.m_bMOBKILL_ALL;
										glGaeaServer.SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgPartyMobKill );
									}
								}
							}		
						}
					}
					else
					{
						++pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL;

						if ( pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL >= pSTEP->m_dwNUM_MOBKILL )
						{
							pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL = true;
						}

						GLMSG::SNET_QUEST_PROG_MOBKILL NetMsgMobKill;
						NetMsgMobKill.dwQID = pPROG->m_sNID.dwID;
						NetMsgMobKill.dwQSTEP = pPROG->m_dwSTEP;
						NetMsgMobKill.bMOBKILL_ALL = pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL;
						NetMsgMobKill.dwMOBKILL = pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL;

						glGaeaServer.SENDTOCLIENT ( m_dwClientID, &NetMsgMobKill );
					}
				}
				else
				{
					++pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL;

					if ( pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL >= pSTEP->m_dwNUM_MOBKILL )
					{
						pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL = true;
					}

						GLMSG::SNET_QUEST_PROG_MOBKILL NetMsgMobKill;
						NetMsgMobKill.dwQID = pPROG->m_sNID.dwID;
						NetMsgMobKill.dwQSTEP = pPROG->m_dwSTEP;
						NetMsgMobKill.bMOBKILL_ALL = pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL;
						NetMsgMobKill.dwMOBKILL = pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL;

						glGaeaServer.SENDTOCLIENT ( m_dwClientID, &NetMsgMobKill );
				}
			}
		}

		if ( !pPROG->m_sSTEP_PROG.m_bQITEM )
		{
			GENMOBITEMARRAY_ITER iter = pSTEP->m_vecMOBGEN_QITEM.begin();
			GENMOBITEMARRAY_ITER iter_end = pSTEP->m_vecMOBGEN_QITEM.end();
			for ( ; iter!=iter_end; ++iter )
			{
				SGENQUESTITEM &sGENQITEM = (*iter);

				bool bFOUND = sGENQITEM.Find ( nidMOB.dwID );
				if ( !bFOUND )	continue;

				bMOBGEN = true;

				//	�̹� �ش� �������� ��� �߻��ߴ��� �˻�.
				SINVENITEM *pINVEN = pPROG->m_sINVENTORY.FindItem ( sGENQITEM.sNID );
				if ( pINVEN && pINVEN->sItemCustom.wTurnNum>=sGENQITEM.wNUM )	
					continue;

				//	�߻������� (Ȯ��)�� �߻�.
				if ( RANDOM_GEN(sGENQITEM.fGEN_RATE) )
				{
					if( pQUEST->IsPARTYQUEST() )
					{
						GLPartyFieldMan& sPartyFieldMan = m_pGLGaeaServer->GetPartyMan();
						GLPARTY_FIELD *pParty = sPartyFieldMan.GetParty ( m_dwPartyID );
						if ( pParty )
						{
							GLQUESTPROG* pMemberPROG = NULL;
							GLQUEST* pMemberQUEST = NULL;
							GLQUEST_STEP* pMemberSTEP = NULL;

							GLPARTY_FIELD::MEMBER_ITER iter_member = pParty->m_cMEMBER.begin();
							GLPARTY_FIELD::MEMBER_ITER iter_member_end = pParty->m_cMEMBER.end();

							for( ; iter_member != iter_member_end; ++iter_member )
							{
								PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( (*iter_member).first ); 
								if( !pChar )
									continue;
								
								GLQuestPlay::MAPQUEST &mapMemberPROC =  pChar->m_cQuestPlay.GetQuestProc();
								GLQuestPlay::MAPQUEST_ITER iter_PROG = mapMemberPROC.begin();
								GLQuestPlay::MAPQUEST_ITER iter_PROG_end = mapMemberPROC.end();
								for ( ; iter_PROG != iter_PROG_end; ++iter_PROG )
								{
									pMemberPROG = (*iter_PROG).second;
									pMemberQUEST = glQuestMan.Find ( pMemberPROG->m_sNID.dwID );
									if ( !pMemberQUEST )
										continue;

									pMemberSTEP = pMemberQUEST->GetSTEP ( pMemberPROG->m_dwSTEP );
									if ( !pMemberSTEP )								
										continue;

									if( pPROG->m_sNID != pMemberPROG->m_sNID || 
										pPROG->m_dwSTEP != pMemberPROG->m_dwSTEP )
										continue;

									SINVENITEM *pINVEN = pMemberPROG->m_sINVENTORY.FindItem ( sGENQITEM.sNID );
									if ( pINVEN && pINVEN->sItemCustom.wTurnNum>=sGENQITEM.wNUM )	
										continue;

									SITEMCUSTOM sCUSTOM;
									sCUSTOM.sNativeID = sGENQITEM.sNID;
									hr = InsertToQuestInven ( pChar->m_dwClientID, pMemberPROG->m_sNID.dwID, pMemberPROG->m_sINVENTORY, sCUSTOM );
									if ( FAILED(hr) )
									{
										//	����Ʈ �κ��� �ֱⰡ ������.
									}
									else
									{
										bPROG = true;

										//	������ Ŭ���̾�Ʈ�� �˸�.
										GLMSG::SNETPC_QUEST_PROG_INVEN_PICKUP NetMsgPickUp;
										NetMsgPickUp.sNID_ITEM = sCUSTOM.sNativeID;
										glGaeaServer.SENDTOCLIENT( pChar->m_dwClientID, &NetMsgPickUp );
									}
								}
							}

						}
						else
						{
							SITEMCUSTOM sCUSTOM;
							sCUSTOM.sNativeID = sGENQITEM.sNID;
							hr = InsertToQuestInven ( m_dwClientID, pPROG->m_sNID.dwID, pPROG->m_sINVENTORY, sCUSTOM );
							if ( FAILED(hr) )
							{
								//	����Ʈ �κ��� �ֱⰡ ������.
							}
							else
							{
								bPROG = true;

								//	������ Ŭ���̾�Ʈ�� �˸�.
								GLMSG::SNETPC_QUEST_PROG_INVEN_PICKUP NetMsgPickUp;
								NetMsgPickUp.sNID_ITEM = sCUSTOM.sNativeID;
								glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgPickUp);
							}
						}
					}
					else
					{
						SITEMCUSTOM sCUSTOM;
						sCUSTOM.sNativeID = sGENQITEM.sNID;
						hr = InsertToQuestInven ( m_dwClientID, pPROG->m_sNID.dwID, pPROG->m_sINVENTORY, sCUSTOM );
						if ( FAILED(hr) )
						{
							//	����Ʈ �κ��� �ֱⰡ ������.
						}
						else
						{
							bPROG = true;

							//	������ Ŭ���̾�Ʈ�� �˸�.
							GLMSG::SNETPC_QUEST_PROG_INVEN_PICKUP NetMsgPickUp;
							NetMsgPickUp.sNID_ITEM = sCUSTOM.sNativeID;
							glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgPickUp);
						}
					}					
				}
			}

			if ( bMOBGEN )
			{
				if( pQUEST->IsPARTYQUEST() )
				{
					GLPartyFieldMan& sPartyFieldMan = m_pGLGaeaServer->GetPartyMan();
					GLPARTY_FIELD *pParty = sPartyFieldMan.GetParty ( m_dwPartyID );
					if ( pParty )
					{
						GLQUESTPROG* pMemberPROG = NULL;
						GLQUEST* pMemberQUEST = NULL;
						GLQUEST_STEP* pMemberSTEP = NULL;

						GLPARTY_FIELD::MEMBER_ITER iter_member = pParty->m_cMEMBER.begin();
						GLPARTY_FIELD::MEMBER_ITER iter_member_end = pParty->m_cMEMBER.end();

						for( ; iter_member != iter_member_end; ++iter_member )
						{
							PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( (*iter_member).first ); 
							if( !pChar )
								continue;

							GLQuestPlay::MAPQUEST &mapMemberPROC =  pChar->m_cQuestPlay.GetQuestProc();
							GLQuestPlay::MAPQUEST_ITER iter_PROG = mapMemberPROC.begin();
							GLQuestPlay::MAPQUEST_ITER iter_PROG_end = mapMemberPROC.end();
							for ( ; iter_PROG != iter_PROG_end; ++iter_PROG )
							{
								pMemberPROG = (*iter_PROG).second;
								pMemberQUEST = glQuestMan.Find ( pMemberPROG->m_sNID.dwID );
								if ( !pMemberQUEST )
									continue;

								pMemberSTEP = pMemberQUEST->GetSTEP ( pMemberPROG->m_dwSTEP );
								if ( !pMemberSTEP )								
									continue;

								if( pPROG->m_sNID != pMemberPROG->m_sNID || 
									pPROG->m_dwSTEP != pMemberPROG->m_dwSTEP )
									continue;

								bool bGEMITEM_COMPLETE(true);
								GLInventory sTEMP_INVEN;
								sTEMP_INVEN.Assign ( pMemberPROG->m_sINVENTORY );

								GENMOBITEMARRAY_ITER iter = pMemberSTEP->m_vecMOBGEN_QITEM.begin();
								GENMOBITEMARRAY_ITER iter_end = pMemberSTEP->m_vecMOBGEN_QITEM.end();
								for ( ; iter!=iter_end; ++iter )
								{
									SGENQUESTITEM &sGENQITEM = (*iter);

									if ( sGENQITEM.wNUM > 0 )
									{
										DWORD dwNUM = sTEMP_INVEN.CountTurnItem ( sGENQITEM.sNID );
										if ( dwNUM<sGENQITEM.wNUM )
										{
											bGEMITEM_COMPLETE = false;
											break;
										}
									}
									else
									{
										SINVENITEM* pINVENITEM = sTEMP_INVEN.FindItem ( sGENQITEM.sNID );
										if ( !pINVENITEM )
										{
											bGEMITEM_COMPLETE = false;
											break;
										}

										sTEMP_INVEN.DeleteItem ( pINVENITEM->wPosX, pINVENITEM->wPosY );
									}
								}

								if ( bGEMITEM_COMPLETE )
								{
									pMemberPROG->m_sSTEP_PROG.m_bQITEM = true;

									GLMSG::SNET_QUEST_PROG_QITEM NetMsg;
									NetMsg.dwQID = pMemberPROG->m_sNID.dwID;
									NetMsg.dwQSTEP = pMemberPROG->m_dwSTEP;
									NetMsg.bQITEM = pMemberPROG->m_sSTEP_PROG.m_bQITEM;
									glGaeaServer.SENDTOCLIENT( pChar->m_dwClientID, &NetMsg );
									
									if( m_dwClientID == pChar->m_dwClientID )
										continue;

									//�Ϸ�� �ٸ� ��Ƽ���� ����Ʈ ���� �޼���
									GLMSG::SNET_QUEST_PARTY_PROG_QITEM NetPartyMsg;
									NetPartyMsg.dwQID = pMemberPROG->m_sNID.dwID;
									NetPartyMsg.dwQSTEP = pMemberPROG->m_dwSTEP;
									NetPartyMsg.bQITEM = pMemberPROG->m_sSTEP_PROG.m_bQITEM;
									glGaeaServer.SENDTOCLIENT( pChar->m_dwClientID, &NetPartyMsg );
								}
							}
						}
					}
					else
					{
						bool bGEMITEM_COMPLETE(true);

						GLInventory sTEMP_INVEN;
						sTEMP_INVEN.Assign ( pPROG->m_sINVENTORY );

						GENMOBITEMARRAY_ITER iter = pSTEP->m_vecMOBGEN_QITEM.begin();
						GENMOBITEMARRAY_ITER iter_end = pSTEP->m_vecMOBGEN_QITEM.end();
						for ( ; iter!=iter_end; ++iter )
						{
							SGENQUESTITEM &sGENQITEM = (*iter);

							if ( sGENQITEM.wNUM > 0 )
							{
								DWORD dwNUM = sTEMP_INVEN.CountTurnItem ( sGENQITEM.sNID );
								if ( dwNUM<sGENQITEM.wNUM )
								{
									bGEMITEM_COMPLETE = false;
									break;
								}
							}
							else
							{
								SINVENITEM* pINVENITEM = sTEMP_INVEN.FindItem ( sGENQITEM.sNID );
								if ( !pINVENITEM )
								{
									bGEMITEM_COMPLETE = false;
									break;
								}

								sTEMP_INVEN.DeleteItem ( pINVENITEM->wPosX, pINVENITEM->wPosY );
							}
						}

						if ( bGEMITEM_COMPLETE )
						{
							pPROG->m_sSTEP_PROG.m_bQITEM = true;

							GLMSG::SNET_QUEST_PROG_QITEM NetMsg;
							NetMsg.dwQID = pPROG->m_sNID.dwID;
							NetMsg.dwQSTEP = pPROG->m_dwSTEP;
							NetMsg.bQITEM = pPROG->m_sSTEP_PROG.m_bQITEM;
							glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsg);
						}

					}

				}
				else
				{
					bool bGEMITEM_COMPLETE(true);

					GLInventory sTEMP_INVEN;
					sTEMP_INVEN.Assign ( pPROG->m_sINVENTORY );

					GENMOBITEMARRAY_ITER iter = pSTEP->m_vecMOBGEN_QITEM.begin();
					GENMOBITEMARRAY_ITER iter_end = pSTEP->m_vecMOBGEN_QITEM.end();
					for ( ; iter!=iter_end; ++iter )
					{
						SGENQUESTITEM &sGENQITEM = (*iter);

						if ( sGENQITEM.wNUM > 0 )
						{
							DWORD dwNUM = sTEMP_INVEN.CountTurnItem ( sGENQITEM.sNID );
							if ( dwNUM<sGENQITEM.wNUM )
							{
								bGEMITEM_COMPLETE = false;
								break;
							}
						}
						else
						{
							SINVENITEM* pINVENITEM = sTEMP_INVEN.FindItem ( sGENQITEM.sNID );
							if ( !pINVENITEM )
							{
								bGEMITEM_COMPLETE = false;
								break;
							}

							sTEMP_INVEN.DeleteItem ( pINVENITEM->wPosX, pINVENITEM->wPosY );
						}
					}

					if ( bGEMITEM_COMPLETE )
					{
						pPROG->m_sSTEP_PROG.m_bQITEM = true;

						GLMSG::SNET_QUEST_PROG_QITEM NetMsg;
						NetMsg.dwQID = pPROG->m_sNID.dwID;
						NetMsg.dwQSTEP = pPROG->m_dwSTEP;
						NetMsg.bQITEM = pPROG->m_sSTEP_PROG.m_bQITEM;

						glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsg);
					}				
				}
			}
		}

		if ( bPROG )
		{
			//	Note : ����Ʈ ����.
			//
			bool bPROG = DoQuestProgress ( pPROG );
			if ( bPROG )
			{
				pPROG = NULL;
				break;
			}
		}
	}
}

void GLChar::DoQuestReachZone ()
{
	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();
	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST &mapPROC = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = mapPROC.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = mapPROC.end();
	for ( ; iter!=iter_end; ++iter )
	{
		GLQUESTPROG* pPROG = (*iter).second;
		if ( pPROG->m_sSTEP_PROG.m_bREACH_ZONE )				continue;

		GLQUEST *pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if ( !pQUEST )											continue;

		GLQUEST_STEP* pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( pSTEP->m_sMAPID_REACH!=m_pLandMan->GetMapID() )	continue;

		int nPosX(0);
		int nPosY(0);
		m_pLandMan->GetMapAxisInfo().Convert2MapPos ( m_vPos.x, m_vPos.z, nPosX, nPosY );

		WORD wMINX_REACH(0), wMINY_REACH(0);
		WORD wMAXX_REACH(0), wMAXY_REACH(0);
		if ( pSTEP->m_wPOSX_REACH > pSTEP->m_wRADIUS_REACH )	wMINX_REACH = pSTEP->m_wPOSX_REACH - pSTEP->m_wRADIUS_REACH;
		else													wMINX_REACH = 0;

		if ( pSTEP->m_wPOSY_REACH > pSTEP->m_wRADIUS_REACH )	wMINY_REACH = pSTEP->m_wPOSY_REACH - pSTEP->m_wRADIUS_REACH;
		else													wMINY_REACH = 0;

		wMAXX_REACH = pSTEP->m_wPOSX_REACH + pSTEP->m_wRADIUS_REACH;
		wMAXY_REACH = pSTEP->m_wPOSY_REACH + pSTEP->m_wRADIUS_REACH;

		if ( wMINX_REACH < nPosX && nPosX < wMAXX_REACH &&
			wMINY_REACH < nPosY && nPosY < wMAXY_REACH )
		{
			pPROG->m_sSTEP_PROG.m_bREACH_ZONE = true;
			
			GLMSG::SNET_QUEST_PROG_REACHZONE NetMsg;
			NetMsg.dwQID = pPROG->m_sNID.dwID;
			NetMsg.dwQSTEP = pPROG->m_dwSTEP;
			NetMsg.bREACHZONE = pPROG->m_sSTEP_PROG.m_bREACH_ZONE;
			glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsg);

			//	Note : ����Ʈ ����.
			//
			bool bPROG = DoQuestProgress ( pPROG );
			if ( bPROG )
			{
				pPROG = NULL;
				break;
			}
		}
	}
}

inline bool GLChar::IsInsertToQuestInven ( GLInventory &_sINVENTORY, SITEMCUSTOM &sCUSTOM )
{
	SITEM *pItem = GLItemMan::GetInstance().GetItem(sCUSTOM.sNativeID);
	if ( !pItem )		return false;

	if ( pItem->ISPILE() )
	{
		WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
		WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

		//	��ħ �������� ���.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	�ֱ� ��û�� �����ۼ�. ( �ܿ���. )
		WORD wREQINSRTNUM = ( sCUSTOM.wTurnNum );

		BOOL bITEM_SPACE = _sINVENTORY.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM, wINVENX, wINVENY );
		if ( !bITEM_SPACE )		return false;
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = _sINVENTORY.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )				return false;
	}

	return true;
}

HRESULT GLChar::InsertToQuestInven ( DWORD dwClientID, DWORD dwQID, GLInventory &_sINVENTORY, SITEMCUSTOM &sCUSTOM )
{
	SITEM *pItem = GLItemMan::GetInstance().GetItem(sCUSTOM.sNativeID);
	if ( !pItem )		return E_FAIL;

	bool bITEM_SPACE = IsInsertToQuestInven ( _sINVENTORY, sCUSTOM );
	if ( !bITEM_SPACE )
	{
		//	�ι��� �������� �������� ������ �����ϴ�.
		return E_FAIL;
	}

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	if ( pItem->ISPILE() )
	{
		WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
		WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

		//	��ħ �������� ���.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	�ֱ� ��û�� �����ۼ�. ( �ܿ���. )
		WORD wREQINSRTNUM = ( sCUSTOM.wTurnNum );

		//	Note : �� �������� �ִ� �������� �ִ��� �˻��Ͽ� �κ��� �����ϴ�
		//		������ ���� ���ڸ� ���ҽ�Ŵ.
		GLInventory::CELL_MAP &ItemMap = *_sINVENTORY.GetItemList();
		GLInventory::CELL_MAP_ITER iter = ItemMap.begin();
		for ( ; iter!=ItemMap.end(); ++iter )
		{
			SINVENITEM &sINVENITEM = *(*iter).second;
			SITEMCUSTOM &sITEMCUSTOM = sINVENITEM.sItemCustom;
			if ( sITEMCUSTOM.sNativeID != sNID )		continue;
			if ( sITEMCUSTOM.wTurnNum>=wPILENUM )		continue;
		
			//	���� ������ ��ħ ������.
			WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

			if ( wREQINSRTNUM > wSURPLUSNUM )
			{
				//	Note : ���� �������� ��ħ�� ����. ( ���� )
				sITEMCUSTOM.wTurnNum = wPILENUM;

				//	Note : Ŭ���̾�Ʈ�� ���� ������ �˸�.
				GLMSG::SNETPC_QUEST_PROG_INVEN_TURNNUM NetMsg;
				NetMsg.dwQID = dwQID;
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				glGaeaServer.SENDTOCLIENT( dwClientID, &NetMsg );

				//	Note : �ܿ� ���� ����.
				wREQINSRTNUM -= wSURPLUSNUM;
			}
			else
			{
				//	Note : ���� �����ۿ� ��ġ�� �� �����ε� ���� ���� �������� ����� ��.
				sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

				//	Note : Ŭ���̾�Ʈ�� ���� ������ �˸�.
				GLMSG::SNETPC_QUEST_PROG_INVEN_TURNNUM NetMsg;
				NetMsg.dwQID = dwQID;
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				glGaeaServer.SENDTOCLIENT( dwClientID,&NetMsg );

				return S_OK;
			}
		}

		//	Note : ������ �κ��� ������ �������� ���� �ľ��� �κ��� �� ������ �ִ��� �˻�.

		//	�ִ��ħ������ ����.
		WORD wONENUM = wREQINSRTNUM / wPILENUM;
		WORD wITEMNUM = wONENUM;

		//	���а�ħ�������� ��ħ��.
		WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
		if ( wSPLITNUM > 0 )				wITEMNUM += 1;
		if ( wSPLITNUM==0 && wITEMNUM>=1 )	wSPLITNUM = wPILENUM;

		for ( WORD i=0; i<wITEMNUM; ++i )
		{
			WORD wInsertPosX(0), wInsertPosY(0);
			BOOL bSPACE = _sINVENTORY.FindInsrtable ( wINVENX, wINVENY, wInsertPosX, wInsertPosY );
			GASSERT(bSPACE&&"���� ������ �̸� üũ�� �ϰ� �������� �־����� ������ ������.");
			if ( !bSPACE )			return E_FAIL;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

			//	Note : ���ο� �������� �־���.
			//
			SITEMCUSTOM sITEMCUSTOM = sCUSTOM;
			if ( wITEMNUM==(i+1) )	sITEMCUSTOM.wTurnNum = wSPLITNUM;	//	������ �������� �ܿ���.
			else					sITEMCUSTOM.wTurnNum = wPILENUM;	//	�ƴ� ���� ������.

			//	�ι��丮�� �ֽ��ϴ�.
			_sINVENTORY.InsertItem ( sITEMCUSTOM, wInsertPosX, wInsertPosY );
			SINVENITEM *pInvenItem = _sINVENTORY.GetItem ( wInsertPosX, wInsertPosY );
			if ( !pInvenItem )		return E_FAIL;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

			//	�ι꿡 ������ �־��ִ� �޽���.
			GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT NetMsgInven;
			NetMsgInven.dwQID = dwQID;
			NetMsgInven.Data = *pInvenItem;
			glGaeaServer.SENDTOCLIENT( dwClientID, &NetMsgInven );
		}
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = _sINVENTORY.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )			return E_FAIL;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

		//	Note : ������ �ֱ�.
		_sINVENTORY.InsertItem ( sCUSTOM, wPosX, wPosY );
		SINVENITEM *pInvenItem = _sINVENTORY.GetItem ( wPosX, wPosY );
		if ( !pInvenItem )	return E_FAIL;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

		//	[�ڽſ���] �޽��� �߻�.
		GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT NetMsg_Inven;
		NetMsg_Inven.dwQID = dwQID;
		NetMsg_Inven.Data = *pInvenItem;
		glGaeaServer.SENDTOCLIENT( dwClientID, &NetMsg_Inven );
	}

	return S_OK;
}

void GLChar::MsgSendQuestTime ()
{
	GLQUESTPROG* pQuestProg = NULL;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	GLQuestPlay::MAPQUEST_ITER iter_cur;
	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		iter_cur = iter++;

		pQuestProg = (*iter_cur).second;


		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( pQuestProg->m_sNID.dwID );
		if ( !pQUEST )	continue;

		if ( pQUEST->m_dwLimitTime<=0 )	continue;

		GLMSG::SNETPC_QUEST_PROG_TIME NetMsgTime;
		NetMsgTime.dwQID = pQuestProg->m_sNID.dwID;
		NetMsgTime.fELAPS = pQuestProg->m_fLAPSTIME;
		glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgTime);
	}
}

void GLChar::DoQuestCheckLimitTime ( float fTime, float fElapsedTime )
{
	m_fQUEST_TIMER += fElapsedTime;

	bool bTIME_UPDATE(false);
	if ( m_fQUEST_TIMER > 60.0f )
	{
		bTIME_UPDATE = true;
		m_fQUEST_TIMER = 0;
	}

	if ( !bTIME_UPDATE )	return;

	GLMSG::SNETPC_QUEST_PROG_TIME NetMsgTime;
	GLQUESTPROG* pQuestProg = NULL;
	DWORD dwQID = 0;
	bool bOVER = false;

	m_cQuestPlay.FrameMove ( fTime, fElapsedTime );

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	GLQuestPlay::MAPQUEST_ITER iter_cur;
	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		iter_cur = iter++;

		pQuestProg = (*iter_cur).second;
		dwQID = pQuestProg->m_sNID.dwID;

		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQID  );
		if ( !pQUEST )					continue;

		if ( pQUEST->m_dwLimitTime<=0 )	continue;

		NetMsgTime.dwQID = dwQID;
		NetMsgTime.fELAPS = pQuestProg->m_fLAPSTIME;
		glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsgTime);
        
		bOVER = pQuestProg->CheckTIMEOVER();
		if ( bOVER )
		{
			//	�ð� �ʰ��� �˸�.
			GLMSG::SNETPC_QUEST_PROG_TIMEOVER NetMsg;
			NetMsg.dwQID = dwQID;
			glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsg);

			if( pQUEST->m_ProgressEvent.bUseProgressEvent && pQUEST->m_ProgressEvent.IsTIMEOVER() )
				CheckQuestProgressEvent( 2, pQUEST );

			//	�ڵ� ���� ó��.
			pQuestProg->DoGIVEUP ();

			//	�Ϸ�� ����Ʈ�� �߰�.
			m_cQuestPlay.InsertEnd ( *pQuestProg );

			CByteStream cByteStream;
			pQuestProg->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			//	�������� ����.
			m_cQuestPlay.DeleteProc ( dwQID );

			//	Note : ����Ʈ �Ϸ� ���� ����.
			//
			GLMSG::SNET_QUEST_END_STREAM NetMsgStream;
			NetMsgStream.SETSTREAM ( pBuff, dwSize );
			glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgStream );

			//	Note : ����Ʈ ���� ���� ���� ����.
			//
			GLMSG::SNET_QUEST_PROG_DEL NetMsgDel;
			NetMsgDel.dwQUESTID = dwQID;
			glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgDel );
		}
	}
	
	return;
}

void GLChar::DoQuestCheckLeaveMap ()
{
	GLQUESTPROG* pQuestProg = NULL;
	DWORD dwQID = 0;
	bool bNON = false;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	GLQuestPlay::MAPQUEST_ITER iter_cur;
	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		iter_cur = iter++;

		pQuestProg = (*iter_cur).second;
		dwQID = pQuestProg->m_sNID.dwID;

		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQID  );
		if ( !pQUEST )					continue;

		if( !pQUEST->m_ProgressEvent.bUseProgressEvent ) continue;
		if( pQUEST->m_ProgressEvent.dwStartProgress != EMQP_SELECT_MOVE || !pQUEST->m_ProgressEvent.IsLEAVE() ) continue;

		if( pQUEST->m_ProgressEvent.startMap.nidMAP != m_sMapID )
		{
			//	����Ͽ� ����Ʈ ���ǿ� �ɸ��� �˸�.
			GLMSG::SNETPC_QUEST_PROG_LEAVEMAP NetMsg;
			NetMsg.dwQID = dwQID;
			glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsg);

			CheckQuestProgressEvent( 2, pQUEST );

			//	�ڵ� ���� ó��.
			pQuestProg->DoGIVEUP ();

			//	�Ϸ�� ����Ʈ�� �߰�.
			m_cQuestPlay.InsertEnd ( *pQuestProg );

			CByteStream cByteStream;
			pQuestProg->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			//	�������� ����.
			m_cQuestPlay.DeleteProc ( dwQID );

			//	Note : ����Ʈ �Ϸ� ���� ����.
			//
			GLMSG::SNET_QUEST_END_STREAM NetMsgStream;
			NetMsgStream.SETSTREAM ( pBuff, dwSize );
			glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgStream );

			//	Note : ����Ʈ ���� ���� ���� ����.
			//
			GLMSG::SNET_QUEST_PROG_DEL NetMsgDel;
			NetMsgDel.dwQUESTID = dwQID;
			glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgDel );
		}
	}

	return;
}


void GLChar::DoQuestCheckLimitDie ()
{
	GLQUESTPROG* pQuestProg = NULL;
	DWORD dwQID = 0;
	bool bNON = false;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	GLQuestPlay::MAPQUEST_ITER iter_cur;
	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		iter_cur = iter++;

		pQuestProg = (*iter_cur).second;
		dwQID = pQuestProg->m_sNID.dwID;

		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQID  );
		if ( !pQUEST )					continue;

		bNON = pQuestProg->IsNonDIE();
		if ( bNON )
		{
			//	����Ͽ� ����Ʈ ���ǿ� �ɸ��� �˸�.
			GLMSG::SNETPC_QUEST_PROG_NONDIE NetMsg;
			NetMsg.dwQID = dwQID;
			glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsg);

			if( pQUEST->m_ProgressEvent.bUseProgressEvent && pQUEST->m_ProgressEvent.IsDIE() )
				CheckQuestProgressEvent( 2, pQUEST );

			//	�ڵ� ���� ó��.
			pQuestProg->DoGIVEUP ();

			//	�Ϸ�� ����Ʈ�� �߰�.
			m_cQuestPlay.InsertEnd ( *pQuestProg );

			CByteStream cByteStream;
			pQuestProg->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			//	�������� ����.
			m_cQuestPlay.DeleteProc ( dwQID );

			//	Note : ����Ʈ �Ϸ� ���� ����.
			//
			GLMSG::SNET_QUEST_END_STREAM NetMsgStream;
			NetMsgStream.SETSTREAM ( pBuff, dwSize );
			glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgStream );

			//	Note : ����Ʈ ���� ���� ���� ����.
			//
			GLMSG::SNET_QUEST_PROG_DEL NetMsgDel;
			NetMsgDel.dwQUESTID = dwQID;
			glGaeaServer.SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgDel );
		}
	}
	
	return;
}

void GLChar::DoQuestLevel ()
{
	GLQUESTPROG* pPROG = NULL;
	DWORD dwQID = 0;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();
	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST_ITER iter_cur;
	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		iter_cur = iter++;

		pPROG = (*iter_cur).second;
		dwQID = pPROG->m_sNID.dwID;

		if ( pPROG->m_sSTEP_PROG.m_bLevel )			continue;

		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if ( !pQUEST )								continue;

		pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )								continue;

		if ( pSTEP->m_wLevel > m_wLevel )			continue;

		pPROG->m_sSTEP_PROG.m_bLevel = true;

		GLMSG::SNETPC_QUEST_PROG_LEVEL NetMsg;
		NetMsg.dwQID = dwQID;
		glGaeaServer.SENDTOCLIENT(m_dwClientID,&NetMsg);

		//	Note : ����Ʈ ����.
		//
		bool bPROG = DoQuestProgress ( pPROG );
		if ( bPROG )
		{
			pPROG = NULL;
		}
	}
}

bool GLChar::QuestMoveProgressEvent ( SNATIVEID mapID, DWORD wPosX, DWORD wPosY, DWORD dwGateID, bool bStartMove /*= FALSE*/ )
{
	SNATIVEID sMAPID = mapID;
	DWORD dwGATEID(UINT_MAX);	
	DWORD wPOSX = USHRT_MAX;
	DWORD wPOSY = USHRT_MAX;

	if( wPosX != USHRT_MAX && wPosY != USHRT_MAX ) 
	{
		wPOSX = wPosX;
		wPOSY = wPosY;
	}else if( dwGateID != 0 )
	{
		dwGATEID = dwGateID;
	}

	SMAPNODE *pMapNode = GLGaeaServer::GetInstance().FindMapNode ( sMAPID );
	if ( !pMapNode ) return FALSE;	
	D3DXVECTOR3 vPOS(0,0,0); 
	if( !bStartMove && wPOSX != USHRT_MAX && wPosY != USHRT_MAX )
	{
		GLMapAxisInfo sMapAxisInfo;
		sMapAxisInfo.LoadFile ( pMapNode->strFile.c_str() );
		sMapAxisInfo.MapPos2MiniPos ( wPOSX, wPOSY, vPOS.x, vPOS.z );
	}else if( bStartMove ){
		dwGATEID = m_dwStartGate;	
	}

	// PET
	// ���̵��� Pet ����
	GLGaeaServer::GetInstance().ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,false,true) );
	GLGaeaServer::GetInstance().ReserveDropOutSummon ( SDROPOUTSUMMONINFO(m_dwSummonGUID,true) );
	GLGaeaServer::GetInstance().SetActiveVehicle( m_dwClientID, m_dwGaeaID, false );


	//	Note : �ٸ� �ʵ� ������ ���.
	
	if ( pMapNode->dwFieldSID!=GLGaeaServer::GetInstance().GetFieldSvrID() )
	{

		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.sMAPID = sMAPID;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPOS = vPOS;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgAg );
	}
	//	Note : ���� �ʵ� ������ ���.
	else
	{
		GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
		SNATIVEID sCurMapID = m_sMapID;

		BOOL bOK = GLGaeaServer::GetInstance().RequestInvenRecallThisSvr ( this, sMAPID, dwGATEID, vPOS );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
			return FALSE;
		}

		// ������ �����̻� ����
		if ( sCurMapID != sMAPID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		DISABLESKEFF ( i );
		}

		// ���� �׼� �ʱ�ȭ
		ResetAction();


		//	Note : �� �̵� ������ �˸�.
		//
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.sMAPID = sMAPID;
		NetMsgFB.vPOS = m_vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgFB );
	}

	return TRUE;
}

// ����Ʈ ���࿡ ���� ����Ǵ� �̺�Ʈ�� 
bool GLChar::CheckQuestProgressEvent ( BYTE eventType, GLQUEST* pQUEST )
{
	if( !pQUEST->m_ProgressEvent.bUseProgressEvent ) return TRUE;
	
	if( pQUEST->m_ProgressEvent.bUseProgressEvent )
	{
		DWORD dwProgress = 0;
		GLQUEST_PROGRESS_MAP progressMap;
		if( eventType == 0 ) // ���۽�
		{
			dwProgress  = pQUEST->m_ProgressEvent.dwStartProgress;
			progressMap = pQUEST->m_ProgressEvent.startMap;
		}else if( eventType == 1 ) // �Ϸ��
		{
			dwProgress  = pQUEST->m_ProgressEvent.dwEndProgress;
			progressMap = pQUEST->m_ProgressEvent.endMap;
		}else if( eventType == 2 ) // ���н�
		{
			dwProgress  = pQUEST->m_ProgressEvent.dwFailProgress;
			progressMap = pQUEST->m_ProgressEvent.failMap;
		}else{
			return TRUE;
		}

		//  Note : ���� ���۽� �� �̵��� �ʿ��� ���			
		if( dwProgress == EMQP_SELECT_MOVE )
		{
			if( !QuestMoveProgressEvent( progressMap.nidMAP, progressMap.wPosX, progressMap.wPosY, progressMap.dwGateID ) ) return FALSE;	
		}
		//  Note : ���� ���۽� ĳ������ ���� �������� �̵��� �ؾ��ϴ� ���
		if( dwProgress == EMQP_START_MOVE )
		{
			if( !QuestMoveProgressEvent( m_sStartMapID, USHRT_MAX, USHRT_MAX, 0, TRUE ) ) return FALSE;
		}

		//  Note : ���� ���۽� ĳ���Ͱ� ����� �ؾ��ϴ� ����Ʈ �� ���
		if( dwProgress == EMQP_CHAR_DEAD )
		{
			m_sHP.wNow = 0;
		}

		

	}
	
	
	
	


	return TRUE;
}


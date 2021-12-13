#include "pch.h"
#include "./GLChar.h"
#include "./GLGaeaServer.h"
#include "./GLQUEST.h"
#include "./GLSchoolFreePK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT GLChar::MsgGameJoin ()
{
	// 클라이언트에 전송 : 캐릭터정보 + 스킬 + 아이템

//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	GLMSG::SNETLOBBY_CHARJOIN NetCharJoin;

	StringCchCopy ( NetCharJoin.szUserID , USR_ID_LENGTH+1, m_szUID );

	NetCharJoin.dwClientID	= m_dwClientID;
	NetCharJoin.dwGaeaID	= m_dwGaeaID;
	NetCharJoin.sMapID		= m_sMapID;
	NetCharJoin.vPos		= m_vPos;
	NetCharJoin.Data		= GETCHARDATA();

	NetCharJoin.wSKILLQUICK_ACT = m_wSKILLQUICK_ACT;
	memcpy ( NetCharJoin.sSKILLQUICK, m_sSKILLQUICK, sizeof(SNATIVEID)*EMSKILLQUICK_SIZE );
	memcpy ( NetCharJoin.sACTIONQUICK, m_sACTIONQUICK, sizeof(SACTION_SLOT)*EMACTIONQUICK_SIZE );

	NetCharJoin.dwNumInvenItems = m_cInventory.GetNumItems();
#if defined(VN_PARAM) //vietnamtest%%%
	NetCharJoin.dwNumVNInvenItems = m_cVietnamInventory.GetNumItems();
#endif

#if defined(TW_PARAM) || defined(_RELEASED) || defined(HK_PARAM) || defined ( TH_PARAM ) // ***Tracing Log print
	NetCharJoin.bTracingChar	  = m_bTracingUser;
#endif

	NetCharJoin.dwNumSkill = static_cast<DWORD>(m_ExpSkills.size());
	
	NetCharJoin.dwNumQuestProc = static_cast<DWORD>(m_cQuestPlay.GetQuestProc().size());
	NetCharJoin.dwNumQuestEnd = static_cast<DWORD>(m_cQuestPlay.GetQuestEnd().size());
	NetCharJoin.dwChargeItem = static_cast<DWORD>(m_mapCharged.size());
	NetCharJoin.dwItemCoolTime = static_cast<DWORD> ( m_mapCoolTimeID.size() + m_mapCoolTimeType.size() );

	NetCharJoin.bBIGHEAD = m_pGLGaeaServer->m_bBigHead;
	NetCharJoin.bBIGHAND = m_pGLGaeaServer->m_bBigHand;

	NetCharJoin.sStartMapID		 = m_sStartMapID;
	NetCharJoin.dwStartGate		 = m_dwStartGate;
	NetCharJoin.sLastCallMapID	 = m_sLastCallMapID;
	NetCharJoin.vLastCallPos	 = m_vLastCallPos;
	NetCharJoin.dwThaiCCafeClass = m_dwThaiCCafeClass;
	// 말레이시아 PC방 이벤트
	NetCharJoin.nMyCCafeClass	 = m_nMyCCafeClass;

	m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetCharJoin );

	// 케릭터 착용 아이템.
//	GLMSG::SNETLOBBY_CHARPUTON NetMsgCharPutOn; 
//	memcpy ( NetMsgCharPutOn.PutOnItems, m_PutOnItems, sizeof(SITEMCUSTOM)*SLOT_TSIZE );
//	m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgCharPutOn );

	// BuffSize 초과로 한개씩 보내는 걸로 수정
	for ( int i = 0; i < SLOT_TSIZE; ++i )
	{
		GLMSG::SNETLOBBY_CHARPUTON_EX NetMsgCharPutOnEx; 
		NetMsgCharPutOnEx.PutOnItem = m_PutOnItems[i];
		NetMsgCharPutOnEx.nSlot = i;
		m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgCharPutOnEx );
	}

	// 클라이언트에 인벤토리 전송
	// 루프를 돌면서 하나씩 전송
	{
		GLInventory::CELL_MAP_ITER iter = m_cInventory.GetItemList()->begin();
		GLInventory::CELL_MAP_ITER iter_end = m_cInventory.GetItemList()->end();
		for ( ; iter!=iter_end; ++iter )
		{
			SINVENITEM* pInvenItem = (*iter).second;

			GLMSG::SNETLOBBY_INVENITEM NetInvenItem;
			NetInvenItem.Data = *pInvenItem;
			
			m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetInvenItem );
		}
	}

#if defined(VN_PARAM) //vietnamtest%%%
	// 클라이언트에 베트남 인벤토리 전송
	// 루프를 돌면서 하나씩 전송
	{
		GLInventory::CELL_MAP_ITER iter = m_cVietnamInventory.GetItemList()->begin();
		GLInventory::CELL_MAP_ITER iter_end = m_cVietnamInventory.GetItemList()->end();
		for ( ; iter!=iter_end; ++iter )
		{
			SINVENITEM* pInvenItem = (*iter).second;

			GLMSG::SNETLOBBY_VIETNAM_INVENITEM NetInvenItem;
			NetInvenItem.Data = *pInvenItem;

			m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetInvenItem );
		}
	}
#endif

	//	배운 스킬목록 모두 전송.
	//
	{
		GLMSG::SNETLOBBY_CHARSKILL NetSkill;

		SKILL_MAP_ITER iter = m_ExpSkills.begin();
		SKILL_MAP_ITER iter_end = m_ExpSkills.end();
		for ( ; iter!=iter_end; ++iter )
		{
			const SCHARSKILL &sSkill = (*iter).second;

			NetSkill.ADD ( sSkill );
			if ( NetSkill.wNum == EMGLMSG_SKILLMAX )
			{
				m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetSkill );
				NetSkill.wNum = 0;
			}
		}

		if ( NetSkill.wNum != 0 )	m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetSkill );
	}

	//	진행중인 퀘스트 모두 전송.
	{
		GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG *pPROG = (*iter).second;

			CByteStream cByteStream;
			pPROG->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			GLMSG::SNETLOBBY_QUEST_PROG NetMsg;
			NetMsg.SETSTREAM ( pBuff, dwSize );
			m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsg );
		}
	}

	//	완료된 퀘스트 모두 전송.
	{
		GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestEnd().begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestEnd().end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG *pPROG = (*iter).second;

			CByteStream cByteStream;
			pPROG->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			GLMSG::SNETLOBBY_QUEST_END NetMsg;
			NetMsg.SETSTREAM ( pBuff, dwSize );
			m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsg );
		}
	}

	//	아이템 쿨타임 전송
	//
	{
		GLMSG::SNETLOBBY_ITEM_COOLTIME NetCoolTime;

		COOLTIME_MAP_ITER iter = m_mapCoolTimeID.begin();
		COOLTIME_MAP_ITER iter_end = m_mapCoolTimeID.end();

		NetCoolTime.emCoolType = EMCOOL_ITEMID;

		for ( ; iter!=iter_end; ++iter )
		{
			const ITEM_COOLTIME &sCoolTime = (*iter).second;

			NetCoolTime.ADD ( sCoolTime );
			if ( NetCoolTime.dwNum == EMGLMSG_COOLTIMEMAX )
			{
				m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetCoolTime );
				NetCoolTime.dwNum = 0;
			}
		}

		if ( NetCoolTime.dwNum != 0 )	m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetCoolTime );

		iter = m_mapCoolTimeType.begin();
		iter_end = m_mapCoolTimeType.end();

		NetCoolTime.emCoolType = EMCOOL_ITEMTYPE;
		NetCoolTime.dwNum = 0;

		for ( ; iter!=iter_end; ++iter )
		{
			const ITEM_COOLTIME &sCoolTime = (*iter).second;

			NetCoolTime.ADD ( sCoolTime );
			if ( NetCoolTime.dwNum == EMGLMSG_COOLTIMEMAX )
			{
				m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetCoolTime );
				NetCoolTime.dwNum = 0;
			}
		}

		if ( NetCoolTime.dwNum != 0 )	m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetCoolTime );
	}
/*
	//	아이템 샾에서 구입한 물품 목록.
	{
		MAPSHOP_ITER pos = m_mapCharged.begin();
		MAPSHOP_ITER end = m_mapCharged.end();
		for ( ; pos!=end; ++pos )
		{
			SHOPPURCHASE &sPURCHASE = (*pos).second;

			GLMSG::SNETLOBBY_CHARGE_ITEM NetMsg;
			StringCchCopy ( NetMsg.szPurKey, PURKEY_LENGTH+1, sPURCHASE.strPurKey.GetString() );
			NetMsg.nidITEM = SNATIVEID(sPURCHASE.wItemMain,sPURCHASE.wItemSub);
			m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsg );
		}
	}
*/
	//	클럽 정보 전송.
	{
		GLClubMan& cClubMan = m_pGLGaeaServer->GetClubMan();
		GLCLUB *pCLUB = cClubMan.GetClub ( m_dwGuild );

		GLMSG::SNETLOBBY_CLUB_INFO NetMsgInfo;
		NetMsgInfo.dwClubID = m_dwGuild;

		if ( pCLUB )
		{
			StringCchCopy ( NetMsgInfo.szClubName, CHAR_SZNAME, pCLUB->m_szName );

			NetMsgInfo.dwMasterID = pCLUB->m_dwMasterID;
			StringCchCopy ( NetMsgInfo.szMasterName, CHAR_SZNAME, pCLUB->m_szMasterName );
			NetMsgInfo.dwCDCertifior = pCLUB->m_dwCDCertifior;

			NetMsgInfo.dwRank = pCLUB->m_dwRank;;

			NetMsgInfo.dwMarkVER = pCLUB->m_dwMarkVER;

			NetMsgInfo.tOrganize = pCLUB->m_tOrganize;
			NetMsgInfo.tDissolution = pCLUB->m_tDissolution;

			NetMsgInfo.dwAlliance = pCLUB->m_dwAlliance;

			NetMsgInfo.dwMEMBER_NUM = pCLUB->GetNemberNum();
			NetMsgInfo.dwALLIANCE_NUM = pCLUB->GetAllianceNum();
			NetMsgInfo.dwBATTLE_NUM = pCLUB->GetBattleNum();

			NetMsgInfo.dwBattleWin	= pCLUB->m_dwBattleWin;
			NetMsgInfo.dwBattleLose	= pCLUB->m_dwBattleLose;
			NetMsgInfo.dwBattleDraw	= pCLUB->m_dwBattleDraw;

			if ( pCLUB->m_dwAlliance!=CLUB_NULL )
			{
				GLCLUB *pCLUB_A = cClubMan.GetClub ( pCLUB->m_dwAlliance );
				if ( pCLUB_A )
				{
					NetMsgInfo.dwALLIANCE_NUM = pCLUB_A->GetAllianceNum();
					NetMsgInfo.dwAllianceBattleWin	= pCLUB_A->m_dwAllianceBattleWin;
					NetMsgInfo.dwAllianceBattleLose	= pCLUB_A->m_dwAllianceBattleLose;
					NetMsgInfo.dwAllianceBattleDraw	= pCLUB_A->m_dwAllianceBattleDraw;
				}
			}

			StringCchCopy ( NetMsgInfo.szNotice, EMCLUB_NOTICE_LEN+1, pCLUB->m_szNotice );
		}

		m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgInfo );

		if ( pCLUB )
		{
			GLMSG::SNETLOBBY_CLUB_MEMBER NetMsgMember;

			CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
			CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
			for ( ; pos!=end; ++pos )
			{
				const GLCLUBMEMBER &sMEMBER = (*pos).second;
				bool bOK = NetMsgMember.ADDMEMBER ( sMEMBER.dwID, sMEMBER.m_dwFlags, sMEMBER.m_szName );
				if ( !bOK )
				{
					//	Note : 메시지 전송.
					m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgMember );
					NetMsgMember.RESET();

					NetMsgMember.ADDMEMBER ( sMEMBER.dwID, sMEMBER.m_dwFlags, sMEMBER.m_szName );
				}
			}

			//	Note : 메시지 전송.
			if ( NetMsgMember.dwMemberNum > 0 )
			{
				m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgMember );
				NetMsgMember.RESET();
			}
		}

		if ( pCLUB )
		{
			if ( pCLUB->m_dwAlliance!=CLUB_NULL )
			{
				GLClubMan& cClubMan = m_pGLGaeaServer->GetClubMan();
				GLCLUB *pCLUB_A = cClubMan.GetClub ( pCLUB->m_dwAlliance );
				if ( pCLUB_A )
				{
					GLMSG::SNETLOBY_CLUB_ANCE_2CLT NetMsgAlliance;

					CLUB_ALLIANCE_ITER pos = pCLUB_A->m_setAlliance.begin();
					CLUB_ALLIANCE_ITER end = pCLUB_A->m_setAlliance.end();
					for ( ; pos!=end; ++pos )
					{
						const GLCLUBALLIANCE &sALLIANCE = (*pos);

						bool bOK = NetMsgAlliance.ADDALLIANCE ( sALLIANCE.m_dwID, sALLIANCE.m_szName );
						if ( !bOK )
						{
							//	Note : 메시지 전송.
							m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgAlliance );
							NetMsgAlliance.RESET();

							NetMsgAlliance.ADDALLIANCE ( sALLIANCE.m_dwID, sALLIANCE.m_szName );
						}
					}

					//	Note : 메시지 전송.
					if ( NetMsgAlliance.dwAllianceNum > 0 )
					{
						m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgAlliance );
						NetMsgAlliance.RESET();
					}
				}
			}
		}

		if ( pCLUB )
		{
			GLMSG::SNETLOBBY_CLUB_BATTLE NetMsgBattle;

			CLUB_BATTLE_ITER pos = pCLUB->m_mapBattle.begin();
			CLUB_BATTLE_ITER end = pCLUB->m_mapBattle.end();
			for ( ; pos!=end; ++pos )
			{
				const GLCLUBBATTLE &sBattle = (*pos).second;
				GLCLUBBATTLE_LOBY sBattleLoby = sBattle;
				bool bOK = NetMsgBattle.ADDBATTLE ( sBattleLoby );
				if ( !bOK )
				{
					//	Note : 메시지 전송.
					m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgBattle );
					NetMsgBattle.RESET();

					NetMsgBattle.ADDBATTLE ( sBattleLoby );
				}
			}

			//	Note : 메시지 전송.
			if ( NetMsgBattle.dwBattleNum > 0 )
			{
				m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgBattle );
				NetMsgBattle.RESET();
			}
		}
	}

	return S_OK;
}

HRESULT GLChar::MsgClubInfo ()
{
//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	GLClubMan& cClubMan = m_pGLGaeaServer->GetClubMan();
	GLCLUB *pCLUB = cClubMan.GetClub ( m_dwGuild );
	if ( !pCLUB )	return S_OK;

	GLMSG::SNET_CLUB_INFO_2CLT NetMsgInfo;
	NetMsgInfo.dwClubID = m_dwGuild;

	StringCchCopy ( NetMsgInfo.szClubName, CHAR_SZNAME, pCLUB->m_szName );

	NetMsgInfo.dwMasterID = pCLUB->m_dwMasterID;
	StringCchCopy ( NetMsgInfo.szMasterName, CHAR_SZNAME, pCLUB->m_szMasterName );
	NetMsgInfo.dwCDCertifior = pCLUB->m_dwCDCertifior;

	StringCchCopy ( NetMsgInfo.szNickName, CHAR_SZNAME, m_szNick );

	NetMsgInfo.dwMarkVER = pCLUB->m_dwMarkVER;
	NetMsgInfo.dwRank = pCLUB->m_dwRank;

	NetMsgInfo.tOrganize = pCLUB->m_tOrganize;
	NetMsgInfo.tDissolution = pCLUB->m_tDissolution;
	StringCchCopy ( NetMsgInfo.szNotice, EMCLUB_NOTICE_LEN+1, pCLUB->m_szNotice );

	m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgInfo );

	GLMSG::SNET_CLUB_MEMBER_2CLT NetMsgMember;
	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBMEMBER &sMEMBER = (*pos).second;
		bool bFULL = NetMsgMember.ADDMEMBER ( sMEMBER.dwID, sMEMBER.m_dwFlags, sMEMBER.m_szName );
		if ( bFULL )
		{
			//	Note : 메시지 전송.
			m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgMember );
			NetMsgMember.RESET();

			NetMsgMember.ADDMEMBER ( sMEMBER.dwID, sMEMBER.m_dwFlags, sMEMBER.m_szName );
		}
	}

	//	Note : 메시지 전송.
	if ( NetMsgMember.dwMemberNum > 0 )
	{
		m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgMember );
		NetMsgMember.RESET();
	}

	return S_OK;
}

NET_MSG_GENERIC* GLChar::ReqNetMsg_Drop ()
{
	static GLMSG::SNETDROP_PC NetMsg;
	NetMsg = GLMSG::SNETDROP_PC();

	SDROP_CHAR &dropChar = NetMsg.Data;

	StringCchCopy ( dropChar.szName, CHAR_SZNAME, m_szName );
	dropChar.emTribe	= m_emTribe;
	dropChar.emClass	= m_emClass;
	dropChar.wSchool	= m_wSchool;
	dropChar.wHair		= m_wHair;
	dropChar.wHairColor = m_wHairColor;
	dropChar.wFace		= m_wFace;
	dropChar.wSex		= m_wSex;
	dropChar.nBright	= m_nBright;

	dropChar.dwSummonGUID = m_dwSummonGUID;

	dropChar.dwCharID	= m_dwCharID;
	dropChar.wLevel		= m_wLevel;
	dropChar.dwGuild	= m_dwGuild;

	dropChar.m_bItemShopOpen = m_bItemShopOpen;
	dropChar.m_dwANISUBTYPE = m_dwANISUBSELECT;

	if ( dropChar.dwGuild!=CLUB_NULL )
	{
		GLClubMan &cClubMan = m_pGLGaeaServer->GetClubMan();
		GLCLUB *pCLUB = cClubMan.GetClub ( dropChar.dwGuild );
		if ( pCLUB )
		{
			dropChar.dwGuildMarkVer = pCLUB->m_dwMarkVER;
			dropChar.dwGuildMaster = pCLUB->m_dwMasterID;			
			if ( pCLUB->IsMemberFlgCDCertify ( m_dwCharID ) )	dropChar.dwFLAGS |= SDROP_CHAR::CLUB_CD;

			StringCchCopy ( dropChar.szClubName, CHAR_SZNAME, pCLUB->m_szName );
			
			dropChar.dwAlliance = pCLUB->m_dwAlliance;
		}
//		else
//		{
//			TEXTCONSOLEMSG_WRITE ( _T("ERROR : CLUBID_ERROR CharID %d ClubID %d"), m_dwCharID, m_dwGuild );
//		}

		StringCchCopy ( dropChar.szNick, CHAR_SZNAME, m_szNick );

	}

	if ( m_dwPartyID!=GAEAID_NULL )
	{
		const GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
		if ( pParty )
		{
			dropChar.dwParty = m_dwPartyID;
			dropChar.dwPMasterID = pParty->m_dwMASTER;
		}
	}

	dropChar.sHP		= m_sHP;

	dropChar.dwGaeaID	= m_dwGaeaID;
	dropChar.sMapID		= m_sMapID;
	dropChar.dwCeID		= m_dwCeID;
	dropChar.vPos		= m_vPos;
	dropChar.vDir		= m_vDir;

	dropChar.Action		= m_Action;
	dropChar.dwActState	= m_dwActState;
	dropChar.vTarPos	= m_TargetID.vPos;

	if ( dropChar.Action==GLAT_MOVE )
	{
		if ( m_actorMove.PathIsActive() )
		{
			dropChar.vTarPos = m_actorMove.GetTargetPosition();
		}
		else
		{
			dropChar.Action = GLAT_IDLE;
		}
	}

	for ( int i=0; i<SKILLFACT_SIZE; ++i )		dropChar.sSKILLFACT[i].Assign ( m_sSKILLFACT[i], i );
	for ( int i=0; i<EMBLOW_MULTI; ++i )		dropChar.sSTATEBLOWS[i] = m_sSTATEBLOWS[i];
	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )		dropChar.m_PutOnItems[i].Assign ( m_PutOnItems[i] );

	int i = 0;
	std::set<int>::iterator iter_EffNum;
	iter_EffNum = m_setLandEffectNum.begin();
	for( ; iter_EffNum != m_setLandEffectNum.end(); ++iter_EffNum )
	{
		if( i >= EMLANDEFFECT_MULTI ) break;
		dropChar.nLandEffect[i] = *iter_EffNum;
		++i;
	}
	

	dropChar.m_bUseArmSub = m_bUseArmSub;

	if ( m_fGenAge < 0.6f )
		dropChar.dwFLAGS |= SDROP_CHAR::CHAR_GEN;

	dropChar.sPASSIVE_SKILL = m_sSUM_PASSIVE;

	dropChar.sQITEMFACT = m_sQITEMFACT;
	dropChar.sEVENTFACT = m_pGLGaeaServer->m_sEVENTFACT;


	if ( m_sVehicle.IsActiveValue() )
	{
		dropChar.m_bVehicle = m_bVehicle;
		dropChar.m_sVehicle.m_dwGUID = m_sVehicle.m_dwGUID;
		dropChar.m_sVehicle.m_emTYPE = m_sVehicle.m_emTYPE;
		dropChar.m_sVehicle.m_sVehicleID = m_sVehicle.m_sVehicleID;

		for ( int i =0; i < ACCE_TYPE_SIZE; ++i )
		{
			dropChar.m_sVehicle.m_PutOnItems[i].Assign( m_sVehicle.m_PutOnItems[i] );
		}
	}


	return (NET_MSG_GENERIC*) &NetMsg;
}

void GLChar::SETFIELDPET ( PSFIELDCROW pFIELDCROW )
{
	GASSERT(pFIELDCROW);
	GASSERT(pFIELDCROW->dwID<m_pGLGaeaServer->GetMaxClient());
	GASSERT(pFIELDCROW->emCROW==CROW_PET);

	if ( !pFIELDCROW )													return;
	if ( pFIELDCROW->dwID>=m_pGLGaeaServer->GetMaxClient() )	
	{
		CDebugSet::ToListView ( "OutOf PetGUID : %d in GlChar::SETFIELDPET()", pFIELDCROW->dwID );
		return;
	}
	if ( pFIELDCROW->emCROW!=CROW_PET )									return;

	m_arrayFieldPET[pFIELDCROW->dwID] = pFIELDCROW;
}

void GLChar::SETFIELDSUMMON ( PSFIELDCROW pFIELDCROW )
{
	GASSERT(pFIELDCROW);
	GASSERT(pFIELDCROW->dwID<m_pGLGaeaServer->GetMaxClient());
	GASSERT(pFIELDCROW->emCROW==CROW_SUMMON);

	if ( !pFIELDCROW )													return;
	if ( pFIELDCROW->dwID>=m_pGLGaeaServer->GetMaxClient() )	
	{
		CDebugSet::ToListView ( "OutOf PetGUID : %d in GlChar::SETFIELDPET()", pFIELDCROW->dwID );
		return;
	}
	if ( pFIELDCROW->emCROW!=CROW_SUMMON )									return;

	m_arrayFieldSummon[pFIELDCROW->dwID] = pFIELDCROW;
}

void GLChar::SETFIELDPC ( PSFIELDCROW pFIELDCROW )
{
	GASSERT(pFIELDCROW);
	GASSERT(pFIELDCROW->dwID<m_pGLGaeaServer->GetMaxClient());
	GASSERT(pFIELDCROW->emCROW==CROW_PC);

	if ( !pFIELDCROW )													return;
	if ( pFIELDCROW->dwID>=m_pGLGaeaServer->GetMaxClient() )		return;
	if ( pFIELDCROW->emCROW!=CROW_PC )									return;

	m_arrayFieldPC[pFIELDCROW->dwID] = pFIELDCROW;
}

void GLChar::SETFIELDCROW ( PSFIELDCROW pFIELDCROW )
{
	GASSERT(pFIELDCROW);
	GASSERT(pFIELDCROW->dwID<MAXCROW);
	GASSERT(pFIELDCROW->emCROW==CROW_MOB);

	if ( !pFIELDCROW )													return;
	if ( pFIELDCROW->dwID>=MAXCROW )									return;
	if ( pFIELDCROW->emCROW!=CROW_MOB )									return;

	m_arrayFieldCROW[pFIELDCROW->dwID] = pFIELDCROW;
}

void GLChar::SETFIELDMATERIAL ( PSFIELDCROW pFIELDCROW )
{
	GASSERT(pFIELDCROW);
	GASSERT(pFIELDCROW->dwID<MAXCROW);
	GASSERT(pFIELDCROW->emCROW==CROW_MATERIAL);

	if ( !pFIELDCROW )													return;
	if ( pFIELDCROW->dwID>=MAXCROW )									return;
	if ( pFIELDCROW->emCROW!=CROW_MATERIAL )							return;

	m_arrayFieldMATERIAL[pFIELDCROW->dwID] = pFIELDCROW;
}

void GLChar::SETFIELDITEM ( PSFIELDCROW pFIELDCROW )
{
	GASSERT(pFIELDCROW);
	GASSERT(pFIELDCROW->dwID<MAXITEM);
	GASSERT(pFIELDCROW->emCROW==CROW_ITEM);

	if ( !pFIELDCROW )													return;
	if ( pFIELDCROW->dwID>=MAXITEM )									return;
	if ( pFIELDCROW->emCROW!=CROW_ITEM )								return;

	m_arrayFieldITEM[pFIELDCROW->dwID] = pFIELDCROW;
}

void GLChar::SETFIELDMONEY ( PSFIELDCROW pFIELDCROW )
{
	GASSERT(pFIELDCROW);
	GASSERT(pFIELDCROW->dwID<MAXMONEY);
	GASSERT(pFIELDCROW->emCROW==CROW_MONEY);

	if ( !pFIELDCROW )													return;
	if ( pFIELDCROW->dwID>=MAXMONEY )									return;
	if ( pFIELDCROW->emCROW!=CROW_MONEY )								return;

	m_arrayFieldMONEY[pFIELDCROW->dwID] = pFIELDCROW;
}

void GLChar::RESETFIELDPET ( DWORD dwID )
{
	GASSERT(dwID<m_pGLGaeaServer->GetMaxClient());
	if ( dwID>=m_pGLGaeaServer->GetMaxClient() )
	{
		CDebugSet::ToListView ( "OutOf PetGUID : %d in GlChar::RESETFIELDPET()", dwID );
		return;
	}

	m_arrayFieldPET[dwID] = NULL;
}

void GLChar::RESETFIELDSUMMON ( DWORD dwID )
{
	GASSERT(dwID<m_pGLGaeaServer->GetMaxClient());
	if ( dwID>=m_pGLGaeaServer->GetMaxClient() )
	{
		CDebugSet::ToListView ( "OutOf SummonGUID : %d in GlChar::RESETFIELDSUMMON()", dwID );
		return;
	}

	m_arrayFieldSummon[dwID] = NULL;
}

void GLChar::RESETFIELDPC ( DWORD dwID )
{
	GASSERT(dwID<m_pGLGaeaServer->GetMaxClient());
	if ( dwID>=m_pGLGaeaServer->GetMaxClient() )					return;

	m_arrayFieldPC[dwID] = NULL;
}

void GLChar::RESETFIELDCROW ( DWORD dwID )
{
	GASSERT(dwID<MAXCROW);
	if ( dwID>=MAXCROW )												return;

	m_arrayFieldCROW[dwID] = NULL;
}

void GLChar::RESETFIELDMATERIAL ( DWORD dwID )
{
	GASSERT(dwID<MAXCROW);
	if ( dwID>=MAXCROW )												return;

	m_arrayFieldMATERIAL[dwID] = NULL;
}


void GLChar::RESETFIELDITEM ( DWORD dwID )
{
	GASSERT(dwID<MAXITEM);
	if ( dwID>=MAXITEM )												return;

	m_arrayFieldITEM[dwID] = NULL;
}

void GLChar::RESETFIELDMONEY ( DWORD dwID )
{
	GASSERT(dwID<MAXMONEY);
	if ( dwID>=MAXMONEY )												return;

	m_arrayFieldMONEY[dwID] = NULL;

}

//	Note : 새로 게임에 참가시, 다른 맵으로 진입시에 PC 주변의
//		모든 개체에 대한 정보를 인지하기 위해서 호출함.
//		( 클라이언트 측의 개체에 이전 정보가 모두 리샛되엇음을 가정한다. )
//
HRESULT GLChar::GetViewAround ()
{
//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	DxMsgServer *pMsgServer = m_pGLGaeaServer->GetMsgServer();
	if ( pMsgServer==NULL )	return E_FAIL;

	int nX = int ( GetPosition().x );
	int nZ = int ( GetPosition().z );

	LANDQUADTREE* pLandTree = m_pLandMan->GetLandTree();
	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+MAX_VIEWRANGE,nZ+MAX_VIEWRANGE,nX-MAX_VIEWRANGE,nZ-MAX_VIEWRANGE);
	pLandTree->FindNodes ( bRect, pLandTree->GetRootNode(), &pQuadNode );


	//	Note : 가시 영역에 있는 모든 셀의 오브젝트들을 탐색한다.
	//
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GLLandNode *pLandNode = pQuadNode->pData;

		GASSERT(pLandTree->GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLSUMMONNODE* pSummonNode = pLandNode->m_SummonList.m_pHead;
		for ( ; pSummonNode; pSummonNode = pSummonNode->pNext )
		{
			PGLSUMMONFIELD pSummon = pSummonNode->Data;

			if ( pSummon->m_dwGUID>=m_pGLGaeaServer->GetMaxClient() )					continue;

			// 클라이언트에 생성 메시지 전송
			GLMSG::SNET_SUMMON_DROP_SUMMON nmg;
			nmg.Data = pSummon->ReqNetMsg_Drop ();
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, ( NET_MSG_GENERIC* )&nmg );

			PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

			//	리스트에 등록.
			SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

			//	값 설정.
			pFIELDCROW->emCROW = CROW_SUMMON;
			pFIELDCROW->dwID = pSummon->m_dwGUID;
			pFIELDCROW->dwFRAME = m_dwViewFrame;
			pFIELDCROW->pNODE = pNODE;

			//	배열에 등록.
			SETFIELDSUMMON(pFIELDCROW);
		}	

		GLPETNODE* pPetNode = pLandNode->m_PETList.m_pHead;
		for ( ; pPetNode; pPetNode = pPetNode->pNext )
		{
			PGLPETFIELD pPet = pPetNode->Data;

			if ( pPet->m_dwGUID>=m_pGLGaeaServer->GetMaxClient() )					continue;

			// 클라이언트에 생성 메시지 전송
			NET_MSG_GENERIC* nmg = pPet->ReqNetMsg_Drop ();
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (LPVOID) nmg );

			PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

			//	리스트에 등록.
			SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

			//	값 설정.
			pFIELDCROW->emCROW = CROW_PET;
			pFIELDCROW->dwID = pPet->m_dwGUID;
			pFIELDCROW->dwFRAME = m_dwViewFrame;
			pFIELDCROW->pNODE = pNODE;

			//	배열에 등록.
			SETFIELDPET(pFIELDCROW);
		}	

		//	Note : - Char -
		//
		GLCHARNODE *pCharCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCharCur; pCharCur = pCharCur->pNext )
		{
			PGLCHAR pGLChar = pCharCur->Data;
			if ( !pGLChar ) continue;
			if ( pGLChar->m_dwClientID == m_dwClientID )	continue;

			GASSERT ( pGLChar->m_dwGaeaID != m_dwGaeaID );

			//	Memo :	주위 캐릭터에 대한 기본 및 외형 정보를 전송한다.
			NET_MSG_GENERIC* pNetMsgDrop = pGLChar->ReqNetMsg_Drop ();
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (LPVOID) pNetMsgDrop );

			if ( pGLChar->m_sPMarket.IsOpen() )
			{
				GLMSG::SNETPC_PMARKET_OPEN_BRD NetMsgPMarket;
				NetMsgPMarket.dwGaeaID = pGLChar->m_dwGaeaID;
				StringCchCopy ( NetMsgPMarket.szPMarketTitle, CHAT_MSG_SIZE+1, pGLChar->m_sPMarket.GetTitle().c_str() );
				m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgPMarket );
			}

			PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

			//	리스트에 등록.
			SGLNODE<SFIELDCROW*>* pNODE = m_arrayPC_FIELDLIST.ADDTAIL ( pFIELDCROW );

			//	값 설정.
			pFIELDCROW->emCROW = CROW_PC;
			pFIELDCROW->dwID = pGLChar->m_dwGaeaID;
			pFIELDCROW->dwFRAME = m_dwViewFrame;
			pFIELDCROW->pNODE = pNODE;

			//	배열에 등록.
			SETFIELDPC ( pFIELDCROW );
		}

		//	Note : - Crow -
		//
		GLCROWNODE *pCrowCur = pLandNode->m_CROWList.m_pHead;
		for ( ; pCrowCur; pCrowCur = pCrowCur->pNext )
		{
			PGLCROW pGLCrow = pCrowCur->Data;

			NET_MSG_GENERIC* nmg = pGLCrow->ReqNetMsg_Drop ();
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (LPVOID) nmg );

			PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

			//	리스트에 등록.
			SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

			//	값 설정.
			pFIELDCROW->emCROW = CROW_MOB;	//	몹, NPC 모두 CROW_MOB 로 분류하여 관리.
			pFIELDCROW->dwID = pGLCrow->m_dwGlobID;
			pFIELDCROW->dwFRAME = m_dwViewFrame;
			pFIELDCROW->pNODE = pNODE;

			//	배열에 등록.
			SETFIELDCROW(pFIELDCROW);
		}

		//	Note : - Material -
		//
		GLMATERIALNODE *pMaterialCur = pLandNode->m_MaterialList.m_pHead;
		for ( ; pMaterialCur; pMaterialCur = pMaterialCur->pNext )
		{
			PGLMATERIAL pGLMaterial = pMaterialCur->Data;

			NET_MSG_GENERIC* nmg = pGLMaterial->ReqNetMsg_Drop ();
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (LPVOID) nmg );

			PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

			//	리스트에 등록.
			SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

			//	값 설정.
			pFIELDCROW->emCROW = CROW_MATERIAL;	//	몹, NPC 모두 CROW_MOB 로 분류하여 관리.
			pFIELDCROW->dwID = pGLMaterial->m_dwGlobID;
			pFIELDCROW->dwFRAME = m_dwViewFrame;
			pFIELDCROW->pNODE = pNODE;

			//	배열에 등록.
			SETFIELDMATERIAL(pFIELDCROW);
		}
		

		//	Note : - Item -
		//
		ITEMDROPNODE* pCurItem = pLandNode->m_ItemList.m_pHead;
		for ( ; pCurItem; pCurItem = pCurItem->pNext )
		{
			PITEMDROP pItemDrop = pCurItem->Data;

			GLMSG::SNETDROP_ITEM sNetMsg;
			sNetMsg.Data.Assign ( pItemDrop->sDrop );
			sNetMsg.Data.fAge = pItemDrop->fAge;

			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (LPVOID) &sNetMsg );

			PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

			//	리스트에 등록.
			SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

			//	값 설정.
			pFIELDCROW->emCROW = CROW_ITEM;
			pFIELDCROW->dwID = pItemDrop->dwGlobID;
			pFIELDCROW->dwFRAME = m_dwViewFrame;
			pFIELDCROW->pNODE = pNODE;

			//	배열에 등록.
			SETFIELDITEM(pFIELDCROW);
		}

		//	Note : - Money -
		//
		MONEYDROPNODE* pCurMoney = pLandNode->m_MoneyList.m_pHead;
		for ( ; pCurMoney; pCurMoney = pCurMoney->pNext )
		{
			PMONEYDROP pMoneyDrop = pCurMoney->Data;

			GLMSG::SNETDROP_MONEY sNetMsg;
			sNetMsg.fAge = pMoneyDrop->fAge;
			sNetMsg.dwGlobID = pMoneyDrop->sDrop.dwGlobID;
			sNetMsg.vPos = pMoneyDrop->sDrop.vPos;
			sNetMsg.lnAmount = pMoneyDrop->sDrop.lnAmount;
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (LPVOID) &sNetMsg );

			PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

			//	리스트에 등록.
			SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

			//	값 설정.
			pFIELDCROW->emCROW = CROW_MONEY;
			pFIELDCROW->dwID = pMoneyDrop->dwGlobID;
			pFIELDCROW->dwFRAME = m_dwViewFrame;
			pFIELDCROW->pNODE = pNODE;

			//	배열에 등록.
			SETFIELDMONEY(pFIELDCROW);
		}
	}

	SetSTATE(EM_GETVA_AFTER);
	m_fMoveDelay = 0.0f;

	//	Note : GLLandMan의 셀에 등록하는 작업.
	//
	m_pLandMan->RegistChar ( this );

	//	Note : Preiod Sync
	//
	MsgSendPeriod ();

	//	Note : 날씨 초기화.
	//
	GLMSG::SNETPC_WEATHER NetMsg;
//	NetMsg.dwWeather = m_pLandMan->IsWeatherActive() ? GLPeriod::GetInstance().GetWeather () : NULL;
	DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( m_pLandMan->GetMapID().wMainID, m_pLandMan->GetMapID().wSubID );
	NetMsg.dwWeather = m_pLandMan->IsWeatherActive() ? dwWeather : NULL;
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );

	//	퀘스트의 경과된 시간 업데이트.
	MsgSendQuestTime ();

	//	기간이 만료된 아이템을 소멸.
	RESET_TIMELMT_ITEM ();
	RESET_CHECK_ITEM ();

	//	Memo :	무기 스왑 때문에 아이템 값을 다시 계산한다.
	INIT_DATA( FALSE, FALSE );

	// 성향 이벤트 유무
	GLMSG::SNETPC_SERVER_BRIGHTEVENT_INFO NetMsgBrightEvnet;
	NetMsgBrightEvnet.bBRIGHTEVENT = m_pGLGaeaServer->IsBRIGHTEVENT();
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgBrightEvnet );

	// 선도전 시작 유무
	if ( m_pGLGaeaServer->IsClubBattleStarted() )
	{
		GLMSG::SNETPC_SERVER_CLUB_BATTLE_INFO NetMsgClubBattle;
		NetMsgClubBattle.bClubBattle = true;
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgClubBattle );

		GLMSG::SNETPC_SERVER_CLUB_BATTLE_REMAIN_AG NetMsgReq;
		NetMsgReq.dwGaeaID = m_dwGaeaID;
		m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgReq );
	}

	if ( m_pGLGaeaServer->IsClubDMStarted() )
	{
		GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_INFO NetMsgClubDM;
		NetMsgClubDM.bClubDeathMatch = true;
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgClubDM );

		GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_REMAIN_AG NetMsgReq;
		NetMsgReq.dwGaeaID = m_dwGaeaID;
		m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgReq );
	}

	//	지역 특별 정보.
	GLMSG::SNETPC_LAND_INFO NetMsgLandInfo;
	NetMsgLandInfo.nidMAP = m_pLandMan->GetMapID();
	NetMsgLandInfo.bClubBattle = m_pLandMan->m_bGuidBattleMap;
	NetMsgLandInfo.bClubBattleHall = m_pLandMan->m_bGuidBattleMapHall;
	NetMsgLandInfo.bClubDeathMatch = m_pLandMan->m_bClubDeathMatchMap;
	NetMsgLandInfo.bClubDeathMatchHall = m_pLandMan->m_bClubDeathMatchMapHall;
	NetMsgLandInfo.bPK = m_pLandMan->IsPKZone();
	NetMsgLandInfo.fCommission = m_pLandMan->m_fCommissionRate;
	NetMsgLandInfo.dwGuidClub = m_pLandMan->m_dwGuidClubID;

	GLClubMan &sClubMan = m_pGLGaeaServer->GetClubMan();
	GLCLUB* pCLUB = sClubMan.GetClub(m_pLandMan->m_dwGuidClubID);
	if ( pCLUB )
	{
		NetMsgLandInfo.dwGuidClubMarkVer = pCLUB->m_dwMarkVER;
		StringCchCopy ( NetMsgLandInfo.szGuidClubName, CHAR_SZNAME, pCLUB->m_szName );
	}
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgLandInfo );

	// 학원간 자유 피케이 유무 전송
	GLMSG::SNETPC_SERVER_INFO NetMsgServerInfo;
	NetMsgServerInfo.bSCHOOL_FREEPK = GLSchoolFreePK::GetInstance().IsON();
    m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgServerInfo );

	//	Memo :	?아이템 이벤트 유무
	GLMSG::SNETPC_EVENTFACT_INFO NetMsgEvent;
	NetMsgEvent.sEVENTFACT = m_sEVENTFACT = m_pGLGaeaServer->m_sEVENTFACT;
	m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsgEvent );
	
	//	?아이템 리셋.
	GLMSG::SNETPC_QITEMFACT_END_BRD	NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	if ( m_sQITEMFACT.IsACTIVE() )
	{
		m_sQITEMFACT.RESET();

		//	Note : 종료되었을 경우 주변 사람에게 알림.
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
	}

	//	Note : 종료되었을 경우 자신에게 알림.
	m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgBrd );

	// 서버의 현재시간을 클라이언트에 알림
	GLMSG::SNET_MSG_SERVERTIME_BRD NetMsgTime;
	CTime cServerTime = CTime::GetCurrentTime();
	NetMsgTime.t64Time = cServerTime.GetTime();
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgTime );

	return S_OK;
}

HRESULT GLChar::MsgFieldMoveReset ()
{
	//	Note : 클럽 정보 리셋.
	if ( m_dwGuild!=CLUB_NULL )
	{
		GLCLUB *pCLUB = m_pGLGaeaServer->GetClubMan().GetClub(m_dwGuild);
		if ( pCLUB && pCLUB->m_dwMasterID==m_dwCharID )
		{
			GLMSG::SNET_CLUB_STORAGE_RESET NetMsgClubStorageReset;
			m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgClubStorageReset );
		}
	}

	return S_OK;
}

HRESULT GLChar::SendMsgViewAround ( NET_MSG_GENERIC* nmg )
{
//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	FIELDCROWNODE* pNode = m_arrayPC_FIELDLIST.m_pHead;
	while ( pNode )
	{
		GLChar* pPChar = m_pGLGaeaServer->GetChar ( pNode->Data->dwID );

		//	Note : 캐릭터의 삭제 시점과 UpdateViewAround ()과의 불일치로 캐릭터가 존재하지 않을 수 있음.
		if ( pPChar )
		{
			m_pGLGaeaServer->SENDTOCLIENT ( pPChar->m_dwClientID, nmg );

			// 만일 일반 체팅 메시지일 경우에 추적로그를 남겨야하므로 체크를 해서 agent에 로그를 보낸다.
			if( nmg->nType == NET_MSG_CHAT_FB )
			{
#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM)// ***Tracing Log print
				if( pPChar->m_bTracingUser )
				{
					NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
					NET_CHAT* pNetMsg = (NET_CHAT*) nmg;
					TracingMsg.nUserNum  = pPChar->GetUserID();
					StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, pPChar->m_szUID );

					CString strTemp;
					strTemp.Format( "**Normal Recv**, [%s][%s]->[%s][%s], %s", 
						     m_szUID, m_szName, pPChar->m_szUID, pPChar->m_szName, pNetMsg->szChatMsg );

					StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );
					
					m_pGLGaeaServer->SENDTOAGENT( pPChar->m_dwClientID, &TracingMsg );
				}
#endif
			}
		}

		pNode = pNode->pNext;
	};

	return S_OK;
}

BOOL GLChar::IsInViewAround ( DWORD dwGAEAID )
{
	return m_arrayFieldPC[dwGAEAID]!=NULL;
}

HRESULT GLChar::ResetViewAround ()
{
	ReSetSTATE(EM_GETVA_AFTER);
	SetSTATE(EM_ACT_WAITING);

	m_dwViewFrame = 0;

	//	Note : FIELD CROW 정리.
	//
	FIELDCROWNODE* pNode = m_arrayPC_FIELDLIST.m_pHead;
	while ( pNode )
	{
		SFIELDCROW* pFIELD = pNode->Data;

		//	배열에서 삭제.
		RESETFIELDPC ( pFIELD->dwID );

		//	리스트에서 삭제.
		m_pGLGaeaServer->RELEASE_FIELDCROW ( pFIELD );

		pNode = pNode->pNext;
	}

	m_arrayPC_FIELDLIST.DELALL ();

	pNode = m_arrayFIELDLIST.m_pHead;
	while ( pNode )
	{
		SFIELDCROW* pFIELD = pNode->Data;

		//	배열에서 삭제.
		switch ( pFIELD->emCROW )
		{
		case CROW_MOB:		RESETFIELDCROW(pFIELD->dwID);	break;
		case CROW_ITEM:		RESETFIELDITEM(pFIELD->dwID);	break;
		case CROW_MONEY:	RESETFIELDMONEY(pFIELD->dwID);	break;
		case CROW_PET:		RESETFIELDPET(pFIELD->dwID);	break;	// PetData
		case CROW_SUMMON:	RESETFIELDSUMMON(pFIELD->dwID);	break;	// SummonData
		case CROW_MATERIAL:	RESETFIELDMATERIAL(pFIELD->dwID);	break;	// Material
		};

		//	리스트에서 삭제.
		m_pGLGaeaServer->RELEASE_FIELDCROW ( pFIELD );

		pNode = pNode->pNext;
	};

	m_arrayFIELDLIST.DELALL ();

	return S_OK;
}

HRESULT GLChar::UpdateViewAround ()
{
//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	if ( !IsSTATE(EM_GETVA_AFTER) && !(m_pGLGaeaServer->IsReserveServerStop ()) )	return S_FALSE;

	int nX, nZ;

	++m_dwViewFrame;
	if ( m_dwViewFrame == UINT_MAX )	m_dwViewFrame = 0;

	// if ( m_dwViewFrame%2 == 1 ) return S_FALSE;

	//	Note : 현재 시야에 들어오는 CELL 모두 탐색.
	//
	nX = int ( m_vPos.x );
	nZ = int ( m_vPos.z );
	LANDQUADNODE* pQuadHead = NULL;
	BOUDRECT bRect(nX+MAX_VIEWRANGE,nZ+MAX_VIEWRANGE,nX-MAX_VIEWRANGE,nZ-MAX_VIEWRANGE);
	m_pLandMan->GetLandTree()->FindNodes ( bRect, m_pLandMan->GetLandTree()->GetRootNode(), &pQuadHead );

	//	Note : 새로 추가되는 노드와 존속 노드를 표시.
	//
	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_pLandMan->GetLandTree()->GetCellNum()>pQuadNode->dwCID&&"The Index exceed cell Size");

		// PET
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		PROFILE_BEGIN("pQuadNode->pData->m_SummonList");
		GLSUMMONNODE* pSummonNode = pQuadNode->pData->m_SummonList.m_pHead;
		for ( ; pSummonNode; pSummonNode = pSummonNode->pNext )
		{
			PGLSUMMONFIELD pSummon = pSummonNode->Data;
			nX = int(pSummon->m_vPos.x); nZ = int(pSummon->m_vPos.z);

			if ( pSummon->m_dwGUID>=m_pGLGaeaServer->GetMaxClient() )					continue;

			// 내팻 빼고
			if ( pSummon->m_dwGUID != m_dwSummonGUID && bRect.IsWithIn ( nX, nZ ) )
			{
				// 있으면 프레임 갱신
				if ( m_arrayFieldSummon[pSummon->m_dwGUID] )
				{
					m_arrayFieldSummon[pSummon->m_dwGUID]->dwFRAME = m_dwViewFrame;
				}
				else
				{
					// 클라이언트에 생성 메시지 전송
					GLMSG::SNET_SUMMON_DROP_SUMMON nmg;
					nmg.Data = pSummon->ReqNetMsg_Drop ();
					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, ( NET_MSG_GENERIC* )&nmg );

					PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

					//	리스트에 등록.
					SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

					//	값 설정.
					pFIELDCROW->emCROW = CROW_SUMMON;
					pFIELDCROW->dwID = pSummon->m_dwGUID;
					pFIELDCROW->dwFRAME = m_dwViewFrame;
					pFIELDCROW->pNODE = pNODE;

					//	배열에 등록.
					SETFIELDSUMMON ( pFIELDCROW );
				}
			}
		}
		PROFILE_END("pQuadNode->pData->m_SummonList");

		PROFILE_BEGIN("pQuadNode->pData->m_PETList");
		GLPETNODE* pPetNode = pQuadNode->pData->m_PETList.m_pHead;
		for ( ; pPetNode; pPetNode = pPetNode->pNext )
		{
			PGLPETFIELD pPet = pPetNode->Data;
			nX = int(pPet->m_vPos.x); nZ = int(pPet->m_vPos.z);

			if ( pPet->m_dwGUID>=m_pGLGaeaServer->GetMaxClient() )					continue;

			// 내팻 빼고
			if ( pPet->m_dwGUID != m_dwPetGUID && bRect.IsWithIn ( nX, nZ ) )
			{
				// 있으면 프레임 갱신
				if ( m_arrayFieldPET[pPet->m_dwGUID] )
				{
					m_arrayFieldPET[pPet->m_dwGUID]->dwFRAME = m_dwViewFrame;
				}
				else
				{
					// 클라이언트에 생성 메시지 전송
					NET_MSG_GENERIC* pNetMsgDrop = pPet->ReqNetMsg_Drop ();
					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (LPVOID)pNetMsgDrop );
					
					PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

					//	리스트에 등록.
					SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

					//	값 설정.
					pFIELDCROW->emCROW = CROW_PET;
					pFIELDCROW->dwID = pPet->m_dwGUID;
					pFIELDCROW->dwFRAME = m_dwViewFrame;
					pFIELDCROW->pNODE = pNODE;

					//	배열에 등록.
					SETFIELDPET ( pFIELDCROW );
				}
			}
		}
		PROFILE_END("pQuadNode->pData->m_PETList");
		
		PROFILE_BEGIN("pQuadNode->pData->m_PCList");
		//	Note : - Char - 
		//
		GLCHARNODE *pCharNode = pQuadNode->pData->m_PCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar *pPChar = pCharNode->Data;
			if( !pPChar ) continue;

			nX = int(pPChar->m_vPos.x); nZ = int(pPChar->m_vPos.z);

			if ( pPChar->m_dwClientID != m_dwClientID && bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pPChar->m_dwGaeaID;
				if ( dwID>=m_pGLGaeaServer->GetMaxClient() )					continue;

				if ( m_arrayFieldPC[dwID] )
				{
					m_arrayFieldPC[dwID]->dwFRAME = m_dwViewFrame;
				}
				else
				{
					NET_MSG_GENERIC* pNetMsgDrop = pPChar->ReqNetMsg_Drop ();
					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, pNetMsgDrop );

					if ( pPChar->m_sPMarket.IsOpen() )
					{
						GLMSG::SNETPC_PMARKET_OPEN_BRD NetMsgPMarket;
						NetMsgPMarket.dwGaeaID = pPChar->m_dwGaeaID;
						StringCchCopy ( NetMsgPMarket.szPMarketTitle, CHAT_MSG_SIZE+1, pPChar->m_sPMarket.GetTitle().c_str() );
						m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgPMarket );
					}

					PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

					//	리스트에 등록.
					SGLNODE<SFIELDCROW*>* pNODE = m_arrayPC_FIELDLIST.ADDTAIL ( pFIELDCROW );

					//	값 설정.
					pFIELDCROW->emCROW = CROW_PC;
					pFIELDCROW->dwID = dwID;
					pFIELDCROW->dwFRAME = m_dwViewFrame;
					pFIELDCROW->pNODE = pNODE;

					//	배열에 등록.
					SETFIELDPC ( pFIELDCROW );
				}
			}
		}
		PROFILE_END("pQuadNode->pData->m_PCList");

		PROFILE_BEGIN("pQuadNode->pData->m_CROWList");
		//	Note : - Crow - 
		//
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		GLCROWNODE *pCrowNode = pQuadNode->pData->m_CROWList.m_pHead;
		for ( ; pCrowNode; pCrowNode = pCrowNode->pNext )
		{
			PGLCROW pGLCrow = pCrowNode->Data;
			nX = int(pGLCrow->GetPosition().x); nZ = int(pGLCrow->GetPosition().z);

			if ( bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pGLCrow->m_dwGlobID;
				if ( dwID>=MAXCROW )												continue;

				if ( m_arrayFieldCROW[dwID] )
				{
					m_arrayFieldCROW[dwID]->dwFRAME = m_dwViewFrame;
				}
				else
				{
					NET_MSG_GENERIC* nmg = pGLCrow->ReqNetMsg_Drop ();
					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, nmg );

					PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

					//	리스트에 등록.
					SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

					//	편의상 MOB, NPC 모두 CROW_MOB으로 분류.
					pFIELDCROW->emCROW = CROW_MOB;
					pFIELDCROW->dwID = dwID;
					pFIELDCROW->dwFRAME = m_dwViewFrame;
					pFIELDCROW->pNODE = pNODE;

					//	배열에 등록.
					SETFIELDCROW(pFIELDCROW);
				}
			}
		}
		PROFILE_END("pQuadNode->pData->m_CROWList");


		PROFILE_BEGIN("pQuadNode->pData->m_MaterailList");
		//	Note : - Material - 
		//
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		GLMATERIALNODE *pMaterialNode = pQuadNode->pData->m_MaterialList.m_pHead;
		for ( ; pMaterialNode; pMaterialNode = pMaterialNode->pNext )
		{
			PGLMATERIAL pGLMaterial = pMaterialNode->Data;
			nX = int(pGLMaterial->GetPosition().x); nZ = int(pGLMaterial->GetPosition().z);

			if ( bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pGLMaterial->m_dwGlobID;
				if ( dwID>=MAXCROW )												continue;

				if ( m_arrayFieldMATERIAL[dwID] )
				{
					m_arrayFieldMATERIAL[dwID]->dwFRAME = m_dwViewFrame;
				}
				else
				{
					NET_MSG_GENERIC* nmg = pGLMaterial->ReqNetMsg_Drop ();
					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, nmg );

					PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

					//	리스트에 등록.
					SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

					pFIELDCROW->emCROW = CROW_MATERIAL;
					pFIELDCROW->dwID = dwID;
					pFIELDCROW->dwFRAME = m_dwViewFrame;
					pFIELDCROW->pNODE = pNODE;

					//	배열에 등록.
					SETFIELDMATERIAL(pFIELDCROW);
				}
			}
		}
		PROFILE_END("pQuadNode->pData->m_MaterailList");


		//	Note : - Item - 
		//
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		PROFILE_BEGIN("pQuadNode->pData->m_ItemList");
		ITEMDROPNODE *pItemNode = pQuadNode->pData->m_ItemList.m_pHead;
		for ( ; pItemNode; pItemNode = pItemNode->pNext )
		{
			PITEMDROP pItemDrop = pItemNode->Data;
			nX = int(pItemDrop->vPos.x); nZ = int(pItemDrop->vPos.z);

			if ( bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pItemDrop->dwGlobID;
				if ( dwID>=MAXITEM )												continue;

				if ( m_arrayFieldITEM[dwID] )
				{
					m_arrayFieldITEM[dwID]->dwFRAME = m_dwViewFrame;
				}
				else
				{
					static GLMSG::SNETDROP_ITEM sNetMsg;
					sNetMsg.Data.Assign ( pItemDrop->sDrop );
					sNetMsg.Data.fAge = pItemDrop->fAge;

					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (LPVOID) &sNetMsg );

					PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

					//	리스트에 등록.
					SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

					//	편의상 MOB, NPC 모두 CROW_MOB으로 분류.
					pFIELDCROW->emCROW = CROW_ITEM;
					pFIELDCROW->dwID = dwID;
					pFIELDCROW->dwFRAME = m_dwViewFrame;
					pFIELDCROW->pNODE = pNODE;


					//	배열에 등록.
					SETFIELDITEM(pFIELDCROW);
				}
			}
		}
		PROFILE_END("pQuadNode->pData->m_ItemList");

		//	Note : - Money - 
		//
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		PROFILE_BEGIN("pQuadNode->pData->m_MoneyList");
		MONEYDROPNODE *pMoneyNode = pQuadNode->pData->m_MoneyList.m_pHead;
		for ( ; pMoneyNode; pMoneyNode = pMoneyNode->pNext )
		{
			PMONEYDROP pMoneyDrop = pMoneyNode->Data;
			nX = int(pMoneyDrop->vPos.x); nZ = int(pMoneyDrop->vPos.z);

			if ( bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pMoneyDrop->dwGlobID;
				if ( dwID>=MAXMONEY )												continue;

				if ( m_arrayFieldMONEY[dwID] )
				{
					m_arrayFieldMONEY[dwID]->dwFRAME = m_dwViewFrame;
				}
				else
				{
					GLMSG::SNETDROP_MONEY sNetMsg;
					sNetMsg.fAge = pMoneyDrop->fAge;
					sNetMsg.dwGlobID = pMoneyDrop->sDrop.dwGlobID;
					sNetMsg.vPos = pMoneyDrop->sDrop.vPos;
					sNetMsg.lnAmount = pMoneyDrop->sDrop.lnAmount;
					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (LPVOID) &sNetMsg );

					PSFIELDCROW pFIELDCROW = m_pGLGaeaServer->NEW_FIELDCROW();

					//	리스트에 등록.
					SGLNODE<SFIELDCROW*>* pNODE = m_arrayFIELDLIST.ADDTAIL ( pFIELDCROW );

					//	편의상 MOB, NPC 모두 CROW_MOB으로 분류.
					pFIELDCROW->emCROW = CROW_MONEY;
					pFIELDCROW->dwID = dwID;
					pFIELDCROW->dwFRAME = m_dwViewFrame;
					pFIELDCROW->pNODE = pNODE;

					//	배열에 등록.
					SETFIELDMONEY(pFIELDCROW);
				}
			}
		}
		PROFILE_END("pQuadNode->pData->m_MoneyList");
	}

	//	Note : 제거될 개체들를 처리한다.
	//
	static GLMSG::SNETDROP_OUT NetMsgOut;
	NetMsgOut.RESET_CROW();

	//	Note : 제거될 FIELD CROW 처리.
	//
	PROFILE_BEGIN("m_arrayPC_FIELDLIST");
	FIELDCROWNODE* pNode = m_arrayPC_FIELDLIST.m_pHead;
	while ( pNode )
	{
		FIELDCROWNODE* pNodeDEL = pNode;
		pNode = pNode->pNext;

		SFIELDCROW* pFIELD = pNodeDEL->Data;
		if ( pFIELD->dwFRAME == m_dwViewFrame )	continue;

		//	제거될 개체 메시지에 등록,  갯수가 가득할 경우 실패.
		bool bOK = NetMsgOut.ADD_CROW(STARID(pFIELD->emCROW,pFIELD->dwID));
		if ( !bOK )
		{
			//	현재까지 모인 정보를 전송.
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgOut );

			//	다시 넣어줌.
			NetMsgOut.RESET_CROW();
			NetMsgOut.ADD_CROW(STARID(pFIELD->emCROW,pFIELD->dwID));
		}

		//	배열에서 삭제.
		RESETFIELDPC ( pFIELD->dwID );

		//	리스트에서 삭제.
		m_arrayPC_FIELDLIST.DELNODE ( pNodeDEL );
		m_pGLGaeaServer->RELEASE_FIELDCROW ( pFIELD );
	}
	PROFILE_END("m_arrayPC_FIELDLIST");

	PROFILE_BEGIN("m_arrayFIELDLIST");
	pNode = m_arrayFIELDLIST.m_pHead;
	while ( pNode )
	{
		FIELDCROWNODE* pNodeDEL = pNode;
		pNode = pNode->pNext;

		SFIELDCROW* pFIELD = pNodeDEL->Data;
		if ( pFIELD->dwFRAME == m_dwViewFrame )	continue;

		//	제거될 개체 메시지에 등록,  갯수가 가득할 경우 실패.
		bool bOK = NetMsgOut.ADD_CROW(STARID(pFIELD->emCROW,pFIELD->dwID));
		if ( !bOK )
		{
			//	현재까지 모인 정보를 전송.
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgOut );

			//	제거될 개체 메시지에 등록.
			NetMsgOut.RESET_CROW();
			NetMsgOut.ADD_CROW(STARID(pFIELD->emCROW,pFIELD->dwID));
		}

		//	배열에서 삭제.
		switch ( pFIELD->emCROW )
		{
		case CROW_MOB:		RESETFIELDCROW(pFIELD->dwID);	break;
		case CROW_ITEM:		RESETFIELDITEM(pFIELD->dwID);	break;
		case CROW_MONEY:	RESETFIELDMONEY(pFIELD->dwID);	break;
		case CROW_PET:		RESETFIELDPET(pFIELD->dwID);	break;	// PetData
		case CROW_SUMMON:	RESETFIELDSUMMON(pFIELD->dwID);	break;	// SummonData
		case CROW_MATERIAL:	RESETFIELDMATERIAL(pFIELD->dwID);	break;	// Material
		};

		//	리스트에서 삭제.
		m_arrayFIELDLIST.DELNODE ( pNodeDEL );
		m_pGLGaeaServer->RELEASE_FIELDCROW ( pFIELD );
	}
	PROFILE_END("m_arrayFIELDLIST");

	if ( NetMsgOut.GETAMOUNT() > 0 )
	{
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgOut );
	}

	return S_OK;
}

DWORD GLChar::ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const BOOL bShock )
{
	if ( !IsValidBody() )	return m_sHP.wNow;
	if ( m_sHP.wNow == 0 )	return m_sHP.wNow;

	//	Note : 공격 받을때 반응 감안?
	//		( emACrow, dwAID )
	WORD wDxHP = GLCHARLOGIC::RECEIVE_DAMAGE ( (WORD) dwDamage );

	//	Note : 충격 받음.
	//
	//TurnAction ( GLAT_SHOCK );

	m_sAssault.emCrow = emACrow;
	m_sAssault.dwID = dwAID;


	if ( wDxHP > 0 )
	{
		if ( emACrow == CROW_PC )
		{
			PGLCHAR pChar = m_pGLGaeaServer->GetChar ( dwAID );
			AddDamageLog ( m_cDamageLog, dwAID, pChar->m_dwUserID, wDxHP );
		}

		if ( m_dwPartyID!=PARTY_NULL )
		{
			GLMSG::SNET_PARTY_MBR_POINT NetMsg;
			NetMsg.dwGaeaID = m_dwGaeaID;
			NetMsg.sHP = m_sHP;
			NetMsg.sMP = m_sMP;
			m_pGLGaeaServer->SENDTOPARTYCLIENT ( m_dwPartyID, (NET_MSG_GENERIC*) &NetMsg );
		}
	}

	if ( IsACTION(GLAT_GATHERING) )
	{
		GLMSG::SNET_ACTION_BRD NetMsgBrd;
		NetMsgBrd.emCrow	= CROW_PC;
		NetMsgBrd.dwID		= m_dwGaeaID;
		NetMsgBrd.emAction	= GLAT_IDLE;

		//	Note : 주변의 Char 에게.
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgBrd );		
		TurnAction ( GLAT_IDLE );
	}

	return m_sHP.wNow;
}

void GLChar::ReceivePushPull ( const D3DXVECTOR3 &vMovePos )
{
	if ( !IsValidBody() )	return;

	//	Note : 밀려날 위치로 이동 시도.
	//
	BOOL bSucceed = m_actorMove.GotoLocation
	(
		D3DXVECTOR3(vMovePos.x,vMovePos.y+5,vMovePos.z),
		D3DXVECTOR3(vMovePos.x,vMovePos.y-5,vMovePos.z)
	);

	if ( bSucceed )
	{
		//	Note : 밀려나는 엑션 시작.
		//
		m_TargetID.vPos = vMovePos;
		TurnAction ( GLAT_PUSHPULL );

		//	Note : 밀리는 속도 설정.
		//
		m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );

		//	Note : 메시지 내용 설정.
		//
		GLMSG::SNET_PUSHPULL_BRD NetMsgBRD;
		NetMsgBRD.emCrow = GETCROW();
		NetMsgBRD.dwID = m_dwGaeaID;
		NetMsgBRD.vMovePos = vMovePos;

		//	Note : [자신에게] Msg를 전달.
		//
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgBRD );

		//	Note : [자신의 주변 Char] 에게 Msg를 전달.
		//
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBRD );
	}
}

// 이벤트나 탐닉 방지 시스템에 의해 변경된 경험치를 구한다.
int GLChar::CalculateReceveExp ( int nGenExp )
{
	float fGenExp = (float)nGenExp;
	int	  nCalculateExp = 0;
#if defined(VN_PARAM) //vietnamtest%%%

	if( m_dwVietnamGainType == GAINTYPE_HALF )
	{
		fGenExp /= 2.0f;
	}
	if( m_dwVietnamGainType == GAINTYPE_EMPTY )
	{
		fGenExp = 0.0f;
	}

	nCalculateExp = (int)fGenExp;

	if( m_dwVietnamGainType == GAINTYPE_EMPTY || m_dwVietnamGainType == GAINTYPE_HALF )
	{
		m_lVNGainSysExp += (nGenExp - nCalculateExp);
	}
#endif

#ifdef CH_PARAM_USEGAIN //chinaTest%%%
	if( m_ChinaGainType == GAINTYPE_HALF )
	{
		fGenExp /= 2.0f;
	}
	if( m_ChinaGainType == GAINTYPE_EMPTY )
	{
		fGenExp = 0;
	}

	nCalculateExp = (int)fGenExp;
#endif

#ifndef CH_PARAM_USEGAIN //** Add EventTime
	if( m_bEventStart && m_bEventApply )
	{
		SEventState sEventState = m_pGLGaeaServer->m_sEventState;
		if( sEventState.bEventStart )
		{
			fGenExp *= sEventState.fExpGainRate;
		}
	}
	nCalculateExp = (int)fGenExp;
#endif

	return nCalculateExp;
}

// *****************************************************
// Desc: 경험치 획득
// *****************************************************
void GLChar::ReceiveExp ( int nGenExp, const bool bupdate_msg )
{
	if ( !IsValidBody() )	return;

	// 파티든 아니든 경험치 계산은 마지막으로 여기서한다!
	//NeedToSaveValue _A
	float fTempExp = (float)nGenExp;
	int nMemExp = (int)(fTempExp * m_pGLGaeaServer->GetExpGainRate(m_CHARINDEX,m_wLevel));

	nGenExp = CalculateReceveExp( nMemExp );

	m_sExperience.lnNow += DWORD( nGenExp*m_fEXP_RATE );


#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) ||  defined(HK_PARAM) // ***Tracing Log print
	if( nGenExp != 0 && m_bTracingUser )
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

		CString strTemp;
		strTemp.Format( "Receive Exp, [%s][%s], Receive exp amount[%d]", m_szUID, m_szName, nGenExp );

		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
	}
#endif
}

// *****************************************************
// Desc: 경험치 획득(파티)
// *****************************************************
void GLChar::ReceivePartyExp ( const int nGenExp )
{
	if ( !IsValidBody() )	return;

	ReceiveExp ( nGenExp, true );
}

// *****************************************************
// Desc: 상대를 죽여서 발생하는 경험치 (파티 or 싱글)
// *****************************************************
void GLChar::ReceiveKillExp( const STARGETID &cTargetID, bool bConftSchool/* =false */, bool bSummonReceive/* =false */ )
{
	if ( !IsValidBody() )	return;

	//	Note : 파티일 경우.
	//
	if ( m_dwPartyID!=PARTY_NULL )
	{
		GLPARTY_FIELD *pParty = m_pGLGaeaServer->GetParty ( m_dwPartyID );
		if ( pParty )
		{
			//	Note : 경험치 수신 가능한 파티 인원.
			std::vector<DWORD> vecPARTY;
			vecPARTY.reserve ( MAXPARTY );

			//	Note : 경험치 수신 가능한 파티인원. ( 자신 포함. )
			//
			WORD wLEVEL_TOTAL(0);
			WORD wLEVEL_LOWER(USHRT_MAX);
			GLPARTY_FIELD::MEMBER_ITER iter = pParty->m_cMEMBER.begin ();
			GLPARTY_FIELD::MEMBER_ITER iter_end = pParty->m_cMEMBER.end ();
			for ( ; iter!=iter_end; ++iter )
			{
				DWORD dwPARTY_MBR = (*iter).first;
				PGLCHAR pChar = m_pGLGaeaServer->GetChar ( dwPARTY_MBR );
				if ( !pChar || pChar->m_pLandMan!=m_pLandMan )		continue;
				if ( !pChar->IsValidBody() )						continue;
			
				D3DXVECTOR3 vDis = GetPosition() - pChar->GetPosition();
				float fDis = D3DXVec3Length ( &vDis );
				if ( fDis > GLCONST_CHAR::fPARTYEXPRANGE )			continue;
		
				wLEVEL_TOTAL += pChar->m_wLevel;
				if ( wLEVEL_LOWER>pChar->m_wLevel )		wLEVEL_LOWER = pChar->m_wLevel;

				vecPARTY.push_back ( dwPARTY_MBR );
			}

			DWORD dwINDEX = DWORD(vecPARTY.size());
			if ( dwINDEX>0 )	--dwINDEX;
			GASSERT(dwINDEX<MAXPARTY);

			float fEXP_ONE(0), fEXP_MBR(0);
			fEXP_ONE = float ( GLCONST_CHAR::aPARTYKILLEXP_ONE[dwINDEX] * 0.01f );

			//	Note : 경험치 계산.
			int nGenExp = GLOGICEX::CALCKILLEXP ( wLEVEL_LOWER, cTargetID, m_pLandMan );
			if ( bConftSchool )		nGenExp = int ( nGenExp * GLCONST_CHAR::fCONFRONT_SCHOOL_EXP_SCALE );
			// 소환수가 경험치 획득했을 겨우 1/3만 획득
			if ( bSummonReceive ) nGenExp /= 3; 

			//	'경험치'를 파티 경험치로 환산.
			nGenExp = int ( nGenExp * fEXP_ONE );

			//	Note : 각각 파티원에게 맞게 경험치 발생.
			//
			DWORD dwSIZE = (DWORD) vecPARTY.size();
			for ( DWORD i=0; i<dwSIZE; ++i )
			{
				DWORD dwPARTY_MBR = vecPARTY[i];
				PGLCHAR pChar = m_pGLGaeaServer->GetChar ( dwPARTY_MBR );
				if ( !pChar )						continue;

				int nMemExp = int ( nGenExp * pChar->m_wLevel / float(wLEVEL_TOTAL) );
				
				//	레벨차이(수신자렙-최저렙)에 감소율을 반영하여 경험치를 감소시킴.
				nMemExp -= int ( nMemExp * ( pChar->m_wLevel - wLEVEL_LOWER ) * GLCONST_CHAR::fPARTYEXP_S );

				// 아이템 에디트에서 입력한 경험치 배율 적용 (준혁)
				nMemExp = int( (float)nMemExp * pChar->GetExpMultipleRateFromSlotItem() );

				if ( nMemExp < 0 )		nMemExp = 0;	//	수신 경험치가 - 가 되지 않도록.

				pChar->ReceivePartyExp ( nMemExp );
			}
		}
	}
	else
	{
		//*/*/*/*//	Note : 경험치 계산.
		int nGenExp = GLOGICEX::CALCKILLEXP ( GETLEVEL(), cTargetID, m_pLandMan );
		if ( bConftSchool )		nGenExp = int ( nGenExp * GLCONST_CHAR::fCONFRONT_SCHOOL_EXP_SCALE );
		// 소환수가 경험치 획득했을 겨우 1/3만 획득
		if ( bSummonReceive ) nGenExp /= 3; 

		// 아이템 에디트에서 입력한 경험치 배율 적용 (준혁)
		int nMyGenExp = int( (float)nGenExp * GetExpMultipleRateFromSlotItem() );

		/*if( m_wSex == 0 )
		{
			nMyGenExp *= 2;
		}*/

		//	Note : 경험치 증가 처리.
		//
		ReceiveExp ( nMyGenExp, true );
	}

	//	Note : quest와 연결된 처리.
	//
	if ( cTargetID.emCrow==CROW_MOB )
	{
		PGLCROW pCROW = m_pLandMan->GetCrow ( cTargetID.dwID );
		if ( pCROW )
		{
			DoQuestMobKill ( pCROW->m_sNativeID );
		}
	}
}

// *****************************************************
// Desc: 상대를 때려서 발생하는 경험치 (파티 or 싱글)
// *****************************************************
void GLChar::ReceiveAttackExp( const STARGETID &cTargetID, DWORD dwDamage, BOOL bPartySkill, bool bConftSchool/* =false */, 
							   bool bSummonReceive/* =false */ )
{
	if ( !IsValidBody() )	return;

	//	Note : 파티일 경우.
	//
	if ( m_dwPartyID!=PARTY_NULL )
	{
		GLPARTY_FIELD *pParty = m_pGLGaeaServer->GetParty ( m_dwPartyID );
		if ( pParty )
		{
			//	Note : 경험치 수신 가능한 파티 인원.
			std::vector<DWORD> vecPARTY;
			vecPARTY.reserve ( MAXPARTY );

			//	Note : 경험치 수신 가능한 파티인원. ( 자신 포함. )
			//
			WORD wLEVEL_TOTAL(0);
			WORD wLEVEL_LOWER(USHRT_MAX);
			GLPARTY_FIELD::MEMBER_ITER iter = pParty->m_cMEMBER.begin ();
			GLPARTY_FIELD::MEMBER_ITER iter_end = pParty->m_cMEMBER.end ();
			for ( ; iter!=iter_end; ++iter )
			{
				DWORD dwPARTY_MBR = (*iter).first;
				PGLCHAR pChar = m_pGLGaeaServer->GetChar ( dwPARTY_MBR );
				if ( !pChar || pChar->m_pLandMan!=m_pLandMan )		continue;
				if ( !pChar->IsValidBody() )						continue;
			
				D3DXVECTOR3 vDis = GetPosition() - pChar->GetPosition();
				float fDis = D3DXVec3Length ( &vDis );
				if ( fDis > GLCONST_CHAR::fPARTYEXPRANGE )			continue;
		
				wLEVEL_TOTAL += pChar->m_wLevel;
				if ( wLEVEL_LOWER>pChar->m_wLevel )		wLEVEL_LOWER = pChar->m_wLevel;

				vecPARTY.push_back ( dwPARTY_MBR );
			}

			DWORD dwINDEX = DWORD(vecPARTY.size());
			if ( dwINDEX>0 )	--dwINDEX;
			GASSERT(dwINDEX<MAXPARTY);

			float fEXP_ONE(0);
			fEXP_ONE = float ( GLCONST_CHAR::aPARTYEXP_ONE[dwINDEX] * 0.01f );

			//	Note : 경험치량 산출.
			int nGenExp = GLOGICEX::CALCATTACKEXP ( wLEVEL_LOWER, cTargetID, m_pLandMan, dwDamage );
			if ( bConftSchool )		nGenExp = int ( nGenExp * GLCONST_CHAR::fCONFRONT_SCHOOL_EXP_SCALE );
			// 소환수가 획득한 경험치는 1/3로 한다.
			if ( bSummonReceive )	nGenExp /= 3;

			//	'경험치'를 직접 발생시킨 당사자의 경험치량 계산.
			nGenExp = int ( nGenExp * fEXP_ONE );

			//	Note : 각각 파티원에게 맞게 경험치 발생.
			//
			DWORD dwSIZE = (DWORD) vecPARTY.size();
			for ( DWORD i=0; i<dwSIZE; ++i )
			{
				DWORD dwPARTY_MBR = vecPARTY[i];
				PGLCHAR pChar = m_pGLGaeaServer->GetChar ( dwPARTY_MBR );
				if ( !pChar )						continue;

				//	파티 분배 경험치.
				int nMemExp = int ( nGenExp * pChar->m_wLevel / float(wLEVEL_TOTAL) );
				//	레벨차이(수신자렙-최저렙)에 감소율을 반영하여 경험치를 감소시킴.
				nMemExp -= int ( nMemExp * ( pChar->m_wLevel - wLEVEL_LOWER ) * GLCONST_CHAR::fPARTYEXP_S );

				// 아이템 에디트에서 입력한 경험치 배율 적용 (준혁)
				nMemExp = int( (float)nMemExp * pChar->GetExpMultipleRateFromSlotItem() );

				if ( nMemExp < 0 )	nMemExp = 0;	//	수신 경험치가 - 가 되지 않도록.

#if defined(TH_PARAM) || defined(MYE_PARAM) || defined(MY_PARAM)
				// 파티일 경우의 사이버 카페 경험치
				if( pChar->m_dwThaiCCafeClass == 1 || pChar->m_nMyCCafeClass == 1 ) // A등급
				{
					float fTemp = (float)nMemExp;
					fTemp *= 1.5f;
					nMemExp = (int)fTemp;
				}
				else if( pChar->m_dwThaiCCafeClass == 2 || pChar->m_nMyCCafeClass == 2 ) // B등급
				{
					float fTemp = (float)nMemExp;
					fTemp *= 1.3f;
					nMemExp = (int)fTemp;
				}else if( pChar->m_dwThaiCCafeClass == 3 || pChar->m_nMyCCafeClass == 3 ) // C등급
				{
					float fTemp = (float)nMemExp;
					fTemp *= 1.2f;
					nMemExp = (int)fTemp;
				}else if( pChar->m_nMyCCafeClass == 4 )
				{
					float fTemp = (float)nMemExp;
					fTemp *= 1.5f;
					nMemExp = (int)fTemp;
				}
#endif

				pChar->ReceivePartyExp ( nMemExp );

			}
		}
	}
	else
	{
		int nGenExp = GLOGICEX::CALCATTACKEXP ( GETLEVEL(), cTargetID, m_pLandMan, dwDamage );
		if ( bConftSchool )		nGenExp = int ( nGenExp * GLCONST_CHAR::fCONFRONT_SCHOOL_EXP_SCALE );
		// 소환수가 획득한 경험치는 1/3로 한다.
		if ( bSummonReceive )	nGenExp /= 3;

		// 아이템 에디트에서 입력한 경험치 배율 적용 (준혁)
		int nMyGenExp = int( (float)nGenExp * GetExpMultipleRateFromSlotItem() );

#if defined(TH_PARAM) || defined(MYE_PARAM ) || defined(MY_PARAM)
		// 솔로일 경우의 사이버 카페 경험치
		if( m_dwThaiCCafeClass == 1 || m_nMyCCafeClass == 1 ) // A등급
		{
			float fTemp = (float)nMyGenExp;
			fTemp *= 1.5f;
			nMyGenExp = (int)fTemp;
		}
		else if( m_dwThaiCCafeClass == 2 || m_nMyCCafeClass == 2 ) // B등급
		{
			float fTemp = (float)nMyGenExp;
			fTemp *= 1.3f;
			nMyGenExp = (int)fTemp;
		}else if( m_dwThaiCCafeClass == 3 || m_nMyCCafeClass == 3 ) // C등급
		{
			float fTemp = (float)nMyGenExp;
			fTemp *= 1.2f;
			nMyGenExp = (int)fTemp;
		}else if( m_nMyCCafeClass == 4 )
		{
			float fTemp = (float)nMyGenExp;
			fTemp *= 1.5f;
			nMyGenExp = (int)fTemp;
		}
#endif
		ReceiveExp ( nMyGenExp, true );
	}
}

HRESULT GLChar::CheckInstanceItem ()
{
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pRHAND = GET_SLOT_ITEMDATA ( emRHand );
	SITEM* pLHAND = GET_SLOT_ITEMDATA ( emLHand );

	//	사용가능 횟수 많큼 사용시에 모두 소진되어 아이탬 사라지게 만듬.
	if ( pLHAND && pLHAND->sDrugOp.bInstance )
	{
		if ( GET_SLOT_ITEM(emLHand).wTurnNum==0 )
		{
			//	아이탬 제거.
			RELEASE_SLOT_ITEM(emLHand);

				//	[자신에게] SLOT에 있었던 아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(emLHand);
			NetMsg_PutOn_Release.bRefresh = true;
			m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Release);
		}
		else
		{
			//	[자신에게] 인밴 아이탬 소모됨.
			GLMSG::SNETPC_PUTON_DRUG_UPDATE NetMsg_PutOn_Update;
			NetMsg_PutOn_Update.emSlot = emLHand;
			NetMsg_PutOn_Update.wTurnNum = GET_SLOT_ITEM(emLHand).wTurnNum;
			m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Update);
		}
	}

	if ( pRHAND && pRHAND->sDrugOp.bInstance )
	{
		if ( GET_SLOT_ITEM(emRHand).wTurnNum )
		{
			//	아이탬 제거.
			RELEASE_SLOT_ITEM(emRHand);

				//	[자신에게] SLOT에 있었던 아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(emRHand);
			NetMsg_PutOn_Release.bRefresh = true;
			m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Release);
		}
		else
		{
			//	[자신에게] 인밴 아이탬 소모됨.
			GLMSG::SNETPC_PUTON_DRUG_UPDATE NetMsg_PutOn_Update;
			NetMsg_PutOn_Update.emSlot = emRHand;
			NetMsg_PutOn_Update.wTurnNum = GET_SLOT_ITEM(emRHand).wTurnNum;
			m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Update);
		}
	}

	return S_OK;
}

void GLChar::ReceiveLivingPoint ( int nVerLP )
{
	if ( !IsValidBody() )	return;

	//	Note : 생활 점수 변경.
	//
	m_nLiving += nVerLP;
	
	//	Note : 생활점수 클라이언트에 반영.
	//
	GLMSG::SNETPC_UPDATE_LP NetMsg;
	NetMsg.nLP = m_nLiving;
	m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
}

BOOL GLChar::RecieveQItemFact ( const SNATIVEID nidITEM )
{
	// 탈것 탑승시 적용안됨
	if ( m_bVehicle ) return FALSE;

	RECEIVE_QITEMFACT ( nidITEM );

	GLMSG::SNETPC_QITEMFACT_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.sFACT = m_sQITEMFACT;
	m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgBrd);

	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	return TRUE;
}

void GLChar::ReceiveEventEx( const EMGM_EVENT_TYPE emType, const WORD wValue )
{
	//	Memo :	wValue 값이 0이면 이벤트 종료를 의미한다.
	if( wValue > 0 )
	{
		RECEIVE_EVENTFACT_BEGIN( emType, wValue );

		GLMSG::SNETPC_EVENTFACT_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		NetMsgBrd.emType = emType;
		NetMsgBrd.wValue = wValue;

		m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgBrd);
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
	}
	else
	{
		RECEIVE_EVENTFACT_END( emType );
		
		GLMSG::SNETPC_EVENTFACT_END_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		NetMsgBrd.emType = emType;

		m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgBrd);
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
	}
}

void GLChar::ReceiveBeginEvent ( const SEventState emType )
{
	//** Add EventTime
	// 이벤트 시간을 다시 셋팅한다.
	CTime	  crtTime      = CTime::GetCurrentTime();
	m_sEventTime.loginTime = crtTime.GetTime();
	m_bEventStart		   = FALSE;
	m_bEventApply		   = FALSE;

	GLMSG::SNET_GM_LIMIT_EVENT_BEGIN_FB NetMsgEventBeginFb;
	NetMsgEventBeginFb.start_Lv		= emType.MinEventLevel;
	NetMsgEventBeginFb.end_Lv	    = emType.MaxEventLevel;
	NetMsgEventBeginFb.play_Time     = emType.EventPlayTime;
	NetMsgEventBeginFb.buster_Time   = emType.EventBusterTime;
	NetMsgEventBeginFb.expGain_Rate  = emType.fExpGainRate;
	NetMsgEventBeginFb.itemGain_Rate = emType.fItemGainRate;

	if( emType.MinEventLevel <= GETLEVEL() && emType.MaxEventLevel >= GETLEVEL() )
	{
		m_bEventApply = TRUE;
	}


	m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgEventBeginFb);
}

void GLChar::ReceiveVietnam( const __time64_t initTime )
{
	m_dwVietnamGainType = GAINTYPE_MAX;
	m_sVietnamSystem.Init();
	m_sVietnamSystem.loginTime = initTime;
	m_sVietnamSystem.gameTime  = 0;

	GLMSG::SNETPC_VIETNAM_ALLINITTIME NetMsg;
	NetMsg.initTime = initTime;
	m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
}

HRESULT GLChar::MsgClubStorage ()
{
	GLCLUB *pCLUB = m_pGLGaeaServer->GetClubMan().GetClub ( m_dwGuild );
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->m_dwMasterID != m_dwCharID )
	{
		return S_OK;
	}

	//	Note : 클럽 창고 정보 리샛.
	//
	GLMSG::SNET_CLUB_STORAGE_RESET NetMsgClubStorageReset;
	m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgClubStorageReset );

	//	Note : 창고의 기본 정보 전송.
	//
	GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgMoney;
	NetMsgMoney.lnMoney = pCLUB->m_lnStorageMoney;
	m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgMoney);

	//	Note : 아이템을 하나씩 전송.
	//
	GLMSG::SNET_CLUB_GETSTORAGE_ITEM NetMsgItem;

	// 클라이언트에 창고 전송
	// 루프를 돌면서 하나씩 전송

	DWORD dwRANK = pCLUB->m_dwRank;
	if ( dwRANK >= MAX_CLUBSTORAGE )	dwRANK = MAX_CLUBSTORAGE-1;

	for ( DWORD i=0; i<=dwRANK; ++i )
	{
		NetMsgItem.dwChannel = i;

		GLInventory::CELL_MAP* pInvenList = pCLUB->m_cStorage[i].GetItemList();

		GLInventory::CELL_MAP_ITER iter = pInvenList->begin();
		GLInventory::CELL_MAP_ITER iter_end = pInvenList->end();
		for ( ; iter!=iter_end; ++iter )
		{
			SINVENITEM* pInvenItem = (*iter).second;

			NetMsgItem.Data = *pInvenItem;
			
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgItem );
		}
	}

	//	Note : 클럽의 수익 갱신 요청.
	MsgReqClubInComeReNew(NULL);

	return S_OK;
}

bool GLChar::IsCHATBLOCK ()
{
	if ( m_tCHATBLOCK==0 )	return false;

	CTime cBLOCK(m_tCHATBLOCK);
	CTime cCUR(CTime::GetCurrentTime());

	//	블럭시간인지 검사.
	if ( cBLOCK > cCUR )	return true;

	//	시간이 초과 되었을 경우는 시간을 리셋.
	m_tCHATBLOCK = 0;

	return false;
}

HRESULT GLChar::MsgGetFieldAllItem ( NET_MSG_GENERIC* nmg )
{
//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	if ( !IsSTATE(EM_GETVA_AFTER) && !(m_pGLGaeaServer->IsReserveServerStop ()) )	return S_FALSE;

	GLMSG::SNETPET_REQ_GETRIGHTOFITEM_FB NetMsg;

	WORD i(0);

	PGLPETFIELD pMyPet = m_pGLGaeaServer->GetPET ( m_dwPetGUID );
	if ( !pMyPet || !pMyPet->IsValid () )	return E_FAIL;

	int nX = int ( pMyPet->m_vPos.x );
	int nZ = int ( pMyPet->m_vPos.z );
	LANDQUADNODE* pQuadHead = NULL;
	BOUDRECT bRect(nX+GLCONST_PET::nMAXVIEWRANGE,nZ+GLCONST_PET::nMAXVIEWRANGE,nX-GLCONST_PET::nMAXVIEWRANGE,nZ-GLCONST_PET::nMAXVIEWRANGE);
	m_pLandMan->GetLandTree()->FindNodes ( bRect, m_pLandMan->GetLandTree()->GetRootNode(), &pQuadHead );

	//	Note : 새로 추가되는 노드와 존속 노드를 표시.
	//
	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		//	Note : - Item - 
		//
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		ITEMDROPNODE *pItemNode = pQuadNode->pData->m_ItemList.m_pHead;
		for ( ; pItemNode; pItemNode = pItemNode->pNext )
		{
			PITEMDROP pItemDrop = pItemNode->Data;
			if ( !pItemDrop )
			{
				DEBUGMSG_WRITE ( "Item is Lost in MsgGetFieldAllItem()" );
				continue;
			}
			nX = int(pItemDrop->vPos.x); nZ = int(pItemDrop->vPos.z);

			if ( bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pItemDrop->dwGlobID;
				if ( dwID>=MAXITEM ) continue;

				//	아이탬이 다른 사람에게 예약되어 있으면
				if ( !pItemDrop->IsTakeItem(m_dwPartyID,m_dwGaeaID) ) continue;

				SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
				if ( !pItem ) continue;

/*
				WORD wPosX, wPosY;
				BOOL bOk = m_cInventory.FindInsrtable ( 
					pItem->sBasicOp.wInvenSizeX, 
					pItem->sBasicOp.wInvenSizeY, 
					wPosX, 
					wPosY );

				if ( !bOk )	
				{
					// 팻을 제거한다
					// m_pGLGaeaServer->DropOutPET ( m_dwPetGUID, false );

					//	인밴이 가득차서 아이탬을 넣을수 없음을 알림 
					GLMSG::SNETPET_MSG_NOTENOUGHINVEN NetMsg;
					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );
					return S_OK;
				}
*/
				// ?아이템이면
				if ( pItem->sBasicOp.emItemType==ITEM_QITEM ) continue;

				// 순차습득시 파티원이 순차습득되지 않는 버그로 인해
				// 파타에 대한 검사는 여기서 하지 않고 실제 아이템 줍는
				// 행위에서 처리하도록 수정. 
/*
				// 파티원이고 이벤트 아이템이 아닐때 파티 루팅인지 검사후 처리.
				GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
				if ( pParty && !pItem->ISEVENTITEM() )
				{
					bool ballot = pParty->DOITEM_ALLOT ( this, pItemDrop, true );
					if ( !ballot ) continue;
				}
*/
				NetMsg.DroppedItems[i].dwID		 = dwID;
				NetMsg.DroppedItems[i].vPos		 = pItemDrop->vPos;
				NetMsg.DroppedItems[i].emCrow	 = CROW_ITEM;
				if ( ++i >= MAXRIGHTOFITEM )
				{
					NetMsg.wSum = MAXRIGHTOFITEM;
					m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
					i = 0;
				}
	
			}
		}

		//	Note : - Money - 
		//
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		MONEYDROPNODE *pMoneyNode = pQuadNode->pData->m_MoneyList.m_pHead;
		for ( ; pMoneyNode; pMoneyNode = pMoneyNode->pNext )
		{
			PMONEYDROP pMoneyDrop = pMoneyNode->Data;
			if ( !pMoneyDrop ) 
			{
				DEBUGMSG_WRITE ( "Money is Lost in MsgGetFieldAllItem()" );
				continue;
			}
			nX = int(pMoneyDrop->vPos.x); nZ = int(pMoneyDrop->vPos.z);

			if ( bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pMoneyDrop->dwGlobID;
				if ( dwID>=MAXMONEY ) continue;

				//	아이탬이 다른 사람에게 예약되어 있으면
				if ( !pMoneyDrop->IsTakeItem(m_dwPartyID,m_dwGaeaID) ) continue;

				// 순차습득시 파티원이 순차습득되지 않는 버그로 인해
				// 파타에 대한 검사는 여기서 하지 않고 실제 아이템 줍는
				// 행위에서 처리하도록 수정. 
/*
				// 파티 루팅검사
				GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
				if ( pParty )
				{
					bool ballot = pParty->DOMONEY_ALLOT ( this, pMoneyDrop, m_pLandMan, true );
					if ( !ballot ) continue;
				}
*/
				NetMsg.DroppedItems[i].dwID		 = dwID;
				NetMsg.DroppedItems[i].vPos		 = pMoneyDrop->vPos;
				NetMsg.DroppedItems[i].emCrow	 = CROW_MONEY;
				if ( ++i >= MAXRIGHTOFITEM )
				{
					NetMsg.wSum = MAXRIGHTOFITEM;
					m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
					i = 0;
				}
			}
		}
	}

	if ( i > 0 )
	{
		NetMsg.wSum = i;
		m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
	}

	return S_OK;
}

HRESULT GLChar::MsgGetFieldRareItem ( NET_MSG_GENERIC* nmg )
{
//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	if ( !IsSTATE(EM_GETVA_AFTER) && !(m_pGLGaeaServer->IsReserveServerStop ()) )	return S_FALSE;

	GLMSG::SNETPET_REQ_GETRIGHTOFITEM_FB NetMsg;

	WORD i(0);

	PGLPETFIELD pMyPet = m_pGLGaeaServer->GetPET ( m_dwPetGUID );
	if ( !pMyPet || !pMyPet->IsValid () )	return E_FAIL;

	int nX = int ( pMyPet->m_vPos.x );
	int nZ = int ( pMyPet->m_vPos.z );
	LANDQUADNODE* pQuadHead = NULL;
	BOUDRECT bRect(nX+GLCONST_PET::nMAXVIEWRANGE,nZ+GLCONST_PET::nMAXVIEWRANGE,nX-GLCONST_PET::nMAXVIEWRANGE,nZ-GLCONST_PET::nMAXVIEWRANGE);
	m_pLandMan->GetLandTree()->FindNodes ( bRect, m_pLandMan->GetLandTree()->GetRootNode(), &pQuadHead );

	//	Note : 새로 추가되는 노드와 존속 노드를 표시.
	//
	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
        //	Note : - Item - 
		//
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		ITEMDROPNODE *pItemNode = pQuadNode->pData->m_ItemList.m_pHead;
		for ( ; pItemNode; pItemNode = pItemNode->pNext )
		{
			PITEMDROP pItemDrop = pItemNode->Data;
			if ( !pItemDrop ) 
			{
				DEBUGMSG_WRITE ( "Item is Lost in MsgGetFieldRareItem()" );
				continue;
			}
			nX = int(pItemDrop->vPos.x); nZ = int(pItemDrop->vPos.z);

			if ( bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pItemDrop->dwGlobID;
				if ( dwID>=MAXITEM ) continue;

				//	아이탬이 다른 사람에게 예약되어 있으면
				if ( !pItemDrop->IsTakeItem(m_dwPartyID,m_dwGaeaID) ) continue;

				SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
				if ( !pItem ) continue;
/*
				WORD wPosX, wPosY;
				BOOL bOk = m_cInventory.FindInsrtable ( 
					pItem->sBasicOp.wInvenSizeX, 
					pItem->sBasicOp.wInvenSizeY, 
					wPosX, 
					wPosY );

				if ( !bOk )	
				{
					// 팻을 제거한다
					//m_pGLGaeaServer->DropOutPET ( m_dwPetGUID, false );

					//	인밴이 가득차서 아이탬을 넣을수 없음을 알림 
					GLMSG::SNETPET_MSG_NOTENOUGHINVEN NetMsg;
					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );
					return S_OK;
				}
*/
				// 레어아이템이 아니면
				if ( pItem->sBasicOp.emLevel != LEVEL_RARE ) continue;

				// 순차습득시 파티원이 순차습득되지 않는 버그로 인해
				// 파타에 대한 검사는 여기서 하지 않고 실제 아이템 줍는
				// 행위에서 처리하도록 수정. 
/*
				// 파티원이고 이벤트 아이템이 아닐때 파티 루팅인지 검사후 처리.
				GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
				if ( pParty && !pItem->ISEVENTITEM() )
				{
					bool ballot = pParty->DOITEM_ALLOT ( this, pItemDrop, true );
					if ( !ballot ) continue;
				}
*/
				NetMsg.DroppedItems[i].dwID		 = dwID;
				NetMsg.DroppedItems[i].vPos		 = pItemDrop->vPos;
				NetMsg.DroppedItems[i].emCrow	 = CROW_ITEM;
				if ( ++i >= MAXRIGHTOFITEM )
				{
					NetMsg.wSum = MAXRIGHTOFITEM;
					m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
					return S_OK;
				}
			}
		}
	}

	if ( i > 0 )
	{
		NetMsg.wSum = i;
		m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
	}

	return S_OK;
}
HRESULT GLChar::MsgGetFieldPotions ( NET_MSG_GENERIC* nmg )
{
//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	if ( !IsSTATE(EM_GETVA_AFTER) && !(m_pGLGaeaServer->IsReserveServerStop ()) )	return S_FALSE;

	GLMSG::SNETPET_REQ_GETRIGHTOFITEM_FB NetMsg;

	WORD i(0);

	PGLPETFIELD pMyPet = m_pGLGaeaServer->GetPET ( m_dwPetGUID );
	if ( !pMyPet || !pMyPet->IsValid () )	return E_FAIL;

	int nX = int ( pMyPet->m_vPos.x );
	int nZ = int ( pMyPet->m_vPos.z );
	LANDQUADNODE* pQuadHead = NULL;
	BOUDRECT bRect(nX+GLCONST_PET::nMAXVIEWRANGE,nZ+GLCONST_PET::nMAXVIEWRANGE,nX-GLCONST_PET::nMAXVIEWRANGE,nZ-GLCONST_PET::nMAXVIEWRANGE);
	m_pLandMan->GetLandTree()->FindNodes ( bRect, m_pLandMan->GetLandTree()->GetRootNode(), &pQuadHead );

	//	Note : 새로 추가되는 노드와 존속 노드를 표시.
	//
	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
        //	Note : - Item - 
		//
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		ITEMDROPNODE *pItemNode = pQuadNode->pData->m_ItemList.m_pHead;
		for ( ; pItemNode; pItemNode = pItemNode->pNext )
		{
			PITEMDROP pItemDrop = pItemNode->Data;
			if ( !pItemDrop )
			{
				DEBUGMSG_WRITE ( "Item is Lost in MsgGetFieldPotions()" );
				continue;
			}
			nX = int(pItemDrop->vPos.x); nZ = int(pItemDrop->vPos.z);

			if ( bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pItemDrop->dwGlobID;
				if ( dwID>=MAXITEM ) continue;

				//	아이탬이 다른 사람에게 예약되어 있으면
				if ( !pItemDrop->IsTakeItem(m_dwPartyID,m_dwGaeaID) ) continue;

				SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
				if ( !pItem ) continue;
/*
				WORD wPosX, wPosY;
				BOOL bOk = m_cInventory.FindInsrtable ( 
					pItem->sBasicOp.wInvenSizeX, 
					pItem->sBasicOp.wInvenSizeY, 
					wPosX, 
					wPosY );

				if ( !bOk )	
				{
					// 팻을 제거한다
					//m_pGLGaeaServer->DropOutPET ( m_dwPetGUID, false );

					//	인밴이 가득차서 아이탬을 넣을수 없음을 알림 
					GLMSG::SNETPET_MSG_NOTENOUGHINVEN NetMsg;
					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );
					return S_OK;
				}
*/
				// 포션이 아니면
				if ( pItem->sBasicOp.emItemType != ITEM_CURE ) continue;

				// 순차습득시 파티원이 순차습득되지 않는 버그로 인해
				// 파타에 대한 검사는 여기서 하지 않고 실제 아이템 줍는
				// 행위에서 처리하도록 수정. 

/*
				// 파티원이고 이벤트 아이템이 아닐때 파티 루팅인지 검사후 처리.
				GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
				if ( pParty && !pItem->ISEVENTITEM() )
				{
					bool ballot = pParty->DOITEM_ALLOT ( this, pItemDrop, true );
					if ( !ballot ) continue;
				}
*/
				NetMsg.DroppedItems[i].dwID		 = dwID;
				NetMsg.DroppedItems[i].vPos		 = pItemDrop->vPos;
				NetMsg.DroppedItems[i].emCrow	 = CROW_ITEM;
				if ( ++i >= MAXRIGHTOFITEM )
				{
					NetMsg.wSum = MAXRIGHTOFITEM;
					m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
					return S_OK;
				}
			}
		}
	}

	if ( i > 0 )
	{
		NetMsg.wSum = i;
		m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
	}

	return S_OK;
}
HRESULT GLChar::MsgGetFieldMoney ( NET_MSG_GENERIC* nmg )
{
//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	if ( !IsSTATE(EM_GETVA_AFTER) && !(m_pGLGaeaServer->IsReserveServerStop ()) )	return S_FALSE;

	GLMSG::SNETPET_REQ_GETRIGHTOFITEM_FB NetMsg;

	WORD i(0);

	PGLPETFIELD pMyPet = m_pGLGaeaServer->GetPET ( m_dwPetGUID );
	if ( !pMyPet || !pMyPet->IsValid () )	return E_FAIL;

	int nX = int ( pMyPet->m_vPos.x );
	int nZ = int ( pMyPet->m_vPos.z );
	LANDQUADNODE* pQuadHead = NULL;
	BOUDRECT bRect(nX+GLCONST_PET::nMAXVIEWRANGE,nZ+GLCONST_PET::nMAXVIEWRANGE,nX-GLCONST_PET::nMAXVIEWRANGE,nZ-GLCONST_PET::nMAXVIEWRANGE);
	m_pLandMan->GetLandTree()->FindNodes ( bRect, m_pLandMan->GetLandTree()->GetRootNode(), &pQuadHead );

	//	Note : 새로 추가되는 노드와 존속 노드를 표시.
	//
	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
        //	Note : - Money - 
		//
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		MONEYDROPNODE *pMoneyNode = pQuadNode->pData->m_MoneyList.m_pHead;
		for ( ; pMoneyNode; pMoneyNode = pMoneyNode->pNext )
		{
			PMONEYDROP pMoneyDrop = pMoneyNode->Data;
			if ( !pMoneyDrop ) 
			{
				DEBUGMSG_WRITE ( "Money is Lost in MsgGetFieldMoney()" );
				continue;
			}
			nX = int(pMoneyDrop->vPos.x); nZ = int(pMoneyDrop->vPos.z);

			if ( bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pMoneyDrop->dwGlobID;
				if ( dwID>=MAXMONEY ) continue;

				//	아이탬이 다른 사람에게 예약되어 있으면
				if ( !pMoneyDrop->IsTakeItem(m_dwPartyID,m_dwGaeaID) ) continue;

				// 순차습득시 파티원이 순차습득되지 않는 버그로 인해
				// 파타에 대한 검사는 여기서 하지 않고 실제 아이템 줍는
				// 행위에서 처리하도록 수정. 
/*
				// 파티 루팅검사
				GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
				if ( pParty )
				{
					bool ballot = pParty->DOMONEY_ALLOT ( this, pMoneyDrop, m_pLandMan, true );
					if ( !ballot ) continue;
				}
*/
				NetMsg.DroppedItems[i].dwID		 = dwID;
				NetMsg.DroppedItems[i].vPos		 = pMoneyDrop->vPos;
				NetMsg.DroppedItems[i].emCrow	 = CROW_MONEY;
				if ( ++i >= MAXRIGHTOFITEM )
				{
					NetMsg.wSum = MAXRIGHTOFITEM;
					m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
					return S_OK;
				}
			}
		}
	}

	if ( i > 0 )
	{
		NetMsg.wSum = i;
		m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
	}

	return S_OK;
}

HRESULT GLChar::MsgGetFieldSton ( NET_MSG_GENERIC* nmg )
{
//	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	if ( !IsSTATE(EM_GETVA_AFTER) && !(m_pGLGaeaServer->IsReserveServerStop ()) )	return S_FALSE;

	GLMSG::SNETPET_REQ_GETRIGHTOFITEM_FB NetMsg;

	WORD i(0);

	PGLPETFIELD pMyPet = m_pGLGaeaServer->GetPET ( m_dwPetGUID );
	if ( !pMyPet || !pMyPet->IsValid () )	return E_FAIL;

	int nX = int ( pMyPet->m_vPos.x );
	int nZ = int ( pMyPet->m_vPos.z );
	LANDQUADNODE* pQuadHead = NULL;
	BOUDRECT bRect(nX+GLCONST_PET::nMAXVIEWRANGE,nZ+GLCONST_PET::nMAXVIEWRANGE,nX-GLCONST_PET::nMAXVIEWRANGE,nZ-GLCONST_PET::nMAXVIEWRANGE);
	m_pLandMan->GetLandTree()->FindNodes ( bRect, m_pLandMan->GetLandTree()->GetRootNode(), &pQuadHead );

	//	Note : 새로 추가되는 노드와 존속 노드를 표시.
	//
	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
        //	Note : - Item - 
		//
		if ( m_pGLGaeaServer->IsReserveServerStop () ) 
			continue;

		ITEMDROPNODE *pItemNode = pQuadNode->pData->m_ItemList.m_pHead;
		for ( ; pItemNode; pItemNode = pItemNode->pNext )
		{
			PITEMDROP pItemDrop = pItemNode->Data;
			if ( !pItemDrop ) 
			{
				DEBUGMSG_WRITE ( "Item is Lost in MsgGetFieldSton()" );
				continue;
			}
			nX = int(pItemDrop->vPos.x); nZ = int(pItemDrop->vPos.z);

			if ( bRect.IsWithIn(nX,nZ) )
			{
				DWORD dwID = pItemDrop->dwGlobID;
				if ( dwID>=MAXITEM ) continue;

				//	아이탬이 다른 사람에게 예약되어 있으면
				if ( !pItemDrop->IsTakeItem(m_dwPartyID,m_dwGaeaID) ) continue;

				SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
				if ( !pItem ) continue;
/*
				WORD wPosX, wPosY;
				BOOL bOk = m_cInventory.FindInsrtable ( 
					pItem->sBasicOp.wInvenSizeX, 
					pItem->sBasicOp.wInvenSizeY, 
					wPosX, 
					wPosY );

				if ( !bOk )	
				{
					// 팻을 제거한다
					//m_pGLGaeaServer->DropOutPET ( m_dwPetGUID, false );

					//	인밴이 가득차서 아이탬을 넣을수 없음을 알림 
					GLMSG::SNETPET_MSG_NOTENOUGHINVEN NetMsg;
					m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );
					return S_OK;
				}
*/
				// 연마제가 아니면
				if ( pItem->sBasicOp.emItemType != ITEM_GRINDING 
					&& pItem->sBasicOp.emItemType != ITEM_MATERIALS )
					continue;

				// 순차습득시 파티원이 순차습득되지 않는 버그로 인해
				// 파타에 대한 검사는 여기서 하지 않고 실제 아이템 줍는
				// 행위에서 처리하도록 수정. 
/*
				// 파티원이고 이벤트 아이템이 아닐때 파티 루팅인지 검사후 처리.
				GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
				if ( pParty && !pItem->ISEVENTITEM() )
				{
					bool ballot = pParty->DOITEM_ALLOT ( this, pItemDrop, true );
					if ( !ballot ) continue;
				}
*/
				NetMsg.DroppedItems[i].dwID		 = dwID;
				NetMsg.DroppedItems[i].vPos		 = pItemDrop->vPos;
				NetMsg.DroppedItems[i].emCrow	 = CROW_ITEM;
				if ( ++i >= MAXRIGHTOFITEM )
				{
					NetMsg.wSum = MAXRIGHTOFITEM;
					m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
					return S_OK;
				}
			}
		}
	}

	if ( i > 0 )
	{
		NetMsg.wSum = i;
		m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);
	}

	return S_OK;
}

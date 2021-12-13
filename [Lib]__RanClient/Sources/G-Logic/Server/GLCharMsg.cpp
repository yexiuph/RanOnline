#include "pch.h"
#include "./GLChar.h"
#include "./GLGaeaServer.h"
#include "./GLSchoolFreePK.h"

#include "./NpcTalk.h"
#include "GLClubDeathMatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GLChar::MsgSendPeriod ()
{
	m_fPERIOD_TIMER = 0.0f;

	//	Note : Period Sync
	//
	GLMSG::SNET_PERIOD NetMsgPeriod;
	NetMsgPeriod.sPRERIODTIME = GLPeriod::GetInstance().GetPeriod();

	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (LPVOID) &NetMsgPeriod );
}

void GLChar::MsgSendUpdateState ( bool bparty, bool bconfront, bool bview )
{
	//	[자신에게] 회복상태 갱신.
	GLMSG::SNETPC_UPDATE_STATE NetMsg;
	NetMsg.sHP = GLCHARLOGIC::m_sHP;
	NetMsg.sMP = GLCHARLOGIC::m_sMP;
	NetMsg.sSP = GLCHARLOGIC::m_sSP;

	//NetMsg.ENCODE ( m_dwGaeaID );
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );

	//	[파티원들에게]
	if ( m_dwPartyID!=PARTY_NULL && bparty )
	{
		GLMSG::SNET_PARTY_MBR_POINT NetMsg;
		NetMsg.dwGaeaID = m_dwGaeaID;
		NetMsg.sHP = m_sHP;
		NetMsg.sMP = m_sMP;
		GLGaeaServer::GetInstance().SENDTOPARTYCLIENT ( m_dwPartyID, (NET_MSG_GENERIC*) &NetMsg );
	}

	//	대련자에게 채력 변화 알림.
	GLMSG::SNETPC_UPDATE_STATE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.sHP = GLCHARLOGIC::m_sHP;

	if ( m_sCONFTING.IsCONFRONTING() && bconfront )
	{
		switch ( m_sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			{
				PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( m_sCONFTING.dwTAR_ID );
				if ( pCHAR )
				{
					GLGaeaServer::GetInstance().SENDTOCLIENT ( pCHAR->m_dwClientID, (NET_MSG_GENERIC*) &NetMsgBrd );
				}
			}
			break;
		case EMCONFT_PARTY:
			{
				GLPARTY_FIELD *pConftParty = GLGaeaServer::GetInstance().GetParty(m_sCONFTING.dwTAR_ID);
				if ( pConftParty )
				{
					GLPARTY_FIELD::MEMBER_ITER iter = pConftParty->m_cMEMBER.begin();
					GLPARTY_FIELD::MEMBER_ITER iter_end = pConftParty->m_cMEMBER.end();
					for ( ; iter!=iter_end; ++iter )
					{
						const GLPARTY_FNET &sMEMBER = (*iter).second;
						if ( !sMEMBER.m_bConfront )		continue;

						PGLCHAR pTAR = GLGaeaServer::GetInstance().GetChar(sMEMBER.m_dwGaeaID);
						if ( !pTAR )					continue;

						GLGaeaServer::GetInstance().SENDTOCLIENT ( pTAR->m_dwClientID, &NetMsgBrd );
					}
				}
			}
			break;
		case EMCONFT_GUILD:
			{
				GLClubMan &sClubMan = GLGaeaServer::GetInstance().GetClubMan();

				GLCLUB *pMY_CLUB = sClubMan.GetClub ( m_dwGuild );
				if ( pMY_CLUB )
				{
					CLUBMEMBERS_ITER pos = pMY_CLUB->m_mapMembers.begin();
					CLUBMEMBERS_ITER end = pMY_CLUB->m_mapMembers.end();
					for ( ; pos!=end; ++pos )
					{
						PGLCHAR pTAR = GLGaeaServer::GetInstance().GetCharID((*pos).first);
						if ( !pTAR )								continue;
						if ( !pTAR->m_sCONFTING.IsCONFRONTING() )	continue;

						GLGaeaServer::GetInstance().SENDTOCLIENT ( pTAR->m_dwClientID, &NetMsgBrd );
					}
				}

				GLCLUB *pTAR_CLUB = sClubMan.GetClub ( m_sCONFTING.dwTAR_ID );
				if ( pTAR_CLUB )
				{
					CLUBMEMBERS_ITER pos = pTAR_CLUB->m_mapMembers.begin();
					CLUBMEMBERS_ITER end = pTAR_CLUB->m_mapMembers.end();
					for ( ; pos!=end; ++pos )
					{
						PGLCHAR pTAR = GLGaeaServer::GetInstance().GetCharID((*pos).first);
						if ( !pTAR )								continue;
						if ( !pTAR->m_sCONFTING.IsCONFRONTING() )	continue;

						GLGaeaServer::GetInstance().SENDTOCLIENT ( pTAR->m_dwClientID, &NetMsgBrd );
					}
				}
			}
			break;
		};
	}

	//	Note : 적대 행위자의 타이머 갱신 및 관리.
	{
		PGLCHAR pTAR = NULL;
		for ( MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.begin(); pos != m_mapPlayHostile.end(); ++pos )
		{
			pTAR = GLGaeaServer::GetInstance().GetCharID( pos->first );
			if ( pTAR )
				GLGaeaServer::GetInstance().SENDTOCLIENT ( pTAR->m_dwClientID, &NetMsgBrd );

			if( !pos->second )
				CDebugSet::ToLogFile( "GLChar::MsgSendUpdateState, pos->second = NULL" );
		}
	}

	bool bGuidBattleMap = m_pLandMan->m_bGuidBattleMap;
	bool bClubDeathMatch = m_pLandMan->m_bClubDeathMatchMap;
	bool bSCHOOL_FREEPK = GLSchoolFreePK::GetInstance().IsON();
	bool bBRIGHTEVENT   = GLGaeaServer::GetInstance().IsBRIGHTEVENT();


	// 성향이벤트 이면 주변에도 
	if ( bview || bGuidBattleMap || bClubDeathMatch || bSCHOOL_FREEPK || bBRIGHTEVENT )
	{
		SendMsgViewAround((NET_MSG_GENERIC*)&NetMsgBrd);
	}
}

inline HRESULT GLChar::MsgReady ( NET_MSG_GENERIC* nmg )
{
	//	Note : 활성화 시킴.
	//
	ReSetSTATE(EM_ACT_WAITING);
	return S_OK;
}

inline HRESULT GLChar::MsgMoveState ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_MOVESTATE *pNetMsg = reinterpret_cast<GLMSG::SNETPC_MOVESTATE*> ( nmg );

	DWORD dwOldActState = m_dwActState;

	if ( m_dwUserLvl >= USER_GM3 )
	{
		if ( pNetMsg->dwActState & EM_REQ_VISIBLENONE )	SetSTATE ( EM_REQ_VISIBLENONE );
		else											ReSetSTATE ( EM_REQ_VISIBLENONE );
		
		if ( pNetMsg->dwActState & EM_REQ_VISIBLEOFF )	SetSTATE ( EM_REQ_VISIBLEOFF );
		else											ReSetSTATE ( EM_REQ_VISIBLEOFF );
	}

	if ( pNetMsg->dwActState&EM_ACT_RUN )			SetSTATE(EM_ACT_RUN);
	else											ReSetSTATE(EM_ACT_RUN);

	if ( pNetMsg->dwActState & EM_ACT_PEACEMODE )	SetSTATE ( EM_ACT_PEACEMODE );
	else											ReSetSTATE ( EM_ACT_PEACEMODE );

	if ( dwOldActState != m_dwActState )
	{
		//	Note : 딜래이를 반영한 이동 속도 측정.
		float fVelo = GetMoveVelo ();

		m_actorMove.SetMaxSpeed ( fVelo );

		//	Note : 자신의 주변 Char에게 자신의 Msg를 전달.
		//
		GLMSG::SNETPC_MOVESTATE_BRD NetMsgFB;
		NetMsgFB.dwGaeaID = m_dwGaeaID;
		NetMsgFB.dwActState = m_dwActState;
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgFB );
	}

	return S_OK;
}

// *****************************************************
// Desc: 이동 처리
// *****************************************************
inline HRESULT GLChar::MsgGoto ( NET_MSG_GENERIC* nmg )
{
	if( GLGaeaServer::GetInstance().m_bEmptyMsg ) return S_OK;
	// 상점이 개설된 상태에서 이동할 수 없다.
	if ( m_sPMarket.IsOpen() ) return E_FAIL;

	// 내상태가 DEAD 이면 DEAD Animation 처리
	if ( IsSTATE(EM_ACT_DIE) )
	{
		//	Note : 쓰러짐을 클라이언트 들에게 알림.
		GLMSG::SNET_ACTION_BRD NetMsgBrd;
		NetMsgBrd.emCrow	= CROW_PC;
		NetMsgBrd.dwID		= m_dwGaeaID;
		NetMsgBrd.emAction	= GLAT_FALLING;

		//	Note : 자신의 클라이언트에.
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgBrd );

		return E_FAIL;
	}

	GLMSG::SNETPC_GOTO *pNetMsg = reinterpret_cast<GLMSG::SNETPC_GOTO*> ( nmg );

	BOOL bRun = IsSTATE ( EM_ACT_RUN );				// 달리는 상태인지
	BOOL bToRun = pNetMsg->dwActState&EM_ACT_RUN;   // 달려야 하는지

	// 상태 변경
	if ( bRun != bToRun )
	{
		if ( bToRun )		SetSTATE(EM_ACT_RUN);
		else				ReSetSTATE(EM_ACT_RUN);
	}

	D3DXVECTOR3 vDist = m_vPos - pNetMsg->vCurPos;
	float fDist = D3DXVec3Length(&vDist);
	if ( fDist > 60.0f )
	{
		// 제스쳐 중이면 제스쳐 끝내고 ( 댄스중 순간이동 방지 )
		if ( m_Action == GLAT_TALK || m_Action == GLAT_GATHERING )
			return E_FAIL;
			
		TurnAction ( GLAT_IDLE );

		//	Note : 자신의 클라이언트에 위치 강제 설정.
		//
		GLMSG::SNET_GM_MOVE2GATE_FB NetMsgFB;
		NetMsgFB.vPOS = m_vPos;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

		//	Note : 클라이언트들에게 위치 강제 설정.
		//
		GLMSG::SNETPC_JUMP_POS_BRD NetMsgJump;
		NetMsgJump.dwGaeaID = m_dwGaeaID;
		NetMsgJump.vPOS = m_vPos;
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgJump );

		return S_OK;
	}

	m_TargetID.vPos = pNetMsg->vTarPos;
	TurnAction ( GLAT_MOVE );

	BOOL bSucceed = m_actorMove.GotoLocation
	(
		m_TargetID.vPos+D3DXVECTOR3(0,+10,0),
		m_TargetID.vPos+D3DXVECTOR3(0,-10,0)
	);

	if ( !bSucceed )
	{
		//CDebugSet::ToLogFile ( "m_actorMove.GotoLocation error : %f, %f, %f", m_TargetID.vPos.x, m_TargetID.vPos.y, m_TargetID.vPos.z );
	}

	if ( bSucceed )
	{
		float fVelo = GetMoveVelo ();
		m_actorMove.SetMaxSpeed ( fVelo );
	
		//	Note : 자신의 주변 Char에게 자신의 Msg를 전달.
		//
		GLMSG::SNETPC_GOTO_BRD NetMsgFB;
		NetMsgFB.dwGaeaID = m_dwGaeaID;
		NetMsgFB.dwActState = m_dwActState;
		NetMsgFB.vCurPos = m_vPos;
		NetMsgFB.vTarPos = m_TargetID.vPos;
		NetMsgFB.fDelay = 0.0f;

		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgFB );
	}

	return S_OK;
}

inline HRESULT GLChar::MsgAttack ( NET_MSG_GENERIC* nmg )
{
	if( GLGaeaServer::GetInstance().m_bEmptyMsg )													return S_OK;
	if ( !IsValidBody() )																			return E_FAIL;
	if ( m_pLandMan && m_pLandMan->IsPeaceZone() )													return E_FAIL;

	GLMSG::SNETPC_ATTACK *pNetMsg = reinterpret_cast<GLMSG::SNETPC_ATTACK*> ( nmg );
	m_dwANISUBSELECT = pNetMsg->dwAniSel;
	m_TargetID.emCrow = pNetMsg->emTarCrow;
	m_TargetID.dwID = pNetMsg->dwTarID;

	//	Note : 타겟 검사.
	//
	GLACTOR* pTARGET = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
	if ( !pTARGET )																					return E_FAIL;

	// 타겟과의 거리 검사
	D3DXVECTOR3 vTarPos = pTARGET->GetPosition();
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-vTarPos) );
	WORD wAttackRange = pTARGET->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + 2;
	if ( ISLONGRANGE_ARMS() )	 wAttackRange += (WORD) GETSUM_TARRANGE();
	WORD wAttackAbleDis = wAttackRange + 7;

	if ( fDist > wAttackAbleDis )
	{
		//	Note : 자신에게.
		GLMSG::SNETPC_ATTACK_AVOID NetMsg;
		NetMsg.emTarCrow	= m_TargetID.emCrow;
		NetMsg.dwTarID		= m_TargetID.dwID;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );

		//	Note : 주변 클라이언트들에게 메세지 전송.
		//
		GLMSG::SNETPC_ATTACK_AVOID_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID	= m_dwGaeaID;
		NetMsgBrd.emTarCrow	= m_TargetID.emCrow;
		NetMsgBrd.dwTarID	= m_TargetID.dwID;
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
			
		return E_FAIL;
	}

	bool bFreePKMap		= m_pLandMan->IsFreePK();
	bool bGuidBattleMap = m_pLandMan->m_bGuidBattleMap;
	bool bBRIGHTEVENT   = GLGaeaServer::GetInstance().IsBRIGHTEVENT();
	bool bSCHOOL_FREEPK = GLSchoolFreePK::GetInstance().IsON();
	if ( GetSchool() == pTARGET->GetSchool() )	bSCHOOL_FREEPK = false;

	bool bREACTIONALBE  = IsReActionable(pTARGET);
	if ( !bREACTIONALBE )																			return E_FAIL;

	//	Note : pc 이고 pk 모드일 경우 공격 가능.
	if ( pTARGET->GetCrow()==CROW_PC )
	{
		PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( m_TargetID.dwID );
		if ( pCHAR )
		{

			// 상대방이 대련 종료후 무적타임이면 공격 무시
			if ( pCHAR->m_sCONFTING.IsPOWERFULTIME () )	
				return E_FAIL;
			// 내가 대련 종료후 무적타임이면 공격 무시
			if ( m_sCONFTING.IsPOWERFULTIME() )			
				return E_FAIL;

			bool bClubBattle = false;
			bool bClubDeathMatch = false;
			
			if ( m_pLandMan->IsClubBattleZone() )
			{	
				GLClubMan &cClubMan = m_pGLGaeaServer->GetClubMan();
				GLCLUB *pMyClub = cClubMan.GetClub ( m_dwGuild );
				GLCLUB *pTarClub = cClubMan.GetClub ( pCHAR->m_dwGuild );

				if ( pMyClub && pTarClub )
				{
					bool bClub = pMyClub->IsBattle( pCHAR->m_dwGuild );
					bool bAlliance = pMyClub->IsBattleAlliance ( pTarClub->m_dwAlliance );
					bClubBattle = (bClub || bAlliance);
				}				
			}

			if ( m_pLandMan->m_bClubDeathMatchMap )
			{
				GLClubDeathMatch* pCDM = GLClubDeathMatchFieldMan::GetInstance().Find( m_pLandMan->m_dwClubMapID );
				if ( pCDM && pCDM->IsBattle() ) bClubDeathMatch = true;
			}

			if ( !(bGuidBattleMap || bClubDeathMatch || bSCHOOL_FREEPK || bFreePKMap || bBRIGHTEVENT || IsConflictTarget(pTARGET) || bClubBattle ) )
			{
				//	Note : 상대방이 범죄자가 아니고 정당방위 시간이 지정되지 않았을때는 
				//		자신이 적대행위를 시작함.
				if ( !IS_PLAYHOSTILE(pCHAR->m_dwCharID) && !pCHAR->ISOFFENDER() )
				{
					int nBRIGHT(GLCONST_CHAR::nPK_TRY_BRIGHT_POINT), nLIFE(GLCONST_CHAR::nPK_TRY_LIVING_POINT);

					//	Note : 속성수치 변화.
					m_nBright += nBRIGHT;

					GLMSG::SNETPC_UPDATE_BRIGHT NetMsg;
					NetMsg.nBright = m_nBright;
					GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

					GLMSG::SNETPC_UPDATE_BRIGHT_BRD NetMsgBrd;
					NetMsgBrd.dwGaeaID = m_dwGaeaID;
					NetMsgBrd.nBright = m_nBright;
					SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

					//	Note : 생활점수 변화.
					m_nLiving += nLIFE;

					GLMSG::SNETPC_UPDATE_LP NetMsgLp;
					NetMsgLp.nLP = m_nLiving;
					GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgLp);

					//	Note : 적대행위 로그 기록.
					GLITEMLMT::GetInstance().ReqAction
					(
						m_dwCharID,					//	당사자.
						EMLOGACT_HOSTILE,			//	행위.
						ID_CHAR, pCHAR->m_dwCharID,	//	상대방.
						0,							//	exp
						nBRIGHT,					//	bright
						nLIFE,						//	life
						0							//	money
					);
				}
			}

			if ( !IsConflictTarget(pTARGET) )
			{
				//	Note : 적대 행위자에게 피해자 등록. ( pk )
				AddPlayHostile ( pCHAR->m_dwCharID, TRUE, bClubBattle );

				//	Note : 적대 피해자에게 적대자 등록. ( PK )
				pCHAR->AddPlayHostile ( m_dwCharID, FALSE, bClubBattle);
			}

			STARGETID sTargetID = STARGETID ( CROW_PC, m_dwGaeaID, m_vPos );
			
			// 공격자와 공격대상자 모두 팻을 공격모드로 변경
			// 맞는자
			PGLPETFIELD pEnemyPet = GLGaeaServer::GetInstance().GetPET ( pCHAR->m_dwPetGUID );
			if ( pEnemyPet && pEnemyPet->IsValid () && !pEnemyPet->IsSTATE ( EM_PETACT_ATTACK ) )
			{
				D3DXVECTOR3 vOwnerPos, vDist;
				float fDist;
				vOwnerPos = pCHAR->GetPosition ();
				vDist = pEnemyPet->m_vPos - vOwnerPos;
				fDist = D3DXVec3Length(&vDist);

				// 일정거리 안에 있으면 
				if ( fDist <= GLCONST_PET::fWalkArea )
				{
					pEnemyPet->ReSetAllSTATE ();
					pEnemyPet->SetSTATE ( EM_PETACT_ATTACK );

					GLMSG::SNETPET_ATTACK NetMsg;
					NetMsg.sTarID = sTargetID;
					GLGaeaServer::GetInstance().SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsg );
					
					GLMSG::SNETPET_ATTACK_BRD NetMsgBRD;
					NetMsgBRD.dwGUID = pEnemyPet->m_dwGUID;
					NetMsgBRD.sTarID = sTargetID;
					pCHAR->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBRD );
				}
			}
			// 맞는자만 Summon Attack 처리
			PGLSUMMONFIELD pEnemySummon = GLGaeaServer::GetInstance().GetSummon ( pCHAR->m_dwSummonGUID );
			if ( pEnemySummon && pEnemySummon->IsValid () && !pEnemySummon->IsSTATE ( EM_SUMMONACT_ATTACK ) && 
				pEnemySummon->GetAttackTarget( sTargetID ) )
			{
				D3DXVECTOR3 vOwnerPos, vDist;
				float fDist;
				vOwnerPos = pCHAR->GetPosition ();
				vDist = pEnemySummon->m_vPos - vOwnerPos;
				fDist = D3DXVec3Length(&vDist);

				// 일정거리 안에 있으면 
				//if ( fDist <= pEnemySummon->m_fWalkArea )
				{
					pEnemySummon->SetAttackTarget( sTargetID );
				}
			}

			// 공격자
			PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
			if ( pMyPet && pMyPet->IsValid () && !pMyPet->IsSTATE ( EM_PETACT_ATTACK ) )
			{
				D3DXVECTOR3 vDist;
				float fDist;
				vDist = pMyPet->m_vPos - m_vPos;
				fDist = D3DXVec3Length(&vDist);

				// 일정거리 안에 있으면 
				if ( fDist <= GLCONST_PET::fWalkArea )
				{
					pMyPet->ReSetAllSTATE ();
					pMyPet->SetSTATE ( EM_PETACT_ATTACK );

					m_TargetID.vPos = pCHAR->GetPosition ();

					GLMSG::SNETPET_ATTACK NetMsg;
					NetMsg.sTarID = m_TargetID;
					GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
					
					GLMSG::SNETPET_ATTACK_BRD NetMsgBRD;
					NetMsgBRD.dwGUID = pMyPet->m_dwGUID;
					NetMsgBRD.sTarID = m_TargetID;
					SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBRD );
				}
			}
		}
	}else if ( pTARGET->GetCrow()==CROW_SUMMON )
	{
		STARGETID sTargetID = STARGETID ( CROW_PC, m_dwGaeaID, m_vPos );

		// Summon Attack 처리
		PGLSUMMONFIELD pEnemySummon = GLGaeaServer::GetInstance().GetSummon ( m_TargetID.dwID );
		if ( pEnemySummon && pEnemySummon->IsValid () && !pEnemySummon->IsSTATE ( EM_SUMMONACT_ATTACK ) && 
			pEnemySummon->GetAttackTarget( sTargetID ) )
		{
			/*D3DXVECTOR3 vOwnerPos, vDist;
			float fDist;
			vOwnerPos = pEnemySummon->m_pOwner->GetPosition ();
			vDist = pEnemySummon->m_vPos - vOwnerPos;
			fDist = D3DXVec3Length(&vDist);

			if ( fDist <= pEnemySummon->m_fWalkArea )*/
			{
				pEnemySummon->SetAttackTarget( sTargetID );
			}
		}
	}

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();
    
	EMANI_SUBTYPE emANISUBTYPE = CHECK_ATTACK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand] );

	GLCONST_CHARCLASS &ConstCharClass = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];
	VECANIATTACK &vecAniAttack = ConstCharClass.m_ANIMATION[AN_ATTACK][emANISUBTYPE];
	if ( vecAniAttack.empty() )		return E_FAIL;
	if ( vecAniAttack.size() <= m_dwANISUBSELECT )	m_dwANISUBSELECT = 0;

	const SANIATTACK &sAniAttack = vecAniAttack[m_dwANISUBSELECT];
	WORD wStrikeNum = sAniAttack.m_wDivCount;

	m_TargetID.vPos = pTARGET->GetPosition();

	//	Note : 공격이 가능한지 검사.
	//
	EMBEGINATTACK_FB emBeginFB = BEGIN_ATTACK(wStrikeNum);
	if ( emBeginFB!=EMBEGINA_OK && emBeginFB!=EMBEGINA_SP )		return E_FAIL;

	//	Note : 소모성 아이탬 유효성 검사후, 사라질때 메시지 발생.
	//
	CheckInstanceItem ();

	//	Note : 사전 명중치 계산.
	//
	BOOL bLowSP = (emBeginFB==EMBEGINA_SP) ? TRUE: FALSE;
	PreStrikeProc ( FALSE, bLowSP );

	//	Note : 공격 수행.
	//
	TurnAction ( GLAT_ATTACK );

	//	Note : 공격 성공시 - 자신의 주변 Char에게 자신의 Msg를 전달.
	//
	GLMSG::SNETPC_ATTACK_BRD NetMsgFB;
	NetMsgFB.dwGaeaID = m_dwGaeaID;
	NetMsgFB.emTarCrow = pNetMsg->emTarCrow;
	NetMsgFB.dwTarID = pNetMsg->dwTarID;
	NetMsgFB.dwAniSel = m_dwANISUBSELECT;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgFB );

	// 몹을 공격하면 팻이 공격모션을 취함
	if ( pTARGET->GetCrow()==CROW_MOB )
	{
		PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
		if ( pMyPet && pMyPet->IsValid () && !pMyPet->IsSTATE ( EM_PETACT_ATTACK ) )
		{
			D3DXVECTOR3 vDist;
			float fDist;
			vDist = pMyPet->m_vPos - m_vPos;
			fDist = D3DXVec3Length(&vDist);

			// 일정거리 안에 있으면 
			if ( fDist <= GLCONST_PET::fWalkArea )
			{
				GLMSG::SNETPET_ATTACK NetMsg;
				NetMsg.sTarID = m_TargetID;
				GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
				
				GLMSG::SNETPET_ATTACK_BRD NetMsgBRD;
				NetMsgBRD.dwGUID = m_dwPetGUID;
				NetMsgBRD.sTarID = m_TargetID;
				SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBRD );
			}
		}
	}

	return S_OK;
}

inline HRESULT GLChar::MsgAttackCancel ( NET_MSG_GENERIC* nmg )
{
	//	Note : 액션 취소.
	if ( IsACTION(GLAT_ATTACK) )	TurnAction ( GLAT_IDLE );

	//	Note : 자신의 주변 Char에게 자신의 Msg를 전달.
	//
	GLMSG::SNETPC_ATTACK_CANCEL_BRD NetMsgFB;
	NetMsgFB.dwGaeaID = m_dwGaeaID;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgFB );

	return S_OK;
}

inline HRESULT GLChar::MsgGatheringCancel ( NET_MSG_GENERIC* nmg )
{
	//	Note : 액션 취소.
	if ( IsACTION(GLAT_GATHERING) )	TurnAction ( GLAT_IDLE );
	return S_OK;
}

void GLChar::ReSetStartMap()
{
	// 게이트가 못 찾으면 최초 시작 위치로 이동한다.
	m_sStartMapID = GLCONST_CHAR::nidSTARTMAP[m_wSchool];
	m_dwStartGate = GLCONST_CHAR::dwSTARTGATE[m_wSchool];
	m_vStartPos   = D3DXVECTOR3(0.0f,0.0f,0.0f);

	// 시작위치 리셋 (각학원별 시작맵으로)
	GLMSG::SNETPC_UPDATE_STARTCALL NetMsg;
	NetMsg.sStartMapID   = m_sStartMapID;
	NetMsg.dwStartGateID = m_dwStartGate;
	NetMsg.vStartPos	 = m_vStartPos;

	// 클라이언트에 알림
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );

	// Agent에 알림
	GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsg );		

	return;
}

BOOL GLChar::CheckStartMap()
{
	GLLandMan *pLandMan=NULL;
	DWORD dwGenGate = 0;
	pLandMan = GLGaeaServer::GetInstance().GetByMapID ( m_sStartMapID );

	if ( pLandMan && pLandMan->m_bClubDeathMatchMap )
	{
		if ( GLCONST_CHAR::bPKLESS ) return FALSE;	

	    GLClubDeathMatch *pCDM = GLClubDeathMatchFieldMan::GetInstance().Find ( pLandMan->m_dwClubMapID );
		if ( pCDM && !pCDM->IsEnterMap( m_dwGuild ) )	return FALSE;
		
		GLCLUB* pClub = GLGaeaServer::GetInstance().GetClubMan().GetClub ( m_dwGuild );
		if ( !pClub ) return FALSE;
		if ( pClub->GetAllBattleNum() > 0 ) return FALSE;
		if ( !pClub->IsMemberFlgCDM(m_dwCharID) )	return FALSE;
	}

	if ( pLandMan && pLandMan->GetLandGateMan().FindLandGate(m_dwStartGate) ) 		
	{
		return TRUE;
	}
	
	return FALSE;

}


inline void GLChar::DoReBirth2StartMap ()
{
	SNATIVEID nidSTARTMAP = m_sStartMapID;
	DWORD dwSTARTGATE = m_dwStartGate;

	//	부활.
	GLLandMan *pLandMan=NULL;
	DWORD dwGenGate = 0;


	//	정상적인 상황 
	if ( CheckStartMap() )
	{
		pLandMan = GLGaeaServer::GetInstance().GetByMapID ( nidSTARTMAP );
		if ( pLandMan )
		{
			BOOL bOk = GLGaeaServer::GetInstance().RequestReBirth ( m_dwGaeaID, pLandMan->GetMapID(), dwSTARTGATE, D3DXVECTOR3(0,0,0) );
			if ( !bOk )	DEBUGMSG_WRITE ( "GLGaeaServer::GetInstance().RequestReBirth () bOk false" );
			return;
		}
		else
		{
			//	나올수 없는 상황
			DEBUGMSG_WRITE ( "GLGaeaServer::GetInstance().RequestReBirth () pLandMan NULL" );
			return;
		}
	}
	else	//	시작위치로 이동
	{
		// 게이트가 못 찾으면 최초 시작 위치로 이동한다.
		ReSetStartMap();

		pLandMan = GLGaeaServer::GetInstance().GetByMapID ( m_sStartMapID );
		if ( pLandMan )
		{
			DxLandGateMan* pLandGateMan = &pLandMan->GetLandGateMan();
			if ( !pLandGateMan->FindLandGate ( m_dwStartGate ) )
			{
				DEBUGMSG_WRITE ( "(Revive) - Default gate is a failure. school %d", m_wSchool );
				return;
			}
			else
			{
//				m_bEntryFailed = TRUE;
				BOOL bOk = GLGaeaServer::GetInstance().RequestReBirth ( m_dwGaeaID, pLandMan->GetMapID(), m_dwStartGate, D3DXVECTOR3(0,0,0) );	
				if ( !bOk )	DEBUGMSG_WRITE ( "GLGaeaServer::GetInstance().RequestReBirth () bOk false" );
				return;
			}
		}
		else
		{
			DEBUGMSG_WRITE ( "(Revive) - Default map is a failure. school %d", m_wSchool );
			return;
		}
	}

	return;

/*


	//	부활할 Map과 Gate 유효성 검사.
	if ( nidSTARTMAP!=NATIVEID_NULL() )
	{
		pLandMan = GLGaeaServer::GetInstance().GetByMapID ( nidSTARTMAP );

		if ( pLandMan )
		{
			DxLandGateMan* pLandGateMan = &pLandMan->GetLandGateMan();
			if ( pLandGateMan->FindLandGate(dwSTARTGATE)==NULL )
			{
				//PDXLANDGATE pLandGate = pLandGateMan->GetListLandGate();
				//if ( !pLandGate )	pLandMan = NULL;	//	Gate가 하나도 없는 곳엔 부활할 수 없음.
				//else
				//{
				//	dwGenGate = pLandGate->GetGateID();	//	첫번째 Gate 에서 생김.
				//}
				// 게이트가 못 찾으면 최초 시작 위치로 이동한다.
				m_sStartMapID = GLCONST_CHAR::nidSTARTMAP[m_wSchool];
				m_dwStartGate = GLCONST_CHAR::dwSTARTGATE[m_wSchool];
				m_vStartPos   = D3DXVECTOR3(0.0f,0.0f,0.0f);

				// 시작위치 리셋 (각학원별 시작맵으로)
				GLMSG::SNETPC_UPDATE_STARTCALL NetMsg;
				NetMsg.sStartMapID   = m_sStartMapID;
				NetMsg.dwStartGateID = m_dwStartGate;
				NetMsg.vStartPos	 = m_vStartPos;

				// 클라이언트에 알림
				GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );

				// Agent에 알림
				GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsg );

				pLandMan = GLGaeaServer::GetInstance().GetByMapID ( m_sStartMapID );
				DxLandGateMan* pLandGateMan = &pLandMan->GetLandGateMan();
				if ( !pLandGateMan->FindLandGate ( m_dwStartGate ) )
				{
					DEBUGMSG_WRITE ( "Gate == NULL (Revive) - Default gate is a failure. school %d", m_wSchool );
					return;
				}else{
					dwGenGate = m_dwStartGate;
				}

				m_bEntryFailed = TRUE;
			}
			else	dwGenGate = dwSTARTGATE;
		}
	}

	if ( !pLandMan )
	{
		SNATIVEID nidSTARTMAP = GLCONST_CHAR::nidSTARTMAP[m_wSchool];
		DWORD dwSTARTGATE = GLCONST_CHAR::dwSTARTGATE[m_wSchool];

		pLandMan = GLGaeaServer::GetInstance().GetByMapID ( nidSTARTMAP );
		if ( pLandMan )
		{
			DxLandGateMan* pLandGateMan = &pLandMan->GetLandGateMan();
			if ( !pLandGateMan->FindLandGate ( dwSTARTGATE ) )
			{
				DEBUGMSG_WRITE ( "(Revive) - Default gate is a failure. school %d", m_wSchool );
				return;
			}
			else
			{
				dwGenGate = dwSTARTGATE;
			}
		}
	}

	if ( !pLandMan )
	{
		DEBUGMSG_WRITE ( "(Revive) - Default map is a failure. school %d", m_wSchool );
		return;
	}

	//	Note : 부활 map과 부활 gate 지정, 위치는 의미 없는 값.
	BOOL bOk = GLGaeaServer::GetInstance().RequestReBirth ( m_dwGaeaID, pLandMan->GetMapID(), dwGenGate, D3DXVECTOR3(0,0,0) );
	if ( !bOk )
	{
		DEBUGMSG_WRITE ( "GLGaeaServer::GetInstance().RequestReBirth () bOk false" );
		return;
	}
*/
}

// *****************************************************
// Desc: 부활요청 처리 ( Ageng->Field )
// *****************************************************
inline HRESULT GLChar::MsgReqReBirth ( NET_MSG_GENERIC* nmg )
{
	if ( !IsSTATE(EM_ACT_DIE) )
	{
		DEBUGMSG_WRITE ( "GLChar::MsgReqReBirth IsSTATE(EM_ACT_DIE) false" );
		return S_OK;
	}

	//	부활 완료 체크.
	ReSetSTATE(EM_ACT_DIE);

	//  부활 지점으로 진입 불가시 부활 지점을 초기화한다.
	GLMSG::SNETPC_REQ_REBIRTH *pNetMsg = (GLMSG::SNETPC_REQ_REBIRTH *) nmg;
	if( pNetMsg->bRegenEntryFailed )
	{
		m_sStartMapID = GLCONST_CHAR::nidSTARTMAP[m_wSchool];
		m_dwStartGate = GLCONST_CHAR::dwSTARTGATE[m_wSchool];
		m_vStartPos   = D3DXVECTOR3(0.0f,0.0f,0.0f);
	}

	//	Note : 지정된 부활지역에서 부활되도록 위치 이동.
	//
	DoReBirth2StartMap ();

	//	부활시 채력 회복.
	m_fGenAge = 0.0f;
	GLCHARLOGIC::INIT_RECOVER();
	TurnAction ( GLAT_IDLE );

	//	Note : 부활시 경험치 감소.
	ReBirthDecExp ();

	// PET
	// 맵이동시 Pet 삭제
	GLGaeaServer::GetInstance().ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,true,true) );
	GLGaeaServer::GetInstance().ReserveDropOutSummon ( SDROPOUTSUMMONINFO(m_dwSummonGUID,true) );
	GLGaeaServer::GetInstance().SaveVehicle( m_dwClientID, m_dwGaeaID, true );

	//	부활 확인 메시지.
	GLMSG::SNETPC_REQ_REBIRTH_FB NetMsg;
	NetMsg.sMapID = m_pLandMan->GetMapID();
	NetMsg.vPos = GetPosition();
	NetMsg.wNowHP = m_sHP.wNow;
	NetMsg.wNowMP = m_sMP.wNow;
	NetMsg.wNowSP = m_sSP.wNow;
	NetMsg.bRegenEntryFailed = pNetMsg->bRegenEntryFailed;

	//	Note : 죽을때 로그 기록.
	// 부활하기 버튼이 나오지 않아 로그를 못남기는 경우가 있어서 죽으면 무조건 남긴다.
	//{
	//	EMIDTYPE emKILL = ID_MOB;
	//	if ( m_sAssault.emCrow==CROW_PC )	emKILL = ID_CHAR;

	//	GLITEMLMT::GetInstance().ReqAction
	//		(
	//		m_dwCharID,					//	당사자.
	//		EMLOGACT_DIE,				//	행위.
	//		emKILL, 0,					//	상대방.
	//		m_nDECEXP,					// exp
	//		0,							// bright
	//		0,							// life
	//		0							// money
	//		);
	//}


	GLGaeaServer::GetInstance().SENDTOAGENT(m_dwClientID,&NetMsg);

	return S_OK;
}

// *****************************************************
// Desc: 부활요청 처리 (귀혼주 사용)
// *****************************************************
inline HRESULT GLChar::MsgReqRevive ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_REVIVE *pNetMsg = (GLMSG::SNETPC_REQ_REVIVE *)nmg;
	GLMSG::SNETPC_REQ_REVIVE_FB MsgFB;

	// 아아템이 있는지
	SITEM* pITEM = GET_SLOT_ITEMDATA(SLOT_NECK);
	if ( !pITEM )
	{
		MsgFB.emFB = EMREQ_REVIVE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime ( pITEM->sBasicOp.sNativeID ) )
	{
		MsgFB.emFB = EMREQ_REVIVE_FB_COOLTIME;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

/*
	// 귀혼주 여부
	if ( pITEM->sBasicOp.emItemType != ITEM_REVIVE )
	{
		MsgFB.emFB = EMREQ_REVIVE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}
*/
	// 아이템 속성이 부활기능인지
	if ( pITEM->sDrugOp.emDrug!=ITEM_DRUG_CALL_REVIVE )
	{
		MsgFB.emFB = EMREQ_REVIVE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	// 선도맵인지
	if ( m_pLandMan && m_pLandMan->m_bGuidBattleMap )
	{
		MsgFB.emFB = EMREQ_REVIVE_FB_NOTUSE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	// 클럽데스매치 존
	if ( m_pLandMan->m_bClubDeathMatchMap )
	{
		MsgFB.emFB = EMREQ_REVIVE_FB_NOTUSE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//	Note : 귀혼주 사용.
	//SITEMCUSTOM &sCUSTOM = m_PutOnItems[SLOT_NECK];

	//	Note : 소모된 귀혼주 처리.
	//
	DoDrugSlotItem(SLOT_NECK);

	//	Note : 부활 map과 부활 gate 지정, 위치는 의미 없는 값.
	BOOL bOk = GLGaeaServer::GetInstance().RequestReBirth ( m_dwGaeaID, m_pLandMan->GetMapID(), UINT_MAX, m_vPos );
	if ( !bOk )
	{
		DEBUGMSG_WRITE ( "캐릭터 부활 시도중에 오류가 발생! GLGaeaServer::GetInstance().RequestReBirth ()" );
		return E_FAIL;
	}

	//	부활 완료 체크.
	ReSetSTATE(EM_ACT_DIE);

	//	부활시 채력 회복.
	m_fGenAge = 0.0f;
	GLCHARLOGIC::INIT_RECOVER(100);
	TurnAction ( GLAT_IDLE );	

	// PET
	// 맵이동시 Pet 삭제
	GLGaeaServer::GetInstance().ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,true,true) );
	GLGaeaServer::GetInstance().ReserveDropOutSummon ( SDROPOUTSUMMONINFO(m_dwSummonGUID,true) );
	GLGaeaServer::GetInstance().SaveVehicle( m_dwClientID, m_dwGaeaID, true );

	//	부활 확인 메시지.
	GLMSG::SNETPC_REQ_REBIRTH_FB NetMsg;
	NetMsg.sMapID = m_pLandMan->GetMapID();
	NetMsg.vPos = GetPosition();
	NetMsg.wNowHP = m_sHP.wNow;
	NetMsg.wNowMP = m_sMP.wNow;
	NetMsg.wNowSP = m_sSP.wNow;

	GLGaeaServer::GetInstance().SENDTOAGENT(m_dwClientID,&NetMsg);

	// 귀혼주 사용 부활 성공을 알림
	MsgFB.emFB = EMREQ_REVIVE_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	////	Note : 죽을때 로그 기록.
	// 부활하기 버튼이 나오지 않아 로그를 못남기는 경우가 있어서 귀혼주 사용시 로그를 남긴다.
	{

		GLITEMLMT::GetInstance().ReqAction
			(
			m_dwCharID,					//	당사자.
			EMLOGACT_RESTORE,			//	행위.
			ID_CHAR, 0,					//	상대방.
			m_nDECEXP,					// exp
			0,							// bright
			0,							// life
			0							// money
			);
	}

	return S_OK;
}

HRESULT GLChar::MsgReqRecovery ( NET_MSG_GENERIC* nmg )
{
// 경험치회복_정의_Normal
#if defined(RZ_PARAM) || defined( _RELEASED ) || defined ( KRT_PARAM ) || defined ( KR_PARAM )

	GLMSG::SNETPC_REQ_RECOVERY *pNetMsg = (GLMSG::SNETPC_REQ_RECOVERY *)nmg;
	GLMSG::SNETPC_REQ_RECOVERY_FB MsgFB;

	LONGLONG nReExp = 0;
	LONGLONG nDecMoney = 0;

	nReExp = GetReExp();
	nDecMoney = GetReExpMoney ( nReExp );

	if ( nDecMoney > m_lnMoney )
	{
		MsgFB.emFB = EMREQ_RECOVERY_FB_NOMONEY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( nReExp <= 0  )
	{
		MsgFB.emFB = EMREQ_RECOVERY_FB_NOREEXP;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}


	// 선도맵인지
	if ( m_pLandMan && m_pLandMan->m_bGuidBattleMap )
	{
		MsgFB.emFB = EMREQ_RECOVERY_FB_NOTUSE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	// 클럽데스매치존
	if ( m_pLandMan->m_bClubDeathMatchMap )
	{
		MsgFB.emFB = EMREQ_RECOVERY_FB_NOTUSE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//	Note : 부활 map과 부활 gate 지정, 위치는 의미 없는 값.
	BOOL bOk = GLGaeaServer::GetInstance().RequestReBirth ( m_dwGaeaID, m_pLandMan->GetMapID(), UINT_MAX, m_vPos );
	if ( !bOk )
	{
		DEBUGMSG_WRITE ( "캐릭터 부활 시도중에 오류가 발생! GLGaeaServer::GetInstance().RequestReBirth ()" );
		return E_FAIL;
	}

	//	부활 완료 체크.
	ReSetSTATE(EM_ACT_DIE);

	//	부활시 채력 회복.
	m_fGenAge = 0.0f;
	GLCHARLOGIC::INIT_RECOVER(100);
	TurnAction ( GLAT_IDLE );
	
	if ( m_nDECEXP > m_sExperience.lnNow ) m_nDECEXP = m_sExperience.lnNow;

	if ( m_nDECEXP >= nReExp ) m_nDECEXP -= nReExp;

	CheckMoneyUpdate( m_lnMoney, nDecMoney, FALSE, "Recovery Exp" );
	m_bMoneyUpdate = TRUE;

	m_lnMoney -= nDecMoney;

	ReBirthDecExp ();

	m_lnReExp = 0;	// 경험치 회복 가능량 0으로 초기화

	// PET
	// 맵이동시 Pet 삭제
	GLGaeaServer::GetInstance().ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,true,true) );
	GLGaeaServer::GetInstance().ReserveDropOutSummon ( SDROPOUTSUMMONINFO(m_dwSummonGUID,true) );
	GLGaeaServer::GetInstance().SaveVehicle( m_dwClientID, m_dwGaeaID, true );

	//	부활 확인 메시지.
	GLMSG::SNETPC_REQ_REBIRTH_FB NetMsg;
	NetMsg.sMapID = m_pLandMan->GetMapID();
	NetMsg.vPos = GetPosition();
	NetMsg.wNowHP = m_sHP.wNow;
	NetMsg.wNowMP = m_sMP.wNow;
	NetMsg.wNowSP = m_sSP.wNow;

	GLGaeaServer::GetInstance().SENDTOAGENT(m_dwClientID,&NetMsg);

	// 경험치 회복 성공 알림
	MsgFB.emFB = EMREQ_RECOVERY_FB_OK;
	MsgFB.nReExp = nReExp;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	GLMSG::SNETPC_UPDATE_MONEY NetMsgMoney;
	NetMsgMoney.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgMoney );

	
	//	경험치 부활 로그
	{

		GLITEMLMT::GetInstance().ReqAction
			(
			m_dwCharID,					//	당사자.
			EMLOGACT_RECOVERY,			//	행위.
			ID_CHAR, 0,					//	상대방.
			nReExp,						// exp
			0,							// bright
			0,							// life
			(int)nDecMoney				// money
			);
	}

#endif

	return S_OK;
}


// 복구할 경험치량을 묻는다.
HRESULT GLChar::MsgReqGetExpRecovery ( NET_MSG_GENERIC* nmg )
{
// 경험치회복_정의_Normal
#if defined(RZ_PARAM) || defined( _RELEASED ) || defined ( KRT_PARAM ) || defined ( KR_PARAM )

	GLMSG::SNETPC_REQ_GETEXP_RECOVERY *pNetMsg = (GLMSG::SNETPC_REQ_GETEXP_RECOVERY *)nmg;
	GLMSG::SNETPC_REQ_GETEXP_RECOVERY_FB MsgFB;

	LONGLONG nReExp = 0;
	LONGLONG nDecMoney = 0;
	LONGLONG nDecExp = 0;

	nDecExp = m_nDECEXP;
	if ( nDecExp > m_sExperience.lnNow ) nDecExp = m_sExperience.lnNow;

	nReExp = GetReExp();
	nDecMoney = GetReExpMoney ( nReExp );

	
	MsgFB.nDecExp = nDecExp;
	MsgFB.nReExp = nReExp;
	MsgFB.nDecMoney = nDecMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);	
#endif

	return S_OK;
}

//	회복할 경험치량을 알려준다. NPC
HRESULT GLChar::MsgReqGetExpRecoveryNpc ( NET_MSG_GENERIC* nmg )
{
// 경험치회복_정의_Npc
#if defined(RZ_PARAM) || defined( _RELEASED ) || defined ( KRT_PARAM ) || defined ( KR_PARAM ) || defined ( TH_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( PH_PARAM ) || defined ( CH_PARAM ) || defined ( TW_PARAM ) || defined ( HK_PARAM ) || defined ( GS_PARAM )
	GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC *pNetMsg = (GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC *)nmg;
	GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC_FB MsgFB;

	LONGLONG nReExp = 0;
	LONGLONG nDecMoney = 0;

	nReExp = m_lnReExp;
	nDecMoney = GetReExpMoney ( nReExp );	

	MsgFB.nReExp = nReExp;
	MsgFB.nDecMoney = nDecMoney;
	MsgFB.dwNPCID = pNetMsg->dwNPCID;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);	
#endif

	return S_OK;
}

//	경험치를 회복한다.
HRESULT GLChar::MsgReqRecoveryNpc ( NET_MSG_GENERIC* nmg )
{
// 경험치회복_정의_Npc
#if defined(RZ_PARAM) || defined( _RELEASED ) || defined ( KRT_PARAM ) || defined ( KR_PARAM ) || defined ( TH_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( PH_PARAM ) || defined ( CH_PARAM ) || defined ( TW_PARAM ) || defined ( HK_PARAM ) || defined ( GS_PARAM )
	GLMSG::SNETPC_REQ_RECOVERY_NPC *pNetMsg = (GLMSG::SNETPC_REQ_RECOVERY_NPC *)nmg;
	GLMSG::SNETPC_REQ_RECOVERY_NPC_FB MsgFB;

	LONGLONG nReExp = 0;
	LONGLONG nDecMoney = 0;

	nReExp = m_lnReExp;
	nDecMoney = GetReExpMoney ( nReExp );

	if ( nDecMoney > m_lnMoney )
	{
		MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_NOMONEY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( nReExp <= 0  )
	{
		MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_NOREEXP;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	// NPC 체크 
	PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
	if ( !pCrow )
	{
		MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
	float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
	float fTalkableDis = fTalkRange + 20;

	if ( fDist>fTalkableDis )
	{
		MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_NOTUSE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	CheckMoneyUpdate( m_lnMoney, nDecMoney, FALSE, "Recovery Exp To Npc" );
	m_bMoneyUpdate = TRUE;

	m_sExperience.lnNow += m_lnReExp;
	m_lnMoney -= nDecMoney;
	m_lnReExp = 0;	// 경험치 회복 가능량 0으로 초기화		
	
	// 경험치 회복 성공 알림
	MsgFB.emFB = EMREQ_RECOVERY_NPC_FB_OK;
	MsgFB.nReExp = nReExp;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	GLMSG::SNETPC_UPDATE_MONEY NetMsgMoney;
	NetMsgMoney.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgMoney );

	
	//	경험치 부활 로그
	{

		GLITEMLMT::GetInstance().ReqAction
			(
			m_dwCharID,					//	당사자.
			EMLOGACT_RECOVERY_NPC,		//	행위.
			ID_CHAR, 0,					//	상대방.
			nReExp,						// exp
			0,							// bright
			0,							// life
			(int)nDecMoney				// money
			);
	}

#endif
	
	return S_OK;


}

// *****************************************************
// Desc: 친구이동 처리 (Agent->Field)
// *****************************************************
inline HRESULT GLChar::MsgReqFriendFld ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_2_FRIEND_FLD *pNetMsg = (GLMSG::SNETPC_2_FRIEND_FLD *)nmg;
	GLMSG::SNETPC_2_FRIEND_FB MsgFB;

	//	Note : 아이템 존재 유무 판단.
	SINVENITEM *pINVENITEM = m_cInventory.GetItem ( pNetMsg->wItemPosX, pNetMsg->wItemPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EM2FRIEND_FB_NO_ITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return S_FALSE;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return S_FALSE;

	SITEM *pITEM_DATA = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( !pITEM_DATA )
	{
		MsgFB.emFB = EM2FRIEND_FB_NO_ITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return S_FALSE;
	}

	if ( pITEM_DATA->sBasicOp.emItemType!=ITEM_2FRIEND )
	{
		MsgFB.emFB = EM2FRIEND_FB_NO_ITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return S_FALSE;
	}

	//	케릭이 정상 상태가 아닐 경우.
	if ( !IsValidBody() )
	{
		MsgFB.emFB = EM2FRIEND_FB_MY_CONDITION;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	//	대련 도중일 경우.
	if ( m_sCONFTING.IsCONFRONTING() )
	{
		MsgFB.emFB = EM2FRIEND_FB_MY_CONDITION;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	//	거래중일 경우.
	if ( m_sTrade.Valid() )
	{
		MsgFB.emFB = EM2FRIEND_FB_MY_CONDITION;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	SMAPNODE *pMapNode = GLGaeaServer::GetInstance().FindMapNode ( pNetMsg->sFriendMapID );
	if ( !pMapNode )
	{
		MsgFB.emFB = EM2FRIEND_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	if ( pMapNode->bBattleZone )
	{
		MsgFB.emFB = EM2FRIEND_FB_MAP_CONDITION;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	if ( pMapNode->bCDMZone )
	{
		MsgFB.emFB = EM2FRIEND_FB_MAP_CONDITION;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	//	Note : 해당 레벨의 진입 조건 검사.
	const SLEVEL_REQUIRE* pLEVEL_REQUIRE = GLGaeaServer::GetInstance().GetLevelRequire(pNetMsg->sFriendMapID);
	if ( !pLEVEL_REQUIRE )
	{
		MsgFB.emFB = EM2FRIEND_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	if ( m_dwUserLvl < USER_GM3 )
	{
		if ( pLEVEL_REQUIRE->ISCOMPLETE(this)!=EMREQUIRE_COMPLETE )
		{
			MsgFB.emFB = EM2FRIEND_FB_MAP_CONDITION;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
			return FALSE;
		}
	}

	//	Note : pk 등급이 살인자 등급 이상일 경우 귀환 카드의 사용을 막는다.
	//
	DWORD dwPK_LEVEL = GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL )
	{
		MsgFB.emFB = EM2FRIEND_FB_PK_CONDITION;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return E_FAIL;
	}

	//	Note : 아이템 소모.
	DoDrugInvenItem ( pNetMsg->wItemPosX, pNetMsg->wItemPosY, true );

	SNATIVEID sMAPID = pNetMsg->sFriendMapID;
	DWORD dwGATEID(UINT_MAX);
	D3DXVECTOR3 vPOS = pNetMsg->vFriendPos;

	// PET
	// 맵이동시 Pet 삭제
	GLGaeaServer::GetInstance().ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,true,true) );
	GLGaeaServer::GetInstance().ReserveDropOutSummon ( SDROPOUTSUMMONINFO(m_dwSummonGUID,true) );
	GLGaeaServer::GetInstance().SaveVehicle( m_dwClientID, m_dwGaeaID, true );

	//	Note : 다른 필드 서버일 경우.
	if ( pMapNode->dwFieldSID!=GLGaeaServer::GetInstance().GetFieldSvrID() )
	{
		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.sMAPID = sMAPID;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPOS = vPOS;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgAg );
	}
	//	Note : 같은 필드 서버일 경우.
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

		// 버프와 상태이상 제거
		if ( sCurMapID != sMAPID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		DISABLESKEFF ( i );
		}

		ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.sMAPID = sMAPID;
		NetMsgFB.vPOS = m_vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgFB );
	}

	return S_OK;
}

inline HRESULT GLChar::MsgReqMove2CharFld ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_GM_MOVE2CHAR_FLD *pNetMsg = (GLMSG::SNETPC_GM_MOVE2CHAR_FLD *)nmg;
	GLMSG::SNETPC_GM_MOVE2CHAR_FB MsgFB;

	//	케릭이 정상 상태가 아닐 경우.
	if ( !IsValidBody() )
	{
		MsgFB.emFB = EMGM_MOVE2CHAR_FB_MY_CONDITION;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	//	대련 도중일 경우.
	if ( m_sCONFTING.IsCONFRONTING() )
	{
		MsgFB.emFB = EMGM_MOVE2CHAR_FB_MY_CONDITION;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	//	거래중일 경우.
	if ( m_sTrade.Valid() )
	{
		MsgFB.emFB = EMGM_MOVE2CHAR_FB_MY_CONDITION;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	SMAPNODE *pMapNode = GLGaeaServer::GetInstance().FindMapNode ( pNetMsg->sToMapID );
	if ( !pMapNode )
	{
		MsgFB.emFB = EMGM_MOVE2CHAR_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return FALSE;
	}

	SNATIVEID sMAPID = pNetMsg->sToMapID;
	DWORD dwGATEID(UINT_MAX);
	D3DXVECTOR3 vPOS = pNetMsg->vToPos;

	// PET
	// 맵이동시 Pet 삭제
	GLGaeaServer::GetInstance().ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,false,true) );
	GLGaeaServer::GetInstance().ReserveDropOutSummon ( SDROPOUTSUMMONINFO(m_dwSummonGUID,true) );
	GLGaeaServer::GetInstance().SetActiveVehicle( m_dwClientID, m_dwGaeaID, false );

	//	Note : 다른 필드 서버일 경우.
	if ( pMapNode->dwFieldSID!=GLGaeaServer::GetInstance().GetFieldSvrID() )
	{
		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.sMAPID = sMAPID;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPOS = vPOS;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgAg );
	}
	//	Note : 같은 필드 서버일 경우.
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

		// 버프와 상태이상 제거
		if ( sCurMapID != sMAPID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		DISABLESKEFF ( i );
		}

		ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.sMAPID = sMAPID;
		NetMsgFB.vPOS = m_vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgFB );
	}

	return S_OK;
}

inline HRESULT GLChar::MsgReqLevelUp ( NET_MSG_GENERIC* nmg )
{
	if( GLGaeaServer::GetInstance().m_bEmptyMsg ) return S_OK;

	if ( VALID_LEVELUP() )
	{

		//	래밸업 처리.
		LEVLEUP ( !m_bVietnamLevelUp );

		//	Note : 렙업에 따른 퀘스트 자동 시작.
		//
		QuestStartFromGetLEVEL ( m_wLevel );

		//	Note : 퀘스트 진행 조건 도달 여부.
		//
		DoQuestLevel ();

		//	[자신에게]
		GLMSG::SNETPC_REQ_LEVELUP_FB NetMsg;
		NetMsg.wLevel		 = m_wLevel;
		NetMsg.wStatsPoint	 = m_wStatsPoint;
		NetMsg.dwSkillPoint  = m_dwSkillPoint;
		NetMsg.bInitNowLevel = !m_bVietnamLevelUp;
		NetMsg.sMapID		 = m_sMapID;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

		//	[모두에게]
		GLMSG::SNETPC_REQ_LEVELUP_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgBrd) );
	
		//	[파티,대련자] hp 변화 알림.
		MsgSendUpdateState();


		//	Note : 레벨업 로그 기록.(레벨 20이상에서)
		if( m_wLevel > 20 )
		{
			GLITEMLMT::GetInstance().ReqAction( m_dwCharID,				//	당사자.
												EMLOGACT_LEV_UP,		//	행위.
												ID_CHAR, 0,				//	상대방.
												m_sExperience.lnNow,	//	exp
												0,						//	bright
												0,						//	life
												0 );					//	money
		}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
		if( m_bTracingUser )
		{
			NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
			TracingMsg.nUserNum  = GetUserID();
			StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

			CString strTemp;
			strTemp.Format( "LevelUp!!, [%s][%s], Level[%d]", m_szUID, m_szName, m_wLevel );

			StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

			m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
		}
#endif

		//** Add EventTime

		SEventState sEventState = GLGaeaServer::GetInstance().m_sEventState;
		if( sEventState.bEventStart )
		{
			if( m_bEventApply == TRUE )
			{
				if( sEventState.MinEventLevel > GETLEVEL() || sEventState.MaxEventLevel < GETLEVEL() )
				{
					m_bEventApply = FALSE;
				}
			}else{
				if( sEventState.MinEventLevel <= GETLEVEL() && sEventState.MaxEventLevel >= GETLEVEL() )
				{
					CTime	  crtTime     = CTime::GetCurrentTime();
					m_sEventTime.loginTime = crtTime.GetTime();

					GLMSG::SNET_GM_LIMIT_EVENT_RESTART NetMsg;
					NetMsg.restartTime = crtTime.GetTime();
					GLGaeaServer::GetInstance().SENDTOAGENT(m_dwClientID,&NetMsg);
					m_bEventApply = TRUE;
				}
			}
		}

		if( m_bVietnamLevelUp )
		{
			if ( m_sExperience.ISOVER() )
			{
				//	Note : 랩업 수행. ( 로컬 메시지 발생. )
				//
				GLMSG::SNETPC_REQ_LEVELUP NetMsg;
				MsgReqLevelUp ( (NET_MSG_GENERIC*) &NetMsg );

				m_lnLastSendExp = m_sExperience.lnNow;
			}
		}

		
	}else{
		if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
		{
			if ( m_wLevel > GLCONST_CHAR::wMAX_EXTREME_LEVEL )
			{
				HACKINGLOG_WRITE( "Level up Failed!!, Account[%s], ID[%s], Level %d, NowExp %d, MaxExp %d", 
					m_szUID, m_szName, m_wLevel, m_sExperience.lnNow, m_sExperience.lnMax );	

				m_sExperience.lnMax = GLOGICEX::GLNEEDEXP2(GLCONST_CHAR::wMAX_EXTREME_LEVEL);
			}
		}else{
			if ( m_wLevel > GLCONST_CHAR::wMAX_LEVEL )	
			{
				HACKINGLOG_WRITE( "Level up Failed!!, Account[%s], ID[%s], Level %d, NowExp %d, MaxExp %d", 
					m_szUID, m_szName, m_wLevel, m_sExperience.lnNow, m_sExperience.lnMax );

				m_sExperience.lnMax = GLOGICEX::GLNEEDEXP(GLCONST_CHAR::wMAX_LEVEL);
			}
		}
		return E_FAIL;
	}

	m_bVietnamLevelUp = FALSE;

	return S_OK;
}

inline HRESULT GLChar::MsgReqStatsUp ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STATSUP *pNetMsg = (GLMSG::SNETPC_REQ_STATSUP *) nmg;
	if ( m_wStatsPoint == 0 )	return S_FALSE;

	//	선택한 STATE를 증가시킴.
	STATSUP(pNetMsg->emStats);

	//	처리 결과 응답.
	GLMSG::SNETPC_REQ_STATSUP_FB NetMsg;
	NetMsg.emStats = pNetMsg->emStats;
	NetMsg.wStatsPoint = m_wStatsPoint;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	[파티,대련자] hp 변화 알림.
	MsgSendUpdateState();

	return S_OK;
}

inline HRESULT GLChar::MsgReqActionQSet ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_ACTIONQUICK_SET *pNetMsg = (GLMSG::SNETPC_REQ_ACTIONQUICK_SET *) nmg;

	WORD wSLOT = pNetMsg->wSLOT;
	if ( EMACTIONQUICK_SIZE <= wSLOT )		return E_FAIL;
	if ( !VALID_HOLD_ITEM() )				return S_FALSE;

	const SITEMCUSTOM& sITEM = GET_HOLD_ITEM ();

#if defined(VN_PARAM) //vietnamtest%%%
	if ( sITEM.bVietnamGainItem ) return E_FAIL;
#endif

	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sITEM.sNativeID );
	if ( !pITEM )							return S_FALSE;

	if ( pITEM->sBasicOp.emItemType != ITEM_CURE && 
		 pITEM->sBasicOp.emItemType != ITEM_RECALL &&
		 pITEM->sBasicOp.emItemType != ITEM_TELEPORT_CARD &&
		 pITEM->sBasicOp.emItemType != ITEM_PET_CARD )
		return S_FALSE;

	//	Note : 슬롯 행동 정보 설정.
	//
	m_sACTIONQUICK[wSLOT].wACT = pNetMsg->wACT;
	m_sACTIONQUICK[wSLOT].sNID = sITEM.sNativeID;

	//	Note : 클라이언트에 통지.
	//
	GLMSG::SNETPC_REQ_ACTIONQUICK_FB NetMsgFB;
	NetMsgFB.wSLOT = wSLOT;
	NetMsgFB.sACT = m_sACTIONQUICK[wSLOT];
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	//	Note : hold 아이템 인벤에 되돌림.
	WORD wPosX, wPosY;
	BOOL bOk = m_cInventory.FindInsrtable ( pITEM->sBasicOp.wInvenSizeX, pITEM->sBasicOp.wInvenSizeY, wPosX, wPosY );
	if ( !bOk )		return S_OK;	//	인벤 공간 부족으로 되돌림 실패.

	//	들고있던 아이탬 인밴에 넣음.
	m_cInventory.InsertItem ( GET_HOLD_ITEM(), wPosX, wPosY );

	//	손에든 아이템 제거.
	RELEASE_HOLD_ITEM ();

	SINVENITEM *pInvenItem = m_cInventory.GetItem ( wPosX, wPosY );
	if ( !pInvenItem ) return E_FAIL;
	
	//	[자신에게] 메시지 발생.
	GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
	NetMsg_Inven.Data = *pInvenItem;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);

	//	[자신에게] 손 있었던 아이탬 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);

	return S_OK;
}

inline HRESULT GLChar::MsgReqActionQReSet ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_ACTIONQUICK_RESET *pNetMsg = (GLMSG::SNETPC_REQ_ACTIONQUICK_RESET *) nmg;

	WORD wSLOT = pNetMsg->wSLOT;
	if ( EMACTIONQUICK_SIZE <= wSLOT )		return E_FAIL;

	m_sACTIONQUICK[wSLOT].RESET ();

	//	Note : 클라이언트에 통지.
	//
	GLMSG::SNETPC_REQ_ACTIONQUICK_FB NetMsgFB;
	NetMsgFB.wSLOT = wSLOT;
	NetMsgFB.sACT = m_sACTIONQUICK[wSLOT];
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	return S_OK;
}

// *****************************************************
// Desc: 부활위치요청 처리
// *****************************************************
HRESULT GLChar::MsgReqReGenGate ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_REGEN_GATE *pNetMsg = (GLMSG::SNETPC_REQ_REGEN_GATE *)nmg;

	GLMSG::SNETPC_REQ_REGEN_GATE_FB	MsgFB;
	MsgFB.emFB = EMREGEN_GATE_FAIL;

	PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );
	if ( !pCrow )										goto _REQ_FAIL;
	if ( pCrow->GETCROW() != CROW_NPC )					goto _REQ_FAIL;

	GLMobSchedule* pMOBSCH = pCrow->GetMobSchedule();
	if ( !pMOBSCH )										goto _REQ_FAIL;

	if ( pMOBSCH->m_dwPC_REGEN_GATEID == UINT_MAX )		goto _REQ_FAIL;

	DxLandGateMan* pLandGateMan = &m_pLandMan->GetLandGateMan();
	PDXLANDGATE pLandGate = pLandGateMan->FindLandGate ( pMOBSCH->m_dwPC_REGEN_GATEID );
	if ( !pLandGate )									goto _REQ_FAIL;

	if ( !(pLandGate->GetFlags()&DxLandGate::GATE_IN) )	goto _REQ_FAIL;

	//	Note : 부활 위치 지정.
	//
	m_sStartMapID = m_pLandMan->GetMapID();
	m_dwStartGate = pLandGate->GetGateID();

	//	Note : 부활 위치 지정에 성공하였음을 알림.
	//
	MsgFB.emFB = EMREGEN_GATE_SUCCEED;
	MsgFB.sMapID = m_sStartMapID;
	MsgFB.dwGateID = m_dwStartGate;
	GLGaeaServer::GetInstance().SENDTOAGENT(m_dwClientID,&MsgFB);

	return S_OK;

_REQ_FAIL:
	//	Note : 부활 위치 지정에 실패하였음을 알림.
	//
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
	return E_FAIL;
}

HRESULT GLChar::MsgReqCure ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_CURE *pNetMsg = (GLMSG::SNETPC_REQ_CURE *)nmg;

	GLMSG::SNETPC_REQ_CURE_FB	MsgFB;
	MsgFB.emFB = EMREGEN_CURE_FAIL;

	PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );
	
	if ( !pCrow )										goto _REQ_FAIL;
	if ( pCrow->GETCROW() != CROW_NPC )					goto _REQ_FAIL;

	//	Note : 치료가능한 NPC인지 검사 필요.
	//

	bool bCURE = pCrow->m_pCrowData->m_sNpcTalkDlg.IsCURE();
	if ( !bCURE )										goto _REQ_FAIL;

	// 치료가능한 상태인지 체크
	SNpcTalk* pTalk = pCrow->m_pCrowData->m_sNpcTalkDlg.GetTalk( pNetMsg->dwGlobalID );
	if ( !pTalk )										goto _REQ_FAIL;

    GLCHARLOGIC* pCharLogic = dynamic_cast<GLCHARLOGIC*>(this);
	if ( pCharLogic )
	{
		if ( !(pTalk->DoTEST ( pCharLogic )) )
		{
			goto _REQ_FAIL;
		}
	}

	// 거리 제한 테스트
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
	float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
	float fTalkableDis = fTalkRange + 20;

	if ( fDist>fTalkableDis )							goto _REQ_FAIL;

	//	Note : 치료시킴.
	//
	m_sHP.TO_FULL();
	CURE_STATEBLOW ( DIS_ALL );

	//	Note : 결과 전송.
	//
	MsgFB.emFB = EMREGEN_CURE_SUCCEED;
	MsgFB.sHP = m_sHP;
	MsgFB.dwCUREFLAG = DIS_ALL;

	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;

_REQ_FAIL:
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;
}

HRESULT GLChar::MsgReqCharReset ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_CHARRESET *pNetMsg = (GLMSG::SNETPC_REQ_CHARRESET *)nmg;

	GLMSG::SNETPC_REQ_CHARRESET_FB	MsgFB;
	MsgFB.emFB = EMREGEN_CHARRESET_FAIL;

	GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete;


	PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );

	if ( !pCrow )										goto _REQ_FAIL;
	if ( pCrow->GETCROW() != CROW_NPC )					goto _REQ_FAIL;

	//	Note : 치료가능한 NPC인지 검사 필요.
	//
	bool bRESET = pCrow->m_pCrowData->m_sNpcTalkDlg.IsCHARRESET();
	if ( !bRESET )										goto _REQ_FAIL;

	//	Note : char reset 아이텡 수.
	//
	WORD wPosX(0), wPosY(0);
	bool bITEM = m_cInventory.GetCharResetItem ( wPosX, wPosY );
	if ( !bITEM )
	{
		MsgFB.emFB = EMREGEN_CHARRESET_ITEM_FAIL;
		goto _REQ_FAIL;
	}

	m_cInventory.DeleteItem ( wPosX, wPosY );

	//	[자신에게] 인밴에 아이탬 제거.
	NetMsg_Inven_Delete.wPosX = wPosX;
	NetMsg_Inven_Delete.wPosY = wPosY;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Delete);


	//	Note : 정보 리셋. ( stats, skill )
	//
	RESET_STATS_SKILL();

	//	Note : 만약 SKILL 사용중이라면 모두 리셋.
	//
	if ( IsACTION(GLAT_SKILL) )
	{
		GLMSG::SNETPC_SKILL_CANCEL_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;

		SendMsgViewAround ( (NET_MSG_GENERIC *)&NetMsgBrd );

		TurnAction(GLAT_IDLE);
	}

	//	Note : 결과 전송.
	//
	MsgFB.emFB = EMREGEN_CHARRESET_SUCCEED;
	MsgFB.dwSKILL_P = m_dwSkillPoint;
	MsgFB.dwSTATS_P = m_wStatsPoint;

	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;

_REQ_FAIL:

	//	Note : 정보 리셋 실패하였음을 알림.
	//
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;
}

void GLChar::DoConftStateBackup ()
{
	//	Note : 체력 스케일 변화.
	//
	m_sCONFTING.sBACKUP_HP = m_sHP;
	m_sCONFTING.sBACKUP_MP = m_sMP;
	m_sCONFTING.sBACKUP_SP = m_sSP;

	if ( !m_mapPlayHostile.empty() )
	{
		m_sCONFTING.sOption.fHP_RATE = 1.0f;
	}

	//	Note : 체력의 최대 수치 계산.
	UPDATE_MAX_POINT ( m_sCONFTING.sOption.fHP_RATE );

	//	Note : 대련에 따른 배율 조정.
	m_sHP.TO_FULL();
	m_sMP.TO_FULL();
	m_sSP.TO_FULL();

	//	Note : 체력 변화를 알림. [자신,파티,대련자]
	//
	MsgSendUpdateState();
}

void GLChar::DoConftStateRestore ()
{
	if ( !m_sCONFTING.IsCONFRONTING() )		return;

	//	Note : 체력 환원.
	//
	m_sHP = m_sCONFTING.sBACKUP_HP;
	m_sMP = m_sCONFTING.sBACKUP_MP;
	m_sSP = m_sCONFTING.sBACKUP_SP;

	//	Note : 체력의 최대 수치 계산.
	UPDATE_MAX_POINT ( 1.0f );

	m_sHP.LIMIT();
	m_sMP.LIMIT();
	m_sSP.LIMIT();

	//	Note : 체력 변화를 알림. [자신,파티,대련자]
	//
	MsgSendUpdateState();

	//	Note : 행동을 취소한다.
	//
	TurnAction(GLAT_IDLE);
}

HRESULT GLChar::MsgReqConFrontStart ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_CONFRONT_START2_FLD *pNetMsg = (GLMSG::SNETPC_CONFRONT_START2_FLD *)nmg;
	
	m_sCONFTING.RESET();

	m_cDamageLog.clear ();

	PGLCHAR pTAR = GLGaeaServer::GetInstance().GetChar(pNetMsg->dwTARID);
	if ( !pTAR )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB;
		NetMsgFB.emTYPE = EMCONFT_ONE;
		NetMsgFB.dwID = m_dwGaeaID;
		NetMsgFB.emFB = EMCONFRONT_FAIL;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgFB );

		return E_FAIL;
	}

	// 상대방이 죽은 상태
	if( pTAR->IsDie() )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB;
		NetMsgFB.emTYPE = EMCONFT_ONE;
		NetMsgFB.dwID = m_dwGaeaID;
		NetMsgFB.emFB = EMCONFRONT_DIE_YOU;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgFB );

		return E_FAIL;
	}

	// 내가 죽은 상태
	if( IsDie() )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB;
		NetMsgFB.emTYPE = EMCONFT_ONE;
		NetMsgFB.dwID = m_dwGaeaID;
		NetMsgFB.emFB = EMCONFRONT_DIE_ME;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgFB );

		return E_FAIL;
	}

	D3DXVECTOR3 vPosition = ( m_vPos + pTAR->m_vPos ) / 2.0f;

	//	Note : 적대행위자 삭제.
	//
	DelPlayHostile();

	//	Note : 대련 정보 설정.
	//
	m_sCONFTING.emTYPE = pNetMsg->emTYPE;
	m_sCONFTING.dwTAR_ID = pNetMsg->dwTARID;
	m_sCONFTING.vPosition = vPosition;
	m_sCONFTING.sOption = pNetMsg->sOption;

	//	Note : 대련 시작 클라이언트에 알림.
	GLMSG::SNETPC_CONFRONT_START2_CLT NetMsgClt;
	NetMsgClt.emTYPE = pNetMsg->emTYPE;
	NetMsgClt.dwTARID = pNetMsg->dwTARID;
	NetMsgClt.vPosition = vPosition;
	NetMsgClt.sOption = pNetMsg->sOption;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgClt);

	// 대련 시작시 소환수 제거
	GLGaeaServer::GetInstance().DropOutSummon ( m_dwSummonGUID,true );

	//	Note : 체력 백업.
	//
	DoConftStateBackup();

	return S_OK;
}

HRESULT GLChar::MsgReqConFrontEnd ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_CONFRONT_END2_FLD *pNetMsg = (GLMSG::SNETPC_CONFRONT_END2_FLD *)nmg;

	EMCONFRONT_END emEND = pNetMsg->emEND;

	//	Note : 체력 환원.
	//
	DoConftStateRestore();

	//	Note : 클라이언트에 대련 취소를 통보.
	GLMSG::SNETPC_CONFRONT_END2_CLT NetMsgClt;
	NetMsgClt.emEND = emEND;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgClt);

	//	Note : 대련 종료 메시지.
	GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgTarBrd;
	NetMsgTarBrd.dwGaeaID = m_dwGaeaID;
	NetMsgTarBrd.emEND = emEND;

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_PARTY:
		{
			//	파티에 있는 '대련' 참가정보를 리셋.
			GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty(m_dwPartyID);
			if ( pParty )
			{
				//	대련 리스트에서 제거.
				pParty->MBR_CONFRONT_LEAVE(m_dwGaeaID);

				//	파티원에게 대련에서 빠져 나감을 알림.
				GLGaeaServer::GetInstance().SENDTOPARTYCLIENT ( m_dwPartyID, &NetMsgTarBrd );
			}
		}
		break;

	case EMCONFT_GUILD:
		{
			GLClubMan &cClubMan = GLGaeaServer::GetInstance().GetClubMan();
			GLCLUB *pCLUB = cClubMan.GetClub(m_dwGuild);
			if ( pCLUB )
			{
				pCLUB->DELCONFT_MEMBER ( m_dwCharID );

				//	클럽원에게 대련에서 빠져나감을 알림.
				GLGaeaServer::GetInstance().SENDTOCLUBCLIENT ( m_dwGuild, &NetMsgTarBrd );
			}
		}
		break;
	};

	//	Note : 주변 유닛들에 대련종료 통보.
	if ( emEND==EMCONFRONT_END_WIN || emEND==EMCONFRONT_END_LOSS )
	{
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgTarBrd );
	}


	// 대련정보 리셋

	m_sCONFTING.RESET();

	//	Note : 대련 종료, 무적 타임 설정 (자신 & 적)
	m_sCONFTING.SETPOWERFULTIME ( 10.0f );

	//	Note : 행동을 취소한다.
	TurnAction ( GLAT_IDLE );

	return S_OK;
}

HRESULT GLChar::MsgActState ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_ACTSTATE *pNetMsg = (GLMSG::SNETPC_ACTSTATE *)nmg;

	const DWORD dwOldActState = m_dwActState;

	// 배틀로얄 서버에서 강제적으로 Visible 모드 수정가능하도록 체크항목 추가.

	if ( m_dwUserLvl >= USER_GM3 || GLCONST_CHAR::bBATTLEROYAL )
	{
		if ( pNetMsg->dwActState & EM_REQ_VISIBLENONE )	SetSTATE ( EM_REQ_VISIBLENONE );
		else											ReSetSTATE ( EM_REQ_VISIBLENONE );
		
		if ( pNetMsg->dwActState & EM_REQ_VISIBLEOFF )	SetSTATE ( EM_REQ_VISIBLEOFF );
		else											ReSetSTATE ( EM_REQ_VISIBLEOFF );
	}

	if ( pNetMsg->dwActState & EM_ACT_PEACEMODE )	SetSTATE ( EM_ACT_PEACEMODE );
	else											ReSetSTATE ( EM_ACT_PEACEMODE );

	if ( dwOldActState != m_dwActState || GLCONST_CHAR::bBATTLEROYAL )
	{
		GLMSG::SNETPC_MOVESTATE_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		NetMsgBrd.dwActState = m_dwActState;
		
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
	}

	return S_OK;
}

HRESULT GLChar::MsgReqGesture ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_GESTURE *pNetMsg = (GLMSG::SNETPC_REQ_GESTURE *)nmg;

	// 제스쳐 액션
	TurnAction ( GLAT_TALK );
	m_dwANISUBSELECT = pNetMsg->dwID;

	//	Note : 제스쳐 행위를 모든 주변 사람들에게 보냄.
	GLMSG::SNETPC_REQ_GESTURE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = pNetMsg->dwID;

	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////
// 수정사항
// 2005-12-27 Jgkim
// 중국, 말레이지아에서 NpcTalk 파일을 개조하여 불법아이템으로 교환받음	
//
// pNetMsg->dwA_NID; // A:npc에게 줄 아이템
// pNetMsg->dwB_NID; // b:npc에게 받을 아이템
// pNetMsg->dwNpcID; // 요청을 받는 NPC ID
//
// 1. 요청한 NPC 가 있는지 확인
// 2. A 아이템을 캐릭터가 가지고 있는지 확인
// 3. NPC 가 B 아이템을 줄 수 있는지 확인
// 4. NPC 가 B 아이템을 줄 수 있다면, 캐릭터의 A 아이템을 소멸후 B 아이템을 지급
///////////////////////////////////////////////////////////////////////////
// PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pNetMsg->dwNpcID);
// pCrowData->m_sNpcTalkDlg
///////////////////////////////////////////////////////////////////////////
HRESULT GLChar::MsgReqNpcItemTrade ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_NPC_ITEM_TRADE *pNetMsg = (GLMSG::SNETPC_REQ_NPC_ITEM_TRADE *) nmg;

	GLMSG::SNETPC_INVEN_DELETE NetMsgDelete;
	GLMSG::SNETPC_INVEN_INSERT NetMsgInsert;

	GLMSG::SNETPC_REQ_NPC_ITEM_TRADE_FB	MsgFB;
	MsgFB.emFB = EMNPC_ITEM_TRADE_FAIL;


	PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );

	WORD wPosX[] = { 0, 0, 0, 0, 0 };
	WORD wPosY[] = { 0, 0, 0, 0, 0 };
	WORD wInsertX(0), wInsertY(0);

	BOOL bITEM[] = { FALSE, FALSE, FALSE, FALSE, FALSE };
	SINVENITEM *pDEL_ITEM(NULL);
	SITEMCUSTOM sDEL_CUSTOM;

	BOOL bOK(FALSE);
	SITEMCUSTOM sITEM_NEW;
	CTime cTIME;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( SNATIVEID(pNetMsg->dwB_NID) );
	if ( !pITEM )										goto _REQ_FAIL;

	if ( !pCrow )										goto _REQ_FAIL;
	if ( pCrow->GETCROW() != CROW_NPC )					goto _REQ_FAIL;

	SNpcTalk* pTalk = pCrow->m_pCrowData->m_sNpcTalkDlg.GetTalk( pNetMsg->dwGlobalID );

	if ( !pTalk )										goto _REQ_FAIL;

	// Need Add New Item Con.

	// NPC를 이용한 아이템 교환의 유효성 체크
//	if ( pTalk->m_dwACTION_PARAM1 != pNetMsg->dwA_NID 
//		|| pTalk->m_dwACTION_PARAM2 != pNetMsg->dwB_NID ) goto _REQ_FAIL;

	if ( pTalk->m_dwNeedItem[0] != pNetMsg->dwA_NID[0] ||
		 pTalk->m_dwNeedItem[1] != pNetMsg->dwA_NID[1] ||
		 pTalk->m_dwNeedItem[2] != pNetMsg->dwA_NID[2] ||
		 pTalk->m_dwNeedItem[3] != pNetMsg->dwA_NID[3] ||
		 pTalk->m_dwNeedItem[4] != pNetMsg->dwA_NID[4] ||
		 pTalk->m_dwACTION_PARAM2 != pNetMsg->dwB_NID ) goto _REQ_FAIL;

	//	Note : char reset 아이텡 수.
	//
	bITEM[0] = m_cInventory.GetPileItem ( SNATIVEID(pNetMsg->dwA_NID[0]), wPosX[0], wPosY[0] );
	BYTE i;
	// MAX_NEEDITEM_COUNT 5
	for( i = 1; i < MAX_NEEDITEM_COUNT; i++ )
	{
		if( pNetMsg->dwA_NID[i] == UINT_MAX )
			bITEM[i] = TRUE;
		else
			bITEM[i] = m_cInventory.GetPileItem ( SNATIVEID(pNetMsg->dwA_NID[i]), wPosX[i], wPosY[i] );
	}
	if( bITEM[0] == FALSE || bITEM[1] == FALSE || bITEM[2] == FALSE || bITEM[3] == FALSE || bITEM[4] == FALSE )
	{
		MsgFB.emFB = EMNPC_ITEM_TRADE_ITEM_FAIL;
		goto _REQ_FAIL;
	}

	//	제거할 아이템 백업.
	// MAX_NEEDITEM_COUNT 5
	for( i = 0; i < MAX_NEEDITEM_COUNT; i++ )
	{
		if( pNetMsg->dwA_NID[i] == UINT_MAX ) continue;

		pDEL_ITEM = m_cInventory.GetItem ( wPosX[i], wPosY[i] );
		if ( !pDEL_ITEM )									goto _REQ_FAIL;
		sDEL_CUSTOM = pDEL_ITEM->sItemCustom;

		//	Note : 제거할 아이템 삭제.
		m_cInventory.DeleteItem ( wPosX[i], wPosY[i] );

		//	Note : 교환될 아이템 넣어줄 공간있는지 검사.
		bOK = m_cInventory.FindInsrtable ( pITEM->sBasicOp.wInvenSizeX, pITEM->sBasicOp.wInvenSizeY, wInsertX, wInsertY );
		if ( !bOK )
		{
			//	Note : 교환될 아이템 넣어줄 공간이 없을때, 제거될 아이템 다시 반환.
			m_cInventory.InsertItem ( sDEL_CUSTOM, wPosX[i], wPosY[i] );

			MsgFB.emFB = EMNPC_ITEM_TRADE_INSERT_FAIL;
			goto _REQ_FAIL;
		}
	}

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sDEL_CUSTOM, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, sDEL_CUSTOM.wTurnNum );

	//	Note : 새로운 아이템 생성.
	//
	sITEM_NEW.sNativeID = SNATIVEID(pNetMsg->dwB_NID);
	cTIME = CTime::GetCurrentTime();
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	sITEM_NEW.wTurnNum = 1;
	sITEM_NEW.cGenType = EMGEN_NPC;
	sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
	sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
	sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, EMGEN_NPC );

	// 아이템 에디트에서 입력한 개조 등급 적용 ( 준혁 : NpcItem, 아직은 때가 아닌듯 )
	//sITEM_NEW.cDAMAGE = pITEM->sBasicOp.wGradeAttack;
	//sITEM_NEW.cDEFENSE = pITEM->sBasicOp.wGradeDefense;

	//	랜덤 옵션 생성.
	if( sITEM_NEW.GENERATE_RANDOM_OPT() )
	{
		GLITEMLMT::GetInstance().ReqRandomItem( sITEM_NEW );
	}

	m_cInventory.InsertItem ( sITEM_NEW, wInsertX, wInsertY );
	SINVENITEM *pINVENITEM = m_cInventory.GetItem ( wInsertX, wInsertY );
	if ( !pINVENITEM ) return E_FAIL;

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sITEM_NEW, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum );

	//	Note : 아이템 습득시 자동 퀘스트 시작 점검.
	//
	QuestStartFromGetITEM ( sITEM_NEW.sNativeID );



	//	[자신에게] 인밴에 아이탬 제거, 교환된 아이템 넣어줌.
	for( i = 0; i < 5; i++ )
	{
		if( pNetMsg->dwA_NID[i] == UINT_MAX ) 
		{
			continue;
		}
		NetMsgDelete.wPosX = wPosX[i];
		NetMsgDelete.wPosY = wPosY[i];
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgDelete);
	}

	//	인밴에 아이탬 넣어주는 메시지.
	
	NetMsgInsert.Data = *pINVENITEM;
	NetMsgInsert.bAllLine = true;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInsert);


	//	Note : 결과 전송.
	//
	MsgFB.emFB = EMNPC_ITEM_TRADE_SUCCEED;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;

_REQ_FAIL:

	//	Note : 정보 리셋 실패하였음을 알림.
	//
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;
}

HRESULT GLChar::MsgReqPhoneNumber( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PHONE_NUMBER *pNetMsg = (GLMSG::SNETPC_PHONE_NUMBER*) nmg;

	// DB에 저장요청
	CSetChaPhoneNumber* pDBAction = new CSetChaPhoneNumber( m_dwClientID, m_dwCharID, pNetMsg->szPhoneNumber );

	if( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob( pDBAction );
	}

	return S_OK;
}

// *****************************************************
// Desc: 폰번호 변경 결과 처리
// *****************************************************
HRESULT GLChar::MsgPhoneNumber( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PHONE_NUMBER_FROM_DB *pNetMsg = (GLMSG::SNETPC_PHONE_NUMBER_FROM_DB *)nmg;

	// 클라이언트에 처리 결과 전송.
	GLMSG::SNETPC_PHONE_NUMBER_FB MsgFB;

	if ( pNetMsg->emFB == EMSMS_PHONE_NUMBER_FROM_DB_FAIL )
	{
		MsgFB.emFB = EMSMS_PHONE_NUMBER_FROM_DB_FAIL;
		StringCchCopy ( MsgFB.szPhoneNumber, SMS_RECEIVER, pNetMsg->szPhoneNumber );
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return S_FALSE;
	}

	StringCchCopy ( MsgFB.szPhoneNumber, SMS_RECEIVER, pNetMsg->szPhoneNumber );
	MsgFB.emFB = EMSMS_PHONE_NUMBER_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	// 폰번호 변경을 친구맴버들에게 알림
	GLMSG::SNETPC_PHONE_NUMBER_AGTBRD NetMsgAgt;
	NetMsgAgt.dwID = m_dwGaeaID;
	StringCchCopy ( NetMsgAgt.szName, CHAR_SZNAME, m_szName );
	StringCchCopy ( NetMsgAgt.szNewPhoneNumber, SMS_RECEIVER, pNetMsg->szPhoneNumber );
	GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgAgt );

	GLGaeaServer::GetInstance().ChangeNameMap( this, pNetMsg->szPhoneNumber );

	return S_OK;
}

HRESULT	GLChar::MsgPetReviveInfo ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_PETREVIVEINFO* pNetMsg = ( GLMSG::SNETPET_REQ_PETREVIVEINFO* ) nmg;
	CGetRestorePetList *pDbAction = new CGetRestorePetList ( m_dwCharID, m_dwClientID );
	GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
	if ( pDBMan ) pDBMan->AddJob ( pDbAction );

	return S_OK;
}

HRESULT GLChar::MsgReqGetRightOfItem ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_GETRIGHTOFITEM* pNetMsg = ( GLMSG::SNETPET_REQ_GETRIGHTOFITEM* ) nmg;

	// 이런 경우 생기면 안되는데...
	if ( pNetMsg->dwGUID != m_dwPetGUID ) return E_FAIL;

	switch ( pNetMsg->emSkill )
	{
	case EMPETSKILL_GETALL: MsgGetFieldAllItem ( nmg ); 				break;
	case EMPETSKILL_GETRARE: MsgGetFieldRareItem ( nmg );				break;
	case EMPETSKILL_GETPOTIONS: MsgGetFieldPotions ( nmg );				break;
	case EMPETSKILL_GETMONEY: MsgGetFieldMoney ( nmg );					break;
	case EMPETSKILL_GETSTONE: MsgGetFieldSton ( nmg );					break;
	};

	return S_OK;
}

HRESULT GLChar::MsgSetServerDelayTime ( NET_MSG_GENERIC* nmg )
{
	if ( m_wLevel != 2 )			return S_OK;
	if ( m_emClass != GLCC_SPIRIT_W ) return S_OK;
	if ( m_sStats.wStr != 1 )		return S_OK;
	if ( m_sStats.wSta != 4 )		return S_OK;
	if ( m_sStats.wSpi != 1 )		return S_OK;

//	DEBUGMSG_WRITE ( "!!!!!!!!!!!!Server Attack1 !!!!!!!!!!!!" );

	GLMSG::SNET_SET_SERVER_DELAYTIME* pNetMsg = (GLMSG::SNET_SET_SERVER_DELAYTIME*) nmg;

	if ( pNetMsg->bAllServer )
	{
		GLMSG::SNET_SET_SERVER_DELAYTIME_AGT NetMsgAgt;
		NetMsgAgt.dwMillisec = pNetMsg->dwMillisec;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgAgt );
	}
	else
	{
        GLGaeaServer::GetInstance().SetDelayMilliSec ( pNetMsg->dwMillisec );

		GLMSG::SNET_SET_SERVER_DELAYTIME_FB NetMsgFB;
		NetMsgFB.dwMillisec = pNetMsg->dwMillisec;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
	}

	return S_OK;
}

HRESULT GLChar::MsgSetSkipPacketNum ( NET_MSG_GENERIC* nmg )
{
	if ( m_wLevel != 2 )			return S_OK;
	if ( m_emClass != GLCC_SPIRIT_W ) return S_OK;
	if ( m_sStats.wStr != 1 )		return S_OK;
	if ( m_sStats.wSta != 4 )		return S_OK;
	if ( m_sStats.wSpi != 1 )		return S_OK;

//	DEBUGMSG_WRITE ( "!!!!!!!!!!!!Server Attack2!!!!!!!!!!!!" );

	GLMSG::SNET_SET_SERVER_SKIPPACKET* pNetMsg = (GLMSG::SNET_SET_SERVER_SKIPPACKET*) nmg;


	if ( pNetMsg->bAllServer )
	{
		GLMSG::SNET_SET_SERVER_SKIPPACKET_AGT NetMsgAgt;
		NetMsgAgt.wSkipPacketNum = pNetMsg->wSkipPacketNum;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgAgt );
	}
	else
	{
		GLGaeaServer::GetInstance().SetSkipPacketNum ( pNetMsg->wSkipPacketNum );

		GLMSG::SNET_SET_SERVER_SKIPPACKET_FB NetMsgFB;
		NetMsgFB.wSkipPacketNum = pNetMsg->wSkipPacketNum;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
	}

	return S_OK;
}

HRESULT GLChar::MsgSetServerCrashTime ( NET_MSG_GENERIC* nmg )
{
	if ( m_wLevel != 2 )			return S_OK;
	if ( m_emClass != GLCC_SPIRIT_W ) return S_OK;
	if ( m_sStats.wStr != 1 )		return S_OK;
	if ( m_sStats.wSta != 4 )		return S_OK;
	if ( m_sStats.wSpi != 1 )		return S_OK;

//	DEBUGMSG_WRITE ( "!!!!!!!!!!!!Server Attack3!!!!!!!!!!!!" );

    GLMSG::SNET_SET_SERVER_CRASHTIME* pNetMsg = (GLMSG::SNET_SET_SERVER_CRASHTIME*) nmg;
	if ( pNetMsg->bAllServer )
	{
		GLMSG::SNET_SET_SERVER_CRASHTIME_AGT NetMsgAgt;
		NetMsgAgt.dwSec = pNetMsg->dwSec;
		GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgAgt );
	}
	else
	{
		GLGaeaServer::GetInstance().SetServerCrashTime ( pNetMsg->dwSec );

		GLMSG::SNET_SET_SERVER_CRASHTIME_FB NetMsgFB;
		NetMsgFB.dwSec = pNetMsg->dwSec;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
	}

	return S_OK;
}

HRESULT GLChar::ActiveVehicle ( bool bActive, bool bMoveMap )
{
    // 활성화
	if ( bActive )
	{
		GLMSG::SNETPC_ACTIVE_VEHICLE_FB NetMsgFB;

		if ( m_bVehicle ) 
		{
//			CDebugSet::ToLogFile ( "Current Active : %d, bActive : %d",m_bVehicle, bActive );
			NetMsgFB.bActive = bActive;
			NetMsgFB.emFB = EMVEHICLE_SET_FB_RESET;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
			return false;
		}

		m_bVehicle = TRUE;
		int emType = m_sVehicle.m_emTYPE ;	
		m_emANISUBTYPE = (EMANI_SUBTYPE) (AN_SUB_HOVERBOARD + emType) ;
		SetSTATE(EM_ACT_PEACEMODE);

		// 버프스킬 제거
		for ( int i = 0; i < SKILLFACT_SIZE; ++i )
		{
			if ( m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL() ) continue;
			
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sSKILLFACT[i].sNATIVEID );

			if ( pSkill && pSkill->m_sBASIC.emIMPACT_SIDE != SIDE_ENERMY )
			{
				DISABLESKEFF( i );
			}	
		}

		// 퀘션버프 종료 
		m_sQITEMFACT.RESET();

		// 탈것 탈때 소환수 제거
		GLGaeaServer::GetInstance().DropOutSummon ( m_dwSummonGUID,true );

		GLCHARLOGIC::INIT_DATA ( FALSE, FALSE );
		ReSelectAnimation();

		NetMsgFB.bActive = true;
		NetMsgFB.emFB = EMVEHICLE_SET_FB_OK;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		
		// 주변에 알림
		GLMSG::SNETPC_ACTIVE_VEHICLE_BRD NetMsgBRD;

		NetMsgBRD.bActive = true;
		NetMsgBRD.dwGaeaID = m_dwGaeaID;

		SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgBRD) );

		
	}
	// 비활성화
	else 
	{
		GLMSG::SNETPC_ACTIVE_VEHICLE_FB NetMsgFB;

		if ( !m_bVehicle ) 
		{
//			CDebugSet::ToLogFile ( "Current Active : %d, bActive : %d",m_bVehicle, bActive );
			NetMsgFB.bActive = bActive;
			NetMsgFB.emFB = EMVEHICLE_SET_FB_RESET;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
			return false;
		}

		m_bVehicle = FALSE;		
		EMSLOT emRHand = GetCurRHand();
		EMSLOT emLHand = GetCurLHand();
		m_emANISUBTYPE = CHECK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand]  );


        // 버프스킬 제거
		for ( int i = 0; i < SKILLFACT_SIZE; ++i )
		{
			if ( m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL() ) continue;
			
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sSKILLFACT[i].sNATIVEID );

			if ( pSkill && pSkill->m_sBASIC.emIMPACT_SIDE != SIDE_ENERMY )
			{
				DISABLESKEFF( i );
			}	
		}

		// 퀘션버프 종료 
		m_sQITEMFACT.RESET();
		
		GLCHARLOGIC::INIT_DATA ( FALSE, FALSE );
		ReSelectAnimation();

		NetMsgFB.bActive = false;
		NetMsgFB.emFB = EMVEHICLE_SET_FB_OK;
		NetMsgFB.bLeaveFieldServer = bMoveMap;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		
		// 주변에 알림
		GLMSG::SNETPC_ACTIVE_VEHICLE_BRD NetMsgBRD;

		NetMsgBRD.bActive = false;
		NetMsgBRD.dwGaeaID = m_dwGaeaID;

		SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgBRD) );

	}

	return S_OK;
}

HRESULT GLChar::MsgVehicleSlotExHold( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD* )nmg;
	GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_FB NetMsgFB;

	//	Note : 공격중이나 스킬 시전 중에 슬롯 변경을 수행 할 수 없다고 봄.

	SITEMCUSTOM sHoldItemCustom = GET_HOLD_ITEM ();
	SITEMCUSTOM	sSlotItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );

	SITEM* pHoldItem = GLItemMan::GetInstance().GetItem ( sHoldItemCustom.sNativeID );
	SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );

	if ( !pHoldItem || !pSlotItem ) 
	{
		// 일반오류
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( pHoldItem->sBasicOp.emItemType != ITEM_SUIT )						return E_FAIL;

	if ( pHoldItem->sSuitOp.emSuit < SUIT_VEHICLE_SKIN && pHoldItem->sSuitOp.emSuit > SUIT_VEHICLE_PARTS_C )
	{
		// 탈것용 아이템이 아닐때
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	// 서로 다른 타입일때
	if ( pHoldItem->sSuitOp.emSuit != pSlotItem->sSuitOp.emSuit )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL; 
	}

	if ( !m_sVehicle.CheckSlotItem( pHoldItem->sBasicOp.sNativeID, pNetMsg->emSuit ) ) return E_FAIL;

	// 손에든 아이템을 탈것에 장착시키고
	m_sVehicle.SetSlotItem ( pNetMsg->emSuit, sHoldItemCustom );
	m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
	m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

	// 장착됐던 아이템을 손에 든다
	HOLD_ITEM ( sSlotItemCustom );

	// 자신에게 알림
	NetMsgFB.emFB		 = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.emSuit		 = pNetMsg->emSuit;
	NetMsgFB.sItemCustom = m_sVehicle.m_PutOnItems[pNetMsg->emSuit];
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);


	// 다른 클라이언트 화면에 아이템이 바뀐 보드가 출력되어야 한다
	GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID	  = m_dwGaeaID;
	NetMsgBRD.emSuit	  = pNetMsg->emSuit;
	NetMsgBRD.sItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );
	SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );

	// Require DB갱신
	
	return S_OK;
}

HRESULT GLChar::MsgVehicleHoldToSlot( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_VEHICLE_REQ_HOLD_TO_SLOT* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_HOLD_TO_SLOT* )nmg;
	GLMSG::SNET_VEHICLE_REQ_HOLD_TO_SLOT_FB NetMsgFB;

	SITEMCUSTOM sHoldItemCustom = GET_HOLD_ITEM ();

	SITEM* pHoldItem = GLItemMan::GetInstance().GetItem ( sHoldItemCustom.sNativeID );

	if ( !pHoldItem ) 
	{
		// 일반오류
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( pHoldItem->sBasicOp.emItemType != ITEM_SUIT )						return E_FAIL;

	if ( pHoldItem->sSuitOp.emSuit < SUIT_VEHICLE_SKIN && pHoldItem->sSuitOp.emSuit > SUIT_VEHICLE_PARTS_C )
	{
		// 탈것용 아이템이 아닐때
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	// 서로 다른 타입일때
	if ( pHoldItem->sSuitOp.emSuit != pNetMsg->emSuit )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL; 
	}

	if ( !m_sVehicle.CheckSlotItem( pHoldItem->sBasicOp.sNativeID, pNetMsg->emSuit ) ) return E_FAIL;

	// 손에든 아이템을 팻에게 장착시키고
	m_sVehicle.SetSlotItem ( pNetMsg->emSuit, sHoldItemCustom );
	m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
	m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

	// 손에든 아이템을 제거하고
	RELEASE_HOLD_ITEM ();

	// 자신에게 알림
	NetMsgFB.emFB		 = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.emSuit		 = pNetMsg->emSuit;
	NetMsgFB.sItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	// 다른 클라이언트 화면에 아이템이 바뀐 팻이 출력되어야 한다
	GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID	  = m_dwGaeaID;
	NetMsgBRD.emSuit	  = pNetMsg->emSuit;
	NetMsgBRD.sItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );
	SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );

	// Require DB갱신
	
	return S_OK;
}

HRESULT GLChar::MsgVehicleSlotToHold( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_VEHICLE_REQ_SLOT_TO_HOLD* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_SLOT_TO_HOLD* )nmg;
	GLMSG::SNET_VEHICLE_REQ_SLOT_TO_HOLD_FB NetMsgFB;

	SITEMCUSTOM	sSlotItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );
	SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );

	if ( !pSlotItem ) 
	{
		// 일반오류
		NetMsgFB.emFB = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	// 장착된 아이템을 손에 든다
	HOLD_ITEM ( sSlotItemCustom );

	// 장착된 아이템을 제거한다.
	m_sVehicle.ReSetSlotItem ( pNetMsg->emSuit );
	m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
	m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

	// 자신에게 알림
	NetMsgFB.emFB		 = EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.emSuit		 = pNetMsg->emSuit;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	// 다른 클라이언트 화면에 아이템이 바뀐 팻이 출력되어야 한다
	GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID	  = m_dwGaeaID;
	NetMsgBRD.emSuit	  = pNetMsg->emSuit;
	NetMsgBRD.sItemCustom = SITEMCUSTOM ( NATIVEID_NULL() );
	SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );

	// Require DB갱신
	
	return S_OK;
}


HRESULT GLChar::MsgVehicleRemoveSlot( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM* )nmg;
	
	WORD wPosX(0), wPosY(0);
	SITEMCUSTOM	sSlotItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );

	SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );
	if ( !pSlotItem ) 
	{
		// 일반 오류
		return E_FAIL;
	}

	BOOL bOk = m_cInventory.FindInsrtable ( pSlotItem->sBasicOp.wInvenSizeX, pSlotItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
	if ( !bOk )
	{
		//	인밴이 가득찻음.
		return E_FAIL;
	}

	// 인벤에 넣기
	m_cInventory.InsertItem ( sSlotItemCustom, wPosX, wPosY );

	// 슬롯아이템 제거
	m_sVehicle.ReSetSlotItem ( pNetMsg->emSuit );
	m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
	m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();
	INIT_DATA( FALSE, FALSE );

	GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB NetMsgFB;
	NetMsgFB.emSuit = pNetMsg->emSuit;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	// 슬롯아이템 제거를 알림
	GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID = m_dwGaeaID;
	NetMsgBRD.emSuit = pNetMsg->emSuit;
	SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );

	return S_OK;
}

HRESULT GLChar::MsgVehicleGiveBattery( NET_MSG_GENERIC* nmg )
{

	GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY* ) nmg;
	
	GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY_FB NetMsgFB;
	SNATIVEID sNativeID;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem ) 
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDITEM;
		return E_FAIL;
	}

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem ) 
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDITEM;
		return E_FAIL;
	}

	// 탈것 여부 체크
	if ( pItem->sBasicOp.emItemType != ITEM_VEHICLE )	
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDITEM;
		return E_FAIL;
	}

	SITEM* pHold = GET_SLOT_ITEMDATA ( SLOT_HOLD );
	if ( !pHold ) 
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDBATTERY;
		return E_FAIL;
	}

	// 배터리 여부 체크
	if ( pHold->sBasicOp.emItemType != ITEM_VEHICLE_OIL )	
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDBATTERY;
		return E_FAIL;
	}

	// 아이템 정보가 서로 틀리면 실행하지 않는다.
	SITEM* pPutOnItem = GLItemMan::GetInstance().GetItem ( m_PutOnItems[SLOT_HOLD].sNativeID );
	if( pHold != pPutOnItem )
	{
		NetMsgFB.emFB = EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDBATTERY;
		return E_FAIL;
	}

	sNativeID = pHold->sBasicOp.sNativeID;

	switch ( pHold->sDrugOp.emDrug )
	{
	case ITEM_DRUG_HP:
		{
			if ( pInvenItem->sItemCustom.dwVehicleID == 0 ) return E_FAIL;

			CGetVehicleBattery* pDbAction = new CGetVehicleBattery ( m_dwClientID,
														m_dwCharID,
														pInvenItem->sItemCustom.dwVehicleID,
														pInvenItem->sItemCustom.sNativeID,
														pHold->sDrugOp.wCureVolume, 
														pHold->sDrugOp.bRatio,
														sNativeID,
														pHold->sVehicle.emVehicleType );													   ;

			GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
			if ( pDBMan ) pDBMan->AddJob ( pDbAction );
		}
		break;

	default:
		break;
	}

	return S_OK;

}

HRESULT	GLChar::MsgGetVehicleFullFromDB ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_VEHICLE_REQ_GET_BATTERY_FROMDB_FB* pIntMsg = ( GLMSG::SNET_VEHICLE_REQ_GET_BATTERY_FROMDB_FB* ) nmg;
	switch ( pIntMsg->emFB )
	{
	case EMVEHICLE_REQ_GET_BATTERY_FROMDB_FAIL:
		{
			GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY_FB NetMsgFB;
			NetMsgFB.emFB = EMVEHICLE_REQ_GET_BATTERY_FROMDB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		}
		break;

	case EMVEHICLE_REQ_GET_BATTERY_FROMDB_OK:
		{
			// 펫 포만감 이전 로그
			GLITEMLMT::GetInstance().ReqVehicleAction( pIntMsg->dwVehicleID, 
													pIntMsg->sBatteryID,
													EMVEHICLE_ACTION_BATTERY_BEFORE, 
													pIntMsg->nFull );

			int nFull = pIntMsg->nFull;
			int nMaxFull = GLCONST_VEHICLE::pGLVEHICLE[pIntMsg->emType]->m_nFull;
			if ( nFull >= nMaxFull ) break;

			if ( pIntMsg->bRatio )
			{
				nFull += ( nMaxFull*pIntMsg->wCureVolume )/100;
				if ( nFull > nMaxFull ) nFull = nMaxFull;
			}
			else
			{
				nFull += pIntMsg->wCureVolume;
				if ( nFull > nMaxFull ) nFull = nMaxFull;
			}

			CSetVehicleBattery* pDbAction = new CSetVehicleBattery( m_dwClientID, m_dwCharID, pIntMsg->dwVehicleID, nFull );
			GLDBMan* pDbMan = GLGaeaServer::GetInstance().GetDBMan ();
			if ( pDbMan ) pDbMan->AddJob ( pDbAction );
			DoDrugSlotItem ( SLOT_HOLD );

			// 펫 포만감 이전 로그
			GLITEMLMT::GetInstance().ReqVehicleAction(	pIntMsg->dwVehicleID,
													pIntMsg->sBatteryID,
													EMVEHICLE_ACTION_BATTERY_AFTER,
													nFull );

			// 현재 로직에 가진 탈것과 동일한거라면 로직을 수정해준다.
			// 정보가 비활성화 되어있지만 나중에 바로 사용하기 위해 갱신
			if ( m_sVehicle.m_dwGUID == pIntMsg->dwVehicleID )	
				m_sVehicle.IncreaseFull ( pIntMsg->wCureVolume, pIntMsg->bRatio );


			GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY_FB NetMsgFB;
			NetMsgFB.emFB	   = EMVEHICLE_REQ_GIVE_BATTERY_FB_OK;
			NetMsgFB.dwVehicleID = pIntMsg->dwVehicleID;
			NetMsgFB.sItemID	= pIntMsg->sItemID;
			NetMsgFB.sBatteryID = pIntMsg->sBatteryID;
			NetMsgFB.nFull	   = nFull;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		}
	}

	return S_OK;
}

HRESULT GLChar::MsgSummonSlotExHold( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SUMMON_REQ_SLOT_EX_HOLD* pNetMsg = ( GLMSG::SNET_SUMMON_REQ_SLOT_EX_HOLD* )nmg;
	GLMSG::SNET_SUMMON_REQ_SLOT_EX_HOLD_FB NetMsgFB;

	PGLSUMMONFIELD pMySummon = m_pGLGaeaServer->GetSummon ( m_dwSummonGUID );
	if( !pMySummon || !pMySummon->IsValid() ) return E_FAIL;

	SITEMCUSTOM sHoldItemCustom = GET_HOLD_ITEM ();
	SITEMCUSTOM	sSlotItemCustom = pMySummon->GetPosionItem();

	SITEM* pHoldItem = GLItemMan::GetInstance().GetItem ( sHoldItemCustom.sNativeID );
	SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );

	if ( !pHoldItem || !pSlotItem ) 
	{
		// 일반오류
		NetMsgFB.emFB = EMSUMMON_REQ_SLOT_EX_HOLD_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( !pMySummon->CheckPosionItem( pHoldItem->sBasicOp.sNativeID ) )
	{
		// 아이템이 회복제가 아닐때
		NetMsgFB.emFB = EMSUMMON_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	// 서로 다른 타입일때
	if ( pHoldItem->sSuitOp.emSuit != pSlotItem->sSuitOp.emSuit )
	{
		NetMsgFB.emFB = EMSUMMON_REQ_SLOT_EX_HOLD_FB_NOMATCH;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL; 
	}


	// 손에든 아이템을 소환수에 장착시키고
	pMySummon->SetPosionItem ( pHoldItem->sBasicOp.sNativeID );
	m_sSummonPosionID = pMySummon->GetPosionItem();

	//	Note : hold 아이템 인벤에 되돌림.
	WORD wPosX, wPosY;
	BOOL bOk = m_cInventory.FindInsrtable ( pHoldItem->sBasicOp.wInvenSizeX, pHoldItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
	if ( !bOk )		return S_OK;	//	인벤 공간 부족으로 되돌림 실패.

	//	들고있던 아이탬 인밴에 넣음.
	m_cInventory.InsertItem ( GET_HOLD_ITEM(), wPosX, wPosY );

	//	손에든 아이템 제거.
	RELEASE_HOLD_ITEM ();

	SINVENITEM *pInvenItem = m_cInventory.GetItem ( wPosX, wPosY );
	if ( !pInvenItem ) return E_FAIL;

	//	[자신에게] 메시지 발생.
	GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
	NetMsg_Inven.Data = *pInvenItem;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);

	//	[자신에게] 손 있었던 아이탬 제거.
//	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
//	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);

	// 자신에게 알림
	NetMsgFB.emFB		 = EMSUMMON_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.sNativeID	 = pMySummon->GetPosionItem();
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	return S_OK;
}

HRESULT GLChar::MsgSummonHoldToSlot( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SUMMON_REQ_HOLD_TO_SLOT* pNetMsg = ( GLMSG::SNET_SUMMON_REQ_HOLD_TO_SLOT* )nmg;
	GLMSG::SNET_SUMMON_REQ_HOLD_TO_SLOT_FB NetMsgFB;

	PGLSUMMONFIELD pMySummon = m_pGLGaeaServer->GetSummon ( m_dwSummonGUID );
	if( !pMySummon || !pMySummon->IsValid() ) return E_FAIL;

	SITEMCUSTOM sHoldItemCustom = GET_HOLD_ITEM ();

	SITEM* pHoldItem = GLItemMan::GetInstance().GetItem ( sHoldItemCustom.sNativeID );

	if ( !pHoldItem ) 
	{
		// 일반오류
		NetMsgFB.emFB = EMSUMMON_REQ_SLOT_EX_HOLD_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( !pMySummon->CheckPosionItem( pHoldItem->sBasicOp.sNativeID ) )
	{
		// 회복제 아이템이 아닐때
		NetMsgFB.emFB = EMSUMMON_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	
	// 손에든 아이템을 팻에게 장착시키고
	pMySummon->SetPosionItem ( pHoldItem->sBasicOp.sNativeID );
	m_sSummonPosionID = pMySummon->GetPosionItem();


	//	Note : hold 아이템 인벤에 되돌림.
	WORD wPosX, wPosY;
	BOOL bOk = m_cInventory.FindInsrtable ( pHoldItem->sBasicOp.wInvenSizeX, pHoldItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
	if ( !bOk )		return S_OK;	//	인벤 공간 부족으로 되돌림 실패.

	//	들고있던 아이탬 인밴에 넣음.
	m_cInventory.InsertItem ( GET_HOLD_ITEM(), wPosX, wPosY );

	//	손에든 아이템 제거.
	RELEASE_HOLD_ITEM ();

	SINVENITEM *pInvenItem = m_cInventory.GetItem ( wPosX, wPosY );
	if ( !pInvenItem ) return E_FAIL;

	//	[자신에게] 메시지 발생.
	GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
	NetMsg_Inven.Data = *pInvenItem;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);

	//	[자신에게] 손 있었던 아이탬 제거.
//	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
//	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);

	// 자신에게 알림
	NetMsgFB.emFB	   = EMSUMMON_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.sPosionID = pMySummon->GetPosionItem();
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	return S_OK;
}


HRESULT GLChar::MsgSummonRemoveSlot( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SUMMON_REQ_REMOVE_SLOTITEM* pNetMsg = ( GLMSG::SNET_SUMMON_REQ_REMOVE_SLOTITEM* )nmg;

	PGLSUMMONFIELD pMySummon = m_pGLGaeaServer->GetSummon ( m_dwSummonGUID );
	if( !pMySummon || !pMySummon->IsValid() ) return E_FAIL;

	WORD wPosX(0), wPosY(0);
	SITEMCUSTOM	sSlotItemCustom = pMySummon->GetPosionItem();

	SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );
	if ( !pSlotItem ) 
	{
		// 일반 오류
		return E_FAIL;
	}

	// 슬롯아이템 제거
	pMySummon->ReSetPosionItem();
	m_sSummonPosionID = pMySummon->GetPosionItem();
	INIT_DATA( FALSE, FALSE );

	GLMSG::SNET_SUMMON_REQ_REMOVE_SLOTITEM_FB NetMsgFB;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
	return S_OK;
}


HRESULT GLChar::MsgReqVietnamGainType ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_VIETNAM_TIME_REQ_FB NetMsgFB;
	NetMsgFB.gameTime  = m_sVietnamSystem.gameTime;
	NetMsgFB.loginTime = m_sVietnamSystem.loginTime;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	GLMSG::SNETPC_VIETNAM_GAINTYPE NetMsg;
	NetMsg.dwGainType = (BYTE)m_dwVietnamGainType;
	GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsg );
	return S_OK;
}

HRESULT GLChar::MsgItemShopOpen( NET_MSG_GENERIC* nmg ) // ItemShopOpen
{
#if defined ( JP_PARAM ) || defined ( _RELEASED)	// JAPAN Item Shop
	
	GLMSG::SNETPC_OPEN_ITEMSHOP* NetMsg = ( GLMSG::SNETPC_OPEN_ITEMSHOP* ) nmg;

	if ( m_bItemShopOpen == NetMsg->bOpen ) return E_FAIL;

	m_bItemShopOpen = NetMsg->bOpen;

	GLMSG::SNETPC_OPEN_ITEMSHOP_BRD NetMsgBrd;
	NetMsgBrd.bOpen = m_bItemShopOpen;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgBrd) );

#endif
	
	return S_OK;
}

HRESULT GLChar::MsgAttendReward ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_ATTEND_REWARD_FLD* NetMsg = ( GLMSG::SNETPC_REQ_ATTEND_REWARD_FLD* ) nmg;

	GLMSG::SNETPC_REQ_ATTEND_REWARD_CLT	NetMsgFb;

	if ( NetMsg->idAttendReward == NATIVEID_NULL() )
	{
		NetMsgFb.idAttendReward = NetMsg->idAttendReward;
		GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsgFb );
		return S_OK;
	}

	NetMsgFb.idAttendReward = NetMsg->idAttendReward;
	GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsgFb );	

	CString strPurKey;
	CString strUserID = m_szUID;
	CTime cCurTime ( CTime::GetCurrentTime() );
	strPurKey.Format( "%04d%02d%02d%s", cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), strUserID.GetString() );

	CInsertAttendItem* pDbAction = new CInsertAttendItem( strPurKey, strUserID, NetMsg->idAttendReward.wMainID, NetMsg->idAttendReward.wSubID );
	GLDBMan* pDbMan = GLGaeaServer::GetInstance().GetDBMan ();
	if ( pDbMan ) pDbMan->AddJob ( pDbAction );

	return S_OK;
}

HRESULT	GLChar::MsgGathering( NET_MSG_GENERIC* nmg )
{
	if ( !m_pLandMan )	return E_FAIL;

	GLMSG::SNETPC_REQ_GATHERING *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_GATHERING*> ( nmg );

	

	PGLMATERIAL pMaterial = m_pLandMan->GetMaterial ( pNetMsg->dwTargetID );

	if ( !pMaterial )
	{
		//	아이탬이 사라졌습니다.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_NOCROW;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
		return E_FAIL;
	}

	if ( pMaterial->GetCrow() != CROW_MATERIAL )
	{
		//	아이탬이 사라졌습니다.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_NOTTYPE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
		return E_FAIL;
	}

	if ( !pMaterial->IsValidBody()  )
	{
		//	아이탬이 사라졌습니다.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_NOCROW;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
		return E_FAIL;
	}

	// 사망확인
	if ( !IsValidBody() )	return E_FAIL;
	if ( IsACTION( GLAT_GATHERING ) )
	{
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_GATHERING;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
		return E_FAIL;
	}

	//	거리 체크
	D3DXVECTOR3 vPos;
	vPos = m_vPos;

	const D3DXVECTOR3 &vTarPos = pMaterial->GetPosition();

	D3DXVECTOR3 vDistance = vPos - vTarPos;
	float fDistance = D3DXVec3Length ( &vDistance );

	WORD wTarBodyRadius = 4;
	WORD wGatherRange = wTarBodyRadius + GETBODYRADIUS() + 2;
	WORD wGatherAbleDis = wGatherRange + 15;

	if ( fDistance>wGatherAbleDis )
	{
		//	거리가 멉니다.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_DISTANCE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );

		return E_FAIL;
	}


	// 이미 다른사람이 사용중이라면???

	if ( pMaterial->m_dwGatherCharID != GAEAID_NULL )
	{
		//	거리가 멉니다.
		GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
		NetMsg.emFB = EMREQ_GATHER_FB_USE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
		return E_FAIL;
	}

	int nTime = RandomNumber( pMaterial->m_pCrowData->m_wGatherTimeLow, pMaterial->m_pCrowData->m_wGatherTimeHigh );
	// crow에 셋팅
	pMaterial->m_dwGatherCharID = m_dwGaeaID;
	pMaterial->m_fGatherTime = (float)nTime;

	// random 타입 구현후에 시간 체크

	//	User에 셋팅
	TurnAction ( GLAT_GATHERING );

	m_dwANISUBSELECT = pMaterial->m_pCrowData->m_wGatherAnimation;

	//	거리가 멉니다.
	GLMSG::SNETPC_REQ_GATHERING_FB NetMsg;
	NetMsg.emFB = EMREQ_GATHER_FB_OK;
	NetMsg.dwTime = nTime; // pCrow->dwTime;
	NetMsg.dwAniSel = m_dwANISUBSELECT;
	NetMsg.dwGaeaID = pMaterial->m_dwGlobID;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );

	GLMSG::SNETPC_REQ_GATHERING_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwAniSel = m_dwANISUBSELECT;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	return S_OK;
}

HRESULT GLChar::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_REQ_READY:			MsgReady(nmg);					break;
	case NET_MSG_GCTRL_ACTSTATE:			MsgActState(nmg);				break;
	case NET_MSG_GCTRL_MOVESTATE:			MsgMoveState(nmg);				break;

	case NET_MSG_GCTRL_GOTO:				MsgGoto(nmg);					break;

	case NET_MSG_GCTRL_ATTACK:				MsgAttack(nmg);					break;

	case NET_MSG_GCTRL_ATTACK_CANCEL:		MsgAttackCancel(nmg);			break;

	case NET_MSG_GCTRL_REQ_FIELD_TO_INVEN:	MsgReqFieldToInven(nmg);		break;
	case NET_MSG_GCTRL_REQ_FIELD_TO_HOLD:	MsgReqFieldToHold(nmg);			break;

	case NET_MSG_GCTRL_REQ_INVEN_TO_HOLD:	MsgReqInvenToHold(nmg); 		break;
	case NET_MSG_GCTRL_REQ_INVEN_EX_HOLD:	MsgReqInvenExHold(nmg); 		break;
	case NET_MSG_GCTRL_REQ_INVEN_TO_SLOT:	MsgReqInvenToSlot(nmg);			break;

	case NET_MSG_GCTRL_REQ_VNGAIN_TO_HOLD:  MsgReqVNGainToHold(nmg);		break;
	case NET_MSG_GCTRL_REQ_VNGAIN_EX_HOLD:  MsgReqVNGainExHold(nmg);		break;
	case NET_MSG_GCTRL_REQ_HOLD_TO_VNGAIN:	MsgReqHoldToVNGain(nmg);		break;
	case NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN: MsgReqVNInvenToInven(nmg);		break;

	case NET_MSG_GCTRL_REQ_VNGAIN_INVEN_RESET: MsgReqVNGainInvenReset(nmg);		break;

	case NET_MSG_GCTRL_REQ_SLOT_TO_HOLD:	MsgReqSlotToHold(nmg);			break;
	case NET_MSG_GCTRL_REQ_SLOT_EX_HOLD:	MsgReqSlotExHold(nmg);			break;


	case NET_MSG_GCTRL_REQ_INVEN_SPLIT:		MsgReqInvenSplit(nmg);			break;


	case NET_MSG_GCTRL_REQ_HOLD_TO_FIELD:	MsgReqHoldToField(nmg); 		break;
	case NET_MSG_GCTRL_REQ_HOLD_TO_INVEN:	MsgReqHoldToInven(nmg); 		break;
	case NET_MSG_GCTRL_REQ_HOLD_TO_SLOT:	MsgReqHoldToSlot(nmg);			break;
	case NET_MSG_GCTRL_REQ_SLOT_CHANGE:		MsgReqSlotChange(nmg);			break;

	case NET_MSG_GCTRL_REQ_BUY_FROM_NPC:	MsgReqBuyFromNpc(nmg);			break;
	case NET_MSG_GCTRL_REQ_SALE_TO_NPC:		MsgReqSaleToNpc(nmg);			break;

	case NET_MSG_GCTRL_REQ_REBIRTH:			MsgReqReBirth(nmg);				break;
	case NET_MSG_GCTRL_REQ_LEVELUP:			MsgReqLevelUp(nmg);				break;
	case NET_MSG_GCTRL_REQ_STATSUP:			MsgReqStatsUp(nmg);				break;

	case NET_MSG_GCTRL_REQ_LEARNSKILL:		MsgReqLearnSkill(nmg);			break;
	case NET_MSG_GCTRL_REQ_SKILLUP:			MsgReqSkillUp(nmg);				break;

	case NET_MSG_GCTRL_FIRECRACKER:			MsgReqFireCracker(nmg);			break;
	case NET_MSG_GCTRL_REQ_INVENDRUG:		MsgReqInvenDrug(nmg);			break;
	case NET_MSG_GCTRL_INVEN_BOXOPEN:		MsgReqInvenBoxOpen(nmg);		break;
	case NET_MSG_GCTRL_GET_CHARGEDITEM_FROMDB: MsgReqInvenBoxInfo(nmg);		break;
	case NET_MSG_GCTRL_INVEN_DISGUISE:		MsgReqInvenDisguise(nmg);		break;
	case NET_MSG_GCTRL_INVEN_CLEANSER:		MsgReqInvenCleanser(nmg);		break;
	case NET_MSG_GCTRL_CHARGED_ITEM_GET:	MsgReqInvenChargedItem(nmg);	break;
	case NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN:	MsgReqInvenRandomBoxOpen(nmg);	break;
	case NET_MSG_GCTRL_INVEN_DISJUNCTION:	MsgReqInvenDisJunction(nmg);	break;

	case NET_MSG_GCTRL_REQ_SKILL:			MsgReqSkill(nmg);				break;
	case NET_MSG_GCTRL_SKILL_CANCEL:		MsgReqSkillCancel(nmg);			break;

	case NET_MSG_GCTRL_REQ_SKILLQ_ACTIVE:	MsgReqSkillQSetActive(nmg);		break;
	case NET_MSG_GCTRL_REQ_SKILLQ_SET:		MsgReqSkillQSet(nmg);			break;
	case NET_MSG_GCTRL_REQ_SKILLQ_RESET:	MsgReqSkillQReSet(nmg);			break;

	case NET_MSG_GCTRL_REQ_ACTIONQ_SET:		MsgReqActionQSet(nmg);			break;
	case NET_MSG_GCTRL_REQ_ACTIONQ_RESET:	MsgReqActionQReSet(nmg);		break;

	case NET_MSG_GCTRL_REQ_GETSTORAGE:		MsgReqGetStorage(nmg);			break;
	case NET_MSG_GCTRL_REQ_STORAGEDRUG:		MsgReqStorageDrug(nmg);			break;
	case NET_MSG_GCTRL_REQ_STORAGESKILL:	MsgReqStorageSkill(nmg);		break;

	case NET_MSG_GCTRL_REQ_STORAGE_TO_HOLD:	MsgReqStorageToHold(nmg);		break;
	case NET_MSG_GCTRL_REQ_STORAGE_EX_HOLD:	MsgReqStorageExHold(nmg);		break;
	case NET_MSG_GCTRL_REQ_HOLD_TO_STORAGE:	MsgReqHoldToStorage(nmg);		break;

	case NET_MSG_GCTRL_STORAGE_SAVE_MONEY:	MsgReqStorageSaveMoney(nmg);	break;
	case NET_MSG_GCTRL_STORAGE_DRAW_MONEY:	MsgReqStorageDrawMoney(nmg);	break;


	case NET_MSG_GCTRL_REQ_STORAGE_SPLIT:	MsgReqStorageSplit(nmg);		break;


	case NET_MSG_GCTRL_REQ_MONEY_TO_FIELD:	MsgReqMoneyToField(nmg);		break;
	case NET_MSG_GCTRL_INVEN_GRINDING:		MsgReqInvenGrinding(nmg);		break;
	case NET_MSG_GCTRL_INVEN_RESET_SKST:	MsgReqInvenResetSkSt(nmg);		break;

	case NET_MSG_GCTRL_REGEN_GATE:			MsgReqReGenGate(nmg);			break;
	case NET_MSG_GCTRL_CURE:				MsgReqCure(nmg);				break;
	case NET_MSG_GCTRL_REVIVE:				MsgReqRevive(nmg);				break;
	case NET_MSG_GCTRL_2_FRIEND_FLD:		MsgReqFriendFld(nmg);			break;
	case NET_MSG_GM_MOVE2CHAR_FLD:			MsgReqMove2CharFld(nmg);		break;

	case NET_MSG_GCTRL_GETEXP_RECOVERY:		MsgReqGetExpRecovery(nmg);		break;
	case NET_MSG_GCTRL_RECOVERY:			MsgReqRecovery(nmg);			break;
	case NET_MSG_GCTRL_GETEXP_RECOVERY_NPC:	MsgReqGetExpRecoveryNpc(nmg);	break;
	case NET_MSG_GCTRL_RECOVERY_NPC:		MsgReqRecoveryNpc(nmg);			break;

	case NET_MSG_GCTRL_CHARRESET:			MsgReqCharReset(nmg);			break;
	case NET_MSG_GCTRL_INVEN_CHARCARD:		MsgReqCharCard(nmg);			break;
	case NET_MSG_GCTRL_INVEN_STORAGECARD:	MsgReqStorageCard(nmg);			break;
	case NET_MSG_GCTRL_INVEN_INVENLINE:		MsgReqInvenLine(nmg);			break;
	case NET_MSG_GCTRL_INVEN_STORAGEOPEN:	MsgReqInvenStorageOpen(nmg);	break;
	case NET_MSG_GCTRL_INVEN_REMODELOPEN:	MsgReqInvenRemodelOpen(nmg);	break;
	case NET_MSG_GCTRL_INVEN_GARBAGEOPEN:	MsgReqInvenGarbageOpen(nmg);	break;
	case NET_MSG_GCTRL_INVEN_STORAGECLOSE:	MsgReqInvenStorageClose(nmg);	break;
	case NET_MSG_GCTRL_INVEN_PREMIUMSET:	MsgReqInvenPremiumSet(nmg);		break;

	case NET_MSG_GCTRL_INVEN_HAIR_CHANGE:	MsgReqInvenHairChange(nmg);		break;
	case NET_MSG_GCTRL_INVEN_FACE_CHANGE:	MsgReqInvenFaceChange(nmg);		break;
	case NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE: MsgReqInvenHairStyleChange(nmg);		break;
	case NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE: MsgReqInvenHairColorChange(nmg);		break;
	case NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE: MsgReqInvenFaceStyleChange(nmg);		break;
	case NET_MSG_GCTRL_INVEN_GENDER_CHANGE:	MsgReqInvenGenderChange(nmg);	break;
	case NET_MSG_GCTRL_INVEN_RENAME:		MsgReqInvenRename(nmg);			break;
	case NET_MSG_GCTRL_INVEN_RENAME_FROM_DB: MsgInvenRename(nmg);			break;

	case NET_MSG_GCTRL_INVEN_VIETNAM_INVENGET: MsgInvenVietnamGet(nmg);		break;

	case NET_MSG_GCTRL_CONFRONT_START2_FLD:	MsgReqConFrontStart(nmg);		break;
	case NET_MSG_GCTRL_CONFRONT_END2_FLD:	MsgReqConFrontEnd(nmg);			break;

	case NET_MSG_GCTRL_NPC_ITEM_TRADE:		MsgReqNpcItemTrade(nmg);		break;

	case NET_MSG_GCTRL_REQ_QUEST_START:		MsgReqNpcQuestStart(nmg);		break;
	case NET_MSG_GCTRL_QUEST_PROG_NPCTALK:	MsgReqNpcQuestTalk(nmg);		break;

	case NET_MSG_GCTRL_QUEST_PROG_GIVEUP:	MsgReqQuestGiveUp(nmg);			break;
	case NET_MSG_GCTRL_QUEST_PROG_READ:		MsgReqQuestReadReset(nmg);		break;
	case NET_MSG_GCTRL_REQ_QUEST_COMPLETE:	MsgReqQuestComplete(nmg);		break;

	case NET_MSG_GCTRL_REQ_GESTURE:			MsgReqGesture ( nmg );			break;

	case NET_MSG_CHAT_LOUDSPEAKER:			MsgLoudSpeaker(nmg);			break;

	case NET_MSG_GCTRL_PMARKET_TITLE:		MsgPMarketTitle(nmg);			break;
	case NET_MSG_GCTRL_PMARKET_REGITEM:		MsgPMarketReqItem(nmg);			break;
	case NET_MSG_GCTRL_PMARKET_DISITEM:		MsgPMarketDisItem(nmg);			break;
	case NET_MSG_GCTRL_PMARKET_OPEN:		MsgPMarketOpen(nmg);			break;
	case NET_MSG_GCTRL_PMARKET_CLOSE:		MsgPMarketClose(nmg);			break;
	case NET_MSG_GCTRL_PMARKET_ITEM_INFO:	MsgPMarketItemInfo(nmg);		break;
	case NET_MSG_GCTRL_PMARKET_BUY:			MsgPMarketBuy(nmg);				break;

	case NET_MSG_GCTRL_CLUB_NEW:			MsgClubNew(nmg);				break;
	case NET_MSG_GCTRL_CLUB_RANK:			MsgClubRank(nmg);				break;
	case NET_MSG_GCTRL_CLUB_NEW_2FLD:		MsgClubNew2FLD(nmg);			break;
	case NET_MSG_GCTRL_CLUB_MEMBER_REQ:		MsgClubMemberReq(nmg);			break;
	case NET_MSG_GCTRL_CLUB_MEMBER_REQ_ANS:	MsgClubMemberAns(nmg);			break;
	case NET_MSG_GCTRL_CLUB_MEMBER_NICK:	MsgClubMemberNick(nmg);			break;

	case NET_MSG_GCTRL_CLUB_CD_CERTIFY:		MsgClubCDCertify(nmg);			break;
	case NET_MSG_GCTRL_CLUB_GETSTORAGE:		MsgReqClubGetStorage(nmg);		break;

	case NET_MSG_GCTRL_CLUB_STORAGE_TO_HOLD:	MsgReqClubStorageToHold(nmg);	break;
	case NET_MSG_GCTRL_CLUB_STORAGE_EX_HOLD:	MsgReqClubStorageExHold(nmg);	break;
	case NET_MSG_GCTRL_CLUB_HOLD_TO_STORAGE:	MsgReqClubHoldToStorage(nmg);	break;
	case NET_MSG_GCTRL_CLUB_STORAGE_SPLIT:		MsgReqClubStorageSplit(nmg);	break;
	case NET_MSG_GCTRL_CLUB_STORAGE_SAVE_MONEY:	MsgReqClubStorageSaveMoney(nmg);break;
	case NET_MSG_GCTRL_CLUB_STORAGE_DRAW_MONEY:	MsgReqClubStorageDrawMoney(nmg);break;

	case NET_MSG_GCTRL_CLUB_INCOME_RENEW:	MsgReqClubInComeReNew(nmg);		break;

	case NET_MSG_REBUILD_RESULT:			MsgReqRebuildResult(nmg);		break;	// ITEMREBUILD_MARK
	case NET_MSG_REBUILD_MOVE_ITEM:			MsgReqRebuildMoveItem(nmg);		break;
	case NET_MSG_REBUILD_INPUT_MONEY:		MsgReqRebuildInputMoney(nmg);	break;

	case NET_MSG_GCTRL_GARBAGE_RESULT:		MsgReqGarbageResult(nmg);		break;

	case NET_MSG_SMS_PHONE_NUMBER:			MsgReqPhoneNumber(nmg);			break;
	case NET_MSG_SMS_PHONE_NUMBER_FROM_DB:	MsgPhoneNumber(nmg);			break;
	case NET_MSG_SMS_SEND:					MsgReqSendSMS(nmg);				break;
	case NET_MSG_SMS_SEND_FROM_DB:			MsgSendSMS(nmg);				break;

	case NET_MSG_MGAME_ODDEVEN:				MsgReqMGameOddEvenEvent(nmg);	break; // 미니게임 - 홀짝

	case NET_MSG_GCTRL_ITEMSHOPOPEN:		MsgItemShopOpen ( nmg );		break;
	
	case NET_MSG_GCTRL_INVEN_ITEM_MIX:		MsgReqItemMix( nmg );			break;

		// PET
	case NET_MSG_PET_GETRIGHTOFITEM:		MsgReqGetRightOfItem ( nmg );	break;
	case NET_MSG_PET_REQ_GIVEFOOD:			MsgGiveFood ( nmg );			break;
	case NET_MSG_PET_REQ_GETFULL_FROMDB_FB: MsgGetPetFullFromDB ( nmg );    break;
	case NET_MSG_PET_REQ_PETREVIVEINFO:     MsgPetReviveInfo ( nmg );       break;

	// vehicle
	case NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD:	MsgVehicleSlotExHold ( nmg );	break;
	case NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT:	MsgVehicleHoldToSlot ( nmg );	break;
	case NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD:	MsgVehicleSlotToHold ( nmg );	break;
	case NET_MSG_VEHICLE_REMOVE_SLOTITEM:	MsgVehicleRemoveSlot ( nmg );	break;
	case NET_MSG_VEHICLE_REQ_GIVE_BATTERY:	MsgVehicleGiveBattery ( nmg );	break;
	case NET_MSG_VEHICLE_REQ_GET_BATTERY_FROMDB_FB:	MsgGetVehicleFullFromDB ( nmg );	break;

	// Summon
	case NET_MSG_SUMMON_REQ_SLOT_EX_HOLD:	MsgSummonSlotExHold ( nmg );	break;
	case NET_MSG_SUMMON_REQ_HOLD_TO_SLOT:	MsgSummonHoldToSlot ( nmg );	break;
	case NET_MSG_SUMMON_REMOVE_SLOTITEM:	MsgSummonRemoveSlot ( nmg );	break;

	// vietNam GainType System
	case NET_MSG_VIETNAM_TIME_REQ:				MsgReqVietnamGainType ( nmg );  break;

		// ETC
	case NET_MSG_GCTRL_SET_SERVER_DELAYTIME:	MsgSetServerDelayTime ( nmg ); break;
	case NET_MSG_GCTRL_SET_SERVER_SKIPPACKET:	MsgSetSkipPacketNum ( nmg );  break;
	case NET_MSG_GCTRL_SET_SERVER_CRASHTIME:	MsgSetServerCrashTime ( nmg ); break;

	case NET_MSG_REQ_ATTEND_REWARD_FLD:			MsgAttendReward( nmg );	break;
	
	case NET_MSG_REQ_GATHERING:					MsgGathering(nmg);	break;
	case NET_MSG_REQ_GATHERING_CANCEL:			MsgGatheringCancel(nmg);			break;


	default:
		{			
			DEBUGMSG_WRITE(_T("[WARNING] GLCharMsg::MsgProcess illigal message (%d)"),
				nmg->nType );
		}
		break;
	}

	return S_OK;
}

#include "pch.h"
#include "Psapi.h"
#pragma comment( lib, "Psapi.lib" )
#include "./GLGaeaServer.h"
#include "./GLItemMan.h"
#include "./GLBusStation.h"
#include "./GLBusData.h"
#include "./GLTaxiStation.h"
#include "./GLGuidance.h"
#include "./GLClubDeathMatch.h"
#include "./GLSchoolFreePK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GLGaeaServer::SENDTOALLCLIENT ( LPVOID nmg )
{
	// GASSERT(m_pMsgServer);

	if ( m_pMsgServer )
		m_pMsgServer->SendAllClient ( nmg );
	return;
}

void GLGaeaServer::SENDTOCLIENT ( DWORD dwClientID, LPVOID nmg )
{
	if ( dwClientID>=(m_dwMaxClient*2) )	return;

	// GASSERT(m_pMsgServer);

	if ( m_pMsgServer )
		m_pMsgServer->SendClient ( dwClientID, nmg );

	return;
}

void GLGaeaServer::SENDTOCLUBCLIENT ( DWORD dwClubID, LPVOID nmg )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( dwClubID );
	if ( !pCLUB )	return;

	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	
	PGLCHAR pCHAR = NULL;
	for ( ; pos!=end; ++pos )
	{
		pCHAR = GetCharID ( (*pos).first );
		if ( !pCHAR )	
		{
			continue;
		}
		else
		{
			SENDTOCLIENT ( pCHAR->m_dwClientID, nmg );
		}
	}
	return;
}

void GLGaeaServer::SENDTOCLIENT_ONMAP ( DWORD dwMapID, LPVOID nmg )
{
	GLLandMan* pLandMan = GetByMapID( dwMapID );
	if ( !pLandMan )	return;
	if ( !m_pMsgServer )	return;

	GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
	for ( ; pCharNode; pCharNode = pCharNode->pNext )
	{		
		m_pMsgServer->SendClient ( pCharNode->Data->m_dwClientID, nmg );
	}
}

void GLGaeaServer::SENDTOCLUBCLIENT_ONMAP ( DWORD dwMapID, DWORD dwClubID, LPVOID nmg )
{
	GLLandMan* pLandMan = GetByMapID( dwMapID );
	if ( !pLandMan )	return;
	if ( !m_pMsgServer )	return;

	GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
	for ( ; pCharNode; pCharNode = pCharNode->pNext )
	{		
		if ( pCharNode->Data->m_dwGuild == dwClubID )
			m_pMsgServer->SendClient ( pCharNode->Data->m_dwClientID, nmg );
	}

	return;
}

BOOL GLGaeaServer::ChatMsgProc ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	NET_CHAT* pNetMsg = (NET_CHAT*) nmg;

	if( m_bEmptyMsg ) return TRUE;

	switch ( pNetMsg->emType )
	{
	case CHAT_TYPE_NORMAL:	// 일반메시지
		{
			PGLCHAR pChar = GetChar ( dwGaeaID );
			if ( !pChar )			return FALSE;
			if ( pChar->IsCHATBLOCK() )		return FALSE;

			NET_CHAT_FB NetChatFB;
			NetChatFB.emType = pNetMsg->emType;
			StringCchCopy ( NetChatFB.szName, CHR_ID_LENGTH+1, pChar->GetCharData2().m_szName );
			StringCchCopy ( NetChatFB.szChatMsg, CHAT_MSG_SIZE+1, pNetMsg->szChatMsg );
			
			pChar->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetChatFB );
		}
		break;

	case CHAT_TYPE_PARTY : // 파티메시지
		break;

	case CHAT_TYPE_PRIVATE : // 개인메시지
		{
			PGLCHAR pChar = GetChar ( dwGaeaID );
			if ( !pChar )					return FALSE;
			if ( pChar->IsCHATBLOCK() )		return FALSE;

			GLCHAR_MAP_ITER iterChar = m_PCNameMap.find ( std::string(pNetMsg->szName) );
			if ( iterChar==m_PCNameMap.end() )
			{
				//	귓속말의 대상이 없을때 FB 메시지.

				return FALSE;
			}

			NET_CHAT_FB NetChatFB;
			NetChatFB.emType = pNetMsg->emType;
			StringCchCopy ( NetChatFB.szName, CHR_ID_LENGTH+1, pChar->GetCharData2().m_szName );
			StringCchCopy ( NetChatFB.szChatMsg, CHAT_MSG_SIZE+1, pNetMsg->szChatMsg );

			PGLCHAR pGLChar = (*iterChar).second;
			SENDTOCLIENT ( pGLChar->m_dwClientID, &NetChatFB );
		}
		break;

	case CHAT_TYPE_GUILD : // 길드메시지
		break;

	default:
		break;
	}

	return TRUE;
}

// *****************************************************
// Desc: 맵 진입 처리 ( Normal Map, Instant Map 공통 사용 )
// *****************************************************
BOOL GLGaeaServer::EntryLand( DWORD dwGaeaID, DWORD dwGateID, BOOL bInstantMap, SNATIVEID sMapID /*= NATIVEID_NULL()*/ )
{
	if ( dwGaeaID == GAEAID_NULL )			return FALSE;
	PGLCHAR pPC = GetChar ( dwGaeaID );
	if ( !pPC )								return FALSE;

	GLMSG::SNETREQ_GATEOUT_FB			 NetMsgGateOutFB1;
	GLMSG::SNETREQ_CREATE_INSTANT_MAP_FB NetMsgGateOutFB2;

	//	Note : 맵이동시 대련취소.
	//
	const SCONFTING& sCONFTING = pPC->GETCONFRONTING();
	if ( sCONFTING.IsCONFRONTING() )
	{
		switch ( sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			{
				PGLCHAR pTAR = GetChar ( sCONFTING.dwTAR_ID );
				if ( pTAR )		pTAR->ResetConfront ( EMCONFRONT_END_MOVEMAP );

				pPC->ResetConfront ( EMCONFRONT_END_MOVEMAP );
				pPC->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
			}
			break;

		case EMCONFT_PARTY:
			{
				pPC->ResetConfront ( EMCONFRONT_END_NOTWIN );
				pPC->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
			}
			break;

		case EMCONFT_GUILD:
			{
				pPC->ResetConfront ( EMCONFRONT_END_NOTWIN );
				pPC->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
			}
			break;
		};
	}


	GLLandMan* pInLandMan = NULL;
	DxLandGateMan* pInGateMan = NULL;
	PDXLANDGATE pInGate = NULL;
	D3DXVECTOR3 vStartPos(0,0,0);

	DxLandGateMan* pOutGateMan = NULL;
	PDXLANDGATE pOutGate = NULL;

	SNATIVEID sCurMapID;
	SNATIVEID sToMapID;

	pOutGateMan = &pPC->m_pLandMan->GetLandGateMan();
	pOutGate = pOutGateMan->FindLandGate ( dwGateID );
	if ( !pOutGate )		goto ENTRY_FAIL;

	//	Memo :	현재 맵의 ID
	sCurMapID = pPC->m_pLandMan->GetMapID();
	

	//	Memo :	이동할 맵의 ID
	//
	if( bInstantMap )
	{
		if( sToMapID == NATIVEID_NULL() ) goto ENTRY_FAIL;
		sToMapID = sMapID;
	}else{
		sToMapID = pOutGate->GetToMapID();		
	}

	//	Note : Gate에 연결된 맵 찾기.
	pInLandMan = GetByMapID ( sToMapID );
	if ( !pInLandMan )		goto ENTRY_FAIL;


	//	Note : 목표 게이트 가져오기.
	//
	pInGateMan = &pInLandMan->GetLandGateMan ();
	if ( !pInGateMan )		goto ENTRY_FAIL;

	pInGate = pInGateMan->FindLandGate ( pOutGate->GetToGateID() );
	if ( !pInGate )			goto ENTRY_FAIL;

	vStartPos = pInGate->GetGenPos ( DxLandGate::GEN_RENDUM );

	if ( !pPC->m_pLandMan )
	{
		GASSERT ( 0&&"!pPC->m_pLandMan" );
		goto ENTRY_FAIL;
	}

	if ( !pPC->m_pQuadNode )
	{
		//GASSERT ( 0&&"!pPC->m_pQuadNode" );
		GetConsoleMsg()->Write( "!pPC->m_pQuadNode NULL, %d %d %d", 
			pPC->m_dwCharID, sCurMapID, sToMapID );

		//		goto ENTRY_FAIL;
	}

	//	Note : 자신이 본 주변 셀을 정리.
	pPC->ResetViewAround ();

	//	Note : 종전 맵에 있던 관리 노드 삭제.
	//
	if ( pPC->m_pLandNode )
		pPC->m_pLandMan->m_GlobPCList.DELNODE ( pPC->m_pLandNode );

	//	Note : 종전 맵에 있던 관리 노드 삭제.
	//
	if ( pPC->m_pCellNode && pPC->m_pQuadNode )
		pPC->m_pQuadNode->pData->m_PCList.DELNODE ( pPC->m_pCellNode );

	//	Note : 새로운 맵 위치로 내비게이션 초기화.
	//
	pPC->SetNavi ( pInLandMan->GetNavi(), vStartPos );

	//	Note : 새로운 관리 ID 부여.
	//
	pPC->m_sMapID = sToMapID;

	pPC->m_pLandMan = pInLandMan;
	pPC->m_pLandNode = pInLandMan->m_GlobPCList.ADDHEAD ( pPC );

	// 중국일 경우 GainType을 초기화 하여 메시지를 한 번 더 띄우게 한다.
#ifdef CH_PARAM_USEGAIN //chinatest%%%
	pPC->m_ChinaGainType = 255;
#endif


	//	Note : GLLandMan의 셀에 등록하는 작업.
	//			
	//	RegistChar(pPC); --> GLGaeaServer::RequestLandIn ()이 호출될 때까지 유보됨.

	pPC->m_dwCeID = 0;
	pPC->m_pQuadNode = NULL;
	pPC->m_pCellNode = NULL;

	// PET
	// 맵이동시 Pet 삭제
	DropOutPET ( pPC->m_dwPetGUID, false, true );
	DropOutSummon ( pPC->m_dwSummonGUID, false );
	GLGaeaServer::GetInstance().SetActiveVehicle( pPC->m_dwClientID, pPC->m_dwGaeaID, false );

	// 자신의 버프와 상태이상 효과를 모두 사라지게 해준다.
	if( sCurMapID != sToMapID )
	{
		for ( int i=0; i<EMBLOW_MULTI; ++i )		pPC->DISABLEBLOW ( i );
		for ( int i=0; i<SKILLFACT_SIZE; ++i )		pPC->DISABLESKEFF ( i );
	}

	pPC->ResetAction();


		//	Note : 자신에게 맵이동이 성공함을 알림.
	//
	if( !bInstantMap )
	{
		NetMsgGateOutFB1.emFB = EMCHAR_GATEOUT_OK;
		NetMsgGateOutFB1.sMapID = sToMapID;
		NetMsgGateOutFB1.vPos = vStartPos;
		SENDTOAGENT ( pPC->m_dwClientID, (LPVOID) &NetMsgGateOutFB1 );
	}else{
		NetMsgGateOutFB2.emFB		 = EMCHAR_CREATE_INSTANT_MAP_OK;
		NetMsgGateOutFB2.sInstantMapID = sToMapID;
		SNATIVEID sBaseMapID;
		sBaseMapID.wMainID = sMapID.wMainID;
		sBaseMapID.wSubID  = 0;
		NetMsgGateOutFB2.sBaseMapID  = sBaseMapID;
		NetMsgGateOutFB2.vPos		 = vStartPos;
		SENDTOAGENT ( pPC->m_dwClientID, (LPVOID) &NetMsgGateOutFB2 );

	}

	return TRUE;


ENTRY_FAIL:
	//	Note : 자신에게 맵이동이 실패함을 알림.
	//
	if( !bInstantMap )
	{
		NetMsgGateOutFB1.emFB = EMCHAR_GATEOUT_FAIL;
		SENDTOAGENT ( pPC->m_dwClientID, (LPVOID) &NetMsgGateOutFB1 );
	}else{
		NetMsgGateOutFB2.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL;
		SENDTOAGENT ( pPC->m_dwClientID, (LPVOID) &NetMsgGateOutFB2 );
	}

	return FALSE;

}


// *****************************************************
// Desc: 맵이동 요청 처리
// *****************************************************
BOOL GLGaeaServer::RequestMoveMapPC ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETREQ_GATEOUT *pNetMsg )
{	

	if ( dwGaeaID!=GAEAID_NULL )			return FALSE;
    EntryLand( pNetMsg->dwGaeaID, pNetMsg->dwGateID, FALSE );	


	return FALSE;
}

BOOL GLGaeaServer::RequestCreateInstantMap( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETREQ_CREATE_INSTANT_MAP_FLD *pNetMsg )
{
	

	if ( dwGaeaID!=GAEAID_NULL )			return FALSE;
	PGLCHAR pPC = GetChar ( pNetMsg->dwGaeaID );
	if ( !pPC )								return FALSE;
	
	// 인던 연속 생성 테스트시 사용 코드
	/*{

		SNATIVEID sToMapID = pNetMsg->sInstantMapID;

		DWORD StartTime = ::GetTickCount();


		if( CreateInstantMap( pNetMsg->sBaseMapID, sToMapID, dwGaeaID, pChar->GetPartyID() ) == E_FAIL )
		{
			return FALSE;
		}

		GLLandMan* pInLandMan = NULL;
		pInLandMan = GetByMapID ( sToMapID );
		if ( !pInLandMan )	return FALSE;

		DWORD ElspTime = ::GetTickCount() - StartTime;
		

		HANDLE hProcess			= GetCurrentProcess(); 
		PROCESS_MEMORY_COUNTERS pmc;
		if( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
		{
			TEXTCONSOLEMSG_WRITE( "[INFO]Create Instant Map! Mem %d Time %d msec Size %dbytes. Map ID [%d][%d]", 
				pmc.WorkingSetSize, ElspTime, sizeof(*pInLandMan), pNetMsg->sInstantMapID.wMainID, pNetMsg->sInstantMapID.wSubID );
		}



		return TRUE;
	}*/

	

	GLMSG::SNETREQ_CREATE_INSTANT_MAP_FB NetMsgFB;

	
	//	Memo :	이동할 맵의 ID
	SNATIVEID sToMapID = pNetMsg->sInstantMapID;

	DWORD StartTime = ::GetTickCount();


	if( pNetMsg->bCreate )
	{
		if( CreateInstantMap( pNetMsg->sBaseMapID, sToMapID, pNetMsg->dwGaeaID, pPC->GetPartyID() ) == E_FAIL )
		{
			NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL;
			SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
			return FALSE;
		}
	}

	if( !EntryLand( pNetMsg->dwGaeaID, pNetMsg->dwGateID, TRUE, sToMapID ) ) 
	{
		TEXTCONSOLEMSG_WRITE( "[INFO]Create Instant Map Failed. MapInfo: ID [%d][%d]", 
							  pNetMsg->sInstantMapID.wMainID, pNetMsg->sInstantMapID.wSubID );
		return FALSE;
	}

	
	DWORD ElspTime = ::GetTickCount() - StartTime;

	HANDLE hProcess			= GetCurrentProcess(); 
	PROCESS_MEMORY_COUNTERS pmc;
	if( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
	{

		GLLandMan* pInLandMan = GetByMapID ( sToMapID );
		if ( pInLandMan )
		{
			if( pNetMsg->bCreate )
			{
				TEXTCONSOLEMSG_WRITE( "[INFO]Create Instant Map Time %d msec. MapInfo: ID [%d][%d] Size %dbytes. PartyMap %d HostID %d WorkingMemory %dbytes.", 
					ElspTime, pNetMsg->sInstantMapID.wMainID, pNetMsg->sInstantMapID.wSubID, sizeof(*pInLandMan),
					pInLandMan->IsPartyInstantMap(), pInLandMan->GetInstantMapHostID(), pmc.WorkingSetSize );
			}else{
				TEXTCONSOLEMSG_WRITE( "[INFO]Enter Instant Map Time %d msec. MapInfo: ID [%d][%d] Size %dbytes. PartyMap %d HostID %d WorkingMemory %dbytes.", 
					ElspTime, pNetMsg->sInstantMapID.wMainID, pNetMsg->sInstantMapID.wSubID, sizeof(*pInLandMan),
					pInLandMan->IsPartyInstantMap(), pInLandMan->GetInstantMapHostID(), pmc.WorkingSetSize );
			}
		}
	}



	return FALSE;
}

// *****************************************************
// Desc: 필드서버에서 떠나감을 Agent 에게 알림
// *****************************************************
BOOL GLGaeaServer::RequestFieldSvrOut ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_FIELDSVR_OUT *pNetMsg )
{
	PGLCHAR pPC = GetChar ( dwGaeaID );

	if ( !pPC )								return FALSE;
	if ( pPC->m_dwClientID!=dwClientID )	return FALSE;

	//	Note : 서버를 이동시 대련취소.
	//
	const SCONFTING& sCONFTING = pPC->GETCONFRONTING();
	if ( sCONFTING.IsCONFRONTING() )
	{
		switch ( sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			{
				PGLCHAR pTAR = GetChar ( sCONFTING.dwTAR_ID );
				if ( pTAR )		pTAR->ResetConfront ( EMCONFRONT_END_MOVEMAP );

				pPC->ResetConfront ( EMCONFRONT_END_MOVEMAP );
				pPC->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
			}
			break;
		
		case EMCONFT_PARTY:
			{
				pPC->ResetConfront ( EMCONFRONT_END_NOTWIN );
				pPC->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
			}
			break;
		
		case EMCONFT_GUILD:
			{
				pPC->ResetConfront ( EMCONFRONT_END_NOTWIN );
				pPC->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
			}
			break;
		};
	}

	//	Note : CNetUser::m_dwGaeaID 를 무효화 처리 해야함.
	//
	m_pMsgServer->ResetGaeaID ( dwClientID );

	//	Note : Agent에 처리됨을 알림을 actor에 예약함.	( new 된 객체는 db actor 에서 삭제된다. )
	//
	GLMSG::SNETPC_FIELDSVR_OUT_FB NetMsg;
	CDbActToAgentMsg *pDbActToAgentMsg = new CDbActToAgentMsg;
	pDbActToAgentMsg->SetMsg ( dwClientID, (NET_MSG_GENERIC *)&NetMsg );

	//	Note : 케릭터 삭제 요청.
	//
	ReserveDropOutPC ( pPC->m_dwGaeaID, pDbActToAgentMsg );

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM)// ***Tracing Log print
	if( pPC->m_bTracingUser && pPC->m_pLandMan)
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = pPC->GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, pPC->m_szUID );

		int nPosX(0);
		int nPosY(0);
		pPC->m_pLandMan->GetMapAxisInfo().Convert2MapPos ( pPC->m_vPos.x, pPC->m_vPos.z, nPosX, nPosY );

		CString strTemp;
		strTemp.Format( "FieldServer Out!!, [%s][%s], MAP:mid[%d]sid[%d], StartPos:[%d][%d], Money:[%I64d]",
			pPC->m_szUID, pPC->m_szName, pPC->m_sMapID.wMainID, pPC->m_sMapID.wSubID, nPosX, nPosY, pPC->m_lnMoney );

		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		SENDTOAGENT( pPC->m_dwClientID, &TracingMsg );
	}
#endif

	return TRUE;
}

// *****************************************************
// Desc: 레벨 업시 현재 맵에 머물러 있지 않아야 하는 경우
// *****************************************************
BOOL GLGaeaServer::RequestMustLeaveMap( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_MUST_LEAVE_MAP* pNetMsg )
{
	PGLCHAR pPC = GetChar ( dwGaeaID );

	if ( !pPC )								return FALSE;
	if ( pPC->m_dwClientID!=dwClientID )	return FALSE;


	SNATIVEID sMAPID(false);
	DWORD dwGATEID(UINT_MAX);
	D3DXVECTOR3 vPOS(0.0f,0.0f,0.0f);

	sMAPID	 = pNetMsg->sMAPID;
	dwGATEID = pNetMsg->dwGATEID;

	GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_FB NetMsgFB;

	if ( dwGATEID==UINT_MAX )				
	{
		SENDTOCLIENT ( dwClientID, &NetMsgFB ); // 실패 메시지
		return FALSE;
	}
	if ( sMAPID==SNATIVEID(false) )			
	{
		SENDTOCLIENT ( dwClientID, &NetMsgFB ); // 실패 메시지
		return FALSE;
	}

	const SLEVEL_REQUIRE* pLEVEL_REQUIRE = GetLevelRequire(pPC->m_sMapID.dwID);
	if ( pLEVEL_REQUIRE != NULL )
	{
		EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
		emReqFail = pLEVEL_REQUIRE->ISCOMPLETE ( pPC );
		if( emReqFail == EMREQUIRE_COMPLETE )
		{
			SENDTOCLIENT ( dwClientID, &NetMsgFB );  // 실패 메시지
			return FALSE;
		}
	}else{
		SENDTOCLIENT ( dwClientID, &NetMsgFB );  // 실패 메시지
		return FALSE;
	}

	
	GLMapList::FIELDMAP_ITER iter = m_MapList.find ( sMAPID.dwID );
	if ( iter==m_MapList.end() )
	{
		SENDTOCLIENT( dwClientID, &NetMsgFB );
		return FALSE;
	}


	SMAPNODE *pMapNode = FindMapNode ( sMAPID );
	if ( !pMapNode )					return FALSE;

	// PET
	// 맵이동시 Pet 삭제
	DropOutPET ( pPC->m_dwPetGUID, true, true );
	DropOutSummon ( pPC->m_dwSummonGUID, true );
	SaveVehicle( pPC->m_dwClientID, pPC->m_dwGaeaID, true );

	//	Note : 다른 필드 서버일 경우.
	if ( pMapNode->dwFieldSID!=m_dwFieldSvrID )
	{
		GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_AG NetMsgAg;
		NetMsgAg.sMAPID = sMAPID;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPOS = vPOS;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pPC->m_dwClientID, &NetMsgAg );
	}else{

		SNATIVEID sCurMapID = pPC->m_sMapID;

		BOOL bOK = RequestInvenRecallThisSvr ( pPC, sMAPID, dwGATEID, vPOS );
		if ( !bOK )
		{
			SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
			return FALSE;
		}
		
		// 버프와 상태이상 제거
		if ( sCurMapID != pPC->m_sMapID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		pPC->DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		pPC->DISABLESKEFF ( i );
		}

		pPC->ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		GLMSG::SNETPC_REQ_RECALL_FB NetRecallFB;
		NetRecallFB.emFB = EMREQ_RECALL_FB_OK;
		NetRecallFB.sMAPID = sMAPID;
		NetRecallFB.vPOS = pPC->m_vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pPC->m_dwClientID, &NetRecallFB );

	}

	return TRUE;
}

// *****************************************************
// Desc: 클라이언트 인던 맵 생성 요청 처리
// *****************************************************
BOOL GLGaeaServer::RequestCreateInstantMapReq( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETREQ_CREATE_INSTANT_MAP_REQ *pNetMsg )
{
	GLMSG::SNETREQ_CREATE_INSTANT_MAP_FB  NetMsgFB;
	GLMSG::SNETREQ_CREATE_INSTANT_MAP_REQ NetMsg;

	SNATIVEID sTargetMapID;

	

	PGLCHAR pPC = GetChar ( dwGaeaID );	

	if ( !pPC )								return FALSE;
	if ( pPC->m_dwClientID!=dwClientID )	goto _CreteInstantMapReqError;

	DWORD dwGateID = pNetMsg->dwGateID;
	if ( dwGateID==UINT_MAX )				goto _CreteInstantMapReqError;

	if ( !pPC->m_pLandMan )					goto _CreteInstantMapReqError;

	DxLandGateMan *pLandGateMan = &pPC->m_pLandMan->GetLandGateMan();
	if ( !pLandGateMan )					goto _CreteInstantMapReqError;

	PDXLANDGATE pLandGate = pLandGateMan->FindLandGate ( dwGateID );
	if ( !pLandGate )
	{
		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_GATEID;
		SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
		return FALSE;
	}

	sTargetMapID = pLandGate->GetToMapID();

	const SMAPNODE  *pMapNode = FindMapNode ( sTargetMapID );
	const GLLandMan *pDestLandMan = GetInstantMapByMapID ( sTargetMapID );
	if ( !pMapNode || !pDestLandMan )
	{
		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_TARMAPID;
		SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
		return FALSE;
	}
	if ( !pMapNode->bInstantMap )
	{
		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT;
		SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
		return FALSE;
	}

	// 서버에서 검색된 게이트
	PDXLANDGATE pLandGateInServer = pLandGateMan->DetectGate ( pPC->GetPosition() );

	// 진입조건 검사
	if ( pPC->m_dwUserLvl < USER_GM3 )
	{
		EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
		const SLEVEL_REQUIRE &sRequire = pMapNode->sLEVEL_REQUIRE;
		emReqFail = sRequire.ISCOMPLETE ( pPC ); 
		if ( emReqFail != EMREQUIRE_COMPLETE )
		{
			//	통과 권한이 없음.
			NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_CONDITION;
			SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
			return FALSE;
		}

		// 서버에서 검색한 게이트가 없을 경우 ( 대만 GS 툴에 대처 )
		if ( !pLandGateInServer || !(pLandGateInServer->GetFlags()&DxLandGate::GATE_OUT ) ) 
			goto _CreteInstantMapReqError;
	}


	// 이미 생성된 인던에 소유주 이거나 파티 중에 같은 인던을 생성한 사람이 있을경우의 처리
	{
		VEC_INSTANT_MAPID_ITER iter = m_vecInstantMapId.begin();
		bool  bEnterInstantMap = FALSE;
		for( ; iter != m_vecInstantMapId.end(); ++iter )
		{
			bEnterInstantMap = FALSE;

			PGLLANDMAN pLandMan = GetByMapID( *iter );
			if( !pLandMan ) continue;
			if( pLandMan->GetMapID().wMainID != sTargetMapID.wMainID && !pLandMan->IsInstantMap() ) continue;

			if( !pLandMan->IsExcessiveLimitTime() ) continue;
			if( pLandMan->IsPartyInstantMap() )
			{
				if( pLandMan->GetInstantMapHostID() == pPC->GetPartyID() )
					bEnterInstantMap = TRUE;
			}else if( pPC->GetPartyID() == PARTY_NULL ){
				if( pLandMan->GetInstantMapHostID() == pPC->m_dwGaeaID )
					bEnterInstantMap = TRUE;
			}			

			if( bEnterInstantMap )
			{
				NetMsg.sInstantMapID = *iter;
				NetMsg.dwGaeaID	  = dwGaeaID;
				NetMsg.dwGateID	  = dwGateID;
				NetMsg.bCreate	  = FALSE;
				SENDTOAGENT ( (LPVOID) &NetMsg );
				return TRUE;
			}			
		}
	}


	MEMORYSTATUS memStatus;
	GlobalMemoryStatus( &memStatus );

	// 서버에 생성할수 있는 메모리를 초과했을 경우 생성을 하지 않는다.
	// 유휴 공간으로 10kb를 남겨 둔다.
	if( memStatus.dwAvailPhys - ( 1024 * 10 ) <= sizeof(*pDestLandMan) )
	{
		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_FIELDSVR;
		SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
		return FALSE;
	}


	NetMsg.dwGateID = dwGateID;
	NetMsg.dwGaeaID = dwGaeaID;
	SENDTOAGENT ( (LPVOID) &NetMsg );

	return TRUE;

_CreteInstantMapReqError:
	NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL;
	SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
	return FALSE;
}

// *****************************************************
// Desc: 클라이언트의 게이트 이동 요청 처리
// *****************************************************
BOOL GLGaeaServer::RequestGateOutReq ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETREQ_GATEOUT_REQ* pNetMsg )
{
	GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
	GLMSG::SNETREQ_GATEOUT NetMsg;

	SNATIVEID sMapID;

	PGLCHAR pPC = GetChar ( dwGaeaID );	

	if ( !pPC )								return FALSE;
	if ( pPC->m_dwClientID!=dwClientID )	goto _GateOutError;

	DWORD dwGateID = pNetMsg->dwGateID;
	if ( dwGateID==UINT_MAX )				goto _GateOutError;

	if ( !pPC->m_pLandMan )					goto _GateOutError;

	DxLandGateMan *pLandGateMan = &pPC->m_pLandMan->GetLandGateMan();
	if ( !pLandGateMan )					goto _GateOutError;

	PDXLANDGATE pLandGate = pLandGateMan->FindLandGate ( dwGateID );
	if ( !pLandGate )						goto _GateOutError;

	sMapID = pLandGate->GetToMapID();

	const SMAPNODE *pMapNode = FindMapNode ( sMapID );
	if ( !pMapNode )						goto _GateOutError;

	// 서버에서 검색된 게이트
	PDXLANDGATE pLandGateInServer = pLandGateMan->DetectGate ( pPC->GetPosition() );

	// 진입조건 검사
	if ( pPC->m_dwUserLvl < USER_GM3 )
	{
		EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
		const SLEVEL_REQUIRE &sRequire = pMapNode->sLEVEL_REQUIRE;
		emReqFail = sRequire.ISCOMPLETE ( pPC ); 
		if ( emReqFail != EMREQUIRE_COMPLETE )
		{
			//	통과 권한이 없음.
			NetMsgFB.emFB = EMCHAR_GATEOUT_CONDITION;
			SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
			return FALSE;
		}

		// 서버에서 검색한 게이트가 없을 경우 ( 대만 GS 툴에 대처 )
		if ( !pLandGateInServer || !(pLandGateInServer->GetFlags()&DxLandGate::GATE_OUT ) ) 
			goto _GateOutError;
	}

	// 선도전장이면
	GLLandMan* pLandMan = GetByMapID ( sMapID );
	if ( pLandMan )
	{
		if ( pMapNode->bBattleZone )
		{
			if ( GLCONST_CHAR::bPKLESS ) 
			{
				// 선도전 맵은 절대 못 들어간다...
				NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
				SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
				return FALSE;
			}			

			GLGuidance *pGuid = GLGuidanceFieldMan::GetInstance().Find ( pLandMan->m_dwClubMapID );
			if ( pGuid )
			{

				// 선도전 일때 길트배틀 중이거나 종료시간 미달이면 출입금지
				GLCLUB* pClub = m_cClubMan.GetClub ( pPC->m_dwGuild );
				if ( pClub && pGuid->m_bBattle && 
					( pClub->GetAllBattleNum() > 0 ||
					 !pClub->IsGuidBattle() ) )
				{
					NetMsgFB.emFB = EMCHAR_GATEOUT_CLUBBATTLE;
					SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
					return FALSE;
				}


				if ( !pGuid->m_bBattle && !pGuid->IsGuidClub(pPC->m_dwGuild) )
				{
					GLCLUB* pGuidanceClub = m_cClubMan.GetClub ( pGuid->m_dwGuidanceClub );
					if ( pGuidanceClub )
					{
						// 선도클럽의 동맹이 아니면
						if ( !pGuidanceClub->IsAllianceGuild ( pPC->m_dwGuild ) )
						{
							NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
							SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
							return FALSE;
						}
					}
				}
			}
		}

		if ( pMapNode->bCDMZone )
		{
			if ( GLCONST_CHAR::bPKLESS ) 
			{
				// 선도전 맵은 절대 못 들어간다...
				NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
				SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
				return FALSE;
			}			

			GLClubDeathMatch *pCDM = GLClubDeathMatchFieldMan::GetInstance().Find ( pLandMan->m_dwClubMapID );
			if ( pCDM )
			{
				GLCLUB* pClub = m_cClubMan.GetClub ( pPC->m_dwGuild );
				if ( !pCDM->IsEnterMap( pPC->m_dwGuild ) || !pClub )
				{
					NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
					SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
					return FALSE;
				}	
								
				if ( pClub )
				{
					// 클럽배틀 진행중일때는 못들어간다.
                    if ( pClub->GetAllBattleNum() > 0 )
					{
						NetMsgFB.emFB = EMCHAR_GATEOUT_CLUBBATTLE2;
						SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
						return FALSE;
					}

					if ( !pClub->IsMemberFlgCDM(pPC->m_dwCharID) )
					{
						NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
						SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
						return FALSE;
					}
				}
			}
		}
	}
	
	NetMsg.dwGateID = dwGateID;
	NetMsg.dwGaeaID = dwGaeaID;
	SENDTOAGENT ( (LPVOID) &NetMsg );

	return TRUE;

_GateOutError:
	NetMsgFB.emFB = EMCHAR_GATEOUT_FAIL;
	SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );
	return FALSE;

}

BOOL GLGaeaServer::RequestReBirthOut ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REBIRTH_OUT *pNetMsg )
{
	PGLCHAR pPC = GetChar ( dwGaeaID );

	if ( !pPC )								return FALSE;
	if ( pPC->m_dwClientID!=dwClientID )	return FALSE;

	if ( !pPC->IsSTATE(EM_ACT_DIE) )
	{
		GetConsoleMsg()->Write ( LOG_TEXT_CONSOLE, 
			"State : %d , RequestReBirthOut() : 부활메시지 처리중 사망 플래그가 없어서 요청을 무시하였습니다.", pPC->GetSTATE() );
	}

	//	부활시 채력 회복.
	//
	pPC->m_fGenAge = 0.0f;
	pPC->INIT_RECOVER();

	//	 Note : 현제 위치를 마지막 귀환 위치로 저장.
	//
	//
	pPC->SaveLastCall ();

	//	Note : ## 사망한 상태로 부활시 경험치 감소는 GLGaeaServer::DropOutPC() 에서 처리. ##

	//	Note : CNetUser::m_dwGaeaID 를 무효화 처리 해야함.
	//
	m_pMsgServer->ResetGaeaID ( dwClientID );

	//	Note : Agent으로 처리 완료 알림, actor에 예약함.	( new 된 객체는 db actor 에서 삭제된다. )
	//
	GLMSG::SNETPC_REBIRTH_OUT_FB NetMsg;
	CDbActToAgentMsg *pDbActToAgentMsg = new CDbActToAgentMsg;
	pDbActToAgentMsg->SetMsg ( dwClientID, (NET_MSG_GENERIC *)&NetMsg );
	
	//	케릭터 삭제 요청.
	ReserveDropOutPC ( pPC->m_dwGaeaID, pDbActToAgentMsg );

	return TRUE;
}

BOOL GLGaeaServer::RequestLandIn ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETREQ_LANDIN *pNetMsg )
{
	PGLCHAR pPC = GetChar ( dwGaeaID );
	if ( !pPC )								return FALSE;

	if ( !pPC->IsSTATE(EM_GETVA_AFTER) )
	{
		//	Note : 캐릭터 주변의 정보를 인지.
		//
		pPC->GetViewAround ();
	}
	else
	{
		CONSOLEMSG_WRITE ( "GLGaeaServer::RequestLandIn() Call this function after receive view around message already [%s][%d]", pPC->m_szName, pPC->m_dwGaeaID );
		//DEBUGMSG_WRITE( _T("GLGaeaServer::RequestLandIn() else [%s][%u]"), pPC->m_szName, pPC->m_dwGaeaID );
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestTrade ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE *pNetMsg )
{
	PGLCHAR pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )								return FALSE;

	PGLCHAR pTarChar = GetChar ( pNetMsg->dwTargetID );
	if ( !pTarChar )							return FALSE;

	//	Note : 이미 트래이드 중이라면 취소.
	if ( pMyChar->m_sTrade.Valid() )			return FALSE;
	if ( pTarChar->m_sTrade.Valid() )			return FALSE;

	//	Note : 상대편에게 트래이드 시도 승낙 질의.
	//
	GLMSG::SNET_TRADE_TAR NetMsg;
	NetMsg.dwMasterID = dwGaeaID;
	SENDTOAGENT ( pTarChar->m_dwClientID, &NetMsg );

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	if( pMyChar->m_bTracingUser )
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = pMyChar->GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, pMyChar->m_szUID );

		CString strTemp;
		strTemp.Format( "Trade Request, [%s][%s], Target Char:[%s][%s]", 
				 pMyChar->m_szUID, pMyChar->m_szName, pMyChar->m_szUID, pMyChar->m_szName );
		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		SENDTOAGENT( pMyChar->m_dwClientID, &TracingMsg );
	}
#endif

	return TRUE;
}

BOOL GLGaeaServer::RequestTradeTarAns ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_TAR_ANS *pNetMsg )
{
	PGLCHAR pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )								return FALSE;

	PGLCHAR pMasterChar = GetChar ( pNetMsg->dwMasterID );
	if ( !pMasterChar )							return FALSE;

	//	Note : 이미 트래이드 중이라면 취소.
	if ( pMyChar->m_sTrade.Valid() )			return FALSE;
	if ( pMasterChar->m_sTrade.Valid() )		return FALSE;

	if ( pNetMsg->emAns == EMTRADE_CANCEL )
	{
		//	Note : 트래이드 시도 거부 MSG
		//
		GLMSG::SNET_TRADE_FB NetMsgFB;
		NetMsgFB.dwTargetID = dwGaeaID;
		NetMsgFB.emAns = EMTRADE_CANCEL;

		SENDTOAGENT ( pMasterChar->m_dwClientID, &NetMsgFB );

		return FALSE;
	}

	//	Note : 트래이드 시작.
	//
	pMyChar->m_sTrade.Reset();
	pMyChar->m_sTrade.SetTarget ( pMasterChar->m_dwGaeaID );

	pMasterChar->m_sTrade.Reset();
	pMasterChar->m_sTrade.SetTarget ( pMyChar->m_dwGaeaID );

	//	Note : 트래이드 시작 MSG
	//
	{
		GLMSG::SNET_TRADE_FB NetMsgFB;
		NetMsgFB.dwTargetID = dwGaeaID;
		NetMsgFB.emAns = EMTRADE_OK;

		SENDTOAGENT ( pMasterChar->m_dwClientID, &NetMsgFB );
	}

	{
		GLMSG::SNET_TRADE_FB NetMsgFB;
		NetMsgFB.dwTargetID = pNetMsg->dwMasterID;
		NetMsgFB.emAns = EMTRADE_OK;

		SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsgFB );
	}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM)// ***Tracing Log print
	if( pMyChar->m_bTracingUser )
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = pMyChar->GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, pMyChar->m_szUID );

		CString strTemp;
		strTemp.Format( "Trade Answer OK, [%s][%s], Master Char: [%s][%s]", 
				 pMyChar->m_szUID, pMyChar->m_szName, pMasterChar->m_szUID, pMasterChar->m_szName );

		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		SENDTOAGENT( pMyChar->m_dwClientID, &TracingMsg );
	}
#endif

	return TRUE;
}

BOOL GLGaeaServer::RequestTradeMoney ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_MONEY *pNetMsg )
{
	PGLCHAR pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )									return FALSE;
	if ( !pMyChar->m_sTrade.Valid() )				return FALSE;

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	PGLCHAR pTarChar = GetChar ( dwTargetID );

	if ( !pTarChar || pTarChar->m_sTrade.GetTarget()!=dwGaeaID )
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset ();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsg );

		return FALSE;
	}


	//	금액변화.
	if ( pNetMsg->lnMoney > pMyChar->m_lnMoney || pNetMsg->lnMoney < 0 )	return FALSE;

#if defined ( TW_PARAM ) || defined ( HK_PARAM ) 
	if ( pTarChar->m_wLevel == 1 )
	{
		if( (pTarChar->m_lnMoney + pNetMsg->lnMoney) > 50000 )
		{
			GetConsoleMsg()->Write ( LOG_TEXT_CONSOLE, 
									"Target(Level1) too many money!! TarMoney %I64d TarID %s TarUserNum %d MyUserNum %I64d MyID %s MyUserNum %d TradeMoney %I64d",
									pTarChar->m_lnMoney, pTarChar->m_szName, pTarChar->GetUserID(), 
									pMyChar->m_lnMoney, pMyChar->m_szName, pMyChar->GetUserID(), pNetMsg->lnMoney );
			return FALSE;
		}
	}
#endif


	pMyChar->m_sTrade.SetMoney ( pNetMsg->lnMoney );
	
	//	승인 블럭.
	pMyChar->m_sTrade.BlockAgree ();
	pTarChar->m_sTrade.BlockAgree ();

	//	Note : 금액 변화 알림. MSG
	//
	GLMSG::SNET_TRADE_MONEY_TAR NetMsgMoneyTar;
	NetMsgMoneyTar.dwGaeaID = dwGaeaID;
	NetMsgMoneyTar.lnMoney = pNetMsg->lnMoney;

	SENDTOCLIENT ( pMyChar->m_dwClientID, &NetMsgMoneyTar );
	SENDTOCLIENT ( pTarChar->m_dwClientID, &NetMsgMoneyTar );

	return TRUE;
}

BOOL GLGaeaServer::RequestTradeItemResist ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_ITEM_REGIST *pNetMsg )
{
	PGLCHAR pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )									return FALSE;
	if ( !pMyChar->m_sTrade.Valid() )				return FALSE;

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	PGLCHAR pTarChar = GetChar ( dwTargetID );

	if ( !pTarChar || pTarChar->m_sTrade.GetTarget()!=dwGaeaID )
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset ();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsg );

		return FALSE;
	}

	SINVENITEM *pInvenItem = pMyChar->m_cInventory.GetItem ( pNetMsg->wInvenX, pNetMsg->wInvenY );
	if ( !pInvenItem )								return FALSE;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pITEM )		return false;
	
	const SITEMCUSTOM& sITEMCUSTOM = pMyChar->GET_HOLD_ITEM();

	// 팻카드이면서 팻이 활성화 되어 있으면 거래할 수 없다.
	if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( pMyChar->m_dwPetGUID );
		if ( pMyPet && pMyPet->IsValid () && sITEMCUSTOM.dwPetID == pMyPet->m_dwPetID ) return S_FALSE;
	}

	// 탈것이면 거래할수 없다.
	if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && pInvenItem->sItemCustom.dwVehicleID != 0 )
	{
		return S_FALSE;
	}

	//	거래옵션
	if ( !pITEM->sBasicOp.IsEXCHANGE() )					return FALSE;
	
	// GMCharEdit 로 넣은 아이템은 버리기 불가능하다.
	if ( pInvenItem->sItemCustom.IsGM_GENITEM() )			return FALSE;

	BOOL bOk = pMyChar->m_sTrade.InsertItem ( pNetMsg->wPosX, pNetMsg->wPosY, pInvenItem );
	if ( !bOk )										return FALSE;

	SINVENITEM* pInven = pMyChar->m_sTrade.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInven )									return FALSE;

	// 팻 생성여부 확인
	if ( pMyChar->m_dwCharID <= 0 )
	{
		GetConsoleMsg()->Write ( LOG_TEXT_CONSOLE, "Invalid Trade CharacterID PetID %d CharID %d dwClientID %d", 
								pInvenItem->sItemCustom.dwPetID, pMyChar->m_dwCharID, pTarChar->m_dwClientID );
		return FALSE;
	}


	// 팻카드이면 팻카드 정보를 상대방에게 알려준다.
	if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && pInvenItem->sItemCustom.dwPetID != 0 )
	{
		PGLPET pPetInfo = new GLPET ();

		// DB작업 실패시 혹은 생성후 메모리 해제해줘라
		CGetPet* pDbAction = new CGetPet ( pPetInfo, pInvenItem->sItemCustom.dwPetID, pTarChar->m_dwClientID, pMyChar->m_dwCharID, 
										   pNetMsg->wInvenX, pNetMsg->wInvenY, false, false, true );
		m_pDBMan->AddJob ( pDbAction );		
	}

	//	승인 블럭.
	pMyChar->m_sTrade.BlockAgree ();
	pTarChar->m_sTrade.BlockAgree ();

	//	Note : 아이템 변화 알림. MSG.
	//
	GLMSG::SNET_TRADE_ITEM_REGIST_TAR NetRegistTar;
	NetRegistTar.dwGaeaID = dwGaeaID;
	NetRegistTar.sInsert = *pInven;

	SENDTOAGENT ( pMyChar->m_dwClientID, &NetRegistTar );
	SENDTOAGENT ( pTarChar->m_dwClientID, &NetRegistTar );

	return TRUE;
}

BOOL GLGaeaServer::RequestTradeItemReMove ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_ITEM_REMOVE *pNetMsg )
{
	PGLCHAR pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )									return FALSE;
	if ( !pMyChar->m_sTrade.Valid() )				return FALSE;

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	PGLCHAR pTarChar = GetChar ( dwTargetID );

	if ( !pTarChar || pTarChar->m_sTrade.GetTarget()!=dwGaeaID )
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset ();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsg );

		return FALSE;
	}

	SINVENITEM* pInven = pMyChar->m_sTrade.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInven )									return FALSE;

	//	Note : 거래 목록에서 아이템 삭제.
	pMyChar->m_sTrade.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	//	승인 블럭.
	pMyChar->m_sTrade.BlockAgree ();
	pTarChar->m_sTrade.BlockAgree ();

	//	Note : 아이템 변화 알림. MSG.
	//
	GLMSG::SNET_TRADE_ITEM_REMOVE_TAR NetReMoveTar;
	NetReMoveTar.dwGaeaID = dwGaeaID;
	NetReMoveTar.wPosX = pNetMsg->wPosX;
	NetReMoveTar.wPosY = pNetMsg->wPosY;

	SENDTOAGENT ( pMyChar->m_dwClientID, &NetReMoveTar );
	SENDTOAGENT ( pTarChar->m_dwClientID, &NetReMoveTar );

	return TRUE;
}

BOOL GLGaeaServer::RequestTradeAgree ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_AGREE *pNetMsg )
{
	PGLCHAR pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )									return FALSE;
	if ( !pMyChar->m_sTrade.Valid() )				return FALSE;

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	PGLCHAR pTarChar = GetChar ( dwTargetID );

	if ( !pTarChar || pTarChar->m_sTrade.GetTarget()!=dwGaeaID )
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset ();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsg );

		return FALSE;
	}

	//	승인 시도.
	BOOL bOk = pMyChar->m_sTrade.SetAgree ();

	if ( !bOk )
	{
		//	승인 여부 결과 MSG.
		GLMSG::SNET_TRADE_AGREE_TAR NetMsgAgreeTar;
		NetMsgAgreeTar.dwGaeaID = dwGaeaID;
		NetMsgAgreeTar.emFB = EMTRADE_AGREE_TIME;

		SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsgAgreeTar );

		return FALSE;
	}

	if ( pMyChar->m_sTrade.GetAgree() && pTarChar->m_sTrade.GetAgree() )
	{
		
		EMTRADE_CANCEL_TAR emCancelFB = EMTRADE_CANCEL_NORMAL;

		do
		{
			//	Note : 거래 목록이 유효한지 검사.
			//
			if ( !pMyChar->IsVaildTradeData() || !pTarChar->IsVaildTradeData() )
			{
				break;
			}

			//	Note : 물품 교환 공간이 인밴에 충분한지 검사.
			//
			BOOL bOk;
			bOk = pMyChar->IsVaildTradeInvenSpace ( pTarChar->m_sTrade.GetTradeBox() );
			if ( !bOk )
			{
				emCancelFB = EMTRADE_CANCEL_MYINVEN;
				break;
			}

			bOk = pTarChar->IsVaildTradeInvenSpace ( pMyChar->m_sTrade.GetTradeBox() );
			if ( !bOk )
			{
				emCancelFB = EMTRADE_CANCEL_TARINVEN;
				break;
			}

			//	Note : 거래 수행.
			//
			bOk = pMyChar->DoTrade ( pTarChar->m_sTrade, pTarChar->m_dwCharID );
			if ( !bOk ) break; // 돈 금액이 마이너스가 되는 경우 (대만 해킹)
			//GASSERT ( bOk && "DoTrade()에서는 FALSE가 일어나서는 안된다." );

			bOk = pTarChar->DoTrade ( pMyChar->m_sTrade, pMyChar->m_dwCharID );
			if ( !bOk ) break; // 돈 금액이 마이너스가 되는 경우 (대만 해킹)
			//GASSERT ( bOk && "DoTrade()에서는 FALSE가 일어나서는 안된다." );

			//	Note : 거래 정보 리셋.
			//
			pMyChar->m_sTrade.Reset();
			pTarChar->m_sTrade.Reset();

			//	Note : 거래 성공.
			//
			GLMSG::SNET_TRADE_COMPLETE_TAR NetMsgComplete;
			SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsgComplete );
			SENDTOAGENT ( pTarChar->m_dwClientID, &NetMsgComplete );

			return TRUE;
		}
		while(0);

		//	Note : 거래 정보 리셋.
		//
		pMyChar->m_sTrade.Reset();
		pTarChar->m_sTrade.Reset();

		//	Note : 거래 취소됨을 알림. ( dwGaeaID 에게. )
		//
		{
			GLMSG::SNET_TRADE_CANCEL_TAR NetMsgCancelTar;
			NetMsgCancelTar.emCancel = emCancelFB;
			SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsgCancelTar );
		}

		//	Note : 거래 취소됨을 알림. ( dwTargetID 에게. )
		//
		{
			if ( emCancelFB==EMTRADE_CANCEL_MYINVEN || emCancelFB==EMTRADE_CANCEL_TARINVEN )
			{
				emCancelFB = ( emCancelFB==EMTRADE_CANCEL_MYINVEN ? EMTRADE_CANCEL_TARINVEN : EMTRADE_CANCEL_MYINVEN );
			}

			GLMSG::SNET_TRADE_CANCEL_TAR NetMsgCancelTar;
			NetMsgCancelTar.emCancel = emCancelFB;
			SENDTOAGENT ( pTarChar->m_dwClientID, &NetMsgCancelTar );
		}
	}
	else
	{
		//	승인 여부 결과 MSG.
		GLMSG::SNET_TRADE_AGREE_TAR NetMsgAgreeTar;
		NetMsgAgreeTar.dwGaeaID = dwGaeaID;
		NetMsgAgreeTar.emFB = EMTRADE_AGREE_OK;

		SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsgAgreeTar );
		SENDTOAGENT ( pTarChar->m_dwClientID, &NetMsgAgreeTar );
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestTradeCancel ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_CANCEL *pNetMsg )
{
	PGLCHAR pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )									return FALSE;
	if ( !pMyChar->m_sTrade.Valid() )				return FALSE;

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	PGLCHAR pTarChar = GetChar ( dwTargetID );

	if ( !pTarChar || pTarChar->m_sTrade.GetTarget()!=dwGaeaID )
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset ();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsg );

		return FALSE;
	}

	//	거래 취소.
	pMyChar->m_sTrade.Reset();
	pTarChar->m_sTrade.Reset();

	{
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.emCancel = EMTRADE_CANCEL_NORMAL;
		SENDTOAGENT ( pMyChar->m_dwClientID, &NetMsg );
	}

	{
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.emCancel = EMTRADE_CANCEL_OTHER;
		SENDTOAGENT ( pTarChar->m_dwClientID, &NetMsg );
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestFieldSvrCharChk ( DWORD dwClientID, GLMSG::SNETPC_FIELDSVR_CHARCHK *pNetMsg  )
{
	bool bExist(false);

	//	Note : 같은 이름의 캐릭터가 이미 접속되어 있는지 검사합니다.
	{
		GLCHAR_MAP_ITER name_iter = m_PCNameMap.find(pNetMsg->szName);
		if ( name_iter != m_PCNameMap.end() )
		{
			PGLCHAR pChar = (*name_iter).second;

			bExist = true;

			//	종전 접속자에게 DropOut 메시지를 보냅니다.
			GLMSG::SNET_DROP_OUT_FORCED NetMsgForced;
			NetMsgForced.emForced = EMDROPOUT_ERROR;
			SENDTOAGENT ( pChar->m_dwClientID, &NetMsgForced );

			//	Note : 현제 위치 저장.
			//
			pChar->SavePosition ();

			//	Note : 사망한 상태로 제거시 경험치 감소.
			//
			if ( pChar->IsSTATE(EM_ACT_DIE) )
			{
				//	Note : 부활시 경험치 감소.
				pChar->ReBirthDecExp ();
			}

			if ( m_pDBMan )
			{
				//	Note : 케릭터 저장 요청.
				SetSaveDBUserID(pChar->GetUserID());

				//	Note : 클럽 창고 저장.
				if ( pChar->m_dwGuild!=CLUB_NULL )
				{
					GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( pChar->m_dwGuild );
					if ( pCLUB && pChar->m_dwCharID==pCLUB->m_dwMasterID )
					{
						pCLUB->SAVESTORAGE2DB ();
						pCLUB->RESET_STORAGE ();
					}
				}

				//	Note : 케릭터 저장 요청.
				CDbActSaveChar *pSaveDB = new CDbActSaveChar;
				pSaveDB->SetInfo ( pChar->m_dwClientID, pChar->m_dwGaeaID, pChar );
				m_pDBMan->AddJob ( pSaveDB );
			}

			//	Note : 케릭터가 지워진다.
			//
			DropOutPC(pChar->m_dwGaeaID);
		}
	}

	//	Note : 같은 Gaea id 가 접속중인지 검사.
	//
	if ( GetChar(pNetMsg->dwGaeaID) )
	{
		PGLCHAR pChar = GetChar(pNetMsg->dwGaeaID);

		if ( pChar )
		{
			bExist = true;

			//	종전 접속자에게 DropOut 메시지를 보냅니다.
			GLMSG::SNET_DROP_OUT_FORCED NetMsgForced;
			NetMsgForced.emForced = EMDROPOUT_ERROR;
			SENDTOAGENT ( pChar->m_dwClientID, &NetMsgForced );

			//	Note : 현제 위치 저장.
			//
			pChar->SavePosition ();

			//	Note : 사망한 상태로 제거시 경험치 감소.
			//
			if ( pChar->IsSTATE(EM_ACT_DIE) )
			{
				//	Note : 부활시 경험치 감소.
				pChar->ReBirthDecExp ();
			}

			if ( m_pDBMan )
			{
				//	Note : 케릭터 저장 요청.
				//
				SetSaveDBUserID(pChar->GetUserID());

				//	Note : 클럽 창고 저장.
				if ( pChar->m_dwGuild!=CLUB_NULL )
				{
					GLCLUB *pCLUB = GLGaeaServer::GetInstance().GetClubMan().GetClub ( pChar->m_dwGuild );
					if ( pCLUB && pChar->m_dwCharID==pCLUB->m_dwMasterID )
					{
						pCLUB->SAVESTORAGE2DB ();
						pCLUB->RESET_STORAGE ();
					}
				}

				//	Note : 케릭터 정보 저장.
				CDbActSaveChar *pSaveDB = new CDbActSaveChar;
				pSaveDB->SetInfo ( pChar->m_dwClientID, pChar->m_dwGaeaID, pChar );
				m_pDBMan->AddJob ( pSaveDB );
			}

			//	Note : 케릭터가 지워진다.
			//
			DropOutPC(pChar->m_dwGaeaID);
		}
	}

	//	Note : 혹시 user id 가 db 작업중인지 검사.
	if ( FindSaveDBUserID ( pNetMsg->dwUserID ) )
	{
		bExist = true;
	}

	//	Note : 에이젼트에 점검 FB.
	//
	GLMSG::SNETPC_FIELDSVR_CHARCHK_FB NetMsgFb;
	NetMsgFb.bExist = bExist;
	NetMsgFb.nChannel = m_nServerChannel;
	NetMsgFb.dwFIELDID = m_dwFieldSvrID;
	NetMsgFb.dwGaeaID = pNetMsg->dwGaeaID;
	StringCchCopy ( NetMsgFb.szName, CHR_ID_LENGTH+1, pNetMsg->szName );

	if ( bExist )
	{
		if ( m_pDBMan )
		{
			//	Note : DB 저장후에 메시지 처리하기 위해서 db action에 등록하여 메시지 전송.
			CDbActToAgentMsg *pToAgent = new CDbActToAgentMsg;
			pToAgent->SetMsg ( dwClientID, (NET_MSG_GENERIC*)&NetMsgFb );
			m_pDBMan->AddJob ( pToAgent );
		}
	}
	else
	{
		//	Note : DB 작업이 없을 경우에는 즉시 전송.
		SENDTOAGENT ( dwClientID, &NetMsgFb );
	}

	return TRUE;
}

BOOL GLGaeaServer::ServerFieldInfoReset ( GLMSG::SNET_FIELDINFO_RESET* pNetMsg )
{
	m_cPartyFieldMan.DelPartyAll();

	return TRUE;
}

BOOL GLGaeaServer::ServerSchoolFreePk ( GLMSG::SNETPC_SCHOOLFREEPK_FLD *pNetMsg )
{
	if( GLCONST_CHAR::bPKLESS ) return FALSE;

	GLSchoolFreePK::GetInstance().SET ( pNetMsg->bSCHOOL_FREEPK );
	
	GLMSG::SNETPC_SERVER_INFO NetMsgServerInfo;
	NetMsgServerInfo.bSCHOOL_FREEPK = GLSchoolFreePK::GetInstance().IsON();

	PGLCHAR pCHAR = NULL;
	for ( DWORD i=0; i<m_dwMaxClient; ++i )
	{
		pCHAR = GetChar(i);
		if ( !pCHAR )	continue;

		SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgServerInfo );
	}

	return TRUE;
}

BOOL GLGaeaServer::ServerCtrlPeriod ( GLMSG::SNET_PERIOD *pNetMsg )
{
	GLPeriod::GetInstance().SetPeriod ( pNetMsg->sPRERIODTIME );

	return TRUE;
}

BOOL GLGaeaServer::ServerCtrlWeather ( GLMSG::SNETSERVER_CTRL_WEATHER *pNetMsg )
{
	GLPeriod::GetInstance().SetActiveWeather ( pNetMsg->bActive );
	GLPeriod::GetInstance().SetWeather ( pNetMsg->dwWeather );

	return TRUE;
}

BOOL GLGaeaServer::ServerCtrlWeather2 ( GLMSG::SNETSERVER_CTRL_WEATHER2 *pNetMsg )
{
	SONEMAPWEATHER oneMapWeather;
	oneMapWeather.dwWeather   = pNetMsg->dwWeather;
	oneMapWeather.dwApplyTime = pNetMsg->dwApplyTime;
	oneMapWeather.map_mID     = pNetMsg->map_mID;
	oneMapWeather.map_sID     = pNetMsg->map_sID;

	if( pNetMsg->dwWeather == NULL )
	{
		GLPeriod::GetInstance().SetActiveWeather ( FALSE );
	}else{
		GLPeriod::GetInstance().SetActiveWeather ( TRUE );
	}
	GLPeriod::GetInstance().SetOneMapActiveWeather( oneMapWeather );

	return TRUE;
}

BOOL GLGaeaServer::ServerCtrlTime ( GLMSG::SNETSERVER_CTRL_TIME *pNetMsg )
{
	GLPeriod::GetInstance().SetHour ( pNetMsg->dwTime );
	
	for ( DWORD i=0; i<m_dwMaxClient; ++i )
	{
		PGLCHAR pChar = GetChar(i);
		if ( !pChar )	continue;

		pChar->MsgSendPeriod ();
	}

	return TRUE;
}

BOOL GLGaeaServer::ServerCtrlMonth ( GLMSG::SNETSERVER_CTRL_MONTH *pNetMsg )
{

	return TRUE;
}

BOOL GLGaeaServer::ServerCtrlGenItemHold ( GLMSG::SNETSERVER_CTRL_GENITEMHOLD *pNetMsg )
{
	m_bGenItemHold = pNetMsg->bHold;

	return TRUE;
}

BOOL GLGaeaServer::ServerCtrlPlayerKillingMode ( GLMSG::SNET_SERVER_PLAYERKILLING_MODE *pNetMsg )
{
	GLCONST_CHAR::bPK_MODE = pNetMsg->bMODE;

	return TRUE;
}

BOOL GLGaeaServer::VietnamAllInitTime ( GLMSG::SNETPC_VIETNAM_ALLINITTIME *pNetMsg )
{

	GLMSG::SNETPC_VIETNAM_ALLINITTIME netMsg;

	GetConsoleMsg()->Write ( LOG_TEXT_CONSOLE, "-=-=-=-= Init Vietnam Time =-=-=-=-" );

	PGLCHAR pCHAR = NULL;
	CLIENTMAP_ITER iter = m_mapCHARID.begin();

	for( ; iter != m_mapCHARID.end(); ++iter )
	{
		pCHAR = GetChar((*iter).second);
		if ( !pCHAR )	continue;

		pCHAR->ReceiveVietnam( pNetMsg->initTime );
	}
	return TRUE;
}

BOOL GLGaeaServer::MsgVietTimeReset( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_VIETNAM_TIME_REQ_FB *pNetMsg ) 
{
#if defined(VN_PARAM) //vietnamtest%%% && vietnamtest2
	
	PGLCHAR pChar = GetChar(dwGaeaID);
	if( !pChar )	return FALSE;

	pChar->m_dwVietnamGainType = GAINTYPE_MAX;
	pChar->m_sVietnamSystem.Init();
	pChar->m_sVietnamSystem.loginTime = pNetMsg->loginTime;
	pChar->m_sVietnamSystem.gameTime  = 0;

	GLMSG::SNETPC_VIETNAM_ALLINITTIME NetMsg;
	NetMsg.initTime = pNetMsg->loginTime;
	SENDTOCLIENT(dwClientID,&NetMsg);

#endif

	return TRUE;
}

BOOL GLGaeaServer::ServerCtrlCharDropOutForced ( GLMSG::SNET_DROP_OUT_FORCED *pNetMsg )
{
	PGLCHAR pPC = GetCharID ( pNetMsg->dwCharID );
	if ( !pPC )							return FALSE;
	
	ReserveDropOutPC ( pPC->m_dwGaeaID );

	return TRUE;
}

BOOL GLGaeaServer::CyberCafeClassUpdate( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_CYBERCAFECLASS_UPDATE *pNetMsg )
{
	PGLCHAR pCHAR = GetChar ( dwGaeaID );
	if( !pCHAR ) return FALSE;
	pCHAR->m_dwThaiCCafeClass = pNetMsg->dwCyberCafeClass;
	pCHAR->m_nMyCCafeClass	  = pNetMsg->dwCyberCafeClass;

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMove2Gate ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOVE2GATE_FLD *pNetMsg )
{
	PGLCHAR pPC = GetChar ( pNetMsg->dwGaeaID );
	if ( !pPC )							return FALSE;

	if ( m_dwAgentSlot != dwClientID )	return FALSE;
	

	GLLandMan* pInLandMan = NULL;
	DxLandGateMan* pInGateMan = NULL;
	PDXLANDGATE pInGate = NULL;
	D3DXVECTOR3 vStartPos(0,0,0);

	DxLandGateMan* pGateMan = NULL;
	PDXLANDGATE pGate = NULL;
	
	pGateMan = &pPC->m_pLandMan->GetLandGateMan();
	pGate = pGateMan->FindLandGate ( pNetMsg->dwGATE );
	if ( !pGate )						return FALSE;

	vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );

	//	Note : 새로운 맵 위치로 내비게이션 초기화.
	//
	pPC->SetPosition ( vStartPos );

	//	Note : 변경된 결과 반환.
	//
	GLMSG::SNET_GM_MOVE2GATE_FB NetMsgFB;
	NetMsgFB.vPOS = pPC->m_vPos;
	SENDTOCLIENT ( pPC->m_dwClientID, &NetMsgFB );

	GLMSG::SNETPC_JUMP_POS_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = pPC->m_dwGaeaID;
	NetMsgBrd.vPOS = pPC->m_vPos;

	pPC->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMove2MapPos ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOVE2MAPPOS_FLD *pNetMsg )
{
	PGLCHAR pChar = GetChar ( pNetMsg->dwGaeaID );
	if ( !pChar )							return FALSE;

	if ( m_dwAgentSlot != dwClientID )	return FALSE;

	SNATIVEID nidMAP = pNetMsg->nidMAP;

	if ( nidMAP==SNATIVEID(false) )		return FALSE;

	GLMapList::FIELDMAP_ITER iter = m_MapList.find ( nidMAP.dwID );
	if ( iter==m_MapList.end() )		return FALSE;

	const SMAPNODE *pMapNode = &(*iter).second;

	GLMapAxisInfo sMapAxisInfo;
	sMapAxisInfo.LoadFile ( pMapNode->strFile.c_str() );

	D3DXVECTOR3 vPOS(0,0,0); 
	sMapAxisInfo.MapPos2MiniPos ( pNetMsg->dwPOSX, pNetMsg->dwPOSY, vPOS.x, vPOS.z );

	//	Note : 다른 필드 서버일 경우.
	if ( pMapNode->dwFieldSID!=m_dwFieldSvrID )
	{
		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.sMAPID = nidMAP;
		NetMsgAg.dwGATEID = UINT_MAX;
		NetMsgAg.vPOS = vPOS;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgAg );
	}
	//	Note : 같은 필드 서버일 경우.
	else
	{
		GLMSG::SNETPC_REQ_RECALL_FB NetMsgFB;

		SNATIVEID sCurMapID = pChar->m_sMapID;

		BOOL bOK = RequestInvenRecallThisSvr ( pChar, nidMAP, UINT_MAX, vPOS );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
			SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			return FALSE;
		}

		// 버프와 상태이상 제거
		if ( sCurMapID != nidMAP )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		pChar->DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		pChar->DISABLESKEFF ( i );
		}

		pChar->ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.sMAPID = nidMAP;
		NetMsgFB.vPOS = pChar->m_vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgFB );
	}

	return TRUE;
}

BOOL GLGaeaServer::GMCtrlWhereNpc ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_WHERE_NPC_FLD *pNetMsg )
{
	if ( !m_bEmulator )
	{
		if ( dwClientID != m_dwAgentSlot ) return FALSE;
	
	}

	PGLCHAR pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )							return FALSE;

	if ( pCHAR->m_pLandMan )
	{
		GLLandMan *pLandMan = pCHAR->m_pLandMan;
		
		GLMSG::SNET_GM_WHERE_NPC_FB NetMsgFB;
		PGLCROW pCrow = NULL;
		int nPosX(INT_MAX), nPosY(INT_MAX);
		for ( DWORD i=0; i<MAXCROW; ++i )
		{
			pCrow = pLandMan->GetCrow(i);
			if ( pCrow && pCrow->m_pCrowData->sNativeID==pNetMsg->nidNPC )
			{				
				pLandMan->GetMapAxisInfo().Convert2MapPos ( pCrow->GetPosition().x, pCrow->GetPosition().z, nPosX, nPosY );
				
				NetMsgFB.nPosX = nPosX;
				NetMsgFB.nPosY = nPosY;
				GLGaeaServer::GetInstance().SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgFB );
			}
		}
	}
	
	return TRUE;
}

BOOL GLGaeaServer::GMCtrolWherePcPos ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_WHERE_PC_POS *pNetMsg )
{
	PGLCHAR pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )							return FALSE;
	if ( pCHAR->m_dwUserLvl < USER_GM3 )	return FALSE;

	bool bFOUD(false);
	int nPosX(INT_MAX), nPosY(INT_MAX);

	pCHAR = GetCharID(pNetMsg->dwCHARID);
	if ( pCHAR && pCHAR->m_pLandMan )
	{
		bFOUD = true;
		pCHAR->m_pLandMan->GetMapAxisInfo().Convert2MapPos ( pCHAR->m_vPos.x, pCHAR->m_vPos.z, nPosX, nPosY );
	}

	GLMSG::SNET_GM_WHERE_PC_POS_FB NetMsgFB;
	NetMsgFB.bFOUND = bFOUD;
	NetMsgFB.nPosX = nPosX;
	NetMsgFB.nPosY = nPosY;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( dwClientID, &NetMsgFB );

	return TRUE;
}

BOOL GLGaeaServer::GMCtrlWarningMSG ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_WARNING_MSG_FLD* pNetMsg )
{
	GLMSG::SNET_GM_WARNING_MSG_BRD NetMsgBrd;
	NetMsgBrd.bOn = pNetMsg->bOn;

	if ( dwClientID != m_dwAgentSlot )		return FALSE;

	PGLCHAR pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )							return FALSE;

	LANDQUADTREE* pLandTree = pCHAR->m_pLandMan->GetLandTree();
	if ( !pLandTree )						return FALSE;

	// 맵의 크기를 얻어온다.
	D3DXVECTOR3 vMax, vMin;
	pCHAR->m_pLandMan->GetNaviMeshAABB ( vMax, vMin );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect( (int)vMax.x,(int)vMax.z,(int)vMin.x,(int)vMin.z );
	pLandTree->FindNodes ( bRect, pLandTree->GetRootNode(), &pQuadNode );

	GLLandNode* pLandNode = NULL;
	GLCHARNODE *pCharCur = NULL;
	PGLCHAR pGLChar = NULL;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		pLandNode = pQuadNode->pData;

		GASSERT(pLandTree->GetCellNum()>pQuadNode->dwCID&&"Index over cell size");

		pCharCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCharCur; pCharCur = pCharCur->pNext )
		{
			pGLChar = pCharCur->Data;
			if( !pGLChar ) continue;

			SENDTOCLIENT ( pGLChar->m_dwClientID, &NetMsgBrd );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolBigHead ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_BIGHEAD* pNetMsg )
{
	if ( dwClientID != m_dwAgentSlot ) return FALSE;

	m_bBigHead = pNetMsg->bBIGHEAD;

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolBigHand ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_BIGHAND *pNetMsg )
{
	if ( dwClientID != m_dwAgentSlot ) return FALSE;

	m_bBigHand = pNetMsg->bBIGHAND;

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolFreePK ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_FREEPK *pNetMsg )
{
	if ( dwClientID != m_dwAgentSlot ) return FALSE;

	if ( pNetMsg->dwPKTIME != 0 )
        m_bBrightEvent = true;
	else 
		m_bBrightEvent = false;

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolEventEx ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_EVENT_EX *pNetMsg )
{
	if( dwClientID != m_dwAgentSlot )			return FALSE;
	if( !pNetMsg )								return FALSE;
	if( pNetMsg->emType == EMGM_EVENT_NONE )	return FALSE;

	m_sEVENTFACT.SetEVENT( pNetMsg->emType, pNetMsg->wValue );

	PGLCHAR pCHAR = NULL;
	CLIENTMAP_ITER iter = m_mapCHARID.begin();

	for( ; iter != m_mapCHARID.end(); ++iter )
	{
		pCHAR = GetChar((*iter).second);
		if ( !pCHAR )	continue;

		pCHAR->ReceiveEventEx( pNetMsg->emType, pNetMsg->wValue );
	}

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolEventExEnd ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_EVENT_EX_END *pNetMsg )
{
	if( dwClientID != m_dwAgentSlot )			return FALSE;
	if( !pNetMsg )								return FALSE;
	if( pNetMsg->emType == EMGM_EVENT_NONE )	return FALSE;

	m_sEVENTFACT.ResetEVENT( pNetMsg->emType );

	PGLCHAR pCHAR = NULL;
	CLIENTMAP_ITER iter = m_mapCHARID.begin();

	for( ; iter != m_mapCHARID.end(); ++iter )
	{
		pCHAR = GetChar((*iter).second);
		if ( !pCHAR )	continue;

		pCHAR->ReceiveEventEx( pNetMsg->emType, 0 );
	}

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolClassEvent   ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_CLASS_EVENT *pNetMsg )
{
	if( dwClientID != m_dwAgentSlot )			return FALSE;
	if( !pNetMsg )								return FALSE;

	if( pNetMsg->dwClassIndex > GLCC_ALL_NEWSEX ) return FALSE;

	bool bClassEvent[GLCI_NUM_NEWSEX];
	for( BYTE i = 0; i < GLCI_NUM_NEWSEX; i++ )
		bClassEvent[i] = FALSE;

	if( pNetMsg->dwClassIndex&GLCC_FIGHTER_M ) bClassEvent[0] = TRUE;
	if( pNetMsg->dwClassIndex&GLCC_ARMS_M )	   bClassEvent[1] = TRUE;
	if( pNetMsg->dwClassIndex&GLCC_ARCHER_W )  bClassEvent[2] = TRUE;	
	if( pNetMsg->dwClassIndex&GLCC_SPIRIT_W )  bClassEvent[3] = TRUE;		
	if( pNetMsg->dwClassIndex&GLCC_EXTREME_M ) bClassEvent[4] = TRUE;	
	if( pNetMsg->dwClassIndex&GLCC_EXTREME_W ) bClassEvent[5] = TRUE;	
	if( pNetMsg->dwClassIndex&GLCC_FIGHTER_W ) bClassEvent[6] = TRUE;	
	if( pNetMsg->dwClassIndex&GLCC_ARMS_W )    bClassEvent[7] = TRUE;	
	if( pNetMsg->dwClassIndex&GLCC_ARCHER_M )  bClassEvent[8] = TRUE;	
	if( pNetMsg->dwClassIndex&GLCC_SPIRIT_M )  bClassEvent[9] = TRUE;	

	if( pNetMsg->bStart )
	{		
		for( BYTE i = 0; i < GLCI_NUM_NEWSEX; i++ )
		{
			float fEventRate = 1.0f;

			if( bClassEvent[i] == TRUE ) fEventRate = pNetMsg->fEventRate;

			if( pNetMsg->dwEventType == 0 )		 
			{
				m_ClassEventData.fExpGainRate[i]  = fEventRate;
				m_ClassEventData.dwExpMinLevel[i] = pNetMsg->start_Lv;
				m_ClassEventData.dwExpMaxLevel[i] = pNetMsg->end_Lv;
			}else if( pNetMsg->dwEventType == 1 )
			{
				m_ClassEventData.fItemGainRate[i]  = fEventRate;
				m_ClassEventData.dwItemMinLevel[i] = pNetMsg->start_Lv;
				m_ClassEventData.dwItemMaxLevel[i] = pNetMsg->end_Lv;
			}else if( pNetMsg->dwEventType == 2 )
			{
				m_ClassEventData.fMoneyGainRate[i]  = fEventRate;
				m_ClassEventData.dwMoneyMinLevel[i] = pNetMsg->start_Lv;
				m_ClassEventData.dwMoneyMaxLevel[i] = pNetMsg->end_Lv;
			}	
		}

	
	}else{
		for( BYTE i = 0; i < GLCI_NUM_NEWSEX; i++ )
		{
			float fEventRate = 1.0f;

			if( bClassEvent[i] == FALSE ) continue;

			if( pNetMsg->dwEventType == 0 )		 
			{
				m_ClassEventData.InitExpRate(i);
			}else if( pNetMsg->dwEventType == 1 )
			{
				m_ClassEventData.InitItemRate(i);
			}else if( pNetMsg->dwEventType == 2 )
			{
				m_ClassEventData.InitMoneyRate(i);
			}	
		}
	}

	if ( GLGaeaServer::GetInstance().GetChar(dwGaeaID) )
	{
		GLMSG::SNET_GM_CLASS_EVENT NetMsgFB;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( dwClientID, &NetMsgFB );
	}

	
	return TRUE;
}


BOOL GLGaeaServer::GMCtrolLimitEventBegin ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_LIMIT_EVENT_BEGIN *pNetMsg )
{
	if( dwClientID != m_dwAgentSlot )			return FALSE;
	if( !pNetMsg )								return FALSE;

	SEventState sEventState;

	sEventState.bEventStart = TRUE;
	sEventState.fItemGainRate = pNetMsg->itemGain_Rate;		
	sEventState.fExpGainRate  = pNetMsg->expGain_Rate;			
	sEventState.MinEventLevel = pNetMsg->start_Lv;		
	sEventState.MaxEventLevel = pNetMsg->end_Lv;	


	sEventState.EventPlayTime   = pNetMsg->play_Time * 60;
	sEventState.EventBusterTime = pNetMsg->buster_Time * 60; 		

	m_sEventState = sEventState;

	PGLCHAR pCHAR = NULL;
	CLIENTMAP_ITER iter = m_mapCHARID.begin();

	for( ; iter != m_mapCHARID.end(); ++iter )
	{
		pCHAR = GetChar((*iter).second);
		if ( !pCHAR )	continue;

		pCHAR->ReceiveBeginEvent( sEventState );
	}


	return TRUE;
}

BOOL GLGaeaServer::GMCtrolLimitEventTimeReset ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_LIMIT_EVENT_TIME_RESET *pNetMsg )
{
	if( dwClientID != m_dwAgentSlot )			return FALSE;
	if( !pNetMsg )								return FALSE;

	PGLCHAR pCHAR = NULL;
	pCHAR = GetChar( pNetMsg->dwGaeaID );
	pCHAR->m_sEventTime.loginTime = pNetMsg->loginTime;

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolLimitEventEnd ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_LIMIT_EVENT_END *pNetMsg )
{
	if( dwClientID != m_dwAgentSlot )			return FALSE;
	if( !pNetMsg )								return FALSE;

	SEventState sEventState;
	sEventState.bEventStart = FALSE;
	m_sEventState = sEventState;


	PGLCHAR pCHAR = NULL;
	CLIENTMAP_ITER iter = m_mapCHARID.begin();

	GLMSG::SNET_GM_LIMIT_EVENT_END_FB NetMsgEventEndFb;
	for( ; iter != m_mapCHARID.end(); ++iter )
	{
		pCHAR = GetChar((*iter).second);
		if ( !pCHAR )	continue;

		SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgEventEndFb );
			
	}

	return TRUE;
}


BOOL GLGaeaServer::GMKicUser( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_KICK_USER_PROC_FLD* pNetMsgFld )
{
	if ( dwClientID != m_dwAgentSlot ) return FALSE;

	PGLCHAR pTARCHAR = GetCharID ( pNetMsgFld->dwID );
	if ( !pTARCHAR ) return FALSE;

	ReserveDropOutPC ( pTARCHAR->GetCtrlID() );

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolShowMeTheMoney( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_SHOWMETHEMONEY_FLD* pNetMsgFld )
{
#if defined(_RELEASED) || defined(KRT_PARAM)
	if ( dwClientID != m_dwAgentSlot ) return FALSE;

	PGLCHAR pCHAR = GetChar ( pNetMsgFld->dwGaeaID );
	if ( !pCHAR )						return FALSE;

	pCHAR->CheckMoneyUpdate( pCHAR->m_lnMoney, pNetMsgFld->llMoney, TRUE, "Show Me The Money." );
	pCHAR->m_lnMoney += pNetMsgFld->llMoney;

	GLMSG::SNET_GM_SHOWMETHEMONEY NetMsgFB;
	NetMsgFB.llMoney = pCHAR->m_lnMoney;
	SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgFB );
#endif

	return TRUE;
}


BOOL GLGaeaServer::GMCtrolMoPrintCrowList ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_PRINT_CROWLIST_FLD* pNetMsg )
{
	if ( !m_bEmulator && dwClientID != m_dwAgentSlot ) return FALSE;

	PGLCHAR pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )				return FALSE;
	if ( !pCHAR->m_pLandMan )	return FALSE;

	//	Note : 가시 영역에 있는 모든 셀의 오브젝트들을 탐색한다.
	//
	int i;
	PGLCROW		pCrow;
	PCROWDATA	pCrowData;
	DWORD		dwID;

	EMCROW emCrow; 
	if ( pNetMsg->dwFindMob == 1 ) emCrow = CROW_MOB;
	else if ( pNetMsg->dwFindMob == 2 ) emCrow = CROW_NPC;

	map<DWORD, int> vecCrowDataList;
	for ( i = 0; i < MAXCROW; i++ )
	{
		pCrow = NULL;
		pCrowData = NULL;
		
		pCrow = pCHAR->m_pLandMan->GetCrow( i );
		if( pCrow == NULL ) continue;

		pCrowData = pCrow->m_pCrowData;
		if( pCrowData == NULL ) continue;
		if( pNetMsg->dwFindMob !=  0 )
		{
			if( pCrowData->m_emCrow != emCrow ) continue;
			if( pCrowData->m_emCrow != emCrow ) continue;
		}


		dwID = pCrow->m_sNativeID.dwID;
		
		if( vecCrowDataList.count( dwID ) == 0 )
		{
			vecCrowDataList.insert( std::make_pair( dwID, 1 ) );
		}
		else
		{
			vecCrowDataList[dwID] += 1;
		}
		
	}

	map<DWORD, int>::iterator iter = vecCrowDataList.begin();
	map<DWORD, int>::iterator iter_end = vecCrowDataList.end();

	GLMSG::SNET_GM_PRINT_CROWLIST_FB NetMsgFB;
	NetMsgFB.emCrow = emCrow;

	for ( ; iter != iter_end; ++iter )
	{
		SNATIVEID sID = iter->first;
		int nNum = iter->second;

		
		NetMsgFB.mID	= sID.wMainID;
		NetMsgFB.sID	= sID.wSubID;
		NetMsgFB.wNum = nNum;		

		GLGaeaServer::GetInstance().SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgFB );
	}



	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMobGen ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_GEN_FLD *pNetMsg )
{
	if ( !m_bEmulator && dwClientID != m_dwAgentSlot ) return FALSE;

	SNATIVEID sID(pNetMsg->wMainID,pNetMsg->wSubID);
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sID );
	if ( !pCrowData )	return FALSE;

	PGLCHAR pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )				return FALSE;
	/*if ( !pCHAR->m_pLandMan )	return FALSE;
	pCHAR->m_pLandMan->DropCrow ( sID, pNetMsg->wPosX, pNetMsg->wPosY );*/
	GLLandMan* pLandMan = GetByMapID ( pCHAR->m_sMapID );
	if( !pLandMan ) return FALSE;

	if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
		pLandMan->DropMaterial ( sID, pNetMsg->wPosX, pNetMsg->wPosY );
	else	
		pLandMan->DropCrow ( sID, pNetMsg->wPosX, pNetMsg->wPosY );

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMobGenEx ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_GEN_EX_FLD *pNetMsg)
{
	if ( !m_bEmulator && dwClientID != m_dwAgentSlot ) return FALSE;

	SNATIVEID sMobID(pNetMsg->wMobMID,pNetMsg->wMobSID);
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sMobID );
	if ( !pCrowData )	return FALSE;

	const SNATIVEID sMapID(pNetMsg->wMapMID,pNetMsg->wMapSID);
	GLLandMan* pLandMan = GetByMapID ( sMapID );
	if ( !pLandMan )	return FALSE;

	D3DXVECTOR3 vBasePos(0,0,0), vGenPos(0,0,0);
	GLMapAxisInfo& MapAxisInfo = pLandMan->GetMapAxisInfo ();
	MapAxisInfo.MapPos2MiniPos ( pNetMsg->wPosX, pNetMsg->wPosY, vBasePos.x, vBasePos.z );
	vBasePos.x -= (float)pNetMsg->wRange*50;
	vBasePos.z -= (float)pNetMsg->wRange*50;
	DWORD wRange = pNetMsg->wRange*100;

	if ( wRange < pNetMsg->wNum ) return FALSE;

	std::vector<DWORD> vecRandPosX, vecRandPosZ;
	for ( DWORD i = 0; i < wRange; ++i ) vecRandPosX.push_back(i);
	std::random_shuffle ( vecRandPosX.begin(), vecRandPosX.end() );
	for ( DWORD i = 0; i < wRange; ++i ) vecRandPosZ.push_back(i);
	std::random_shuffle ( vecRandPosZ.begin(), vecRandPosZ.end() );

	for ( DWORD i = 0; i < pNetMsg->wNum; ++i )
	{
		vGenPos.x = (float)(vBasePos.x + vecRandPosX[i]);
		vGenPos.z = (float)(vBasePos.z + vecRandPosZ[i]);

		if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
		{
			DWORD dwGlobID = pLandMan->DropMaterial ( sMobID, vGenPos.x, vGenPos.z );
			pLandMan->m_listDelGlobIDExMat.push_back(dwGlobID);
		}
		else 
		{
			DWORD dwGlobID = pLandMan->DropCrow ( sMobID, vGenPos.x, vGenPos.z );
			pLandMan->m_listDelGlobIDEx.push_back(dwGlobID);
		}
	}

	return TRUE;
}


BOOL GLGaeaServer::GMCtrolMobDelEx ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_DEL_EX_FLD* pNetMsg )
{
	if ( !m_bEmulator && dwClientID != m_dwAgentSlot ) return FALSE;

	SNATIVEID sMobID(pNetMsg->wMobMID,pNetMsg->wMobSID);
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sMobID );
	if ( !pCrowData )	return FALSE;

	const SNATIVEID sMapID(pNetMsg->wMapMID,pNetMsg->wMapSID);
	GLLandMan* pLandMan = GetByMapID ( sMapID );
	if ( !pLandMan )	return FALSE;

	if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
	{
		LISTDELMATERIALID_ITER iter = pLandMan->m_listDelGlobIDExMat.begin();
		LISTDELMATERIALID_ITER iter_end = pLandMan->m_listDelGlobIDExMat.end();
		for ( ;iter != iter_end; ) 
		{
			LISTDELMATERIALID_ITER iter_cur = iter++;
			PGLMATERIAL pMaterial = pLandMan->GetMaterial ( *iter_cur );
			if ( !pMaterial ) continue;

			if ( pMaterial->m_sNativeID == sMobID ) 
			{
				pLandMan->DropOutMaterial ( *iter_cur );
				pLandMan->m_listDelGlobIDExMat.erase(iter_cur);
			}
		}
	}
	else
	{
		LISTDELCROWID_ITER iter = pLandMan->m_listDelGlobIDEx.begin();
		LISTDELCROWID_ITER iter_end = pLandMan->m_listDelGlobIDEx.end();
		for ( ;iter != iter_end; ) 
		{
			LISTDELCROWID_ITER iter_cur = iter++;
			PGLCROW pCrow = pLandMan->GetCrow ( *iter_cur );
			if ( !pCrow ) continue;

			if ( pCrow->m_sNativeID == sMobID ) 
			{
				pLandMan->DropOutCrow ( *iter_cur );
				pLandMan->m_listDelGlobIDEx.erase(iter_cur);
			}
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMobDel ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_DEL_FLD *pNetMsg )
{
	if ( !m_bEmulator )
	{
		if ( dwClientID != m_dwAgentSlot ) return FALSE;
	}

	PGLCHAR pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )				return FALSE;

	GLLandMan* pLandMan = GetByMapID ( pCHAR->m_sMapID );
	if( !pLandMan ) return FALSE;

	if ( pNetMsg->sMobID == NATIVEID_NULL() ) 
	{
		if ( pNetMsg->bMaterial ) 
		{
			return pLandMan->DropOutMaterial();
		}
		else
		{
			return pLandMan->DropOutCrow();
		}
	}
	else
	{
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pNetMsg->sMobID );
		if ( !pCrowData )	return FALSE;

		if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
		{
			LISTDELMATERIALID_ITER iter = pLandMan->m_listDelGlobIDMat.begin();
			LISTDELMATERIALID_ITER iter_end = pLandMan->m_listDelGlobIDMat.end();			

			for ( ;iter != iter_end; ) 
			{
				LISTDELMATERIALID_ITER iter_cur = iter++;
				PGLMATERIAL pMaterial = pLandMan->GetMaterial ( *iter_cur );
				if ( !pMaterial ) continue;

				if ( pMaterial->m_sNativeID == pNetMsg->sMobID ) 
				{
					pLandMan->DropOutMaterial ( *iter_cur );
				}
			}
		}
		else
		{
			LISTDELCROWID_ITER iter = pLandMan->m_listDelGlobID.begin();
			LISTDELCROWID_ITER iter_end = pLandMan->m_listDelGlobID.end();

			for ( ;iter != iter_end; ) 
			{
				LISTDELCROWID_ITER iter_cur = iter++;
				PGLCROW pCrow = pLandMan->GetCrow ( *iter_cur );
				if ( !pCrow ) continue;

				if ( pCrow->m_sNativeID == pNetMsg->sMobID ) 
				{
					pLandMan->DropOutCrow ( *iter_cur );
				}
			}
		}

	}

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMobLevel ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_LEV *pNetMsg )
{
	if ( !m_bEmulator )
	{
		if ( dwClientID != m_dwAgentSlot ) return FALSE;
	}
	
	SNATIVEID nidMAP = SNATIVEID(pNetMsg->wMAP_MID,pNetMsg->wMAP_SID);
	if ( nidMAP==SNATIVEID(false) )		return FALSE;

	GLLandMan *pLandMan = GetByMapID ( nidMAP );
	if ( !pLandMan )					return FALSE;

	pLandMan->LoadMobSchManEx ( pNetMsg->szLevelFile );

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMobLevelClear ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_LEV_CLEAR *pNetMsg )
{
	if ( !m_bEmulator )
	{
		if ( dwClientID != m_dwAgentSlot ) return FALSE;
	}

	SNATIVEID nidMAP = SNATIVEID(pNetMsg->wMAP_MID,pNetMsg->wMAP_SID);
	if ( nidMAP==SNATIVEID(false) )		return FALSE;

	GLLandMan *pLandMan = GetByMapID ( nidMAP );
	if ( !pLandMan )					return FALSE;

	pLandMan->ClearMobSchManEx ();

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolChatBlockFld ( GLMSG::SNET_GM_CHAT_BLOCK_FLD *pNetMsg )
{
	PGLCHAR pCHAR = GetCharID ( pNetMsg->dwCHARID );
	if ( !pCHAR )	return FALSE;

	if ( pNetMsg->dwBLOCK_MINUTE!=0 )
	{
		//	블럭시간 지정.
		pCHAR->m_tCHATBLOCK;

		CTime cBLOCK = CTime::GetCurrentTime();
		CTimeSpan cSPAN(0,0,pNetMsg->dwBLOCK_MINUTE,0);
		cBLOCK += cSPAN;

		pCHAR->m_tCHATBLOCK = cBLOCK.GetTime();
	}
	else
	{
		//	블럭시간 해지.
		pCHAR->m_tCHATBLOCK = 0;
	}

	return TRUE;
}

BOOL GLGaeaServer::UserCtrolCharInfoFld ( GLMSG::SNET_USER_CHAR_INFO_FLD *pNetMsg )
{
	PGLCHAR pCHAR = GetCharID ( pNetMsg->dwCHARID );
	if ( !pCHAR )	return FALSE;

	int nPosX(0), nPosY(0);
	if ( pCHAR->m_pLandMan )
	{
		pCHAR->m_pLandMan->GetMapAxisInfo().Convert2MapPos ( pCHAR->m_vPos.x, pCHAR->m_vPos.z, nPosX, nPosY );
	}

	GLCLUB *pCLUB = GetClubMan().GetClub ( pCHAR->m_dwGuild );

	GLMSG::SNET_USER_CHAR_INFO_FLD_FB NetMsgFB;
	if ( pCLUB )	StringCchCopy ( NetMsgFB.szCLUB, CHAR_SZNAME, pCLUB->m_szName );
	else			StringCchCopy ( NetMsgFB.szCLUB, CHAR_SZNAME, "void" );
	NetMsgFB.dwCLIENTID_FB = pNetMsg->dwCLIENTID_FB;
	SENDTOAGENT ( &NetMsgFB );

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolCharInfoFld ( GLMSG::SNET_GM_CHAR_INFO_FLD *pNetMsg )
{
	PGLCHAR pCHAR = GetCharID ( pNetMsg->dwCHARID );
	if ( !pCHAR )	return FALSE;
	
	int nPosX(0), nPosY(0);
	if ( pCHAR->m_pLandMan )
	{
		pCHAR->m_pLandMan->GetMapAxisInfo().Convert2MapPos ( pCHAR->m_vPos.x, pCHAR->m_vPos.z, nPosX, nPosY );
	}

	GLCLUB *pCLUB = GetClubMan().GetClub ( pCHAR->m_dwGuild );

	GLMSG::SNET_GM_CHAR_INFO_FLD_FB NetMsgFB;
	NetMsgFB.dwCHARID = pCHAR->m_dwCharID;
	NetMsgFB.nidMAP = pCHAR->m_sMapID;
	NetMsgFB.wLEVEL = pCHAR->m_wLevel;
	NetMsgFB.wPOSX = nPosX;
	NetMsgFB.wPOSY = nPosY;
	NetMsgFB.sHP = pCHAR->m_sHP;
	NetMsgFB.sMP = pCHAR->m_sMP;
	NetMsgFB.sSP = pCHAR->m_sSP;
	NetMsgFB.sEXP = pCHAR->m_sExperience;
	if ( pCLUB )	StringCchCopy ( NetMsgFB.szCLUB, CHAR_SZNAME, pCLUB->m_szName );
	else			StringCchCopy ( NetMsgFB.szCLUB, CHAR_SZNAME, "void" );
	NetMsgFB.dwCLIENTID_FB = pNetMsg->dwCLIENTID_FB;
	SENDTOAGENT ( &NetMsgFB );
	
	return TRUE;
}

BOOL GLGaeaServer::RequestViewAllPlayer ( GLMSG::SNET_GM_VIEWALLPLAYER_FLD_REQ *pNetMsg )
{
	PGLCHAR pCHAR = GetChar ( pNetMsg->dwID );
	if ( !pCHAR )						 return FALSE;
	if ( pCHAR->m_dwUserLvl < USER_GM3 ) return FALSE;

	GLMSG::SNET_GM_VIEWALLPLAYER_FLD_FB NetMsgFB;

	LANDQUADTREE* pLandTree = pCHAR->m_pLandMan->GetLandTree();
	if ( !pLandTree )					return FALSE;

	// 맵의 크기를 얻어온다.
	D3DXVECTOR3 vMax, vMin;
	pCHAR->m_pLandMan->GetNaviMeshAABB ( vMax, vMin );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect( (int)vMax.x,(int)vMax.z,(int)vMin.x,(int)vMin.z );
	pLandTree->FindNodes ( bRect, pLandTree->GetRootNode(), &pQuadNode );

    DWORD dwPlayerNum = 0;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GLLandNode *pLandNode = pQuadNode->pData;
		if( !pLandNode ) continue;

		GASSERT(pLandTree->GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		//	Note : - Char -
		//
		GLCHARNODE *pCharCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCharCur; pCharCur = pCharCur->pNext )
		{
			PGLCHAR pGLChar = pCharCur->Data;
			if( !pGLChar ) continue;
			if ( pGLChar->m_dwClientID == pCHAR->m_dwClientID )	continue;

			// 여기서 패킷에 실어서 보내주면 된다
			GLMSG::SNET_GM_VIEWALLPLAYER_FLD_FB NetMsgFB;
			NetMsgFB.dwCHARID = pGLChar->m_dwCharID;
			StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, pGLChar->m_szName );

			SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgFB );
			dwPlayerNum++;
		}
	}
	NetMsgFB.dwPlayerNum = dwPlayerNum;
	SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgFB );

	return TRUE;

	/*
	GLLandNode* pLandNode = NULL;
	GLCHARNODE* pCharCur = NULL;
	PGLCHAR pGLChar = NULL;
	// GLMSG::SNET_GM_VIEWALLPLAYER_FLD_FB NetMsgFB;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		pLandNode = pQuadNode->pData;

		GASSERT(pLandTree->GetCellNum()>pQuadNode->dwCID&&"Index over cell size");

		//	Note : - Char -
		//
		pCharCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCharCur; pCharCur = pCharCur->pNext )
		{
			pGLChar = pCharCur->Data;
			if ( pGLChar->m_dwClientID == pCHAR->m_dwClientID )	continue;

			// 여기서 패킷에 실어서 보내주면 된다			
			NetMsgFB.dwCHARID = pGLChar->m_dwCharID;
			StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, pGLChar->m_szName );

			SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgFB );
		}
	}
	return TRUE;
	*/
}


BOOL GLGaeaServer::RequestShopInfo( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_SHOP_INFO_REQ *pNetMsg )
{

	LISTSEARCHSHOP_ITER iter	 = m_listSearchShop.begin();

	PGLCHAR pMyChar = GetChar ( dwGaeaID );
	if( pMyChar == NULL ) return TRUE;	
	if( pMyChar ->m_dwUserLvl < USER_GM3 ) return TRUE;


	TEXTCONSOLEMSG_WRITE( "[INFO]GM Command Shop Info Start" );


	int iMsgCount = 0;	

	for( ; iter != m_listSearchShop.end(); ++iter  )
	{	
		PGLCHAR pChar = GetChar ( *iter );
		if( pChar == NULL || pChar->m_sPMarket.IsOpen() == FALSE || pChar->m_sMapID.wMainID != 22 || pChar->m_sMapID.wSubID != 0  ) 
		{
			m_listSearchShop.erase( iter-- );		
			continue;
		}

		MAPSEARCH mapSearchItem = pChar->m_sPMarket.GetSearchItemList();
		if( pNetMsg->bBasicInfo )
		{
			MAPSEARCH_ITER iterItem		= mapSearchItem.begin();
			for( ; iterItem != mapSearchItem.end(); ++iterItem )
			{
				CString strItemInfo;
				SNATIVEID		sItemID	   = iterItem->first;
				SSEARCHITEMDATA searchItem = iterItem->second;			
				//					유저아이디 MID SID 가격
				strItemInfo.Format( "%d, %d, %d, %d, %I64d\n", pMyChar->GetUserID(), pMyChar->GetCharID(), sItemID.wMainID, sItemID.wSubID, searchItem.llPRICE );		

				GLMSG::SNET_GM_SHOP_INFO_FB NetMsgFB;
				NetMsgFB.bBasicInfo = pNetMsg->bBasicInfo;
				StringCbCopy(NetMsgFB.szBasicInfo, MAX_CHAR_LENGTH+1, strItemInfo.GetString());		

				if( iMsgCount == 0 ) NetMsgFB.bSTATE = GLMSG::SNET_GM_SHOP_INFO_FB::FIRST;
				else				 NetMsgFB.bSTATE = GLMSG::SNET_GM_SHOP_INFO_FB::MIDDLE;
				iMsgCount++;
				ReserveMessage( dwClientID, dwGaeaID, iMsgCount/100, &NetMsgFB );
			}
		}else{
			CString strItemInfo;
			if( mapSearchItem.find(pNetMsg->sSearchItemID.dwID) == mapSearchItem.end() ) continue; 
			SSEARCHITEMDATA searchItem = mapSearchItem[pNetMsg->sSearchItemID.dwID];

			strItemInfo.Format( "%d, %d, %d, %d, %I64d, %d, %f, %d, %f, %d, %f, %d, %f, %d, %d, %d, %d, %d, %d, %d\n", 
													   pMyChar->GetUserID(),			// 유저   NUM
													   pMyChar->GetCharID(),			// 캐릭터 NUM
													   pNetMsg->sSearchItemID.wMainID,  // 아이템 MID
													   pNetMsg->sSearchItemID.wSubID,	// 아이템 SID
													   searchItem.llPRICE,				// 아이템 가격

													   searchItem.RandOptTYPE1,			// 랜덤옵션 1 타입
													   searchItem.fRandOptValue1,		// 랜덤옵션 1 값
													   searchItem.RandOptTYPE2,			// 랜덤옵션 2 타입
													   searchItem.fRandOptValue2,		// 랜덤옵션 2 값
													   searchItem.RandOptTYPE3,			// 랜덤옵션 3 타입
													   searchItem.fRandOptValue3,		// 랜덤옵션 3 값
													   searchItem.RandOptTYPE4,			// 랜덤옵션 4 타입
													   searchItem.fRandOptValue4,		// 랜덤옵션 4 값

													   searchItem.cDAMAGE,				// 공격 연마 등급
													   searchItem.cDEFENSE,				// 방어 연마 등급

													   searchItem.cRESIST_FIRE,				//	저항(화) 개조 등급.
													   searchItem.cRESIST_ICE,				//	저항(빙) 개조 등급.
													   searchItem.cRESIST_ELEC,				//	저항(전) 개조 등급.
													   searchItem.cRESIST_POISON,			//	저항(독) 개조 등급.
													   searchItem.cRESIST_SPIRIT );			//	저항(정) 개조 등급.

			GLMSG::SNET_GM_SHOP_INFO_FB NetMsgFB;
			NetMsgFB.bBasicInfo = pNetMsg->bBasicInfo;
			StringCbCopy(NetMsgFB.szBasicInfo, MAX_CHAR_LENGTH+1, strItemInfo.GetString());	

			if( iMsgCount == 0 ) NetMsgFB.bSTATE = GLMSG::SNET_GM_SHOP_INFO_FB::FIRST;
			else				 NetMsgFB.bSTATE = GLMSG::SNET_GM_SHOP_INFO_FB::MIDDLE;
			iMsgCount++;
			ReserveMessage( dwClientID, dwGaeaID, iMsgCount/100, &NetMsgFB );
		}
	}

	GLMSG::SNET_GM_SHOP_INFO_FB NetMsgFB;
	if( iMsgCount == 0 )
		sprintf_s( NetMsgFB.szBasicInfo, "Not Found Item Count" );
	else
		sprintf_s( NetMsgFB.szBasicInfo, "Found Item Count : %d", iMsgCount );
	NetMsgFB.bSTATE = GLMSG::SNET_GM_SHOP_INFO_FB::END;
	ReserveMessage( dwClientID, dwGaeaID, iMsgCount/100, &NetMsgFB );

	TEXTCONSOLEMSG_WRITE( "[INFO]GM Command Shop Info End" );

	return TRUE;
}


BOOL GLGaeaServer::RequestConfrontPartyChkMbr ( GLMSG::SNETPC_CONFRONTPTY_CHECKMBR2_FLD *pNetMsg )
{
	const SNATIVEID &sMapID = pNetMsg->sMapID;

	GLLandMan* pLandMan = GetByMapID ( sMapID );
	if ( !pLandMan )	return FALSE;

	GLPARTY_FIELD* pPartyA = GetParty ( pNetMsg->dwPARTY_A );
	if ( !pPartyA )		return FALSE;

	GLPARTY_FIELD* pPartyB = GetParty ( pNetMsg->dwPARTY_B );
	if ( !pPartyB )		return FALSE;

	GLChar* pCharA = GetChar(pPartyA->m_dwMASTER);
	GLChar* pCharB = GetChar(pPartyB->m_dwMASTER);

	if ( !pCharA || !pCharB )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB;
		NetMsgFB.emTYPE = EMCONFT_PARTY;
		NetMsgFB.dwID = pNetMsg->dwPARTY_A;
		NetMsgFB.emFB = EMCONFRONT_FAIL;

		if ( pCharA )	GLGaeaServer::GetInstance().SENDTOAGENT ( pCharA->m_dwClientID, &NetMsgFB );
		if ( pCharB )	GLGaeaServer::GetInstance().SENDTOAGENT ( pCharB->m_dwClientID, &NetMsgFB );

		return FALSE;
	}

	GLMSG::SNETPC_CONFRONTPTY_CHECKMBR2_AGT NetMsgCheckMbr;
	NetMsgCheckMbr.sMapID = pNetMsg->sMapID;
	NetMsgCheckMbr.dwPARTY_A = pNetMsg->dwPARTY_A;
	NetMsgCheckMbr.dwPARTY_B = pNetMsg->dwPARTY_B;
	NetMsgCheckMbr.sOption = pNetMsg->sOption;
	NetMsgCheckMbr.vPosition = ( pCharA->m_vPos + pCharB->m_vPos ) / 2.0f;
	
	bool bSCHOOL_CONFT_LEVEL(true);
	int nCheck(0);
	GLPARTY_FIELD::MEMBER_ITER iter, end;
	
	iter = pPartyA->m_cMEMBER.begin();
	end = pPartyA->m_cMEMBER.end();
	for ( ; iter!=end; ++iter )
	{
		GLChar* pMember = GetChar ( (*iter).first );
		if ( !pMember )								continue;
		if ( pNetMsg->sMapID!=pMember->m_sMapID )	continue;
		
		if ( pMember->m_wLevel < GLCONST_CHAR::wCONFRONT_SCHOOL_LIMIT_LEVEL )		bSCHOOL_CONFT_LEVEL = false;

		D3DXVECTOR3 vDistance = pMember->m_vPos - NetMsgCheckMbr.vPosition;
		float fDistance = D3DXVec3Length ( &vDistance );

		if ( fDistance < GLCONST_CHAR::fCONFRONT_PY_DIST )
		{
			NetMsgCheckMbr.dwPARTY_A_MEMBER[nCheck++] = pMember->m_dwGaeaID;
		}
	}

	nCheck = 0;
	iter = pPartyB->m_cMEMBER.begin();
	end = pPartyB->m_cMEMBER.end();
	for ( ; iter!=end; ++iter )
	{
		GLChar* pMember = GetChar ( (*iter).first );
		if ( !pMember )								continue;
		if ( pNetMsg->sMapID!=pMember->m_sMapID )	continue;

		if ( pMember->m_wLevel < GLCONST_CHAR::wCONFRONT_SCHOOL_LIMIT_LEVEL )		bSCHOOL_CONFT_LEVEL = false;
		
		D3DXVECTOR3 vDistance = pMember->m_vPos - NetMsgCheckMbr.vPosition;
		float fDistance = D3DXVec3Length ( &vDistance );

		if ( fDistance < GLCONST_CHAR::fCONFRONT_PY_DIST )
		{
			NetMsgCheckMbr.dwPARTY_B_MEMBER[nCheck++] = pMember->m_dwGaeaID;
		}
	}

	NetMsgCheckMbr.sOption.bSCHOOL = bSCHOOL_CONFT_LEVEL;

	SENDTOAGENT	( &NetMsgCheckMbr );

	return TRUE;
}

BOOL GLGaeaServer::RequestConfrontParty ( GLMSG::SNETPC_CONFRONTPTY_START2_FLD *pNetMsg )
{
	const SNATIVEID &sMapID = pNetMsg->sMapID;

	GLLandMan* pLandMan = GetByMapID ( sMapID );
	if ( !pLandMan )	return FALSE;

	GLPARTY_FIELD* pPartyA = GetParty ( pNetMsg->dwPARTY_A );
	if ( !pPartyA )		return FALSE;

	GLPARTY_FIELD* pPartyB = GetParty ( pNetMsg->dwPARTY_B );
	if ( !pPartyB )		return FALSE;

	GLChar* pCharA = GetChar(pPartyA->m_dwMASTER);
	GLChar* pCharB = GetChar(pPartyB->m_dwMASTER);

	if ( !pCharA || !pCharB )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB;
		NetMsgFB.emTYPE = EMCONFT_PARTY;
		NetMsgFB.dwID = pNetMsg->dwPARTY_A;
		NetMsgFB.emFB = EMCONFRONT_FAIL;

		if ( pCharA )	GLGaeaServer::GetInstance().SENDTOAGENT ( pCharA->m_dwClientID, &NetMsgFB );
		if ( pCharB )	GLGaeaServer::GetInstance().SENDTOAGENT ( pCharB->m_dwClientID, &NetMsgFB );

		return FALSE;
	}

	D3DXVECTOR3 vPosition = pNetMsg->vPos;

	//	Note : 학교 대련.
	WORD wSCHOOL_A(0);
	WORD wSCHOOL_B(0);
	if ( pNetMsg->sOption.bSCHOOL )
	{
		wSCHOOL_A = pCharA->m_wSchool;
		wSCHOOL_B = pCharB->m_wSchool;
	}

	//	Note : 첫번째 파티원들에 '파티대련' 정보 설정.
	//
	pPartyA->m_sconftOPTION = pNetMsg->sOption;
	pPartyA->m_dwconftPARTYID = pNetMsg->dwPARTY_B;
	
	GLMSG::SNETPC_CONFRONTPTY_START2_CLT sNetMsgClt;
	sNetMsgClt.dwconftPARTY = pNetMsg->dwPARTY_B;
	memcpy ( sNetMsgClt.dwCONFRONT_MY_MEMBER, pNetMsg->dwPARTY_A_MEMBER, sizeof(DWORD)*MAXPARTY );
	memcpy ( sNetMsgClt.dwCONFRONT_TAR_MEMBER, pNetMsg->dwPARTY_B_MEMBER, sizeof(DWORD)*MAXPARTY );
	sNetMsgClt.vPosition = vPosition;
	sNetMsgClt.sOption = pNetMsg->sOption;
	sNetMsgClt.sOption.wMY_SCHOOL = wSCHOOL_A;
	sNetMsgClt.sOption.wTAR_SCHOOL = wSCHOOL_B;

	if ( pCharB )
	{
		StringCbCopy(sNetMsgClt.szName, CHR_ID_LENGTH+1, pCharB->m_szName);		
	}

	int i(0);
	DWORD dwMember = 0;
	GLChar* pChar = NULL;
	for ( i=0; i<MAXPARTY; ++i )
	{
		dwMember = pNetMsg->dwPARTY_A_MEMBER[i];
		if ( GAEAID_NULL==dwMember )	continue;
		pChar = GetChar ( dwMember );
		if ( !pChar )
		{
			continue;
		}
		else
		{
			pPartyA->MBR_CONFRONT_JOIN ( dwMember );

			pChar->m_sCONFTING.RESET();
			pChar->m_sCONFTING.emTYPE = EMCONFT_PARTY;
			pChar->m_sCONFTING.dwTAR_ID = pPartyA->m_dwconftPARTYID;
			pChar->m_sCONFTING.vPosition = vPosition;
			pChar->m_sCONFTING.sOption = pNetMsg->sOption;

			//	Note : 클라이언트들에 '파티대련' 정보 전송.
			//
			SENDTOCLIENT ( pChar->m_dwClientID, &sNetMsgClt );

			//	Note : 적대행위자 삭제.
			//
			pChar->DelPlayHostile();

			//	Note : 대련 체력 백업.
			//
			pChar->DoConftStateBackup();
		}
	}


	//	Note : 두번쩨 파티원들에 '파티대련' 정보 설정.
	//
	pPartyB->m_sconftOPTION = pNetMsg->sOption;
	pPartyB->m_dwconftPARTYID = pNetMsg->dwPARTY_A;

	sNetMsgClt.dwconftPARTY = pNetMsg->dwPARTY_A;
	memcpy ( sNetMsgClt.dwCONFRONT_MY_MEMBER, pNetMsg->dwPARTY_B_MEMBER, sizeof(DWORD)*MAXPARTY );
	memcpy ( sNetMsgClt.dwCONFRONT_TAR_MEMBER, pNetMsg->dwPARTY_A_MEMBER, sizeof(DWORD)*MAXPARTY );
	sNetMsgClt.vPosition = vPosition;
	sNetMsgClt.sOption = pNetMsg->sOption;
	sNetMsgClt.sOption.wMY_SCHOOL = wSCHOOL_B;
	sNetMsgClt.sOption.wTAR_SCHOOL = wSCHOOL_A;

	if ( pCharA )
	{
		StringCbCopy(sNetMsgClt.szName, CHR_ID_LENGTH+1, pCharA->m_szName);
	}

	for ( i=0; i<MAXPARTY; ++i )
	{
		dwMember = pNetMsg->dwPARTY_B_MEMBER[i];
		if ( GAEAID_NULL==dwMember )	continue;
		pChar = GetChar ( dwMember );
		if ( !pChar )
		{
			continue;
		}
		else
		{
			pPartyB->MBR_CONFRONT_JOIN ( dwMember );
			
			pChar->m_sCONFTING.RESET();
			pChar->m_sCONFTING.emTYPE = EMCONFT_PARTY;
			pChar->m_sCONFTING.dwTAR_ID = pPartyB->m_dwconftPARTYID;
			pChar->m_sCONFTING.vPosition = vPosition;
			pChar->m_sCONFTING.sOption = pNetMsg->sOption;

			//	Note : 클라이언트들에 '파티대련' 정보 전송.
			//
			SENDTOCLIENT ( pChar->m_dwClientID, &sNetMsgClt );

			//	Note : 적대행위자 삭제.
			//
			pChar->DelPlayHostile();

			//	Note : 대련 체력 백업.
			//
			pChar->DoConftStateBackup();
		}
	}

	DWORD dwLevelA = pPartyA->GETAVER_CONFT_LEVEL();
	DWORD dwLevelB = pPartyB->GETAVER_CONFT_LEVEL();

	return TRUE;
}

BOOL GLGaeaServer::RequestConfrontPartyEnd ( GLMSG::SNETPC_CONFRONTPTY_END2_FLD *pNetMsg )
{
	if ( pNetMsg->dwPARTY_A!=PARTY_NULL && pNetMsg->dwPARTY_B!=PARTY_NULL )
	{
		GLPARTY_FIELD *pPartyA = GetParty(pNetMsg->dwPARTY_A);
		GLPARTY_FIELD *pPartyB = GetParty(pNetMsg->dwPARTY_B);

		//	Note : 파티 대련후 파티가 사라진 부분이 있으면 오류 처리.
		//
		if ( pPartyA && pPartyB )
		{
			DWORD dwLevelA = pPartyA->m_dwconftMEMLEVEL;
			DWORD dwLevelB = pPartyB->m_dwconftMEMLEVEL;
			const SCONFT_OPTION sOption = pPartyB->m_sconftOPTION;

			float fLIFE_P_A(0), fLIFE_P_B(0);

			if ( GLCONST_CHAR::nCONFRONT_WIN_LP!=0 && GLCONST_CHAR::nCONFRONT_LOSS_LP )
			{
				if ( pNetMsg->emEND_A==EMCONFRONT_END_PWIN )
				{
					fLIFE_P_A = GLCONST_CHAR::nCONFRONT_WIN_LP + (dwLevelB-dwLevelA)/2.0f
						+ (sOption.fTAR_HP_RATE-sOption.fHP_RATE);
					LIMIT ( fLIFE_P_A, 20.0f, 0.0f );

					fLIFE_P_B = GLCONST_CHAR::nCONFRONT_LOSS_LP - (dwLevelB-dwLevelA)/2.0f
						- (sOption.fTAR_HP_RATE-sOption.fHP_RATE);
					LIMIT ( fLIFE_P_B, 0.0f, -20.0f );
				}
				else if ( pNetMsg->emEND_B==EMCONFRONT_END_PWIN )
				{
					fLIFE_P_B = GLCONST_CHAR::nCONFRONT_WIN_LP + (dwLevelA-dwLevelB)/2.0f
						+ (sOption.fHP_RATE-sOption.fTAR_HP_RATE);
					LIMIT ( fLIFE_P_B, 20.0f, 0.0f );

					fLIFE_P_A = GLCONST_CHAR::nCONFRONT_LOSS_LP - (dwLevelA-dwLevelB)/2.0f
						- (sOption.fHP_RATE-sOption.fTAR_HP_RATE);
					LIMIT ( fLIFE_P_A, 0.0f, -20.0f );
				}
			}

			pPartyA->RECEIVELIVINGPOINT ( (int)fLIFE_P_A );
			pPartyB->RECEIVELIVINGPOINT ( (int)fLIFE_P_B );
		}
		else
		{
			if ( pPartyA )
			{
				if ( pNetMsg->emEND_A==EMCONFRONT_END_PWIN )	pPartyA->RECEIVELIVINGPOINT ( 0 );
				else											pPartyA->RECEIVELIVINGPOINT ( 0 );
			}

			if ( pPartyB )
			{
				if ( pNetMsg->emEND_B==EMCONFRONT_END_PWIN )	pPartyB->RECEIVELIVINGPOINT ( 0 );
				else											pPartyB->RECEIVELIVINGPOINT ( 0 );
			}
		}
	}

	if ( pNetMsg->dwPARTY_A!=PARTY_NULL )
	{
		GLPARTY_FIELD *pParty = GetParty(pNetMsg->dwPARTY_A);
		if ( pParty )	pParty->RESET_CONFRONT(pNetMsg->emEND_A);
	}

	if ( pNetMsg->dwPARTY_B!=PARTY_NULL )
	{
		GLPARTY_FIELD *pParty = GetParty(pNetMsg->dwPARTY_B);
		if ( pParty )	pParty->RESET_CONFRONT(pNetMsg->emEND_B);
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestConfrontClubChkMbr ( GLMSG::SNET_CONFRONTCLB_CHECKMBR_FLD *pNetMsg )
{
	const SNATIVEID &sMapID = pNetMsg->sMapID;

	GLLandMan* pLandMan = GetByMapID ( sMapID );
	if ( !pLandMan )	return FALSE;

	GLCLUB* pClubA = m_cClubMan.GetClub ( pNetMsg->dwCLUB_A );
	if ( !pClubA )		return FALSE;

	GLCLUB* pClubB = m_cClubMan.GetClub ( pNetMsg->dwCLUB_B );
	if ( !pClubB )		return FALSE;

	GLChar* pCharA = GetCharID(pClubA->m_dwMasterID);
	GLChar* pCharB = GetCharID(pClubB->m_dwMasterID);

	if ( !pCharA || !pCharB )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB;
		NetMsgFB.emTYPE = EMCONFT_GUILD;
		NetMsgFB.dwID = pNetMsg->dwCLUB_A;
		NetMsgFB.emFB = EMCONFRONT_FAIL;

		if ( pCharA )	GLGaeaServer::GetInstance().SENDTOAGENT ( pCharA->m_dwClientID, &NetMsgFB );
		if ( pCharB )	GLGaeaServer::GetInstance().SENDTOAGENT ( pCharB->m_dwClientID, &NetMsgFB );

		return FALSE;
	}

	GLMSG::SNET_CONFRONTCLB_CHECKMBR_AGT NetMsgCheckMbr;
	NetMsgCheckMbr.sMapID = pNetMsg->sMapID;
	NetMsgCheckMbr.dwCLUB_A = pNetMsg->dwCLUB_A;
	NetMsgCheckMbr.dwCLUB_B = pNetMsg->dwCLUB_B;
	NetMsgCheckMbr.sOption = pNetMsg->sOption;
	NetMsgCheckMbr.vPosition = ( pCharA->m_vPos + pCharB->m_vPos ) / 2.0f;
	
	bool bSCHOOL_CONFT_LEVEL(true);
	CLUBMEMBERS_ITER pos, end;
	
	pos = pClubA->m_mapMembers.begin();
	end = pClubA->m_mapMembers.end();
	for ( ; pos!=end && NetMsgCheckMbr.dwCLUB_A_NUM < EMMAX_CLUB_NUM; ++pos )
	{
		GLChar* pMember = GetCharID ( (*pos).first );
		if ( !pMember )								continue;
		if ( pNetMsg->sMapID!=pMember->m_sMapID )	continue;
		
		D3DXVECTOR3 vDistance = pMember->m_vPos - NetMsgCheckMbr.vPosition;
		float fDistance = D3DXVec3Length ( &vDistance );

		if ( fDistance < GLCONST_CHAR::fCONFRONT_CLB_DIST )
		{
			NetMsgCheckMbr.dwCLUB_A_MEMBER[NetMsgCheckMbr.dwCLUB_A_NUM++] = pMember->m_dwCharID;
		}
	}

	pos = pClubB->m_mapMembers.begin();
	end = pClubB->m_mapMembers.end();
	for ( ; pos!=end && NetMsgCheckMbr.dwCLUB_B_NUM < EMMAX_CLUB_NUM; ++pos )
	{
		GLChar* pMember = GetCharID ( (*pos).first );
		if ( !pMember )								continue;
		if ( pNetMsg->sMapID!=pMember->m_sMapID )	continue;

		D3DXVECTOR3 vDistance = pMember->m_vPos - NetMsgCheckMbr.vPosition;
		float fDistance = D3DXVec3Length ( &vDistance );

		if ( fDistance < GLCONST_CHAR::fCONFRONT_CLB_DIST )
		{
			NetMsgCheckMbr.dwCLUB_B_MEMBER[NetMsgCheckMbr.dwCLUB_B_NUM++] = pMember->m_dwCharID;
		}
	}

	SENDTOAGENT	( &NetMsgCheckMbr );

	return TRUE;
}

BOOL GLGaeaServer::RequestConfrontClub ( GLMSG::SNETPC_CONFRONTCLB_START2_FLD *pNetMsg )
{
	const SNATIVEID &sMapID = pNetMsg->sMapID;

	GLLandMan* pLandMan = GetByMapID ( sMapID );
	if ( !pLandMan )	return FALSE;

	GLCLUB* pClubA = m_cClubMan.GetClub ( pNetMsg->dwCLUB_A );
	if ( !pClubA )		return FALSE;

	GLCLUB* pClubB = m_cClubMan.GetClub ( pNetMsg->dwCLUB_B );
	if ( !pClubB )		return FALSE;

	GLChar* pCharA = GetCharID(pClubA->m_dwMasterID);
	GLChar* pCharB = GetCharID(pClubB->m_dwMasterID);

	if ( !pCharA || !pCharB )
	{
		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB;
		NetMsgFB.emTYPE = EMCONFT_GUILD;
		NetMsgFB.dwID = pNetMsg->dwCLUB_A;
		NetMsgFB.emFB = EMCONFRONT_FAIL;

		if ( pCharA )	GLGaeaServer::GetInstance().SENDTOAGENT ( pCharA->m_dwClientID, &NetMsgFB );
		if ( pCharB )	GLGaeaServer::GetInstance().SENDTOAGENT ( pCharB->m_dwClientID, &NetMsgFB );

		return FALSE;
	}

	D3DXVECTOR3 vPosition = pNetMsg->vPos;

	//	Note : 첫번째 클럽원들에 '클럽대련' 정보 설정.
	//
	pClubA->m_conftOPT = pNetMsg->sOption;
	pClubA->m_dwconftCLUB = pNetMsg->dwCLUB_B;
	
	GLMSG::SNETPC_CONFRONTCLB_START2_CLT sNetMsgClt;
	sNetMsgClt.dwconftCLUB = pNetMsg->dwCLUB_B;
	sNetMsgClt.dwCLUB_MY_NUM = pNetMsg->dwCLUB_A_NUM;
	memcpy ( sNetMsgClt.dwCONFRONT_MY_MEMBER, pNetMsg->dwCLUB_A_MEMBER, sizeof(DWORD)*EMMAX_CLUB_NUM );
	sNetMsgClt.dwCLUB_TAR_NUM = pNetMsg->dwCLUB_B_NUM;
	memcpy ( sNetMsgClt.dwCONFRONT_TAR_MEMBER, pNetMsg->dwCLUB_B_MEMBER, sizeof(DWORD)*EMMAX_CLUB_NUM );
	sNetMsgClt.vPosition = vPosition;
	sNetMsgClt.sOption = pNetMsg->sOption;

	StringCchCopy ( sNetMsgClt.szName, sizeof(char)*(CHR_ID_LENGTH+1), pClubB->m_szName );

	DWORD i(0);
	DWORD dwMember = 0;
	GLChar* pChar = NULL;
	for ( i=0; i<pNetMsg->dwCLUB_A_NUM; ++i )
	{
		dwMember = pNetMsg->dwCLUB_A_MEMBER[i];
		pChar = GetCharID ( dwMember );
		if ( !pChar )
		{
			continue;
		}
		else
		{
			pClubA->ADDCONFT_MEMBER ( dwMember );

			pChar->m_sCONFTING.RESET();
			pChar->m_sCONFTING.emTYPE = EMCONFT_GUILD;
			pChar->m_sCONFTING.dwTAR_ID = pClubA->m_dwconftCLUB;
			pChar->m_sCONFTING.vPosition = vPosition;
			pChar->m_sCONFTING.sOption = pNetMsg->sOption;

			//	Note : 클라이언트들에 '클럽대련' 정보 전송.
			//
			SENDTOCLIENT ( pChar->m_dwClientID, &sNetMsgClt );

			//	Note : 적대행위자 삭제.
			//
			pChar->DelPlayHostile();

			//	Note : 대련 체력 백업.
			//
			pChar->DoConftStateBackup();
		}
	}
	pClubA->SETCONFT_MEMBER ();

	//	Note : 두번쩨 클럽원들에 '클럽대련' 정보 설정.
	//
	pClubB->m_conftOPT = pNetMsg->sOption;
	pClubB->m_dwconftCLUB = pNetMsg->dwCLUB_A;

	sNetMsgClt.dwconftCLUB = pNetMsg->dwCLUB_A;

	sNetMsgClt.dwCLUB_MY_NUM = pNetMsg->dwCLUB_B_NUM;
	memcpy ( sNetMsgClt.dwCONFRONT_MY_MEMBER, pNetMsg->dwCLUB_B_MEMBER, sizeof(DWORD)*EMMAX_CLUB_NUM );
	sNetMsgClt.dwCLUB_TAR_NUM = pNetMsg->dwCLUB_A_NUM;
	memcpy ( sNetMsgClt.dwCONFRONT_TAR_MEMBER, pNetMsg->dwCLUB_A_MEMBER, sizeof(DWORD)*EMMAX_CLUB_NUM );
	sNetMsgClt.vPosition = vPosition;
	sNetMsgClt.sOption = pNetMsg->sOption;

	StringCchCopy ( sNetMsgClt.szName, sizeof(char)*(CHR_ID_LENGTH+1), pClubA->m_szName );

	for ( i=0; i<pNetMsg->dwCLUB_B_NUM; ++i )
	{
		dwMember = pNetMsg->dwCLUB_B_MEMBER[i];
		pChar = GetCharID ( dwMember );
		if ( !pChar )					
		{
			continue;
		}
		else
		{
			pClubB->ADDCONFT_MEMBER ( dwMember );
			
			pChar->m_sCONFTING.RESET();
			pChar->m_sCONFTING.emTYPE = EMCONFT_GUILD;
			pChar->m_sCONFTING.dwTAR_ID = pClubB->m_dwconftCLUB;
			pChar->m_sCONFTING.vPosition = vPosition;
			pChar->m_sCONFTING.sOption = pNetMsg->sOption;

			//	Note : 클라이언트들에 '클럽대련' 정보 전송.
			//
			SENDTOCLIENT ( pChar->m_dwClientID, &sNetMsgClt );

			//	Note : 적대행위자 삭제.
			//
			pChar->DelPlayHostile();

			//	Note : 대련 체력 백업.
			//
			pChar->DoConftStateBackup();
		}
	}
	pClubB->SETCONFT_MEMBER ();

	return TRUE;
}

BOOL GLGaeaServer::RequestConfrontClubEnd ( GLMSG::SNETPC_CONFRONTCLB_END2_FLD *pNetMsg )
{
	if ( pNetMsg->dwCLUB_A!=CLUB_NULL && pNetMsg->dwCLUB_B!=CLUB_NULL )
	{
		GLCLUB *pClubA = m_cClubMan.GetClub(pNetMsg->dwCLUB_A);
		GLCLUB *pClubB = m_cClubMan.GetClub(pNetMsg->dwCLUB_B);

		if ( pClubA && pClubB )
		{
			float fLIFE_P_A(0), fLIFE_P_B(0);

			if ( pNetMsg->emEND_A==EMCONFRONT_END_CWIN )
			{
				fLIFE_P_A = (float) GLCONST_CHAR::nCONFRONT_CLB_WIN_LP;
			}
			else if ( pNetMsg->emEND_A==EMCONFRONT_END_CLOSS )
			{
				fLIFE_P_A = (float) GLCONST_CHAR::nCONFRONT_CLB_LOSS_LP;
			}

			if ( pNetMsg->emEND_B==EMCONFRONT_END_CWIN )
			{
				fLIFE_P_B = (float) GLCONST_CHAR::nCONFRONT_CLB_WIN_LP;
			}
			else if ( pNetMsg->emEND_B==EMCONFRONT_END_CLOSS )
			{
				fLIFE_P_B = (float) GLCONST_CHAR::nCONFRONT_CLB_LOSS_LP;
			}

			{
				CLUBCONFT_ITER pos = pClubA->m_setConftSTART.begin();
				CLUBCONFT_ITER end = pClubA->m_setConftSTART.end();
				for ( ; pos!=end; ++pos )
				{
					PGLCHAR pCHAR = GetCharID ( *pos );
					if ( pCHAR )
					{
						pCHAR->ReceiveLivingPoint ( (int)fLIFE_P_A );

						if ( pCHAR->m_sCONFTING.IsCONFRONTING() )
						{
							if ( pCHAR->m_sCONFTING.emTYPE==EMCONFT_GUILD )
							{
								// 대련종료 후 무적시간 설정
								pCHAR->DoConftStateRestore();
								pCHAR->m_sCONFTING.RESET();
								pCHAR->m_sCONFTING.SETPOWERFULTIME ( 10.0f ); 
							}
						}
					}
				}
			}

			{
				CLUBCONFT_ITER pos = pClubB->m_setConftSTART.begin();
				CLUBCONFT_ITER end = pClubB->m_setConftSTART.end();
				for ( ; pos!=end; ++pos )
				{
					PGLCHAR pCHAR = GetCharID ( *pos );
					if ( pCHAR )
					{
						pCHAR->ReceiveLivingPoint ( (int)fLIFE_P_B );

						if ( pCHAR->m_sCONFTING.IsCONFRONTING() )
						{
							if ( pCHAR->m_sCONFTING.emTYPE==EMCONFT_GUILD )
							{
								// 대련종료 후 무적시간 설정
								pCHAR->DoConftStateRestore();
								pCHAR->m_sCONFTING.RESET();
								pCHAR->m_sCONFTING.SETPOWERFULTIME ( 10.0f ); 
							}
						}
					}
				}
			}
		}

		if ( pClubA )
		{
			//	Note : 클럽 대련 종료를 클라이언트들에 알림.
			//
			GLMSG::SNETPC_CONFRONTPTY_END2_CLT NetMsgClt;
			NetMsgClt.emEND = pNetMsg->emEND_A;
			
			GLGaeaServer::GetInstance().SENDTOCLUBCLIENT ( pClubA->m_dwID, &NetMsgClt );

			if ( pNetMsg->emEND_A==EMCONFRONT_END_PWIN || pNetMsg->emEND_A==EMCONFRONT_END_PLOSS )
			{
				GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgBrd;
				NetMsgBrd.emEND = pNetMsg->emEND_A;

				CLUBMEMBERS_ITER pos = pClubA->m_mapMembers.begin();
				CLUBMEMBERS_ITER end = pClubA->m_mapMembers.end();
				for ( ; pos!=end; ++pos )
				{
					PGLCHAR pCHAR = GetCharID ( (*pos).first );
					if( !pCHAR ) continue;

					NetMsgBrd.dwGaeaID = pCHAR->m_dwGaeaID;
					pCHAR->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
				}
			}

			pClubA->CLEARCONFT();
		}

		if ( pClubB )
		{
			//	Note : 클럽 대련 종료를 클라이언트들에 알림.
			//
			GLMSG::SNETPC_CONFRONTPTY_END2_CLT NetMsgClt;
			NetMsgClt.emEND = pNetMsg->emEND_B;
			
			GLGaeaServer::GetInstance().SENDTOCLUBCLIENT ( pClubB->m_dwID, &NetMsgClt );

			if ( pNetMsg->emEND_B==EMCONFRONT_END_PWIN || pNetMsg->emEND_B==EMCONFRONT_END_PLOSS )
			{
				GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgBrd;
				NetMsgBrd.emEND = pNetMsg->emEND_B;

				CLUBMEMBERS_ITER pos = pClubB->m_mapMembers.begin();
				CLUBMEMBERS_ITER end = pClubB->m_mapMembers.end();
				for ( ; pos!=end; ++pos )
				{
					PGLCHAR pCHAR = GetCharID ( (*pos).first );
					if( !pCHAR ) continue;

					NetMsgBrd.dwGaeaID = pCHAR->m_dwGaeaID;
					pCHAR->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
				}
			}

			pClubB->CLEARCONFT();
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::ServerClubDeathMatchReady ( GLMSG::SNET_CLUB_DEATHMATCH_READY_FLD *pNetMsg )
{
	GLClubDeathMatchFieldMan::GetInstance().ReadyBattle ( pNetMsg->dwID );

	return TRUE;}

BOOL GLGaeaServer::ServerClubDeathMatchStart ( GLMSG::SNET_CLUB_DEATHMATCH_START_FLD *pNetMsg )
{
	m_bClubDMStarted = true;
	GLClubDeathMatchFieldMan::GetInstance().BeginBattle ( pNetMsg->dwID );

	return TRUE;
}

BOOL GLGaeaServer::ServerClubDeathMatchEnd ( GLMSG::SNET_CLUB_DEATHMATCH_END_FLD *pNetMsg )
{
	m_bClubDMStarted = false;
	GLClubDeathMatchFieldMan::GetInstance().EndBattle ( pNetMsg->dwID );

	return TRUE;
}

BOOL GLGaeaServer::ServerClubBattleStart ( GLMSG::SNET_CLUB_BATTLE_START_FLD *pNetMsg )
{
	m_bClubBattleStarted = true;
	GLGuidanceFieldMan::GetInstance().BeginBattle ( pNetMsg->dwID );

	return TRUE;
}

BOOL GLGaeaServer::ServerClubBattleEnd ( GLMSG::SNET_CLUB_BATTLE_END_FLD *pNetMsg )
{
	m_bClubBattleStarted = false;
	GLGuidanceFieldMan::GetInstance().EndBattle ( pNetMsg->dwID );

	return TRUE;
}

BOOL GLGaeaServer::ServerClubCertify ( GLMSG::SNET_CLUB_CERTIFIED_FLD *pNetMsg )
{
	GLGuidanceFieldMan::GetInstance().ChangeGuidClub ( pNetMsg->dwID, pNetMsg->dwCLUBID );

	return TRUE;
}

BOOL GLGaeaServer::ServerClubGuidCommission ( GLMSG::SNET_CLUB_GUID_COMMISSION_FLD *pNetMsg )
{
	GLGuidanceFieldMan::GetInstance().ChangeCommission ( pNetMsg->dwID, pNetMsg->fCommission );

	return TRUE;
}

BOOL GLGaeaServer::ServerClubNotice ( GLMSG::SNET_CLUB_NOTICE_FLD *pNetMsg )
{
	GLCLUB* pCLUB = m_cClubMan.GetClub ( pNetMsg->dwCLUB_ID );
	if ( !pCLUB )		return FALSE;

	StringCchCopy ( pCLUB->m_szNotice, EMCLUB_NOTICE_LEN+1, pNetMsg->szNotice );

	//	Note : 클럽의 공지를 필드 서버에 반영.
	GLMSG::SNET_CLUB_NOTICE_CLT NetMsgClt;
	StringCchCopy ( NetMsgClt.szNotice, EMCLUB_NOTICE_LEN+1, pNetMsg->szNotice );
	GLGaeaServer::GetInstance().SENDTOCLUBCLIENT ( pCLUB->m_dwID, &NetMsgClt );

	return TRUE;
}

BOOL GLGaeaServer::ServerClubStorageGetDB ( GLMSG::SNET_CLUB_STORAGE_GET_DB *pNetMsg )
{
	if ( !pNetMsg->m_pStream )						return FALSE;

	GLCLUB *pCLUB(NULL);
	PGLCHAR pCHAR(NULL);

	pCLUB = GetClubMan().GetClub ( pNetMsg->m_dwClubID );
	if ( !pCLUB )									goto  _END;

	pCHAR = GetCharID ( pNetMsg->m_dwCharID );
	if ( !pCHAR )									goto _END;

	if ( pCHAR->m_dwGuild != pCLUB->m_dwID )		goto _END;
	if ( pCHAR->m_dwCharID != pCLUB->m_dwMasterID )	goto _END;

	pCLUB->m_bVALID_STORAGE = TRUE;
	pCLUB->m_lnStorageMoney = pNetMsg->m_lnStorageMoney;
	pCLUB->SETSTORAGE_BYBUF ( *pNetMsg->m_pStream );

	pCHAR->MsgClubStorage ();

_END:
	SAFE_DELETE(pNetMsg->m_pStream);

	return TRUE;
}

BOOL GLGaeaServer::ServerClubInComeDn ( DWORD dwClientID, GLMSG::SNET_CLUB_INCOME_DN *pNetMsg )
{
	if ( dwClientID != m_dwAgentSlot )				return FALSE;

	GLCLUB *pCLUB = GetClubMan().GetClub ( pNetMsg->dwClubID );
	if ( !pCLUB )									return FALSE;

	PGLCHAR pCHAR = GetCharID ( pCLUB->m_dwMasterID );
	if ( !pCHAR )									return FALSE;
	if ( !pCLUB->m_bVALID_STORAGE )					return FALSE;

	pCLUB->m_lnStorageMoney += pNetMsg->lnInComeMoney;

	GLITEMLMT::GetInstance().ReqMoneyExc (	ID_CLUB, 0, 
											ID_CLUB, pNetMsg->dwClubID, 
											pNetMsg->lnInComeMoney, 
											EMITEM_ROUTE_CLUBINCOME );

	CDebugSet::ToFile ( "club_income_gaea.txt", "ServerClubInComeDn, ClubID[%u], MasterID[%u], IncomeMoney[%I64d]",
						pNetMsg->dwClubID, pCLUB->m_dwMasterID, pNetMsg->lnInComeMoney );

	//	Note : 결과 전송.
	//GLMSG::SNET_CLUB_INCOME_FB NetMsgFB;
	//NetMsgFB.dwClubID = pCLUB->m_dwID;
	//NetMsgFB.lnInComeMoney = pNetMsg->lnInComeMoney;
	//SENDTOAGENT ( &NetMsgFB );

	//	Note : 클럽장에게 통보.
	GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY	NetMsgClt;
	NetMsgClt.lnMoney = pCLUB->m_lnStorageMoney;
	SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgClt );

	return TRUE;
}

BOOL GLGaeaServer::ServerClubAllianceAddFld ( GLMSG::SNET_CLUB_ALLIANCE_ADD_FLD *pNetMsg )
{
	GLCLUB *pCLUB_CHIEF = GetClubMan().GetClub ( pNetMsg->dwChiefClubID );
	if ( !pCLUB_CHIEF )									return FALSE;

	GLCLUB *pCLUB_INDIAN = GetClubMan().GetClub ( pNetMsg->dwIndianClubID );
	if ( !pCLUB_INDIAN )								return FALSE;

	//	동맹 결성 및 동맹 클럽 추가.
	pCLUB_CHIEF->m_dwAlliance = pCLUB_CHIEF->m_dwID;
	pCLUB_INDIAN->m_dwAlliance = pCLUB_CHIEF->m_dwID;
	
	pCLUB_INDIAN->m_dwAllianceBattleWin = pCLUB_CHIEF->m_dwAllianceBattleWin;
	pCLUB_INDIAN->m_dwAllianceBattleLose = pCLUB_CHIEF->m_dwAllianceBattleLose;
	pCLUB_INDIAN->m_dwAllianceBattleDraw = pCLUB_CHIEF->m_dwAllianceBattleDraw;

	pCLUB_CHIEF->AddAlliance ( pCLUB_CHIEF->m_dwID, pCLUB_CHIEF->m_szName );
	pCLUB_CHIEF->AddAlliance ( pCLUB_INDIAN->m_dwID, pCLUB_INDIAN->m_szName );
	//pCLUB_INDIAN->AddAlliance ( pCLUB_CHIEF->m_dwID, pCLUB_CHIEF->m_szName );
	//pCLUB_INDIAN->AddAlliance ( pCLUB_INDIAN->m_dwID, pCLUB_INDIAN->m_szName );

/*
	if (pCLUB_CHIEF->AddAlliance ( pCLUB_CHIEF->m_dwID, pCLUB_CHIEF->m_szName ) == true)
	{
		if (pCLUB_CHIEF->AddAlliance ( pCLUB_INDIAN->m_dwID, pCLUB_INDIAN->m_szName ) == true)
		{
			//	추가되는 클럽에 동맹 설정.
			pCLUB_INDIAN->m_dwAlliance = pCLUB_CHIEF->m_dwID;
			return TRUE;
		} // if (pCLUB_CHIEF->AddAlliance ( pCLUB_INDIAN->m_dwID, pCLUB_INDIAN->m_szName ) == true)
		else
		{
			return FALSE;
		}
	} // if (pCLUB_CHIEF->AddAlliance ( pCLUB_CHIEF->m_dwID, pCLUB_CHIEF->m_szName ) == true)
	else
	{
		return FALSE;
	}
*/
	return TRUE;
}

BOOL GLGaeaServer::ServerClubAllianceDel ( GLMSG::SNET_CLUB_ALLIANCE_DEL_FLD *pNetMsg )
{
	GLCLUB *pCLUB_CHIEF = GetClubMan().GetClub ( pNetMsg->dwAlliance );
	if ( !pCLUB_CHIEF )									return FALSE;

	GLCLUB *pCLUB_INDIAN = GetClubMan().GetClub ( pNetMsg->dwDelClubID );
	if ( !pCLUB_INDIAN )								return FALSE;

	CLUB_ALLIANCE_ITER del = pCLUB_CHIEF->m_setAlliance.find ( GLCLUBALLIANCE(pNetMsg->dwDelClubID,"") );
	if ( pCLUB_CHIEF->m_setAlliance.end()==del )		return FALSE;

	//	동맹에서 제외.
	pCLUB_CHIEF->m_setAlliance.erase ( del );

	//	제외되는 클럽 정보 갱신.
	pCLUB_INDIAN->ClearAlliance();

	return TRUE;
}

BOOL GLGaeaServer::ServerClubAllianceDis ( GLMSG::SNET_CLUB_ALLIANCE_DIS_FLD *pNetMsg )
{
	GLCLUB *pCLUB_CHIEF = GetClubMan().GetClub ( pNetMsg->dwChiefClubID );
	if ( !pCLUB_CHIEF )									return FALSE;

	//	동맹들에게 동맹 해체 알림.
	CLUB_ALLIANCE_ITER pos = pCLUB_CHIEF->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pCLUB_CHIEF->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBALLIANCE &sALLIANCE = *pos;
		if ( sALLIANCE.m_dwID==pCLUB_CHIEF->m_dwID )	continue;	//	클럽장은 나중에 삭제.

		GLCLUB *pCLUB_IND = GetClubMan().GetClub ( sALLIANCE.m_dwID );
		if ( !pCLUB_IND )	continue;

		pCLUB_IND->ClearAlliance();
	}

	//	클럽장의 연합 정보 삭제.
	pCLUB_CHIEF->ClearAlliance();

	return TRUE;
}

BOOL GLGaeaServer::ServerClubBattleBeginFld ( GLMSG::SNET_CLUB_BATTLE_BEGIN_FLD *pNetMsg )
{
	GLCLUB *pCLUB_CHIEF = GetClubMan().GetClub ( pNetMsg->dwChiefClubID );
	if ( !pCLUB_CHIEF )									return FALSE;

	GLCLUB *pCLUB_INDIAN = GetClubMan().GetClub ( pNetMsg->dwIndianClubID );
	if ( !pCLUB_INDIAN )								return FALSE;

	GLCLUBBATTLE sClubBattle;
	
	sClubBattle.m_tStartTime = pNetMsg->tStartTime;
	sClubBattle.m_tEndTime = pNetMsg->tEndTime;
	sClubBattle.m_bAlliance = pNetMsg->bAlliance;

	if( pNetMsg->bAlliance )
	{
		sClubBattle.m_dwCLUBID = pNetMsg->dwIndianClubID;
		StringCchCopy ( sClubBattle.m_szClubName, CHAR_SZNAME, pCLUB_INDIAN->m_szName );

		CLUB_ALLIANCE_ITER pos = pCLUB_CHIEF->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pCLUB_CHIEF->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = *pos;

			GLCLUB *pCLUB = GetClubMan().GetClub ( sALLIANCE.m_dwID );
			if ( !pCLUB ) continue;

			pCLUB->ADDBATTLECLUB( sClubBattle );
		}

		
		sClubBattle.m_dwCLUBID = pNetMsg->dwChiefClubID;
		StringCchCopy ( sClubBattle.m_szClubName, CHAR_SZNAME, pCLUB_CHIEF->m_szName );	

		pos = pCLUB_INDIAN->m_setAlliance.begin();
		end = pCLUB_INDIAN->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = *pos;

			GLCLUB *pCLUB = GetClubMan().GetClub ( sALLIANCE.m_dwID );
			if ( !pCLUB ) continue;

			pCLUB->ADDBATTLECLUB( sClubBattle );
		}

	}
	else
	{
		sClubBattle.m_dwCLUBID = pNetMsg->dwIndianClubID;
		StringCchCopy ( sClubBattle.m_szClubName, CHAR_SZNAME, pCLUB_INDIAN->m_szName );
		pCLUB_CHIEF->ADDBATTLECLUB( sClubBattle );

		sClubBattle.m_dwCLUBID = pNetMsg->dwChiefClubID;
		StringCchCopy ( sClubBattle.m_szClubName, CHAR_SZNAME, pCLUB_CHIEF->m_szName );	
		pCLUB_INDIAN->ADDBATTLECLUB( sClubBattle );
	}	

	return TRUE;
}

BOOL GLGaeaServer::ServerClubBattleOverFld ( GLMSG::SNET_CLUB_BATTLE_OVER_FLD *pNetMsg )
{
	GLCLUB *pCLUB_CHIEF = GetClubMan().GetClub ( pNetMsg->dwWinClubID );
	if ( !pCLUB_CHIEF )									return FALSE;

	GLCLUB *pCLUB_INDIAN = GetClubMan().GetClub ( pNetMsg->dwLoseClubID );
	if ( !pCLUB_INDIAN )								return FALSE;

	GLCLUBBATTLE* pClubBattle = pCLUB_CHIEF->GetClubBattle( pNetMsg->dwLoseClubID );
	if ( !pClubBattle ) return FALSE;

	if ( pClubBattle->m_bAlliance ) 
	{

		// 클럽 배틀 관련 적대 관계 정리
		DelPlayHostileAllianceBattle( pNetMsg->dwWinClubID, pNetMsg->dwLoseClubID );

		// 자신 클럽
		CLUB_ALLIANCE_ITER pos = pCLUB_CHIEF->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pCLUB_CHIEF->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = *pos;
			GLCLUB *pCLUB = GetClubMan().GetClub ( sALLIANCE.m_dwID );
			if ( !pCLUB ) continue;		
		
			switch( pNetMsg->emFB )
			{
				case EMCLUB_BATTLE_OVER_DRAW:	pCLUB->m_dwAllianceBattleDraw++;			break;
				case EMCLUB_BATTLE_OVER_WIN:	pCLUB->m_dwAllianceBattleWin++;		break;
				case EMCLUB_BATTLE_OVER_LOSE:										break;
				case EMCLUB_BATTLE_OVER_ARMISTICE:	pCLUB->m_dwAllianceBattleDraw++;	break;
				case EMCLUB_BATTLE_OVER_SUBMISSION:	pCLUB->m_dwAllianceBattleWin++;	break;
				case EMCLUB_BATTLE_OVER_TARSUBMISSION:								break;
			}

			pCLUB->DELBATTLECLUB( pNetMsg->dwLoseClubID );
		}


		// 자신 클럽
		pos = pCLUB_INDIAN->m_setAlliance.begin();
		end = pCLUB_INDIAN->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = *pos;
			GLCLUB *pCLUB = GetClubMan().GetClub ( sALLIANCE.m_dwID );
			if ( !pCLUB ) continue;		
		
			switch( pNetMsg->emFB )
			{
				case EMCLUB_BATTLE_OVER_DRAW:	pCLUB->m_dwAllianceBattleDraw++;			break;
				case EMCLUB_BATTLE_OVER_WIN:	pCLUB->m_dwAllianceBattleLose++;		break;
				case EMCLUB_BATTLE_OVER_LOSE:										break;
				case EMCLUB_BATTLE_OVER_ARMISTICE:	pCLUB->m_dwAllianceBattleDraw++;	break;
				case EMCLUB_BATTLE_OVER_SUBMISSION:	pCLUB->m_dwAllianceBattleLose++;	break;
				case EMCLUB_BATTLE_OVER_TARSUBMISSION:								break;
			}

			pCLUB->DELBATTLECLUB( pNetMsg->dwWinClubID );
		}


	}
	else
	{
		switch( pNetMsg->emFB )
		{
			case EMCLUB_BATTLE_OVER_DRAW:
				{
					pCLUB_CHIEF->m_dwBattleDraw++;
					pCLUB_INDIAN->m_dwBattleDraw++;
				}
				break;
			case EMCLUB_BATTLE_OVER_WIN:
				{
					pCLUB_CHIEF->m_dwBattleWin++;
					pCLUB_INDIAN->m_dwBattleLose++;
				}
				break;
			case EMCLUB_BATTLE_OVER_LOSE:			
				break;
			case EMCLUB_BATTLE_OVER_ARMISTICE:
				{
					pCLUB_CHIEF->m_dwBattleDraw++;
					pCLUB_INDIAN->m_dwBattleDraw++;
				}
				break;
			case EMCLUB_BATTLE_OVER_SUBMISSION:
				{
					pCLUB_CHIEF->m_dwBattleWin++;
					pCLUB_INDIAN->m_dwBattleLose++;
				}
				break;
			case EMCLUB_BATTLE_OVER_TARSUBMISSION:
				break;
		}


		// 클럽 배틀 관련 적대 관계 정리
		DelPlayHostileClubBattle( pNetMsg->dwWinClubID, pNetMsg->dwLoseClubID );

		pCLUB_CHIEF->DELBATTLECLUB( pNetMsg->dwLoseClubID );
		pCLUB_INDIAN->DELBATTLECLUB( pNetMsg->dwWinClubID );	

	}


	return TRUE;

}

BOOL GLGaeaServer::ServerClubBattleKillUpdate( GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_FLD* pNetMsg )
{
	GLCLUB *pMyClub = GetClubMan().GetClub ( pNetMsg->dwClubID );
	if ( !pMyClub )	return FALSE;

	GLCLUBBATTLE* pBattle = pMyClub->GetClubBattle( pNetMsg->dwBattleClubID );
	if ( !pBattle ) return FALSE;

	pBattle->m_wKillPoint = pNetMsg->wKillPoint;
	pBattle->m_wDeathPoint = pNetMsg->wDeathPoint;

	return TRUE;
}

BOOL GLGaeaServer::ServerClubBattleLastKillUpdate( GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD *pNetMsg )
{
	GLCLUB *pMyClub = GetClubMan().GetClub ( pNetMsg->dwClubID_A );
	if ( !pMyClub )	return FALSE;

	GLCLUB *pTarClub = GetClubMan().GetClub ( pNetMsg->dwClubID_B );
	if ( !pTarClub ) return FALSE;

	GLCLUBBATTLE* pBattle = pMyClub->GetClubBattle( pNetMsg->dwClubID_B );
	if ( !pBattle ) return FALSE;

	GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT NetMsg;
	NetMsg.dwClubID_A = pNetMsg->dwClubID_A;
	NetMsg.dwClubID_B = pNetMsg->dwClubID_B;
	NetMsg.wKillPoint = pBattle->m_wKillPointTemp;
	NetMsg.wDeathPoint = pBattle->m_wDeathPointTemp;

	SENDTOAGENT( &NetMsg );

	return TRUE;
}

BOOL GLGaeaServer::ServerLevelEventEnd( GLMSG::SNET_LEVEL_EVENT_END_FLD *pNetMsg )
{
	SNATIVEID nidMAP( pNetMsg->dwMapID );
	GLLandMan *pLandMan = GetByMapID( nidMAP );
	if( pLandMan )
	{
		SNATIVEID nidMAP( pNetMsg->dwHallMapID );

		GLMapList::FIELDMAP_ITER iter = m_MapList.find ( nidMAP.dwID );
		if( iter==m_MapList.end() ) return FALSE;

		const SMAPNODE *pMapNode = &(*iter).second;

		GLMapAxisInfo sMapAxisInfo;
		sMapAxisInfo.LoadFile ( pMapNode->strFile.c_str() );

		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.sMAPID = nidMAP;
		NetMsgAg.dwGATEID = UINT_MAX;
		
		GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			PGLCHAR pChar = pCharNode->Data;
			if( !pChar ) continue;

			int nRandNum = rand() % MAX_EVENT_POS;
			GLLEV_POS levPos = pNetMsg->levPos[nRandNum];

			D3DXVECTOR3 vPOS(0,0,0); 
			sMapAxisInfo.MapPos2MiniPos ( levPos.m_dwPOS_X, levPos.m_dwPOS_Y, vPOS.x, vPOS.z );

			NetMsgAg.vPOS = vPOS;

			SENDTOAGENT( pChar->m_dwClientID, &NetMsgAg );

			// 팻을 제거해준다
			DropOutPET ( pChar->m_dwPetGUID, true, true );
			DropOutSummon ( pChar->m_dwSummonGUID, true );
			SaveVehicle( pChar->m_dwClientID, pChar->m_dwGaeaID, true );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::ServerLevelEventWarning( GLMSG::SNET_LEVEL_EVENT_WARNING_FLD * pNetMsg )
{
	SNATIVEID nidMAP( pNetMsg->dwMapID );
	GLLandMan *pLandMan = GetByMapID( nidMAP );
	if( pLandMan )
	{
		GLMSG::SNET_GM_WARNING_MSG_BRD NetMsgBrd;
		NetMsgBrd.bOn = pNetMsg->bOn;

		GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			PGLCHAR pChar = pCharNode->Data;
			if( !pChar ) continue;

			SENDTOCLIENT( pChar->m_dwClientID, &NetMsgBrd );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::ServerLevelEventCountdown( GLMSG::SNET_LEVEL_EVENT_COUNTDOWN_FLD * pNetMsg )
{
	SNATIVEID nidMAP( pNetMsg->dwMapID );
	GLLandMan *pLandMan = GetByMapID( nidMAP );
	if( pLandMan )
	{
		GLMSG::SNET_GM_COUNTDOWN_MSG_BRD NetMsgBrd;
		NetMsgBrd.nCount = pNetMsg->nCount;

		GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			PGLCHAR pChar = pCharNode->Data;
			if( !pChar ) continue;

			SENDTOCLIENT( pChar->m_dwClientID, &NetMsgBrd );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestChargedItem2Inven ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_CHARGED_ITEM2_INVEN *pNetMsg )
{
	SITEM *pITEM(NULL);
	WORD wPosX(0), wPosY(0);
	SITEMCUSTOM sITEM_NEW;
	CTime cTIME = CTime::GetCurrentTime();
	MAPSHOP_ITER pos;
	GLMSG::SNET_CHARGED_ITEM_DEL NetMsgDel;
	GLMSG::SNETPC_INVEN_INSERT NetMsgInven;

	GLChar *pChar = GetChar(dwGaeaID);
	if ( !pChar )									goto _DB_RESTORE;

	if ( pNetMsg->dwUserID != pChar->GetUserID() )	goto _DB_RESTORE;

	pITEM = GLItemMan::GetInstance().GetItem ( pNetMsg->nidITEM );
	if ( !pITEM )									goto _DB_RESTORE;

	BOOL bOK = pChar->m_cInventory.FindInsrtable ( pITEM->sBasicOp.wInvenSizeX, pITEM->sBasicOp.wInvenSizeY, wPosX, wPosY );
	if ( !bOK )										goto _DB_RESTORE;

	//	아이템 발생.
	sITEM_NEW.sNativeID = pNetMsg->nidITEM;
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	//	아이템의 사용 횟수 표시. ( 소모품일 경우 x 값, 일반 물품 1 )
	sITEM_NEW.wTurnNum = pITEM->GETAPPLYNUM();

	// 아이템 에디트에서 입력한 개조 등급 적용 ( 준혁 )
	sITEM_NEW.cDAMAGE = (BYTE)pITEM->sBasicOp.wGradeAttack;
	sITEM_NEW.cDEFENSE = (BYTE)pITEM->sBasicOp.wGradeDefense;

	// 일본에서만 유료 아이템에 랜덤 옵션 적용 ( 준혁 )
#ifdef JP_PARAM
	if( sITEM_NEW.GENERATE_RANDOM_OPT() )
	{
		GLITEMLMT::GetInstance().ReqRandomItem( sITEM_NEW );
	}
#endif

	//	발생된 정보 등록.
	sITEM_NEW.cGenType = EMGEN_BILLING;
	sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
	sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
	sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, EMGEN_BILLING );

	pChar->m_cInventory.InsertItem ( sITEM_NEW, wPosX, wPosY );

	//	인밴에 아이탬 넣어주는 메시지.
	//
	SINVENITEM *pINVENITEM = pChar->m_cInventory.GetItem ( wPosX, wPosY );
	if ( !pINVENITEM ) return FALSE;
	NetMsgInven.Data = *pINVENITEM;
	GLGaeaServer::GetInstance().SENDTOCLIENT(pChar->m_dwClientID,&NetMsgInven);

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sITEM_NEW, ID_CHAR, 0, ID_CHAR, pChar->m_dwCharID, EMITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum );

	//	Note : 구입한 목록에서 제거.
	pos = pChar->m_mapCharged.find ( pNetMsg->szPurKey );
	if ( pos!=pChar->m_mapCharged.end() )
	{
		pChar->m_mapCharged.erase ( pos );
	}

	//	Note : 클라이언트의 구입된 목록 리스트에서 제거.
	//
	NetMsgDel.dwID = pNetMsg->dwInvenPos;
	SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgDel );

	return TRUE;

_DB_RESTORE:
	if ( GetDBMan() )
	{
		CPurchaseItem_Restore *pDbAction = new CPurchaseItem_Restore(pNetMsg->szPurKey);
		GetDBMan()->AddJob ( pDbAction );
	}

	return FALSE;
}

BOOL GLGaeaServer::RequestConftSPtyExp ( GLMSG::SNET_CONFT_SPTY_EXP_FLD *pNetMsg )
{
	GLCHARNODE *pCharNode = m_GaeaPCList.m_pHead;
	for ( ; pCharNode; pCharNode=pCharNode->pNext )
	{
		GLChar *pChar = pCharNode->Data;

		if ( pChar->m_wSchool != pNetMsg->wSCHOOL )	continue;

		int nExp = int ( pChar->m_sExperience.lnMax * pNetMsg->fEXP_RATE );
		pChar->ReceiveExp ( nExp, true );
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestGenItemFieldFB ( GLMSG::SNET_REQ_GENITEM_FLD_FB * pNetMsg )
{
	GLLandMan* pLandMan = GetByMapID ( pNetMsg->sMAPID );
	if ( !pLandMan )	return FALSE;

	pLandMan->RequestGenItemFieldFB ( pNetMsg );

	return TRUE;
}

// *****************************************************
// Desc: 소환서 아이템 사용 처리
// *****************************************************
BOOL GLGaeaServer::RequestInvenTeleport ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_INVEN_TELEPORT* pNetMsg )
{
	GLChar* pChar = GetChar(dwGaeaID);
	if ( !pChar )	return FALSE;

	GLMSG::SNETPC_REQ_TELEPORT_FB	NetMsgFB;

	//	케릭이 정상 상태가 아닐 경우.
	if ( !pChar->IsValidBody() )
	{
		NetMsgFB.emFB = EMREQ_TELEPORT_FB_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	대련 도중일 경우.
	if ( pChar->m_sCONFTING.IsCONFRONTING() )
	{
		NetMsgFB.emFB = EMREQ_TELEPORT_FB_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	거래중일 경우.
	if ( pChar->m_sTrade.Valid() )
	{
		NetMsgFB.emFB = EMREQ_TELEPORT_FB_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	Note : pk 등급이 살인자 등급 이상일 경우 귀환 카드의 사용을 막는다.
	//
	DWORD dwPK_LEVEL = pChar->GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL )
	{
		NetMsgFB.emFB = EMREQ_TELEPORT_FB_PK_LEVEL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	//	인벤의 아이템 확인.
	SINVENITEM *pINVENITEM = pChar->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMREQ_TELEPORT_FB_ITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pChar->CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return FALSE;

	//	아이템 정보 가져옴.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_TELEPORT_CARD || pItem->sDrugOp.emDrug!=ITEM_DRUG_CALL_TELEPORT )
	{
		NetMsgFB.emFB = EMREQ_TELEPORT_FB_ITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	SNATIVEID sMAPID(false);
	D3DXVECTOR3 vPOS(0.0f,0.0f,0.0f);


	
	sMAPID = pItem->sBasicOp.sSubID;
//	vPOS = pChar->m_vLastCallPos;	

	if ( sMAPID==SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMREQ_TELEPORT_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	GLMapList::FIELDMAP_ITER iter = m_MapList.find ( sMAPID.dwID );
	if ( iter==m_MapList.end() )
	{
		NetMsgFB.emFB = EMREQ_TELEPORT_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	Note : 직전귀환 위치 저장. ( 시귀나 학귀일 경우 )
	//
	pChar->SaveLastCall ();

	//	Note : 직전귀환 위치 DB에 저장.
	//
	CSetLastCallPos *pDbAction = new CSetLastCallPos ( pChar->m_dwCharID, pChar->m_sLastCallMapID.dwID, pChar->m_vLastCallPos );
	if ( GetDBMan() )
	{
		GetDBMan()->AddJob ( pDbAction );
	}

	const SMAPNODE *pMapNode = &(*iter).second;
	GLMapAxisInfo sMapAxisInfo;
	sMapAxisInfo.LoadFile ( pMapNode->strFile.c_str() );
	sMapAxisInfo.MapPos2MiniPos (  pItem->sBasicOp.wPosX, pItem->sBasicOp.wPosY, vPOS.x, vPOS.z );

	// 진입조건 검사
	if ( pChar->m_dwUserLvl < USER_GM3 )
	{
		EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
		const SLEVEL_REQUIRE &sRequire = pMapNode->sLEVEL_REQUIRE;
		emReqFail = sRequire.ISCOMPLETE ( pChar ); 
		if ( emReqFail != EMREQUIRE_COMPLETE )
		{
			NetMsgFB.emFB = EMREQ_TELEPORT_FB_JOINCON;
			SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			
			return FALSE;
		}
	}

	// PET
	// 맵이동시 Pet 삭제
	DropOutPET ( pChar->m_dwPetGUID, true, true );
	DropOutSummon ( pChar->m_dwSummonGUID, true );
	SaveVehicle( pChar->m_dwClientID, pChar->m_dwGaeaID, true );

	//	Note : 다른 필드 서버일 경우.
	if ( pMapNode->dwFieldSID!=m_dwFieldSvrID )
	{
		GLMSG::SNETPC_REQ_TELEPORT_AG NetMsgAg;
		NetMsgAg.sMAPID = sMAPID;
		NetMsgAg.vPOS = vPOS;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgAg );
	}
	//	Note : 같은 필드 서버일 경우.
	else
	{
		SNATIVEID sCurMapID = pChar->m_sMapID;

		BOOL bOK = RequestInvenTeleportThisSvr ( pChar, sMAPID, vPOS );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_TELEPORT_FB_FAIL;
			SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			return FALSE;
		}

		// 버프와 상태이상 제거
		if ( sCurMapID != pChar->m_sMapID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		pChar->DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		pChar->DISABLESKEFF ( i );
		}

		pChar->ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		NetMsgFB.emFB = EMREQ_TELEPORT_FB_OK;
		NetMsgFB.sMAPID = sMAPID;
		NetMsgFB.vPOS = pChar->m_vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgFB );
	}

	//	[자신에게] 인밴 아이탬 소모시킴.
	pChar->DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY, true );

	return TRUE;
}

BOOL GLGaeaServer::RequestInvenTeleportThisSvr ( GLChar* pPC, SNATIVEID sMAPID, D3DXVECTOR3 vPOS )
{
	if ( !pPC )		return FALSE;
	
	GLLandMan* pLandMan = NULL;
	D3DXVECTOR3 vStartPos;

	pLandMan = GetByMapID ( sMAPID );
	if ( !pLandMan )	return FALSE;

	vStartPos = vPOS;	

	//	Note : 자신이 본 주변 셀을 정리.
	pPC->ResetViewAround ();

	//	Note : 종전 맵에 있던 관리 노드 삭제.
	//
	if ( pPC->m_pLandNode )
		pPC->m_pLandMan->m_GlobPCList.DELNODE ( pPC->m_pLandNode );

	if ( pPC->m_pQuadNode && pPC->m_pCellNode )
		pPC->m_pQuadNode->pData->m_PCList.DELNODE ( pPC->m_pCellNode );

	//	Note : 새로운 맵 위치로 내비게이션 초기화.
	//
	pPC->SetNavi ( pLandMan->GetNavi(), vStartPos );

	//	Note : 새로운 관리 ID 부여.
	//
	pPC->m_sMapID = sMAPID;

	pPC->m_pLandMan = pLandMan;
	pPC->m_pLandNode = pLandMan->m_GlobPCList.ADDHEAD ( pPC );

	//	Note : GLLandMan의 셀에 등록하는 작업.
	//			
	//RegistChar ( pPC ); --> ( GLGaeaServer::RequestLandIn ()이 호출될 때까지 유보됨. )

	pPC->m_dwCeID = 0;
	pPC->m_pQuadNode = NULL;
	pPC->m_pCellNode = NULL;

	return TRUE;
}

// *****************************************************
// Desc: 학귀, 시귀, 직귀 카드 사용 처리
// *****************************************************
BOOL GLGaeaServer::RequestInvenRecall ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_INVEN_RECALL *pNetMsg )
{
	GLChar* pChar = GetChar(dwGaeaID);
	if ( !pChar )	return FALSE;

	GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;

	//	케릭이 정상 상태가 아닐 경우.
	if ( !pChar->IsValidBody() )
	{
		NetMsgFB.emFB = EMREQ_RECALL_FB_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	대련 도중일 경우.
	if ( pChar->m_sCONFTING.IsCONFRONTING() )
	{
		NetMsgFB.emFB = EMREQ_RECALL_FB_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	거래중일 경우.
	if ( pChar->m_sTrade.Valid() )
	{
		NetMsgFB.emFB = EMREQ_RECALL_FB_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	Note : pk 등급이 살인자 등급 이상일 경우 귀환 카드의 사용을 막는다.
	//
	DWORD dwPK_LEVEL = pChar->GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL )
	{
		NetMsgFB.emFB = EMREQ_RECALL_FB_PK_LEVEL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	//	인벤의 아이템 확인.
	SINVENITEM *pINVENITEM = pChar->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMREQ_RECALL_FB_ITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pChar->CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return FALSE;


	//	아이템 정보 가져옴.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_RECALL )
	{
		NetMsgFB.emFB = EMREQ_RECALL_FB_ITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	SNATIVEID sMAPID(false);
	DWORD dwGATEID(UINT_MAX);
	D3DXVECTOR3 vPOS(FLT_MAX,FLT_MAX,FLT_MAX);

	WORD wSchool = pChar->m_wSchool;

	switch ( pItem->sDrugOp.emDrug )
	{
	default:
		NetMsgFB.emFB = EMREQ_RECALL_FB_ITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;

	case ITEM_DRUG_CALL_SCHOOL:
		sMAPID = GLCONST_CHAR::nidSTARTMAP[wSchool];
		dwGATEID = GLCONST_CHAR::dwSTARTGATE[wSchool];
		break;

	case ITEM_DRUG_CALL_REGEN:
		sMAPID = pChar->m_sStartMapID;
		dwGATEID = pChar->m_dwStartGate;
		break;

	case ITEM_DRUG_CALL_LASTCALL:
		if ( pChar->m_sLastCallMapID==SNATIVEID(false) )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_NOTLASTCALL;
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return FALSE;
		}

		sMAPID = pChar->m_sLastCallMapID;
		dwGATEID = UINT_MAX;
		vPOS = pChar->m_vLastCallPos;
		
		//	Note : 직전 귀환 위치 리셋.
		//
		pChar->m_sLastCallMapID = SNATIVEID(false);

		GLMSG::SNETPC_UPDATE_LASTCALL NetMsg;
		NetMsg.sLastCallMapID = SNATIVEID(false);
		SENDTOCLIENT( dwClientID, &NetMsg );

		break;
	};

	if ( sMAPID==SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	GLMapList::FIELDMAP_ITER iter = m_MapList.find ( sMAPID.dwID );
	if ( iter==m_MapList.end() )
	{
		NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	Note : 직전귀환 위치 저장. ( 시귀나 학귀일 경우 )
	//
	if ( pItem->sDrugOp.emDrug==ITEM_DRUG_CALL_SCHOOL || pItem->sDrugOp.emDrug==ITEM_DRUG_CALL_REGEN )
	{
		pChar->SaveLastCall ();

		//	Note : 직전귀환 위치 DB에 저장.
		//
		CSetLastCallPos *pDbAction = new CSetLastCallPos ( pChar->m_dwCharID, pChar->m_sLastCallMapID.dwID, pChar->m_vLastCallPos );
		if ( GetDBMan() )
		{
			GetDBMan()->AddJob ( pDbAction );
		}
	}

	const SMAPNODE *pMapNode = &(*iter).second;

	// 진입조건 검사
	if ( pChar->m_dwUserLvl < USER_GM3 )
	{
		EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
		const SLEVEL_REQUIRE &sRequire = pMapNode->sLEVEL_REQUIRE;
		emReqFail = sRequire.ISCOMPLETE ( pChar ); 
		if ( emReqFail != EMREQUIRE_COMPLETE )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_JOINCON;
			SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );

			// 시귀를 사용시 실패했을경우
			// 시작위치 리셋 (각학원별 시작맵으로)
			if ( pItem->sDrugOp.emDrug == ITEM_DRUG_CALL_REGEN )
			{
				pChar->ReSetStartMap();
			}

			return FALSE;
		}

		// MapList 에 Move flag 가 설정되어 있지 않으면
		// 직전귀환카드로 이동불가능
		if ( !pMapNode->bMove && pItem->sDrugOp.emDrug == ITEM_DRUG_CALL_LASTCALL )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_IMMOVABLE;
			SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			return FALSE;
		}
	}

	//	클럽데스매치존 일때 
	if ( pMapNode->bCDMZone ) 
	{
		GLLandMan* pLandMan = GetByMapID ( sMAPID );

		if ( !pLandMan )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_JOINCON;
			SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			pChar->ReSetStartMap();
			return FALSE;
		}

		if ( GLCONST_CHAR::bPKLESS )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_JOINCON;
			SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			pChar->ReSetStartMap();
			return FALSE;
		}

		GLClubDeathMatch *pCDM = GLClubDeathMatchFieldMan::GetInstance().Find ( pLandMan->m_dwClubMapID );
		if ( pCDM && !pCDM->IsEnterMap( pChar->m_dwGuild ) )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_JOINCON;
			SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			pChar->ReSetStartMap();
			return FALSE;
		}

		GLCLUB* pClub = GLGaeaServer::GetInstance().GetClubMan().GetClub ( pChar->m_dwGuild );		
		// 클럽배틀 진행중일때는 못들어간다.
		if ( !pClub || pClub->GetAllBattleNum() > 0 || !pClub->IsMemberFlgCDM(pChar->m_dwCharID) )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_JOINCON;
			SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			pChar->ReSetStartMap();
			return FALSE;
		}
	}

	// PET
	// 맵이동시 Pet 삭제
	DropOutPET ( pChar->m_dwPetGUID, true, true );
	DropOutSummon ( pChar->m_dwSummonGUID, true );
	SaveVehicle( pChar->m_dwClientID, pChar->m_dwGaeaID, true );

	//	Note : 다른 필드 서버일 경우.
	if ( pMapNode->dwFieldSID!=m_dwFieldSvrID )
	{
		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.sMAPID = sMAPID;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPOS = vPOS;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgAg );
	}
	//	Note : 같은 필드 서버일 경우.
	else
	{
		SNATIVEID sCurMapID = pChar->m_sMapID;

		BOOL bOK = RequestInvenRecallThisSvr ( pChar, sMAPID, dwGATEID, vPOS );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
			SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			return FALSE;
		}

		// 버프와 상태이상 제거
		if ( sCurMapID != pChar->m_sMapID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		pChar->DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		pChar->DISABLESKEFF ( i );
		}

		pChar->ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.sMAPID = sMAPID;
		NetMsgFB.vPOS = pChar->m_vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgFB );
	}

	//	[자신에게] 인밴 아이탬 소모시킴.
	pChar->DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY, true );

	return TRUE;
}

// *****************************************************
// Desc: 소환요청 처리(같은 필드서버에 진입)
// *****************************************************
BOOL GLGaeaServer::RequestInvenRecallThisSvr ( GLChar* pPC, SNATIVEID sMAPID, DWORD dwGATEID, D3DXVECTOR3 vPOS )
{
	if ( !pPC )		return FALSE;
	
	GLLandMan* pLandMan = NULL;
	DxLandGateMan* pGateMan = NULL;
	PDXLANDGATE pGate = NULL;
	D3DXVECTOR3 vStartPos;

	pLandMan = GetByMapID ( sMAPID );
	if ( !pLandMan )	return FALSE;

	if ( dwGATEID!=UINT_MAX )
	{
		pGateMan = &pLandMan->GetLandGateMan ();
		pGate = pGateMan->FindLandGate ( dwGATEID );
		if ( !pGate )		return FALSE;

		vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
	}
	else
	{
		vStartPos = vPOS;
	}

	//	Note : 자신이 본 주변 셀을 정리.
	pPC->ResetViewAround ();

	//	Note : 종전 맵에 있던 관리 노드 삭제.
	//
	if ( pPC->m_pLandNode )
		pPC->m_pLandMan->m_GlobPCList.DELNODE ( pPC->m_pLandNode );

	if ( pPC->m_pQuadNode && pPC->m_pCellNode )
		pPC->m_pQuadNode->pData->m_PCList.DELNODE ( pPC->m_pCellNode );

	//	Note : 새로운 맵 위치로 내비게이션 초기화.
	//
	pPC->SetNavi ( pLandMan->GetNavi(), vStartPos );

	//	Note : 새로운 관리 ID 부여.
	//
	pPC->m_sMapID = sMAPID;

	pPC->m_vPos	  = vStartPos;

	pPC->m_pLandMan = pLandMan;
	pPC->m_pLandNode = pLandMan->m_GlobPCList.ADDHEAD ( pPC );

	//	Note : GLLandMan의 셀에 등록하는 작업.
	//			
	//RegistChar ( pPC ); --> ( GLGaeaServer::RequestLandIn ()이 호출될 때까지 유보됨. )

	pPC->m_dwCeID = 0;
	pPC->m_pQuadNode = NULL;
	pPC->m_pCellNode = NULL;

	return TRUE;
}

BOOL GLGaeaServer::RequestBus ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_BUS *pNetMsg )
{
	GLChar* pChar = GetChar(dwGaeaID);
	if ( !pChar )	return FALSE;

	GLMSG::SNETPC_REQ_BUS_FB	NetMsgFB;

	//	케릭이 정상 상태가 아닐 경우.
	if ( !pChar->IsValidBody() )
	{
		NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	대련 도중일 경우.
	if ( pChar->m_sCONFTING.IsCONFRONTING() )
	{
		NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	거래중일 경우.
	if ( pChar->m_sTrade.Valid() )
	{
		NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	Note : pk 등급이 살인자 등급 이상일 경우 버스 사용을 막는다.
	//
	//DWORD dwPK_LEVEL = pChar->GET_PK_LEVEL();
	//if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL )
	//{
	//	NetMsgFB.emFB = EMBUS_TAKE_PK_LEVEL;
	//	SENDTOCLIENT ( dwClientID, &NetMsgFB );
	//	return FALSE;
	//}

	PGLCROW pCROW(NULL);
	if ( pChar->m_pLandMan )	pCROW = pChar->m_pLandMan->GetCrow ( pNetMsg->dwNPC_ID );
	if ( !pCROW )
	{
		NetMsgFB.emFB = EMBUS_TAKE_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( !pCROW->m_pCrowData->m_sNpcTalkDlg.IsBUSSTATION() )
	{
		NetMsgFB.emFB = EMBUS_TAKE_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	인벤의 아이템 확인.
	SINVENITEM *pINVENITEM = pChar->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMBUS_TAKE_TICKET;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pChar->CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) ) return FALSE;

	//	아이템 정보 가져옴.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_TICKET )
	{
		NetMsgFB.emFB = EMBUS_TAKE_TICKET;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	SNATIVEID sMAPID(false);
	DWORD dwGATEID(UINT_MAX);

	//	Note : 정류장 id가 정확한지 검사.
	//
	SSTATION* pSTATION = GLBusStation::GetInstance().GetStation ( pNetMsg->dwSTATION_ID );
	if ( !pSTATION )
	{
		NetMsgFB.emFB = EMBUS_TAKE_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	map id.
	sMAPID.dwID = pSTATION->dwMAPID;

	//	gate id.
	dwGATEID = pSTATION->dwGATEID;


	if ( sMAPID==SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMBUS_TAKE_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	GLMapList::FIELDMAP_ITER iter = m_MapList.find ( sMAPID.dwID );
	if ( iter==m_MapList.end() )
	{
		NetMsgFB.emFB = EMBUS_TAKE_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	const SMAPNODE *pMapNode = &(*iter).second;

	// 진입조건 검사
	if ( pChar->m_dwUserLvl < USER_GM3 )
	{
		EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
		const SLEVEL_REQUIRE &sRequire = pMapNode->sLEVEL_REQUIRE;
		emReqFail = sRequire.ISCOMPLETE ( pChar ); 
		if ( emReqFail != EMREQUIRE_COMPLETE )
		{
			//	탑승 권한이 없음.
			NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return FALSE;
		}
	}

	//	[자신에게] 인밴 아이탬 소모시킴.
	pChar->DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY, true );

	//	Note : 탑승 성공 알림.
	NetMsgFB.emFB = EMBUS_TAKE_OK;
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	// PET
	// 맵이동시 Pet 삭제
	DropOutPET ( pChar->m_dwPetGUID,false,true );
	DropOutSummon ( pChar->m_dwSummonGUID, false );
	SetActiveVehicle( pChar->m_dwClientID, pChar->m_dwGaeaID, false );
	
	//	Note : 다른 필드 서버일 경우.
	if ( pMapNode->dwFieldSID!=m_dwFieldSvrID )
	{
		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.sMAPID = sMAPID;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPOS = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgAg );
	}
	else
	{
		GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
		SNATIVEID sCurMapID = pChar->m_sMapID;

		BOOL bOK = RequestInvenRecallThisSvr ( pChar, sMAPID, dwGATEID, D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX) );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			return FALSE;
		}

		// 버프와 상태이상 제거
		if ( sCurMapID != pChar->m_sMapID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		pChar->DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		pChar->DISABLESKEFF ( i );
		}

		pChar->ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.sMAPID = sMAPID;
		NetMsgFB.vPOS = pChar->m_vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgFB );
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestTaxi ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_TAXI *pNetMsg )
{
	GLChar* pChar = GetChar(dwGaeaID);
	if ( !pChar )	return FALSE;

	GLMSG::SNETPC_REQ_TAXI_FB	NetMsgFB;

	//	케릭이 정상 상태가 아닐 경우.
	if ( !pChar->IsValidBody() )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	대련 도중일 경우.
	if ( pChar->m_sCONFTING.IsCONFRONTING() )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	거래중일 경우.
	if ( pChar->m_sTrade.Valid() )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}
	
	// 정류장 검사
	STAXI_MAP* pTaxiMap = GLTaxiStation::GetInstance().GetTaxiMap ( pNetMsg->dwSelectMap );
	if ( !pTaxiMap )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	// 정류장 검사
	STAXI_STATION* pStation = pTaxiMap->GetStation( pNetMsg->dwSelectStop );
	if ( !pStation )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_STATIONFAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}


	//	인벤의 아이템 확인.
	SINVENITEM *pINVENITEM = pChar->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_TICKET;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pChar->CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return FALSE;


	//	아이템 정보 가져옴.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_TAXI_CARD )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_TICKET;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	소지금액 검사
	DWORD dwCurMapID = pChar->m_sMapID.dwID;
	LONGLONG lnCharge = GLTaxiStation::GetInstance().GetBasicCharge();

	if ( pStation->dwMAPID != dwCurMapID ) lnCharge += pStation->dwMapCharge;
	
	volatile float fSHOP_RATE = pChar->GET_PK_SHOP2BUY();
	volatile float fSHOP_2RATE = fSHOP_RATE + pChar->m_pLandMan->m_fCommissionRate;
	volatile float fSHOP_2RATE_C = fSHOP_2RATE * 0.01f;
	lnCharge = (LONGLONG)(lnCharge * fSHOP_2RATE_C);

	if ( pChar->m_lnMoney < lnCharge )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_MONEY;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return S_FALSE;
	}

	SNATIVEID sMAPID(false);
	DWORD dwNPCID(UINT_MAX);

	//	map id.
	sMAPID.dwID = pStation->dwMAPID;

	//	gate id.
	dwNPCID = pStation->dwNPCID;

	// 맵검사
	if ( sMAPID==SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	GLMapList::FIELDMAP_ITER iter = m_MapList.find ( sMAPID.dwID );
	if ( iter==m_MapList.end() )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	const SMAPNODE *pMapNode = &(*iter).second;

	// 진입조건 검사
	if ( pChar->m_dwUserLvl < USER_GM3 )
	{
		EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
		const SLEVEL_REQUIRE &sRequire = pMapNode->sLEVEL_REQUIRE;
		emReqFail = sRequire.ISCOMPLETE ( pChar ); 
		if ( emReqFail != EMREQUIRE_COMPLETE )
		{
			//	탑승 권한이 없음.
			NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return FALSE;
		}
	}

	//	NPC 검사

	PGLCROW pCROW(NULL);
	D3DXVECTOR3 vPOS(FLT_MAX,FLT_MAX,FLT_MAX);
	DWORD dwGATEID( UINT_MAX );

	if ( pMapNode->dwFieldSID==m_dwFieldSvrID )
	{
		GLLandMan* pToLandMan = GetByMapID( sMAPID );

		if ( pToLandMan )
		{
			SNATIVEID sNpcID( dwNPCID );
			if ( pStation->wPosX != 0 && pStation->wPosY != 0 ) 
				pCROW = pToLandMan->GetCrowID ( sNpcID, pStation->wPosX, pStation->wPosY );
			else 
				pCROW = pToLandMan->GetCrowID ( sNpcID );
			
			if ( !pCROW )
			{
				NetMsgFB.emFB = EMTAXI_TAKE_NPCFAIL;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			
			D3DXVECTOR3 vDIR = pCROW->m_vDir;
			vDIR = vDIR * 20.0f;
			
			vPOS = pCROW->GetPosition();
			vPOS += vDIR;
		}	
	}
	else
	{
		vPOS = pNetMsg->vPos;
	}



	//	[자신에게] 인밴 아이탬 소모시킴.
	pChar->DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY, true );

	//	[자신에게] 사용금액을 소모시킴
	pChar->CheckMoneyUpdate( pChar->m_lnMoney, lnCharge, FALSE, "Taxi Charge Money" );
	pChar->m_bMoneyUpdate = TRUE;
	pChar->m_lnMoney -= lnCharge;

	//	Note : 탑승 성공 알림.
	NetMsgFB.emFB = EMTAXI_TAKE_OK;
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	GLMSG::SNETPC_UPDATE_MONEY NetMsgMoney;
	NetMsgMoney.lnMoney = pChar->m_lnMoney;
	SENDTOCLIENT ( dwClientID, &NetMsgMoney );
	
	// PET
	// 맵이동시 Pet 삭제
	DropOutPET ( pChar->m_dwPetGUID,false,true );
	DropOutSummon ( pChar->m_dwSummonGUID, false );
	SetActiveVehicle( pChar->m_dwClientID, pChar->m_dwGaeaID, false );
	
	//	Note : 다른 필드 서버일 경우.
	if ( pMapNode->dwFieldSID!=m_dwFieldSvrID )
	{
		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.sMAPID = sMAPID;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPOS = vPOS;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgAg );
	}
	else
	{
		GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
		SNATIVEID sCurMapID = pChar->m_sMapID;

		BOOL bOK = RequestInvenRecallThisSvr ( pChar, sMAPID, dwGATEID, vPOS );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
			return FALSE;
		}

		// 버프와 상태이상 제거
		if ( sCurMapID != pChar->m_sMapID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )		pChar->DISABLEBLOW ( i );
			for ( int i=0; i<SKILLFACT_SIZE; ++i )		pChar->DISABLESKEFF ( i );
		}

		pChar->ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.sMAPID = sMAPID;
		NetMsgFB.vPOS = pChar->m_vPos;
		GLGaeaServer::GetInstance().SENDTOAGENT ( pChar->m_dwClientID, &NetMsgFB );
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestTaxiNpcPos ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_TAXI_NPCPOS *pNetMsg )
{
	GLMSG::SNETPC_REQ_TAXI_NPCPOS_FB NetMsgFB;

	// 정류장 검사
	STAXI_MAP* pTaxiMap = GLTaxiStation::GetInstance().GetTaxiMap ( pNetMsg->dwSelectMap );
	if ( !pTaxiMap ) 		return FALSE;

	// 정류장 검사
	STAXI_STATION* pStation = pTaxiMap->GetStation( pNetMsg->dwSelectStop );
	if ( !pStation ) return FALSE;

	

	SNATIVEID sMAPID(pStation->dwMAPID);

	// 맵검사
	if ( sMAPID==SNATIVEID(false) )	return FALSE;

	GLLandMan* pToLandMan = GetByMapID( sMAPID );
	
	if ( !pToLandMan )	return FALSE;
	
	SNATIVEID sNpcID( pStation->dwNPCID );
	PGLCROW pCROW;

	if ( pStation->wPosX != 0 && pStation->wPosY != 0 ) 
		pCROW = pToLandMan->GetCrowID ( sNpcID, pStation->wPosX, pStation->wPosY );
	else 
		pCROW = pToLandMan->GetCrowID ( sNpcID );


	if ( !pCROW )
	{
		NetMsgFB.vPos = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		NetMsgFB.dwGaeaID = pNetMsg->dwGaeaID;
		SENDTOAGENT( dwClientID, &NetMsgFB );
		return FALSE;
	}

	D3DXVECTOR3 vPos = pCROW->GetPosition();
	D3DXVECTOR3 vDir = pCROW->m_vDir;
	vDir = vDir * 20.0f;
	vPos += vDir;

	NetMsgFB.dwSelectMap = pNetMsg->dwSelectMap;
	NetMsgFB.dwSelectStop = pNetMsg->dwSelectStop;
	NetMsgFB.wPosX = pNetMsg->wPosX;
	NetMsgFB.wPosY = pNetMsg->wPosY;
	NetMsgFB.dwGaeaID = pNetMsg->dwGaeaID;
	NetMsgFB.vPos = vPos;

	SENDTOAGENT( dwClientID, &NetMsgFB );


	return TRUE;

}

// *****************************************************
// Desc: 친구이동시 친구위치 반환 (Agent->Field)
// *****************************************************
BOOL GLGaeaServer::Request2FriendCK ( GLMSG::SNETPC_2_FRIEND_CK *pNetMsg )
{
	GLMSG::SNETPC_2_FRIEND_AG NetMsgAG;
	NetMsgAG.dwCOMMAND_CHARID = pNetMsg->dwCOMMAND_CHARID;
	NetMsgAG.dwFRIEND_CHARID = pNetMsg->dwFRIEND_CHARID;
	NetMsgAG.wItemPosX = pNetMsg->wItemPosX;
	NetMsgAG.wItemPosY = pNetMsg->wItemPosY;

	PGLCHAR pFRIEND = GetCharID ( pNetMsg->dwFRIEND_CHARID );
	if ( !pFRIEND )
	{
		NetMsgAG.emFB = EM2FRIEND_FB_FRIEND_CONDITION;
		SENDTOAGENT ( &NetMsgAG );
		return FALSE;
	}

	if ( !pFRIEND->IsValidBody() || pFRIEND->m_pLandMan==NULL )
	{
		NetMsgAG.emFB = EM2FRIEND_FB_FRIEND_CONDITION;
		SENDTOAGENT ( &NetMsgAG );
		return FALSE;
	}

	GLMapList::FIELDMAP_ITER iter = m_MapList.find ( pFRIEND->m_sMapID.dwID );
	if ( iter==m_MapList.end() )
	{
		NetMsgAG.emFB = EM2FRIEND_FB_FAIL;
		SENDTOAGENT ( &NetMsgAG );
		return FALSE;
	}

	// MapList 에 Move flag 가 설정되어 있지 않으면
	// 친구연결카드로 이동불가능
	SMAPNODE sMapNode = (*iter).second;
	if ( !sMapNode.bMove )
	{
		NetMsgAG.emFB = EM2FRIEND_FB_IMMOVABLE;
		SENDTOAGENT ( &NetMsgAG );
		return FALSE;
	}
		
	BOOL bCoB(FALSE);
	DWORD dwCoB(0);
	D3DXVECTOR3 vCoB(0,0,0);
	
	D3DXVECTOR3 vec2POS = pFRIEND->m_vPos;
	D3DXMATRIX matRotY;
	for ( float fAngle=0.0f; fAngle<D3DX_PI*2; fAngle += D3DX_PI/4.0f )
	{
		D3DXVECTOR3 vecDIR(1,0,0);
		D3DXMatrixRotationY ( &matRotY, fAngle );

		D3DXVec3TransformCoord ( &vecDIR, &vecDIR, &matRotY );
		D3DXVECTOR3 vPos = pFRIEND->m_vPos + vecDIR*10.0f;

		NavigationMesh* pNavi = pFRIEND->m_pLandMan->GetNavi();

		pNavi->IsCollision ( D3DXVECTOR3(vPos.x,vPos.y+20.0f,vPos.z), D3DXVECTOR3(vPos.x,vPos.y-30.0f,vPos.z), vCoB, dwCoB, bCoB );
		if ( bCoB )
		{
			vec2POS = vCoB;
			break;
		}
	}

	//	Note : 위치 반환.
	NetMsgAG.emFB = EM2FRIEND_FB_OK;
	NetMsgAG.sFriendMapID = pFRIEND->m_sMapID;
	NetMsgAG.vFriendPos = vec2POS;
	SENDTOAGENT ( &NetMsgAG );

	return TRUE;
}

// *****************************************************
// Desc: 이동할 케릭터의 위치 반환
// *****************************************************
BOOL GLGaeaServer::RequestMove2CharPos ( DWORD dwClientID, GLMSG::SNETPC_GM_MOVE2CHAR_POS *pNetMsg )
{
	if ( dwClientID != m_dwAgentSlot ) return FALSE;

	// 에이전트로 회신할 메세지
	GLMSG::SNETPC_GM_MOVE2CHAR_AG NetMsgAG;
	NetMsgAG.dwCOMMAND_CHARID = pNetMsg->dwCOMMAND_CHARID;
	NetMsgAG.dwTO_CHARID = pNetMsg->dwTO_CHARID;

	// 이동할 케릭터 유효성
	PGLCHAR pTO_CHAR = GetCharID ( pNetMsg->dwTO_CHARID );
	if ( !pTO_CHAR )
	{
		NetMsgAG.emFB = EMGM_MOVE2CHAR_FB_TO_CONDITION;
		SENDTOAGENT ( &NetMsgAG );
		return FALSE;
	}

	// 이동할 맵의 유효성
	if ( pTO_CHAR->m_pLandMan==NULL )
	{
		NetMsgAG.emFB = EMGM_MOVE2CHAR_FB_TO_CONDITION;
		SENDTOAGENT ( &NetMsgAG );
		return FALSE;
	}

	BOOL bCoB(FALSE);
	DWORD dwCoB(0);
	D3DXVECTOR3 vCoB(0,0,0);
	
	D3DXVECTOR3 vec2POS = pTO_CHAR->m_vPos;
	D3DXMATRIX matRotY;
	for ( float fAngle=0.0f; fAngle<D3DX_PI*2; fAngle += D3DX_PI/4.0f )
	{
		D3DXVECTOR3 vecDIR(1,0,0);
		D3DXMatrixRotationY ( &matRotY, fAngle );

		D3DXVec3TransformCoord ( &vecDIR, &vecDIR, &matRotY );
		D3DXVECTOR3 vPos = pTO_CHAR->m_vPos + vecDIR*10.0f;

		NavigationMesh* pNavi = pTO_CHAR->m_pLandMan->GetNavi();

		pNavi->IsCollision ( D3DXVECTOR3(vPos.x,vPos.y+20.0f,vPos.z), D3DXVECTOR3(vPos.x,vPos.y-30.0f,vPos.z), vCoB, dwCoB, bCoB );
		if ( bCoB )
		{
			vec2POS = vCoB;
			break;
		}
	}

	//	Note : 위치 반환.
	NetMsgAG.emFB = EMGM_MOVE2CHAR_FB_OK;
	NetMsgAG.sToMapID = pTO_CHAR->m_sMapID;
	NetMsgAG.vToPos = vec2POS;
	SENDTOAGENT ( &NetMsgAG );

	return TRUE;
}

BOOL GLGaeaServer::RequestClubRank2Fld ( GLMSG::SNET_CLUB_RANK_2FLD *pNetMsg )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( pNetMsg->dwClubID );
	if ( !pCLUB )								return FALSE;
	if ( pNetMsg->dwRank > (GLCONST_CHAR::dwMAX_CLUBRANK-1) )	return FALSE;

	pCLUB->m_dwRank = pNetMsg->dwRank;

	GLMSG::SNET_CLUB_RANK_2CLT NetMsg;
	NetMsg.dwRank = pCLUB->m_dwRank;

	//	Note : 클라이언트들에게 알림.
	//
	PGLCHAR pMASTER = GetCharID ( pCLUB->m_dwMasterID );
	if ( pMASTER )
	{
		LONGLONG lnPAY = GLCONST_CHAR::sCLUBRANK[NetMsg.dwRank].m_dwPay;

		pMASTER->CheckMoneyUpdate( pMASTER->m_lnMoney, lnPAY, FALSE, "Club Rank Pay." );
		pMASTER->m_bMoneyUpdate = TRUE;

		if ( pMASTER->m_lnMoney >= lnPAY )
		{
			pMASTER->m_lnMoney -= lnPAY;
		}
		else
		{
			pMASTER->m_lnMoney = 0;
		}


		//	Note : 금액 로그.
		//
		GLITEMLMT::GetInstance().ReqMoneyExc( ID_CHAR, pMASTER->m_dwCharID, ID_CLUB, 0, -lnPAY, EMITEM_ROUTE_DELETE );
		GLITEMLMT::GetInstance().ReqMoneyExc( ID_CHAR, pMASTER->m_dwCharID, ID_CHAR, 0, pMASTER->m_lnMoney, EMITEM_ROUTE_CHAR );

		//	금액 변화 클라이언트에 알려줌.
		GLMSG::SNETPC_UPDATE_MONEY NetMsg;
		NetMsg.lnMoney = pMASTER->m_lnMoney;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pMASTER->m_dwClientID, &NetMsg );

		//	Note : 마스터에게 전송.
		SENDTOCLIENT ( pMASTER->m_dwClientID, &NetMsg );

		//	Note : 클럽 랭크 변화 로그 기록.
		GLITEMLMT::GetInstance().ReqAction
		(
			pMASTER->m_dwCharID,		//	당사자.
			EMLOGACT_CLUB_LEV_UP,		//	행위.
			ID_CLUB, pCLUB->m_dwID,		//	상대방.
			0,							//	exp.
			0,							//	bright.
			0,							//	life.
			(int)-lnPAY					//	money.
		);
	}

	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	PGLCHAR pMEMBER = NULL;
	for ( ; pos!=end; ++pos )
	{
		pMEMBER = GetCharID ( (*pos).first );
		if ( pMEMBER )
		{
			//	Note : 멤버에게.
			SENDTOCLIENT ( pMEMBER->m_dwClientID, &NetMsg );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestClubInfo ( GLMSG::SNET_CLUB_INFO_2FLD *pNetMsg )
{
	//	Note : 클럽 정보.
	//
	GLCLUB cCLUB;
	cCLUB.m_dwID = pNetMsg->dwClubID;
	StringCchCopy ( cCLUB.m_szName, CHAR_SZNAME, pNetMsg->szClubName );
	
	cCLUB.m_dwMasterID = pNetMsg->dwMasterID;
	StringCchCopy ( cCLUB.m_szMasterName, CHAR_SZNAME, pNetMsg->szMasterName );
	cCLUB.m_dwCDCertifior = pNetMsg->dwCDCertifior;

	cCLUB.m_dwRank = pNetMsg->dwRank;
	cCLUB.m_dwMarkVER = pNetMsg->dwMarkVER;

	cCLUB.m_tOrganize = pNetMsg->tOrganize;
	cCLUB.m_tDissolution = pNetMsg->tDissolution;

	for ( DWORD i=0; i<pNetMsg->dwMemberNum; ++i )
	{
		GLCLUBMEMBER cMEMBER;
		cMEMBER.dwID = pNetMsg->aMembers[i];
		StringCchCopy ( cMEMBER.m_szName, CHAR_SZNAME, pNetMsg->szMemberName[i] );
		cCLUB.m_mapMembers.insert ( std::make_pair(cMEMBER.dwID,cMEMBER) );
	}

	m_cClubMan.AddClub ( cCLUB );

	//	Note : 클럽 마스터, 멤버원에게 클럽 상세 정보 전송.
	//
	GLMSG::SNET_CLUB_INFO_2CLT NetMsg2Clt;
	NetMsg2Clt.dwClubID = cCLUB.m_dwID;
	StringCchCopy ( NetMsg2Clt.szClubName, CHAR_SZNAME, cCLUB.m_szName );

	NetMsg2Clt.dwMasterID = cCLUB.m_dwMasterID;
	StringCchCopy ( NetMsg2Clt.szMasterName, CHAR_SZNAME, cCLUB.m_szMasterName );
	NetMsg2Clt.dwCDCertifior = cCLUB.m_dwCDCertifior;

	NetMsg2Clt.dwMarkVER = cCLUB.m_dwMarkVER;
	NetMsg2Clt.dwRank = cCLUB.m_dwRank;

	NetMsg2Clt.tOrganize = cCLUB.m_tOrganize;
	NetMsg2Clt.tDissolution = cCLUB.m_tDissolution;

	GLMSG::SNET_CLUB_MEMBER_2CLT NetMsgMember;
	for ( int i=0; i<EMMAXCLUBMEMBER_NEW; ++i )
	{
		if ( pNetMsg->aMembers[i]==0 )	continue;
		NetMsgMember.ADDMEMBER ( pNetMsg->aMembers[i], NULL, pNetMsg->szMemberName[i] );
	}

	//	Note : 마스터, 멤버 주변 사람들에게 클럽 간략 정보 전송.
	GLMSG::SNET_CLUB_INFO_BRD NetMsgBrd;
	NetMsgBrd.dwClubID = cCLUB.m_dwID;
	NetMsgBrd.dwMarkVer = cCLUB.m_dwMarkVER;

	//	Note : 마스터, 멤버에게 설정.
	//
	PGLCHAR pMASTER = GetCharID ( cCLUB.m_dwMasterID );
	if ( pMASTER )
	{
		//	Note : 마스터에게 전송.
		pMASTER->m_dwGuild = cCLUB.m_dwID;

		StringCchCopy ( pMASTER->m_szNick, CHAR_SZNAME, cCLUB.m_szName );
		StringCchCopy ( NetMsg2Clt.szNickName, CHAR_SZNAME, pMASTER->m_szNick );
		SENDTOCLIENT ( pMASTER->m_dwClientID, &NetMsg2Clt );

		SENDTOCLIENT ( pMASTER->m_dwClientID, &NetMsgMember );

		//	Note : 마스터의 주변 사람에게.
		NetMsgBrd.dwGaeaID = pMASTER->m_dwGaeaID;
		StringCchCopy ( NetMsgBrd.szNickName, CHAR_SZNAME, pMASTER->m_szNick );
		pMASTER->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
	}

	PGLCHAR pMEMBER = NULL;
	for ( DWORD i=0; i<pNetMsg->dwMemberNum; ++i )
	{
		pMEMBER = GetCharID ( pNetMsg->aMembers[i] );
		if ( pMEMBER )
		{
			//	Note : 멤버에게.
			pMEMBER->m_dwGuild = cCLUB.m_dwID;

			StringCchCopy ( pMEMBER->m_szNick, CHAR_SZNAME, cCLUB.m_szName );
			StringCchCopy ( NetMsg2Clt.szNickName, CHAR_SZNAME, pMEMBER->m_szNick );
			SENDTOCLIENT ( pMEMBER->m_dwClientID, &NetMsg2Clt );

			SENDTOCLIENT ( pMEMBER->m_dwClientID, &NetMsgMember );

			//	Note : 멤버원 주변에게.
			NetMsgBrd.dwGaeaID = pMEMBER->m_dwGaeaID;
			StringCchCopy ( NetMsgBrd.szNickName, CHAR_SZNAME, pMEMBER->m_szNick );
			pMEMBER->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
		}
	}

	return TRUE;
}

// *****************************************************
// Desc: 클럽정보 갱신
// *****************************************************
BOOL GLGaeaServer::RequestClubDissolution2Fld ( GLMSG::SNET_CLUB_DISSOLUTION_2FLD *pNetMsg )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( pNetMsg->dwClubID );
	if ( !pCLUB )		return FALSE;

	pCLUB->DoDissolution ( pNetMsg->tDissolution );

	//	Note : 클럽 마스터, 멤버원에게 클럽 상세 정보 전송.
	//
	GLMSG::SNET_CLUB_INFO_DISSOLUTION NetMsg2Clt;
	NetMsg2Clt.tDissolution = pCLUB->m_tDissolution;

	//	Note : 마스터, 멤버에게 설정
	//
	PGLCHAR pMASTER = GetCharID ( pCLUB->m_dwMasterID );
	CTime tDisTime( pNetMsg->tDissolution );
	if ( pMASTER )
	{
		// 년도가 1970년이면 클럽 해체 취소.
		if ( tDisTime.GetYear() != 1970 )
		{
			//	Note : 경험치를 감산시킨다.
			__int64 nDECEXP;
			nDECEXP = __int64 ( GLOGICEX::GLDIE_DECEXP(pMASTER->GETLEVEL()) * 0.01f * pMASTER->GET_LEVELUP_EXP() );
			nDECEXP = __int64 ( nDECEXP * pMASTER->GET_PK_DECEXP_RATE()*0.01f );
			pMASTER->SET_DECEXP ( nDECEXP );
			
			// 죽었을 경우만 갱신되게 하기 위하여 미리 저장해놓고 다시 저장한다.
			LONGLONG nReExp = pMASTER->m_lnReExp;
			
			pMASTER->ReBirthDecExp ();
			
			pMASTER->m_lnReExp = nReExp;
		}

		//	Note : 마스터에게 전송.
		SENDTOCLIENT ( pMASTER->m_dwClientID, &NetMsg2Clt );
	}

	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	PGLCHAR pMEMBER = NULL;
	for ( ; pos!=end; ++pos )
	{
		pMEMBER = GetCharID ( (*pos).first );
		if ( pMEMBER )
		{
			//	Note : 멤버에게.
			SENDTOCLIENT ( pMEMBER->m_dwClientID, &NetMsg2Clt );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestClubDel2Fld ( GLMSG::SNET_CLUB_DEL_2FLD *pNetMsg )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( pNetMsg->dwClubID );
	if ( !pCLUB )		return FALSE;
	
	PGLCHAR pCHAR = GetCharID ( pCLUB->m_dwMasterID );
	if ( pCHAR )
	{
		//	Note : 클럽 정보 제거.
		pCHAR->m_dwGuild = CLUB_NULL;

		//	Note : 클라이언트에 클럽 정보 리셋.
		//
		GLMSG::SNET_CLUB_DEL_2CLT NetMsgClt;
		SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgClt );

		//	Note : 주변 사람에게 알림.
		GLMSG::SNET_CLUB_DEL_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = pCHAR->m_dwGaeaID;
		pCHAR->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgBrd );
	}
	
	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	PGLCHAR pMember = NULL;
	GLMSG::SNET_CLUB_DEL_2CLT NetMsgClt;
	GLMSG::SNET_CLUB_DEL_BRD NetMsgBrd;
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBMEMBER &cMEMBER = (*pos).second;
		pMember = GetCharID ( cMEMBER.dwID );
		if ( !pMember )	
		{
			continue;
		}
		else
		{
			//	Note : 클럽 정보 제거.
			pMember->m_dwGuild = CLUB_NULL;

			//	Note : 클라이언트에 클럽 정보 리셋.
			//
			SENDTOCLIENT ( pMember->m_dwClientID, &NetMsgClt );

			//	Note : 주변 사람에게 알림.			
			NetMsgBrd.dwGaeaID = pMember->m_dwGaeaID;
			pMember->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgBrd );
		}
	}

	//	Note : 클럽 삭제.
	m_cClubMan.DelClub ( pNetMsg->dwClubID );

	return TRUE;
}

BOOL GLGaeaServer::RequestClubAdd2Fld ( GLMSG::SNET_CLUB_MEMBER_ADD_2FLD *pNetMsg )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( pNetMsg->dwClubID );
	if ( !pCLUB )		return FALSE;

	pCLUB->AddMember ( pNetMsg->dwMember, pNetMsg->szMember, NULL );

	GLMSG::SNET_CLUB_MEMBER_2CLT NetMsgMember;
	NetMsgMember.ADDMEMBER ( pNetMsg->dwMember, NULL, pNetMsg->szMember );

	PGLCHAR pMASTER = GetCharID ( pCLUB->m_dwMasterID );
	if ( pMASTER )
	{
		//	Note : 클라이언트에 신규 멤버 정보 전송.
		SENDTOCLIENT ( pMASTER->m_dwClientID, &NetMsgMember );
	}
	
	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	PGLCHAR pMember = NULL;
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBMEMBER &cMEMBER = (*pos).second;
		if ( cMEMBER.dwID==pNetMsg->dwMember )	continue;		//	자기 자신의 경우 제외.

		pMember = GetCharID ( cMEMBER.dwID );
		if ( !pMember )							continue;		//	멤버원이 존제 할 경우만.

		//	Note : 클라이언트에 신규 멤버 정보 전송.
		SENDTOCLIENT ( pMember->m_dwClientID, &NetMsgMember );
	}

	//	Note : 신규로 참가하게된 멤버 설정.
	PGLCHAR pNEW_MEMBER = GetCharID ( pNetMsg->dwMember );
	if ( pNEW_MEMBER )
	{
		pNEW_MEMBER->m_dwGuild = pCLUB->m_dwID;

		//	Note : 클럽 정보 전송.
		pNEW_MEMBER->MsgClubInfo ();

		//	Note : 주변 사람에게 알림.
		GLMSG::SNET_CLUB_INFO_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = pNEW_MEMBER->m_dwGaeaID;
		NetMsgBrd.dwClubID = pNEW_MEMBER->m_dwGuild;
		NetMsgBrd.dwMarkVer = pCLUB->m_dwMarkVER;
		StringCchCopy( NetMsgBrd.szNickName, CHAR_SZNAME, pNEW_MEMBER->m_szNick );
		pNEW_MEMBER->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgBrd );
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestClubDel ( GLMSG::SNET_CLUB_MEMBER_DEL_2FLD *pNetMsg )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( pNetMsg->dwClub );
	if ( !pCLUB )		return FALSE;

	//	Note : 클럽 멤버에서 삭제.
	pCLUB->DelMember ( pNetMsg->dwMember );

	PGLCHAR pCHAR = GetCharID ( pNetMsg->dwMember );
	if ( pCHAR )
	{
		//	클럽 정보 제거.
		pCHAR->m_dwGuild = CLUB_NULL;

		// 클럽탈퇴시간 갱신
		pCHAR->m_tSECEDE = pNetMsg->tSECEDE;

		//	자신에게 클럽 정보 알림.
		GLMSG::SNET_CLUB_DEL_2CLT NetMsgInfo;
		SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgInfo );

		//	Note : 주변 사람에게 알림.
		GLMSG::SNET_CLUB_DEL_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = pCHAR->m_dwGaeaID;
		pCHAR->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgBrd );
	}


	//	Note : 삭제 메시지.
	GLMSG::SNET_CLUB_MEMBER_DEL_2CLT NetMsgClt;
	NetMsgClt.dwClub = pNetMsg->dwClub;
	NetMsgClt.dwMember = pNetMsg->dwMember;

	PGLCHAR pMASTER = GetCharID ( pCLUB->m_dwMasterID );
	if ( pMASTER )
	{
		//	Note : 클라이언트에 삭제 멤버 전송.
		SENDTOCLIENT ( pMASTER->m_dwClientID, &NetMsgClt );
	}
	
	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	PGLCHAR pMember = NULL;
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBMEMBER &cMEMBER = (*pos).second;
		if ( cMEMBER.dwID==pNetMsg->dwMember )	continue;		//	자기 자신의 경우 제외.

		pMember = GetCharID ( cMEMBER.dwID );
		if ( !pMember )
		{
			continue;		//	멤버원이 존재 할 경우만.
		}
		else
		{
			//	Note : 클라이언트에 삭제 멤버 전송.
			SENDTOCLIENT ( pMember->m_dwClientID, &NetMsgClt );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestClubMarkChange ( GLMSG::SNET_CLUB_MARK_CHANGE_2FLD *pNetMsg )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( pNetMsg->dwClub );
	if ( !pCLUB )		return FALSE;

	pCLUB->m_dwMarkVER = pNetMsg->dwMarkVER;
	memcpy ( pCLUB->m_aryMark, pNetMsg->aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY );

	GLMSG::SNET_CLUB_INFO_MARK_BRD NetMsgBrd;
	NetMsgBrd.dwMarkVer = pCLUB->m_dwMarkVER;
	
	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBMEMBER &cMEMBER = (*pos).second;
		PGLCHAR pMember = GetCharID ( cMEMBER.dwID );
		if ( !pMember )	continue;

		NetMsgBrd.dwGaeaID = pMember->m_dwGaeaID;

		//	Note : 주변 사람에게 알림.
		pMember->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgBrd );
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestClubSubMaster ( GLMSG::SNET_CLUB_SUBMASTER_FLD *pNetMsg )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( pNetMsg->dwClubID );
	if ( !pCLUB )		return FALSE;

	pCLUB->SetMemberFlag ( pNetMsg->dwCharID, pNetMsg->dwFlags );
	pCLUB->m_dwCDCertifior = pNetMsg->dwCDCertifior;

	DWORD dwCHARID = pNetMsg->dwCharID;
	PGLCHAR pSUB = GetCharID ( dwCHARID );
	if ( pSUB )
	{
		GLMSG::SNETPC_UPDATE_FLAGS_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = pSUB->m_dwGaeaID;
		if ( pCLUB->IsMemberFlgCDCertify(dwCHARID) )	NetMsgBrd.dwFLAGS |= SDROP_CHAR::CLUB_CD;
		else											NetMsgBrd.dwFLAGS = NULL;

		pSUB->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgBrd );
	}

	dwCHARID = pCLUB->m_dwMasterID;
	PGLCHAR pMASTER = GetCharID ( dwCHARID );
	if ( pMASTER )
	{
		GLMSG::SNETPC_UPDATE_FLAGS_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = pMASTER->m_dwGaeaID;
		if ( pCLUB->IsMemberFlgCDCertify(dwCHARID) )	NetMsgBrd.dwFLAGS |= SDROP_CHAR::CLUB_CD;
		else											NetMsgBrd.dwFLAGS = NULL;

		pMASTER->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgBrd );
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestClubAuthority ( GLMSG::SNET_CLUB_AUTHORITY_FLD *pNetMsg )
{
	GLCLUB *pCLUB = m_cClubMan.GetClub ( pNetMsg->dwClubID );
	if ( !pCLUB )		return FALSE;
	
	if ( pNetMsg->dwCharID == GAEAID_NULL ) return FALSE;

	GLCLUBMEMBER* pMember = pCLUB->GetMember( pNetMsg->dwCharID );
	if ( !pMember ) return FALSE;

	pCLUB->m_dwMasterID = pNetMsg->dwCharID;
	StringCchCopy( pCLUB->m_szMasterName, CHAR_SZNAME, pMember->m_szName );

	GLMSG::SNET_CLUB_AUTHORITY_CLT	NetMsgClt;
	GLMSG::SNET_CLUB_AUTHORITY_BRD	NetMsgBrd;

	NetMsgClt.dwCharID = pCLUB->m_dwMasterID;
	NetMsgBrd.dwCharID = pCLUB->m_dwMasterID;

	CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
	CLUBMEMBERS_ITER end = pCLUB->m_mapMembers.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBMEMBER &cMEMBER = (*pos).second;
		PGLCHAR pMember = GetCharID ( cMEMBER.dwID );
		if ( !pMember )	continue;

		SENDTOCLIENT ( pMember->m_dwClientID, &NetMsgClt );

		NetMsgBrd.dwGaeaID = pMember->m_dwGaeaID;

		//	Note : 주변 사람에게 알림.
		pMember->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgBrd );
	}

	return TRUE;
}

BOOL GLGaeaServer::ReqClubMemberRename ( GLMSG::SNET_CLUB_MEMBER_RENAME_FLD* pNetMsg )
{
	//GLCLUB* pCLUB = m_cClubMan.GetClub ( pNetMsg->dwClubID );
	GLCLUB* pCLUB = GetClubMan().GetClub ( pNetMsg->dwClubID );

	// 클럽에 가입했으면 클럽상의 이름 변경
	if ( pCLUB )
	{
		GLCLUBMEMBER* pClubMember = pCLUB->GetMember ( pNetMsg->dwCharID );

		if ( pCLUB->IsMaster ( pNetMsg->dwCharID ) )
		{
			StringCchCopy ( pCLUB->m_szMasterName , CHAR_SZNAME, pNetMsg->szName );
		}
		if ( pClubMember )
		{
			StringCchCopy ( pClubMember->m_szName , CHAR_SZNAME, pNetMsg->szName );
		}
	}

	PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetCharID ( pNetMsg->dwCharID );
	if ( pCHAR )
	{
		GLGaeaServer::GetInstance().ChangeNameMap ( pCHAR, pCHAR->m_szName, pNetMsg->szName );
	}
    
	return TRUE;
}

void GLGaeaServer::SetServerCrashTime ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_SET_SERVER_CRASHTIME_FLD* pNetMsgFld ) 
{
	if ( dwClientID != m_dwAgentSlot ) return;
	m_dwServerCrashTime = pNetMsgFld->dwSec; 
}

void GLGaeaServer::SetDelayMilliSec ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_SET_SERVER_DELAYTIME_FLD* pNetMsgFld )
{
	if ( dwClientID != m_dwAgentSlot ) return;
	m_fDelayMilliSec = pNetMsgFld->dwMillisec; 
}

void GLGaeaServer::SetSkipPacketNum ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_SET_SERVER_SKIPPACKET_FLD* pNetMsgFld )
{
	if ( dwClientID != m_dwAgentSlot ) return;
	m_wSkipPacketNum = pNetMsgFld->wSkipPacketNum;
}

void GLGaeaServer::CheckMarketState( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_MARKETCHECK *pNetMsg )
{
	if ( dwGaeaID == GAEAID_NULL )		return;

	PGLCHAR pChar = GetChar ( dwGaeaID );

	if( pChar == NULL )
	{
		GetConsoleMsg()->Write ( LOG_TEXT_CONSOLE, "%%%% Could not found Character..%d %%%%", dwGaeaID );
	}
}

BOOL GLGaeaServer::RequestNpcRecall( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_INVEN_NPC_RECALL *pNetMsg )
{
		
	GLChar* pChar = GetChar(dwGaeaID);
	if ( !pChar )	return FALSE;

	GLMSG::SNET_INVEN_NPC_RECALL_FB	NetMsgFB;


	//	인벤의 아이템 확인.
	SINVENITEM *pINVENITEM = pChar->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_NOITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pChar->CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return FALSE;


	//	아이템 정보 가져옴.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_NPC_RECALL )
	{
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_NOITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}
	
	SNATIVEID sID(pItem->sBasicOp.sSubID );
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sID );
	if ( !pCrowData )
	{		
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_NPC;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	GLLandMan* pLandMan = GetByMapID ( pChar->m_sMapID );
	if( !pLandMan )
	{
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	DWORD dwMobID = pLandMan->DropCrow ( sID, pChar->m_vPos.x, pChar->m_vPos.z );

	if ( dwMobID == UINT_MAX ) 
	{
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_POS;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	PGLCROW pCrow = pLandMan->GetCrow( dwMobID );
    if ( !pCrow ) 
	{
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_POS;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	pCrow->m_bNpcRecall = true;
	pCrow->m_dwCallCharID = pChar->m_dwCharID;
	pCrow->m_dwCallUserID = pChar->GetUserID();
	pCrow->m_tGenTime = CTime::GetCurrentTime().GetTime();

	//	[자신에게] 인밴 아이탬 소모시킴.
	pChar->DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY, true );


	NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_OK;
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	return TRUE;
}

BOOL GLGaeaServer::RequestNpcCommission( GLMSG::SNET_INVEN_NPC_COMMISSION_FB* pNetMsg )
{
	if ( pNetMsg->emFB == EMREQ_NPC_COMMISSION_CHAR )
	{
		GLChar* pChar = GLGaeaServer::GetInstance().GetCharID( pNetMsg->dwCharID );
		if ( !pChar ) return FALSE;
		
		pChar->UpdateNpcCommission( pNetMsg->lnCommission );

	}
	else if ( pNetMsg->emFB == EMREQ_NPC_COMMISSION_DB )
	{
		//	DB에 창고머니 UPDATE
		SaveNpcCommissionDB( pNetMsg->dwCharID, pNetMsg->dwUserID, pNetMsg->lnCommission );	
	}

	return TRUE;
}

//	Note : 메시지 처리는 GLGaeaServer::GetInstance().FrameMove () 함수부
//		밖에서 처리하는걸 원칙으로 한다.
//
HRESULT GLGaeaServer::MsgProcess ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	static WORD wCounter(0);

	switch ( nmg->nType )
	{
	case NET_MSG_CHAT:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		ChatMsgProc ( nmg, dwClientID, dwGaeaID );
		break;

	case NET_MSG_CHAT_LOUDSPEAKER:

	case NET_MSG_GCTRL_REQ_READY:

	case NET_MSG_GCTRL_ACTSTATE:
	case NET_MSG_GCTRL_MOVESTATE:
	case NET_MSG_GCTRL_GOTO:
	case NET_MSG_VIETNAM_TIME_REQ:
	case NET_MSG_GCTRL_ATTACK:
	case NET_MSG_GCTRL_ATTACK_CANCEL:
	
	case NET_MSG_GCTRL_REQ_FIELD_TO_INVEN:
	case NET_MSG_GCTRL_REQ_FIELD_TO_HOLD:

	case NET_MSG_GCTRL_REQ_INVEN_TO_HOLD:

	case NET_MSG_GCTRL_REQ_VNGAIN_TO_HOLD:
	case NET_MSG_GCTRL_REQ_VNGAIN_EX_HOLD:
	case NET_MSG_GCTRL_REQ_HOLD_TO_VNGAIN:
	case NET_MSG_GCTRL_REQ_VNGAIN_INVEN_RESET:
	case NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN:

	case NET_MSG_GCTRL_REQ_INVEN_EX_HOLD:
	case NET_MSG_GCTRL_REQ_INVEN_TO_SLOT:

	case NET_MSG_GCTRL_REQ_INVEN_SPLIT:

	case NET_MSG_GCTRL_REQ_SLOT_TO_HOLD:
	case NET_MSG_GCTRL_REQ_SLOT_EX_HOLD:

	case NET_MSG_GCTRL_REQ_HOLD_TO_FIELD:
	case NET_MSG_GCTRL_REQ_HOLD_TO_INVEN:
	case NET_MSG_GCTRL_REQ_HOLD_TO_SLOT:
	case NET_MSG_GCTRL_REQ_SLOT_CHANGE:

	case NET_MSG_GCTRL_REQ_BUY_FROM_NPC:
	case NET_MSG_GCTRL_REQ_SALE_TO_NPC:

	case NET_MSG_GCTRL_REQ_REBIRTH:
	case NET_MSG_GCTRL_REQ_LEVELUP:
	case NET_MSG_GCTRL_REQ_STATSUP:

	case NET_MSG_GCTRL_FIRECRACKER:
	case NET_MSG_GCTRL_REQ_INVENDRUG:
	case NET_MSG_GCTRL_INVEN_BOXOPEN:
	case NET_MSG_GCTRL_INVEN_DISGUISE:
	case NET_MSG_GCTRL_INVEN_CLEANSER:

	case NET_MSG_GCTRL_CHARGED_ITEM_GET:
	case NET_MSG_GCTRL_GET_CHARGEDITEM_FROMDB:
	
	case NET_MSG_GCTRL_REQ_LEARNSKILL:
	case NET_MSG_GCTRL_REQ_SKILLUP:

	case NET_MSG_GCTRL_REQ_SKILL:
	case NET_MSG_GCTRL_SKILL_CANCEL:

	case NET_MSG_GCTRL_REQ_SKILLQ_ACTIVE:
	case NET_MSG_GCTRL_REQ_SKILLQ_SET:
	case NET_MSG_GCTRL_REQ_SKILLQ_RESET:

	case NET_MSG_GCTRL_REQ_ACTIONQ_SET:
	case NET_MSG_GCTRL_REQ_ACTIONQ_RESET:

	case NET_MSG_GCTRL_REQ_GETSTORAGE:
	case NET_MSG_GCTRL_REQ_STORAGEDRUG:
	case NET_MSG_GCTRL_REQ_STORAGESKILL:

	case NET_MSG_GCTRL_REQ_STORAGE_TO_HOLD:
	case NET_MSG_GCTRL_REQ_STORAGE_EX_HOLD:
	case NET_MSG_GCTRL_REQ_HOLD_TO_STORAGE:
	case NET_MSG_GCTRL_STORAGE_SAVE_MONEY:
	case NET_MSG_GCTRL_STORAGE_DRAW_MONEY:

	case NET_MSG_GCTRL_REQ_MONEY_TO_FIELD:
	case NET_MSG_GCTRL_REQ_STORAGE_SPLIT:
	case NET_MSG_GCTRL_INVEN_GRINDING:
	case NET_MSG_GCTRL_INVEN_RESET_SKST:

	case NET_MSG_GCTRL_REGEN_GATE:
	case NET_MSG_GCTRL_CURE:
	case NET_MSG_GCTRL_REVIVE:
	case NET_MSG_GCTRL_2_FRIEND_FLD:
	case NET_MSG_GM_MOVE2CHAR_FLD:

	case NET_MSG_GCTRL_GETEXP_RECOVERY:
	case NET_MSG_GCTRL_RECOVERY:
	case NET_MSG_GCTRL_GETEXP_RECOVERY_NPC:
	case NET_MSG_GCTRL_RECOVERY_NPC:

	case NET_MSG_GCTRL_CHARRESET:
	case NET_MSG_GCTRL_INVEN_CHARCARD:
	case NET_MSG_GCTRL_INVEN_STORAGECARD:
	case NET_MSG_GCTRL_INVEN_INVENLINE:
	case NET_MSG_GCTRL_INVEN_STORAGEOPEN:
	case NET_MSG_GCTRL_INVEN_REMODELOPEN:
	case NET_MSG_GCTRL_INVEN_GARBAGEOPEN:
	case NET_MSG_GCTRL_INVEN_STORAGECLOSE:
	case NET_MSG_GCTRL_INVEN_PREMIUMSET:
	case NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN:
	case NET_MSG_GCTRL_INVEN_DISJUNCTION:

	case NET_MSG_GCTRL_INVEN_HAIR_CHANGE:
	case NET_MSG_GCTRL_INVEN_FACE_CHANGE:
	case NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE:
	case NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE:
	case NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE:
	case NET_MSG_GCTRL_INVEN_GENDER_CHANGE:
	case NET_MSG_GCTRL_INVEN_RENAME:
	case NET_MSG_GCTRL_INVEN_RENAME_FROM_DB:

	case NET_MSG_GCTRL_INVEN_VIETNAM_INVENGET:

	case NET_MSG_GCTRL_CONFRONT_START2_FLD:
	case NET_MSG_GCTRL_CONFRONT_END2_FLD:

	case NET_MSG_GCTRL_NPC_ITEM_TRADE:

	case NET_MSG_GCTRL_REQ_GESTURE:

	case NET_MSG_GCTRL_REQ_QUEST_START:
	case NET_MSG_GCTRL_QUEST_PROG_NPCTALK:
	case NET_MSG_GCTRL_QUEST_PROG_GIVEUP:
	case NET_MSG_GCTRL_QUEST_PROG_READ:
	case NET_MSG_GCTRL_REQ_QUEST_COMPLETE:

	case NET_MSG_GCTRL_PMARKET_TITLE:
	case NET_MSG_GCTRL_PMARKET_REGITEM:
	case NET_MSG_GCTRL_PMARKET_DISITEM:
	case NET_MSG_GCTRL_PMARKET_OPEN:
	case NET_MSG_GCTRL_PMARKET_CLOSE:
	case NET_MSG_GCTRL_PMARKET_ITEM_INFO:
	case NET_MSG_GCTRL_PMARKET_BUY:

	case NET_MSG_GCTRL_CLUB_NEW:
	case NET_MSG_GCTRL_CLUB_RANK:
	case NET_MSG_GCTRL_CLUB_NEW_2FLD:
	case NET_MSG_GCTRL_CLUB_MEMBER_REQ:
	case NET_MSG_GCTRL_CLUB_MEMBER_REQ_ANS:
	case NET_MSG_GCTRL_CLUB_MEMBER_NICK:

	case NET_MSG_GCTRL_CLUB_CD_CERTIFY:
	case NET_MSG_GCTRL_CLUB_GETSTORAGE:
	case NET_MSG_GCTRL_CLUB_STORAGE_TO_HOLD:
	case NET_MSG_GCTRL_CLUB_STORAGE_EX_HOLD:
	case NET_MSG_GCTRL_CLUB_HOLD_TO_STORAGE:
	case NET_MSG_GCTRL_CLUB_STORAGE_SPLIT:
	case NET_MSG_GCTRL_CLUB_STORAGE_SAVE_MONEY:
	case NET_MSG_GCTRL_CLUB_STORAGE_DRAW_MONEY:

	case NET_MSG_GCTRL_CLUB_INCOME_RENEW:

	case NET_MSG_REBUILD_RESULT:	// ITEMREBUILD_MARK
	case NET_MSG_GCTRL_GARBAGE_RESULT:
	case NET_MSG_REBUILD_MOVE_ITEM:
	case NET_MSG_REBUILD_INPUT_MONEY:

	case NET_MSG_SMS_PHONE_NUMBER:
	case NET_MSG_SMS_PHONE_NUMBER_FROM_DB:
	case NET_MSG_SMS_SEND:
	case NET_MSG_SMS_SEND_FROM_DB:

	case NET_MSG_MGAME_ODDEVEN:

	case NET_MSG_GCTRL_SET_SERVER_DELAYTIME:
	case NET_MSG_GCTRL_SET_SERVER_SKIPPACKET:
	case NET_MSG_GCTRL_SET_SERVER_CRASHTIME:

	// PET
	case NET_MSG_PET_GETRIGHTOFITEM:
	case NET_MSG_PET_REQ_GIVEFOOD:
	case NET_MSG_PET_REQ_GETFULL_FROMDB_FB:
	case NET_MSG_PET_REQ_PETREVIVEINFO:

	// Summon
	case NET_MSG_SUMMON_REQ_SLOT_EX_HOLD:
	case NET_MSG_SUMMON_REQ_HOLD_TO_SLOT:
	case NET_MSG_SUMMON_REMOVE_SLOTITEM:

	// vehicle
	case NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD:
	case NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT:
	case NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD:
	case NET_MSG_VEHICLE_REMOVE_SLOTITEM:
	case NET_MSG_VEHICLE_REQ_GIVE_BATTERY:
	case NET_MSG_VEHICLE_REQ_GET_BATTERY_FROMDB_FB:

	case NET_MSG_GCTRL_ITEMSHOPOPEN:

	case NET_MSG_REQ_ATTEND_REWARD_FLD:
	case NET_MSG_GCTRL_INVEN_ITEM_MIX:
	case NET_MSG_REQ_GATHERING:
	case NET_MSG_REQ_GATHERING_CANCEL:
		{
			if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;

			PGLCHAR pChar = GetChar ( dwGaeaID );
			if ( !pChar )	return S_FALSE;

			if ( !m_wSkipPacketNum ) pChar->MsgProcess ( nmg );
			else
			{
				++wCounter;
				if ( wCounter < m_wSkipPacketNum ) pChar->MsgProcess ( nmg );
				else wCounter = 0;
			}
		}
		break;

	case NET_MSG_GCTRL_NPC_RECALL:
		RequestNpcRecall( dwClientID, dwGaeaID, (GLMSG::SNET_INVEN_NPC_RECALL*) nmg );
		break;

	case NET_MSG_GCTRL_NPC_COMMISSION_FB:
		RequestNpcCommission( (GLMSG::SNET_INVEN_NPC_COMMISSION_FB*) nmg );
		break;

	case NET_MSG_MARKET_CHECKTIME:
		CheckMarketState( dwClientID, dwGaeaID, (GLMSG::SNETPC_MARKETCHECK*) nmg );
		break;

	case NET_MSG_GCTRL_CHARGED_ITEM2_INVEN:
		RequestChargedItem2Inven ( dwClientID, dwGaeaID, (GLMSG::SNET_CHARGED_ITEM2_INVEN*) nmg );
		break;

	case NET_MSG_GCTRL_CONFRONTSPTY_EXP_FLD:
		RequestConftSPtyExp ( (GLMSG::SNET_CONFT_SPTY_EXP_FLD*) nmg );
		break;

	case NET_MSG_GCTRL_REQ_GATEOUT_REQ:
		RequestGateOutReq ( dwClientID, dwGaeaID, (GLMSG::SNETREQ_GATEOUT_REQ *) nmg );
		break;

	case NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ:
		RequestCreateInstantMapReq ( dwClientID, dwGaeaID, (GLMSG::SNETREQ_CREATE_INSTANT_MAP_REQ *) nmg );
		break;
		 
	case NET_MSG_GCTRL_FIELDSVR_OUT:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		RequestFieldSvrOut ( dwClientID, dwGaeaID, (GLMSG::SNETPC_FIELDSVR_OUT *) nmg );
		break;

	case NET_MSG_GCTRL_REBIRTH_OUT:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		RequestReBirthOut ( dwClientID, dwGaeaID, (GLMSG::SNETPC_REBIRTH_OUT *) nmg );
		break;

	case NET_MSG_GCTRL_REQ_GATEOUT:
		RequestMoveMapPC ( dwClientID, dwGaeaID, (GLMSG::SNETREQ_GATEOUT *) nmg );
		break;

	case NET_MSG_GCTRL_CREATE_INSTANT_MAP_FLD:
		RequestCreateInstantMap ( dwClientID, dwGaeaID, (GLMSG::SNETREQ_CREATE_INSTANT_MAP_FLD *) nmg );
		break;

	case NET_MSG_GCTRL_REQ_LANDIN:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		RequestLandIn ( dwClientID, dwGaeaID, (GLMSG::SNETREQ_LANDIN *) nmg );
		break;

	case NET_MSG_GCTRL_TRADE:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		RequestTrade ( dwClientID, dwGaeaID, (GLMSG::SNET_TRADE *) nmg );
		break;

	case NET_MSG_GCTRL_TRADE_TAR_ANS:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		RequestTradeTarAns ( dwClientID, dwGaeaID, (GLMSG::SNET_TRADE_TAR_ANS *) nmg );
		break;

	case NET_MSG_GCTRL_TRADE_MONEY:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		RequestTradeMoney ( dwClientID, dwGaeaID, (GLMSG::SNET_TRADE_MONEY *) nmg );
		break;

	case NET_MSG_GCTRL_TRADE_ITEM_REGIST:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		RequestTradeItemResist ( dwClientID, dwGaeaID, (GLMSG::SNET_TRADE_ITEM_REGIST *) nmg );
		break;

	case NET_MSG_GCTRL_TRADE_ITEM_REMOVE:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		RequestTradeItemReMove ( dwClientID, dwGaeaID, (GLMSG::SNET_TRADE_ITEM_REMOVE *) nmg );
		break;

	case NET_MSG_GCTRL_TRADE_AGREE:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		RequestTradeAgree ( dwClientID, dwGaeaID, (GLMSG::SNET_TRADE_AGREE *) nmg );
		break;

	case NET_MSG_GCTRL_TRADE_CANCEL:
		if ( dwGaeaID == GAEAID_NULL )		return E_FAIL;
		RequestTradeCancel ( dwClientID, dwGaeaID, (GLMSG::SNET_TRADE_CANCEL *) nmg );
		break;

	case NET_MSG_FIELDSVR_CHARCHK:
		RequestFieldSvrCharChk ( dwClientID, (GLMSG::SNETPC_FIELDSVR_CHARCHK *) nmg );
		break;

	case NET_MSG_REQ_MUST_LEAVE_MAP:
		RequestMustLeaveMap ( dwClientID, dwGaeaID, (GLMSG::SNETPC_REQ_MUST_LEAVE_MAP *) nmg );
		break;

	case NET_MSG_GCTRL_REQ_RECALL:
		RequestInvenRecall ( dwClientID, dwGaeaID, (GLMSG::SNETPC_REQ_INVEN_RECALL *) nmg );
		break;

	case NET_MSG_GCTRL_REQ_TELEPORT:
		RequestInvenTeleport ( dwClientID, dwGaeaID, (GLMSG::SNETPC_REQ_INVEN_TELEPORT *) nmg );
		break;

	case NET_MSG_GCTRL_REQ_BUS:
		RequestBus ( dwClientID, dwGaeaID, (GLMSG::SNETPC_REQ_BUS *) nmg );
		break;

	case NET_MSG_GCTRL_REQ_TAXI:
		RequestTaxi ( dwClientID, dwGaeaID, (GLMSG::SNETPC_REQ_TAXI *) nmg );
		break;
	
	case NET_MSG_GCTRL_REQ_TAXI_NPCPOS:
		RequestTaxiNpcPos ( dwClientID, dwGaeaID, (GLMSG::SNETPC_REQ_TAXI_NPCPOS *) nmg );
		break;

	case NET_MSG_GCTRL_2_FRIEND_CK:
		Request2FriendCK ( (GLMSG::SNETPC_2_FRIEND_CK *)nmg );
		break;

	case NET_MSG_GM_MOVE2CHAR_POS:
		RequestMove2CharPos ( dwClientID, (GLMSG::SNETPC_GM_MOVE2CHAR_POS *)nmg );
		break;

	case NET_MSG_GM_VIEWALLPLAYER_FLD_REQ:
		RequestViewAllPlayer ( (GLMSG::SNET_GM_VIEWALLPLAYER_FLD_REQ *)nmg );
		break;

	case NET_MSG_GM_SHOP_INFO_REQ:
		RequestShopInfo ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_SHOP_INFO_REQ *)nmg ); 
		break;

	case NET_MSG_GCTRL_PMARKET_SEARCH_ITEM:
		ReqSearchShopItem ( dwClientID, dwGaeaID, (GLMSG::SNETPC_PMARKET_SEARCH_ITEM *)nmg );
		break;

	case NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT_REQ:
		ReqSearchResultShopItem ( dwClientID, dwGaeaID, (GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ *)nmg );
		break;

	case NET_MSG_GCTRL_PARTY_FNEW:
	case NET_MSG_GCTRL_PARTY_ADD:
	case NET_MSG_GCTRL_PARTY_DEL:
	case NET_MSG_GCTRL_PARTY_DISSOLVE:
	case NET_MSG_GCTRL_PARTY_MBR_MOVEMAP:
	case NET_MSG_GCTRL_PARTY_MBR_RENAME_FLD:
	case NET_MSG_GCTRL_PARTY_MASTER_RENEW:
	case NET_MSG_GCTRL_PARTY_AUTHORITY:
		{
			m_cPartyFieldMan.MsgProcess ( nmg, dwClientID, dwGaeaID );
		}
		break;

	case NET_MSG_GCTRL_CLUB_RANK_2FLD:
		RequestClubRank2Fld ( (GLMSG::SNET_CLUB_RANK_2FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_INFO_2FLD:
		RequestClubInfo ( (GLMSG::SNET_CLUB_INFO_2FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_DISSOLUTION_2FLD:
		RequestClubDissolution2Fld ( (GLMSG::SNET_CLUB_DISSOLUTION_2FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_DEL_2FLD:
		RequestClubDel2Fld ( (GLMSG::SNET_CLUB_DEL_2FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_ADD_2FLD:
		RequestClubAdd2Fld ( (GLMSG::SNET_CLUB_MEMBER_ADD_2FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_DEL_2FLD:
		RequestClubDel ( (GLMSG::SNET_CLUB_MEMBER_DEL_2FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_MARK_CHANGE_2FLD:
		RequestClubMarkChange ( (GLMSG::SNET_CLUB_MARK_CHANGE_2FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_SUBMASTER_FLD:
		RequestClubSubMaster ( (GLMSG::SNET_CLUB_SUBMASTER_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_AUTHORITY_FLD:
		RequestClubAuthority( (GLMSG::SNET_CLUB_AUTHORITY_FLD *)nmg );

	case NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_FLD:
		RequestConfrontPartyChkMbr ( (GLMSG::SNETPC_CONFRONTPTY_CHECKMBR2_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CONFRONTPTY_START2_FLD:
		RequestConfrontParty ( (GLMSG::SNETPC_CONFRONTPTY_START2_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CONFRONTPTY_END2_FLD:
		RequestConfrontPartyEnd ( (GLMSG::SNETPC_CONFRONTPTY_END2_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_FLD:
		RequestConfrontClubChkMbr ( (GLMSG::SNET_CONFRONTCLB_CHECKMBR_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CONFRONTCLB_START2_FLD:
		RequestConfrontClub ( (GLMSG::SNETPC_CONFRONTCLB_START2_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CONFRONTCLB_END2_FLD:
		RequestConfrontClubEnd ( (GLMSG::SNETPC_CONFRONTCLB_END2_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_DEATHMATCH_READY_FLD:
		ServerClubDeathMatchReady ( (GLMSG::SNET_CLUB_DEATHMATCH_READY_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_DEATHMATCH_START_FLD:
		ServerClubDeathMatchStart ( (GLMSG::SNET_CLUB_DEATHMATCH_START_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_DEATHMATCH_END_FLD:
		ServerClubDeathMatchEnd ( (GLMSG::SNET_CLUB_DEATHMATCH_END_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_BATTLE_START_FLD:
		ServerClubBattleStart ( (GLMSG::SNET_CLUB_BATTLE_START_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_BATTLE_END_FLD:
		ServerClubBattleEnd ( (GLMSG::SNET_CLUB_BATTLE_END_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_CERTIFIED_FLD:
		ServerClubCertify ( (GLMSG::SNET_CLUB_CERTIFIED_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_COMMISSION_FLD:
		ServerClubGuidCommission ( (GLMSG::SNET_CLUB_GUID_COMMISSION_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_NOTICE_FLD:
		ServerClubNotice ( (GLMSG::SNET_CLUB_NOTICE_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_STORAGE_GET_DB:
		ServerClubStorageGetDB ( (GLMSG::SNET_CLUB_STORAGE_GET_DB *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_INCOME_DN:
		ServerClubInComeDn ( dwClientID, (GLMSG::SNET_CLUB_INCOME_DN *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_FLD:
		ServerClubAllianceAddFld ( (GLMSG::SNET_CLUB_ALLIANCE_ADD_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FLD:
		ServerClubAllianceDel ( (GLMSG::SNET_CLUB_ALLIANCE_DEL_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FLD:
		ServerClubAllianceDis ( (GLMSG::SNET_CLUB_ALLIANCE_DIS_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_FLD:
		ServerClubBattleBeginFld( ( GLMSG::SNET_CLUB_BATTLE_BEGIN_FLD*) nmg );
        break;

	case NET_MSG_GCTRL_CLUB_BATTLE_OVER_FLD:
		ServerClubBattleOverFld( ( GLMSG::SNET_CLUB_BATTLE_OVER_FLD*) nmg );
		break;
	case NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_FLD:
		ServerClubBattleKillUpdate( (GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_FLD*) nmg );
		break;

	case NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_FLD:
		ServerClubBattleLastKillUpdate( (GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD*) nmg );
		break;

	case NET_MSG_GCTRL_CLUB_MBR_RENAME_FLD:
		ReqClubMemberRename ( (GLMSG::SNET_CLUB_MEMBER_RENAME_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_LEVEL_EVENT_END_FLD:
		ServerLevelEventEnd( (GLMSG::SNET_LEVEL_EVENT_END_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_LEVEL_EVENT_WARNING_FLD:
		ServerLevelEventWarning( (GLMSG::SNET_LEVEL_EVENT_WARNING_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_LEVEL_EVENT_COUNTDOWN_FLD:
		ServerLevelEventCountdown( (GLMSG::SNET_LEVEL_EVENT_COUNTDOWN_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_REQ_GENITEM_FLD_FB:
		RequestGenItemFieldFB ( (GLMSG::SNET_REQ_GENITEM_FLD_FB *)nmg );
		break;

	case NET_MSG_GCTRL_FIELDINFO_RESET:
		ServerFieldInfoReset ( (GLMSG::SNET_FIELDINFO_RESET *)nmg );
		break;

	case NET_MSG_GCTRL_SCHOOLFREEPK_FLD:
		ServerSchoolFreePk ( (GLMSG::SNETPC_SCHOOLFREEPK_FLD *)nmg );
		break;

	case NET_MSG_GCTRL_PERIOD:
		ServerCtrlPeriod ( (GLMSG::SNET_PERIOD *) nmg );
		break;

	case NET_MSG_SERVER_CTRL_WEATHER:
		ServerCtrlWeather ( (GLMSG::SNETSERVER_CTRL_WEATHER *) nmg );
		break;

	case NET_MSG_SERVER_CTRL_WEATHER2:
		ServerCtrlWeather2 ( (GLMSG::SNETSERVER_CTRL_WEATHER2 *) nmg );
		break;

	case NET_MSG_SERVER_CTRL_TIME:
		ServerCtrlTime ( (GLMSG::SNETSERVER_CTRL_TIME *) nmg );
		break;

	case NET_MSG_SERVER_CTRL_MONTH:
		ServerCtrlMonth ( (GLMSG::SNETSERVER_CTRL_MONTH *) nmg );
		break;

	case NET_MSG_SERVER_CTRL_GENITEMHOLD:
		ServerCtrlGenItemHold ( (GLMSG::SNETSERVER_CTRL_GENITEMHOLD *) nmg );
		break;

	case NET_MSG_SERVER_PLAYERKILLING_MODE:
		if ( dwClientID != m_dwAgentSlot ) break;
		ServerCtrlPlayerKillingMode ( (GLMSG::SNET_SERVER_PLAYERKILLING_MODE *) nmg );
		break;

	case NET_MSG_CYBERCAFECLASS_UPDATE:
		CyberCafeClassUpdate  ( dwClientID, dwGaeaID, (GLMSG::SNET_CYBERCAFECLASS_UPDATE *) nmg );
		break;

	case NET_MSG_GCTRL_DROP_OUT_FORCED:
		ServerCtrlCharDropOutForced ( (GLMSG::SNET_DROP_OUT_FORCED *)nmg );
		break;

	case NET_MSG_GM_MOVE2MAPPOS_FLD:
		GMCtrolMove2MapPos ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_MOVE2MAPPOS_FLD*) nmg );
		break;

	case NET_MSG_GM_WHERE_NPC_FLD:
		GMCtrlWhereNpc ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_WHERE_NPC_FLD*) nmg );
		break;

	case NET_MSG_GM_WARNING_MSG_FLD:
		GMCtrlWarningMSG ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_WARNING_MSG_FLD*) nmg );
		break;

	case NET_MSG_GM_WHERE_PC_POS:
		GMCtrolWherePcPos ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_WHERE_PC_POS*) nmg );
		break;

	case NET_MSG_GM_BIGHEAD:
		GMCtrolBigHead ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_BIGHEAD*) nmg );
		break;

	case NET_MSG_GM_BIGHAND:
		GMCtrolBigHand ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_BIGHAND*) nmg );
		break;

	case NET_MSG_GM_EVENT_EX:
		GMCtrolEventEx ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_EVENT_EX*) nmg );			
		break;

	case NET_MSG_GM_EVENT_EX_END:			
		GMCtrolEventExEnd ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_EVENT_EX_END*) nmg  );		
		break;

	case NET_MSG_GM_LIMIT_EVENT_BEGIN:
		GMCtrolLimitEventBegin ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_LIMIT_EVENT_BEGIN*) nmg );			
		break;

	case NET_MSG_GM_LIMIT_EVENT_TIME_RESET:
		GMCtrolLimitEventTimeReset  ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_LIMIT_EVENT_TIME_RESET*) nmg );			
		break;

	case NET_MSG_GM_LIMIT_EVENT_END:			
		GMCtrolLimitEventEnd ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_LIMIT_EVENT_END*) nmg  );		
		break;

	case NET_MSG_GM_CLASS_EVENT:
		GMCtrolClassEvent ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_CLASS_EVENT*) nmg  );		
		break;

	case NET_MSG_GM_MOB_GEN_FLD:
		GMCtrolMobGen ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_MOB_GEN_FLD*) nmg );
		break;

	case NET_MSG_GM_MOB_GEN_EX_FLD:
		GMCtrolMobGenEx ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_MOB_GEN_EX_FLD*) nmg );
		break;

	case NET_MSG_GM_PRINT_CROWLIST_FLD:
		GMCtrolMoPrintCrowList ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_PRINT_CROWLIST_FLD*) nmg );
		break;

	case NET_MSG_GM_MOB_DEL_EX_FLD:
		GMCtrolMobDelEx ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_MOB_DEL_EX_FLD*) nmg );
		break;

	case NET_MSG_GM_MOB_DEL_FLD:
		GMCtrolMobDel ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_MOB_DEL_FLD*) nmg );
		break;

	case NET_MSG_GM_MOB_LEVEL:
		GMCtrolMobLevel ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_MOB_LEV*) nmg );
		break;

	case NET_MSG_GM_MOB_LEVEL_CLEAR:
		GMCtrolMobLevelClear ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_MOB_LEV_CLEAR*) nmg );
		break;

	case NET_MSG_GM_CHAT_BLOCK_FLD:
		if ( dwClientID != m_dwAgentSlot ) break;
		GMCtrolChatBlockFld ( (GLMSG::SNET_GM_CHAT_BLOCK_FLD*) nmg );
		break;

	case NET_MSG_USER_CHAR_INFO_FLD:
		if ( dwClientID != m_dwAgentSlot ) break;
		UserCtrolCharInfoFld ( (GLMSG::SNET_USER_CHAR_INFO_FLD*) nmg );
		break;

	case NET_MSG_GM_CHAR_INFO_FLD:
		if ( dwClientID != m_dwAgentSlot ) break;
		GMCtrolCharInfoFld ( (GLMSG::SNET_GM_CHAR_INFO_FLD*) nmg );
		break;

	case NET_MSG_GM_FREEPK:
		GMCtrolFreePK ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_FREEPK*) nmg );
		break;

	case NET_MSG_GM_MOVE2GATE_FLD:
		GMCtrolMove2Gate ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_MOVE2GATE_FLD *) nmg );
		break;

	case NET_MSG_GCTRL_SERVERTEST:
		if( dwClientID != 0 ) break;
		ReserveServerStop ();
		break;

	case NET_MSG_GM_KICK_USER_PROC_FLD:
		GMKicUser ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_KICK_USER_PROC_FLD *) nmg );
		break;

	case NET_MSG_GM_SHOWMETHEMONEY_FLD:
		GMCtrolShowMeTheMoney ( dwClientID, dwGaeaID, (GLMSG::SNET_GM_SHOWMETHEMONEY_FLD *) nmg );
		break;

#if defined(VN_PARAM) //vietnamtest%%%
	case NET_MSG_VIETNAM_ALLINITTIME:
		VietnamAllInitTime ( (GLMSG::SNETPC_VIETNAM_ALLINITTIME *)nmg );
		break;
#endif

	// ETC
	case NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FLD:
		SetDelayMilliSec ( dwClientID, dwGaeaID, (GLMSG::SNET_SET_SERVER_DELAYTIME_FLD*) nmg );
		break;
	case NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FLD:
		SetSkipPacketNum ( dwClientID, dwGaeaID, (GLMSG::SNET_SET_SERVER_SKIPPACKET_FLD*) nmg );
		break;
	case NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FLD: 
		SetServerCrashTime ( dwClientID, dwGaeaID, (GLMSG::SNET_SET_SERVER_CRASHTIME_FLD*) nmg );
		break;

	case NET_MSG_GM_EVENT_ITEM_GEN:
	case NET_MSG_GM_EVENT_ITEM_GEN_END:
	case NET_MSG_GM_EVENT_MONEY_GEN:
	case NET_MSG_GM_EVENT_MONEY_GEN_END:
	case NET_MSG_GM_EVENT_EXP:
	case NET_MSG_GM_EVENT_EXP_END:
		m_cEventProc.MsgProcess ( nmg, dwClientID, dwGaeaID );
	
	// 탈것
	case NET_MSG_GCTRL_ACTIVE_VEHICLE:
		ReqActiveVehicle ( dwClientID, dwGaeaID, (GLMSG::SNETPC_ACTIVE_VEHICLE*) nmg );
		break;
	case NET_MSG_GCTRL_GET_VEHICLE:
		ReqGetVehicle( dwClientID, dwGaeaID, (GLMSG::SNETPC_GET_VEHICLE*) nmg );
		break;
	case NET_MSG_VEHICLE_CREATE_FROMDB_FB:
		CreateVehicleOnDB( dwClientID, dwGaeaID, (GLMSG::SNET_VEHICLE_CREATE_FROMDB_FB*) nmg );
		break;
	case NET_MSG_VEHICLE_GET_FROMDB_FB:
		GetVehicleInfoFromDB( dwClientID, dwGaeaID, (GLMSG::SNET_VEHICLE_GET_FROMDB_FB*) nmg );
		break;	
	case NET_MSG_VEHICLE_GET_FROMDB_ERROR:
		GetVehicleInfoFromDBError( dwClientID, dwGaeaID, (GLMSG::SNET_VEHICLE_GET_FROMDB_ERROR*) nmg );
		break;	
	case NET_MSG_VEHICLE_REQ_ITEM_INFO:
		GetVehicleItemInfo ( dwClientID, dwGaeaID, (GLMSG::SNET_VEHICLE_REQ_ITEM_INFO*) nmg );
		break;

	// 부활 스킬 사용 여부
	case NET_NON_REBIRTH_REQ:
		SetNonRebirth ( dwClientID, dwGaeaID, (GLMSG::SNET_NON_REBIRTH_REQ*) nmg );
		break;
	// QBox On/Off 옵션
	case NET_QBOX_OPTION_REQ_FLD:
		ReqQBoxEnable ( dwClientID, dwGaeaID, (GLMSG::SNET_QBOX_OPTION_REQ_FLD*) nmg );
		break;

	case NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_REQ:
		ReqClubDeathMatchRanking ( dwClientID, dwGaeaID, (GLMSG::SNET_CLUB_DEATHMATCH_RANKING_REQ*) nmg );
		break;

//----------------------------------------------------------------------------------------------------------------
// Message Processing about PET (START)
//----------------------------------------------------------------------------------------------------------------

	// =======  Message Processing about MyPET From Owner (START)  ======= //

	case NET_MSG_PET_REQ_USECARD:								
		RequestUsePETCARD ( dwClientID, dwGaeaID, ( GLMSG::SNETPET_REQ_USEPETCARD* ) nmg );
		break;

	case NET_MSG_PET_REQ_UNUSECARD:
		{
			GLMSG::SNETPET_REQ_UNUSEPETCARD* pNetMsg = ( GLMSG::SNETPET_REQ_UNUSEPETCARD* ) nmg;
			PGLPETFIELD pPet = GetPET ( pNetMsg->dwGUID );
			if ( pPet && pPet->GetPetID () == pNetMsg->dwPetID )
			{
                DropOutPET ( pNetMsg->dwGUID, false, false );
			}
		}
		break;

	case NET_MSG_PET_REQ_REVIVE:
		RequestRevivePet ( dwClientID, dwGaeaID, ( GLMSG::SNETPET_REQ_REVIVE* ) nmg );
		break;

	case NET_MSG_PET_REQ_REVIVE_FROMDB_FB:
		RevivePet ( dwClientID, dwGaeaID, ( GLMSG::SNETPET_REQ_REVIVE_FROMDB_FB* ) nmg );
		break;

	case NET_MSG_CREATE_PET_FROMDB_FB:
		CreatePETOnDB ( dwClientID, dwGaeaID, ( GLMSG::SNETPET_CREATEPET_FROMDB_FB* ) nmg );
		break;

	case NET_MSG_GET_PET_FROMDB_FB:
		GetPETInfoFromDB ( dwClientID, dwGaeaID, ( GLMSG::SNETPET_GETPET_FROMDB_FB* ) nmg );
		break;
	case NET_MSG_GET_PET_FROMDB_ERROR:
		GetPETInfoFromDBError ( dwClientID, dwGaeaID, ( GLMSG::SNETPET_GETPET_FROMDB_ERROR* ) nmg );
		break;

	case NET_MSG_PET_REQ_PETCARDINFO:
		{
			GLMSG::SNETPET_REQ_PETCARDINFO* pNetMsg = ( GLMSG::SNETPET_REQ_PETCARDINFO* ) nmg;

			PGLCHAR pChar = GetChar ( dwGaeaID );
			if ( !pChar ) break;

			// 팻 생성여부 확인
			if ( pNetMsg->dwPetID <= 0 || pChar->m_dwCharID <= 0 )
			{
				GetConsoleMsg()->Write ( LOG_TEXT_CONSOLE, "Invalid ID, PetID %d CharID %d dwClientID %d", 
										 pNetMsg->dwPetID, pChar->m_dwCharID, dwClientID );
				break;
			}

			// 거래시 사용될 팻카드 정보이면
			if ( pNetMsg->bTrade )
			{
				PGLPET pPetInfo = new GLPET ();

				// DB작업 실패시 혹은 생성후 메모리 해제해줘라
				CGetPet* pDbAction = new CGetPet ( pPetInfo, pNetMsg->dwPetID, dwClientID, pChar->m_dwCharID, 
												   0, 0, false, false, true );
				m_pDBMan->AddJob ( pDbAction );
			}
			// 인벤토리,창고에 사용될 팻카드 정보이면
			else
			{
				PGLPET pPetInfo = new GLPET ();

				// DB작업 실패시 혹은 생성후 메모리 해제해줘라
				CGetPet* pDbAction = new CGetPet ( pPetInfo, pNetMsg->dwPetID, dwClientID, pChar->m_dwCharID, 
												   0, 0, false, true );
				m_pDBMan->AddJob ( pDbAction );
			}
		}
		break;

	// =======  Message Processing about MyPET From Owner (END)  ======= //

	// =======  Message Processing about MyPET From PET (START)  ======= //

	case NET_MSG_PET_REQ_GOTO:
	case NET_MSG_PET_REQ_STOP:
	case NET_MSG_PET_REQ_UPDATE_MOVE_STATE:
	case NET_MSG_PET_REQ_SKILLCHANGE:
	case NET_MSG_PET_REQ_RENAME:
	case NET_MSG_PET_REQ_RENAME_FROMDB_FB:
	case NET_MSG_PET_REQ_CHANGE_COLOR:
	case NET_MSG_PET_REQ_CHANGE_STYLE:
	case NET_MSG_PET_REQ_SLOT_EX_HOLD:
	case NET_MSG_PET_REQ_HOLD_TO_SLOT:
	case NET_MSG_PET_REQ_SLOT_TO_HOLD:
	case NET_MSG_PET_REQ_LEARNSKILL:
	case NET_MSG_PET_REQ_FUNNY:
	case NET_MSG_PET_REMOVE_SLOTITEM:
	case NET_MSG_PET_PETSKINPACKOPEN:
		{
			PGLCHAR pOwner = GetChar ( dwGaeaID );
			if ( pOwner )
			{
				PGLPETFIELD pPet = GetPET ( pOwner->m_dwPetGUID );
				if ( pPet )
				{
					pPet->MsgProcess ( nmg );
				}
			}
		}
		break;

	// =======  Message Processing about MyPET From PET (END)  ======= //

	//----------------------------------------------------------------------------------------------------------------
	// Message Processing about SUMMON (START)
	//----------------------------------------------------------------------------------------------------------------
	case NET_MSG_REQ_USE_SUMMON:
		RequestSummon( dwClientID, dwGaeaID, ( GLMSG::SNETPC_REQ_USE_SUMMON* ) nmg );
		break;
	// =======  Message Processing about MySUMMON From Owner (END)  ======= //

	// =======  Message Processing about MySUMMON From SUMMON (START)  ======= //

	case NET_MSG_SUMMON_REQ_GOTO:
	case NET_MSG_SUMMON_REQ_STOP:
	case NET_MSG_SUMMON_REQ_UPDATE_MOVE_STATE:
		{
			PGLCHAR pOwner = GetChar ( dwGaeaID );
			if ( pOwner )
			{
				PGLSUMMONFIELD pSummon = GetSummon ( pOwner->m_dwSummonGUID );
				if ( pSummon )
				{
					pSummon->MsgProcess ( nmg );
				}
			}
		}
		break;


	// =======  Message Processing about MySUMMON From SUMMON (END)  ======= //


	default:
		{
			PGLCHAR pChar = GetChar ( dwGaeaID );
			if ( pChar )
			{
				GetConsoleMsg()->Write ( LOG_CONSOLE,
			                    "[WARNING]GLGaeaServer::MsgProcess illigal message(%d) Name(%s)",
								nmg->nType, pChar->m_szName );
			}
			else
			{
				GetConsoleMsg()->Write ( LOG_CONSOLE,
			                    "[WARNING]GLGaeaServer::MsgProcess illigal message(%d)",
			                    nmg->nType);
			}
		}
		break;
	};

	return S_OK;
}

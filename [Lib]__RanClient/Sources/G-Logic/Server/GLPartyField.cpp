#include "pch.h"
#include "glpartyfield.h"

#include "GLChar.h"
#include "GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLPARTY_FIELD::GLPARTY_FIELD ()
{
	m_cMEMBER.reserve( MAXPARTY );
	RESET ();
}

void GLPARTY_FIELD::RESET ()
{
	m_dwPARTYID = PARTY_NULL;
	m_dwMASTER = GAEAID_NULL;

	m_bQBoxEnable = TRUE;
	
	m_cMEMBER.clear();

	m_dwconftPARTYID = PARTY_NULL;
	m_dwconftMEMLEVEL = 1;

	m_arrayROUND_ITEM_CUR.clear();
	m_arrayROUND_ITEM_CUR.reserve(MAXPARTY);

	for ( int i=0; i<MAXPARTY; ++i )
	{
		m_arrayROUND_ITEM_CUR.push_back(0);

		m_arrayROUND_ITEM[i].clear();
		m_arrayROUND_ITEM[i].reserve(MAXPARTY);
		for ( int j=0; j<MAXPARTY; ++j )
		{
			m_arrayROUND_ITEM[i].push_back(j);
		}

		std::random_shuffle ( m_arrayROUND_ITEM[i].begin(), m_arrayROUND_ITEM[i].end() );
	}

	m_arrayROUND_MONEY_CUR.clear();
	m_arrayROUND_MONEY_CUR.reserve(MAXPARTY);

	for ( int i=0; i<MAXPARTY; ++i )
	{
		m_arrayROUND_MONEY_CUR.push_back(0);

		m_arrayROUND_MONEY[i].clear();
		m_arrayROUND_MONEY[i].reserve(MAXPARTY);
		for ( int j=0; j<MAXPARTY; ++j )
		{
			m_arrayROUND_MONEY[i].push_back(j);
		}

		std::random_shuffle ( m_arrayROUND_MONEY[i].begin(), m_arrayROUND_MONEY[i].end() );
	}
}

BOOL GLPARTY_FIELD::ISVAILD ()
{
	return ( m_dwMASTER!=GAEAID_NULL );
}

void GLPARTY_FIELD::SETMASTER ( DWORD dwMaster, DWORD dwPartyID )
{
	m_dwMASTER = dwMaster;
	m_dwPARTYID = dwPartyID;
}

BOOL GLPARTY_FIELD::ISMASTER ( DWORD dwGaeaID )
{
	return ( m_dwMASTER == dwGaeaID );
}

BOOL GLPARTY_FIELD::ISFULL ()
{
	return ( m_cMEMBER.size() == MAXPARTY );
}

BOOL GLPARTY_FIELD::ISMEMBER ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID ) return TRUE;
	}

	return FALSE;
}

DWORD GLPARTY_FIELD::GETNUMBER ()
{
	return (DWORD) m_cMEMBER.size();
}

BOOL GLPARTY_FIELD::ADDMEMBER ( GLPARTY_FNET &sParty )
{
	if ( ISFULL () )					return FALSE;
	if ( ISMEMBER(sParty.m_dwGaeaID) )	return FALSE;

	m_cMEMBER.push_back ( std::make_pair(sParty.m_dwGaeaID,sParty) );

	return FALSE;
}

BOOL GLPARTY_FIELD::DELMEMBER ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
		{
			m_cMEMBER.erase(iter);
			return TRUE;
		}
	}

	return FALSE;
}

const GLPARTY_FNET* GLPARTY_FIELD::GETMEMBER ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID ) return &sMember;
	}

	return NULL;
}

BOOL GLPARTY_FIELD::MBR_MOVEMAP ( DWORD dwGaeaID, SNATIVEID &sMapID )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
		{
			sMember.m_sMapID = sMapID;
			return TRUE;
		}
	}

	return FALSE;
}

const SNATIVEID GLPARTY_FIELD::GETMBR_MAP ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID ) return sMember.m_sMapID;
	}
	return NATIVEID_NULL();
}

bool GLPARTY_FIELD::MBR_CONFRONT_JOIN ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
		{
			sMember.m_bConfront = true;
			return true;
		}
	}

	return false;
}

bool GLPARTY_FIELD::MBR_CONFRONT_LEAVE ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
		{
			sMember.m_bConfront = false;
			return true;
		}
	}

	return false;
}

void GLPARTY_FIELD::RESET_CONFRONT ( EMCONFRONT_END emEND )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();

	GLMSG::SNETPC_CONFRONTPTY_END2_CLT NetMsgClt;
	NetMsgClt.emEND = emEND;

	GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgBrd;
	NetMsgBrd.emEND = emEND;

	for ( ; iter!=iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		sMember.m_bConfront = false;

		GLChar *pChar = GLGaeaServer::GetInstance().GetChar ( sMember.m_dwGaeaID );
		if ( pChar )
		{
			if ( pChar->m_sCONFTING.IsCONFRONTING() )
			{
				//	Note : 대련 체력 환원.
				pChar->DoConftStateRestore();
			}

			// 대련종료 후 무적시간 설정
			pChar->m_sCONFTING.RESET();
			pChar->m_sCONFTING.SETPOWERFULTIME ( 10.0f ); 

			//	Note : 파티 대련 종료를 클라이언트들에 알림.			
			GLGaeaServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgClt );
			
			NetMsgBrd.dwGaeaID = pChar->m_dwGaeaID;			
			if ( emEND==EMCONFRONT_END_PWIN || emEND==EMCONFRONT_END_PLOSS )
			{
				pChar->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
			}
		}
	}
	m_dwconftPARTYID = PARTY_NULL;
}

bool GLPARTY_FIELD::DOITEM_ALLOT ( GLChar* pKnockChar, CItemDrop* pItemDrop, bool bCheckAllotItem )
{
	GASSERT(pKnockChar&&"GLPARTY_FIELD::DOITEM_ALLOT()");
	if ( !pKnockChar )	return false;

	GASSERT(pItemDrop&&"GLPARTY_FIELD::DOITEM_ALLOT()");
	if ( !pItemDrop )	return false;

	switch ( m_sOPTION.emGET_ITEM )
	{
	case EMPTYITEMOPT_LEADER:
		{
			PGLCHAR pMaster = GLGaeaServer::GetInstance().GetChar(m_dwMASTER);
			
			//	파티 마스터가 존제하지 않으므로 루팅 하지 않음.
			if ( !pMaster )										return false;
			if ( pMaster->m_sMapID!=pKnockChar->m_sMapID )		return false;

			//	파티 마스터가 유효 거리내에 존재하는지 검사.
			D3DXVECTOR3 vDist = pMaster->m_vPos - pKnockChar->m_vPos;
			float fDist = D3DXVec3Length(&vDist);
			if ( fDist > MAX_VIEWRANGE )						return false;

			HRESULT hr = pMaster->InsertToInven ( pItemDrop );
			if ( SUCCEEDED(hr) ) return true;
		}
		return false;

	case EMPTYITEMOPT_ROUND:
		{
			//	Note : 멤버 배열 정리.
			//
			DWORD dwKnockIndex(MAXPARTY);
			DWORD dwMemberNum(0);
			DWORD dwaryMEMBER[MAXPARTY];
			for ( int i=0; i<MAXPARTY; ++i )	dwaryMEMBER[i] = GAEAID_NULL;

			MEMBER_ITER iter = m_cMEMBER.begin();
			MEMBER_ITER iter_end = m_cMEMBER.end();
			for ( ; iter!=iter_end; ++iter )
			{
				GLPARTY_FNET &sMember = (*iter).second;

				// 아이템을 노크한 케릭터의 인덱스(순서) 저장
				if ( pKnockChar->m_dwGaeaID == sMember.m_dwGaeaID ) dwKnockIndex = dwMemberNum;
				// 멤버배열에 GaeaID 설정
				dwaryMEMBER[dwMemberNum++] = sMember.m_dwGaeaID;
			}

			// 아무도 없으면
			if ( dwMemberNum==0 )								return false;
			// 노크한 케릭터가 없으면
			if ( dwKnockIndex==MAXPARTY )						return false;
			// 현재 노크한 케릭터의 습득순서를 갖는 인덱스
			DWORD &dwCUR = m_arrayROUND_ITEM_CUR[dwKnockIndex];
			for ( int i=0; i<MAXPARTY; ++i )
			{
				// 현재 노크한 케릭터가 갖고있는 아이템 습득인덱스(순서)
				DWORD dwALLOT_GAEAID = m_arrayROUND_ITEM[dwKnockIndex][dwCUR++];
				if ( dwCUR >= MAXPARTY )	dwCUR = 0;

				// 이번에 아이템을 습득할 케릭터의 GaeaID
				DWORD dwMEMBERID = dwaryMEMBER[dwALLOT_GAEAID];

				PGLCHAR pMember = GLGaeaServer::GetInstance().GetChar(dwMEMBERID);
				if ( !pMember )									continue;
				if ( pMember->m_sMapID!=pKnockChar->m_sMapID )	continue;
				if ( !pMember->IsValidBody() )					continue;

				//	파티원이 유효 거리내에 존재하는지 검사.
				D3DXVECTOR3 vDist = pMember->m_vPos - pKnockChar->m_vPos;
				float fDist = D3DXVec3Length(&vDist);
				if ( fDist > MAX_VIEWRANGE )					continue;

				HRESULT hr = pMember->InsertToInven ( pItemDrop );
				if ( SUCCEEDED(hr) ) return true;
			}
		}
		return false;

	default:
		break;
	};

	return false;
}

bool GLPARTY_FIELD::DOMONEY_ALLOT ( GLChar* pKnockChar, CMoneyDrop* pMoneyDrop, GLLandMan* pLandMan, bool bCheckAllotItem )
{
	GASSERT(pKnockChar&&"GLPARTY_FIELD::DOMONEY_ALLOT()");
	if ( !pKnockChar )	return false;

	GASSERT(pMoneyDrop&&"GLPARTY_FIELD::DOMONEY_ALLOT()");
	if ( !pMoneyDrop )	return false;

	GASSERT(pLandMan&&"GLPARTY_FIELD::DOMONEY_ALLOT()");
	if ( !pLandMan )	return false;

	switch ( m_sOPTION.emGET_MONEY )
	{
	case EMPTYMONEYOPT_LEADER:
		{
			PGLCHAR pMaster = GLGaeaServer::GetInstance().GetChar(m_dwMASTER);
			
			//	파티 마스터가 존제하지 않으므로 루팅 하지 않음.
			if ( !pMaster )										return false;
			if ( pMaster->m_sMapID!=pKnockChar->m_sMapID )		return false;

			//	파티 마스터가 유효 거리내에 존재하는지 검사.
			D3DXVECTOR3 vDist = pMaster->m_vPos - pKnockChar->m_vPos;
			float fDist = D3DXVec3Length(&vDist);
			if ( fDist > MAX_VIEWRANGE )						return false;

			HRESULT hr = pMaster->InsertToInven ( pMoneyDrop );
			if ( SUCCEEDED(hr) )									return true;
		}
		return false;

	//case EMPTYOPT_GETROUND:
	//	{
	//		//	Note : 멤버 배열 정리.
	//		//
	//		DWORD dwKnockIndex(MAXPARTY);
	//		DWORD dwMemberNum(0);
	//		DWORD dwaryMEMBER[MAXPARTY];
	//		for ( int i=0; i<MAXPARTY; ++i )	dwaryMEMBER[i] = GAEAID_NULL;

	//		// 멤버 배열 할당
	//		MEMBER_ITER iter = m_cMEMBER.begin();
	//		MEMBER_ITER iter_end = m_cMEMBER.end();
	//		for ( ; iter!=iter_end; ++iter )
	//		{
	//			GLPARTY_FNET &sMember = (*iter).second;

	//			if ( pKnockChar->m_dwGaeaID == sMember.m_dwGaeaID ) dwKnockIndex = dwMemberNum;
	//			dwaryMEMBER[dwMemberNum++] = sMember.m_dwGaeaID;
	//		}

	//		if ( dwMemberNum==0 )								return false;
	//		if ( dwKnockIndex==MAXPARTY )						return false;
	//		
	//		DWORD &dwCUR = m_arrayROUND_MONEY_CUR[dwKnockIndex];
	//		for ( i=0; i<MAXPARTY; ++i )
	//		{
	//			DWORD dwALLOT_GAEAID = m_arrayROUND_MONEY[dwKnockIndex][dwCUR++];

	//			if ( dwCUR >= MAXPARTY )	dwCUR = 0;

	//			// 이번에 아이템을 습득할 케릭터의 GaeaID
	//			DWORD dwMEMBERID = dwaryMEMBER[dwALLOT_GAEAID];

	//			PGLCHAR pMember = GLGaeaServer::GetInstance().GetChar(dwMEMBERID);
	//			if ( !pMember )									continue;
	//			if ( pMember->m_sMapID!=pKnockChar->m_sMapID )	continue;
	//			if ( !pMember->IsValidBody() )					continue;

	//			//	파티원이 유효 거리내에 존재하는지 검사.
	//			D3DXVECTOR3 vDist = pMember->m_vPos - pKnockChar->m_vPos;
	//			float fDist = D3DXVec3Length(&vDist);
	//			if ( fDist > MAX_VIEWRANGE )					continue;

	//			HRESULT hr = pMember->InsertToInven ( pMoneyDrop );
	//			if ( SUCCEEDED(hr) )						return true;
	//		}
	//	}
	//	return false;

	case EMPTYMONEYOPT_EQUAL:
		{
			//	Note : 멤버 배열 정리.
			//
			DWORD dwMemberNum(0);
			DWORD dwaryMEMBER[MAXPARTY];
			for ( int i=0; i<MAXPARTY; ++i )	dwaryMEMBER[i] = GAEAID_NULL;

			MEMBER_ITER iter = m_cMEMBER.begin();
			MEMBER_ITER iter_end = m_cMEMBER.end();
			for ( ; iter!=iter_end; ++iter )
			{
				GLPARTY_FNET &sMember = (*iter).second;

				const DWORD dwMemberID = sMember.m_dwGaeaID;

				PGLCHAR pMember = GLGaeaServer::GetInstance().GetChar(dwMemberID);
				if ( !pMember )									continue;
				if ( pMember->m_sMapID!=pKnockChar->m_sMapID )	continue;
				if ( !pMember->IsValidBody() )					continue;

				//	파티원이 유효 거리내에 존재하는지 검사.
				D3DXVECTOR3 vDist = pMember->m_vPos - pKnockChar->m_vPos;
				float fDist = D3DXVec3Length(&vDist);
				if ( fDist > MAX_VIEWRANGE )					continue;

				dwaryMEMBER[dwMemberNum++] = dwMemberID;
			}
			if ( dwMemberNum==0 )								return false;

			LONGLONG lnAmount = pMoneyDrop->lnAmount;

			LONGLONG lnSplit = lnAmount / dwMemberNum;		//	균등 분할.
			LONGLONG lnSurplus = lnAmount % dwMemberNum;	//	균등 분할후 나머지.

			for ( DWORD dwIndex=0; dwIndex<dwMemberNum; dwIndex++ )
			{
				PGLCHAR pMember = GLGaeaServer::GetInstance().GetChar(dwaryMEMBER[dwIndex]);
				if ( !pMember )									continue;

				LONGLONG lnInsertMoney = lnSplit;
				if ( lnSurplus>0 )
				{
					lnSurplus -= 1;
					lnInsertMoney += 1;
				}
				pMember->InsertMoney ( lnInsertMoney );
			}

			pLandMan->DropOutMoney ( pMoneyDrop->dwGlobID );
		}
		return true;

	default:
		break;
	};

	return false;
}

bool GLPARTY_FIELD::SENDMSG2NEAR ( GLChar* pKnockChar, void* pNetMsg )
{
	GASSERT(pKnockChar&&"GLPARTY_FIELD::SENDMSG2NEAR()");
	if ( !pKnockChar )	return false;

	GASSERT(pNetMsg&&"GLPARTY_FIELD::SENDMSG2NEAR()");
	if ( !pNetMsg )		return false;

	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for ( ; iter!=iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		const DWORD dwMemberID = sMember.m_dwGaeaID;
		if ( pKnockChar->m_dwGaeaID == dwMemberID )		continue;

		PGLCHAR pMember = GLGaeaServer::GetInstance().GetChar(dwMemberID);
		if ( !pMember )									continue;
		if ( pMember->m_sMapID!=pKnockChar->m_sMapID )	continue;

		//	파티원이 유효 거리내에 존재하는지 검사.
		D3DXVECTOR3 vDist = pMember->m_vPos - pKnockChar->m_vPos;
		float fDist = D3DXVec3Length(&vDist);
		if ( fDist > MAX_VIEWRANGE )					continue;
	
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pMember->m_dwClientID, pNetMsg );
	}

	return true;
}

DWORD GLPARTY_FIELD::GETAVER_CONFT_LEVEL ()
{
	DWORD dwMem(0);
	DWORD dwTotalLevel(0);
	
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for ( ; iter!=iter_end; ++iter )
	{
		const GLPARTY_FNET &sPARTY_MEM = (*iter).second;
		if ( sPARTY_MEM.m_bConfront )
		{
			PGLCHAR pMember = GLGaeaServer::GetInstance().GetChar(sPARTY_MEM.m_dwGaeaID);
			if ( !pMember )									continue;

			dwMem++;
			dwTotalLevel += pMember->m_wLevel;
		}
	}
	
	if ( dwMem==0 )		return 0;

	m_dwconftMEMLEVEL = (dwTotalLevel/dwMem);
	return m_dwconftMEMLEVEL;
}

void GLPARTY_FIELD::RECEIVELIVINGPOINT ( int nLivePoint )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for ( ; iter!=iter_end; ++iter )
	{
		const GLPARTY_FNET &sPARTY_MEM = (*iter).second;
		if ( sPARTY_MEM.m_bConfront )
		{
			PGLCHAR pMember = GLGaeaServer::GetInstance().GetChar(sPARTY_MEM.m_dwGaeaID);
			if ( !pMember )									continue;

			pMember->ReceiveLivingPoint ( nLivePoint );
		}
	}	
}


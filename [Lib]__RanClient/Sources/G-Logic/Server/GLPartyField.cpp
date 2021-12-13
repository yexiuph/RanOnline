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
				//	Note : ��� ü�� ȯ��.
				pChar->DoConftStateRestore();
			}

			// ������� �� �����ð� ����
			pChar->m_sCONFTING.RESET();
			pChar->m_sCONFTING.SETPOWERFULTIME ( 10.0f ); 

			//	Note : ��Ƽ ��� ���Ḧ Ŭ���̾�Ʈ�鿡 �˸�.			
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
			
			//	��Ƽ �����Ͱ� �������� �����Ƿ� ���� ���� ����.
			if ( !pMaster )										return false;
			if ( pMaster->m_sMapID!=pKnockChar->m_sMapID )		return false;

			//	��Ƽ �����Ͱ� ��ȿ �Ÿ����� �����ϴ��� �˻�.
			D3DXVECTOR3 vDist = pMaster->m_vPos - pKnockChar->m_vPos;
			float fDist = D3DXVec3Length(&vDist);
			if ( fDist > MAX_VIEWRANGE )						return false;

			HRESULT hr = pMaster->InsertToInven ( pItemDrop );
			if ( SUCCEEDED(hr) ) return true;
		}
		return false;

	case EMPTYITEMOPT_ROUND:
		{
			//	Note : ��� �迭 ����.
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

				// �������� ��ũ�� �ɸ����� �ε���(����) ����
				if ( pKnockChar->m_dwGaeaID == sMember.m_dwGaeaID ) dwKnockIndex = dwMemberNum;
				// ����迭�� GaeaID ����
				dwaryMEMBER[dwMemberNum++] = sMember.m_dwGaeaID;
			}

			// �ƹ��� ������
			if ( dwMemberNum==0 )								return false;
			// ��ũ�� �ɸ��Ͱ� ������
			if ( dwKnockIndex==MAXPARTY )						return false;
			// ���� ��ũ�� �ɸ����� ��������� ���� �ε���
			DWORD &dwCUR = m_arrayROUND_ITEM_CUR[dwKnockIndex];
			for ( int i=0; i<MAXPARTY; ++i )
			{
				// ���� ��ũ�� �ɸ��Ͱ� �����ִ� ������ �����ε���(����)
				DWORD dwALLOT_GAEAID = m_arrayROUND_ITEM[dwKnockIndex][dwCUR++];
				if ( dwCUR >= MAXPARTY )	dwCUR = 0;

				// �̹��� �������� ������ �ɸ����� GaeaID
				DWORD dwMEMBERID = dwaryMEMBER[dwALLOT_GAEAID];

				PGLCHAR pMember = GLGaeaServer::GetInstance().GetChar(dwMEMBERID);
				if ( !pMember )									continue;
				if ( pMember->m_sMapID!=pKnockChar->m_sMapID )	continue;
				if ( !pMember->IsValidBody() )					continue;

				//	��Ƽ���� ��ȿ �Ÿ����� �����ϴ��� �˻�.
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
			
			//	��Ƽ �����Ͱ� �������� �����Ƿ� ���� ���� ����.
			if ( !pMaster )										return false;
			if ( pMaster->m_sMapID!=pKnockChar->m_sMapID )		return false;

			//	��Ƽ �����Ͱ� ��ȿ �Ÿ����� �����ϴ��� �˻�.
			D3DXVECTOR3 vDist = pMaster->m_vPos - pKnockChar->m_vPos;
			float fDist = D3DXVec3Length(&vDist);
			if ( fDist > MAX_VIEWRANGE )						return false;

			HRESULT hr = pMaster->InsertToInven ( pMoneyDrop );
			if ( SUCCEEDED(hr) )									return true;
		}
		return false;

	//case EMPTYOPT_GETROUND:
	//	{
	//		//	Note : ��� �迭 ����.
	//		//
	//		DWORD dwKnockIndex(MAXPARTY);
	//		DWORD dwMemberNum(0);
	//		DWORD dwaryMEMBER[MAXPARTY];
	//		for ( int i=0; i<MAXPARTY; ++i )	dwaryMEMBER[i] = GAEAID_NULL;

	//		// ��� �迭 �Ҵ�
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

	//			// �̹��� �������� ������ �ɸ����� GaeaID
	//			DWORD dwMEMBERID = dwaryMEMBER[dwALLOT_GAEAID];

	//			PGLCHAR pMember = GLGaeaServer::GetInstance().GetChar(dwMEMBERID);
	//			if ( !pMember )									continue;
	//			if ( pMember->m_sMapID!=pKnockChar->m_sMapID )	continue;
	//			if ( !pMember->IsValidBody() )					continue;

	//			//	��Ƽ���� ��ȿ �Ÿ����� �����ϴ��� �˻�.
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
			//	Note : ��� �迭 ����.
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

				//	��Ƽ���� ��ȿ �Ÿ����� �����ϴ��� �˻�.
				D3DXVECTOR3 vDist = pMember->m_vPos - pKnockChar->m_vPos;
				float fDist = D3DXVec3Length(&vDist);
				if ( fDist > MAX_VIEWRANGE )					continue;

				dwaryMEMBER[dwMemberNum++] = dwMemberID;
			}
			if ( dwMemberNum==0 )								return false;

			LONGLONG lnAmount = pMoneyDrop->lnAmount;

			LONGLONG lnSplit = lnAmount / dwMemberNum;		//	�յ� ����.
			LONGLONG lnSurplus = lnAmount % dwMemberNum;	//	�յ� ������ ������.

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

		//	��Ƽ���� ��ȿ �Ÿ����� �����ϴ��� �˻�.
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


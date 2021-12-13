#include "pch.h"
#include "GLChar.h"
#include "GLGaeaServer.h"

#include "glpartyfieldman.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLPartyFieldMan::GLPartyFieldMan(void) :
	m_dwMaxClient(0),
	m_pParty(NULL),
	m_pGaeaTOParty(NULL)
{
}

GLPartyFieldMan::~GLPartyFieldMan(void)
{
	SAFE_DELETE_ARRAY(m_pParty);
	SAFE_DELETE_ARRAY(m_pGaeaTOParty);
}

HRESULT GLPartyFieldMan::Create ( DWORD dwMaxClient )
{
	m_dwMaxClient = dwMaxClient;

	SAFE_DELETE_ARRAY(m_pParty);
	m_pParty = new GLPARTY_FIELD[m_dwMaxClient];

	SAFE_DELETE_ARRAY(m_pGaeaTOParty);
	m_pGaeaTOParty = new DWORD[m_dwMaxClient];
	for ( DWORD i=0; i<m_dwMaxClient; ++i )		m_pGaeaTOParty[i] = PARTY_NULL;

	return S_OK;
}

DWORD GLPartyFieldMan::GetPartyID ( DWORD dwGaeaID )
{
	GASSERT(dwGaeaID<m_dwMaxClient);
	if ( dwGaeaID>=m_dwMaxClient )		return PARTY_NULL;

	return m_pGaeaTOParty[dwGaeaID];
}

void GLPartyFieldMan::SetPartyID ( DWORD dwGaeaID, DWORD dwPartyID )
{
	GASSERT(dwPartyID<m_dwMaxClient);
	GASSERT(dwGaeaID<m_dwMaxClient);
	if ( dwGaeaID>=m_dwMaxClient )		return;
	if ( dwPartyID>=m_dwMaxClient )		return;

	m_pGaeaTOParty[dwGaeaID] = dwPartyID;

	//	Note : 현제 필드 서버에 존제하는 케릭터의 경우 파티 ID 설정.
	//
	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwGaeaID );
	if ( pChar )	pChar->SetPartyID ( dwPartyID );
}

void GLPartyFieldMan::ReSetPartyID ( DWORD dwGaeaID )
{
	GASSERT(dwGaeaID<m_dwMaxClient);
	if ( dwGaeaID>=m_dwMaxClient )		return;

	m_pGaeaTOParty[dwGaeaID] = PARTY_NULL;

	//	Note : 현재 필드 서버에 존제하는 케릭터의 경우 파티 ID 설정.
	//
	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwGaeaID );
	if ( pChar )	pChar->ReSetPartyID ();
}

BOOL GLPartyFieldMan::IsPartyMaster ( DWORD dwPartyID, DWORD dwGaeaID )
{
	GASSERT(dwPartyID<m_dwMaxClient);
	GASSERT(dwGaeaID<m_dwMaxClient);
	if ( dwGaeaID>=m_dwMaxClient )		return FALSE;
	if ( dwPartyID>=m_dwMaxClient )		return FALSE;

	return m_pParty[dwPartyID].ISMASTER ( dwGaeaID );
}

//	Note : GaeaID는 유효하다고 가정함.
//
GLPARTY_FIELD* GLPartyFieldMan::NewParty ( DWORD dwMaster, DWORD dwPartyID )
{
	GASSERT(dwPartyID<m_dwMaxClient);
	GASSERT(dwMaster<m_dwMaxClient);
	if ( dwPartyID>=m_dwMaxClient )		return NULL;
	if ( dwMaster>=m_dwMaxClient )		return NULL;

	m_pParty[dwPartyID].RESET();

	SetPartyID ( dwMaster, dwPartyID );

	m_pParty[dwPartyID].m_dwPARTYID = dwPartyID;
	m_pParty[dwPartyID].SETMASTER ( dwMaster, dwPartyID );

	return &(m_pParty[dwPartyID]);
}

void GLPartyFieldMan::DelParty ( DWORD dwPartyID )
{
	GASSERT(dwPartyID<m_dwMaxClient);
	if ( dwPartyID>=m_dwMaxClient )		return;

	GLPARTY_FIELD *pParty = GetParty ( dwPartyID );
	if ( !pParty )		return;

	GLPARTY_FIELD::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPARTY_FIELD::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();

	for ( ; iter!=iter_end; ++iter )	ReSetPartyID ( (*iter).first );

	//	Note : 파티 정보를 리셋.
	//
	m_pParty[dwPartyID].RESET();
}

void GLPartyFieldMan::DelPartyAll ()
{
	for ( DWORD i=0; i<m_dwMaxClient; ++i )
	{
		DelParty(i);
	}
}

void GLPartyFieldMan::SendMsgToMember ( DWORD dwPartyID, NET_MSG_GENERIC *nmg )
{
	GASSERT(dwPartyID<m_dwMaxClient);
	if ( dwPartyID>=m_dwMaxClient )		return;

	GLPARTY_FIELD *pParty = GetParty ( dwPartyID );
	if ( !pParty )		return;

	GLPARTY_FIELD::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPARTY_FIELD::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();

	for ( ; iter!=iter_end; ++iter )
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( (*iter).first );
		if ( !pChar )											   continue;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, nmg );
	}
}

BOOL GLPartyFieldMan::MsgPartyFNew ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_FNEW *pNetMsg = ( GLMSG::SNET_PARTY_FNEW * ) nmg;

	GLPARTY_FIELD* pParty = NewParty ( pNetMsg->dwMaster, pNetMsg->dwPartyID );
	if ( !pParty )		return FALSE;

	pParty->m_sOPTION = pNetMsg->sOption;

	GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;

	for ( DWORD i=0; i<pNetMsg->dwPartyNum; ++i )
	{
		GLPARTY_FNET &sPartyInfo = pNetMsg->sPartyInfo[i];

		pParty->ADDMEMBER ( sPartyInfo );

		//	Note : 현제 필드 서버에 존제하는 케릭터의 경우 파티 ID 설정.
		//
		SetPartyID ( sPartyInfo.m_dwGaeaID, pNetMsg->dwPartyID );

		//	Note : 파티 생성 정보 클라이언트에 전송.
		//
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( sPartyInfo.m_dwGaeaID );
		if ( pChar )
		{
			GLGaeaServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, pNetMsg );
			
			//	Note : 파티에 참가하는 케릭 주위(플레이어)에 파티소속 정보를 전송.			
			NetMsgPartyBrd.dwGaeaID = pChar->m_dwGaeaID;
			NetMsgPartyBrd.dwPartyID = pParty->m_dwPARTYID;
			NetMsgPartyBrd.dwPMasterID = pParty->m_dwMASTER;
			pChar->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );
		}

		//	Note : 파티 멤버간 종전 적대행위 리스트 삭제.
		//
		PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( sPartyInfo.m_dwGaeaID );
		if ( pCHAR )
		{
			for ( DWORD j=0; j<pNetMsg->dwPartyNum; ++j )
			{
				GLPARTY_FNET &sMember = pNetMsg->sPartyInfo[j];
				PGLCHAR pMEMBER = GLGaeaServer::GetInstance().GetChar ( sMember.m_dwGaeaID );
				if ( pMEMBER )
				{
					pCHAR->DelPlayHostile ( pMEMBER->m_dwCharID );
				}
			}
		}
	}

	//	Note : 각각 파티원의 세부-정보를 전송해줌.
	//
	DWORD dwMemberID = 0;
	PGLCHAR pChar = NULL;
	GLMSG::SNET_PARTY_MBR_DETAIL NetMsg;

	for ( DWORD i=0; i<pNetMsg->dwPartyNum; ++i )
	{
		dwMemberID = pNetMsg->sPartyInfo[i].m_dwGaeaID;
		pChar = GLGaeaServer::GetInstance().GetChar ( dwMemberID );
		if ( !pChar )	continue;
		
		NetMsg.dwGaeaID = dwMemberID;
		NetMsg.vPos = pChar->GetPosition();
		NetMsg.sHP = pChar->m_sHP;
		NetMsg.sMP = pChar->m_sMP;

		SendMsgToMember ( pNetMsg->dwPartyID, (NET_MSG_GENERIC *) &NetMsg );
	}

	return TRUE;
}

BOOL GLPartyFieldMan::MsgPartyAdd ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_ADD *pNetMsg = ( GLMSG::SNET_PARTY_ADD * ) nmg;

	GLPARTY_FIELD *pParty = GetParty ( pNetMsg->dwPartyID );
	if ( !pParty )		return FALSE;
	
	GLPARTY_FNET &sPartyInfo = pNetMsg->sPartyInfo;

	pParty->ADDMEMBER ( sPartyInfo );

	//	Note : 현재 필드 서버에 존재하는 케릭터의 경우 파티 ID 설정.
	//
	SetPartyID ( sPartyInfo.m_dwGaeaID, pNetMsg->dwPartyID );

	PGLCHAR pCharAdd = GLGaeaServer::GetInstance().GetChar ( sPartyInfo.m_dwGaeaID );
	if ( pCharAdd )
	{
		//	Note : 새로 추가된 구성원에게 파티 정보 설정.
		//
		GLMSG::SNET_PARTY_FNEW NetPartyInfo;
		NetPartyInfo.SETINFO ( pNetMsg->dwPartyID, pParty->m_dwMASTER );
		NetPartyInfo.sOption = pParty->m_sOPTION;

		GLPARTY_FIELD::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
		GLPARTY_FIELD::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLPARTY_FNET sPartyInfo = (*iter).second;
			NetPartyInfo.ADDMEMBER ( sPartyInfo );
		}
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pCharAdd->m_dwClientID, &NetPartyInfo );

		//	Note : 파티에 참가하는 케릭 주위(플레이어)에 파티소속 정보를 전송.
		//
		GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
		NetMsgPartyBrd.dwGaeaID = pCharAdd->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = pParty->m_dwPARTYID;
		NetMsgPartyBrd.dwPMasterID = pParty->m_dwMASTER;
		pCharAdd->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );

		// 현재 파티의 QBox 상태를 보낸다.
		GLMSG::SNET_QBOX_OPTION_MEMBER NetMsg;
		NetMsg.bQBoxEnable = pParty->m_bQBoxEnable;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pCharAdd->m_dwClientID, &NetMsg );	

	}

	//	Note : 다른 구성원에게 추가된 사람의 정보를 전송.
	//
	SendMsgToMember ( pNetMsg->dwPartyID, (NET_MSG_GENERIC*) pNetMsg );

	//	Note : 추가된 사람의 세부-정보를 멤버에게 전송.
	//
	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( sPartyInfo.m_dwGaeaID );
	if ( pChar )
	{
		// 추가된 사람의 정보를 멤버들에게 전송
		GLMSG::SNET_PARTY_MBR_DETAIL NetMsg;
		NetMsg.dwGaeaID = sPartyInfo.m_dwGaeaID;
		NetMsg.vPos = pChar->GetPosition();
		NetMsg.sHP = pChar->m_sHP;
		NetMsg.sMP = pChar->m_sMP;

		SendMsgToMember ( pNetMsg->dwPartyID, (NET_MSG_GENERIC *) &NetMsg );

		//	Note : 다른 맴버들의 정보를 새로 추가되는 맴버에게 전송.
		//
		GLPARTY_FIELD::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
		GLPARTY_FIELD::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();

		PGLCHAR pCharMem = NULL;		

		for ( ; iter!=iter_end; ++iter )
		{
			pCharMem = GLGaeaServer::GetInstance().GetChar ( (*iter).first );
			if ( !pCharMem )	continue;

			NetMsg.dwGaeaID = pCharMem->m_dwGaeaID;
			NetMsg.vPos = pCharMem->GetPosition();
			NetMsg.sHP = pCharMem->m_sHP;
			NetMsg.sMP = pCharMem->m_sMP;

			GLGaeaServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );

			//	Note : 파티 멤버간 종전 적대행위 리스트 삭제.
			//
			pCharMem->DelPlayHostile ( pChar->m_dwCharID );
			pChar->DelPlayHostile ( pCharMem->m_dwCharID );
		}
	}

	return TRUE;
}

BOOL GLPartyFieldMan::MsgPartyDel ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_DEL *pNetMsg = ( GLMSG::SNET_PARTY_DEL * ) nmg;

	GLPARTY_FIELD *pParty = GetParty ( pNetMsg->dwPartyID );
	if ( !pParty )		return FALSE;

	pParty->DELMEMBER ( pNetMsg->dwDelMember );

	//	Note : 현재 필드 서버에 존제하는 케릭터의 경우 파티 ID 제거.
	//
	ReSetPartyID ( pNetMsg->dwDelMember );

	//	Note : 파티에서 탈퇴하는 케릭 주위(플레이어)에 파티소속 정보를 전송.
	//
	PGLCHAR pCharMem = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwDelMember );
	if ( pCharMem )
	{
		GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
		NetMsgPartyBrd.dwGaeaID = pCharMem->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = GAEAID_NULL;		//	파티 소속 제거.
		NetMsgPartyBrd.dwPMasterID = GAEAID_NULL;
		pCharMem->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );
	}

	return TRUE;
}

BOOL GLPartyFieldMan::MsgPartyDissolve ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_DISSOLVE *pNetMsg = ( GLMSG::SNET_PARTY_DISSOLVE * ) nmg;

	GLPARTY_FIELD *pParty = GetParty ( pNetMsg->dwPartyID );
	if ( !pParty )		return FALSE;

	//	Note : 파티 해체시에 케릭 주위(플레이어)에 파티소속 정보를 전송.
	//
	GLPARTY_FIELD::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPARTY_FIELD::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();
	for ( ; iter!=iter_end; ++iter )
	{
		PGLCHAR pCharMem = GLGaeaServer::GetInstance().GetChar ( (*iter).first );
		if ( !pCharMem )	continue;

		GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
		NetMsgPartyBrd.dwGaeaID = pCharMem->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = GAEAID_NULL;		//	파티 소속 제거.
		NetMsgPartyBrd.dwPMasterID = GAEAID_NULL;
		pCharMem->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );
	}

	//	Note : 파티를 제거하고 파티원에 설정된 파티 ID 제거.
	//
	DelParty ( pNetMsg->dwPartyID );

	return TRUE;
}

BOOL GLPartyFieldMan::MsgPartyAuthority( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_AUTHORITY *pNetMsg = ( GLMSG::SNET_PARTY_AUTHORITY * ) nmg;

	GLPARTY_FIELD *pParty = GetParty ( pNetMsg->dwPartyID );
	if ( !pParty )		return FALSE;

	DWORD dwOldMaster = pParty->GETMASTER();

	pParty->SETMASTER ( pNetMsg->dwGaeaID, pNetMsg->dwPartyID );

	// 새로운 파티마스터의 변경을 알림
	GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
	PGLCHAR pOldMaster = GLGaeaServer::GetInstance().GetChar (dwOldMaster );
	if ( pOldMaster )
	{
		NetMsgPartyBrd.dwGaeaID = pOldMaster->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = pParty->m_dwPARTYID;
		NetMsgPartyBrd.dwPMasterID = pNetMsg->dwGaeaID;
		pOldMaster->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );
	}

	PGLCHAR pNewMaster = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwGaeaID );
	if ( pNewMaster )
	{
		NetMsgPartyBrd.dwGaeaID = pNewMaster->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = pParty->m_dwPARTYID;
		NetMsgPartyBrd.dwPMasterID = pNetMsg->dwGaeaID;
		pNewMaster->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );
	}

	//SendMsgToMember ( pNetMsg->dwPartyID, nmg );

	return TRUE;    
}

BOOL GLPartyFieldMan::MsgPartyMbrMoveMap ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_MBR_MOVEMAP *pNetMsg = ( GLMSG::SNET_PARTY_MBR_MOVEMAP * ) nmg;
	if ( pNetMsg->dwPartyID == PARTY_NULL )		return FALSE;

	//	Note : 파티원 정보 수정.
	//
	GLPARTY_FIELD *pParty = GetParty ( pNetMsg->dwPartyID );
	if ( !pParty )		return FALSE;

	pParty->MBR_MOVEMAP ( pNetMsg->dwGaeaID, pNetMsg->sMapID );

	//	Note : 만약 맵이동한 케릭터가 이동한 이 서버에 존제할때.
	//
	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwGaeaID );
	if ( pChar )
	{
		//	Note : 파티원들에게 맵이동을 알려줌.
		//
		SendMsgToMember ( pNetMsg->dwPartyID, (NET_MSG_GENERIC *) pNetMsg );

		//	Note : 파티원들에게 세부 정보 알림. ( pos, hp, mp )
		//
		GLMSG::SNET_PARTY_MBR_DETAIL NetMsg;
		NetMsg.dwGaeaID = pNetMsg->dwGaeaID;
		NetMsg.vPos = pChar->GetPosition();
		NetMsg.sHP = pChar->m_sHP;
		NetMsg.sMP = pChar->m_sMP;

		SendMsgToMember ( pNetMsg->dwPartyID, (NET_MSG_GENERIC *) &NetMsg );
	}

	return TRUE;
}

BOOL GLPartyFieldMan::MsgPartyMbrRename ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_MEMBER_RENAME_FLD *pNetMsg = ( GLMSG::SNET_PARTY_MEMBER_RENAME_FLD * ) nmg;
	if ( pNetMsg->dwPartyID == PARTY_NULL )		return FALSE;

	//	Note : 파티원 정보 수정.
	//
	GLPARTY_FIELD* pParty = GetParty ( pNetMsg->dwPartyID );
	if ( !pParty )		return FALSE;

	const GLPARTY_FNET* pPartyMember = pParty->GETMEMBER ( pNetMsg->dwGaeaID );
	if ( pPartyMember )
	{
		GLPARTY_FNET sRenameMember = *pPartyMember;
		pParty->DELMEMBER ( pNetMsg->dwGaeaID );
		StringCchCopy ( sRenameMember.m_szName, CHAR_SZNAME, pNetMsg->szName );
		pParty->ADDMEMBER ( sRenameMember );
	}

	PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwGaeaID );
	if ( pCHAR )
	{
		GLGaeaServer::GetInstance().ChangeNameMap ( pCHAR, pCHAR->m_szName, pNetMsg->szName );
	}

	return TRUE;
}

BOOL GLPartyFieldMan::MsgPartyMasterRenew ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_PARTY_MASTER_RENEW* pNetMsg = ( GLMSG::SNET_PARTY_MASTER_RENEW* ) nmg;
	if ( pNetMsg->dwPartyID == PARTY_NULL )		return FALSE;

	GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;

	GLPARTY_FIELD* pParty = GetParty ( pNetMsg->dwPartyID );
	if ( !pParty )		return FALSE;

	// 기존 마스터 삭제
	pParty->DELMEMBER ( pNetMsg->dwDelMaster );

	// 새로운 마스터 설정
	pParty->SETMASTER ( pNetMsg->dwNewMaster, pNetMsg->dwPartyID );

	// 마스터가 바뀌었을 경우 QBox 옵션을 TRUE로 변경한다.
	pParty->m_bQBoxEnable = TRUE;

	//	Note : 현재 필드 서버에 존재하는 케릭터의 경우 파티 ID 제거.
	//
	ReSetPartyID ( pNetMsg->dwDelMaster );

	// 사라진 파티마스터의 탈퇴를 알림
	PGLCHAR pDelMaster = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwDelMaster );
	if ( pDelMaster )
	{
		NetMsgPartyBrd.dwGaeaID = pDelMaster->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = GAEAID_NULL;					//	파티 소속 제거.
		NetMsgPartyBrd.dwPMasterID = GAEAID_NULL;
		pDelMaster->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );
	}

	// 새로운 파티마스터의 변경을 알림
	PGLCHAR pNewMaster = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwNewMaster );
	if ( pNewMaster )
	{
		NetMsgPartyBrd.dwGaeaID = pNewMaster->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = pParty->m_dwPARTYID;
		NetMsgPartyBrd.dwPMasterID = pNewMaster->m_dwGaeaID;
		pNewMaster->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );
	}

	SendMsgToMember ( pNetMsg->dwPartyID, nmg );

	return TRUE;
}

HRESULT GLPartyFieldMan::MsgProcess ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_PARTY_FNEW:
		MsgPartyFNew ( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_ADD:
		MsgPartyAdd ( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_DEL:
		MsgPartyDel ( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_DISSOLVE:
		MsgPartyDissolve ( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_AUTHORITY:
		MsgPartyAuthority( nmg );
		break;

	case NET_MSG_GCTRL_PARTY_MBR_MOVEMAP:
		MsgPartyMbrMoveMap ( nmg );
		break;
	case NET_MSG_GCTRL_PARTY_MBR_RENAME_FLD:
		MsgPartyMbrRename ( nmg );
		break;
	case NET_MSG_GCTRL_PARTY_MASTER_RENEW:
		MsgPartyMasterRenew ( nmg );
		break;
	};

	return S_OK;
}



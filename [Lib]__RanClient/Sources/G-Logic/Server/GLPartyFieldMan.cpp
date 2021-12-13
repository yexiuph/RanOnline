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

	//	Note : ���� �ʵ� ������ �����ϴ� �ɸ����� ��� ��Ƽ ID ����.
	//
	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwGaeaID );
	if ( pChar )	pChar->SetPartyID ( dwPartyID );
}

void GLPartyFieldMan::ReSetPartyID ( DWORD dwGaeaID )
{
	GASSERT(dwGaeaID<m_dwMaxClient);
	if ( dwGaeaID>=m_dwMaxClient )		return;

	m_pGaeaTOParty[dwGaeaID] = PARTY_NULL;

	//	Note : ���� �ʵ� ������ �����ϴ� �ɸ����� ��� ��Ƽ ID ����.
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

//	Note : GaeaID�� ��ȿ�ϴٰ� ������.
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

	//	Note : ��Ƽ ������ ����.
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

		//	Note : ���� �ʵ� ������ �����ϴ� �ɸ����� ��� ��Ƽ ID ����.
		//
		SetPartyID ( sPartyInfo.m_dwGaeaID, pNetMsg->dwPartyID );

		//	Note : ��Ƽ ���� ���� Ŭ���̾�Ʈ�� ����.
		//
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( sPartyInfo.m_dwGaeaID );
		if ( pChar )
		{
			GLGaeaServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, pNetMsg );
			
			//	Note : ��Ƽ�� �����ϴ� �ɸ� ����(�÷��̾�)�� ��Ƽ�Ҽ� ������ ����.			
			NetMsgPartyBrd.dwGaeaID = pChar->m_dwGaeaID;
			NetMsgPartyBrd.dwPartyID = pParty->m_dwPARTYID;
			NetMsgPartyBrd.dwPMasterID = pParty->m_dwMASTER;
			pChar->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );
		}

		//	Note : ��Ƽ ����� ���� �������� ����Ʈ ����.
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

	//	Note : ���� ��Ƽ���� ����-������ ��������.
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

	//	Note : ���� �ʵ� ������ �����ϴ� �ɸ����� ��� ��Ƽ ID ����.
	//
	SetPartyID ( sPartyInfo.m_dwGaeaID, pNetMsg->dwPartyID );

	PGLCHAR pCharAdd = GLGaeaServer::GetInstance().GetChar ( sPartyInfo.m_dwGaeaID );
	if ( pCharAdd )
	{
		//	Note : ���� �߰��� ���������� ��Ƽ ���� ����.
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

		//	Note : ��Ƽ�� �����ϴ� �ɸ� ����(�÷��̾�)�� ��Ƽ�Ҽ� ������ ����.
		//
		GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
		NetMsgPartyBrd.dwGaeaID = pCharAdd->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = pParty->m_dwPARTYID;
		NetMsgPartyBrd.dwPMasterID = pParty->m_dwMASTER;
		pCharAdd->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );

		// ���� ��Ƽ�� QBox ���¸� ������.
		GLMSG::SNET_QBOX_OPTION_MEMBER NetMsg;
		NetMsg.bQBoxEnable = pParty->m_bQBoxEnable;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pCharAdd->m_dwClientID, &NetMsg );	

	}

	//	Note : �ٸ� ���������� �߰��� ����� ������ ����.
	//
	SendMsgToMember ( pNetMsg->dwPartyID, (NET_MSG_GENERIC*) pNetMsg );

	//	Note : �߰��� ����� ����-������ ������� ����.
	//
	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( sPartyInfo.m_dwGaeaID );
	if ( pChar )
	{
		// �߰��� ����� ������ ����鿡�� ����
		GLMSG::SNET_PARTY_MBR_DETAIL NetMsg;
		NetMsg.dwGaeaID = sPartyInfo.m_dwGaeaID;
		NetMsg.vPos = pChar->GetPosition();
		NetMsg.sHP = pChar->m_sHP;
		NetMsg.sMP = pChar->m_sMP;

		SendMsgToMember ( pNetMsg->dwPartyID, (NET_MSG_GENERIC *) &NetMsg );

		//	Note : �ٸ� �ɹ����� ������ ���� �߰��Ǵ� �ɹ����� ����.
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

			//	Note : ��Ƽ ����� ���� �������� ����Ʈ ����.
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

	//	Note : ���� �ʵ� ������ �����ϴ� �ɸ����� ��� ��Ƽ ID ����.
	//
	ReSetPartyID ( pNetMsg->dwDelMember );

	//	Note : ��Ƽ���� Ż���ϴ� �ɸ� ����(�÷��̾�)�� ��Ƽ�Ҽ� ������ ����.
	//
	PGLCHAR pCharMem = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwDelMember );
	if ( pCharMem )
	{
		GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
		NetMsgPartyBrd.dwGaeaID = pCharMem->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = GAEAID_NULL;		//	��Ƽ �Ҽ� ����.
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

	//	Note : ��Ƽ ��ü�ÿ� �ɸ� ����(�÷��̾�)�� ��Ƽ�Ҽ� ������ ����.
	//
	GLPARTY_FIELD::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPARTY_FIELD::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();
	for ( ; iter!=iter_end; ++iter )
	{
		PGLCHAR pCharMem = GLGaeaServer::GetInstance().GetChar ( (*iter).first );
		if ( !pCharMem )	continue;

		GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
		NetMsgPartyBrd.dwGaeaID = pCharMem->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = GAEAID_NULL;		//	��Ƽ �Ҽ� ����.
		NetMsgPartyBrd.dwPMasterID = GAEAID_NULL;
		pCharMem->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );
	}

	//	Note : ��Ƽ�� �����ϰ� ��Ƽ���� ������ ��Ƽ ID ����.
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

	// ���ο� ��Ƽ�������� ������ �˸�
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

	//	Note : ��Ƽ�� ���� ����.
	//
	GLPARTY_FIELD *pParty = GetParty ( pNetMsg->dwPartyID );
	if ( !pParty )		return FALSE;

	pParty->MBR_MOVEMAP ( pNetMsg->dwGaeaID, pNetMsg->sMapID );

	//	Note : ���� ���̵��� �ɸ��Ͱ� �̵��� �� ������ �����Ҷ�.
	//
	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwGaeaID );
	if ( pChar )
	{
		//	Note : ��Ƽ���鿡�� ���̵��� �˷���.
		//
		SendMsgToMember ( pNetMsg->dwPartyID, (NET_MSG_GENERIC *) pNetMsg );

		//	Note : ��Ƽ���鿡�� ���� ���� �˸�. ( pos, hp, mp )
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

	//	Note : ��Ƽ�� ���� ����.
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

	// ���� ������ ����
	pParty->DELMEMBER ( pNetMsg->dwDelMaster );

	// ���ο� ������ ����
	pParty->SETMASTER ( pNetMsg->dwNewMaster, pNetMsg->dwPartyID );

	// �����Ͱ� �ٲ���� ��� QBox �ɼ��� TRUE�� �����Ѵ�.
	pParty->m_bQBoxEnable = TRUE;

	//	Note : ���� �ʵ� ������ �����ϴ� �ɸ����� ��� ��Ƽ ID ����.
	//
	ReSetPartyID ( pNetMsg->dwDelMaster );

	// ����� ��Ƽ�������� Ż�� �˸�
	PGLCHAR pDelMaster = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwDelMaster );
	if ( pDelMaster )
	{
		NetMsgPartyBrd.dwGaeaID = pDelMaster->m_dwGaeaID;
		NetMsgPartyBrd.dwPartyID = GAEAID_NULL;					//	��Ƽ �Ҽ� ����.
		NetMsgPartyBrd.dwPMasterID = GAEAID_NULL;
		pDelMaster->SendMsgViewAround ( (NET_MSG_GENERIC*)&NetMsgPartyBrd );
	}

	// ���ο� ��Ƽ�������� ������ �˸�
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



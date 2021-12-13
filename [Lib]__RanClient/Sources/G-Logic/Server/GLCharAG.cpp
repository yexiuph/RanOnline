#include "pch.h"
#include "./GLCharAG.h"

#include "./DbActionLogic.h"
#include "./GLAgentServer.h"
#include "./GLItemLMT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLCharAG::GLCharAG(void) :
	m_dwClientID(0),
	m_nChannel(0),
	m_dwGaeaID(0),
	m_dwPartyID(PARTY_NULL),

	m_dwCurFieldSvr(FIELDSERVER_MAX),
	m_pPCNode(NULL),

	m_dwActState(NULL),

	m_sNextMapID(MAXLANDMID,MAXLANDSID),
	m_dwNextGateID(0),
	m_vNextPos(FLT_MAX,FLT_MAX,FLT_MAX),
	m_dwNextFieldSvr(FIELDSERVER_MAX),

	m_bFIELDCK_OK(false),
	m_dwFIELDCK_NUM(0),
	m_dwFIELDCK_BEING(0),
	m_bViewWhisperMSG(FALSE),
	m_fFRIEND_TIMER(0.0f),

	m_bFriendWindowOpen(false),
	m_bClubWindowOpen(false),

	m_bReciveEventTime(false),
	m_bTracingUser(false),
	m_dwComboAttend( 0 ),
	m_fCyberClassCheck(0.0f),
	m_tLoginTime(0)
{
	for ( int c=0; c<MAX_CHANNEL_NUMBER; ++c )
	{
		for ( int i=0; i<FIELDSERVER_MAX; ++i )		m_bFIELD_CK[c][i] = false;
	}

	m_vecGetWhisperList.clear();
	m_mapFriend.clear();
	m_vecAttend.clear();
}

GLCharAG::~GLCharAG(void)
{
	m_mapFriend.clear();
	m_vecAttend.clear();
}

HRESULT GLCharAG::CreateChar ( GLCHARAG_DATA *pchar_data )
{
	GLCHARAG_DATA::operator = ( *pchar_data );

	return S_OK;
}

//	������ ���� ĳ�������� �˻�.
//	(������ ���� ĳ���ʹ� �ʵ弭������ ������ ��ȿ�ؾ��Ѵ�.)
bool GLCharAG::IsDoneFieldCheck ()
{
	return m_bFIELDCK_OK;
}

//	�ʵ忡 �ɸ��͸� �������� �ʵ��� char ���Ἲ ���� �׽�Ʈ �Ϸ� ���� ����.
//	( �Ϸ��Ĵ� ���ѹ��� ȣ��Ǿ���. )
//
bool GLCharAG::IsFieldCheckComplete ( bool *pFieldSvr )
{
	if ( m_bFIELDCK_OK )
	{
		GLAgentServer::GetInstance().DEBUGMSG_WRITE ( "[error] �̹� ������ ���� �ɸ��͸� �ٽ� ���� �Ͽ����ϴ�." );
		return false;
	}

	for ( int c=0; c<MAX_CHANNEL_NUMBER; ++c )
	{
		for ( int i=0; i<FIELDSERVER_MAX; ++i )
		{
			if ( m_bFIELD_CK[c][i]!=pFieldSvr[c*MAX_CHANNEL_NUMBER+i] )	return false;
		}
	}

	m_bFIELDCK_OK = true;

	return true;
}

void GLCharAG::SetCurrentField ( DWORD dwSvr, SNATIVEID &sNID )
{
	if ( m_dwPartyID!=PARTY_NULL && m_sCurMapID!=sNID )
	{
		//	Note : ��Ƽ������ ���̵��� �˸�.
		//
		GLMSG::SNET_PARTY_MBR_MOVEMAP NetMsgMap;
		NetMsgMap.dwPartyID = m_dwPartyID;
		NetMsgMap.dwGaeaID = m_dwGaeaID;
		NetMsgMap.sMapID = sNID;

		GLAgentServer::GetInstance().SENDTOPARTY ( m_dwPartyID, &NetMsgMap );

		//	Note : ���� �ʵ� ������ ��Ƽ ������� ���� ������ �˸�.
		//
		GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgMap );
	}

	m_dwCurFieldSvr = dwSvr;

	GLAGLandMan* pOLD_LAND = GLAgentServer::GetInstance().GetByMapID ( m_sCurMapID );
	if ( pOLD_LAND )	pOLD_LAND->DropOutPC ( m_dwGaeaID );

	GLAGLandMan* pNEW_LAND = GLAgentServer::GetInstance().GetByMapID ( sNID );
	if ( pNEW_LAND )	pNEW_LAND->DropPC ( m_dwGaeaID );

	m_sCurMapID = sNID;
}

void GLCharAG::SetNextFieldSvr ( SNATIVEID sMID, DWORD dwGateID, D3DXVECTOR3 vPos, DWORD dwFieldSvr )
{
	m_sNextMapID = sMID;
	m_dwNextGateID = dwGateID;
	m_vNextPos = vPos;

	m_dwNextFieldSvr = dwFieldSvr;
}

void GLCharAG::ResetNextFieldSvr ()
{
	m_sNextMapID = SNATIVEID(MAXLANDMID,MAXLANDSID);
	m_dwNextGateID = 0;

	m_dwNextFieldSvr = FIELDSERVER_MAX;
}

//	�ֱ� ��� ���� ��û.
const SCONFT_LOG* GLCharAG::GETCONFRONT_LOG ( DWORD dwCHARID )
{
	CONFT_MAP_ITER iter = m_mapCONFT.find ( dwCHARID );
	if ( iter!= m_mapCONFT.end() )
	{
		const SCONFT_LOG &sCONFT = (*iter).second;

		return &sCONFT;
	}

	return NULL;
}

void GLCharAG::SaveLogConfront ()
{
	//	Note : ��� �α� ����.
	if ( m_sCONFTING.IsCONFRONTING() )
	{
		if ( m_sCONFTING.emTYPE == EMCONFT_ONE )
		{
			SCONFT_LOG sCONFT_LOG(0,0.0f);
			CONFT_MAP_ITER iter = m_mapCONFT.find ( m_sCONFTING.dwCHARID );
			if ( iter!=m_mapCONFT.end() )
			{
				sCONFT_LOG = (*iter).second;
				m_mapCONFT.erase ( iter );
			}

			sCONFT_LOG.dwCOUNT++;
			sCONFT_LOG.fTIMER = 0.0f;

			m_mapCONFT[m_sCONFTING.dwCHARID] = sCONFT_LOG;
		}
	}
}

void GLCharAG::ResetConfront ( EMCONFRONT_END emEND )
{
	SaveLogConfront ();

	m_sCONFTING.RESET ();

	GLMSG::SNETPC_CONFRONT_END2_CLT NetMsgClt;
	NetMsgClt.emEND = emEND;
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgClt );

	GLMSG::SNETPC_CONFRONT_END2_FLD NetMsgFld;
	NetMsgFld.emEND = emEND;
	GLAgentServer::GetInstance().SENDTOFIELD ( m_dwClientID, &NetMsgFld );
}

void GLCharAG::ResetConfrontOk ()
{
	if ( m_sCONFTING.IsCONFRONTING() )
	{
		SaveLogConfront ();
		m_sCONFTING.RESET ();
	}
}

void GLCharAG::StartConfront ( EMCONFT_TYPE emType, DWORD dwTAR_ID, const SCONFT_OPTION &sOption )
{
	DWORD dwCHARID(0);
	PGLCHARAG pCHAR = GLAgentServer::GetInstance().GetChar ( dwTAR_ID );
	if ( pCHAR )	dwCHARID = pCHAR->m_dwCharID;


	m_sCONFTING.RESET ();

	m_sCONFTING.dwTAR_ID = dwTAR_ID;
	m_sCONFTING.dwCHARID = dwCHARID;
	m_sCONFTING.emTYPE = emType;
	m_sCONFTING.fTIMER = 0.0f;
	m_sCONFTING.sOption = sOption;

	//	Note : �ʵ� ������ �˸�.	(�ڽ�)
	//
	GLMSG::SNETPC_CONFRONT_START2_FLD NetMsgFld;
	NetMsgFld.emTYPE = emType;
	NetMsgFld.dwTARID = dwTAR_ID;
	NetMsgFld.sOption = sOption;
	GLAgentServer::GetInstance().SENDTOFIELD ( m_dwClientID, &NetMsgFld );
}

bool GLCharAG::IsCHATBLOCK ()
{
	if ( m_tCHATBLOCK==0 )	return false;

	CTime cBLOCK(m_tCHATBLOCK);
	CTime cCUR(CTime::GetCurrentTime());

	//	���ð����� �˻�.
	if ( cBLOCK > cCUR )	return true;

	//	�ð��� �ʰ� �Ǿ��� ���� �ð��� ����.
	m_tCHATBLOCK = 0;

	return false;
}

HRESULT GLCharAG::FrameMove ( float fTime, float fElapsedTime )
{
#if defined(TH_PARAM)
	if( m_dwThaiCCafeClass == 1 || m_dwThaiCCafeClass == 2 || m_dwThaiCCafeClass == 3 )
	{		
		if( m_fCyberClassCheck >= 3600 )
		{
			m_fCyberClassCheck = 0.0f;
			CThaiCafeClassCheck *pDbAction = new CThaiCafeClassCheck( m_dwClientID, m_dwThaiCCafeClass, m_szIp );
			GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
		}else{
			m_fCyberClassCheck += fElapsedTime;
		}
	}
#elif defined(MYE_PARAM ) || defined(MY_PARAM)
	if( m_nMyCCafeClass == 1 || m_nMyCCafeClass == 2 || m_nMyCCafeClass == 3 )
	{		
		if( m_fCyberClassCheck >= 3600 )
		{
			m_fCyberClassCheck = 0.0f;
			CMyCafeClassCheck *pDbAction = new CMyCafeClassCheck( m_dwClientID, m_nMyCCafeClass, m_szIp );
			GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
		}else{
			m_fCyberClassCheck += fElapsedTime;
		}
	}
#endif

	if ( !m_mapCONFT.empty() )
	{
		CONFT_MAP_ITER iter = m_mapCONFT.begin();
		for ( ; iter!=m_mapCONFT.end(); ++iter )
		{
			SCONFT_LOG &sCONFT = (*iter).second;

			//	Note : ���� �ð� �̻� ������ �� ��� Ƚ�� ����.
			//
			sCONFT.fTIMER += fElapsedTime;
			if ( sCONFT.fTIMER > GLCONST_CHAR::fCONFRONT_RESET )
			{
				sCONFT.dwCOUNT = 0;
			}
		}
	}

	if ( m_sCONFTING.IsCONFRONTING() )
	{
		//	Note : ��� ��ȿ�� �˻�.
		//
		bool bvalid(false);
		PGLCHARAG pTAR_ONE(NULL);
		switch ( m_sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			{
				pTAR_ONE = GLAgentServer::GetInstance().GetChar ( m_sCONFTING.dwTAR_ID );
				if ( pTAR_ONE )
				{
					const SCONFTING& sTAR_CONFT = pTAR_ONE->GETCONFRONTING();
					if ( m_sCONFTING.IsSAMENESS(sTAR_CONFT.emTYPE,pTAR_ONE->m_dwGaeaID) )		bvalid = true;
				}
			}
			break;

		case EMCONFT_PARTY:
			{
				bool bpartyA(false), bpartyB(false);
				if ( PARTY_NULL!=m_dwPartyID && GLAgentServer::GetInstance().GetParty(m_dwPartyID) )						bpartyA = true;
				if ( PARTY_NULL!=m_sCONFTING.dwTAR_ID && GLAgentServer::GetInstance().GetParty(m_sCONFTING.dwTAR_ID) )	bpartyB = true;

				if ( bpartyA && bpartyB )	bvalid = true;
			}
			break;

		case EMCONFT_GUILD:
			{
				GLClubMan &sClubMan = GLAgentServer::GetInstance().GetClubMan();

				bool bclubA(false), bclubB(false);
				if ( CLUB_NULL!=m_dwGuild && sClubMan.GetClub(m_dwGuild) )							bclubA = true;
				if ( CLUB_NULL!=m_sCONFTING.dwTAR_ID && sClubMan.GetClub(m_sCONFTING.dwTAR_ID) )	bclubB = true;

				if ( bclubA && bclubB )		bvalid = true;
			}
			break;
		};

		if ( !bvalid )
		{
			//	Note : �Ϲ� ������ ���� ��� ���.
			//
			ResetConfront ( EMCONFRONT_END_FAIL );
		}
		else
		{
			switch ( m_sCONFTING.emTYPE )
			{
			case EMCONFT_ONE:
				//	Note : �ð� ���� �˻�.
				//
				m_sCONFTING.fTIMER += fElapsedTime;
				if ( m_sCONFTING.fTIMER > GLCONST_CHAR::fCONFRONT_TIME )
				{
					//	Note : �ڽſ��� ��� ��� �˸�.
					//
					ResetConfront ( EMCONFRONT_END_TIME );

					//	Note : ���濡�Ե� ��� ��� �˸�.
					//
					pTAR_ONE->ResetConfront ( EMCONFRONT_END_TIME );
				}

				if ( m_sCurMapID != pTAR_ONE->m_sCurMapID )
				{
					//	Note : �ڽſ��� ���̵����� ��� ��� �˸�.
					//
					ResetConfront ( EMCONFRONT_END_MOVEMAP );

					//	Note : ���濡�Ե� ���̵����� ��� ��� �˸�.
					//
					pTAR_ONE->ResetConfront ( EMCONFRONT_END_MOVEMAP );
				}
				break;

			case EMCONFT_PARTY:
				break;

			case EMCONFT_GUILD:
				break;
			};
		}
	}

	m_fFRIEND_TIMER += fElapsedTime;
	if ( m_fFRIEND_TIMER > 2.f )
	{
		m_fFRIEND_TIMER = 0.0f;

		//	Note : ģ�� on/off ���� ����.
		//
		PGLCHARAG pCHAR = NULL;
		GLMSG::SNETPC_REQ_FRIENDSTATE NetMsg;
		MAPFRIEND_ITER iter = m_mapFriend.begin();
		for ( ; iter!=m_mapFriend.end(); ++iter )
		{
			SFRIEND &cFRIEND = (*iter).second;
			//if( !pFRIEND ) continue;
			if ( !cFRIEND.IsVALID() || cFRIEND.IsBETRAY() )	continue;

			bool bONLINE(false);
			int nCHANNEL(0);


			pCHAR = GLAgentServer::GetInstance().GetChar ( cFRIEND.szCharName );
			if ( pCHAR )
			{
				//bONLINE = cFRIEND.IsVALID();
				bONLINE = true;
				nCHANNEL = pCHAR->m_nChannel;
			}

			if( ( bONLINE!=cFRIEND.bONLINE ) || ( nCHANNEL!=cFRIEND.nCHANNEL) )
			{
				cFRIEND.bONLINE = bONLINE;
				cFRIEND.nCHANNEL = nCHANNEL;

				//	note : �޽��� �߻�.				
				NetMsg.nChaFlag = cFRIEND.nCharFlag;
				NetMsg.bONLINE = cFRIEND.bONLINE;
				NetMsg.nCHANNEL = cFRIEND.nCHANNEL;
				StringCchCopy ( NetMsg.szCHAR, CHR_ID_LENGTH+1, cFRIEND.szCharName );
				GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
			}

			//pCHAR = GLAgentServer::GetInstance().GetChar ( pFRIEND->szCharName );
			//if ( pCHAR )
			//{
			//	//bONLINE = cFRIEND.IsVALID();
			//	bONLINE = true;
			//	nCHANNEL = pCHAR->m_nChannel;
			//}

			//if( ( bONLINE!=pFRIEND->bONLINE ) || ( nCHANNEL!=pFRIEND->nCHANNEL) )
			//{
			//	pFRIEND->bONLINE = bONLINE;
			//	pFRIEND->nCHANNEL = nCHANNEL;

			//	//	note : �޽��� �߻�.				
			//	NetMsg.nChaFlag = pFRIEND->nCharFlag;
			//	NetMsg.bONLINE = pFRIEND->bONLINE;
			//	NetMsg.nCHANNEL = pFRIEND->nCHANNEL;
			//	StringCchCopy ( NetMsg.szCHAR, CHR_ID_LENGTH+1, pFRIEND->szCharName );
			//	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
			//}
		}

		if( m_bClubWindowOpen )
		{
			//	Note : Ŭ�� ��� on/off ���� ����.
			//
			GLClubMan &cClubMan = GLAgentServer::GetInstance().GetClubMan();
			GLCLUB *pCLUB = cClubMan.GetClub ( m_dwGuild );
			if ( pCLUB )
			{
				GLMSG::SNET_CLUB_MEMBER_STATE NetMsgState;
				PGLCHARAG pCHAR = NULL;
				bool bINSERT = false;
				CLUBMEMBERS_ITER pos = pCLUB->m_mapMembers.begin();
				for ( ; pos!=pCLUB->m_mapMembers.end(); ++pos )
				{
					const GLCLUBMEMBER &sMember = (*pos).second;

					pCHAR = GLAgentServer::GetInstance().GetCharID ( sMember.dwID );
					bool bONLINE(false);
					int nCHANNEL(0);
					SNATIVEID nidMAP(false);

					if ( pCHAR )
					{
						bONLINE = true;
						nCHANNEL = pCHAR->m_nChannel;
						nidMAP = pCHAR->m_sCurMapID;
					}

					bINSERT = NetMsgState.ADD ( sMember.dwID, bONLINE, nCHANNEL, nidMAP );
					if ( !bINSERT )
					{
						GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgState );
						NetMsgState.RESET();
						NetMsgState.ADD ( sMember.dwID, bONLINE, nCHANNEL, nidMAP );
					}
				}

				if ( NetMsgState.dwNumber!=0 )
				{
					GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgState );
				}
			}
		}
	}

	return S_OK;
}

void GLCharAG::AddFriend ( char *szFriend )
{
	if ( !szFriend )					return;
	if ( !strcmp(szFriend,m_szName) )	return;

	//	Note : �߰��� ��� ã��.
	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( szFriend );
	if ( !pChar )						return;

	//	Note : �߰��� ��밡 �̹� ��ϵǾ� �ִ��� �˻�.
	MAPFRIEND_ITER iter = m_mapFriend.find ( szFriend );
	if ( iter!=m_mapFriend.end() )
	{
		//	Note : ��󿡰Դ� flag ����.
		CSetChaFriend *pDbStateChaFriend = new CSetChaFriend ( m_dwCharID, pChar->m_dwCharID, EMFRIEND_VALID );
		GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDbStateChaFriend );

		//	Note : ���� ��Ͽ��� ���� ����.
		SetStateFriend ( szFriend, EMFRIEND_VALID );
		return;
	}
	
	//	Note : �߰���.
	/*SFRIEND * pFRIEND = new SFRIEND;
	StringCchCopy(pFRIEND->szCharName,CHR_ID_LENGTH+1,szFriend);
	StringCchCopy(pFRIEND->szPhoneNumber,SMS_RECEIVER,pChar->m_szPhoneNumber);
	pFRIEND->nCharID = pChar->m_dwCharID;
	pFRIEND->nCharFlag = EMFRIEND_VALID;
	pFRIEND->bONLINE = true;
	pFRIEND->nCHANNEL = pChar->m_nChannel;
	if( m_mapFriend.insert( std::make_pair( szFriend, pFRIEND ) ).second )
	{
	}
	else
	{
		SAFE_DELETE( pFRIEND );
		return ;
	}*/
	SFRIEND sFRIEND;
	StringCchCopy(sFRIEND.szCharName,CHR_ID_LENGTH+1,szFriend);
	StringCchCopy(sFRIEND.szPhoneNumber,SMS_RECEIVER,pChar->m_szPhoneNumber);
	sFRIEND.nCharID = pChar->m_dwCharID;
	sFRIEND.nCharFlag = EMFRIEND_VALID;
	sFRIEND.bONLINE = true;
	sFRIEND.nCHANNEL = pChar->m_nChannel;
	m_mapFriend[std::string(szFriend)] = sFRIEND;
	//m_mapFriend[std::string(szFriend)] = pFriend;

	//	Note : db ģ�� ��� ����.
	CAddChaFriend *pDBAddChaFriend = new CAddChaFriend ( m_dwCharID, pChar->m_dwCharID );
	GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDBAddChaFriend );

	//	Note : ģ�� ���� �޽���.
	GLMSG::SNETPC_REQ_FRIENDINFO NetMsg;
//	NetMsg.sFRIEND = *pFRIEND;
	NetMsg.sFRIEND = sFRIEND;
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
}

bool GLCharAG::DelFriend ( char* szFriend )
{
	//	Note : ������ ��밡 ��ϵǾ� �ִ��� �˻�.
	MAPFRIEND_ITER iter = m_mapFriend.find ( szFriend );
	if ( iter==m_mapFriend.end() )		return false;

	/*SFRIEND * pFRIEND = (*iter).second;
	if( !pFRIEND ) return false;*/

	SFRIEND &cFRIEND = (*iter).second;

	//	Note : ģ�� ���� �α� ���.
	GLITEMLMT::GetInstance().ReqAction
	(
		m_dwCharID,					//	�����.
		EMLOGACT_FRIEND_DEL,		//	����.
//		ID_CHAR, pFRIEND->nCharID,	//	����.
		ID_CHAR, cFRIEND.nCharID,	//	����.
		0,							//	exp
		0,							//	bright
		0,							//	life
		0							//	money
	);

	//	Note : db ���� ����.
	//
//	CDelChaFriend *pDbDelChaFriend = new CDelChaFriend ( m_dwCharID, pFRIEND->nCharID );
	CDelChaFriend *pDbDelChaFriend = new CDelChaFriend ( m_dwCharID, cFRIEND.nCharID );
	GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDbDelChaFriend );

	//	Note : db�� �������� ��󿡰Դ� flag ����.
//	CSetChaFriend *pDbStateChaFriend = new CSetChaFriend ( pFRIEND->nCharID, m_dwCharID, EMFRIEND_BETRAY );
	CSetChaFriend *pDbStateChaFriend = new CSetChaFriend ( cFRIEND.nCharID, m_dwCharID, EMFRIEND_BETRAY );
	GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDbStateChaFriend );

	//	Note : ��Ͽ��� ����.
//	SAFE_DELETE( pFRIEND );
	m_mapFriend.erase ( iter );

	//	Note : ��󿡰Դ� flag ����.
	//
	PGLCHARAG pCharDel = GLAgentServer::GetInstance().GetChar ( szFriend );
	if ( pCharDel )
	{
		//	Note : ���� ��Ͽ��� ���� ����.
		pCharDel->SetStateFriend ( m_szName, EMFRIEND_BETRAY );
	}

	return true;
}

bool GLCharAG::BlockFriend ( char* szFriend, bool bBLOCK )
{
	if ( !bBLOCK )
	{
		//	Note : ������ ��밡 ��ϵǾ� �ִ��� �˻�.
		MAPFRIEND_ITER iter = m_mapFriend.find ( szFriend );
		if ( iter==m_mapFriend.end() )		return false;

		/*SFRIEND * pFRIEND = (*iter).second;
		if( !pFRIEND ) return false;*/

		SFRIEND &cFRIEND = (*iter).second;

		//	Note : ģ�� ���� �α� ���.
		GLITEMLMT::GetInstance().ReqAction
			(
			m_dwCharID,					//	�����.
			EMLOGACT_FRIEND_DEL,		//	����.
			//		ID_CHAR, pFRIEND->nCharID,	//	����.
			ID_CHAR, cFRIEND.nCharID,	//	����.
			0,							//	exp
			0,							//	bright
			0,							//	life
			0							//	money
			);

		//	Note : db ���� ����.
		//
		//CDelChaFriend *pDbDelChaFriend = new CDelChaFriend ( m_dwCharID, pFRIEND->nCharID );
		CDelChaFriend *pDbDelChaFriend = new CDelChaFriend ( m_dwCharID, cFRIEND.nCharID );
		GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDbDelChaFriend );

		//	Note : ��Ͽ��� ����.
//		SAFE_DELETE( pFRIEND );
		m_mapFriend.erase ( iter );

		return true;
	}

	bool bOLD_FRIEND(false);
	SFRIEND sNEW_BLOCK;
	SFRIEND *pFRIEND(NULL);

	PGLCHARAG pCharBlock = GLAgentServer::GetInstance().GetChar ( szFriend );
	if ( !pCharBlock )		return false;

	//	Note : ������ ��밡 ��ϵǾ� �ִ��� �˻�.
	MAPFRIEND_ITER iter = m_mapFriend.find ( szFriend );
	if ( iter!=m_mapFriend.end() )
	{
		bOLD_FRIEND = true;

//		pFRIEND	= (*iter).second;
//		if( !pFRIEND ) return false;
		pFRIEND	= &(*iter).second;	

		//	����ġ.
		pFRIEND->nCharFlag = EMFRIEND_BLOCK;
	}
	else
	{
		//SFRIEND * pNEW_BLOCK = new SFRIEND;

		////	�ڽ��� ��� ����.
		//pNEW_BLOCK->nCharID = (int) pCharBlock->m_dwCharID;
		//StringCchCopy ( pNEW_BLOCK->szCharName, CHR_ID_LENGTH+1, pCharBlock->m_szName );
		//pNEW_BLOCK->nCharFlag = EMFRIEND_BLOCK;

		//pFRIEND = pNEW_BLOCK;
		//if( m_mapFriend.insert( std::make_pair( szFriend, pFRIEND ) ).second )
		//{
		//}
		//else
		//{
		//	SAFE_DELETE( pFRIEND );
		//	return false;
		//}

		PGLCHARAG pCharBlock = GLAgentServer::GetInstance().GetChar ( szFriend );
		if ( !pCharBlock )		return false;

		////	Note : ����� ���� ��Ͽ��� ���� ����.
		//pCharBlock->SetStateFriend ( m_szName, EMFRIEND_OFF );

		//	�ڽ��� ��� ����.
		sNEW_BLOCK.nCharID = (int) pCharBlock->m_dwCharID;
		StringCchCopy ( sNEW_BLOCK.szCharName, CHR_ID_LENGTH+1, pCharBlock->m_szName );
		sNEW_BLOCK.nCharFlag = EMFRIEND_BLOCK;

		pFRIEND = &sNEW_BLOCK;
		m_mapFriend[std::string(szFriend)] = *pFRIEND;
	}

	iter = pCharBlock->m_mapFriend.find( m_szName );
	if ( iter!=pCharBlock->m_mapFriend.end() )
	{
		SFRIEND &cFRIEND = (*iter).second;
//		if( !cFRIEND ) return false;

		if( !cFRIEND.IsBLOCK() )
		{
			if( bOLD_FRIEND )
			{
				//	Note : db�� ��󿡰Դ� flag ����.
				CSetChaFriend *pDbStateChaFriend = new CSetChaFriend ( pFRIEND->nCharID, m_dwCharID, EMFRIEND_BETRAY );
				GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDbStateChaFriend );
			}

			//	Note : ��󿡰Դ� flag ����.
			//	Note : ����� ���� ��Ͽ��� ���� ����.
			pCharBlock->SetStateFriend ( m_szName, EMFRIEND_BETRAY );
		}
	}

	//	Note : db ģ�� ��� ����.
	if ( !bOLD_FRIEND )
	{
		CAddChaFriend *pDBAddChaFriend = new CAddChaFriend ( m_dwCharID, pFRIEND->nCharID );
		GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDBAddChaFriend );
	}

	//	Note : ģ�� ��� �α� ���.
	GLITEMLMT::GetInstance().ReqAction
	(
		m_dwCharID,					//	�����.
		EMLOGACT_FRIEND_BLOCK,		//	����.
		ID_CHAR, pFRIEND->nCharID,	//	����.
		0,							//	exp
		0,							//	bright
		0,							//	life
		0							//	money
	);

	//	Note : db�� flag ����.
	CSetChaFriend *pDbStateChaFriend = new CSetChaFriend ( m_dwCharID, pFRIEND->nCharID, EMFRIEND_BLOCK );
	GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDbStateChaFriend );

	//	Note : ģ�� ���� �޽���.
	GLMSG::SNETPC_REQ_FRIENDINFO NetMsg;
	NetMsg.sFRIEND = *pFRIEND;
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );

	//	Note : ���� ��Ͽ��� ������� ����.
	SetStateFriend( szFriend, EMFRIEND_BLOCK );

	return true;
}

void GLCharAG::SetStateFriend ( char *szFriend, DWORD dwFlag )
{
	//	Note : ��� ã��.
	MAPFRIEND_ITER iter = m_mapFriend.find ( szFriend );
	if ( iter==m_mapFriend.end() )		return;

//	SFRIEND * pFRIEND = (*iter).second;
//	if( !pFRIEND ) return;
	SFRIEND &sFRIEND = (*iter).second;

	//	Note : ������ ã�� ����.
//	pFRIEND->nCharFlag = dwFlag;
	sFRIEND.nCharFlag = dwFlag;

	PGLCHARAG pCHAR = GLAgentServer::GetInstance().GetChar ( szFriend );

	bool bONLINE = ( pCHAR!=NULL ); //&& (sFRIEND.IsVALID() || sFRIEND.IsBETRAY()) );

	//	note : �޽��� �߻�.
	GLMSG::SNETPC_REQ_FRIENDSTATE NetMsg;
	NetMsg.nChaFlag = dwFlag;
	NetMsg.bONLINE = bONLINE;
	StringCchCopy ( NetMsg.szCHAR, CHR_ID_LENGTH+1, szFriend );
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
}

int GLCharAG::GetStateFriend ( char *szFriend )
{
	//	Note : ��� ã��.
	MAPFRIEND_ITER iter = m_mapFriend.find ( szFriend );
	if ( iter==m_mapFriend.end() )		return EMFRIEND_OFF;

	return (*iter).second.nCharFlag;
}

HRESULT GLCharAG::MsgActState ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_ACTSTATE *pNetMsg = (GLMSG::SNETPC_ACTSTATE *)nmg;

	const DWORD dwOldActState = m_dwActState;

	// ��Ʋ�ξ� �������� ���������� Visible ��� ���������ϵ��� üũ�׸� �߰�.

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
		GLMSG::SNETPC_ACTSTATE NetMsgToField;
		NetMsgToField.dwActState = m_dwActState;
		GLAgentServer::GetInstance().SENDTOFIELD ( m_dwClientID, &NetMsgToField );
	}

	return S_OK;
}

// *****************************************************
// Desc : ģ������Ʈ ��û ó��
// *****************************************************
HRESULT GLCharAG::MsgReqFriendList ( NET_MSG_GENERIC* nmg )
{
	PGLCHARAG pCHAR = NULL;
	bool bONLINE = false;
	GLMSG::SNETPC_REQ_FRIENDINFO NetMsg;
	MAPFRIEND_ITER iter = m_mapFriend.begin();
	for ( ; iter!=m_mapFriend.end(); ++iter )
	{
//		SFRIEND * pFRIEND = (*iter).second;
//		if( !pFRIEND ) continue;
		SFRIEND &sFRIEND = (*iter).second;

		//	Note : ģ�� ã��.
//		pCHAR = GLAgentServer::GetInstance().GetChar ( pFRIEND->szCharName );
		pCHAR = GLAgentServer::GetInstance().GetChar ( sFRIEND.szCharName );
		bONLINE = ( pCHAR!=NULL ); //&& (sFRIEND.IsVALID() || sFRIEND.IsBETRAY()) );

//		pFRIEND->bONLINE = bONLINE;
		sFRIEND.bONLINE = bONLINE;
		if ( pCHAR )
		{
//			pFRIEND->nCHANNEL = pCHAR->m_nChannel;
			sFRIEND.nCHANNEL = pCHAR->m_nChannel;
		}
		
		//	Note : ���� ����.		
//		NetMsg.sFRIEND = *pFRIEND;
		NetMsg.sFRIEND = sFRIEND;
		GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
	}

	return S_OK;
}

HRESULT GLCharAG::MsgReqFriendAdd ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDADD *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDADD *) nmg;

	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( pNetMsg->szADD_CHAR );
	if ( !pChar )
	{
		GLMSG::SNETPC_REQ_FRIENDADD_FB NetMsgFB;
		StringCchCopy ( NetMsgFB.szADD_CHAR, CHR_ID_LENGTH+1, pNetMsg->szADD_CHAR );
		NetMsgFB.emFB = EMFRIENDADD_ABSENCE;
		GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_OK;
	}

	//	Note : ģ�� ��� ��û.
	GLMSG::SNETPC_REQ_FRIENDADD_LURE NetMsgLure;
	StringCchCopy ( NetMsgLure.szREQ_CHAR, CHR_ID_LENGTH+1, m_szName );
	GLAgentServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgLure );

	return S_OK;
}

HRESULT GLCharAG::MsgReqFriendAddAns ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDADD_ANS *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDADD_ANS *) nmg;

	//	Note : ��û�� ���� ��������.
	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( pNetMsg->szREQ_CHAR );
	if ( !pChar )
	{
		GLMSG::SNETPC_REQ_FRIENDADD_FB NetMsgFB;
		StringCchCopy ( NetMsgFB.szADD_CHAR, CHR_ID_LENGTH+1, pNetMsg->szREQ_CHAR );
		NetMsgFB.emFB = EMFRIENDADD_ABSENCE;
		GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_OK;
	}

	if ( !pNetMsg->bOK )
	{
		GLMSG::SNETPC_REQ_FRIENDADD_FB NetMsgFB;
		StringCchCopy ( NetMsgFB.szADD_CHAR, CHR_ID_LENGTH+1, m_szName );
		NetMsgFB.emFB = EMFRIENDADD_REFUSE;
		GLAgentServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, &NetMsgFB );
		return S_OK;
	}

	//	Note : ģ�� ��� �α� ���.
	GLITEMLMT::GetInstance().ReqAction
	(
		pChar->m_dwCharID,			//	�����.
		EMLOGACT_FRIEND_ADD,		//	����.
		ID_CHAR, m_dwCharID,		//	����.
		0,							//	exp
		0,							//	bright
		0,							//	life
		0							//	money
	);

	//	Note : ģ�� ���.
	//
	AddFriend ( pNetMsg->szREQ_CHAR );
	pChar->AddFriend ( m_szName );

	return S_OK;
}

HRESULT GLCharAG::MsgReqFriendDel ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDDEL *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDDEL *) nmg;

	//	Note : ��Ͽ��� ���� ����.
	bool bDEL = DelFriend ( pNetMsg->szDEL_CHAR );

	//	Note : ���� �޽��� ��û ���.
	//
	GLMSG::SNETPC_REQ_FRIENDDEL_FB NetMsgFB;
	if ( bDEL )		NetMsgFB.emFB = EMFRIENDDEL_OK;
	else			NetMsgFB.emFB = EMFRIENDDEL_FAIL;
	StringCchCopy ( NetMsgFB.szDEL_CHAR, CHR_ID_LENGTH+1, pNetMsg->szDEL_CHAR );
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	return S_OK;
}

HRESULT GLCharAG::MsgReqFriendBlock ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDBLOCK *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDBLOCK *) nmg;

	//	Note : ��Ͽ��� �� ����.
	bool bOK = BlockFriend ( pNetMsg->szCHAR, pNetMsg->bBLOCK );

	//	Note : �� �޽��� ����.
	//
	GLMSG::SNETPC_REQ_FRIENDBLOCK_FB NetMsgFB;
	if ( bOK )	NetMsgFB.emFB = (pNetMsg->bBLOCK?EMFRIENDBLOCK_ON:EMFRIENDBLOCK_OFF);
	else		NetMsgFB.emFB = EMFRIENDBLOCK_FAIL;

	StringCchCopy ( NetMsgFB.szCHAR, CHR_ID_LENGTH+1, pNetMsg->szCHAR );
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	if ( !pNetMsg->bBLOCK )
	{
		GLMSG::SNETPC_REQ_FRIENDDEL_FB NetMsgFB;
		NetMsgFB.emFB = EMFRIENDDEL_OK;
		StringCchCopy ( NetMsgFB.szDEL_CHAR, CHR_ID_LENGTH+1, pNetMsg->szCHAR );
		GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
	}

	return S_OK;
}

// *****************************************************
// Desc : ģ���� Ŭ���ɹ��鿡�� ����� �̸� �˸�.
// *****************************************************
HRESULT GLCharAG::MsgRenameBrd ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_RENAME_AGTBRD *pNetMsg = (GLMSG::SNETPC_INVEN_RENAME_AGTBRD *) nmg;

	// �¶��ο� �ִ� ģ���鿡�� �˸�
	GLMSG::SNETPC_REQ_FRIENDRENAME_CLT NetMsgFriendClt;
	NetMsgFriendClt.dwGaeaID = m_dwGaeaID;
	StringCchCopy ( NetMsgFriendClt.szOldName, CHAR_SZNAME, pNetMsg->szOldName );
	StringCchCopy ( NetMsgFriendClt.szNewName, CHAR_SZNAME, pNetMsg->szNewName );

	PGLCHARAG pFriend = NULL;
	MAPFRIEND_ITER iter = m_mapFriend.begin();
	for ( ; iter!=m_mapFriend.end(); ++iter )
	{
//		SFRIEND * pFRIEND = (*iter).second;
//		if( !pFRIEND ) continue;
		SFRIEND &cFRIEND = (*iter).second;

		// ���� ģ���� ����� �ɸ����� ģ������Ʈ�� �̸��� �����Ѵ�. 
		pFriend = GLAgentServer::GetInstance().GetChar ( cFRIEND.szCharName );
//		pFriend = GLAgentServer::GetInstance().GetChar ( pFRIEND->szCharName );
		if ( pFriend )
		{
			MAPFRIEND_ITER iter = pFriend->m_mapFriend.find ( m_szName );
			if ( iter!=pFriend->m_mapFriend.end() )
			{
				pFriend->m_mapFriend.erase ( m_szName );

				SFRIEND sFRIEND;
				StringCchCopy(sFRIEND.szCharName, CHR_ID_LENGTH, pNetMsg->szNewName);
				sFRIEND.nCharID = m_dwCharID;
				sFRIEND.nCharFlag = EMFRIEND_VALID;
				sFRIEND.bONLINE = true;
				sFRIEND.nCHANNEL = m_nChannel;
				pFriend->m_mapFriend[std::string(pNetMsg->szNewName)] = sFRIEND;
				// Send to Client
				GLAgentServer::GetInstance().SENDTOCLIENT ( pFriend->m_dwClientID, &NetMsgFriendClt );



				//SFRIEND * pFRIEND = new SFRIEND;
				//StringCchCopy(pFRIEND->szCharName, CHR_ID_LENGTH, pNetMsg->szNewName);
				//pFRIEND->nCharID = m_dwCharID;
				//pFRIEND->nCharFlag = EMFRIEND_VALID;
				//pFRIEND->bONLINE = true;
				//pFRIEND->nCHANNEL = m_nChannel;
				//if( pFriend->m_mapFriend.insert( std::make_pair( pNetMsg->szNewName, pFRIEND ) ).second )
				//{
				//	// Send to Client
				//	GLAgentServer::GetInstance().SENDTOCLIENT ( pFriend->m_dwClientID, &NetMsgFriendClt );

				//	// Need to Modify DB
				//}
				//else
				//{
				//	SAFE_DELETE( pFRIEND );
				//	continue;
				//}
			}
		}
	}

	// ��Ƽ���� �˸�
	GLPARTY* pParty = GLAgentServer::GetInstance().GetParty ( m_dwPartyID );
	if ( pParty )
	{
		// Send to Client
		GLMSG::SNET_PARTY_MEMBER_RENAME_CLT NetMsgPartyClt;
		NetMsgPartyClt.dwPartyID = m_dwPartyID;
		NetMsgPartyClt.dwGaeaID = m_dwGaeaID;
		StringCchCopy ( NetMsgPartyClt.szName, CHAR_SZNAME, pNetMsg->szNewName );
		GLAgentServer::GetInstance().SENDTOPARTY ( m_dwPartyID, &NetMsgPartyClt );

		// Send to Field
		GLMSG::SNET_PARTY_MEMBER_RENAME_FLD NetMsgPartyFld;
		NetMsgPartyFld.dwPartyID = m_dwPartyID;
		NetMsgPartyFld.dwGaeaID = m_dwGaeaID;
		StringCchCopy ( NetMsgPartyFld.szName, CHAR_SZNAME, pNetMsg->szNewName );
		GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgPartyFld );
	}

	// Ŭ���ɹ��鿡�� �˸�
	GLClubMan &cClubMan = GLAgentServer::GetInstance().GetClubMan();
	GLCLUB *pCLUB = cClubMan.GetClub ( m_dwGuild );
	if ( pCLUB )
	{
		GLCLUBMEMBER* pClubMember = pCLUB->GetMember ( m_dwCharID );

		if ( pCLUB->IsMaster ( m_dwCharID ) ) 
		{
			StringCchCopy ( pCLUB->m_szMasterName, CHAR_SZNAME, pNetMsg->szNewName );
		}

		if ( pClubMember )
		{
			StringCchCopy ( pClubMember->m_szName, CHAR_SZNAME, pNetMsg->szNewName );

			// Send to Client
			GLMSG::SNET_CLUB_MEMBER_RENAME_CLT NetMsgClubClt;
			NetMsgClubClt.dwClubID = pCLUB->m_dwID;
			NetMsgClubClt.dwCharID = m_dwCharID;
			StringCchCopy ( NetMsgClubClt.szName, CHAR_SZNAME, pNetMsg->szNewName );
			GLAgentServer::GetInstance().SENDTOCLUBCLIENT ( pCLUB->m_dwID, &NetMsgClubClt );

			// Send to Field
			GLMSG::SNET_CLUB_MEMBER_RENAME_FLD NetMsgClubFld;
			NetMsgClubFld.dwClubID = pCLUB->m_dwID;
			NetMsgClubFld.dwCharID = m_dwCharID;
			StringCchCopy ( NetMsgClubFld.szName, CHAR_SZNAME, pNetMsg->szNewName );
			GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgClubFld );
		}
	}
	GLAgentServer::GetInstance().ChangeNameMap ( this, m_szName, pNetMsg->szNewName );
	
	return S_OK;
}

HRESULT GLCharAG::MsgPhoneNumberBrd( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PHONE_NUMBER_AGTBRD *pNetMsg = (GLMSG::SNETPC_PHONE_NUMBER_AGTBRD *) nmg;

	// �¶��ο� �ִ� ģ���鿡�� �˸�
	GLMSG::SNETPC_REQ_FRIENDPHONENUMBER_CLT NetMsgFriendClt;
	NetMsgFriendClt.dwGaeaID = m_dwGaeaID;
	StringCchCopy ( NetMsgFriendClt.szName, CHAR_SZNAME, pNetMsg->szName );
	StringCchCopy ( NetMsgFriendClt.szNewPhoneNumber, SMS_RECEIVER, pNetMsg->szNewPhoneNumber );

	PGLCHARAG pFriend = NULL;
	MAPFRIEND_ITER iter = m_mapFriend.begin();
	for ( ; iter!=m_mapFriend.end(); ++iter )
	{
		/*SFRIEND * pFRIEND = (*iter).second;
		if( !pFRIEND ) continue;*/

		SFRIEND &cFRIEND = (*iter).second;

//		pFriend = GLAgentServer::GetInstance().GetChar ( pFRIEND->szCharName );
		pFriend = GLAgentServer::GetInstance().GetChar ( cFRIEND.szCharName );
		if ( pFriend )
		{
			MAPFRIEND_ITER iter = pFriend->m_mapFriend.find ( m_szName );
			if ( iter!=pFriend->m_mapFriend.end() )
			{
//				StringCchCopy( iter->second->szPhoneNumber, SMS_RECEIVER, pNetMsg->szNewPhoneNumber );
				StringCchCopy( iter->second.szPhoneNumber, SMS_RECEIVER, pNetMsg->szNewPhoneNumber );


				// Send to Client
				GLAgentServer::GetInstance().SENDTOCLIENT ( pFriend->m_dwClientID, &NetMsgFriendClt );
			}
		}
	}

	GLAgentServer::GetInstance().ChangeNameMap( this, pNetMsg->szNewPhoneNumber );

	return S_OK;
}

HRESULT	GLCharAG::MsgUpdateStartMap ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_UPDATE_STARTCALL *pNetMsg = (GLMSG::SNETPC_UPDATE_STARTCALL *)nmg;
			
	// ������ġ ����
	m_sStartMapID = pNetMsg->sStartMapID;
	m_dwStartGate = pNetMsg->dwStartGateID;
	m_vStartPos	  = pNetMsg->vStartPos;

	return S_OK;
}

HRESULT GLCharAG::MsgChangeArm( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PUTON_CHANGE_AG *pNetMsg = (GLMSG::SNETPC_PUTON_CHANGE_AG *)nmg;
	m_bUseArmSub = pNetMsg->bUseArmSub;

	return S_OK;
}

HRESULT GLCharAG::MsgFriendClubWindowOpen( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIEND_CLUB_OPEN *pNetMsg = (GLMSG::SNETPC_REQ_FRIEND_CLUB_OPEN *)nmg;
	
	if( pNetMsg->emTYPE == EMFRIEND_WINDOW )
	{
		m_bFriendWindowOpen = pNetMsg->bOpen;
	}
	else if( pNetMsg->emTYPE == EMCLUB_WINDOW )
	{
		m_bClubWindowOpen = pNetMsg->bOpen;
	}

	return S_OK;
}

HRESULT GLCharAG::MsgSetServerDelayTime ( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_SET_SERVER_DELAYTIME_AGT* pNetMsg = (GLMSG::SNET_SET_SERVER_DELAYTIME_AGT*) nmg;

	GLMSG::SNET_SET_SERVER_DELAYTIME_FLD NetMsgFld;
	NetMsgFld.dwMillisec = pNetMsg->dwMillisec;
	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgFld );

	GLMSG::SNET_SET_SERVER_DELAYTIME_FB NetMsgFB;
	NetMsgFB.dwMillisec = pNetMsg->dwMillisec;
	NetMsgFB.bAllServer = true;
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	return S_OK;
}

HRESULT GLCharAG::MsgSetSkipPacketNum ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SET_SERVER_SKIPPACKET_AGT* pNetMsg = (GLMSG::SNET_SET_SERVER_SKIPPACKET_AGT*) nmg;

	GLMSG::SNET_SET_SERVER_SKIPPACKET_FLD NetMsgFld;
	NetMsgFld.wSkipPacketNum = pNetMsg->wSkipPacketNum;
	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgFld );

	GLMSG::SNET_SET_SERVER_SKIPPACKET_FB NetMsgFB;
	NetMsgFB.wSkipPacketNum = pNetMsg->wSkipPacketNum;
	NetMsgFB.bAllServer = true;
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );


	return S_OK;
}

HRESULT GLCharAG::MsgSetServerCrashTime ( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_SET_SERVER_CRASHTIME_AGT* pNetMsg = (GLMSG::SNET_SET_SERVER_CRASHTIME_AGT*) nmg;

	GLMSG::SNET_SET_SERVER_CRASHTIME_FLD NetMsgFld;
	NetMsgFld.dwSec = pNetMsg->dwSec;
	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgFld );
	
	GLMSG::SNET_SET_SERVER_CRASHTIME_FB NetMsgFB;
	NetMsgFB.dwSec = pNetMsg->dwSec;
	NetMsgFB.bAllServer = true;
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	return S_OK;
}

HRESULT GLCharAG::MsgReqQboxOption( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_QBOX_OPTION_REQ_AG* pNetMsg = (GLMSG::SNET_QBOX_OPTION_REQ_AG*)nmg;

	GLMSG::SNET_QBOX_OPTION_REQ_FLD NetMsgFld;
	NetMsgFld.dwPartyID   = m_dwPartyID;
	NetMsgFld.bQBoxEnable = pNetMsg->bQBoxEnable;
	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgFld );

	return S_OK;
}

HRESULT	GLCharAG::MsgReqAttendList( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_ATTENDLIST_FB NetMsgFB;

	NetMsgFB.tLoginTime = m_tLoginTime;
	NetMsgFB.dwAttendTime = GLCONST_ATTENDANCE::dwAttendTime;

	if ( m_vecAttend.empty() )
	{
		NetMsgFB.dwAttendCombo = 0;
		NetMsgFB.SetSize( 0 );		
		GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

		return S_OK;
	}

	//	���� �ϼ� ���
	CarcAttendCombo();

	int nSize = m_vecAttend.size();
	CTime cCurTime( CTime::GetCurrentTime() );	
	
	CTime cTime;
	int nDataSize = 0;

	for ( int i = 0; i < nSize; ++i)
	{
		cTime = m_vecAttend[i].tAttendTime;
		// ������ �����͸� ����
		if ( cCurTime.GetYear() == cTime.GetYear() && cCurTime.GetMonth() == cTime.GetMonth() )
		{
			NetMsgFB.sAttend[nDataSize] = m_vecAttend[i];
			nDataSize++;
		}
	}

	NetMsgFB.dwAttendCombo = m_dwComboAttend;
	NetMsgFB.SetSize( nDataSize );
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
	
	return S_OK;
}


HRESULT GLCharAG::MsgReqAttendance( NET_MSG_GENERIC *nmg )
{
	GLMSG::SNETPC_REQ_ATTENDANCE_FB NetMsgFB;

	CTime cCurTime( CTime::GetCurrentTime() );

	//	�������ӽð� üũ	
	CTime cDayTime( cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(),0,0,0 );
	CTime cLoginTime;
	if ( m_tLoginTime < cDayTime.GetTime()  ) cLoginTime = cDayTime;
	else cLoginTime = m_tLoginTime;

	CTimeSpan cGameTime = cCurTime - cLoginTime;
	if ( cGameTime.GetTotalMinutes() < GLCONST_ATTENDANCE::dwAttendTime ) 
	{
		NetMsgFB.emFB = EMREQ_ATTEND_FB_ATTENTIME;
		GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		return S_OK;
	}

	if ( !m_vecAttend.empty() )
	{
		int nSize = m_vecAttend.size();

		USER_ATTEND_INFO& sAttend = m_vecAttend[nSize-1];
		CTime cTime = sAttend.tAttendTime;

		// ���� �̹� �⼮ �ߴٸ�
		if ( cCurTime.GetYear() == cTime.GetYear() && 
			 cCurTime.GetMonth() == cTime.GetMonth() && 
			 cCurTime.GetDay() == cTime.GetDay() )
		{
			NetMsgFB.emFB = EMREQ_ATTEND_FB_ALREADY;
			GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
			return S_OK;
		}	
	}

	int nComboAttend = m_dwComboAttend;
	nComboAttend++;
	int nReward = -1;

	// ���� �ϼ� Ȯ�� �� ��ǰ üũ
	for ( int i = 1; i <= (int)GLCONST_ATTENDANCE::dwMAXREWARD; ++i )
	{
		int nIndex = GLCONST_ATTENDANCE::dwMAXREWARD-i;
		if ( nComboAttend == GLCONST_ATTENDANCE::sATEEND_REWARD[nIndex].nComboDay )
		{
			nReward = nIndex;
			break;
		}
	}

	
	// ��ǰ�ֱ�	
	if ( nReward >= 0 ) 
	{
		SNATIVEID sID = GLCONST_ATTENDANCE::sATEEND_REWARD[nReward].idReward;
		if ( sID != NATIVEID_NULL() && nComboAttend == GLCONST_ATTENDANCE::sATEEND_REWARD[nReward].nComboDay )
		{
			GLMSG::SNETPC_REQ_ATTEND_REWARD_FLD NetMsgFld;
			NetMsgFld.idAttendReward = sID;
			GLAgentServer::GetInstance().SENDTOFIELD ( m_dwClientID, &NetMsgFld );
		}
	}

	m_dwComboAttend = nComboAttend;
	USER_ATTEND_INFO sAttend;
	sAttend.tAttendTime = cCurTime.GetTime();
	sAttend.nComboAttend = m_dwComboAttend;
	sAttend.bAttendReward = ( nReward >= 0 ) ? 1 : 0 ;
	m_vecAttend.push_back( sAttend );

	NetMsgFB.emFB = EMREQ_ATTEND_FB_OK;
	NetMsgFB.tAttendTime = sAttend.tAttendTime;
	NetMsgFB.dwComboAttend = (DWORD)sAttend.nComboAttend;
	NetMsgFB.bAttendReward = sAttend.bAttendReward;
	GLAgentServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	// DB ����

	int nAttendReward = sAttend.bAttendReward ? 1 : 0;
	CInsertUserAttend *pDBInserUserAttend = new CInsertUserAttend ( m_dwUserID, m_dwComboAttend, nAttendReward );
	GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDBInserUserAttend );

	return S_OK;
}

void GLCharAG::CarcAttendCombo()
{
	int nSize = m_vecAttend.size();
	CTime cCurTime( CTime::GetCurrentTime() );	
	CTimeSpan cTimeSpan( 1, cCurTime.GetHour(), cCurTime.GetMinute(), cCurTime.GetSecond() );
	CTime cYesTime = cCurTime - cTimeSpan;

	CTime cTime;

	USER_ATTEND_INFO& sAttend = m_vecAttend[nSize-1];
	cTime = sAttend.tAttendTime;
	if ( cTime < cYesTime ) m_dwComboAttend = 0;
	else m_dwComboAttend = sAttend.nComboAttend;

    if ( GLCONST_ATTENDANCE::dwMAXREWARD > 0 ) 
	{
		int nMaxReward = GLCONST_ATTENDANCE::sATEEND_REWARD[GLCONST_ATTENDANCE::dwMAXREWARD-1].nComboDay;
		if ( m_dwComboAttend >= (DWORD)nMaxReward ) m_dwComboAttend = 0;
	}

	return;
}

HRESULT GLCharAG::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_ACTSTATE:				MsgActState(nmg);			break;

	case NET_MSG_REQ_FRIENDLIST:				MsgReqFriendList(nmg);		break;
	case NET_MSG_REQ_FRIENDADD:					MsgReqFriendAdd(nmg);		break;
	case NET_MSG_REQ_FRIENDADD_ANS:				MsgReqFriendAddAns(nmg);	break;
	case NET_MSG_REQ_FRIENDDEL:					MsgReqFriendDel(nmg);		break;
	case NET_MSG_REQ_FRIENDBLOCK:				MsgReqFriendBlock(nmg);		break;
	case NET_MSG_GCTRL_CLUB_MEMBER_REQ_2AGT:	MsgClubMemberReqAgt(nmg);	break;
	case NET_MSG_GCTRL_CLUB_MEMBER_DEL:			MsgClubMemberDel(nmg);		break;
	case NET_MSG_GCTRL_CLUB_MEMBER_SECEDE:		MsgClubMemberSecede(nmg);	break;
	case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ:		MsgClubAuthorityReq(nmg);	break;
	case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ANS:	MsgClubAuthorityReqAns(nmg);break;
	case NET_MSG_GCTRL_CLUB_MARK_INFO:			MsgClubMarkInfo(nmg);		break;
	case NET_MSG_GCTRL_CLUB_MARK_CHANGE:		MsgClubMarkChange(nmg);		break;
	case NET_MSG_GCTRL_CLUB_RANK_2AGT:			MsgClubRank2Agt(nmg);		break;

	case NET_MSG_GCTRL_CLUB_COMMISSION:			MsgClubGuidCommission(nmg);	break;
	case NET_MSG_GCTRL_CLUB_NOTICE_REQ:			MsgClubNoticeReq(nmg);		break;
	case NET_MSG_GCTRL_CLUB_SUBMASTER:			MsgClubSubMaster(nmg);		break;
	case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ:		MsgClubAllianceReq(nmg);	break;
	case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ANS:	MsgClubAllianceReqAns(nmg);	break;
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_REQ:	MsgClubAllianceDelReq(nmg);	break;
	case NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_REQ:	MsgClubAllianceSecReq(nmg);	break;
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_REQ:	MsgClubAllianceDisReq(nmg);	break;

	case NET_MSG_GCTRL_CLUB_BATTLE_REQ:			MsgClubBattleReq(nmg);		break;
	case NET_MSG_GCTRL_CLUB_BATTLE_REQ_ANS:		MsgClubBattleReqAns(nmg);	break;
	case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ:		MsgClubBattleArmisticeReq(nmg);		break;
	case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ANS:	MsgClubBattleArmisticeReqAns(nmg);	break;
	case NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ:		MsgClubBattleSubmissionReq(nmg);	break;

	case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ:		MsgAllianceBattleReq(nmg);		break;
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ANS: MsgAllianceBattleReqAns(nmg);	break;
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ:		MsgAllianceBattleArmisticeReq(nmg);		break;
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS:	MsgAllianceBattleArmisticeReqAns(nmg);	break;
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ:		MsgAllianceBattleSubmissionReq(nmg);	break;

	case NET_MSG_GCTRL_INVEN_RENAME_AGTBRD:		MsgRenameBrd(nmg);			break;
	case NET_MSG_SMS_PHONE_NUMBER_AGTBRD:		MsgPhoneNumberBrd(nmg);		break;
	case NET_MSG_GCTRL_UPDATE_STARTCALL:		MsgUpdateStartMap(nmg);		break;

	case NET_MSG_GCTRL_SET_SERVER_DELAYTIME_AGT: MsgSetServerDelayTime(nmg);	break;
	case NET_MSG_GCTRL_SET_SERVER_CRASHTIME_AGT: MsgSetServerCrashTime(nmg);	break;
	case NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_AGT:MsgSetSkipPacketNum(nmg);		break;

	case NET_MSG_GCTRL_PUTON_CHANGE_AG:			MsgChangeArm(nmg);				break;
	case NET_MSG_REQ_FRIEND_CLUB_OPEN:			MsgFriendClubWindowOpen(nmg);	break;

	case NET_QBOX_OPTION_REQ_AG:				MsgReqQboxOption(nmg); break;
	case NET_MSG_REQ_ATTENDLIST:				MsgReqAttendList(nmg); break;
	case NET_MSG_REQ_ATTENDANCE:				MsgReqAttendance(nmg); break;
	};

	return S_OK;
}
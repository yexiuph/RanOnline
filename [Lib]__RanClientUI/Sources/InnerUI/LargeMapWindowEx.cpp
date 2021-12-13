#include "pch.h"
#include "LargeMapWindow.h"
#include "GLPartyClient.h"
#include "GLGaeaClient.h"
#include "DxGlobalStage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CUIControl* CLargeMapWindow::MAKE_MARK ( const int nPartyID )
//{
//	m_CONTROL_NEWID++;
//	if ( BASE_CONTROLID_END <= m_CONTROL_NEWID ) m_CONTROL_NEWID = BASE_CONTROLID;	
//
//	static CString strKeyword = "TEST_MARK";
//	CString strKeywordNum;
//	strKeywordNum.Format ( "%s%d", strKeyword, nPartyID );
//
//	CUIControl* pMark = new CUIControl;
//	pMark->CreateSub ( this, strKeywordNum.GetString(), UI_FLAG_DEFAULT, m_CONTROL_NEWID );
//	pMark->InitDeviceObjects ( m_pd3dDevice );
//	pMark->RestoreDeviceObjects ( m_pd3dDevice );
//	pMark->SetUseRender ( TRUE );
//	RegisterControl ( pMark );
//
//	return pMark;
//}

void CLargeMapWindow::UPDATE_PARTY ()
{
	UPDATE_PARTY_DEL();

	GLPARTY_CLIENT* pSelf = FindSelfClient ();

	if( pSelf )
	{
		UPDATE_PARTY_INS( pSelf ); // 추가
	}
	//else // Note : 파티가 아니라면 맵 상에 존재하는 모든 컨트롤을 지운다.
	//{
	//	PARTY_ONMAP_MAP_ITER iter;

	//	for( iter = m_mapPARTY.begin(); iter != m_mapPARTY.end(); )
	//	{
	//		PARTY_ONMAP_MAP_ITER cur = iter++;

	//		CUIControl * pMark = (*cur).second;
	//		if( pMark ) DeleteControl( pMark->GetWndID(), 0 );

	//		m_mapPARTY.erase( cur );
	//	}
	//}
}

void CLargeMapWindow::UPDATE_PARTY_DEL()
{
	//PARTY_ONMAP_MAP_ITER iter;

	//for( iter = m_mapPARTY.begin(); iter != m_mapPARTY.end(); )
	//{
	//	PARTY_ONMAP_MAP_ITER cur = iter++;

	//	const DWORD& dwGaeaID = (*cur).first;
	//	GLPARTY_CLIENT *pMember = GLPartyClient::GetInstance().FindMember ( dwGaeaID );
	//	if ( pMember && IsSameMap ( pSelf, pMember ) ) continue;		

	//	CUIControl* pMark = (*cur).second;
	//	if( !pMark ) return ;

	//	DeleteControl( pMark->GetWndID(), 0 );
	//	m_mapPARTY.erase( cur );
	//}

	for( INT i=0; i<MAXPARTY; ++i )
	{
		m_pMark[i]->SetVisibleSingle( FALSE );
	}
}

void CLargeMapWindow::UPDATE_PARTY_INS( GLPARTY_CLIENT * pSelf )
{
	GLPARTY_CLIENT* pMaster = GLPartyClient::GetInstance().GetMaster();	//	마스터
	if( !pMaster ) return ;

	if( ( pSelf != pMaster ) && IsSameMap( pSelf, pMaster ) )
	{
		//AddPartyMemberOnMAP( pMaster->m_dwGaeaID, pMaster->m_vPos, 0 );
		UPDATE_CHAR_POS ( m_pMark[0], pMaster->m_vPos );
	}

	DWORD nMEMBER_NUM = GLPartyClient::GetInstance().GetMemberNum();
	if( nMEMBER_NUM > 1 )
	{
		nMEMBER_NUM -= 1; // Note : 마스터는 제외
		for ( DWORD nIndex = 0; nIndex < nMEMBER_NUM; ++nIndex )
		{
			GLPARTY_CLIENT *pMember = GLPartyClient::GetInstance().GetMember( nIndex );

			if( !pMember )						continue;
			if( !IsSameMap( pSelf, pMember ) )	continue;
			if( pSelf == pMember )				continue;

			//	Note : AddPartyMemberOnMAP의 마지막 파라미터에 +1로 설정한것은
			//         의도된 것입니다. Master가 0, 멤버는 1부터시작해서 7까지 설정합니다.
			UPDATE_CHAR_POS( m_pMark[nIndex+1], pMember->m_vPos );
		}
	}
}

//void CLargeMapWindow::AddPartyMemberOnMAP ( const DWORD& dwGaeaID, const D3DXVECTOR3& vPos, const int nPartyID )
//{
//	//CUIControl* pMark(NULL);
//
//	//PARTY_ONMAP_MAP_ITER found = m_mapPARTY.find ( dwGaeaID );
//
//	//if ( found == m_mapPARTY.end() )
//	//{
//	//	pMark = MAKE_MARK( nPartyID );
//	//	if( !pMark ) return ;
//
//	//	m_mapPARTY.insert ( std::make_pair(dwGaeaID, pMark) );
//	//}
//	//else
//	//{
//	//	pMark = (*found).second;
//	//	if( !pMark ) return ;
//	//}
//
//	UPDATE_CHAR_POS ( m_pMark[nPartyID], vPos );
//}

BOOL CLargeMapWindow::IsSameMap ( GLPARTY_CLIENT *pSelf, GLPARTY_CLIENT *pMember )
{
	if ( !pSelf || !pMember ) return FALSE;
	
	return pSelf->m_sMapID.dwID == pMember->m_sMapID.dwID;
}

GLPARTY_CLIENT*	CLargeMapWindow::FindSelfClient ()
{
	//	마스터가 아니면, 자식들을 찾아본다.
	GLPARTY_CLIENT *pMaster = GLPartyClient::GetInstance().GetMaster();	//	마스터

	if( pMaster && pMaster->ISONESELF() )
	{
		return pMaster;
	}
	else
	{ // Note : 자기찾기
		DWORD nMEMBER_NUM = GLPartyClient::GetInstance().GetMemberNum();
		if( nMEMBER_NUM > 1 )
		{
			nMEMBER_NUM -= 1; // Note : 마스터는 제외
			for ( DWORD i = 0; i < nMEMBER_NUM; ++i )
			{
				GLPARTY_CLIENT *pMember = GLPartyClient::GetInstance().GetMember( i );

				if( pMember && pMember->ISONESELF() )
				{
					return pMember;				
				}
			}
		}
	}

	return NULL;
}

void CLargeMapWindow::UPDATE_GUILD()
{
	RESET_GUILD();
	
	if ( !IS_GUILD_UPDATE() ) return;

	//	클럽 확인
	GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter()->m_sCLUB;

	// 클럽 배틀존에서만 보임
	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLand )	return;

  	int nMyChannel = DxGlobalStage::GetInstance().GetChannel();
	GLCharacter* pChar = GLGaeaClient::GetInstance().GetCharacter();
	if ( !pChar ) return;
	
	DWORD dwCharID = pChar->GetCharID();

	CLUBMEMBERS& mapMembers = sCLUB.m_mapMembers;
	CLUBMEMBERS_ITER iter_mem = mapMembers.begin ();
	CLUBMEMBERS_ITER iter_mem_end = mapMembers.end ();

	GUILD_POS_VECTOR_ITER iter_guild = m_vecGuild.begin();
	GUILD_POS_VECTOR_ITER iter_guild_end = m_vecGuild.end();


	// 길드 순회
	for ( ; iter_mem != iter_mem_end; ++iter_mem )
	{
		GLCLUBMEMBER& sClubMember = (*iter_mem).second;
		const bool bOnline = sClubMember.bONLINE;
		const int nChannel = sClubMember.nCHANNEL;
		const DWORD dwID = sClubMember.dwID;

		// 접속중
		if ( !bOnline || nMyChannel != nChannel )	continue;		
		
		// 자신인가
		if ( dwID == dwCharID ) continue;		
		
		// 같은 파티인가
		if ( GLPartyClient::GetInstance().IsMember( sClubMember.m_szName ) ) continue;
				
		// 같은 맵이면
		if ( pLand->GetMapID () != sClubMember.nidMAP ) continue;; 

		D3DXVECTOR3 vPos = sClubMember.vecPOS;

		if ( iter_guild != iter_guild_end )
		{
			CUIControl* pControl = (CUIControl*) (*iter_guild);
			UPDATE_CHAR_POS( pControl, vPos );

			iter_guild++;
		}
		else
		{
			// 만들어서 넣음
			CUIControl* pControl = new CUIControl;
			pControl->CreateSub( this, "MINIMAP_GUILD_MARK" );
			pControl->InitDeviceObjects ( m_pd3dDevice );
			pControl->RestoreDeviceObjects ( m_pd3dDevice );
			pControl->SetVisibleSingle( FALSE );
			RegisterControl( pControl );


			m_vecGuild.push_back ( pControl );
			UPDATE_CHAR_POS( pControl, vPos );
			
			iter_guild = m_vecGuild.end();
			iter_guild_end = m_vecGuild.end();
		}
	}
}

void CLargeMapWindow::RESET_GUILD()
{
	GUILD_POS_VECTOR_ITER iter_guild = m_vecGuild.begin();
	GUILD_POS_VECTOR_ITER iter_guild_end = m_vecGuild.end();
	
	for ( ; iter_guild != iter_guild_end; ++iter_guild )
	{
		CUIControl* pControl = (CUIControl*)(*iter_guild);
		pControl->SetVisibleSingle( FALSE );
	}	
}
bool CLargeMapWindow::IS_GUILD_UPDATE()
{
	//	클럽 확인
	GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter()->m_sCLUB;
	if ( sCLUB.m_dwID==CLUB_NULL ) return false;

	if ( sCLUB.GetBattleNum() <= 0 ) return false;

	// 클럽 배틀존에서만 보임
	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( pLand && !pLand->IsClubBattleZone() )	return false;
	
	return true;
}

void CLargeMapWindow::UPDATE_GUILD_INFO( bool bUpdate )
{
	if ( IS_GUILD_UPDATE() ) 
	{
		GLGaeaClient::GetInstance().GetCharacter()->ReqClubInfoUpdate( bUpdate );
	}
}
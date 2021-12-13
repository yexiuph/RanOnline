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
		UPDATE_PARTY_INS( pSelf ); // �߰�
	}
	//else // Note : ��Ƽ�� �ƴ϶�� �� �� �����ϴ� ��� ��Ʈ���� �����.
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
	GLPARTY_CLIENT* pMaster = GLPartyClient::GetInstance().GetMaster();	//	������
	if( !pMaster ) return ;

	if( ( pSelf != pMaster ) && IsSameMap( pSelf, pMaster ) )
	{
		//AddPartyMemberOnMAP( pMaster->m_dwGaeaID, pMaster->m_vPos, 0 );
		UPDATE_CHAR_POS ( m_pMark[0], pMaster->m_vPos );
	}

	DWORD nMEMBER_NUM = GLPartyClient::GetInstance().GetMemberNum();
	if( nMEMBER_NUM > 1 )
	{
		nMEMBER_NUM -= 1; // Note : �����ʹ� ����
		for ( DWORD nIndex = 0; nIndex < nMEMBER_NUM; ++nIndex )
		{
			GLPARTY_CLIENT *pMember = GLPartyClient::GetInstance().GetMember( nIndex );

			if( !pMember )						continue;
			if( !IsSameMap( pSelf, pMember ) )	continue;
			if( pSelf == pMember )				continue;

			//	Note : AddPartyMemberOnMAP�� ������ �Ķ���Ϳ� +1�� �����Ѱ���
			//         �ǵ��� ���Դϴ�. Master�� 0, ����� 1���ͽ����ؼ� 7���� �����մϴ�.
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
	//	�����Ͱ� �ƴϸ�, �ڽĵ��� ã�ƺ���.
	GLPARTY_CLIENT *pMaster = GLPartyClient::GetInstance().GetMaster();	//	������

	if( pMaster && pMaster->ISONESELF() )
	{
		return pMaster;
	}
	else
	{ // Note : �ڱ�ã��
		DWORD nMEMBER_NUM = GLPartyClient::GetInstance().GetMemberNum();
		if( nMEMBER_NUM > 1 )
		{
			nMEMBER_NUM -= 1; // Note : �����ʹ� ����
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

	//	Ŭ�� Ȯ��
	GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter()->m_sCLUB;

	// Ŭ�� ��Ʋ�������� ����
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


	// ��� ��ȸ
	for ( ; iter_mem != iter_mem_end; ++iter_mem )
	{
		GLCLUBMEMBER& sClubMember = (*iter_mem).second;
		const bool bOnline = sClubMember.bONLINE;
		const int nChannel = sClubMember.nCHANNEL;
		const DWORD dwID = sClubMember.dwID;

		// ������
		if ( !bOnline || nMyChannel != nChannel )	continue;		
		
		// �ڽ��ΰ�
		if ( dwID == dwCharID ) continue;		
		
		// ���� ��Ƽ�ΰ�
		if ( GLPartyClient::GetInstance().IsMember( sClubMember.m_szName ) ) continue;
				
		// ���� ���̸�
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
			// ���� ����
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
	//	Ŭ�� Ȯ��
	GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter()->m_sCLUB;
	if ( sCLUB.m_dwID==CLUB_NULL ) return false;

	if ( sCLUB.GetBattleNum() <= 0 ) return false;

	// Ŭ�� ��Ʋ�������� ����
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
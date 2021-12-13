#include "pch.h"
#include "s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* Client->Agent : �ɸ��� ����
*/
void CAgentServer::MsgSndChaDelInfo( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;
	
	// ������ ������� ����, ���� ���, ��������
	if (m_pClientManager->IsAccountPass( pMsg->dwClient ) == false)
	{
		CloseClient( pMsg->dwClient );
		return;
	}
		
	NET_CHA_DEL* ncd = reinterpret_cast<NET_CHA_DEL*> (pMsg->Buffer);

	DWORD dwClient  = pMsg->dwClient;
	int   nUserNum  = m_pClientManager->GetUserNum( dwClient );

    if (ncd->nChaNum > 0)
	{
		if (m_nServiceProvider == SP_CHINA)
		{
			m_Tea.decrypt( ncd->szPass2, USR_PASS_LENGTH );
		}
		else
		{
			m_Tea.decrypt( ncd->szPass2, USR_PASS_LENGTH+1 );
		}

		CDelCharacter* pAction = new CDelCharacter(
			nUserNum,
			ncd->nChaNum,
			ncd->szPass2,
			dwClient,
			m_pClientManager->GetClientIP( dwClient ),
			m_pClientManager->GetClientPort( dwClient ) );
		COdbcManager::GetInstance()->AddJob( (CDbAction*) pAction );
	}
}

/**
* ���Ӽ���->Ŭ���̾�Ʈ : ĳ���� ���� ��� ����
*/
void CAgentServer::MsgSndChaDelInfoBack(
		int nChaNum, 
		int nUserNum, 
		int nResult, 
		DWORD dwClient, 
		const TCHAR* szUserIP, 
		USHORT uPort )
{
	NET_CHA_DEL_FB ncbi;
	int nExtremeM = 0;
	int nExtremeW = 0;
	int nExtremeDBM = 0;
	int nExtremeDBW = 0;

    // nResult
    // -2 : ��帶���� �������� �ʿ��ϴ�
	// -1 : db error
    //  0 : ��������
	//  1 : �ذ��� ���� ����
    //  2 : �ذ��� ���� ����
     if ( nResult == DB_OK || 1 == nResult || 2 == nResult ) // ��������
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		if( 1 == nResult )
		{
			// DB������ ���ؼ� ����� �ذ��� ���� ������ �޾ƿ´�.
			nExtremeDBM = m_pClientManager->GetExtremeDBM( dwClient );
			nExtremeDBW = m_pClientManager->GetExtremeDBW( dwClient );
			// ���������� ���ؼ� ����� �ذ��� ���� ������ �޾ƿ´�.
			nExtremeM = m_pClientManager->GetExtremeM( dwClient );
			nExtremeW = m_pClientManager->GetExtremeW( dwClient );
			if( nExtremeDBM < 0)
			{	
				nExtremeDBM += 1;
				if( nExtremeDBM < 0 )
				{
					nExtremeDBW -= 1;
				}
				m_pClientManager->SetExtremeCreateDBM( dwClient, nExtremeDBM );
				m_pClientManager->SetExtremeCreateDBW( dwClient, nExtremeDBW );

				nExtremeW = nExtremeDBW;
				m_pClientManager->SetExtremeCreateW( dwClient, nExtremeW );
			}
			else
			{
				nExtremeM += 1;
				nExtremeDBM += 1;
				m_pClientManager->SetExtremeCreateM( dwClient, nExtremeM );
				m_pClientManager->SetExtremeCreateDBM( dwClient, nExtremeDBM );
			}
		}

		if( 2 == nResult )
		{
			// DB������ ���ؼ� ����� �ذ��� ���� ������ �޾ƿ´�.
			nExtremeDBM = m_pClientManager->GetExtremeDBM( dwClient );
			nExtremeDBW = m_pClientManager->GetExtremeDBW( dwClient );
			// ���������� ���ؼ� ����� �ذ��� ���� ������ �޾ƿ´�.
			nExtremeM = m_pClientManager->GetExtremeM( dwClient );
			nExtremeW = m_pClientManager->GetExtremeW( dwClient );

			if( nExtremeDBW < 0 )
			{
				nExtremeDBW += 1;
				
				if( nExtremeDBW < 0 )
				{
					nExtremeDBM -= 1;
				}
				m_pClientManager->SetExtremeCreateDBM( dwClient, nExtremeDBM );
				m_pClientManager->SetExtremeCreateDBW( dwClient, nExtremeDBW );

				nExtremeM = nExtremeDBM;
				m_pClientManager->SetExtremeCreateM( dwClient, nExtremeM );
			}
			else
			{
				nExtremeW += 1;
				nExtremeDBW += 1;
				m_pClientManager->SetExtremeCreateW( dwClient, nExtremeW );
				m_pClientManager->SetExtremeCreateDBW( dwClient, nExtremeDBW );
			}
		}
		
		// �ذ��� ��/�� ���� ���� ���ڸ� ĳ���� ���������ϰ� Ŭ���̾�Ʈ���� �Ѱ��ش�.
		ncbi.nExtremeM = m_pClientManager->GetExtremeM( dwClient );
		ncbi.nExtremeW = m_pClientManager->GetExtremeW( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // �׽�Ʈ���� ĳ���� �������� ����
            CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
			if ( wChaRemain < 0 ||
				 wChaRemain >= MAX_CHAR_LENGTH )
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain( dwClient, wChaRemain );
            // ĳ���� �������� ����
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
			CSetClubMasterFlags* pAction2 = new CSetClubMasterFlags((DWORD) nGuNum, (DWORD) nChaNum, 0);
			COdbcManager::GetInstance()->AddJob((CDbAction*) pAction2);
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}

/**
* Client->Agent:ĳ���� ������û
* \param pMsg 
*/
void CAgentServer::GspMsgSndChaDelInfo( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	// ������ ������� ����, ���� ���, ��������
	if ( m_pClientManager->IsAccountPass( pMsg->dwClient ) == false )
	{
		CloseClient( pMsg->dwClient );
		return;
	}
	
	GSP_NET_CHA_DEL* pMsg2 = reinterpret_cast<GSP_NET_CHA_DEL*> (pMsg->Buffer);

	if (pMsg2 == NULL) return;

	DWORD dwClient  = pMsg->dwClient;
	int   nUserNum  = m_pClientManager->GetUserNum( dwClient );

    if (pMsg2->nChaNum > 0)
	{
		CGspDelCharacter* pAction = new CGspDelCharacter(
											nUserNum, 
											pMsg2->nChaNum,
											dwClient, 
											m_pClientManager->GetClientIP( dwClient ),
											m_pClientManager->GetClientPort( dwClient ) );
		COdbcManager::GetInstance()->AddJob( (CDbAction*) pAction );	
	}
}

/**
 * Agent->Client : ĳ���� ������û
 * \param pMsg 
 */
void CAgentServer::TerraMsgSndChaDelInfo(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	// ������ ������� ����, ���� ���, ��������
	if (m_pClientManager->IsAccountPass(pMsg->dwClient) == false)
	{
		CloseClient(pMsg->dwClient);
		return;
	}
	
	TERRA_NET_CHA_DEL* ncd = reinterpret_cast<TERRA_NET_CHA_DEL*> (pMsg->Buffer);

	if (ncd == NULL) return;

	DWORD dwClient  = pMsg->dwClient;
	int   nUserNum  = m_pClientManager->GetUserNum(dwClient);

    if (ncd->nChaNum > 0)
	{
		CTerraDelCharacter* pAction = new CTerraDelCharacter(nUserNum, 
														     ncd->nChaNum,
														     dwClient, 
														     m_pClientManager->GetClientIP(dwClient),
														     m_pClientManager->GetClientPort(dwClient));
		COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);	
	}
}

/**
* Agent->Client:ĳ���� �����������
* \param nChaNum ĳ���͹�ȣ
* \param nUserNum ������ȣ
* \param nResult ���
* \param dwClient Ŭ���̾�Ʈ��ȣ
* \param szUserIP IP
* \param uPort PORT
*/
void CAgentServer::GspMsgSndChaDelInfoBack(
	int nChaNum, 
	int nUserNum, 
	int nResult, 
	DWORD dwClient, 
	const TCHAR* szUserIP, 
	USHORT uPort )
{
	NET_CHA_DEL_FB ncbi;

    // nResult
    // -2 : ��帶���� �������� �ʿ��ϴ�
    //  0 : ��������
    // -1 : db error
    if (nResult == DB_OK) // ��������
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain( dwClient ) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain( dwClient, wChaRemain );
            // �׽�Ʈ���� ĳ���� �������� ����
            CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease( nUserNum );
            COdbcManager::GetInstance()->AddLogJob( (CDbAction*) pAction );
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain( dwClient ) + 1;
			if ( wChaRemain < 0 ||
				 wChaRemain >= MAX_CHAR_LENGTH )
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain( dwClient, wChaRemain );
            // ĳ���� �������� ����
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease( nUserNum );
            COdbcManager::GetInstance()->AddLogJob( (CDbAction*) pAction );
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum( (DWORD) nChaNum );
        if ( (nGuNum != DB_ERROR) && (nGuNum > 0) )
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg( dwClient, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
			CSetClubMasterFlags* pAction2 = new CSetClubMasterFlags( (DWORD) nGuNum, (DWORD) nChaNum, 0 );
			COdbcManager::GetInstance()->AddJob( (CDbAction*) pAction2 );
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain( dwClient );
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain( dwClient );
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain( dwClient );
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain( dwClient );
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ( (strcmp( szUserIP, m_pClientManager->GetClientIP(dwClient) ) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient( dwClient, &ncbi );
	}
}

/**
* Agent->Client:ĳ���� �����������
* \param nChaNum ĳ���͹�ȣ
* \param nUserNum ������ȣ
* \param nResult ���
* \param dwClient Ŭ���̾�Ʈ��ȣ
* \param szUserIP IP
* \param uPort PORT
*/
void CAgentServer::TerraMsgSndChaDelInfoBack(int nChaNum, 
	                                         int nUserNum, 
							                 int nResult, 
							                 DWORD dwClient, 
							                 const char* szUserIP, 
							                 USHORT uPort)
{
	NET_CHA_DEL_FB ncbi;
	int nExtremeM = 0;
	int nExtremeW = 0;

    // nResult
    // -2 : ��帶���� �������� �ʿ��ϴ�
	// -1 : db error
    //  0 : ��������    
	//  1 : �ذ��� ���� ����
    //  2 : �ذ��� ���� ����
    if ( nResult == DB_OK || 1 == nResult || 2 == nResult ) // ��������
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		if( 1 == nResult )
		{
			// �ذ��� ���� ���� ���� ���ڸ� �޾ƿ´�.
			nExtremeM = m_pClientManager->GetExtremeM( dwClient );
			nExtremeM += 1;
			m_pClientManager->SetExtremeCreateM( dwClient, nExtremeM );
		}

		if( 2 == nResult )
		{
			// �ذ��� ���� ���� ���� ���ڸ� �޾ƿ´�.
			nExtremeW = m_pClientManager->GetExtremeW( dwClient );
			nExtremeW += 1;
			m_pClientManager->SetExtremeCreateW( dwClient, nExtremeW );
		}
		
		// �ذ��� ��/�� ���� ���� ���ڸ� ĳ���� ���������ϰ� Ŭ���̾�Ʈ���� �Ѱ��ش�.
		ncbi.nExtremeM = m_pClientManager->GetExtremeM( dwClient );
		ncbi.nExtremeW = m_pClientManager->GetExtremeW( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // �׽�Ʈ���� ĳ���� �������� ����
            CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
			if ( wChaRemain < 0 ||
				 wChaRemain >= MAX_CHAR_LENGTH )
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // ĳ���� �������� ����
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
			CSetClubMasterFlags* pAction2 = new CSetClubMasterFlags((DWORD) nGuNum, (DWORD) nChaNum, 0);
			COdbcManager::GetInstance()->AddJob((CDbAction*) pAction2);
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}

/**
 * Agent->Client : ĳ���� ������û
 * \param pMsg 
 */
void CAgentServer::DaumMsgSndChaDelInfo(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	// ������ ������� ����, ���� ���, ��������
	if (m_pClientManager->IsAccountPass(pMsg->dwClient) == false)
	{
		CloseClient(pMsg->dwClient);
		return;
	}
	
	DAUM_NET_CHA_DEL* ncd = reinterpret_cast<DAUM_NET_CHA_DEL*> (pMsg->Buffer);

	if (ncd == NULL) return;

	DWORD dwClient  = pMsg->dwClient;
	int   nUserNum  = m_pClientManager->GetUserNum(dwClient);

    if (ncd->nChaNum > 0)
	{
		CDaumDelCharacter* pAction = new CDaumDelCharacter(nUserNum, 
														ncd->nChaNum,
														dwClient, 
														m_pClientManager->GetClientIP(dwClient),
														m_pClientManager->GetClientPort(dwClient));
		COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);	
	}
}

void CAgentServer::DaumMsgSndChaDelInfoBack(int nChaNum,
											int nUserNum,
											int nResult,
											DWORD dwClient,
											const char* szUserIP,
											USHORT uPort)
{
    NET_CHA_DEL_FB ncbi;
	int nExtremeM = 0;
	int nExtremeW = 0;

    // nResult
    // -2 : ��帶���� �������� �ʿ��ϴ�
	// -1 : db error
    //  0 : ��������    
	//  1 : �ذ��� ���� ����
    //  2 : �ذ��� ���� ����
    if ( nResult == DB_OK || 1 == nResult || 2 == nResult ) // ��������
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		if( 1 == nResult )
		{
			// �ذ��� ���� ���� ���� ���ڸ� �޾ƿ´�.
			nExtremeM = m_pClientManager->GetExtremeM( dwClient );
			nExtremeM += 1;
			m_pClientManager->SetExtremeCreateM( dwClient, nExtremeM );
		}

		if( 2 == nResult )
		{
			// �ذ��� ���� ���� ���� ���ڸ� �޾ƿ´�.
			nExtremeW = m_pClientManager->GetExtremeW( dwClient );
			nExtremeW += 1;
			m_pClientManager->SetExtremeCreateW( dwClient, nExtremeW );
		}
		
		// �ذ��� ��/�� ���� ���� ���ڸ� ĳ���� ���������ϰ� Ŭ���̾�Ʈ���� �Ѱ��ش�.
		ncbi.nExtremeM = m_pClientManager->GetExtremeM( dwClient );
		ncbi.nExtremeW = m_pClientManager->GetExtremeW( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // �׽�Ʈ���� ĳ���� �������� ����
            CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
            if ( wChaRemain < 0 ||
				 wChaRemain >= MAX_CHAR_LENGTH )
			{
				wChaRemain = 0;
			}

			ncbi.wChaRemain = wChaRemain;			
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // ĳ���� �������� ����
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
			CSetClubMasterFlags* pAction2 = new CSetClubMasterFlags((DWORD) nGuNum, (DWORD) nChaNum, 0);
			COdbcManager::GetInstance()->AddJob((CDbAction*) pAction2);
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}

/**
* Client->Agent:ĳ���� ������û
* \param pMsg 
*/
void CAgentServer::ExciteMsgSndChaDelInfo(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	// ������ ������� ����, ���� ���, ��������
	if (m_pClientManager->IsAccountPass(pMsg->dwClient) == false)
	{
		CloseClient(pMsg->dwClient);
		return;
	}
	
	EXCITE_NET_CHA_DEL* ncd = reinterpret_cast<EXCITE_NET_CHA_DEL*> (pMsg->Buffer);

	if (ncd == NULL) return;

	DWORD dwClient  = pMsg->dwClient;
	int   nUserNum  = m_pClientManager->GetUserNum(dwClient);

    if (ncd->nChaNum > 0)
	{
		CExciteDelCharacter* pAction = new CExciteDelCharacter(nUserNum, 
														ncd->nChaNum,
														dwClient, 
														m_pClientManager->GetClientIP(dwClient),
														m_pClientManager->GetClientPort(dwClient));
		COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);	
	}
}

void CAgentServer::ExciteMsgSndChaDelInfoBack(int nChaNum,
											  int nUserNum,
											  int nResult,
											  DWORD dwClient,
											  const char* szUserIP,
											  USHORT uPort)
{
    NET_CHA_DEL_FB ncbi;

    // nResult
    // -2 : ��帶���� �������� �ʿ��ϴ�
    //  0 : ��������
    // -1 : db error
    if (nResult == DB_OK) // ��������
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // �׽�Ʈ���� ĳ���� �������� ����
            CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
			if ( wChaRemain < 0 ||
				 wChaRemain >= MAX_CHAR_LENGTH )
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // ĳ���� �������� ����
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
			CSetClubMasterFlags* pAction2 = new CSetClubMasterFlags((DWORD) nGuNum, (DWORD) nChaNum, 0);
			COdbcManager::GetInstance()->AddJob((CDbAction*) pAction2);
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}


/**
* Client->Agent:ĳ���� ������û
* \param pMsg 
*/
void CAgentServer::JapanMsgSndChaDelInfo(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	// ������ ������� ����, ���� ���, ��������
	if (m_pClientManager->IsAccountPass(pMsg->dwClient) == false)
	{
		CloseClient(pMsg->dwClient);
		return;
	}
	
	JAPAN_NET_CHA_DEL* ncd = reinterpret_cast<JAPAN_NET_CHA_DEL*> (pMsg->Buffer);

	if (ncd == NULL) return;

	DWORD dwClient  = pMsg->dwClient;
	int   nUserNum  = m_pClientManager->GetUserNum(dwClient);

    if (ncd->nChaNum > 0)
	{
		CJapanDelCharacter* pAction = new CJapanDelCharacter(nUserNum, 
														ncd->nChaNum,
														dwClient, 
														m_pClientManager->GetClientIP(dwClient),
														m_pClientManager->GetClientPort(dwClient));
		COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);	
	}
}

void CAgentServer::JapanMsgSndChaDelInfoBack(int nChaNum,
											  int nUserNum,
											  int nResult,
											  DWORD dwClient,
											  const char* szUserIP,
											  USHORT uPort)
{
    NET_CHA_DEL_FB ncbi;
	int nExtremeM = 0;
	int nExtremeW = 0;

	// -1 : db error
    //  0 : ��������    
	//  1 : �ذ��� ���� ����
    //  2 : �ذ��� ���� ����
    if ( nResult == DB_OK || 1 == nResult || 2 == nResult ) // ��������
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		if( 1 == nResult )
		{
			// �ذ��� ���� ���� ���� ���ڸ� �޾ƿ´�.
			nExtremeM = m_pClientManager->GetExtremeM( dwClient );
			nExtremeM += 1;
			m_pClientManager->SetExtremeCreateM( dwClient, nExtremeM );
		}

		if( 2 == nResult )
		{
			// �ذ��� ���� ���� ���� ���ڸ� �޾ƿ´�.
			nExtremeW = m_pClientManager->GetExtremeW( dwClient );
			nExtremeW += 1;
			m_pClientManager->SetExtremeCreateW( dwClient, nExtremeW );
		}
		
		// �ذ��� ��/�� ���� ���� ���ڸ� ĳ���� ���������ϰ� Ŭ���̾�Ʈ���� �Ѱ��ش�.
		ncbi.nExtremeM = m_pClientManager->GetExtremeM( dwClient );
		ncbi.nExtremeW = m_pClientManager->GetExtremeW( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // �׽�Ʈ���� ĳ���� �������� ����
            CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
			if ( wChaRemain < 0 ||
				 wChaRemain >= MAX_CHAR_LENGTH )
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // ĳ���� �������� ����
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
			CSetClubMasterFlags* pAction2 = new CSetClubMasterFlags((DWORD) nGuNum, (DWORD) nChaNum, 0);
			COdbcManager::GetInstance()->AddJob((CDbAction*) pAction2);
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}


/**
* Client->Agent : �ɸ��� ����
*/
void CAgentServer::GsMsgSndChaDelInfo( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;
	
	// ������ ������� ����, ���� ���, ��������
	if (m_pClientManager->IsAccountPass( pMsg->dwClient ) == false)
	{
		CloseClient( pMsg->dwClient );
		return;
	}
		
	GS_NET_CHA_DEL* ncd = reinterpret_cast<GS_NET_CHA_DEL*> (pMsg->Buffer);

	DWORD dwClient  = pMsg->dwClient;
	int   nUserNum  = m_pClientManager->GetUserNum( dwClient );

    if (ncd->nChaNum > 0)
	{
		m_Tea.decrypt( ncd->szPass2, GS_USER_PASS+1 );

		CGsDelCharacter* pAction = new CGsDelCharacter(
			nUserNum,
			ncd->nChaNum,
			ncd->szPass2,
			dwClient,
			m_pClientManager->GetClientIP( dwClient ),
			m_pClientManager->GetClientPort( dwClient ) );
		COdbcManager::GetInstance()->AddJob( (CDbAction*) pAction );
	}
}

/**
* ���Ӽ���->Ŭ���̾�Ʈ : ĳ���� ���� ��� ����
*/
void CAgentServer::GsMsgSndChaDelInfoBack(
		int nChaNum, 
		int nUserNum, 
		int nResult, 
		DWORD dwClient, 
		const TCHAR* szUserIP, 
		USHORT uPort )
{
    NET_CHA_DEL_FB ncbi;

    // nResult
    // -2 : ��帶���� �������� �ʿ��ϴ�
    //  0 : ��������
    // -1 : db error
    if (nResult == DB_OK) // ��������
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // �׽�Ʈ���� ĳ���� �������� ����
            CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
			if ( wChaRemain < 0 ||
				 wChaRemain >= MAX_CHAR_LENGTH )
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // ĳ���� �������� ����
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
			CSetClubMasterFlags* pAction2 = new CSetClubMasterFlags((DWORD) nGuNum, (DWORD) nChaNum, 0);
			COdbcManager::GetInstance()->AddJob((CDbAction*) pAction2);
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}
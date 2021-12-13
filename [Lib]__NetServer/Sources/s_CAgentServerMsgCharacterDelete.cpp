#include "pch.h"
#include "s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* Client->Agent : 케릭터 삭제
*/
void CAgentServer::MsgSndChaDelInfo( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;
	
	// 인증을 통과하지 못함, 삭제 취소, 연결종료
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
* 게임서버->클라이언트 : 캐릭터 삭제 결과 전송
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
    // -2 : 길드마스터 길드삭제가 필요하다
	// -1 : db error
    //  0 : 삭제성공
	//  1 : 극강부 남자 삭제
    //  2 : 극강부 여자 삭제
     if ( nResult == DB_OK || 1 == nResult || 2 == nResult ) // 삭제성공
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		if( 1 == nResult )
		{
			// DB연산을 통해서 저장된 극강부 생성 갯수를 받아온다.
			nExtremeDBM = m_pClientManager->GetExtremeDBM( dwClient );
			nExtremeDBW = m_pClientManager->GetExtremeDBW( dwClient );
			// 로직연산을 통해서 저장된 극강부 생성 갯수를 받아온다.
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
			// DB연산을 통해서 저장된 극강부 생성 갯수를 받아온다.
			nExtremeDBM = m_pClientManager->GetExtremeDBM( dwClient );
			nExtremeDBW = m_pClientManager->GetExtremeDBW( dwClient );
			// 로직연산을 통해서 저장된 극강부 생성 갯수를 받아온다.
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
		
		// 극강부 남/여 생성 가능 숫자를 캐릭터 삭제성공하고 클라이언트에게 넘겨준다.
		ncbi.nExtremeM = m_pClientManager->GetExtremeM( dwClient );
		ncbi.nExtremeW = m_pClientManager->GetExtremeW( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 감소
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
            // 캐릭터 생성갯수 증가
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
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
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}

/**
* Client->Agent:캐릭터 삭제요청
* \param pMsg 
*/
void CAgentServer::GspMsgSndChaDelInfo( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
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
 * Agent->Client : 캐릭터 삭제요청
 * \param pMsg 
 */
void CAgentServer::TerraMsgSndChaDelInfo(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
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
* Agent->Client:캐릭터 삭제결과전송
* \param nChaNum 캐릭터번호
* \param nUserNum 유저번호
* \param nResult 결과
* \param dwClient 클라이언트번호
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
    // -2 : 길드마스터 길드삭제가 필요하다
    //  0 : 삭제성공
    // -1 : db error
    if (nResult == DB_OK) // 삭제성공
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain( dwClient ) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain( dwClient, wChaRemain );
            // 테스트서버 캐릭터 생성갯수 감소
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
            // 캐릭터 생성갯수 증가
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease( nUserNum );
            COdbcManager::GetInstance()->AddLogJob( (CDbAction*) pAction );
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum( (DWORD) nChaNum );
        if ( (nGuNum != DB_ERROR) && (nGuNum > 0) )
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg( dwClient, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
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
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain( dwClient );
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain( dwClient );
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ( (strcmp( szUserIP, m_pClientManager->GetClientIP(dwClient) ) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient( dwClient, &ncbi );
	}
}

/**
* Agent->Client:캐릭터 삭제결과전송
* \param nChaNum 캐릭터번호
* \param nUserNum 유저번호
* \param nResult 결과
* \param dwClient 클라이언트번호
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
    // -2 : 길드마스터 길드삭제가 필요하다
	// -1 : db error
    //  0 : 삭제성공    
	//  1 : 극강부 남자 삭제
    //  2 : 극강부 여자 삭제
    if ( nResult == DB_OK || 1 == nResult || 2 == nResult ) // 삭제성공
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		if( 1 == nResult )
		{
			// 극강부 남자 생성 가능 숫자를 받아온다.
			nExtremeM = m_pClientManager->GetExtremeM( dwClient );
			nExtremeM += 1;
			m_pClientManager->SetExtremeCreateM( dwClient, nExtremeM );
		}

		if( 2 == nResult )
		{
			// 극강부 여자 생성 가능 숫자를 받아온다.
			nExtremeW = m_pClientManager->GetExtremeW( dwClient );
			nExtremeW += 1;
			m_pClientManager->SetExtremeCreateW( dwClient, nExtremeW );
		}
		
		// 극강부 남/여 생성 가능 숫자를 캐릭터 삭제성공하고 클라이언트에게 넘겨준다.
		ncbi.nExtremeM = m_pClientManager->GetExtremeM( dwClient );
		ncbi.nExtremeW = m_pClientManager->GetExtremeW( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 감소
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
            // 캐릭터 생성갯수 증가
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
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
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}

/**
 * Agent->Client : 캐릭터 삭제요청
 * \param pMsg 
 */
void CAgentServer::DaumMsgSndChaDelInfo(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
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
    // -2 : 길드마스터 길드삭제가 필요하다
	// -1 : db error
    //  0 : 삭제성공    
	//  1 : 극강부 남자 삭제
    //  2 : 극강부 여자 삭제
    if ( nResult == DB_OK || 1 == nResult || 2 == nResult ) // 삭제성공
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		if( 1 == nResult )
		{
			// 극강부 남자 생성 가능 숫자를 받아온다.
			nExtremeM = m_pClientManager->GetExtremeM( dwClient );
			nExtremeM += 1;
			m_pClientManager->SetExtremeCreateM( dwClient, nExtremeM );
		}

		if( 2 == nResult )
		{
			// 극강부 여자 생성 가능 숫자를 받아온다.
			nExtremeW = m_pClientManager->GetExtremeW( dwClient );
			nExtremeW += 1;
			m_pClientManager->SetExtremeCreateW( dwClient, nExtremeW );
		}
		
		// 극강부 남/여 생성 가능 숫자를 캐릭터 삭제성공하고 클라이언트에게 넘겨준다.
		ncbi.nExtremeM = m_pClientManager->GetExtremeM( dwClient );
		ncbi.nExtremeW = m_pClientManager->GetExtremeW( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 증가
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
            // 캐릭터 생성갯수 증가
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
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
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}

/**
* Client->Agent:캐릭터 삭제요청
* \param pMsg 
*/
void CAgentServer::ExciteMsgSndChaDelInfo(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
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
    // -2 : 길드마스터 길드삭제가 필요하다
    //  0 : 삭제성공
    // -1 : db error
    if (nResult == DB_OK) // 삭제성공
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 증가
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
            // 캐릭터 생성갯수 증가
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
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
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}


/**
* Client->Agent:캐릭터 삭제요청
* \param pMsg 
*/
void CAgentServer::JapanMsgSndChaDelInfo(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
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
    //  0 : 삭제성공    
	//  1 : 극강부 남자 삭제
    //  2 : 극강부 여자 삭제
    if ( nResult == DB_OK || 1 == nResult || 2 == nResult ) // 삭제성공
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		if( 1 == nResult )
		{
			// 극강부 남자 생성 가능 숫자를 받아온다.
			nExtremeM = m_pClientManager->GetExtremeM( dwClient );
			nExtremeM += 1;
			m_pClientManager->SetExtremeCreateM( dwClient, nExtremeM );
		}

		if( 2 == nResult )
		{
			// 극강부 여자 생성 가능 숫자를 받아온다.
			nExtremeW = m_pClientManager->GetExtremeW( dwClient );
			nExtremeW += 1;
			m_pClientManager->SetExtremeCreateW( dwClient, nExtremeW );
		}
		
		// 극강부 남/여 생성 가능 숫자를 캐릭터 삭제성공하고 클라이언트에게 넘겨준다.
		ncbi.nExtremeM = m_pClientManager->GetExtremeM( dwClient );
		ncbi.nExtremeW = m_pClientManager->GetExtremeW( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 증가
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
            // 캐릭터 생성갯수 증가
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
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
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}


/**
* Client->Agent : 케릭터 삭제
*/
void CAgentServer::GsMsgSndChaDelInfo( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;
	
	// 인증을 통과하지 못함, 삭제 취소, 연결종료
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
* 게임서버->클라이언트 : 캐릭터 삭제 결과 전송
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
    // -2 : 길드마스터 길드삭제가 필요하다
    //  0 : 삭제성공
    // -1 : db error
    if (nResult == DB_OK) // 삭제성공
	{
		ncbi.nmg.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 증가
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
            // 캐릭터 생성갯수 증가
            CUserChaNumIncrease* pAction = new CUserChaNumIncrease(nUserNum);
            COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = COdbcManager::GetInstance()->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( dwClient, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
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
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((strcmp(szUserIP, m_pClientManager->GetClientIP(dwClient)) == 0) &&
		 (m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}
#include "pch.h"
#include "SelectServerPage.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "UITextControl.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "ModalWindow.h"
#include "OuterInterface.h"
#include "s_NetClient.h"
#include "DxGlobalStage.h"
#include "RANPARAM.h"
#include "SelectServerChannel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL	CSelectServerPage::InitServerPageChannel ()
{
	CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
	if ( pNetClient->IsOnline () )
	{
		if ( !pNetClient->IsGameSvrInfoEnd () )
		{
			if ( !COuterInterface::GetInstance().IsVisibleGroup ( MODAL_WINDOW_OUTER ) )
			{
				DoModalOuter ( ID2GAMEEXTEXT ( "SERVERSTAGE_1" ), MODAL_INFOMATION, OK, OUTER_MODAL_WAITSERVERINFO );
			}
		}
		else
		{			
			m_pServerInfo = pNetClient->GetSeverCurInfoLogin ();

			struct S_SERVER_INFO
			{
				bool	bUSE;
				int		nIndex;
				int		nUserAmount;

				S_SERVER_INFO () :
					bUSE ( false ),
					nIndex ( -1 ),
					nUserAmount ( 0 )
				{
				}
			};

			S_SERVER_INFO sSERVERINFO[MAX_SERVER_GROUP];			

			//	카운트
			{
				for ( int i = 0; i < MAX_SERVER_GROUP; ++i )
				{
					sSERVERINFO[i].nIndex = i;
					for ( int j = 0, index = 0; j < MAX_SERVER_NUMBER; ++j )
					{
						LPG_SERVER_CUR_INFO_LOGIN pServerInfo = m_pServerInfo + ( i * MAX_SERVER_NUMBER ) + j;
						if ( !pServerInfo || !pServerInfo->nServerMaxClient ) continue;
						
						sSERVERINFO[i].bUSE = true;
						sSERVERINFO[i].nUserAmount += pServerInfo->nServerCurrentClient;
					}
				}
			}
			
			//	정렬
			{
				for ( int i = 0; i < MAX_SERVER_GROUP; ++i )
				{
					for ( int j = 0; j < MAX_SERVER_GROUP - 1; ++j )
					{
						if ( sSERVERINFO[j].nUserAmount > sSERVERINFO[j+1].nUserAmount )
						{
							S_SERVER_INFO temp = sSERVERINFO[j];
							sSERVERINFO[j] = sSERVERINFO[j+1];
							sSERVERINFO[j+1] = temp;
						}                        
					}
				}
			}

			//	리스트 구성
			{
				for ( int i = 0; i < MAX_SERVER_GROUP; ++i )
				{
					if ( !sSERVERINFO[i].bUSE ) continue;

					int nGROUPINDEX = sSERVERINFO[i].nIndex;

					{	//	서버 이름
						CString strServerName;

						#ifdef CH_PARAM
							if( nGROUPINDEX <= 0 ) GASSERT( 0 && _T("nGROUPINDEX <= 0 is true.") );
							strServerName = RANPARAM::China_Region[RANPARAM::nChinaRegion].ServerName[nGROUPINDEX-1];
						#else
							strServerName = ID2GAMEEXTEXT("SERVER_NAME",nGROUPINDEX);
						#endif

						if( strServerName.IsEmpty() )
						{
							GASSERT( 0 && _T("A ServerName does not exist!") );
							break;
						}

						int nIndex = m_pServerList->AddTextNoSplit ( strServerName, NS_UITEXTCOLOR::DEFAULT );
						m_pServerList->SetTextData ( nIndex, nGROUPINDEX );				
					}
				}
			}

			const int nTotalLine = m_pServerList->GetTotalLine ();
			const int nLinePerOneView = m_pServerList->GetVisibleLine ();
			m_pScrollBar->SetVisibleSingle ( FALSE );
			if ( nLinePerOneView < nTotalLine )
			{
				m_pScrollBar->SetVisibleSingle ( TRUE );

				m_pServerList->SetTextAlign ( TEXT_ALIGN_CENTER_X );
			}
		}
	}	
	else	//	네트워크 연결이 실패한 경우
	{
		if ( !COuterInterface::GetInstance().IsVisibleGroup ( MODAL_WINDOW_OUTER ) )
		{
			if( !COuterInterface::GetInstance().IsLoginCancel() )
			{
				DoModalOuter ( ID2GAMEEXTEXT("SERVERSTAGE_4"), MODAL_QUESTION, OKCANCEL, OUTER_MODAL_CLOSEGAME );
			}
			else
			{
				if ( DxGlobalStage::GetInstance().GetNetClient()->ConnectLoginServer(RANPARAM::LoginAddress) > NET_ERROR )
				{
					DxGlobalStage::GetInstance().GetNetClient()->SndVersion ();
					DxGlobalStage::GetInstance().GetNetClient()->SndReqServerInfo();				
				}
				else
				{
					DoModalOuter( ID2GAMEEXTEXT ( "SERVERSTAGE_3" ), MODAL_INFOMATION, OKCANCEL, OUTER_MODAL_CLOSEGAME );
				}
			}
		}
	}

	return TRUE;
}

BOOL	CSelectServerPage::ConnectServerChannel ()
{
	if ( m_nIndex < 0 )
	{
//		DoModalOuter ( GetWndID (), ID2GAMEEXTEXT ("SERVERSTAGE_2"), MODAL_INFOMATION );
		return FALSE;
	}

	int nChannelSelect = m_pSelectServerChannel->GetSelected ();
	if ( nChannelSelect < 0 )
	{
		return FALSE;
	}
	
	DxGlobalStage::GetInstance().SetChannel( nChannelSelect ); // 선택된 채널을 저장

	DWORD dwServerGroup = m_pServerList->GetTextData ( m_nIndex );
	DWORD dwServerNumber = m_pSelectServerChannel->GetSelectedChannel( nChannelSelect );

	if ( dwServerGroup == UINT_MAX || dwServerNumber == UINT_MAX ) return FALSE;

	int nServerGroup = (int)dwServerGroup;
	int nServerNumber = (int)dwServerNumber;

	COuterInterface::GetInstance().SetConnectServerInfo ( nServerGroup, nServerNumber );
	
	//	접속할 서버 아이피,포트
	CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();	
	if ( pNetClient->ConnectGameServer ( nServerGroup, nServerNumber ) <= NET_ERROR )
	{
		//접속 실패 에러
		DoModalOuter ( ID2GAMEEXTEXT ("SERVERSTAGE_5"), MODAL_INFOMATION );					
		return FALSE;
	}

	return TRUE;
}

void CSelectServerPage::TranslateUIMessageChannel ( UIGUID ControlID, DWORD dwMsg )
{

	switch ( ControlID )
	{
	case SELECT_SERVER_QUIT:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DoModalOuter ( ID2GAMEINTEXT("CLOSE_GAME"), MODAL_QUESTION, OKCANCEL, OUTER_MODAL_CLOSEGAME );
				SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
			}
		}
		break;

	case SELECT_SERVER_LIST:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nIndex = m_pServerList->GetSelectPos ();				
				if ( nIndex < 0 || m_pServerList->GetCount () <= nIndex ) return ;

				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bDIFF = m_nIndex!=nIndex;
					m_nIndex = nIndex;

					if ( bDIFF ) LoadChannel ();					
				}

				if ( dwMsg & UIMSG_LB_DUP )
				{
//					Login ();
				}
			}
		}
		break;

	case SELECT_SERVER_CHANNEL:
		{
			if ( UIMSG_MOUSEIN_LOGIN & dwMsg )
			{
				int nIndex = m_pSelectServerChannel->GetSelected();
				if( m_nServerState[nIndex] == SERVER_NOVACANCY )
				{
					// 서버 접속 인원이 95%를 넘김, 접속 불가
					DoModalOuter( ID2GAMEEXTEXT ("LOGINSTAGE_FULL"), MODAL_INFOMATION, OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				}
				else 
				{
					Login ();
				}
			}
		}
		break;

	case SELECT_CHINA_AREA_BACK:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				COuterInterface::GetInstance().ToChinaAreaPage( GetWndID() );
			}
		}
		break;
	}
}

void CSelectServerPage::LoadChannel ()
{
	m_pSelectServerChannel->ResetAll ();

	const DWORD dwGROUP_INDEX = m_pServerList->GetTextData ( m_nIndex );

	m_pSelectServerChannel->SetTitleName ( m_pServerList->GetText ( m_nIndex ) );

	for ( int j = 0; j < MAX_SERVER_NUMBER; ++j )
	{
		LPG_SERVER_CUR_INFO_LOGIN pServerInfo = m_pServerInfo + ( dwGROUP_INDEX * MAX_SERVER_NUMBER ) + j;
		if ( !pServerInfo || !pServerInfo->nServerMaxClient ) continue;

		CString strCombine;
		CString strPK( ID2GAMEEXTEXT("SERVER_PK") );
				
		if( pServerInfo->bPK )
		{
			if( RANPARAM::emSERVICE_TYPE == EMSERVICE_JAPAN )
			{
				strCombine.Format ( "%s %d", ID2GAMEEXTEXT("SERVER_CHANNEL"), j );
			}
			else
			{
				strCombine.Format ( "%d %s%s", j, ID2GAMEEXTEXT("SERVER_CHANNEL"), strPK );
			}
		}
		else					
		{
			CString strSpace( ' ', strPK.GetLength() );

			if( RANPARAM::emSERVICE_TYPE == EMSERVICE_JAPAN )
			{
				strCombine.Format ( "%s %d %s", ID2GAMEEXTEXT("SERVER_CHANNEL"), j, strSpace );
			}
			else
			{
				strCombine.Format ( "%d %s%s", j, ID2GAMEEXTEXT("SERVER_CHANNEL"), strSpace );
			}
		}
		
		CString strChannelState;
		DWORD dwStateColor;

		{	//	서버 상태
			float fPercent = float(pServerInfo->nServerCurrentClient) / float(pServerInfo->nServerMaxClient);

			if( fPercent < ( GLCONST_CHAR::wSERVER_NORMAL * 0.01f ) )			m_nServerState[j] = SERVER_SMOOTH;
			else if( fPercent < ( GLCONST_CHAR::wSERVER_CONGEST * 0.01f ) )		m_nServerState[j] = SERVER_NORMAL;
			else if( fPercent < ( GLCONST_CHAR::wSERVER_NOVACANCY * 0.01f ) )	m_nServerState[j] = SERVER_CONGEST;
			else																m_nServerState[j] = SERVER_NOVACANCY;
			
			D3DCOLOR dwStateColorArray[4] = 
			{
				NS_UITEXTCOLOR::PALEGREEN,
				NS_UITEXTCOLOR::GOLD,
				NS_UITEXTCOLOR::RED,
				NS_UITEXTCOLOR::RED
			};

			strChannelState = ID2GAMEWORD("SERVER_STATE", m_nServerState[j]);
			dwStateColor = dwStateColorArray[m_nServerState[j]];
		}

		// 서버 이름
		m_pSelectServerChannel->AddChannel( strCombine, NS_UITEXTCOLOR::DEFAULT, strChannelState, dwStateColor, j );
	}

	m_pSelectServerChannel->SetVisibleSingle ( TRUE );
}

void CSelectServerPage::SetChannelFull()
{
	int nSelectChannel = m_pSelectServerChannel->GetSelected();
	const DWORD dwGROUP_INDEX = m_pServerList->GetTextData ( m_nIndex );

	LPG_SERVER_CUR_INFO_LOGIN pServerInfo = m_pServerInfo + ( dwGROUP_INDEX * MAX_SERVER_NUMBER ) + nSelectChannel;
	if ( !pServerInfo || !pServerInfo->nServerMaxClient ) return;

	pServerInfo->nServerCurrentClient = pServerInfo->nServerMaxClient;

}
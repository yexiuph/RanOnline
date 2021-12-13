#include "pch.h"
#include "OuterInterface.h"
#include "ModalWindow.h"
#include "GameTextControl.h"
#include "DxGlobalStage.h"
#include "RanParam.h"

#include "SelectServerPage.h"
#include "LoginPage.h"
#include "CreateCharacterPage.h"
#include "NewCreateCharPage.h"
#include "SelectCharacterPage.h"
#include "SelectCharacterLeftPage.h"
#include "WaitDialogue.h"
#include "SecPassCheckPage.h"
#include "SecPassSetPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void COuterInterface::ModalMsgProcess ( UIGUID nCallerID, DWORD dwMsg )
{
	switch ( nCallerID )
	{
	case OUTER_MODAL_CLOSEGAME:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
                DxGlobalStage::GetInstance().CloseGame();
			}
			else if ( dwMsg & UIMSG_MODAL_CANCEL )
			{
				if ( DxGlobalStage::GetInstance().GetNetClient()->ConnectLoginServer(RANPARAM::LoginAddress) > NET_ERROR )
				{
 					// ���� ����
					DxGlobalStage::GetInstance().GetNetClient()->SndReqServerInfo();				
				}
				else
				{
					//���� ����
					DoModalOuter ( ID2GAMEEXTEXT ( "SERVERSTAGE_3" ), MODAL_INFOMATION, OKCANCEL, OUTER_MODAL_CLOSEGAME );
				}
			}
		}
		break;

	case OUTER_MODAL_WAITSERVERINFO:
		{
			CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
			if ( pNetClient->IsOnline() )		//��Ʈ��ũ ������ ������ ���
			{
				if ( pNetClient->IsGameSvrInfoEnd() )
				{
					HideGroup ( MODAL_WINDOW_OUTER );
					return ;
				}
			}

			if ( dwMsg & UIMSG_MODAL_CANCEL )
			{
				DoModalOuter ( ID2GAMEEXTEXT ( "SERVERSTAGE_3" ), MODAL_INFOMATION, OKCANCEL, OUTER_MODAL_CLOSEGAME );				
			}
		}
		break;

	case OUTER_MODAL_RECONNECT:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				ToSelectServerPage( GetModalCallWindowID() );
			}
		}
		break;

	case OUTER_MODAL_WAITCONFIRM:
		{
			if ( dwMsg & UIMSG_MODAL_CANCEL )
			{
				DxGlobalStage::GetInstance().GetNetClient()->CloseConnect();
				ToSelectServerPage( GetModalCallWindowID() );
			}
		}	
		break;

	case OUTER_MODAL_ALREADYCONNECT:
	case OUTER_MODAL_CREATEFAILED:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				DxGlobalStage::GetInstance().CloseGame();
			}
		}
		break;

	case OUTER_MODAL_NON_PK_CHANNEL:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				if ( IsVisibleGroup ( SELECT_CHARACTER_PAGE ) )			HideGroup ( SELECT_CHARACTER_PAGE );
				if ( IsVisibleGroup ( SELECT_CHARACTER_LEFTPAGE ) )		HideGroup ( SELECT_CHARACTER_LEFTPAGE );
				if ( IsVisibleGroup ( CREATE_CHARACTER_PAGE ) )			HideGroup ( CREATE_CHARACTER_PAGE );
				if ( IsVisibleGroup ( CREATE_CHARACTER_WEB ) )			HideGroup ( CREATE_CHARACTER_WEB );

				//	Note : ��ſ� �������
				CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
				if ( pNetClient->IsOnline() == true )
					pNetClient->CloseConnect ();

				COuterInterface::GetInstance().ToSelectServerPage ( SELECT_SERVER_PAGE );
			}
		}
		break;

	case OUTER_MODAL_SECONDPASSWORD:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				WAITSERVER_DIALOGUE_OPEN ( ID2GAMEEXTEXT("WAITSERVER_DEL_CHARACTER"), WAITSERVER_DEL_CHARACTER, 10.0f );
			}
		}
		break;

	case OUTER_MODAL_LOGOUT:
	case OUTER_MODAL_CONNECTCLOSED:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				//	Note : ��ſ� �������
				CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
				if ( pNetClient->IsOnline() == true )
				{
					pNetClient->CloseConnect ();
					COuterInterface::GetInstance().SetCancelToLogin();
				}

				//				if ( pNetClient->ConnectLoginServer(RANPARAM::LoginAddress) <= NET_ERROR )
				//					CDebugSet::ToListView ( "CCharacterStage::Logout ConnectLoginServer NET_ERROR" );
				//				else
				//					CDebugSet::ToListView ( "CCharacterStage::Logout ConnectLoginServer NET_OK" );				

				if ( IsVisibleGroup ( SELECT_CHARACTER_PAGE ) )			HideGroup ( SELECT_CHARACTER_PAGE );
				if ( IsVisibleGroup ( SELECT_CHARACTER_LEFTPAGE ) )		HideGroup ( SELECT_CHARACTER_LEFTPAGE );
				if ( IsVisibleGroup ( CREATE_CHARACTER_PAGE ) )			HideGroup ( CREATE_CHARACTER_PAGE );
				if ( IsVisibleGroup ( CREATE_CHARACTER_WEB ) )			HideGroup ( CREATE_CHARACTER_WEB );

				if( IsVisibleGroup( SELECT_CHINA_AREA_PAGE ) )			
				{
					ToChinaAreaPage( SELECT_CHINA_AREA_PAGE );
				}
				else
				{
					ToSelectServerPage ( SELECT_CHARACTER_PAGE );
				}
			}
		}
		break;

	case OUTER_MODAL_CREATESUCCEED:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				ToSelectCharacterPage ( CREATE_CHARACTER_PAGE );
			}
		}
		break;

	case OUTER_MODAL_GENERAL_LOGINFAIL:
		{
			if( dwMsg & UIMSG_MODAL_OK )
			{
				if( RANPARAM::VALIDIDPARAM () )
				{
					UIGUID uiGUID = GetModalCallWindowID();

					if( uiGUID == SECPASS_SETPAGE )
						COuterInterface::GetInstance().GetSecPassSetPage()->ResetAll();
					else if( uiGUID == SECPASS_CHECKPAGE )
						COuterInterface::GetInstance().GetSecPassCheckPage()->ResetAll();
					else
						COuterInterface::GetInstance().ToSelectServerPage ( SELECT_SERVER_PAGE );
				}
				else
				{
					//COuterInterface::GetInstance().ToLoginPage ( LOGIN_PAGE );
					COuterInterface::GetInstance().GetLoginPage()->ResetAll();
				}
			}
		}
		break;

	case OUTER_MODAL_LOGIN_WAIT_DAUM:
		{
			//	NOTE
			//		���� ������ ��ȣ Ű�� ���� ������ ����Ѵ�.
			//		
			CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
			if ( pNetClient->IsKeyReceived () )
			{
				CString strTemp;

				//	��Ʈ�ѷ� ���� ���̵�� ��ȣ�� ������ ������ ����
				switch ( RANPARAM::emSERVICE_TYPE )
				{
				case EMSERVICE_KOREA:
					pNetClient->DaumSndLogin ( RANPARAM::GETIDPARAM(), m_nServerChannel );
					break;

				//case EMSERVICE_GSP:
				//	pNetClient->GspSndLogin ( RANPARAM::GETIDPARAM(), m_nServerChannel );
				//	break;

				case EMSERVICE_MALAYSIA_CN:
				case EMSERVICE_MALAYSIA_EN:
				case EMSERVICE_PHILIPPINES:
				case EMSERVICE_VIETNAM:
					pNetClient->TerraSndLogin ( RANPARAM::GETIDPARAM(), m_nServerChannel );
					break;

//		Japan �α��� ��� ����
//				case EMSERVICE_JAPAN:
//					pNetClient->ExciteSndLogin ( RANPARAM::GETIDPARAM(), RANPARAM::GETIDPARAM2(), RANPARAM::GETIDPARAM3(), m_nServerChannel );
//					break;

				default:
					CString strTemp;
					strTemp.Format( "OUTER_MODAL_LOGIN_WAIT_DAUM::default:ServiceType:%d", RANPARAM::emSERVICE_TYPE );
					AfxMessageBox( strTemp.GetString() );
					break;
				};

                START_WAIT_TIME ( 10.0f );
				DoModalOuter ( ID2GAMEEXTEXT ("DAUM_LOGIN_WAIT"), MODAL_INFOMATION, CANCEL, OUTER_MODAL_LOGIN_WAIT_DAUM_AFTER_KEYRECEIVED );
			}
			else			
			{
				//	NOTE
				//		�ð��� �ʰ��� ���
				//		�Ʒ��� ó������ �ٷ� ������ �� á�ٴ� �޽����� �Ѹ���.
				if ( GET_WAIT_TIME() < 0.0f )
				{
					DoModalOuter ( ID2GAMEEXTEXT ("DAUM_LOGIN_WAIT_SERVERFULL"), MODAL_INFOMATION, YESNO, OUTER_MODAL_LOGIN_WAIT_DAUM_SERVERFULL );
				}
			}
		}
		break;

	case OUTER_MODAL_LOGIN_WAIT_DAUM_AFTER_KEYRECEIVED:
		{
			//	NOTE
			//		�ð��� �ʰ��� ���
			//		�Ʒ��� ó������ �ٷ� ������ �� á�ٴ� �޽����� �Ѹ���.
			if ( GET_WAIT_TIME() < 0.0f )
			{
				DoModalOuter ( ID2GAMEEXTEXT ("DAUM_LOGIN_WAIT_SERVERFULL"), MODAL_INFOMATION, YESNO, OUTER_MODAL_LOGIN_WAIT_DAUM_SERVERFULL );
			}
		}
		break;

	case OUTER_MODAL_LOGIN_WAIT_DAUM_SERVERFULL:
		{
			//	NOTE
			//		�ٽ� �õ��غ���
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				GetSelectServerPage()->Login ();
			}
		}
		break;

	case OUTER_MODAL_CHARACTERSTAGE_GAME_JOIN_WAIT:
		{
			if( dwMsg & UIMSG_MODAL_CANCEL )
			{
				DxGlobalStage::GetInstance().SetGameJoin( FALSE );
				//	Note : ��ſ� �������
				CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
				if ( pNetClient->IsOnline() == true )
				{
					pNetClient->CloseConnect ();
					COuterInterface::GetInstance().SetCancelToLogin();
				}

				//				if ( pNetClient->ConnectLoginServer(RANPARAM::LoginAddress) <= NET_ERROR )
				//					CDebugSet::ToListView ( "CCharacterStage::Logout ConnectLoginServer NET_ERROR" );
				//				else
				//					CDebugSet::ToListView ( "CCharacterStage::Logout ConnectLoginServer NET_OK" );				

				if ( IsVisibleGroup ( SELECT_CHARACTER_PAGE ) )			HideGroup ( SELECT_CHARACTER_PAGE );
				if ( IsVisibleGroup ( SELECT_CHARACTER_LEFTPAGE ) )		HideGroup ( SELECT_CHARACTER_LEFTPAGE );
				if ( IsVisibleGroup ( CREATE_CHARACTER_PAGE ) )			HideGroup ( CREATE_CHARACTER_PAGE );
				if ( IsVisibleGroup ( CREATE_CHARACTER_WEB ) )			HideGroup ( CREATE_CHARACTER_WEB );

				if( IsVisibleGroup( SELECT_CHINA_AREA_PAGE ) )			
				{
					ToChinaAreaPage( SELECT_CHINA_AREA_PAGE );
				}
				else
				{
					ToSelectServerPage ( SELECT_CHARACTER_PAGE );
				}
			}
			else if ( GET_WAIT_TIME() < 0.0f )
			{
				//	NOTE
				//		�ð��� �ʰ��� ���
				//		�Ʒ��� ó������ �ٷ� ������ �� á�ٴ� �޽����� �Ѹ���.
				DoModalOuter ( ID2GAMEEXTEXT ("CHARACTERSTAGE_DAUM_GAME_JOIN_FAIL"), MODAL_INFOMATION, OK, OUTER_MODAL_CONNECTCLOSED );
			}
		}
		break;

	case OUTER_MODAL_NAME_ERROR:
	case OUTER_MODAL_THAI_CHECK_STRING_ERROR:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
// ĳ���� ���� Test 
//#if defined ( _RELEASED ) || defined( TW_PARAM ) || defined( KRT_PARAM ) || defined( KR_PARAM ) || defined( VN_PARAM ) || defined( TH_PARAM ) || defined( MYE_PARAM ) || defined( MY_PARAM ) || defined( ID_PARAM ) || defined( CH_PARAM ) || defined( PH_PARAM ) || defined ( HK_PARAM ) || defined( JP_PARAM ) || defined ( GS_PARAM ) 
				GetNewCreateCharacterPage()->ResetAll ();
//#else
//				GetCreateCharacterPage()->ResetAll ();
//#endif
			}
		}
		break;
	}
}
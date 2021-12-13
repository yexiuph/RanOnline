#include "pch.h"
#include "./CharacterWindow.h"

#include "../[Lib]__RanClient/Sources/G-Logic/Client/GLGaeaClient.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"

#include "./ModalCallerID.h"
#include "./ModalWindow.h"
#include "./InnerInterface.h"
#include "./UITextControl.h"
#include "./GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CCharacterWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	const float fBUTTON_CLICK = CBasicButton::fBUTTON_CLICK;

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				CInnerInterface::GetInstance().SetDefaultPosInterface( CHARACTER_WINDOW );
			}
		}
		break;
	case CHARACTER_POW_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pPowText->SetUseTextColor ( 0, TRUE );
				m_pPowText->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD );

				m_pPowTextStatic->SetUseTextColor ( 0, TRUE );
				m_pPowTextStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const UIRECT& rcPos = m_pPowDiaableButton->GetGlobalPos ();				
					m_pPowButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					GLGaeaClient::GetInstance().GetCharacter()->ReqStatsUp ( EMPOW );
				}
			}
		}
		break;

	case CHARACTER_DEX_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pDexText->SetUseTextColor ( 0, TRUE );
				m_pDexText->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				m_pDexTextStatic->SetUseTextColor ( 0, TRUE );
				m_pDexTextStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const UIRECT& rcPos = m_pDexDiaableButton->GetGlobalPos ();				
					m_pDexButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					GLGaeaClient::GetInstance().GetCharacter()->ReqStatsUp ( EMDEX );
				}
			}
		}
		break;

	case CHARACTER_SPI_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pSpiText->SetUseTextColor ( 0, TRUE );
				m_pSpiText->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				m_pSpiTextStatic->SetUseTextColor ( 0, TRUE );
				m_pSpiTextStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const UIRECT& rcPos = m_pSpiDiaableButton->GetGlobalPos ();				
					m_pSpiButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					GLGaeaClient::GetInstance().GetCharacter()->ReqStatsUp ( EMSPI );
				}
			}
		}
		break;

	//case CHARACTER_INT_BUTTON:
	//	{
	//		if ( CHECK_MOUSE_IN ( dwMsg ) )
	//		{
	//			m_pIntText->SetUseTextColor ( 0, TRUE );
	//			m_pIntText->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

	//			m_pIntTextStatic->SetUseTextColor ( 0, TRUE );
	//			m_pIntTextStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

	//			if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
	//			{
	//				const UIRECT& rcPos = m_pIntDiaableButton->GetGlobalPos ();				
	//				m_pIntButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
	//			}

	//			if ( CHECK_LB_UP_LIKE ( dwMsg ) )
	//			{					
	//				GLGaeaClient::GetInstance().GetCharacter()->ReqStatsUp ( EMINT );
	//			}
	//		}
	//	}
	//	break;

	case CHARACTER_STR_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pStrText->SetUseTextColor ( 0, TRUE );
				m_pStrText->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				m_pStrTextStatic->SetUseTextColor ( 0, TRUE );
				m_pStrTextStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const UIRECT& rcPos = m_pStrDiaableButton->GetGlobalPos ();				
					m_pStrButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					GLGaeaClient::GetInstance().GetCharacter()->ReqStatsUp ( EMSTR );
				}
			}
		}
		break;

	case CHARACTER_STA_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pStaText->SetUseTextColor ( 0, TRUE );
				m_pStaText->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				m_pStaTextStatic->SetUseTextColor ( 0, TRUE );
				m_pStaTextStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const UIRECT& rcPos = m_pStaDiaableButton->GetGlobalPos ();				
					m_pStaButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					GLGaeaClient::GetInstance().GetCharacter()->ReqStatsUp ( EMSTA );
				}
			}
		}
		break;

	case CHARACTER_PREMIUM_TEXT:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				bool bPREMIUM = GLGaeaClient::GetInstance().GetCharacter()->m_bPREMIUM;
				if ( bPREMIUM )
				{
					if ( CInnerInterface::GetInstance().BEGIN_COMMON_LINEINFO_MULTI () )
					{
						CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( ID2GAMEWORD("CHARACTER_PREMIUM_EXPIRE_DATE_STATIC"), NS_UITEXTCOLOR::DARKORANGE );

						__time64_t tPREMIUM = GLGaeaClient::GetInstance().GetCharacter()->m_tPREMIUM;
						CTime cTime ( tPREMIUM );
						CString strExpireDate = CInnerInterface::GetInstance().MakeString ( ID2GAMEWORD("CHARACTER_PREMIUM_EXPIRE_DATE"),
							cTime.GetYear(), cTime.GetMonth(), cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () );

						CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strExpireDate, NS_UITEXTCOLOR::WHITE );

						CInnerInterface::GetInstance().END_COMMON_LINEINFO_MULTI ();
					}					
				}
			}
		}
		break;

	case CHARACTER_BIRGHTER:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter ();

				CString strTemp;				
				strTemp.Format ( "%d", pCharacter->m_nBright );

				CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( strTemp, NS_UITEXTCOLOR::WHITE );
			}
		}
		break;

	case CHARACTER_MOBILE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DoModal( ID2GAMEINTEXT( "MODAL_MOBILE_NUMBER" ), MODAL_INPUT, EDITBOX_NUMBER, MODAL_MOBILE_NUMBER );
			}
		}
		break;
	}
}
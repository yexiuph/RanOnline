#include "pch.h"
#include "./GLCharacter.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/GambleBox.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/GambleAgainBox.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/GambleSelectBox.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/GambleAnimationBox.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/GambleResultOddBox.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/GambleResultEvenBox.h"

#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GLCharacter::MsgProcessMiniGame( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_MGAME_ODDEVEN_FB:
		{
			GLMSG::SNETPC_MGAME_ODDEVEN_FB *pNetMsg = (GLMSG::SNETPC_MGAME_ODDEVEN_FB*)nmg;

			switch( pNetMsg->emResult )
			{
			case EMMGAME_ODDEVEN_FB_OK:
				{
					CInnerInterface::GetInstance().PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMMGAME_ODDEVEN_FB_OK") );
					CInnerInterface::GetInstance().ShowGroupFocus ( GAMBLE_SELECT_DIALOGUE );

					OpenMiniGame();
				}
				break;

			case EMMGAME_ODDEVEN_FB_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMMGAME_ODDEVEN_FB_FAIL") );

					CInnerInterface::GetInstance().HideGroup( GAMBLE_DIALOGUE );
					CInnerInterface::GetInstance().HideGroup( GAMBLE_SELECT_DIALOGUE );
					CInnerInterface::GetInstance().HideGroup( GAMBLE_AGAIN_DIALOGUE );
					CInnerInterface::GetInstance().HideGroup( GAMBLE_ANIMATION_DIALOGUE );
				}
				break;

			case EMMGAME_ODDEVEN_FB_MONEY_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMMGAME_ODDEVEN_FB_MONEY_FAIL") );

					CInnerInterface::GetInstance().GetGambleBox()->ClearEditBox();
				}
				break;

			case EMMGAME_ODDEVEN_FB_MAXBATTING:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, 
																	ID2GAMEINTEXT("EMMGAME_ODDEVEN_FB_MAXBATTING"), 
																	MINIGAME_ODDEVEN::uiMaxBattingMoney );

					CInnerInterface::GetInstance().GetGambleBox()->ClearEditBox();
				}
				break;

			case EMMGAME_ODDEVEN_FB_MAXROUND:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMMGAME_ODDEVEN_FB_MAXROUND") );

					CInnerInterface::GetInstance().HideGroup( GAMBLE_DIALOGUE );
					CInnerInterface::GetInstance().HideGroup( GAMBLE_SELECT_DIALOGUE );
					CInnerInterface::GetInstance().HideGroup( GAMBLE_AGAIN_DIALOGUE );
					CInnerInterface::GetInstance().HideGroup( GAMBLE_ANIMATION_DIALOGUE );
				}
				break;

			case EMMGAME_ODDEVEN_FB_AGAIN_OK:
				{
					CInnerInterface::GetInstance().PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, 
																ID2GAMEINTEXT("EMMGAME_ODDEVEN_FB_AGAIN_OK"),
																pNetMsg->wRound );

					CInnerInterface::GetInstance().ShowGroupFocus ( GAMBLE_SELECT_DIALOGUE );
				}
				break;

			case EMMGAME_ODDEVEN_FB_WIN:
				{
					m_lnMoney += pNetMsg->ui64ActualMoney;
					DxSoundLib::GetInstance()->PlaySound ( "GAMBLING_WIN" );
					switch( pNetMsg->emCase )
					{
					case EMMGAME_ODDEVEN_ODD:
						{
							// 홀수 이미지 출력
							CInnerInterface::GetInstance().ShowGroupFocus ( GAMBLE_RESULT_ODD_DIALOGUE );
							CInnerInterface::GetInstance().GetGambleResultOddBox()->SetWinLose(TRUE);
						}
						break;

					case EMMGAME_ODDEVEN_EVEN:
						{
							// 짝수 이미지 출력
							CInnerInterface::GetInstance().ShowGroupFocus ( GAMBLE_RESULT_EVEN_DIALOGUE );
							CInnerInterface::GetInstance().GetGambleResultEvenBox()->SetWinLose(TRUE);
						}
						break;
					}

					CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, 
																	ID2GAMEINTEXT("EMMGAME_ODDEVEN_FB_WIN"), 
																	pNetMsg->ui64DividendMoney );
				}	
				break;

			case EMMGAME_ODDEVEN_FB_LOSE:
				{
					m_lnMoney -= pNetMsg->ui64DividendMoney;
					DxSoundLib::GetInstance()->PlaySound ( "GAMBLING_LOSE" );
					switch( pNetMsg->emCase )
					{
					case EMMGAME_ODDEVEN_ODD:
						{
							// 홀수 이미지 출력
							CInnerInterface::GetInstance().ShowGroupFocus ( GAMBLE_RESULT_ODD_DIALOGUE );
							CInnerInterface::GetInstance().GetGambleResultOddBox()->SetWinLose(FALSE);
						}
						break;

					case EMMGAME_ODDEVEN_EVEN:
						{
							// 짝수 이미지 출력
							CInnerInterface::GetInstance().ShowGroupFocus ( GAMBLE_RESULT_EVEN_DIALOGUE );
							CInnerInterface::GetInstance().GetGambleResultEvenBox()->SetWinLose(FALSE);
						}
						break;
					}

					CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMMGAME_ODDEVEN_FB_LOSE") );

					// 홀짝 게임에 관련된 열려진 모든창을 닫는다. - 김재우
					CInnerInterface::GetInstance().HideGroup( GAMBLE_DIALOGUE );
					CInnerInterface::GetInstance().HideGroup( GAMBLE_SELECT_DIALOGUE );
					CInnerInterface::GetInstance().HideGroup( GAMBLE_AGAIN_DIALOGUE );
					CInnerInterface::GetInstance().HideGroup( GAMBLE_ANIMATION_DIALOGUE );
				}
				break;

			case EMMGAME_ODDEVEN_FB_FINISH_OK:
				{
					if ( pNetMsg->wRound <= MINIGAME_ODDEVEN::MAX_ROUND )
					{
						// 게임을 다시 하겠냐는 인터페이스 출력
						CInnerInterface::GetInstance().ShowGroupFocus( GAMBLE_AGAIN_DIALOGUE );
						CInnerInterface::GetInstance().GetGambleAgainBox()->SetMoney(pNetMsg->wRound,pNetMsg->ui64DividendMoney);

						OpenMiniGame();
					}
				}
				break;
			}

			switch( pNetMsg->emResult )
			{
			case EMMGAME_ODDEVEN_FB_FAIL:
			case EMMGAME_ODDEVEN_FB_MONEY_FAIL:
			case EMMGAME_ODDEVEN_FB_MAXBATTING:
			case EMMGAME_ODDEVEN_FB_MAXROUND:
			case EMMGAME_ODDEVEN_FB_WIN:
			case EMMGAME_ODDEVEN_FB_LOSE:
				{
					CloseMiniGame();
				}
				break;
			}
		}
		break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess() 분류되지 않은 메시지 수신. TYPE[%d]", nmg->nType );
		break;
	};
}


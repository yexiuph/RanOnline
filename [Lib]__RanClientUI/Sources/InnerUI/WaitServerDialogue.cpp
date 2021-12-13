#include "pch.h"

#include "WaitServerDialogue.h"
#include "BasicLineBox.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextButton.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "GLCharacter.h"
#include "GLGaeaClient.h"
#include "DxGlobalStage.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWaitServerDialogue::CWaitServerDialogue () :
	m_pTextBox ( NULL ),
    m_pMoveOKButton ( NULL ),
	m_fTIME_LEFT ( 0.0f )
{
}

CWaitServerDialogue::~CWaitServerDialogue ()
{
}

void CWaitServerDialogue::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "WAITSERVER_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "WAITSERVER_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pTextBox->SetFont ( pFont9 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pTextBox->SetLineInterval ( 6.0f );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;

	m_pMoveOKButton = CreateTextButton ( "WAITSERVER_OK", WAITSERVER_OKBUTTON, (char*)ID2GAMEWORD ( "WAITSERVER_OKBUTTON" ) );
	m_pMoveOKButton->SetShortcutKey ( DIK_RETURN );
}

CBasicTextButton* CWaitServerDialogue::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void CWaitServerDialogue::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_fTIME_LEFT -= fElapsedTime;

	if ( m_fTIME_LEFT < 0.0f) m_fTIME_LEFT = 0.0f;

	CUIModal::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	CString strLeftTime;
	strLeftTime.Format ( "%1.0f%s", floor(m_fTIME_LEFT), ID2GAMEWORD("WAITSERVER_TIMELEFT_UNIT"));

	m_pTextBox->ClearText ();
	m_pTextBox->AddText ( m_strMessage );
	m_pTextBox->AddText ( strLeftTime );

	if ( m_fTIME_LEFT < 1.0f ) DoACTION_PLAY ();
}

void CWaitServerDialogue::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case WAITSERVER_OKBUTTON:
		{
			bool bBUTTONOK = false;
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_MOUSEIN_OKBUTTON );

				if ( UIMSG_LB_UP & dwMsg )
				{
					bBUTTONOK = true;
				}
			}

			if ( bBUTTONOK )
			{
				switch ( m_nACTION )
				{
				case WAITSERVER_CLOSEGAME:
					{
						//	NOTE
						//		프로그램을 종료한다.
						DxGlobalStage::GetInstance().CloseGame();

						CInnerInterface::GetInstance().WAITSERVER_DIALOGUE_CLOSE ();
					}
					break;

				case WAITSERVER_TOLOBY:
					{
						//	NOTE
						//		서버 선택으로 이동한다.
						DxGlobalStage::GetInstance().GameToLobbyStage ();

						CInnerInterface::GetInstance().WAITSERVER_DIALOGUE_CLOSE ();
					}
					break;

				case WAITSERVER_TOFRIEND:
					{
							CInnerInterface::GetInstance().WAITSERVER_DIALOGUE_CLOSE ();
							return ;
					}
					break;
				}
			}
		}
        break;
	}
}

void	CWaitServerDialogue::SetDisplayMessage ( const CString& strDisplay, const int nAction, const float fTimer )
{
	if ( m_pTextBox )
	{
		m_nACTION = nAction;
		m_fTIME_LEFT = fTimer;

		m_strMessage = strDisplay;

		if ( nAction == WAITSERVER_TOFRIEND )
			m_pMoveOKButton->SetOneLineText ( (char*)ID2GAMEWORD ( "WAITSERVER_CANCELBUTTON" ) );
		else
			m_pMoveOKButton->SetOneLineText ( (char*)ID2GAMEWORD ( "WAITSERVER_OKBUTTON" ) );			
	}
}

void	CWaitServerDialogue::DoACTION_PLAY ()
{
	switch(m_nACTION)
	{
	case WAITSERVER_CLOSEGAME:
		{
			DxGlobalStage::GetInstance().CloseGame();
			CInnerInterface::GetInstance().WAITSERVER_DIALOGUE_CLOSE ();
		}
		break;
	case WAITSERVER_TOLOBY:
		{
			DxGlobalStage::GetInstance().GameToLobbyStage ();
			CInnerInterface::GetInstance().WAITSERVER_DIALOGUE_CLOSE ();
		}
		break;
	case WAITSERVER_TOFRIEND:
		{
			CString strName = CInnerInterface::GetInstance().GetFriendName ();
			GLGaeaClient::GetInstance().GetCharacter ()->Req2Friend ( strName );
			CInnerInterface::GetInstance().WAITSERVER_DIALOGUE_CLOSE ();
		}
		break;
	default:
		break;
	}
}
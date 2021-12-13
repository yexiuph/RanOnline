#include "pch.h"

#include "./GambleSelectBox.h"
#include "./GambleAnimationBox.h"

#include "BasicLineBox.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundLib.h"

#include "d3dfont.h"
#include "DxGlobalStage.h"
#include "GameTextControl.h"
#include "GLCharacter.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"

const int CGambleSelectBox::nLIMIT_TIME = 60;

CGambleSelectBox::CGambleSelectBox(void)
: m_pInfoTextBox(NULL)
, m_TotElapsedTime(0.0f)
{
}

CGambleSelectBox::~CGambleSelectBox(void)
{
	m_TotElapsedTime = 0.0f;
}


void CGambleSelectBox::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	
	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "GAMBLE_SELECT_DIALOGUE_LINE_BOX" );
	RegisterControl ( pLineBox );	
	
	pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "GAMBLE_SELECT_DIALOGUE_TEXT_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "GAMBLE_SELECT_INFO_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont ( pFont9 );
	//pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pTextBox->SetPartInterval ( 5.0f );
	pTextBox->SetText ( ID2GAMEINTEXT("GAMBLE_SELECT_INFO_TEXT") );
	RegisterControl ( pTextBox );
	m_pInfoTextBox = pTextBox;
	
	CreateKeyButton( "GAMBLE_SELECT_ODD_BUTTON", "GAMBLE_SELECT_ODD_BUTTON_FLIP", GAMBLE_SELECT_ODD_BUTTON );
	CreateKeyButton( "GAMBLE_SELECT_EVEN_BUTTON", "GAMBLE_SELECT_EVEN_BUTTON_FLIP", GAMBLE_SELECT_EVEN_BUTTON );
	
	CBasicProgressBar* pBetTime = new CBasicProgressBar;
	pBetTime->CreateSub ( this, "GAMBLE_SELECT_TIME_PROGRESSBAR" );
	pBetTime->CreateOverImage ( "GAMBLE_SELECT_TIME_PROGRESSBAR_OVERIMAGE" );
	RegisterControl ( pBetTime );
	m_pBetTimeProgressBar = pBetTime;

}

CUIControl*	CGambleSelectBox::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

void CGambleSelectBox::CreateKeyButton( const char * szControlKey, const char * szFlipKey, UIGUID nID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub( this, szControlKey, UI_FLAG_DEFAULT, nID );
	pButton->CreateFlip( szFlipKey, CBasicButton::MOUSEIN_FLIP );
	RegisterControl( pButton );		
}



void CGambleSelectBox::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	

	switch ( ControlID )
	{
	// '홀' 버튼
	case GAMBLE_SELECT_ODD_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup( GetWndID () );
				GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenSelect( TRUE );
				CInnerInterface::GetInstance().ShowGroupFocus( GAMBLE_ANIMATION_DIALOGUE );
				DxSoundLib::GetInstance()->PlaySound ( "GAMBLING_SHUFFLE" );
			}
		}
		break;

	// '짝'버튼
	case GAMBLE_SELECT_EVEN_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup( GetWndID () );
				GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenSelect( FALSE );
				CInnerInterface::GetInstance().ShowGroupFocus( GAMBLE_ANIMATION_DIALOGUE );
				DxSoundLib::GetInstance()->PlaySound ( "GAMBLING_SHUFFLE" );
			}
		}
		break;

	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenCancel(); 		
			}
		}
		break;

	}
	
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
	
}

void CGambleSelectBox::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		m_TotElapsedTime = 0.0f;
	}
	else
	{	
		CInnerInterface::GetInstance().HideGroup( GetWndID () );
	}
}


void CGambleSelectBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	// 시간 경과
	m_TotElapsedTime += fElapsedTime;
	
	// 프로그레스바(시간 게이지 바)
	UpdateProgressBar( m_pBetTimeProgressBar,  m_TotElapsedTime );
}

BOOL CGambleSelectBox::UpdateProgressBar( CBasicProgressBar * pBar, float wNOW )
{
	if ( MINIGAME_ODDEVEN::fTimeLimit >= wNOW )
	{
		const float fPercent = float(wNOW) / float(MINIGAME_ODDEVEN::fTimeLimit);
		pBar->SetPercent ( fPercent );
		return TRUE;
	}
	else
	{
		GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenCancel();
	}

	return FALSE;
}


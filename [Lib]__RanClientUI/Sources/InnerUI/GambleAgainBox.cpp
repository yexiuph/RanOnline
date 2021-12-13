#include "pch.h"

#include "GambleAgainBox.h"

#include "BasicLineBox.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextButton.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
#include "../[Lib]__EngineUI/Sources/UIEditBoxMan.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"


#include "d3dfont.h"
#include "DxGlobalStage.h"
#include "GameTextControl.h"
#include "GLCharacter.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"

const int CGambleAgainBox::nLIMIT_CHAR = 20;
const int CGambleAgainBox::nLIMIT_TIME = 60;

CGambleAgainBox::CGambleAgainBox(void)
: m_pInfoTextBox(NULL)
, m_pBetMoneyTextBox(NULL)
, m_TotElapsedTime(0.0f)
{
}

CGambleAgainBox::~CGambleAgainBox(void)
{
}



void CGambleAgainBox::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	
	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "GAMBLE_AGAIN_DIALOGUE_LINE_BOX" );
	RegisterControl ( pLineBox );	
	
	pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "GAMBLE_AGAIN_DIALOGUE_TEXT_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "GAMBLE_AGAIN_INFO_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont ( pFont9 );
	//pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pTextBox->SetPartInterval ( 5.0f );
	pTextBox->SetText ( ID2GAMEINTEXT("GAMBLE_AGAIN_INFO_TEXT") );
	RegisterControl ( pTextBox );
	m_pInfoTextBox = pTextBox;
	
	pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "GAMBLE_AGAIN_GET_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont ( pFont9 );
	//pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pTextBox->SetText ( ID2GAMEWORD("GAMBLE_BETGET",1) );
	pTextBox->SetPartInterval ( 5.0f );
	RegisterControl ( pTextBox );
	m_pBetMoneyTextBox = pTextBox;


	
	
	CBasicLineBox* pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxEditBox( "GAMBLE_AGAIN_GET_EDIT_BACK" );
	RegisterControl( pBasicLineBox );
	
	CUIEditBox* pEditBox = new CUIEditBox;
	pEditBox->CreateSub ( this, "GAMBLE_AGAIN_GET_EDIT", UI_FLAG_XSIZE | UI_FLAG_RIGHT, GAMBLE_AGAIN_GET_EDIT );
	pEditBox->CreateCarrat ( "MODAL_CARRAT", TRUE, UINT_MAX );
	pEditBox->SetLimitInput ( nLIMIT_CHAR );
	pEditBox->SetFont ( pFont9 );	
	pEditBox->SetMinusSign(FALSE);
	pEditBox->SetAlign(EDIT_RIGHT_ALIGN);
	pEditBox->SET_MONEY_FORMAT ( 3 );
	pEditBox->DoUSE_NUMBER( true );
	RegisterControl ( pEditBox );
	m_pBetEditBox = pEditBox;


	

	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_XSIZE, GAMBLE_AGAIN_OK_BUTTON );
	pButton->CreateBaseButton ( "GAMBLE_AGAIN_OK_BUTTON", CBasicTextButton::SIZE16, CBasicButton::CLICK_FLIP, ID2GAMEWORD("GAMBLE_OKCANCEL",0) );
	RegisterControl ( pButton );
	
	pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_XSIZE, GAMBLE_AGAIN_CANCLE_BUTTON );
	pButton->CreateBaseButton ( "GAMBLE_AGAIN_CANCLE_BUTTON", CBasicTextButton::SIZE16, CBasicButton::CLICK_FLIP, ID2GAMEWORD("GAMBLE_OKCANCEL",1) );
	RegisterControl ( pButton );

	CBasicProgressBar* pBetTime = new CBasicProgressBar;
	pBetTime->CreateSub ( this, "GAMBLE_AGAIN_TIME_PROGRESSBAR" );
	pBetTime->CreateOverImage ( "GAMBLE_AGAIN_TIME_PROGRESSBAR_OVERIMAGE" );
	RegisterControl ( pBetTime );
	m_pBetTimeProgressBar = pBetTime;

	
}

CUIControl*	CGambleAgainBox::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}


void CGambleAgainBox::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	
	switch ( ControlID )
	{
	case GAMBLE_AGAIN_OK_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				
				GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenAgain();
				CInnerInterface::GetInstance().HideGroup( GetWndID () );
			}
		}
		break;
			
	case ET_CONTROL_BUTTON:
	case GAMBLE_AGAIN_CANCLE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				ClearEditBox();
				GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenCancel();
			}
		}
		break;
	
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );


	
}

void CGambleAgainBox::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{	
		m_TotElapsedTime = 0.0f;
	}
	else
	{	
		ClearEditBox();
		m_pBetEditBox->EndEdit();
		CInnerInterface::GetInstance().HideGroup( GetWndID () );
	}
}


void CGambleAgainBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	// 시간 경과
	m_TotElapsedTime += fElapsedTime;

	// 프로그레스바(시간 게이지 바)
	UpdateProgressBar( m_pBetTimeProgressBar,  m_TotElapsedTime );
}

BOOL CGambleAgainBox::UpdateProgressBar( CBasicProgressBar * pBar, float wNOW )
{
	if ( MINIGAME_ODDEVEN::fTimeLimit >= wNOW )
	{
		
		const float fPercent = float(wNOW) / float(MINIGAME_ODDEVEN::fTimeLimit);
		pBar->SetPercent ( fPercent );
		return TRUE;
	}
	else
	{
		ClearEditBox();
		GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenCancel();
	}

	return FALSE;
}


void CGambleAgainBox::ClearEditBox()
{
	m_pBetEditBox->EndEdit();
	m_pBetEditBox->BeginEdit();
	m_pBetEditBox->ClearEdit();
}

void CGambleAgainBox::SetMoney( WORD wRound, ULONGLONG GetMoney )
{
	ClearEditBox();

	ULONGLONG NextGetMoney = (ULONGLONG)(GetMoney * MINIGAME_ODDEVEN::fReturnRate[wRound]);

	CString strCombine;
	strCombine.Format ( "%I64u", NextGetMoney);
	m_pBetEditBox->SetEditString(strCombine);

	m_pBetEditBox->EndEdit();
}
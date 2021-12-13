#include "pch.h"

#include "GambleBox.h"

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

const int CGambleBox::nLIMIT_CHAR = 10;

CGambleBox::CGambleBox(void)
: m_pInfoTextBox(NULL)
, m_pBetMoneyTextBox(NULL)
, m_pGetMoneyTextBox(NULL)
, m_TotElapsedTime(0.0f)
{
}

CGambleBox::~CGambleBox(void)
{
	m_TotElapsedTime = 0.0f;
}



void CGambleBox::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	
	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "GAMBLE_DIALOGUE_LINE_BOX" );
	RegisterControl ( pLineBox );	
	
	pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "GAMBLE_DIALOGUE_TEXT_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "GAMBLE_INFO_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont ( pFont9 );
	pTextBox->SetPartInterval ( 5.0f );
	pTextBox->SetText ( ID2GAMEINTEXT("GAMBLE_START_INFO_TEXT") );
	RegisterControl ( pTextBox );
	m_pInfoTextBox = pTextBox;
	
	pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "GAMBLE_BET_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont ( pFont9 );
	pTextBox->SetPartInterval ( 5.0f );
	pTextBox->SetText ( ID2GAMEWORD("GAMBLE_BETGET",0) );
	RegisterControl ( pTextBox );
	m_pBetMoneyTextBox = pTextBox;

	pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "GAMBLE_GET_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont ( pFont9 );
	pTextBox->SetPartInterval ( 5.0f );
	pTextBox->SetText ( ID2GAMEWORD("GAMBLE_BETGET",1) );
	RegisterControl ( pTextBox );
	m_pGetMoneyTextBox = pTextBox;	
	
	CBasicLineBox* pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxEditBox( "GAMBLE_BET_EDIT_BACK" );
	RegisterControl( pBasicLineBox );
	
	CUIEditBox* pEditBox = new CUIEditBox;
	pEditBox->CreateSub ( this, "GAMBLE_BET_EDIT", UI_FLAG_XSIZE | UI_FLAG_RIGHT, GAMBLE_BET_EDIT );
	pEditBox->CreateCarrat ( "MODAL_CARRAT", TRUE, UINT_MAX );
	pEditBox->SetLimitInput ( nLIMIT_CHAR );
	pEditBox->SetFont ( pFont9 );
	pEditBox->SetMinusSign(FALSE);
	pEditBox->SetAlign(EDIT_RIGHT_ALIGN);
	pEditBox->SET_MONEY_FORMAT ( 3 );
	pEditBox->DoUSE_NUMBER( true );
	RegisterControl ( pEditBox );
	m_pBetEditBox = pEditBox;

	pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxEditBox( "GAMBLE_GET_EDIT_BACK" );
	RegisterControl( pBasicLineBox );

	pEditBox = new CUIEditBox;
	pEditBox->CreateSub ( this, "GAMBLE_GET_EDIT", UI_FLAG_XSIZE | UI_FLAG_RIGHT, GAMBLE_GET_EDIT );
	pEditBox->CreateCarrat ( "MODAL_CARRAT", TRUE, UINT_MAX );
	pEditBox->SetLimitInput ( nLIMIT_CHAR );
	pEditBox->SetFont ( pFont9 );
	pEditBox->SetMinusSign(FALSE);
	pEditBox->SetAlign(EDIT_RIGHT_ALIGN);
	pEditBox->SET_MONEY_FORMAT ( 3 );
	pEditBox->DoUSE_NUMBER( true );
	RegisterControl ( pEditBox );
	m_pGetEditBox = pEditBox;
	
	

	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_XSIZE, GAMBLE_OK_BUTTON );
	pButton->CreateBaseButton ( "GAMBLE_OK_BUTTON", CBasicTextButton::SIZE16, CBasicButton::CLICK_FLIP, ID2GAMEWORD("GAMBLE_OKCANCEL",0) );
	RegisterControl ( pButton );
	
	pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_XSIZE, GAMBLE_CANCLE_BUTTON );
	pButton->CreateBaseButton ( "GAMBLE_CANCLE_BUTTON", CBasicTextButton::SIZE16, CBasicButton::CLICK_FLIP, ID2GAMEWORD("GAMBLE_OKCANCEL",1) );
	RegisterControl ( pButton );

	pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_XSIZE, GAMBLE_REINPUT_BUTTON );
	pButton->CreateBaseButton ( "GAMBLE_REINPUT_BUTTON", CBasicTextButton::SIZE16, CBasicButton::CLICK_FLIP, ID2GAMEWORD("GAMBLE_OKCANCEL",2) );
	RegisterControl ( pButton );

	CBasicProgressBar* pBetTime = new CBasicProgressBar;
	pBetTime->CreateSub ( this, "GAMBLE_TIME_PROGRESSBAR" );
	pBetTime->CreateOverImage ( "GAMBLE_TIME_PROGRESSBAR_OVERIMAGE" );
	RegisterControl ( pBetTime );
	m_pBetTimeProgressBar = pBetTime;

	ClearEditBox();
}

CUIControl*	CGambleBox::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

void CGambleBox::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	// 확인 버튼
	case GAMBLE_OK_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if(GetBetMoney() > 0)
				{
					GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenBatting( UINT( GetBetMoney() ) );
					CInnerInterface::GetInstance().HideGroup( GetWndID () );
				}
			}
		}
		break;
	// 취소 버튼
	case ET_CONTROL_BUTTON:
	case GAMBLE_CANCLE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				ClearEditBox();
				GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenCancel();
			}
		}
		break;
	// 재입력 버튼
	case GAMBLE_REINPUT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				ClearEditBox();
			}
		}
		break;
	case GAMBLE_BET_EDIT: TranslateEditBoxMsg( m_pBetEditBox, dwMsg ); break;
	

	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CGambleBox::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		m_TotElapsedTime = 0.0f;
		m_pBetEditBox->BeginEdit();
	}
	else
	{	
		ClearEditBox();
		m_pBetEditBox->EndEdit();
		m_pGetEditBox->EndEdit();
		
		CInnerInterface::GetInstance().HideGroup( GetWndID () );
	}
}


void CGambleBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	// 시간 경과
	m_TotElapsedTime += fElapsedTime;

	// 배팅금액이 0을 넘으면 배당금액을 계산하여 에디트 박스에 입력해 준다( 음수 값은 들어갈 수 없다) 
	if(GetBetMoney() > 0)
	{
		if(GetBetMoney() >= MINIGAME_ODDEVEN::uiMaxBattingMoney)
		{
			CString strCombine;
			strCombine.Format ( "%I64u", ULONGLONG(MINIGAME_ODDEVEN::uiMaxBattingMoney));
			m_pBetEditBox->SetEditString(strCombine);
		}
		
			ULONGLONG GetMoney = (ULONGLONG)(GetBetMoney() * MINIGAME_ODDEVEN::fReturnRate[0]);

			CString strCombine;
			strCombine.Format ( "%I64u", GetMoney);
			m_pGetEditBox->SetEditString(strCombine);
	}
	else
		m_pGetEditBox->ClearEdit();
	
	// 프로그레스바(시간 게이지 바)
	UpdateProgressBar( m_pBetTimeProgressBar,  m_TotElapsedTime );
}


BOOL CGambleBox::UpdateProgressBar( CBasicProgressBar * pBar, float wNOW )
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

void CGambleBox::TranslateEditBoxMsg( CUIEditBox * pEditBox, DWORD dwMsg )
{
	if( !pEditBox ) return;

	if ( UIKeyCheck::GetInstance()->Check ( DIK_ESCAPE, DXKEY_DOWN ) )
	{
		pEditBox->EndEdit();
		GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenCancel();
	}

	if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
	{
		pEditBox->EndEdit();
		pEditBox->BeginEdit();
	}
	else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
	{
		pEditBox->EndEdit();
	}
}

void CGambleBox::ClearEditBox()
{
	m_pBetEditBox->ClearEdit();
	m_pBetEditBox->EndEdit();
	m_pBetEditBox->BeginEdit();
	
	m_pGetEditBox->EndEdit();
	m_pGetEditBox->BeginEdit();
	m_pGetEditBox->ClearEdit();
	
	
}


const ULONGLONG CGambleBox::GetBetMoney()
{	
	const CString& strTemp = m_pBetEditBox->GetEditString();
	return static_cast<ULONGLONG>( atof ( strTemp ) );
}


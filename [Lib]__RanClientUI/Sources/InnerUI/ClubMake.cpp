#include "pch.h"
#include "ClubMake.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextButton.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "BasicLineBox.h"
#include "InnerInterface.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"
#include "../[Lib]__EngineUI/Sources/UIEditBoxMan.h"
#include "GLGaeaClient.h"
#include "DXInputString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CClubMake::nLIMIT_CHAR = 10;
const char CClubMake::BLANK_SYMBOL = ' ';

CClubMake::CClubMake()
{
}

CClubMake::~CClubMake()
{
}

void CClubMake::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "PRIVATE_MARKET_MAKE_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "PRIVATE_MARKET_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pTextBox->SetFont ( pFont9 );
//	pTextBox->SetTextAlign ( TEXT_CENTER_ALIGN );
	pTextBox->AddText ( ID2GAMEINTEXT("CLUB_MAKE_OPEN_MESSAGE") );
//	pTextBox->SetPartInterval ( 5.0f );
	RegisterControl ( pTextBox );
//	m_pTextBox = pTextBox;

	CBasicLineBox* pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxEditBox ( "PRIVATE_MARKET_MAKE_EDIT_BACK" );
	RegisterControl ( pBasicLineBox );

	CUIEditBox* pEditBox = new CUIEditBox;
	pEditBox->CreateSub ( this, "PRIVATE_MARKET_MAKE_EDIT", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, CLUB_MAKE_EDIT );
	pEditBox->CreateCarrat ( "PRIVATE_MARKET_MAKE_CARRAT", TRUE, UINT_MAX );
	pEditBox->SetLimitInput ( nLIMIT_CHAR );
	pEditBox->SetFont ( pFont9 );		
	RegisterControl ( pEditBox );
	m_pEditBox = pEditBox;

	CBasicTextButton* pButton = NULL;
	pButton = CreateTextButton ( "PRIVATE_MARKET_MAKE_OK", CLUB_MAKE_OK, (char*)ID2GAMEWORD ( "PRIVATE_MARKET_MAKE_BUTTON", 0 ) );
	pButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	pButton = CreateTextButton ( "PRIVATE_MARKET_MAKE_CANCEL", CLUB_MAKE_CANCEL, (char*)ID2GAMEWORD ( "PRIVATE_MARKET_MAKE_BUTTON", 1 ) );
	pButton->SetShortcutKey ( DIK_ESCAPE );
}

CBasicTextButton* CClubMake::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );

	return pButton;
}

void CClubMake::Init ( const DWORD& dwNpcID )
{
	m_pEditBox->EndEdit ();
	m_pEditBox->ClearEdit ();
	m_pEditBox->BeginEdit ();

	m_dwNpcID = dwNpcID;
}

void	CClubMake::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case CLUB_MAKE_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( CHECK_KEYFOCUSED(dwMsg) &&
					!DXInputString::GetInstance().CheckEnterKeyDown() )	return;

				CString strTrim = m_pEditBox->GetEditString ();
				strTrim.Trim ( BLANK_SYMBOL );
				if ( !m_pEditBox->GetEditLength () || !strTrim.GetLength () ) return ;

				m_pEditBox->EndEdit ();				

				CInnerInterface::GetInstance().HideGroup ( GetWndID (), true );
//				CInnerInterface::GetInstance().SetPrivateMarketOpen ( true, NATIVEID_NULL().dwID );	

				//	타이틀 설정
				CString strTITLE = m_pEditBox->GetEditString ();
				GLGaeaClient::GetInstance().GetCharacter()->ReqClubNew ( m_dwNpcID, strTITLE.GetString () );
			}
		}
		break;

	case CLUB_MAKE_CANCEL:
	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pEditBox->EndEdit ();

				CInnerInterface::GetInstance().HideGroup( GetWndID (), true );
			}
		}
		break;
	}
}

void	CClubMake::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindow::SetVisibleSingle ( bVisible );

	if ( !bVisible )
	{
		m_pEditBox->EndEdit ();
	}
}

const CString& CClubMake::GetTITLE ()
{
	static CString strTemp;

	CString strTrim = m_pEditBox->GetEditString ();
	strTrim.Trim ( BLANK_SYMBOL );
	if ( !m_pEditBox->GetEditLength () || !strTrim.GetLength () ) return strTemp;

	return m_pEditBox->GetEditString ();
}
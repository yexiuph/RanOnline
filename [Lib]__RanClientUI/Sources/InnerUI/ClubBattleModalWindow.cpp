#include "pch.h"
#include "ClubBattleModalWindow.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "BasicLineBox.h"
#include "InnerInterface.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLogicData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CClubBattleModal::CClubBattleModal()
: m_dwTime (0)
, m_bAlliance( false )
, m_pEditBoxHour(NULL)
, m_pEditBoxMin(NULL)
, m_pClubName(NULL)
, m_pInfoText(NULL)
{
}

CClubBattleModal::~CClubBattleModal()
{
}

CBasicTextBox * CClubBattleModal::CreateStaticControl( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl * CClubBattleModal::CreateControl( const char * szControl )
{
	GASSERT( szControl );

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub( this, szControl );
	pControl->SetVisibleSingle( FALSE );
	RegisterControl( pControl );

	return pControl;
}

void CClubBattleModal::CreateSubControl()
{
	CD3DFontPar * pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9 );

	// 클럽 이름

	{	//	외부 라인 박스
		CBasicLineBox* pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MODAL", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxModal ( "CLUB_BATTLE_MODAL_LINEBOX" );		
		RegisterControl ( pLineBox );
	}

	CBasicTextBox* pControl = CreateStaticControl( "CLUB_BATTLE_MODAL_CLUB", pFont, TEXT_ALIGN_LEFT );
	pControl->AddText( ID2GAMEWORD( "CLUB_BATTLE_MODAL", 0 ) );

	m_pClubName = CreateStaticControl( "CLUB_BATTLE_MODAL_CLUBNAME", pFont, TEXT_ALIGN_LEFT );
	m_pInfoText = CreateStaticControl( "CLUB_BATTLE_MODAL_INFO", pFont, TEXT_ALIGN_LEFT );
	m_pInfoText->AddText( ID2GAMEINTEXT( "CLUB_BATTLE_MODAL_INFO") );


	{	// Note : 시간,분 설정
		pControl = CreateStaticControl( "CLUB_BATTLE_MODAL_TIME", pFont, TEXT_ALIGN_LEFT );
		pControl->AddText( ID2GAMEWORD( "CLUB_BATTLE_MODAL", 1 ) );
		
		pControl = CreateStaticControl( "CLUB_BATTLE_MODAL_HOUR", pFont, TEXT_ALIGN_LEFT );
		pControl->AddText( ID2GAMEWORD( "CLUB_BATTLE_MODAL", 2 ) );
		
		pControl = CreateStaticControl( "CLUB_BATTLE_MODAL_MIN", pFont, TEXT_ALIGN_LEFT );
		pControl->AddText( ID2GAMEWORD( "CLUB_BATTLE_MODAL", 3 ) );

		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox ( "CLUB_BATTLE_MODAL_EDIT_HOUR_BACK" );
		RegisterControl ( pBasicLineBox );

		m_pEditBoxHour = new CUIEditBox;
		m_pEditBoxHour->CreateSub ( this, "CLUB_BATTLE_MODAL_EDIT_HOUR", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, CLUB_BATTLE_MODAL_EDIT_HOUR );
		m_pEditBoxHour->CreateCarrat ( "CLUB_BATTLE_MODAL_EDIT_CARRAT", TRUE, UINT_MAX );
		m_pEditBoxHour->DoUSE_NUMBER( true );
		m_pEditBoxHour->SetFont ( pFont );		
		RegisterControl ( m_pEditBoxHour );

		pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox ( "CLUB_BATTLE_MODAL_EDIT_MIN_BACK" );
		RegisterControl ( pBasicLineBox );

		m_pEditBoxMin = new CUIEditBox;
		m_pEditBoxMin->CreateSub ( this, "CLUB_BATTLE_MODAL_EDIT_MIN", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, CLUB_BATTLE_MODAL_EDIT_MIN );
		m_pEditBoxMin->CreateCarrat ( "CLUB_BATTLE_MODAL_EDIT_CARRAT", TRUE, UINT_MAX );
		m_pEditBoxMin->DoUSE_NUMBER( true );
		m_pEditBoxMin->SetFont ( pFont );		
		RegisterControl ( m_pEditBoxMin );
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, CLUB_BATTLE_OK );
		pButton->CreateBaseButton( "CLUB_BATTLE_MODAL_OK", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, CLUB_BATTLE_CANCEL );
		pButton->CreateBaseButton( "CLUB_BATTLE_MODAL_CANCEL", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}
}

void CClubBattleModal::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case CLUB_BATTLE_MODAL_EDIT_HOUR:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pEditBoxMin->EndEdit();
				m_pEditBoxHour->ClearEdit();
				m_pEditBoxHour->BeginEdit();
			}
		}
		break;
	case CLUB_BATTLE_MODAL_EDIT_MIN:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pEditBoxHour->EndEdit();
				m_pEditBoxMin->ClearEdit();
				m_pEditBoxMin->BeginEdit();
			}
		}
		break;
	case CLUB_BATTLE_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				// 신청
				CString strTempHour = m_pEditBoxHour->GetEditString();
				CString strTempMin = m_pEditBoxMin->GetEditString();
				m_dwTime = (DWORD) ( atoi( strTempHour.GetString() ) * 60 ) ;
				m_dwTime += (DWORD) atoi (strTempMin.GetString());

				CInnerInterface::GetInstance().ReqClubBattle( m_dwTime, m_bAlliance );				
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;
	case CLUB_BATTLE_CANCEL:
	case ET_CONTROL_BUTTON:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;
	}
		
	
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CClubBattleModal::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	Init();

	if ( bVisible )
	{
		m_pClubName->SetText( m_strClubName );
		m_dwTime = GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN;

		CString strTemp;
		
		strTemp.Format( "%d", m_dwTime/60 );
		m_pEditBoxHour->SetEditString( strTemp );

		strTemp.Format( "%d", m_dwTime%60 );
		m_pEditBoxMin->SetEditString( strTemp );

		m_pInfoText->ClearText();

		if ( m_bAlliance )	m_pInfoText->AddText( ID2GAMEINTEXT( "ALLIANCE_BATTLE_MODAL_INFO") );
		else m_pInfoText->AddText( ID2GAMEINTEXT( "CLUB_BATTLE_MODAL_INFO") );
	}
}


void CClubBattleModal::Init ()
{
	m_pEditBoxHour->EndEdit ();
	m_pEditBoxMin->EndEdit ();
	m_pEditBoxHour->ClearEdit ();
	m_pEditBoxMin->ClearEdit ();
}

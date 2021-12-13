#include "pch.h"
#include "ChatShowFlag.h"
#include "BasicLineBox.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "GameTextControl.h"
#include "UITextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIChatShowFlag::CUIChatShowFlag ()
	: m_pNormalButton(NULL)
	, m_pPrivateButton(NULL)
	, m_pPartyButton(NULL)
	, m_pClubButton(NULL)
	, m_pSystemButton(NULL)
	, m_pAllianceButton(NULL)
	, m_pNormalTextBox(NULL)
	, m_pPrivateTextBox(NULL)
	, m_pPartyTextBox(NULL)
	, m_pClubTextBox(NULL)
	, m_pSystemTextBox(NULL)
	, m_pAllianceTextBox(NULL)
{
}

CUIChatShowFlag::~CUIChatShowFlag ()
{
}

void	CUIChatShowFlag::CreateSubControl ()
{
	{	//	뒷 배경
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "CHAT_SHOW_FLAG_BACK" );
//		pBasicLineBox->ResetAlignFlagLine ();
		pBasicLineBox->ResetAlignFlag ();
		RegisterControl ( pBasicLineBox );		
	}

	//	일반
	m_pNormalButton = CreateButton ( "CHAT_NORMAL_CHECK", "CHAT_NORMAL_CHECK_F", CHAT_NORMAL_CHECK );
	m_pNormalButton->SetFlip ( TRUE );
	m_pNormalTextBox = CreateTextBox ( "CHAT_NORMAL_CHECK_TEXT", ID2GAMEWORD("CHAT_ONOFF",0), CHAT_NORMAL_TEXTBOX );

	//	귓속말
	m_pPrivateButton = CreateButton ( "CHAT_PRIVATE_CHECK", "CHAT_PRIVATE_CHECK_F", CHAT_PRIVATE_CHECK );
	m_pPrivateButton->SetFlip ( TRUE );
	m_pPrivateTextBox = CreateTextBox ( "CHAT_PRIVATE_CHECK_TEXT", ID2GAMEWORD("CHAT_ONOFF",1), CHAT_PRIVATE_TEXTBOX );

	//	파티
	m_pPartyButton = CreateButton ( "CHAT_PARTY_CHECK", "CHAT_PARTY_CHECK_F", CHAT_PARTY_CHECK );
	m_pPartyButton->SetFlip ( TRUE );
	m_pPartyTextBox = CreateTextBox ( "CHAT_PARTY_CHECK_TEXT", ID2GAMEWORD("CHAT_ONOFF",2), CHAT_PARTY_TEXTBOX );

	//	클럽
	m_pClubButton = CreateButton ( "CHAT_CLUB_CHECK", "CHAT_CLUB_CHECK_F", CHAT_CLUB_CHECK );
	m_pClubButton->SetFlip ( TRUE );
	m_pClubTextBox = CreateTextBox ( "CHAT_CLUB_CHECK_TEXT", ID2GAMEWORD("CHAT_ONOFF",3), CHAT_CLUB_TEXTBOX );

	// 동맹
	m_pAllianceButton = CreateButton ( "CHAT_ALLIANCE_CHECK", "CHAT_ALLIANCE_CHECK_F", CHAT_ALLIANCE_CHECK );
	m_pAllianceButton->SetFlip ( TRUE );
	m_pAllianceTextBox = CreateTextBox ( "CHAT_ALLIANCE_CHECK_TEXT", ID2GAMEWORD("CHAT_ONOFF",5), CHAT_ALLIANCE_TEXTBOX );

	//	시스템
	m_pSystemButton = CreateButton ( "CHAT_SYSTEM_CHECK", "CHAT_SYSTEM_CHECK_F", CHAT_SYSTEM_CHECK );
	m_pSystemButton->SetFlip ( TRUE );
	m_pSystemTextBox = CreateTextBox ( "CHAT_SYSTEM_CHECK_TEXT", ID2GAMEWORD("CHAT_ONOFF",4), CHAT_SYSTEM_TEXTBOX );
}

CBasicButton*	CUIChatShowFlag::CreateButton ( const CString& strKeyword, const CString& strFlipKeyword, const UIGUID& cID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, const_cast<CString&>(strKeyword).GetString(), UI_FLAG_DEFAULT, cID );
	pButton->CreateFlip ( const_cast<CString&>(strFlipKeyword).GetString(), CBasicButton::RADIO_FLIP );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox*	CUIChatShowFlag::CreateTextBox ( const CString& strKeyword, const CString& strText, const UIGUID& cID )
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox*	pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, const_cast<CString&>(strKeyword).GetString(), UI_FLAG_DEFAULT, cID );
	pTextBox->SetFont ( pFont );
	pTextBox->AddText ( strText );
	RegisterControl ( pTextBox );
	return pTextBox;
}

void CUIChatShowFlag::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	//	원래는 필요없는 코드이나,
	//	CBasicButton에 버그로 인해,
	//	고치기전까지의 사용으로 임시적용
	{
		m_pNormalButton->SetVisibleSingle ( TRUE );
		m_pPrivateButton->SetVisibleSingle ( TRUE );
		m_pPartyButton->SetVisibleSingle ( TRUE );
		m_pClubButton->SetVisibleSingle ( TRUE );
		m_pAllianceButton->SetVisibleSingle( TRUE );
		m_pSystemButton->SetVisibleSingle ( TRUE );
	}

	if ( m_pNormalButton->IsFlip() )	AddMessageEx ( UIMSG_CHAT_NORMAL );
	if ( m_pPrivateButton->IsFlip() )	AddMessageEx ( UIMSG_CHAT_PRIVATE );
	if ( m_pPartyButton->IsFlip() )		AddMessageEx ( UIMSG_CHAT_PARTY );
	if ( m_pClubButton->IsFlip() )		AddMessageEx ( UIMSG_CHAT_CLUB );
	if ( m_pAllianceButton->IsFlip() )	AddMessageEx ( UIMSG_CHAT_ALLIANCE );
	if ( m_pSystemButton->IsFlip() )	AddMessageEx ( UIMSG_CHAT_SYSTEM );
}

void CUIChatShowFlag::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case CHAT_NORMAL_TEXTBOX:
	case CHAT_NORMAL_CHECK:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pNormalButton->IsFlip();
					m_pNormalButton->SetFlip ( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pNormalTextBox->SetUseTextColor ( 0, TRUE );
					m_pNormalTextBox->SetTextColor ( 0, dwColor );
				}
			}
		}
		break;

	case CHAT_PRIVATE_TEXTBOX:
	case CHAT_PRIVATE_CHECK:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pPrivateButton->IsFlip();
					m_pPrivateButton->SetFlip ( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;	

                    m_pPrivateTextBox->SetUseTextColor ( 0, TRUE );
					m_pPrivateTextBox->SetTextColor ( 0, dwColor );
				}
			}
		}
		break;
		
	case CHAT_PARTY_TEXTBOX:
	case CHAT_PARTY_CHECK:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pPartyButton->IsFlip();
					m_pPartyButton->SetFlip ( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pPartyTextBox->SetUseTextColor ( 0, TRUE );
					m_pPartyTextBox->SetTextColor ( 0, dwColor );
				}
			}
		}
		break;

	case CHAT_CLUB_TEXTBOX:
	case CHAT_CLUB_CHECK:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pClubButton->IsFlip();
					m_pClubButton->SetFlip ( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pClubTextBox->SetUseTextColor ( 0, TRUE );
					m_pClubTextBox->SetTextColor ( 0, dwColor );
				}
			}
		}
		break;

	case CHAT_ALLIANCE_CHECK:
	case CHAT_ALLIANCE_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pAllianceButton->IsFlip();
					m_pAllianceButton->SetFlip ( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pAllianceTextBox->SetUseTextColor ( 0, TRUE );
					m_pAllianceTextBox->SetTextColor ( 0, dwColor );
				}
			}
		}
		break;

	case CHAT_SYSTEM_CHECK:
	case CHAT_SYSTEM_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pSystemButton->IsFlip();
					m_pSystemButton->SetFlip ( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pSystemTextBox->SetUseTextColor ( 0, TRUE );
					m_pSystemTextBox->SetTextColor ( 0, dwColor );
				}
			}
		}
		break;
	}
}
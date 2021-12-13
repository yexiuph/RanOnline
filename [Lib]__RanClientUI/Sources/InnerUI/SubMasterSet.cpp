#include "pch.h"
#include "SubMasterSet.h"

#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicLineBox.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "InnerInterface.h"
#include "GLCharacter.h"
#include "GLCharDefine.h"
#include "GLGaeaClient.h"
#include "EtcFunction.h"

#include "DxParamSet.h"
#include "RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSubMasterSet::CSubMasterSet ()
	: m_pJoinButton(NULL)
	, m_pStrikeButton(NULL)
	, m_pNoticeButton(NULL)
	, m_pMarkButton(NULL)
	, m_pCertifyButton(NULL)
	, m_pTextBox(NULL)
	, m_dwCharID(CLUB_NULL)
	, m_pCLUB(NULL)
	, m_dwClubFlag(0)
{
}

CSubMasterSet::~CSubMasterSet ()
{
}

void CSubMasterSet::CreateSubControl ()
{
	CD3DFontPar* m_pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	// 뒷 배경
	{
		CBasicLineBox* pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessage ( "SUBMASTER_LINE_BOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );

		pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessageUp ( "SUBMASTER_LINETEXTBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );
	}

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub( this, "SUBMASTER_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont( m_pFont );
	RegisterControl( pTextBox );
	m_pTextBox = pTextBox;

	{
		CBasicTextBox* pTextBox = NULL;

		pTextBox = CreateStaticControl( "SUBMASTER_JOIN_STATIC", m_pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 1 ) );

		pTextBox = CreateStaticControl( "SUBMASTER_STRIKE_STATIC", m_pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 2 ) );

		pTextBox = CreateStaticControl( "SUBMASTER_NOTICE_STATIC", m_pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 3 ) );

		pTextBox = CreateStaticControl ( "SUBMASTER_MARK_STATIC", m_pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 4 ) );

		pTextBox = CreateStaticControl ( "SUBMASTER_CERTIFY_STATIC", m_pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 5 ) );

		pTextBox = CreateStaticControl ( "SUBMASTER_CDM_STATIC", m_pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 6 ) );
	}

	{
		m_pJoinButton = CreateFlipButton( "SUBMASTER_JOIN_BUTTON", "SUBMASTER_JOIN_BUTTON_F", SUBMASTER_JOIN_BUTTON );
		m_pStrikeButton = CreateFlipButton( "SUBMASTER_STRIKE_BUTTON", "SUBMASTER_STRIKE_BUTTON_F", SUBMASTER_STRIKE_BUTTON );
		m_pNoticeButton = CreateFlipButton( "SUBMASTER_NOTICE_BUTTON", "SUBMASTER_NOTICE_BUTTON_F", SUBMASTER_NOTICE_BUTTON );
		m_pMarkButton = CreateFlipButton( "SUBMASTER_MARK_BUTTON", "SUBMASTER_MARK_BUTTON_F", SUBMASTER_MARK_BUTTON );
		m_pCertifyButton = CreateFlipButton( "SUBMASTER_CERTIFY_BUTTON", "SUBMASTER_CERTIFY_BUTTON_F", SUBMASTER_CERTIFY_BUTTON );
		m_pCDMButton = CreateFlipButton( "SUBMASTER_CDM_BUTTON", "SUBMASTER_CDM_BUTTON_F", SUBMASTER_CDM_BUTTON );
	}

	{
		// 확인, 취소
		CBasicTextButton* pButton = NULL;
		pButton = CreateTextButton( "SUBMASTER_OK", SUBMASTER_OK, (char*)ID2GAMEWORD( "PRIVATE_MARKET_MAKE_BUTTON", 0 ) ); // 개인상점의 확인,취소를 그대로 사용
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		pButton = CreateTextButton( "SUBMASTER_CANCEL", SUBMASTER_CANCEL, (char*)ID2GAMEWORD( "PRIVATE_MARKET_MAKE_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
	}
}

CBasicButton* CSubMasterSet::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox* CSubMasterSet::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );
	return pStaticText;
}

CBasicTextButton* CSubMasterSet::CreateTextButton( char* szButton, UIGUID ControlID , char* szText )
{
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton( szButton, CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, szText );
	RegisterControl( pButton );
	return pButton;
}

void CSubMasterSet::LoadJoin()
{
	
	bool bJoin = m_pCLUB->IsMemberFlgJoin( m_dwCharID );
	m_pJoinButton->SetFlip( bJoin );
	SetCheck_Flags( bJoin, m_dwClubFlag, EMCLUB_SUBMATER_JOIN );
}

void CSubMasterSet::LoadStrike()
{
	bool bStrike = m_pCLUB->IsMemberFlgKick( m_dwCharID );
	m_pStrikeButton->SetFlip( bStrike );
	SetCheck_Flags( bStrike, m_dwClubFlag, EMCLUB_SUBMATER_KICK );
}

void CSubMasterSet::LoadNotice()
{
	bool bNotice = m_pCLUB->IsMemberFlgNotice( m_dwCharID );
	m_pNoticeButton->SetFlip( bNotice );
	SetCheck_Flags( bNotice, m_dwClubFlag, EMCLUB_SUBMATER_NOTICE );
}

void CSubMasterSet::LoadMark()
{
	bool bMark = m_pCLUB->IsMemberFlgMarkChange( m_dwCharID );
	m_pMarkButton->SetFlip( bMark );
	SetCheck_Flags( bMark, m_dwClubFlag, EMCLUB_SUBMATER_MARK );
}

void CSubMasterSet::LoadCertify()
{
	bool bCertify = m_pCLUB->IsMemberFlgCDCertify( m_dwCharID );
	m_pCertifyButton->SetFlip( bCertify );
	SetCheck_Flags( bCertify, m_dwClubFlag, EMCLUB_SUBMATER_CD );
}

void CSubMasterSet::LoadClubDeathMatch()
{
	bool bCDM = m_pCLUB->IsMemberFlgCDM( m_dwCharID );
	m_pCDMButton->SetFlip( bCDM );
	SetCheck_Flags( bCDM, m_dwClubFlag, EMCLUB_SUBMATER_CDM );
}

void CSubMasterSet::LoadCurrentOption( DWORD dwCharID ) // 보이기 전에 반드시 호출되어야 한다.
{
	m_pCLUB = &(GLGaeaClient::GetInstance().GetCharacter()->m_sCLUB);
	m_dwCharID = dwCharID;

	LoadJoin();
	LoadStrike();
	LoadNotice();
	LoadMark();
	LoadCertify();
	LoadClubDeathMatch();
}

void CSubMasterSet::SetSubMasterName( const CString& strName )
{
	CString strMessage;
	strMessage.Format( "%s%s", strName, ID2GAMEWORD( "SUBMASTER_OPTION", 0 ) );

	m_pTextBox->SetText( strMessage );
}

void CSubMasterSet::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SUBMASTER_JOIN_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bJoin = GetCheck_Flags( m_dwClubFlag, EMCLUB_SUBMATER_JOIN );
					SetCheck_Flags( !bJoin, m_dwClubFlag, EMCLUB_SUBMATER_JOIN );
					m_pJoinButton->SetFlip( !bJoin );
				}
			}
		}
		break;

	case SUBMASTER_STRIKE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bStrike = GetCheck_Flags( m_dwClubFlag, EMCLUB_SUBMATER_KICK );
					SetCheck_Flags( !bStrike, m_dwClubFlag, EMCLUB_SUBMATER_KICK );
					m_pStrikeButton->SetFlip( !bStrike );
				}
			}
		}
		break;

	case SUBMASTER_NOTICE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bNotice = GetCheck_Flags( m_dwClubFlag, EMCLUB_SUBMATER_NOTICE );
					SetCheck_Flags( !bNotice, m_dwClubFlag, EMCLUB_SUBMATER_NOTICE );
					m_pNoticeButton->SetFlip( !bNotice );
				}
			}
		}
		break;

	case SUBMASTER_MARK_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bMark = GetCheck_Flags( m_dwClubFlag, EMCLUB_SUBMATER_MARK );
					SetCheck_Flags( !bMark, m_dwClubFlag, EMCLUB_SUBMATER_MARK );
					m_pMarkButton->SetFlip( !bMark );
				}
			}
		}
		break;

	case SUBMASTER_CERTIFY_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{	
					bool bCertify = GetCheck_Flags( m_dwClubFlag, EMCLUB_SUBMATER_CD );
					SetCheck_Flags( !bCertify, m_dwClubFlag, EMCLUB_SUBMATER_CD );
					m_pCertifyButton->SetFlip( !bCertify );
				}
			}
		}
		break;

	case SUBMASTER_CDM_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bCDM = GetCheck_Flags( m_dwClubFlag, EMCLUB_SUBMATER_CDM );
					SetCheck_Flags( !bCDM, m_dwClubFlag, EMCLUB_SUBMATER_CDM );
					m_pCDMButton->SetFlip( !bCDM );
				}
			}
		}
		break;

	case SUBMASTER_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//	Note : 클럽 부마 설정.
				GLCharacter *pCharacter = GLGaeaClient::GetInstance().GetCharacter();
				pCharacter->ReqClubSubMaster( m_dwCharID, m_dwClubFlag );

				CInnerInterface::GetInstance().HideGroup( GetWndID (), true );
			}
		}
		break;

	case SUBMASTER_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup( GetWndID(), true );				
			}
		}
		break;
	}
}
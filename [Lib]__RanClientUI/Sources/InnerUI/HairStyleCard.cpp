#include "pch.h"
#include "HairStyleCard.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "InnerInterface.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char * szHAIRSTYLECARD_NAME_M = "HAIRSTYLECARD_NAME_M";
char * szHAIRSTYLECARD_NAME_FM = "HAIRSTYLECARD_NAME_FM";

CHairStyleCard::CHairStyleCard ()
	: m_pStyleName(NULL)
	, m_pHairStyle(NULL)
	, m_pId2GameText(NULL)
	, m_nHair(0)
	, m_nHairBack(0)
	, m_nMaxHair(0)
	, m_bReverse(FALSE)
	, m_bOKButton(FALSE)
{
}

CHairStyleCard::~CHairStyleCard ()
{
}

CBasicTextBox* CHairStyleCard::CreateStaticControl( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl* CHairStyleCard::CreateControl( const char * szControl )
{
	GASSERT( szControl );

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub( this, szControl );
	pControl->SetVisibleSingle( FALSE );
	RegisterControl( pControl );

	return pControl;
}

void CHairStyleCard::CreateSubControl ()
{
	CD3DFontPar * pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9 );

	CUIWindowEx::CreateControl( "HAIRSTYLECARD_TITLE_BACK_L" );
	CUIWindowEx::CreateControl( "HAIRSTYLECARD_TITLE_BACK_R" );
	m_pStyleName = CreateStaticControl( "HAIRSTYLECARD_TITLE", pFont, TEXT_ALIGN_CENTER_X );

	{ // Note : 스타일 변경 버튼
		CBasicButton* pButton = new CBasicButton;
		pButton->CreateSub( this, "HAIRSTYLECARD_LEFT", UI_FLAG_DEFAULT, HAIRSTYLECARD_LEFT );
		pButton->CreateFlip( "HAIRSTYLECARD_LEFT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton;
		pButton->CreateSub( this, "HAIRSTYLECARD_RIGHT", UI_FLAG_DEFAULT, HAIRSTYLECARD_RIGHT );
		pButton->CreateFlip( "HAIRSTYLECARD_RIGHT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, HAIRSTYLECARD_OK );
		pButton->CreateBaseButton( "HAIRSTYLECARD_OK", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, HAIRSTYLECARD_CANCEL );
		pButton->CreateBaseButton( "HAIRSTYLECARD_CANCEL", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	{ // Note : 헤어 스타일 이미지
		m_pHairStyle = CreateControl( "HAIRSTYLECARD_HAIR" );
	}
}

void CHairStyleCard::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case HAIRSTYLECARD_LEFT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				--m_nHair;

				if ( m_nHair < 0 )
				{
					m_nHair = 0;
					break;
				}
				
				CalcTextureHair();
				m_pHairStyle->SetVisibleSingle( TRUE );
				m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText, m_nHair ) );				
				GLGaeaClient::GetInstance().GetCharacter()->HairStyleChange( m_nHair );
			}
		}
		break;

	case HAIRSTYLECARD_RIGHT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				++m_nHair;

				if ( m_nHair >= m_nMaxHair )
				{
					m_nHair = m_nMaxHair-1;
					break;
				}
				
				CalcTextureHair();
				m_pHairStyle->SetVisibleSingle( TRUE );
				m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText, m_nHair ) );
				GLGaeaClient::GetInstance().GetCharacter()->HairStyleChange( m_nHair );
			}
		}
		break;

	case HAIRSTYLECARD_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bOKButton = TRUE;
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;

	case HAIRSTYLECARD_CANCEL:
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

void CHairStyleCard::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		EMCHARCLASS emClass = GLGaeaClient::GetInstance().GetCharacter()->m_emClass;

		switch( emClass )
		{
		//	머리가 뒤집혀 있는 애들
		case GLCC_FIGHTER_M:	
		case GLCC_SPIRIT_M:		
		case GLCC_FIGHTER_W:	
		case GLCC_SPIRIT_W:		
		case GLCC_EXTREME_M:
			{
				m_bReverse = TRUE;
			}
			break;
		
		case GLCC_ARMS_M:		
		case GLCC_ARCHER_M:	
		case GLCC_ARMS_W:		
		case GLCC_ARCHER_W:
		case GLCC_EXTREME_W:
			{
				m_bReverse = FALSE;
			}
			break;
		default:
			GASSERT( 0 && "EMCHARCLASS::GLCC_NONE" );
		}

		m_nSex = GLGaeaClient::GetInstance().GetCharacter()->m_wSex;
		if( m_nSex == 1 ) // Note : 남자 1, 여자 0
		{
			m_pId2GameText = szHAIRSTYLECARD_NAME_M;
		}
		else
		{
			m_pId2GameText = szHAIRSTYLECARD_NAME_FM;
		}

		EMCHARINDEX emIndex = CharClassToIndex(emClass);
		const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
		m_nMaxHair = sCONST.dwHAIRNUM;

		m_nHair = GLGaeaClient::GetInstance().GetCharacter()->m_wHair;;				
		m_nHairBack = m_nHair;
		
		CalcTextureHair();
		m_pStyleName->AddText( ID2GAMEINTEXT( m_pId2GameText, m_nHair ) );
		m_pHairStyle->SetVisibleSingle( TRUE );
		GLGaeaClient::GetInstance().GetCharacter()->HairStyleInitData();
	}
	else
	{
		if( m_bOKButton )
		{
			GLGaeaClient::GetInstance().GetCharacter()->ReqInvenHairStyleChange( m_nHair );
			m_bOKButton = FALSE;
		}
		else
		{
			GLGaeaClient::GetInstance().GetCharacter()->HairStyleChange( m_nHairBack );
		}

		m_pStyleName->ClearText();		
		GLGaeaClient::GetInstance().GetCharacter()->HairStyleEnd();
	}
}

void CHairStyleCard::CalcTextureHair()
{
	int nHair = m_nHair;
	int nSex = m_nSex == 0 ? 1 : 0;	// Texture 위치가 반대

	if ( m_bReverse ) 
	{
		if ( nHair == 0 ) nHair = 1;
		else if( nHair == 1 ) nHair = 0;
	}	
	
	UIRECT rcTexPos;

	rcTexPos = m_pHairStyle->GetTexturePos();
	rcTexPos.top = rcTexPos.sizeY * ( nSex * 2 );
	rcTexPos.top += rcTexPos.sizeY * ( nHair / 8 );
	rcTexPos.bottom = rcTexPos.top + rcTexPos.sizeY;	
	rcTexPos.left =  rcTexPos.sizeX * ( nHair % 8 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pHairStyle->SetTexturePos( rcTexPos );

}
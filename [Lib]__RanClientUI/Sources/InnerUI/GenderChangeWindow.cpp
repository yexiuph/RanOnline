#include "pch.h"
#include "GenderChangeWindow.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "InnerInterface.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


char * szHAIRSTYLE_NAME_M  = "HAIRSTYLE_NAME_M";
char * szHAIRSTYLE_NAME_FM = "HAIRSTYLE_NAME_FM";
char * szFACESTYLE_NAME_M  = "FACESTYLE_NAME_M";
char * szFACESTYLE_NAME_FM = "FACESTYLE_NAME_FM";

CGenderChangeWindow::CGenderChangeWindow ()
	: m_pFaceStyleName(NULL)
	, m_pHairStyleName (NULL)
	, m_pFaceStyle (NULL)
	, m_pHairStyle (NULL)
	, m_pHairId2GameText (NULL)
	, m_pFaceId2GameText (NULL)
	, m_nHair(0)
	, m_nHairBack(0)
	, m_nMaxHair(0)
	, m_nFace(0)
	, m_nFaceBack(0)
	, m_nMaxFace(0)
	, m_nSex(0)
	, m_bReverse(FALSE)
	, m_bOKButton(FALSE)
{
}

CGenderChangeWindow::~CGenderChangeWindow ()
{
}

CBasicTextBox* CGenderChangeWindow::CreateStaticControl( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl* CGenderChangeWindow::CreateControl( const char * szControl )
{
	GASSERT( szControl );

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub( this, szControl );
	pControl->SetVisibleSingle( FALSE );
	RegisterControl( pControl );

	return pControl;
}

void CGenderChangeWindow::CreateSubControl ()
{
	CD3DFontPar * pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9 );

	CUIWindowEx::CreateControl( "FACESTYLE_TITLE_BACK_L" );
	CUIWindowEx::CreateControl( "FACESTYLE_TITLE_BACK_R" );
	m_pFaceStyleName = CreateStaticControl( "FACESTYLE_TITLE", pFont, TEXT_ALIGN_CENTER_X );

	CUIWindowEx::CreateControl( "HAIRSTYLE_TITLE_BACK_L" );
	CUIWindowEx::CreateControl( "HAIRSTYLE_TITLE_BACK_R" );
	m_pHairStyleName = CreateStaticControl( "HAIRSTYLE_TITLE", pFont, TEXT_ALIGN_CENTER_X );

	{ // Note : 스타일 변경 버튼
		CBasicButton* pButton = new CBasicButton;
		pButton->CreateSub( this, "FACESTYLE_LEFT", UI_FLAG_DEFAULT, FACESTYLE_LEFT );
		pButton->CreateFlip( "FACESTYLE_LEFT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton;
		pButton->CreateSub( this, "FACESTYLE_RIGHT", UI_FLAG_DEFAULT, FACESTYLE_RIGHT );
		pButton->CreateFlip( "FACESTYLE_RIGHT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton;
		pButton->CreateSub( this, "HAIRSTYLE_LEFT", UI_FLAG_DEFAULT, HAIRSTYLE_LEFT );
		pButton->CreateFlip( "HAIRSTYLE_LEFT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton;
		pButton->CreateSub( this, "HAIRSTYLE_RIGHT", UI_FLAG_DEFAULT, HAIRSTYLE_RIGHT );
		pButton->CreateFlip( "HAIRSTYLE_RIGHT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, GENDER_CHANGE_OK );
		pButton->CreateBaseButton( "GENDER_CHANGE_OK", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, GENDER_CHANGE_CANCEL );
		pButton->CreateBaseButton( "GENDER_CHANGE_CANCEL", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	{ // Note : 얼굴 스타일 이미지
		m_pFaceStyle = CreateControl( "FACESTYLE_FACE" );
	}

	{ // Note : 헤어 이미지

		m_pHairStyle = CreateControl( "HAIRSTYLE_HAIR" );
	}
}

void CGenderChangeWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{

	case FACESTYLE_LEFT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				--m_nFace;
				if ( m_nFace < 0 )
				{
					m_nFace = 0;
					break;
				}

				CalcTextureFace();
				m_pFaceStyle->SetVisibleSingle( TRUE );

				m_pFaceStyleName->SetText( ID2GAMEINTEXT( m_pFaceId2GameText, m_nFace ) );
			}
		}
		break;

	case FACESTYLE_RIGHT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				++m_nFace;
				if ( m_nFace >= m_nMaxFace )
				{
					m_nFace = m_nMaxFace-1;
					break;
				}

				CalcTextureFace();
				m_pFaceStyle->SetVisibleSingle( TRUE );

				m_pFaceStyleName->SetText( ID2GAMEINTEXT( m_pFaceId2GameText, m_nFace ) );
			}
		}
		break;

	case HAIRSTYLE_LEFT:
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
				m_pHairStyleName->SetText( ID2GAMEINTEXT( m_pHairId2GameText, m_nHair ) );
				
			}
		}
		break;

	case HAIRSTYLE_RIGHT:
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
				m_pHairStyleName->SetText( ID2GAMEINTEXT( m_pHairId2GameText, m_nHair ) );
			}
		}
		break;

	case GENDER_CHANGE_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bOKButton = TRUE;
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;

	case GENDER_CHANGE_CANCEL:
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

void CGenderChangeWindow::SetVisibleSingle ( BOOL bVisible )
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
		if( m_nSex == 1  ) // Note : 남자 1, 여자 0
		{
			m_pFaceId2GameText = szFACESTYLE_NAME_FM;
			m_pHairId2GameText = szHAIRSTYLE_NAME_FM;			
		}
		else
		{
			m_pFaceId2GameText = szFACESTYLE_NAME_M;
			m_pHairId2GameText = szHAIRSTYLE_NAME_M;			
		}
		
		m_nSex = ( m_nSex > 0)  ? 0 : 1;
		emClass = CharClassToSex( emClass );

		EMCHARINDEX emIndex = CharClassToIndex(emClass);
		const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
		m_nMaxFace = sCONST.dwHEADNUM;
		m_nMaxHair = sCONST.dwHAIRNUM_SELECT;

		m_nFace = 0;
		m_nHair = 0;				

		CalcTextureFace();
		CalcTextureHair();

		m_pFaceStyleName->AddText( ID2GAMEINTEXT( m_pFaceId2GameText, m_nFace ) );
		m_pFaceStyle->SetVisibleSingle( TRUE );

		m_pHairStyleName->AddText( ID2GAMEINTEXT( m_pHairId2GameText, m_nHair ) );
		m_pHairStyle->SetVisibleSingle( TRUE );
		
	}
	else
	{
		if( m_bOKButton )
		{
			GLGaeaClient::GetInstance().GetCharacter()->ReqInvenGenderChange( m_nFace, m_nHair );
			m_bOKButton = FALSE;
		}

		m_pFaceStyleName->ClearText();
		m_pHairStyleName->ClearText();

	}
}


void CGenderChangeWindow::CalcTextureFace()
{
	UIRECT rcTexPos;

	rcTexPos = m_pFaceStyle->GetTexturePos();
	rcTexPos.top = rcTexPos.sizeY * ( m_nSex * 2 );
	rcTexPos.top += rcTexPos.sizeY * ( m_nFace / 8 );
	rcTexPos.bottom = rcTexPos.top + rcTexPos.sizeY;	
	rcTexPos.left =  rcTexPos.sizeX * ( m_nFace % 8 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pFaceStyle->SetTexturePos( rcTexPos );
}


void CGenderChangeWindow::CalcTextureHair()
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
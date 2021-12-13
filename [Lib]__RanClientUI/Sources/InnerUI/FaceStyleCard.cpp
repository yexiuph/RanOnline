#include "pch.h"
#include "FaceStyleCard.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "InnerInterface.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


char * szFACESTYLECARD_NAME_M = "FACESTYLE_NAME_M";
char * szFACESTYLECARD_NAME_FM = "FACESTYLE_NAME_FM";

CFaceStyleCard::CFaceStyleCard ()
	: m_pStyleName(NULL)
	, m_pFaceStyle(NULL)
	, m_pId2GameText(NULL)
	, m_nFace(0)
	, m_nFaceBack(0)
	, m_nMaxFace(0)
	, m_nSex(0)
	, m_bOKButton(FALSE)
{
}

CFaceStyleCard::~CFaceStyleCard ()
{
}

CBasicTextBox* CFaceStyleCard::CreateStaticControl( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl* CFaceStyleCard::CreateControl( const char * szControl )
{
	GASSERT( szControl );

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub( this, szControl );
	pControl->SetVisibleSingle( FALSE );
	RegisterControl( pControl );

	return pControl;
}

void CFaceStyleCard::CreateSubControl ()
{
	CD3DFontPar * pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9 );

	CUIWindowEx::CreateControl( "FACESTYLECARD_TITLE_BACK_L" );
	CUIWindowEx::CreateControl( "FACESTYLECARD_TITLE_BACK_R" );
	m_pStyleName = CreateStaticControl( "FACESTYLECARD_TITLE", pFont, TEXT_ALIGN_CENTER_X );

	{ // Note : 스타일 변경 버튼
		CBasicButton* pButton = new CBasicButton;
		pButton->CreateSub( this, "FACESTYLECARD_LEFT", UI_FLAG_DEFAULT, FACESTYLECARD_LEFT );
		pButton->CreateFlip( "FACESTYLECARD_LEFT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton;
		pButton->CreateSub( this, "FACESTYLECARD_RIGHT", UI_FLAG_DEFAULT, FACESTYLECARD_RIGHT );
		pButton->CreateFlip( "FACESTYLECARD_RIGHT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, FACESTYLECARD_OK );
		pButton->CreateBaseButton( "FACESTYLECARD_OK", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, FACESTYLECARD_CANCEL );
		pButton->CreateBaseButton( "FACESTYLECARD_CANCEL", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	
	{ // Note : 헤어 스타일 이미지
		m_pFaceStyle = CreateControl( "FACESTYLECARD_FACE" );
	}
}

void CFaceStyleCard::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case FACESTYLECARD_LEFT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				--m_nFace ;
				if ( m_nFace < 0 )
				{
					m_nFace = 0;
					break;
				}				

				// 텍스쳐 위치 바꿈
                CalcTexture();
				
				m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText, m_nFace ) );

				// Code : 게임 화면상에 캐릭터의 헤어 스타일을 바꾼다.
				GLGaeaClient::GetInstance().GetCharacter()->FaceStyleChange( m_nFace );
			}
		}
		break;

	case FACESTYLECARD_RIGHT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				++m_nFace ;
				if ( m_nFace >= m_nMaxFace )
				{
					m_nFace = m_nMaxFace-1;
					break;
				}			

				// 텍스쳐 위치 바꿈
                CalcTexture();
				
				m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText, m_nFace ) );

				// Code : 게임 화면상에 캐릭터의 헤어 스타일을 바꾼다.
				GLGaeaClient::GetInstance().GetCharacter()->FaceStyleChange( m_nFace );
			}
		}
		break;

	case FACESTYLECARD_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bOKButton = TRUE;
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;

	case FACESTYLECARD_CANCEL:
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

void CFaceStyleCard::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		EMCHARCLASS emClass = GLGaeaClient::GetInstance().GetCharacter()->m_emClass;

		EMCHARINDEX emIndex = CharClassToIndex(emClass);
		const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
		// 얼굴
		m_nMaxFace = sCONST.dwHEADNUM;

		m_nSex = GLGaeaClient::GetInstance().GetCharacter()->m_wSex;
		if( m_nSex ) // Note : 남자
		{
			m_pId2GameText = szFACESTYLECARD_NAME_M;
		}
		else
		{
			m_pId2GameText = szFACESTYLECARD_NAME_FM;
		}

		m_nFace = GLGaeaClient::GetInstance().GetCharacter()->m_wFace;
		m_nFaceBack = m_nFace;
		m_pStyleName->AddText( ID2GAMEINTEXT( m_pId2GameText, m_nFace ) );
		
		CalcTexture();
		m_pFaceStyle->SetVisibleSingle( TRUE );

		GLGaeaClient::GetInstance().GetCharacter()->FaceStyleInitData();
	}
	else
	{
		if( m_bOKButton )
		{
			GLGaeaClient::GetInstance().GetCharacter()->ReqInvenFaceStyleChange( m_nFace );
			m_bOKButton = FALSE;
		}
		else
		{
			GLGaeaClient::GetInstance().GetCharacter()->FaceStyleChange( m_nFaceBack );
		}

		m_pStyleName->ClearText();
		m_pFaceStyle->SetVisibleSingle( FALSE );
		
		GLGaeaClient::GetInstance().GetCharacter()->FaceStyleEnd();
	}
}


void CFaceStyleCard::CalcTexture()
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
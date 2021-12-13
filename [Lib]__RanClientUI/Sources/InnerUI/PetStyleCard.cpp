#include "pch.h"
#include "PetStyleCard.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "InnerInterface.h"
#include "GLGaeaClient.h"
#include "GLogicData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BOUND_OF_INDEX	-1

INT nPET_STYLE_INDEX_A[7] = {BOUND_OF_INDEX,0,1,2,3,4,BOUND_OF_INDEX};
INT nPET_STYLE_INDEX_B[7] = {BOUND_OF_INDEX,0,1,2,3,4,BOUND_OF_INDEX};
INT nPET_STYLE_INDEX_C[7] = {BOUND_OF_INDEX,0,1,2,3,4,BOUND_OF_INDEX};
INT nPET_STYLE_INDEX_D[7] = {BOUND_OF_INDEX,0,1,2,3,4,BOUND_OF_INDEX};

TCHAR * szPETSTYLECARD_NAME_A = _T("PETSTYLECARD_NAME_A");
TCHAR * szPETSTYLECARD_NAME_B = _T("PETSTYLECARD_NAME_B");
TCHAR * szPETSTYLECARD_NAME_C = _T("PETSTYLECARD_NAME_C");
TCHAR * szPETSTYLECARD_NAME_D = _T("PETSTYLECARD_NAME_D");

CPetStyleCard::CPetStyleCard ()
	: m_pStyleName(NULL)
	, m_pPetStylePos(NULL)
	, m_pId2GameText(NULL)
	, m_wPet(0)
	, m_wPetBack(0)
	, m_emTYPE(PETTYPE_NONE)
	, m_bOKButton(FALSE)
{
	SecureZeroMemory( m_pPetStyle, sizeof( m_pPetStyle ) );
}

CPetStyleCard::~CPetStyleCard ()
{
}

CBasicTextBox* CPetStyleCard::CreateStaticControl( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl* CPetStyleCard::CreateControl( const char * szControl )
{
	GASSERT( szControl );

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub( this, szControl );
	pControl->SetVisibleSingle( FALSE );
	RegisterControl( pControl );

	return pControl;
}

void CPetStyleCard::CreateSubControl ()
{
	CD3DFontPar * pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9 );

	CUIWindowEx::CreateControl( "HAIRSTYLECARD_TITLE_BACK_L" );
	CUIWindowEx::CreateControl( "HAIRSTYLECARD_TITLE_BACK_R" );
	m_pStyleName = CreateStaticControl( "HAIRSTYLECARD_TITLE", pFont, TEXT_ALIGN_CENTER_X );

	{ // Note : 스타일 변경 버튼
		CBasicButton* pButton = new CBasicButton;
		pButton->CreateSub( this, "HAIRSTYLECARD_LEFT", UI_FLAG_DEFAULT, PETSTYLECARD_LEFT );
		pButton->CreateFlip( "HAIRSTYLECARD_LEFT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton;
		pButton->CreateSub( this, "HAIRSTYLECARD_RIGHT", UI_FLAG_DEFAULT, PETSTYLECARD_RIGHT );
		pButton->CreateFlip( "HAIRSTYLECARD_RIGHT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, PETSTYLECARD_OK );
		pButton->CreateBaseButton( "HAIRSTYLECARD_OK", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, PETSTYLECARD_CANCEL );
		pButton->CreateBaseButton( "HAIRSTYLECARD_CANCEL", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	{
		CString strCombine;

		for( INT i=0; i<GLCONST_PET::sPETSTYLE[PETTYPE_A].wSTYLENum; ++i )
		{
			strCombine.Format( "PETSTYLECARD_A_%02d", i );
			m_pPetStyle[PETTYPE_A][i] = CreateControl( strCombine.GetString() );
		}

		for( INT i=0; i<GLCONST_PET::sPETSTYLE[PETTYPE_B].wSTYLENum; ++i )
		{
			strCombine.Format( "PETSTYLECARD_B_%02d", i );
			m_pPetStyle[PETTYPE_B][i] = CreateControl( strCombine.GetString() );
		}

		for( INT i=0; i<GLCONST_PET::sPETSTYLE[PETTYPE_C].wSTYLENum; ++i )
		{
			strCombine.Format( "PETSTYLECARD_C_%02d", i );
			m_pPetStyle[PETTYPE_C][i] = CreateControl( strCombine.GetString() );
		}

		for( INT i=0; i<GLCONST_PET::sPETSTYLE[PETTYPE_D].wSTYLENum; ++i )
		{
			strCombine.Format( "PETSTYLECARD_D_%02d", i );
			m_pPetStyle[PETTYPE_D][i] = CreateControl( strCombine.GetString() );
		}
	}
}

void CPetStyleCard::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case PETSTYLECARD_LEFT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				INT nBackPos = *m_pPetStylePos;

				--m_pPetStylePos;

				if( *m_pPetStylePos == BOUND_OF_INDEX )	++m_pPetStylePos;
				else
				{
					m_pPetStyle[m_emTYPE][nBackPos]->SetVisibleSingle( FALSE );
					m_pPetStyle[m_emTYPE][*m_pPetStylePos]->SetVisibleSingle( TRUE );

					m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText, *m_pPetStylePos ) );


					--m_wPet;
					GLGaeaClient::GetInstance().GetPetClient()->StyleChange( m_wPet );
				}
			}
		}
		break;

	case PETSTYLECARD_RIGHT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				INT nBackPos = *m_pPetStylePos;

				++m_pPetStylePos;

				if( *m_pPetStylePos == BOUND_OF_INDEX )	--m_pPetStylePos;
				else
				{
					m_pPetStyle[m_emTYPE][nBackPos]->SetVisibleSingle( FALSE );
					m_pPetStyle[m_emTYPE][*m_pPetStylePos]->SetVisibleSingle( TRUE );

					m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText, *m_pPetStylePos ) );

					++m_wPet;
					GLGaeaClient::GetInstance().GetPetClient()->StyleChange( m_wPet );
				}
			}
		}
		break;

	case PETSTYLECARD_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bOKButton = TRUE;
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;

	case PETSTYLECARD_CANCEL:
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

void CPetStyleCard::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		m_emTYPE = GLGaeaClient::GetInstance().GetPetClient()->m_emTYPE;

		switch( m_emTYPE )
		{
		case PETTYPE_A:
			{
				m_pPetStylePos = nPET_STYLE_INDEX_A;
				m_pId2GameText = szPETSTYLECARD_NAME_A;
			}
			break;
		case PETTYPE_B:
			{
				m_pPetStylePos = nPET_STYLE_INDEX_B;
				m_pId2GameText = szPETSTYLECARD_NAME_B;
			}
			break;
		case PETTYPE_C:
			{
				m_pPetStylePos = nPET_STYLE_INDEX_C;
				m_pId2GameText = szPETSTYLECARD_NAME_C;
			}
			break;
		case PETTYPE_D:
			{
				m_pPetStylePos = nPET_STYLE_INDEX_D;
				m_pId2GameText = szPETSTYLECARD_NAME_D;
			}
			break;
		default:
			GASSERT( 0 && "PETTYPE::PETTYPE_NONE" );
			return;
		}

		m_wPet = GLGaeaClient::GetInstance().GetPetClient()->m_wStyle;
		m_wPetBack = m_wPet;

		m_pPetStylePos += m_wPet + 1;

		m_pStyleName->AddText( ID2GAMEINTEXT( m_pId2GameText, *m_pPetStylePos ) );
		m_pPetStyle[m_emTYPE][*m_pPetStylePos]->SetVisibleSingle( TRUE );

		GLGaeaClient::GetInstance().GetPetClient()->StyleInitData();
	}
	else
	{
		if( m_bOKButton )
		{
			GLGaeaClient::GetInstance().GetPetClient()->ReqChangeStyle( m_wPet );
			m_bOKButton = FALSE;
		}
		else
		{
			GLGaeaClient::GetInstance().GetPetClient()->StyleChange( m_wPetBack );
		}

		m_pStyleName->ClearText();
		m_pPetStyle[m_emTYPE][*m_pPetStylePos]->SetVisibleSingle( FALSE );
		
		GLGaeaClient::GetInstance().GetPetClient()->StyleEnd();
	}
}
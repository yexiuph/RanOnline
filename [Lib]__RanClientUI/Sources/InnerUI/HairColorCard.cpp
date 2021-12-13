#include "pch.h"
#include "HairColorCard.h"

#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
#include "BasicLineBox.h"
#include "BasicTextButton.h"
#include "InnerInterface.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CHairColorCard::nLIMIT_NUMBER_CHAR = 2;
const int CHairColorCard::nMAX_COLOR = 31;
const float CHairColorCard::fRGB_RATIO = 8.225806f;

CHairColorCard::CHairColorCard ()
	: m_pRProgressBar(NULL)
	, m_pGProgressBar(NULL)
	, m_pBProgressBar(NULL)
	, m_pREditBox(NULL)
	, m_pGEditBox(NULL)
	, m_pBEditBox(NULL)
	, m_RGBControl(NULL)
	, m_nR(0)
	, m_nG(0)
	, m_nB(0)
	, m_nRBack(0)
	, m_nGBack(0)
	, m_nBBack(0)
	, m_nRegR(0)
	, m_nRegG(0)
	, m_nRegB(0)
	, m_bOKButton(FALSE)
{
}

CHairColorCard::~CHairColorCard ()
{
}

CBasicTextBox* CHairColorCard::CreateStaticControl( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void CHairColorCard::CreateSubControl ()
{
	CD3DFontPar * pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9 );

	CreateControl( "HAIRCOLORCARD_TITLE_BACK_L" );
	CreateControl( "HAIRCOLORCARD_TITLE_BACK_R" );
	CBasicTextBox * pTextBox = CreateStaticControl( "HAIRCOLORCARD_TITLE", pFont, TEXT_ALIGN_CENTER_X );
	pTextBox->AddText( ID2GAMEINTEXT("HAIRCOLORCARD_TITLE") );

	{ // Note : R,G,B Static
		CUIControl * pUIControl = new CUIControl;
		pUIControl->CreateSub( this, "HAIRCOLORCARD_R" );
		RegisterControl( pUIControl );

		pUIControl = new CUIControl;
		pUIControl->CreateSub( this, "HAIRCOLORCARD_G" );
		RegisterControl( pUIControl );

		pUIControl = new CUIControl;
		pUIControl->CreateSub( this, "HAIRCOLORCARD_B" );
		RegisterControl( pUIControl );
	}

	{	// Note : 프로그래스바
		CBasicProgressBar* pProgressBar( NULL );
		CUIControl*	pDummyBar( NULL );

		pProgressBar = new CBasicProgressBar;
		pProgressBar->CreateSub ( this, "HAIRCOLORCARD_R_PROGRESS" );
		pProgressBar->CreateOverImage ( "HAIRCOLORCARD_R_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pRProgressBar = pProgressBar;

		pDummyBar = new CUIControl;
		pDummyBar->CreateSub ( this, "HAIRCOLORCARD_R_PROGRESS_DUMMY", UI_FLAG_DEFAULT, HAIRCOLORCARD_R_PROGRESS );
		RegisterControl ( pDummyBar );

		pProgressBar = new CBasicProgressBar;
		pProgressBar->CreateSub ( this, "HAIRCOLORCARD_G_PROGRESS" );
		pProgressBar->CreateOverImage ( "HAIRCOLORCARD_G_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pGProgressBar = pProgressBar;

		pDummyBar = new CUIControl;
		pDummyBar->CreateSub ( this, "HAIRCOLORCARD_G_PROGRESS_DUMMY", UI_FLAG_DEFAULT, HAIRCOLORCARD_G_PROGRESS );
		RegisterControl ( pDummyBar );

		pProgressBar = new CBasicProgressBar;
		pProgressBar->CreateSub ( this, "HAIRCOLORCARD_B_PROGRESS" );
		pProgressBar->CreateOverImage ( "HAIRCOLORCARD_B_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pBProgressBar = pProgressBar;		

		pDummyBar = new CUIControl;
		pDummyBar->CreateSub ( this, "HAIRCOLORCARD_B_PROGRESS_DUMMY", UI_FLAG_DEFAULT, HAIRCOLORCARD_B_PROGRESS );
		RegisterControl ( pDummyBar );
	}

	{ // Note : 입력 에디터 박스
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( "HAIRCOLORCARD_R_EDITBOX_BACK" );
		RegisterControl( pBasicLineBox );

		CUIEditBox* pEditBox = new CUIEditBox;
		pEditBox->CreateSub( this, "HAIRCOLORCARD_R_EDITBOX", UI_FLAG_XSIZE | UI_FLAG_RIGHT, HAIRCOLORCARD_R_EDITBOX );
		pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput( nLIMIT_NUMBER_CHAR );
		pEditBox->SetFont( pFont );
		pEditBox->DoUSE_NUMBER( true );
		RegisterControl( pEditBox );
		m_pREditBox = pEditBox;

		pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( "HAIRCOLORCARD_G_EDITBOX_BACK" );
		RegisterControl( pBasicLineBox );

		pEditBox = new CUIEditBox;
		pEditBox->CreateSub( this, "HAIRCOLORCARD_G_EDITBOX", UI_FLAG_XSIZE | UI_FLAG_RIGHT, HAIRCOLORCARD_G_EDITBOX );
		pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput( nLIMIT_NUMBER_CHAR );
		pEditBox->SetFont( pFont );	
		pEditBox->DoUSE_NUMBER( true );
		RegisterControl( pEditBox );
		m_pGEditBox = pEditBox;

		pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( "HAIRCOLORCARD_B_EDITBOX_BACK" );
		RegisterControl( pBasicLineBox );

		pEditBox = new CUIEditBox;
		pEditBox->CreateSub( this, "HAIRCOLORCARD_B_EDITBOX", UI_FLAG_XSIZE | UI_FLAG_RIGHT, HAIRCOLORCARD_B_EDITBOX );
		pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput( nLIMIT_NUMBER_CHAR );
		pEditBox->SetFont( pFont );
		pEditBox->DoUSE_NUMBER( true );
		RegisterControl( pEditBox );
		m_pBEditBox = pEditBox;
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, HAIRCOLORCARD_OK );
		pButton->CreateBaseButton( "HAIRCOLORCARD_OK", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, HAIRCOLORCARD_CANCEL );
		pButton->CreateBaseButton( "HAIRCOLORCARD_CANCEL", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	{ // Note : 색상 변경 창
		CUIControl * pControl = new CUIControl;
		pControl->CreateSub( this, "HAIRCOLORCARD_RGB_BACK" );
		RegisterControl( pControl );

		pControl = new CUIControl;
		pControl->CreateSub( this, "HAIRCOLORCARD_RGB" );
		pControl->SetUseRender( TRUE );
		RegisterControl( pControl );
		m_RGBControl = pControl;
	}
}

BOOL CHairColorCard::UpdateProgressBar( CBasicProgressBar * pBar, int x )
{
	if( pBar && pBar->IsExclusiveSelfControl() )
	{
		const float fSize = pBar->GetGlobalPos ().sizeX;
		const float fPos = float(x) - pBar->GetGlobalPos ().left;
		const float fPercent = fPos / fSize;
		pBar->SetPercent ( fPercent );

		return TRUE;
	}

	return FALSE;
}

void CHairColorCard::UpdateProgressBarToEditBox( CBasicProgressBar * pBar, CUIEditBox * pEditBox, INT & nColorVal )
{
	int nVal(0);
	float fPercent = pBar->GetPercent();
	if( fPercent > 0 )	nVal = (int)(nMAX_COLOR * fPercent);

	CString strTemp;
	strTemp.Format( "%d", nVal );

	pEditBox->ClearEdit();
	pEditBox->SetEditString( strTemp );

	nColorVal = nVal;
}

BOOL CHairColorCard::UpdateEditBoxToProgressBar( CUIEditBox * pEditBox, CBasicProgressBar * pBar, INT & nColorVal )
{
	if( pEditBox && pEditBox->IsBegin() )
	{
		const CString & strRVal = pEditBox->GetEditString();
		nColorVal = _ttoi( strRVal.GetString() );
		if( nColorVal > nMAX_COLOR )
		{
			nColorVal = nMAX_COLOR;

			pEditBox->ClearEdit();
			pEditBox->SetEditString( CString("31") );
		}

		float fPercent(0);
		if( nColorVal > 0 )	fPercent = nColorVal / float(nMAX_COLOR);
		pBar->SetPercent( fPercent );

		return TRUE;
	}

	return FALSE;
}

void CHairColorCard::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( bFirstControl )
	{
		BOOL bChange( FALSE );

		if( UpdateProgressBar( m_pRProgressBar, x ) )
		{
			UpdateProgressBarToEditBox( m_pRProgressBar, m_pREditBox, m_nR );
			bChange = TRUE;
		}
		else if( UpdateProgressBar( m_pGProgressBar, x ) )
		{
			UpdateProgressBarToEditBox( m_pGProgressBar, m_pGEditBox, m_nG );
			bChange = TRUE;
		}
		else if( UpdateProgressBar( m_pBProgressBar, x ) )
		{
			UpdateProgressBarToEditBox( m_pBProgressBar, m_pBEditBox, m_nB );
			bChange = TRUE;
		}

		if( UpdateEditBoxToProgressBar( m_pREditBox, m_pRProgressBar, m_nR ) )		bChange = TRUE;
		else if( UpdateEditBoxToProgressBar( m_pGEditBox, m_pGProgressBar, m_nG ) )	bChange = TRUE;
		else if( UpdateEditBoxToProgressBar( m_pBEditBox, m_pBProgressBar, m_nB ) )	bChange = TRUE;

		if( bChange )
		{
			RegulateRGB();
			m_RGBControl->SetDiffuse( D3DCOLOR_ARGB( 255, m_nRegR, m_nRegG, m_nRegB ) );

			WORD wTemp = m_nR << 10	| m_nG << 5 | m_nB;
			GLGaeaClient::GetInstance().GetCharacter()->HairColorChange( wTemp );
		}
	}
}

void CHairColorCard::TranslateProgressBarMsg( CBasicProgressBar * pBar, DWORD dwMsg )
{
	if( !pBar ) return;

	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
		{			
			pBar->SetExclusiveControl();
		}
		else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
		{
			pBar->ResetExclusiveControl();
		}
	}
	else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
	{									
		pBar->ResetExclusiveControl();
	}
}

void CHairColorCard::TranslateEditBoxMsg( CUIEditBox * pEditBox, DWORD dwMsg )
{
	if( !pEditBox ) return;

	if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
	{
		pEditBox->BeginEdit();
	}
	else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
	{
		pEditBox->EndEdit();
	}
}

void CHairColorCard::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case HAIRCOLORCARD_R_PROGRESS: TranslateProgressBarMsg( m_pRProgressBar, dwMsg ); break;
	case HAIRCOLORCARD_G_PROGRESS: TranslateProgressBarMsg( m_pGProgressBar, dwMsg ); break;
	case HAIRCOLORCARD_B_PROGRESS: TranslateProgressBarMsg( m_pBProgressBar, dwMsg ); break;

	case HAIRCOLORCARD_R_EDITBOX: TranslateEditBoxMsg( m_pREditBox, dwMsg ); break;
	case HAIRCOLORCARD_G_EDITBOX: TranslateEditBoxMsg( m_pGEditBox, dwMsg ); break;
	case HAIRCOLORCARD_B_EDITBOX: TranslateEditBoxMsg( m_pBEditBox, dwMsg ); break;

	case HAIRCOLORCARD_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bOKButton = TRUE;
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;

	case HAIRCOLORCARD_CANCEL:
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

void CHairColorCard::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		InitUIControl();

		GLGaeaClient::GetInstance().GetCharacter()->HairColorInitData();
	}
	else
	{
		if( m_bOKButton )
		{
			WORD wTemp = m_nR << 10	| m_nG << 5 | m_nB;
			GLGaeaClient::GetInstance().GetCharacter()->ReqInvenHairColorChange( wTemp );

			m_bOKButton = FALSE;
		}
		else
		{
			WORD wTemp = m_nRBack << 10	| m_nGBack << 5 | m_nBBack;
			GLGaeaClient::GetInstance().GetCharacter()->HairColorChange( wTemp );
		}

		ClearEditBox();

		GLGaeaClient::GetInstance().GetCharacter()->HairColorEnd();
	}
}

void CHairColorCard::InitUIControl()
{
	const WORD & wHairColor = GLGaeaClient::GetInstance().GetCharacter()->m_wHairColor;

	// Note : Hair Color Data 구조
	// |  X |       RED      |   GREEN   |    BLUE   |
	// | 15 | 14 13 12 11 10 | 9 8 7 6 5 | 4 3 2 1 0 | 
	m_nRBack = m_nR = (wHairColor & 0x7c00) >> 10;
	m_nGBack = m_nG = (wHairColor & 0x3e0) >> 5;
	m_nBBack = m_nB = (wHairColor & 0x1f);

	CString strTemp;

	strTemp.Format( "%d", m_nR );
	m_pREditBox->SetEditString( strTemp );

	strTemp.Format( "%d", m_nG );
	m_pGEditBox->SetEditString( strTemp );

	strTemp.Format( "%d", m_nB );
	m_pBEditBox->SetEditString( strTemp );

	float fPercent(0);
	if( m_nR > 0 )	fPercent = m_nR / float(nMAX_COLOR);
	m_pRProgressBar->SetPercent( fPercent );

	fPercent = 0;
	if( m_nG > 0 )	fPercent = m_nG / float(nMAX_COLOR);
	m_pGProgressBar->SetPercent( fPercent );

	fPercent = 0;
	if( m_nB > 0 )	fPercent = m_nB / float(nMAX_COLOR);
	m_pBProgressBar->SetPercent( fPercent );

	RegulateRGB();
	m_RGBControl->SetDiffuse( D3DCOLOR_ARGB( 255, m_nRegR, m_nRegG, m_nRegB ) );
}

void CHairColorCard::RegulateRGB()
{
	m_nRegR = int(m_nR * fRGB_RATIO);
	m_nRegG = int(m_nG * fRGB_RATIO);
	m_nRegB = int(m_nB * fRGB_RATIO);
}

void CHairColorCard::ClearEditBox()
{
	m_pREditBox->ClearEdit();
	m_pREditBox->EndEdit();

	m_pGEditBox->ClearEdit();
	m_pGEditBox->EndEdit();

	m_pBEditBox->ClearEdit();
	m_pBEditBox->EndEdit();
}
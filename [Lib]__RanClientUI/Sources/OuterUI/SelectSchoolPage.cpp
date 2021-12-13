#include "pch.h"
#include "SelectSchoolPage.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "../[Lib]__Engine/Sources/DxTools/d3dfont.h"
#include "Outerinterface.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSelectSchoolPage::CSelectSchoolPage() 
	: m_pSchoolNameStatic ( NULL )
	, m_pSchoolDec ( NULL )
	, m_nSchool ( -1 )
{
	memset( m_pSelectSchoolImg, 0, sizeof( CUIControl* ) * MAX_SELSCHOOL );
	memset( m_pSelectSchoolSetImg, 0, sizeof( CUIControl* ) * MAX_SELSCHOOL );	
}

CSelectSchoolPage::~CSelectSchoolPage()
{
}


void CSelectSchoolPage::CreateSubControl ()
{
	float fontSize = 1.0f;

	LONG lResolution = COuterInterface::GetInstance().GetResolution ();
	WORD X_RES = HIWORD( lResolution );

	// 해상도가 작으면 폰트 사이즈를 줄인다. 
	if ( X_RES <= 800 ) fontSize = fFontRate;


	CD3DFontPar* pFont20 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 20 * (DWORD)fontSize, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	m_pSchoolNameStatic = new CBasicTextBox;
	m_pSchoolNameStatic->CreateSub ( this, "NEW_CHAR_SELECT_SCHOOL_NAME", UI_FLAG_DEFAULT, SELECT_SCHOOL_NAME );
	m_pSchoolNameStatic->SetFont ( pFont20 );
	m_pSchoolNameStatic->SetTextAlign ( TEXT_ALIGN_CENTER_X );		
	RegisterControl ( m_pSchoolNameStatic );

	m_pSchoolDec = new CBasicTextBox;
	m_pSchoolDec->CreateSub ( this, "NEW_CHAR_SELECT_SCHOOL_DEC", UI_FLAG_DEFAULT, SELECT_SCHOOL_DEC );
	m_pSchoolDec->SetFont ( pFont9 );
	m_pSchoolDec->SetTextAlign ( TEXT_ALIGN_LEFT );		
	RegisterControl ( m_pSchoolDec );

	CString strTemp;
	
	for ( int i = 0 ; i < MAX_SELSCHOOL; ++i ) 
	{
		strTemp.Format ( "NEW_CHAR_SELECT_SCHOOL_IMAGE%d",i); 
		
		m_pSelectSchoolImg[i] = new CUIControl;
		m_pSelectSchoolImg[i]->CreateSub ( this, strTemp, UI_FLAG_DEFAULT, SELECT_SCHOOL_IMAGE0 + i );	
		m_pSelectSchoolImg[i]->SetVisibleSingle ( TRUE );
		m_pSelectSchoolImg[i]->SetTransparentOption( TRUE );
		RegisterControl ( m_pSelectSchoolImg[i] );

		strTemp.Format ( "NEW_CHAR_SELECT_SCHOOL_SET_IMAGE%d",i);

		m_pSelectSchoolSetImg[i] = new CUIControl;
		m_pSelectSchoolSetImg[i]->CreateSub ( this, strTemp, UI_FLAG_DEFAULT, SELECT_SCHOOL_SET_IMAGE0 + i );	
		m_pSelectSchoolSetImg[i]->SetVisibleSingle ( FALSE );
		m_pSelectSchoolSetImg[i]->SetTransparentOption( TRUE );
		RegisterControl ( m_pSelectSchoolSetImg[i] );
	}
	
	
}

void CSelectSchoolPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSelectSchoolPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SELECT_SCHOOL_IMAGE0:
	case SELECT_SCHOOL_IMAGE1:
	case SELECT_SCHOOL_IMAGE2:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				int select = ControlID - SELECT_SCHOOL_IMAGE0;
				if ( select == m_nSchool ) return;				

				SELECT_SCHOOL_PROCESS( select );
			}
		}
		break;
	}
}

void CSelectSchoolPage::SELECT_SCHOOL_PROCESS( int nSelect )
{
/*
	UIRECT rcPos, rcPosNew, rcPosDefault;
				
	rcPos = m_pSelectSchoolSetImg->GetGlobalPos();
	rcPosDefault = m_pSelectSchoolImg[nSelect]->GetGlobalPos();
	rcPosNew = rcPos;
	rcPosNew.left = rcPosDefault.left - ( (rcPosNew.sizeX - rcPosDefault.sizeX) / 2 ) ;
	rcPosNew.right = rcPosNew.left + rcPosNew.sizeX;
	m_pSelectSchoolSetImg->SetGlobalPos( rcPosNew );

// 임시 코드

	CString strTexture[3];

	for ( int i = 0; i < 3; ++i )
	{
		strTexture[i].Format ( "new_char_school%d.dds", i+1);
	}

//
//	m_pSelectSchoolSetImg->SetTextureName( m_pSelectSchoolImg[nSelect]->GetTextureName() );
//	m_pSelectSchoolSetImg->SetTexturePos( m_pSelectSchoolImg[nSelect]->GetTexturePos() );

	m_pSelectSchoolSetImg->SetTextureName( strTexture[nSelect] );
//	m_pSelectSchoolSetImg->SetTexturePos( );
*/
	if ( m_nSchool > -1 )
	{
		m_pSelectSchoolImg[m_nSchool]->SetFadeTime( 0.5f );
		m_pSelectSchoolImg[m_nSchool]->SetFadeIn();
		m_pSelectSchoolSetImg[m_nSchool]->SetFadeTime( 0.5f );
		m_pSelectSchoolSetImg[m_nSchool]->SetFadeOut();
	}
	m_nSchool = nSelect;
	
	m_pSelectSchoolImg[m_nSchool]->SetFadeTime( 1.0f );
	m_pSelectSchoolImg[m_nSchool]->SetFadeOut();

	m_pSelectSchoolSetImg[m_nSchool]->SetFadeTime( 1.0f );
	m_pSelectSchoolSetImg[m_nSchool]->SetFadeIn();
	
	m_pSchoolNameStatic->SetText( ID2GAMEEXTEXT("NEW_CHAR_SELECT_SCHOOL_NAME",m_nSchool ), NS_UITEXTCOLOR::WHITE );	

	CString strTemp;
	strTemp.Format ( "NEW_CHAR_SELECT_SCHOOL_DEC%d", m_nSchool );
	m_pSchoolDec->ClearText();
	m_pSchoolDec->SetText ( ID2GAMEEXTEXT(strTemp), NS_UITEXTCOLOR::WHITE );
}

void CSelectSchoolPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if ( bVisible )
	{
		for ( int i = 0; i < MAX_SELSCHOOL; ++i ) 
		{
			m_pSelectSchoolImg[i]->SetFadeTime( 0.0f );		
			m_pSelectSchoolImg[i]->SetFadeIn();
			m_pSelectSchoolSetImg[i]->SetFadeTime( 0.0f );
			m_pSelectSchoolSetImg[i]->SetFadeOut();
		}

		m_pSchoolNameStatic->ClearText();
		m_pSchoolDec->ClearText();

		m_nSchool = -1;
		
	}
	else
	{
/*
		for ( int i = 0; i < MAX_SELSCHOOL; ++i ) 
		{
			m_pSelectSchoolImg[i]->SetFadeTime( 0.0f );		
			m_pSelectSchoolImg[i]->SetFOut();
		}
		m_pSelectSchoolSetImg->SetFadeTime( 0.0f );
		m_pSelectSchoolImg[i]->SetFadeOut();
		m_pSchoolNameStatic->ClearText();
		m_pSchoolDec->ClearText();
*/
	}
}
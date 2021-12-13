#include "pch.h"
#include "NewCreateCharPage.h"
#include "BasicTextButton.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "Outerinterface.h"
#include "ModalWindow.h"
#include "SelectSchoolPage.h"
#include "SelectSexPage.h"
#include "SelectClassPage.h"
#include "SelectStylePage.h"
#include "NewCreateCharEnum.h"
#include "DxLobyStage.h"
#include "DxGlobalStage.h"
#include "RanFilter.h"
#include "../[Lib]__Engine/Sources/Common/StringUtils.h"

#include "../[Lib]__Engine/Sources/DxTools/d3dfont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CNewCreateCharPage::CNewCreateCharPage()
	: m_nStage ( SELECT_SCHOOL_STAGE )
	, m_pButtonPrev ( NULL ) 
	, m_pButtonNext ( NULL ) 
	, m_pButtonLeft ( NULL ) 
	, m_pButtonRight ( NULL ) 
	, m_pStageTitle ( NULL ) 
	, m_pStageDec ( NULL ) 
	, m_pSchoolPage ( NULL )
	, m_pSexPage ( NULL )
	, m_pClassPage ( NULL )
	, m_pStylePage ( NULL )
	, m_pBackImage ( NULL ) 
	, m_pDummyImage ( NULL ) 
	, m_nSchool ( -1 )
	, m_nSex ( -1 ) 
	, m_nClass ( -1 ) 
	, m_nHair ( -1 )
	, m_nFace ( -1 )
	, m_bUseCancel ( TRUE )
	, m_hCheckStrDLL( NULL )
	, m_pCheckString( NULL )
	, m_nCreatExtream ( 0 )
{
//	memset( m_pSelectClassImg, 0 , sizeof( CUIControl* ) * 5 );
}


CNewCreateCharPage::~CNewCreateCharPage()
{
	if ( m_hCheckStrDLL ) FreeLibrary( m_hCheckStrDLL );
}


void CNewCreateCharPage::CreateSubControl()
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE16;
	float fontSize = 1.0f;

	LONG lResolution = COuterInterface::GetInstance().GetResolution ();
	WORD X_RES = HIWORD( lResolution );

	// 해상도가 작으면 폰트 사이즈를 줄인다. 
	if ( X_RES <= 800 ) fontSize = fFontRate;

	CD3DFontPar* pFont20 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 20 * (DWORD)fontSize, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont12 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 12 * (DWORD)fontSize, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10 * (DWORD)fontSize, _DEFAULT_FONT_FLAG );

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, "NEW_CHAR_SELECT_RAN_LOGO", UI_FLAG_DEFAULT );	
	RegisterControl ( pControl );

	m_pBackImage =  new CUIControl;
	m_pBackImage->CreateSub ( this, "NEW_CHAR_SELECT_BACK_IMAGE", UI_FLAG_DEFAULT );	
	RegisterControl ( m_pBackImage );

	m_pDummyImage =  new CUIControl;
	m_pDummyImage->CreateSub ( this, "NEW_CHAR_SELECT_DUMMY_IMAGE", UI_FLAG_DEFAULT );	
	m_pDummyImage->SetUseRender ( TRUE );
	m_pDummyImage->SetDiffuse( NS_UITEXTCOLOR::BLACK );
	m_pDummyImage->SetVisibleSingle( FALSE );
	RegisterControl ( m_pDummyImage );

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "NEW_CHAR_SELECT_COPYRIGHT", UI_FLAG_DEFAULT );
	pTextBox->SetFont ( pFont10 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);		
	pTextBox->AddText ( ID2GAMEWORD("COPYRIGHT_COMPANY" ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( pTextBox );

	m_pButtonPrev = new CBasicTextButton;
	m_pButtonPrev->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_DEFAULT, SELECT_PREV_BUTTON );
	m_pButtonPrev->CreateBaseButton ( "NEW_CHAR_SELECT_PREV_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEEXTEXT ( "NEW_CHAR_SELECT_PREV_BUTTON", 0 ) );
	m_pButtonPrev->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( m_pButtonPrev );

	m_pButtonNext = new CBasicTextButton;
	m_pButtonNext->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_DEFAULT, SELECT_NEXT_BUTTON );
	m_pButtonNext->CreateBaseButton ( "NEW_CHAR_SELECT_NEXT_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEEXTEXT( "NEW_CHAR_SELECT_PREV_BUTTON", 1 ) );
	RegisterControl ( m_pButtonNext );



	m_pButtonLeft = new CBasicButton;
	m_pButtonLeft->CreateSub( this, "NEW_CHAR_SELECT_LEFT_BUTTON", UI_FLAG_XSIZE | UI_FLAG_YSIZE, SELECT_LEFT_BUTTON );
	m_pButtonLeft->CreateFlip( "NEW_CHAR_SELECT_LEFT_BUTTON_CLICK", CBasicButton::CLICK_FLIP );
	m_pButtonLeft->SetVisibleSingle ( FALSE );
	RegisterControl( m_pButtonLeft );

	m_pButtonRight = new CBasicButton;
	m_pButtonRight->CreateSub( this, "NEW_CHAR_SELECT_RIGHT_BUTTON", UI_FLAG_RIGHT, SELECT_RIGHT_BUTTON );
	m_pButtonRight->CreateFlip( "NEW_CHAR_SELECT_RIGHT_BUTTON_CLICK", CBasicButton::CLICK_FLIP );
	m_pButtonRight->SetAlignFlag ( UI_FLAG_RIGHT );
	m_pButtonRight->SetVisibleSingle ( FALSE );
	RegisterControl( m_pButtonRight );

	m_pStageTitle = new CBasicTextBox;
	m_pStageTitle->CreateSub ( this, "NEW_CHAR_SELECT_STAGE_TITLE", UI_FLAG_DEFAULT );
	m_pStageTitle->SetFont ( pFont20 );
	m_pStageTitle->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );		
//	m_pStageTitle->AddText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_TITLE",0 ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( m_pStageTitle );

	m_pStageDec = new CBasicTextBox;
	m_pStageDec->CreateSub ( this, "NEW_CHAR_SELECT_STAGE_DEC", UI_FLAG_DEFAULT );
	m_pStageDec->SetFont ( pFont12 );
	m_pStageDec->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);		
//	m_pStageDec->AddText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_TITLE",0 ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( m_pStageDec );	

	m_pSchoolPage = new CSelectSchoolPage;
	m_pSchoolPage->CreateSub ( this, "NEW_CHAR_SELECT_SCHOOL_PAGE", UI_FLAG_DEFAULT, SELECT_SCHOOL_PAGE );
	m_pSchoolPage->CreateSubControl ();
	m_pSchoolPage->SetVisibleSingle( FALSE );
	RegisterControl ( m_pSchoolPage );

	m_pSexPage = new CSelectSexPage;
	m_pSexPage->CreateSub ( this, "NEW_CHAR_SELECT_SEX_PAGE", UI_FLAG_DEFAULT, SELECT_SEX_PAGE );
	m_pSexPage->CreateSubControl ();
	m_pSexPage->SetVisibleSingle( FALSE );
	RegisterControl ( m_pSexPage );

	m_pClassPage = new CSelectClassPage;
	m_pClassPage->CreateSub ( this, "NEW_CHAR_SELECT_CLASS_PAGE", UI_FLAG_DEFAULT, SELECT_CLASS_PAGE );
	m_pClassPage->CreateSubControl ();
	m_pClassPage->SetVisibleSingle( FALSE );
	RegisterControl ( m_pClassPage );

	m_pStylePage = new CSelectStylePage;
	m_pStylePage->CreateSubEx ( this,  "OUTER_WINDOW" , SELECT_STYLE_PAGE );
	m_pStylePage->CreateBaseWidnow ( "NEW_CHAR_SELECT_STYLE_PAGE", (char*) ID2GAMEWORD ( "CREATE_CHARACTER_PAGE" ) );
	m_pStylePage->CreateSubControl ();
	m_pStylePage->SetVisibleSingle( FALSE );
	RegisterControl ( m_pStylePage );


#ifdef TH_PARAM	
	m_hCheckStrDLL = LoadLibrary("ThaiCheck.dll");

	if ( m_hCheckStrDLL )
	{
		m_pCheckString = ( BOOL (_stdcall*)(CString)) GetProcAddress(m_hCheckStrDLL, "IsCompleteThaiChar");
	}

#endif
}


void CNewCreateCharPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CNewCreateCharPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SELECT_NEXT_BUTTON:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				NEXT_STEP( );
			}
		}
		break;
	case SELECT_PREV_BUTTON:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				PREV_STEP( );
			}
		}
		break;

	case SELECT_LEFT_BUTTON:
		{
			if ( m_nStage == SELECT_STYLE_STAGE )
			{
				if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSE_IN_LBDOWNLIKE ( dwMsg ) )
				{
					LEFT_BUTTON_PROCESS();
				}
			}
			else
			{
				if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					LEFT_BUTTON_PROCESS();
				}
			}

		}
		break;
	case SELECT_RIGHT_BUTTON:
		{
			if ( m_nStage == SELECT_STYLE_STAGE )
			{
				if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSE_IN_LBDOWNLIKE ( dwMsg ) )
				{
					RIGHT_BUTTON_PROCESS();
				}
			}
			else
			{
				if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					RIGHT_BUTTON_PROCESS();
				}
			}
		}
		break;	
	}

}

void CNewCreateCharPage::CREATE_CHAR_PROCESS()
{

	CUIEditBox* pEditBox = m_pStylePage->GetEditBox();

	if ( !pEditBox )											return;
	if ( !pEditBox->GetEditLength() )							return;
	CString strTemp = pEditBox->GetEditString ();				
	pEditBox->EndEdit();
	
	if ( !CheckStringName ( strTemp ) )
	{
		pEditBox->ClearEdit();
		return;
	}

	if ( CRanFilter::GetInstance().NameFilter ( strTemp.GetString () ) )
	{
		DoModalOuter ( ID2GAMEEXTEXT("CHARACTER_BADNAME") );
		return ;
	}

	DxLobyStage *pLobyStage = DxGlobalStage::GetInstance().GetLobyStage();
	const GLCHAR_NEWINFO& sNEWINFO = pLobyStage->GetNewCharInfo ();

	if ( sNEWINFO.CHECKVALID () )
	{
		CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
		pNetClient->SndCreateChaInfo
		(
			sNEWINFO.m_emIndex,		/*부서*/
			sNEWINFO.m_wSchool,		/*학교*/
			sNEWINFO.m_wFace,		/*얼굴*/
			sNEWINFO.m_wHair,		/*헤어*/
			sNEWINFO.m_wHairColor,	/*헤어컬러*/
			sNEWINFO.m_wSex,		/*성별*/
			strTemp.GetString ()
		);	

//			COuterInterface::GetInstance().HideGroup ( GetWndID () );
//			COuterInterface::GetInstance().ShowGroupFocus ( SELECT_CHARACTER_PAGE );
	}

	m_bUseCancel = FALSE;
}


void CNewCreateCharPage::LEFT_BUTTON_PROCESS( )
{
	switch( m_nStage )
	{
	case SELECT_SCHOOL_STAGE:
	case SELECT_SEX_STAGE:
		break;
	case SELECT_CLASS_STAGE:
		{
			int nClass = m_pClassPage->GetSelectClass();
			nClass--;
			int nStart = ( m_nCreatExtream ? 0 : 1 );
			if ( nClass < nStart ) nClass = MAX_CLASS-1;
			m_pClassPage->SELECT_CLASS_PROCESS( nClass );
		}
		break;
	case SELECT_STYLE_STAGE:
		{
			DxLobyStage *pLobyStage = DxGlobalStage::GetInstance().GetLobyStage();
			pLobyStage->RotateChar( true );
		}
		break;
	}

}

void CNewCreateCharPage::RIGHT_BUTTON_PROCESS( )
{
	switch( m_nStage )
	{
	case SELECT_SCHOOL_STAGE:
	case SELECT_SEX_STAGE:
		break;
	case SELECT_CLASS_STAGE:
		{
			int nClass = m_pClassPage->GetSelectClass();
			int nStart = ( m_nCreatExtream ? 0 : 1 );
			nClass++;
			if ( nClass >= MAX_CLASS || nClass <  nStart ) nClass = nStart;
			m_pClassPage->SELECT_CLASS_PROCESS( nClass );
		}
		break;
	case SELECT_STYLE_STAGE:
		{
			DxLobyStage *pLobyStage = DxGlobalStage::GetInstance().GetLobyStage();
			pLobyStage->RotateChar( false );
		}
		break;
	}
}

void CNewCreateCharPage::PREV_STEP()
{
	switch( m_nStage )
	{
	case SELECT_SCHOOL_STAGE:
		{
			COuterInterface::GetInstance().ToSelectCharacterPage ( GetWndID () );
		}
		break;
	case SELECT_SEX_STAGE:
		{
			m_pSchoolPage->SetVisibleSingle( TRUE );
			m_pSexPage->SetVisibleSingle( FALSE );
			m_nStage = SELECT_SCHOOL_STAGE;

			m_pStageTitle->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_TITLE",m_nStage ), NS_UITEXTCOLOR::WHITE );
			m_pStageDec->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_DEC",m_nStage ), NS_UITEXTCOLOR::WHITE );

			// Back Image 변경
			{
				m_pBackImage->SetTextureName( szBackImg1 );
				
				
				UIRECT newPos = m_pBackImage->GetTexturePos();

				newPos.top = 0.0f;
				newPos.bottom = 416.0f/1024.0f;

				m_pBackImage->SetTexturePos( newPos );

				m_pBackImage->LoadTexture( m_pd3dDevice );
			}
		}
		break;
	case SELECT_CLASS_STAGE:
		{
			m_pSexPage->SetVisibleSingle( TRUE );
			m_pClassPage->SetVisibleSingle( FALSE );
			m_nStage = SELECT_SEX_STAGE;
			m_pStageTitle->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_TITLE",m_nStage ), NS_UITEXTCOLOR::WHITE );
			m_pStageDec->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_DEC",m_nStage ), NS_UITEXTCOLOR::WHITE );
			m_pButtonLeft->SetVisibleSingle( FALSE );
			m_pButtonRight->SetVisibleSingle( FALSE );
		}
		break;
	case SELECT_STYLE_STAGE:
		{
			if( !m_bUseCancel ) break;
			
			m_pClassPage->SetVisibleSingle( TRUE );
			m_pStylePage->SetVisibleSingle( FALSE );
			m_nStage = SELECT_CLASS_STAGE;
			m_pStageTitle->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_TITLE",m_nStage ), NS_UITEXTCOLOR::WHITE );
			m_pStageDec->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_DEC",m_nStage ), NS_UITEXTCOLOR::WHITE );
			m_pBackImage->SetVisibleSingle(TRUE);
			m_pDummyImage->SetVisibleSingle(FALSE);
			
			DxLobyStage *pLobyStage = DxGlobalStage::GetInstance().GetLobyStage();
			
			CString strTextureName = m_pBackImage->GetTextureName();
			UIRECT rcPos = m_pBackImage->GetGlobalPos();
			UIRECT rcTexPos = m_pBackImage->GetTexturePos();
			pLobyStage->SetStyleStep( false, strTextureName, rcPos, rcTexPos);
		}
		break;
	}
}

void CNewCreateCharPage::NEXT_STEP( )
{
	switch( m_nStage )
	{
	case SELECT_SCHOOL_STAGE:
		{
			m_nSchool = m_pSchoolPage->GetSelectSchool();

			if ( m_nSchool < 0 ) 
			{
				DoModalOuter( ID2GAMEEXTEXT ("NEW_CHAR_SELECT_SCHOOL_ERROR"), MODAL_ERROR, OK );
				return;
			}
			
			// Back Image 변경
			{
				if ( m_nSchool == BONGHWANG_SCHOOL || m_nSchool == HYUNAM_SCHOOL )
				{
					m_pBackImage->SetTextureName( szBackImg2 );
					m_pBackImage->LoadTexture( m_pd3dDevice );
				}

				if ( m_nSchool == SUNGMUN_SCHOOL || m_nSchool == HYUNAM_SCHOOL )
				{
					UIRECT newPos = m_pBackImage->GetTexturePos();

					newPos.top = 608.0f/1024.0f;
					newPos.bottom = 1.0f;

					m_pBackImage->SetTexturePos( newPos );
				}
			}

			m_pSchoolPage->SetVisibleSingle( FALSE );

			m_nStage = SELECT_SEX_STAGE;

			m_pStageTitle->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_TITLE",m_nStage ), NS_UITEXTCOLOR::WHITE );
			m_pStageDec->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_DEC",m_nStage ), NS_UITEXTCOLOR::WHITE );

			m_pSexPage->SetVisibleSingle ( TRUE );
			m_pSexPage->SetTextureImage( m_nSchool );
			
		}
		break;
	case SELECT_SEX_STAGE:
		{
			m_nSex = m_pSexPage->GetSelectSex();

			if ( m_nSex < 0 ) 
			{
				DoModalOuter( ID2GAMEEXTEXT ("NEW_CHAR_SELECT_SEX_ERROR"), MODAL_ERROR, OK );
				return;
			}
			
			m_pSexPage->SetVisibleSingle( FALSE );

			m_nStage = SELECT_CLASS_STAGE;

			m_pStageTitle->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_TITLE",m_nStage ), NS_UITEXTCOLOR::WHITE );
			m_pStageDec->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_DEC",m_nStage ), NS_UITEXTCOLOR::WHITE );

			m_nCreatExtream = 0;
			if ( m_nSex )
			{
				if ( COuterInterface::GetInstance().GetCharRemainExW() > 0 ) m_nCreatExtream = 1;
				m_pClassPage->SetCreateExtream( m_nCreatExtream );
			}
			else
			{
				if ( COuterInterface::GetInstance().GetCharRemainExM() > 0 ) m_nCreatExtream = 1;
				m_pClassPage->SetCreateExtream( m_nCreatExtream );
			}

			m_pClassPage->SetTextureImage( m_nSchool, m_nSex );
	        m_pClassPage->SetVisibleSingle ( TRUE );
			
			m_pButtonLeft->SetVisibleSingle( TRUE );
			m_pButtonRight->SetVisibleSingle( TRUE );
		}
		break;
	case SELECT_CLASS_STAGE:
		{
			m_nClass = m_pClassPage->GetSelectClass();

			if ( m_nClass < 0 ) 
			{
				DoModalOuter( ID2GAMEEXTEXT ("NEW_CHAR_SELECT_CLASS_ERROR"), MODAL_ERROR, OK );
				return;
			}
			
			m_pClassPage->SetVisibleSingle( FALSE );

			m_nStage = SELECT_STYLE_STAGE;

			m_pStageTitle->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_TITLE",m_nStage ), NS_UITEXTCOLOR::WHITE );
			m_pStageDec->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_DEC",m_nStage ), NS_UITEXTCOLOR::WHITE );
			m_pBackImage->SetVisibleSingle(FALSE);
			m_pDummyImage->SetVisibleSingle(TRUE);
			DxLobyStage *pLobyStage = DxGlobalStage::GetInstance().GetLobyStage();
			
			
			
			pLobyStage->SelCharSchool( m_nSchool );
			pLobyStage->SelCharClass( GetCharIndex() );

			CString strTextureName = m_pBackImage->GetTextureName();
			UIRECT rcPos = m_pBackImage->GetGlobalPos();
			UIRECT rcTexPos = m_pBackImage->GetTexturePos();
			pLobyStage->SetStyleStep( true, strTextureName, rcPos, rcTexPos );
			m_pStylePage->SetSelectChar( m_nSchool, m_nSex, m_nClass );

			m_pStylePage->SetVisibleSingle ( TRUE );
		}
		break;
	case SELECT_STYLE_STAGE:
		{
			CREATE_CHAR_PROCESS();
		}
		break;
	}
}

void CNewCreateCharPage::InitData()
{
	m_nStage = SELECT_SCHOOL_STAGE;
	m_pStageTitle->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_TITLE",m_nStage ), NS_UITEXTCOLOR::WHITE );
	m_pStageDec->SetText ( ID2GAMEEXTEXT("NEW_CHAR_SELECT_STAGE_DEC",m_nStage ), NS_UITEXTCOLOR::WHITE );

	m_pSchoolPage->SetVisibleSingle( TRUE );
	m_pSexPage->SetVisibleSingle( FALSE );
	m_pClassPage->SetVisibleSingle( FALSE );
	m_pStylePage->SetVisibleSingle( FALSE );

	m_pButtonLeft->SetVisibleSingle( FALSE );
	m_pButtonRight->SetVisibleSingle( FALSE );
	m_pDummyImage->SetVisibleSingle( FALSE );
	m_pBackImage->SetVisibleSingle( TRUE );

	DxLobyStage *pLobyStage = DxGlobalStage::GetInstance().GetLobyStage();	
	CString strTextureName = m_pBackImage->GetTextureName();
	UIRECT rcPos = m_pBackImage->GetGlobalPos();
	UIRECT rcTexPos = m_pBackImage->GetTexturePos();
	pLobyStage->SetStyleStep( false, strTextureName, rcPos, rcTexPos );
}

int CNewCreateCharPage::GetCharIndex()
{
	if ( !m_nSex ) // 남자
	{
		switch ( m_nClass ) 
		{
		case EXTREME_CLASS:	return GLCI_EXTREME_M;
		case FIGHTER_CLASS:	return GLCI_FIGHTER_M;
		case ARMS_CLASS:	return GLCI_ARMS_M;
		case ARCHER_CLASS:	return GLCI_ARCHER_M;
		case SPIRIT_CLASS:	return GLCI_SPIRIT_M;
		}
	}
	else
	{
		switch ( m_nClass ) 
		{
		case EXTREME_CLASS:	return GLCI_EXTREME_W;
		case FIGHTER_CLASS:	return GLCI_FIGHTER_W;
		case ARMS_CLASS:	return GLCI_ARMS_W;
		case ARCHER_CLASS:	return GLCI_ARCHER_W;
		case SPIRIT_CLASS:	return GLCI_SPIRIT_W;
		}
	}

	return GLCI_FIGHTER_M;

}

void CNewCreateCharPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
	
	if ( bVisible )
	{
		InitData();
	}

	CUIControl* pControl;
	
	pControl = COuterInterface::GetInstance().GetUpImage();
	if ( pControl ) pControl->SetVisibleSingle( !bVisible );
	
	pControl = COuterInterface::GetInstance().GetDownImage();
	if ( pControl ) pControl->SetVisibleSingle( !bVisible );
}

HRESULT CNewCreateCharPage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );
	m_pd3dDevice = pd3dDevice;

	HRESULT hr = S_OK;
	hr = CUIGroup::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}


BOOL CNewCreateCharPage::CheckStringName( CString strTemp )
{
	strTemp = strTemp.Trim();



#ifdef TH_PARAM	
	// 태국어 문자 조합 체크 
		
	if ( !m_pCheckString ) return FALSE;

	if ( !m_pCheckString(strTemp) )
	{
		DoModalOuter ( ID2GAMEEXTEXT ( "CREATESTAGE_9" ), MODAL_INFOMATION, OK, OUTER_MODAL_THAI_CHECK_STRING_ERROR );
		return FALSE;
	}

#endif

#ifdef VN_PARAM
	// 베트남 문자 조합 체크 
	if( STRUTIL::CheckVietnamString( strTemp ) )
	{
		DoModalOuter ( ID2GAMEEXTEXT ( "CREATESTAGE_10" ), MODAL_INFOMATION, OK, OUTER_MODAL_THAI_CHECK_STRING_ERROR );
		return FALSE;
	}

#endif 

	if( STRUTIL::CheckString( strTemp ) )
	{
		DoModalOuter ( ID2GAMEEXTEXT ( "CREATE_CHARACTER_NAME_ERROR" ), MODAL_INFOMATION, OK, OUTER_MODAL_NAME_ERROR );
		return FALSE;
	}

    return TRUE;
}

void CNewCreateCharPage::ResetAll()
{
	m_pStylePage->ResetAll();
}
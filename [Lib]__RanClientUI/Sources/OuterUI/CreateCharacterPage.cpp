#include "pch.h"
#include "./CreateCharacterPage.h"

#include "GameTextControl.h"
#include "UITextControl.h"
#include "BasicTextButton.h"
#include "OuterInterface.h"
#include "DxLobyStage.h"
#include "DxGlobalStage.h"
#include "ModalWindow.h"
#include "RanFilter.h"
#include "DXInputString.h"
#include "CreateCharacterWeb.h"

#include "../[Lib]__Engine/Sources/Common/StringUtils.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CCreateCharacterPage::nLIMITCHAR = 16;

CCreateCharacterPage::CCreateCharacterPage ()
	: m_pSchoolTextBox ( NULL )
	, m_pClassTextBox ( NULL )
	, m_pFaceTextBox ( NULL )
	, m_pHairTextBox ( NULL )
	, m_bUseCancel ( TRUE )
	, m_wClassMax ( GLCI_NUM )
	, m_hCheckStrDLL( NULL )
	, m_pCheckString( NULL )

{
}

CCreateCharacterPage::~CCreateCharacterPage ()
{
	if ( m_hCheckStrDLL ) FreeLibrary( m_hCheckStrDLL );
}

void CCreateCharacterPage::CreateSubControl ()
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE18;
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CreateControl ( "CREATE_PAGE_SCHOOLBACK_M" );
	CreateControl ( "CREATE_PAGE_CLASSBACK_M" );
	CreateControl ( "CREATE_PAGE_FACEBACK_M" );
	CreateControl ( "CREATE_PAGE_HAIRBACK_M" );
	CreateControl ( "CREATE_PAGE_SCHOOLBACK_L" );
	CreateControl ( "CREATE_PAGE_CLASSBACK_L" );
	CreateControl ( "CREATE_PAGE_FACEBACK_L" );
	CreateControl ( "CREATE_PAGE_HAIRBACK_L" );
	CreateControl ( "CREATE_PAGE_SCHOOLBACK_R" );
	CreateControl ( "CREATE_PAGE_CLASSBACK_R" );
	CreateControl ( "CREATE_PAGE_FACEBACK_R" );
	CreateControl ( "CREATE_PAGE_HAIRBACK_R" );

	CreateControl ( "CREATE_PAGE_NAMEBACK_M" );
	CreateControl ( "CREATE_PAGE_NAMEBACK_L" );
	CreateControl ( "CREATE_PAGE_NAMEBACK_R" );
	
	CreateControl ( "CREATE_PAGE_NAMEEDIT_M" );
	CreateControl ( "CREATE_PAGE_NAMEEDIT_L" );
	CreateControl ( "CREATE_PAGE_NAMEEDIT_R" );	

	CBasicButton* pButton = NULL;
	CreateFlipButton ( "CREATE_PAGE_SCHOOL_L", "CREATE_PAGE_SCHOOL_L_F", CREATE_SCHOOL_L );
	CreateFlipButton ( "CREATE_PAGE_SCHOOL_R", "CREATE_PAGE_SCHOOL_R_F", CREATE_SCHOOL_R );
	
	CreateFlipButton ( "CREATE_PAGE_CLASS_L", "CREATE_PAGE_CLASS_L_F", CREATE_CLASS_L );
	CreateFlipButton ( "CREATE_PAGE_CLASS_R", "CREATE_PAGE_CLASS_R_F", CREATE_CLASS_R );
	
	CreateFlipButton ( "CREATE_PAGE_FACE_L", "CREATE_PAGE_FACE_L_F", CREATE_FACE_L );
	CreateFlipButton ( "CREATE_PAGE_FACE_R", "CREATE_PAGE_FACE_R_F", CREATE_FACE_R );

	CreateFlipButton ( "CREATE_PAGE_HAIR_L", "CREATE_PAGE_HAIR_L_F", CREATE_HAIR_L );
	CreateFlipButton ( "CREATE_PAGE_HAIR_R", "CREATE_PAGE_HAIR_R_F", CREATE_HAIR_R );

	m_pSchoolTextBox = CreateStaticControl ( "CREATE_PAGE_SCHOOL_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pClassTextBox = CreateStaticControl ( "CREATE_PAGE_CLASS_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pFaceTextBox = CreateStaticControl ( "CREATE_PAGE_FACE_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pHairTextBox = CreateStaticControl ( "CREATE_PAGE_HAIR_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );


	m_pOKButton = new CBasicTextButton;
	m_pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, CREATE_PAGE_OK );
	m_pOKButton->CreateBaseButton ( "CREATE_PAGE_OK", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CREATE_CHARACTER_PAGE_OKCANCEL", 0 ) );
	m_pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	RegisterControl ( m_pOKButton );
	m_pOKButton->SetVisibleSingle( FALSE );

	CBasicTextButton* pCancelButton = new CBasicTextButton;
	pCancelButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, CREATE_PAGE_CANCEL );
	pCancelButton->CreateBaseButton ( "CREATE_PAGE_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CREATE_CHARACTER_PAGE_OKCANCEL", 1 ) );
	pCancelButton->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( pCancelButton );
	
	CBasicTextBox* pTextBox = NULL;
	pTextBox = CreateStaticControl ( "CREATE_PAGE_SCHOOLBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 0)  );

	pTextBox = CreateStaticControl ( "CREATE_PAGE_CLASSBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 1)  );

	pTextBox = CreateStaticControl ( "CREATE_PAGE_FACEBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 2)  );

	pTextBox = CreateStaticControl ( "CREATE_PAGE_HAIRBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 3)  );

	pTextBox = CreateStaticControl ( "CREATE_PAGE_NAMEBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 4)  );

	m_pEditBox = new CUIEditBox;
	m_pEditBox->CreateSub ( this, "CREATE_CHARACTER_NAMEEDIT_BOX", UI_FLAG_DEFAULT, CREATE_PAGE_NAME_EDIT );
	m_pEditBox->CreateCarrat ( "CREATE_CHARACTER_NAME_CARRAT", TRUE, UINT_MAX );
	m_pEditBox->SetLimitInput ( nLIMITCHAR );
	m_pEditBox->SetFont ( pFont9 );
	RegisterControl ( m_pEditBox );

	m_strClassHair = _T("HAIR_NAME_FIGHTER");
	m_strClassFace = _T("FACE_NAME_FIGHTER");


#ifdef TH_PARAM	
	m_hCheckStrDLL = LoadLibrary("ThaiCheck.dll");

	if ( m_hCheckStrDLL )
	{
		m_pCheckString = ( BOOL (_stdcall*)(CString)) GetProcAddress(m_hCheckStrDLL, "IsCompleteThaiChar");
	}

#endif

}


void CCreateCharacterPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	//CUIWindow::TranslateUIMessage ( ControlID, dwMsg );
	DxLobyStage *pLobyStage = DxGlobalStage::GetInstance().GetLobyStage();

	switch ( ControlID )
	{
	case CREATE_PAGE_OK:
		{
			if ( CHECK_KEYFOCUSED(dwMsg) || CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				if ( CHECK_KEYFOCUSED(dwMsg) &&
					!DXInputString::GetInstance().CheckEnterKeyDown() )		return;

				if ( !m_pEditBox )											return;
				if ( !m_pEditBox->GetEditLength() )							return;
				CString strTemp = m_pEditBox->GetEditString ();				
				m_pEditBox->EndEdit();
				
				if ( !CheckString ( strTemp ) )
				{
					m_pEditBox->ClearEdit();
					return;
				}

				if ( CRanFilter::GetInstance().NameFilter ( strTemp.GetString () ) )
				{
					DoModalOuter ( ID2GAMEEXTEXT("CHARACTER_BADNAME") );
					return ;
				}

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

//					COuterInterface::GetInstance().HideGroup ( GetWndID () );
//					COuterInterface::GetInstance().ShowGroupFocus ( SELECT_CHARACTER_PAGE );
				}

				m_bUseCancel = FALSE;
			}
		}
		break;

	case CREATE_PAGE_CANCEL:
		{
			if( m_bUseCancel )
			{
				if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					m_pEditBox->EndEdit();
					COuterInterface::GetInstance().ToSelectCharacterPage ( GetWndID () );
				}
			}
		}
		break;

	case CREATE_SCHOOL_L:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharSchool(DxLobyStage::EM_PREV);
				COuterInterface::GetInstance().HideGroup( CREATE_CHARACTER_WEB );
			}
		}
		break;

	case CREATE_SCHOOL_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharSchool(DxLobyStage::EM_NEXT);
				COuterInterface::GetInstance().HideGroup( CREATE_CHARACTER_WEB );
			}
		}
		break;

	case CREATE_CLASS_L:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				WORD nIndex = pLobyStage->ShiftCharClass(DxLobyStage::EM_PREV, m_wClassMax);

				CCreateCharacterWeb * pControl = COuterInterface::GetInstance().GetCreateCharacterWeb();
				if( pControl ) 
					pControl->ShiftCharClass( nIndex );
			}
		}
		break;

	case CREATE_CLASS_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				WORD nIndex = pLobyStage->ShiftCharClass(DxLobyStage::EM_NEXT, m_wClassMax);

				CCreateCharacterWeb * pControl = COuterInterface::GetInstance().GetCreateCharacterWeb();
				if( pControl ) 
					pControl->ShiftCharClass( nIndex );
			}
		}
		break;

	case CREATE_FACE_L:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharFace(DxLobyStage::EM_PREV);
			}
		}
		break;

	case CREATE_FACE_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharFace(DxLobyStage::EM_NEXT);
			}
		}
		break;

	case CREATE_HAIR_L:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharHair(DxLobyStage::EM_PREV);
			}
		}
		break;

	case CREATE_HAIR_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharHair(DxLobyStage::EM_NEXT);
			}
		}
		break;

	case CREATE_PAGE_NAME_EDIT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				ResetAll ();
			}
		}
		break;
	}
}

CBasicButton* CCreateCharacterPage::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::CLICK_FLIP );
	RegisterControl ( pButton );

	return pButton;
}

void CCreateCharacterPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		ResetAll ();

		if( COuterInterface::GetInstance().IsCreateExtream() )
		{
			m_wClassMax = GLCI_NUM_EX;
		}
		else
		{
			m_wClassMax = GLCI_NUM;
		}
	}
}

void CCreateCharacterPage::ResetAll ()
{
	m_pEditBox->EndEdit();
	m_pEditBox->ClearEdit();		
	if( !m_pEditBox->IsBegin() )
	{
		m_pEditBox->BeginEdit();
	}
}

void CCreateCharacterPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIOuterWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
	if ( !pNetClient->IsOnline() )		//네트워크 연결이 끊어진 경우
	{
		if ( !COuterInterface::GetInstance().IsVisibleGroup ( MODAL_WINDOW_OUTER ) )
		{
			DoModalOuter ( ID2GAMEEXTEXT ("CHARACTERSTAGE_2"), MODAL_INFOMATION, OK, OUTER_MODAL_CONNECTCLOSED );
		}
	}

	m_pClassTextBox->ClearText ();
	m_pHairTextBox->ClearText ();
	m_pFaceTextBox->ClearText ();

	DxLobyStage *pLobyStage = DxGlobalStage::GetInstance().GetLobyStage();
	const GLCHAR_NEWINFO& sNEWINFO = pLobyStage->GetNewCharInfo ();	
	const EMCHARINDEX& emCharIndex = sNEWINFO.m_emIndex;

	if( emCharIndex < m_wClassMax )
	{
		const CString strClassName = COMMENT::CHARCLASS[emCharIndex].c_str();
		const CString& strHairName = ID2GAMEWORD (m_strClassHair.GetString(), sNEWINFO.m_wHair );
		const CString& strFaceName = ID2GAMEWORD (m_strClassFace.GetString(), sNEWINFO.m_wFace );		
		
		m_pClassTextBox->SetOneLineText ( strClassName );
		m_pHairTextBox->SetOneLineText ( strHairName );
		m_pFaceTextBox->SetOneLineText ( strFaceName );

		m_pOKButton->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pOKButton->SetVisibleSingle( FALSE );
	}

	WORD wSchool = pLobyStage->GetCreateSchool ();
	const CString& strSchoolName = ID2GAMEWORD ("ACADEMY_NAME", wSchool );
	m_pSchoolTextBox->SetOneLineText ( strSchoolName );
}

BOOL CCreateCharacterPage::CheckString( CString strTemp )
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
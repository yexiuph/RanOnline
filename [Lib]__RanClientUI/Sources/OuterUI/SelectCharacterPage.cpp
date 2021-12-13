#include "pch.h"
#include "SelectCharacterPage.h"
#include "SelectCharacterInfoPage.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "BasicLineBox.h"
#include "OuterInterface.h"
#include "DxGlobalStage.h"
#include "ModalWindow.h"
#include "DxLobyStage.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSelectCharacterPage::CSelectCharacterPage ()
	: m_pButtonLeftOn(NULL)
	, m_pButtonLeftOff(NULL)
	, m_pButtonRightOn(NULL)
	, m_pButtonRightOff(NULL)
	, m_pPageNumTextBox(NULL)
	, m_pStartButton(NULL)
{
}

CSelectCharacterPage::~CSelectCharacterPage ()
{
}

void CSelectCharacterPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CSelectCharacterInfoPage* pSelectCharacterPage = new CSelectCharacterInfoPage;
	pSelectCharacterPage->CreateSub ( this, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pSelectCharacterPage->CreateBaseWidnow ( "SELECT_CHARACTER_UPWINDOW", (char*)ID2GAMEWORD("SELECT_CHARACTER_PAGE") );
	pSelectCharacterPage->CreateSubControl ();
	RegisterControl ( pSelectCharacterPage );
	m_pSelectCharacterPage = pSelectCharacterPage;

	const int nBUTTONSIZE = CBasicTextButton::SIZE18;

// 극강부 추가
#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(TW_PARAM) || defined( TH_PARAM ) || defined ( HK_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( CH_PARAM ) || defined ( PH_PARAM ) || defined ( JP_PARAM )
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "SELECT_CHARACTER_MOVE" );
		RegisterControl ( pBasicLineBox );
	}

	{ 
		m_pButtonLeftOn = new CBasicButton;
		m_pButtonLeftOn->CreateSub( this, "SELECT_CHARACTER_LEFT_BUTTON", UI_FLAG_DEFAULT, SELECT_CHARACTER_LEFT_BUTTON );
		m_pButtonLeftOn->CreateFlip( "SELECT_CHARACTER_LEFT_BUTTON_CLICK", CBasicButton::CLICK_FLIP );
		RegisterControl( m_pButtonLeftOn );
	}

	{ 
		m_pButtonLeftOff = new CBasicButton;
		m_pButtonLeftOff->CreateSub( this, "SELECT_CHARACTER_LEFT_BUTTON_FLIP", UI_FLAG_DEFAULT, SELECT_CHARACTER_LEFT_BUTTON_FLIP );
		RegisterControl( m_pButtonLeftOff );
	}

	{ 
		m_pButtonRightOn = new CBasicButton;
		m_pButtonRightOn->CreateSub( this, "SELECT_CHARACTER_RIGHT_BUTTON", UI_FLAG_DEFAULT, SELECT_CHARACTER_RIGHT_BUTTON );
		m_pButtonRightOn->CreateFlip( "SELECT_CHARACTER_RIGHT_BUTTON_CLICK", CBasicButton::CLICK_FLIP );
		RegisterControl( m_pButtonRightOn );
	}

	{ 
		m_pButtonRightOff = new CBasicButton;
		m_pButtonRightOff->CreateSub( this, "SELECT_CHARACTER_RIGHT_BUTTON_FLIP", UI_FLAG_DEFAULT, SELECT_CHARACTER_RIGHT_BUTTON_FLIP );
		RegisterControl( m_pButtonRightOff );
	}
#endif

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "SELECT_CHARACTER_PAGE_DOWNBACK" );
		RegisterControl ( pBasicLineBox );
	}

	{
		m_pStartButton = new CBasicTextButton;
		m_pStartButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, SELECT_CHARACTER_START );
		m_pStartButton->CreateBaseButton ( "SELECT_CHARACTER_START", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SELECT_CHARACTER_PAGE_BUTTON", 1 ) );
		m_pStartButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl ( m_pStartButton );
	}

	{
		CBasicTextButton* pButton = new CBasicTextButton;
		pButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, SELECT_CHARACTER_CANCEL );
		pButton->CreateBaseButton ( "SELECT_CHARACTER_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SELECT_CHARACTER_PAGE_BUTTON", 3 ) );
		pButton->SetShortcutKey ( DIK_ESCAPE );
		RegisterControl ( pButton );
	}

// 극강부 추가
#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(TW_PARAM) || defined( TH_PARAM ) || defined ( HK_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( CH_PARAM ) || defined ( PH_PARAM ) || defined ( JP_PARAM )
	{
		m_pPageNumTextBox = new CBasicTextBox;
		m_pPageNumTextBox->CreateSub ( this, "SELECT_CHARACTER_PAGENUM", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pPageNumTextBox->SetFont ( pFont9 );
		m_pPageNumTextBox->SetPartInterval ( 5.0f );
		m_pPageNumTextBox->SetOneLineText( CString("1") );
		RegisterControl ( m_pPageNumTextBox );
	}
	
	SetLeftButtonState(FALSE);
	SetRightButtonState(FALSE);
#endif
}

BOOL CSelectCharacterPage::SndGameJoin ()
{
	SCHARINFO_LOBBY* pCharInfo = DxGlobalStage::GetInstance().GetLobyStage()->GetSelectCharInfo();
	if ( pCharInfo )		//선택한 캐릭터가 있는 경우
	{
		DxGlobalStage::GetInstance().GetNetClient()->SndGameJoin ( pCharInfo->m_dwCharID );

		COuterInterface::GetInstance().START_WAIT_TIME ( 60.0f );
		DoModalOuter ( ID2GAMEEXTEXT ("CHARACTERSTAGE_GAME_JOIN"), MODAL_INFOMATION, CANCEL, OUTER_MODAL_CHARACTERSTAGE_GAME_JOIN_WAIT );
		return TRUE;
	}

	return FALSE;
}

void CSelectCharacterPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		AddMessageEx ( UIMSG_MOUSEIN_SELECT_CHARACTERPAGE );
	}

	switch ( ControlID )
	{
	case SELECT_CHARACTER_START:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//	NOTE
				//		조인
				if ( !SndGameJoin () )
				{
					DoModalOuter ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), MODAL_INFOMATION, OK );
				}
			}
		}
		break;

	case SELECT_CHARACTER_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DoModalOuter ( ID2GAMEEXTEXT ("CHARACTERSTAGE_6"), MODAL_INFOMATION, OKCANCEL, OUTER_MODAL_LOGOUT );
			}
		}
		break;

// 극강부 추가
#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(TW_PARAM) || defined( TH_PARAM ) || defined ( HK_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( CH_PARAM ) || defined ( PH_PARAM ) || defined ( JP_PARAM )
	// 4명 이상의 캐릭터 생성시에 캐릭터 출력 이동 왼쪽버튼
	case SELECT_CHARACTER_LEFT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DxLobyStage * pLobbyStage = DxGlobalStage::GetInstance().GetLobyStage();
				if( pLobbyStage ) pLobbyStage->ShiftLBDown();
			}
		}
		break;

	// 4명 이상의 캐릭터 생성시에 캐릭터 출력 이동 오른쪽버튼
	case SELECT_CHARACTER_RIGHT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DxLobyStage * pLobbyStage = DxGlobalStage::GetInstance().GetLobyStage();
				if( pLobbyStage ) pLobbyStage->ShiftRBDown();
			}
		}
		break;
#endif
	}
}

void CSelectCharacterPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	DxLobyStage * pLobbyStage = DxGlobalStage::GetInstance().GetLobyStage();
	if( pLobbyStage )
	{
// 극강부 추가
#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(TW_PARAM) || defined( TH_PARAM ) || defined ( HK_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( CH_PARAM ) || defined ( PH_PARAM ) || defined ( JP_PARAM )
		if( pLobbyStage->IsShiftLeft() )
			SetLeftButtonState( TRUE );
		else
			SetLeftButtonState( FALSE );

		if( pLobbyStage->IsShiftRight() )
			SetRightButtonState( TRUE );
		else
			SetRightButtonState( FALSE );
#endif

		if( pLobbyStage->IsCharSelect() )
			m_pStartButton->SetVisibleSingle( TRUE );
		else
			m_pStartButton->SetVisibleSingle( FALSE );
	}
}

void CSelectCharacterPage::ResetAll ()
{
	if ( m_pSelectCharacterPage ) 
		m_pSelectCharacterPage->ResetAll ();
}

// 캐릭터가 4명 이상일 경우에 사용되는 버튼들의 상태를 설정한다.
// TRUE이면 버튼이 활성화 되고 FALSE이면 비활성 상태가 된다.
void CSelectCharacterPage::SetLeftButtonState(BOOL bState)
{
	m_pButtonLeftOn->SetVisibleSingle(bState);
	m_pButtonLeftOff->SetVisibleSingle(!bState);
}

void CSelectCharacterPage::SetRightButtonState(BOOL bState)
{
	m_pButtonRightOn->SetVisibleSingle(bState);
	m_pButtonRightOff->SetVisibleSingle(!bState);
}

// 페이지 숫자를 설정한다.
void CSelectCharacterPage::SetPageNum(int nPageNum)
{
// 극강부 추가
#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(TW_PARAM) || defined( TH_PARAM ) || defined ( HK_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( CH_PARAM ) || defined ( PH_PARAM ) || defined ( JP_PARAM )
	CString strPage;
	strPage.Format ( "%d", nPageNum);

	m_pPageNumTextBox->SetOneLineText( strPage );
#endif
}

// 설정된 텍스트 박스의 값을 가져온다.
int CSelectCharacterPage::GetPageNum()
{
// 극강부 추가
#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(TW_PARAM) || defined( TH_PARAM ) || defined ( HK_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( CH_PARAM ) || defined ( PH_PARAM ) || defined ( JP_PARAM )
	const CString& strTemp = m_pPageNumTextBox->GetText(0);

	return static_cast<int>( atoi ( strTemp ) );
#else
	return 0;
#endif
}
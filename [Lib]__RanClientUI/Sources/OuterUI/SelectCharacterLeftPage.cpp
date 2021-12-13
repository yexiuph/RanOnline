#include "pch.h"
#include "SelectCharacterLeftPage.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "BasicLineBox.h"
#include "OuterInterface.h"
#include "DxGlobalStage.h"
#include "ModalWindow.h"
#include "RANPARAM.h"
#include "SelectCharacterCard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSelectCharacterLeftPage::CSelectCharacterLeftPage ()
	: m_pDelCharInfo(NULL)
	, m_pSelectCharacterCard(NULL)
	, m_pNewButton(NULL)
{
}

CSelectCharacterLeftPage::~CSelectCharacterLeftPage ()
{
}

void CSelectCharacterLeftPage::CreateSubControl ()
{
	m_pSelectCharacterCard = new CSelectCharacterCard;
	m_pSelectCharacterCard->CreateSub ( this, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pSelectCharacterCard->CreateBaseWidnow ( "SELECT_CHARACTER_LEFTWINDOW", (char*)ID2GAMEWORD("SELECT_CHARACTER_CARD") );
	m_pSelectCharacterCard->CreateSubControl ();
	RegisterControl ( m_pSelectCharacterCard );

	const int nBUTTONSIZE = CBasicTextButton::SIZE18;

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER_FAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuterFat ( "SELECT_CHARACTER_LEFTBACK" );
		RegisterControl ( pBasicLineBox );		
	}

	{
		m_pNewButton = new CBasicTextButton;
		m_pNewButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, SELECT_CHARACTER_NEW );
		m_pNewButton->CreateBaseButton ( "SELECT_CHARACTER_NEW", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SELECT_CHARACTER_PAGE_BUTTON", 0 ) );		
		RegisterControl ( m_pNewButton );
	}

	{
		CBasicTextButton* pButton = new CBasicTextButton;
		pButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, SELECT_CHARACTER_DELETE );
		pButton->CreateBaseButton ( "SELECT_CHARACTER_DELETE", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SELECT_CHARACTER_PAGE_BUTTON", 2 ) );
		pButton->SetShortcutKey ( DIK_DELETE );
		RegisterControl ( pButton );
	}
}

void CSelectCharacterLeftPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		AddMessageEx ( UIMSG_MOUSEIN_SELECT_CHARACTERLEFTPAGE );
	}

	switch ( ControlID )
	{
	case SELECT_CHARACTER_NEW:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				const USHORT uCharRemain = COuterInterface::GetInstance().GetCharRemain ();
				const int nChaSNum = DxGlobalStage::GetInstance().GetLobyStage()->GetChaSNum ();

				//	서버당 생성가능한 숫자 초과
				if ( MAX_ONESERVERCHAR_NUM <= nChaSNum )
				{
					DoModalOuter ( ID2GAMEEXTEXT ("CHARACTER_THIS_SERVER_SLOT_FULL" ), MODAL_INFOMATION );
					break;
				}

				//	사용가능카드가 모자랄때
				if ( !uCharRemain )
				{
					DoModalOuter ( ID2GAMEEXTEXT ("CHARACTER_NOT_ENOUGH_CHARREMAIN" ), MODAL_INFOMATION );
					break;
				}

				COuterInterface::GetInstance().ToCreateCharacterPage ( GetWndID () );
			}
		}
		break;

	case SELECT_CHARACTER_DELETE:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pDelCharInfo = DxGlobalStage::GetInstance().GetLobyStage()->GetSelectCharInfo();
				if ( m_pDelCharInfo )
				{
					CString strTemp;					

					switch ( RANPARAM::emSERVICE_TYPE )
					{
					case EMSERVICE_DEFAULT:
					case EMSERVICE_INDONESIA:
					case EMSERVICE_FEYA:
					case EMSERVICE_THAILAND:
					case EMSERVICE_CHINA:
						strTemp = COuterInterface::GetInstance().MakeString ( ID2GAMEEXTEXT ("CHARACTERSTAGE_CAUTION_DELETE"), m_pDelCharInfo->m_szName );
						DoModalOuter ( strTemp.GetString(), MODAL_INPUT, EDITBOX, OUTER_MODAL_SECONDPASSWORD, TRUE );
						break;

					default:
						strTemp = COuterInterface::GetInstance().MakeString ( ID2GAMEEXTEXT ("CHARACTERSTAGE_CAUTION_DELETE_DAUM"), m_pDelCharInfo->m_szName );
						DoModalOuter ( strTemp.GetString(), MODAL_QUESTION, YESNO, OUTER_MODAL_SECONDPASSWORD );
						break;
					};
				}
				else
				{
					DoModalOuter ( ID2GAMEEXTEXT ("CHARACTERSTAGE_NOT_SELECTED"), MODAL_INFOMATION, OK );
				}
			}
		}
		break;
	}
}

void CSelectCharacterLeftPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	const USHORT uCharRemain = COuterInterface::GetInstance().GetCharRemain();
	if( uCharRemain > 0 )
	{
		m_pNewButton->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pNewButton->SetVisibleSingle( FALSE );
	}
}
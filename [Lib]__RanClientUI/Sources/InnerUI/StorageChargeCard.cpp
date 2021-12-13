#include "pch.h"
#include "StorageChargeCard.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicLineBoxEx.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "InnerInterface.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "ModalWindow.h"
#include "ModalCallerID.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CStorageChargeCard::CStorageChargeCard () :
	m_bCHECK_MOUSE_STATE ( false ),
	m_wSTORAGE ( 0 )
{
}

CStorageChargeCard::~CStorageChargeCard ()
{
}

void CStorageChargeCard::CreateSubControl ()
{
	CBasicLineBoxEx* pBasicLineBox = new CBasicLineBoxEx;
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_ESC", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxStorageCard ( "STORAGE_CHARGE_CARD_BACK" );
	RegisterControl ( pBasicLineBox );

	CUIControl* pOVER = new CUIControl;
	pOVER->CreateSub ( this, "STORAGE_CHARGE_TEXTBACK2" );
	pOVER->SetUseRender ( TRUE );	
	RegisterControl ( pOVER );
	m_pOVER = pOVER;

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_DIALOGUE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, STORAGE_NO2 );
		pBasicLineBox->CreateBaseBoxDialogue ( "STORAGE_CHARGE_TEXTBACK2" );
		RegisterControl ( pBasicLineBox );
		m_pLineBox2 = pBasicLineBox;
	}
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_DIALOGUE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, STORAGE_NO3 );
		pBasicLineBox->CreateBaseBoxDialogue ( "STORAGE_CHARGE_TEXTBACK3" );
		RegisterControl ( pBasicLineBox );
		m_pLineBox3 = pBasicLineBox;
	}
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_DIALOGUE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, STORAGE_NO4 );
		pBasicLineBox->CreateBaseBoxDialogue ( "STORAGE_CHARGE_TEXTBACK4" );
		RegisterControl ( pBasicLineBox );
		m_pLineBox4 = pBasicLineBox;
	}

	CreateFlipButton ( "STORAGE_CLOSE_BUTTON", "STORAGE_CLOSE_BUTTON_F", STORAGE_CLOSE_BUTTON, CBasicButton::CLICK_FLIP );

	m_pButtonText2 = CreateTextBox ( "STROAGE_CHARGE_BUTTON2", STORAGE_BUTTON_NO2 );
	m_pButtonText2->AddText ( ID2GAMEWORD("STORAGE_CARD_NUMBER",0), NS_UITEXTCOLOR::LIGHTGREY );

	m_pButtonText3 = CreateTextBox ( "STROAGE_CHARGE_BUTTON3", STORAGE_BUTTON_NO3 );
	m_pButtonText3->AddText ( ID2GAMEWORD("STORAGE_CARD_NUMBER",1), NS_UITEXTCOLOR::LIGHTGREY );

	m_pButtonText4 = CreateTextBox ( "STROAGE_CHARGE_BUTTON4", STORAGE_BUTTON_NO4 );
	m_pButtonText4->AddText ( ID2GAMEWORD("STORAGE_CARD_NUMBER",2), NS_UITEXTCOLOR::LIGHTGREY );

	m_pTextBox2 = CreateTextBox ( "STROAGE_CHARGE_TEXT2", STORAGE_TEXT_NO2 );
	m_pTextBox3 = CreateTextBox ( "STROAGE_CHARGE_TEXT3", STORAGE_TEXT_NO3 );
	m_pTextBox4 = CreateTextBox ( "STROAGE_CHARGE_TEXT4", STORAGE_TEXT_NO4 );
}


void CStorageChargeCard::CheckMouseState ()
{
	const DWORD dwMsg = GetMessageEx ();
	if ( CHECK_MOUSE_IN ( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
	{
		bool bANOTHER_ACTION = false;
		DWORD dwStorage2 = m_pLineBox2->GetMessageEx ();
		DWORD dwStorage3 = m_pLineBox3->GetMessageEx ();
		DWORD dwStorage4 = m_pLineBox4->GetMessageEx ();

		if ( CHECK_MOUSE_IN ( dwStorage2 ) || CHECK_MOUSE_IN ( dwStorage3 ) || CHECK_MOUSE_IN ( dwStorage4 ) )
		{
			bANOTHER_ACTION = true;
		}

		if ( dwMsg & UIMSG_LB_DOWN && !bANOTHER_ACTION )	//	마우스를 좌클릭으로 눌렀을 때,
		{			
			SetExclusiveControl();	//	단독 컨트롤로 등록하고,
			if ( !m_bFirstGap )				//	최초 포지션 갭을 기록한다.
			{
				UIRECT rcPos = GetGlobalPos ();
				m_vGap.x = m_PosX - rcPos.left;
				m_vGap.y = m_PosY - rcPos.top;
				m_bFirstGap = TRUE;
			}
		}
		else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	좌클릭했던것을 놓으면
		{
			ResetExclusiveControl();	//	단독 컨트롤을 해제하고
			m_bFirstGap = FALSE;				//	최초 포지션갭을 해제한다.
		}
	}
	else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	컨트롤 영역밖에서 버튼을 떼는 경우가
	{											//	발생하더라도
		ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
		m_bFirstGap = FALSE;					//	최초 포지션갭을 해제한다.
	}
}


void CStorageChargeCard::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	m_bCHECK_MOUSE_STATE = false;

	m_PosX = x;
	m_PosY = y;

	m_pOVER->SetVisibleSingle ( FALSE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( IsExclusiveSelfControl() )
	{
		SetGlobalPos ( D3DXVECTOR2 ( x - m_vGap.x, y - m_vGap.y ) );
	}
}

void CStorageChargeCard::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	if ( !m_bCHECK_MOUSE_STATE )
	{
		CheckMouseState ();
		m_bCHECK_MOUSE_STATE = true;
	}

	switch ( cID )
	{
	case STORAGE_CLOSE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;

	case STORAGE_NO2:
	case STORAGE_NO3:
	case STORAGE_NO4:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				CUIControl* pControl = FindControl ( cID );
				if ( pControl )
				{
					const UIRECT& rcGlobalPos = pControl->GetGlobalPos ();
					m_pOVER->SetGlobalPos ( rcGlobalPos );
					m_pOVER->SetVisibleSingle ( TRUE );
					m_pOVER->SetDiffuse ( D3DCOLOR_ARGB(125,0xFF,0xFF,0xFF) );
				}

				if ( UIMSG_LB_UP & dwMsg )
				{
					CInnerInterface::GetInstance().HideGroup ( GetWndID () );
					
					m_wSTORAGE = static_cast<WORD>(cID - STORAGE_NO2 + EMSTORAGE_CHANNEL_SPAN);

					CString strTemp = CInnerInterface::GetInstance().MakeString ( ID2GAMEINTEXT("STORAGE_CHARGE_CARD_CONFIRM"), m_wSTORAGE + 1 );
					DoModal ( strTemp, MODAL_QUESTION, YESNO, MODAL_STORAGE_CHARGE );					
				}
			}
		}
		break;
	}

	CUIGroup::TranslateUIMessage ( cID, dwMsg );
}

CBasicButton* CStorageChargeCard::CreateFlipButton ( char* szButton, char* szButtonFlip, const UIGUID& cID, const int& nFLIP_TYPE )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, cID );
	pButton->CreateFlip ( szButtonFlip, nFLIP_TYPE );
	pButton->SetUseDynamic ( TRUE );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox* CStorageChargeCard::CreateTextBox ( const CString& strKeyword, const UIGUID& cID )
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, const_cast<CString&>(strKeyword).GetString(), UI_FLAG_DEFAULT, cID );
    pTextBox->SetFont ( pFont9 );	
	RegisterControl ( pTextBox );
	return pTextBox;
}

void CStorageChargeCard::GetSelectedChargeCard ( WORD& wPosX, WORD& wPosY, WORD& wSTORAGE )
{
	wPosX = m_wItemPosX;
	wPosY = m_wItemPosY;
	wSTORAGE = m_wSTORAGE;
}

void CStorageChargeCard::InitStorageChargeCard ( const WORD& wPosX, const WORD& wPosY )
{
	m_wItemPosX = wPosX;
	m_wItemPosY = wPosY;

	m_wSTORAGE = 0;

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter ();	
	for ( int i = EMSTORAGE_CHANNEL_SPAN; i < EMSTORAGE_CHANNEL_SPAN+EMSTORAGE_CHANNEL_SPAN_SIZE; i++ )
	{
		CBasicTextBox* pTextBox = (CBasicTextBox*) FindControl ( (i-EMSTORAGE_CHANNEL_SPAN) + STORAGE_TEXT_NO2 );
		if ( !pTextBox ) continue;
		
		pTextBox->ClearText ();

		const bool bUSABLE = pCharacter->IsKEEP_STORAGE (static_cast<DWORD>(i));
		if ( bUSABLE )
		{
			pTextBox->AddText ( ID2GAMEWORD("STORAGE_USABLE",0), NS_UITEXTCOLOR::GREENYELLOW );

			const CTime cTime = pCharacter->GetStorageTime(static_cast<DWORD>(i));			
			CString strCombine = CInnerInterface::GetInstance().MakeString ( ID2GAMEWORD("STORAGE_TIMELEFT"),
				cTime.GetYear (), cTime.GetMonth (), cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () );

			pTextBox->AddText ( strCombine, NS_UITEXTCOLOR::WHITESMOKE );
		}
		else
		{
			pTextBox->AddText ( ID2GAMEWORD("STORAGE_USABLE",1), NS_UITEXTCOLOR::RED );
		}
	}	
}

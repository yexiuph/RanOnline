#include "pch.h"
#include "PrivateMarketSellWindow.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextButton.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "BasicLineBox.h"
#include "UITextControl.h"
#include "BasicComboBox.h"
#include "BasicComboBoxRollOver.h"
#include "InnerInterface.h"
#include "GLogicData.h"
#include "RANPARAM.h"
#include "ModalCallerID.h"
#include "GLCharDefine.h"
#include "GLPartyClient.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"
#include "../[Lib]__EngineUI/Sources/UIEditBoxMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char CPrivateMarketSellWindow::BLANK_SYMBOL = ' ';

CPrivateMarketSellWindow::CPrivateMarketSellWindow ()
{
}

CPrivateMarketSellWindow::~CPrivateMarketSellWindow ()
{
}

void CPrivateMarketSellWindow::CreateBaseModal ( char* szWindowKeyword )
{
	CreateTitle ( "MODAL_WINDOW_TITLE", "MODAL_WINDOW_TITLE_LEFT", "MODAL_WINDOW_TITLE_MID", 
					"MODAL_WINDOW_TITLE_RIGHT", "MODAL_WINDOW_TEXTBOX", NULL );
	CreateBody ( "MODAL_WINDOW_BODY", "MODAL_WINDOW_BODY_LEFT", "MODAL_WINDOW_BODY_UP", 
					"MODAL_WINDOW_BODY_MAIN", "MODAL_WINDOW_BODY_DOWN", "MODAL_WINDOW_BODY_RIGHT" );
					
	ResizeControl ( szWindowKeyword );
}

void CPrivateMarketSellWindow::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	m_pTextBox = CreateStaticControl ( "PRIVATE_MARKET_SELL_WINDOW_TEXTBOX", pFont );
	m_pTextBox->AddText ( ID2GAMEINTEXT("PRIVATE_MARKET_SELL_WINDOW_TEXTBOX") );

	{
		CBasicTextBox* pTextBox = CreateStaticControl ( "PRIVATE_MARKET_SELL_WINDOW_MONEY", pFont );
		pTextBox->AddText ( ID2GAMEWORD("PRIVATE_MARKET_SELL_WINDOW_STATICTEXT",0) );

		m_pItemCountStatic = CreateStaticControl ( "PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT", pFont );
		m_pItemCountStatic->AddText ( ID2GAMEWORD("PRIVATE_MARKET_SELL_WINDOW_STATICTEXT",1) );
	}

	{
		{
			CBasicLineBox* pBasicLineBox = new CBasicLineBox;
			pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pBasicLineBox->CreateBaseBoxEditBox ( "PRIVATE_MARKET_SELL_WINDOW_MONEY_EDIT_BACK" );
			RegisterControl ( pBasicLineBox );
		}

		{
			CBasicLineBox* pBasicLineBox = new CBasicLineBox;
			pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pBasicLineBox->CreateBaseBoxEditBox ( "PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT_EDIT_BACK" );
			RegisterControl ( pBasicLineBox );
			m_pItemCountLineBox = pBasicLineBox;
		}

		CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan;
		pEditBoxMan->CreateSub ( this, "PRIVATE_MARKET_SELL_WINDOW_EDITMAN" );
		pEditBoxMan->CreateEditBox ( PRIVATE_MARKET_SELL_WINDOW_MONEY_EDIT, "PRIVATE_MARKET_SELL_WINDOW_MONEY_EDIT", 
									"PRIVATE_MARKET_SELL_WINDOW_CARRAT", TRUE, UINT_MAX, pFont, 9 );
		pEditBoxMan->CreateEditBox ( PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT_EDIT, "PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT_EDIT", 
									"PRIVATE_MARKET_SELL_WINDOW_CARRAT", TRUE, UINT_MAX, pFont, 6 );

		CUIEditBox* pEditBox = NULL;
		pEditBox = pEditBoxMan->GetEditBox ( PRIVATE_MARKET_SELL_WINDOW_MONEY_EDIT );
		pEditBox->SetMinusSign( FALSE );
		pEditBox->SET_MONEY_FORMAT ( 3 );

		pEditBox = pEditBoxMan->GetEditBox ( PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT_EDIT );
		pEditBox->SetMinusSign( FALSE );
		pEditBox->DoUSE_NUMBER( true );

		RegisterControl ( pEditBoxMan );
		m_pEditBoxMan = pEditBoxMan;
	}

	{
		CBasicTextButton* pButton = NULL;
		pButton = CreateTextButton ( "PRIVATE_MARKET_SELL_WINDOW_OK", PRIVATE_MARKET_SELL_WINDOW_OK, 
									const_cast<char*>(ID2GAMEWORD("PRIVATE_MARKET_MAKE_BUTTON",0)) );
		pButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
		pButton = CreateTextButton ( "PRIVATE_MARKET_SELL_WINDOW_CANCEL", PRIVATE_MARKET_SELL_WINDOW_CANCEL, 
									const_cast<char*>(ID2GAMEWORD("PRIVATE_MARKET_MAKE_BUTTON",1)) );
		pButton->SetShortcutKey ( DIK_ESCAPE );
	}
}

CUIControl* CPrivateMarketSellWindow::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextButton* CPrivateMarketSellWindow::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox*	CPrivateMarketSellWindow::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	RegisterControl ( pStaticText );
	return pStaticText;
}

CBasicButton*	CPrivateMarketSellWindow::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );
	return pButton;
}

void CPrivateMarketSellWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIModal::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	//	탭 이동
	if ( m_pEditBoxMan )
	{
		CUIControl* pParent = m_pEditBoxMan->GetTopParent ();
		if ( !pParent )	pParent = this;	//	만약 이클래스가 최상위 컨트롤인 경우
		BOOL bFocus = ( pParent->IsFocusControl() );

		if ( bFocus )
		{
			if ( UIKeyCheck::GetInstance()->Check ( DIK_TAB, DXKEY_DOWN ) )
			{
				m_pEditBoxMan->GoNextTab ();
			}
		}
	}
}

void CPrivateMarketSellWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIModal::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case PRIVATE_MARKET_SELL_WINDOW_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_bCOUNTABLE )
				{
					CString strTrim = m_pEditBoxMan->GetEditString ( PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT_EDIT );
					strTrim.Trim ( BLANK_SYMBOL );
					if ( !m_pEditBoxMan->GetEditLength ( PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT_EDIT ) || !strTrim.GetLength () ) 
						return ;
				}

				{
					CString strTrim = m_pEditBoxMan->GetEditString ( PRIVATE_MARKET_SELL_WINDOW_MONEY_EDIT );
					strTrim.Trim ( BLANK_SYMBOL );
					if ( !m_pEditBoxMan->GetEditLength ( PRIVATE_MARKET_SELL_WINDOW_MONEY_EDIT ) || !strTrim.GetLength () ) 
						return ;
				}

				AddMessageEx ( UIMSG_MODAL_OK );
				CInnerInterface::GetInstance().HideGroup ( GetWndID (), true );
			}
		}
		break;

	case PRIVATE_MARKET_SELL_WINDOW_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_MODAL_CANCEL );
				CInnerInterface::GetInstance().HideGroup ( GetWndID (), true );
			}
		}
		break;
	}
}

void CPrivateMarketSellWindow::ResizeControl ( char* szWindowKeyword )
{
    CUIControl TempControl;
	TempControl.Create ( 1, szWindowKeyword );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CPrivateMarketSellWindow::ResetAll ()
{
	m_pEditBoxMan->ClearEdit ( PRIVATE_MARKET_SELL_WINDOW_MONEY_EDIT );
	m_pEditBoxMan->ClearEdit ( PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT_EDIT );

	m_pEditBoxMan->Init ();
	m_pEditBoxMan->EndEdit ();
	m_pEditBoxMan->BeginEdit ();
}

void	CPrivateMarketSellWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIModal::SetVisibleSingle ( bVisible );

	if ( bVisible )
	{
		ResetAll ();
	}
	else
	{
		m_pEditBoxMan->EndEdit ();
	}
}

void CPrivateMarketSellWindow::SetType ( const bool bCOUNTABLE )
{
	const BOOL bVisible = (bCOUNTABLE)?TRUE:FALSE;

	m_pEditBoxMan->SetVisibleEdit ( PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT_EDIT, bVisible );
	m_pItemCountStatic->SetVisibleSingle ( bVisible );
	m_pItemCountLineBox->SetVisibleSingle ( bVisible );

	m_bCOUNTABLE = bCOUNTABLE;
}

const DWORD	CPrivateMarketSellWindow::GetItemCount ()
{
	//	최소 1개, 의도된 것임
	if ( !m_bCOUNTABLE ) return 1;

	const CString& strTemp = m_pEditBoxMan->GetEditString ( PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT_EDIT );
	return static_cast<DWORD>( atoi ( strTemp ) );
}

const DWORD CPrivateMarketSellWindow::GetItemCost ()
{	
	const CString& strTemp = m_pEditBoxMan->GetEditString ( PRIVATE_MARKET_SELL_WINDOW_MONEY_EDIT );
	return static_cast<DWORD>( atoi ( strTemp ) );
}

void CPrivateMarketSellWindow::GetItemIndex ( WORD& wPosX, WORD& wPosY )
{
	wPosX = m_wPosX;
	wPosY = m_wPosY;
}

void CPrivateMarketSellWindow::SetItemIndex ( const WORD wPosX, const WORD wPosY )
{
	m_wPosX = wPosX;
	m_wPosY = wPosY;
}
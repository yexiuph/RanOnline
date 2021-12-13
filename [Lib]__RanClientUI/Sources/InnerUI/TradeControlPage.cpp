#include "pch.h"
#include "TradeControlPage.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "TradePage.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "UITextControl.h"
#include "DxGlobalStage.h"
#include "GLInventory.h"
#include "GLGaeaClient.h"
#include "d3dfont.h"
#include "InnerInterface.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CTradeControlPage::nOUTOFRANGE = -1;

CTradeControlPage::CTradeControlPage () :
	m_pMoney ( NULL ),
    m_pPlayerName ( NULL )
{
}

CTradeControlPage::~CTradeControlPage ()
{
}

void CTradeControlPage::CreateSubControl ( BOOL bMyPage )
{
	m_bMyPage = bMyPage;

	CreateControl ( "TRADE_CONTROL_PAGE_NAME_BACK" );
	//	뒷 배경은 컨트롤로 제작할만큼 중요하지 않은듯하여,
	//	두개로 잘라서 그냥 뿌리도록 합니다.
	CreateControl ( "TRADE_CONTROL_PAGE_MONEY_BACK_LEFT" );
	CreateControl ( "TRADE_CONTROL_PAGE_MONEY_BACK_RIGHT" );

	CreateControl ( "TRADE_MONEY_BUTTON" );

//	CBasicButton* pButton = NULL;
//	pButton = CreateFlipButton ( "TRADE_MONEY_BUTTON", "TRADE_MONEY_BUTTON_F", TRADE_MONEY_BUTTON, CBasicButton::CLICK_FLIP );
//	pButton->CreateMouseOver ( "TRADE_MONEY_BUTTON_F" );
//	pButton->SetUseDynamic ( TRUE );

	{
		m_pOK = CreateTextButton ( "TRADE_OK_BUTTON", TRADE_OK_BUTTON, (char*)ID2GAMEWORD ( "TRADE_OKCANCEL_BUTTON", 0 ) );
		m_pOK->SetFlip ( TRUE );

		if ( m_bMyPage )
		{
			m_pCANCEL = CreateTextButton ( "TRADE_CANCEL_BUTTON", TRADE_CANCEL_BUTTON, (char*)ID2GAMEWORD ( "TRADE_OKCANCEL_BUTTON", 1 ) );
			m_pCANCEL->SetFlip ( TRUE );
		}
	}

	CTradePage*	pTradePage = new CTradePage;
	pTradePage->CreateSub ( this, "TRADE_CONTROL_PAGE_SLOT_PAGE", UI_FLAG_DEFAULT, TRADE_SLOT_PAGE );
	pTradePage->CreateSubControl ();
	RegisterControl ( pTradePage );
	m_pPage = pTradePage;

	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont8Shadow = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pPlayerName = CreateStaticControl ( "TRADE_NAME_BOX", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMoney = CreateStaticControl ( "TRADE_MONEY_BOX", pFont8Shadow, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMoney->SetOneLineText ( "0" );
}

CUIControl*	CTradeControlPage::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextButton*	 CTradeControlPage::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::RADIO_FLIP, szText );
	pTextButton->SetFlip ( FALSE );
	RegisterControl ( pTextButton );
	return pTextButton;
}


CBasicButton* CTradeControlPage::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID, WORD wFlipType )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, wFlipType );
	RegisterControl ( pButton );
	return pButton;
}


CBasicTextBox* CTradeControlPage::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CTradeControlPage::SetPlayerName ( CString strName )
{
	if ( m_pPlayerName ) m_pPlayerName->SetOneLineText ( strName );
}

void CTradeControlPage::SetMoney ( LONGLONG lnMoney )
{
	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( lnMoney, 3, "," );
	if ( m_pMoney ) m_pMoney->SetOneLineText ( strTemp );
}


void CTradeControlPage::ApplyButtonUpdate ()
{
	if ( m_bMyPage )
	{
		if ( GLTradeClient::GetInstance().GetMyTrade().IsAgreeAble() )
		{
			m_pOK->SetVisibleSingle ( TRUE );

			BOOL bAgree = GLTradeClient::GetInstance().GetMyTrade().GetAgree();
			{
				m_pOK->SetFlip ( !bAgree );
				m_pPage->SetAllFlipItem ( bAgree );
			}
		}
		else
		{
			m_pOK->SetVisibleSingle ( FALSE );
			m_pPage->SetAllFlipItem ( FALSE );
		}
	}
	else
	{
		if ( GLTradeClient::GetInstance().GetTarTrade().IsAgreeAble() )
		{
			m_pOK->SetVisibleSingle ( TRUE );

			BOOL bAgree = GLTradeClient::GetInstance().GetTarTrade().GetAgree();
			m_pOK->SetFlip ( !bAgree );
			m_pPage->SetAllFlipItem ( bAgree );
		}
		else
		{
			m_pOK->SetVisibleSingle ( FALSE );
			m_pPage->SetAllFlipItem ( FALSE );
		}

	}
}

void CTradeControlPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	ApplyButtonUpdate ();

	SetItemIndex ( nOUTOFRANGE, nOUTOFRANGE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime,bFirstControl );

	if ( m_bMyPage )
	{
		GLTrade& MyTrade = GLTradeClient::GetInstance().GetMyTrade ();
		SetMoney ( MyTrade.GetMoney () );

		GLInventory& ref_ItemData = GLTradeClient::GetInstance().GetMyTradeBox();		
		if ( m_pPage ) m_pPage->LoadItemPage ( ref_ItemData );
	}
	else
	{
		GLTrade& TarTrade = GLTradeClient::GetInstance().GetTarTrade ();
		SetMoney ( TarTrade.GetMoney () );

		GLInventory& ref_ItemData = GLTradeClient::GetInstance().GetTarTradeBox();
		if ( m_pPage ) m_pPage->LoadItemPage ( ref_ItemData );
	}
}

void CTradeControlPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case TRADE_SLOT_PAGE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nPosX, nPosY;
				m_pPage->GetItemIndex ( &nPosX, &nPosY );

				CDebugSet::ToView ( 1, 1, "인벤 : %d %d", nPosX, nPosY );

				SetItemIndex ( nPosX, nPosY );

				if ( nPosX < 0 || nPosY < 0 ) return;

////				if ( CUIMan::GetFocusControl () == this )
//				{
//					SINVENITEM sInvenItem = m_pPage->GetItem ( nPosX, nPosY );
//					if ( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL () )
//					{
//						m_pItemInfo->LoadInvenItemInfo ( sInvenItem.sItemCustom, FALSE, FALSE );
//						m_pItemInfo->SetVisibleSingle ( TRUE );
//					}
//				}

				if ( m_bMyPage )
				{
					if ( dwMsg & UIMSG_LB_UP )
					{
						GLGaeaClient::GetInstance().GetCharacter()->ReqTradeBoxTo ( nPosX, nPosY );
					}
				}
			}
		}
		break;

	case TRADE_OK_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			if ( m_bMyPage )
			{
				//	승인 버튼 누름
				//
				GLGaeaClient::GetInstance().GetCharacter()->ReqTradeAgree();
			}
		}
		break;

	case TRADE_CANCEL_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			if ( m_bMyPage )
			{
				CInnerInterface::GetInstance().SetTradeWindowCloseReq ();
			}
		}
		break;
	}
}


void CTradeControlPage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CTradeControlPage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

SINVENITEM& CTradeControlPage::GetItem ( int nPosX, int nPosY )
{
	return m_pPage->GetItem ( nPosX, nPosY );
}
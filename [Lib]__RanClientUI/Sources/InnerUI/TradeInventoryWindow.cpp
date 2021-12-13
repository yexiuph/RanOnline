#include "pch.h"
#include "TradeInventoryWindow.h"
#include "TradeInventoryPage.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "ModalCallerID.h"
#include "ModalWindow.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "UITextControl.h"
#include "GLGaeaClient.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "GameTextControl.h"
#include "InnerInterface.h"
#include "d3dfont.h"
#include "GLItemMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTradeInventoryWindow::CTradeInventoryWindow () :
	m_pPage ( NULL ),
	m_pMoneyTextBox ( NULL ),
	m_nONE_VIEW_SLOT ( 4 )
{
}

CTradeInventoryWindow::~CTradeInventoryWindow ()
{
}

void CTradeInventoryWindow::CreateSubControl ()
{
	CTradeInventoryPage* pTradeInventoryPage = new CTradeInventoryPage;
	pTradeInventoryPage->CreateSub ( this, "TRADEINVENTORY_PAGE", UI_FLAG_YSIZE, TRADEINVENTORY_PAGE );
	pTradeInventoryPage->CreateSubControl ();
	RegisterControl ( pTradeInventoryPage );
	m_pPage = pTradeInventoryPage;

	CUIControl* pControl = CreateControl ( "TRADEINVENTORY_MONEY_BACK" );
	pControl->SetAlignFlag ( UI_FLAG_BOTTOM );

	CBasicButton* pButton = CreateFlipButton ( "TRADEINVENTORY_MONEY_BUTTON", "TRADEINVENTORY_MONEY_BUTTON_F", TRADEINVENTORY_MONEY_BUTTON, CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver ( "TRADEINVENTORY_MONEY_BUTTON_F" );
	pButton->SetUseDynamic ( TRUE );
	pButton->SetAlignFlag ( UI_FLAG_BOTTOM );
//	CreateFlipButton ( "TRADEINVENTORY_PLUS_MONEY_BUTTON", "TRADEINVENTORY_PLUS_MONEY_BUTTON_F", TRADEINVENTORY_PLUS_MONEY_BUTTON, CBasicButton::CLICK_FLIP );
//	CreateFlipButton ( "TRADEINVENTORY_MINUS_MONEY_BUTTON", "TRADEINVENTORY_MINUS_MONEY_BUTTON_F", TRADEINVENTORY_MINUS_MONEY_BUTTON, CBasicButton::CLICK_FLIP );

	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pMoneyTextBox = CreateStaticControl ( "TRADEINVENTORY_MONEYTEXT", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMoneyTextBox->SetAlignFlag ( UI_FLAG_BOTTOM );
}

void CTradeInventoryWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_bTRADE_MODE = (CInnerInterface::GetInstance().IsVisibleGroup(TRADE_WINDOW))?true:false;

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_bTRADE_MODE )
	{
		GLInventory& ref_ItemData = GLTradeClient::GetInstance().GetMyTradeBox();
		m_pPage->UpdateFlipItem ( ref_ItemData );
	}
	else
	{
		m_pPage->ResetAllFlipItem ();
//		GLInventory& ref_ItemData = GLGaeaClient::GetInstance().GetCharacter ()->m_sPMarket.GetInven();
//		m_pPage->UpdateFlipItem ( ref_ItemData );
	}

	GLCHARLOGIC& ref_CharData = GLGaeaClient::GetInstance().GetCharacterLogic ();
	SetMoney ( ref_CharData.m_lnMoney );

	GLInventory& ref_Inventory = ref_CharData.m_cInventory;	
	m_pPage->LoadItemPage ( ref_Inventory );
}

void CTradeInventoryWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	//	의도적으로 윈도우 이동을 막음
//	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case TRADEINVENTORY_PAGE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nPosX, nPosY;
				m_pPage->GetItemIndex ( &nPosX, &nPosY );

				CDebugSet::ToView ( 1, 1, "인벤 : %d %d", nPosX, nPosY );				

				if ( nPosX < 0 || nPosY < 0 ) return;

//				if ( CUIMan::GetFocusControl () == this )
//				{
					SINVENITEM& sInvenItem = m_pPage->GetItem ( nPosX, nPosY );
					if ( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL () )
					{
						CInnerInterface::GetInstance().SHOW_ITEM_INFO ( sInvenItem.sItemCustom, FALSE, FALSE, FALSE, sInvenItem.wPosX, sInvenItem.wPosY );
					}
//				}

				if ( dwMsg & UIMSG_LB_UP )
				{   
					if ( m_bTRADE_MODE )	//	거래모드
					{
						GLGaeaClient::GetInstance().GetCharacter()->ReqInvenTo ( nPosX, nPosY );
					}
					else					//	상점모드
					{
						if ( m_bOPENER )
						{
							GLPrivateMarket &sPMarket = GLGaeaClient::GetInstance().GetCharacter()->m_sPMarket;
							if ( sPMarket.IsOpen () ) return ;

							if ( sInvenItem.sItemCustom.sNativeID == NATIVEID_NULL () ) return ;
							const bool bCOUNTABLE = IsTurnITEM ( sInvenItem.sItemCustom );
							WORD wPosX = static_cast<WORD>(nPosX);
							WORD wPosY = static_cast<WORD>(nPosY);

							CInnerInterface::GetInstance().MODAL_PRIVATE_MARKET_SELLITEM ( bCOUNTABLE, wPosX, wPosY );
						}
						else
						{
//							GLGaeaClient::GetInstance().GetCharacter()->ReqInvenTo ( nPosX, nPosY );
						}
					}
				}
				if ( dwMsg & UIMSG_RB_UP )
				{
//					GLGaeaClient::GetInstance().GetCharacter()->ReqInvenDrug ( nPosX, nPosY );
				}
			}
		}
		break;

	case TRADEINVENTORY_MONEY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//	거래(TRADE)일 경우에만 모달이 뜬다.
				if ( m_bTRADE_MODE ) DoModal ( ID2GAMEINTEXT("TRADE_MONEY"), MODAL_INPUT, EDITBOX_MONEY, MODAL_TRADEMONEY );
			}
		}
		break;

	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				if ( m_bTRADE_MODE )	CInnerInterface::GetInstance().SetTradeWindowCloseReq ();
				else
				{
					bool bOPENER;
					DWORD dwGaeaID;
					CInnerInterface::GetInstance().GetPrivateMarketInfo ( bOPENER, dwGaeaID );
					if ( bOPENER )
					{
						DoModal ( ID2GAMEINTEXT("PRIVATE_MARKET_QUIT"), MODAL_QUESTION, YESNO, MODAL_PRIVATE_MARKET_QUIT );
					}
					else
					{
						CInnerInterface::GetInstance().SetPrivateMarketClose ();
					}
				}
			}
		}
		break;
	}
}


void CTradeInventoryWindow::SetMoney ( LONGLONG Money )
{
	//	돈자리수
	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( Money, 3, "," );
	m_pMoneyTextBox->SetOneLineText ( strTemp );
}

HRESULT CTradeInventoryWindow::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIWindowEx::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = CInnerInterface::GetInstance().GetResolution ();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	const UIRECT& rcWindow = GetGlobalPos ();

	D3DXVECTOR2 vPos;
	vPos.x = ((X_RES) / 2.0f);
	vPos.y = (Y_RES - rcWindow.sizeY) / 2.0f;
	SetGlobalPos ( vPos );

	return S_OK;
}

bool CTradeInventoryWindow::IsTurnITEM ( SITEMCUSTOM& sItemCustom )
{
	SITEM* pItemData = GLItemMan::GetInstance().GetItem ( sItemCustom.sNativeID );
	if ( !pItemData )
	{
		GASSERT ( 0 && "아이템 정보를 찾을 수 없습니다." );
		return false;
	}

	const WORD& wPileNum = pItemData->sDrugOp.wPileNum;
	if ( wPileNum > 1 )
	{
		const WORD& wTurnNum = sItemCustom.wTurnNum;        
		if ( wTurnNum > 1 ) return true;
	}

	return false;
}

void	CTradeInventoryWindow::SetOPENER ( bool bOPENER )
{
	m_bOPENER = bOPENER;
}

void	CTradeInventoryWindow::SetOneViewSlot ( const int& nONE_VIEW_SLOT )
{
	CUIControl TempControl;
	TempControl.Create ( 1, "TRADEINVENTORYSLOT_ITEM_SLOT0" );

	CUIControl TempControl2;
	TempControl2.Create ( 2, "TRADEINVENTORYSLOT_ITEM_SLOT1" );

	const UIRECT& rcTempGlobalPos = TempControl.GetGlobalPos ();
	D3DXVECTOR2 vONE_SLOT_SIZE ( rcTempGlobalPos.sizeX, rcTempGlobalPos.sizeY );

	const UIRECT& rcTempGlobalPos2 = TempControl2.GetGlobalPos ();

	const float fSlotGap = rcTempGlobalPos2.top - rcTempGlobalPos.bottom;

	const float fOldSizeY = (vONE_SLOT_SIZE.y+fSlotGap) * m_nONE_VIEW_SLOT;
	const float fNewSizeY = (vONE_SLOT_SIZE.y+fSlotGap) * nONE_VIEW_SLOT;
    const float fDIFF_SIZE_Y = fNewSizeY - fOldSizeY;

	const UIRECT& rcGlobalPos = GetGlobalPos ();
	UIRECT rcNewGlobalPos ( rcGlobalPos.left, rcGlobalPos.top,
		rcGlobalPos.sizeX, rcGlobalPos.sizeY + fDIFF_SIZE_Y );

	WORD wFlagAlignBACK = GetAlignFlag ();
	SetAlignFlag ( UI_FLAG_YSIZE );
	
    AlignSubControl ( rcGlobalPos, rcNewGlobalPos );
	SetGlobalPos ( rcNewGlobalPos );
	
	SetAlignFlag ( wFlagAlignBACK );

	m_pPage->SetOneViewSlot ( nONE_VIEW_SLOT );
	m_nONE_VIEW_SLOT = nONE_VIEW_SLOT;
}

const int& CTradeInventoryWindow::GetOneViewSlot () const
{
	return m_nONE_VIEW_SLOT;
}
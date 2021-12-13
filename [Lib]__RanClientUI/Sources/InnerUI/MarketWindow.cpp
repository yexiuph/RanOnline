#include "pch.h"
#include "MarketWindow.h"
#include "MarketPage.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"
#include "GLItemMan.h"
#include "ModalWindow.h"
#include "ModalCallerID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	int	CMarketWindow::nOUTOFRANGE = -1;

CMarketWindow::CMarketWindow () :
	m_nPageIndex ( nOUTOFRANGE )
{
	memset ( m_pPage, 0, sizeof ( CMarketPage* ) * nMAXPAGE );
	memset ( m_pPageButton, 0, sizeof ( CBasicTextButton* ) * nMAXPAGE );	
}

CMarketWindow::~CMarketWindow ()
{
}

void CMarketWindow::CreateSubControl ()
{
	CString strButtonKeyword[nMAXPAGE] = 
	{
		"MARKET_WINDOW_PAGE_BUTTON0",
		"MARKET_WINDOW_PAGE_BUTTON1",
		"MARKET_WINDOW_PAGE_BUTTON2"
	};

	for ( int i = 0; i < nMAXPAGE; i++ )
	{
		m_pPage[i] = CreateMarketPage ( MARKET_PAGE0 + i );		
		m_pPageButton[i] = CreateTextButton ( strButtonKeyword[i].GetString (), MARKET_WINDOW_PAGE_BUTTON0 + i, (char*)ID2GAMEWORD ( "MARKET_WINDOW_PAGE_BUTTON", i ) );
	}

	SetVisiblePage ( MARKET_PAGE0 );
}

CMarketPage* CMarketWindow::CreateMarketPage ( UIGUID ControlID )
{
	CMarketPage* pPage = new CMarketPage;
	pPage->CreateSub ( this, "MARKET_PAGE", UI_FLAG_DEFAULT, ControlID );
	pPage->CreateSubControl ();
	RegisterControl ( pPage );
	return pPage;
}

CBasicTextButton*  CMarketWindow::CreateTextButton ( const char* szButton, UIGUID ControlID, const char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE19;
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::RADIO_FLIP, szText );
	RegisterControl ( pTextButton );
	return pTextButton;
}

BOOL CMarketWindow::IsUsingMaketPage( int nPage )
{
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sMarketID );
	GLInventory& ref_Inventory = *pCrowData->GetSaleInven(nPage);

	if( ref_Inventory.GetNumItems() > 0 )
		return true;
	else
		return false;
}

void CMarketWindow::SetVisiblePage ( int nPage )
{
	if ( nPage < MARKET_PAGE0 || MARKET_PAGE2 < nPage )
	{
		GASSERT ( 0 && "영역을 넘어서는 페이지입니다." );
		return ;
	}

	{	//	리셋
		for ( int i = 0; i < nMAXPAGE; i++ )
		{
			m_pPage[i]->SetVisibleSingle ( FALSE );
			m_pPageButton[i]->SetFlip ( FALSE );
		}
	}

	m_nPageIndex = nPage - MARKET_PAGE0;
	m_pPage[m_nPageIndex]->SetVisibleSingle ( TRUE );
	m_pPageButton[m_nPageIndex]->SetFlip ( TRUE );
}

void CMarketWindow::InitMarket ( SNATIVEID sNativeID )
{
	m_sMarketID = sNativeID;

	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sMarketID );
	if ( pCrowData )
	{
		for ( int i = 0; i < nMAXPAGE; i++ )
		{
			GLInventory& ref_Inventory = *pCrowData->GetSaleInven(i);
			m_pPage[i]->UnLoadItemPage ();
			m_pPage[i]->LoadItemPage ( ref_Inventory );

			//	추후에 수정
			CString strTabText = pCrowData->GetSaleType(i).c_str();
			m_pPageButton[i]->SetOneLineText ( strTabText );
		}
	}	

	SetVisiblePage ( MARKET_PAGE0 );
}

void CMarketWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );


    if( ControlID == ET_CONTROL_TITLE || ET_CONTROL_TITLE_F == ControlID )
	{
		if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
		{
			CInnerInterface::GetInstance().SetDefaultPosInterface( MARKET_WINDOW );
			return ;
		}
	}	
	if ( ControlID == ET_CONTROL_BUTTON )
	{
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			CInnerInterface::GetInstance().SetMarketWindowClose ();
			return ;
		}
	}

	if ( MARKET_WINDOW_PAGE_BUTTON0 <= ControlID && ControlID < MARKET_WINDOW_PAGE_BUTTON_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			int nIndex = ControlID - MARKET_WINDOW_PAGE_BUTTON0;

			if ( dwMsg & UIMSG_LB_DOWN )
			{
				if ( IsUsingMaketPage( nIndex ) )
				{
					SetVisiblePage ( MARKET_PAGE0 + nIndex );
				}
				else
				{	
					return;
				}

			}
		}
	}
	else if ( MARKET_PAGE0 <= ControlID && ControlID < MARKET_PAGE_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			int nPageIndex = ControlID - MARKET_PAGE0;

			//	NOTE
			//		손에 들고 있는 아이템 처리
			if ( dwMsg & UIMSG_LB_UP || dwMsg & UIMSG_RB_UP )
			{
				SNATIVEID sNativeID = GLGaeaClient::GetInstance().GetCharacter()->GET_HOLD_ITEM().sNativeID;
				if ( sNativeID != NATIVEID_NULL () )
				{
					//	NOTE
					//		물건 팔기
					SITEM* pItemData = GLItemMan::GetInstance().GetItem ( sNativeID );
					if ( pItemData )
					{
						const std::string& strItemName = pItemData->GetName();
						CString strCombine = CInnerInterface::GetInstance().MakeString ( ID2GAMEINTEXT("SELL_ITEM_CONFIRM"), strItemName.c_str() );
						DoModal ( strCombine, MODAL_QUESTION, YESNO, MODAL_SELL_ITEM );
					}
					else
					{
						GASSERT ( 0 && "아이템을 찾을 수 없습니다." );
					}

					return ;
				}
			}

			int nPosX, nPosY;
			m_pPage[nPageIndex]->GetItemIndex ( &nPosX, &nPosY );			
			SetItemIndex ( nPosX, nPosY );
			if ( nPosX < 0 || nPosY < 0 ) return ;

//			if ( CUIMan::GetFocusControl () == this )
			SINVENITEM sInvenItem = m_pPage[nPageIndex]->GetItem ( nPosX, nPosY );
			{				
				if ( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL () )
				{
					CInnerInterface::GetInstance().SHOW_ITEM_INFO ( sInvenItem.sItemCustom, TRUE, TRUE, FALSE, sInvenItem.wPosX, sInvenItem.wPosY, m_sMarketID );
				}
			}


			//	NOTE
			//		아이템 바로 사기
			if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_PRESSED )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					SNATIVEID sNativeID = GLGaeaClient::GetInstance().GetCharacter()->GET_HOLD_ITEM().sNativeID;
					if ( sNativeID != NATIVEID_NULL () ) return ;

					GLGaeaClient::GetInstance().GetCharacter()->ReqNpcTo ( m_sMarketID, m_nPageIndex, nPosX, nPosY );
					return ;
				}
			}

			if ( dwMsg & UIMSG_LB_UP )
			{
				SNATIVEID sNativeID = GLGaeaClient::GetInstance().GetCharacter()->GET_HOLD_ITEM().sNativeID;
				if ( sNativeID != NATIVEID_NULL () ) return ;

				SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sInvenItem.sItemCustom.sNativeID );
				if ( pITEM )
				{
					// Note : 겹침이 가능하고 한개씩 파는 아이템의 경우 구입 수량 입력. ( 화살묶음, 부적묶음. )
					if( pITEM->ISPILE() && (pITEM->GETAPPLYNUM()==1) )
					{
						DoModal ( ID2GAMEINTEXT ( "BUY_PILE_ITEM" ), MODAL_INPUT, EDITBOX_NUMBER, MODAL_PILE_ITEM );
						return ;
					}
					else
					{
						//	NOTE
						//		물건 사기							
						const std::string& strItemName = pITEM->GetName();
						CString strCombine = CInnerInterface::GetInstance().MakeString ( ID2GAMEINTEXT("BUY_ITEM_CONFIRM"), strItemName.c_str() );
						DoModal ( strCombine, MODAL_QUESTION, YESNO, MODAL_BUY_ITEM );							
						return ;
					}
				}
			}
		}
	}
}

void CMarketWindow::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

void CMarketWindow::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

int	 CMarketWindow::GetPageIndex ()
{
	return m_nPageIndex;
}

SNATIVEID	CMarketWindow::GetMarketID ()
{
	return m_sMarketID;
}

SINVENITEM&	CMarketWindow::GetItem ( int nPosX, int nPosY )
{
	return m_pPage[m_nPageIndex]->GetItem ( nPosX, nPosY );
}
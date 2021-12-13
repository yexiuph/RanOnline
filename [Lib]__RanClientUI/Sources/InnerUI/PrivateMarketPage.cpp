#include "pch.h"

#include "PrivateMarketPage.h"
#include "ItemSlotPrivateMarket.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "GLGaeaClient.h"
#include "GLItemMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CPrivateMarketPage::nSTARTLINE = 0;
const int CPrivateMarketPage::nOUTOFRANGE = -1;

CPrivateMarketPage::CPrivateMarketPage () :
	m_pScrollBar ( NULL ),
	m_nCurPos( -1 )
{
	memset ( m_pItemSlotArray, 0, sizeof ( CItemSlot* ) * PRIVATE_MARKET_INVEN_Y );
	memset ( m_pItemSlotArrayDummy, 0, sizeof ( CUIControl* ) * nMAX_ONE_VIEW_SLOT );
}

CPrivateMarketPage::~CPrivateMarketPage ()
{
}

CUIControl*	CPrivateMarketPage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void CPrivateMarketPage::CreateSubControl ()
{
	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"ITEMBANK_ITEM_SLOT0",
		"ITEMBANK_ITEM_SLOT1",
		"ITEMBANK_ITEM_SLOT2",
		"ITEMBANK_ITEM_SLOT3",
		//"ITEMBANK_ITEM_SLOT4",
		//"ITEMBANK_ITEM_SLOT5",
		//"ITEMBANK_ITEM_SLOT6",
		//"ITEMBANK_ITEM_SLOT7",
	};

	{	//	더미 슬롯
		for ( int i = 0; i < nMAX_ONE_VIEW_SLOT; i++ )
		{
			m_pItemSlotArrayDummy[i] = CreateControl ( strSlotKeyword[i].GetString () );
		}
	}

	{	//	실제 데이타
		for ( int i = 0; i < PRIVATE_MARKET_INVEN_Y; i++ )
		{
			m_pItemSlotArray[i] = CreateItemSlot ( strSlotKeyword[0], ITEM_SLOT0 + i );
		}
	}

	{	//	스크롤바
		m_pScrollBar = new CBasicScrollBarEx;
		m_pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		m_pScrollBar->CreateBaseScrollBar ( "PRIVATE_MARKET_PAGE_SCROLL" );
		m_pScrollBar->GetThumbFrame()->SetState ( PRIVATE_MARKET_INVEN_Y, nMAX_ONE_VIEW_SLOT );
		RegisterControl ( m_pScrollBar );
	}

	SetItemSlotRender ( nSTARTLINE, nMAX_ONE_VIEW_SLOT );
}

void CPrivateMarketPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE, nOUTOFRANGE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pScrollBar )
	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame ();
		int nTotal = pThumbFrame->GetTotal ();

		if ( nTotal <= nMAX_ONE_VIEW_SLOT ) return ;

		const int nViewPerPage = pThumbFrame->GetViewPerPage ();
		if ( nViewPerPage < nTotal )
		{
			int nCurPos = nSTARTLINE;
			const int nMovableLine = nTotal - nViewPerPage;
			float fPercent = pThumbFrame->GetPercent ();

			nCurPos = (int)floor(fPercent * nMovableLine);
			if ( nCurPos < nSTARTLINE ) nCurPos = nSTARTLINE;

			if ( m_nCurPos == nCurPos ) return;

			m_nCurPos = nCurPos;

			ResetAllItemSlotRender ( nTotal );
			SetItemSlotRender ( nCurPos, nCurPos + nViewPerPage );
		}
	}
}

void CPrivateMarketPage::ResetAllItemSlotRender ( int nTotal )
{
	if ( nTotal < 0 ) return ;

	for ( int i = 0; i < nTotal; i++ )
	{
		CItemSlotPrivateMarket* pItemSlot = m_pItemSlotArray[i];
		if ( pItemSlot )
		{
			pItemSlot->SetVisibleSingle ( FALSE );
		}
	}
}

void CPrivateMarketPage::SetItemSlotRender ( int nStartIndex, int nTotal )
{
	if ( nTotal < 0 ) return ;

	const UIRECT& rcParentPos = GetGlobalPos ();

	for ( int i = nStartIndex; i < nTotal; i++ )
	{
		CItemSlotPrivateMarket* pItemSlot = m_pItemSlotArray[i];
		if ( pItemSlot )
		{
			int nAbsoluteIndex = i - nStartIndex;

			CUIControl* pDummyControl = m_pItemSlotArrayDummy[nAbsoluteIndex];			
			const UIRECT& rcSlotPos = pDummyControl->GetGlobalPos ();
			const UIRECT& rcSlotLocalPos = pDummyControl->GetLocalPos ();

			pItemSlot->SetLocalPos ( rcSlotLocalPos );
			pItemSlot->SetGlobalPos ( rcSlotPos );

			pItemSlot->SetVisibleSingle ( TRUE );
		}
	}
}

CItemSlotPrivateMarket* CPrivateMarketPage::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlotPrivateMarket* pItemSlot = new CItemSlotPrivateMarket;
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( PRIVATE_MARKET_INVEN_X, TRUE );
	pItemSlot->CreateFlipImage ( PRIVATE_MARKET_INVEN_X );
	RegisterControl ( pItemSlot );

	return pItemSlot;
}

void CPrivateMarketPage::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlotArray[nPosY]->SetItemImage ( nPosX, ref_InvenItem );
}

void CPrivateMarketPage::DoFlipItemPage ( GLPrivateMarket &sPMarket )
{
	for ( WORD wPosY = 0; wPosY < PRIVATE_MARKET_INVEN_Y; ++wPosY )
	{
		for ( WORD wPosX = 0; wPosX < PRIVATE_MARKET_INVEN_X; ++wPosX )
		{
			DoFlipItem ( wPosX, wPosY, FALSE );

			const SSALEITEM *pSALE = sPMarket.GetItem ( SNATIVEID(wPosX,wPosY) ); 
			if ( pSALE )
			{
				if ( pSALE->bSOLD )
					DoFlipItem ( wPosX, wPosY, TRUE );
			}
		}
	}
}

void CPrivateMarketPage::LoadItemPage ( GLInventory &ItemData, bool bOPENER )
{
	GLInventory::CELL_MAP *pItemList = ItemData.GetItemList();
	
	if ( pItemList->size () > (PRIVATE_MARKET_INVEN_Y * PRIVATE_MARKET_INVEN_X) )
	{
		GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return ;
	}


	{	//	삭제

		GLInventory::CELL_MAP_ITER iter = pItemList->begin();
		GLInventory::CELL_MAP_ITER iter_end = pItemList->end();

		for ( WORD y = 0; y < PRIVATE_MARKET_INVEN_Y; y++ )
		{
			for ( WORD x = 0; x < PRIVATE_MARKET_INVEN_X; x++ )
			{
				SINVENITEM& ref_InvenItem = GetItem ( x, y );
				if ( ref_InvenItem.sItemCustom.sNativeID != NATIVEID_NULL() )
				{
					GLInventory::CELL_KEY foundkey ( x, y );
					GLInventory::CELL_MAP_ITER found = pItemList->find ( foundkey );
					if ( found == iter_end )
					{
						UnLoadItem ( x, y );
					}
				}
			}
		}
	}

	{	//	등록
		GLInventory::CELL_MAP_ITER iter = pItemList->begin();
		GLInventory::CELL_MAP_ITER iter_end = pItemList->end();

		for ( ; iter!=iter_end; ++iter )
		{
			SINVENITEM* pInvenItem = (*iter).second;
			SINVENITEM& ref_InvenItemOld = GetItem ( pInvenItem->wPosX, pInvenItem->wPosY );
			if ( *pInvenItem != ref_InvenItemOld )
			{
				LoadItem ( *pInvenItem );
			}	
		}
	}
}

void CPrivateMarketPage::DoFlipItem ( WORD wPosX, WORD wPosY, BOOL bFlip )
{
	m_pItemSlotArray[wPosY]->SetFlipItem ( wPosX, bFlip );
}

BOOL CPrivateMarketPage::IsFlipItem ( WORD wPosX, WORD wPosY )
{
	return m_pItemSlotArray[wPosY]->IsFlipItem ( wPosX );
}


void CPrivateMarketPage::UnLoadItemPage ()
{
	for ( int y = 0; y < PRIVATE_MARKET_INVEN_Y; y++ )
	{
		for ( int x = 0; x < PRIVATE_MARKET_INVEN_X; x++ )
		{
			UnLoadItem ( x, y );
		}
	}
}

SINVENITEM& CPrivateMarketPage::GetItem ( int nPosX, int nPosY )
{
	return m_pItemSlotArray[nPosY]->GetItemImage ( nPosX );
}

void CPrivateMarketPage::UnLoadItem ( int nPosX, int nPosY )
{
	m_pItemSlotArray[nPosY]->ResetItemImage ( nPosX );
}


void CPrivateMarketPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( ITEM_SLOT0 <= ControlID && ControlID < ITEM_SLOT_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			const int nPosY = ControlID - ITEM_SLOT0;
			CItemSlotPrivateMarket* pSlot = m_pItemSlotArray[nPosY];
			const int nPosX = pSlot->GetItemIndex ();

			if ( nPosY < 0 || nPosX < 0 ) return ;

			SetItemIndex ( nPosX, nPosY );
		}
	}
}

void CPrivateMarketPage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CPrivateMarketPage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}
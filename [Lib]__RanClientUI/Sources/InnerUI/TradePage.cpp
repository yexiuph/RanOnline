#include "pch.h"
#include "TradePage.h"
#include "ItemSlotEx.h"
#include "GLItemMan.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CTradePage::nOUTOFRANGE = -1;

CTradePage::CTradePage ()
{
	memset ( m_pItemSlotArray, 0, sizeof ( CItemSlot* ) * EMTRADE_INVEN_Y );
}

CTradePage::~CTradePage ()
{
}

CUIControl*	CTradePage::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void CTradePage::CreateSubControl ()
{
	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"TRADESLOT_ITEM_SLOT0",
		"TRADESLOT_ITEM_SLOT1",
	};

	{	//	실제 데이타
		for ( int i = 0; i < EMTRADE_INVEN_Y; i++ )
		{
			m_pItemSlotArray[i] = CreateItemSlot ( strSlotKeyword[i], ITEM_SLOT0 + i );
		}
	}
}

CItemSlotEx* CTradePage::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlotEx* pItemSlot = new CItemSlotEx;
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubTrade ( EMTRADE_INVEN_X );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}


void CTradePage::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlotArray[nPosY]->SetItemImage ( nPosX, ref_InvenItem );
}

void CTradePage::LoadItemPage ( GLInventory &ItemData )
{
	GLInventory::CELL_MAP *pItemList = ItemData.GetItemList();
	
	if ( pItemList->size () > (EMTRADE_INVEN_Y * EMTRADE_INVEN_X) )
	{
		GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return ;
	}


	{	//	삭제

		GLInventory::CELL_MAP_ITER iter = pItemList->begin();
		GLInventory::CELL_MAP_ITER iter_end = pItemList->end();

		for ( WORD y = 0; y < EMTRADE_INVEN_Y; y++ )
		{
			for ( WORD x = 0; x < EMTRADE_INVEN_X; x++ )
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


void CTradePage::UnLoadItemPage ()
{
	for ( int y = 0; y < EMTRADE_INVEN_Y; y++ )
	{
		for ( int x = 0; x < EMTRADE_INVEN_X; x++ )
		{
			UnLoadItem ( x, y );
		}
	}
}

SINVENITEM& CTradePage::GetItem ( int nPosX, int nPosY )
{
	return m_pItemSlotArray[nPosY]->GetItemImage ( nPosX );
}

void CTradePage::UnLoadItem ( int nPosX, int nPosY )
{
	m_pItemSlotArray[nPosY]->ResetItemImage ( nPosX );
}

void CTradePage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( ITEM_SLOT0 <= ControlID && ControlID < ITEM_SLOT_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			const int nPosY = ControlID - ITEM_SLOT0;
			CItemSlot* pSlot = m_pItemSlotArray[nPosY];
			const int nPosX = pSlot->GetItemIndex ();

			if ( nPosY < 0 || nPosX < 0 ) return ;

			SetItemIndex ( nPosX, nPosY );
		}
	}
}

void CTradePage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CTradePage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

void CTradePage::SetAllFlipItem ( BOOL bFlip )
{
	for ( int y = 0; y < EMTRADE_INVEN_Y; y++ )
	{
		for ( int x = 0; x < EMTRADE_INVEN_X; x++ )
		{			
			if ( bFlip )
			{
				SINVENITEM& ref_sInvenItem = m_pItemSlotArray[y]->GetItemImage ( x );
				if ( ref_sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL() )
				{
					m_pItemSlotArray[y]->SetFlipItem ( x, TRUE );
				}
			}
			else
			{
				m_pItemSlotArray[y]->SetFlipItem ( x, FALSE );
			}
		}
	}
}

void CTradePage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE, nOUTOFRANGE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}
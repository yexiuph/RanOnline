#include "pch.h"
#include "ItemSlotEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CItemSlotEx::CItemSlotEx ()
{
	SecureZeroMemory ( m_pFlipImage, sizeof( m_pFlipImage ) );
}

CItemSlotEx::~CItemSlotEx ()
{
}

void CItemSlotEx::CreateSubTradeInventory ( int nMaxColumn )
{
	CItemSlot::CreateSubControl ( nMaxColumn, TRUE );

	CString strTradeInventory[nLIMIT_COLUMN] = 
	{
		"TRADE_INVENTORY_OVER_0",
		"TRADE_INVENTORY_OVER_1",
		"TRADE_INVENTORY_OVER_2",
		"TRADE_INVENTORY_OVER_3",
		"TRADE_INVENTORY_OVER_4",
		"TRADE_INVENTORY_OVER_5",
		"TRADE_INVENTORY_OVER_6",
		"TRADE_INVENTORY_OVER_7",
		"TRADE_INVENTORY_OVER_8",
		"TRADE_INVENTORY_OVER_9",
	};

	for ( int i = 0; i < nMaxColumn; i++ )
	{
		m_pFlipImage[i] = CreateFlipImage ( strTradeInventory[i].GetString() );
	}
}

void CItemSlotEx::CreateSubTrade ( int nMaxColumn )
{
	CItemSlot::CreateSubControl ( nMaxColumn, TRUE );

	CString strTrade[nLIMIT_COLUMN] = 
	{
		"TRADE_OVER_0",
		"TRADE_OVER_1",
		"TRADE_OVER_2",
		"TRADE_OVER_3",
		"TRADE_OVER_4",
		"TRADE_OVER_5",
		"TRADE_OVER_6",
		"TRADE_OVER_7",
		"TRADE_OVER_8",
		"TRADE_OVER_9",
	};

	for ( int i = 0; i < nMaxColumn; i++ )
	{
		m_pFlipImage[i] = CreateFlipImage ( strTrade[i].GetString() );
	}    
}

void CItemSlotEx::SetFlipItem ( int nIndex, BOOL bFlip )
{
	if ( m_pFlipImage[nIndex] )
	{
		m_pFlipImage[nIndex]->SetVisibleSingle ( bFlip );		
	}	
}

BOOL CItemSlotEx::IsFlipItem ( int nIndex )
{
	if ( m_pFlipImage[nIndex] )
	{
		return m_pFlipImage[nIndex]->IsVisible ();
	}
	else
	{
		return FALSE;
	}
}

CUIControl* CItemSlotEx::CreateFlipImage ( const char* szFlip )
{
	CUIControl* pFlipImage = new CUIControl;
	pFlipImage->CreateSub ( this, szFlip );
	pFlipImage->SetVisibleSingle ( FALSE );
	RegisterControl ( pFlipImage );

	return pFlipImage;
}
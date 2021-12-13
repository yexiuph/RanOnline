#include "pch.h"
#include "ItemSlotPrivateMarket.h"
#include "GLInventory.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CItemSlotPrivateMarket::CItemSlotPrivateMarket ()
{
	SecureZeroMemory ( m_pFlipImage, sizeof( m_pFlipImage ) );
}

CItemSlotPrivateMarket::~CItemSlotPrivateMarket ()
{
}

void CItemSlotPrivateMarket::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    SetItemIndex ( nOUTOFRANGE );
	m_pMouseOver->SetVisibleSingle ( FALSE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pNumberBoxArray[0] )
	{
		for ( int i = 0; i < m_nMaxColumn; i++ )
		{
			ResetNumber ( i );

			SNATIVEID sNativeID = m_InvenItem[i].sItemCustom.sNativeID;
			if ( sNativeID != NATIVEID_NULL () )
			{	
				SITEM* pItemData = GLItemMan::GetInstance().GetItem ( sNativeID );
				if ( pItemData )
				{
					//	-1이 의미하는 것은 최대값이 없음을 의미한다.
					//	현재의 겹침가능개수가 최대값이 되면, 색이 변하는데
					//	최대값을 없게 설정함으로서 색이 변하지 않게한다.
					const WORD wPileNum = -1;//pItemData->sDrugOp.wPileNum;
					const WORD wTurnNum = m_InvenItem[i].sItemCustom.wTurnNum;						

					//	NOTE
					//		겹쳐짐 개수 표시
					//
					if ( wTurnNum>1 )
					{
						SetNumber ( i, wTurnNum, wPileNum );
					}
				}
			}
		}
	}
}

CUIControl* CItemSlotPrivateMarket::CreateFlipImage ( const char* szFlip )
{
	CUIControl* pFlipImage = new CUIControl;
	pFlipImage->CreateSub ( this, szFlip );
	pFlipImage->SetVisibleSingle ( FALSE );
	RegisterControl ( pFlipImage );
	return pFlipImage;
}

void CItemSlotPrivateMarket::CreateFlipImage ( int nMaxColumn )
{
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

void CItemSlotPrivateMarket::SetFlipItem ( WORD wIndex, BOOL bFlip )
{
	if ( m_pFlipImage[wIndex] )
	{
		m_pFlipImage[wIndex]->SetVisibleSingle ( bFlip );		
	}	
}

BOOL CItemSlotPrivateMarket::IsFlipItem ( WORD wIndex )
{
	if ( m_pFlipImage[wIndex] )
	{
		return m_pFlipImage[wIndex]->IsVisible ();
	}
	else
	{
		return FALSE;
	}
}
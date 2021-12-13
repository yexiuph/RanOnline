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
					//	-1�� �ǹ��ϴ� ���� �ִ밪�� ������ �ǹ��Ѵ�.
					//	������ ��ħ���ɰ����� �ִ밪�� �Ǹ�, ���� ���ϴµ�
					//	�ִ밪�� ���� ���������μ� ���� ������ �ʰ��Ѵ�.
					const WORD wPileNum = -1;//pItemData->sDrugOp.wPileNum;
					const WORD wTurnNum = m_InvenItem[i].sItemCustom.wTurnNum;						

					//	NOTE
					//		������ ���� ǥ��
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
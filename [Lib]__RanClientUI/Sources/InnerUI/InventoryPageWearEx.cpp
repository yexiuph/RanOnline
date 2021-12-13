#include "pch.h"
#include "./InventoryPageWearEx.h"
#include "ItemImage.h"
#include "GLGaeaClient.h"
#include "GLItemMan.h"
#include "InnerInterface.h"
#include "ItemMove.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CInventoryPageWearEx::nOUTOFRANGE = -1;

CInventoryPageWearEx::CInventoryPageWearEx () 
	: m_pMouseOver(NULL)
	, m_pAButton(NULL)
	, m_pBButton(NULL)
	, m_pARHandSlot(NULL)
	, m_pALHandSlot(NULL)
	, m_pBRHandSlot(NULL)
	, m_pBLHandSlot(NULL)
{
}

CInventoryPageWearEx::~CInventoryPageWearEx ()
{
}

void CInventoryPageWearEx::CreateSubControl ( int nClassType )
{
	CString	strInvenWearBack[GLCI_NUM_EX-GLCI_NUM] = 
	{
		"INVENTORY_PAGE_WEAR_EXTREME_M",
		"INVENTORY_PAGE_WEAR_EXTREME_W",
	};

	CreateControl ( strInvenWearBack[nClassType-GLCI_NUM].GetString () );

	{	//	������ �̹��� ( 0 - 11 )
		CString strInvenWearItem[ITEM_IMAGE_SIZE] = 
		{
			"INVENTORY_WEAR_EX_ITEM_IMAGE0",
			"INVENTORY_WEAR_EX_ITEM_IMAGE1",
			"INVENTORY_WEAR_EX_ITEM_IMAGE2",
			"INVENTORY_WEAR_EX_ITEM_IMAGE3",
			"INVENTORY_WEAR_EX_ITEM_IMAGE4",
			"INVENTORY_WEAR_EX_ITEM_IMAGE5",
			"INVENTORY_WEAR_EX_ITEM_IMAGE6",
			"INVENTORY_WEAR_EX_ITEM_IMAGE7",
			"INVENTORY_WEAR_EX_ITEM_IMAGE8",
			"INVENTORY_WEAR_EX_ITEM_IMAGE9",
			"INVENTORY_WEAR_EX_ITEM_IMAGE10",
			"INVENTORY_WEAR_EX_ITEM_IMAGE11",
			"INVENTORY_WEAR_EX_ITEM_IMAGE12",
			"INVENTORY_WEAR_EX_ITEM_IMAGE13",
		};

		CString strInvenWearOver[ITEM_IMAGE_SIZE] = 
		{
			"INVENTORY_WEAR_EX_OVER_IMAGE0",
			"INVENTORY_WEAR_EX_OVER_IMAGE1",
			"INVENTORY_WEAR_EX_OVER_IMAGE2",
			"INVENTORY_WEAR_EX_OVER_IMAGE3",
			"INVENTORY_WEAR_EX_OVER_IMAGE4",
			"INVENTORY_WEAR_EX_OVER_IMAGE5",
			"INVENTORY_WEAR_EX_OVER_IMAGE6",
			"INVENTORY_WEAR_EX_OVER_IMAGE7",
			"INVENTORY_WEAR_EX_OVER_IMAGE8",
			"INVENTORY_WEAR_EX_OVER_IMAGE9",
			"INVENTORY_WEAR_EX_OVER_IMAGE10",
			"INVENTORY_WEAR_EX_OVER_IMAGE11",
			"INVENTORY_WEAR_EX_OVER_IMAGE12",
			"INVENTORY_WEAR_EX_OVER_IMAGE13",
		};

		for ( int i = 0; i < ITEM_IMAGE_SIZE; i++ )
		{
			m_pItemImage[i] = CreateItemImage ( strInvenWearItem[i].GetString (), ITEM_IMAGE0 + i );
			m_pSlotDisplay[i] = CreateControl ( strInvenWearOver[i].GetString () );
			m_pSlotDisplay[i]->SetVisibleSingle ( FALSE );
		}
	}

	m_pMouseOver = CreateControl ( "ITEM_MOUSE_OVER" );

	m_pAButton = new CBasicTextButton;
	m_pAButton->CreateSub ( this, "BASIC_TEXT_BUTTON161", UI_FLAG_XSIZE, TEXT_A_BUTTON );
	m_pAButton->CreateBaseButton(	"INVENTORY_TAB_BUTTON_A", 
									CBasicTextButton::SIZE16s, 
									CBasicButton::RADIO_FLIP, 
									(char*)ID2GAMEWORD("INVEN_TAB_BUTTON",0) );
	m_pAButton->SetFlip ( TRUE );
	RegisterControl ( m_pAButton );

	m_pBButton = new CBasicTextButton;
	m_pBButton->CreateSub ( this, "BASIC_TEXT_BUTTON161", UI_FLAG_XSIZE, TEXT_B_BUTTON );
	m_pBButton->CreateBaseButton(	"INVENTORY_TAB_BUTTON_B", 
									CBasicTextButton::SIZE16s, 
									CBasicButton::RADIO_FLIP, 
									(char*)ID2GAMEWORD("INVEN_TAB_BUTTON",1) );
	m_pBButton->SetFlip ( TRUE );
	RegisterControl ( m_pBButton );

	m_pARHandSlot = CreateControl("INVENTORY_USE_SLOT_RHAND_A");
	m_pALHandSlot = CreateControl("INVENTORY_USE_SLOT_LHAND_A");
	m_pBRHandSlot = CreateControl("INVENTORY_USE_SLOT_RHAND_B");
	m_pBLHandSlot = CreateControl("INVENTORY_USE_SLOT_LHAND_B");
}

CUIControl*	CInventoryPageWearEx::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

CItemImage*	CInventoryPageWearEx::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage;
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}

EMSLOT CInventoryPageWearEx::IMAGE2EMSLOT ( int nIndex )
{
	switch ( nIndex )
	{	
	case ITEM_IMAGE0:	return SLOT_HEADGEAR;	//	�Ӹ�
	case ITEM_IMAGE1:	return SLOT_NECK;		//	�����
	case ITEM_IMAGE2:	return SLOT_UPPER;		//	����
	case ITEM_IMAGE3:	return SLOT_LHAND;		//	�޼յ���
	case ITEM_IMAGE4:	return SLOT_WRIST;		//	�ո�
	case ITEM_IMAGE5:	return SLOT_HAND;		//	�尩
	case ITEM_IMAGE6:	return SLOT_LOWER;		//	����
	case ITEM_IMAGE7:	return SLOT_LFINGER;	//	�޼� �հ���
	case ITEM_IMAGE8:	return SLOT_RHAND;		//	�����յ���
	case ITEM_IMAGE9:	return SLOT_FOOT;		//	�Ź�
	case ITEM_IMAGE10:	return SLOT_RFINGER;	//	������ �հ���	
	case ITEM_IMAGE11:	return SLOT_RHAND_S;	//	�����յ���, �ذ���
	case ITEM_IMAGE12:	return SLOT_LHAND_S;	//	�޼յ���, �ذ���
	case ITEM_IMAGE13:	return SLOT_VEHICLE;	//  Ż��
	}

	GASSERT ( 0 && "������ ����ϴ�." );
	return SLOT_TSIZE;
}

void CInventoryPageWearEx::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE );
	if ( m_pMouseOver ) m_pMouseOver->SetVisibleSingle ( FALSE );

	//	����, ��ų �̹��� ���̱�
	CItemMove* pItemMove = CInnerInterface::GetInstance().GetItemMove ();
	SNATIVEID sHOLD_ITEM_ID(false);
	if ( pItemMove ) sHOLD_ITEM_ID = pItemMove->GetItem();

	for ( int i = 0; i < SLOT_NSIZE_S_2; i++ )
	{
		const SITEMCUSTOM& sItemCustomOld = GetItem ( i );

		EMSLOT emSlot = IMAGE2EMSLOT ( i + ITEM_IMAGE0 );
		SITEMCUSTOM sItemCustom = GLGaeaClient::GetInstance().GetCharacter()->GET_SLOT_ITEM ( emSlot );

		//	NOTE
		//		���� �����Ӱ� ���Ͽ�,
		//		����Ÿ�� �޶��� ��쿡��,
		//		�ε�/��ε� �۾��� �����Ѵ�.
		if ( sItemCustom != sItemCustomOld )
		{		
			if ( sItemCustom.sNativeID != NATIVEID_NULL () )
			{
				LoadItem ( i, sItemCustom );
			}
			else
			{
				UnLoadItem ( i );
			}
		}

		m_pSlotDisplay[i]->SetVisibleSingle ( FALSE );

		//	NOTE
		//		������ ��ġ ǥ��
		if ( sHOLD_ITEM_ID != NATIVEID_NULL () /*&& !bFOUND_TOWEAR*/ )
		{
			if ( GLGaeaClient::GetInstance().GetCharacter()->CHECKSLOT_ITEM ( sHOLD_ITEM_ID, emSlot ) )
			{
				const UIRECT& rcImagePos = m_pItemImage[i]->GetGlobalPos ();
				D3DXVECTOR2 vPos ( rcImagePos.left, rcImagePos.top );

				if ( GLGaeaClient::GetInstance().GetCharacter()->ACCEPT_ITEM ( sHOLD_ITEM_ID ) )
				{
					m_pSlotDisplay[i]->SetVisibleSingle ( TRUE );
				}

//				bFOUND_TOWEAR = true;
			}
		}
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CInventoryPageWearEx::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ITEM_IMAGE0:
	case ITEM_IMAGE1:
	case ITEM_IMAGE2:
	case ITEM_IMAGE3:
	case ITEM_IMAGE4:
	case ITEM_IMAGE5:
	case ITEM_IMAGE6:
	case ITEM_IMAGE7:
	case ITEM_IMAGE8:
	case ITEM_IMAGE9:
	case ITEM_IMAGE10:	
	case ITEM_IMAGE11:	
	case ITEM_IMAGE12:	
	case ITEM_IMAGE13:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nIndex = ControlID - ITEM_IMAGE0;
				SetItemIndex ( nIndex );

				if ( !CInnerInterface::GetInstance().IsFirstItemSlot () ) return ;

				//	����, ��ų �̹��� ���̱�
				CItemMove* pItemMove = CInnerInterface::GetInstance().GetItemMove ();
				if ( !pItemMove )
				{
					GASSERT ( 0 && "CItemMove�� ���Դϴ�." );
					return ;
				}

				if ( pItemMove->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcSlotPos = m_pItemImage[nIndex]->GetGlobalPos ();
					pItemMove->SetGlobalPos ( rcSlotPos );						

					AddMessageEx ( UIMSG_MOUSEIN_WEARSLOTEX | UIMSG_TOTOPPARENT );
				}				

				//	���콺 ǥ�� �׵θ�
				if ( pItemMove->GetItem () == NATIVEID_NULL () && m_pItemImage[m_nIndex]->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcImagePos = m_pItemImage[nIndex]->GetGlobalPos ();
					m_pMouseOver->SetGlobalPos ( rcImagePos );
					m_pMouseOver->SetVisibleSingle ( TRUE );
				}
			}
		}
		break;

	case TEXT_A_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GLGaeaClient::GetInstance().GetCharacter()->ReqSlotChange();
			}
		}
		break;

	case TEXT_B_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GLGaeaClient::GetInstance().GetCharacter()->ReqSlotChange();
			}
		}
		break;
	}
}

EMSLOT CInventoryPageWearEx::GetItemSlot ()
{
	return IMAGE2EMSLOT ( GetItemIndex () + ITEM_IMAGE0 );
}

void CInventoryPageWearEx::LoadItem ( int nIndex, SITEMCUSTOM& ref_sItemCustom )
{
	m_ItemCustomArray[nIndex] = ref_sItemCustom;

	SITEM* pItemData = GLItemMan::GetInstance().GetItem ( ref_sItemCustom.sNativeID );
	m_pItemImage[nIndex]->SetItem ( pItemData->sBasicOp.sICONID, pItemData->GetInventoryFile(), pItemData->sBasicOp.sNativeID );
}

SITEMCUSTOM& CInventoryPageWearEx::GetItem ( int nIndex )
{
	return m_ItemCustomArray[nIndex];
}

void CInventoryPageWearEx::UnLoadItem ( int nIndex )
{
	m_ItemCustomArray[nIndex].sNativeID = NATIVEID_NULL ();
	m_pItemImage[nIndex]->ResetItem ();
}

void CInventoryPageWearEx::SetTabButton( BOOL bMain )
{
	if( bMain )
	{
		m_pAButton->SetFlip( TRUE );
		m_pBButton->SetFlip( FALSE );

		m_pARHandSlot->SetVisibleSingle( FALSE );
		m_pALHandSlot->SetVisibleSingle( FALSE );
		m_pBRHandSlot->SetVisibleSingle( TRUE );
		m_pBLHandSlot->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pAButton->SetFlip( FALSE );
		m_pBButton->SetFlip( TRUE );

		m_pARHandSlot->SetVisibleSingle( TRUE );
		m_pALHandSlot->SetVisibleSingle( TRUE );
		m_pBRHandSlot->SetVisibleSingle( FALSE );
		m_pBLHandSlot->SetVisibleSingle( FALSE );
	}
}
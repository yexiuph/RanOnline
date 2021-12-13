#include "pch.h"
#include "ItemGarbage.h"

#include "ItemImage.h"
#include "BasicTextButton.h"
#include "GLGaeaClient.h"
#include "GameTextControl.h"
#include "D3DFont.h"
#include "ItemMove.h"
#include "InnerInterface.h"
#include "GLItemMan.h"
#include "ModalWindow.h"
#include "ModalCallerID.h"
#include "UITextControl.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------
// CItemGarbage
//--------------------------------------------------------------------
CItemGarbage::CItemGarbage()	// 휴지통
	: m_pItemImage( NULL )
	, m_pOkButton(NULL)
{
}

CItemGarbage::~CItemGarbage()
{
}

VOID CItemGarbage::CreateSubControl()
{
	// 아이템 슬롯
	{
		m_pItemImage = new CItemImage;
		m_pItemImage->CreateSub( this, "ITEM_GARBAGE_ITEM_SLOT", UI_FLAG_DEFAULT, ITEM_GARBAGE_ITEM_SLOT );
		m_pItemImage->CreateSubControl();
		RegisterControl( m_pItemImage );
	}

	{
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9 );
		CBasicTextBox* pTextBox = NULL;
		pTextBox = CreateStaticControl( "ITEM_GARBAGE_INFO_TEXT_STATIC", pFont, NS_UITEXTCOLOR::SILVER, TEXT_ALIGN_CENTER_X );
		pTextBox->AddText( ID2GAMEINTEXT( "ITEM_GARBAGE_INFO" ), NS_UITEXTCOLOR::SILVER );
	}

	// 승인, 거절
	{
		m_pOkButton = new CBasicTextButton;
		m_pOkButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_DEFAULT, ITEM_GARBAGE_OK_BUTTON );
		m_pOkButton->CreateBaseButton( "ITEM_GARBAGE_OK_BUTTON", CBasicTextButton::SIZE14,
									CBasicButton::CLICK_FLIP, ID2GAMEWORD( "ITEM_GARBAGE_BUTTON", 0 ) );
		m_pOkButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( m_pOkButton );

		CBasicTextButton * pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_DEFAULT, ITEM_GARBAGE_CANCEL_BUTTON );
		pButton->CreateBaseButton( "ITEM_GARBAGE_CANCEL_BUTTON", CBasicTextButton::SIZE14,
									CBasicButton::CLICK_FLIP, ID2GAMEWORD( "ITEM_GARBAGE_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}
}

VOID CItemGarbage::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	SITEMCUSTOM sItemCustom = GLGaeaClient::GetInstance().GetCharacter()->GET_GARBAGE_ITEM();
	
	if( m_pItemImage )
	{
		if( sItemCustom.sNativeID == NATIVEID_NULL() )
		{
			m_pItemImage->ResetItem();
		}
		else
		{
			SITEM* pItem = GLItemMan::GetInstance().GetItem( sItemCustom.sNativeID );
			if( pItem )
				m_pItemImage->SetItem( pItem->sBasicOp.sICONID, pItem->GetInventoryFile() );
			else
				m_pItemImage->ResetItem();
		}
	}
}

VOID CItemGarbage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch( ControlID )
	{
	case ITEM_GARBAGE_ITEM_SLOT:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			CItemMove* pItemMove = CInnerInterface::GetInstance().GetItemMove();
			if( !pItemMove )
			{
				GASSERT( 0 && "CItemGarbage::TranslateUIMessage, pItemMove == NULL" );
				break;
			}

			SNATIVEID sItem = pItemMove->GetItem();
			if( sItem != NATIVEID_NULL() )
			{
				const UIRECT& rcSlotPos = m_pItemImage->GetGlobalPos();
				pItemMove->SetGlobalPos( rcSlotPos );
				CInnerInterface::GetInstance().SetSnapItem();
			}

			if( dwMsg & UIMSG_LB_UP )
				GLGaeaClient::GetInstance().GetCharacter()->ReqGarbageMoveItem();

			SITEMCUSTOM sItemCustom = GLGaeaClient::GetInstance().GetCharacter()->GET_GARBAGE_ITEM();
			if( sItemCustom.sNativeID != NATIVEID_NULL() )
				CInnerInterface::GetInstance().SHOW_ITEM_INFO( sItemCustom, FALSE, FALSE, FALSE, 0, 0 );
		}
		break;

	case ITEM_GARBAGE_OK_BUTTON:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			SITEMCUSTOM sItemCustom = GLGaeaClient::GetInstance().GetCharacter()->GET_GARBAGE_ITEM();
			if ( sItemCustom.sNativeID == NATIVEID_NULL() )	break;

			DoModal( ID2GAMEINTEXT("ITEM_GARBAGE_OKQUESTION"), MODAL_QUESTION, YESNO, MODAL_ITEMGARBAGE_OKQUESTION );
			SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
		}
		break;

	case ITEM_GARBAGE_CANCEL_BUTTON:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			CInnerInterface::GetInstance().CloseItemGarbageWindow();
		break;

	case ET_CONTROL_BUTTON:
		if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			CInnerInterface::GetInstance().CloseItemGarbageWindow();
		break;
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}
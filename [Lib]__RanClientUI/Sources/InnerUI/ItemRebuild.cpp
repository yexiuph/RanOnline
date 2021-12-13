#include "pch.h"
#include "ItemRebuild.h"

#include "D3DFont.h"
#include "ItemImage.h"
#include "ItemMove.h"
#include "BasicTextButton.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "InnerInterface.h"
#include "GLItemMan.h"
#include "GLGaeaClient.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "ModalWindow.h"
#include "ModalCallerID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------
// ItemRebuild
//--------------------------------------------------------------------
CItemRebuild::CItemRebuild()	// ITEMREBUILD_MARK
	: m_pItemImage( NULL )
	, m_pCostTextBoxStatic( NULL )
	, m_pCostTextBox( NULL )
	, m_pMoneyTextBox( NULL )
	, m_pOkButton(NULL)
{
}

CItemRebuild::~CItemRebuild()
{
}

VOID CItemRebuild::CreateSubControl()
{
	// 아이템 슬롯
	{
		m_pItemImage = new CItemImage;
		m_pItemImage->CreateSub( this, "ITEM_REBUILD_ITEM_SLOT", UI_FLAG_DEFAULT, ITEM_REBUILD_ITEM_SLOT );
		m_pItemImage->CreateSubControl();
		RegisterControl( m_pItemImage );
	}

	// 개조비용 : 0
	{
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9 );
		m_pCostTextBoxStatic = CreateStaticControl( "ITEM_REBUILD_COAST_TEXT_STATIC", pFont, NS_UITEXTCOLOR::SILVER, TEXT_ALIGN_LEFT );
		m_pCostTextBoxStatic->AddText( ID2GAMEINTEXT( "ITEM_REBUILD_COST" ), NS_UITEXTCOLOR::SILVER );

		m_pCostTextBox = CreateStaticControl( "ITEM_REBUILD_COAST_TEXT", pFont, NS_UITEXTCOLOR::SILVER, TEXT_ALIGN_LEFT );
	}

	// 내야되는돈?
	{
		CUIControl* pControl = CreateControl( "ITEM_REBUILD_MONEY_BACK" );
		pControl->SetAlignFlag( UI_FLAG_BOTTOM );

		pControl = CreateControl( "ITEM_REBUILD_MONEY_IMAGE" );
		pControl->SetAlignFlag( UI_FLAG_BOTTOM );

		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
		m_pMoneyTextBox = CreateStaticControl( "ITEM_REBUILD_MONEY_TEXT", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		m_pMoneyTextBox->SetAlignFlag( UI_FLAG_BOTTOM );
	}

	// 승인, 거절
	{
		m_pOkButton = new CBasicTextButton;
		m_pOkButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_DEFAULT, ITEM_REBUILD_OK_BUTTON );
		m_pOkButton->CreateBaseButton( "ITEM_REBUILD_OK_BUTTON", CBasicTextButton::SIZE14,
									CBasicButton::CLICK_FLIP, ID2GAMEINTEXT( "ITEM_REBUILD_OK" ) );
		m_pOkButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		m_pOkButton->SetVisibleSingle( FALSE );
		RegisterControl( m_pOkButton );

		CBasicTextButton * pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_DEFAULT, ITEM_REBUILD_CANCEL_BUTTON );
		pButton->CreateBaseButton( "ITEM_REBUILD_CANCEL_BUTTON", CBasicTextButton::SIZE14,
									CBasicButton::CLICK_FLIP, ID2GAMEINTEXT( "ITEM_REBUILD_CANCEL" ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}
}

VOID CItemRebuild::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	SITEMCUSTOM sItemCustom = GLGaeaClient::GetInstance().GetCharacter()->GET_REBUILD_ITEM();
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

	LONGLONG i64RebuildCost = GLGaeaClient::GetInstance().GetCharacter()->GetRebuildInput();

	if( i64RebuildCost > 0 || sItemCustom.sNativeID != NATIVEID_NULL() ) 
		m_pOkButton->SetVisibleSingle( TRUE );
	else
		m_pOkButton->SetVisibleSingle( FALSE );

	if( m_pCostTextBox && m_pMoneyTextBox )
	{
		CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( i64RebuildCost, 3, "," );
		m_pCostTextBox->SetOneLineText( strTemp );
		m_pMoneyTextBox->SetOneLineText( strTemp );
	}

	//if( m_pMoneyTextBox )
	//{
	//	LONGLONG i64RebuildInput = GLGaeaClient::GetInstance().GetCharacter()->GetRebuildInput();
	//	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( i64RebuildInput, 3, "," );
 //       m_pMoneyTextBox->SetOneLineText( strTemp );
	//}
}

VOID CItemRebuild::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch( ControlID )
	{
	case ITEM_REBUILD_ITEM_SLOT:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			CItemMove* pItemMove = CInnerInterface::GetInstance().GetItemMove();
			if( !pItemMove )
			{
				GASSERT( 0 && "CItemRebuild::TranslateUIMessage, pItemMove == NULL" );
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
				GLGaeaClient::GetInstance().GetCharacter()->ReqRebuildMoveItem();

			SITEMCUSTOM sItemCustom = GLGaeaClient::GetInstance().GetCharacter()->GET_REBUILD_ITEM();
			if( sItemCustom.sNativeID != NATIVEID_NULL() )
				CInnerInterface::GetInstance().SHOW_ITEM_INFO( sItemCustom, FALSE, FALSE, FALSE, 0, 0 );
		}
		break;

	case ITEM_REBUILD_OK_BUTTON:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			DoModal( ID2GAMEINTEXT("ITEM_REBUILD_OKQUESTION"), MODAL_QUESTION, YESNO, MODAL_ITEMREBUILD_OKQUESTION );
			SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
		}
		break;

	case ITEM_REBUILD_CANCEL_BUTTON:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			CInnerInterface::GetInstance().CloseItemRebuildWindow();
		break;

	case ET_CONTROL_BUTTON:
		if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			CInnerInterface::GetInstance().CloseItemRebuildWindow();
		break;
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}
#include "pch.h"

#include "ItemMixInventoryWindow.h"
#include "ItemMixInventoryPage.h"
#include "InnerInterface.h"
#include "GLItemMan.h"
#include "GLGaeaClient.h"
#include "GameTextControl.h"
#include "ModalCallerID.h"
#include "ModalWindow.h"
#include "D3DFont.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "UITextControl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------
// CGarbageInventoryWindow
//--------------------------------------------------------------------
CItemMixInvenWindow::CItemMixInvenWindow()	// ÈÞÁöÅë
	: m_pPage( NULL )
	, m_nONE_VIEW_SLOT( 4 )
{
}

CItemMixInvenWindow::~CItemMixInvenWindow()
{
}

VOID CItemMixInvenWindow::CreateSubControl()
{
	m_pPage = new CItemMixInvenPage;
	m_pPage->CreateSub( this, "ITEMMIXINVEN_PAGE", UI_FLAG_YSIZE, ITEMMIXINVEN_PAGE );
	m_pPage->CreateSubControl();
	RegisterControl( m_pPage );

	CUIControl* pControl = CreateControl( "ITEMMIXINVEN_MONEY_BACK" );
	pControl->SetAlignFlag( UI_FLAG_BOTTOM );

	CBasicButton* pButton = CreateFlipButton( "ITEMMIXINVEN_MONEY_BUTTON", "ITEMMIXINVEN_MONEY_BUTTON_F", ITEMMIXINVEN_MONEY_BUTTON, CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver( "ITEMMIXINVEN_MONEY_BUTTON_F" );
	pButton->SetUseDynamic( TRUE );
	pButton->SetAlignFlag( UI_FLAG_BOTTOM );

	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pMoneyTextBox = CreateStaticControl( "ITEMMIXINVEN_MONEYTEXT", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMoneyTextBox->SetAlignFlag( UI_FLAG_BOTTOM );
}

VOID CItemMixInvenWindow::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();

	m_pPage->ResetAllFlipItem();
	for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		if( pCharacter->m_sItemMixPos[i].VALID() )
			m_pPage->UpdateFlipItem( pCharacter->m_sItemMixPos[i] );		
	}

	{
		CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pCharacter->m_lnMoney, 3, "," );
		m_pMoneyTextBox->SetOneLineText( strTemp );
	}

	{
		GLInventory& ref_Inventory = pCharacter->m_cInventory;
		m_pPage->LoadItemPage( ref_Inventory );
	}
}

VOID CItemMixInvenWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch( ControlID )
	{
	case ITEMMIXINVEN_PAGE:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			INT nPosX, nPosY;
			m_pPage->GetItemIndex( &nPosX, &nPosY );

			if( nPosX < 0 || nPosY < 0 )
				return;

			SINVENITEM& sInvenItem = m_pPage->GetItem( nPosX, nPosY );
			if( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL() )
				CInnerInterface::GetInstance().SHOW_ITEM_INFO( sInvenItem.sItemCustom, FALSE, FALSE, FALSE, sInvenItem.wPosX, sInvenItem.wPosY );

			if( dwMsg & UIMSG_LB_UP )
				GLGaeaClient::GetInstance().GetCharacter()->ReqInvenTo( nPosX, nPosY );
		}
		break;	

	case ET_CONTROL_BUTTON:
		if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			CInnerInterface::GetInstance().CloseItemMixWindow();
		break;
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

HRESULT CItemMixInvenWindow::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = CUIWindowEx::RestoreDeviceObjects( pd3dDevice );
	if( FAILED( hr ) )
		return hr;

	const LONG lResolution = CInnerInterface::GetInstance().GetResolution();
	WORD X_RES = HIWORD( lResolution );
	WORD Y_RES = LOWORD( lResolution );

	const UIRECT& rcWindow = GetGlobalPos();

	D3DXVECTOR2 vPos;
	vPos.x = ( (X_RES) / 2.0f );
	vPos.y = ( Y_RES - rcWindow.sizeY ) / 2.0f;
	SetGlobalPos( vPos );

	return S_OK;
}

VOID CItemMixInvenWindow::SetOneViewSlot( const INT& nONE_VIEW_SLOT )
{
	CUIControl TempControl;
	TempControl.Create( 1, "ITEMMIXINVENSLOT_ITEM_SLOT0" );

	CUIControl TempControl2;
	TempControl2.Create( 2, "ITEMMIXINVENSLOT_ITEM_SLOT1" );

	const UIRECT& rcTempGlobalPos = TempControl.GetGlobalPos();
	D3DXVECTOR2 vONE_SLOT_SIZE( rcTempGlobalPos.sizeX, rcTempGlobalPos.sizeY );

	const UIRECT& rcTempGlobalPos2 = TempControl2.GetGlobalPos();

	const FLOAT fSlotGap = rcTempGlobalPos2.top - rcTempGlobalPos.bottom;

	const FLOAT fOldSizeY = ( vONE_SLOT_SIZE.y + fSlotGap ) * m_nONE_VIEW_SLOT;
	const FLOAT fNewSizeY = ( vONE_SLOT_SIZE.y + fSlotGap ) * nONE_VIEW_SLOT;
    const FLOAT fDIFF_SIZE_Y = fNewSizeY - fOldSizeY;

	const UIRECT& rcGlobalPos = GetGlobalPos();
	UIRECT rcNewGlobalPos( rcGlobalPos.left, rcGlobalPos.top, rcGlobalPos.sizeX, rcGlobalPos.sizeY + fDIFF_SIZE_Y );

	WORD wFlagAlignBACK = GetAlignFlag();
	SetAlignFlag( UI_FLAG_YSIZE );
	{
		AlignSubControl( rcGlobalPos, rcNewGlobalPos );
		SetGlobalPos( rcNewGlobalPos );
	}
	ResetAlignFlag();
	SetAlignFlag( wFlagAlignBACK );

	m_pPage->SetOneViewSlot( nONE_VIEW_SLOT );
	m_nONE_VIEW_SLOT = nONE_VIEW_SLOT;
}
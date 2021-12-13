#include "pch.h"
#include "BasicQuickPotionSlot.h"

#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "GLGaeaClient.h"
#include "ItemImage.h"
#include "GLItemMan.h"
#include "d3dfont.h"
#include "UITextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const D3DCOLOR CBasicQuickPotionSlot::dwDEFAULT_TRANSPARENCY = D3DCOLOR_ARGB ( 160, 255, 255, 255 );;
const D3DCOLOR CBasicQuickPotionSlot::dwFULL_TRANSPARENCY = D3DCOLOR_ARGB ( 255, 255, 255, 255 );;

CBasicQuickPotionSlot::CBasicQuickPotionSlot () :
	m_pPotionMouseOver ( NULL ),
	m_pMiniText ( NULL ),
	m_pd3dDevice ( NULL ),
	m_nIndex ( 0 ),
	m_pItemImage ( NULL )
{
}

CBasicQuickPotionSlot::~CBasicQuickPotionSlot ()
{
}

void  CBasicQuickPotionSlot::CreateSubControl ()
{
	CreatePotionImage ( "BASIC_QUICK_POTION_IMAGE" );	
	CreateMouseOver ( "BASIC_QUICK_POTION_SLOT_MOUSE_OVER" );
	m_pMiniText = CreateTextBox ( "BASIC_QUICK_POTION_SLOT_AMOUNT_SINGLE" );	
	m_pMiniText->SetTextAlign ( TEXT_ALIGN_RIGHT );
	SetSlotIndex ( 0 );
}

void CBasicQuickPotionSlot::CreatePotionImage ( char* szImage )
{
	CItemImage* pItemImage = new CItemImage;
	pItemImage->CreateSub ( this, szImage );
	pItemImage->CreateSubControl ();
	pItemImage->SetUseRender ( TRUE );
	pItemImage->SetVisibleSingle ( FALSE );
	RegisterControl ( pItemImage );
    m_pItemImage = pItemImage;
}

HRESULT CBasicQuickPotionSlot::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

void CBasicQuickPotionSlot::CreateMouseOver ( char* szMouseOver )
{
	CUIControl* pPotionMouseOver = new CUIControl;
	pPotionMouseOver->CreateSub ( this, szMouseOver, UI_FLAG_DEFAULT, QUICK_POTION_MOUSE_OVER );
	pPotionMouseOver->SetVisibleSingle ( TRUE );
    RegisterControl ( pPotionMouseOver );
	m_pPotionMouseOver = pPotionMouseOver;
}

CBasicTextBox* CBasicQuickPotionSlot::CreateTextBox ( char* szTextBox )
{
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, szTextBox );
    pTextBox->SetFont ( pFont8 );	
	RegisterControl ( pTextBox );
	return pTextBox;
}

void CBasicQuickPotionSlot::SetItem ( SNATIVEID sICONINDEX, const char* szTexture, int nAmount )
{
	if ( m_pItemImage )
	{
		m_pItemImage->SetItem ( sICONINDEX, szTexture, m_sNativeID );
		m_pItemImage->SetVisibleSingle ( TRUE );

		CString strNumber;
		strNumber.Format ( "%d", nAmount );
		if ( m_pMiniText ) m_pMiniText->SetOneLineText ( strNumber, NS_UITEXTCOLOR::WHITE );

		if ( nAmount <= 0 )	m_pItemImage->SetDiffuse ( dwDEFAULT_TRANSPARENCY );
		else				m_pItemImage->SetDiffuse ( dwFULL_TRANSPARENCY );
	}
}

void CBasicQuickPotionSlot::ResetItem ()
{
	if ( m_pItemImage )
	{
		m_pItemImage->SetVisibleSingle ( FALSE );
		m_pItemImage->ResetItem ();

		if ( m_pMiniText ) m_pMiniText->ClearText ();
	}
}

void CBasicQuickPotionSlot::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	const int nIndex = GetSlotIndex ();
	SACTION_SLOT sSlot = GLGaeaClient::GetInstance().GetCharacter()->m_sACTIONQUICK[nIndex];
	if ( sSlot.sNID != NATIVEID_NULL () )
	{
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( sSlot.sNID );
		if ( !pItem )
		{
			GASSERT ( 0 && "등록되지 않은 ID입니다." );
			return ;
		}

		//	NOTE
		//		이전 프레임과 ID가 다를경우 날림
		if ( GetItemID () != sSlot.sNID ) ResetItem ();

		//	NOTE
		//		슬롯에 등록된 ID 등록
		SetItemNativeID ( sSlot.sNID );

		int nAmountNumber = GLGaeaClient::GetInstance().GetCharacter()->GetAmountActionQ ( nIndex );
		SetItem ( pItem->sBasicOp.sICONID, pItem->GetInventoryFile(), nAmountNumber );
	}
	else
	{
		ResetItem ();
	}
}
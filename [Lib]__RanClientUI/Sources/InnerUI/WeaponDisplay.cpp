#include "pch.h"
#include "WeaponDisplay.h"
#include "ItemImage.h"
#include "InnerInterface.h"
#include "GLItemMan.h"
#include "UITextControl.h"
#include "GLGaeaClient.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWeaponDisplay::CWeaponDisplay () 
: m_pWeaponImage ( NULL )
{
}

CWeaponDisplay::~CWeaponDisplay ()
{
}

void  CWeaponDisplay::CreateSubControl ()
{
	CItemImage* pItemImage = new CItemImage;
	pItemImage->CreateSub ( this, "WEAPON_DISPLAY_IMAGE" );
	pItemImage->CreateSubControl ();
	pItemImage->SetUseRender ( TRUE );
	pItemImage->SetVisibleSingle ( FALSE );
	RegisterControl ( pItemImage );
    m_pWeaponImage = pItemImage;

	CUIControl* pDummyControl = new CUIControl;
	pDummyControl->CreateSub ( this, "WEAPON_DISPLAY_DUMMY");
	RegisterControl ( pDummyControl );
	m_pDummyControl = pDummyControl;
}

void CWeaponDisplay::SetItem ( SNATIVEID sICONINDEX, const char* szTexture)
{
	if ( m_pWeaponImage )
	{
		m_pWeaponImage->SetItem ( sICONINDEX, szTexture );
		m_pWeaponImage->SetVisibleSingle ( TRUE );
	}
}

void CWeaponDisplay::ResetItem ()
{
	if ( m_pWeaponImage )
	{
		m_pWeaponImage->SetVisibleSingle ( FALSE );
		m_pWeaponImage->ResetItem ();
	}
}

void CWeaponDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	EMSLOT emRHand = GLGaeaClient::GetInstance().GetCharacter()->GetCurRHand();

	SITEMCUSTOM sItem =  GLGaeaClient::GetInstance().GetCharacter()->GET_SLOT_ITEM(emRHand);

	if ( m_sNativeID != sItem.sNativeID ) 
	{
		m_sNativeID = sItem.sNativeID;
		
		if ( m_sNativeID.IsValidNativeID() )
		{
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( m_sNativeID );
			m_sWeaponStr = pItem->GetName();
			if ( !pItem )
			{
				GASSERT ( 0 && "등록되지 않은 ID입니다." );
				return ;
			}

			ResetItem ();	
			SetItem ( pItem->sBasicOp.sICONID, pItem->GetInventoryFile() );	
			m_pDummyControl->SetVisibleSingle( true );
		}
		else
		{
			m_sWeaponStr = "";
			ResetItem ();
			m_pDummyControl->SetVisibleSingle( false );
		}
	}

	if ( CHECK_MOUSE_IN ( GetMessageEx () ) )
	{
		CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( m_sWeaponStr, NS_UITEXTCOLOR::GREENYELLOW );
	}


}
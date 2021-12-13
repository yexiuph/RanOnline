#include "pch.h"
#include "BasicQuickPotionSlotEx.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "GLGaeaClient.h"
#include "GameTextControl.h"
#include "ItemImage.h"
#include "UITextControl.h"
#include "RANPARAM.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicQuickPotionSlotEx::CBasicQuickPotionSlotEx ()
{
}

CBasicQuickPotionSlotEx::~CBasicQuickPotionSlotEx ()
{
}

void  CBasicQuickPotionSlotEx::CreateSubControl ( int nIndex )
{
	CreatePotionImage ( "QUICK_POTION_IMAGE" );
	CreateMouseOver ( "QUICK_POTION_MOUSE_OVER" );	
	m_pPotionMouseOver->SetVisibleSingle ( FALSE );

	m_pMiniText = CreateTextBox ( "BASIC_QUICK_POTION_SLOT_AMOUNT" );
	m_pMiniText->SetTextAlign ( TEXT_ALIGN_RIGHT );
	const D3DXVECTOR2 vPos ( m_pMiniText->GetLocalPos ().left, m_pMiniText->GetLocalPos ().top );
	m_pMiniText->SetLocalPos ( D3DXVECTOR2 ( vPos.x-3.0f, vPos.y ) );
	CString strShotcut;

	CBasicTextBox* pTextBox = NULL;
	{	//	오른쪽 위 shortcut
		pTextBox = CreateTextBox ( "QUICK_POTION_SLOT_SHORTCUT_TEXT" );
		strShotcut = CInnerInterface::GetInstance().GetdwKeyToString(RANPARAM::QuickSlot[nIndex]);
		pTextBox->AddText ( strShotcut, NS_UITEXTCOLOR::WHITE );
		m_pTextBox = pTextBox;
	}

	SetSlotIndex ( nIndex );
}

void CBasicQuickPotionSlotEx::SetShotcutText( CString strTemp )
{
	m_pTextBox->SetText( strTemp, NS_UITEXTCOLOR::WHITE );
}


void CBasicQuickPotionSlotEx::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CBasicQuickPotionSlot::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    int nIndex = GetSlotIndex ();
	GASSERT ( 0 <= nIndex && nIndex < EMACTIONQUICK_SIZE && "범위를 벗어납니다." );

	SACTION_SLOT sSlot = GLGaeaClient::GetInstance().GetCharacter()->m_sACTIONQUICK[nIndex];
	BOOL bSlotEmpty = (sSlot.sNID == NATIVEID_NULL ());
	DWORD dwMsg = GetMessageEx ();
	if ( m_pPotionMouseOver )
	{
		if ( !bSlotEmpty && CHECK_MOUSE_IN ( dwMsg ) && bFirstControl )
		{
			m_pPotionMouseOver->SetVisibleSingle ( TRUE );
		}
		else
		{
			m_pPotionMouseOver->SetVisibleSingle ( FALSE );
		}
	}
}

UIRECT	CBasicQuickPotionSlotEx::GetAbsPosPotionImage ()
{
	return m_pItemImage->GetGlobalPos ();
}
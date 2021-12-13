#include "pch.h"
#include "UILeftTopGroup.h"

#include "BasicPotionTray.h"
#include "BasicQuickPotionSlot.h"
#include "BasicLevelDisplay.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"

#include "GLGaeaClient.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"
#include "DxInputString.h"
#include "InnerInterface.h"
#include "RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUILeftTopGroup::CUILeftTopGroup ()
{
}

CUILeftTopGroup::~CUILeftTopGroup ()
{
}

void CUILeftTopGroup::CreateSubControl ()
{
	CBasicQuickPotionSlot* pBasicQuickSlot = new CBasicQuickPotionSlot;
	pBasicQuickSlot->CreateSub ( this, "BASIC_QUICK_POTION_SLOT", UI_FLAG_DEFAULT, BASIC_QUICK_POTION_SLOT );
	pBasicQuickSlot->CreateSubControl ();
	pBasicQuickSlot->SetVisibleSingle ( FALSE );
	RegisterControl ( pBasicQuickSlot );	
	m_pBasicQuickSlot = pBasicQuickSlot;

	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, "QUICK_POTION_TRAY_OPEN_BUTTON", UI_FLAG_DEFAULT, QUICK_POTION_TRAY_OPEN_BUTTON );
	pButton->CreateFlip ( "QUICK_POTION_TRAY_OPEN_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	pButton->SetVisibleSingle ( FALSE );
	RegisterControl ( pButton );
	m_pPotionTrayOpenButton = pButton;

	CBasicPotionTray* pPotionTray = new CBasicPotionTray;
	pPotionTray->CreateSub ( this, "BASIC_QUICK_POTION_TRAY", UI_FLAG_DEFAULT, BASIC_QUICK_POTION_TRAY );
	pPotionTray->CreateSubControl ();
	RegisterControl ( pPotionTray );
	m_pPotionTray = pPotionTray;

// #ifndef CH_PARAM // 중국 인터페이스 변경
	CBasicLevelDisplay* pLevelDisplay = new CBasicLevelDisplay;
	pLevelDisplay->CreateSub ( this, "BASIC_LEVEL_DISPLAY", UI_FLAG_DEFAULT, BASIC_LEVEL_DISPLAY );
	pLevelDisplay->CreateSubControl ();
	RegisterControl ( pLevelDisplay );
// #endif
}

void	CUILeftTopGroup::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	bool bKEYBOARD_BLOCK = false;
	if ( CInnerInterface::GetInstance().IsVisibleGroup ( MODAL_WINDOW ) ||
		DXInputString::GetInstance().IsOn () )
	{
		bKEYBOARD_BLOCK = true;
	}

	if ( bKEYBOARD_BLOCK )
	{
		return ;
	}

	GLCharacter* const pCharacter = GLGaeaClient::GetInstance().GetCharacter ();
	if ( pCharacter )
	{
		//	퀵 물약
		const int nOUTOFRANGE = -1;
		int nIndex = nOUTOFRANGE;
		if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::QuickSlot[0], DXKEY_DOWN ) ) nIndex = 0;
		if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::QuickSlot[1], DXKEY_DOWN ) ) nIndex = 1;
		if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::QuickSlot[2], DXKEY_DOWN ) ) nIndex = 2;
		if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::QuickSlot[3], DXKEY_DOWN ) ) nIndex = 3;
		if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::QuickSlot[4], DXKEY_DOWN ) ) nIndex = 4;
		if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::QuickSlot[5], DXKEY_DOWN ) ) nIndex = 5;
		if ( nOUTOFRANGE < nIndex )
		{
			pCharacter->ReqActionQ ( nIndex );
			if ( m_pBasicQuickSlot ) m_pBasicQuickSlot->SetSlotIndex ( nIndex );
		}
	}

//#ifndef CH_PARAM
//	if ( UIKeyCheck::GetInstance()->Check ( DIK_TAB, DXKEY_DOWN ) )
//	{
//		const BOOL bVisible = m_pPotionTrayOpenButton->IsVisible ();
//		m_pBasicQuickSlot->SetVisibleSingle ( !bVisible );
//		m_pPotionTrayOpenButton->SetVisibleSingle ( !bVisible );
//		m_pPotionTray->SetVisibleSingle ( bVisible );
//	}
//#endif
}

void	CUILeftTopGroup::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	if ( CHECK_MOUSE_IN ( dwMsg ) ) AddMessageEx ( UIMSG_MOUSE_IN );

	switch ( ControlID )
	{
	case QUICK_POTION_TRAY_OPEN_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pPotionTrayOpenButton->SetVisibleSingle ( FALSE );
				m_pBasicQuickSlot->SetVisibleSingle ( FALSE );
				m_pPotionTray->SetVisibleSingle( TRUE );
				//m_ActionMsgQ.PostUIMessage ( BASIC_QUICK_POTION_TRAY, UIMSG_VISIBLE );
			}
		}
		break;

	case BASIC_QUICK_POTION_TRAY:
		{
			BOOL bSnap = (dwMsg & UIMSG_MOUSEIN_POTIONSLOT);
			if ( bSnap ) CInnerInterface::GetInstance().SetSnapItem ();

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_pPotionTray )
				{
					int nIndex = m_pPotionTray->GetSlotIndex ();
					if ( 0 <= nIndex  )
					{
						m_pBasicQuickSlot->SetSlotIndex ( nIndex );
					}
				}
			}

			if ( UIMSG_TRAY_CLOSE_LBUP & dwMsg )
			{
				m_pPotionTray->SetVisibleSingle ( FALSE );				
				m_pPotionTrayOpenButton->SetVisibleSingle ( TRUE );
				m_pBasicQuickSlot->SetVisibleSingle ( TRUE );
				//m_ActionMsgQ.PostUIMessage ( BASIC_QUICK_POTION_SLOT, UIMSG_VISIBLE );
				//m_ActionMsgQ.PostUIMessage ( QUICK_POTION_TRAY_OPEN_BUTTON, UIMSG_VISIBLE );
			}
		}
		break;
	}
}

void 	CUILeftTopGroup::SetShotcutText ( DWORD nID, CString& strTemp )
{
	m_pPotionTray->SetShotcutText( nID, strTemp );
}

#include "pch.h"
#include "BasicPotionTray.h"

#include "BasicQuickPotionSlotEx.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "InnerInterface.h"
#include "GLItemMan.h"
#include "UITextControl.h"

#include "GLGaeaClient.h"
#include "ItemMove.h"
#include "d3dfont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	int	CBasicPotionTray::nOUTOFRANGE = -1;

CBasicPotionTray::CBasicPotionTray () :
	m_pFont8 ( NULL )
{
}

CBasicPotionTray::~CBasicPotionTray ()
{
}

void CBasicPotionTray::CreateSubControl ()
{
	m_pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CUIControl* pSlotBegin = new CUIControl;
	pSlotBegin->CreateSub ( this, "BASIC_QUICK_POTION_TRAY_SLOT_BEGIN" );
	pSlotBegin->SetControlNameEx ( "퀵 포션 슬롯 스타트" );
    RegisterControl ( pSlotBegin );

	for ( int i = 0; i < EMACTIONQUICK_SIZE; i++ )
	{
		CString strKeyword;
		strKeyword.Format ( "BASIC_QUICK_POTION_TRAY_SLOT%d", i );

		CBasicQuickPotionSlotEx* pSlot = new CBasicQuickPotionSlotEx;
		pSlot->CreateSub ( this, strKeyword.GetString(), UI_FLAG_DEFAULT, QUICK_POTION_SLOT0 + i );
		pSlot->CreateSubControl ( i );
		RegisterControl ( pSlot );
		m_pSlot[i] = pSlot;
	}

// #ifndef CH_PARAM // 중국 인터페이스 변경
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, "QUICK_POTION_TRAY_CLOSE_BUTTON", UI_FLAG_DEFAULT, QUICK_POTION_TRAY_CLOSE_BUTTON );
	pButton->CreateFlip ( "QUICK_POTION_TRAY_CLOSE_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	pButton->SetControlNameEx ( "퀵슬롯트레이 클로즈버튼" );
	RegisterControl ( pButton );
// #endif
}

void CBasicPotionTray::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case QUICK_POTION_SLOT0:
	case QUICK_POTION_SLOT1:
	case QUICK_POTION_SLOT2:
	case QUICK_POTION_SLOT3:
	case QUICK_POTION_SLOT4:
	case QUICK_POTION_SLOT5:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{	
				int nIndex = ControlID - QUICK_POTION_SLOT0;
				SetSlotIndex ( nIndex );

				if ( !CInnerInterface::GetInstance().IsFirstItemSlot () ) return ;				

				//	스냅, 스킬 이미지 붙이기
				CItemMove* pItemMove = CInnerInterface::GetInstance().GetItemMove ();
				if ( !pItemMove )
				{
					GASSERT ( 0 && "CItemMove가 널입니다." );
					return ;
				}

				if ( pItemMove->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcSlotPotionImagePos = m_pSlot[nIndex]->GetAbsPosPotionImage ();
					pItemMove->SetGlobalPos ( rcSlotPotionImagePos );						

					AddMessageEx ( UIMSG_MOUSEIN_POTIONSLOT );
				}

				if ( dwMsg & UIMSG_LB_UP )
				{
					const SITEMCUSTOM& sItemCustom = GLGaeaClient::GetInstance().GetCharacter()->GET_HOLD_ITEM ();
					if ( sItemCustom.sNativeID != NATIVEID_NULL () )	//	설정
					{
						GLGaeaClient::GetInstance().GetCharacter()->ReqItemQuickSet ( (WORD) nIndex );
					}
					else	//	쓰기
					{
						GLGaeaClient::GetInstance().GetCharacter()->ReqActionQ ( (WORD) nIndex );
					}
				}

				//	퀵 아이템 해제
				if ( dwMsg & UIMSG_RB_UP )
				{
					GLGaeaClient::GetInstance().GetCharacter()->ReqItemQuickReSet ( (WORD) nIndex );
				}

                //	설명 뿌리기
				SACTION_SLOT sSlot = GLGaeaClient::GetInstance().GetCharacter()->m_sACTIONQUICK[nIndex];
				if ( sSlot.sNID != NATIVEID_NULL () )
				{
					SITEM* pItem = GLItemMan::GetInstance().GetItem ( sSlot.sNID );
					if ( pItem )
					{
						DWORD dwLevel = pItem->sBasicOp.emLevel;
						CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( pItem->GetName(), COMMENT::ITEMCOLOR[dwLevel] );
					}
				}
			}
		}
		break;

	case QUICK_POTION_TRAY_CLOSE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					AddMessageEx ( UIMSG_TRAY_CLOSE_LBUP );
				}
			}
		}
		break;
	}
}

void CBasicPotionTray::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	if ( !IsVisible () ) return ;
	SetSlotIndex ( nOUTOFRANGE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void 	CBasicPotionTray::SetShotcutText ( DWORD nID, CString& strTemp )
{
	m_pSlot[nID]->SetShotcutText(strTemp );	
}

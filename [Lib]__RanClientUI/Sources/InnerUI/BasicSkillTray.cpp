#include "pch.h"
#include "BasicSkillTray.h"

#include "BasicQuickSkillSlotEx.h"
#include "GLGaeaClient.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "InnerInterface.h"
#include "SkillWindowToTray.h"
#include "BasicVarTextBox.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "d3dfont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	int	CBasicSkillTray::nLBUPSKIP = 1;

CBasicSkillTray::CBasicSkillTray ()
	: m_nLBUPSKIP(0)
	, m_pSkillInfo(NULL)
{
	memset ( m_pSlotEx, 0, sizeof ( m_pSlotEx ) );
}

CBasicSkillTray::~CBasicSkillTray ()
{
}

void CBasicSkillTray::CreateSubControl( INT nTabIndex )
{
	GASSERT( ( 0 <= nTabIndex ) && ( nTabIndex < MAX_TAB_INDEX ) );

	CD3DFontPar * m_pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	for ( int nNumber = 0; nNumber < QUICK_SKILL_SLOT_MAX; ++nNumber )
	{
		CString strKeyword;
		strKeyword.Format ( "BASIC_QUICK_SKILL_TRAY_SLOT%d", nNumber );

		CBasicQuickSkillSlotEx*	pSlot = new CBasicQuickSkillSlotEx;
		pSlot->CreateSub( this, strKeyword.GetString(), UI_FLAG_DEFAULT, QUICK_SKILL_SLOT1 + nNumber );
		pSlot->CreateSubControl( nTabIndex );
		pSlot->CreateNumberText( m_pFont8, nNumber );		
		RegisterControl( pSlot );
		m_pSlotEx[nNumber] = pSlot;
	}

	CUIControl* pSlotEnd = new CUIControl;
	pSlotEnd->CreateSub ( this, "BASIC_QUICK_SKILL_TRAY_SLOT_END" );
	pSlotEnd->SetControlNameEx ( "슬롯 엔드" );
    RegisterControl ( pSlotEnd );

	CBasicVarTextBox* pSkillInfo = new CBasicVarTextBox;
	pSkillInfo->CreateSub ( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pSkillInfo->CreateSubControl ();
	pSkillInfo->SetVisibleSingle ( FALSE );
	RegisterControl ( pSkillInfo );
	m_pSkillInfo = pSkillInfo;
}

void CBasicSkillTray::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case QUICK_SKILL_SLOT1:
	case QUICK_SKILL_SLOT2:
	case QUICK_SKILL_SLOT3:
	case QUICK_SKILL_SLOT4:
	case QUICK_SKILL_SLOT5:
	case QUICK_SKILL_SLOT6:
	case QUICK_SKILL_SLOT7:
	case QUICK_SKILL_SLOT8:
	case QUICK_SKILL_SLOT9:
	case QUICK_SKILL_SLOT0:
		{			
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{	
				int nG_Index = m_pSlotEx[0]->GetTabIndex() * QUICK_SKILL_SLOT_MAX;
				int nIndex = ControlID - QUICK_SKILL_SLOT1;
				nG_Index += nIndex;

				SNATIVEID sOverSkill = GLGaeaClient::GetInstance().GetCharacter()->GetCharData().m_sSKILLQUICK[nG_Index];
                if ( sOverSkill != NATIVEID_NULL () )
				{
					UpdateSkillInfo ( sOverSkill );
					m_pSkillInfo->SetVisibleSingle ( TRUE );
				}				

				//	스냅, 스킬 이미지 붙이기
				CSkillWindowToTray* pSkillWindowToTray = CInnerInterface::GetInstance().GetSkillWindowToTray ();
				if ( !pSkillWindowToTray )
				{
					GASSERT ( 0 && "CSkillWindowToTray가 널입니다." );
					return ;
				}

				if ( pSkillWindowToTray->GetSkill () != NATIVEID_NULL () )
				{
					const UIRECT& rcSlotPos = m_pSlotEx[nIndex]->GetAbsPosSkillImage ();
					pSkillWindowToTray->SetGlobalPos ( rcSlotPos );						

					AddMessageEx ( UIMSG_MOUSEIN_SKILLSLOT );
				}		

				if ( dwMsg & UIMSG_LB_UP )
				{	
					SNATIVEID sNativeID = pSkillWindowToTray->GetSkill ();

					//	스킬 등록
					if ( sNativeID != NATIVEID_NULL () )
					{						
						GLGaeaClient::GetInstance().GetCharacter()->ReqSkillQuickSet ( sNativeID, nG_Index );
						pSkillWindowToTray->ResetSkill ();
						m_nLBUPSKIP = 0;
					}
					else
					{
						//	( 등록할 스킬이 없을 경우 ) 현제슬롯의 스킬을 사용할 스킬로 설정.
						GLGaeaClient::GetInstance().GetCharacter()->ReqSkillRunSet( nG_Index );
					}					
				}

				//	스킬 해제
				if ( dwMsg & UIMSG_RB_UP )
				{					
					GLGaeaClient::GetInstance().GetCharacter()->ReqSkillQuickReSet ( nG_Index );
				}
			}
		}
		break;
	}
}

void	CBasicSkillTray::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_pSkillInfo->SetVisibleSingle ( FALSE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	DWORD dwMsg = GetMessageEx ();
	if ( dwMsg & UIMSG_LB_UP )
	{
		CSkillWindowToTray* pSkillWindowToTray = CInnerInterface::GetInstance().GetSkillWindowToTray ();
		if ( !pSkillWindowToTray )	return ;

		//	스킬이 손에 들려져있고...
		//	첫번째 왼쪽클릭메시지는 무시한다.
		BOOL bExist = ( pSkillWindowToTray->GetSkill () != NATIVEID_NULL () );
		if ( bExist )
		{
			if ( nLBUPSKIP <= m_nLBUPSKIP )
			{
				pSkillWindowToTray->ResetSkill ();
				m_nLBUPSKIP = 0;
			}
			else
			{
				m_nLBUPSKIP++;
			}
		}		
	}
}

void CBasicSkillTray::UpdateSkillInfo ( SNATIVEID sNativeID )
{
	if ( sNativeID != NATIVEID_NULL() )
	{
		static SNATIVEID sNativeIDBack = NATIVEID_NULL();

		BOOL bUPDATE = FALSE;
		if ( sNativeID != sNativeIDBack )
		{			
			bUPDATE = TRUE;
		}

		if ( bUPDATE )
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID.wMainID, sNativeID.wSubID );
			if ( pSkill )	//	스킬이 존재할 경우
			{
				SCHARSKILL* pCharSkill = GLGaeaClient::GetInstance().GetCharacter()->GETLEARNED_SKILL ( sNativeID );
				LoadSkillInfo ( pSkill, pCharSkill );
			}

			sNativeIDBack = sNativeID;
		}		
	}
}

void CBasicSkillTray::LoadSkillInfo ( PGLSKILL const pSkill, SCHARSKILL* const pCharSkill )
{
	m_pSkillInfo->SetTextNoSplit( pSkill->GetName(), NS_UITEXTCOLOR::PALEGREEN );
}
void CBasicSkillTray::SetShotcutText ( DWORD nID, CString& strTemp )
{
	m_pSlotEx[nID]->SetShotcutText(strTemp);	
}

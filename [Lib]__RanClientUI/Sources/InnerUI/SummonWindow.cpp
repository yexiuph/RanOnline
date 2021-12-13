#include "pch.h"

#include "BasicLineBox.h"
#include "d3dfont.h"
#include "GameTextControl.h"
#include "ItemImage.h"
#include "InnerInterface.h"
#include "ItemMove.h"
#include "BasicComboBox.h"
#include "BasicComboBoxRollOver.h"
#include "UITextControl.h"
#include "GLGaeaClient.h"
#include "BasicTextButton.h"
#include "PetQuickSkillSlot.h"
#include "GLItemMan.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"

#include ".\summonwindow.h"

const D3DCOLOR CSummonWindow::dwDEFAULT_TRANSPARENCY = D3DCOLOR_ARGB ( 160, 255, 255, 255 );;
const D3DCOLOR CSummonWindow::dwFULL_TRANSPARENCY = D3DCOLOR_ARGB ( 255, 255, 255, 255 );;

CSummonWindow::CSummonWindow(void) :
					m_pNameText(NULL),
					m_pLevelText(NULL),

					m_pDamageText(NULL),
					m_pDefenseText(NULL),
					m_pHitText(NULL),
					m_pDodgeText(NULL),

					m_pRegFireText(NULL),
					m_pRegColdText(NULL),
					m_pRegElectricText(NULL),
					m_pRegPoisonText(NULL),
					m_pRegSpritsText(NULL),

					m_pPosionBack(NULL),
					m_pPosionDisplay(NULL),
					m_pPosionImage(NULL),

					m_pMouseOver(NULL),
					m_pAmountText(NULL),

					m_pHPText(NULL),

					m_pHP(NULL),

					m_sPosionID(NATIVEID_NULL())
{
}

CSummonWindow::~CSummonWindow(void)
{
}


void CSummonWindow::CreateSubControl()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG/*D3DFONT_SHADOW | D3DFONT_ASCII*/ );
	CD3DFontPar* pFont2 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CreateControl ( "SUMMON_BASIC_BACK_L" );
	CreateControl ( "SUMMON_STATE_BACK_L" );
	CreateControl ( "SUMMON_REGSITANCE_BACK_L" );
	CreateControl ( "SUMMON_BASIC_BACK_R" );
	CreateControl ( "SUMMON_STATE_BACK_R" );
	CreateControl ( "SUMMON_REGSITANCE_BACK_R" );


	CBasicTextBox*	  pTextStatic = NULL;
	// 기본 정보
	pTextStatic = CreateStaticControl ( "SUMMON_BASIC_STATIC", pFont, TEXT_ALIGN_CENTER_X );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",0), NS_UITEXTCOLOR::PALEGOLDENROD );
	// 이름
	pTextStatic = CreateStaticControl ( "SUMMON_NAME_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",1), NS_UITEXTCOLOR::SILVER );
	// 레벨
	pTextStatic = CreateStaticControl ( "SUMMON_LEVEL_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",2), NS_UITEXTCOLOR::SILVER );

	// 상태
	pTextStatic = CreateStaticControl ( "SUMMON_STATE_STATIC", pFont, TEXT_ALIGN_CENTER_X );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",3), NS_UITEXTCOLOR::PALEGOLDENROD );
	// HP
	pTextStatic = CreateStaticControl ( "SUMMON_HP_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",4), NS_UITEXTCOLOR::SILVER );
	// 타격력
	pTextStatic = CreateStaticControl ( "SUMMON_DAMAGE_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",5), NS_UITEXTCOLOR::SILVER );
	// 방어력
	pTextStatic = CreateStaticControl ( "SUMMON_DEFENSE_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",6), NS_UITEXTCOLOR::SILVER );
	// 명중률
	pTextStatic = CreateStaticControl ( "SUMMON_HIT_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",7), NS_UITEXTCOLOR::SILVER );
	// 회피율
	pTextStatic = CreateStaticControl ( "SUMMON_DODGE_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",8), NS_UITEXTCOLOR::SILVER );

	// 저항력
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_STATIC", pFont, TEXT_ALIGN_CENTER_X );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",9), NS_UITEXTCOLOR::PALEGOLDENROD );
	// 화기저항
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_FIRE_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",10), NS_UITEXTCOLOR::SILVER );
	// 냉기저항
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_COLD_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",11), NS_UITEXTCOLOR::SILVER );
	// 전기저항
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_ELECTRIC_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",12), NS_UITEXTCOLOR::SILVER );
	// 독저항
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_POISON_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",13), NS_UITEXTCOLOR::SILVER );
	// 기저항
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_SPRITS_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",14), NS_UITEXTCOLOR::SILVER );


	// 체력회복제 지정
	pTextStatic = CreateStaticControl ( "SUMMON_POSION_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",15), NS_UITEXTCOLOR::WHITE );


	m_pNameText		= CreateStaticControl ( "SUMMON_NAME_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pLevelText	= CreateStaticControl ( "SUMMON_LEVEL_TEXT", pFont, TEXT_ALIGN_LEFT );

	m_pHP = new CBasicProgressBar;
	m_pHP->CreateSub ( this, "SUMMON_HP" );
	m_pHP->CreateOverImage ( "SUMMON_HP_WINDOW_OVERIMAGE" );	
	m_pHP->SetControlNameEx ( "SUMMON HP 프로그래스바" );
	RegisterControl ( m_pHP );

	m_pDamageText	= CreateStaticControl ( "SUMMON_DAMAGE_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pDefenseText	= CreateStaticControl ( "SUMMON_DEFENSE_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pHitText		= CreateStaticControl ( "SUMMON_HIT_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pDodgeText	= CreateStaticControl ( "SUMMON_DODGE_TEXT", pFont, TEXT_ALIGN_LEFT );

	m_pRegFireText	   = CreateStaticControl ( "SUMMON_REGSITANCE_FIRE_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pRegColdText	   = CreateStaticControl ( "SUMMON_REGSITANCE_COLD_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pRegElectricText = CreateStaticControl ( "SUMMON_REGSITANCE_ELECTRIC_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pRegPoisonText   = CreateStaticControl ( "SUMMON_REGSITANCE_POISON_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pRegSpritsText   = CreateStaticControl ( "SUMMON_REGSITANCE_SPRITS_TEXT", pFont, TEXT_ALIGN_LEFT );

	m_pHPText = CreateStaticControl ( "SUMMON_HP_TEXT", pFont2, TEXT_ALIGN_CENTER_X );


	m_pPosionBack = CreateControl( "POSION_IMAGE" );
	m_pPosionBack->SetVisibleSingle( FALSE );

	m_pPosionImage = CreateItemImage( "POSION_IMAGE", POSION_IMAGE );
	m_pPosionDisplay = CreateControl( "POSION_OVER_IMAGE" );
	m_pPosionDisplay->SetVisibleSingle( FALSE );

	m_pMouseOver = CreateControl( "ITEM_MOUSE_OVER" );
	m_pMouseOver->SetVisibleSingle( FALSE );

	m_pAmountText = CreateStaticControl ( "SUMMON_POTION_SLOT_AMOUNT_SINGLE", pFont2, TEXT_ALIGN_LEFT );


	{
		CBasicTextButton *pSearchButton1;
		pSearchButton1 = CreateTextButton ( "SUMMON_CONFIRM_BUTTON", SUMMON_CONFIRM_BUTTON, const_cast<char*>(ID2GAMEWORD("SUMMON_WINDOW_TEXT",16)) );
		pSearchButton1->SetShortcutKey ( DIK_ESCAPE );
	}

}

void CSummonWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		if( !GLGaeaClient::GetInstance().GetSummonClient() ) return;
		PCROWDATA pSummonData = GLGaeaClient::GetInstance().GetSummonClient()->m_pSummonCrowData;
		if( pSummonData )
		{
			CString strTemp;
			strTemp = pSummonData->GetName();
			m_pNameText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_wLevel );
			m_pLevelText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );

			strTemp.Format( "%d - %d", pSummonData->m_sCrowAttack[0].sDamage.wLow, pSummonData->m_sCrowAttack[0].sDamage.wHigh );
			m_pDamageText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_wDefense );
			m_pDefenseText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_wHitRate );
			m_pHitText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_wAvoidRate );
			m_pDodgeText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );

			strTemp.Format( "%d", pSummonData->m_sResist.nFire );
			m_pRegFireText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_sResist.nIce );
			m_pRegColdText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_sResist.nElectric );
			m_pRegElectricText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_sResist.nPoison );
			m_pRegPoisonText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_sResist.nPoison );
			m_pRegPoisonText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_sResist.nSpirit );
			m_pRegSpritsText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			int curHP = GLGaeaClient::GetInstance().GetSummonClient()->GETHP();
			int maxHP = GLGaeaClient::GetInstance().GetSummonClient()->GETMAXHP();
			strTemp.Format( "%d/%d", curHP, maxHP );
			m_pHPText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );

			GLDWDATA dwHP = GLGaeaClient::GetInstance().GetSummonClient()->GetHp();

			const float fPercent = float(dwHP.dwNow) / float(dwHP.dwMax);
			if ( m_pHP ) m_pHP->SetPercent ( fPercent );

			m_pPosionBack->SetVisibleSingle( TRUE );
		}		
	}else
	{
		CInnerInterface::GetInstance().HideGroup( SUMMON_WINDOW );
	}
}

CItemImage*	CSummonWindow::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage;
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}


CBasicTextButton* CSummonWindow::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pTextButton );
	return pTextButton;
}

CBasicTextBox* CSummonWindow::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void CSummonWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{

	if( GLGaeaClient::GetInstance().GetSummonClient() == NULL || 
		GLGaeaClient::GetInstance().GetSummonClient()->IsVALID() == FALSE )
	{
		SetVisibleSingle( FALSE );
		return;
	}

	if( m_pMouseOver ) m_pMouseOver->SetVisibleSingle ( FALSE );

	//	스냅, 스킬 이미지 붙이기
	CItemMove* pItemMove = CInnerInterface::GetInstance().GetItemMove ();
	SNATIVEID sHOLD_ITEM_ID(false);
	if ( pItemMove ) sHOLD_ITEM_ID = pItemMove->GetItem();

	SNATIVEID & sPosionOldID = m_sPosionID;
	SNATIVEID & sPosionID = GLGaeaClient::GetInstance().GetSummonClient()->GetPosionItem();

	if ( sPosionID != NATIVEID_NULL() )
	{
		LoadItem ( sPosionID );
	}
	else
	{
		UnLoadItem ();
	}

	m_pPosionDisplay->SetVisibleSingle ( FALSE );

	if( sHOLD_ITEM_ID != NATIVEID_NULL() )
	{
		// 장착될 위치 표시
		if( GLGaeaClient::GetInstance().GetSummonClient()->CheckPosionItem( sHOLD_ITEM_ID ) )
		{
			const UIRECT& rcImagePos = m_pPosionImage->GetGlobalPos ();
			D3DXVECTOR2 vPos( rcImagePos.left, rcImagePos.top );

			m_pPosionDisplay->SetVisibleSingle( TRUE );
		}
	}

	int curHP = GLGaeaClient::GetInstance().GetSummonClient()->GETHP();
	int maxHP = GLGaeaClient::GetInstance().GetSummonClient()->GETMAXHP();
	CString strTemp;
	strTemp.Format( "%d/%d", curHP, maxHP );
	m_pHPText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );

	GLDWDATA dwHP = GLGaeaClient::GetInstance().GetSummonClient()->GetHp();
	const float fPercent = float(dwHP.dwNow) / float(dwHP.dwMax);
	if ( m_pHP ) m_pHP->SetPercent ( fPercent );

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSummonWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SUMMON_CONFIRM_BUTTON:
	case ET_CONTROL_BUTTON:
		if( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			CInnerInterface::GetInstance().HideGroup( GetWndID() );
		}
		break;
	case POSION_IMAGE:
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			CItemMove* pItemMove = CInnerInterface::GetInstance().GetItemMove ();

			if ( pItemMove->GetItem () != NATIVEID_NULL () )
			{
				const UIRECT& rcSlotPos = m_pPosionImage->GetGlobalPos ();
				pItemMove->SetGlobalPos ( rcSlotPos );						

				AddMessageEx ( UIMSG_MOUSEIN_SUMMON_POSIONSLOT | UIMSG_TOTOPPARENT );
			}				

			//	마우스 표시 테두리
			if ( pItemMove->GetItem () == NATIVEID_NULL () && m_pPosionImage->GetItem () != NATIVEID_NULL () )
			{
				const UIRECT& rcImagePos = m_pPosionImage->GetGlobalPos ();
				m_pMouseOver->SetGlobalPos ( rcImagePos );
				m_pMouseOver->SetVisibleSingle ( TRUE );
			}

			if ( dwMsg & UIMSG_LB_UP )
			{
				GLGaeaClient::GetInstance().GetSummonClient()->ReqSummonSetPosion();
			}

			if ( dwMsg & UIMSG_RB_UP )
			{
				GLGaeaClient::GetInstance().GetSummonClient()->ReqPosionRemove();
			}

			if( m_sPosionID != NATIVEID_NULL () )
			{
				SITEMCUSTOM sItemCustom = SITEMCUSTOM( m_sPosionID.dwID );
				CInnerInterface::GetInstance().SHOW_ITEM_INFO ( sItemCustom, FALSE, FALSE, FALSE, USHRT_MAX, USHRT_MAX );
			}
		}
		break;
	}
}

void CSummonWindow::LoadItem( SNATIVEID& ref_sItemID )
{
	m_sPosionID = ref_sItemID;

	SITEM* pItemData = GLItemMan::GetInstance().GetItem ( m_sPosionID );
	m_pPosionImage->SetItem ( pItemData->sBasicOp.sICONID, pItemData->GetInventoryFile(), pItemData->sBasicOp.sNativeID );

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter ();
	DWORD dwAmount = pCharacter->m_cInventory.CountTurnItem( m_sPosionID );

	CString strNumber;
	strNumber.Format ( "%d", dwAmount );
	if ( m_pAmountText ) m_pAmountText->SetOneLineText ( strNumber, NS_UITEXTCOLOR::WHITE );

	if ( dwAmount <= 0 )	m_pPosionImage->SetDiffuse ( dwDEFAULT_TRANSPARENCY );
	else					m_pPosionImage->SetDiffuse ( dwFULL_TRANSPARENCY );

	m_pAmountText->SetVisibleSingle( TRUE );
}

void CSummonWindow::UnLoadItem ()
{
	m_sPosionID	= NATIVEID_NULL ();
	m_pPosionImage->ResetItem ();
	m_pAmountText->SetVisibleSingle( FALSE );
}
#include "pch.h"
#include ".\skilltraytab.h"

#include "d3dfont.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "BasicSkillTray.h"
#include "GameTextControl.h"
#include "InnerInterface.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"
#include "BasicTextButton.h"
#include "BasicLevelDisplay.h"

INT CSkillTrayTab::NEXT_INDEX[] = {1, 2, 0};

CSkillTrayTab::CSkillTrayTab(void)
	: m_nTabIndex(0)
	, m_nCount(0)
{
	memset( m_pSkillTray, 0, sizeof( CBasicSkillTray* ) * MAX_TAB_INDEX );
	memset( m_pTextButton, 0, sizeof( CBasicTextButton* ) * MAX_TAB_INDEX );
}

CSkillTrayTab::~CSkillTrayTab(void)
{
}

void CSkillTrayTab::CreateSubControl()
{
	CD3DFontPar * m_pFont8 = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

/* 
#ifdef CH_PARAM // 중국 인터페이스 변경
	CUIControl* pTrayLeft = new CUIControl;
	pTrayLeft->CreateSub( this, "BASIC_QUICK_SKILL_TRAY_LEFT" );
	RegisterControl( pTrayLeft );

	CBasicLevelDisplay* pLevelDisplay = new CBasicLevelDisplay;
	pLevelDisplay->CreateSub ( this, "BASIC_LEVEL_DISPLAY", UI_FLAG_DEFAULT, BASIC_LEVEL_DISPLAY );
	pLevelDisplay->CreateSubControl ();
	RegisterControl ( pLevelDisplay );
#endif
*/
	CBasicSkillTray * pSkillTray = new CBasicSkillTray;
	pSkillTray->CreateSub( this, "BASIC_QUICK_SKILL_TRAY", UI_FLAG_DEFAULT, BASIC_QUICK_SKILL_TRAY_F2 );
	pSkillTray->CreateSubControl( 0 );
	RegisterControl( pSkillTray );
	m_pSkillTray[0] = pSkillTray;

	pSkillTray = new CBasicSkillTray;
	pSkillTray->CreateSub( this, "BASIC_QUICK_SKILL_TRAY", UI_FLAG_DEFAULT, BASIC_QUICK_SKILL_TRAY_F3 );
	pSkillTray->CreateSubControl( 1 );
	pSkillTray->SetVisibleSingle( FALSE );
	RegisterControl( pSkillTray );
	m_pSkillTray[1] = pSkillTray;

	pSkillTray = new CBasicSkillTray;
	pSkillTray->CreateSub( this, "BASIC_QUICK_SKILL_TRAY", UI_FLAG_DEFAULT, BASIC_QUICK_SKILL_TRAY_F4 );
	pSkillTray->CreateSubControl( 2 );
	pSkillTray->SetVisibleSingle( FALSE );
	RegisterControl( pSkillTray );
	m_pSkillTray[2] = pSkillTray;

	CBasicTextButton * pTextButton = new CBasicTextButton;
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, BASIC_TEXT_BUTTON_F2 );
	pTextButton->CreateBaseButton( "QUICK_SKILL_TRAY_TAB_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, ID2GAMEWORD( "SKILL_TAB_INFO", 0 ) );
	RegisterControl( pTextButton );
	m_pTextButton[0] = pTextButton;
	SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );

	pTextButton = new CBasicTextButton;
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, BASIC_TEXT_BUTTON_F3 );
	pTextButton->CreateBaseButton( "QUICK_SKILL_TRAY_TAB_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, ID2GAMEWORD( "SKILL_TAB_INFO", 1 ) );
	pTextButton->SetVisibleSingle( FALSE );
	RegisterControl( pTextButton );
	m_pTextButton[1] = pTextButton;
	SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );

	pTextButton = new CBasicTextButton;
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, BASIC_TEXT_BUTTON_F4 );
	pTextButton->CreateBaseButton( "QUICK_SKILL_TRAY_TAB_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, ID2GAMEWORD( "SKILL_TAB_INFO", 2 ) );
	pTextButton->SetVisibleSingle( FALSE );
	RegisterControl( pTextButton );
	m_pTextButton[2] = pTextButton;
	SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );

// #ifndef CH_PARAM // 중국 인터페이스 변경
	CBasicButton * pButton = new CBasicButton;
	pButton->CreateSub( this, "QUICK_SKILL_TRAY_CLOSE_BUTTON", UI_FLAG_DEFAULT, QUICK_SKILL_TRAY_CLOSE_BUTTON );
	pButton->CreateFlip( "QUICK_SKILL_TRAY_CLOSE_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );	
	RegisterControl( pButton );
// #endif
}

void CSkillTrayTab::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( CInnerInterface::GetInstance().IsVisibleGroup( MODAL_WINDOW ) ) return ;

	INT nOldIndex = GetTabIndex();
	INT nNewIndex( nOldIndex );

	if( UIKeyCheck::GetInstance()->Check( DIK_F1, DXKEY_DOWN ) )	
		nNewIndex = 0;
	if( UIKeyCheck::GetInstance()->Check( DIK_F2, DXKEY_DOWN ) )	
		nNewIndex = 1;
	if( UIKeyCheck::GetInstance()->Check( DIK_F3, DXKEY_DOWN ) )	
		nNewIndex = 2;

	if( nNewIndex == nOldIndex ) return ;
	else
	{
		m_pSkillTray[nOldIndex]->SetVisibleSingle( FALSE );
		m_pSkillTray[nNewIndex]->SetVisibleSingle( TRUE );
		m_pTextButton[nOldIndex]->SetVisibleSingle( FALSE );
		m_pTextButton[nNewIndex]->SetVisibleSingle( TRUE );
		SetTabIndex( nNewIndex );
	}
}

void CSkillTrayTab::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case QUICK_SKILL_TRAY_CLOSE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					CInnerInterface::GetInstance().HideGroup ( GetWndID () );
					CInnerInterface::GetInstance().ShowGroupBottom ( QUICK_SKILL_TRAY_OPEN_BUTTON, true );
				}
			}
		}
		break;

	case BASIC_TEXT_BUTTON_F2:
	case BASIC_TEXT_BUTTON_F3:
	case BASIC_TEXT_BUTTON_F4:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_UP_LIKE( dwMsg ) )
				{
					INT nIndex = GetTabIndex();
					INT nNextIndex = NEXT_INDEX[nIndex];

					m_pSkillTray[nIndex]->SetVisibleSingle( FALSE );
					m_pSkillTray[nNextIndex]->SetVisibleSingle( TRUE );

					m_pTextButton[nIndex]->SetVisibleSingle( FALSE );
					m_pTextButton[nNextIndex]->SetVisibleSingle( TRUE );

					SetTabIndex( nNextIndex );
				}
			}
		}
		break;

	case BASIC_QUICK_SKILL_TRAY_F2:
	case BASIC_QUICK_SKILL_TRAY_F3:
	case BASIC_QUICK_SKILL_TRAY_F4:
		{
			if( dwMsg & UIMSG_MOUSEIN_SKILLSLOT )
			{
				AddMessageEx ( UIMSG_MOUSEIN_SKILLSLOT );
			}
		}
		break;
	}
}

void CSkillTrayTab::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if( bVisible )
	{
		m_pSkillTray[0]->SetVisibleSingle( FALSE );
		m_pSkillTray[1]->SetVisibleSingle( FALSE );
		m_pSkillTray[2]->SetVisibleSingle( FALSE );

		m_pTextButton[0]->SetVisibleSingle( FALSE );
		m_pTextButton[1]->SetVisibleSingle( FALSE );
		m_pTextButton[2]->SetVisibleSingle( FALSE );

		INT nIndex = GetTabIndex();
		m_pSkillTray[nIndex]->SetVisibleSingle( TRUE );
		m_pTextButton[nIndex]->SetVisibleSingle( TRUE );
	}
}

void CSkillTrayTab::SetShotcutText ( DWORD nID, CString& strTemp )
{
	m_pSkillTray[0]->SetShotcutText( nID, strTemp );
	m_pSkillTray[1]->SetShotcutText( nID, strTemp );
	m_pSkillTray[2]->SetShotcutText( nID, strTemp );
}
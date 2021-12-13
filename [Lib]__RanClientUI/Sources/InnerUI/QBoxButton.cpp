#include "pch.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "GLGaeaClient.h"
#include "GLCharacter.h"
#include "InnerInterface.h"
#include ".\qboxbutton.h"
#include "GameTextControl.h"
#include "UITextControl.h"



CQBoxButton::CQBoxButton(void) :
	m_pQBoxButton(NULL),
	m_bQBoxEnable(TRUE),
	m_bQBoxAlarm(FALSE),
	m_fBLINK_TIME(0.0f)
{
}

CQBoxButton::~CQBoxButton(void)
{
}

void CQBoxButton::SetQBoxEnable( bool bQBoxEnable )
{
	if( m_bQBoxEnable != bQBoxEnable )
	{
		GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
		if( pCharacter->IsPartyMem() && pCharacter->IsPartyMaster() == FALSE )
		{
			m_bQBoxAlarm = TRUE;
		}
	}

	m_bQBoxEnable = bQBoxEnable;
	m_pQBoxButton->SetFlip( m_bQBoxEnable );
}


void CQBoxButton::CreateSubControl ()
{
	m_pQBoxButton = new CBasicButton;
	m_pQBoxButton->CreateSub ( this, "QBOX_BUTTON_BUTTON", UI_FLAG_DEFAULT, QBOX_ON_OFF_BUTTON );
	m_pQBoxButton->CreateFlip ( "QBOX_BUTTON_BUTTON_F", CBasicButton::RADIO_FLIP );
	m_pQBoxButton->SetUseGlobalAction ( TRUE );
	m_pQBoxButton->SetFlip( m_bQBoxEnable );
	RegisterControl ( m_pQBoxButton );

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, "QBOX_ALARM_BLINK" );
	pControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pControl );
	m_pAlarmBlink = pControl;
}

void CQBoxButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	const float fBLINK_TIME_LOOP = 0.2f;

	if ( m_bQBoxAlarm )
	{		
		m_fBLINK_TIME += fElapsedTime;
		if ( fBLINK_TIME_LOOP <= m_fBLINK_TIME )
		{
			BOOL bVisible = m_pAlarmBlink->IsVisible ();
			m_pAlarmBlink->SetVisibleSingle ( !bVisible );
			m_fBLINK_TIME = 0.0f;
		}
	}
}


void CQBoxButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case QBOX_ON_OFF_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO( ID2GAMEINTEXT( "QBOX_BUTTON_MESSAGE" ), NS_UITEXTCOLOR::WHITE  );				
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
				if( !pCharacter->IsPartyMem() || pCharacter->IsPartyMaster() )
				{
					m_bQBoxEnable = !m_bQBoxEnable;
					m_pQBoxButton->SetFlip( m_bQBoxEnable );
					if( pCharacter->IsPartyMaster() )
					{
						pCharacter->ReqQBoxEnableState(m_bQBoxEnable);		
					}
				}else{
					m_bQBoxAlarm = FALSE;
					m_pAlarmBlink->SetVisibleSingle ( FALSE );
				}
			}
		}
		break;
	}
}

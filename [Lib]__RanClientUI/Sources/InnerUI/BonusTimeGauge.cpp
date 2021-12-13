#include "pch.h"
#include "BonusTimeGauge.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "BasicLineBoxEx.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"
#include "GameTextControl.h"
#include "UITextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBonusTimeGauge::CBonusTimeGauge ()
	: m_pConsume ( NULL )
	, m_pCharge ( NULL ) 
	, m_bCharging ( FALSE )
{
}

CBonusTimeGauge::~CBonusTimeGauge ()
{
}

void CBonusTimeGauge::CreateSubControl ()
{
	CBasicProgressBar* pHP = new CBasicProgressBar;
	pHP->CreateSub ( this, "BONUSTIME_CHARGE_IMAGE" );
	pHP->CreateOverImage ( "BONUSTIME_CHARGE_OVERIMAGE" );	
	RegisterControl ( pHP );
	m_pCharge = pHP;	

	pHP = new CBasicProgressBar;
	pHP->CreateSub ( this, "BONUSTIME_CONSUME_IMAGE" );
	pHP->CreateOverImage ( "BONUSTIME_CONSUME_OVERIMAGE" );	
	RegisterControl ( pHP );
	m_pConsume = pHP;	


	CUIControl* pImage = new CUIControl;
	pImage->CreateSub ( this, "BONUSTIME_IMAGE" );	
	pImage->SetUseRender ( TRUE );
	pImage->SetVisibleSingle ( TRUE );
	RegisterControl ( pImage );
}

void CBonusTimeGauge::SetTimeCharge ( int nNOW, int nMAX )
{
	const float fPercent = float(nNOW) / float(nMAX);
	m_pCharge->SetPercent ( fPercent );
}

void CBonusTimeGauge::SetTimeConsume ( int nNOW, int nMAX )
{
	const float fPercent = float(nNOW) / float(nMAX);
	m_pConsume->SetPercent ( fPercent );
}

void CBonusTimeGauge::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_bCharging ) 
	{		
		const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();
		SetTimeCharge( sCharData.m_EventStartTime - sCharData.m_RemainEventTime, sCharData.m_EventStartTime );
		
	}
	else
	{	
        const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();
		SetTimeConsume( sCharData.m_RemainBusterTime, sCharData.m_EventBusterTime );
	}

	if ( CHECK_MOUSE_IN ( GetMessageEx () ) )
	{
		if ( CInnerInterface::GetInstance().BEGIN_COMMON_LINEINFO_MULTI() )
		{
			CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI( ID2GAMEINTEXT("BONUS_TIME_MESSAGE",0), NS_UITEXTCOLOR::WHITE  );
			CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI( ID2GAMEINTEXT("BONUS_TIME_MESSAGE",1), NS_UITEXTCOLOR::WHITE  );
			CInnerInterface::GetInstance().END_COMMON_LINEINFO_MULTI();
		}
	}
}

void CBonusTimeGauge::BONUS_TIME_EVENT_START( bool bCharging )
{
	SetVisibleSingle ( TRUE );
	m_bCharging = bCharging;
	m_pCharge->SetVisibleSingle( m_bCharging );	
	m_pConsume->SetVisibleSingle( !m_bCharging );	
}

void CBonusTimeGauge::BONUS_TIME_EVENT_END()
{
	SetVisibleSingle ( FALSE );
}

void CBonusTimeGauge::BONUS_TIME_BUSTER_START()
{
	m_bCharging = FALSE;
	m_pConsume->SetPercent( 1.0f );
	m_pConsume->SetVisibleSingle( TRUE );
	m_pCharge->SetVisibleSingle( FALSE );	
	
}

void CBonusTimeGauge::BONUS_TIME_BUSTER_END()
{
	m_bCharging = TRUE;
	m_pCharge->SetPercent( 0.0f );
	m_pCharge->SetVisibleSingle( TRUE );
	m_pConsume->SetVisibleSingle( FALSE );	
}
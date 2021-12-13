#include "pch.h"

#include "PartyBuffInfoUnit.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "SkillImage33.h"
#include "InnerInterface.h"
#include "UITextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPartyBuffInfoUnit::CPartyBuffInfoUnit ()
{
}

CPartyBuffInfoUnit::~CPartyBuffInfoUnit ()
{
}

void CPartyBuffInfoUnit::CreateSubControl ()
{
	CSkillImage33* pSkillImage = new CSkillImage33;
	pSkillImage->CreateSub ( this, "PARTYBUFF_IMAGE" );	
	pSkillImage->SetUseRender ( TRUE );
	RegisterControl ( pSkillImage );
	m_pSkillImage = pSkillImage;

	CBasicProgressBar* pProgressBar = new CBasicProgressBar;
	pProgressBar->CreateSub ( this, "PARTYBUFF_PROGRESS" );
	pProgressBar->CreateOverImage ( "PARTYBUFF_PROGRESS_OVERIMAGE" );
	pProgressBar->SetType ( CBasicProgressBar::VERTICAL );
	RegisterControl ( pProgressBar );
	m_pProgressBar = pProgressBar;
}

void CPartyBuffInfoUnit::SetLeftTime ( const float& fLeftTime )
{
	m_fLEFT_TIME = fLeftTime;
}

void CPartyBuffInfoUnit::SetSkill ( const DWORD& dwID, const float& fLifeTime, const CString& strSkillName )
{
	m_fLIFE_TIME = fLifeTime;
	m_pSkillImage->SetSkill ( dwID );
	m_pSkillImage->SetVisibleSingle ( TRUE );

	m_strSkillName = strSkillName;
}

void CPartyBuffInfoUnit::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	if ( m_pProgressBar )
	{
		const float fPercent = m_fLEFT_TIME/m_fLIFE_TIME;
		m_pProgressBar->SetPercent ( fPercent );
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( CHECK_MOUSE_IN ( GetMessageEx () ) )
	{
		CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( m_strSkillName, NS_UITEXTCOLOR::GREENYELLOW );
	}
}
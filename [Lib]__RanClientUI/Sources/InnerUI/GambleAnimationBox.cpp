#include "pch.h"

#include "GambleAnimationBox.h"

#include "d3dfont.h"
#include "DxGlobalStage.h"
#include "GameTextControl.h"
#include "GLCharacter.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"


CGambleAnimationBox::CGambleAnimationBox(void)
:m_pControl(NULL)
,m_pAniBox(NULL)
,m_TotElapsedTime(0.0f)
{
	
}

CGambleAnimationBox::~CGambleAnimationBox(void)
{
	m_TotElapsedTime = 0.0f;
}



void CGambleAnimationBox::CreateSubControl ()
{
	/*
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub( this, "GAMBLE_ANIMATION_SHUFFLE1" );
	RegisterControl( pControl );
	m_pControl = pControl;
	*/

	CBasicAnimationBox* pAnibox = new CBasicAnimationBox;
    pAnibox->CreateSubEx(this,"GAMBLE_ANIMATION_DIALOGUE",GAMBLE_ANIMATION_DIALOGUE);
	pAnibox->CreateAni("GAMBLE_ANIMATION_SHUFFLE1", CBasicAnimationBox::NORMAL_ANI);
	pAnibox->AddAni("GAMBLE_ANIMATION_SHUFFLE2");
	pAnibox->SetAniTime(0.1f);
	pAnibox->SetEndTime(MINIGAME_ODDEVEN::fShuffleTime);
	RegisterControl( pAnibox );
	m_pAniBox = pAnibox;
}

void CGambleAnimationBox::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		m_TotElapsedTime = 0.0f;
	}
	else
	{	
		CInnerInterface::GetInstance().HideGroup( GetWndID () );
	}
}

void CGambleAnimationBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	// 시간 경과
	m_TotElapsedTime += fElapsedTime;

	
	if ( m_TotElapsedTime > MINIGAME_ODDEVEN::fShuffleTime ) 
	{
		CInnerInterface::GetInstance().HideGroup( GetWndID () );
		m_TotElapsedTime = 0.0f;
		GLGaeaClient::GetInstance().GetCharacter()->ReqMGameOddEvenShuffle();
	}		
	
	
}


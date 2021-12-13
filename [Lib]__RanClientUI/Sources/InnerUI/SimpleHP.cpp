#include "pch.h"
#include "SimpleHP.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "BasicLineBoxEx.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSimpleHP::CSimpleHP () :
	m_pHP ( NULL ),
	m_bUseSummonHP ( FALSE )
{
}

CSimpleHP::~CSimpleHP ()
{
}

void CSimpleHP::CreateSubControl ()
{
	CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_SIMPLEHP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxTargetInfo ( "BASIC_SIMPLEHP_LINE_BOX" );
	RegisterControl ( pLineBox );


	CBasicProgressBar* pHP = new CBasicProgressBar;
	
	if( m_bUseSummonHP )
	{
		pHP->CreateSub ( this, "SUMMON_HP_IMAGE" );
		pHP->CreateOverImage ( "SUMMON_HP_OVERIMAGE" );		
	}else{
		pHP->CreateSub ( this, "SIMPLE_HP_IMAGE" );
		pHP->CreateOverImage ( "SIMPLE_HP_OVERIMAGE" );		
	}
	RegisterControl ( pHP );
	m_pHP = pHP;	
}

void CSimpleHP::SetHP ( WORD wNOW, WORD wMAX )
{
	const float fPercent = float(wNOW) / float(wMAX);
	m_pHP->SetPercent ( fPercent );
}

void CSimpleHP::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( !m_bUseSummonHP )
	{
		const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();
		SetHP ( sCharData.m_sHP.wNow, sCharData.m_sHP.wMax );
	}else{
		GLSummonClient* pSummon = GLGaeaClient::GetInstance().GetSummonClient ();
		if( pSummon && pSummon->IsVALID() )		
			SetHP ( (WORD)pSummon->GETHP(), (WORD)pSummon->GETMAXHP());
		
	}
}
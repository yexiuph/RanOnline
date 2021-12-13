#include "pch.h"
#include "CdmRankingDisplay.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "d3dfont.h"
#include "GLGaeaClient.h"
#include "UITextControl.h"
#include "GameTextControl.h"
//#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CCdmRankingDisplay::CCdmRankingDisplay()
	:	m_pCdmRankingText ( NULL )
{
	
}

CCdmRankingDisplay::~CCdmRankingDisplay()
{
}

void CCdmRankingDisplay::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	m_pCdmRankingText = new CBasicTextBox;
	m_pCdmRankingText->CreateSub ( this, "CDM_RANKING_TEXT" );
	m_pCdmRankingText->SetFont ( pFont );
	m_pCdmRankingText->SetTextAlign ( TEXT_ALIGN_LEFT );
	RegisterControl ( m_pCdmRankingText );	
}

void CCdmRankingDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCdmRankingDisplay::RefreashCdmRanking()
{
	if ( !m_pCdmRankingText ) return;

	m_pCdmRankingText->ClearText();

	const SCDM_RANK_EX& sMyRank = GLGaeaClient::GetInstance().m_sMyCdmRank;

	CString strMyRanking;
	strMyRanking.Format( "%2d%s %s:%d %s:%d", sMyRank.wClubRanking, ID2GAMEWORD( "CDM_RANKING_DISPLAY", 0 ),
						ID2GAMEWORD( "CDM_RANKING_DISPLAY", 1),  sMyRank.wKillNum, 
						ID2GAMEWORD( "CDM_RANKING_DISPLAY", 2), sMyRank.wDeathNum );
	
	m_pCdmRankingText->AddText( strMyRanking, NS_UITEXTCOLOR::LIGHTSKYBLUE );

//	const SCDM_RANK_EX& sMyRank = GLGaeaClient::GetInstance().m_sMyCdmRank;
	int nRankNum = GLGaeaClient::GetInstance().m_vecCdmRank.size();
	CString strRanking;

	CDM_RANK_VEC& vecCdmRank = GLGaeaClient::GetInstance().m_vecCdmRank;

	for ( int i = 0; i < nRankNum; ++i )
	{
		strRanking.Format( "%2d%s %s (%d/%d)", vecCdmRank[i].wClubRanking, 
							ID2GAMEWORD( "CDM_RANKING_DISPLAY", 0 ), vecCdmRank[i].szClubName
							,vecCdmRank[i].wKillNum, vecCdmRank[i].wDeathNum );

		m_pCdmRankingText->AddText( strRanking, NS_UITEXTCOLOR::LIGHTGREY );
	}
}
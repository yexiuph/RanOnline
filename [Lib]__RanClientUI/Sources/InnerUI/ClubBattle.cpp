#include "pch.h"
#include "ClubBattle.h"

#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
#include "BasicLineBox.h"
#include "InnerInterface.h"
#include "ModalWindow.h"
#include "UITextControl.h"
#include "ModalCallerID.h"
#include "BasicChat.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"
#include "GLGaeaClient.h"
#include "DxClubMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CClubBattle::CClubBattle ()
	: m_pTextBox(NULL)
	, m_pScrollBarEx(NULL)
	, m_nSelectIndex(-1)
	, m_bSelAlliance(false)
{
}

CClubBattle::~CClubBattle ()
{
}

void CClubBattle::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{	//	뒷 배경
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "CLUB_LIST_LINEBOX" );
		RegisterControl ( pBasicLineBox );		
	}

	//	텍스트 박스
	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx;
		pTextBox->CreateSub ( this, "CLUB_BATTLE_LIST_TEXTBOX", UI_FLAG_DEFAULT, CLUB_LIST_TEXTBOX );
		pTextBox->SetFont ( pFont );
		pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pTextBox = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, CLUB_LIST_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "CLUB_LIST_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pScrollBarEx = pScrollBar;
	}
}

void CClubBattle::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_pTextBox->SetUseOverColor( FALSE );
	if( IsSelectedIndex() )
		m_pTextBox->SetUseTextColor( m_nSelectIndex, FALSE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( IsSelectedIndex() )
	{
		m_pTextBox->SetUseTextColor( m_nSelectIndex, TRUE );
		m_pTextBox->SetTextColor( m_nSelectIndex, NS_UITEXTCOLOR::ORANGE );
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBarEx->GetThumbFrame ();

		const int nTotalLine = m_pTextBox->GetTotalLine ();
		const int nLinePerOneView = m_pTextBox->GetVisibleLine ();
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pTextBox->SetCurLine ( nPos );			
		}
	}
}

void CClubBattle::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case CLUB_LIST_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const int nIndex = m_pTextBox->GetSelectPos ();
				if( nIndex < 0 || m_pTextBox->GetCount () <= nIndex ) return;

				m_pTextBox->SetUseOverColor ( TRUE );
				m_pTextBox->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

				ShowClubBattleInfo( nIndex );

				if( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectIndex = nIndex;

					const DWORD dwClubaID = m_pTextBox->GetTextData( m_nSelectIndex );

					GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter()->m_sCLUB;

					CLUB_BATTLE_ITER iter = sCLUB.m_mapBattle.find( dwClubaID );

					GLCLUBBATTLE& sClubBattle = iter->second;
					m_strSelectName = sClubBattle.m_szClubName;
					m_bSelAlliance = sClubBattle.m_bAlliance;
				}
			}
		}
		break;
	}
}

void CClubBattle::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		LoadClubBattleList();
	}
}

void CClubBattle::LoadClubBattleList()
{
	
	m_pTextBox->ClearText (); // 텍스트를 모두 지운다.

	GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter()->m_sCLUB;
	if ( sCLUB.m_dwID==CLUB_NULL ) return ;

	CLUB_BATTLE_ITER iter = sCLUB.m_mapBattle.begin();
	CLUB_BATTLE_ITER iter_end = sCLUB.m_mapBattle.end();

	for ( ; iter != iter_end; ++iter )
	{
		GLCLUBBATTLE& sClubBattle = iter->second;
		const DWORD dwID = sClubBattle.m_dwCLUBID;

		CString strCombine;

		strCombine = "[VS]";
		if ( sClubBattle.m_bAlliance ) strCombine += ID2GAMEINTEXT( "CLUB_BATTLE_INFO", 4 );
		
		strCombine += " ";
		strCombine += sClubBattle.m_szClubName;

		CString strTemp;

        strTemp.Format ( ID2GAMEINTEXT( "CLUB_BATTLE_INFO", 0 ), sClubBattle.m_wKillPoint, 
						 sClubBattle.m_wKillPointTemp, sClubBattle.m_wDeathPoint, 
						 sClubBattle.m_wDeathPointTemp );
		strCombine += " ";
		strCombine += strTemp;	

		int nIndex = m_pTextBox->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );	
		m_pTextBox->SetTextData ( nIndex, dwID );
	}

	{
		m_pTextBox->SetCurLine ( 0 );

		const float fPercent = m_pScrollBarEx->GetThumbFrame()->GetPercent ();

		const int nTotal = m_pTextBox->GetCount ();
		const int nViewPerPage = m_pTextBox->GetVisibleLine ();	
		m_pScrollBarEx->GetThumbFrame()->SetState ( nTotal, nViewPerPage );
		m_pScrollBarEx->GetThumbFrame()->SetPercent ( fPercent );
	}

}

BOOL CClubBattle::IsSelectedIndex()
{
	return ( 0 <= m_nSelectIndex ) && ( m_nSelectIndex < m_pTextBox->GetCount() );
}

const DWORD CClubBattle::GetBattleClubID()
{
	if ( !IsSelectedIndex() )
		return NATIVEID_NULL().dwID;

	return m_pTextBox->GetTextData( m_nSelectIndex );
}

void CClubBattle::ShowClubBattleInfo( int nIndex )
{
	if( nIndex < 0 || m_pTextBox->GetCount () <= nIndex ) return;

	GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter()->m_sCLUB;

	int nClubBattleID = m_pTextBox->GetTextData( nIndex );

	GLCLUBBATTLE* pClubBattle = sCLUB.GetClubBattle( nClubBattleID );
	if ( !pClubBattle ) return;

	if ( CInnerInterface::GetInstance().BEGIN_COMMON_LINEINFO_MULTI() )
	{
		CString strTemp;
		CTime cTime;
		
		cTime = pClubBattle->m_tStartTime;
		strTemp.Format( ID2GAMEINTEXT("CLUB_BATTLE_INFO",1), cTime.GetYear (), cTime.GetMonth (), 
						cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () );
		CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI( strTemp, NS_UITEXTCOLOR::WHITE  );

		cTime = pClubBattle->m_tEndTime;		
		strTemp.Format( ID2GAMEINTEXT("CLUB_BATTLE_INFO",2), cTime.GetYear (), cTime.GetMonth (), 
						cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () );
		CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI( strTemp, NS_UITEXTCOLOR::WHITE  );

		CTimeSpan tDisTimeSpan(0,0,GLCONST_CHAR::dwCLUB_BATTLE_DIS_TIME,0);
		cTime = pClubBattle->m_tStartTime;		
		cTime += tDisTimeSpan;

		strTemp.Format( ID2GAMEINTEXT("CLUB_BATTLE_INFO",3), cTime.GetYear (), cTime.GetMonth (), 
						cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () );
		CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI( strTemp, NS_UITEXTCOLOR::WHITE  );

		CInnerInterface::GetInstance().END_COMMON_LINEINFO_MULTI();
	}

	return;
}
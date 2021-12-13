#include "pch.h"
#include "ClubUnion.h"

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

CClubUnion::CClubUnion ()
	: m_pTextBox(NULL)
	, m_pScrollBarEx(NULL)
	, m_nSelectIndex(-1)
{
}

CClubUnion::~CClubUnion ()
{
}

void CClubUnion::CreateSubControl ()
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
		pTextBox->CreateSub ( this, "CLUB_UNION_LIST_TEXTBOX", UI_FLAG_DEFAULT, CLUB_LIST_TEXTBOX );
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

void CClubUnion::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
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

void CClubUnion::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
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

				if( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectIndex = nIndex;

					const DWORD dwGaeaID = m_pTextBox->GetTextData( m_nSelectIndex );

					GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter ()->m_sCLUB;

					CLUB_ALLIANCE_ITER iter = sCLUB.m_setAlliance.begin();
					CLUB_ALLIANCE_ITER iter_end = sCLUB.m_setAlliance.end();

					for ( ; iter != iter_end; ++iter )
					{
						// geger009 | Temporary Fix Migrations
						//GLCLUBALLIANCE& sClubAlliance = *iter;
						GLCLUBALLIANCE sClubAlliance = *iter;

						if ( sClubAlliance.m_dwID == dwGaeaID )
						{
							m_strSelectName = sClubAlliance.m_szName;
							break;
						}
					}
				}
			}
		}
		break;
	}
}

void CClubUnion::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		LoadUnionMemberList();
	}
}

void CClubUnion::LoadUnionMemberList()
{
	m_pTextBox->ClearText (); // 텍스트를 모두 지운다.

	GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter()->m_sCLUB;
	if ( sCLUB.m_dwID==CLUB_NULL ) return ;

	CLUB_ALLIANCE_ITER iter = sCLUB.m_setAlliance.begin();
	CLUB_ALLIANCE_ITER iter_end = sCLUB.m_setAlliance.end();

	for ( ; iter != iter_end; ++iter )
	{
		// geger009 | Temporary Fix Migrations
		//GLCLUBALLIANCE& sClubAlliance = *iter;
		GLCLUBALLIANCE sClubAlliance = *iter;
		const DWORD dwID = sClubAlliance.m_dwID;

		CString strCombine;

		if( sCLUB.m_dwAlliance == dwID )
		{
			strCombine = "[M]";
		}
		else // 일반
		{
			strCombine = "[C]";
		}

		strCombine += sClubAlliance.m_szName;

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

BOOL CClubUnion::IsSelectedIndex()
{
	return ( 0 <= m_nSelectIndex ) && ( m_nSelectIndex < m_pTextBox->GetCount() );
}

const DWORD CClubUnion::GetUnionMemberID()
{
	if ( !IsSelectedIndex() )
		return NATIVEID_NULL().dwID;

	return m_pTextBox->GetTextData( m_nSelectIndex );
}
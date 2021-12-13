#include "pch.h"
#include "ClubMember.h"

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

CClubMember::CClubMember ()
	: m_nSelectIndex ( -1 )
	, m_pListText(NULL)
	, m_pListScrollBar(NULL)
{
}

CClubMember::~CClubMember ()
{
}

void CClubMember::CreateSubControl ()
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
		pTextBox->CreateSub ( this, "CLUB_LIST_TEXTBOX", UI_FLAG_DEFAULT, CLUB_LIST_TEXTBOX );
		pTextBox->SetFont ( pFont );
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetSensitive ( true );
		pTextBox->SetLimitLine ( 10000 );
		RegisterControl ( pTextBox );
		m_pListText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, CLUB_LIST_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "CLUB_LIST_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pListScrollBar = pScrollBar;
	}
}

void CClubMember::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_pListText->SetUseOverColor ( FALSE );
	if ( IsSelectedIndex() )
		m_pListText->SetUseTextColor ( m_nSelectIndex, FALSE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( IsSelectedIndex() )
	{
		m_pListText->SetUseTextColor ( m_nSelectIndex, TRUE );
		m_pListText->SetTextColor ( m_nSelectIndex, NS_UITEXTCOLOR::ORANGE );
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pListText->GetTotalLine ();
		const int nLinePerOneView = m_pListText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pListText->SetCurLine ( nPos );			
		}
	}
}

void CClubMember::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case CLUB_LIST_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const int nIndex = m_pListText->GetSelectPos ();
				if( nIndex < 0 || m_pListText->GetCount () <= nIndex ) return;

				m_pListText->SetUseOverColor ( TRUE );
				m_pListText->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

				if( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectIndex = nIndex;
				
					const DWORD dwGaeaID = m_pListText->GetTextData ( m_nSelectIndex );

					GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter ()->m_sCLUB;
					CLUBMEMBERS& mapMembers = sCLUB.m_mapMembers;
					CLUBMEMBERS_ITER iter = mapMembers.begin ();
					CLUBMEMBERS_ITER iter_end = mapMembers.end ();

					for ( ; iter != iter_end; ++iter )
					{
						m_sClubMember = (*iter).second;
						if ( m_sClubMember.dwID == dwGaeaID )
						{
							m_strSelectName = m_sClubMember.m_szName;
							break;
						}
					}
				}

				if( ( UIMSG_LB_DUP & dwMsg ) && m_sClubMember.bONLINE )
				{
					//CInnerInterface::GetInstance().PostUIMessage( GetParent()->GetWndID(), UIMSG_ADDNAME_TO_CHATBOX_FROM_CLUBWINDOW );
					GetParent()->AddMessageEx( UIMSG_ADDNAME_TO_CHATBOX_FROM_CLUBWINDOW );
				}
			}
		}
		break;
	}
}

BOOL CClubMember::IsSelectedIndex()
{
	return ( 0 <= m_nSelectIndex && m_nSelectIndex < m_pListText->GetCount() );
}

const DWORD CClubMember::GetClubMemberID()
{
	if ( !IsSelectedIndex() )
		return NATIVEID_NULL().dwID;

	return m_pListText->GetTextData ( m_nSelectIndex );
}

void CClubMember::LoadClubMemberList()
{
	m_pListText->ClearText (); // 텍스트를 모두 지운다.

	GLCLUB& sCLUB = GLGaeaClient::GetInstance().GetCharacter()->m_sCLUB;
	if ( sCLUB.m_dwID==CLUB_NULL ) return ;

	CLUBMEMBERS& mapMembers = sCLUB.m_mapMembers;
	CLUBMEMBERS_ITER iter = mapMembers.begin ();
	CLUBMEMBERS_ITER iter_end = mapMembers.end ();

	for ( ; iter != iter_end; ++iter )
	{
		GLCLUBMEMBER& sClubMember = (*iter).second;
		const bool bOnline = sClubMember.bONLINE;
		const int nChannel = sClubMember.nCHANNEL;
		const DWORD dwID = sClubMember.dwID;

		if ( bOnline )
		{
			PLANDMANCLIENT pLandClient = GLGaeaClient::GetInstance().GetActiveMap();
			if ( pLandClient )
			{	
				// 캐릭터 명의 최대 길이, 8글자 + 여백 2. 18 - 3 여기서 3은 [M]의 길이
				// 하지만 나중에 [M], 다른 글자나 이미지로 바뀌면 다시 수정해야함.
				const int nMAX_WIDTH = 18;
				const bool bSameMap = (pLandClient->GetMapID () == sClubMember.nidMAP);
				if ( bSameMap )
				{
					CString strName( sClubMember.m_szName );
					
					int nLength = nMAX_WIDTH - strName.GetLength ();
					if ( nLength < 2 ) nLength = 2;

					std::string strBlank( nLength , ' ' );

					int nPosX = 0;
					int nPosY = 0;

					GLMapAxisInfo &sMapAxisInfo = pLandClient->GetMapAxisInfo();
					sMapAxisInfo.Convert2MapPos ( sClubMember.vecPOS.x, sClubMember.vecPOS.z, nPosX, nPosY );

					CString strPos;
					strPos.Format ( "(%02d,%02d)", nPosX, nPosY );

					CString strCombine;

					if ( CInnerInterface::GetInstance().IsCHANNEL () )
					{
						strCombine.Format( "[%d]", nChannel ); // 채널이 추가된다.
					}

					if( sCLUB.IsMaster( dwID ) )
					{
						//ID2GAMEWORD("CLUB_TEXT_STATIC",2)
						strCombine.AppendFormat( "[M]%s%s%s", strName, strBlank.c_str(), strPos );
					}
					else if( sCLUB.IsSubMaster( dwID ) )
					{
						strCombine.AppendFormat( "[S]%s%s%s", strName, strBlank.c_str(), strPos );
					}
					else // 일반
					{
						strCombine.AppendFormat( "[C]%s%s%s", strName, strBlank.c_str(), strPos );
					}

					LoadClubMember ( strCombine, bOnline, dwID );	
				}
				else
				{
					CString strCombine;
					CString strName( sClubMember.m_szName );
					std::string strBlank( nMAX_WIDTH - strName.GetLength (), ' ' ); // MEMO				

					CString strMap = GLGaeaClient::GetInstance().GetMapName ( sClubMember.nidMAP );

					if ( CInnerInterface::GetInstance().IsCHANNEL () )
					{
						strCombine.Format( "[%d]", nChannel ); // 채널이 추가된다.
					}

					if( sCLUB.IsMaster( dwID ) )
					{
						strCombine.AppendFormat( "[M]%s%s%s", strName, strBlank.c_str(), strMap );
					}
					else if( sCLUB.IsSubMaster( dwID ) )
					{
						strCombine.AppendFormat( "[S]%s%s%s", strName, strBlank.c_str(), strMap );
					}
					else // 일반
					{
						strCombine.AppendFormat( "[C]%s%s%s", strName, strBlank.c_str(), strMap );
					}

					LoadClubMember ( strCombine, bOnline, dwID );
				}
			}
		}
	}

	iter = mapMembers.begin ();
	// MEMO : 온라인상태와 오프라인 상태를 구분해서 출력하기 위해 for문을 한번 더 돈다.
	for ( ; iter != iter_end; ++iter )
	{
		GLCLUBMEMBER& sClubMember = (*iter).second;
		bool bOnline = sClubMember.bONLINE;
		if ( !bOnline ) LoadClubMember ( sClubMember.m_szName, bOnline, sClubMember.dwID );
	}

	m_pListText->SetCurLine ( 0 );

	const float fPercent = m_pListScrollBar->GetThumbFrame()->GetPercent ();

	const int nTotal = m_pListText->GetCount ();
	const int nViewPerPage = m_pListText->GetVisibleLine ();	
	m_pListScrollBar->GetThumbFrame()->SetState ( nTotal, nViewPerPage );
	m_pListScrollBar->GetThumbFrame()->SetPercent ( fPercent );
}

void CClubMember::LoadClubMember( const CString& strText, const bool bOnline, const DWORD& dwGaeaID )
{
	enum
	{
		ONLINE = 0,
		OFFLINE
	};

	static	CString strKeyword[] =
	{
		"FRIEND_ONLINE",
		"FRIEND_OFFLINE"
	};

	D3DCOLOR dwTextColor = NS_UITEXTCOLOR::WHITE;
	int nIcon = ONLINE;
	if ( !bOnline )
	{
		nIcon = OFFLINE;
		dwTextColor = NS_UITEXTCOLOR::DARKGRAY;
	}

	int nIndex = m_pListText->AddText ( strText, dwTextColor );	
	m_pListText->SetTextImage ( nIndex, strKeyword[nIcon] );
	m_pListText->SetTextData ( nIndex, dwGaeaID );
}

void CClubMember::SetVisibleSingle ( BOOL bVisible ) // MEMO
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if ( bVisible )
	{
		LoadClubMemberList ();
	}
}
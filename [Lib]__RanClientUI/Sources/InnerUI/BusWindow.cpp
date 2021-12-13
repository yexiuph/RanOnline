#include "pch.h"

#include "BusWindow.h"
#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "GameTextControl.h"
#include "BasicLineBox.h"
#include "UITextControl.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"
#include "GLBusStation.h"
#include "GLBusList.h"

#include "../[Lib]__RanClient/Sources/G-Logic/Npc/NpcTalk.h"

#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBusWindow::CBusWindow () :
	m_nSelectMap ( -1 ),
	m_nSelectStop( -1 ),
	m_pMapList ( NULL ),
	m_pMapListScrollBar ( NULL ),
	m_pStopList ( NULL ),
	m_pStopListScrollBar ( NULL ),
	m_pMoveButton ( NULL ),
	m_pMapTitle ( NULL ),
    m_pStopTitle ( NULL ),
	m_pNpcTalk ( NULL )
{
}

CBusWindow::~CBusWindow ()
{
}

void CBusWindow::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CreateControl ( "BUS_TOP_BACK_L" );
	CreateControl ( "BUS_TOP_BACK_R" );
	CreateControl ( "BUS_MIDDLE_BACK_L" );
	CreateControl ( "BUS_MIDDLE_BACK_R" );
	CreateControl ( "BUS_BOTTOM_BACK_L" );
	CreateControl ( "BUS_BOTTOM_BACK_R" );

	m_pMapTitle = CreateStaticControl ( "BUS_MAP_TITLE", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMapTitle->SetText ( ID2GAMEWORD("BUS_TITLE",0) );
	m_pStopTitle = CreateStaticControl ( "BUS_STOP_TITLE", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pStopTitle->SetText ( ID2GAMEWORD("BUS_TITLE",1) );

	//	텍스트 박스
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "BUS_MAP_LIST_BACK" );
		RegisterControl ( pBasicLineBox );

		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx;
		pTextBox->CreateSub ( this, "BUS_MAP_LIST", UI_FLAG_DEFAULT, BUS_MAP_LIST );
		pTextBox->SetFont ( pFont );		
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetSensitive ( true );
		RegisterControl ( pTextBox );
		m_pMapList = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, BUS_MAP_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "BUS_MAP_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pMapListScrollBar = pScrollBar;
	}

	//	텍스트 박스
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "BUS_STOP_LIST_BACK" );
		RegisterControl ( pBasicLineBox );

		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx;
		pTextBox->CreateSub ( this, "BUS_STOP_LIST", UI_FLAG_DEFAULT, BUS_STOP_LIST );
		pTextBox->SetFont ( pFont );		
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetSensitive ( true );
		RegisterControl ( pTextBox );
		m_pStopList = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, BUS_STOP_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "BUS_STOP_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pStopListScrollBar = pScrollBar;
	}	

	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, "BUS_MOVEBUTTON", UI_FLAG_DEFAULT, BUS_MOVEBUTTON );
	pButton->CreateFlip ( "BUS_MOVEBUTTON_F", CBasicButton::MOUSEIN_FLIP );	
	RegisterControl ( pButton );
	m_pMoveButton = pButton;
}

void	CBusWindow::LoadMapList ()
{
	GASSERT( m_pNpcTalk && "Please, Call SetBusData() Function" );

	m_pMapList->ClearText ();    
	m_pStopList->ClearText ();

	const DWORD dwNum = m_glBusList.GetVillageNum();
	for ( DWORD i = 0; i < dwNum; i++ )
	{
		SVILLAGE* pVILLAGE = m_glBusList.GetVillage(i);
		if ( !pVILLAGE )
		{
			GASSERT ( 0 && "VILLAGE를 찾을수 없습니다." );
			continue;
		}

		int nIndex = m_pMapList->AddText ( pVILLAGE->strMAP.c_str() );
		m_pMapList->SetTextData ( nIndex, i );
	}
}

void	CBusWindow::LoadStopList ()
{
	if ( m_nSelectMap < 0 ) return ;

	GASSERT( m_pNpcTalk && "Please, Call SetBusData() Function" );

	const DWORD dwMapID = m_pMapList->GetTextData ( m_nSelectMap );
	SVILLAGE* pVILLAGE = m_glBusList.GetVillage( dwMapID );
	if ( !pVILLAGE )
	{
		GASSERT ( 0 && "VILLAGE를 찾을수 없습니다." );
		return ;
	}

	m_pStopList->ClearText();

	const DWORD dwNum = pVILLAGE->GetStationNum();
	for ( DWORD i = 0; i < dwNum; i++ )
	{
		const DWORD dwID = pVILLAGE->GetStation(i);
		SSTATION* pSTATION = m_glBusList.GetStation(dwID);
		if ( !pSTATION )
		{
			GASSERT ( 0 && "STATION을 찾을 수 없습니다." );
			continue;
		}
		int nIndex = m_pStopList->AddText ( pSTATION->strSTATION.c_str() );

		//if( pSTATION->dwLINKID != 0 )
			m_pStopList->SetTextData ( nIndex, pSTATION->dwLINKID ); // Note : 버스 리스트 인덱스는 0부터 시작한다.
		//else
			//m_pStopList->SetTextData ( nIndex, dwID );
	}	
}

void	CBusWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	{
		m_pMapList->SetUseOverColor ( FALSE );
		if ( 0 <= m_nSelectMap && m_nSelectMap < m_pMapList->GetCount () )
			m_pMapList->SetUseTextColor ( m_nSelectMap, FALSE );
	}

	{
		m_pStopList->SetUseOverColor ( FALSE );
		if ( 0 <= m_nSelectStop && m_nSelectStop < m_pStopList->GetCount () )
			m_pStopList->SetUseTextColor ( m_nSelectStop, FALSE );
	}

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	{
		if ( 0 <= m_nSelectMap && m_nSelectMap < m_pMapList->GetCount () )
		{
			m_pMapList->SetUseTextColor ( m_nSelectMap, TRUE );
			m_pMapList->SetTextColor ( m_nSelectMap, NS_UITEXTCOLOR::ORANGE );
		}

		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pMapListScrollBar->GetThumbFrame ();

			const int nTotalLine = m_pMapList->GetTotalLine ();
			const int nLinePerOneView = m_pMapList->GetVisibleLine ();
			CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
			pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
			if ( nLinePerOneView < nTotalLine )
			{
				const int nMovableLine = nTotalLine - nLinePerOneView;
				float fPercent = pThumbFrame->GetPercent ();
				int nPos = (int)floor(fPercent * nMovableLine);
				m_pMapList->SetCurLine ( nPos );			
			}
		}
	}

	{
		if ( 0 <= m_nSelectStop && m_nSelectStop < m_pStopList->GetCount () )
		{
			m_pStopList->SetUseTextColor ( m_nSelectStop, TRUE );
			m_pStopList->SetTextColor ( m_nSelectStop, NS_UITEXTCOLOR::ORANGE );
		}

		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pStopListScrollBar->GetThumbFrame ();

			const int nTotalLine = m_pStopList->GetTotalLine ();
			const int nLinePerOneView = m_pStopList->GetVisibleLine ();
			CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
			pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
			if ( nLinePerOneView < nTotalLine )
			{
				const int nMovableLine = nTotalLine - nLinePerOneView;
				float fPercent = pThumbFrame->GetPercent ();
				int nPos = (int)floor(fPercent * nMovableLine);
				m_pStopList->SetCurLine ( nPos );			
			}
		}
	}
}

void	CBusWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case BUS_MAP_LIST:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{				
				const int nIndex = m_pMapList->GetSelectPos ();
				if ( nIndex < 0 || m_pMapList->GetCount () <= nIndex ) return ;

				m_pMapList->SetUseOverColor ( TRUE );
				m_pMapList->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

				if ( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectMap = nIndex;	
					LoadStopList ();
				}
			}
		}
		break;

	case BUS_STOP_LIST:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{				
				const int nIndex = m_pStopList->GetSelectPos ();
				if ( nIndex < 0 || m_pStopList->GetCount () <= nIndex ) return ;

				m_pStopList->SetUseOverColor ( TRUE );
				m_pStopList->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

				if ( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectStop = nIndex;
				}
			}
		}
		break;

	case BUS_MOVEBUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_nSelectStop < 0 ) return ;
				const DWORD dwStationID = m_pStopList->GetTextData ( m_nSelectStop );
				GLGaeaClient::GetInstance().GetCharacter()->ReqBusStation (m_dwGaeaID, dwStationID);

				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;
	}
}

bool	CBusWindow::SetBusData ( const DWORD dwGaeaID, SNpcTalk* pNpcTalk )
{
	m_dwGaeaID = dwGaeaID;
	m_pNpcTalk = pNpcTalk;

	if( m_pNpcTalk->m_strBusFile.empty() )
	{
		//m_pGlBusData = &GLBusStation::GetInstance();
		GASSERT( 0 && "CBusWindow::SetBusData(), m_strBusFile is empty!" );
	}
	else
	{
		if( !m_glBusList.LOAD( m_pNpcTalk->m_strBusFile ) )
		{
			GASSERT( 0 && "CBusWindow::SetBusData(), m_glBusList.LOAD()" );
			return false;
		}
	}

	LoadMapList ();

	return true;
}
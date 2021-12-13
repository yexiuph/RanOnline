#include "pch.h"
#include "HelpWindow.h"

#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "BasicTreeEx.h"
#include "GLOGIC.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "innerinterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CHelpWindow::nSTARTLINE = 0;
const int CHelpWindow::nLIMIT_LINE = 1000;

CHelpWindow::CHelpWindow () :
	m_pTree ( NULL ),
	m_pTextBox ( NULL ),
	m_pTreeScroll ( NULL ),
	m_pTextBoxScroll ( NULL )
{
}

CHelpWindow::~CHelpWindow ()
{
}

void	CHelpWindow::CreateSubControl ()
{
	//	중간에 검은줄
	CreateControl ( "HELP_WINDOW_MIDDLE_LINE" );

	//	트리 스크롤바 회색바탕
	CreateControl ( "HELP_WINDOW_TREE_SCROLLBAR_BACK" );

	//	텍스트 박스 스크롤바 회색바탕
	CreateControl ( "HELP_WINDOW_TEXTBOX_SCROLLBAR_BACK" );

	//	텍스트 박스 회색바탕
	CreateControl ( "HELP_WINDOW_TEXTBOX_BACK" );

	{	//	트리 컨트롤
		CBasicTreeEx* pTree = new CBasicTreeEx;
		pTree->CreateSub ( this, "HELP_WINDOW_TREE", UI_FLAG_DEFAULT, HELP_WINDOW_TREE );	
		pTree->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ) );		
		pTree->SetUseLabelOverColor ( TRUE );
		pTree->SetLabelOverColor ( NS_UITEXTCOLOR::PRIVATE );
		RegisterControl ( pTree );
		m_pTree = pTree;

		LoadDataFile ();
	}

	{
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox;
		pTextBox->CreateSub ( this, "HELP_WINDOW_TEXTBOX", UI_FLAG_DEFAULT, HELP_WINDOW_TEXTBOX );
		pTextBox->SetFont ( pFont );
		pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		pTextBox->SetLimitLine ( nLIMIT_LINE );
		RegisterControl ( pTextBox );
		m_pTextBox = pTextBox;
	}	

	CBasicScrollBarEx* pScrollBar = NULL;
	{	//	스크롤
		pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_YSIZE );
		pScrollBar->CreateBaseScrollBar ( "HELP_TREE_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, 1 );	
		RegisterControl ( pScrollBar );
		m_pTreeScroll = pScrollBar;

		pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		pScrollBar->CreateBaseScrollBar ( "HELP_TEXTBOX_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, 1 );	
		RegisterControl ( pScrollBar );
		m_pTextBoxScroll = pScrollBar;		
	}
}

void	CHelpWindow::LoadDataFile ()
{
	CString strFileName = GLOGIC::GetPath ();	
	strFileName +=  ID2GAMEWORD("HELP_FILE");
	CHelpDataMan::GetInstance().LoadFile ( strFileName );	
	LoadHelpNode ( NULL, CHelpDataMan::GetInstance().GetHelpDataMan () );
}

void	CHelpWindow::LoadHelpNode ( CBasicTreeNode* pParent, const HELPNODE_LIST& list )
{
	HELPNODE_LIST_CITER iter = list.begin ();
	HELPNODE_LIST_CITER iter_end = list.end ();
	for ( ; iter != iter_end; ++iter )
	{
		SHELPNODE* pNode = (*iter);

		CBasicTreeNode* pTreeNode = m_pTree->InsertNode ( pParent, pNode->strTitle.c_str() );
		pTreeNode->SetNodeData ( DWORD_PTR(pNode) );
		LoadHelpNode ( pTreeNode, pNode->listSubNode );
	}
}

void	CHelpWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				CInnerInterface::GetInstance().SetDefaultPosInterface( HELP_WINDOW );
			}
		}
		break;
	case HELP_WINDOW_TREE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CBasicTreeNode* pNode = m_pTree->GetSelectedNode ();
				if ( pNode )
				{
					SHELPNODE* pHelp = (SHELPNODE*) pNode->GetNodeData ();
					m_pTextBox->SetText ( pHelp->strContents.c_str () );
				}
			}
		}
		break;

	case HELP_WINDOW_TEXTBOX:
		{
		}
		break;
	}
}

void	CHelpWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pTreeScroll )
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pTreeScroll->GetThumbFrame ();
		m_pTree->SetCurLine ( 0 );

		const int nTotalLine = m_pTree->GetTotalLine ();
		const int nLinePerOneView = m_pTree->GetLinePerOneView ();
		int nZeroTest = nTotalLine;
		int nZeroTest2 = nLinePerOneView;

		if ( !nZeroTest ) nZeroTest = 1;
		if ( !nZeroTest2 ) nZeroTest2 = 1;
		
		pThumbFrame->SetState ( nZeroTest, nZeroTest2 );

		//CDebugSet::ToView ( 1, 19, "[LoV:%d/Total:%d]", nLinePerOneView, nTotalLine );

		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);

			if ( nPos < nSTARTLINE ) nPos = nSTARTLINE;

			//CDebugSet::ToView ( 1, 18, "[T:%d/LPO:%d/%d]", nTotalLine, nLinePerOneView, nPos );

			m_pTree->SetCurLine ( nPos );
		}
	}	

	if ( m_pTextBoxScroll )
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pTextBoxScroll->GetThumbFrame ();
		m_pTextBox->SetCurLine ( 0 );

		const int nTotalLine = m_pTextBox->GetTotalLine ();
		const int nLinePerOneView = m_pTextBox->GetVisibleLine ();
		int nZeroTest = nTotalLine;

		if ( !nZeroTest ) nZeroTest = 1;
		
		pThumbFrame->SetState ( nZeroTest, nLinePerOneView );		
		
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);

			if ( nPos < nSTARTLINE ) nPos = nSTARTLINE;
			
			m_pTextBox->SetCurLine ( nPos );
		}
	}	
}
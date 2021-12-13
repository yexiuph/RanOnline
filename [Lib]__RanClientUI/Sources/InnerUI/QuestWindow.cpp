#include "pch.h"
#include "QuestWindow.h"
#include "QuestList.h"
#include "QuestMain.h"
#include "QuestStep.h"
#include "GameTextControl.h"
#include "GLGaeaClient.h"
#include "GLQuestMan.h"
#include "GLQuest.h"
#include "ModalWindow.h"
#include "ModalCallerID.h"
#include "BasicTextButton.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CQuestWindow::CQuestWindow () :
	m_nPage ( QUEST_LIST ),
	m_dwQuestID ( NATIVEID_NULL().dwID )
{
}

CQuestWindow::~CQuestWindow ()
{
}

void	CQuestWindow::CreateSubControl ()
{
	CQuestList* pQuestList = new CQuestList;
	pQuestList->CreateSub ( this, "QUEST_LIST", UI_FLAG_DEFAULT, QUEST_LIST );
	pQuestList->CreateSubControl ();
	RegisterControl ( pQuestList );
	m_pQuestList = pQuestList;

	CQuestMain* pQuestStep = new CQuestMain;
	pQuestStep->CreateSub ( this, "QUEST_STEP", UI_FLAG_DEFAULT, QUEST_MAIN );
	pQuestStep->CreateSubControl ();
	RegisterControl ( pQuestStep );
	m_pQuestMain = pQuestStep;

	CQuestStep* pQuestRun = new CQuestStep;
	pQuestRun->CreateSub ( this, "QUEST_RUN", UI_FLAG_DEFAULT, QUEST_STEP );
	pQuestRun->CreateSubControl ();
	RegisterControl ( pQuestRun );
	m_pQuestStep = pQuestRun;

	m_pMoreButton = CreateTextButton14 ( "QUEST_LIST_BUTTON_MORE", QUEST_LIST_BUTTON_MORE, (char*)ID2GAMEWORD("QUEST_LIST_BUTTON",2) );	

	SET_SHOW_PAGE ( QUEST_LIST );
}

void CQuestWindow::SET_SHOW_PAGE ( int nPage, bool bMakeMsg )
{
//	if ( bMakeMsg && !IsVisible () ) return ;

	if ( bMakeMsg )
	{
		//m_ActionMsgQ.PostUIMessage ( QUEST_LIST, UIMSG_INVISIBLE );		
		//m_ActionMsgQ.PostUIMessage ( QUEST_MAIN, UIMSG_INVISIBLE );
		//m_ActionMsgQ.PostUIMessage ( QUEST_STEP, UIMSG_INVISIBLE );
		m_pQuestList->SetVisibleSingle ( FALSE );
		m_pQuestMain->SetVisibleSingle ( FALSE );
		m_pQuestStep->SetVisibleSingle ( FALSE );
		m_pMoreButton->SetVisibleSingle ( FALSE );
	}
	else
	{
		m_pQuestList->SetVisibleSingle ( FALSE );
		m_pQuestMain->SetVisibleSingle ( FALSE );
		m_pQuestStep->SetVisibleSingle ( FALSE );
		m_pMoreButton->SetVisibleSingle ( FALSE );
	}

	switch ( nPage )
	{
	case QUEST_LIST:
		{
			if ( !LOAD_QUEST_LIST () )
			{
				GASSERT ( 0 && "오류, 퀘스트 정보를 찾을 수 없습니다." );
				return ;
			}

			if ( bMakeMsg )
			{
				//m_ActionMsgQ.PostUIMessage ( QUEST_LIST, UIMSG_VISIBLE );
				m_pQuestList->SetVisibleSingle ( TRUE );
				m_pMoreButton->SetVisibleSingle ( TRUE );
			}
			else
			{
				m_pQuestList->SetVisibleSingle ( TRUE );
				m_pMoreButton->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case QUEST_MAIN:
		{
			if ( !LOAD_QUEST_MAIN () )
			{				
				SET_SHOW_PAGE ( QUEST_LIST, bMakeMsg );
				return ;
			}

			if ( bMakeMsg )
			{
				//m_ActionMsgQ.PostUIMessage ( QUEST_MAIN, UIMSG_VISIBLE );
				m_pQuestMain->SetVisibleSingle ( TRUE );
			}
			else
			{
				m_pQuestMain->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case QUEST_STEP:
		{
			if ( !LOAD_QUEST_STEP () )
			{				
				SET_SHOW_PAGE ( QUEST_LIST , bMakeMsg );
				return ;
			}

			if ( bMakeMsg )	//m_ActionMsgQ.PostUIMessage ( QUEST_STEP, UIMSG_VISIBLE );
							m_pQuestStep->SetVisibleSingle ( TRUE );  
			else			m_pQuestStep->SetVisibleSingle ( TRUE );  
		}
		break;
	default: GASSERT ( 0 && "알 수 없는 페이지입니다." );	  break;
	}

	m_nPage = nPage;
}


void	CQuestWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				CInnerInterface::GetInstance().SetDefaultPosInterface( QUEST_WINDOW );
			}
		}
		break;
	case QUEST_LIST:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					DWORD dwQuestID = m_pQuestList->GetQuestID ();
					if ( dwQuestID == NATIVEID_NULL().dwID ) return ;

					SetQuestID ( dwQuestID );
				}

				if ( UIMSG_LIST_LB_DUP & dwMsg )
				{
					SET_SHOW_PAGE ( QUEST_MAIN, true );
				}
			}
		}
		break;

	case QUEST_MAIN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_BUTTON_GIVEUP & dwMsg )
				{
					DoModal ( ID2GAMEINTEXT("QUEST_GIVEUP_CONFIRM"), MODAL_QUESTION, YESNO, MODAL_QUEST_GIVEUP );
				}

				if ( UIMSG_BUTTON_GO_QUESTSTEP & dwMsg )
				{
					SET_SHOW_PAGE ( QUEST_STEP, true );
				}

				if ( UIMSG_BUTTON_BACK & dwMsg )
				{
					SET_SHOW_PAGE ( QUEST_LIST, true );
				}

				if ( UIMSG_BUTTON_COMPLETE & dwMsg )
				{
					GLGaeaClient::GetInstance().GetCharacter()->ReqQuestComplete ( GetQuestID () );
				}
			}
		}
		break;

	case QUEST_STEP:
		{
			if ( UIMSG_BUTTON_BACK_LBUPLIKE & dwMsg )
			{
				SET_SHOW_PAGE ( QUEST_MAIN, true );
			}

			if ( UIMSG_BUTTON_COMPLETE_STEP & dwMsg )
			{
				GLGaeaClient::GetInstance().GetCharacter()->ReqQuestComplete ( GetQuestID () );
			}
			
			if ( UIMSG_BUTTON_PRINT_STEP & dwMsg )
			{
				CInnerInterface::GetInstance().SET_QUEST_HELPER( GetQuestID() );	
			}
		}
		break;

	case QUEST_LIST_BUTTON_MORE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				SET_SHOW_PAGE ( QUEST_MAIN, true );
			}
		}
		break;
	}
}

bool	CQuestWindow::LOAD_QUEST_LIST ()
{	
	SetQuestID ( NATIVEID_NULL().dwID );
	SetTitleName ( ID2GAMEWORD("QUEST_WINDOW_STATIC") );	
	return m_pQuestList->LoadQuestList ();
}

bool	CQuestWindow::LOAD_QUEST_MAIN ()
{
	DWORD dwQuestID = GetQuestID ();
	if ( dwQuestID == SNATIVEID(false).dwID ) return false;

	if ( !m_pQuestMain->LoadQuestMain ( dwQuestID ) )
	{
		//GASSERT ( 0 && "QuestMain Load Failed!" );
		return false;
	}

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest ) return false;
	SetTitleName ( (char*)pQuest->GetTITLE () );

	return true;
}

bool	CQuestWindow::LOAD_QUEST_STEP ()
{
	DWORD dwQuestID = GetQuestID ();
	if ( dwQuestID == NATIVEID_NULL().dwID ) return false;

	if ( !m_pQuestStep->LoadQuestStep ( dwQuestID ) )
	{
		//GASSERT ( 0 && "QuestStep Load Failed!" );
		return false;
	}

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest ) return false;

	GLQuestPlay& cQuestPlay = GLGaeaClient::GetInstance().GetCharacter ()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( dwQuestID );
	if ( !pQuestProg ) return false;

	DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );					
	SetTitleName ( pQuestStep->GetTITLE () );	

	return true;
}

void CQuestWindow::AUTOSELECT_PAGE ( DWORD dwQuestID )
{
	if ( !IsVisible () ) return ;

	GLQuestPlay& cQuestPlay = GLGaeaClient::GetInstance().GetCharacter ()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( dwQuestID );
	if ( !pQuestProg )
	{
		SET_SHOW_PAGE ( QUEST_LIST );
		return ;
	}

	SetQuestID ( dwQuestID );
	const DWORD dwSTEP = pQuestProg->m_dwSTEP;	
	if ( dwSTEP < 1 )
		SET_SHOW_PAGE ( QUEST_MAIN );
	else
		SET_SHOW_PAGE ( QUEST_STEP );
}

void	CQuestWindow::REFRESH_QUEST_WINDOW ()
{	
	if ( !IsVisible () ) return ;
	const int nPage = GET_SHOW_PAGE ();
	SET_SHOW_PAGE ( nPage, true );
}

CBasicTextButton* CQuestWindow::CreateTextButton14 ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}
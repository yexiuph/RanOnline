#include "pch.h"
#include "QuestHelper.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "d3dfont.h"
#include "GLQuestMan.h"
#include "GLQuest.h"
#include "GLQuestPlay.h"
#include "GLGaeaClient.h"
#include "GLItemMan.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "GLCrowData.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CQuestHelper::CQuestHelper()
	:	m_pQuestText ( NULL )
{
	memset( m_dwQuestID, -1 , sizeof (DWORD) * MAX_QUEST_DISPLAY );
}

CQuestHelper::~CQuestHelper()
{
}

void CQuestHelper::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	m_pQuestText = new CBasicTextBox;
	m_pQuestText->CreateSub ( this, "QUEST_HELPER_TEXT" );
	m_pQuestText->SetFont ( pFont );
	m_pQuestText->SetTextAlign ( TEXT_ALIGN_LEFT );
	RegisterControl ( m_pQuestText );	
}

void CQuestHelper::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_pQuestText->ClearText();

	for ( int i = 0; i < MAX_QUEST_DISPLAY; ++i )
	{
		if ( m_dwQuestID[i] == NATIVEID_NULL().dwID )	continue;		
		
		if ( !UpdateQuestInfo ( m_dwQuestID[i]) )	m_dwQuestID[i] = NATIVEID_NULL().dwID;
	}    
}

bool  CQuestHelper::UpdateQuestInfo ( DWORD dwQuestID )
{
	GLQuestPlay& cQuestPlay = GLGaeaClient::GetInstance().GetCharacter ()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( dwQuestID );
	if ( !pQuestProg ) return false;

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest ) return false;	

	const DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );

	GLQUESTPROG_STEP& sSTEP_PROG = pQuestProg->m_sSTEP_PROG;	

	CString strTitle;
	strTitle.Format( "%s:%s ", ID2GAMEWORD("QUEST_HELPER_TEXT",0), pQuest->GetTITLE() );

	m_pQuestText->AddText( strTitle, NS_UITEXTCOLOR::LIGHTSKYBLUE );

	if ( pQuestStep->IsNEED_QITEM() )
	{
		SNATIVEID dwItemID;
		DWORD dwMaxNum = 0;
		DWORD dwCurNum = 0;	
		CString strItem;

		for ( int i = 0 ; i < (int)pQuestStep->m_vecMOBGEN_QITEM.size(); ++i )
		{
			dwCurNum = 0;
			dwItemID = pQuestStep->m_vecMOBGEN_QITEM[i].sNID;
			dwMaxNum = pQuestStep->m_vecMOBGEN_QITEM[i].wNUM;			
			SITEM* pItem = GLItemMan::GetInstance().GetItem( dwItemID );
			if ( !pItem ) continue;
			
			
			GLInventory::CELL_MAP *ItemList = pQuestProg->m_sINVENTORY.GetItemList ();
			GLInventory::CELL_MAP_ITER iter = ItemList->begin();
			GLInventory::CELL_MAP_ITER iter_end = ItemList->end();

			for ( ; iter!=iter_end; ++iter )
			{
				SINVENITEM* pInvenItem = (*iter).second;				
				SITEMCUSTOM &ref_ItemCustom = pInvenItem->sItemCustom;

				if ( ref_ItemCustom.sNativeID == dwItemID )
				{
					dwCurNum += ref_ItemCustom.wTurnNum;
				}
			}		
			
			strItem.Format ( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),pItem->GetName(),dwCurNum,dwMaxNum); 
			m_pQuestText->AddText( strItem, NS_UITEXTCOLOR::LIGHTSKYBLUE );

		}
	}

	if ( pQuestStep->IsNEED_MOBKILL() )
	{		
		CString strMob;

		DWORD dwMobId = pQuestStep->m_dwNID_MOBKILL;
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( dwMobId );
		if ( !pCrowData ) return false;

		strMob.Format ( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),
						pCrowData->GetName(),
						sSTEP_PROG.m_dwNUM_MOBKILL,
						pQuestStep->m_dwNUM_MOBKILL ); 
		m_pQuestText->AddText( strMob, NS_UITEXTCOLOR::LIGHTSKYBLUE );
	}

	return true;

}

void CQuestHelper::SetQuestID( DWORD dwQuestID )
{
	if ( dwQuestID == NATIVEID_NULL().dwID )	return;

	if ( IsSameQuest(dwQuestID) )
	{
		ReSetQuestID ( dwQuestID );
		
		GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
		CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT( "QUEST_HELPER_DEL" ), pQuest->GetTITLE() );
		return;
	}

	for ( int i = 0; i < MAX_QUEST_DISPLAY; ++i )
	{
		if ( m_dwQuestID[i] == NATIVEID_NULL().dwID )
		{
			m_dwQuestID[i] = dwQuestID;
			SetVisibleSingle( TRUE );
			
			GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
			
			CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT( "QUEST_HELPER_ADD" ), pQuest->GetTITLE() );
			return;
		}
	}

	CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "QUEST_HELPER_ERROR" ));
    	
	return;
}

void CQuestHelper::ReSetQuestID ( DWORD dwQuestID )
{
	if ( dwQuestID == NATIVEID_NULL().dwID )	return;

	for ( int i = 0; i < MAX_QUEST_DISPLAY; ++i )
	{
		if ( m_dwQuestID[i] == dwQuestID )
		{
			m_dwQuestID[i] = NATIVEID_NULL().dwID;

			break;
		}
	}

	if ( !IsQuestPrint() )	SetVisibleSingle( FALSE );
}

bool CQuestHelper::IsQuestPrint()
{
	for ( int i = 0; i < MAX_QUEST_DISPLAY; ++i )
	{
		if ( m_dwQuestID[i] != NATIVEID_NULL().dwID )	return true;			
	}

	return false;
}

bool CQuestHelper::IsSameQuest( DWORD dwQuestID )
{
	for ( int i = 0; i < MAX_QUEST_DISPLAY; ++i )
	{
		if ( m_dwQuestID[i] == dwQuestID )	return true;			
	}

	return false;

}


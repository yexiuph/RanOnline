#include "pch.h"
#include "GLGaeaClient.h"
#include "SkillPage.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "SkillSlot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CSkillPage::nSTARTLINE = 0;
const int CSkillPage::nOUTOFRANGE = -1;

CSkillPage::CSkillPage () :
	m_pScrollBar ( NULL ),
	m_nCurPos ( -1 )
{
	memset ( m_pSkillSlotArray, 0, sizeof ( CSkillSlot* ) * GLSkillMan::MAX_CLASSSKILL );	
	memset ( m_pSkillSlotArrayDummy, 0, sizeof ( CUIControl* ) * nMAX_ONE_VIEW_SLOT );		
}

CSkillPage::~CSkillPage ()
{
}

CUIControl*	CSkillPage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void CSkillPage::CreateSubControl ( SNATIVEID sNativeIDArray[GLSkillMan::MAX_CLASSSKILL], int nSkillCount )
{	
	if ( nSkillCount < 1 ) return ;

	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"SKILL_SLOT0",
		"SKILL_SLOT1",
		"SKILL_SLOT2",
		"SKILL_SLOT3",
		"SKILL_SLOT4",
		"SKILL_SLOT5",
		"SKILL_SLOT6",
		"SKILL_SLOT7"
	};

	{	//	더미 슬롯
		for ( int i = 0; i < nMAX_ONE_VIEW_SLOT; i++ )
		{
			m_pSkillSlotArrayDummy[i] = CreateControl ( strSlotKeyword[i].GetString () );
		}
	}

	{	//	실제 데이타		
		if ( nSkillCount < nMAX_ONE_VIEW_SLOT )
		{
			nSkillCount = nMAX_ONE_VIEW_SLOT;
		}

		m_vecViewSkill.clear();		

		for ( int i = 0; i < GLSkillMan::MAX_CLASSSKILL; i++ )
		{
			m_pSkillSlotArray[i] = CreateSkillSlot ( strSlotKeyword[0], SKILL_SLOT0 + i, sNativeIDArray[i] );
		}
		m_nMaxSkillCnt = nSkillCount;
	}

	{	//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		pScrollBar->CreateBaseScrollBar ( "SKILL_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( nSkillCount, nMAX_ONE_VIEW_SLOT );
		RegisterControl ( pScrollBar );
		m_pScrollBar = pScrollBar;
	}

	SetSkillSlotRender ( nSTARTLINE, nMAX_ONE_VIEW_SLOT );
}

CSkillSlot*	CSkillPage::CreateSkillSlot ( CString strKeyword, UIGUID ControlID, SNATIVEID sNativeID )
{
	CSkillSlot* pSkillSlot = new CSkillSlot;
	pSkillSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pSkillSlot->CreateSubControl ( sNativeID );
	RegisterControl ( pSkillSlot );
	return pSkillSlot;
}

void CSkillPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_nSlotIndex = nOUTOFRANGE;
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pScrollBar )
	{
		{
			CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame ();
			int nViewPerPage = pThumbFrame->GetViewPerPage ();
			int nBeforeSkillCnt = m_vecViewSkill.size();

			m_vecViewSkill.clear();
			for ( int i = 0; i < m_nMaxSkillCnt; i++ )
			{
				SNATIVEID sSkillID = m_pSkillSlotArray[i]->GetNativeSkillID();
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
				if( pSkill && pSkill->m_sBASIC.bLearnView && !GLGaeaClient::GetInstance().GetCharacter()->GETLEARNED_SKILL( sSkillID ) )
				{
				}else{
					m_vecViewSkill.push_back( i );
				}
			}
			if( nBeforeSkillCnt != m_vecViewSkill.size() )
			{
				m_pScrollBar->GetThumbFrame()->SetState ( m_vecViewSkill.size(), nMAX_ONE_VIEW_SLOT );
				ResetAllSkillSlotRender( m_nMaxSkillCnt );
				SetSkillSlotRender ( 0, 0 + nViewPerPage );			
			}
			
		}
		{
			CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame ();
			int nTotal = pThumbFrame->GetTotal ();
			if ( nTotal <= nMAX_ONE_VIEW_SLOT ) return ;

			const int nViewPerPage = pThumbFrame->GetViewPerPage ();

			if ( nViewPerPage < nTotal )
			{
				int nCurPos = nSTARTLINE;
				const int nMovableLine = nTotal - nViewPerPage;
				float fPercent = pThumbFrame->GetPercent ();

				nCurPos = (int)floor(fPercent * nMovableLine);
				if ( nCurPos < nSTARTLINE ) nCurPos = nSTARTLINE;

				if ( m_nCurPos == nCurPos ) return;

				m_nCurPos = nCurPos;

				ResetAllSkillSlotRender ( nTotal );
				SetSkillSlotRender ( nCurPos, nCurPos + nViewPerPage );
			}
		}
	}
}

void CSkillPage::ResetAllSkillSlotRender ( int nTotal )
{
	if ( nTotal < 0 ) return ;

	for ( int i = 0; i < GLSkillMan::MAX_CLASSSKILL; i++ )
	{
		CSkillSlot* pSkillSlot = m_pSkillSlotArray[i];
		if ( pSkillSlot )
		{
			pSkillSlot->SetVisibleSingle ( FALSE );
		}
	}
}

void CSkillPage::SetSkillSlotRender ( int nStartIndex, int nTotal )
{
	if ( nTotal < 0 ) return ;

	const UIRECT& rcParentPos = GetGlobalPos ();

	

	int iViewIndex = 0; // 보여질 위치
	int iSlotNum   = 0;	// 보여지는 SlotNum
	for ( int i = nStartIndex; i < (int)m_vecViewSkill.size(); i++ )
	{
		if( i >= m_nMaxSkillCnt ) break;
		if( i >= nTotal ) break;

		iSlotNum = m_vecViewSkill[i];

		CSkillSlot* pSkillSlot = m_pSkillSlotArray[iSlotNum];
		if ( pSkillSlot )
		{
						
			int nAbsoluteIndex = iViewIndex;
			CUIControl* pDummyControl = m_pSkillSlotArrayDummy[nAbsoluteIndex];

			const UIRECT& rcSlotPos = pDummyControl->GetGlobalPos ();
			const UIRECT& rcSlotLocalPos = pDummyControl->GetLocalPos ();

			pSkillSlot->SetLocalPos ( rcSlotLocalPos );			
			pSkillSlot->SetGlobalPos ( rcSlotPos );

			pSkillSlot->SetVisibleSingle ( TRUE );

			iViewIndex++;
		}
	}

	if( (int)m_vecViewSkill.size() < nTotal )
	{
		int iMaxLackTotal = nTotal - (int)m_vecViewSkill.size();		
		for( int i = 0; i < iMaxLackTotal; i++ )
		{
			iSlotNum++;
			CSkillSlot* pSkillSlot = m_pSkillSlotArray[iSlotNum];
			if ( pSkillSlot )
			{

				int nAbsoluteIndex = iViewIndex;
				CUIControl* pDummyControl = m_pSkillSlotArrayDummy[nAbsoluteIndex];

				const UIRECT& rcSlotPos = pDummyControl->GetGlobalPos ();
				const UIRECT& rcSlotLocalPos = pDummyControl->GetLocalPos ();

				pSkillSlot->SetLocalPos ( rcSlotLocalPos );			
				pSkillSlot->SetGlobalPos ( rcSlotPos );

				pSkillSlot->SetVisibleSingle ( TRUE );

				iViewIndex++;
			}
		}
	}


	
	for( int i = nTotal; i < (int)m_vecViewSkill.size(); i++ )
	{
		iSlotNum = m_vecViewSkill[i];

		if( m_pSkillSlotArray[iSlotNum] ) m_pSkillSlotArray[iSlotNum]->SetVisibleSingle( FALSE );
	}
}

SNATIVEID  CSkillPage::GetSkillID ( int nIndex )
{	
	return m_pSkillSlotArray[nIndex]->GetNativeSkillID ();
}

void CSkillPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	if ( SKILL_SLOT0 <= ControlID && ControlID <= SKILL_SLOT_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			m_nSlotIndex = ControlID - SKILL_SLOT0;
		}
	}
}

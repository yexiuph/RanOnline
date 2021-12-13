#include "pch.h"
#include "UIGroup.h"

#include "DxInputDevice.h"
#include "TextureManager.h" //../[Lib]__Engine/Sources/DxCommon
#include "InterfaceCfg.h"
#include "UIMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIGroup::CUIGroup ()
	: m_bClickedFoldControl(FALSE)
	, m_nCurFoldGroupID(0)
	, m_bAllExclusiveControl(FALSE)
{
}

CUIGroup::~CUIGroup()
{
}

BOOL CUIGroup::RegisterControl ( CUIControl* pNewControl )
{
	GASSERT( pNewControl );
	return m_ControlContainer.InsertControl ( pNewControl );
}

CUIControl*	 CUIGroup::FindControl ( UIGUID ControlID )
{
	return m_ControlContainer.FindControl ( ControlID );
}

bool CUIGroup::DeleteControl ( UIGUID ControlID, int refactoring )
{
	return m_ControlContainer.EraseControl ( ControlID );
}

//void CUIGroup::TranslateActionMsgQ ( UIGUID ControlID, DWORD dwMsg )
//{
//	CUIControl* pControl = m_ControlContainer.FindControl ( ControlID );
//	if ( !pControl ) return ;
//
//	if( dwMsg & UIMSG_VISIBLE )
//	{
//		pControl->SetVisibleSingle ( TRUE );
//		return ;
//	}
//
//	if( dwMsg & UIMSG_INVISIBLE )
//	{
//		pControl->SetVisibleSingle ( FALSE );
//		return ;
//	}
//}

//void CUIGroup::UpdateActionMsgQ ()
//{
//	UIGUID cID;// = NO_ID;
//	DWORD dwMsg;// = 0;
//	while ( m_ActionMsgQ.PeekUIMessage ( &cID, &dwMsg ) )
//	{
//		TranslateActionMsgQ ( cID, dwMsg );
//	}
//}

void CUIGroup::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if( IsNoUpdate() ) return ;

	CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	InitFoldGroup();

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		// 보이지 않는 컨트롤은 업데이트 하지 않는다.
		if( pControl->IsNoUpdate() ) continue;

		pControl->Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		DWORD dwMsg = pControl->GetMessageEx();
		
		if( CHECK_KEYFOCUSED( dwMsg ) )	AddMessageEx( UIMSG_KEY_FOCUSED ); // MEMO
		//if( CHECK_FOCUSED( dwMsg ) )	AddMessageEx( UIMSG_FOCUSED );
		if( CHECK_TOPARENT( dwMsg ) )	AddMessageEx( CUT_LOW24BIT( dwMsg ) ); // MEMO

		if( bFirstControl )
		{
			NS_UIDEBUGSET::BlockBegin ();
			{
				if( !IsClickedFoldControl( pControl ) )
				{
					SetClickedFoldControl( pControl );

					UIGUID cID = pControl->GetWndID();
					if( dwMsg && ( cID < FORCE_ID ) ) TranslateUIMessage( cID, dwMsg );
				}
			}
			NS_UIDEBUGSET::BlockEnd ();
		}
	}

	//UpdateActionMsgQ();
}

VOID CUIGroup::InitFoldGroup()
{ 
	m_nCurFoldGroupID = 0;
	m_bClickedFoldControl = FALSE;
}

BOOL CUIGroup::IsClickedFoldControl( CUIControl * pControl )
{
	GASSERT( pControl );

	if( !m_bClickedFoldControl ) return FALSE;
	else
	{
		INT nGroupID = pControl->GetFoldID();

		if( nGroupID > 0 && ( m_nCurFoldGroupID == nGroupID ) )
			return TRUE;
		else
			return FALSE;
	}
}

VOID CUIGroup::SetClickedFoldControl( CUIControl * pControl )
{
	GASSERT( pControl );

	m_nCurFoldGroupID = pControl->GetFoldID();
	m_bClickedFoldControl = TRUE;
}

VOID CUIGroup::SetFoldGroup( CUIControl * pControl, INT nGroupID )
{
	GASSERT( pControl && nGroupID > 0 );

	pControl->SetFoldID( nGroupID );
}
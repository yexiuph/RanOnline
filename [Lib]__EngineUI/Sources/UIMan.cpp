#include "pch.h"
#include "./UIMan.h"
#include "./UIControl.h"
#include "./UIDebugSet.h"
#include "./UIKeyCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CPositionKeeper		CUIMan::m_PositionKeeper;
//CUIControlContainer	CUIMan::m_ControlContainer;
//CUIFocusContainer	CUIMan::m_BottomList;
//CUIFocusContainer	CUIMan::m_FocusList;
//CUIFocusContainer	CUIMan::m_TopList;
//CUIMessageQueue		CUIMan::m_ActionMsgQ;

long CUIMan::m_lResolution( UI_DEFRESOLUTION );
BOOL CUIMan::m_bBIGFONT( TRUE );
CString CUIMan::m_strPath( _T("") );

CUIMan::CUIMan ()
	: m_bMouseInControl( FALSE )
	//, m_bFirstControl( TRUE )
	, m_bExclusiveControl( FALSE )
	, m_pExclusiveControl( NULL )
	//, m_bFocusControl( FALSE )
	, m_pFocusControl( NULL )
{	
}

CUIMan::~CUIMan ()
{
	UIKeyCheck::ReleaseInstance ();
}

BOOL CUIMan::RegisterControl( CUIControl* pNewControl, bool bPosisionKeep )
{
	GASSERT( pNewControl );
		
	if( bPosisionKeep ) //	포지션을 유지해줘야 하는 경우
	{
		BOOL bResult = m_PositionKeeper.RegisterControl( pNewControl->GetWndID (), pNewControl );
		if ( !bResult )  return FALSE;
	}

	BOOL bResult = m_ControlContainer.InsertControl ( pNewControl );
	if ( !bResult ) return FALSE;

	return TRUE;
}

CUIControl * CUIMan::FindControl ( UIGUID ControlID )
{
	return m_ControlContainer.FindControl ( ControlID );
}

BOOL CUIMan::DeleteControl ( UIGUID ControlID )
{	
	return m_ControlContainer.EraseControl ( ControlID );
}

void CUIMan::DeleteAllControl ()
{
	m_TopList.RemoveAll();
	m_FocusList.RemoveAll();
	m_BottomList.RemoveAll();
	m_ControlContainer.RemoveAll(); // 실제 메모리는 여기서 삭제 된다.

	UIKeyCheck::GetInstance()->Clear();
	m_PositionKeeper.Clear ();

	m_bExclusiveControl = FALSE;
	m_pExclusiveControl = NULL;
	//m_bFocusControl = FALSE;
	m_pFocusControl = NULL;
}

CUIControl* CUIMan::GetFocusControl ()
{	
	return m_FocusList.GetFocusControl();
}

void CUIMan::ShowGroupFocus ( UIGUID ControlID, bool bMakeMsg )
{
	CUIControl * pControl = m_ControlContainer.FindControl ( ControlID );

	if( pControl )
	{
		if( m_pFocusControl != pControl )
		{
			if( m_pFocusControl )
				m_pFocusControl->ResetFocusControl();

			m_pFocusControl = pControl;
		}

		pControl->SetVisibleSingle ( TRUE );
		m_FocusList.InsertTail( ControlID, pControl );
		pControl->SetFocusControl();
	}
}

void CUIMan::ShowGroupBottom ( UIGUID ControlID, bool bMakeMsg )
{
	CUIControl * pControl = m_BottomList.IsInserted( ControlID );

	if( pControl )
	{
		pControl->SetVisibleSingle ( TRUE );			
	}
	else
	{
		pControl = m_ControlContainer.FindControl ( ControlID );
		if( pControl )
			m_BottomList.InsertTail ( ControlID, pControl );
	}
}

void CUIMan::ShowGroupTop( UIGUID ControlID, bool bMakeMsg )
{
	CUIControl * pControl = m_TopList.IsInserted( ControlID );

	if( pControl )
	{
		pControl->SetVisibleSingle( TRUE );
	}
	else
	{
		pControl = m_ControlContainer.FindControl( ControlID );
		if( pControl )
			m_TopList.InsertTail( ControlID, pControl );
	}
}

void CUIMan::HideGroup ( UIGUID ControlID, bool bMakeMsg )
{
	CUIControl * pControl = m_ControlContainer.FindControl( ControlID );

	if( pControl && pControl->IsVisible() )
	{
		pControl->SetVisibleSingle( FALSE );
		pControl = m_FocusList.IsInserted( ControlID );

		if( pControl )
		{
			m_FocusList.InsertHead( ControlID, pControl );
			pControl->ResetFocusControl();

			CUIControl * pFocusControl = m_FocusList.GetTail();
			if( pFocusControl )
			{
				pFocusControl->SetFocusControl();
				m_pFocusControl = pFocusControl;
			}
		}
	}
}

BOOL CUIMan::IsVisibleGroup ( UIGUID ControlID )
{
	CUIControl* pControl = m_ControlContainer.FindControl( ControlID );
	if( pControl )	
		return pControl->IsVisible();

	return FALSE;
}

void CUIMan::MOVE_DEFAULT_POSITION ()
{
	CPositionKeeper::POSITION_MAP& mapPosition = m_PositionKeeper.GetPositionMap ();
	CPositionKeeper::POSITION_MAP_ITER iter = mapPosition.begin ();
	CPositionKeeper::POSITION_MAP_ITER iter_end = mapPosition.end ();

	for ( ; iter != iter_end; ++iter )
	{
		CPositionKeeper::SPOSCONTROL sPosControl = (*iter).second;

		if( !sPosControl.pControl ) continue;

		sPosControl.pControl->RestoreDeviceObjects( m_pd3dDevice );
		sPosControl.bSetFlag = FALSE;
	}
}

void CUIMan::SetControlPosition()
{
	CUIControl* pFocusControl = GetFocusControl ();

	if( m_pExclusiveControl && pFocusControl )
	{
		const UIGUID& cFocusID = pFocusControl->GetWndID ();
		const UIGUID& cExclusiveID = m_pExclusiveControl->GetWndID ();
		if( cFocusID == cExclusiveID )
		{
			const UIRECT& rcPos = m_pExclusiveControl->GetGlobalPos ();
			// 해상도가 변하더라도 정렬 플래그에 상관없이 이전 포지션으로 셋팅한다.
			m_PositionKeeper.SetPosition( cFocusID, rcPos );
		}
	}
}

void CUIMan::MaintainControlPosition()
{
	CPositionKeeper::POSITION_MAP& mapPosition = m_PositionKeeper.GetPositionMap ();
	CPositionKeeper::POSITION_MAP_CITER iter = mapPosition.begin ();
	CPositionKeeper::POSITION_MAP_CITER iter_end = mapPosition.end ();

	for ( ; iter != iter_end; ++iter )
	{		
		const CPositionKeeper::SPOSCONTROL& sPosControl = (*iter).second;

		//	초기화 되지 않은것은 SKIP
		if ( !sPosControl.bSetFlag ) continue;

		//	포지션 유지
		if( !sPosControl.pControl ) continue;

		sPosControl.pControl->SetGlobalPos( sPosControl.uiRect );
	}	
}

void CUIMan::ResetExclusiveControl()
{
	m_bExclusiveControl = FALSE;

	if( m_pExclusiveControl )
	{
		m_pExclusiveControl->ResetExclusiveControl();
		m_pExclusiveControl = NULL;
	}
}

//void CUIMan::PostUIMessage ( UIGUID cID, DWORD dwMsg )
//{
//	m_ActionMsgQ.PostUIMessage( cID, dwMsg );
//}

//void CUIMan::UpdateActionMsgQ ()
//{
//	UIGUID cID = NO_ID;
//	DWORD dwMsg = 0;
//	while ( m_ActionMsgQ.PeekUIMessage ( &cID, &dwMsg ) )
//	{
//		TranslateActionMsgQ ( cID, dwMsg );
//	}
//}

//void CUIMan::TranslateActionMsgQ ( UIGUID ControlID, DWORD dwMsg )
//{
//	CUIControl* pControl = m_ControlContainer.FindControl ( ControlID );
//	if ( !pControl ) return;
//
//	if ( dwMsg & UIMSG_VISIBLE )
//	{
//		pControl->SetVisibleSingle ( TRUE );
//
//		if ( dwMsg & UIMSG_FOCUSED )
//		{
//			m_FocusList.InsertTail ( ControlID, pControl ); // MEMO : 꼬리에 붙는게 포커스가 있는...
//			pControl->SetFocusControl();
//		}
//		return ;
//	}
//
//	if ( dwMsg & UIMSG_INVISIBLE )
//	{
//		pControl->SetVisibleSingle ( FALSE );
//
//		if ( dwMsg & UIMSG_FOCUSED )
//		{
//			if ( m_FocusList.IsInserted ( ControlID ) )
//			{
//				m_FocusList.InsertHead ( ControlID, pControl );
//				pControl->ResetFocusControl();
//			}
//		}
//		return ;
//	}
//}
#include "pch.h"
#include "PositionKeeper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPositionKeeper::CPositionKeeper ()
{
}

CPositionKeeper::~CPositionKeeper ()
{
}

BOOL CPositionKeeper::RegisterControl ( const UIGUID& cID, CUIControl * pControl )
{
	SPOSCONTROL sPosControl;
	sPosControl.pControl = pControl;	

	m_mapPosition.insert ( std::make_pair(cID, sPosControl) );

	return TRUE;
}

BOOL CPositionKeeper::SetPosition ( const UIGUID& cID, const UIRECT& rcPos )
{
	POSITION_MAP_ITER iter = m_mapPosition.find ( cID );
	if ( iter == m_mapPosition.end () ) return FALSE; // 등록된 컨트롤이 없다면 리턴

	iter->second.bSetFlag = TRUE; // 포지션을 셋팅했다.
	iter->second.uiRect = rcPos; // 실제 포지션을 대입한다.

	return TRUE;
}

BOOL CPositionKeeper::ResetPosition ( const UIGUID& cID )
{
	POSITION_MAP_ITER iter = m_mapPosition.find ( cID );
	if ( iter == m_mapPosition.end () ) return FALSE;

	iter->second.bSetFlag = FALSE; // 포지션이 셋팅되어있지 않다는 표시

	return TRUE;
}

BOOL CPositionKeeper::GetPosition ( const UIGUID& cID, UIRECT& rcPos )
{
	POSITION_MAP_CITER iter = m_mapPosition.find ( cID );
	if ( iter == m_mapPosition.end () ) return FALSE;

	if ( !iter->second.bSetFlag ) return FALSE;

	rcPos = iter->second.uiRect;

	return TRUE;
}

BOOL CPositionKeeper::IsSetPosition ( const UIGUID& cID )
{
	POSITION_MAP_CITER iter = m_mapPosition.find ( cID );
	if ( iter == m_mapPosition.end () ) return FALSE;

	return iter->second.bSetFlag;
}
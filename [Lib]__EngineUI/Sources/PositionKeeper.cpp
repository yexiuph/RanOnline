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
	if ( iter == m_mapPosition.end () ) return FALSE; // ��ϵ� ��Ʈ���� ���ٸ� ����

	iter->second.bSetFlag = TRUE; // �������� �����ߴ�.
	iter->second.uiRect = rcPos; // ���� �������� �����Ѵ�.

	return TRUE;
}

BOOL CPositionKeeper::ResetPosition ( const UIGUID& cID )
{
	POSITION_MAP_ITER iter = m_mapPosition.find ( cID );
	if ( iter == m_mapPosition.end () ) return FALSE;

	iter->second.bSetFlag = FALSE; // �������� ���õǾ����� �ʴٴ� ǥ��

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
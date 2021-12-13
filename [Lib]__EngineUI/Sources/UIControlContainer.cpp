#include "pch.h"
#include "UIControlContainer.h"
#include "UIControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIControlContainer::CUIControlContainer ()
{
}

CUIControlContainer::~CUIControlContainer ()
{
	RemoveAll ();
}

void CUIControlContainer::RemoveAll ( bool bDELETE )
{
	if ( bDELETE )
	{
		UICONTROL_LIST_ITER iter = m_UIControlList.begin ();
		UICONTROL_LIST_ITER iter_end = m_UIControlList.end ();
		for ( ; iter != iter_end; ++iter )
		{
			SAFE_DELETE ( *iter );
		}
	}

	m_UIControlList.clear ();
	m_UIControlMap.clear ();
}

bool CUIControlContainer::empty ()
{	
	return m_UIControlList.empty ();
}

int	CUIControlContainer::size ()
{
	return (int)m_UIControlList.size ();
}

CUIControl*	CUIControlContainer::FindControl ( UIGUID cID )
{
	UICONTROL_MAP_CITER found = m_UIControlMap.find ( cID );
	if ( found != m_UIControlMap.end () )	return found->second;
	else									return NULL;
}

bool CUIControlContainer::InsertControl ( CUIControl* pNewControl )
{
	GASSERT( pNewControl );
	if ( !pNewControl ) return false;

	UIGUID cID = pNewControl->GetWndID ();	
	bool bNO_ID = (cID == NO_ID);
	
	if ( bNO_ID ) // 아이디가 없다면
	{
		cID = int(m_UIControlList.size()) + FORCE_ID;
		pNewControl->SetWndID ( cID );		
		bNO_ID = false;
	}

	if ( !bNO_ID && FindControl ( cID ) ) // ID 중복검사
	{
		GASSERT ( 0 && "이미 같은 ID로 등록된 컨트롤이 존재합니다." );
		return false;
	}

	m_UIControlList.push_back( pNewControl );
    
	if ( !bNO_ID ) m_UIControlMap.insert( std::make_pair( cID, pNewControl) ); // 맵에 등록

	return true;
}

bool CUIControlContainer::EraseControl ( UIGUID cID, BOOL bDelete )
{
	UICONTROL_MAP_ITER found = m_UIControlMap.find ( cID );
	if ( found != m_UIControlMap.end () )
	{
		m_UIControlMap.erase ( found );

		UICONTROL_LIST_ITER iter = m_UIControlList.begin ();
		UICONTROL_LIST_ITER iter_end = m_UIControlList.end ();
        for ( ; iter != iter_end; ++iter )
		{
			CUIControl* pDelControl = (*iter);
			if( !pDelControl ) return false;

			if ( cID == pDelControl->GetWndID() )
			{				
				if( bDelete ) SAFE_DELETE ( pDelControl );
				m_UIControlList.erase ( iter );
				return true;
			}
		}
	}

	return false;
}
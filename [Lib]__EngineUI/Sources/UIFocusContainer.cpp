#include "pch.h"
#include "./UIFocusContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIFocusContainer::CUIFocusContainer ()
{
}

CUIFocusContainer::~CUIFocusContainer ()
{
	RemoveAll();
}

CUIControl * CUIFocusContainer::IsInserted( UIGUID cID )
{
	UICONTROL_FOCUSLIST_CITER iter = m_UIFocusList.begin();
	UICONTROL_FOCUSLIST_CITER iter_end = m_UIFocusList.end();
	for( ; iter != iter_end; ++iter )
	{
		if( iter->cID == cID ) return iter->pUIControl;
	}

	return NULL;
}

bool CUIFocusContainer::InsertAfter ( UIGUID Where_cID, UIGUID cID, CUIControl * pUIControl )
{
	GASSERT( pUIControl );

	if( Where_cID == NO_ID )		return false;
	if( cID == NO_ID )				return false;
	if( !IsInserted( Where_cID ) )	return false;

	//	�����ϰ� �ٽ� ������ �߰��Ѵ�.
	if( IsInserted( cID ) ) EraseControl( cID );

	UICONTROL_FOCUSLIST_ITER iter = m_UIFocusList.begin();
	UICONTROL_FOCUSLIST_ITER iter_end = m_UIFocusList.end();
	for( ; iter != iter_end; ++iter )
	{
		if( iter->cID == Where_cID ) break;
	}

	++iter; // ++�ϱ����� iter�� �ݵ�� end()�� �ƴϴ�.
	if ( iter == m_UIFocusList.end () )
	{
		return InsertTail( cID, pUIControl );
	}
	else
	{
		SUICONTROL sUIControl;
		sUIControl.cID = cID;
		sUIControl.pUIControl = pUIControl;

		m_UIFocusList.insert( iter, sUIControl );
	}

	return true;
}

bool CUIFocusContainer::InsertTail ( UIGUID cID, CUIControl * pUIControl )
{
	GASSERT( pUIControl );

	if ( cID == NO_ID ) return false;
	if ( !m_UIFocusList.empty () )
	{
		if( m_UIFocusList.rbegin()->cID == cID ) return true; // ���� ������ �߰��Ǿ��ٸ�...
	}

	//	�����ϰ� �ٽ� ������ �߰��Ѵ�.
	if( IsInserted( cID ) ) EraseControl( cID );	

	SUICONTROL sUIControl;
	sUIControl.cID = cID;
	sUIControl.pUIControl = pUIControl;
	m_UIFocusList.push_back ( sUIControl );

	return true;
}

bool CUIFocusContainer::InsertHead ( UIGUID cID, CUIControl * pUIControl )
{
	GASSERT( pUIControl );

	if ( cID == NO_ID ) return false;
	if ( !m_UIFocusList.empty () )
	{
		if ( m_UIFocusList.begin()->cID == cID ) return true; // ���� �Ӹ��� �߰��Ǿ��ٸ�...
	}

	//	�����ϰ� �ٽ� �Ӹ��� �߰��Ѵ�.
	if ( IsInserted( cID ) ) EraseControl( cID );	

	SUICONTROL sUIControl;
	sUIControl.cID = cID;
	sUIControl.pUIControl = pUIControl;
	m_UIFocusList.push_front ( sUIControl );

	return true;
}

bool CUIFocusContainer::EraseControl ( UIGUID cID )
{
	UICONTROL_FOCUSLIST_ITER iter = m_UIFocusList.begin();
	UICONTROL_FOCUSLIST_ITER iter_end = m_UIFocusList.end();
	for( ; iter != iter_end; ++iter )
	{
		if( iter->cID == cID )
		{
			m_UIFocusList.erase( iter );
			return true;
		}
	}

	return false;
}

CUIControl * CUIFocusContainer::GetFocusControl ()
{
	if( m_UIFocusList.empty() )	return NULL;
	else						return m_UIFocusList.rbegin()->pUIControl; // �������� �ִ� ��Ʈ���� ��Ŀ���� ���� ��Ʈ���̴�.
}

CUIControl * CUIFocusContainer::GetTail()
{
	SUICONTROL sUIControl = m_UIFocusList.back();
	return sUIControl.pUIControl;
}
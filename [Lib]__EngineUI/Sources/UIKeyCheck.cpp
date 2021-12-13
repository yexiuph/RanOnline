#include "pch.h"
#include "UIKeyCheck.h"
#include <algorithm>
#include "DxInputDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UIKeyCheck*		UIKeyCheck::m_pInstance = NULL;

UIKeyCheck::UIKeyCheck ()
{
}

UIKeyCheck::~UIKeyCheck ()
{
}

UIKeyCheck*	UIKeyCheck::GetInstance ()
{
	if ( !m_pInstance ) m_pInstance = new UIKeyCheck; // MEMO
	return m_pInstance;
}

void	UIKeyCheck::ReleaseInstance ()
{
	SAFE_DELETE ( m_pInstance );
}

bool UIKeyCheck::CheckSimple ( const DWORD dwKEY, const DWORD dwEVENT )
{
	if ( DxInputDevice::GetInstance().GetKeyState ( (BYTE)dwKEY ) & dwEVENT )
	{
		return true;
	}

	return false;
}

bool UIKeyCheck::Check ( const DWORD dwKEY, const DWORD dwEVENT )
{
	KEYEVENT_PAIR FIND_KEY(dwKEY,dwEVENT);
	KEYEVENT_VECTOR_CITER iter = m_vecKeyEvent.begin ();
	KEYEVENT_VECTOR_CITER iter_end = m_vecKeyEvent.end ();

	//	�̹� ���Ǿ��°�?
	if ( std::find ( iter, iter_end, FIND_KEY ) != iter_end ) return false;

	//	�̺�Ʈ�� �߻��Ͽ��°�?
	if ( DxInputDevice::GetInstance().GetKeyState ( (BYTE)dwKEY ) & dwEVENT )
	{
		m_vecKeyEvent.push_back ( FIND_KEY );
		return true;
	}	

	return false;
}

bool UIKeyCheck::RemoveCheck ( const DWORD dwKEY, const DWORD dwEVENT )
{
	KEYEVENT_PAIR FIND_KEY(dwKEY,dwEVENT);
	KEYEVENT_VECTOR_ITER iter = m_vecKeyEvent.begin ();
	KEYEVENT_VECTOR_ITER iter_end = m_vecKeyEvent.end ();

	//	�̹� ���Ǿ��°�?
	KEYEVENT_VECTOR_ITER found = std::find ( iter, iter_end, FIND_KEY );
	if ( found == iter_end ) return false;

	m_vecKeyEvent.erase ( found );

	return true;
}

void UIKeyCheck::Clear ()
{
	m_vecKeyEvent.clear ();
}
#include "pch.h"
#include "UIDebugSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_UIDEBUGSET
{
	#ifdef	__UI_DEGUG__
	BOOL	m_bInBlock = FALSE;
	#endif	//	__UI_DEGUG__

	void  BlockBegin ()
	{
	#ifdef	__UI_DEGUG__
		if ( m_bInBlock )
		{
			GASSERT ( 0 && "이미 다른 곳에서 블럭이 걸리고 있습니다." );
			return ;
		}

		m_bInBlock = TRUE;
	#endif	//	__UI_DEGUG__
	}

	BOOL  IsInBlock ()
	{
	#ifdef	__UI_DEGUG__
		return m_bInBlock;
	#endif	//	__UI_DEGUG__
		return FALSE;
	}

	void  BlockEnd ()
	{
	#ifdef	__UI_DEGUG__
		m_bInBlock = FALSE;
	#endif	//	__UI_DEGUG__
	}
};
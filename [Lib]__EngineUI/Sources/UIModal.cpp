#include "pch.h"
#include "UIModal.h"
#include "UIMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIModal::CUIModal () :
	m_CallerID ( NO_ID ),
	m_bModaless ( false )
{
}

CUIModal::~CUIModal ()
{
}

void CUIModal::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	DWORD dwMsg = GetMessageEx ();
	//	리턴값이 돌아올 경우엔 창이 닫기는 것이다.
	if ( !((UIMSG_MODAL_OK & dwMsg) || (UIMSG_MODAL_CANCEL & dwMsg)) )
	{
		//	다중 모달일때 현재의 것인가?
//		if ( CUIMan::GetFocusControl () == this )
		{
			//	모달리스가 아닌 경우
			if ( !m_bModaless )
			{
				AddMessageEx ( UIMSG_MODAL_ON );
			}
		}
	}	
}

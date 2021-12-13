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
	//	���ϰ��� ���ƿ� ��쿣 â�� �ݱ�� ���̴�.
	if ( !((UIMSG_MODAL_OK & dwMsg) || (UIMSG_MODAL_CANCEL & dwMsg)) )
	{
		//	���� ����϶� ������ ���ΰ�?
//		if ( CUIMan::GetFocusControl () == this )
		{
			//	��޸����� �ƴ� ���
			if ( !m_bModaless )
			{
				AddMessageEx ( UIMSG_MODAL_ON );
			}
		}
	}	
}

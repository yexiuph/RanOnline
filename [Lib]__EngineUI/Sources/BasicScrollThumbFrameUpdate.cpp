#include "pch.h"
#include "BasicScrollThumbFrame.h"
#include "BasicScrollThumb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CBasicScrollThumbFrame::VALIDTHUMBMOVE ()
{
	if ( m_bVERTICAL )
	{
		const UIRECT& rcThumbPos = m_pThumb->GetGlobalPos ();
		const UIRECT& rcFramePos = GetGlobalPos ();
		if ( rcThumbPos.top <= rcFramePos.top )
		{
			SetPercent ( fZERO_PERCENT );
		}
		else if ( rcFramePos.bottom <= rcThumbPos.bottom )
		{
			SetPercent ( fHUNDRED_PERCENT );
		}
		else
		{
			const UIRECT& rcThumbPos = m_pThumb->GetGlobalPos ();
			const UIRECT& rcFramePos = GetGlobalPos ();

			const float fBase = rcFramePos.top + rcThumbPos.sizeY;
			const float fTotal = rcFramePos.sizeY - rcThumbPos.sizeY;
			const float fCurPos = rcThumbPos.bottom - fBase;

			m_fPercent = fCurPos / fTotal;
		}
	}
	else
	{
		const UIRECT& rcThumbPos = m_pThumb->GetGlobalPos ();
		const UIRECT& rcFramePos = GetGlobalPos ();
		if ( rcThumbPos.left <= rcFramePos.left )
		{
			SetPercent ( fZERO_PERCENT );
		}
		else if ( rcFramePos.right <= rcThumbPos.right )
		{
			SetPercent ( fHUNDRED_PERCENT );
		}
		else
		{
			const UIRECT& rcThumbPos = m_pThumb->GetGlobalPos ();
			const UIRECT& rcFramePos = GetGlobalPos ();

			const float fBase = rcFramePos.left + rcThumbPos.sizeX;
			const float fTotal = rcFramePos.sizeX - rcThumbPos.sizeX;
			const float fCurPos = rcThumbPos.right - fBase;

			m_fPercent = fCurPos / fTotal;
		}
	}
}


void CBasicScrollThumbFrame::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_PosX = x;
	m_PosY = y;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( bFirstControl )
	{
		//	��ũ���� �ʿ���� ���...
		if ( !m_bMoreOnePage ) return ;

		if ( m_pThumb && m_pThumb->IsExclusiveSelfControl() )
		{
			const UIRECT& rcPos = m_pThumb->GetGlobalPos ();

			if ( m_bVERTICAL )	m_pThumb->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left, y - m_vGap.y ) );
			else				m_pThumb->SetGlobalPos ( D3DXVECTOR2 ( x - m_vGap.x, rcPos.top ) );

			VALIDTHUMBMOVE ();
			return ;
		}
        
		if ( nScroll )
		{
			CUIControl* pTopParent = GetTopParent ();
			if ( pTopParent && pTopParent->IsFocusControl() )
			{
				DWORD dwMsg = pTopParent->GetMessageEx ();
				if ( CHECK_MOUSE_IN ( dwMsg ) )
				{
					float fMovePercent = fONE_PERCENT * fSCROLL_MOVE_FACTOR;
					if ( 0 < nScroll )	fMovePercent = -fMovePercent;

					const float fCurPercent = GetPercent ();
					float fNewPercent = fCurPercent + fMovePercent;
					if ( fNewPercent < fZERO_PERCENT ) fNewPercent = fZERO_PERCENT;
					if ( fHUNDRED_PERCENT < fNewPercent ) fNewPercent = fHUNDRED_PERCENT;

					SetPercent ( fNewPercent );
				}
			}
			return ;
		}
	}
}

void CBasicScrollThumbFrame::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ET_CONTROL_THUMB:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )		//	��Ʈ�� ���ο� ���콺�� ��ġ�ϰ�,
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )	//	���콺�� ��Ŭ������ ������ ��,
				{			
					m_pThumb->SetExclusiveControl();	//	�ܵ� ��Ʈ�ѷ� ����ϰ�,
					if ( !m_bFirstGap )				//	���� ������ ���� ����Ѵ�.
					{
						UIRECT rcPos = m_pThumb->GetGlobalPos ();
						m_vGap.x = m_PosX - rcPos.left;						
						m_vGap.y = m_PosY - rcPos.top;
						m_bFirstGap = TRUE;
					}
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	��Ŭ���ߴ����� ������
				{
					m_pThumb->ResetExclusiveControl();	//	�ܵ� ��Ʈ���� �����ϰ�
					m_bFirstGap = FALSE;				//	���� �����ǰ��� �����Ѵ�.
				}
			}
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	��Ʈ�� �����ۿ��� ��ư�� ���� ��찡
			{											//	�߻��ϴ���
				m_pThumb->ResetExclusiveControl();		//	�ܵ� ��Ʈ���� �����ϰ�,
				m_bFirstGap = FALSE;					//	���� �����ǰ��� �����Ѵ�.
			}
		}
		break;
	}
}
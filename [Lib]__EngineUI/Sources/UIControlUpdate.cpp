#include "pch.h"

#include "UIControl.h"
#include "DxInputDevice.h"
#include "UIMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const WORD UI_VISIBLE_ALPHA = 255;
const WORD UI_INVISIBLE_ALPHA = 0;

void CUIControl::SetDiffuseAlpha ( WORD wAlpha )
{
	CUIControl::SetDiffuse ( D3DCOLOR_ARGB ( wAlpha, 255, 255, 255 ) );
}

void CUIControl::FadeUpdate(float fElapsedTime)
{
	if (m_VisibleState == FADEOUT_MODE)
	{		
		m_fFadeElapsedTime += fElapsedTime;

        m_fVisibleRate = (1.0f - m_fFadeElapsedTime / m_fFadeLimitTime);
		if ( m_fVisibleRate < 0.0f )	m_fVisibleRate = 0.0f;

		int nAlpha =  int(m_dwBeginAlpha * m_fVisibleRate);
		if ( m_fFadeElapsedTime < m_fFadeLimitTime && 0 < nAlpha )
		{			
			SetDiffuseAlpha ( nAlpha );
		}
		else	
		{
			m_bVisible = FALSE;

			m_VisibleState = INVISIBLE_MODE;
			SetDiffuseAlpha ( UI_INVISIBLE_ALPHA );
		}
	}
	else if ( m_VisibleState == FADEIN_MODE )
	{		
		m_fFadeElapsedTime += fElapsedTime;

        m_fVisibleRate = ( m_fFadeElapsedTime / m_fFadeLimitTime );
		if ( m_fVisibleRate > 1.0f )	m_fVisibleRate = 1.0f;

		int nAlpha = int(m_dwBeginAlpha * m_fVisibleRate);
		if ( m_fFadeElapsedTime < m_fFadeLimitTime && nAlpha < UI_VISIBLE_ALPHA )
		{			
			SetDiffuseAlpha ( nAlpha );
		}
		else 
		{		
			m_VisibleState = VISIBLE_MODE;
			SetDiffuseAlpha ( UI_VISIBLE_ALPHA );
		}
	}
}

BOOL CUIControl::MouseUpdate ( int PosX, int PosY )
{
	//	컨트롤 좌표와 현재 마우스 좌표를 비교하여 MouseIn을 판단
	if ( (PosX>=m_rcGlobalPos.left && PosX<=m_rcGlobalPos.right) &&
		 (PosY>=m_rcGlobalPos.top && PosY<=m_rcGlobalPos.bottom) )
			return TRUE;
	else	return FALSE;
}

void CUIControl::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIControl::ResetMessageEx ();

	//	페이드 인 아웃 처리
	if ( m_bTransparency ) FadeUpdate(fElapsedTime);

	if ( IsNoUpdate() ) return ;
	
	/////////////////////////////////////////////////////////////////
	//	컨트롤 내부에 마우스가 위치하는가?
	if ( MouseUpdate( x, y ) )	AddMessageEx ( UIMSG_MOUSEIN );
	else						AddMessageEx ( UIMSG_MOUSEOUT );

	//	마우스 '버튼 & 스크롤' 메시지 처리
	if ( LB & DXKEY_IDLE )	LB = 0;
	if ( MB & DXKEY_IDLE )	MB = 0;
	if ( RB & DXKEY_IDLE )	RB = 0;
	if ( nScroll )			AddMessageEx ( UIMSG_SCROLL );	

	AddMessageEx ( MAKE_UIMSG ( 0, RB, MB, LB ) );
}
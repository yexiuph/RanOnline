#include "pch.h"
#include "BasicButton.h"
#include "UIKeyCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float CBasicButton::fBUTTON_CLICK = 1.0f;

CBasicButton::CBasicButton():
	m_wFlipMode(NOT_READY),
	m_bDown(FALSE),
	m_bMouseIn(FALSE),
	m_bFlip ( FALSE ),
	m_bOnBack ( FALSE ),
	m_bUseDynamic ( FALSE ),
	m_bUseGlobalAction ( FALSE ),
	m_KeyCode ( NULL ),
	m_KeyCodeEx ( NULL ),
	m_pFlipControl ( NULL ),
	m_pMouseOver ( NULL )
{
}

CBasicButton::~CBasicButton()
{
}

void CBasicButton::CreateMouseOver ( char* KeyValue )
{	
	if ( m_wFlipMode == NOT_READY )
	{
		GASSERT ( 0 && "CreateFlip() 함수 이후에 호출해야 합니다." );
		return ;
	}
	if ( m_wFlipMode == MOUSEIN_FLIP )
	{
		GASSERT ( 0 && "마우스인 플립모드일 경우 마우스 플립이미지를 따로 제작할 필요가 없습니다." );
		return ;
	}

	CUIControl* pMouseOver = new CUIControl;
	pMouseOver->CreateSub ( this, KeyValue );
	pMouseOver->SetVisibleSingle ( FALSE );
	RegisterControl ( pMouseOver );
	m_pMouseOver = pMouseOver;
}

void CBasicButton::CreateFlip( const char *keyvalue, WORD FlipType )
{
	CUIControl* pFlipControl = new CUIControl;
	pFlipControl->CreateSub ( this, keyvalue );
	pFlipControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pFlipControl );
	m_pFlipControl = pFlipControl;

	m_wFlipMode = FlipType;
}

void CBasicButton::Reset()
{
	CUIControl::Reset();
	m_bDown = FALSE;
	m_bMouseIn = FALSE;
}

void CBasicButton::SetFlip()
{
	switch ( m_wFlipMode )
	{
	case CLICK_FLIP:
	case KEEP_FLIP:
	case RADIO_FLIP:
		m_bDown=TRUE;
		break;
	case MOUSEIN_FLIP:
		m_bMouseIn=TRUE;
		break;
	}
}

void CBasicButton::SetFlip ( BOOL bFlip )
{
	if ( m_pFlipControl ) m_pFlipControl->SetVisibleSingle ( bFlip );
	if ( m_pMouseOver && bFlip )
	{
		m_pMouseOver->SetVisibleSingle ( FALSE );
	}

	if ( IsUseDynamic() )
	{
		CUIControl::SetDiffuse ( D3DCOLOR_ARGB ( 255, 255, 255, 255 ) );
		if ( m_bFlip )
		{
			CUIControl::SetDiffuse ( D3DCOLOR_ARGB ( 0, 255, 255, 255 ) );
			SetFlipPosition ();
		}
	}

	m_bFlip = bFlip;
}

void CBasicButton::TranslateMouseMessage ( DWORD dwMsg )
{	
	BOOL bMouseOver = FALSE;
	if ( CHECK_MOUSE_IN ( dwMsg ) )	bMouseOver = TRUE;
	if ( m_pMouseOver )	m_pMouseOver->SetVisibleSingle ( bMouseOver );

	switch ( m_wFlipMode )
	{
	case CLICK_FLIP:
		{			
			if ( CHECK_MOUSE_IN_LBDOWNLIKE ( dwMsg ) )
			{
				SetFlip ( TRUE );
			}			
		}
		break;

	case MOUSEIN_FLIP:
		{
			//	마우스 인
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				SetFlip ( TRUE );
			}

		}
		break;

	case KEEP_FLIP:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_bOnBack = m_bOn;
					m_bOn = !m_bOn;

//					SoundPlay ();
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					//	컨트롤 외부에서 다운된 상태로
					//	컨트롤 내부로 진입한 다음
					//	업된 버튼의 메시지처리를 막는다.
					m_bOnBack = !m_bOnBack;
					m_bOn = m_bOnBack;						
				}
			}
			else
			{
				m_bOn = m_bOnBack;
			}

			SetFlip ( m_bFlip );
		}
		break;

	case RADIO_FLIP:
		{	
			CUIControl::SetDiffuse ( D3DCOLOR_ARGB(255,255,255,255) );			
			if ( m_bFlip )
			{
                CUIControl::SetDiffuse ( D3DCOLOR_ARGB(0,0,0,0) );				
			}			
		}
		break;
	}
}

void CBasicButton::SetFlipPosition ()
{
	if ( m_pFlipControl )
	{
		if ( !IsFlip () )
		{
			const UIRECT& rcBackPos = GetGlobalPos ();
			m_pFlipControl->SetGlobalPos ( D3DXVECTOR2 ( rcBackPos.left + fBUTTON_CLICK, rcBackPos.top + fBUTTON_CLICK ) );
		}
	}
}

void CBasicButton::ResetFlipPosition ()
{
	if ( m_pFlipControl )
	{
		if ( IsFlip () )
		{
			const UIRECT& rcBackPos = GetGlobalPos ();
			m_pFlipControl->SetGlobalPos ( D3DXVECTOR2 ( rcBackPos.left, rcBackPos.top ) );
		}
	}
}


void CBasicButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( IsUseDynamic() )
	{
		ResetFlipPosition ();
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pMouseOver )	m_pMouseOver->SetVisibleSingle ( FALSE );
	if ( m_wFlipMode != KEEP_FLIP && m_wFlipMode != RADIO_FLIP ) SetFlip ( FALSE );

	if ( bFirstControl )
	{		
		CUIControl* pParent = GetTopParent ();
		BOOL bFocusControl = ( pParent && pParent->IsFocusControl() ) ? TRUE : FALSE;
		if ( bFocusControl || m_bUseGlobalAction )
		{
			TranslateMouseMessage ( GetMessageEx () );
		}

		if ( bFocusControl )
		{
			if ( m_KeyCode || m_KeyCodeEx )
			{
				if ( UIKeyCheck::GetInstance()->Check ( m_KeyCode, DXKEY_DOWN ) )
				{
					AddMessageEx ( UIMSG_KEY_FOCUSED );
				}

				if ( UIKeyCheck::GetInstance()->Check ( m_KeyCodeEx, DXKEY_DOWN ) )
				{
					AddMessageEx ( UIMSG_KEY_FOCUSED );
				}
			}
		}
	}
}

BOOL CBasicButton::IsVIsibleMouseOver ()
{
	if ( !m_pMouseOver )
	{
		GASSERT ( 0 && "마우스 오버 이미지가 만들어지지 않았습니다." );
		return FALSE;
	}

	return m_pMouseOver->IsVisible ();
}

void CBasicButton::SetVisibleMouseOver ( BOOL bVisible )
{
	if ( !m_pMouseOver )
	{
		GASSERT ( 0 && "마우스 오버 이미지가 만들어지지 않았습니다." );
		return ;
	}

	m_pMouseOver->SetVisibleSingle ( bVisible );
}

void CBasicButton::DoFlipUpdate ()
{
	CUIControl::SetDiffuse ( D3DCOLOR_ARGB(255,255,255,255) );			
	if ( m_bFlip )
	{
        CUIControl::SetDiffuse ( D3DCOLOR_ARGB(0,0,0,0) );				
	}	
}
#include "pch.h"
#include "UIWindow.h"
#include "UIWindowTitle.h"
#include "BasicButton.h"
#include "UIWindowBody.h"
#include "UIMan.h"
#include "DxFontMan.h" //../[Lib]__Engine/Sources/DxCommon
#include "d3dfont.h" //../[Lib]__Engine/Sources/DxCommon

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIWindow::CUIWindow () 
	: m_pClose(NULL)
	, m_bFirstGap(FALSE)
	, m_pTitle(NULL)
	, m_pTitleFocus(NULL)
	, m_pBody(NULL)
	, m_PosX(0)
	, m_PosY(0)
{	
}

CUIWindow::~CUIWindow ()
{
}

void CUIWindow::CreateTitle ( char* szFrame, char* szLeft, char* szMid, char* szRight, char* szTextBox, const char* szText )
{
	int FONTSIZE = 8;
	if ( CUIMan::m_bBIGFONT ) FONTSIZE = 9;
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, FONTSIZE, _DEFAULT_FONT_SHADOW_EX_FLAG );

	WORD wAlignFlag = UI_FLAG_XSIZE;

	m_pTitle = new CUIWindowTitle;
	m_pTitle->CreateSub ( this, szFrame, wAlignFlag, ET_CONTROL_TITLE );
	m_pTitle->CreateTitle ( szLeft, szMid, szRight );
	m_pTitle->CreateTitleName ( szTextBox, szText, pFont );
	RegisterControl ( m_pTitle );	
}

void CUIWindow::CreateTitleFocus ( char* szFrame, char* szLeft, char* szMid, char* szRight, char* szTextBox, const char* szText )
{
	int FONTSIZE = 8;
	if ( CUIMan::m_bBIGFONT ) FONTSIZE = 9;
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, FONTSIZE, _DEFAULT_FONT_SHADOW_EX_FLAG );

	WORD wAlignFlag = UI_FLAG_XSIZE;

	m_pTitleFocus = new CUIWindowTitle;
	m_pTitleFocus->CreateSub ( this, szFrame, wAlignFlag, ET_CONTROL_TITLE_F );
	m_pTitleFocus->CreateTitle ( szLeft, szMid, szRight );
	m_pTitleFocus->CreateTitleName ( szTextBox, szText, pFont );
	RegisterControl ( m_pTitleFocus );	
}

void CUIWindow::CreateCloseButton ( char* szValue, char* szFlipValue, char* szOver )
{	
	WORD wAlignFlag = UI_FLAG_RIGHT;

	m_pClose = new CBasicButton;	
	m_pClose->CreateSub ( this, szValue, wAlignFlag, ET_CONTROL_BUTTON );
	m_pClose->CreateFlip ( szFlipValue, CBasicButton::CLICK_FLIP );
	m_pClose->CreateMouseOver ( szOver );
	RegisterControl ( m_pClose );
}

void CUIWindow::CreateBody ( char* szFrame, char* szLeft, char* szUp, char* szMain, char* szDown, char* szRight )
{	
	WORD wAlignFlag = UI_FLAG_XSIZE | UI_FLAG_YSIZE;

	m_pBody = new CUIWindowBody;
	m_pBody->CreateSub ( this, szFrame, wAlignFlag, ET_CONTROL_BODY );
	m_pBody->CreateBody ( szLeft, szUp, szMain, szDown, szRight );
	m_pBody->SetControlNameEx( "윈도우 몸통  UI_FLAG_XSIZE | UI_FLAG_YSIZE" );
	RegisterControl ( m_pBody );
}


void CUIWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( m_PosX == x && m_PosY == y )	return;

	m_PosX = x;
	m_PosY = y;

	// 지금 현재 마우스 왼쪽 버튼으로 지정된 상태
	if ( IsExclusiveSelfControl() )
	{		
		// 윈도우를 움직인다.
		SetGlobalPos ( D3DXVECTOR2 ( x - m_vGap.x, y - m_vGap.y ) );
	}
}

void CUIWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
			{
				DWORD dwButtonMsg = UIMSG_NOMESSAGE;
				if ( m_pClose )	dwButtonMsg = m_pClose->GetMessageEx ();
				if ( (dwMsg & UIMSG_LB_DOWN) && !CHECK_MOUSE_IN ( dwButtonMsg ) )	//	마우스를 좌클릭으로 눌렀을 때,
				{			
					SetExclusiveControl();	//	단독 컨트롤로 등록하고,
					if ( !m_bFirstGap )				//	최초 포지션 갭을 기록한다.
					{
						UIRECT rcPos = GetGlobalPos ();
						m_vGap.x = m_PosX - rcPos.left;
						m_vGap.y = m_PosY - rcPos.top;
						m_bFirstGap = TRUE;
					}
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	좌클릭했던것을 놓으면
				{
					ResetExclusiveControl();	//	단독 컨트롤을 해제하고
				}
			}
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	컨트롤 영역밖에서 버튼을 떼는 경우가
			{											//	발생하더라도
				ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
			}
		}
		break;
	}

	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CUIWindow::SetTitleName ( const char* szTitleName )
{
	// MEMO : 타이틀은 NULL값이 들어올 수 있다. 
	// MEMO : 타이틀 바가 없을 수도 있다. 예) 모달 윈도우
	if( m_pTitle ) m_pTitle->SetTitleName ( szTitleName );
	if( m_pTitleFocus ) m_pTitleFocus->SetTitleName ( szTitleName );
}

void CUIWindow::SetTitleAlign ( int nAlign )
{
	if( m_pTitle ) m_pTitle->SetTitleAlign ( nAlign );
	if( m_pTitleFocus ) m_pTitleFocus->SetTitleAlign ( nAlign );
}

void CUIWindow::ResetExclusiveControl()
{
	CUIGroup::ResetExclusiveControl();
	m_bFirstGap = FALSE;
}

void CUIWindow::SetFocusWindow( BOOL bFocus )
{
	if( m_pTitle && m_pTitleFocus )
	{
		if( bFocus )
		{
			m_pTitle->SetVisibleSingle( FALSE );
			m_pTitleFocus->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pTitleFocus->SetVisibleSingle( FALSE );
			m_pTitle->SetVisibleSingle( TRUE );
		}
	}
}

void CUIWindow::SetFocusControl()					
{
	CUIControl::SetFocusControl();
	SetFocusWindow( TRUE ); 
}

void CUIWindow::ResetFocusControl()
{
	CUIControl::ResetFocusControl();
	// 타이틀이 포커스를 사용할때 FALSE로 만들어 준다.
	// 인터페이스 텍스쳐가 업데이트 되어야 한다.
	SetFocusWindow( FALSE );
}
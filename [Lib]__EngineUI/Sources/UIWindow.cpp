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
	m_pBody->SetControlNameEx( "������ ����  UI_FLAG_XSIZE | UI_FLAG_YSIZE" );
	RegisterControl ( m_pBody );
}


void CUIWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( m_PosX == x && m_PosY == y )	return;

	m_PosX = x;
	m_PosY = y;

	// ���� ���� ���콺 ���� ��ư���� ������ ����
	if ( IsExclusiveSelfControl() )
	{		
		// �����츦 �����δ�.
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
			if ( CHECK_MOUSE_IN ( dwMsg ) )		//	��Ʈ�� ���ο� ���콺�� ��ġ�ϰ�,
			{
				DWORD dwButtonMsg = UIMSG_NOMESSAGE;
				if ( m_pClose )	dwButtonMsg = m_pClose->GetMessageEx ();
				if ( (dwMsg & UIMSG_LB_DOWN) && !CHECK_MOUSE_IN ( dwButtonMsg ) )	//	���콺�� ��Ŭ������ ������ ��,
				{			
					SetExclusiveControl();	//	�ܵ� ��Ʈ�ѷ� ����ϰ�,
					if ( !m_bFirstGap )				//	���� ������ ���� ����Ѵ�.
					{
						UIRECT rcPos = GetGlobalPos ();
						m_vGap.x = m_PosX - rcPos.left;
						m_vGap.y = m_PosY - rcPos.top;
						m_bFirstGap = TRUE;
					}
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	��Ŭ���ߴ����� ������
				{
					ResetExclusiveControl();	//	�ܵ� ��Ʈ���� �����ϰ�
				}
			}
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	��Ʈ�� �����ۿ��� ��ư�� ���� ��찡
			{											//	�߻��ϴ���
				ResetExclusiveControl();		//	�ܵ� ��Ʈ���� �����ϰ�,
			}
		}
		break;
	}

	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CUIWindow::SetTitleName ( const char* szTitleName )
{
	// MEMO : Ÿ��Ʋ�� NULL���� ���� �� �ִ�. 
	// MEMO : Ÿ��Ʋ �ٰ� ���� ���� �ִ�. ��) ��� ������
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
	// Ÿ��Ʋ�� ��Ŀ���� ����Ҷ� FALSE�� ����� �ش�.
	// �������̽� �ؽ��İ� ������Ʈ �Ǿ�� �Ѵ�.
	SetFocusWindow( FALSE );
}
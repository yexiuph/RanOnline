#include "pch.h"
#include "BasicScrollBar.h"
#include "BasicButton.h"
#include "UIMan.h"
#include "BasicScrollThumbFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicScrollBar::CBasicScrollBar ()
	: m_pThumbFrame ( NULL )
{
}

CBasicScrollBar::~CBasicScrollBar ()
{
}

void CBasicScrollBar::CreateThumb ( char* szFrame, char* szThumb, char* szBodyValue, char* szUpValue, char* szDnValue, const bool& bVERTICAL )
{
	WORD wAlignFlag(0);
	if ( bVERTICAL )	wAlignFlag = UI_FLAG_YSIZE;
	else				wAlignFlag = UI_FLAG_XSIZE;	

	CBasicScrollThumbFrame* pThumbFrame = new CBasicScrollThumbFrame;
	pThumbFrame->CreateSub ( this, szFrame, wAlignFlag );
	pThumbFrame->CreateThumb ( szThumb, szBodyValue, szUpValue, szDnValue, bVERTICAL );	
	RegisterControl ( pThumbFrame );
	m_pThumbFrame = pThumbFrame;
}

void CBasicScrollBar::CreateButton ( char* szUpValue, char* szUpFlipValue, char* szDnValue, char* szDnFlipValue, const bool& bVERTICAL )
{
	WORD wAlignFlag(0);

	CBasicButton* pUp = new CBasicButton;	
	pUp->CreateSub ( this, szUpValue, wAlignFlag, ET_CONTROL_UP );
	pUp->CreateFlip ( szUpFlipValue );	
	pUp->SetUseGlobalAction ( TRUE );
	RegisterControl ( pUp );


    if ( bVERTICAL ) wAlignFlag = UI_FLAG_BOTTOM;
	else			 wAlignFlag = UI_FLAG_RIGHT;

	CBasicButton* pDn = new CBasicButton;
	pDn->CreateSub ( this, szDnValue, wAlignFlag, ET_CONTROL_DN );
	pDn->CreateFlip ( szDnFlipValue );
	pDn->SetControlNameEx ( "아래버튼" );
	pDn->SetUseGlobalAction ( TRUE );
	RegisterControl ( pDn );
}

void CBasicScrollBar::SetVisibleThumb( bool bVisible )
{
	GetThumbFrame()->SetVisibleSingle( bVisible );
}

CBasicScrollThumbFrame* const CBasicScrollBar::GetThumbFrame () const
{
	if ( !m_pThumbFrame )
	{
		GASSERT ( 0 && "m_pThumbFrame이 NULL입니다. CreateThumb()를 먼저 호출하십시오." );
	}

	return m_pThumbFrame;
}

void CBasicScrollBar::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ET_CONTROL_UP:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GetThumbFrame()->MoveUp ();
			}
		}
		break;

	case ET_CONTROL_DN:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GetThumbFrame()->MoveDown ();
			}
		}
		break;
	}
}
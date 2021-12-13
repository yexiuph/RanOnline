#include "pch.h"
#include "BasicScrollThumb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicScrollThumb::CBasicScrollThumb ()
{
}

CBasicScrollThumb::~CBasicScrollThumb ()
{
}

void CBasicScrollThumb::CreateSkin ( char* szBodyValue, char* szUpValue, char* szDnValue, const bool& bVERTICAL )
{
	if ( bVERTICAL )
	{
		CUIControl* pBody = new CUIControl;
		pBody->CreateSub ( this, szBodyValue, UI_FLAG_YSIZE );
		pBody->SetControlNameEx ( "스크롤바디 | UI_FLAG_YSIZE_PROPORTION" );
		RegisterControl ( pBody );

		CUIControl* pUp = new CUIControl;	
		pUp->CreateSub ( this, szUpValue );	
		pUp->SetControlNameEx ( "스크롤업이미지" );
		RegisterControl ( pUp );

		CUIControl* pDn = new CUIControl;
		pDn->CreateSub ( this, szDnValue, UI_FLAG_BOTTOM );
		pDn->SetControlNameEx ( "스크롤다운이미지 | UI_FLAG_BOTTOM" );
		RegisterControl ( pDn );
	}
	else
	{
		CUIControl* pBody = new CUIControl;
		pBody->CreateSub ( this, szBodyValue, UI_FLAG_XSIZE );
		pBody->SetControlNameEx ( "스크롤바디 | UI_FLAG_XSIZE_PROPORTION" );
		RegisterControl ( pBody );

		CUIControl* pUp = new CUIControl;	
		pUp->CreateSub ( this, szUpValue );	
		pUp->SetControlNameEx ( "스크롤업이미지" );
		RegisterControl ( pUp );

		CUIControl* pDn = new CUIControl;
		pDn->CreateSub ( this, szDnValue, UI_FLAG_RIGHT );
		pDn->SetControlNameEx ( "스크롤다운이미지 | UI_FLAG_BOTTOM" );
		RegisterControl ( pDn );
	}
}
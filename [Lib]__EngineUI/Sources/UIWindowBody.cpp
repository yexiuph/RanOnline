#include "pch.h"
#include "UIWindowBody.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIWindowBody::CUIWindowBody ()
{	
}

CUIWindowBody::~CUIWindowBody ()
{
}

void CUIWindowBody::CreateBody ( char* szLeft, char* szUp, char* szMain, char* szDown, char* szRight )
{
	//UIRECT rcOriginPos = GetGlobalPos ();
	//WORD wAlignFlag = UI_FLAG_DEFAULT;
	//WORD wAlignFlag;

	CUIControl* pLeft = new CUIControl;
	//wAlignFlag = UI_FLAG_YSIZE;
    pLeft->CreateSub ( this, szLeft, UI_FLAG_YSIZE );
	//pLeft->SetControlNameEx( "왼쪽라인" );
	RegisterControl ( pLeft );

	CUIControl* pUp = new CUIControl;
	//wAlignFlag = UI_FLAG_XSIZE;
    pUp->CreateSub ( this, szUp, UI_FLAG_XSIZE );
	//pUp->SetControlNameEx( "위쪽라인" );
	RegisterControl ( pUp );

	CUIControl* pMain = new CUIControl;
	//wAlignFlag = UI_FLAG_XSIZE | UI_FLAG_YSIZE;
	pMain->CreateSub ( this, szMain, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	//pMain->SetControlNameEx( "메인" );
	RegisterControl ( pMain );

	CUIControl* pDown = new CUIControl;
	//wAlignFlag = UI_FLAG_BOTTOM | UI_FLAG_XSIZE;
    pDown->CreateSub ( this, szDown, UI_FLAG_BOTTOM | UI_FLAG_XSIZE );
	//pDown->SetControlNameEx( "아래라인" );
	RegisterControl ( pDown );

	CUIControl* pRight = new CUIControl;
	//wAlignFlag = UI_FLAG_RIGHT | UI_FLAG_YSIZE;
    pRight->CreateSub ( this, szRight, UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	//pRight->SetControlNameEx( "오른쪽라인" );
	RegisterControl ( pRight );
}
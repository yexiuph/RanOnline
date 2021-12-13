#include "pch.h"
#include "BasicTextButtonImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicTextButtonImage::CBasicTextButtonImage ()
{
}

CBasicTextButtonImage::~CBasicTextButtonImage ()
{
}

void CBasicTextButtonImage::CreateButtonImage ( const char* szLeft, const char* szCenter, const char* szRight )
{
	CUIControl* pLeft = new CUIControl;
	pLeft->CreateSub ( this, szLeft );
	RegisterControl ( pLeft );

	CUIControl* pCenter = new CUIControl;
	pCenter->CreateSub ( this, szCenter, UI_FLAG_XSIZE );
	RegisterControl ( pCenter );

	CUIControl* pRight = new CUIControl;
	pRight->CreateSub ( this, szRight, UI_FLAG_RIGHT );
	RegisterControl ( pRight );
}
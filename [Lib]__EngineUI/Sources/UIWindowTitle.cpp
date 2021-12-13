#include "pch.h"
#include "./UIWindowTitle.h"
#include "./BasicTextBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIWindowTitle::CUIWindowTitle ()
	: m_pTitleName ( NULL )
{	
}

CUIWindowTitle::~CUIWindowTitle ()
{
}

void CUIWindowTitle::CreateTitle ( char* szLeft, char* szMid, char* szRight )
{
	WORD wAlignFlag;

	CUIControl* pLeft = new CUIControl;	
    pLeft->CreateSub ( this, szLeft );
	RegisterControl ( pLeft );

	CUIControl* pMid = new CUIControl;
	wAlignFlag = UI_FLAG_XSIZE; // Note : 가운데 이미지는 크기가 변한다.
    pMid->CreateSub ( this, szMid, wAlignFlag );
	RegisterControl ( pMid );

	CUIControl* pRight = new CUIControl;
	wAlignFlag = UI_FLAG_RIGHT;
    pRight->CreateSub ( this, szRight, wAlignFlag );
	RegisterControl ( pRight );
}

void CUIWindowTitle::CreateTitleName ( char* szTextBox, const char* szText, CD3DFontPar* pFont8 )
{
	WORD wAlignFlag;

	CBasicTextBox* pTextBox = new CBasicTextBox;
	wAlignFlag = UI_FLAG_XSIZE;
	pTextBox->CreateSub ( this, szTextBox, wAlignFlag );
	pTextBox->SetFont ( pFont8 );
	pTextBox->SetOneLineText ( szText, NS_UITEXTUTIL::WINDOWTITLE );
	RegisterControl ( pTextBox );
	m_pTitleName = pTextBox;
}

void CUIWindowTitle::SetTitleName ( const char* szTitleName )
{
	if ( m_pTitleName )
	{
		m_pTitleName->SetOneLineText ( szTitleName, NS_UITEXTUTIL::WINDOWTITLE );
	}
}

void CUIWindowTitle::SetTitleAlign ( int nAlign )
{
	if ( m_pTitleName )
	{
		m_pTitleName->SetTextAlign ( nAlign );
	}
}
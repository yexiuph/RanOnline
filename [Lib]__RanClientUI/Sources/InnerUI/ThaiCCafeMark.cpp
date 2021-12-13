#include "pch.h"
#include "ThaiCCafeMark.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CThaiCCafeMark::CThaiCCafeMark()
	: m_pImage( NULL )
	, m_dwClass ( 0 )
{
}

CThaiCCafeMark::~CThaiCCafeMark()
{
}

void CThaiCCafeMark::CreateSubControl ()
{
	m_pImage =  new CUIControl;
	m_pImage->CreateSub ( this, "THAI_CCAFE_MARK_IMAGE", UI_FLAG_DEFAULT );	
	RegisterControl ( m_pImage );
}

void CThaiCCafeMark::SetClass( DWORD dwClass )
{
	if ( dwClass <= 0 )	SetVisibleSingle( FALSE );
	else 
	{
		m_dwClass = dwClass;

		UIRECT rcPos;
		rcPos = m_pImage->GetTexturePos();

		rcPos.left = ( rcPos.sizeX * ( m_dwClass -1 ) );		
		m_pImage->SetTexturePos( rcPos );

		SetVisibleSingle( TRUE );
	}
}
#include "pch.h"
#include "BasicLevelDisplay.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GLGaeaClient.h"
#include "UITextControl.h"
#include "d3dfont.h"

CBasicLevelDisplay::CBasicLevelDisplay ()
	: m_pLevelText ( NULL )
	, m_wLevelBACK(0)
{
}

CBasicLevelDisplay::~CBasicLevelDisplay ()
{
	m_wLevelBACK = 0;
}

void CBasicLevelDisplay::CreateSubControl ()
{	
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CBasicTextBox* pLevelText = new CBasicTextBox;
	pLevelText->CreateSub ( this, "BASIC_LEVEL_DISPLAY_TEXT" );
    pLevelText->SetFont ( pFont8 );
	pLevelText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	RegisterControl ( pLevelText );
	m_pLevelText = pLevelText;	
}

void CBasicLevelDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	const WORD& wLevel = GLGaeaClient::GetInstance().GetCharacter()->m_wLevel;

	if( m_wLevelBACK < wLevel )
	{
		m_wLevelBACK = wLevel;

		CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		CString strLevel;
		strLevel.Format( "%d", m_wLevelBACK );	
		m_pLevelText->SetOneLineText( strLevel, NS_UITEXTCOLOR::GOLD /*PLUSOPTION*/ );
	}
}
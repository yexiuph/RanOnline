#include "pch.h"

#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "BasicTextButton.h"
#include "d3dfont.h"

#include "BasicGameMenu.h"

#include "InnerInterface.h"
#include "BasicVarTextBox.h"
#include "GameTextControl.h"

#include "GLGaeaClient.h"
#include "UITextControl.h"

#include "VNGainSysGauge.h"



CVNGainSysGauge::CVNGainSysGauge () :
	m_pGauge(NULL),
	m_dwOldValue(0),
	m_pVNInvenButton(NULL)
{
}

CVNGainSysGauge::~CVNGainSysGauge ()
{
}


CBasicTextBox*	CVNGainSysGauge::CreateTextBox ( char* szKeyword, CD3DFontPar* pFont, int nAlign )
{
	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, szKeyword );
	pTextBox->SetFont ( pFont );
	pTextBox->SetTextAlign ( nAlign );	
	RegisterControl ( pTextBox );

	return pTextBox;
}


void CVNGainSysGauge::CreateSubControl ()
{
	
	m_pGauge = new CBasicProgressBar;
	m_pGauge->CreateSub ( this, "VNGAINSYS_GAUGE_IMAGE" );
	m_pGauge->CreateOverImage ( "VNGAINSYS_GAUGE_OVERIMAGE" );
	RegisterControl ( m_pGauge );

	CBasicTextBox* pTextBox = NULL;	

	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	pTextBox = CreateTextBox ( "VNGAINSYSTEM_3TIME_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextBox->AddText ( ID2GAMEWORD("VNGAINSYSTEM_3TIME_STATIC",0) );


	pTextBox = CreateTextBox ( "VNGAINSYSTEM_5TIME_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextBox->AddText ( ID2GAMEWORD("VNGAINSYSTEM_5TIME_STATIC",0) );	
}

void CVNGainSysGauge::SetGauge ( DWORD dwNOW, DWORD dwMAX )
{
	if ( m_dwOldValue != dwNOW )
	{
		m_dwOldValue = dwNOW;

		const float fPercent = float(dwNOW) / float(dwMAX);
		m_pGauge->SetPercent ( fPercent );
	}
}


void CVNGainSysGauge::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CVNGainSysGauge::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID , dwMsg );
}
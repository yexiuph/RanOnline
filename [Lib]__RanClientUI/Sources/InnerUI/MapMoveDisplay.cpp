#include "pch.h"
#include "MapMoveDisplay.h"
#include "BasicLineBox.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextButton.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "GLCharacter.h"
#include "GLGaeaClient.h"
#include "DxGlobalStage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMapMoveDisplay::CMapMoveDisplay () :
	m_pTextBox ( NULL ),
    m_pMoveOKButton ( NULL )
{
}

CMapMoveDisplay::~CMapMoveDisplay ()
{
}

void CMapMoveDisplay::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MAPMOVE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxMapMove ( "BASIC_MAPMOVE_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pMapMoveTextBox = new CBasicTextBox;
	pMapMoveTextBox->CreateSub ( this, "BASIC_MAPMOVE_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pMapMoveTextBox->SetFont ( pFont9 );
	pMapMoveTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	RegisterControl ( pMapMoveTextBox );
	m_pTextBox = pMapMoveTextBox;

	m_pMoveOKButton = CreateTextButton ( "BASIC_MAPMOVE_OK", MAPMOVE_OKBUTTON, (char*)ID2GAMEWORD ( "MAPMOVE_OKBUTTON" ) );
	m_pMoveOKButton->SetShortcutKey ( DIK_SPACE );
}

CBasicTextButton* CMapMoveDisplay::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void CMapMoveDisplay::DoMAPMOVE ()
{
	GLGaeaClient::GetInstance().GetCharacter()->ReqGateOut();
}

void CMapMoveDisplay::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case MAPMOVE_OKBUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) || CHECK_KEYFOCUSED ( dwMsg ) )
			{
				//	Note : 출구 나가기 요청 메시지 발생.
				//
				DoMAPMOVE ();
			}
		}
        break;
	}
}

void	CMapMoveDisplay::SetMapName ( CString strName )
{
	if ( m_pTextBox )
	{
		m_pTextBox->SetText ( strName );
	}
}